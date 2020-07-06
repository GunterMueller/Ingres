# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<me.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"rbftype.h"
# include	"rbfglob.h"
# include	<cm.h>
# include	<st.h>
# include	<nm.h>
# include	<ut.h>
# include	<lo.h>
# include	<uf.h>
# include	<ug.h>
# include	<ui.h>
# include	<fedml.h>
# include	<flddesc.h>
# include <ooclass.h>
# include <oocat.h>
# include	<oodefine.h>
# include <uigdata.h>
# include	<errw.h>
# include       <erug.h>
# include       <rcons.h>
/**
** Name:	rfcat.qsc - catalog frame for rbf.
**
** Description:
**	This file defines:
**
**	rfLast		set name of last report edited.
**	rFdetdsply	display report details frame.
**	rFrunrep	cover to call IIRFcrp_Callreport.
**	IIRFcrp_Callreport
**			Gets output options (screen, filname, printer) from
**			user and runs report by calling ReportWriter
**	rFnoop		DUMMY noop for OO routines.
**	rFdestroy	destroy report.
**	IIRFgtn_getname Prompts user for object/file name.
**	IIRFgts_getstyle  
**			Gets report style (tabular, indented, block, labels, 
**			or master/detail) from user.
**
** History:
**	10/15/82 (jn)	written for GBF.
**	10/31/82 (ps)	modified for RBF.
**	5/5/83	 (gac)	now checks for no reports in catalog.
**	7/14/83	 (ncg)	modified to use new table fields,
**			and ate my lunch at Freddy's.
**	2/22/84 (gac)	added mult-statement transactions.
**	1/13/86 (prs)	change FEfehelp call to FEhelp.
**	26-nov-1986 (yamamoto)
**			Modified for double byte characters.
**	27-jul-1987 (rdesmond)
**			Modified for Jupiter.  Uses OO routines.
**	04-jan-1988 (rdesmond)
**			Changed initialize section to handle reports
**			specified on command line correctly.
**	10-feb-1988 (rdesmond)
**			Now sets En_rid to appropriate value before
**			calling rFget() (to avoid unnecessary DB access).
**	18-feb-1988 (rdesmond)
**			Removed range statements.
**	12-jul-88 (sylviap)
**			Changed the QUEL to SQL.
**			Took out the HACKFOR50.
**	22-aug-1988 (danielt)
**			changed rename and destroy activate blocks to re-readin
**			the reports  (a DBA owned report might be uncovered)
**      25-aug-88 (sylviap)
**                      Changed create so it can accept a mixed case table name.
**	1-sep-1988 (danielt)
**			took out MST (OOcatDestroy() takes care
**			of it's own transaction).
**	20-9-1988 (elein)
**			B3376: Check report width before editing. Give error
**			message and disallow it if it is wider that maximum
**			width (-ln or terminal capability).  Also, pass on
**			current value of max width (En_lmax) to report so
**			that report can run reports as wide as rbf can.
**	29-sep-1988 (sylviap)
**			Took out transaction code in rFdestroy.
**	10/4/88 (bobm)	Add "AutoReport" item.
**	10-5-88 (elein)
**		B3626: If rename or destroy fail, don't clear out
**		catalog, just resume.
**	11/1/88 (elein)
**		B3751 -- changed declaration of repflags to correspond with
**		autoreport declaration.  This will make the prompt for the
**		file name a consistent size. The "60" is just leeway--
**		there is nothing magic about it.
**	08-nov-88 (sylviap)
**		Added another paramter, a customized help screen, to the 
**		parameter list calling OOdetail.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**      29-nov-88 (sylviap)
**              Sets En_rid to -1 when before archiving a report.  Fixes bug
**              when archiving multiple reports, first report archived is
**              repeatedly archived, regardless of the report chosen.
**	20-dec-88 (sylviap)
**		Performance changes - reads in rfiidet.frm.
**	13-jul-89 (sylviap)
**		Added Direct Printing in rFrunrep.
**	27-jul-89 (sylviap)
**		Bug 7209 - For default reports, after rFget, if En_lmax is 
**		smaller than the oldEn_lmax, reset En_lmax back to the old max.
**	10-aug-89 (sylviap)
**		Added Popups for AutoReport and Create menu options.
**	9/22/89 (elein) UG changes ingresug change #90045
**		changed <rbftype.h> & <rbfglob.h> to "rbftype.h" & "rbfglob.h"
**	9/25/89 (martym)
**		(GARNET) Added support for JoinDefs.
**	03-sep-89 (cmr ) fix for bug 8149
**		Change the singleton select to a select loop because
**		there may be more than one row for the FROM clause.
**	11/27/89 (martym) - Added support for RBF report template styles.
**	12/12/89 (elein)
**		*Substantial* changes to Go/AutoReport and Create and
**		routines used by these functions in order to 
**		implement Listpick and Listpick handlers.
**      18-dec-89 (sylviap)
**              Added Batch calls and batch report pop-ups.
**	12/27/89 (elein)
**		Added special case for RBF called from ABF.  Updated
**		error messages (lots) that were numbered wrong
**	1/4/90 (elein) again!
**		Modified RBF called from ABF to use a real form
**		instead of a default form.  Moved calls to do new edit
**		to gtn and gts make error recovery better.
**      09-jan-90 (sylviap)
**		If in batch mode, does not print executing msgs ("Sending report
**		  to file 'report.out'").
**		If non-batch report, does not display declared variables screen.
**      15-jan-90 (sylviap)
**		Added Help files to the batch popups.
**	1/23/90 (martym)
**		Changed to check return code from rFdisplay() to detect when 
**		En_lmax is exceeded for Indented and Master/Detail report 
**		styles.
**      30-jan-90 (sylviap)
**		Fixed bug w/non-batch report so it  does not display declared 
**		variables screen.
**	2/7/90 (martym)
**		Coding standards cleanup.
**      15-feb-90 (sylviap)
**		Initializes val_str in IIRFgdv_GetDeclaredVars. US #573 & #575.
**      22-feb-90 (sylviap)
**		Took out y/n abbreviations in the batch print and file popups.
**	03/01/90 (dkh) - Changed to use IIUIgdata->prompt_func instead of
**			 IIUFprompt() directly.
**      19-mar-90 (sylviap)
**		Only calls IIxflag if not batch report.
**      20-mar-90 (sylviap)
**		Fixed SIprintf.  Calls SIflush right after. Fixes bug on VMS.
**      28-mar-90 (sylviap)
**		If rbf is run w/o the -l flag, RBF can create a wide default
**		report with no error messages.  If w/the -l, RBF will wrap, 
**		displaying error message report is wrapping to stay within 
**		the -l boundaries.  RBF will never allow a label style report 
**		to wrap. (#396, #709)
**      02-apr-90 (sylviap)
**		New -l behavior.  If called w/-l flag, will now out up pop-up
**		(IIRFwid_WideReport) prompting user to either continue or
**		select another style.
**	4/3/90 (martym)
**		Added a parameter to call to IIRFwid_WideReport().
**      03-apr-90 (sylviap)
**		Fixed bug #788 - full prompt for declared variables.
**      11-apr-90 (sylviap)
**		Will pass NULL as the output file (location) to UTexe if not 
**		in batch mode.  Necessary because CL checks if should re-direct
**		output depending upon if the LOCATION is NULL or not.
**	4/11/90 (martym)
**              Fixed JUPBUG #4468. When using '-e' flag, and a wild
**              card is used to retrieve report names, will not allow
**              the user to trash the object names in the RBF catalog.	
**      16-apr-90 (sylviap)
**		Saves En_lmax when creating/editting a report and then resets 
**		it back since it's now possible to create a wide report. 
**      18-apr-90 (sylviap)
**		Will only send -l flag to RW if set on the command line. (#847)
**		Created the Archive pop-up.  Calls IIRFarp_ArchiveReport. (#243)
**	6/18/90 (martym)
**		Changed "rFdetdsply()" to display the name of the data source 
**		for the report correctlly for case(s) when it is not a single 
**		table (such as a JoinDef for now).
**	6/21/90 (martym)
**		Changed to use E_RF0090 & E_RF0091.
**      17-aug-90 (sylviap)
**		Uses global En_lmax_orig rather than old_En_lmax. En_lmax_orig
**		is needed for label style reports in rfframe.c.
**	27-aug-90 (cmr/esd/sylviap) Integration of VM Porting changes
**		For CMS call TOwsnme() to convert 'file ext' to 'file.ext'
**		before writing the report to the file (Preview/Go).
**	28-aug-1990 (Joe)
**	    	Changed references to IIUIgdata to the UIDBDATA structure
**	    	returned from IIUIdbdata().
**	04-sep-90 (sylviap)
**		Changed call to UTexe so the user is passed through the 'user='.
**		Now username will be portable to a platform that is case 
**		sensitive.
**	04-sep-90 (sylviap)
**		Added explanations to the activate menuitems. #32699
**	10-jan-91 (steveh)
**		Fixed bug 35177 in which duplication of a report incorrectly
**		set the right-hand margin of the new report.
**	08-mar-91 (steveh)
**		Fixed bug 34587 in which selecting the `default' printer
**		actually attempted to print to the printer called "default."
**	15-mar-91 (sylviap)
**		Modified IIRFgfn_GetFilename and IIRFgpr_GetPrinter so the
**		"batch" prompt is invisible for those platforms that cannot
**		implement concurrent reports.  RBF checks if
**		(UT_WAIT == UT_NOWAIT).  I changed the forms rffile and rfprint.
**		I made the line "Wait for report" a field rather than trim so it
**		could have the "invisible" attribute on platforms that do not
**		need the "wait for completion" prompt.
**		I also changed the "instuctions" from trim to a field.  At 
**		runtime, RBF can change the instructions to reflect if there is
**		a "wait" prompt or not.
**	03/28/91 (dkh) - Replaced old inquire_frs syntax with new.
**	04/17/91 (steveh) - Fixed bug 36997 in which default report widths
**		were incorrectly detemined.
**      05/20/91 (steveh) - Modified previous fix to bug 36997 in which
**            	default report widths were incorrectly detemined.
**            	Set return type for rfLast(), IIRFarp_ArchiveReport(),
**            	rfabfcr(), IIRFgfn_GetFilename() and IIRFgpr_GetPrinter().
**		This also corrects bugs 37574 and 37671.
**	28-jun-91 (kirke)
**		Added fnamefld initialization to IIRFarp_ArchiveReport().
**      07/02/91 (steveh) Fixed declaration of IIOOidCheck.  Added missing
**		final argument to call to IIOOidCheck. (bug 38996)
**	07/02/91 (steveh) Removed unused variables.
**	08/12/91 (steveh) Fixed bug 38434 where RBF failed to pause for
**		the user to read any REPORT messages if REPORT failed.
**	17-aug-91 (leighb) DeskTop Porting Change:
**		Set value of IituMainform via a function to avoid data
**		references across facilities (for DLL's on IBM/PC).
**	12/31/91 (steveh) rFrunrep now properly returns status.  Lack of
**		the proper status caused the caller to use the garbage
**		value on the stack and caused spurious returns to the Report
**		Catalog.  This fixes bug 39872.
**	04/06/92 (rdrane) 41180
**		When searching for the name of the object upon which a report is
**		based (assuming it's not a single table), set rcosequence to a
**		numeric "1" rather than a string (varchar) "1".  Some gateways
**		see the string representation and do not effect a conversion
**		to numeric, which is the defined format for the column.
**	31-aug-1992 (rdrane)
**		Prelim fixes for 6.5 - change over from IIUGlbo_lowerBEobject()
**		and FErelexists() for FErel_ffetch().
**	7-oct-1992 (rdrane)
**		Use new constants in (s/r)_reset invocations.
**	12/05/92 (dkh) - Added Choose Columns support for rbf.
**	12/14/92 (dkh) - Changed to use set_frs interface instead of special
**			 entry point to change number of rows of a table field.
**	12/18/92 (dkh) - Added check to eliminate unsupported datatypes
**			 from the ChooseColumns popup display.
**	05-jan-93 (sylviap)
**		Added a new menuitem 'Edit' to allow editing/creating of 
**		joindefs from rbf.  Also changed the behavior to display an 
**		error when creating a report.  If no report/table/joindef name 
**		is entered in the 'create popup', then an error is displayed.
**		This behavior is consistent with vifred's.
**		Changed ChooseColumns menuitem only comes up for tables popup.
**	21-jan-1993 (rdrane)
**		Rework the ChooseColumns support to follow 6.5 protocols for
**		owner.tablename and delimited identifiers.
**		Removed external declaratiions which have been moved into
**		included header files.  Redeclared other routines as being
**		static.  This required that the statics preceed the GLOBALDEFs
**		since one of the initializers is a static routine. Replaced
**		instances of '\0' with EOS.
**	10-feb-1993 (rdrane)
**		Have IIRFgtn_getname() check for initial specification of owner
**		and/or tablename -- ChooseColumn was failing if selected
**		immediately after specifiing a table name for the first time
**		after utility invocation. 
**	11-mar-1993 (rdrane)
**		Have rFdetdsply() use the r_g_ident() routine to extract the
**		table name from the rcotext.  This prevents it from being
**		confused by embedded spaces or commas in delimited identifiers.
**	24-mar-1993 (lan)
**		Fixed bug 49656 where RBF did not return an error when
**		REPORT failed due to a machine resource limitation.  Report
**		Operating System level error messages if available.
**	15-Apr-93 (fredb) hp9_mpe
**		Porting changes/fixes to IIRFcrp_Callreport. Bug #50554.
**		Pass a LOCATION containing the filename, "$STDLIST" to UTexe
**		as the err_log parameter to prevent i/o from going into the
**		bit bucket when in interactive mode.
**	7-jun-1993 (rdrane)
**		Call r_g_skip() before calling r_g_ident(0 in rFdetdsply().
**		Nothing was being returned if rcotext had leading white space
**		(bug 52532).
**	7-jul-1993 (rdrane)
**		Fix-up all IIUGerr() invocations to use UG_ERR_ERROR or
**		UG_ERR_FATAL #define instead of hard-coded constant.
**	4-jan-1994 (rdrane)
**		Check dlm_Case capability directly to determine if
**		delimited identifiers are suppported, rather than
**		trusting the StandardCatalogLevel.
**      21-jun-1995 (harpa06)
**              Convert tables names given into it's BE representation before 
**              continuing to process the table so that delimited table name 
**              output is correct.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-Nov-1999 (hanch04)
**          First stage of updating ME calls to use OS memory management.
**          Make sure me.h is included.  Use SIZE_TYPE for lengths.
[@history_template@]...
**/
/* static's */
static		VOID    IIRFarp_ArchiveReport();
static		VOID	IIRFccChooseColumns();
static		i4	IIRFcrCreateReport();
static		STATUS	IIRFcrp_Callreport();
static		i4	IIRFdne_DoNewEdit();
static		VOID    IIRFgfn_GetFilename();
static		VOID    IIRFgpr_GetPrinter();
static		i4	IIRFgtn_getname();
static		i4	IIRFgts_getstyle();
static		i4	IIRFgtd_getdest();
static		char    *IIRFmps_MakeParams();
static		char    *IIRFgdv_GetDeclaredVars();
static		STATUS  IIRFSetReadOnly();
static		VOID    rfabfcr();
static		STATUS	rFdestroy();
static		STATUS	rFdetdsply();
static		STATUS	rFnoop();
static		STATUS	rFrunrep();
static	char	Lastname[(FE_MAXNAME + 1)] = ERx("");
static	bool	Lastdifferent = FALSE;
static	bool	Lastsaved     = FALSE;
static	bool	name_frm = FALSE;	  /* TRUE= name frame initialized */ 
static	char	oldlmax_str[20] = ERx("");	  /*B3376*/
static	char	lmax_str[20] = ERx("");		  /*B3376*/
static	bool	AutoReport = FALSE;
static	bool	haveccform = FALSE;
static	bool	haveacform = FALSE;
static	char	ccol_name[(FE_MAXNAME + 1)] = ERx("");
static	char	ccol_owner[(FE_MAXNAME + 1)] = ERx("");
static	char	ob_ndest[(FE_MAXNAME + 1)] = ERx("");
static	char	ob_odest[(FE_MAXNAME + 1)] = ERx("");
static	RFCOLDESC *del_cols = (RFCOLDESC *)NULL;
/* extern's */
FUNC_EXTERN	VOID    rfLast();
FUNC_EXTERN	STATUS	IIRFwid_WideReport();
FUNC_EXTERN	STATUS	IIOOidCheck();
FUNC_EXTERN	char	*IITUtpTblPik();
FUNC_EXTERN	char	*IIUFopObjPik();
FUNC_EXTERN	char	*IIRFfwd_FullWord();
FUNC_EXTERN	char	*IIxflag();
GLOBALREF       char    *Nrdetails;
GLOBALREF       bool    alloc_detail;
/* GLOBALDEF's */
GLOBALDEF	OO_CATREC	rfoocatrec =
{
	0, 0, ERx(""), ERx(""),
	0, 0, ERx(""),
	ERx(""), ERx(""),
	ERx(""), 0, ERx("")
};
GLOBALDEF 	MENUPARAM	rf_menup[] =
{
	{ NULL, 4, rFrunrep },
	{ NULL, 0, NULL },
};
GLOBALDEF	RFCOLDESC	*IIRFcollist = NULL;
GLOBALDEF	i4		IIRFcccount = 0;
GLOBALDEF	i4		IIRFcdcount = 0;
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _EndExpl	F_FE0102_ExplEnd
#define _FindExpl	F_FE0104_ExplFind
#define _TopExpl	F_FE0105_ExplTop
#define _BottomExpl	F_FE0106_ExplBottom
#define _MoreInfoExpl	F_FE0107_ExplMoreInfo
# define	REP_LEN		FE_MAXNAME+FE_PROMPTSIZE+60
# define        WANT_PROMPT     0
# define        WANT_QUOTE      1
# define        FOUND_PROMPT    2
# define        BUF_SIZE        256  
# define	ROW_DELETED	4
# define	CHOOSE_MAX_ROWS	10
# define	ADD_MAX_ROWS	5
/*{
** Name:	IIRFacAddColumn - Add back a deleted column name.
**
** Description:
**	Allow user to add back a previously deleted column.  Deleted
**	columns are obtained from the "del_cols" array.  On exit
**	due to a "Select", the remaining columns are loaded back
**	into "del_cols".
**
** Inputs:
**
** Outputs:
**	aname		If return value is TRUE, contains name of column that
**			was selected.
**	alen		If return value is TRUE, contains datatype length of
**			column that was selected.
**	adt		If return value is TRUE, contains datatype of column
**			that was selected.
**	aprec		If return value is TRUE, contains precision of
**			column that was selected.
**
**	Returns:
**		TRUE	If a deleted column was selected.
**		FALSE	If user selected "Cancel".
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	12/06/92 (dkh) - Initial version.
*/
bool
IIRFacAddColumn(aname, alen, adt, aprec)
  char *aname;
  i4 *alen;
  i2 *adt;
  i2 *aprec;
{
	i4		i;
	RFCOLDESC	*coldesc;
	bool		retval = TRUE;
  char xname[FE_MAXNAME + 1];
  i4 xlen;
  i2 xdt;
  i2 xprec;
  i4 rstate;
	i4		tblrows;
	/*
	**  Check to make sure that the rfadd_columns
	**  form is available.
	*/
	if (!haveacform)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(),
			ERx("rfadd_columns")) != OK)
		{
			IIUGerr(E_RF002D_Cannot_locate_form,
				UG_ERR_ERROR, 1, ERx("rfadd_columns"));
			return(FALSE);
		}
		haveacform = TRUE;
	}
	/* Adjust the size of the table field and form. */
	if (IIRFcdcount > ADD_MAX_ROWS)
	{
		tblrows = ADD_MAX_ROWS;
	}
	else
	{
		tblrows = IIRFcdcount;
	}
	/*
	**  Make call similar to a ## set_frs to changed the number
	**  rows for a table field.
	*/
	if (IIiqset((int) 3, (int) 0, ERx("rfadd_columns"), (char *) 0,
		(char *) 0) != (int) 0)
	{
		IIstfsio((int)30, ERx("colnames"), (int)0, (short *) 0,
			(int) 1, (int)30, (int)4, &tblrows);
	}
