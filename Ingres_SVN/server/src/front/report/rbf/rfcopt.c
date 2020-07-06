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
# include       "rbftype.h"
# include       "rbfglob.h"
# include	<ug.h>
# include	<si.h>
# include	<cm.h>
# include	<te.h>
# include	<flddesc.h>
# include 	<st.h>
# include 	<uf.h>
FUNC_EXTERN	char	*IIRFfwd_FullWord();
FUNC_EXTERN	VOID	create_section();
FUNC_EXTERN	bool	IIRFdso_DupSortOrder();
FUNC_EXTERN VOID	AdjustSections();
static		VOID	_rFagguHandler();
static			FixSectionSeq();
static		bool	BadSortOrder();
bool 			IIRFIsBreakColumn();
STATUS 			IIRFSetBrkOptions();
STATUS 			IIRFSetNewPageOption();
/*
**   RFCOLOPTS - display and control the COPTIONS form to select
**	the selection criteria and control the sort order of the
**	columns.
**
**	Note that this fills in the fields of the Coptions form
**	from the current COPT structure, which should be up to date.
**
**	Parameters:
**		noUndo		Pointer to a boolean.  Set to TRUE if the
**				Report Layout changes (due to break section
**				re-ordering or deletion of a unique agg).
**				This tells RFVIFRED to make Undo a NOP.
**
**	Returns:
**		TRUE if any changes made, FALSE if not.
**
**	Side Effects:
**		Fields in the COPT structure.
**
**	Called By:
**		rFdisplay.
**
**	Trace Flags:
**
**	Error Messages:
**		none.
**
**	History:
**		6/2/82 (ps)	written.
**		12/3/84 (rlk) - Rewritten to use 1 form containing a
**				tablefield.
**		26-nov-1986 (yamamoto)
**			Modified for double byte characters.
**		1/15/87 (rld)	change syntax of INQUIRE_FRS and SET_FRS calls.
**		2/12/87 (rld)	changed to accomodate new agg structures.
**		05-may-88 (marian)	bug 12071
**			Lowercase 'X' so subsequent routines will know
**			aggregates were requested.
**		06/24/88 (dkh) - Fixed 5.0/05a integration to work in 6.1.
**		20-dec-88 (sylviap)
**			Performance changes - reads in colopts.frm.
**      9/22/89 (elein) UG changes ingresug change #90045
**	changed <rbftype.h> & <rbfglob.h> to "rbftype.h" & "rbfglob.h"
**		21-aug-89 (cmr)	rewritten to support visible break 
**				headers and footers.  Also reflects 
**				changes to the coloptions form.  Break 
**				out some COPT info into rfstruct.qsc.
**		22-nov-89 (cmr)	after calling rFbreak() update str_brkcol.
**		11/27/89  (martym) - Added parameter to indicate if rfvifred 
**				should be called.
**		08-jan-89 (cmr)	rewrite to support new user interface.
**		28-jan-90 (cmr)	put in scrolling activations.
**		08-feb-90 (cmr)	default for sort direction is 'a' (not blank).
**		20-feb-90 (sylviap)
**			Took out all abbreviations ("ascending", "descending",
**			"none", "range", "value")
**		15-mar-90 (cmr)
**			Put in support to re-order break headers and/or
**			footers whenever the sort sequences change. Most
**			of the work is actually one in rfvifred.
**		5/22/90 (martym)
**			Added IIRFIsBreakColumn(), IIRFSetBrkOptions(),
**			and IIRFSetNewPageOption().
**	31-jul-90 (sylviap)
**		Changed "clear screen" to "redisplay" so selecting the
**		"Save" menuitem does not blank out the screen.
**		03-aug-90 (cmr) fix for bugs 32080 and 32083
**				Pass bool to create_section() to tell rfvifred 
**				whether or not to initialize the line array.  
**	04-sep-90 (sylviap)
**		Added explanations to the activate menuitems. #32699
**	18-sep-90 (sylviap)
**		Added call to IIRFdso_DupSortOrder.
**	12-oct-90 (steveh)
**		Removed FTbell calls from up/down arrow operations.
**		Although, there is no beep when the user attempts to
**		move past the end/top of the list, the bell does not
**		beep when a valid entry is made into the last column
**		of the table field. (bug 32908)
**	30-oct-1992 (rdrane)
**		Ensure that unsupported datatype fields cannot be selected.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
  i4
  rFcoptions(noUndo)
bool	*noUndo;
  {
	/* external declarations */
    GLOBALREF char *Ncoloptions;
    GLOBALREF bool alloc_opts;
	/* internal declarations */
    char attbuf[FE_MAXNAME + 1];
    char criteria[6];
    char direction[11];
    i2 sequence = 0;
	register COPT	*copt;			/* COPT structure */
	i4		i;
	bool		adjust_sections = FALSE;
	Sec_node	*n;
	ATT		*att;
    i4 anychanges;
    i4 attord;
    char *attname;
    char *form;
    char colname[10];
    char *fieldname;
	char		*prompt;
    char *choices;
	char		*help_scr;
    static char *dir_tbl[2];
    static char *sel_tbl[3];
    char *fullwrd;
    char *srttbl;
    char *srtseq;
    char *srtdir;
    char *selval;
    char *srtcolname;
	/* start of routine */
	Ncoloptions = F_COPTIONS;
	srttbl = F_SRTTBL;
	srtseq = F_SRTSEQ;
	srtdir = F_SRTDIR;
	selval = F_SELVAL;
	srtcolname = F_COLNAM;
	if ( !alloc_opts)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form = Ncoloptions) 
			!= OK)
		{
			IIUGerr(ERget(E_RF002D_Cannot_locate_form), 0, 1, form);
			return(FALSE);
		}
		alloc_opts = TRUE;
		/* init 'choices' tables */
		dir_tbl[0] = ERget (F_RF0025_sortup);
		dir_tbl[1] = ERget (F_RF0026_sortdown);
		sel_tbl[0] = ERget (F_RF000C_nosel);
		sel_tbl[1] = ERget (F_RF000A_value);
		sel_tbl[2] = ERget (F_RF000B_range);
	}
