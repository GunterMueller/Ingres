# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	JOINS.QSC - This file contains the routine mq_joins()
**		    which displays the join spec. form and
**		    join field information. Once the form and current
**		    join information are displayed the user can do
**		    any of the following tasks:
**			-get attribute info. on any table.
**			-modify the joins used in the current qdef.
**			-go to the semantics info form for this qdef.
**			-return to the table entry form for this qdef.
**
**	History:
**		written	  7/22/83 (nml)
**		modified 11/16/83 (nml) -rewritten to check states of rows
**				 returned form UNLOADTABLE and added
**				 code to free up space allocated that
**				 is no longer needed.
**		31-jul-86 (marian)	bug 9981
**			Changed if statement in mqtblinqdef() so it no longer
**			checks for the table name.
**		13-jul-87 (bab) Changed memory allocation to use [FM]Ereqm
**		25-aug-87 (scl) Changed _column1 and _column2 to _col1 and
**				_col2
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh.  Added code to deal
**			with case-sensitive backend object names.
**		10-dec-87 (bruceb)
**			Changed some buffer lengths to accept changed form
**			field sizes for dg.
**		22-dec-1987 (peter)
**			Integrate fix for bug 13021 from 5.0.
**		28-jan-1988 (peter)
**			Performance changes using FErel_ffetch.  Also, use
**			the FE_FEL_INFO structure to call FEatt_fopen.
**		19-feb-88 (kenl and marian)
**			Integrate fix for bug 12969 from 6.0.  Changed
**			mqupd_attribs() so it now changes mdtype for the
**			attribute the same way it changes formfield.
**		22-apr-88 (kenl)
**			Made major changes to fix numerous JoinDef bugs.
**		18-jul-88 (kenl)
**			Fixed bug #2926.  Redundant rows on a join 
**			specification are removed.
**		10-aug-88 (kenl)
**			Fixed bug #3002.  Since rev 5.0 a comment was not
**			terminated.
**		10-aug-88 (kenl)
**			Fixed bug #3154.  mdtype was getting set wrong when
**			a join was enter as an DM join instead of a MD join.
**		08/03/89 (dkh) - Updated with interface change to adc_lenchk().
**		4/19/90 (martym)
**			Added "IIQFRvToTabName()".
**      	10/05/90 (emerson)
**			Correct way return code from mqcolexists
**			is tested (was causing E_QF0056 to be sometimes
**			reported instead of E_QF005A, and vice versa).
**      	10/09/90 (emerson)
**			Major revisions for logical keys
**			(bug 8593) and for bug 33795.
**			Deleted function mqupd_attribs;
**			added functions IIQFmauMarkAllUnjoined,
**			IIQFmjMarkJoined, and IIQFfjaFixJoinedAttributes.
**		10/13/90 (emerson)
**			Modified mq_joins for logical key support
**			(to enable errors to be flagged properly).
**			See mq_joins for details.
**		10/13/90 (emerson)
**			Modified mq_joins to fix bug 32075 (partially).
**		10/15/90 (emerson)
**			Fixed obvious logic error in my new routine
**			IIQFfjaFixJoinedAttributes.
**		11/03/90 (emerson)
**			Implement the decision to disallow
**			joins on logical keys for this release.
**		08/12/91 (emerson)
**			Fix for bug 37365 in mq_joins.
**		19-feb-92 (leighb) DeskTop Porting Change:
**			FEchkname() has only 1 arg, bogus 2nd one deleted.
**	15-Nov-1993 (tad)
**		Bug #56449
**		Replace %x with %p for pointer values where necessary.
**
**	Copyright (c) 2004 Ingres Corporation
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include	<compat.h>
# include	<cv.h>		 
# include	<st.h>		 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ui.h>
# include "mqbf.h"
# include	<si.h>
# include "mqsymbol.h"
# include	<me.h>
# include	<er.h>
# include	<ug.h>
# include	<afe.h>
# include	"erqf.h"
/*
**  For this release (6.3/03), disallow any join involving a logical key
**  (because of problems in back-end: SIR 30708; bugs 33799 and 34254).
**  Note that logical key joins can be allowed by zapping the globaldef
**  below to FALSE.
*/
GLOBALDEF	bool	IIQFljd_LkeyJoinsDisallowed = TRUE;
/*	Forward declarations */
FUNC_EXTERN	STATUS	FErel_ffetch();
FUNC_EXTERN	STATUS	FEatt_fopen();
FUNC_EXTERN	STATUS	FEatt_fetch();
FUNC_EXTERN	STATUS	FEatt_close();
FUNC_EXTERN	VOID	mq_resetatt();
FUNC_EXTERN	VOID	IIQFmauMarkAllUnjoined();
FUNC_EXTERN	bool	IIQFmjMarkJoined();
FUNC_EXTERN	bool	IIQFfjaFixJoinedAttributes();
FUNC_EXTERN     char    *IITUtpTblPik();
FUNC_EXTERN	VOID	unique_it();
static	i4	join_seqno = 0;
/*{
** Name:	mq_joins() -- Specify Join information in a JoinDef
**
** Description:
**	This routine displays the JoinDef Join Specification screen.
**	This enables the user to update the list of joins.
**	The user may exit the screen by selecting 'Rules', 'End', or 'Cancel'.
**	If any errors are detected, the user is not allowed to exit *unless*
**	'Cancel' was selected.
**
** Input:
** 	*qdef		the query definition structure.
** 	*changed	boolean flag:
** 			at entry, indicates whether changes
** 			have occurred that will necessitate
** 			editing the join specification and
** 			rebuilding the join table even if
** 			no changes are entered on the screen.
** Output:
**	*changed
** 			at exit, indicates whether changes
** 			were entered on the screen (and not canceled).
**
** Returns: 
**	1 if 'Rules' selected; 
**	0 otherwise.
**
** History:
** 	10/13/90 (emerson)
** 		Created functional description and history.
** 		Set *changed to FALSE if 'Cancel' selected,
** 		even if *changed was TRUE at entry.
** 		This enables the caller to determine if errors
** 		have been corrected.
** 	10/13/90 (emerson)
** 		Check for error E_QF015D (bug 32075).
** 	08/12/91 (emerson)
** 		When FErel_ffetch encounters an error,
** 		display new error E_QF0175_BadTable
** 		instead of trying to treat the return code
** 		as a message ID (bug 37365).
** 	21-sep-91 (daver)
**		Added owner.table support for 6.5 FIPS.
**		Removed extraneous variable declarations.
**	13-apr-93 (daver)
**		Added delim id support in frames, un-normalizing table
**		and column information (i.e, displaying quotes).
**	30-dec-1993 (rdrane)
**		Owners can be delimited identifiers!  Bug 57866.
**	20-jan-94 (connie) Bug #57587
**		Moved the calling to unique_it from mq_resetatt
**		to mq_updjoins -- this fix a regression bug from 6.4
*/
mq_joins(qdef, changed)
MQQDEF	*qdef;
i4	*changed;
  {
	JOININFO	*join;
	i4		numjoins = qdef->mqnumjoins;
	/* enough storage for table name plus '.' plus column name plus EOS */
    char c1[2 * FE_UNRML_MAXNAME + 1 + 1];
    char c2[2 * FE_UNRML_MAXNAME + 1 + 1];
    char full_table[FE_MAXTABNAME];
    char formname[FE_MAXNAME + 1];
    char tfld1[FE_MAXNAME + 1];
    char jcode[4];
    char tbl_own[FE_MAXNAME+1];
    int state;
	i4	i;
	int	sj;		/* return code */
	i4	updjoins;
	i4	reswhere;
	i4	j1, j2;
	char	last_table[FE_MAXTABNAME];
	i4	chgd;
    i4 avail1, avail2;
	jcode[0] = ' ';
	STcopy(ERx(" "), last_table);
	STcopy(_mqbfjoin, formname);
	STcopy(_tblfmt, tfld1);
	sj = 0;
	chgd = FALSE;
/* # line 219 "joins.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 220 "joins.qsc" */	/* inittable */
    {
      if (IItbinit(_mqbfjoin,_tbljoins,(char *)"f") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 221 "joins.qsc" */	/* inittable */
    {
      if (IItbinit(_mqbfjoin,_tblfmt,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 222 "joins.qsc" */	/* display */
    {
      if (IIdispfrm(_mqbfjoin,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 226 "joins.qsc" */	/* inquire_frs */
        {
          if (IIiqset(4,0,_mqbfjoin,_tbljoins,(char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&avail1,24,_col1,0);
            IIiqfsio((short *)0,1,30,4,&avail2,24,_col2,0);
          } /* IIiqset */
        }
/* # line 230 "joins.qsc" */	/* host code */
		/* 
		** The stuff in the qdef is always normalized, and then
		** un-normalized it when we display it or send it in a query.
		** This may be expensive if we don't keep 4 booleans in the 
		** mqjoins structure (rv1, col1, rv2, col2) that determine if 
		** the identifier needs un-normalizing; the UG routine we use 
		** may take some time, even if we're not delimited. If 
		** performance problems arise, this is a good place for it; 
		** but in the meantime, lets just call the Utility routines 
		** and hope for the best.
		*/
		for (i = 0; i < numjoins; i++)
		{
			join = qdef->mqjoins[i];
			/*
			** IIUIxrt_tbl() is used to dealing with owner.table,
			** but this is analogous to table.column, which is
			** what we have here. Make sure that these objects get
			** double quoted, if necessary.
			*/
			IIUIxrt_tbl(join->rv1, join->col1, c1);
			if (STlength(c1) > avail1)
			{
			    IIUGerr(E_QF0136_COLUMN_SPEC_TOO_WIDE, 0, 1, c1);
/* # line 255 "joins.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 256 "joins.qsc" */	/* host code */
			}
			IIUIxrt_tbl(join->rv2, join->col2, c2);
			if (STlength(c2) > avail2)
			{
			    IIUGerr(E_QF0136_COLUMN_SPEC_TOO_WIDE, 0, 1, c2);
/* # line 263 "joins.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 264 "joins.qsc" */	/* host code */
			}
			STcopy(join->jcode, &jcode[1]);
/* # line 266 "joins.qsc" */	/* loadtable */
        {
          if (IItbact(_mqbfjoin,_tbljoins,1) != 0) {
            IItcoputio((char *)"jtype",(short *)0,1,32,0,jcode);
            IItcoputio(_col1,(short *)0,1,32,0,c1);
            IItcoputio(_col2,(short *)0,1,32,0,c2);
            IITBceColEnd();
          } /* IItbact */
        }
/* # line 269 "joins.qsc" */	/* host code */
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 274 "joins.qsc" */	/* unloadtable */
            {
              if (IItbact(_mqbfjoin,_tbljoins,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IITBceColEnd();
                {
/* # line 276 "joins.qsc" */	/* host code */
			if (state != UNCHANGED && state != UNDEF)
				chgd = TRUE;
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 280 "joins.qsc" */	/* host code */
		if (chgd || *changed)
		{
/* # line 282 "joins.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0050_Updating_joins______));
            }
/* # line 283 "joins.qsc" */	/* host code */
			updjoins = mq_updjoins(qdef);
			switch(updjoins)
			{
			case 0:
				/* all went well */
				break;
			/* bug 3002
			** Comment below was not terminated in rev 5.0
			*/
			case 1:
				/* resume to table1 column of _tbljoins */
			case 2:
				/* resume to _col1 column of _tbljoins */
/* # line 297 "joins.qsc" */	/* resume */
            {
              IIrescol(_tbljoins,_col1);
              if (1) goto IIfdB1;
            }
/* # line 298 "joins.qsc" */	/* host code */
				break;
			case 3:
				/* resume to table2 column of _tbljoins */
			case 4:
				/* resume to _col2 column of _tbljoins */
/* # line 304 "joins.qsc" */	/* resume */
            {
              IIrescol(_tbljoins,_col2);
              if (1) goto IIfdB1;
            }
/* # line 305 "joins.qsc" */	/* host code */
				break;
			}
		}
		sj = 1;
/* # line 310 "joins.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 311 "joins.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 315 "joins.qsc" */	/* host code */
		char	table_part[(FE_UNRML_MAXNAME + 1)];
		char	owner_part[(FE_UNRML_MAXNAME + 1)];
		reswhere = 0;
/* # line 319 "joins.qsc" */	/* getform */
            {
              if (IIfsetio(_mqbfjoin) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXTABNAME-1,full_table,(char *)"\
table1");
              } /* IIfsetio */
            }
/* # line 321 "joins.qsc" */	/* host code */
		STtrmwhite(full_table);
		if ((full_table[0] != '\0') && 
			((STcompare(full_table, last_table)) != 0))
		{
			FE_RSLV_NAME    resolved_name;
			FE_REL_INFO	relinfo;
			STATUS		rstat;
			char		result[FE_MAXNAME + 1];
/* # line 331 "joins.qsc" */	/* clear */
            {
              IIfldclear(_tblfmt);
            }
/* # line 333 "joins.qsc" */	/* host code */
			/* Initialize the FE_RSLV_NAME structure */
			resolved_name.name = full_table;
			resolved_name.name_dest = table_part;
			resolved_name.owner_dest = owner_part;
			resolved_name.is_nrml = FALSE;
			/*
			** Removed call to IIUGlbo_lowerBEobject() and
			** FEchk_name(). Replaced with FE_decompose(), to
			** split up the possible owner.table construct, 
			** IIUGdlm_ChkdlmBEobject() to check for valid
			** characters and perform the casing/delim id
			** processing.  Note that we are not interested in
			** existence/accesability at this time.
			*/
			FE_decompose(&resolved_name);
			if (IIUGdlm_ChkdlmBEobject(table_part, result, FALSE)
				== UI_BOGUS_ID)
			{
				IIUGerr(E_QF0051___s__is_an_invalid_ta,
					0,1,full_table);
/* # line 355 "joins.qsc" */	/* resume */
            {
              IIresfld((char *)"table1");
              if (1) goto IIfdB1;
            }
/* # line 356 "joins.qsc" */	/* host code */
			}
			STcopy(result, table_part);
			if (owner_part[0] != EOS)
			{
			    if  (IIUGdlm_ChkdlmBEobject(owner_part,
							result,FALSE) ==
						UI_BOGUS_ID)
			    {
				IIUGerr(E_QF0051___s__is_an_invalid_ta,
					0,1,full_table);
/* # line 367 "joins.qsc" */	/* resume */
            {
              IIresfld((char *)"table1");
              if (1) goto IIfdB1;
            }
/* # line 368 "joins.qsc" */	/* host code */
			    }
			    STcopy(result, owner_part);
			}
			/*
			** Check if a current abbreviation was given;
			** if so, pick up the corresponding table name,
			** extracted by FE_decompose().
			** While were at it, pick up the owner name too.
			*/
			for (i = 0; i < qdef->mqnumtabs; i++)
			{
			    if (STcompare(table_part, 
					  qdef->mqtabs[i]->mqrangevar) == 0)
			    {
				STcopy(qdef->mqtabs[i]->mqrelid, table_part);
				STcopy(qdef->mqtabs[i]->mqrelown, owner_part);
				break;
			    }
			}
			/* 
			** Changed parameterization to FErel_ffetch, replacing
			** 2nd param with owner_part. FErel_ffetch will
			** populate the relinfo structure with necessary info
			*/
			rstat = FErel_ffetch(table_part, owner_part, &relinfo);
			if (rstat != OK)
			{
				/* changed = to == below (esd 10/12/90) */
				if (rstat == E_UI0002_EOS)
				{	/* Table does not exist */
					IIUGerr(E_QF0117_11023, 0, 1, 
						table_part);
				}
				else
				{	/* Any other error */
					IIUGerr(E_QF0175_BadTable, 0, 1, 
						table_part);
				}
/* # line 408 "joins.qsc" */	/* clear */
            {
              IIfldclear((char *)"table1");
            }
/* # line 409 "joins.qsc" */	/* clear */
            {
              IIfldclear(_tblfmt);
            }
/* # line 410 "joins.qsc" */	/* host code */
				STcopy(ERx(" "), last_table);
/* # line 411 "joins.qsc" */	/* resume */
            {
              IIresfld((char *)"table1");
              if (1) goto IIfdB1;
            }
/* # line 412 "joins.qsc" */	/* host code */
			}
			/* fetch table columns */
			if (mqgetcols(&relinfo, formname, tfld1) == 0)
			{
/* # line 417 "joins.qsc" */	/* resume */
            {
              IIresfld((char *)"table1");
              if (1) goto IIfdB1;
            }
/* # line 418 "joins.qsc" */	/* host code */
			}
			reswhere = 1;
		}
		STcopy(full_table, last_table);
		if (reswhere == 1)
/* # line 424 "joins.qsc" */	/* resume */
            {
              IIresfld(_tblfmt);
              if (1) goto IIfdB1;
            }
/* # line 425 "joins.qsc" */	/* host code */
		else
/* # line 426 "joins.qsc" */	/* resume */
            {
              IIresfld((char *)"table1");
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 430 "joins.qsc" */	/* host code */
		chgd = FALSE;
/* # line 431 "joins.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 4) {
          {
            i4 isonfld;
            char full_tabname[FE_MAXTABNAME];
/* # line 441 "joins.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_mqbfjoin,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&isonfld,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 443 "joins.qsc" */	/* host code */
		/* isonfld set to 0 if on the simple field, 1 on the tblflds */
		if (isonfld)
		{
			IIUFfieldHelp();
		}
		else
		{
			/*
			** We're on the only simple field on the form,
			** the table1 field. Lets do the listpick thang!
			*/
			STcopy(IITUtpTblPik(ERx("%"),ERx("QBF")), full_tabname);
/* # line 455 "joins.qsc" */	/* putform */
            {
              if (IIfsetio(_mqbfjoin) != 0) {
                IIputfldio((char *)"table1",(short *)0,1,32,0,full_tabname);
              } /* IIfsetio */
            }
/* # line 456 "joins.qsc" */	/* host code */
		}
/* # line 457 "joins.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 462 "joins.qsc" */	/* host code */
		FEhelp(ERx("qjoin.hlp"), ERget(S_QF0052_QBF_Join_Specificatio));
          }
        } else if (IIretval() == 6) {
          {
/* # line 467 "joins.qsc" */	/* unloadtable */
            {
              if (IItbact(_mqbfjoin,_tbljoins,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IITBceColEnd();
                {
/* # line 469 "joins.qsc" */	/* host code */
			if (state != UNCHANGED && state != UNDEF)
				chgd = TRUE;
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 473 "joins.qsc" */	/* host code */
		if (chgd || *changed)
		{
/* # line 475 "joins.qsc" */	/* message */
            {
              IImessage(ERget(S_QF0053_Updating_joins______));
            }
/* # line 476 "joins.qsc" */	/* host code */
			updjoins = mq_updjoins(qdef);
			switch (updjoins)
			{
			case 0:
				/* all went well */
				break;
			case 1:
				/* resume to table1 column of _tbljoins */
			case 2:
				/* resume to _col1 column of _tbljoins */
/* # line 487 "joins.qsc" */	/* resume */
            {
              IIrescol(_tbljoins,_col1);
              if (1) goto IIfdB1;
            }
/* # line 488 "joins.qsc" */	/* host code */
				break;
			case 3:
				/* resume to table2 column of _tbljoins */
			case 4:
				/* resume to _col2 column of _tbljoins */
/* # line 495 "joins.qsc" */	/* resume */
            {
              IIrescol(_tbljoins,_col2);
              if (1) goto IIfdB1;
            }
/* # line 496 "joins.qsc" */	/* host code */
				break;
			}
		}
/* # line 500 "joins.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
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
      if (IInmuact(ERget(F_QF000A_Rules),(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_GetTableDef),(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Forget),(char *)0,2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,2,4) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),(char *)0,2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 503 "joins.qsc" */	/* host code */
	*changed = chgd;
	return(sj);
  }
mq_updjoins(qdef)
MQQDEF	*qdef;
  {
	ATTRIBINFO	**atts = qdef->mqatts;
	i4		numatts = qdef->mqnumatts;
	/* enough storage for table name plus '.' plus column name plus EOS */
    typedef char db_maxname_array[2 * FE_UNRML_MAXNAME + 1 + 1];
    db_maxname_array tbl_dot_col[2];
    db_maxname_array r[2];
    db_maxname_array c[2];
    db_maxname_array t[2];
    char jcode[4];
    i4 state;
    i4 rec;
    i4 frmt[2];
	i4	lkeyinfo[2];
	i4	num;
	register i4	i;
	i4	l;
	i4	numj;
	int	rcode;
	char	oldff[FE_MAXNAME + 1];
	char	bufr[10];
	i4	jpos;
	JOININFO	*newjoins[MQ_MAXJOINS];
	JOININFO	*currj;
	FE_REL_INFO	relinfo;
	ATTRIBINFO	*satt;
	ATTRIBINFO	*col_att[2];
	DB_DATA_VALUE	*dbv;
	DB_DATA_VALUE	sdbv;
# ifdef DEBUG
	if (mq_debug)
	    mqqdef_debug(qdef);
# endif /* DEBUG */
	rcode = 0;
	bufr[0] = '\0';
	numj = 0;
	/*
	**  first unjoin old joins
	*/
	join_seqno = 0;
	for (i = 0; i < qdef->mqnumjoins; i++)
	{
		i4	j1, j2, k;
		currj = qdef->mqjoins[i];
		j1 = mqfindatt(qdef, currj->rv1, currj->col1);
		j2 = mqfindatt(qdef, currj->rv2, currj->col2);
		if (atts[j1]->jpart != mqNOT_JOIN)
		{
		    mq_resetatt(qdef, j1);
		}
		if (atts[j2]->jpart != mqNOT_JOIN)
		{
		    mq_resetatt(qdef, j2);
		}
		STcopy(atts[j1]->formfield, oldff);
		for (k = 0; k < numatts; k++)
		{
			if (k == j1)
				continue;
			if ((STcompare(atts[k]->formfield, oldff)) == 0)
			{
			    unique_it(qdef, atts[k]->formfield, k, 0);
			}
		}
	}
/* # line 583 "joins.qsc" */	/* unloadtable */
    {
      if (IItbact(_mqbfjoin,_tbljoins,0) == 0) goto IItbE3;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,3,jcode,(char *)"jtype");
        IItcogetio((short *)0,1,32,2 * FE_UNRML_MAXNAME + 1 + 1-1,
        tbl_dot_col[0],_col1);
        IItcogetio((short *)0,1,32,2 * FE_UNRML_MAXNAME + 1 + 1-1,
        tbl_dot_col[1],_col2);
        IItcogetio((short *)0,1,30,4,&state,(char *)"_state");
        IItcogetio((short *)0,1,30,4,&rec,(char *)"_record");
        IITBceColEnd();
        {
/* # line 587 "joins.qsc" */	/* host code */
		FE_RSLV_NAME	rslv_tbl_col;
		/* do this for both tbl_dot_col[0] and tbl_dot_col[1] */
		for (i = 0; i < 2; i++)
		{
		    /* 
		    ** Changed this "clever" code which scanned through the
		    ** table.column name, stopping at either . or the
		    ** end of string, copying the string over to c array
		    ** for the column, and null terminating at the . for
		    ** the r array. That's gotta change now, since the table
		    ** or the column can contain a . as a delimited
		    ** identifier. 
		    ** 
		    ** Call FE_decompose to break up the individual table and
		    ** column parts. This is analogous to owner.table, so
		    ** for this case, owner_dest is the table, and
		    ** name_dest is the column. Place the identifiers in
		    ** their respective arrays (tables/relations go in r,
		    ** columns in c), and strip potential delim id quotes 
		    ** (i.e, "normalize") for proper storage
		    **
		    ** But only do all this if there's something there.
		    ** Otherwise, things get screwed up if you blank out a
		    ** join field that already exists. Set r[i][0] and c[i][0]
		    ** to EOS for each join field so the existing code
		    ** continues to work.
		    */
		    if (tbl_dot_col[i][0] == EOS)
		    {
			r[i][0] = EOS;
			c[i][0] = EOS;
		    }
		    else
		    {
		    	rslv_tbl_col.name = tbl_dot_col[i];
		    	rslv_tbl_col.owner_dest = r[i];
		    	rslv_tbl_col.name_dest = c[i];
		    	rslv_tbl_col.is_nrml = FALSE;
		    	FE_decompose(&rslv_tbl_col);
		    	IIUGdlm_ChkdlmBEobject(r[i], r[i],FALSE);
		    	IIUGdlm_ChkdlmBEobject(c[i], c[i],FALSE);
		    }
		}
		STtrmwhite(jcode);
		switch(state)
		{
		    register RELINFO	*rel;
		    i4			type[2];
		    i4			ext_length[2];
		    case UNDEF:
			continue;
		    case UNCHANGED:
		    case CHANGED:
		    case NEW:
			if (r[0][0] == '\0' &&
			    r[1][0] == '\0')
			{
/* # line 649 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 650 "joins.qsc" */	/* clearrow */
          {
            if (IItbsetio(6,_mqbfjoin,_tbljoins,1) != 0) {
              IItclrrow();
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 651 "joins.qsc" */	/* host code */
				break;
			}
			/* Bug fix #2926
			** Redundant rows are removed from join specification.
			*/
			for (i = 0; i < numj; i++)
			{
			    currj = newjoins[i];
			    if ((STcompare(r[0], currj->rv1) == 0 &&
				STcompare(c[0], currj->col1) == 0 &&
			    	STcompare(r[1], currj->rv2) == 0 &&
				STcompare(c[1], currj->col2) == 0) ||
			        (STcompare(r[0], currj->rv2) == 0 &&
				STcompare(c[0], currj->col2) == 0 &&
			    	STcompare(r[1], currj->rv1) == 0 &&
				STcompare(c[1], currj->col1) == 0))
			    {
/* # line 669 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 670 "joins.qsc" */	/* clearrow */
          {
            if (IItbsetio(6,_mqbfjoin,_tbljoins,1) != 0) {
              IItclrrow();
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 671 "joins.qsc" */	/* host code */
				rcode = 1;
				break;
			    }
			}
			if (rcode != 0)
			{
			    rcode = 0;
			    break;
			}
			for (i = 0; i < 2; i++)
			{
			    if (c[i][0] == '\0')
			    {
				IIUGerr(E_QF0054_Improper_format__use_,0,0);
				rcode = 2*i+2;
			    }
			    /* check before putting in new join struct */
			    else if ((num = mqtblinqdef(r[i], qdef)) == -1)
			    {
				IIUGerr(E_QF0055_Table_range_variable_,0,
					1, r[i]);
				rcode = i*2 + 1;
			    }
			    /* Check for SQL keywords in column name.  */
			    else if (IIUGIsSQLKeyWord(c[i]))
			    {
				IIUGerr(E_QF015D_ColIsSQLKeyWord, 0,
				    2, qdef->mqtabs[num]->mqrangevar, c[i]);
				rcode = i*2 + 2;
			    }
			    if (rcode)
			    {
/* # line 705 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 706 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 707 "joins.qsc" */	/* host code */
			    }
			    rel = qdef->mqtabs[num];
			    STcopy(rel->mqrangevar, t[i]);
			    type[i] = rel->mqtype;
			    if ((jpos = mqfindatt(qdef, t[i], c[i])) == -1)
			    {
				satt = (ATTRIBINFO *) NULL;
			    }
			    else
			    {
				satt = qdef->mqatts[jpos];
			    }
			    /*
			    ** Fill in an FE_REL_INFO structure to make
			    ** the attribute lookup faster
			    */
			    _VOID_ STcopy(rel->mqrelid, relinfo.name);
			    _VOID_ STcopy(rel->mqrelown, relinfo.owner);
			    if (satt == (ATTRIBINFO *) NULL)
			    {
			        if (mqcolexists(&relinfo, c[i], satt) != OK)
			        {
				    IIUGerr(E_QF0056_Column___s__not_in_ta,0,
					    2, c[i], r[i]);
/* # line 734 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 735 "joins.qsc" */	/* host code */
				    rcode = i*2 + 2;
/* # line 736 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 737 "joins.qsc" */	/* host code */
			        }
			        else
				{
			            IIUGerr(E_QF005A_Problem_updating_attr,0,0);
/* # line 741 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 742 "joins.qsc" */	/* host code */
				    rcode = i*2 + 2;
/* # line 743 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 744 "joins.qsc" */	/* host code */
				}
			    }
			    lkeyinfo[i] = satt->lkeyinfo;
			    dbv = &satt->dbv;
			    frmt[i] = dbv->db_datatype;
			    col_att[i] = satt;
			    if (adc_lenchk(FEadfcb(), FALSE, dbv,
				    &sdbv) != OK)
			    {
				FEafeerr(FEadfcb());
				rcode = 1;
/* # line 756 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 757 "joins.qsc" */	/* host code */
			    }
			    ext_length[i] = sdbv.db_length;
			}
			if (STcompare(r[0], r[1]) == 0)
			{
			    IIUGerr(E_QF0057_Illegal_join_pair___s,0,1,r[0]);
/* # line 764 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 765 "joins.qsc" */	/* host code */
			    rcode = 1;
			    break;
			}
			/* 
			**  check for compatible attribute formats
			*/
			for (i = 0; i < 2; i++)
			{
			    if ( (lkeyinfo[  i ] & mqOBJECT_KEY) &&
				!(lkeyinfo[ !i ] & mqOBJECT_KEY) )
			    {
			        IIUGerr(E_QF0166_bad_object_key_join, 0,
				    4, c[i], r[i], c[!i], r[!i]);
/* # line 779 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 780 "joins.qsc" */	/* host code */
			        rcode = 1;
/* # line 781 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 782 "joins.qsc" */	/* host code */
			    }
			    if ( (lkeyinfo[  i ] & mqTABLE_KEY) &&
				!(lkeyinfo[ !i ] & mqTABLE_KEY) )
			    {
			        IIUGerr(E_QF0167_bad_table_key_join, 0,
				    4, c[i], r[i], c[!i], r[!i]);
/* # line 788 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 789 "joins.qsc" */	/* host code */
			        rcode = 1;
/* # line 790 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 791 "joins.qsc" */	/* host code */
			    }
			}
		    if (IIQFljd_LkeyJoinsDisallowed)
		    {
			/*
			**  If joins on logical keys are being disallowed,
			**  we'll complain about any such join (error E_QF0172).
			*/
			if ( (lkeyinfo[0] != 0) &&
			     (lkeyinfo[1] != 0) )
			{
			    IIUGerr(E_QF0172_lkey_lkey_join, 0,
				4, c[0], r[0], c[1], r[1]);
/* # line 804 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 805 "joins.qsc" */	/* host code */
			    rcode = 1;
/* # line 806 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 807 "joins.qsc" */	/* host code */
			}
		    }
		    else
		    {
			if ( (lkeyinfo[0] & mqSYSTEM_MAINT) &&
			     (lkeyinfo[1] & mqSYSTEM_MAINT) )
			{
			    IIUGerr(E_QF0168_system_system_join, 0,
				4, c[0], r[0], c[1], r[1]);
/* # line 816 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 817 "joins.qsc" */	/* host code */
			    rcode = 1;
/* # line 818 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 819 "joins.qsc" */	/* host code */
			}
			for (i = 0; i < 2; i++)
			{
			    if ( (lkeyinfo[i] & mqSYSTEM_MAINT) &&
				 (type[i] == 1) && (type[!i] == 0) )
			    {
			        IIUGerr(E_QF0169_user_system_MD_join, 0,
				    2, c[i], r[i]);
/* # line 827 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 828 "joins.qsc" */	/* host code */
			        rcode = 1;
/* # line 829 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 830 "joins.qsc" */	/* host code */
			    }
			}
		    }
			{
			    ADI_FI_ID	dummy = 0;
			    if ((adi_ficoerce(FEadfcb(), frmt[0],
				frmt[1], &dummy) != OK) ||
			        (adi_ficoerce(FEadfcb(), frmt[1],
				frmt[0], &dummy) != OK))
			    {
			        IIUGerr(E_QF0058_Illegal_join_pair___t,0,0);
/* # line 841 "joins.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_mqbfjoin,_tbljoins,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 842 "joins.qsc" */	/* host code */
			        rcode = 1;
/* # line 843 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 844 "joins.qsc" */	/* host code */
			    }
			}
			if ((abs(frmt[0])) != (abs(frmt[1])))
			{
			    IIUGerr(E_QF0144_Mixed_Datatype_Join, 0,
				4, r[0], c[0], r[1], c[1]);
			}
			if ((AFE_NULLABLE_MACRO(frmt[0])) !=
				(AFE_NULLABLE_MACRO(frmt[1])))
			{
			    IIUGerr(E_QF0145_Mixed_Nullable_Join, 0,
				4, r[0], c[0], r[1], c[1]);
			}
			if (ext_length[0] != ext_length[1])
			{
			    IIUGerr(E_QF0146_Mixed_Length_Join, 0,
				4, r[0], c[0], r[1], c[1]);
			}
			jcode[0] = ' ';
			jcode[1] = type[0]==0? 'M': 'D';
			jcode[2] = type[1]==0? 'M': 'D';
			jcode[3] = '\0';
			/* good join pair - copy in new info */
			if (mqaddnewj(newjoins, numj, jcode, t[0], c[0], t[1], c[1]) != TRUE)
			{
			    IIUGerr(E_QF0059_Problem_adding_change,0,0);
			    rcode = 1;
/* # line 876 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
/* # line 877 "joins.qsc" */	/* host code */
			}
			numj++;
			/*
			**  Mark the columns as joined
			**  (merge the circular lists of columns
			**  each is already joined to).
			*/
			(VOID) IIQFmjMarkJoined(col_att[0], col_att[1]);
			break;
		   default:
			IIUGerr(E_QF005B_DEFAULT__bad_row_stat,0,1,(PTR)&state);
			numj--;
			rcode = 1;
			break;
		}
		if (rcode != 0)
/* # line 896 "joins.qsc" */	/* endloop */
          {
            if (1) goto IItbE3;
          }
        }
      } /* IItunload */
IItbE3:
      IItunend();
    }
/* # line 899 "joins.qsc" */	/* host code */
	/*
	**  If no error detected already, fix attributes of joined columns.
	**  If an error is detected, set rcode to indicate
	**  an error occurred and that the cursor should be 
	**  positioned in col1 of tbljoins.
	*/
	if (rcode == 0 && !IIQFfjaFixJoinedAttributes(qdef, TRUE))
	{
		rcode = 1;
	}
	/*
	**  If an error occurred, free up the space allocated for new joins
	**  and return.
	*/
	if (rcode != 0)
	{
		for (i = 0; i < numj; i++)
		{
			MEfree(newjoins[i]);
		}
		return(rcode);
	}
#ifdef DEBUG
	if (mq_debug)
	{
	    SIfprintf(mqoutf, ERx("MQ_UPDJOINS: new formfields follow:\n"));
	    for (i = 0; i < qdef->mqnumatts; i++)
	    {
		att = qdef->mqatts[i];
		SIfprintf(mqoutf, ERx("ATT NUM=%d, TBL=%s, COL=%s, FLDNAME=%s\n"),
		    i, att->rvar, att->col, att->formfield);
	    }
	}
#endif
	/* all went well, move new join info to join struct */
	mqrepl_joins(qdef, newjoins, numj);
/* # line 939 "joins.qsc" */	/* clear */
    {
      IIfldclear(_tbljoins);
    }
/* # line 940 "joins.qsc" */	/* host code */
	for (i = 0; i < qdef->mqnumjoins; i++)
	{
		currj = qdef->mqjoins[i];
		STprintf(c[0], ERx("%s.%s"), currj->rv1, currj->col1);
		STprintf(c[1], ERx("%s.%s"), currj->rv2, currj->col2);
		STcopy(currj->jcode, &jcode[1]);
/* # line 946 "joins.qsc" */	/* loadtable */
    {
      if (IItbact(_mqbfjoin,_tbljoins,1) != 0) {
        IItcoputio((char *)"jtype",(short *)0,1,32,0,jcode);
        IItcoputio(_col1,(short *)0,1,32,0,c[0]);
        IItcoputio(_col2,(short *)0,1,32,0,c[1]);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 949 "joins.qsc" */	/* host code */
	}
	return(rcode);
  }
mqaddnewj(newjoins, numj, jcode, r1, c1, r2, c2)
JOININFO	*newjoins[];
i4	numj;
char	*jcode;
char	*r1;
char	*c1;
char	*r2;
char	*c2;
  {
	register JOININFO	*join;
	/* allocate space for join info */
	if (numj >= MQ_MAXJOINS)
	{
		char	maxjoinstr[20];
		CVna((i4)(MQ_MAXJOINS), maxjoinstr);
		FDerror(JOIN_OVERFLOW, 1, maxjoinstr);
		return(FALSE);
	}
	if ((newjoins[numj] = (JOININFO *)MEreqmem((u_i4)0,
	    (u_i4)sizeof(struct joininfo), TRUE, (STATUS *)NULL)) == NULL)
	{
		FDerror(ERRALLOC, 0, ERx(""));
		return(FALSE);
	}
	join = newjoins[numj];
	STcopy(r1, join->rv1);
	STcopy(c1, join->col1);
	STcopy(r2, join->rv2);
	STcopy(c2, join->col2);
	STcopy(&jcode[1], join->jcode);
	return(TRUE);
  }
/*
** mqtblinqdef	-   check to see if table 'r1' is in the current
**		 qdef.
**
**	Returns:
**		0 if not in current qdef
**		n if in current qdef, where n is index to rels structure
**			where 'r1' was found
**
*/
i4
mqtblinqdef(r1, qdef)
char	*r1;
MQQDEF	*qdef;
  {
	i4	tblokay;
	i4	i;
	tblokay = -1;
	for (i = 0; i < qdef->mqnumtabs; i++)
	{
		/* bug 9981
		**	Change if statement so it only checks range variable.
		**	It doesn't need to check the table name.
		**
		*/
		if (STcompare(r1, qdef->mqtabs[i]->mqrangevar) == 0)
		{
			tblokay = i;
			break;
		}
	}
	return(tblokay);
  }
/*
** mqcolexists	- Check to see if the column specified to use as
**		  a join field really exists in the given relation
**		  (emerson 10/12/90) If it *does* exist, this routine appears
**		  to have the side effect of filling in the attribute's dbv
**		  from the attribute's FE_ATT_INFO.
**
**
**	Returns:
**		OK if the column exists; something else otherwise
**
** History:
**	30-sep-1986 (sandyd)
**		Saturn changes.	 Added FEconnect() before retrieve of
**		"attribute" relation.
**	29-jan-1988 (peter)
**		For performance, changed the table name parameter to
**		a FE_REL_INFO structure.
**	12-oct-1990 (emerson)
**		Added note to functional description and corrected
**		description of return codes.
*/
STATUS
mqcolexists(relinfo, c, satt)
FE_REL_INFO	*relinfo;
char		*c;
ATTRIBINFO	*satt;
{
	FE_ATT_INFO	attinfo;
	STATUS		rstat;
	if ((rstat = FEatt_ffetch(relinfo, c, &attinfo)) == OK)
	{
	    if (satt != (ATTRIBINFO *)NULL)
	    {
	        FEatt_dbdata(&attinfo, &satt->dbv);
	    }
	}
	return(rstat);
}
/*
** mqrepl_joins	 -  this routine replaces the original join info
**		    in the qdef struct (qdef->mqjoins) with the new
**		    join info for the modified joins (newjoins)
*/
mqrepl_joins(qdef, newjoins, newnumj)
MQQDEF		*qdef;
JOININFO	*newjoins[];
i4		newnumj;
{
	i4	i;
	/* free up old space */
	for (i = 0; i < qdef->mqnumjoins; i++)
	{
		MEfree(qdef->mqjoins[i]);
		qdef->mqjoins[i] = NULL;
	}
	/* now set join pointers to point to new join info */
	for (i = 0; i < newnumj; i++)
	{
		qdef->mqjoins[i] = newjoins[i];
	}
	qdef->mqnumjoins = newnumj;
}
/*
**  This routine appears to check for error E_QF012B (esd 10/09/90).
*/
mqjmdchk(qdef)
MQQDEF	*qdef;
{
	register JOININFO	*currj;
	char		*mrngvar;
	char		*drngvar;
	i4		j;
	bool		joinok;
	bool		firstmd;
	firstmd = TRUE;
	joinok = TRUE;
	for (j=0; j<qdef->mqnumjoins; j++)
	{
		currj = qdef->mqjoins[j];
		if ((STcompare(currj->jcode, ERx("MD")) == 0) ||
			(STcompare(currj->jcode, ERx("DM")) == 0))
		{
			if (firstmd)
			{
				if (currj->jcode[0] == 'M')
				{
					mrngvar = currj->rv1;
					drngvar = currj->rv2;
				}
				else
				{
					mrngvar = currj->rv2;
					drngvar = currj->rv1;
				}
				firstmd = FALSE;
			}
			else
			{
				if (currj->jcode[0] == 'M')
				{
					if ((STcompare(mrngvar, currj->rv1) == 0)
						&& (STcompare(drngvar, currj->rv2) == 0))
					{
						joinok = TRUE;
					}
					else
					{
						joinok = FALSE;
					}
				}
				else
				{
					if ((STcompare(mrngvar, currj->rv2) == 0)
						&& (STcompare(drngvar, currj->rv1) == 0))
					{
						joinok = TRUE;
					}
					else
					{
						joinok = FALSE;
					}
				}
				if (!joinok)
				{
					FDerror(ILLEGALMDJ, 0, ERx(""));
					return(FALSE);
				}
			}
		}
	}
	return(TRUE);
}
mqqdef_debug(qdef)
MQQDEF	*qdef;
{
	register i4	i;
	SIprintf(ERx("MQQDEF @ %p\n"), qdef);
	SIprintf(ERx("	mqname	    \"%s\"\n"), qdef->mqcr.name);
	SIprintf(ERx("	mqtabs	    "));
	for (i = 0; i < 4; i++)
	{
	    SIprintf(ERx("%x "), qdef->mqtabs[i]);
	}
	SIprintf(ERx("\n"));
	SIprintf(ERx("	mqatts	    "));
	for (i = 0; i < 4; i++)
	{
	    SIprintf(ERx("%x "), qdef->mqatts[i]);
	}
	SIprintf(ERx("\n"));
	SIprintf(ERx("	mqjoins	    "));
	for (i = 0; i < 4; i++)
	{
	    SIprintf(ERx("%x "), qdef->mqjoins[i]);
	}
	SIprintf(ERx("\n"));
	SIprintf(ERx("	mqmpform    \"%s\"\n"), qdef->mqmpform);
	SIprintf(ERx("	mqdpform    \"%s\"\n"), qdef->mqdpform);
	SIprintf(ERx("	mqmgform    \"%s\"\n"), qdef->mqmgform);
	SIprintf(ERx("	mqmgargv    %p\n"),qdef->mqmgargv);
	SIprintf(ERx("	mqdgform    \"%s\"\n"), qdef->mqdgform);
	SIprintf(ERx("	mqdgargv    %p\n"),qdef->mqdgargv);
	SIprintf(ERx("	mqmrecl	    %d\n"),qdef->mqmrecl);
	SIprintf(ERx("	mqdrecl	    %d\n"),qdef->mqdrecl);
	SIprintf(ERx("	mqqdesc	    %p\n"),qdef->mqqdesc);
	SIprintf(ERx("	mqnumtabs   %d\n"),qdef->mqnumtabs);
	SIprintf(ERx("	mqnumatts   %d\n"),qdef->mqnumatts);
	SIprintf(ERx("	mqnumjoins  %d\n"),qdef->mqnumjoins);
	SIprintf(ERx("	mqdeadtabs  "));
	for (i = 0; i < MQ_MAXRELS; i++)
	{
	    SIprintf(ERx("%d"), qdef->mqdeadtabs[i]);
	}
	SIprintf(ERx("\n"));
	SIprintf(ERx("	mqdata	    %x\n"),qdef->mqdata);
	SIprintf(ERx("	mqid	    %d\n"),qdef->mqcr.id);
}
/*
**  Note (esd 10/09/90)
**
**  This function appears to make an attribute "unjoined".
**  It's called only from within this source file and getinfo.qsc.
**  In both cases, the calls are within a loop that loops through
**  all columns that were part of any join.
**  I have added a statement to set the attribute's join_att to itself.
*/
VOID
mq_resetatt(qdef, attno)
MQQDEF		*qdef;
i4		attno;
{
    i4		num;
    FE_REL_INFO	relinfo;
    RELINFO	*rel;
    ATTRIBINFO	*satt;
    satt = qdef->mqatts[attno];
    num = mqtblinqdef(satt->rvar, qdef);
    rel = qdef->mqtabs[num];
    satt->ttype = rel->mqtype;
    satt->mdtype = rel->mqtype;
    /*
    ** Call IIUGfnFeName to eliminate problems with cols like "a b"
    */
    IIUGfnFeName(satt->col, satt->formfield);
    CVlower(satt->formfield);
    if (satt->lkeyinfo == 0)
    {
	satt->opupdate = 1;
    }
    if (qdef_type)
    {
	satt->intbl = tblfield? 1: 0;
    }
    else
    {
	satt->intbl = tblfield && rel->mqtype? 1: 0;
    }
    _VOID_ STcopy(rel->mqrelid, relinfo.name);
    _VOID_ STcopy(rel->mqrelown, relinfo.owner);
    if (satt->jpart == mqHIDDEN_JOIN)
    {
        mqcolexists(&relinfo, satt->col, satt);
    }	
    satt->jpart = mqNOT_JOIN;
    satt->join_seqno = -1;	/* unnecessary - for dump analysis */
    satt->join_att = satt;
}
/*
** Name:    IIQFRvToTabName()
**
** Description:
**      Given a table range variable return the table's full-name.
**      If no such table found, return NULL. 
**
** Parameters:
**	     JoinDef -- The JoinDef definition.
**           rv      -- The range variable.
**
** Returns:
**           Table name if found, NULL otherwise.
**
** Called By:
**
** Trace Flags:
**           None.
**
** Side Effects:
**           None.
**
*/
char *IIQFRvToTabName(JoinDef, rv)
MQQDEF	*JoinDef;
char 	*rv;  
{
    i4  i = 0;
    if (rv == NULL || *rv == EOS)
        return(NULL);
    for (i = 0; i < JoinDef->mqnumtabs; i++)
	if (STcompare(rv, JoinDef->mqtabs[i]->mqrangevar) == 0)
	    return(JoinDef->mqtabs[i]->mqrelid);
    return(NULL);
}
/*
**  IIQFmauMarkAllUnjoined
**
**		    Mark all attributes as "unjoined":
**
**		    (1) Set jpart to mqNOT_JOIN (if requested). 
**		    (2) Set join_att to itself.
**
**		    Also clear join_seqno (so that new joins 
**		    will be assigned sequence numbers starting from 0;
**		    this should prevent sequence numbers from overflowing).
**
**		    Clear a couple global vars (see IIQFfjaFixJoinedAttributes).
*/
VOID
IIQFmauMarkAllUnjoined(qdef, clear_jpart)
MQQDEF	*qdef;
bool	clear_jpart;
{
	ATTRIBINFO	*att;
	i4	i;
	join_seqno = 0;
	IIQFsmmSystemUserLkeyMM = FALSE;
	IIQFsmdSystemUserLkeyMD = FALSE;
	IIQFsddSystemUserLkeyDD = FALSE;
	for (i = 0; i < qdef->mqnumatts; i++)
	{
		att = qdef->mqatts[i];
		att->join_seqno = -1;	/* unnecessary - for dump analysis */
		att->join_att = att;
		if (clear_jpart)
		{
			att->jpart = mqNOT_JOIN;
		}
	}
}
/*
**  IIQFmjMarkJoined
**
**		    Mark a pair of attributes as joined
**		    (merge the circular lists of attributes
**		    each is already joined to).
**
**		    For the first and second attribute in turn,
**		    if the attribute was not previously part of a join,
**		    the attribute's join_seqno is set to indicate
**		    when the attribute was first joined
**		    (unless the attributes are equal).
**
**		    If the attributes were already joined
**		    (or if the attributes are equal),
**		    FALSE is returned; otherwise TRUE is returned.
**
**		    Performance hint:  when merging lists of unequal size,
**		    performance is better if the larger list is specified first.
*/
bool
IIQFmjMarkJoined(att0, att1)
ATTRIBINFO	*att0, *att1;
{
	ATTRIBINFO	*att;
	i4		join_seqno_at_entry = join_seqno;
	if (att0 == att1) return (FALSE);
	if (att0 == att0->join_att)
	{
		att0->join_seqno = join_seqno++;
	}
	if (att1 == att1->join_att)
	{
		att1->join_seqno = join_seqno++;
	}
	if (join_seqno == join_seqno_at_entry)
	{
		att = att1;
		do
		{
			if (att == att0) return (FALSE);
			att = att->join_att;
		}	while (att != att1);
	}
	att = att1->join_att;
	att1->join_att = att0->join_att;
	att0->join_att = att;
	return (TRUE);
}
/*
**  IIQFfjaFixJoinedAttributes
**
**		    Fix the attributes to contain
**		    the correct info for a new set of joins.
**		    Check for errors E_QF0168 and E_QF0169
**		    (displaying error message(s) where appropriate).
**		    Set global variables IIQFsmmSystemUserLkeyMM and
**		    IIQFsmdSystemUserLkeyMD and IIQFsddSystemUserLkeyDD
**		    to indicate whether there are any MM, MD, or DD joins
**		    from a system-maintained logical key
**		    to a user-maintained logical key.
**
**		    At entry, all attributes have
**		    their join_att fields set to link groups
**		    of joined attributes into circular lists.
**		    (Unjoined attributes are in 1-element circular lists).
**		    Note that the jpart field may be used to help choose
**		    the attribute to be marked mqJOIN_FIELD (see below).
**
**		    At exit, unjoined attributes are unmodified,
**		    and joined attributes are modified as follows:
**
**		    (1) jpart is set to mqJOIN_FIELD for the "best" attribute
**			in the group, and to mqHIDDEN_JOIN for other attributes.
**		    (2) mdtype is set to the ttype of the "best" attribute.
**		    (3) join_att is set to point to the "best" attribute.
**		    (4) formfield is copied from the first attribute
**			in the group.
**		    (5) opupdate is set to 0, **if requested**.
**
**		    The "best" attribute is determined by the following
**		    criteria (most important first):
**
**		    (1) An attribute in a master is better than one in a detail.
**		    (2) A system-maintained logical key is better than
**			a user-maintained logical key.
**		    (3) An attr that had jpart == mqJOIN_FIELD is better than
**			an attr that had jpart != mqJOIN_FIELD.
**		    (4) Attribute A is better than attribute B if
**			A was joined before B (A's join_seqno is smaller).
**
**		    Input parameters:
**
**		    qdef		pointer to query descriptor
**		    clear_opupdate	should opupdate fields be cleared?
**
**		    Returns:  FALSE if errors were detected; TRUE otherwise.
*/
bool
IIQFfjaFixJoinedAttributes(qdef, clear_opupdate)
MQQDEF	*qdef;
bool	clear_opupdate;
{
    ATTRIBINFO	*att, *next_att, *best_att, *first_att, *sys_att;
    bool	found_master, found_syslkey, found_joinfld, success;
    i4		i, best_join_seqno;
    success = TRUE;	/* until we find an error */
    IIQFsmmSystemUserLkeyMM = FALSE;
    IIQFsmdSystemUserLkeyMD = FALSE;
    IIQFsddSystemUserLkeyDD = FALSE;
    for (i = 0; i < qdef->mqnumatts; i++)
    {
	att = qdef->mqatts[i];
	/*
	**  If this attribute hasn't been joined to anything or is part
	**  of a group of joined columns that we've already processed,
	**  skip it.
	*/
	next_att = att->join_att;
	if (next_att->join_att == next_att) continue;
	/*
	**  We've found the first attribute in a group 
	**  of joined attributes that need processing.
	*/
	first_att = att;
	/*
	**  Now we find the "best" attribute in the group.
	*/
	best_att = att;
	found_master = FALSE;
	found_syslkey = FALSE;
	found_joinfld = FALSE;
	best_join_seqno = MAXI2;
	do
	{
            /*
            ** This second loop WAS a "for (;;)" loop, with comments 
            ** indicating it's "just a block -- not a true loop". For
            ** readability, the code breaks out of a loop
            ** rather than constructing some "clever" but
            ** completely unreadable, monstrous, if-then-else
            ** code. The wonderful ACC compiler doesn't like it,
            ** since we always "break", never reaching the end
            ** of loop (effectively only executing this code
            ** once). Well, to fool the stupid compiler, we use
            ** a do-while (1==0) loop, which will allow us
            ** to keep the "readable" code as is.
            **
            ** 10-aug-92 (daver)
            */
	    do
	    {
		if (!found_master && att->ttype == mqIN_MASTER)
		{
		    found_master = TRUE;
		    found_syslkey = FALSE;
		    found_joinfld = FALSE;
		    best_join_seqno = MAXI2;
		    best_att = att;
		}
		if (found_master && att->ttype != mqIN_MASTER)
		{
		    break;
		}
		if (!found_syslkey && (att->lkeyinfo & mqSYSTEM_MAINT))
		{
		    found_syslkey = TRUE;
		    found_joinfld = FALSE;
		    best_join_seqno = MAXI2;
		    best_att = att;
		}
		if (found_syslkey && !(att->lkeyinfo & mqSYSTEM_MAINT))
		{
		    break;
		}
		if (!found_joinfld && (att->jpart == mqJOIN_FIELD))
		{
		    found_joinfld = TRUE;
		    best_join_seqno = MAXI2;
		    best_att = att;
		}
		if (found_joinfld && (att->jpart != mqJOIN_FIELD))
		{
		    break;
		}
		if (att->join_seqno < best_join_seqno)
		{
		    best_join_seqno = att->join_seqno;
		    best_att = att;
		}
	    } while (1 == 0);
	    /* only execute the above ONCE -- its a block, not a loop */
	    att = att->join_att;
	}   while (att != first_att);
	/*
	**  If the joined attributes are logical keys,
	**  we want to do some special edits.
	**  (Note that we can't have a mixture of logical keys
	**  and non-logical keys within a group of joined attributes,
	**  because a join between a logical key and a non-logical key
	**  is not permitted).
	*/
	if (att->lkeyinfo != 0)
	{
	if (IIQFljd_LkeyJoinsDisallowed)
	{
	    /*
	    **  If joins on logical keys are being disallowed,
	    **  we'll complain about all the joined logical keys
	    **  (error E_QF0172).
	    */
	    success = FALSE;
	    att = best_att->join_att;
	    while (att != best_att)
	    {
		IIUGerr(E_QF0172_lkey_lkey_join, 0,
		    4, att->col, att->rvar,
		    best_att->col, best_att->rvar);
		att = att->join_att;
	    }
	}
	else
	{
	    bool    found_master_syslkey;
	    bool    found_master_usrlkey;
	    bool    found_detail_syslkey;
	    bool    found_detail_usrlkey;
	    /*
	    **  If we found a master, we'll look for a detail
	    **  system-maintained logical key, which is error E_QF0169.
	    **  We'll also set flags indicating which types of logical keys
	    **  are present.  Note that in this case, found_syslkey really
	    **  indicates whether we found a *master* system logical key.
	    */
	    if (found_master)
	    {
		found_master_syslkey = found_syslkey;
		found_master_usrlkey = FALSE;
		found_detail_syslkey = FALSE;
		found_detail_usrlkey = FALSE;
		att = first_att;
		do
		{
		    if (att->ttype == mqIN_MASTER)
		    {
			if (!(att->lkeyinfo & mqSYSTEM_MAINT))
			{
			    found_master_usrlkey = TRUE;
			}
		    }
		    else
		    {
			if (att->lkeyinfo & mqSYSTEM_MAINT)
			{
			    found_detail_syslkey = TRUE;
			    success = FALSE;
			    IIUGerr(E_QF0169_user_system_MD_join, 0,
				2, att->col, att->rvar);
			}
			else
			{
			    found_detail_usrlkey = TRUE;
			}
		    }
		    att = att->join_att;
		}   while (att != first_att);
	    }
	    /*
	    **  If we did *not* find a master, we still need to set flags
	    **  indicating which types of logical keys are present.
	    */
	    else
	    {
		found_master_syslkey = FALSE;
		found_master_usrlkey = FALSE;
		found_detail_syslkey = found_syslkey;
		found_detail_usrlkey = FALSE;
		att = first_att;
		do
		{
		    if (!(att->lkeyinfo & mqSYSTEM_MAINT))
		    {
			found_detail_usrlkey = TRUE;
		    }
		    att = att->join_att;
		}   while (att != first_att);
	    }
	    /*
	    **  Set global variables based on what sorts of logical keys
	    **  we found.  (They indicate the restrictions we have to place
	    **  on appends for this release).
	    */
	    IIQFsmmSystemUserLkeyMM |=
		(found_master_syslkey && found_master_usrlkey);
	    IIQFsmdSystemUserLkeyMD |=
		(found_master_syslkey && found_detail_usrlkey);
	    IIQFsddSystemUserLkeyDD |=
		(found_detail_syslkey && found_detail_usrlkey);
	    /*
	    **  If we found a system-maintained logical key,
	    **  we'll look for another, which is error E_QF0168.
	    */
	    if (found_master_syslkey || found_detail_syslkey)
	    {
		att = first_att;
		while (!(att->lkeyinfo & mqSYSTEM_MAINT))
		{
		    att = att->join_att;
		}
		sys_att = att;
		att = att->join_att;
		while (att != first_att)
		{
		    if (att->lkeyinfo & mqSYSTEM_MAINT)
		    {
			success = FALSE;
			IIUGerr(E_QF0168_system_system_join, 0,
			    4, att->col, att->rvar,
			    sys_att->col, sys_att->rvar);
		    }
		    att = att->join_att;
		}
	    }
	}
	}
	/*
	**  Now we go through the group 
	**  and set jpart, mdtype, opupdate, formfield, and join_att
	**  to their proper values.
	*/
	att = first_att;
	do
	{
	    att->mdtype = best_att->ttype;
	    if (clear_opupdate)
	    {
		att->opupdate = 0;
	    }
	    if (att != best_att)
	    {
		att->jpart = mqHIDDEN_JOIN;
		STcopy(best_att->formfield, att->formfield);
	    }
	    next_att = att->join_att;
	    att->join_att = best_att;
	    att = next_att;
	}   while (att != first_att);
	best_att->jpart = mqJOIN_FIELD;
    }
    return (success);
}