/* # line 503 "rfcat.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"rfadd_columns",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 506 "rfcat.qsc" */	/* host code */
		/* Load the table field with the deleted column list */
/* # line 507 "rfcat.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"rfadd_columns",(char *)"colnames",(char *)"\
r") != 0) {
          IIthidecol((char *)"len",(char *)"i4");
          IIthidecol((char *)"dt",(char *)"i2");
          IIthidecol((char *)"prec",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 509 "rfcat.qsc" */	/* host code */
		for (i = 0, coldesc = del_cols; i < IIRFcdcount;
			i++, coldesc++)
		{
			STcopy(coldesc->name, xname);
			xlen = coldesc->length;
			xdt = coldesc->datatype;
			xprec = coldesc->prec;
/* # line 516 "rfcat.qsc" */	/* loadtable */
      {
        if (IItbact((char *)"rfadd_columns",(char *)"colnames",1) != 0) {
          IItcoputio((char *)"colnames",(short *)0,1,32,0,xname);
          IItcoputio((char *)"len",(short *)0,1,30,4,&xlen);
          IItcoputio((char *)"dt",(short *)0,1,30,2,&xdt);
          IItcoputio((char *)"prec",(short *)0,1,30,2,&xprec);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 518 "rfcat.qsc" */	/* host code */
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 523 "rfcat.qsc" */	/* host code */
		/* Get values for the row that has been selected */
/* # line 524 "rfcat.qsc" */	/* getrow */
          {
            if (IItbsetio(2,(char *)"rfadd_columns",(char *)"colnames",-2) != 
            0) {
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,xname,(char *)
"colnames");
              IItcogetio((short *)0,1,30,4,&xlen,(char *)"len");
              IItcogetio((short *)0,1,30,2,&xdt,(char *)"dt");
              IItcogetio((short *)0,1,30,2,&xprec,(char *)"prec");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 526 "rfcat.qsc" */	/* host code */
		STcopy(xname, aname);
		*adt = xdt;
		*alen = xlen;
		*aprec = xprec;
		/*
		**  Throw away the selected row so it will be ignore in the
		**  unloadtable loop below.
		*/
/* # line 535 "rfcat.qsc" */	/* deleterow */
          {
            if (IItbsetio(4,(char *)"rfadd_columns",(char *)"colnames",-2) != 
            0) {
              if (IItdelrow(0) != 0) {
              } /* IItdelrow */
            } /* IItbsetio */
          }
/* # line 537 "rfcat.qsc" */	/* host code */
		/*
		**  Store the remaining deleted columns in the
		**  del_cols array.
		*/
		coldesc = del_cols;
/* # line 543 "rfcat.qsc" */	/* unloadtable */
          {
            if (IItbact((char *)"rfadd_columns",(char *)"colnames",0) == 0) 
            goto IItbE1;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,xname,(char *)
"colnames");
              IItcogetio((short *)0,1,30,4,&xlen,(char *)"len");
              IItcogetio((short *)0,1,30,2,&xdt,(char *)"dt");
              IItcogetio((short *)0,1,30,2,&xprec,(char *)"prec");
              IItcogetio((short *)0,1,30,4,&rstate,(char *)"_STATE");
              IITBceColEnd();
              {
/* # line 547 "rfcat.qsc" */	/* host code */
			if (rstate != ROW_DELETED)
			{
				STcopy(xname, coldesc->name);
				coldesc->datatype = xdt;
				coldesc->length = xlen;
				coldesc->prec = xprec;
				coldesc->deleted = TRUE;
				coldesc++;
			}
              }
            } /* IItunload */
IItbE1:
            IItunend();
          }
/* # line 558 "rfcat.qsc" */	/* host code */
		IIRFcdcount--;
/* # line 559 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 564 "rfcat.qsc" */	/* host code */
		retval = FALSE;
/* # line 565 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 570 "rfcat.qsc" */	/* host code */
		FEhelp(ERx("rfccadd.hlp"), ERget(S_RF00A7_ChCol_Add));
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
    if (IInmuact(ERget(FE_Select),ERget(F_FE0108_ExplSelect),2,2,1) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 573 "rfcat.qsc" */	/* host code */
	return(retval);
}
/*{
** Name:	IIRFccChooseColumns - Allow user to select which columns
**				      to include in the default report.
**
** Description:
**	This routine allows the user to edit the list of columns that
**	will be used in the default report.  User may delete, put back
**	and reorder the columns.
**
** Inputs:
**	name	Name of table to list columns for.  Expected to be fully
**		normalized.
**	owner	Owner of table to list columns for.  Expected to be fully
**		normalized.
**
** Outputs:
**	None.
**
**	Returns:
**		None.
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	12/09/92 (dkh) - Initial version.
**	21-jan-1993 (rdrane)
**		Rework to follow 6.5 protocols for owner.tablename and
**		delimited identifiers.  This primarily involved adding the
**		owner parameter.  Disambiguate failed memory allocation
**		message text.  Make routine static.
**	13-apr-1993 (rdrane)
**		Ensure that we don't try and delete columns after they've
**		all been deleted (bugs 50683, 50685, and 50761)
**	7-jul-1993 (rdrane)
**		Put out the unsupported datatype message for any
**		disqualified columns whenever we initially set-up the
**		list.
*/
static
VOID
IIRFccChooseColumns(name,owner)
char	*name;
char	*owner;
{
	FE_ATT_QBLK	qblk;
	FE_ATT_INFO	attinfo;
	RFCOLDESC	*coldesc;
	RFCOLDESC	*dcoldesc;
	RFCOLDESC	*prev;
	i4		count = 0;
	i4		i;
	DB_DATA_VALUE	dbv;
  i4 rowstate;
  i4 delrownum;
  i4 row;
  i4 db_len;
  i2 db_dt;
  i2 db_prec;
  char cname[FE_MAXNAME + 1];
	i4		orig_count;
	i4		orig_dcount;
	i4		tblrows;
	char		dsply_name[(FE_UNRML_MAXNAME + 1)];
	/*
	**  Allocate the column descriptor arrays if they
	**  have not been already.
	*/
	if (IIRFcollist == NULL)
	{
		if ((IIRFcollist = (RFCOLDESC *) MEreqmem((u_i4) 0,
			(SIZE_TYPE)(sizeof(RFCOLDESC) * DB_GW2_MAX_COLS), FALSE,
			(STATUS *) NULL)) == NULL)
		{
			/* This does not return */
			IIUGbmaBadMemoryAllocation(
				ERx("IIRFccChooseColumns - IIRFcollist"));
		}
		if ((del_cols = (RFCOLDESC *) MEreqmem((u_i4) 0,
			(SIZE_TYPE)(sizeof(RFCOLDESC) * DB_GW2_MAX_COLS), FALSE,
			(STATUS *) NULL)) == NULL)
		{
			/* This does not return */
			IIUGbmaBadMemoryAllocation(
				ERx("IIRFccChooseColumns - del_cols"));
		}
	}
	coldesc = IIRFcollist;
	/*
	**  Save count information so that we can store state on
	**  a cancel.  Note that the array pointed to by IIRFcollist
	**  is not changed until the user selects OK.  This makes
	**  it easy to do a cancel.
	*/
	orig_count = IIRFcccount;
	orig_dcount = IIRFcdcount;
	/*
	**  If needed, get the column names for a table.  Only necessary
	**  if we have a change in table name or this is first time
	**  here in the change of events to create a report based on a
	**  table.
	**
	**  We've already done a full resolve call earlier, so there
	**  is no need to check the return value from FEatt_open().
	*/
	if (IIRFcccount == 0)
	{
		_VOID_ FEatt_open(&qblk, name, owner);
		/*
		**  Grab the column names and store it
		**  in the IIRFcollist array.
		*/
		while(FEatt_fetch(&qblk, &attinfo) == OK)
		{
			FEatt_dbdata(&attinfo, &dbv);
			/*
			**  Skip any unsupported datatypes.
			*/
			if (!IIAFfedatatype(&dbv))
			{
				_VOID_ IIUGxri_id(&attinfo.column_name[0],
						  &dsply_name[0]);
				IIUGerr(E_RW1414_ignored_attrib,
					UG_ERR_ERROR,1,&dsply_name[0]);
				continue;
			}
			coldesc->datatype = dbv.db_datatype;
			coldesc->length = dbv.db_length;
			coldesc->prec = dbv.db_prec;
			coldesc->deleted = FALSE;
			STcopy(attinfo.column_name, coldesc->name);
			coldesc++;
			count++;
		}
		_VOID_ FEatt_close(&qblk);
		/*
		**  Set the total number of columns that were found.
		*/
		if (count == 0)
		{
			_VOID_ IIUGxri_id(name,&dsply_name[0]);
			IIUGerr(E_RF00B5_NoSuppCols,
				UG_ERR_ERROR,1,&dsply_name[0]);
			return;
		}
		IIRFcccount = count;
		IIRFcdcount = 0;
	}
	else
	{
		/*
		**  Use existing count from a previous pass through here.
		*/
		count = IIRFcccount;
	}
	/*  Adjust size of table field and form. */
	if (IIRFcccount - IIRFcdcount > CHOOSE_MAX_ROWS)
	{
		tblrows = CHOOSE_MAX_ROWS;
	}
	else
	{
		tblrows = IIRFcccount - IIRFcdcount;
	}
	/*
	**  Make call similar to a ## set_frs to changed the number
	**  rows for a table field.
	*/
	if (IIiqset((int) 3, (int) 0, ERx("rfchoose_columns"), (char *) 0,
		(char *) 0) != (int) 0)
	{
		IIstfsio((int)30, ERx("colnames"), (int)0, (short *) 0,
			(int) 1, (int)30, (int)4, &tblrows);
	}
/* # line 767 "rfcat.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"rfchoose_columns",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 770 "rfcat.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"rfchoose_columns",(char *)"colnames",(char *)"\
r") != 0) {
          IIthidecol((char *)"len",(char *)"i4");
          IIthidecol((char *)"dt",(char *)"i2");
          IIthidecol((char *)"prec",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 773 "rfcat.qsc" */	/* host code */
		dcoldesc = del_cols;
		for (i = 0, coldesc = IIRFcollist; i < count; i++, coldesc++)
		{
			db_dt = coldesc->datatype;
			db_len = coldesc->length;
			db_prec = coldesc->prec;
			STcopy(coldesc->name, cname);
			if (coldesc->deleted)
			{
				STcopy(cname, dcoldesc->name);
				dcoldesc->datatype = db_dt;
				dcoldesc->length = db_len;
				dcoldesc->prec = db_prec;
				dcoldesc->deleted = TRUE;
				dcoldesc++;
			}
			else
			{
/* # line 793 "rfcat.qsc" */	/* loadtable */
      {
        if (IItbact((char *)"rfchoose_columns",(char *)"colnames",1) != 0) {
          IItcoputio((char *)"colnames",(short *)0,1,32,0,cname);
          IItcoputio((char *)"len",(short *)0,1,30,4,&db_len);
          IItcoputio((char *)"dt",(short *)0,1,30,2,&db_dt);
          IItcoputio((char *)"prec",(short *)0,1,30,2,&db_prec);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 796 "rfcat.qsc" */	/* host code */
			}
		}
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 801 "rfcat.qsc" */	/* host code */
		/*
		**  Check to make sure that there is at least one column
		**  left.  If everything has been deleted, issue error
		**  and resume.
		*/
		if (IIRFcccount == IIRFcdcount)
		{
			IIUGerr(E_RF00A8_NoColsLeft, UG_ERR_ERROR, 0);
/* # line 809 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 810 "rfcat.qsc" */	/* host code */
		}
		/*
		**  Unload the values.
		*/
		coldesc = IIRFcollist;
/* # line 816 "rfcat.qsc" */	/* unloadtable */
          {
            if (IItbact((char *)"rfchoose_columns",(char *)"colnames",0) == 0
            ) goto IItbE2;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,cname,(char *)
"colnames");
              IItcogetio((short *)0,1,30,4,&db_len,(char *)"len");
              IItcogetio((short *)0,1,30,2,&db_dt,(char *)"dt");
              IItcogetio((short *)0,1,30,2,&db_prec,(char *)"prec");
              IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
              IITBceColEnd();
              {
/* # line 820 "rfcat.qsc" */	/* host code */
			/*
			**  Save away columns that are still part of
			**  the report.
			*/
			if (rowstate != ROW_DELETED)
			{
				STcopy(cname, coldesc->name);
				coldesc->datatype = db_dt;
				coldesc->length = db_len;
				coldesc->prec = db_prec;
				coldesc->deleted = FALSE;
				coldesc++;
			}
              }
            } /* IItunload */
IItbE2:
            IItunend();
          }
/* # line 836 "rfcat.qsc" */	/* host code */
		if (IIRFcdcount != 0)
		{
			dcoldesc = del_cols;
			for (i = 0; i < IIRFcdcount; i++)
			{
				STcopy(dcoldesc->name, coldesc->name);
				coldesc->datatype = dcoldesc->datatype;
				coldesc->length = dcoldesc->length;
				coldesc->prec = dcoldesc->prec;
				coldesc->deleted = TRUE;
				coldesc++;
				dcoldesc++;
			}
		}
/* # line 874 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 878 "rfcat.qsc" */	/* host code */
		/*
		**  Nothing to add if user has not deleted anything.
		*/
		if (IIRFcdcount == 0)
		{
			IIUGerr(E_RF00A4_No_Del_Cols, UG_ERR_ERROR, 0);
/* # line 884 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 885 "rfcat.qsc" */	/* host code */
		}
		/*
		**  Call routine to allow user to add backa deleted column.
		*/
		if (IIRFacAddColumn(cname, &db_len, &db_dt, &db_prec))
		{
/* # line 892 "rfcat.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,(char *)"rfchoose_columns",(char *)0, (char *)0) 
            != 0) {
              IIiqfsio((short *)0,1,30,4,&row,29,(char *)"colnames",0);
            } /* IIiqset */
          }
/* # line 895 "rfcat.qsc" */	/* host code */
			/*
			**  Add column back.  Decrementing "row" puts
			**  the column ABOVE the current row.
			*/
			row--;
/* # line 900 "rfcat.qsc" */	/* insertrow */
          {
            if (IItbsetio(5,(char *)"rfchoose_columns",(char *)"colnam\
es",row) != 0) {
              if (IItinsert() != 0) {
                IItcoputio((char *)"colnames",(short *)0,1,32,0,cname);
                IItcoputio((char *)"len",(short *)0,1,30,4,&db_len);
                IItcoputio((char *)"dt",(short *)0,1,30,2,&db_dt);
                IItcoputio((char *)"prec",(short *)0,1,30,2,&db_prec);
                IITBceColEnd();
              } /* IItinsert */
            } /* IItbsetio */
          }
/* # line 908 "rfcat.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 912 "rfcat.qsc" */	/* host code */
		if (IIRFcccount == IIRFcdcount)
		{
			IIUGerr(E_RF00A8_NoColsLeft, UG_ERR_ERROR, 0);
/* # line 915 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 916 "rfcat.qsc" */	/* host code */
		}
		/*
		**  Get column being deleted.
		*/
/* # line 920 "rfcat.qsc" */	/* getrow */
          {
            if (IItbsetio(2,(char *)"rfchoose_columns",(char *)"colnam\
es",-2) != 0) {
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,cname,(char *)
"colnames");
              IItcogetio((short *)0,1,30,4,&db_len,(char *)"len");
              IItcogetio((short *)0,1,30,2,&db_prec,(char *)"prec");
              IItcogetio((short *)0,1,30,2,&db_dt,(char *)"dt");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 923 "rfcat.qsc" */	/* host code */
		/*
		**  Delete it from the table field.
		*/
/* # line 926 "rfcat.qsc" */	/* deleterow */
          {
            if (IItbsetio(4,(char *)"rfchoose_columns",(char *)"colnam\
es",-2) != 0) {
              if (IItdelrow(0) != 0) {
              } /* IItdelrow */
            } /* IItbsetio */
          }
/* # line 928 "rfcat.qsc" */	/* host code */
		/*
		**  Save the column away so it can be brought back
		**  via the "Add" menuitem.
		*/
		coldesc = &(del_cols[IIRFcdcount]);
		STcopy(cname, coldesc->name);
		coldesc->length = db_len;
		coldesc->datatype = db_dt;
		coldesc->prec = db_prec;
		coldesc->deleted = TRUE;
		IIRFcdcount++;
        }
      } else if (IIretval() == 4) {
        {
/* # line 944 "rfcat.qsc" */	/* getrow */
          {
            if (IItbsetio(2,(char *)"rfchoose_columns",(char *)"colnam\
es",-2) != 0) {
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,cname,(char *)
"colnames");
              IItcogetio((short *)0,1,30,4,&db_len,(char *)"len");
              IItcogetio((short *)0,1,30,2,&db_dt,(char *)"dt");
              IItcogetio((short *)0,1,30,2,&db_prec,(char *)"prec");
              IItcogetio((short *)0,1,30,4,&delrownum,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 947 "rfcat.qsc" */	/* host code */
		/*
		**  Highlight the selected row.
		*/
/* # line 950 "rfcat.qsc" */	/* unloadtable */
          {
            if (IItbact((char *)"rfchoose_columns",(char *)"colnames",0) == 0
            ) goto IItbE3;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,30,4,&row,(char *)"_RECORD");
              IITBceColEnd();
              {
/* # line 952 "rfcat.qsc" */	/* host code */
			if (row == delrownum)
			{
/* # line 954 "rfcat.qsc" */	/* set_frs */
                {
                  if (IIiqset(5,0,(char *)"rfchoose_columns",(char *)
"colnames",(char *)0) != 0) {
                    IIstfsio(18,(char *)"colnames",0,(short *)0,1,30,sizeof(1)
                    ,(void *)IILQint(1));
                  } /* IIiqset */
                }
/* # line 956 "rfcat.qsc" */	/* endloop */
                {
                  if (1) goto IItbE3;
                }
/* # line 957 "rfcat.qsc" */	/* host code */
			}
              }
            } /* IItunload */
IItbE3:
            IItunend();
          }
/* # line 960 "rfcat.qsc" */	/* display */
          {
            if (IInestmu() == 0) goto IIfdE3;
            goto IImuI3;
IIfdI3:;
IIfdB3:
            while (IIrunnest() != 0) {
              if (IIretval() == 1) {
                {
/* # line 964 "rfcat.qsc" */	/* host code */
			/*
			**  If cursor is on same row as the one selected
			**  for move, then there is nothing to do.  We
			**  Just unhilight the row and breakout
			*/
/* # line 969 "rfcat.qsc" */	/* getrow */
                  {
                    if (IItbsetio(2,(char *)"rfchoose_columns",(char *)
"colnames",-2) != 0) {
                      IItcogetio((short *)0,1,30,4,&row,(char *)"_RECO\
RD");
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
/* # line 970 "rfcat.qsc" */	/* host code */
			if (row == delrownum)
			{
/* # line 972 "rfcat.qsc" */	/* unloadtable */
                  {
                    if (IItbact((char *)"rfchoose_columns",(char *)
"colnames",0) == 0) goto IItbE4;
                    while (IItunload() != 0) {
                      IItcogetio((short *)0,1,30,4,&row,(char *)"_RECO\
RD");
                      IITBceColEnd();
                      {
/* # line 975 "rfcat.qsc" */	/* host code */
					if (row == delrownum)
					{
/* # line 977 "rfcat.qsc" */	/* set_frs */
                        {
                          if (IIiqset(5,0,(char *)"rfchoose_columns",(char *)"\
colnames",(char *)0) != 0) {
                            IIstfsio(14,(char *)"colnames",0,(short *)0,1,30,
                            sizeof(1),(void *)IILQint(1));
                          } /* IIiqset */
                        }
/* # line 980 "rfcat.qsc" */	/* endloop */
                        {
                          if (1) goto IItbE4;
                        }
/* # line 981 "rfcat.qsc" */	/* host code */
					}
                      }
                    } /* IItunload */
IItbE4:
                    IItunend();
                  }
/* # line 983 "rfcat.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE3;
                  }
/* # line 984 "rfcat.qsc" */	/* host code */
			}
/* # line 985 "rfcat.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,(char *)"rfchoose_columns",(char *)
"colnames",-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,delrownum) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 986 "rfcat.qsc" */	/* deleterow */
                  {
                    if (IItbsetio(4,(char *)"rfchoose_columns",(char *)
"colnames",-2) != 0) {
                      if (IItdelrow(0) != 0) {
                      } /* IItdelrow */
                    } /* IItbsetio */
                  }
/* # line 988 "rfcat.qsc" */	/* host code */
			/*
			**  If the row to move comes before the destination,
			**  then we must decrement the destination row number
			**  to account for the deleted row.
			*/
			if (delrownum < row)
			{
				row--;
			}
/* # line 997 "rfcat.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,(char *)"rfchoose_columns",(char *)
"colnames",-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,row) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 998 "rfcat.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(3,0,(char *)"rfchoose_columns",(char *)0, 
                    (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&row,29,(char *)"coln\
ames",0);
                    } /* IIiqset */
                  }
/* # line 1000 "rfcat.qsc" */	/* host code */
			/*
			**  Decrement so that we put the row being moved
			**  above the target.
			*/
			row--;
/* # line 1005 "rfcat.qsc" */	/* insertrow */
                  {
                    if (IItbsetio(5,(char *)"rfchoose_columns",(char *)
"colnames",row) != 0) {
                      if (IItinsert() != 0) {
                        IItcoputio((char *)"colnames",(short *)0,1,32,0,cname);
                        IItcoputio((char *)"len",(short *)0,1,30,4,&db_len);
                        IItcoputio((char *)"dt",(short *)0,1,30,2,&db_dt);
                        IItcoputio((char *)"prec",(short *)0,1,30,2,&db_prec);
                        IITBceColEnd();
                      } /* IItinsert */
                    } /* IItbsetio */
                  }
/* # line 1008 "rfcat.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE3;
                  }
                }
              } else if (IIretval() == 2) {
                {
/* # line 1013 "rfcat.qsc" */	/* host code */
			/*
			**  If cursor is on same row as the one selected
			**  for move, then there is nothing to do.  We
			**  Just unhilight the row and breakout
			*/
/* # line 1018 "rfcat.qsc" */	/* getrow */
                  {
                    if (IItbsetio(2,(char *)"rfchoose_columns",(char *)
"colnames",-2) != 0) {
                      IItcogetio((short *)0,1,30,4,&row,(char *)"_RECO\
RD");
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
/* # line 1019 "rfcat.qsc" */	/* host code */
			if (row == delrownum)
			{
/* # line 1021 "rfcat.qsc" */	/* unloadtable */
                  {
                    if (IItbact((char *)"rfchoose_columns",(char *)
"colnames",0) == 0) goto IItbE5;
                    while (IItunload() != 0) {
                      IItcogetio((short *)0,1,30,4,&row,(char *)"_RECO\
RD");
                      IITBceColEnd();
                      {
/* # line 1024 "rfcat.qsc" */	/* host code */
					if (row == delrownum)
					{
/* # line 1026 "rfcat.qsc" */	/* set_frs */
                        {
                          if (IIiqset(5,0,(char *)"rfchoose_columns",(char *)"\
colnames",(char *)0) != 0) {
                            IIstfsio(14,(char *)"colnames",0,(short *)0,1,30,
                            sizeof(1),(void *)IILQint(1));
                          } /* IIiqset */
                        }
/* # line 1029 "rfcat.qsc" */	/* endloop */
                        {
                          if (1) goto IItbE5;
                        }
/* # line 1030 "rfcat.qsc" */	/* host code */
					}
                      }
                    } /* IItunload */
IItbE5:
                    IItunend();
                  }
/* # line 1032 "rfcat.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE3;
                  }
/* # line 1033 "rfcat.qsc" */	/* host code */
			}
/* # line 1034 "rfcat.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,(char *)"rfchoose_columns",(char *)
"colnames",-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,delrownum) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 1035 "rfcat.qsc" */	/* deleterow */
                  {
                    if (IItbsetio(4,(char *)"rfchoose_columns",(char *)
"colnames",-2) != 0) {
                      if (IItdelrow(0) != 0) {
                      } /* IItdelrow */
                    } /* IItbsetio */
                  }
/* # line 1037 "rfcat.qsc" */	/* host code */
			/*
			**  If the row to move comes before the destination,
			**  then we must decrement the destination row number
			**  to account for the deleted row.
			*/
			if (delrownum < row)
			{
				row--;
			}
/* # line 1046 "rfcat.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,(char *)"rfchoose_columns",(char *)
"colnames",-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,row) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 1047 "rfcat.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(3,0,(char *)"rfchoose_columns",(char *)0, 
                    (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&row,29,(char *)"coln\
ames",0);
                    } /* IIiqset */
                  }
/* # line 1049 "rfcat.qsc" */	/* insertrow */
                  {
                    if (IItbsetio(5,(char *)"rfchoose_columns",(char *)
"colnames",row) != 0) {
                      if (IItinsert() != 0) {
                        IItcoputio((char *)"colnames",(short *)0,1,32,0,cname);
                        IItcoputio((char *)"len",(short *)0,1,30,4,&db_len);
                        IItcoputio((char *)"dt",(short *)0,1,30,2,&db_dt);
                        IItcoputio((char *)"prec",(short *)0,1,30,2,&db_prec);
                        IITBceColEnd();
                      } /* IItinsert */
                    } /* IItbsetio */
                  }
/* # line 1052 "rfcat.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE3;
                  }
                }
              } else if (IIretval() == 3) {
                {
/* # line 1057 "rfcat.qsc" */	/* host code */
			FEhelp(ERx("rfccmove.hlp"), ERget(S_RF00A6_ChCol_Move));
                }
              } else if (IIretval() == 4) {
                {
/* # line 1062 "rfcat.qsc" */	/* host code */
			/*
			**  Unhighlight the selected row since user
			**  decided not to move the column.
			*/
/* # line 1066 "rfcat.qsc" */	/* unloadtable */
                  {
                    if (IItbact((char *)"rfchoose_columns",(char *)
"colnames",0) == 0) goto IItbE6;
                    while (IItunload() != 0) {
                      IItcogetio((short *)0,1,30,4,&row,(char *)"_RECO\
RD");
                      IITBceColEnd();
                      {
/* # line 1069 "rfcat.qsc" */	/* host code */
				if (row == delrownum)
				{
/* # line 1071 "rfcat.qsc" */	/* set_frs */
                        {
                          if (IIiqset(5,0,(char *)"rfchoose_columns",(char *)"\
colnames",(char *)0) != 0) {
                            IIstfsio(14,(char *)"colnames",0,(short *)0,1,30,
                            sizeof(1),(void *)IILQint(1));
                          } /* IIiqset */
                        }
/* # line 1073 "rfcat.qsc" */	/* endloop */
                        {
                          if (1) goto IItbE6;
                        }
/* # line 1074 "rfcat.qsc" */	/* host code */
				}
                      }
                    } /* IItunload */
IItbE6:
                    IItunend();
                  }
/* # line 1076 "rfcat.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE3;
                  }
                }
              } else {
                if (1) goto IIfdE3;
              } /* IIretval */
            } /* IIrunnest */
IIfdF3:
            if (IIchkfrm() == 0) goto IIfdB3;
            goto IIfdE3;
IImuI3:
            if (IIinitmu() == 0) goto IIfdE3;
            if (IInmuact(ERget(F_RF0093_Above),ERget(F_RF0097_ExplAbove),2,2,1)
             == 0) goto IIfdE3;
            if (IInmuact(ERget(F_RF0094_Below),ERget(F_RF0098_ExplBelow),2,2,2)
             == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,3) == 0) goto 
            IIfdE3;
            IIFRInternal(0);
            if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,4) == 0) goto IIfdE3;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE3;
            if (IIendmu() == 0) goto IIfdE3;
            goto IIfdI3;
IIfdE3:;
            IIendnest();
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 1082 "rfcat.qsc" */	/* host code */
		if (IIRFcccount == IIRFcdcount)
		{
			IIUGerr(E_RF00A8_NoColsLeft, UG_ERR_ERROR, 0);
/* # line 1085 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 1086 "rfcat.qsc" */	/* host code */
		}
		coldesc = &(del_cols[IIRFcdcount]);
/* # line 1088 "rfcat.qsc" */	/* unloadtable */
          {
            if (IItbact((char *)"rfchoose_columns",(char *)"colnames",0) == 0
            ) goto IItbE7;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,cname,(char *)
"colnames");
              IItcogetio((short *)0,1,30,4,&db_len,(char *)"len");
              IItcogetio((short *)0,1,30,2,&db_dt,(char *)"dt");
              IItcogetio((short *)0,1,30,2,&db_prec,(char *)"prec");
              IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
              IITBceColEnd();
              {
/* # line 1092 "rfcat.qsc" */	/* host code */
			if (rowstate != ROW_DELETED)
			{
				STcopy(cname, coldesc->name);
				coldesc->datatype = db_dt;
				coldesc->length = db_len;
				coldesc->prec = db_prec;
				coldesc->deleted = TRUE;
				coldesc++;
				IIRFcdcount++;
			}
              }
            } /* IItunload */
IItbE7:
            IItunend();
          }
/* # line 1104 "rfcat.qsc" */	/* inittable */
          {
            if (IItbinit((char *)"rfchoose_columns",(char *)"colnames",
            (char *)"r") != 0) {
              IIthidecol((char *)"len",(char *)"i4");
              IIthidecol((char *)"dt",(char *)"i2");
              IIthidecol((char *)"prec",(char *)"i2");
              IItfill();
            } /* IItbinit */
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 1110 "rfcat.qsc" */	/* host code */
		/*
		**  Just restore the original state on a cancel.
		*/
		IIRFcccount = orig_count;
		IIRFcdcount = orig_dcount;
/* # line 1115 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 1119 "rfcat.qsc" */	/* host code */
		FEhelp(ERx("rfchcols.hlp"), ERget(S_RF00A5_ChooseColumns));
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_OK),ERget(F_RF0099_ExplOK),2,2,1) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact(ERget(F_RF0091_Add),ERget(F_RF0095_ExplAdd),2,2,2) == 0) 
    goto IIfdE2;
    if (IInmuact(ERget(FE_Delete),ERget(F_RF009A_ExplDel),2,2,3) == 0) goto 
    IIfdE2;
    if (IInmuact(ERget(FE_Move),ERget(F_RF009B_ExplMove),2,2,4) == 0) goto 
    IIfdE2;
    if (IInmuact(ERget(F_RF0092_RemoveAll),ERget(F_RF0096_ExplRemoveAll),2,2,5)
     == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,2,6) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,6) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,7) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,7) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 1121 "rfcat.qsc" */	/* host code */
}
VOID
  rFcatalog()
  {
    char r_name[(FE_MAXTABNAME + 1)];
    char *filename;
	OOID		tmpclss;
	LOCATION	tloc;
    char *form;
    char buf[512];
	char            help_scr[80];   	  /* the help screen buffer */
    char *tname;
	char		*title;
	char		*prompt;
	char		*choices;
	bool		stat = TRUE;
	IIOOinit((OO_OBJECT **)NULL);
/* # line 1143 "rfcat.qsc" */	/* display */
    {
      if (IIdispfrm(_iicatalog,(char *)"r") == 0) goto IIfdE4;
      goto IImuI4;
IIfdI4:;
      if (IIfsetio((char *)0) == 0) goto IIfdE4;
      {
/* # line 1146 "rfcat.qsc" */	/* putform */
        {
          if (IIfsetio(_iicatalog) != 0) {
            IIputfldio((char *)"title",(short *)0,1,32,0,
            ERget(F_RF0001_RBF___Report_Catalog));
          } /* IIfsetio */
        }
/* # line 1147 "rfcat.qsc" */	/* host code */
		En_SrcTyp = NoRepSrc;
		rfLast(ERx(""));
		if (STlength(En_report) > 0)
		{
		    if( Rbf_from_ABF == FALSE )
		    {
                    	OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
		    	En_rid = -1;
			if( St_style == RS_NULL)
			{
				/*
				** If St_repspec is FALSE, then En_report
				** should reflect the full, unnormalized
				** form of the table name.
				*/
		    		if (rFget(En_report, St_repspec, St_style))
		    		{
			    	   	rfLast(En_report);
					_VOID_ rFdisplay();
				}
		    	}
			else
			{
				En_SrcTyp = TabRepSrc;
				stat = IIRFdne_DoNewEdit(En_report);
			}
		    }
		    else /* Called from ABF */
		    {
                    	OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
			tmpclss = OC_RBFREP;
                        if(IIOOidCheck( &tmpclss, &En_rid, En_report,
				(char *)NULL, (i4)NULL) == OK )
			{
				/*
				** If St_repspec is FALSE, then En_report
				** should reflect the full, unnormalized
				** form of the table name.
				*/
		    		if (rFget(En_report, St_repspec, St_style))
		    		{
					if (St_style == RS_NULL)
				    		rfLast(En_report);
					_VOID_ rFdisplay();
		    		}
/* # line 1193 "rfcat.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE4;
        }
/* # line 1194 "rfcat.qsc" */	/* host code */
			}
                        /* Display the name of requested form. */
                        form = F_RBFABF;
			rfabfcr(form);
			tname = En_report;
/* # line 1200 "rfcat.qsc" */	/* putform */
        {
          if (IIfsetio(form) != 0) {
            IIputfldio((char *)"rep_name",(short *)0,1,32,0,tname);
          } /* IIfsetio */
        }
/* # line 1201 "rfcat.qsc" */	/* host code */
			STcopy(ERget(S_RF005E_Create_Report), help_scr);
                        En_SrcTyp = IIFRrlpRunListPick(form,
                        	F_RFAFTYPE, LPGR_FLOAT, LPGR_FLOAT,
				help_scr, H_SOURCE,
                        	IIRFcrCreateReport, (PTR) En_report);
/* # line 1207 "rfcat.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE4;
        }
/* # line 1208 "rfcat.qsc" */	/* host code */
		    }
		}
		if (Rbf_noload)
		{
			OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
		}
		else
		{
			OOcatTableLoad(OC_REPORT, _iicatalog, _cattable,
			    	(bool) TRUE, ERx("*"), Lastname, NULL);
/* # line 1218 "rfcat.qsc" */	/* set_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(21,(char *)0,0,(short *)0,1,32,0,(char *)"r");
          } /* IIiqset */
        }
/* # line 1219 "rfcat.qsc" */	/* host code */
		}
		En_SrcTyp = NoRepSrc;
		St_style = NoStyle;
      }
IIfdB4:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 1226 "rfcat.qsc" */	/* host code */
		/* 
		** Display Popup with the 'source' options.  User may choose
		** joindef, table, or view.  This begins a series
		** of listpick handling routines.
		**
		** After this point we should have a valid name ,r_name,
		** a style, St_style, and a data source, En_SrcTyp
		*/
		STcopy(ERget(S_RF005E_Create_Report), help_scr);
                prompt    = ERget(F_RF0042_Source_Prompt);
                choices   = ERget(F_RF0043_Source_Choices);
		En_SrcTyp = IIFDlpListPick(prompt, choices, 0, LPGR_FLOAT,
				LPGR_FLOAT, help_scr, H_SOURCE,
				IIRFcrCreateReport, (PTR) r_name);
		if (Rbf_noload)
		{
		    OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
		}
		else if (Lastname[0] != EOS)
		{
		    OOcatTableLoad(OC_REPORT,
		        _iicatalog, _cattable, 
		        (bool) TRUE, ERx("*"), 
			 Lastname, NULL);
		}
		En_SrcTyp = NoRepSrc;
          }
        } else if (IIretval() == 2) {
          {
/* # line 1258 "rfcat.qsc" */	/* host code */
# ifdef DGC_AOS
		/*
		** Until INGRES supports a readonly mode, DG will use the
		** following code to avoid ugly error messages that occur
		** when trying to write to the database when we are only
		** connected for readonly access.
		*/
		if (IIUIcro_CheckReadOnly())
		{
			IIUGerr(E_UI0016_ReadOnly_err, UG_ERR_ERROR, 0);
/* # line 1268 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 1269 "rfcat.qsc" */	/* host code */
		}
# endif
                /*
		**  Currently (9/88), OOcatDestroy has transaction code
		**  deleting ii_id.  If OOcatDestroy returns OK, then
		**  effectively, the report has been deleted to the user since
	        **  it will NOT show up in the RBF catalog frame even tho the
		**  detail tables (ii_reports, ii_rcommands) have not yet been
	        **  deleted.  There is NO transaction code in rFdestroy.
		**  A possible change could be to get rid of all transaction
		**  code in OOcatDestroy and surround OOcatDestroy AND rFdestroy
		**  by transaction code. (sylviap/danielt)
		*/
		if (OOcatDestroy(OC_REPORT, _iicatalog, _cattable, &rfoocatrec,
		    Rbf_noload) == OK)
		{
			rFdestroy(rfoocatrec.id, NULL);
		}
		else
		{
                        if (IIRFSetReadOnly() == OK)
/* # line 1290 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 1291 "rfcat.qsc" */	/* host code */
		}
		if (Rbf_noload)
		{
			OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
		}
		else 
		{
			STcopy(rfoocatrec.name, r_name);
			OOcatTableLoad(OC_REPORT, _iicatalog, _cattable,
				(bool) TRUE, ERx("*"), r_name, NULL);
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 1307 "rfcat.qsc" */	/* host code */
		En_SrcTyp = NoRepSrc;
		if (OOcatCheck(OC_RBFREP, _iicatalog, _cattable, &rfoocatrec)
		    != OK)
		{
			_VOID_ IIRFSetReadOnly();
/* # line 1312 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 1313 "rfcat.qsc" */	/* host code */
		}
		STcopy(rfoocatrec.name, r_name);
		rfLast(r_name);
		Lastdifferent = FALSE;
		Lastsaved = FALSE;
		En_lmax_orig = En_lmax;	/* save it cause it may change */
		/* load report */
		En_rid = rfoocatrec.id;
		if (rFget(r_name, TRUE, RS_NULL))
		    /* edit report */
		    _VOID_ rFdisplay();
		/* redisplay catalog frame */
		if (Rbf_noload)
		{
			OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
		}
		else if (Lastdifferent)
		{
			OOcatTableLoad(OC_REPORT, _iicatalog, _cattable,
				(bool) TRUE, ERx("*"), Lastname, NULL);
		}
		else if (Lastsaved)
		{
			OOcatTableLoad(OC_REPORT, _iicatalog, _cattable,
				(bool) TRUE, ERx("*"), r_name, NULL);
		}
		En_SrcTyp = NoRepSrc;
		En_lmax = En_lmax_orig;	/* now reset back to original setting */
          }
        } else if (IIretval() == 4) {
          {
/* # line 1349 "rfcat.qsc" */	/* host code */
# ifdef DGC_AOS
		/*
		** Until INGRES supports a readonly mode, DG will use the
		** following code to avoid ugly error messages that occur
		** when trying to write to the database when we are only
		** connected for readonly access.
		*/
		if (IIUIcro_CheckReadOnly())
		{
			IIUGerr(E_UI0016_ReadOnly_err, UG_ERR_ERROR, 0);
/* # line 1359 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 1360 "rfcat.qsc" */	/* host code */
		}
# endif
		if (OOcatRename(OC_REPORT, _iicatalog, _cattable, &rfoocatrec,
			Rbf_noload) != OK )
		{
			_VOID_ IIRFSetReadOnly();
/* # line 1367 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 1368 "rfcat.qsc" */	/* host code */
		}
		STcopy(rfoocatrec.name, r_name);
		if (Rbf_noload)
		{
			OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
		}
		else 
		{
			OOcatTableLoad(OC_REPORT, _iicatalog, _cattable,
				(bool) TRUE, ERx("*"), r_name, NULL);
		}
          }
        } else if (IIretval() == 5) {
          {
/* # line 1384 "rfcat.qsc" */	/* host code */
	 	/* See if the detail frame has already been initialized */
		if (!alloc_detail)
	 	{
	 		Nrdetails = F_RDETAILS;
			if (IIUFgtfGetForm(IIUFlcfLocateForm(),
				(form = Nrdetails))!=OK)
			{
				IIUGerr(E_RF002D_Cannot_locate_form,
					UG_ERR_ERROR,1,form);
/* # line 1393 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 1394 "rfcat.qsc" */	/* host code */
	     		}
	     		alloc_detail = TRUE;
	  	}        
		if (OOcatCheck(OC_REPORT, _iicatalog, _cattable, &rfoocatrec)
		    != OK)
		{
			_VOID_ IIRFSetReadOnly();
/* # line 1402 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 1403 "rfcat.qsc" */	/* host code */
		}
		rf_menup[0].mpstring = ERget(FE_Go);
		OOdetail(rfoocatrec.class, _iicatalog, _cattable,
		    &rfoocatrec, Nrdetails, 
		    ERget(F_RF0006_RBF___Detail_on_a_rep),
		    NULL, rFdetdsply, rFnoop, rFnoop, rFdestroy, rFnoop,
		    rf_menup, H_DETAIL);
                if (Rbf_noload)
			OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
          }
        } else if (IIretval() == 6) {
          {
/* # line 1418 "rfcat.qsc" */	/* host code */
		STcopy (ERget(S_RF0049_Auto_Sub), buf);
		/* 
		** Get table name and mode.  Pass call to IIRFcrp_Callreport
		** to execute report.
		*/
		prompt    = ERget(F_RF004A_Table_Name);
		title     = ERget(F_RF004B_Run_Table_Report);
		En_SrcTyp = TabRepSrc;
		AutoReport = TRUE;
    		stat = IIRFgtn_getname(F_GETNAME, F_GTNTITLE,
			F_GTNPROMPT, F_GTNOBJECT, prompt, title,
			H_GTNTBL, buf, (char *)r_name);
		AutoReport = FALSE;
		if (Rbf_noload)
			OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
          }
        } else if (IIretval() == 7) {
          {
/* # line 1440 "rfcat.qsc" */	/* host code */
		for (;;)
		{
/* # line 1442 "rfcat.qsc" */	/* submenu */
            {
              goto IImuI5;
IIfdB5:
              IIrunmu(0);
              if (IIretval() == 1) {
                {
                  char bufr[136];
/* # line 1448 "rfcat.qsc" */	/* host code */
				if (OOcatCheck(OC_REPORT, _iicatalog, _cattable,
				  &rfoocatrec) != OK)
				{
					_VOID_ IIRFSetReadOnly();
/* # line 1452 "rfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB4;
                  }
/* # line 1453 "rfcat.qsc" */	/* host code */
				}
				STcopy(rfoocatrec.name, r_name);
				IIRFarp_ArchiveReport (bufr);
				if ( STtrmwhite(bufr) <= 0 )
				{
					if (Rbf_noload)
						OOcatEmpty(OC_REPORT, _iicatalog,_cattable);
/* # line 1463 "rfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB4;
                  }
/* # line 1464 "rfcat.qsc" */	/* host code */
				}
				filename = (char *) STalloc(s_chk_file(bufr));
#ifdef CMS
                 		/* accept fn ft; convert to fn.ft */
                 		TOwscnme(filename);
# endif
				IIUGmsg(ERget(S_RF0018_Writing___s__to_file_),
				    (bool) FALSE, 2, r_name, filename);
				/*
				** Now write out the report to a file.
				** Initialize En_rid, so will archive report
				** based on report name, not on previous id
				** number.
				*/
				En_rid = rfoocatrec.id;
				s_reset(RP_RESET_SRC_FILE,RP_RESET_REPORT,RP_RESET_LIST_END);
				s_ren_set(r_name,
				    (rfoocatrec.class == OC_RBFREP) ? ERx("f")
				      : ERx("s"), En_rid);
				if (!cr_dmp_rep(rfoocatrec.owner, Ptr_ren_top,
				    filename, FALSE))
				    /* something went wrong */
				    IIUGerr(E_RF0011_Could_not_write_repor,
					    UG_ERR_ERROR, 0);
				if (Rbf_noload)
				{
				    OOcatEmpty(OC_REPORT, _iicatalog,_cattable);
				}
				MEfree((PTR)filename);
				break;
                }
              } else if (IIretval() == 2) {
                {
/* # line 1503 "rfcat.qsc" */	/* host code */
				FEhelp(H_UTILS,
				  ERget(S_RF001A_Utilities_Submenu));
				continue;
                }
              } else if (IIretval() == 3) {
                {
/* # line 1509 "rfcat.qsc" */	/* host code */
				if (Rbf_noload)
				    OOcatEmpty(OC_REPORT, _iicatalog,_cattable);
				break;
                }
              } else {
                if (1) goto IIfdE5;
              } /* IIretval */
              goto IIfdE5;
IImuI5:
              IImuonly();
              if (IIinitmu() == 0) goto IIfdE5;
              if (IInmuact(ERget(FE_Archive),ERget(F_RF007E_archive),2,2,1) ==
               0) goto IIfdE5;
              if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,2) == 0) goto 
              IIfdE5;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,2,2,2) == 0) goto IIfdE5;
              if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,3) == 0) goto 
              IIfdE5;
              IIFRInternal(0);
              if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE5;
              if (IIendmu() == 0) goto IIfdE5;
              goto IIfdB5;
