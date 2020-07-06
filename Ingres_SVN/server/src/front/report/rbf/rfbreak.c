# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	 Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	 "rbftype.h"
# include	 "rbfglob.h"
# include	 "errf.h"
# include	 <erug.h>
# include	 <si.h>
# include	 <st.h>
# include	 <cm.h>
/*
**   RFBREAK - display and control the BREAK form to select
**	a sort column suitable for creating/deleting a break
**	header or footer.
**
**	Note that this fills in the fields of the Break form
**	from the current COPT structure, which should be up to date.
**
**	Parameters:
**		sect		which break section (SEC_BRKHDR/SEC_BRKFTR)
**		seq		ptr to sequence number of sort column
**		name		ptr to name of the sort column
**		
**
**	Returns:
**		TRUE if a selection was made, FALSE otherwise.
**
**	Side Effects:
**		Fields in the COPT structure.
**		Sets seq and name.
**
**	Called By:
**		rFcopt.
**
**	Trace Flags:
**
**	Error Messages:
**		none.
**
**	History:
**	22-aug-89 (cmr) -	written.
**	16-oct-89 (cmr)	-	calculate a default sort seq if the
**				user doesn't specify one.
**	22-nov-89 (cmr)	-	sec_list* routines now take another argument.
**	11/27/89  (martym) - Added parameter to indicate if rfvifred should 
**			be called.
**	03-jan-90 (cmr) -	uppercase first char of break name.
**	08-jan-90 (cmr)	-	rewrite for new user interface changes.
**	24-jan-90 (cmr) -	corrected order of menu commands.
**	25-apr-90 (elein) -	change brk_srtord from invisible to hidden
**	04-sep-90 (sylviap) -	Added explanations to the activate menuitems
**				#32699
**	19-oct-1990 (mgw)
**		Fixed include of local errf.h, rbftype.h and rbfglob.h to ""
**		instead of <>
**	30-oct-1992 (rdrane)
**		Ensure that unsupported datatype fields cannot be selected.
**		Note that they shouldn't be able to assigned a sort sequence
**		in the first place, and so we may just be being paranoid.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
bool
rFbreak( sect, seq, name )
i4	sect, *seq;
char	*name;
{
	/* external declarations */
  GLOBALREF char *Nstructure;
	/* internal declarations */
	register COPT	*copt;			/* COPT structure */
  char colname[FE_MAXNAME +1];
  char *attname;
  i4 i;
	ATT		*att;
	bool		selected, no_sort_cols = TRUE;
	Nstructure = F_BREAK;
/* # line 92 "rfbreak.qsc" */	/* display */
  {
    if (IIdispfrm(Nstructure,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 96 "rfbreak.qsc" */	/* inittable */
      {
        if (IItbinit(Nstructure,(char *)"brktbl",(char *)"u") != 0) {
          IIthidecol((char *)"brk_srtord",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 98 "rfbreak.qsc" */	/* host code */
		for ( i = 1; i <= En_n_attribs; i++ )
		{
			copt = rFgt_copt( i );
			if ( !copt->copt_sequence )
				continue;
			if ( (sect == SEC_BRKHDR &&
			    	copt->copt_brkhdr == 'n')
			    || ( sect == SEC_BRKFTR &&
			    	copt->copt_brkftr == 'n') )
			{
				/*
				** Guard against unsupported datatype.
				** Still, they shouldn't have a non-zero
				** sort sequence.
				*/
				if  ((att = r_gt_att(i)) == (ATT *)NULL)
				{
					continue;
				}
				attname = att->att_name;
				no_sort_cols = FALSE;
/* # line 119 "rfbreak.qsc" */	/* loadtable */
      {
        if (IItbact(Nstructure,(char *)"brktbl",1) != 0) {
          IItcoputio((char *)"brk_srtord",(short *)0,1,30,4,&i);
          IItcoputio((char *)"brk_srtcol",(short *)0,1,32,0,attname);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 122 "rfbreak.qsc" */	/* host code */
			}
		}
		if ( no_sort_cols )
		{
			IIUGerr(E_RF0074_No_sort_cols, 0, 0);
			selected = FALSE;
/* # line 128 "rfbreak.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE1;
      }
/* # line 129 "rfbreak.qsc" */	/* host code */
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 136 "rfbreak.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Nstructure,(char *)"brktbl",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&i,(char *)"brk_srtord");
              IItcogetio((short *)0,1,32,FE_MAXNAME +1-1,colname,(char *)
"brk_srtcol");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 140 "rfbreak.qsc" */	/* host code */
		copt = rFgt_copt( i );
		*seq = copt->copt_sequence;
		STcopy( colname, name );
		if ( sect == SEC_BRKHDR )
			copt->copt_brkhdr = 'y';
		else
			copt->copt_brkftr = 'y';
		selected = TRUE;
/* # line 148 "rfbreak.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 154 "rfbreak.qsc" */	/* host code */
		selected = FALSE;
/* # line 155 "rfbreak.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 161 "rfbreak.qsc" */	/* host code */
		FEhelp(H_STRUCTURE, ERget(F_RF0028_Report_Sort_Order));
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
    if (IInmuact(ERget(FE_Select),ERget(F_RF0060_sel_sort_cols),2,2,1) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,3) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 164 "rfbreak.qsc" */	/* host code */
	return(selected);
}