/* # line 184 "rfcopt.qsc" */	/* display */
    {
      if (IIdispfrm(Ncoloptions,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 188 "rfcopt.qsc" */	/* inittable */
        {
          if (IItbinit(Ncoloptions,(char *)"rfsrttbl",(char *)"u") != 0) {
            IItfill();
          } /* IItbinit */
        }
/* # line 190 "rfcopt.qsc" */	/* host code */
		/* First fill in the fields from the copt structure */
		for(i = 1; i <= En_n_attribs; i++)
		{
			/* Get the attribute name, but
			** Guard against unsupported datatype.
			*/
			if  ((att = r_gt_att(i)) == (ATT *)NULL)
			{
				continue;
			}
			attname = att->att_name;
			/* fill in the sort attributes */
			copt = rFgt_copt(i);
			sequence = copt->copt_sequence;
			if ( copt->copt_order == 'a' )
			    	STcopy( dir_tbl[0], direction );
			else
			        STcopy( dir_tbl[1], direction );
			switch ( copt->copt_select )
			{
			   case 'r':
				STcopy( sel_tbl[2], criteria );
				break;
			   case 'v':
			   	STcopy( sel_tbl[1], criteria );
				break;
			   default:
				STcopy( sel_tbl[0], criteria );
				break;
			}
			/*	Load a row of the tablefield */
/* # line 227 "rfcopt.qsc" */	/* loadtable */
        {
          if (IItbact(Ncoloptions,(char *)"rfsrttbl",1) != 0) {
            IItcoputio(srtcolname,(short *)0,1,32,0,attname);
            IItcoputio(srtseq,(short *)0,1,30,2,&sequence);
            IItcoputio(srtdir,(short *)0,1,32,0,direction);
            IItcoputio(selval,(short *)0,1,32,0,criteria);
            IITBceColEnd();
          } /* IItbact */
        }
/* # line 232 "rfcopt.qsc" */	/* host code */
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 239 "rfcopt.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,Ncoloptions,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,9,colname,34,(char *)0,0);
              } /* IIiqset */
            }
/* # line 240 "rfcopt.qsc" */	/* host code */
		if (STcompare (colname, srtseq) == 0)
		{
			choices   = ERget(S_RF006F_sort_seq);
/* # line 243 "rfcopt.qsc" */	/* message */
            {
              IIFRgpcontrol(1,0);
              IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
              IIFRgpcontrol(2,0);
              IImessage(choices);
            }
/* # line 244 "rfcopt.qsc" */	/* host code */
		}
		else if (STcompare (colname, srtdir) == 0)
		{
                	/*
			** Display Popup with the 'direction' options.  User 
			** may choose ascending or descending.  
			*/
			help_scr  = ERget(S_RF0075_Sort_direction);
			prompt    = ERget(F_RF0063_sort_dir_title);
			choices   = ERget(F_RF0064_asc_desc);
			i = IIFDlpListPick(prompt, choices, 0, 
				LPGR_FLOAT, LPGR_FLOAT, help_scr, H_SRTDIR,
				NULL,  NULL);
			if (i >= 0)
			{
				/* put in what user selected from listpick */
/* # line 260 "rfcopt.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcoputio(srtdir,(short *)0,1,32,0,dir_tbl[i]);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 261 "rfcopt.qsc" */	/* host code */
			}
		}
		else
		{
                	/*
			** Display Popup with the 'selection criteria' options.
			** User may choose none, value or range.  
			*/
			help_scr  = ERget(S_RF0076_Selection_criteria);
			prompt    = ERget(F_RF0065_sel_crit_title);
			choices   = ERget(F_RF0066_sel_crit_choices);
			i = IIFDlpListPick(prompt, choices, 0, 
				LPGR_FLOAT, LPGR_FLOAT, help_scr, H_SELVAL,
				NULL,  NULL);
			if (i >= 0)
			{
				/* put in what user selected from listpick */
/* # line 278 "rfcopt.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcoputio(selval,(short *)0,1,32,0,sel_tbl[i]);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 279 "rfcopt.qsc" */	/* host code */
			}
		}
/* # line 281 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 288 "rfcopt.qsc" */	/* host code */
		FEhelp(H_COPTIONS, ERget(F_RF0013_Column_Options));
          }
        } else if (IIretval() == 3) {
          {
/* # line 294 "rfcopt.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 296 "rfcopt.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&anychanges,20,Ncoloptions,0);
              } /* IIiqset */
            }
/* # line 298 "rfcopt.qsc" */	/* host code */
		/* Check for duplicate sort sequence */
		if (!IIRFdso_DupSortOrder(FALSE, &attord))
		{
/* # line 301 "rfcopt.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Ncoloptions,(char *)"rfsrttbl",-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,attord) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 302 "rfcopt.qsc" */	/* resume */
            {
              IIrescol((char *)"rfsrttbl",srtseq);
              if (1) goto IIfdB1;
            }
/* # line 303 "rfcopt.qsc" */	/* host code */
		}
		/*
		** If the sort order has changed then we need 
		** to re-arrange the headers and/or footers.
		*/
		if ( BadSortOrder() )
		{
			AdjustSections();
			*noUndo = TRUE;
		}
/* # line 314 "rfcopt.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 318 "rfcopt.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcogetio((short *)0,1,30,4,&attord,(char *)"_RECORD");
                IItcogetio((short *)0,1,30,2,&sequence,srtseq);
                IItcogetio((short *)0,1,32,10,direction,srtdir);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 323 "rfcopt.qsc" */	/* host code */
		/* make sure that COPT is up to date */
		copt = rFgt_copt(attord);
		if ( sequence == 0 )
		{
			if ( copt->copt_brkhdr == 'y' || copt->copt_brkftr == 'y' )
			{
				IIUGerr(E_RF0073_Must_delete_hdr_ftr, 0, 0);
				sequence = copt->copt_sequence;
/* # line 331 "rfcopt.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcoputio(srtseq,(short *)0,1,30,2,&sequence);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 333 "rfcopt.qsc" */	/* resume */
            {
              IIrescol((char *)"rfsrttbl",srtseq);
              if (1) goto IIfdB1;
            }
/* # line 334 "rfcopt.qsc" */	/* host code */
			}
			if ( copt->copt_sequence != 0 )
			{
				/* are there any unique aggs on this column? */
				if ( copt->agu_list )
				{
					/* let the user decide what to do */
					i = IIFDlpListPick( 
					   ERget(F_RF006C_Unique_Aggs),
					   ERget(F_RF006D_Unique_Aggs_Choices),
					   0, LPGR_FLOAT, LPGR_FLOAT, 
					   ERget(F_RF006E_Help_Unique_Aggs), 
					   H_UNIQAGGS, NULL, NULL);
					if ( i >= 0) /* delete or change */
					{
						_rFagguHandler(attord, copt, i);
						*noUndo = TRUE;
					}
					else	/* Cancel */
					{
						sequence = copt->copt_sequence;
/* # line 355 "rfcopt.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcoputio(srtseq,(short *)0,1,30,2,&sequence);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 357 "rfcopt.qsc" */	/* resume */
            {
              IIrescol((char *)"rfsrttbl",srtseq);
              if (1) goto IIfdB1;
            }
/* # line 358 "rfcopt.qsc" */	/* host code */
					}
				}
			}
			copt->copt_sequence = 0;
			copt->copt_break = 'n';
			copt->copt_whenprint = 'a';
			copt->copt_newpage = FALSE;
		}
		else
		{
			/* reset the seq in the break hdr and/or ftr */
			FixSectionSeq( attord, sequence );
			copt->copt_sequence = sequence;
			copt->copt_break = 'y';
		}
/* # line 374 "rfcopt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 379 "rfcopt.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcogetio((short *)0,1,30,4,&attord,(char *)"_RECORD");
                IItcogetio((short *)0,1,32,10,direction,srtdir);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 383 "rfcopt.qsc" */	/* host code */
  		copt = rFgt_copt( attord );
		/* get the full word */
		if ((fullwrd = 
			IIRFfwd_FullWord(direction, 2, dir_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, 0, 0); 
/* # line 390 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 391 "rfcopt.qsc" */	/* host code */
		}
		else 
		{ 
			copt->copt_order = STcompare
				(fullwrd, 
				 ERget(F_RF0026_sortdown)) != 0 ? 'a' : 'd';
/* # line 397 "rfcopt.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcoputio(srtdir,(short *)0,1,32,0,fullwrd);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 398 "rfcopt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 399 "rfcopt.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 6) {
          {
/* # line 403 "rfcopt.qsc" */	/* getrow */
            {
              if (IItbsetio(2,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcogetio((short *)0,1,30,4,&attord,(char *)"_RECORD");
                IItcogetio((short *)0,1,32,5,criteria,selval);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 407 "rfcopt.qsc" */	/* host code */
  		copt = rFgt_copt( attord );
		/* get the full word */
		if ((fullwrd = 
			IIRFfwd_FullWord (criteria, 3, sel_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, 0, 0);
/* # line 414 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 415 "rfcopt.qsc" */	/* host code */
		}
		else
		{
			if (STcompare(fullwrd, ERget(F_RF000A_value)) == 0 )
				copt->copt_select = 'v';
			else 
				if (!STcompare(fullwrd, ERget(F_RF000B_range)))
					copt->copt_select = 'r';
				else
					copt->copt_select = 'n';
/* # line 425 "rfcopt.qsc" */	/* putrow */
            {
              if (IItbsetio(3,Ncoloptions,(char *)"rfsrttbl",-2) != 0) {
                IItcoputio(selval,(short *)0,1,32,0,fullwrd);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 426 "rfcopt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 427 "rfcopt.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 7) {
          {
/* # line 431 "rfcopt.qsc" */	/* host code */
		/* bug 32908: removed FTbell call to 
		   avoid signaling error on valid input. */ 
		if ( attord > 1 )
		{
/* # line 435 "rfcopt.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Ncoloptions,(char *)"rfsrttbl",-3) != 0) {
                IItbsmode((char *)"down");
                if (IItscroll(0,-3) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 436 "rfcopt.qsc" */	/* host code */
		}
		else
		{
/* # line 439 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 440 "rfcopt.qsc" */	/* host code */
		}
/* # line 441 "rfcopt.qsc" */	/* resume */
            {
              IIresfld((char *)"rfsrttbl");
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 446 "rfcopt.qsc" */	/* host code */
		/* bug 32908: removed FTbell call to 
		   avoid signaling error on valid input. */ 
		if ( attord < En_n_attribs )
		{
/* # line 450 "rfcopt.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Ncoloptions,(char *)"rfsrttbl",-3) != 0) {
                IItbsmode((char *)"up");
                if (IItscroll(0,-3) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 451 "rfcopt.qsc" */	/* host code */
		}
		else
		{
/* # line 454 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 455 "rfcopt.qsc" */	/* host code */
		}
/* # line 456 "rfcopt.qsc" */	/* resume */
            {
              IIresfld((char *)"rfsrttbl");
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 461 "rfcopt.qsc" */	/* host code */
		FEtabfnd(Ncoloptions, srttbl);
          }
        } else if (IIretval() == 10) {
          {
/* # line 465 "rfcopt.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Ncoloptions,(char *)"rfsrttbl",-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 469 "rfcopt.qsc" */	/* scroll */
            {
              if (IItbsetio(1,Ncoloptions,(char *)"rfsrttbl",-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
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
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,1) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,2) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_RF0076_return_layout),2,2,3) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IITBcaClmAct(srttbl,srtseq,0,4) == 0) goto IIfdE1;
      if (IITBcaClmAct(srttbl,srtdir,0,5) == 0) goto IIfdE1;
      if (IITBcaClmAct(srttbl,selval,0,6) == 0) goto IIfdE1;
      if (IIactscrl((char *)"rfsrttbl",1,7) == 0) goto IIfdE1;
      if (IIactscrl((char *)"rfsrttbl",0,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,11) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 474 "rfcopt.qsc" */	/* host code */
	return(anychanges);
  }
/*
** FixSectionSeq() - For a given attribute, if there is an associated
**		     break header and/or footer then update the sort 
**		     sequence for the section(s).
*/
static
FixSectionSeq( ord, sequence )
i4	ord;
i4   sequence;
{
	COPT	*copt;
	Sec_node *n;
	copt = rFgt_copt( ord );
	if ( copt->copt_brkhdr != 'y' && copt->copt_brkftr != 'y' )
		return;
	for ( n = Sections.head; n; n = n->next )
	{
		if ( n->sec_type != SEC_BRKHDR && n->sec_type != SEC_BRKFTR )
			continue;
		if ( (n->sec_brkseq == copt->copt_sequence) &&
			(n->sec_brkord == ord) )
				n->sec_brkseq = sequence;
	}
}
static bool
/*
** BadSortOrder() - returns TRUE if the Report Layout (Sections) is
**		    out of date with respect to the sort order.
**		    Otherwise it returns FALSE.
*/
BadSortOrder()
{
	Sec_node *n, *prev, *next;
	for ( n = Sections.head; n ; n = n->next ) 
	{
		if ( n->sec_type != SEC_BRKHDR && n->sec_type != SEC_BRKFTR )
			continue;
		prev = n->prev;
		next = n->next;
		if ( prev && (prev->sec_brkseq > 0) 
			&& (prev->sec_brkseq > n->sec_brkseq) )
		{
			return( TRUE );
		}
		if ( next && (next->sec_brkseq > 0) 
			&& (next->sec_brkseq < n->sec_brkseq) )
		{
			return( TRUE );
		}
	}
	return( FALSE );
}
/*
** AdjustSections() - Re-order the break headers and footers to reflect
**		      the correct sort order.  This routine constructs a 
**		      sorted array of sort sequences and passes it to
**		      RebuildLines(rfvifred), which does most of the work.
*/
VOID
AdjustSections()
{
	i4	*sort_sequence, *s;
	i4	numseq=0;
	i4	i, j, temp, gap;
	COPT	*copt;
	TAGID	seq_tag;
	/*  allocate sort_sequence array */
	seq_tag = FEgettag();
	if ((sort_sequence = (i4 *)FEreqmem(seq_tag, En_n_attribs*sizeof(i4),
		TRUE, (STATUS *) NULL)) == NULL)
	{
		IIUGbmaBadMemoryAllocation(ERx("AdjustSections"));
	}
	/* copy sort sequences into the sort_sequence array */
	s = sort_sequence;
	for ( i = 1; i <= En_n_attribs; i++ )
	{
		copt = rFgt_copt(i);
		if ( copt->copt_sequence == 0 )
			continue;
		if ( copt->copt_brkhdr != 'y' && copt->copt_brkftr != 'y' )
			continue;
		*s++ = copt->copt_sequence;
		numseq++;
	}
	s = sort_sequence;
	/* Sort the sort sequences (shell sort from K&R) */
	for ( gap = numseq/2; gap > 0; gap /= 2 )
	{
		for ( i = gap; i < numseq; i++ )
		{
			for ( j = i-gap; j >= 0 && s[j] > s[j+gap]; j -= gap )
			{
				temp = s[j];
				s[j] = s[j+gap];
				s[j+gap] = temp;
			}
		}
	}
	/* Rebuild the lines table */
	RebuildLines( sort_sequence );
	FEfree(seq_tag);
}
/*
** rFagguHandler() - For each unique aggregate on the agu_list for this
**		     attribute, call vifred to act on it (choice is
**		     either to delete it or make in non-unique).
*/
static VOID
_rFagguHandler(ord, copt, choice)
COPT	*copt;
i4	choice;
{
	LIST	*list;
	FIELD	*fld;
	list = copt->agu_list;
	while (list)
	{
		fld = list->lt_field;
		VifAgguHandler(ord, fld, choice);
		list = list->lt_next;
	}
}
/*
** Name:    IIRFIsBreakColumn - Check to see if a column is a break 
**			    	column or not.
**                    
**
** Description:
**        This module will return TRUE if the column (field) passed 
**	  in is a break column; and FALSE if it is not a break column, 
**        it is an aggregate or we are editing a Labels Style report.
**
** Parameters:
**           fld  -  Field to check.
**
** Returns: 
**           TRUE   -  If break column.
**	     FALSE  -  If not break column.
**
** Called By:
**           edit2com() "in RFVIFRED".
**
** Trace Flags:
**           None.
**
** Side Effects:
**           None.
**
** History:
**	5/22/90 (martym)    Written for RBF.
**	30-oct-1992 (rdrane)
**		Ensure that unsupported datatype fields will always return
**		FALSE.
*/
bool IIRFIsBreakColumn(fld)
FIELD *fld;
{
	FLDHDR *FDgethdr();
	COPT   *copt;			
	i4    ord;
	/*
	** See if we have an aggregate first:
	*/
	if ( ((FDgethdr(fld))->fhd2flags & fdDERIVED) || 
			(St_style == RS_LABELS))
	{
		return(FALSE);
	}
	ord = r_mtch_att(fld->fldname);
	/*
	** Guard against unsupported datatype.
	*/
	if  (ord == A_ERROR)
	{
		return(FALSE);
	}
	copt = rFgt_copt(ord);
	if (copt->copt_sequence > 0)
		return(TRUE);
	else
		return(FALSE);
}
/*
** Name:    IIRFSetBrkOptions - Set options on a break column.
**                    
**
** Description:
**
** Parameters:
**           	fld    		Field to check.
**		noUndo		Set to TRUE if the Report Layout changes due to
**				a section being created.  This tells RFVIFRED 
**				to make Undo a NOP.
**
** Returns: 
**           OK   -  If option set or operation canceled.
**	     FAIL -  If option not set due to error.
**
** History:
**         5/22/90 (martym)    Written for RBF.
**	9-aug-1993 (rdrane)
**		Add yes_str and no_str so we can get rid of the embedded string
**		literals (internationalization).  At some point, we should get
**		rid of the whole yn_tbl[] interface and use IIUGyn() directly.
*/
  STATUS
  IIRFSetBrkOptions(fld, noUndo)
FIELD *fld;
bool	*noUndo;
  {
    GLOBALREF char *Nrfbrkopt;
    GLOBALREF bool alloc_rfbrkopt;
	COPT 		*copt;
	i4  		ord;
	Sec_node 	*n = (Sec_node *)NULL;
	char 		sectitle [FE_MAXNAME + 16];
	bool		canceled = FALSE;
	bool		initialized;
    char *frm;
    char *fullword;
    char *fname;
    char *clname;
    char *pralways;
    char *pronbreak;
    char *pronpage;
    char *isnewpage;
    char tmpstr[FE_MAXNAME + 1];
    char tmpbuff[FE_MAXNAME + 1];
    char *choice;
	char	*yes_str;
	char	*no_str;
	Nrfbrkopt = F_BRKOPT;
	clname = F_BKCOLN;
	pralways = F_PRNTAL;
	pronbreak = F_PRNTBRK;
	pronpage = F_PRNTPAGE;
	isnewpage = F_NEWPAGE;
	yes_str = yn_tbl[0];
	no_str = yn_tbl[1];
	fname = fld->fld_var.flregfld->flhdr.fhdname;
	ord = r_mtch_att(fname);
	copt = rFgt_copt(ord);
	if (!alloc_rfbrkopt)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), frm = Nrfbrkopt) != OK)
		{
			IIUGerr(E_RF002D_Cannot_locate_form, 0, 1, frm);
			return(FAIL);
		}
		alloc_rfbrkopt = TRUE;
	}
/* # line 769 "rfcopt.qsc" */	/* display */
    {
      if (IIdispfrm(Nrfbrkopt,(char *)"u") == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      {
/* # line 773 "rfcopt.qsc" */	/* putform */
        {
          if (IIfsetio(Nrfbrkopt) != 0) {
            IIputfldio(clname,(short *)0,1,32,0,fname);
          } /* IIfsetio */
        }
/* # line 775 "rfcopt.qsc" */	/* host code */
		STcopy(no_str, tmpstr);
		if (copt->copt_whenprint == 'a')
			STcopy(yes_str, tmpstr);
/* # line 778 "rfcopt.qsc" */	/* putform */
        {
          if (IIfsetio(Nrfbrkopt) != 0) {
            IIputfldio(pralways,(short *)0,1,32,0,tmpstr);
          } /* IIfsetio */
        }
/* # line 780 "rfcopt.qsc" */	/* host code */
		STcopy(no_str, tmpstr);
		if (copt->copt_whenprint == 'b' || copt->copt_whenprint == 't')
			STcopy(yes_str, tmpstr);
/* # line 783 "rfcopt.qsc" */	/* putform */
        {
          if (IIfsetio(Nrfbrkopt) != 0) {
            IIputfldio(pronbreak,(short *)0,1,32,0,tmpstr);
          } /* IIfsetio */
        }
/* # line 785 "rfcopt.qsc" */	/* host code */
		STcopy(no_str, tmpstr);
		if (copt->copt_whenprint == 'p' || copt->copt_whenprint == 't')
			STcopy(yes_str, tmpstr);
/* # line 788 "rfcopt.qsc" */	/* putform */
        {
          if (IIfsetio(Nrfbrkopt) != 0) {
            IIputfldio(pronpage,(short *)0,1,32,0,tmpstr);
          } /* IIfsetio */
        }
/* # line 790 "rfcopt.qsc" */	/* host code */
		STcopy(no_str, tmpstr);
		if (copt->copt_newpage)
			STcopy(yes_str, tmpstr);
/* # line 793 "rfcopt.qsc" */	/* putform */
        {
          if (IIfsetio(Nrfbrkopt) != 0) {
            IIputfldio(isnewpage,(short *)0,1,32,0,tmpstr);
          } /* IIfsetio */
        }
      }
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 799 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pralways);
              } /* IIfsetio */
            }
/* # line 800 "rfcopt.qsc" */	/* host code */
		if ((fullword = IIRFfwd_FullWord(tmpstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, 0, 0);
/* # line 803 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 804 "rfcopt.qsc" */	/* host code */
		}
		else
		{
			if (STequal(fullword, yes_str))
			{
				STcopy(no_str, tmpstr);
/* # line 810 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pronbreak,(short *)0,1,32,0,tmpstr);
              } /* IIfsetio */
            }
/* # line 811 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pronpage,(short *)0,1,32,0,tmpstr);
              } /* IIfsetio */
            }
/* # line 812 "rfcopt.qsc" */	/* host code */
			}
			else	/* We have a "no" */
			{
/* # line 815 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pronbreak);
              } /* IIfsetio */
            }
/* # line 816 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpbuff,pronpage);
              } /* IIfsetio */
            }
/* # line 817 "rfcopt.qsc" */	/* host code */
				if (STequal(tmpstr, no_str) &&
					STequal(tmpbuff, no_str))
				{
					STcopy(yes_str, tmpstr);
/* # line 821 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pronbreak,(short *)0,1,32,0,tmpstr);
              } /* IIfsetio */
            }
/* # line 822 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pronpage,(short *)0,1,32,0,tmpstr);
              } /* IIfsetio */
            }
/* # line 823 "rfcopt.qsc" */	/* host code */
				}
			}
/* # line 825 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pralways,(short *)0,1,32,0,fullword);
              } /* IIfsetio */
            }
/* # line 826 "rfcopt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
/* # line 827 "rfcopt.qsc" */	/* host code */
   		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 833 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pronbreak);
              } /* IIfsetio */
            }