IIfdE5:;
            }
/* # line 1513 "rfcat.qsc" */	/* host code */
		}
		if (Rbf_noload)
			OOcatEmpty(OC_REPORT, _iicatalog,_cattable);
          }
        } else if (IIretval() == 8) {
          {
/* # line 1520 "rfcat.qsc" */	/* host code */
		if (OOcatCheck(OC_RBFREP, _iicatalog, _cattable, &rfoocatrec)
		    != OK)
		{
			_VOID_ IIRFSetReadOnly();
/* # line 1524 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 1525 "rfcat.qsc" */	/* host code */
		}
		/* run report */
		_VOID_ rFrunrep();
                if (Rbf_noload)
			OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
          }
        } else if (IIretval() == 9) {
          {
/* # line 1535 "rfcat.qsc" */	/* host code */
		FEtabfnd(_iicatalog, _cattable);
          }
        } else if (IIretval() == 10) {
          {
/* # line 1540 "rfcat.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_iicatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 1545 "rfcat.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_iicatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 12) {
          {
/* # line 1551 "rfcat.qsc" */	/* host code */
		FEhelp(H_RCATALOG, ERget(S_RF001C_Report_Catalogs));
		if (Rbf_noload)
			OOcatEmpty(OC_REPORT, _iicatalog, _cattable);
          }
        } else if (IIretval() == 13) {
          {
/* # line 1558 "rfcat.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 1559 "rfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE4;
            }
          }
        } else if (IIretval() == 14) {
          {
/* # line 1564 "rfcat.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 1565 "rfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE4;
            }
          }
        } else {
          if (1) goto IIfdE4;
        } /* IIretval */
      } /* IIrunform */
IIfdF4:
      if (IIchkfrm() == 0) goto IIfdB4;
      goto IIfdE4;
IImuI4:
      if (IIinitmu() == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Create),ERget(F_RF0078_create),2,2,1) == 0) goto 
      IIfdE4;
      if (IInmuact(ERget(FE_Destroy),ERget(F_RF0079_delete),2,2,2) == 0) goto 
      IIfdE4;
      if (IInmuact(ERget(FE_Edit),ERget(F_RF007A_edit),2,2,3) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Rename),ERget(F_RF007B_rename),2,2,4) == 0) goto 
      IIfdE4;
      if (IInmuact(ERget(FE_MoreInfo),ERget(_MoreInfoExpl),2,2,5) == 0) goto 
      IIfdE4;
      if (IInmuact(ERget(F_RF002C_AutoReport),ERget(F_RF007C_preview),2,2,6) 
      == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Utilities),ERget(F_RF007D_utilities),2,2,7) == 0) 
      goto IIfdE4;
      if (IInmuact(ERget(FE_Go),ERget(F_RF007F_go),2,2,8) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,8) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(_FindExpl),2,2,9) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(_TopExpl),2,2,10) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(_BottomExpl),2,2,11) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,12) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,12) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_End),ERget(F_RF0080_quit),2,2,13) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,13) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Quit),ERget(F_RF0080_quit),2,2,14) == 0) goto 
      IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,2,14) == 0) goto IIfdE4;
      if (IIendmu() == 0) goto IIfdE4;
      goto IIfdI4;
