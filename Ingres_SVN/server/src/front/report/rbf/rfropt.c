# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/* static char	Sccsid[] = "@(#)rfropt.qc	30.1	11/14/84"; */
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include       "rbftype.h"
# include       "rbfglob.h"
# include	 <cm.h>
# include	<st.h>
# include	<ug.h>
# include       <flddesc.h>
# ifdef i64_win
# pragma optimize("", off)
# endif
/*
**   RFROPTIONS - display and control the ROPTIONS form to select
**	options to the report.
**
**	Parameters:
**		none.
**
**	Returns:
**		non-zero if changes made, zero if none.
**
**	Side Effects:
**		Fields in the ROPTIONS form.
**
**	Called By:
**		rFdisplay.
**
**	Trace Flags:
**		230, 233.
**
**	Error Messages:
**		none.
**
**	History:
**		6/2/82 (ps)	written.
**		26-nov-1986 (yamamoto)
**			Modified for double byte characters.
**		1/15/87 (rld)	change syntax of INQUIRE_FRS calls.
**      	9/22/89 (elein) UG changes ingresug change #90045
**			changed <rbftype.h> & <rbfglob.h> to 
**			"rbftype.h" & "rbfglob.h"
**		15-aug-89 (cmr)	support for new form field 'rphdr_first'.
**		30-jan-90 (cmr)	updated the way underlining capabilities
**				are set for the report; a new tablefield 
**				ropttbl allows the user to specify the 
**				underlining for each section in the report.
**		31-jan-90 (cmr)	Update Opt struct when form changessince 
**				rFm_rhead() now uses this struct instead
**				of doing getforms.
**		20-feb-90 (sylviap)	
**			Took out all uses of abbreviations.
**		04-sep-90 (sylviap)	
**			Added explanations to the activate menuitems. #32699
**		04-mar-91 (steveh)
**			Fixed bug 9527.  This bug caused null option strings
**			which contained quotes (") to corrupt the report.
**			The problem was fixed by inserting backslashes (\)
**			before the quotes when copying from the form and
**			removing the backslashes before displaying in the
**			form.
**		01-oct-91 (jillb/jroy--DGC)
**		    make function declarations consistent with their definitions
**		07-jul-93 (sylviap/rdrane)
**		    Modified the form roptions.frm to prompt for first formfeed
**		    and to edit the date, time and page formats in an RBF 
**		    report.  Fix-up all IIUGerr() invocations to use
**		    UG_ERR_ERROR or UG_ERR_FATAL #define instead of hard-coded
**		    constant.
**		12-jul-1993 (rdrane)
**		    Correct setting of Opt.no1stff - negative logic can be
**		    tricky.
**		19-jul-93 (sylviap)
**		    Added listchoices for yes/no choices for the fields: insert
**		    formfeeds, insert first ff, print page hdr and include 
**		    report components.
**		13-sep-1993 (rdrane)
**		    Ensure that the no1stff field remains invisible under all
**		    circumstances if we're connected to a pre-6.5 database.
**		    This prevents the user from shooting themselves in the
**		    foot by preventing subsequent generation of a 6.5
**		    .NOFIRSTFF RW command.
**		25-oct-1993 (rdrane)
**		    Re-work validation of date, time, and pageno formats as fix
**		    for 55163 and 55141.  Validation code commonized and placed
**		    in static routine rfoptchkfmt().  Note that this validation
**		    scheme dependes upon the form effecting the validation of
**		    the y/n inclusion fields.
**		10-dec-1993 (rdrane)
**			Don't validate the format on exit if the inclusion
**			field is "no" (b57945).
**		22-mar-1995 (forky01)
**		    Previously it was assumed that all 6.5 catalogs had a level
**		    of 00605 for STANDARD_CATALOG_LEVEL.  Under STAR this was
**		    reverted to 00602 for some reason.  Rather than try to
**		    rearchitect the entire system for catalog versions to be
**		    consistent, which could break other things, I now check to
**		    see if cat_level is less than 6.5, to also test if this is
**		    a >= 6.2 STAR database.  If this is the case then we allow
**		    the invisible print first formfeed field to be visible.
**		    Older versions of report do not support that command, so
**		    must verify somehow that we are 6.5.  This is just a
**		    temporary fix, as a later project will redefine what STAR
**		    catalogs are.  Fixes bug 67325.
**      02-mar-1996 (morayf)
**                  Use of IIUIdcd_dist as a boolean variable is incorrect.
**                  It is declared as a pointer to a boolean-returning function,
**                  and hence needs to be _called_ not just referenced, as
**		    indeed it is elsewhere.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	26-feb-2004 (somsa01)
**	    Added NO_OPTIM for i64_win to prevent SEGV in RBF from test
**	    rbf029.sep.
*/
/*
** Date/time/pageno validation routine return codes
*/
# define RFO_OK			0
# define RFO_DF_TYPE		1
# define RFO_DF_INTERNAL	-1
# define RFO_TF_TYPE		2
# define RFO_TF_INTERNAL	-2
# define RFO_PF_TYPE		3
# define RFO_PF_INTERNAL	-3
# define RFO_FMT_INTERNAL	-10
FUNC_EXTERN VOID 	IIUFfieldHelp();
FUNC_EXTERN char    	*IIRFfwd_FullWord();
FUNC_EXTERN FMT    	*vfchkFmt();
static	    char	*put_ulstr();
static	    i4		rfoptchkfmt();
static	    i4		rows, cols;
  static char *und_tbl[3];
  static char datef[(MAX_FMTSTR + 1)];
  static char timef[(MAX_FMTSTR + 1)];
  static char pagef[(MAX_FMTSTR + 1)];