/* # line 834 "rfcopt.qsc" */	/* host code */
		if ((fullword = IIRFfwd_FullWord(tmpstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, 0, 0);
/* # line 837 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 838 "rfcopt.qsc" */	/* host code */
		}
		else
		{
			if (STequal(fullword, no_str))
			{
/* # line 843 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pronpage);
              } /* IIfsetio */
            }
/* # line 844 "rfcopt.qsc" */	/* host code */
				if (STequal(tmpstr, no_str))
				{
					STcopy(yes_str, tmpstr);
/* # line 847 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pralways,(short *)0,1,32,0,tmpstr);
              } /* IIfsetio */
            }
/* # line 848 "rfcopt.qsc" */	/* host code */
				}
			}
			else	/* We have a "yes" */
			{
/* # line 852 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pralways);
              } /* IIfsetio */
            }
/* # line 853 "rfcopt.qsc" */	/* host code */
				if (STequal(tmpstr, yes_str))
				{
					STcopy(no_str, tmpstr);
/* # line 856 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pralways,(short *)0,1,32,0,tmpstr);
              } /* IIfsetio */
            }
/* # line 857 "rfcopt.qsc" */	/* host code */
				}
			}
/* # line 859 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pronbreak,(short *)0,1,32,0,fullword);
              } /* IIfsetio */
            }