IIfdE4:;
      IIendfrm();
    }
/* # line 1567 "rfcat.qsc" */	/* host code */
	return;
  }
VOID
rfLast(name)
char	*name;
{
	if  ((!Lastdifferent) && (Lastname[0] != EOS))
	{
		if (STcompare(name, Lastname) != 0)
		{
			Lastdifferent = TRUE;
		}
	}
	Lastsaved = TRUE;
	STcopy(name, Lastname);
}
static
STATUS
rFdetdsply(id, none)
  OOID id;
  PTR none;
{
  char rcotype[3];
  char rcosection[FE_MAXNAME+1];
  char rcotext[101];
  char rcotext2[101 + 5];
  char *r_rbfedit;
  char *r_tname;
  char *r_qlang;
	i4	ql_def;			/* default query language */
	i4	ql_avail;		/* available query languages */
  char *formname;
	char	*delim;
	char 	*ptr;
	i4 	srctyp;
	bool	free_r_tname;
	formname = Nrdetails;
	/* set rbf editable field */
	if (rfoocatrec.class == OC_RBFREP)
		r_rbfedit = ERget(F_UG0002_Yes2);
	else
		r_rbfedit = ERget(F_UG0007_No2);
/* # line 1621 "rfcat.qsc" */	/* putform */
  {
    if (IIfsetio(formname) != 0) {
      IIputfldio((char *)"xfield1",(short *)0,1,32,0,r_rbfedit);
    } /* IIfsetio */
  }
/* # line 1623 "rfcat.qsc" */	/* host code */
	r_tname = NULL;
	free_r_tname = FALSE;
	/*
	** If the report is based on an object other than a single 
	** table (such as a JoinDef), pull out the object's name.
	** Remember that rcosequence is NUMERIC (b41180):
	*/
/* # line 2161 "rfcat.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rcotext from ii_rcommands where ii_rcommands.object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)"and(ii_rcommands.rcotype=");
    IIputdomio((short *)0,1,32,0,ERx("ac"));
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_rcommands.rcosection=");
    IIputdomio((short *)0,1,32,0,ERx("h"));
    IIwritio(0,(short *)0,1,32,0,(char *)
"and ii_rcommands.rcosequence=1 and ii_rcommands.rcocommand=");
    IIputdomio((short *)0,1,32,0,ERx("begin"));
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,101 + 5-1,rcotext2);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 2170 "rfcat.sc" */	/* host code */
		if ((delim = STindex(rcotext2, ERx(","), 0)) != NULL)
		{
			CMnext(delim);
			ptr = delim;
			CMnext(ptr);
			*ptr = EOS;
			CVan(delim, &srctyp);
			if (srctyp == JDRepSrc)
			{
				CMnext(delim); CMnext(delim);
				_VOID_ STcat(delim, ERx(" (JD)"));
				r_tname = delim;
			}
		}
/* # line 2184 "rfcat.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 2186 "rfcat.sc" */	/* host code */
	/* grab base table and query language from rcommands */
/* # line 2187 "rfcat.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rcotype, rcosection, rcotext from ii_rcommands where ii_rcomma\
nds.object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)"and((ii_rcommands.rcotype=");
    IIputdomio((short *)0,1,32,0,ERx("qu"));
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_rcommands.rcosection=");
    IIputdomio((short *)0,1,32,0,ERx("range"));
    IIwritio(0,(short *)0,1,32,0,(char *)")or(ii_rcommands.rcotype=");
    IIputdomio((short *)0,1,32,0,ERx("sq"));
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_rcommands.rcosection=");
    IIputdomio((short *)0,1,32,0,ERx("from"));
    IIwritio(0,(short *)0,1,32,0,(char *)")or ii_rcommands.rcotype=");
    IIputdomio((short *)0,1,32,0,ERx("ta"));
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,2,rcotype);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,rcosection);
      IIgetdomio((short *)0,1,32,100,rcotext);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 2197 "rfcat.sc" */	/* host code */
		/*
		** Set table name in form, must extract name from sql spec.
		** This is returned as dynamically allocated storage, which
		** is freed upon return from this routine.
		*/
		if (r_tname == NULL)
		{
			/*
			** Skip leading space so r_g_ident() doesn't get
			** confused and return nothing!  Bug 52532.
			*/
			r_g_set(rcotext);
			_VOID_ r_g_skip();
			r_tname = r_g_ident(TRUE);
			free_r_tname = TRUE;
		}
/* # line 2213 "rfcat.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 2215 "rfcat.sc" */	/* host code */
/* # line 1687 "rfcat.qsc" */	/* putform */
  {
    if (IIfsetio(formname) != 0) {
      IIputfldio((char *)"xfield2",(short *)0,1,32,0,r_tname);
    } /* IIfsetio */
  }
/* # line 1689 "rfcat.qsc" */	/* host code */
	/* set query language field in form */
	FEdml(&ql_def, &ql_avail);
	if (ql_avail == FEDMLBOTH)
	{
		if (STcompare(rcotype, ERx("qu")) == 0)
			r_qlang = ERget(F_RF0007_quel);
		else if (STcompare(rcotype, ERx("sq")) == 0)
			r_qlang = ERget(F_RF0008_sql);
		else
			r_qlang = (ql_def == FEDMLQUEL) ? ERget(F_RF0007_quel) :
			  ERget(F_RF0008_sql);
/* # line 1701 "rfcat.qsc" */	/* putform */
  {
    if (IIfsetio(formname) != 0) {
      IIputfldio((char *)"xfield3",(short *)0,1,32,0,
      ERget(F_RF0009_Query_Language_));
      IIputfldio((char *)"xfield4",(short *)0,1,32,0,r_qlang);
    } /* IIfsetio */
  }
/* # line 1706 "rfcat.qsc" */	/* host code */
	}
	else
	{
/* # line 1709 "rfcat.qsc" */	/* clear */
  {
    IIfldclear((char *)"xfield3");
  }
/* # line 1710 "rfcat.qsc" */	/* clear */
  {
    IIfldclear((char *)"xfield4");
  }
/* # line 1711 "rfcat.qsc" */	/* host code */
	}
	if  (free_r_tname)
	{
		_VOID_ MEfree(r_tname);
	}
	return (OK);
}
static
STATUS
  rFrunrep(id, none)
OOID	id;
PTR	none;
  {
	char	r_name[FE_MAXNAME + 1];
	i4     dest;
	STcopy(rfoocatrec.name, r_name);
	St_style = RS_NULL;
	/* Get Destination *and* call the report */
	dest = IIRFgtd_getdest(r_name);
	/* Return the status of the above operation.
	   This fixes bug 39872 (steveh).           */
	return dest >= 0 ? OK : FAIL;
  }