i4
rFroptions()
{
	/* external declarations */
  GLOBALREF char *Nroptions;
	/* internal declarations */
  i4 anychanges;
  i4 i;
  i4 r_index;
  char sec_name[FE_MAXNAME + 16];
  char und[16];
	char		*l_name;
  char tstr[(MAX_FMTSTR + 1)];
  char tstr2[(MAX_FMTSTR + 1)];
  char ffield[FE_MAXNAME + 1];
	Sec_node	*n;
	static bool	init_tbl = FALSE;	/* true = und_tbl has been 
						** initialized */
	char            *prompt;
  char *choices;
  char *help_scr;
  char *fullwrd;
  char *date;
  char *time;
  char *page;
  char *firstff;
  char *include;
  char *format;
		/* names of fields in frame roptions.frm */
  char *opt_tbl;
  char *ropt_und;
  char *rpglen;
  char *rundchr;
  char *rformfeed;
  char *rpghdr;
  char *rnull;
  char *rcomp_tbl;
	/* intialize variables for the form roptions.frm */
	opt_tbl = F_OPTTBL;
	ropt_und = F_UNDER;
	rpglen = F_PGLEN;
	rundchr = F_UNDCHR;
	rformfeed = F_FRMFD;
	rpghdr = F_PGHDR;
	rnull = F_NULL;
	rcomp_tbl = F_RCOMP;
	date = RBF_DATE_LIT;
	time = RBF_TIME_LIT;
	page = RBF_PAGE_LIT;
	firstff = F_FIRSTFF;
	include = F_INCL;
	format = F_FMT;
	/*
	** Copy the date/time and pageno formats into FRS-known variables
	*/
	STcopy(&Opt.rdate_fmt[0],&datef[0]);
	STcopy(&Opt.rtime_fmt[0],&timef[0]);
	STcopy(&Opt.rpageno_fmt[0],&pagef[0]);
	/* start of routine */
#	ifdef	xRTR1
	if (TRgettrace(230,0) || TRgettrace(233,0))
	{
		SIprintf(ERx("rFroptions: entry.\r\n"));
	}
#	endif
	if (!init_tbl)
	{
		/* initialize the table for underlining choices */
		und_tbl[0] = ERget (F_RF001E_ul_none);
		und_tbl[1] = ERget (F_RF001F_ul_all);
		und_tbl[2] = ERget (F_RF001D_ul_last);
		init_tbl = TRUE;
	}
/* # line 247 "rfropt.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 248 "rfropt.qsc" */	/* display */
  {
    if (IIdispfrm(Nroptions,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 252 "rfropt.qsc" */	/* host code */
		STcopy(Opt.rpl, tstr);
/* # line 253 "rfropt.qsc" */	/* putform */
      {
        if (IIfsetio(Nroptions) != 0) {
          IIputfldio(rpglen,(short *)0,1,32,0,tstr);
        } /* IIfsetio */
      }
/* # line 255 "rfropt.qsc" */	/* host code */
		/*
		** Fill in default value for inserting formfeeds and
		** make the first ff prompt in/visible
		*/
/* # line 260 "rfropt.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,Nroptions,(char *)0, (char *)0) != 0) {
          IIstfsio(54,firstff,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 261 "rfropt.qsc" */	/* host code */
		if (Opt.rffs == ERx('y'))
		{
	    		STcopy(yn_tbl[0], tstr);
			if ( (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) >= 0)
			     || ( IIUIdcd_dist() == TRUE &&
				  ( STcompare(IIUIscl_StdCatLevel(),
					    UI_LEVEL_62) >= 0 )) 
			   )
			{
/* # line 270 "rfropt.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,Nroptions,(char *)0, (char *)0) != 0) {
          IIstfsio(54,firstff,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 271 "rfropt.qsc" */	/* host code */
			}
		}
		else
		{
	    		STcopy(yn_tbl[1], tstr);
		}
/* # line 277 "rfropt.qsc" */	/* putform */
      {
        if (IIfsetio(Nroptions) != 0) {
          IIputfldio(rformfeed,(short *)0,1,32,0,tstr);
        } /* IIfsetio */
      }
/* # line 279 "rfropt.qsc" */	/* host code */
		/*
		** Be careful defaulting the "No first formfeed", since
		** the form says "Print..." and a 'y' in ReportOptions
		** Opt structure means "Suppress" (wonderful thing, negative
		** logic)
		*/
		if (Opt.no1stff == ERx('y'))
		{
	    		STcopy(yn_tbl[1], tstr);
		}
		else
		{
	    		STcopy(yn_tbl[0], tstr);
		}
/* # line 293 "rfropt.qsc" */	/* putform */
      {
        if (IIfsetio(Nroptions) != 0) {
          IIputfldio(firstff,(short *)0,1,32,0,tstr);
        } /* IIfsetio */
      }
/* # line 295 "rfropt.qsc" */	/* host code */
		STprintf(tstr,ERx("%c"),Opt.rulchar);
/* # line 296 "rfropt.qsc" */	/* putform */
      {
        if (IIfsetio(Nroptions) != 0) {
          IIputfldio(rundchr,(short *)0,1,32,0,tstr);
        } /* IIfsetio */
      }
/* # line 298 "rfropt.qsc" */	/* host code */
		/* strip backslashes from the string (bug 9527) */
		rF_unbstrcpy(tstr, Opt.rnulstr);
/* # line 301 "rfropt.qsc" */	/* putform */
      {
        if (IIfsetio(Nroptions) != 0) {
          IIputfldio(rnull,(short *)0,1,32,0,tstr);
        } /* IIfsetio */
      }
/* # line 303 "rfropt.qsc" */	/* host code */
		if (Opt.rphdr_first == ERx('y'))
	    		STcopy(yn_tbl[0], tstr);
		else
	    		STcopy(yn_tbl[1], tstr);
/* # line 307 "rfropt.qsc" */	/* putform */
      {
        if (IIfsetio(Nroptions) != 0) {
          IIputfldio(rpghdr,(short *)0,1,32,0,tstr);
        } /* IIfsetio */
      }
/* # line 309 "rfropt.qsc" */	/* inittable */
      {
        if (IItbinit(Nroptions,opt_tbl,(char *)"u") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 310 "rfropt.qsc" */	/* host code */
		for ( n = Sections.head; n->sec_type != SEC_END; n = n->next )
		{
			STcopy( n->sec_name, sec_name );
			l_name = sec_name;
			while ( l_name = STindex( sec_name, ERx("-"), 0) )
				*l_name = ' ';
			STcopy(put_ulstr(n->sec_under), und);
/* # line 317 "rfropt.qsc" */	/* loadtable */
      {
        if (IItbact(Nroptions,opt_tbl,1) != 0) {
          IItcoputio((char *)"ropt_section",(short *)0,1,32,0,sec_name);
          IItcoputio(ropt_und,(short *)0,1,32,0,und);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 320 "rfropt.qsc" */	/* host code */
		}
		/* Load the component table with 'Date', 'Time' and 'Page' */
/* # line 324 "rfropt.qsc" */	/* inittable */
      {
        if (IItbinit(Nroptions,rcomp_tbl,(char *)"u") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 325 "rfropt.qsc" */	/* host code */
		/*
		** Set the "include component" field based upon the
		** value in Report Options.
		*/
		if (Opt.rdate_inc_fmt == ERx('y'))
		{
	    		STcopy(yn_tbl[0],tstr);	/* copy yes */
			STcopy(datef, tstr2);	/* copy date format */
		}
		else
		{
	    		STcopy(yn_tbl[1],tstr);	/* copy no */
			tstr2[0] = EOS;		/* blank out date format */
		}
/* # line 339 "rfropt.qsc" */	/* loadtable */
      {
        if (IItbact(Nroptions,rcomp_tbl,1) != 0) {
          IItcoputio((char *)"p_comp",(short *)0,1,32,0,date);
          IItcoputio(include,(short *)0,1,32,0,tstr);
          IItcoputio(format,(short *)0,1,32,0,tstr2);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 343 "rfropt.qsc" */	/* host code */
		if (Opt.rtime_inc_fmt == ERx('y'))
		{
	    		STcopy(yn_tbl[0],tstr);	/* copy yes */
			STcopy(timef, tstr2);	/* copy time format */
		}
		else
		{
	    		STcopy(yn_tbl[1],tstr);	/* copy no */
			tstr2[0] = EOS;		/* blank out time format */
		}
/* # line 353 "rfropt.qsc" */	/* loadtable */
      {
        if (IItbact(Nroptions,rcomp_tbl,1) != 0) {
          IItcoputio((char *)"p_comp",(short *)0,1,32,0,time);
          IItcoputio(include,(short *)0,1,32,0,tstr);
          IItcoputio(format,(short *)0,1,32,0,tstr2);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 357 "rfropt.qsc" */	/* host code */
		if (Opt.rpageno_inc_fmt == ERx('y'))
		{
	    		STcopy(yn_tbl[0],tstr);	/* copy yes */
			STcopy(pagef, tstr2);	/* copy page format */
		}
		else
		{
	    		STcopy(yn_tbl[1],tstr);	/* copy no */
			tstr2[0] = EOS;		/* blank out page format */
		}
/* # line 367 "rfropt.qsc" */	/* loadtable */
      {
        if (IItbact(Nroptions,rcomp_tbl,1) != 0) {
          IItcoputio((char *)"p_comp",(short *)0,1,32,0,page);
          IItcoputio(include,(short *)0,1,32,0,tstr);
          IItcoputio(format,(short *)0,1,32,0,tstr2);
          IITBceColEnd();
        } /* IItbact */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 373 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,rpghdr);
            } /* IIfsetio */
          }
/* # line 376 "rfropt.qsc" */	/* host code */
		 /* get the full word */
		if ((fullwrd =
			IIRFfwd_FullWord (tstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, UG_ERR_ERROR, 0);
/* # line 381 "rfropt.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 382 "rfropt.qsc" */	/* host code */
		}
		else
		{
/* # line 385 "rfropt.qsc" */	/* putform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIputfldio(rpghdr,(short *)0,1,32,0,fullwrd);
            } /* IIfsetio */
          }
/* # line 386 "rfropt.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 387 "rfropt.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 2) {
        {
/* # line 391 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,rformfeed);
            } /* IIfsetio */
          }
/* # line 394 "rfropt.qsc" */	/* host code */
		 /* get the full word */
		if ((fullwrd =
			IIRFfwd_FullWord (tstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, UG_ERR_ERROR, 0); 
/* # line 399 "rfropt.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 400 "rfropt.qsc" */	/* host code */
		}
		else 
		{ 
/* # line 403 "rfropt.qsc" */	/* putform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIputfldio(rformfeed,(short *)0,1,32,0,fullwrd);
            } /* IIfsetio */
          }
/* # line 404 "rfropt.qsc" */	/* host code */
			/* 
			** Check to make 'First Formfeed' field in/visible
			** based on whether report contains formfeeds.
			** It will always be/remain invisible for pre-6.5
			** connections.
			*/
			if  ((STcompare(fullwrd,ERget(F_UG0002_Yes2)) == 0) &&
			     ((STcompare(IIUIscl_StdCatLevel(),
                                         UI_LEVEL_65) >= 0)
			       || ( IIUIdcd_dist() == TRUE &&
				    ( STcompare(IIUIscl_StdCatLevel(),
						UI_LEVEL_62) >= 0 ))) 
			    )
			{
/* # line 418 "rfropt.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIstfsio(54,firstff,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
              0));
            } /* IIiqset */
          }
/* # line 419 "rfropt.qsc" */	/* host code */
			}
			else
			{
/* # line 422 "rfropt.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIstfsio(54,firstff,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
              1));
            } /* IIiqset */
          }
/* # line 423 "rfropt.qsc" */	/* host code */
			}
/* # line 424 "rfropt.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 425 "rfropt.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 430 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,firstff);
            } /* IIfsetio */
          }
/* # line 432 "rfropt.qsc" */	/* host code */
		 /* get the full word */
		if ((fullwrd =
			IIRFfwd_FullWord (tstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, UG_ERR_ERROR, 0); 
/* # line 437 "rfropt.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 438 "rfropt.qsc" */	/* host code */
		}
		else 
		{
/* # line 441 "rfropt.qsc" */	/* putform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIputfldio(firstff,(short *)0,1,32,0,fullwrd);
            } /* IIfsetio */
          }
/* # line 442 "rfropt.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 443 "rfropt.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 4) {
        {
/* # line 448 "rfropt.qsc" */	/* host code */
		/* 
		** Make sure valid yes/no input is entered.  If partial
		** input like 'y' enter, then fill it out w/ 'yes'.
		*/
/* # line 452 "rfropt.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Nroptions,rcomp_tbl,-2) != 0) {
              IItcogetio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,include);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 453 "rfropt.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&r_index,29,(char *)0,0);
            } /* IIiqset */
          }
/* # line 454 "rfropt.qsc" */	/* host code */
		if ((fullwrd =
			IIRFfwd_FullWord (tstr, 2, yn_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, UG_ERR_ERROR, 0); 
/* # line 458 "rfropt.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 459 "rfropt.qsc" */	/* host code */
		}
		else 
		{ 
/* # line 462 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(include,(short *)0,1,32,0,fullwrd);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 463 "rfropt.qsc" */	/* host code */
			if (STcompare(fullwrd, ERget(F_UG0002_Yes2)) == 0 )
			{
			    if (r_index == 1)		/* row = date */
			    {
/* # line 467 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,datef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 468 "rfropt.qsc" */	/* host code */
			    }
			    else if (r_index == 2)	/* row = time */
			    {
/* # line 471 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,timef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 472 "rfropt.qsc" */	/* host code */
			    }
			    else 		        /* row = page */
			    {
/* # line 475 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,pagef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 476 "rfropt.qsc" */	/* host code */
			    }
/* # line 477 "rfropt.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 478 "rfropt.qsc" */	/* host code */
			}
			else
			{
			    /* 
			    ** User selected 'no' so blank out format and
			    ** move cursor to next row depending upon which
			    ** direction they were scrolling.
			    **  4 = nextfield - tab
			    **  5 = previousfield - ^P
			    **  6 = downline - down arrow
			    **  7 = upline - up arrow
			    ** 10 = scrollup
			    ** 11 = scrolldown
			    ** 12 = nextitem
			    */
/* # line 493 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,(char *)"");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 494 "rfropt.qsc" */	/* inquire_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&i,38,(char *)0,0);
            } /* IIiqset */
          }
/* # line 495 "rfropt.qsc" */	/* host code */
			    if (i == 4) 
			    {
				/* 
				** If tabbing then tab to the page length field.
				*/
/* # line 500 "rfropt.qsc" */	/* resume */
          {
            IIresfld(rpglen);
            if (1) goto IIfdB1;
          }
/* # line 501 "rfropt.qsc" */	/* host code */
			    }
			    else if (i == 5) 
			    {
				/* 
				** If ^P (Previousfield) move to underline 
				** tablefield
				*/
/* # line 508 "rfropt.qsc" */	/* resume */
          {
            IIresfld(opt_tbl);
            if (1) goto IIfdB1;
          }
/* # line 509 "rfropt.qsc" */	/* host code */
			    }
			    else if ((i == 7) || (i == 10))
			    {
			        if ((r_index == 2) || (r_index == 3))
				    r_index--;	      /* move to previous row */
				else
/* # line 515 "rfropt.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 516 "rfropt.qsc" */	/* host code */
			    }
			    else 
			    {
			        if ((r_index == 1) || (r_index == 2))
				    r_index++;	         /* move to next row */
				else
				    /* move to page length field */
/* # line 523 "rfropt.qsc" */	/* resume */
          {
            IIresfld(rpglen);
            if (1) goto IIfdB1;
          }
/* # line 524 "rfropt.qsc" */	/* host code */
			    }
/* # line 525 "rfropt.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Nroptions,rcomp_tbl,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,r_index) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 526 "rfropt.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 527 "rfropt.qsc" */	/* host code */
			}
		}
        }
      } else if (IIretval() == 5) {
        {
/* # line 533 "rfropt.qsc" */	/* host code */
		/* 
		** First need to check if component is 'included'.  If not, 
		** then move cursor to 'include' column - don't want cursor on
		** blank format column.
		*/
/* # line 538 "rfropt.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Nroptions,rcomp_tbl,-2) != 0) {
              IItcogetio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,include);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 539 "rfropt.qsc" */	/* host code */
		if (STcompare(tstr, ERget(F_UG0002_Yes2)) != 0 )
/* # line 540 "rfropt.qsc" */	/* resume */
          {
            IIrescol(rcomp_tbl,include);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 545 "rfropt.qsc" */	/* host code */
		/* 
		** Now we know component *is* included so need to validate 
		** formats, so get the inputed format;
		** get the row count (1 = date, 2 = time, 3 = page)
		** and validate the format.
		**
		** NOTE: Format field is scrollable to 96 characters because
		**       can only save up to 100 chars in ii_rcommands
		**	 and RBF needs to escape single quotes:
		**	 '<96 char format>' + escaping 2 internal single quotes
		**				= 100 chars
		*/
/* # line 557 "rfropt.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Nroptions,rcomp_tbl,-2) != 0) {
              IItcogetio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,format);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 559 "rfropt.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&i,29,(char *)0,0);
            } /* IIiqset */
          }
/* # line 560 "rfropt.qsc" */	/* host code */
		switch(rfoptchkfmt(i,tstr))		/* validate format*/
		{
		case RFO_OK:
/* # line 563 "rfropt.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 564 "rfropt.qsc" */	/* host code */
			break;
		case RFO_DF_TYPE:
		case RFO_DF_INTERNAL:
/* # line 567 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,datef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 568 "rfropt.qsc" */	/* host code */
			break;
		case RFO_TF_TYPE:
		case RFO_TF_INTERNAL:
/* # line 571 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,timef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 572 "rfropt.qsc" */	/* host code */
			break;
		case RFO_PF_TYPE:
		case RFO_PF_INTERNAL:
/* # line 575 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,pagef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 576 "rfropt.qsc" */	/* host code */
			break;
		default:
			switch(i)
			{
			case 1:
/* # line 581 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,datef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 582 "rfropt.qsc" */	/* host code */
				break;
			case 2:
/* # line 584 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,timef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 585 "rfropt.qsc" */	/* host code */
				break;
			case 3:
/* # line 587 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,pagef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 588 "rfropt.qsc" */	/* host code */
				break;
			default:
				break;
			}
			break;
		}
/* # line 594 "rfropt.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 599 "rfropt.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Nroptions,opt_tbl,-2) != 0) {
              IItcogetio((short *)0,1,30,4,&r_index,(char *)"_RECORD");
              IItcogetio((short *)0,1,32,15,und,ropt_und);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 603 "rfropt.qsc" */	/* host code */
		for ( i = 1, n = Sections.head; i < r_index; i++, n = n->next );
		/* get the full word */
		if ((fullwrd =
			IIRFfwd_FullWord (und, 3, und_tbl)) == NULL)
		{
			IIUGerr(E_RF007E_input_not_found, UG_ERR_ERROR, 0); 
/* # line 610 "rfropt.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 611 "rfropt.qsc" */	/* host code */
		}
		else 
		{ 
			if (STcompare(fullwrd, ERget(F_RF001D_ul_last)) == 0 )
				n->sec_under = 'l';
			else
				if (!STcompare(fullwrd, ERget(F_RF001F_ul_all)))
					n->sec_under = ERx('a');
				else
					n->sec_under = ERx('n');
/* # line 621 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,opt_tbl,-2) != 0) {
              IItcoputio(ropt_und,(short *)0,1,32,0,fullwrd);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 622 "rfropt.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 623 "rfropt.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 8) {
        {
/* # line 629 "rfropt.qsc" */	/* inquire_frs */
          {
            if (IIiqset(2,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,ffield,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 630 "rfropt.qsc" */	/* host code */
		/* check what field/tablefield cursor is on */
		if (STcompare (ffield, opt_tbl) == 0)
		{
			/*
			** Display Popup with the 'underlining' options.  User
			** may choose none, last or all.
			*/
			help_scr  = ERget(S_RF0077_Underlining);
			prompt    = ERget(F_RF0067_und_line_title);
			choices   = ERget(F_RF0068_underline_choices);
			i = IIFDlpListPick(prompt, choices, 0,
				LPGR_FLOAT, LPGR_FLOAT, help_scr, H_UNDOPT,
				NULL,  NULL);
			if (i >= 0)
			{
				/* put in what user selected from listpick */
/* # line 646 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,opt_tbl,-2) != 0) {
              IItcoputio(ropt_und,(short *)0,1,32,0,und_tbl[i]);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 647 "rfropt.qsc" */	/* host code */
			}
		}
		else if (STcompare (ffield, rcomp_tbl) == 0)
		{
/* # line 651 "rfropt.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,ffield,34,(char *)0,0);
            } /* IIiqset */
          }
/* # line 652 "rfropt.qsc" */	/* host code */
			if (STcompare (ffield, include) == 0)
			{
				/*
				** Display Popup with the 'include report
				** components' options.  User may choose yes or
				** no.
				*/
				help_scr  = ERget(S_RF00B7_include_help);
				prompt    = ERget(F_RF00A3_include_title);
				choices   = ERget(F_RF00A4_include_choice);
				i = IIFDlpListPick(prompt, choices, 0,
				   LPGR_FLOAT, LPGR_FLOAT, help_scr, H_INCLCOMP,
				   NULL,  NULL);
				/* Need to row which row cursor is on */
/* # line 667 "rfropt.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&r_index,29,(char *)0,0);
            } /* IIiqset */
          }
/* # line 668 "rfropt.qsc" */	/* host code */
				if (i >= 0)
				{
					/* 
					** display what user selected in 
					** listpick 
					*/
	    				STcopy(yn_tbl[i], tstr); 
/* # line 675 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(include,(short *)0,1,32,0,tstr);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 677 "rfropt.qsc" */	/* host code */
					if (i == 1)
					{
					   /* 
					   ** User selected 'No' from 
					   ** listchoices so need to blank out 
					   ** format 
					   */
/* # line 684 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,(char *)"");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 686 "rfropt.qsc" */	/* host code */
					}
					else 		
					{
					   /* 
					   ** User selected `yes' from 
					   ** listchoices, so fill in format 
					   ** and global variable
					   */
			                   if (r_index==1)	/* row = date */
			                   {
/* # line 696 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,datef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 698 "rfropt.qsc" */	/* host code */
			                    }
			                    else if (r_index==2)/* row = time */
			                    {
/* # line 701 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,timef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 703 "rfropt.qsc" */	/* host code */
			    		    }
			    		    else                /* row = page */
			                    {
/* # line 706 "rfropt.qsc" */	/* putrow */
          {
            if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
              IItcoputio(format,(short *)0,1,32,0,pagef);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 708 "rfropt.qsc" */	/* host code */
			                    }
					}
				}
			}
			else
			{
				/* 
				** User is on format column, so display format 
				** listchoices msg 
				*/
				choices   = ERget(S_RF00B0_format);
/* # line 719 "rfropt.qsc" */	/* message */
          {
            IIFRgpcontrol(1,0);
            IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
            IIFRgpcontrol(2,0);
            IImessage(choices);
          }
/* # line 720 "rfropt.qsc" */	/* host code */
			}
		}
		else if (STcompare (ffield, rformfeed) == 0)
		{
			/*
			** Display Popup with the 'Formfeed' options.  User
			** may choose yes or no.
			*/
			help_scr  = ERget(S_RF00AF_ff_help);
			prompt    = ERget(F_RF009F_ff_title);
			choices   = ERget(F_RF00A0_ff_choice);
			i = IIFDlpListPick(prompt, choices, 0,
				LPGR_FLOAT, LPGR_FLOAT, help_scr, H_FORMFEED,
				NULL,  NULL);
			if (i >= 0)
			{
				/*
				** Put in what user selected from listpick.
				** Note that firstff will always be/remain
				** invisible for pre-6.5 connections.
				*/
			        STcopy(yn_tbl[i], tstr); 
/* # line 742 "rfropt.qsc" */	/* putform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIputfldio(rformfeed,(short *)0,1,32,0,tstr);
            } /* IIfsetio */
          }
/* # line 743 "rfropt.qsc" */	/* host code */
				if  (STcompare(tstr, ERget(F_UG0007_No2)) == 0) 
				{
/* # line 745 "rfropt.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIstfsio(54,firstff,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
              1));
            } /* IIiqset */
          }
/* # line 747 "rfropt.qsc" */	/* host code */
				}
				else if ( (STcompare(IIUIscl_StdCatLevel(),
                                                     UI_LEVEL_65) >= 0)
					 || ( IIUIdcd_dist() == TRUE &&
					      ( STcompare(IIUIscl_StdCatLevel(),
							  UI_LEVEL_62) >= 0 )) 
				        )
				{
/* # line 755 "rfropt.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,Nroptions,(char *)0, (char *)0) != 0) {
              IIstfsio(54,firstff,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
              0));
            } /* IIiqset */
          }
/* # line 757 "rfropt.qsc" */	/* host code */
				}
			}
		}
		else if (STcompare (ffield, rpghdr) == 0)
		{
			/*
			** Display Popup with the 'Page Hdr' options.  User
			** may choose yes or no.
			*/
			help_scr  = ERget(S_RF00AE_pg_hdr_help);
			prompt    = ERget(F_RF009D_pg_hdr_title);
			choices   = ERget(F_RF009E_pg_hdr_choice);
			i = IIFDlpListPick(prompt, choices, 0,
				LPGR_FLOAT, LPGR_FLOAT, help_scr, H_PAGEHDR,
				NULL,  NULL);
			if (i >= 0)
			{
				/* put in what user selected from listpick */
			        STcopy(yn_tbl[i], tstr); 
/* # line 776 "rfropt.qsc" */	/* putform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIputfldio(rpghdr,(short *)0,1,32,0,tstr);
            } /* IIfsetio */
          }
/* # line 777 "rfropt.qsc" */	/* host code */
			}
		}
		else if (STcompare (ffield, firstff) == 0)
		{
			/*
			** Display Popup with the 'First Formfeed' options.  
			** User may choose yes or no.
			*/
			help_scr  = ERget(S_RF00B6_first_ff_help);
			prompt    = ERget(F_RF00A1_first_ff_title);
			choices   = ERget(F_RF00A2_first_ff_choice);
			i = IIFDlpListPick(prompt, choices, 0,
				LPGR_FLOAT, LPGR_FLOAT, help_scr, H_FIRSTFF,
				NULL,  NULL);
			if (i >= 0)
			{
				/* put in what user selected from listpick */
			        STcopy(yn_tbl[i], tstr); 
/* # line 795 "rfropt.qsc" */	/* putform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIputfldio(firstff,(short *)0,1,32,0,tstr);
            } /* IIfsetio */
          }
/* # line 796 "rfropt.qsc" */	/* host code */
			}
		}
		else 
		{
			if (STcompare (ffield, rpglen) == 0)
				choices   = ERget(S_RF0078_pg_len);
			else if (STcompare (ffield, rundchr) == 0)
				choices   = ERget(S_RF0079_underline_chr);
			else if (STcompare (ffield, rnull) == 0)
				choices   = ERget(S_RF007C_nullstring);
/* # line 806 "rfropt.qsc" */	/* message */
          {
            IIFRgpcontrol(1,0);
            IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
            IIFRgpcontrol(2,0);
            IImessage(choices);
          }
/* # line 807 "rfropt.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 9) {
        {
/* # line 813 "rfropt.qsc" */	/* host code */
		FEhelp(H_ROPTIONS, ERget(F_RF0024_Report_Options));
        }
      } else if (IIretval() == 10) {
        {
/* # line 820 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,rpglen);
            } /* IIfsetio */
          }
/* # line 821 "rfropt.qsc" */	/* host code */
		STcopy( tstr, Opt.rpl);
/* # line 823 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,rundchr);
            } /* IIfsetio */
          }
/* # line 824 "rfropt.qsc" */	/* host code */
		Opt.rulchar = tstr[0];
/* # line 826 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,rnull);
            } /* IIfsetio */
          }
/* # line 828 "rfropt.qsc" */	/* host code */
		/* copy to tstr inserting backslashes in 
		   front of quotes. (bug9527)            */
		rF_bstrcpy(Opt.rnulstr, tstr);
/* # line 832 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,rformfeed);
            } /* IIfsetio */
          }
/* # line 833 "rfropt.qsc" */	/* host code */
		Opt.rffs = tstr[0];
/* # line 835 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,firstff);
            } /* IIfsetio */
          }
/* # line 836 "rfropt.qsc" */	/* host code */
		/*
		** We know it's a valid yes/no, so we know
		** to call IIUGyn(), and there's no need
		** to look at any STATUS return.  Watch out again
		** for the negative logic!
		*/
		if  (IIUGyn(tstr,(STATUS *)NULL))
			Opt.no1stff = ERx('n');
		else
			Opt.no1stff = ERx('y');
/* # line 847 "rfropt.qsc" */	/* getform */
          {
            if (IIfsetio(Nroptions) != 0) {
              IIgetfldio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,rpghdr);
            } /* IIfsetio */
          }
/* # line 848 "rfropt.qsc" */	/* host code */
		Opt.rphdr_first = tstr[0];
		/*
		** Get if the time, date and page should be included,
		** and validate it if it should.
		*/
		i = FALSE;
/* # line 855 "rfropt.qsc" */	/* unloadtable */
          {
            if (IItbact(Nroptions,rcomp_tbl,0) == 0) goto IItbE1;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr,include);
              IItcogetio((short *)0,1,32,(MAX_FMTSTR + 1)-1,tstr2,format);
              IItcogetio((short *)0,1,30,4,&r_index,(char *)"_RECORD");
              IITBceColEnd();
              {
/* # line 858 "rfropt.qsc" */	/* host code */
			if (r_index == 1)		/* row = date */
			{
				Opt.rdate_inc_fmt = tstr[0];
			}
			else if (r_index == 2)		/* row = time */
			{
				Opt.rtime_inc_fmt = tstr[0];
			}
			else 		        	/* row = page */
			{
				Opt.rpageno_inc_fmt = tstr[0];
			}
			/*
			** validate format if the item is included
			*/
			if  (STcompare(tstr,ERget(F_UG0002_Yes2)) == 0)
			{
				switch(rfoptchkfmt(r_index,tstr2))
				{
				case RFO_OK:
					break;
				case RFO_DF_TYPE:
				case RFO_DF_INTERNAL:
/* # line 881 "rfropt.qsc" */	/* putrow */
                {
                  if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
                    IItcoputio(format,(short *)0,1,32,0,datef);
                    IITBceColEnd();
                  } /* IItbsetio */
                }
/* # line 883 "rfropt.qsc" */	/* host code */
					i = TRUE;
					break;
				case RFO_TF_TYPE:
				case RFO_TF_INTERNAL:
/* # line 887 "rfropt.qsc" */	/* putrow */
                {
                  if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
                    IItcoputio(format,(short *)0,1,32,0,timef);
                    IITBceColEnd();
                  } /* IItbsetio */
                }
/* # line 889 "rfropt.qsc" */	/* host code */
					i = TRUE;
					break;
				case RFO_PF_TYPE:
				case RFO_PF_INTERNAL:
/* # line 893 "rfropt.qsc" */	/* putrow */
                {
                  if (IItbsetio(3,Nroptions,rcomp_tbl,-2) != 0) {
                    IItcoputio(format,(short *)0,1,32,0,pagef);
                    IITBceColEnd();
                  } /* IItbsetio */
                }
/* # line 895 "rfropt.qsc" */	/* host code */
					i = TRUE;
					break;
				default:
					i = TRUE;
					break;
				}
			}
              }
            } /* IItunload */