/* # line 860 "rfcopt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
/* # line 861 "rfcopt.qsc" */	/* host code */
  		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 866 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pronpage);
              } /* IIfsetio */
            }
/* # line 867 "rfcopt.qsc" */	/* host code */
		if ((fullword = IIRFfwd_FullWord(tmpstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, 0, 0);
/* # line 870 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 871 "rfcopt.qsc" */	/* host code */
		}
		else
		{
			if (STequal(fullword, no_str))
			{
/* # line 876 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pronbreak);
              } /* IIfsetio */
            }
/* # line 877 "rfcopt.qsc" */	/* host code */
				if (STequal(tmpstr, no_str))
				{
					STcopy(yes_str, tmpstr);
/* # line 880 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pralways,(short *)0,1,32,0,tmpstr);
              } /* IIfsetio */
            }
/* # line 881 "rfcopt.qsc" */	/* host code */
				}
			}
			else	/* We have a "yes" */
			{
/* # line 885 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pralways);
              } /* IIfsetio */
            }
/* # line 886 "rfcopt.qsc" */	/* host code */
				if (STequal(tmpstr, yes_str))
				{
					STcopy(no_str, tmpstr);
/* # line 889 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pralways,(short *)0,1,32,0,tmpstr);
              } /* IIfsetio */
            }