/*
** Name: IIRFcrp_Callreport
**
** Description: This is a listpick handler function called 
**		indirectly from the IIRFgtd_getdest() routine.
**		The destination of the report is passed in as
**		the choice.  If St_style is RS_NULL, there is
**		no specified style so this is a "GO" function
**		to run an existing report.  If St_Style has a
**		value, this is a preview function.
**
**	26-jan-1994 (rdrane)
**		Unnormalize userid as required before calling UTexe().
**	15-feb-1994 (mgw) Bug #59800
**		Take out the debugging noise left in from the previous
**		change.
*/
static
STATUS
IIRFcrp_Callreport(obname, dest, resume )
PTR  obname;	/* Report or table name */
i4  dest;	/* destination of the report desired */
bool  *resume;
  {
    char repflags[REP_LEN];
    char *db_name;
    char *rep_mode;
    char *r_name;
    char *printer;
    i4 copy_num;
	char	*form;
	u_i4	len;
	STATUS	rval;
#ifdef hp9_mpe
	char	err_locb[MAX_LOC + 1];	   /* buffer for err_loc */
	LOCATION err_loc;		   /* MPE stdout name: $STDLIST */
#endif
	LOCATION *temp_loc;		   /* temporary location */
	LOCATION log_loc;		   /* location for the error log name */
	char    log_name[MAX_LOC + 1];	   /* buffer for error log name */
    char buf[MAX_LOC + 1];
	bool    decl_vars;		   /* TRUE if report has declared 
					   ** variables */
	bool    batch;			   /* TRUE if running report in batch */
	bool    break_state;		   /* TRUE if hit CANCEL */
	char    *param_buf;		   /* points to declared variables 
					   ** values to send as a parameters to
					   ** RW */
	i4      mode;			   /* batch or interactive mode */
	CL_ERR_DESC	err_code;
	char	*cmdline;
	char	*dbconnect;
	i4	run_params;
	char	unrml_uid[(FE_UNRML_MAXNAME + 1)];
	r_name = (char *)obname;
	batch = FALSE;
	break_state = FALSE;
	switch( dest)
	{
	case ToNowhere:
		return (dest);
	case ToDefault:
/* # line 1807 "rfcat.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 1808 "rfcat.qsc" */	/* host code */
		if (St_style == RS_NULL)
    	   		IIUGmsg(ERget(S_RF001E_Running_report___s___), 
			(bool) FALSE, 1, r_name);
		else
	   		IIUGmsg(ERget(S_RF004B_Running_auto), (bool) FALSE, 
		 	1, r_name);
		/* 
		** Just run the report.  No flags since default is
		** the screen.
		*/
    		*repflags = EOS;
    		len = 0;
		break;
	case ToFile:
		/* get file name to write report to */
		IIRFgfn_GetFilename(buf, &break_state, &batch, &log_loc, 
			log_name, &param_buf, &decl_vars); 
		if ((break_state) || ((len = STtrmwhite(buf)) <= 0 ))
		{
			/* 
			** If CANCEL, only want to return to last popup so set
			** resume to TRUE.
			*/
			*resume = TRUE;
			return(OK);
		}
#ifdef CMS
		/* accept fn ft; convert to fn.ft */
		TOwscnme(buf);
# endif
		/* create the report flag using the -f flag */
		_VOID_ STprintf (repflags, ERx("-f%s"), buf);
/* # line 1841 "rfcat.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 1842 "rfcat.qsc" */	/* host code */
		if (St_style == RS_NULL)
	    	  IIUGmsg(ERget(S_RF001F_Writing___s__to_file_), 
			(bool) FALSE, 2, r_name, buf);
		else
		  IIUGmsg(ERget(S_RF004C_Writing_auto), (bool) FALSE, 
			2, r_name, buf);
	    	len += 2; /* length is 2 + length of filename */
		break;
	case ToPrinter:
		/* option = Printer */
		IIRFgpr_GetPrinter(buf, &copy_num, &break_state, &batch, 
			&log_loc, log_name, &param_buf, &decl_vars); 
		/* break_state is true if CANCEL is hit */
		if (break_state)	
		{
			/* 
			** Just return and set resume to TRUE to display last
			** popup 
			*/
			*resume = TRUE;
			return(OK);
		}
		/* set -o(output) option */
		STcopy(ERx("-o"), repflags); 
		/* get printer and number of copies */
		printer = &repflags[2];	     
		STcopy(buf, printer); 
		if ((len = STtrmwhite(printer)) > 0 ) 
		{
			/* check if default printer */
			CVlower (buf);
			if (STcompare(buf, ERget(F_RF0018_default)) == 0)
			{
				/* 
				** Use default printer by
				** only passing -o flag
				*/
	    			len = 2;
			}
			else
			{
				/* 
				** include printer name in flag 
				*/
	    			len += 2;
			}
		}
		else
		{
			/* No printer specified. Use default */
	    		len = 2;
		}
		/* truncate the report flags string (bug34587) */
		repflags[len] = 0;
		if (copy_num > 1)
		{
			_VOID_ STprintf( repflags + len, ERx(" -n%d"), 
				copy_num);
			/*
			**   3 = (" -n") 
			** + 3 = (copy_num max is 3 digits,
			** ----   controlled by the rfprint form)
			**   6 =  more to add to current len
			*/
			len += 6; 
		}
/* # line 1916 "rfcat.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 1917 "rfcat.qsc" */	/* host code */
	    	IIUGmsg (ERget(S_RF0052_Printing_report), (bool) FALSE, 1, 
			r_name);
		break;
	}
	/*
	 * B3376: Pass along -l value only if specified on command line
	 */
	if (St_lspec)
		_VOID_ STprintf( repflags + len, ERx(" -l%d"), En_lmax); 
    	/* fix for bug 4465 */
	if (St_silent)
		STcat(repflags, ERx("-s"));
	db_name = En_database;
	if (batch)
	{
		/* 
		** If batch, pass to RW the batch flag, '+e'  and don't wait
		** for RW to finish execution.
		*/
		temp_loc = &log_loc;
		mode = UT_NOWAIT;
		STcat(repflags, ERx(" +e"));
		dbconnect = NULL;	        /* no shared db connection */
		/* 
		** If any declared variables, then add the parameter list to 
		** the report flags 
		*/
		if (decl_vars)
			_VOID_ STpolycat(3, repflags, ERx(" "), param_buf, 
				repflags);
		if (St_style == RS_NULL)
		{
			cmdline=ERx("database=%S,report=%S,flags=%S,user=%S");
			run_params = 4;
		}
		else
		{
		 	cmdline = 
			   ERx("database=%S,name=%S,mode=%S,flags=%S,user=%S");
			run_params = 5;
		}
	}
	else
	{
#ifdef hp9_mpe
		/* Bug #50554 */
		temp_loc = &err_loc;
		STcopy( ERx("$STDLIST"), err_locb);
		LOfroms( FILENAME, err_locb, temp_loc);
#else
		temp_loc = (LOCATION *)NULL;
#endif
		mode = UT_WAIT;
		dbconnect = IIxflag();	        /* shared db connection */
		if (St_style == RS_NULL)
		{
			cmdline=
			 ERx("database=%S,report=%S,flags=%S,user=%S,equel=%S");
			run_params = 5;
		}
		else
		{
		 	cmdline = 
			   ERx("database=%S,name=%S,mode=%S,flags=%S,user=%S,equel=%S");
			run_params = 6;
		}
	}
	/* 
	** Set up screen before calling UTexe - get out of 
	** forms mode 
	*/
/* # line 1994 "rfcat.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 1995 "rfcat.qsc" */	/* host code */
	FTrestore(FT_NORMAL); 
	SIprintf(ERx("\n\n"));
	SIflush(stdout);
	/*
	** Unnormalize the userid as required.  Note that in the case
	** of a tablename, the tablename is already unnormalized.
	*/
	_VOID_ IIUGxri_id(IIUIdbdata()->suser,&unrml_uid[0]);
	/* 
	** Always pass dbconnect, even if in batch mode and don't need it.  
	** run_params always has the correct number of parameters needed.  
	** Makes it easier to only have two different UTexe calls, rather 
	** than four calls.
	*/
	if (St_style == RS_NULL)
	{
		rval = UTexe(mode, temp_loc, (ModDictDesc *) NULL, 
			(PTR)NULL,
		 ERx ("report"),
			 &err_code, cmdline, 
			run_params,db_name,r_name,repflags,&unrml_uid[0],
			dbconnect);
	}
	else
	{
		switch (St_style)
		{
			case RS_TABULAR:
			case RS_COLUMN:
				rep_mode = ERget(F_RF0019_column);
				break;
			case RS_BLOCK:
				rep_mode = ERget(F_RF001B_block);
				break;
			case RS_WRAP:
				rep_mode = ERget(F_RF001A_wrap);
				break;
			default:
				rep_mode = ERx("");
				break;
		}
		rval = UTexe(mode, temp_loc, (ModDictDesc *) NULL, 
			(PTR)NULL,
		 ERx ("report"),
			 &err_code,
		 	cmdline, run_params, db_name, r_name, rep_mode, 
			repflags,&unrml_uid[0], dbconnect);
	}
	/*
	** Emit a message and wait for the user to press RETURN
	** if an error occurred while running report. (38434)
	*/
	if (rval != OK)
	{
		i4             msg_len = ER_MAX_LEN;
		char            msg_buf[ER_MAX_LEN];
		CL_ERR_DESC     sys_err;
		msg_buf[0] = EOS;
		/* Report OS level error messages if available - bug 49656 */
		if  (ERlookup((i4)0, &err_code, 0, (i4 *)NULL,
			      &msg_buf[0], msg_len, 1, &msg_len, &sys_err,
			      (i4)0, (ER_ARGUMENT *)NULL) == OK)
		{
			if  (msg_buf[0] != EOS)
			{
				IIUGerr(E_RF0007_OSError, UG_ERR_ERROR, 1,
					&msg_buf[0]);
			}
		}
		IIUGmsg(ERget(S_RW0025_Hit_RETURN_when_done), (bool)TRUE, 0);
	}
	FTrestore(FT_FORMS); 
	return (OK);
  }
static
STATUS
rFnoop(id, none)
OOID	id;
PTR	none;
{
	return (OK);
}
static
STATUS
rFdestroy(rfooid, none)
  OOID rfooid;
  PTR none;
{
	i4	tries;
	i4	completed;
	iiuicw1_CatWriteOn();
	completed = FALSE;
	for (tries = 0; tries < MAX_RETRIES; tries++)
	{
		ing_deadlock = FALSE;
/* # line 2627 "rfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_reports where object_id=");
    IIputdomio((short *)0,1,30,4,&rfooid);
    IIsyncup((char *)0,0);
  }
/* # line 2628 "rfcat.sc" */	/* host code */
		if (deadlock())
		{
			continue;
		}
		else if (FEinqerr())
		{
			break;
		}
/* # line 2637 "rfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_rcommands where object_id=");
    IIputdomio((short *)0,1,30,4,&rfooid);
    IIsyncup((char *)0,0);
  }
/* # line 2638 "rfcat.sc" */	/* host code */
		if (deadlock())
		{
			continue;
		}
		else if (FEinqerr())
		{
			break;
		}
		completed = TRUE;
		break;
	}
	iiuicw0_CatWriteOff();
	if (!completed)
	{
		if (ing_deadlock)
		    IIUGerr(E_RF0020_Deadlock_has_occurred,
			    UG_ERR_ERROR, 0);
		else
		    IIUGerr(E_RF0021_Error_occurred_in_del, UG_ERR_ERROR, 0);
	}
	return (OK);
}
/*
**  Name:  IIRFcrCreateReport
**  Description:
**      Displays the pop up to get the name of data source (JoinDef, Table, or 
**      View), and calls the routine to get the style of report.
*/
static
i4
IIRFcrCreateReport(r_name, SrcTyp, resume)
PTR     r_name;          /* Report name */
i4      SrcTyp;         /* Method of form creation. */
bool    *resume;        /* Another chance to select form creation method? */
{
	char *prompt;
	char *title;
	char *tname;
	char *help;
	i4  stat = TRUE;
	*resume = FALSE;
	/* Get the object name and report style  **/
	En_SrcTyp = SrcTyp;
	switch( En_SrcTyp)
	{
		case TabRepSrc:
			prompt = ERget(F_RF004A_Table_Name);
			title  = ERget(F_RF004C_Create_Table_Report);
			help   = H_GTNTBL;
			break;
		case JDRepSrc:
			prompt = ERget(F_RF0048_JoinDef_Name);
			title  = ERget(F_RF004F_Create_JD_Report);
			help   = H_GTNJD;
			break;
		case DupRepSrc:
			prompt = ERget(F_RF004D_Existing_Report_Name);
			title  = ERget(F_RF004E_Create_by_Dup);
			help   = H_GTNDUP;
			break;
		case VQRepSrc:
			/* WHOOPS--not implemented */
			*resume = TRUE;
			return(En_SrcTyp);
		default:	/* FORGET */
			return(En_SrcTyp);
	}
    	stat = IIRFgtn_getname(F_GETNAME, F_GTNTITLE,
		F_GTNPROMPT, F_GTNOBJECT, prompt, title,
		help, prompt, (char *)r_name);
	/*
	** If no name entered, resume
	*/
	if  (!stat)
	{
		*resume = TRUE;
	}
	return(En_SrcTyp);
}
/*
**  Name:  IIRFgtn_getname
**  Description:
**      Displays the pop up to get the name of data source (JoinDef, Table, or 
**      View), and calls the routine to get the style of report.
**
**	3-nov-1992 (rdrane)
**		Replace FErelexists() call with FE_fullresolve() to support
**		6.5 owner.tablename constructs.
**	8-dec-1992 (rdrane)
**		If creating a report based upon a JoinDef or a Table and
**		the connection is to a 6.5 database, then enable expanded
**		namespace.
**	05-jan-93 (sylviap)
**		Added a new menuitem 'Edit' to allow editing/creating of 
**		joindefs from rbf.  Also changed the behavior to display an 
**		error when creating a report.  If no report/table/joindef name 
**		is entered in the 'create popup', then an error is displayed.
**		This behavior is consistent with vifred's.
**	21-jan-1993 (rdrane)
**		Ensure that the object name (ob_name) buffer is large enough
**		for an owner.tablename specification!
**	13-apr-1993 (rdrane)
**		Effect full resolution of table name before determining
**		initial/changed specification (b50688).
**	6-oct-1993 (rdrane)
**		If AutoReport is TRUE, then we got here via Preview.  That
**		being the case, ChooseColumns is NOT a valid option.   This
**		addresses b54951.
**	26-jan-1994 (rdrane)
**		Unnormalize userid as required before calling UTexe.
**	15-feb-1994 (mgw) Bug #59800
**		Take out the debugging noise left in from the previous
**		change.
**	21-jun-1995 (harpa06)
**		Convert tables names given into it's BE representation before 
**		continuing to process the table so that delimited table name 
**		output is correct.
*/
static
i4
IIRFgtn_getname(form_name, title_fld, prompt_fld, name_fld,  prompt_str,
		title_str, help_scrn, help_title, o_name )
  char *form_name;
  char *title_fld;
  char *prompt_fld;
  char *name_fld;
  char *prompt_str;
  char *title_str;
char	*help_scrn;	/* help screen file */
char	*help_title;	/* help screen title */
  char *o_name;
{
  char ob_name[(FE_MAXTABNAME +1)];
  i2 null_ob;
  char *lcpick;
  i4 joindef_flag;
  i4 tbl_flag;
	FE_RSLV_NAME cat_ferslv;	/* Tablename validation work struct */
    	FE_REL_INFO 	msg;
	bool MasDet = FALSE;  /* If we are dealing with a JoinDef as 
			the source of data see if it's a master/detail */
	char lpbuf[1024];
	char hlpbuf[80];
	char *prompt;
	char *title;
        OOID    ooclass;
        OOID    ooid;
	i4	stat = TRUE;
	char	unrml_uid[(FE_UNRML_MAXNAME + 1)];
	/* 
	** if popup not already initialized 
	*/
	if (!(name_frm))	
  {
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name)!=OK)
		{  
			IIUGerr(E_RF002D_Cannot_locate_form, UG_ERR_ERROR,
				1, form_name);
			return (FALSE);
		}
		name_frm = TRUE;
  }
	/* 
	** Display Popup that prompts to enter the table/view/joindef name 
	*/
/* # line 2302 "rfcat.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(form_name,(char *)"f") == 0) goto IIfdE6;
    goto IImuI6;
IIfdI6:;
    if (IIfsetio((char *)0) == 0) goto IIfdE6;
    IIputfldio(prompt_fld,(short *)0,1,32,0,prompt_str);
    IIputfldio(title_fld,(short *)0,1,32,0,title_str);
    {
/* # line 2305 "rfcat.qsc" */	/* host code */
		St_style = RS_NULL;
		/*
		** Reset some stuff for the Choose Columns feature.
		** Disable/enable 'ChooseColumns ' menuitem.  Disable if user 
		** is not creating a report based on a table.  If user is
		** creating a report based on a table, enable menuitem to 
		** allow user to select which columns to base the report on.
		** However, if this is an AutoReport (Preview), then
		** ChooseColumns is disallowed (b54951).
		*/
		if ((En_SrcTyp == TabRepSrc) && (!AutoReport))
			tbl_flag = 1;
		else
			tbl_flag = 0;
/* # line 2320 "rfcat.qsc" */	/* set_frs */
      {
        if (IIiqset(7,0,form_name,(char *)0, (char *)0) != 0) {
          IIstfsio(67,ERget(F_RF008F_ChooseCols),0,(short *)0,1,30,4,&tbl_flag);
        } /* IIiqset */
      }
/* # line 2323 "rfcat.qsc" */	/* host code */
		IIRFcccount = IIRFcdcount = 0;
		ccol_owner[0] = EOS;
		ccol_name[0] = EOS;
		/* 
		** Disable/enable 'Create Joindef' menuitem.  Disable if user 
		** is not creating a report based on a joindef.  If user is
		** creating a report based on a joindef, enable menuitem to 
		** allow user to create/edit the joindef.
		*/
		if (En_SrcTyp == JDRepSrc)
			joindef_flag = 1;
		else
			joindef_flag = 0;
/* # line 2338 "rfcat.qsc" */	/* set_frs */
      {
        if (IIiqset(7,0,form_name,(char *)0, (char *)0) != 0) {
          IIstfsio(67,ERget(FE_Edit),0,(short *)0,1,30,4,&joindef_flag);
        } /* IIiqset */
      }
    }
IIfdB6:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 2344 "rfcat.qsc" */	/* host code */
		/* 
		** Get the user specified object name:
		*/
/* # line 2347 "rfcat.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio(&null_ob,1,32,(FE_MAXTABNAME +1)-1,ob_name,name_fld);
            } /* IIfsetio */
          }
