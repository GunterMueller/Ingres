# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
#include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<cm.h>
#include 	<st.h> 
#include	"rbftype.h"
#include	"rbfglob.h"
# include	<flddesc.h>
#include        <uf.h>
/*
NO_OPTIM=int_lnx int_rpl ibm_lnx
*/
/*
**   RFEDITLAYOUT - display and control the LAYOUT form to select
**	which sections are to be added/removed fro the layout.
**
**	Parameters:
**		none.
**
**	Returns:
**		TRUE if any changes made, FALSE if not.
**
**	Side Effects:
**		Changes the Sections list.
**
**	Called By:
**
**	Trace Flags:
**
**	Error Messages:
**		none.
**
**	History:
**	22-aug-89 (cmr) -	written.
**	16-oct-89 (cmr)	-	changed interface to rfvifred calls.
**	22-nov-89 (cmr) -	sec_list* routines now take another argument.
**	03-jan-90 (cmr) -	delete Top/Bottom option, don't clearscreen, 
**				and setup Help.
**	08-jan-90 (cmr) -	rewrite to support new user interface.
**	15-jan-90 (cmr) -	support for Help and fixed an off-by-one error.
**	08-feb-90 (cmr) -	Call SectionsUpdate() when creating/deleting a
**				section.  Vifred no longer calls newLimits().
**	26-feb-90 (cmr) -	Deleting a section - check status returned  by
**				by delnLines(). Also, check # of rows before 
**				doing a getrow.
**	2/28/90 (martym)- 	Added check, not to allow users to delete the 
**				Report Footer Section when the report is a 
**				Labels Style Report.
**	15-mar-90 (cmr) -	Set the column ordinal (sec_brkord) for 
**				break headers and footers.
**	25-apr-90 (elein) -	Change lay_sectype and lay_secseq from
**				invisible columns to hidden columns
**	18-may-90 (cmr) -	Do a ##loadtable rather than a ##insertrow if
**				the section catalog is empty.
**	20-jul-90 (cmr)	-	Corrected logic for return status.
**	03-aug-90 (cmr)	-	Fix for bugs 32080 and 32083.  Added boolean
**				to [create,delete]_section() to tell rfvifred
**				to initialize the line array.  This needs to 
**				happen only once per Layout operation before 
**				the first section is created or deleted (not 
**				every time a section is created/deleted).  And
**				you don't want the initialization up front cuz
**				the user may just do an End w/o changes, and
**				Undo would get screwed up.
**	04-sep-90 (sylviap)-	Added explanations to activate menuitems. #32699
**	19-oct-1990 (mgw)
**		Fixed include of local rbftype.h and rbfglob.h to ""
**		instead of <>
**	05/19/93 (dkh) - Fixed bug 50791.  The layout handling code will
**			 now correctly handle straightedges when deleting
**			 a section.
**	05-jun-1998 (toumi01)
**		NO_OPTIM lnx_us5 because optimized version causes rbf to
**		SEGV adding a report footer when the value of the "data"
**		variable goes to zero mid-way through routine rFlpCreateSec.
**     02-Jul-1999 (hweho01)
**              Included uf.h header file, it contains the function     
**              prototype of IIUFlcfLocateForm(). Without the explicit
**              declaration, the default int return value for a  
**              function will truncate a 64-bit address on ris_u64 
**              platform.
**	06-oct-1999 (toumi01)
**		Change Linux config string from lnx_us5 to int_lnx.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	22-aug-2000 (somsa01)
**		NO_OPTIM ibm_lnx because optimized version causes rbf to
**		SEGV adding a report footer when the value of the "data"
**		variable goes to zero mid-way through routine rFlpCreateSec.
**       6-Nov-2006 (hanal04) SIR 117044
**          Add int.rpl for Intel Rpath Linux build.
*/
FUNC_EXTERN	i4	rFlpCreateSec();
FUNC_EXTERN	void	IIVFdscDelSecCleanup();
static	bool	CheckBreakOptions();
static  bool	delete_section();
VOID		create_section();
bool		changes;
bool
rFeditlayout()
{
  GLOBALREF char *Nlayout;
  GLOBALREF bool alloc_layout;
  char sec_name[FE_MAXNAME + 16];
char		*l_name;
  i4 sect, seq, rows;
char		choices[200];
char		*form, *s;
COPT		*copt;
i4		i;
Sec_node	*n;
i4		sec_array[6], *sections;
STATUS		status;
if ( !alloc_layout)
{
	Nlayout = F_EDITLAYOUT;
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), form = Nlayout) != OK)
	{
		IIUGerr(E_RF002D_Cannot_locate_form, 0, 1, form);
		return(FALSE);
	}
	alloc_layout = TRUE;
}
	changes = FALSE;