/* # line 890 "rfcopt.qsc" */	/* host code */
				}
			}
/* # line 892 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(pronpage,(short *)0,1,32,0,fullword);
              } /* IIfsetio */
            }
/* # line 893 "rfcopt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
/* # line 894 "rfcopt.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 899 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,isnewpage);
              } /* IIfsetio */
            }
/* # line 900 "rfcopt.qsc" */	/* host code */
		if ((fullword = IIRFfwd_FullWord(tmpstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, 0, 0);
/* # line 903 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
/* # line 904 "rfcopt.qsc" */	/* host code */
		}
		else
		{
/* # line 907 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIputfldio(isnewpage,(short *)0,1,32,0,fullword);
              } /* IIfsetio */
            }
/* # line 908 "rfcopt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
/* # line 909 "rfcopt.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 5) {
          {
/* # line 916 "rfcopt.qsc" */	/* host code */
		copt->copt_whenprint = ' ';
/* # line 917 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pralways);
              } /* IIfsetio */
            }
/* # line 918 "rfcopt.qsc" */	/* host code */
		if (STequal(tmpstr, yes_str))
			copt->copt_whenprint = 'a';
/* # line 921 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pronbreak);
              } /* IIfsetio */
            }
/* # line 922 "rfcopt.qsc" */	/* host code */
		if (STequal(tmpstr, yes_str))
			copt->copt_whenprint = 'b';