IItbE1:
            IItunend();
          }
/* # line 904 "rfropt.qsc" */	/* host code */
		/*
		** If any of the rcomp_tbl validations failed,
		** then "cancel" the exit
		*/
		if  (i)
		{
/* # line 910 "rfropt.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 911 "rfropt.qsc" */	/* host code */
		}
		/* 
		** Copy over the time, date and page formats .
		** Note that the length (Opt.r*_w_fmt) is saved
		** when the format is verified.
		*/
	        STcopy(datef,Opt.rdate_fmt);
	        STcopy(timef,Opt.rtime_fmt);
		STcopy(pagef,Opt.rpageno_fmt);
/* # line 921 "rfropt.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&anychanges,20,Nroptions,0);
            } /* IIiqset */
          }
/* # line 922 "rfropt.qsc" */	/* enddisplay */
          {
            if (1) goto IIfdF1;
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
    if (IIFRafActFld(rpghdr,0,1) == 0) goto IIfdE1;
    if (IIFRafActFld(rformfeed,0,2) == 0) goto IIfdE1;
    if (IIFRafActFld(firstff,0,3) == 0) goto IIfdE1;
    if (IITBcaClmAct(rcomp_tbl,include,0,4) == 0) goto IIfdE1;
    if (IITBcaClmAct(rcomp_tbl,format,1,5) == 0) goto IIfdE1;
    if (IITBcaClmAct(rcomp_tbl,format,0,6) == 0) goto IIfdE1;
    if (IITBcaClmAct(opt_tbl,ropt_und,0,7) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,8) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,8) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,9) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,9) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(F_RF0076_return_layout),2,0,10) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,0,10) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 927 "rfropt.qsc" */	/* host code */