/* # line 2349 "rfcat.qsc" */	/* host code */
		switch (En_SrcTyp)
		{
		case JDRepSrc:
			if ((null_ob == (-1)) || ( STtrmwhite(ob_name) <= 0 ))
			{
				/* If no name, then print error */
				IIUGerr(E_RF00A9_NoJDName,UG_ERR_ERROR, 0);
/* # line 2356 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2357 "rfcat.qsc" */	/* host code */
			}
			ooclass = OC_JOINDEF;
			ooid = -1;
                        if(IIOOidCheck( &ooclass, &ooid, ob_name, (char *)NULL,
				(i4)NULL) == OK )
                        {
				/* This should always be TRUE */
				if (!r_JDLoad(ob_name, &MasDet))
				{
/* # line 2366 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2367 "rfcat.qsc" */	/* host code */
				}
				STcopy(ob_name, o_name);
			}
			else
			{
				IIUGerr(E_RF0090_Bad_JoinDef,UG_ERR_ERROR,
					1,ob_name);
/* # line 2374 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2375 "rfcat.qsc" */	/* host code */
			}
			break;
		case TabRepSrc:
			if ((null_ob == (-1)) || ( STtrmwhite(ob_name) <= 0 ))
			{
				/* If no name, then print error */
				IIUGerr(E_RF00AA_NoTableName,UG_ERR_ERROR, 0);
/* # line 2382 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2383 "rfcat.qsc" */	/* host code */
			}
			/*
			** If we are dealing with a table or view object,
			** decompose any owner.tablename, and verify its
			** existence/accessability.  If not found, 
			** reset the counts to make it appear that
			** the user did not do any Choose Columns work
			** in case a name change occurred.
			*/
			cat_ferslv.name = ob_name;
			cat_ferslv.owner_dest = &ob_odest[0];
			cat_ferslv.name_dest = &ob_ndest[0];
			cat_ferslv.is_nrml = FALSE;
			if  (!(FE_fullresolve(&cat_ferslv)))
			{
				IIRFcccount = IIRFcdcount = 0;
				ccol_owner[0] = EOS;
				ccol_name[0] = EOS;
				IIUGerr(E_RF004A_Bad_Table,UG_ERR_ERROR,
					1,ob_name);
/* # line 2404 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2405 "rfcat.qsc" */	/* host code */
			}
		        /* convert the user table name input into it's BE
			** representation for user output.
			**/
			if (STbcompare (cat_ferslv.owner_dest,
				        STlength(cat_ferslv.owner_dest),
				        Usercode,STlength(Usercode),TRUE) !=0)
				              IIUIxrt_tbl(cat_ferslv.owner_dest,
				                          cat_ferslv.name_dest,
							  ob_name);
			else 
			{
				if (IIUGdlm_ChkdlmBEobject(ob_name,NULL,FALSE) 
				    !=UI_DELIM_ID)
				    	STcopy(cat_ferslv.name_dest,ob_name);
			}
			/*
			**  If user changed the name, then we just
			**  reset the counts to make it appear that
			**  the user did not do any Choose Columns work.
			*/
			if ((STcompare(&ob_odest[0], &ccol_owner[0]) != 0) ||
			    (STcompare(&ob_ndest[0], &ccol_name[0]) != 0))
			{
				IIRFcccount = IIRFcdcount = 0;
				ccol_owner[0] = EOS;
				ccol_name[0] = EOS;
			}
			break;
		case DupRepSrc:
			if ((null_ob == (-1)) || ( STtrmwhite(ob_name) <= 0 ))
			{
				/* If no name, then print error */
				IIUGerr(E_RF00AB_NoRWName,UG_ERR_ERROR, 0);
/* # line 2441 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2442 "rfcat.qsc" */	/* host code */
			}
			St_style = RS_NULL;
			/* verifiy by checking catalog name and class */
			ooid = -1;
			ooclass = OC_RBFREP;
                        if(IIOOidCheck( &ooclass, &ooid, ob_name, (char *)NULL,
				(i4)NULL) == OK )
                        {
				STcopy(ob_name, o_name);
				if( (stat = IIRFdne_DoNewEdit((char *)o_name)) 
					== FALSE)
				{
/* # line 2454 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2455 "rfcat.qsc" */	/* host code */
				}
/* # line 2456 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE6;
          }
/* # line 2457 "rfcat.qsc" */	/* host code */
			}
			else
			{
				IIUGerr(E_RF0091_Bad_Report,UG_ERR_ERROR,
					1,ob_name);
/* # line 2462 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2463 "rfcat.qsc" */	/* host code */
			}
			break;
		}
		/*
		** ob_name & friends are valid at this point
		*/
		/*
		** The order here is important
		** It corresponds to the selections
		** numbers defined in rbfcons.h
		*/
		prompt = ERget(F_RF0044_Style_Prompt);
		STcopy(ERget(S_RF005A_Style_Choices),lpbuf);
		STcopy(ERget(S_RF005F_Report_Styles),hlpbuf);
		if (!AutoReport)
		{
			STcat(lpbuf,ERget(S_RF005B_IndStyle_Choice));
			STcat(lpbuf,ERget(S_RF0063_Labels_Choice));
			if (MasDet)
			{
				STcat(lpbuf,ERget(S_RF005C_MDStyle_Choice));
			}
		}
		St_style = IIFDlpListPick(prompt,lpbuf,0,
			LPGR_FLOAT,LPGR_FLOAT, hlpbuf,
			H_STYLES, IIRFgts_getstyle, (PTR) ob_name );
		if( St_style == RS_NULL )
		{
			/*
			** no style was entered by user
			** Return with empty name
			*/
			*o_name = EOS;
/* # line 2499 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2500 "rfcat.qsc" */	/* host code */
		}
		/*
		** If the server supports delimited identifiers, then
		** ensure that .DELIMID is enabled since we're doing a report
		** based upon a table, and it may require such services.
		*/
		if  ((IIUIdlmcase() != UI_UNDEFCASE) &&
		     (En_SrcTyp != DupRepSrc))
		{
			Rbf_xns_given = TRUE;
			St_xns_given = TRUE;
		}
/* # line 2513 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE6;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 2519 "rfcat.qsc" */	/* host code */
		STATUS          rval;		/* return value from UTexe */
		LOCATION        *temp_loc;      /* temporary location */
		i4              mode;           /* batch or interactive mode */
		CL_ERR_DESC     err_code;	/* error status from UTexe */
		char            *cmdline;	/* cmdline for UTexe */
		char            *dbconnect;	/* -X connection */
		i4     	run_params;	/* # of parameters */
		char		*db_name;	/* database name */
		/* 
		** Get the user specified object name:
		*/
/* # line 2533 "rfcat.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio(&null_ob,1,32,(FE_MAXTABNAME +1)-1,ob_name,name_fld);
            } /* IIfsetio */
          }
/* # line 2535 "rfcat.qsc" */	/* host code */
		if ((null_ob == (-1)) || ( STtrmwhite(ob_name) <= 0 ))
		{
			/* If no name, then print error */
			IIUGerr(E_RF00A9_NoJDName,UG_ERR_ERROR, 0);
/* # line 2539 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2540 "rfcat.qsc" */	/* host code */
		}
		mode = UT_WAIT;		/* execute interactive, not batch */
		temp_loc = NULL;	/* no log file */
		dbconnect = IIxflag();  /* pass along db connection */
		db_name = En_database;  /* database name */
		cmdline=ERx("database=%S,nogo=%S,user=%S,equel=%S");
		run_params = 4;
/* # line 2549 "rfcat.qsc" */	/* clear */
          {
            IIclrscr();
          }
/* # line 2552 "rfcat.qsc" */	/* host code */
		/*
		** Unormalize the userid as required.
		*/
		_VOID_ IIUGxri_id(IIUIdbdata()->suser,&unrml_uid[0]);
		rval = UTexe(mode, temp_loc, (ModDictDesc *) NULL, 
			(PTR)NULL, ERx ("qbf"), &err_code, cmdline, 
			run_params,db_name,ob_name,&unrml_uid[0],
			dbconnect);
                /* Check if any errors occurred */
                if (rval != OK)
                {
                        i4 err = rval;
                        IIUGerr(E_RF00AC_ExecutionError, UG_ERR_ERROR, 2,
                                ERx("qbf"), (PTR)&err);
                }
/* # line 2570 "rfcat.qsc" */	/* clear */
          {
            IIclrscr();
          }
/* # line 2572 "rfcat.qsc" */	/* redisplay */
          {
            IIredisp();
          }
/* # line 2573 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 2579 "rfcat.qsc" */	/* host code */
		/*
		**  Must check that there is a name, it is valid
		**  and user has access to it.  Each of these
		**  conditions must be caught and separate errors
		**  issued.
		*/
		if (!haveccform)
		{
			if (IIUFgtfGetForm(IIUFlcfLocateForm(),
				ERx("rfchoose_columns")) != OK)
			{
				IIUGerr(E_RF002D_Cannot_locate_form,
					UG_ERR_ERROR,1,ERx("rfchoose_columns"));
/* # line 2593 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2594 "rfcat.qsc" */	/* host code */
			}
			haveccform = TRUE;
		}
		/*
		**  Get the table name.
		*/
/* # line 2601 "rfcat.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio(&null_ob,1,32,(FE_MAXTABNAME +1)-1,ob_name,name_fld);
            } /* IIfsetio */
          }
/* # line 2603 "rfcat.qsc" */	/* host code */
		if (null_ob == (-1) || STtrmwhite(ob_name) <= 0)
		{
			/*
			**  User has not entered a name yet.
			**  Give error and continue.
			*/
			IIUGerr(E_RF00A3_No_Tbl_Name, UG_ERR_ERROR, 0);
/* # line 2610 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2611 "rfcat.qsc" */	/* host code */
		}
		/*
		** In order to get an accurate indication of any initial
		** or changed specification, effect a full resolve on the
		** table name before checking against the saved values
		** (b50688).  While this is a bit of overkill, the only
		** superfluous operation is the resolve_table(), and it
		** can be argued that the named table could have disappeared
		** or otherwise become/changed as a synonym.  So,
		** unconditionally determine if the table name is any good
		** that user has access to it.
		*/
		cat_ferslv.name = ob_name;
		cat_ferslv.owner_dest = &ob_odest[0];
		cat_ferslv.name_dest = &ob_ndest[0];
		cat_ferslv.is_nrml = FALSE;
		if  (!(FE_fullresolve(&cat_ferslv)))
		{
			IIUGerr(E_RF004A_Bad_Table,UG_ERR_ERROR,1,ob_name);
/* # line 2631 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB6;
          }
/* # line 2632 "rfcat.qsc" */	/* host code */
		}
		/*
		** Check for initial or changed specification
		*/
		if  ((STcompare(&ob_odest[0],&ccol_owner[0]) != 0) ||
		     (STcompare(&ob_ndest[0],&ccol_name[0]) != 0))
		{
			STcopy(&ob_odest[0],&ccol_owner[0]);
			STcopy(&ob_ndest[0],&ccol_name[0]);
			IIRFcccount = IIRFcdcount = 0;
		}
		IIRFccChooseColumns(&ccol_name[0], &ccol_owner[0]);
        }
      } else if (IIretval() == 4) {
        {
/* # line 2649 "rfcat.qsc" */	/* host code */
		stat = FALSE;
/* # line 2650 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE6;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 2656 "rfcat.qsc" */	/* host code */
	    switch (En_SrcTyp)
	    {
	    	case TabRepSrc:
			/* Don't allow table creation, examination. */
			set_IituMainform(FALSE);		 
			lcpick = IITUtpTblPik(NULL, ERget(F_RF0023_RBF));
			break;
	    	case DupRepSrc:
			STcopy(ERget(F_RF0050_Existing_Reports), hlpbuf);
			lcpick = IIUFopObjPik(hlpbuf, H_LPREPORTS,  OC_RBFREP,
			    	ERget(F_RF0051_reports),
			    	ERget(F_RF0052_Selecting_Reports),
			    	ERget(F_RF0053_Report_Name));
			break;
	    	case JDRepSrc:
			STcopy(ERget(S_RF005D_List_of_JD), hlpbuf);
			lcpick = IIUFopObjPik(hlpbuf, H_LPJOINDEFS, OC_JOINDEF,
				    ERget(F_RF0049_JoinDefs),
				    ERget(F_RF0047_Selecting_Jdef_names),
				    ERget(F_RF0048_JoinDef_Name));
			break;
	    }
	    if ((lcpick != NULL) && (*lcpick != EOS))
	    {
/* # line 2680 "rfcat.qsc" */	/* putform */
          {
            if (IIfsetio(form_name) != 0) {
              IIputfldio(name_fld,(short *)0,1,32,0,lcpick);
            } /* IIfsetio */
          }
/* # line 2681 "rfcat.qsc" */	/* host code */
	    }
        }
      } else if (IIretval() == 6) {
        {
/* # line 2687 "rfcat.qsc" */	/* host code */
		FEhelp(help_scrn,help_title);
        }
      } else {
        if (1) goto IIfdE6;
      } /* IIretval */
    } /* IIrunform */
IIfdF6:
    if (IIchkfrm() == 0) goto IIfdB6;
    goto IIfdE6;
IImuI6:
    if (IIinitmu() == 0) goto IIfdE6;
    if (IInmuact(ERget(FE_OK),ERget(F_RF0081_enter_object),2,2,1) == 0) goto 
    IIfdE6;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE6;
    if (IInmuact(ERget(FE_Edit),ERget(F_RF009C_EditExpl),2,2,2) == 0) goto 
    IIfdE6;
    if (IInmuact(ERget(F_RF008F_ChooseCols),ERget(F_RF0090_ChColExpl),2,2,3) 
    == 0) goto IIfdE6;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,4) == 0) 
    goto IIfdE6;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,4) == 0) goto IIfdE6;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,5) == 0) goto IIfdE6;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE6;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,6) == 0) goto IIfdE6;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,6) == 0) goto IIfdE6;
    if (IIendmu() == 0) goto IIfdE6;
    goto IIfdI6;
IIfdE6:;
    IIendfrm();
  }
/* # line 2690 "rfcat.qsc" */	/* host code */
	return (stat);
}
/*
**  Name:  IIRFgts_getstyle
**  Description:
**      Displays the pop up to the default style of the report.
*/
static
i4
 IIRFgts_getstyle(obname, choice, resume)
PTR  obname;     /* If not NULL called from auto report--call callreport */
i4  choice;     /* style/mode of the report desired */
bool  *resume;
{
	i4 dest;
	i4 stat;
	/* 
	** Display Popup with the 'style' options.  User may create the 
	** report using the following styles: tabluar, indented, block
	** or labels.
	*/
	switch (choice)
	{
		/* Tabular */
		case TabStyle:
			St_style = RS_TABULAR;
			break;
		/* Wrap */
		case WrpStyle:
			St_style = RS_WRAP;
			break;
		/* Indented */
		case IndStyle:
			St_style = RS_INDENTED;
			break;
		/* Block */
		case BlkStyle:
			St_style = RS_BLOCK;
			break;
		/* Labels */
		case LblStyle:
			St_style = RS_LABELS;
			break;
		/* Master/Detail */
		case MDStyle:
			St_style = RS_MASTER_DETAIL;
			break;
		/* Forget this */
		case NoStyle:
		default:
			St_style = RS_NULL;
			break;
	}
	/* If a style was chosen and
	** we were called from auto report,
	** get the destination of the report
	** AND run it
	**
	** Otherwise do edit of new object
	*/
	if( St_style != RS_NULL )
	{
		if(  AutoReport )
		{
			if( (dest = IIRFgtd_getdest((char *)obname)) 
						== ToNowhere )
				*resume  = TRUE;
		}
		else 
		{
			if( (stat = IIRFdne_DoNewEdit((char *)obname)) == FALSE)
				*resume  = TRUE;
		}
	}
	return( St_style);
}
/*
**  Name:  IIRFgtd_getdestination
**  Description:
**      Displays the pop up to the destination of the report.
**      Calls Callreport to actually do the report handling, pasing
**	along the name
*/
static
i4
IIRFgtd_getdest(obname)
char *obname;
{
	i4 dest;
	char *prompt;
	char *choices;
	char hlpbuf[80];
	prompt  = ERget(F_RF0045_Dest_Prompt);
	choices = ERget(F_RF0046_Dest_Choices);
	STcopy( ERget(S_RF0060_Report_Dest),hlpbuf);
	dest = IIFDlpListPick(prompt, choices, 0,
		LPGR_FLOAT,LPGR_FLOAT, hlpbuf, H_DEST,
		IIRFcrp_Callreport, (PTR) obname);
	return (dest);
}
/*
**  Name:  IIRFdne_DoNewEdit
**  Description:
**      Edit a new report
*/
static
i4
IIRFdne_DoNewEdit(obname)
char *obname;
{
	bool stat = TRUE;
	En_lmax_orig = En_lmax;     /*B3376*/
	if ((En_SrcTyp != NoRepSrc) && (obname != NULL) && (*obname != EOS))
	{
	       	En_rid = -1;
		if (En_SrcTyp == DupRepSrc)
		{
			rfLast(ERx(""));
			Lastdifferent = TRUE;
			Lastsaved = FALSE;
			/* load report */
	    		En_rid = -1;
			stat = rFget(obname, TRUE, RS_NULL);
		}
		else if (En_SrcTyp == JDRepSrc)
		{
			stat = r_JDrFget(obname, St_style);
		}
		else if (En_SrcTyp == TabRepSrc)
		{
			/*
			** obname wants to be full, unnormalized spec
			*/
			stat = rFget(obname, FALSE, St_style);
		}
	    	if (stat)
	   	{
	            /* B3376: See if the default report was too 
		    ** wide (It changes En_lmax!) Don't allow 
		    ** that.  Give message and change it back.
		    */
		    if( En_lmax_orig >= En_lmax )
		    {
		  	/* 
		   	** B7209: En_lmax get reset in rFget.
		   	** If it comes back smaller than the 
		   	** original, then set it back.  Reset 
			** does not take place if we are dup-
			** licating an already existing report.
			** (fixes bug 35177 - steveh).
		   	*/
			if (Rbf_ortype != OT_DUPLICATE && Rbf_ortype != OT_EDIT)
			{
				En_lmax = En_lmax_orig;
			}
		    }
		    else if (St_style == RS_LABELS)
		    {
			/* Never let a label report wrap */
		        CVna(En_lmax,lmax_str);
		        CVna(En_lmax_orig,oldlmax_str);
		        IIUGerr (E_RW13E9_Default_rpt_too_wide,
			   UG_ERR_ERROR,2,lmax_str,oldlmax_str);
		        En_lmax = En_lmax_orig;
			return(FALSE);
		    }
		    else if (St_lspec && 
		       ((St_style == RS_TABULAR) || (St_style == RS_COLUMN)))
		    {
		       /* 
		       ** Let the user choose whether to create the report
		       ** greater than the -l flag specified.  #396, #709
		       ** Indented and Master/Detail reports are caught 
		       ** in r_IndentedSetUp and r_JDMasterDetailSetUp 
		       ** (rbfstyles.c).
		       */
		       if (IIRFwid_WideReport(En_lmax, En_lmax_orig) != OK)
		       {
			   En_lmax = En_lmax_orig;
			   return(FALSE);
		       }
		    }
		   rfLast(ERx(""));
		   if (!rFdisplay())
			return(FALSE);
		   /* Now reset back to original setting */
		   En_lmax = En_lmax_orig;
		}
	}
	return(stat);
}
/*
**  Name:  IIRFmps_MakeParams
**  Description:
**      Creates the parameter list to send off to ReportWriter based
**	on the declared variables in the report.  The values for the variables
**	are entered in by the user on the 'declared variables' frame.
**	IIRFmps_MakeParams then matches the values to the variable name to
**	create the parameter list.
**	12-nov-1992 (rdrane)
**		Fix-up IIUGbmaBadMemoryAllocation() ambiguities.
*/
static
char *
IIRFmps_MakeParams (frmname, tblname, var_col, var_value)
  char *frmname;
  char *tblname;
  char *var_col;
  char *var_value;
{
  char rcoattid[FE_MAXNAME + 1];
  char val[101];
	static  i4	max_buf;	/* memory management variables */
	static  u_i4	param_tag;
	static  u_i4	free_tag;
	static  char	*start_buf;
	u_i4	tag1, tag2, temp_tag;
	bool	first_param;	/* TRUE=searching for first param */
	i4	mem_used;	/* how much of param buffer has been used */
	char	*param_buf;	/* points to the next open space */
	char	*old_buf;
	char	param[200];
	i4	param_len;
  i2 null_ind;
	/* is memory allocated to build the command line of parameters? */
	if (max_buf == 0)
	{
		/* first allocate the buffer */
		max_buf = BUF_SIZE;
		tag1 = FEgettag();
		tag2 = FEgettag();
		param_tag = tag1;
		free_tag = tag2;
		if ((start_buf = (char *) FEreqmem (param_tag, max_buf, TRUE, 
			(STATUS *) NULL)) == NULL)
		{
			IIUGbmaBadMemoryAllocation (
				ERx("IIRFmps_MakeParams - max_buf"));
		}
	}
	first_param = TRUE;	/* still looking for the first parameter */
	mem_used = 0;		/* no memory in buffer has been used */
	param_buf = start_buf;
	STcopy(ERx("\"("),param_buf);
	/* 
	** We know the characters is a "( so don't 
	** need to do a CMbyteinc
	*/
	param_buf += 2;		/* inc. counter */
	mem_used += 2;
	/* get values from frame */
/* # line 2969 "rfcat.qsc" */	/* unloadtable */
  {
    if (IItbact(frmname,tblname,0) == 0) goto IItbE8;
    while (IItunload() != 0) {
      IItcogetio(&null_ind,1,32,100,val,var_value);
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,rcoattid,var_col);
      IITBceColEnd();
      {
/* # line 2972 "rfcat.qsc" */	/* host code */
		/*
		** The declared variables are FE "things", and so can't be
		** delimited identifiers.  But why are we bothering to case
		** them?  RW should handle this at run time ...
		*/
		/* 
		** If found the first parameter, then do NOT need a ","
		** otherwise insert comma before parameter.
		*/
		if (first_param)
		{
			if (null_ind < 0)
				STprintf(param,"%s=''", rcoattid);
			else
				STprintf(param,"%s='%s'", rcoattid,val);
			first_param = FALSE;
		}
		else
		{
			if (null_ind < 0)
				STprintf(param,",%s=''", rcoattid);
			else
				STprintf(param,",%s='%s'",rcoattid,val);
		}
		param_len = STlength (param);
		/* if parameter does not fit in buffer, allocate new buffer */
		if (param_len + mem_used > max_buf)
		{
			max_buf = max(max_buf * 2,param_len+max_buf+1);
			old_buf = start_buf;
			if ((start_buf = (char *) FEreqmem(free_tag, 
				max_buf, TRUE, (STATUS *)NULL))== NULL)
			{
				IIUGbmaBadMemoryAllocation 
				   (ERx("IIRFmps_MakeParams - start_buf"));
			}
			_VOID_ CMcopy(old_buf, mem_used, start_buf);
			_VOID_ FEfree(param_tag); 
			temp_tag = free_tag;
			free_tag = param_tag; 
			param_tag = temp_tag;;
			param_buf = start_buf + mem_used;
		}
		STcopy(param,param_buf);
		while (*param_buf != EOS)
			_VOID_ CMnext(param_buf);
		mem_used += param_len;
      }
    } /* IItunload */
IItbE8:
    IItunend();
  }