/* # line 925 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,pronpage);
              } /* IIfsetio */
            }
/* # line 926 "rfcopt.qsc" */	/* host code */
		if (STequal(tmpstr, yes_str))
		{
			if (copt->copt_whenprint == 'b')
				copt->copt_whenprint = 't';
			else
				copt->copt_whenprint = 'p';
		}
/* # line 934 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfbrkopt) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,isnewpage);
              } /* IIfsetio */
            }
/* # line 935 "rfcopt.qsc" */	/* host code */
		if (STequal(tmpstr, yes_str))
			copt->copt_newpage = TRUE;
		else
			copt->copt_newpage = FALSE;
/* # line 940 "rfcopt.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 946 "rfcopt.qsc" */	/* host code */
		canceled = TRUE;
/* # line 947 "rfcopt.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 955 "rfcopt.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,Nrfbrkopt,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,19,(char *)0,
                0);
              } /* IIiqset */
            }
/* # line 957 "rfcopt.qsc" */	/* host code */
		if (STequal(tmpstr, pralways))
			choice = ERget(S_RF008B_PrintAlways);
		else
		if (STequal(tmpstr, pronbreak))
			choice = ERget(S_RF008C_PrintOnBreak);
		else
		if (STequal(tmpstr, pronpage))
			choice = ERget(S_RF008D_PrintOnPageBreak);
		else
		if (STequal(tmpstr, isnewpage))
			choice = ERget(S_RF008E_SetNewPage);