#	ifdef	xRTR2
	if (TRgettrace(233,0))
	{
		SIprintf(ERx("	Exit from rFroptions.  anychanges:%d\r\n"),
			anychanges);
	}
#	endif
/* # line 935 "rfropt.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 936 "rfropt.qsc" */	/* host code */
	return(anychanges);
}
/*
** Name:	put_ultype - determine display underline type from 
**			     internal type.
**
** Description:
**	given an underline type return the string to display to the user.
**
** Inputs:
**	utype	type of underlining.
**		ULS_NONE - none.
**		ULS_LAST - last line only.
**		ULS_ALL - all.
**
** Outputs:
**
**	Returns:
**		User string to display, which is a pointer to permanently
**		allocated memory.
**
**	Exceptions:
**		none.
**
** Side Effects:
**
** History:
**	08-aug-1987 (rdesmond) written.
*/
static
char *
put_ulstr(utype)
char	utype;
{
	if (utype == 'l')
	    return(und_tbl[2]);
	else if (utype == ERx('n'))
	    return(und_tbl[0]);
	else if (utype == ERx('a'))
	    return(und_tbl[1]);
}
/*
** RFOPTCHKFMT - Validate date, time, and pageno format strings.
**
*/
static
i4
rfoptchkfmt(fmt_row,fmt_str)
	i4	fmt_row;
	char	*fmt_str;
{
	i4	ret_value;
	FMT	*fmt_block;
	fmt_block = vfchkFmt(fmt_str);		/* validate format*/
	switch(fmt_row)
	{
	case 1:					/* Date Format	*/
		if  ((fmt_block == NULL) ||
		     ((fmt_block->fmt_type != F_DT) && 
		      (fmt_block->fmt_type != F_C)))
		{
			IIUGerr(E_RF00B1_date_format, UG_ERR_ERROR, 0);
			ret_value = RFO_DF_TYPE;
			break;
		}
		if  (fmt_block->fmt_type == F_C)
		{
			if  (fmt_size(Adf_scb,fmt_block,NULL,&rows,&cols) != OK)
			{
				IIUGerr(E_RF00B4_roptions_cant_get_fmt,
					UG_ERR_ERROR, 0);
				ret_value = RFO_DF_INTERNAL;
				break;
			}
			if  (rows != 1)
			{
				IIUGerr(E_RF00B1_date_format, UG_ERR_ERROR, 0);
				ret_value = RFO_DF_TYPE;
				break;
			}
		}
		/*
		** Reset form FMT variable, and save the FMT
		** and its resultant width in the Report Options
		** global structure.
		*/
		STcopy(fmt_str,&datef[0]);
		Opt.rdate_w_fmt = cols;
		ret_value = RFO_OK;
		break;
	case 2:					/* Time Format	*/
		if  ((fmt_block == NULL) ||
		     ((fmt_block->fmt_type != F_DT) && 
		      (fmt_block->fmt_type != F_C)))
		{
			IIUGerr(E_RF00B2_time_format, UG_ERR_ERROR, 0);
			ret_value = RFO_TF_TYPE;
			break;
		}
		if  (fmt_block->fmt_type == F_C)
		{
			if  (fmt_size(Adf_scb,fmt_block,NULL,&rows,&cols) != OK)
			{
				IIUGerr(E_RF00B4_roptions_cant_get_fmt,
					UG_ERR_ERROR, 0);
				ret_value = RFO_TF_INTERNAL;
				break;
			}
			if  (rows != 1)
			{
				IIUGerr(E_RF00B2_time_format, UG_ERR_ERROR, 0);
				ret_value = RFO_TF_TYPE;
				break;
			}
		}
		/*
		** Reset form FMT variable, and save the FMT
		** and its resultant width in the Report Options
		** global structure.
		*/
		STcopy(fmt_str,&timef[0]);
		Opt.rtime_w_fmt = cols;
		ret_value = RFO_OK;
		break;
	case 3:				/* Page Number Format	*/  
		/*
		** Check for a numeric template or
		** an integer format
		*/
		if  ((fmt_block == NULL)||
		     ((fmt_block->fmt_type != F_NT) && 
		      (fmt_block->fmt_type != F_I)))
		{
			IIUGerr(E_RF00B3_page_format, UG_ERR_ERROR, 0);
			ret_value = RFO_PF_TYPE;
			break;
		}
		if  (fmt_size(Adf_scb, fmt_block, NULL, &rows, &cols) != OK)
		{
			IIUGerr(E_RF00B4_roptions_cant_get_fmt,
				UG_ERR_ERROR, 0);
			ret_value = RFO_PF_INTERNAL;
			break;
		}
		/*
		** Reset form FMT variable, and save the FMT
		** and its resultant width in the Report Options
		** global structure.
		*/
		STcopy(fmt_str,&pagef[0]);
		Opt.rpageno_w_fmt = cols;
		ret_value = RFO_OK;
		break;
	default:
		IIUGerr(E_RF00B4_roptions_cant_get_fmt,
			UG_ERR_ERROR, 0);
		ret_value = RFO_FMT_INTERNAL;
		break;
	}
	return(ret_value);
}