/* # line 3022 "rfcat.qsc" */	/* host code */
	STcopy(ERx(")\""),param_buf);
	return (start_buf);
}
/*
**  Name:  IIRFgdv_GetDeclaredVars
**  Description:
**	Displays the declared variable frame, so the user can enter in the
**	values for the declared variables in the report.  The prompts are
**	supplied by the 'with prompt' clause in the .DECLARE command.
**	IIRFgdv_GetDeclaredVars queries the database for the .DECLARE,
**	then parses for the prompt string.  A declared variable does not
**	necessarily have a prompt associated with it.
*/
static
char *
IIRFgdv_GetDeclaredVars(break_state,declared)
bool	*break_state;		/* TRUE if CANCEL was hit */
bool	*declared;		/* TRUE if there are any declared variables */
{
  char text[101];
  char attid[FE_MAXNAME + 1];
  char val_str[101];
  i2 null_val;
  i4 id;
  char *prompt_string;
  char *Nrvariable;
  char *var_tbl;
  char *prmpt_fld;
  char *value_fld;
  char *Nrprompt;
  char *colname_fld;
  char *fullpr_fld;
	char    *string;
	char    *token;
	i4     state;
	i4     token_type;
	char	*form;
	char	*params;
	static  bool    decl_frm = FALSE;   /* TRUE = declared variable popup
					    ** initialized */
	static  bool    prompt_frm = FALSE; /* TRUE=prompt popup initialized */
	/* set up the pop-up variables */
	Nrvariable = F_VARIABLE;	/* pop up "rfvar" */
	var_tbl = T_VAR;
	prmpt_fld = F_PROMPT;
	value_fld = F_VALUE;
	Nrprompt = F_FPROMPT;		/* pop up "rfprompt" */
	colname_fld = F_COLUMN;
	fullpr_fld = F_FULLPR;
	/* 
	** Check if there are any declared variables with prompt 
	** strings.
	*/
	state = WANT_PROMPT;	/* initialize state */
	*break_state = FALSE;	/* did not hit CANCEL */
	*declared = FALSE;      /* no declared variables found */
	/* Initialize .DECLARE frame */
	if (!decl_frm)	
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(),(form = Nrvariable))!=OK)
		{  
	      	   IIUGerr(E_RF002D_Cannot_locate_form,UG_ERR_ERROR,1,form);
		   *break_state = TRUE;
	      	   return (NULL);
		}
		else
		{
	      	   decl_frm = TRUE;
		}
	}
	/* Init table with col_name as a hidden column */
/* # line 3103 "rfcat.qsc" */	/* inittable */
  {
    if (IItbinit(Nrvariable,var_tbl,(char *)"u") != 0) {
      IIthidecol((char *)"col_name",(char *)"varchar(33)");
      IItfill();
    } /* IItbinit */
  }
/* # line 3105 "rfcat.qsc" */	/* host code */
	/* Get all .DECLARE rows */
	id = rfoocatrec.id;
/* # line 3708 "rfcat.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rcoattid, rcotext from ii_rcommands where ii_rcommands.object_\
id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_rcommands.rcotype=");
    IIputdomio((short *)0,1,32,0,ERx("de"));
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,attid);
      IIgetdomio((short *)0,1,32,100,text);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 3714 "rfcat.sc" */	/* host code */
	/* Check if this row has the prompt string */
      	if((string = STindex(text,ERx("prompt"),100))!=NULL)
       	{
     	   	/* 
   		** Found the word 'prompt', now look for either the 
   		** single/double quote 
   		*/
   		state = WANT_QUOTE;
   		r_g_set(string);
	   	token = r_g_name();         /* should be "prompt" */
	   	token_type = r_g_skip();
	   	if (token_type == TK_QUOTE || token_type == TK_SQUOTE)
	   	{
			state = FOUND_PROMPT;
	   	}
	}
	else
	{
	   	/* The word 'prompt' is not in this row... */
	   	r_g_set(text);
	   	switch (state)
	   	{
	   		case WANT_PROMPT:
	      			break;
	   		case WANT_QUOTE:
	      			token_type = r_g_skip();
	      			if (token_type == TK_QUOTE || 
					token_type == TK_SQUOTE) 
	      			{   
		 			state = FOUND_PROMPT;
	      			}
	      			else 
	      			{
		 			/* error */
					IIUGerr (E_RF0064_Bad_decl_var,
						UG_ERR_FATAL, attid);
	      			}
	   	}
	}
	if (state == FOUND_PROMPT)
	{
	   	/* 
		** Set flag cause found a declared variable with 
		** prompt 
		*/
	   	if (!(*declared))
			*declared = TRUE;
	   	prompt_string = r_g_string(token_type);
	   	r_strpslsh(prompt_string);
	   	/* Put prompt string in the frame to be displayed */
/* # line 3167 "rfcat.qsc" */	/* loadtable */
  {
    if (IItbact(Nrvariable,var_tbl,1) != 0) {
      IItcoputio(prmpt_fld,(short *)0,1,32,0,prompt_string);
      IItcoputio((char *)"col_name",(short *)0,1,32,0,attid);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 3169 "rfcat.qsc" */	/* host code */
	   	state=WANT_PROMPT;	/* get next prompt */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	if (state != WANT_PROMPT)
	{
		/* error */
		IIUGerr (E_RF0064_Bad_decl_var, UG_ERR_FATAL, attid);
	}	
	if (*declared)
	{
/* # line 3180 "rfcat.qsc" */	/* display */
  {
    if (IIdispfrm(Nrvariable,(char *)"f") == 0) goto IIfdE7;
    goto IImuI7;
IIfdI7:;
IIfdB7:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 3185 "rfcat.qsc" */	/* host code */
			/* 
			** Build the parameter list from the values 
			** filled in from the form.
			*/
			params = IIRFmps_MakeParams (Nrvariable,  var_tbl,
			   ERx("col_name"), value_fld);
/* # line 3191 "rfcat.qsc" */	/* enddisplay */
          {
            if (1) goto IIfdF7;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 3196 "rfcat.qsc" */	/* host code */
			/* 
			** Get value, if any has been entered, to 
			** initialize the full prompt pop-up 
			*/
/* # line 3200 "rfcat.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Nrvariable,var_tbl,-2) != 0) {
              IItcogetio(&null_val,1,32,100,val_str,value_fld);
              IItcogetio((short *)0,1,32,0,prompt_string,prmpt_fld);
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,attid,(char *)
"col_name");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 3202 "rfcat.qsc" */	/* host code */
			if (null_val < 0)
			{
				val_str[0]= EOS;
			}
			if (!prompt_frm)  /* if popup not already initialized */
			{
				if (IIUFgtfGetForm(IIUFlcfLocateForm(), 
		         			(form = Nrprompt))!=OK)
				{  
		      			IIUGerr(E_RF002D_Cannot_locate_form,
						UG_ERR_ERROR,1,form);
					*break_state = TRUE;
		      			return (NULL);
				}
				else
				{
		      			prompt_frm = TRUE;
				}
			}
/* # line 3221 "rfcat.qsc" */	/* display */
          {
            if (IIdispfrm(Nrprompt,(char *)"f") == 0) goto IIfdE8;
            goto IImuI8;
IIfdI8:;
            if (IIfsetio((char *)0) == 0) goto IIfdE8;
            IIputfldio(colname_fld,(short *)0,1,32,0,attid);
            IIputfldio(value_fld,(short *)0,1,32,0,val_str);
            IIputfldio(fullpr_fld,(short *)0,1,32,0,prompt_string);
IIfdB8:
            while (IIrunform() != 0) {
              if (IIretval() == 1) {
                {
/* # line 3227 "rfcat.qsc" */	/* getform */
                  {
                    if (IIfsetio(Nrprompt) != 0) {
                      IIgetfldio(&null_val,1,32,100,val_str,value_fld);
                    } /* IIfsetio */
                  }
/* # line 3228 "rfcat.qsc" */	/* host code */
				if (null_val < 0)
				{
					val_str[0] = EOS;
				}
/* # line 3232 "rfcat.qsc" */	/* putrow */
                  {
                    if (IItbsetio(3,Nrvariable,(char *)"var",-2) != 0) {
                      IItcoputio(value_fld,(short *)0,1,32,0,val_str);
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
/* # line 3233 "rfcat.qsc" */	/* enddisplay */
                  {
                    if (1) goto IIfdF8;
                  }
                }
              } else if (IIretval() == 2) {
                {
/* # line 3240 "rfcat.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE8;
                  }
                }
              } else if (IIretval() == 3) {
                {
/* # line 3245 "rfcat.qsc" */	/* host code */
				FEhelp(H_PROMPT,
					ERget(S_RF006D_Full_Prompt));
                }
              } else {
                if (1) goto IIfdE8;
              } /* IIretval */
            } /* IIrunform */
IIfdF8:
            if (IIchkfrm() == 0) goto IIfdB8;
            goto IIfdE8;
IImuI8:
            if (IIinitmu() == 0) goto IIfdE8;
            if (IInmuact(ERget(FE_OK),ERget(F_RF0084_ok_full_prompt),2,2,1) ==
             0) goto IIfdE8;
            IIFRInternal(0);
            if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE8;
            if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,2) ==
             0) goto IIfdE8;
            IIFRInternal(0);
            if (IInfrskact(9,(char *)0,0,0,2) == 0) goto IIfdE8;
            if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,3) == 0) goto 
            IIfdE8;
            IIFRInternal(0);
            if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE8;
            if (IIendmu() == 0) goto IIfdE8;
            goto IIfdI8;
IIfdE8:;
            IIendfrm();
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 3254 "rfcat.qsc" */	/* host code */
			*break_state = TRUE;
/* # line 3255 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE7;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 3260 "rfcat.qsc" */	/* host code */
			FEhelp(H_DECLVAR,
				ERget(S_RF006A_Decl_Vars));
        }
      } else {
        if (1) goto IIfdE7;
      } /* IIretval */
    } /* IIrunform */
IIfdF7:
    if (IIchkfrm() == 0) goto IIfdB7;
    goto IIfdE7;
IImuI7:
    if (IIinitmu() == 0) goto IIfdE7;
    if (IInmuact(ERget(FE_OK),ERget(F_RF0082_decl_vars),2,2,1) == 0) goto 
    IIfdE7;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE7;
    if (IInmuact(ERget(F_RF0041_Full_Prompt),ERget(F_RF0083_full_prompt),2,2,2)
     == 0) goto IIfdE7;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,3) == 0) 
    goto IIfdE7;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,3) == 0) goto IIfdE7;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,4) == 0) goto IIfdE7;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE7;
    if (IIendmu() == 0) goto IIfdE7;
    goto IIfdI7;
IIfdE7:;
    IIendfrm();
  }
/* # line 3263 "rfcat.qsc" */	/* host code */
	}
	return (params);
}
/*
**  Name:  IIRFgpr_GetPrinter
**  Description:
**      Gets the printer name and the number of copies of the report to print
**	out.  Also asks user for the mode (interactive or batch) to run the
**	report in.  If batch, calls IIRFgdv_GetDeclaredVars to get the values
**	for all the declared variables in the report.  IIRFgdv_GetDeclaredVars 
**	is NOT called if the report is an Auto/Preview report since there 
**	cannot be any declared variables. 
*/
static
VOID
IIRFgpr_GetPrinter(printer, copy_no, break_state, batch_fl,
		   log_loc, log_name, params, d_vars)
  char *printer;
  i4 *copy_no;
bool	*break_state;		/* TRUE = user entered CANCEL */
bool	*batch_fl;		/* TRUE = run report in batch */
LOCATION *log_loc;		/* error log location */
  char *log_name;
char	**params;		/* parameter list */
bool	*d_vars;		/* TRUE = report has declared variables */
{
  i2 null_pr;
  i2 null_log;
  i2 null_batch;
  i2 null_copy;
  char run_batch[2];
  char *Nrprint;
  char *log_fld;
  char *copies_fld;
  char *printer_fld;
  char *batch_fld;
  char *batch1_fld;
  char *instr_fld;
  char *yes_string;
  char *fullwrd;
  char *instructions;
	static  bool    print_frm = FALSE; /* TRUE = Print popup initialized */
	char	*form;
	FILE	*fl_ptr;		   /* file pointer */
	LOCATION tmp_loc;		   /* temporary location */
	char	tmp_name[MAX_LOC + 1];	   /* temporary name */
	/* set up the pop-up variables */
	Nrprint = F_PRINT;
	log_fld = F_LOGNAME;
	printer_fld = F_PRINTER;
	copies_fld = F_COPIES;
	batch1_fld = F_BATCH1;
	batch_fld = F_BATCH;
	instr_fld = F_INSTR;
	yes_string = yn_tbl[0];
	if (!print_frm)	/* if popup not already initialized */
  {
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), (form = Nrprint))!=OK)
		{  
	      		IIUGerr (E_RF002D_Cannot_locate_form, UG_ERR_ERROR,
				 1, form);
			*break_state = TRUE;
	      		return;
		}
		else
		{
	      		print_frm = TRUE;
		}
  }
	/* make the error log prompt invisible */
/* # line 3340 "rfcat.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Nrprint,(char *)0, (char *)0) != 0) {
      IIstfsio(54,log_fld,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 3342 "rfcat.qsc" */	/* host code */
	/* 
	** Make the "wait" prompt invisible if UT_WAIT == UT_NOWAIT.  This
	** means this platform *cannot* spawn sub-processes or run a
	** batch job.  In other words, this platform has no way of implementing
	** a concurrent report, so don't even give the users the option.
	*/
	if (UT_WAIT == UT_NOWAIT)
	{
		/* make both lines of the "wait" prompt invisible */
/* # line 3351 "rfcat.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Nrprint,(char *)0, (char *)0) != 0) {
      IIstfsio(54,batch1_fld,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 3352 "rfcat.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Nrprint,(char *)0, (char *)0) != 0) {
      IIstfsio(54,batch_fld,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 3354 "rfcat.qsc" */	/* host code */
		/* Display instructions that do not have "wait" prompt */
		instructions = ERget (F_RF008C_pr_no_concurrent);
	}
	else
	{
		/* Display instructions that do have "wait" prompt */
		instructions = ERget (F_RF008B_pr_yes_concurrent);
	}
/* # line 3362 "rfcat.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Nrprint,(char *)"f") == 0) goto IIfdE9;
    goto IImuI9;
IIfdI9:;
    if (IIfsetio((char *)0) == 0) goto IIfdE9;
    IIputfldio(printer_fld,(short *)0,1,32,0,ERget(F_RF0018_default));
    IIputfldio(instr_fld,(short *)0,1,32,0,instructions);
    IIputfldio(copies_fld,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIputfldio(batch_fld,(short *)0,1,32,0,yes_string);
IIfdB9:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 3370 "rfcat.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio(&null_batch,1,32,1,run_batch,batch_fld);
            } /* IIfsetio */
          }
/* # line 3372 "rfcat.qsc" */	/* host code */
		 /* get the full word */
		if ((null_batch < 0) || ((fullwrd =
			IIRFfwd_FullWord (run_batch, 2, yn_tbl)) == NULL))
		{
			IIUGerr(E_RF009F_batch_report, UG_ERR_ERROR, 0);
/* # line 3377 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB9;
          }
/* # line 3378 "rfcat.qsc" */	/* host code */
		}
/* # line 3379 "rfcat.qsc" */	/* putform */
          {
            if (IIfsetio(Nrprint) != 0) {
              IIputfldio(batch_fld,(short *)0,1,32,0,fullwrd);
            } /* IIfsetio */
          }
/* # line 3381 "rfcat.qsc" */	/* host code */
		/* Does user want to run report in batch? */
		if (!IIUGyn (fullwrd, NULL))
		{
			/*
			** If want to run in batch, make the error log 
			** prompt visible
			*/
/* # line 3388 "rfcat.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,Nrprint,(char *)0, (char *)0) != 0) {
              IIstfsio(54,log_fld,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
              0));
            } /* IIiqset */
          }
/* # line 3389 "rfcat.qsc" */	/* host code */
			*batch_fl = TRUE;
		}
		else
		{
			/*
			** If interactive mode, make the error log 
			** prompt invisible
			*/
/* # line 3397 "rfcat.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,Nrprint,(char *)0, (char *)0) != 0) {
              IIstfsio(54,log_fld,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
              1));
            } /* IIiqset */
          }
/* # line 3398 "rfcat.qsc" */	/* host code */
			*batch_fl = FALSE;
		}
/* # line 3400 "rfcat.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB9;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 3405 "rfcat.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio(&null_pr,1,32,0,printer,printer_fld);
              IIgetfldio((short *)0,1,32,1,run_batch,batch_fld);
              IIgetfldio(&null_copy,1,30,4,copy_no,copies_fld);
              IIgetfldio(&null_log,1,32,0,log_name,log_fld);
            } /* IIfsetio */
          }