/* # line 969 "rfcopt.qsc" */	/* message */
            {
              IIFRgpcontrol(1,0);
              IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
              IIFRgpcontrol(2,0);
              IImessage(choice);
            }
/* # line 970 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 977 "rfcopt.qsc" */	/* host code */
		FEhelp(H_BRKOPTS, ERget(F_RF0072_Break_Options));
          }
        } else {
          if (1) goto IIfdE2;
        } /* IIretval */
      } /* IIrunform */
IIfdF2:
      if (IIchkfrm() == 0) goto IIfdB2;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      goto IIfdE2;
IImuI2:
      if (IIFRafActFld(pralways,0,1) == 0) goto IIfdE2;
      if (IIFRafActFld(pronbreak,0,2) == 0) goto IIfdE2;
      if (IIFRafActFld(pronpage,0,3) == 0) goto IIfdE2;
      if (IIFRafActFld(isnewpage,0,4) == 0) goto IIfdE2;
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_OK),ERget(F_RF0077_brk_options),2,2,5) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,5) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,6) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,6) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,7) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,7) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,8) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,8) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 981 "rfcopt.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 983 "rfcopt.qsc" */	/* host code */
	if (canceled)
		return(OK);
	/*
	** See if we need to create any sections to reflect the 
	** choices made by the user:
	*/
	initialized = FALSE;
	if (copt->copt_whenprint == 'b' || copt->copt_whenprint == 't')
	{
		if (copt->copt_brkhdr == 'n') 
		{
			copt->copt_brkhdr = 'y';
			n = sec_node_alloc(SEC_BRKHDR, copt->copt_sequence, 0);
			sec_list_insert(n, &Sections);
			STcopy(fname, sectitle);
			CMtoupper(sectitle, sectitle);
			STcat(sectitle, ERget(F_RF003E_Break_Header));
			n->sec_name = STalloc(sectitle);
			n->sec_brkord = r_mtch_att(fname);
			create_section(n, initialized);
			initialized = TRUE;
			*noUndo = TRUE;
		}
	}
	if (copt->copt_whenprint == 'p' || copt->copt_whenprint == 't')
	{
		if (!(n = sec_list_find(SEC_PAGHDR, 0, &Sections)))
		{
			n = sec_node_alloc(SEC_PAGHDR, 0, 0);
			sec_list_insert(n, &Sections);
			STcopy(ERget(F_RF003A_Page_Header), sectitle);
			n->sec_name = STalloc(sectitle);
			n->sec_brkord = 0;
			create_section(n, initialized);
			initialized = TRUE;
			*noUndo = TRUE;
		}
	}
	if (copt->copt_newpage)
	{
		if (copt->copt_brkftr == 'n') 
		{
			copt->copt_brkftr = 'y';
			n = sec_node_alloc(SEC_BRKFTR, copt->copt_sequence, 0);
			sec_list_insert(n, &Sections);
			STcopy(fname, sectitle);
			CMtoupper(sectitle, sectitle);
			STcat(sectitle, ERget(F_RF003F_Break_Footer));
			n->sec_name = STalloc(sectitle);
			n->sec_brkord = r_mtch_att(fname);
			create_section(n, initialized);
			initialized = TRUE;
			*noUndo = TRUE;
		}
	}
	return(OK);
  }
  STATUS
  IIRFSetNewPageOption(fld, noUndo)