/* # line 141 "rfedtlay.qsc" */	/* display */
  {
    if (IIdispfrm(Nlayout,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 144 "rfedtlay.qsc" */	/* inittable */
      {
        if (IItbinit(Nlayout,(char *)"rflaytbl",(char *)"u") != 0) {
          IIthidecol((char *)"lay_sectype",(char *)"i4");
          IIthidecol((char *)"lay_secseq",(char *)"i4");
          IItfill();
        } /* IItbinit */
      }
/* # line 145 "rfedtlay.qsc" */	/* host code */
		for ( n = Sections.head; n; n = n->next )
		{
			if ( n->sec_type == SEC_DETAIL 
				|| n->sec_type == SEC_END )
					continue;
			STcopy( n->sec_name, sec_name );
			l_name = sec_name;
			while ( l_name = STindex( sec_name, ERx("-"), 0) )
				*l_name = ' ';
			sect = n->sec_type;
			seq = n->sec_brkseq;
/* # line 156 "rfedtlay.qsc" */	/* loadtable */
      {
        if (IItbact(Nlayout,(char *)"rflaytbl",1) != 0) {
          IItcoputio((char *)"lay_sec",(short *)0,1,32,0,sec_name);
          IItcoputio((char *)"lay_sectype",(short *)0,1,30,4,&sect);
          IItcoputio((char *)"lay_secseq",(short *)0,1,30,4,&seq);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 160 "rfedtlay.qsc" */	/* host code */
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 167 "rfedtlay.qsc" */	/* host code */
	s = choices;
	sections = sec_array;
	for ( i = SEC_REPHDR; i < SEC_END; i++ )
	{
		if ( n = sec_list_find( i, 0, &Sections )  )
			continue;
		switch ( i )
		{
			case SEC_REPHDR:
				STcopy( ERget(F_RF0033_Report_Header), s );
				break;
			case SEC_REPFTR:
				STcopy( ERget(F_RF0034_Report_Footer), s );
				break;
			case SEC_PAGHDR:
				STcopy( ERget(F_RF0035_Page_Header), s );
				break;
			case SEC_PAGFTR:
				STcopy( ERget(F_RF0036_Page_Footer), s );
				break;
			case SEC_BRKHDR:
				STcopy( ERget(F_RF0057_Break_Header), s );
				break;
			case SEC_BRKFTR:
				STcopy( ERget(F_RF0058_Break_Footer), s );
				break;
		}
		s += STlength( s );
		*s++ = '\n';
		*sections++ = i;
	}
	*--s = EOS;
	IIFDlpListPick( ERget(F_RF005B_Create_Section_Title),
			choices, 0, LPGR_FLOAT, LPGR_FLOAT, 
			ERget(F_RF005C_Create_Section_Help), 
			H_CREATSECT, rFlpCreateSec, (PTR)sec_array );
        }
      } else if (IIretval() == 2) {
        {
/* # line 208 "rfedtlay.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Nlayout,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&rows,32,(char *)0,0);
            } /* IIiqset */
          }
/* # line 210 "rfedtlay.qsc" */	/* host code */
		if ( rows == 0 )
		{
			IIUGerr(E_RF007F_No_more_sections, 0, 0);
/* # line 213 "rfedtlay.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 214 "rfedtlay.qsc" */	/* host code */
		}
/* # line 216 "rfedtlay.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Nlayout,(char *)"rflaytbl",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&sect,(char *)"lay_sectype");
              IItcogetio((short *)0,1,30,4,&seq,(char *)"lay_secseq");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 220 "rfedtlay.qsc" */	/* host code */
		if ( n = sec_list_find(sect, seq, &Sections))
		{
			if (sect == SEC_REPFTR && St_style == RS_LABELS)
			{
				IIUGerr(E_RF0080_Cant_Del_Lab_RepFtr, 0, 0);
/* # line 225 "rfedtlay.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 226 "rfedtlay.qsc" */	/* host code */
			}
			else
			{
				/*
				** Check if this sections is needed to support 
				** the options set in the BreakOptions popup.
				*/
				if (!CheckBreakOptions(n))
/* # line 234 "rfedtlay.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 235 "rfedtlay.qsc" */	/* host code */
				status = delete_section(n, changes);
				if (status == FALSE)
/* # line 237 "rfedtlay.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 238 "rfedtlay.qsc" */	/* host code */
				else
					changes = TRUE;
			}
		}
		if ( sect == SEC_BRKHDR || sect == SEC_BRKFTR )
		{
			for ( i = 1; i <= En_n_attribs; i++ )
			{
				copt = rFgt_copt( i );
				if ( copt->copt_sequence != seq )
					continue; 
					if ( sect == SEC_BRKHDR ) 
						copt->copt_brkhdr = 'n';
				else
					copt->copt_brkftr = 'n';
				break;
			}
		}
		changes = TRUE;
/* # line 257 "rfedtlay.qsc" */	/* deleterow */
          {
            if (IItbsetio(4,Nlayout,(char *)"rflaytbl",-2) != 0) {
              if (IItdelrow(0) != 0) {
              } /* IItdelrow */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 263 "rfedtlay.qsc" */	/* host code */
		FEhelp(H_SECTIONS, ERget(F_RF0040_Edit_Layout));
        }
      } else if (IIretval() == 4) {
        {
/* # line 267 "rfedtlay.qsc" */	/* enddisplay */
          {
            if (1) goto IIfdF1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 271 "rfedtlay.qsc" */	/* host code */
		FEtabfnd(Nlayout, ERx("rflaytbl"));
        }
      } else if (IIretval() == 6) {
        {
/* # line 276 "rfedtlay.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Nlayout,(char *)"rflaytbl",-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 281 "rfedtlay.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Nlayout,(char *)"rflaytbl",-3) != 0) {
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
    if (IInmuact(ERget(F_RF0059_Create_Layout_Section),
    ERget(F_RF0073_cr_layout_sec),2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(F_RF005A_Delete_Layout_Section),
    ERget(F_RF0074_del_layout_sec),2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,3) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,7) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 286 "rfedtlay.qsc" */	/* host code */
return(changes);
}
rFlpCreateSec( data, choice, resume )
PTR	data;
i4	choice;
bool	*resume;
{
	i4 	*sec_array = (i4 *)data;
  i4 sec_type, sequence;
  i4 tot_recs;
	char	colname[FE_MAXNAME + 1];
	char    sec_title[FE_MAXNAME + 16];
  char *layout_name;
	char	*s;
	Sec_node	*n;
	*resume = FALSE;
	if ( choice == -1 )
		return;
	sec_type = sec_array[choice];
	switch ( sec_type )
	{
		case SEC_REPHDR:
			sequence = 0;
			STcopy(ERget(F_RF0038_Report_Header), sec_title);
			break;
		case SEC_REPFTR:
			sequence = 0;
			STcopy(ERget(F_RF0039_Report_Footer), sec_title);
			break;
		case SEC_PAGHDR:
			sequence = 0;
			STcopy(ERget(F_RF003A_Page_Header), sec_title);
			break;
		case SEC_PAGFTR:
			sequence = 0;
			STcopy(ERget(F_RF003B_Page_Footer), sec_title);
			break;
		case SEC_BRKHDR:
		case SEC_BRKFTR:
			if ( !rFbreak( sec_type, &sequence, colname ) )
			{
				*resume = TRUE;
				return;
			}
			STcopy( colname, sec_title );
			CMtoupper( sec_title, sec_title );
			if ( sec_type == SEC_BRKHDR )
			{
				STcat(sec_title, ERget(F_RF003E_Break_Header));
			}
			else
			{
				STcat(sec_title, ERget(F_RF003F_Break_Footer));
			}
			break;
		}
		if (!(n = sec_list_find(sec_type, sequence, &Sections)))
		{
			n = sec_node_alloc( sec_type, sequence, 0);
			sec_list_insert( n, &Sections );
			n->sec_name = STalloc( sec_title );
			n->sec_brkord = r_mtch_att( colname );
			create_section( n, changes );
			changes = TRUE;
		}
		layout_name = sec_title;
		while ( s = STindex( layout_name, ERx("-"), 0 ) )
			*s = ' ';
/* # line 358 "rfedtlay.qsc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,Nlayout,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&tot_recs,32,(char *)0,0);
    } /* IIiqset */
  }
/* # line 360 "rfedtlay.qsc" */	/* host code */
		if (tot_recs == 0)
		{
/* # line 362 "rfedtlay.qsc" */	/* loadtable */
  {
    if (IItbact(Nlayout,(char *)"rflaytbl",1) != 0) {
      IItcoputio((char *)"lay_sec",(short *)0,1,32,0,layout_name);
      IItcoputio((char *)"lay_sectype",(short *)0,1,30,4,&sec_type);
      IItcoputio((char *)"lay_secseq",(short *)0,1,30,4,&sequence);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 366 "rfedtlay.qsc" */	/* host code */
		}
		else
		{
/* # line 369 "rfedtlay.qsc" */	/* insertrow */
  {
    if (IItbsetio(5,Nlayout,(char *)"rflaytbl",-2) != 0) {
      if (IItinsert() != 0) {
        IItcoputio((char *)"lay_sec",(short *)0,1,32,0,layout_name);
        IItcoputio((char *)"lay_sectype",(short *)0,1,30,4,&sec_type);
        IItcoputio((char *)"lay_secseq",(short *)0,1,30,4,&sequence);
        IITBceColEnd();
      } /* IItinsert */
    } /* IItbsetio */
  }
/* # line 373 "rfedtlay.qsc" */	/* host code */
		}
		changes = TRUE;
		return;
}
static bool
/*
** CheckBreakOptions -	This routine checks to see if the section passed in
** is needed to support the suppression of repitition of break values or to 
** support a page feed after a break.  If so, give an error and return FALSE;
** The user is not allowed to delete the section.
*/
CheckBreakOptions(n)
Sec_node	*n;
{
	i4 i;
	COPT *copt;
	bool	restricted = FALSE;
	if (n->sec_type != SEC_PAGHDR && n->sec_type != SEC_BRKHDR &&
	    n->sec_type != SEC_BRKFTR)
		return(TRUE);
	for (i = 1; i <= En_n_attribs; i++)
	{
		copt = rFgt_copt(i);
		if (n->sec_type == SEC_PAGHDR &&
			(copt->copt_whenprint == 'p' || copt->copt_whenprint == 't'))
		{
			restricted = TRUE;
			break;
		}
		if (n->sec_type == SEC_BRKHDR && n->sec_brkord == i &&
			(copt->copt_whenprint == 'b' || copt->copt_whenprint == 't'))
		{
			restricted = TRUE;
			break;
		}
		if (n->sec_type == SEC_BRKFTR && n->sec_brkord == i &&
			copt->copt_newpage == TRUE)
		{
			restricted = TRUE;
			break;
 		}
	}
	if (restricted)
	{
		switch (n->sec_type) {
			case SEC_PAGHDR:
			case SEC_BRKHDR:
				IIUGerr(E_RF0089_Can_t_delete_section,
					0, 1, n->sec_name);
				break;
			case SEC_BRKFTR:
				IIUGerr(E_RF008A_Can_t_delete_section,
					0, 1, n->sec_name);
				break;
		}
		return(FALSE);
	}
	return(TRUE);
}
VOID
create_section( n, initialized )
Sec_node	*n;
bool		initialized;
{
	Sec_node	*p = n->next;
	int		current_y;
	current_y = p->sec_y;
	insnLines(current_y, 2, initialized);
	insSpecLine(0, current_y, n->sec_name);
	SectionsUpdate();
}
static bool
delete_section( n, initialized )
Sec_node	*n;
bool		initialized;
{
	Sec_node	*p = n->next;
	i4		lines_to_delete;
	STATUS		status;
	lines_to_delete = p->sec_y - n->sec_y;
	status = delnLines(n->sec_y, lines_to_delete, initialized);
	if (status != SUCCEED)
		return(FALSE);
	sec_list_remove(n, &Sections);
	SectionsUpdate();
	IIVFdscDelSecCleanup();
	return(TRUE);
}