/* # line 3409 "rfcat.qsc" */	/* host code */
		/* check that printer and number of copies have been entered */
		if (null_pr < 0) 
		{
			IIUGerr (E_RF00A0_enter_printer, UG_ERR_ERROR, 0);
/* # line 3413 "rfcat.qsc" */	/* resume */
          {
            IIresfld(printer_fld);
            if (1) goto IIfdB9;
          }
/* # line 3414 "rfcat.qsc" */	/* host code */
		}
		if (null_copy < 0) 
		{
			IIUGerr (E_RF00A1_enter_copy, UG_ERR_ERROR, 0);
/* # line 3418 "rfcat.qsc" */	/* resume */
          {
            IIresfld(copies_fld);
            if (1) goto IIfdB9;
          }
/* # line 3419 "rfcat.qsc" */	/* host code */
		}
		if (!IIUGyn (run_batch, NULL))
		{
			if (null_log < 0)
			{
				IIUGerr (E_RF0072_Need_errorlog, UG_ERR_ERROR,
					 0);
/* # line 3426 "rfcat.qsc" */	/* resume */
          {
            IIresfld(log_fld);
            if (1) goto IIfdB9;
          }
/* # line 3427 "rfcat.qsc" */	/* host code */
			}
			else
			{
				/* 
				** Verify if the error log name is a valid
				** path/filename by creating a temp file in the
				** specified directory.  LOfroms checks for a 
				** valid filename. If the temp file can be 
				** created, then the directory is writeable.
				** Otherwise, tell user to enter a different
				** name.  Delete the temp file created since 
				** the real file will be created when the report
				** runs in batch mode.
				*/
				if (LOfroms(PATH & FILENAME, log_name, 
					log_loc) != OK)
				{
					IIUGerr (E_RF0082_bad_errorlog,
						 UG_ERR_ERROR, 0);
/* # line 3446 "rfcat.qsc" */	/* resume */
          {
            IIresfld(log_fld);
            if (1) goto IIfdB9;
          }
/* # line 3447 "rfcat.qsc" */	/* host code */
				}
				LOcopy (log_loc, tmp_name, &tmp_loc);
				LOuniq (ERx("ra"),ERx("tmp"),&tmp_loc);
				if (SIopen(&tmp_loc, ERx("w"), &fl_ptr) != OK)
				{
					IIUGerr (E_RF0084_cannot_open_file,
						 UG_ERR_ERROR,0);
/* # line 3454 "rfcat.qsc" */	/* resume */
          {
            IIresfld(log_fld);
            if (1) goto IIfdB9;
          }
/* # line 3455 "rfcat.qsc" */	/* host code */
				}
				SIclose(fl_ptr);
				LOdelete(&tmp_loc);
				if (St_style == RS_NULL)
				{
					/*
					** If NOT executing an AutoReport, 
					** then get any declared variables.
					*/
					*params = IIRFgdv_GetDeclaredVars
						(break_state, d_vars);
					if (*break_state)
/* # line 3468 "rfcat.qsc" */	/* resume */
          {
            if (1) goto IIfdB9;
          }
/* # line 3469 "rfcat.qsc" */	/* host code */
				}
				else
				{
					/* no declared variables in an 
					** AutoReport */
					*d_vars = FALSE;
				}
			}
		}
		else
		{
			/* run the report in interactive mode */
			*d_vars = FALSE;
		}
/* # line 3483 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE9;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 3489 "rfcat.qsc" */	/* host code */
		*break_state = TRUE;
/* # line 3490 "rfcat.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE9;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 3495 "rfcat.qsc" */	/* host code */
		FEhelp(H_PRINT, ERget(S_RF006B_Get_Printer));
        }
      } else {
        if (1) goto IIfdE9;
      } /* IIretval */
    } /* IIrunform */
IIfdF9:
    if (IIchkfrm() == 0) goto IIfdB9;
    goto IIfdE9;
IImuI9:
    if (IIFRafActFld(batch_fld,0,1) == 0) goto IIfdE9;
    if (IIinitmu() == 0) goto IIfdE9;
    if (IInmuact(ERget(FE_OK),ERget(F_RF0085_enter_printer),2,2,2) == 0) goto 
    IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE9;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,3) == 0) 
    goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,3) == 0) goto IIfdE9;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,4) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE9;
    if (IIendmu() == 0) goto IIfdE9;
    goto IIfdI9;
IIfdE9:;
    IIendfrm();
  }
/* # line 3497 "rfcat.qsc" */	/* host code */
}
/*
**  Name:  IIRFgfn_GetFilename
**  Description:
**      Gets the file name to put the report output into.  Also asks user for 
**	the mode (interactive or batch) to run the report in.  If batch, calls 
**	IIRFgdv_GetDeclaredVars to get the values for all the declared 
**	variables in the report.  IIRFgdv_GetDeclaredVars is NOT called if the
**	report is an Auto/Preview report since there cannot be any declared 
**	variables.
*/
static
VOID
IIRFgfn_GetFilename(filename, break_state, batch_fl,
		    log_loc, logname, params, d_vars)
  char *filename;
bool	*break_state;
bool	*batch_fl;		/* TRUE = run report in batch */
LOCATION *log_loc;
  char *logname;
char	**params;
bool	*d_vars;		/* TRUE = report has declared variables */
  {
    i2 null_file;
    i2 null_btch;
    i2 null_lg;
    char batch_ans[2];
    char *Nrfile;
    char *logfld;
    char *batchfld;
    char *batch1fld;
    char *fnamefld;
    char *instrfld;
    char *yes_str;
    char *fullword;
    char *instr;
	char	*form;
	LOCATION file_loc;
	LOCATION tmp_loc;		   /* temporary location */
	FILE	*fl_ptr;		   /* file pointer */
	char	tmp_name[MAX_LOC + 1];	   /* temporary name */
	static  bool    file_frm = FALSE;  /* TRUE=Filename popup initialized */
	/* set up the pop-up variables */
	Nrfile = F_FILE;
	logfld = F_LOGNAME;
	batchfld = F_BATCH;
	batch1fld = F_BATCH1;
	fnamefld = F_FNAME;
	instrfld = F_INSTR;
	yes_str = yn_tbl[0];
	if (!file_frm)	/* if popup not already initialized */
    {
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), (form = Nrfile))!=OK)
		{  
	      		IIUGerr (E_RF002D_Cannot_locate_form, UG_ERR_ERROR,
				 1, form);
			*break_state = TRUE;
	      		return; 
		}
		else
		{
	      		file_frm = TRUE;
		}
    }
	/* make the error log prompt invisible */
/* # line 3567 "rfcat.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Nrfile,(char *)0, (char *)0) != 0) {
        IIstfsio(54,logfld,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 3569 "rfcat.qsc" */	/* host code */
	/* 
	** Make the "wait" prompt invisible if UT_WAIT == UT_NOWAIT.  This
	** means this platform *cannot* spawn sub-processes or run a
	** batch job.  In other words, this platform has no way of implementing
	** a concurrent report, so don't even give the users the option.
	*/
	if (UT_WAIT == UT_NOWAIT)
	{
		/* make both lines of the "wait" prompt invisible */
/* # line 3578 "rfcat.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Nrfile,(char *)0, (char *)0) != 0) {
        IIstfsio(54,batch1fld,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 3579 "rfcat.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,Nrfile,(char *)0, (char *)0) != 0) {
        IIstfsio(54,batchfld,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 3581 "rfcat.qsc" */	/* host code */
		/* Display instructions that do not have "wait" prompt */
		instr = ERget(F_RF008E_fi_no_concurrent);
	}
	else
	{
		/* Display instructions that do have "wait" prompt */
		instr = ERget(F_RF008D_fi_yes_concurrent);
	}
/* # line 3590 "rfcat.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(Nrfile,(char *)"f") == 0) goto IIfdE10;
      goto IImuI10;
IIfdI10:;
      if (IIfsetio((char *)0) == 0) goto IIfdE10;
      IIputfldio(batchfld,(short *)0,1,32,0,yes_str);
      IIputfldio(instrfld,(short *)0,1,32,0,instr);
IIfdB10:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 3594 "rfcat.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio(&null_btch,1,32,1,batch_ans,batchfld);
              } /* IIfsetio */
            }
/* # line 3596 "rfcat.qsc" */	/* host code */
		 /* get the full word */
		if ((null_btch < 0) || ((fullword =
			IIRFfwd_FullWord (batch_ans, 2, yn_tbl)) == NULL))
		{
			IIUGerr(E_RF009F_batch_report, UG_ERR_ERROR, 0);
/* # line 3601 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB10;
            }
/* # line 3602 "rfcat.qsc" */	/* host code */
		}
/* # line 3603 "rfcat.qsc" */	/* putform */
            {
              if (IIfsetio(Nrfile) != 0) {
                IIputfldio(batchfld,(short *)0,1,32,0,fullword);
              } /* IIfsetio */
            }
/* # line 3605 "rfcat.qsc" */	/* host code */
		/* Does user want to run report in batch? */
		if (!IIUGyn (fullword, NULL))
		{
			/*
			** If want to run in batch, make the error log 
			** prompt visible
			*/
/* # line 3612 "rfcat.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Nrfile,(char *)0, (char *)0) != 0) {
                IIstfsio(54,logfld,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 3613 "rfcat.qsc" */	/* host code */
			*batch_fl = TRUE;
		}
		else
		{
			/*
			** If interactive mode, make the error log 
			** prompt invisible
			*/
/* # line 3621 "rfcat.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,Nrfile,(char *)0, (char *)0) != 0) {
                IIstfsio(54,logfld,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 3622 "rfcat.qsc" */	/* host code */
			*batch_fl = FALSE;
		}
/* # line 3624 "rfcat.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB10;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 3629 "rfcat.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio(&null_file,1,32,0,filename,fnamefld);
                IIgetfldio((short *)0,1,32,1,batch_ans,batchfld);
                IIgetfldio(&null_lg,1,32,0,logname,logfld);
              } /* IIfsetio */
            }
/* # line 3632 "rfcat.qsc" */	/* host code */
		/* Verify the file name as a valid path/filename. */
		if ((null_file <0) || 
			(LOfroms(PATH & FILENAME, filename, &file_loc) != OK))
		{
			IIUGerr (E_RF0083_bad_filename, UG_ERR_ERROR, 0);
/* # line 3638 "rfcat.qsc" */	/* resume */
            {
              IIresfld(fnamefld);
              if (1) goto IIfdB10;
            }
/* # line 3639 "rfcat.qsc" */	/* host code */
		}
		LOcopy (&file_loc, tmp_name, &tmp_loc);
		LOuniq (ERx("ra"),ERx("tmp"),&tmp_loc);
		if (SIopen(&tmp_loc, ERx("w"), &fl_ptr) != OK)
		{
			IIUGerr (E_RF0084_cannot_open_file, UG_ERR_ERROR, 0);
/* # line 3645 "rfcat.qsc" */	/* resume */
            {
              IIresfld(fnamefld);
              if (1) goto IIfdB10;
            }
/* # line 3646 "rfcat.qsc" */	/* host code */
		}
		SIclose(fl_ptr);
		LOdelete(&tmp_loc);
		if (!IIUGyn (batch_ans, NULL)) 
		{
			if (null_lg < 0)
			{
				IIUGerr (E_RF0072_Need_errorlog, UG_ERR_ERROR,
					 0);
/* # line 3656 "rfcat.qsc" */	/* resume */
            {
              IIresfld(logfld);
              if (1) goto IIfdB10;
            }
/* # line 3657 "rfcat.qsc" */	/* host code */
			}
			else
			{
				/* 
				** Verify if the error log name is a valid
				** path/filename by creating a temp file in the
				** specified directory.  LOfroms checks for a 
				** valid filename. If the temp file can be 
				** created, then the directory is writeable.
				** Otherwise, tell user to enter a different
				** name.  Delete the temp file created since 
				** the real file will be created when the report
				** runs in batch mode.
				*/
				if (LOfroms(PATH & FILENAME, logname, 
					log_loc) != OK)
				{
					IIUGerr (E_RF0082_bad_errorlog,
						 UG_ERR_ERROR, 0);
/* # line 3676 "rfcat.qsc" */	/* resume */
            {
              IIresfld(logfld);
              if (1) goto IIfdB10;
            }
/* # line 3677 "rfcat.qsc" */	/* host code */
				}
				LOcopy (log_loc, tmp_name, &tmp_loc);
				LOuniq (ERx("ra"),ERx("tmp"),&tmp_loc);
				if (SIopen(&tmp_loc, ERx("w"), &fl_ptr) != OK)
				{
					IIUGerr (E_RF0084_cannot_open_file,
						 UG_ERR_ERROR,0);
/* # line 3684 "rfcat.qsc" */	/* resume */
            {
              IIresfld(logfld);
              if (1) goto IIfdB10;
            }
/* # line 3685 "rfcat.qsc" */	/* host code */
				}
				SIclose(fl_ptr);
				LOdelete(&tmp_loc);
				if (St_style == RS_NULL)
				{
					*params = IIRFgdv_GetDeclaredVars
						(break_state, d_vars);
					if (*break_state)
/* # line 3694 "rfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB10;
            }
/* # line 3695 "rfcat.qsc" */	/* host code */
				}
				else
				{
					/* no declared variables in an 
					** AutoReport */
					*d_vars = FALSE;
				}
			}
		}
		else
		{
			/* run the report in interactive mode */
			*d_vars = FALSE;
		}
/* # line 3709 "rfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE10;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 3715 "rfcat.qsc" */	/* host code */
		*break_state = TRUE;
/* # line 3716 "rfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE10;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 3721 "rfcat.qsc" */	/* host code */
		FEhelp(H_FILE, ERget(S_RF006C_Send_File));
          }
        } else {
          if (1) goto IIfdE10;
        } /* IIretval */
      } /* IIrunform */
IIfdF10:
      if (IIchkfrm() == 0) goto IIfdB10;
      goto IIfdE10;
IImuI10:
      if (IIFRafActFld(batchfld,0,1) == 0) goto IIfdE10;
      if (IIinitmu() == 0) goto IIfdE10;
      if (IInmuact(ERget(FE_OK),ERget(F_RF0086_enter_filename),2,2,2) == 0) 
      goto IIfdE10;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE10;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,3) == 0) 
      goto IIfdE10;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,3) == 0) goto IIfdE10;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,4) == 0) goto IIfdE10;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE10;
      if (IIendmu() == 0) goto IIfdE10;
      goto IIfdI10;
IIfdE10:;
      IIendfrm();
    }
  }
/* # line 3727 "rfcat.qsc" */	/* host code */
static
VOID
rfabfcr(form)
  char *form;
{
  char *rep_type = F_RFAFTYPE;
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), form)!=OK)
	{
		IIUGerr(E_RF002D_Cannot_locate_form, UG_ERR_ERROR, 1, F_RBFABF);
		return ;
	}
/* # line 3740 "rfcat.qsc" */	/* inittable */
  {
    if (IItbinit(form,rep_type,(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 3741 "rfcat.qsc" */	/* loadtable */
  {
    if (IItbact(form,rep_type,1) != 0) {
      IItcoputio((char *)"source",(short *)0,1,32,0,ERget(S_RF0065_Dup_Choice));
      IItcoputio((char *)"expl",(short *)0,1,32,0,ERget(S_RF0066_Dup_Expl));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 3743 "rfcat.qsc" */	/* loadtable */
  {
    if (IItbact(form,rep_type,1) != 0) {
      IItcoputio((char *)"source",(short *)0,1,32,0,ERget(S_RF0067_Tbl_Choice));
      IItcoputio((char *)"expl",(short *)0,1,32,0,ERget(S_RF0068_Tbl_Expl));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 3745 "rfcat.qsc" */	/* loadtable */
  {
    if (IItbact(form,rep_type,1) != 0) {
      IItcoputio((char *)"source",(short *)0,1,32,0,ERget(S_RF0069_JD_Choice));
      IItcoputio((char *)"expl",(short *)0,1,32,0,ERget(S_RF0070_JD_Expl));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 3747 "rfcat.qsc" */	/* host code */
}
/*
** Name:        IIRFSetReadOnly
**
** Description:
**      If we are in expert mode ('-e' flag has been specified), and
**      there are more than one rows being displayed in the RBF catalog
**      the user must have used a wild card, so make the form read only.
**
** History:
**      4/11/90 (martym)  Written for RBF.
**
** Parameters:
**      None.
**
** Returns:
**      o Will return OK if wild card is used and objects are found.
**      o Will return FAIL if not in expert mode or user specified object
**        was not found.
**
*/
static
STATUS
IIRFSetReadOnly()
{
  i4 rowcnt = 0;
	if (!Rbf_noload)
		return(FAIL);
/* # line 3782 "rfcat.qsc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,_iicatalog,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&rowcnt,31,_cattable,0);
    } /* IIiqset */
  }
/* # line 3783 "rfcat.qsc" */	/* host code */
	if (rowcnt > 1)
	{
/* # line 3785 "rfcat.qsc" */	/* set_frs */
  {
    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(21,(char *)0,0,(short *)0,1,32,0,(char *)"r");
    } /* IIiqset */
  }
/* # line 3786 "rfcat.qsc" */	/* host code */
		return(OK);
	}
	return(FAIL);
}
/*
**  Name:  IIRFarp_ArchiveReport
**  Description:  Displays pop-up that prompts user to enter the name of a
**		  file to put the archived report.
*/
static
VOID
IIRFarp_ArchiveReport(fname)
  char *fname;
  {
	char		*form;		 /* form name */
	static bool	archive_frm = FALSE;
					 /* TRUE = archive pop-up initialized*/
    char *Narchive;
    char *archivefld;
    i2 null_file;
    char *fnamefld;
	LOCATION file_loc;
	LOCATION tmp_loc;                  /* temporary location */
	FILE    *fl_ptr;                   /* file pointer */
	char    tmp_name[MAX_LOC + 1];     /* temporary name */
        Narchive = F_ARCHIVE;
	archivefld = F_FNAME;
	fnamefld = F_FNAME;
	/* if popup not already initialized */
	if (!archive_frm)  
    {
		if (IIUFgtfGetForm(IIUFlcfLocateForm(),
			(form = Narchive))!=OK)
		{  
			IIUGerr (E_RF002D_Cannot_locate_form, UG_ERR_ERROR,
				 1, form);
			return;
		}
		else
		{
			archive_frm = TRUE;
		}
    }
/* # line 3838 "rfcat.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(Narchive,(char *)"f") == 0) goto IIfdE11;
      goto IImuI11;
IIfdI11:;
IIfdB11:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 3842 "rfcat.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio(&null_file,1,32,0,fname,archivefld);
              } /* IIfsetio */
            }
/* # line 3844 "rfcat.qsc" */	/* host code */
		 /* Verify the file name as a valid path/filename. */
		if (LOfroms(PATH & FILENAME, fname, &file_loc) != OK)
		{
			IIUGerr (E_RF0083_bad_filename, UG_ERR_ERROR, 0);
/* # line 3849 "rfcat.qsc" */	/* resume */
            {
              IIresfld(fnamefld);
              if (1) goto IIfdB11;
            }
/* # line 3850 "rfcat.qsc" */	/* host code */
		}
		LOcopy (&file_loc, tmp_name, &tmp_loc);
		LOuniq (ERx("ra"),ERx("tmp"),&tmp_loc);
		if (SIopen(&tmp_loc, ERx("w"), &fl_ptr) != OK)
		{
			IIUGerr (E_RF0084_cannot_open_file, UG_ERR_ERROR, 0);
/* # line 3856 "rfcat.qsc" */	/* resume */
            {
              IIresfld(archivefld);
              if (1) goto IIfdB11;
            }
/* # line 3857 "rfcat.qsc" */	/* host code */
		}
		SIclose(fl_ptr);
		LOdelete(&tmp_loc);
/* # line 3860 "rfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE11;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 3866 "rfcat.qsc" */	/* host code */
		*fname = EOS;
/* # line 3867 "rfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE11;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 3872 "rfcat.qsc" */	/* host code */
		FEhelp(H_ARCHIVE, ERget(S_RF0015_Archiving_Report));
          }
        } else {
          if (1) goto IIfdE11;
        } /* IIretval */
      } /* IIrunform */
IIfdF11:
      if (IIchkfrm() == 0) goto IIfdB11;
      goto IIfdE11;
IImuI11:
      if (IIinitmu() == 0) goto IIfdE11;
      if (IInmuact(ERget(FE_OK),ERget(F_RF0087_archive_rep),2,2,1) == 0) goto 
      IIfdE11;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE11;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,2) == 0) 
      goto IIfdE11;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,2) == 0) goto IIfdE11;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,3) == 0) goto IIfdE11;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE11;
      if (IIendmu() == 0) goto IIfdE11;
      goto IIfdI11;
IIfdE11:;
      IIendfrm();
    }
  }