FIELD *fld;
bool	*noUndo;
  {
    GLOBALREF char *Nrfnwpag;
    GLOBALREF bool alloc_rfnwpag;
	COPT 		*copt;
	i4  		ord;
	Sec_node 	*n = (Sec_node *)NULL;
	char 		sectitle [FE_MAXNAME + 16];
	bool		canceled = FALSE;
    char *frm;
    char *fullword;
    char *fname;
    char *clname;
    char *isnewpage;
    char tmpstr[FE_MAXNAME + 1];
    char *choice;
	char	*yes_str;
	char	*no_str;
	Nrfnwpag = F_PAGOPT;
	clname = F_BKCOLN;
	isnewpage = F_NEWPAGE;
	yes_str = yn_tbl[0];
	no_str = yn_tbl[1];
	fname = fld->fld_var.flregfld->flhdr.fhdname;
	ord = r_mtch_att(fname);
	copt = rFgt_copt(ord);
	if (!alloc_rfnwpag)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), frm = Nrfnwpag) != OK)
		{
			IIUGerr(E_RF002D_Cannot_locate_form, 0, 1, frm);
			return(FAIL);
		}
		alloc_rfnwpag = TRUE;
	}
/* # line 1094 "rfcopt.qsc" */	/* display */
    {
      if (IIdispfrm(Nrfnwpag,(char *)"u") == 0) goto IIfdE3;
      goto IImuI3;
IIfdI3:;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
      {
/* # line 1098 "rfcopt.qsc" */	/* putform */
        {
          if (IIfsetio(Nrfnwpag) != 0) {
            IIputfldio(clname,(short *)0,1,32,0,fname);
          } /* IIfsetio */
        }
/* # line 1100 "rfcopt.qsc" */	/* host code */
		STcopy(no_str, tmpstr);
		if (copt->copt_newpage)
			STcopy(yes_str, tmpstr);
/* # line 1103 "rfcopt.qsc" */	/* putform */
        {
          if (IIfsetio(Nrfnwpag) != 0) {
            IIputfldio(isnewpage,(short *)0,1,32,0,tmpstr);
          } /* IIfsetio */
        }
      }
IIfdB3:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 1109 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfnwpag) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,isnewpage);
              } /* IIfsetio */
            }
/* # line 1110 "rfcopt.qsc" */	/* host code */
		if ((fullword = IIRFfwd_FullWord(tmpstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, 0, 0);
/* # line 1113 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB3;
            }
/* # line 1114 "rfcopt.qsc" */	/* host code */
		}
		else
		{
/* # line 1117 "rfcopt.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfnwpag) != 0) {
                IIputfldio(isnewpage,(short *)0,1,32,0,fullword);
              } /* IIfsetio */
            }
/* # line 1118 "rfcopt.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB3;
            }
/* # line 1119 "rfcopt.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 1126 "rfcopt.qsc" */	/* getform */
            {
              if (IIfsetio(Nrfnwpag) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tmpstr,isnewpage);
              } /* IIfsetio */
            }
/* # line 1127 "rfcopt.qsc" */	/* host code */
		if (STequal(tmpstr, yes_str))
			copt->copt_newpage = TRUE;
		else
			copt->copt_newpage = FALSE;
/* # line 1132 "rfcopt.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 1138 "rfcopt.qsc" */	/* host code */
		canceled = TRUE;
/* # line 1139 "rfcopt.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 1147 "rfcopt.qsc" */	/* host code */
		choice = ERget(S_RF008E_SetNewPage);
/* # line 1149 "rfcopt.qsc" */	/* message */
            {
              IIFRgpcontrol(1,0);
              IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
              IIFRgpcontrol(2,0);
              IImessage(choice);
            }
/* # line 1150 "rfcopt.qsc" */	/* resume */
            {
              if (1) goto IIfdB3;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 1157 "rfcopt.qsc" */	/* host code */
		FEhelp(H_PAGOPTS, ERget(F_RF0072_Break_Options));
          }
        } else {
          if (1) goto IIfdE3;
        } /* IIretval */
      } /* IIrunform */
IIfdF3:
      if (IIchkfrm() == 0) goto IIfdB3;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
      goto IIfdE3;
IImuI3:
      if (IIFRafActFld(isnewpage,0,1) == 0) goto IIfdE3;
      if (IIinitmu() == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_OK),ERget(F_RF0077_brk_options),2,2,2) == 0) goto 
      IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,3) == 0) 
      goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,3) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,4) == 0) goto 
      IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,4) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,5) == 0) 
      goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,5) == 0) goto IIfdE3;
      if (IIendmu() == 0) goto IIfdE3;
      goto IIfdI3;
IIfdE3:;
      IIendfrm();
    }
/* # line 1161 "rfcopt.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 1163 "rfcopt.qsc" */	/* host code */
	if (canceled)
		return(OK);
	/*
	** See if we need to create any sections to reflect the 
	** choices made by the user:
	*/
	if (copt->copt_newpage)
	{
		if (copt->copt_brkftr == 'n') 
		{
			copt->copt_brkftr = 'y';
			n = sec_node_alloc(SEC_BRKFTR, copt->copt_sequence, 0);
			sec_list_insert(n, &Sections);
			STcopy(fname, sectitle);
			CMtoupper(sectitle, sectitle);
			STcat(sectitle, ERget(F_RF003F_Break_Footer));
			n->sec_name = STalloc(sectitle);
			n->sec_brkord = r_mtch_att(fname);
			create_section(n, FALSE);
			*noUndo = TRUE;
		}
	}
	return(OK);
  }
