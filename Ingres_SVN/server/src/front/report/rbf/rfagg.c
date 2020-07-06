# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	 Copyright (c) 2004 Ingres Corporation
*/
/* static char	Sccsid[] = "@(#)rfcopt.qc	30.1	11/14/84"; */
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"rbftype.h"
# include	"rbfglob.h"
# include	<cm.h>
# include	<ug.h>
# include	<flddesc.h>
# include   "rbfcons.h"
# include       "uf.h"
/*
**    RFAGG - called when the Create Aggregate selection is made on
**	the main RBF layout form.  This displays a form which allows
**	allows the user to create an aggregate in a footer section.
**
**	Parameters:
**
**	Returns:
**		TRUE	if everything is OK,  FALSE if anything wrong.
**
**	Called by:
**		VIFRED routines.
**
**	Side Effects:
**
**	Trace Flags:
**
**	History:
**		30-oct-89 (cmr)	written.
**		11/28/89  (matrym)  Included 'rbfcons.h'.
**		11/29/89  (martym)  Took out the call to rFag_sel().
**		03-jan-90 (cmr)	check that unique (prime) aggs only happen
**				on break columns.  Also, rFagg() returns the
**				index of the AGG_INFO not a ptr to it.
**		15-jan-90 (cmr)	Put in support for Help.
**		28-jan-90 (cmr) Sound bell if user tries to scroll beyond
**				first or last row in the tablefield.
**		06-feb-90 (cmr)	Cancel is mapped to FRSkey9 not FRSkey3.
**		22-apr-90 (cmr)	When the user selects an agg function display
**				another popup to allow the user to specify
**				cumulative aggregation.
**		4/25/90 (elein) Change ag_index from invisible column to a
**				hidden column
**		17-may-90 (cmr)	Limit the table fields rows to 10 when 
**				calling IIFDlpListPick().
**		06-sep-90 (sylviap)
**			Added explanations to Help, Keys. #32699
**		19-oct-1990 (mgw)
**			Fixed include of local rbftype.h and rbfglob.h to ""
**			instead of <>
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
  GLOBALREF char *Naggregates;
GLOBALREF       bool    alloc_aggs;
i4
rFagg(ord, cum)
i4	ord;
bool	*cum;
{
	/* internal declarations */
	COPT	*copt;			/* COPT struct for attribute */
  i4 i;
  i4 crow;
  char ibuf[33];
  char *attname;
  i4 a_index = -1;
  char *form;
	i4	rows = 0;
	Naggregates = F_AGGREGATES;
	if (!alloc_aggs)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(),
			(form = Naggregates))!=OK)
		{
	    		IIUGerr(E_RF002D_Cannot_locate_form, 0, 1, form);
		    	return (NULL);
		 }
		 alloc_aggs = TRUE;
	}        
	*cum = FALSE;
/* # line 107 "rfagg.qsc" */	/* display */
  {
    if (IIdispfrm(Naggregates,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 110 "rfagg.qsc" */	/* inittable */
      {
        if (IItbinit(Naggregates,(char *)"aggtbl",(char *)"f") != 0) {
          IIthidecol((char *)"ag_index",(char *)"i4");
          IItfill();
        } /* IItbinit */
      }
/* # line 111 "rfagg.qsc" */	/* host code */
		/* Add the appropriate aggregate fields to the tablefield */
		copt = rFgt_copt(ord);
		/* for each aggregate for attribute */
		for (i = 0; i < copt->num_ags; i++)
		{
			/* prime (unique) aggs are for break columns only. */
			if ( copt->copt_break != 'y' &&
				copt->agginfo[i].agnames->ag_prime == TRUE )
					continue;
			/*	Get the aggregate name */
			STcopy(copt->agginfo[i].agnames->ag_framelabel, ibuf);
/* # line 126 "rfagg.qsc" */	/* loadtable */
      {
        if (IItbact(Naggregates,(char *)"aggtbl",1) != 0) {
          IItcoputio((char *)"ag_index",(short *)0,1,30,4,&i);
          IItcoputio((char *)"ag_name",(short *)0,1,32,0,ibuf);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 130 "rfagg.qsc" */	/* host code */
			rows++;
			}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 136 "rfagg.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Naggregates,(char *)"aggtbl",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&a_index,(char *)"ag_index");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 139 "rfagg.qsc" */	/* host code */
		i = IIFDlpListPick( ERget(F_RF006F_Cum_Agg_Prompt),
			ERget(F_RF0070_Cum_Agg_Choices), 10, LPGR_FLOAT, 
			LPGR_FLOAT, ERget(F_RF0071_Cum_Agg_Help),
			H_CUMAGGS, NULL, NULL);
			if ( i >= 0)
			{
				*cum = (i != 0 ? TRUE : FALSE);
			}
			else	/* Cancel */
			{
				a_index = -1;
/* # line 150 "rfagg.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 151 "rfagg.qsc" */	/* host code */
			}
/* # line 152 "rfagg.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 157 "rfagg.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 163 "rfagg.qsc" */	/* host code */
		FEhelp(H_AGGREGATE, ERget(F_RF0061_Aggregate_Functions));
        }
      } else if (IIretval() == 4) {
        {
/* # line 168 "rfagg.qsc" */	/* host code */
		FEtabfnd(Naggregates, ERx("aggtbl"));
        }
      } else if (IIretval() == 5) {
        {
/* # line 173 "rfagg.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Naggregates,(char *)"aggtbl",-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 178 "rfagg.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Naggregates,(char *)"aggtbl",-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 183 "rfagg.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Naggregates,(char *)"aggtbl",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&crow,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 184 "rfagg.qsc" */	/* host code */
		if ( crow == 1 )
		{
			FTbell();
		}
		else
		{
/* # line 190 "rfagg.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Naggregates,(char *)"aggtbl",-3) != 0) {
              IItbsmode((char *)"down");
              if (IItscroll(0,-3) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 191 "rfagg.qsc" */	/* host code */
		}
/* # line 192 "rfagg.qsc" */	/* resume */
          {
            IIresfld((char *)"aggtbl");
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 197 "rfagg.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Naggregates,(char *)"aggtbl",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&crow,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 198 "rfagg.qsc" */	/* host code */
		if ( crow == rows )
		{
			FTbell();
		}
		else
		{
/* # line 204 "rfagg.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Naggregates,(char *)"aggtbl",-3) != 0) {
              IItbsmode((char *)"up");
              if (IItscroll(0,-3) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 205 "rfagg.qsc" */	/* host code */
		}
/* # line 206 "rfagg.qsc" */	/* resume */
          {
            IIresfld((char *)"aggtbl");
            if (1) goto IIfdB1;
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
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Select),ERget(F_RF0088_sel_agg),2,2,1) == 0) goto 
    IIfdE1;
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
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,6) == 0) goto IIfdE1;
    if (IIactscrl((char *)"aggtbl",1,7) == 0) goto IIfdE1;
    if (IIactscrl((char *)"aggtbl",0,8) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 208 "rfagg.qsc" */	/* host code */
	return (a_index);
}
