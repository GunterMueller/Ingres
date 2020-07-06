# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1986, 2004 Ingres Corporation
**	All rights reserved.
*/
/* # include's */
# include	<compat.h>
# include	<iisqlda.h>		 
# include	<ci.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	<st.h>
# include <ui.h>
/*
# include	<ui.h>
*/
# include	<uf.h>
# include	<tu.h>
# include	<erfe.h>
# include	"ertu.h"
# include	"tuconst.h"
# include       <stdprmpt.h>
# include <uigdata.h>
/*
# include	<uigdata.h>
*/
/**
** Name:	tumain.qc -	Main procedure for Tables Utility.
**
** Description:
**	This file was extracted from tblutils.qc. The routine, tu_main,
**	was called listtables then. It drives the top frame of the
**	Tables Utility. This file defines:
**
**	tu_main		Driver for the top frame of the Tables Utility.
**
** History:
**	1/7/86 (bab)	Fix for bug 7063.
**	27-mar-1987	(daver) Extracted from tblutils.qc.
**	21-apr-1987	(daver) Added new system catalogs access.
**	24-apr-1987	(daver) Added end/quit code for QBF.
**	22-oct-1987	(peter) Take out FEmsg and change y and n.
**	6/88		(bobm)	pass in title for form
**	19-aug-1988	(peter) update column names in catalogs for 6.1.
**	12-dec-1989	(pete)	Remove menuitems Top, Bottom and Find;
**			Add (validate=0, activate=0) to Help, End and Quit.
**	17-jan-1993	(rogerl) Juggle normal/un names a bit.  Increase
**			some array sizes to hold unormalized names.  Use EOS.
**	24-mar-1993	(mgw) change repitem case to grab the table name off
**			the screen into the same buffer to be re-quoted. This
**			fixes a problem where reports were being run on the
**			last table examined/queried/etc. or on the null string
**			if no other table was examined/queried/etc.
**	15-apr-1993	(rogerl) Before calling report or qbf subsystem, if
**			we's runn'n against a 6.5 DB, assemble own.tab for
**			passing along so we query the right table. Bug 50690.
**	9-jun-1993	(rogerl) At a cancel, init second_pass_emode to TRUE
**			so, if we are in expert mode, we can allow selection
**			of a delimited id from the table field in utils.qsc
**      6-apr-1995     (harpa06) Bug #67891: Added error message to let STAR 
**			users know that adding default values to tables in a 
**			STAR database are currently not supported since the 
**			STAR catalogs are currently at level 6.2. At 6.2, 
**			default values are not stored in STAR catalogs. 
**              	This fix is to be removed in OPING2.0
**	28-jun-1995	(rudtr01) removed previous fix (bug 67891) to avoid
**			problems with gateways & 6.4 backend systems.
**      23-Jul-1998	(horda03) Bug 92973
**                      Delimited table names are enclosed in one
**			double quote ("), this is a problem as the command lines
**                      generated for QBF and REPORT are also enclosed within quotes.
**                      Need to enclose Delimited table names in two double quotes
**                      (e.g ""delim table"").
**      29-Oct-1998	(horda03) Bug 94036
**			The above changed broke the TABLES utility when it attempted
**			to invoke the QBF utility on UNIX platforms. I have modified
**			the above change to be VMS specific.
**	16-Nov-1998	(kinte01) 
**			Cross integrate change 438485 from 1.2 for bug 94036
**			Extend the fix to take care of table owners also
**			being delimited identifiers. The cross integration
**			plus plus the extension to the fix is also the
**			correct solution for bug 82672 
**	01-dec-1999 (kitch01)
**		Bug 98490. In the course of the above 3 VMS fixes a mixture
**		of delimited and non-delimited owner/tablename's was inadvertantly
**		used to compare when returning from QBF or Reporter. This meant that
**		the compare failed resulting in the current row always being set
**		to row 1 on return from these 2 utilities. I have amened this to 
**		always use the undelimited forms.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	21-jan-03 (hayke02)
**	    Remove local prototyping of IIUFccConfirmChoice() - this is now
**	    done in fe.h.
**	09-apr-2004 (abbjo03)
**	    When quoting a table/owner name that includes an apostrophe
**	    (single quote), the names should only be surrounded by pairs
**	    (rather than triples) of double quotes.
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**/
/* # define's */
/* extern's */
FUNC_EXTERN	i4	FEinqerr();
FUNC_EXTERN	void	mqbfstart();
FUNC_EXTERN	void	reportstart();
FUNC_EXTERN	bool	FEecats();
FUNC_EXTERN	void	FEsetecats();
FUNC_EXTERN	bool	ltcheck();
FUNC_EXTERN	void	gettbls();
FUNC_EXTERN	void	newtable();
FUNC_EXTERN     void    IITUstlSetTitle();
FUNC_EXTERN	void	IITUtbiTableinfo();
FUNC_EXTERN	i4	IITUfr_FindRow();
FUNC_EXTERN	i4 IITUstf_ScrollTF();
/* static's */
static		bool	tu_emptycats;
static		bool	formready = FALSE;
static
  char *create_menuitem;
static
  char *destroy_menuitem;
static		char	_Wildcard[] = ERx("%");
/* following could be put in .roc file */
static
  char _Form[]= ERx("tutopfr"), _Tblfld[]= ERx("tbllist"), _ColName[]= ERx("table")
  , _ColOwner[]= ERx("owner"), _ColType[]= ERx("type"), _ColicType[]= ERx("ictype")
  ;
static	char    Empty_str[]	= ERx("");
static	char	_TuccDestroyHelpFile[] = ERx("tuccdest.hlp");
GLOBALREF       bool    second_pass_emode;
/*{
** Name:	tu_main - main entry point for tables utility
**
** Description:
**	This routine is the interface for calling the Tables Utility as
**	a subroutine. It defines the main, or top screen for the utility.
**	The Tables Utility may also be called as a seperate process via
**	the standard utexe protocol.
**
**	If the Tables Utility is to be called as a subroutine, a few
**	parameters are expected, yet optional: database name, optional
**	menu item information, and a flag indicating which (if any) 'special'
**	menu itmes should be displayed.
**
**	If the database name was passed, it was displayed is the upper
**	right hand corner. Due to the nature of distributed databases, the
**	length of the database name could get us in trouble. This feature
**	has been taken out of the Tables Utility, but the parameter remains
**	for backward compatability. Pass the empty string.
**
**	An optional menu item may also be passed to the Tables Utility. This
**	is needed because the old tables utility in qbf could perform a "Go"
**	function (running qbf on the current table). To take advantage of this
**	feature, a properly filled in TU_MENUSTRUCT must be passed to tu_main.
**
**	The TU_MENUSTRUCT contains the name of the menu item, the address
**	of the routine the CALLER wants to have executed when the menu item
**	is selected, a PTR to a structure containing the routine's parameters,
**	an explanation string for Lotus Menus, the name of the table the user
**	wishes to operate on, and a bool indicating if "End" or "Quit" was
**	used to exit the Tables Utility. The structure looks like:
**
**	    typedef struct
**	    {
**		char	*name;		( menuitem name to be displayed )
**		void	(*rout)();	( caller's routine to be executed )
**		PTR	mem;		( memory for routine's parameters )
**		char	*expl;		( explanation string for lotus menus )
**		bool	end_not_quit;	( TRUE if end selected, FALSE if quit )
**	    } TU_MENUSTRUCT;
**
**	One note about the caller's routine's parameters. The routine will
**	be passed two parameters: the mem field, above, and the table name
**	(a char *) which should be operated on. To pass a set of parameters,
**	the caller should use a structure, then cast the PTR to point to it.
**	The caller's host routine should cast the PTR parameter structure
**	back into whatever the original structure was before accessing the
**	parameters.
**
**	This optional menu item, if provided, will appear on position 4 on
**	the menu line following menu items "Create", "Destroy", and "Examine".
**	It will be mapped to FRSkey4, or the 'Enter' key, to be consistant
**	with the "Go" menu item from other subsystems.
**
**	The third parameter is a bit map integer indicating which 'special'
**	menu items are to be displayed as options for our users. The Tables
**	Utility from INGMENU is allowed to call QBF or REPORT WRITER on the
**	table currently pointed to. This functionality is not currently
**	available in ABF's, QBF's or RTINGRES' tables utilities, but may be
**	desired later. From QBF's Tables Utility, one can "End" from Tables
**	but remain in QBF, or "Quit" the program entirely and return to the
**	shell. Hence the need for a 'special' flag indicating which special
**	flavor we're running.
**
**	This third parameter should simply be zero (0) if no 'special'
**	menu item is desired; or contain the proper menuitem flag (or flags,
**	or'ed together). The possible values (currently) are TU_NONE,
**	TU_QBF_ENABLED, TU_REPORT_ENABLED, TU_QUIT_ENABLED, and
**	TU_ONLYQUIT_ENABLED, the latter disabling the "End" menu item while
**	enabling the "Quit" menutiem.
**
**	For example, our caller wants a specific menu item called "DoIt" to
**	call some host routine from the Tables Utility.
**
**	The initialization and call to tu_main might look like:
**
**	TU_MENUSTRUCT	menuitm;
**	char		itemname[20];
**	void		some_host_routine();
**
**		STcopy("DoIt",itemname);
**		menuitm.name = itemname;
**		menuitm.rout = some_host_routine();
**
**		(params is a struct for the routine's parameters)
**		params.data1 = something;
**		params.data2 = anotherthing;
**		menuitm.mem = (PTR)(&params);
**
**		tu_main(dbname,&menuitm,TU_NONE);
**
**	While the host routine might look like:
**
**	void
**	some_host_routine(params, tblname)
**	PTR	params;
**	char	*tblname;
**	{
**		REAL_PARAM_STRUCT *real;
**
**		real = (REAL_PARAM_STRUCT *)params;
**			...
**
** Inputs:
**	dbname			name of the current database in use; or null
**				(NO LONGER USED)
**
**	menu_item		the TU_MENUSTRUCT for the optional menu item.
**
**		.name		name of the menu item to be displayed.
**		.rout		pointer to the routine to be executed.
**		.mem		a PTR to a structure containing the
**				    routine's parameters.
**		.expl		an explanation string for Lotus Menus.
**
**	special			bitmask parameter which tells which, if any,
**				    special menuitems should be available.
**				    values are:
**					TU_NONE
**					TU_QBF_ENABLED
**					TU_REPORT_ENABLED
**					TU_QUIT_ENABLED
**					TU_ONLYQUIT_ENABLED
**					TU_EMPTYCATS
**
**	title_prefix		prefix to use as basis for building title
**				for tables utility screens.  Pass in
**				NULL to use default ("TABLES -")
**
** Outputs:
**	menu_item		the TU_MENUSTRUCT for the optional menu item.
**
**		.tblname	Name of the table to be operated on.
**		.end_not_quit	TRUE if end was selected, FALSE if quit was.
**
**
** History:
**	13-mar-1987	(daver) First Written.
**	24-apr-1987	(daver) Added end/quit code for QBF.
**	4-oct-1989	(pete)  Change Create and Destroy menuitems to
**				variables.
**	18-jan-1990	(pete)	Fix bug (no number -- caught during 6.4 alpha
**				testing) caused by create_menuitem being
**				declared as both local & static var.
**	2/28-90		(pete)  Change Destroy verifications to use
**				IIUFccConfirmChoice, rather than IIUFver.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	9/14/90	(pete)	Change order of Report submenu items (report styles)
**			to be same as new RBF.
**	4-jan-1991	(pete)	Fix bug 31786. Bad SCROLL TO after drop a
**				table when only its indexes follow it in the
**				table field.
**	03-mar-92 (leighb) DeskTop Porting Change:
**		Increased size of quititem[] and qbfitem[] to 30 bytes each.
**	18-jun-1992 (mgw)
**		Added code to destroy synonyms too, not just tables, view,
**		and indexes.
**	22-mar-94 (donc)
**		Replace CI_XXXX_RUNTIME references w/ CI_CREATE_TABLE
**      06-apr-95 (harpa06)
**	        Bug #67891: Added error message to let STAR users know that 
**		adding default values to tables in a STAR database are currently
**		not supported since the STAR catalogs are currently at level 
**		6.2. At 6.2, default values are not stored in STAR catalogs. 
**		This fix is to be removed in OPING2.0
**	28-jun-95 (rudtr01) 
**		removed previous fix (bug 67891) to avoid
**		problems with gateways & 6.4 backend systems.
**      23-Jul-1998 (horda03) Bug 92973.
**              If the table name is a delimted table, then enclose the
**		name within two quotes ("").
**      29-10-1998 (horda03) Bug 94036
**              Modified the above change to be VMS specific.
**	01-Dec-2000 (kitch01)
**		Bug 98490. Amend store/comparison of table owner/name to be
**		consistent. Non-delimited form is now used.
*/
void
tu_main(dbase, menu_item, special,title_prefix)
char		*dbase;			/* database name -- NO LONGER used */
TU_MENUSTRUCT	*menu_item;		/* optional dynamic menuitems */
i4		special;		/* flag field for dynamic menuitems */
  char *title_prefix;
  {
    char itemname[256];
    char qbfitem[30];
    char repitem[20];
    char quititem[30];
    char enditem[30];
    char tblname[FE_UNRML_MAXNAME+1];
    char rrowner[FE_MAXNAME+1];
    char title[TU_TITLE_SIZE +1];
    char ltable[FE_MAXNAME+1];
    char lowner[FE_MAXNAME+1];
	char	owndottbl[FE_MAXTABNAME+1];
    i4 next_activate;
    i4 prev_activate;
    i4 prev_validate;
    i4 tbltype;
	i4	count;
    i4 maxrows;
    i4 newmaxrows;
    i4 rownumber;
	i4	err_code;
	bool	restart = FALSE;
    char nrmltblname[ FE_MAXNAME + 1 ];
    char nrmlowner[FE_MAXNAME+1];
	if (menu_item == NULL)
	{
		itemname[0] = EOS;
	}
	else
	{
		STcopy(menu_item->name, itemname);
	}
	quititem[0] = EOS;
	STcopy(ERget(FE_End), enditem);
	if ( (special & TU_QUIT_ENABLED) || (special & TU_ONLYQUIT_ENABLED) )
	{
		STcopy(ERget(FE_Quit), quititem);
		if (special & TU_ONLYQUIT_ENABLED)
		{
			enditem[0] = EOS;
		}
	}
	tu_emptycats = (special & TU_EMPTYCATS);
	/* set the FE static if this is set or if it is already set */
	FEsetecats( tu_emptycats || (FEecats()) );
	/*
	** Initialize the qbf and report item names, if they are to be
	** enabled. If they both are, we're running ingmenu (or a new
	** subsystem), and we don't want to deal with displaying the
	** database name anymore.
	*/
	qbfitem[0] = EOS;
	repitem[0] = EOS;
	if (special & TU_QBF_ENABLED)
	{
		STcopy(ERget(FE_Query),qbfitem);
	}
	if (special & TU_REPORT_ENABLED)
	{
		STcopy(ERget(FE_Report),repitem);
	}
	/* save current activate/validate states and restore when exit. */
/* # line 409 "tumain.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&prev_activate,37,(char *)0,0);
        IIiqfsio((short *)0,1,30,4,&prev_validate,3,(char *)0,0);
        IIiqfsio((short *)0,1,30,4,&next_activate,36,(char *)0,0);
      } /* IIiqset */
    }
/* # line 413 "tumain.qsc" */	/* host code */
	/* turn on previousfield activate et al */
/* # line 414 "tumain.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(37,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(3,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(36,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 417 "tumain.qsc" */	/* host code */
	if (!formready) {
		STATUS stat;
		stat = IIUFgtfGetForm(IIUFlcfLocateForm(),
							_Form);
		if (stat != OK) {
			IIUGerr (E_TU0041_Get_Form_Failed, UG_ERR_FATAL, 1,
				_Form ) ;
		}
		/* Users running Embedded (RUNTIME) INGRES, can't Create
		** or Drop tables. Check authorization string for Runtime
		** INGRES and suppress menuitems if set.
		*/
		create_menuitem  = ERget(FE_Create);
		destroy_menuitem = ERget(FE_Destroy);
		formready = TRUE ;
	}
/* # line 436 "tumain.qsc" */	/* display */
    {
      if (IIdispfrm(_Form,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 439 "tumain.qsc" */	/* host code */
		if (tu_emptycats)
		{
			fix_emptycat_form();
		}
		else
		{
			gettbls(_Wildcard, Empty_str);
		}
		/* fix up screen titles for all tables utility screens*/
		IITUstlSetTitle (title_prefix, ERget(F_TU002D_Table_Catalog),
				title);
/* # line 452 "tumain.qsc" */	/* putform */
        {
          if (IIfsetio(_Form) != 0) {
            IIputfldio((char *)"utitle",(short *)0,1,32,0,title);
          } /* IIfsetio */
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 457 "tumain.qsc" */	/* host code */
		newtable(tblname, title_prefix);
		    /* tblname is still unormalized */
			/* need normalized copy */
		IIUGdlm_ChkdlmBEobject( tblname, nrmltblname, FALSE );
		if (tu_emptycats)
		{
			fix_emptycat_form();
		}
		else if (tblname[0] != EOS)
		{
			/* user created a new table */
            i4 record = 0;
            i4 disp_row = 0;
            UIDBDATA *uidbdata;
			bool insert_row;
			uidbdata = IIUIdbdata();
			/* find where to insert new table in sorted table field
			** and scroll table field to that row. Scrolls to
			** proper row, assuming an INSERTROW will take place
			** (insertrow opens row after current one).
			*/
			record = IITUfr_FindRow(_Form, _Tblfld, _ColName,
				_ColOwner, tblname, uidbdata->suser);
			disp_row = IITUstf_ScrollTF (_Form, _Tblfld, _ColName,
					record, tblname, &insert_row);
			if (insert_row) {
/* # line 489 "tumain.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,_Form,_Tblfld,disp_row) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio(_ColName,(short *)0,1,32,0,nrmltblname);
                  IItcoputio(_ColOwner,(short *)0,1,32,0,uidbdata->suser);
                  IItcoputio(_ColType,(short *)0,1,32,0,ERget(F_TU002E_TABLE));
                  IItcoputio(_ColicType,(short *)0,1,30,sizeof(ICTABLE),
                  (void *)IILQint(ICTABLE));
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 494 "tumain.qsc" */	/* host code */
			}
			else
			{
/* # line 497 "tumain.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,disp_row) != 0) {
                IItcoputio(_ColName,(short *)0,1,32,0,nrmltblname);
                IItcoputio(_ColOwner,(short *)0,1,32,0,uidbdata->suser);
                IItcoputio(_ColType,(short *)0,1,32,0,ERget(F_TU002E_TABLE));
                IItcoputio(_ColicType,(short *)0,1,30,sizeof(ICTABLE),
                (void *)IILQint(ICTABLE));
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 502 "tumain.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 508 "tumain.qsc" */	/* host code */
  char sql_buf[FE_MAXNAME + 20];
# ifdef DGC_AOS
		/*
		** Until INGRES supports a readonly mode, DG will use the
		** following code to avoid ugly error messages that occur
		** when trying to write to the database when we are only
		** connected for readonly access.
		*/
		if (IIUIcro_CheckReadOnly())
		{
			IIUGerr(E_UI0016_ReadOnly_err, 0, 0);
/* # line 522 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 523 "tumain.qsc" */	/* host code */
		}
# endif
			/* get data off current row */
		if (ltcheck(TRUE, nrmltblname, nrmlowner))
		{
/* # line 529 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 530 "tumain.qsc" */	/* host code */
		}
			/* if fullfills delimited id criteria, then it must
			** be requoted; otherwise don't quote it, so display
			** in the popup is quoted only if it is a del id
			*/
		if ( IIUGdlm_ChkdlmBEobject( nrmltblname, NULL, TRUE ) )
		    IIUGrqd_Requote_dlm(nrmltblname, tblname);
		else
		    STcopy( nrmltblname, tblname ); 
		if ( IIUGdlm_ChkdlmBEobject( nrmlowner, NULL, TRUE ) )
		    IIUGrqd_Requote_dlm(nrmlowner, rrowner);
		else
		    STcopy( nrmlowner, rrowner ); 
/* # line 545 "tumain.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&tbltype,(char *)"ictype");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 547 "tumain.qsc" */	/* host code */
/*
** note: when in expert mode (-e), the 'ictype' will be ICTABLE.
** Thus, in expert mode only tables can be destroyed. This was entered
** as jupbug 6243, which I (pete) marked status 'q', cause it would
** take some time to fix and I believe few users use this feature.
*/
		if (tbltype == ICTABLE)
		{
                    if (IIUFccConfirmChoice(CONF_DESTROY, tblname,
			ERget(F_TU006A_Table), ERget(F_TU006D_HelpSubject),
			_TuccDestroyHelpFile) != CONFCH_YES)
                    {
			second_pass_emode = TRUE;
/* # line 560 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 561 "tumain.qsc" */	/* host code */
                    }
		    IIUGmsg (ERget(F_TU003D_Destroying_table), FALSE,
				1, (PTR) tblname) ;
		    STcopy(ERx("DROP TABLE "), sql_buf);
		    STcat(sql_buf, tblname);
		}
		else if (tbltype == ICVIEW)
		{
                    if (IIUFccConfirmChoice(CONF_DESTROY, tblname,
			ERget(F_TU006B_View), ERget(F_TU006D_HelpSubject),
			_TuccDestroyHelpFile) != CONFCH_YES)
                    {
/* # line 574 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 575 "tumain.qsc" */	/* host code */
                    }
		    IIUGmsg (ERget(F_TU003E_Destroying_view), FALSE,
				1, (PTR) tblname) ;
		    STcopy(ERx("DROP VIEW "), sql_buf);
		    STcat(sql_buf, tblname);
		}
		else if (tbltype == ICINDEX)	/* DEBUG: not in 6.4? */
		{
                    if (IIUFccConfirmChoice(CONF_DESTROY, tblname,
			ERget(F_TU006C_Index), ERget(F_TU006D_HelpSubject),
			_TuccDestroyHelpFile) != CONFCH_YES)
		    {
/* # line 587 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 588 "tumain.qsc" */	/* host code */
		    }
		    IIUGmsg (ERget(F_TU003F_Destroying_index), FALSE,
			1, (PTR) tblname) ;
		    STcopy(ERx("DROP INDEX "), sql_buf);
		    STcat(sql_buf, tblname);
		}
		else if (tbltype == ICSYNONYM)
		{
		    if (IIUFccConfirmChoice(CONF_DESTROY, tblname,
			ERget(F_TU006E_Synonym), ERget(F_TU006D_HelpSubject),
			_TuccDestroyHelpFile) != CONFCH_YES)
		    {
/* # line 600 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 601 "tumain.qsc" */	/* host code */
		    }
		    IIUGmsg(ERget(F_TU0046_Destroying_synonym), FALSE,
			1, (PTR) tblname);
		    STcopy(ERx("DROP SYNONYM "), sql_buf);
		    STcat(sql_buf, tblname);
		}
		else
		{
			IIUGerr(E_TU004B_Unknown_tbltype, 
				UG_ERR_ERROR, (i4) 2, (PTR) tblname,
				(PTR) &tbltype);
/* # line 612 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 613 "tumain.qsc" */	/* host code */
		}
/* # line 649 "tumain.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"tuma_stmnt",(char *)0,0,sql_buf);
  }
/* # line 650 "tumain.sc" */	/* host code */
		err_code = FEinqerr();
		if (err_code != OK)
		{
			if (!tu_emptycats)
			{
				IIUGerr(E_TU0021_Err_destroying, 
					UG_ERR_ERROR, (i4) 1, (PTR) tblname);
			}
			else	/* expert mode */
			{
				IIUGerr(E_TU004C_Err_destroy_exprt, 
					UG_ERR_ERROR, (i4) 1, (PTR) tblname);
			}
/* # line 629 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 630 "tumain.qsc" */	/* host code */
		}
/* # line 669 "tumain.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"tuma_stmnt",0);
    IIsyncup((char *)0,0);
  }
/* # line 670 "tumain.sc" */	/* host code */
		err_code = FEinqerr();
		if (err_code != OK)
		{
			IIUGerr(E_TU0021_Err_destroying, 
				UG_ERR_ERROR, (i4) 1, (PTR) tblname);
		}
		else if (tu_emptycats)
		{
			fix_emptycat_form();
		}
		else
		{
/* # line 645 "tumain.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_reco\
rd");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 646 "tumain.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&maxrows,32,_Tblfld,0);
              } /* IIiqset */
            }
/* # line 649 "tumain.qsc" */	/* host code */
			gettbls(_Wildcard, Empty_str);	/* reload table field */
/* # line 651 "tumain.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&newmaxrows,32,_Tblfld,0);
              } /* IIiqset */
            }
/* # line 654 "tumain.qsc" */	/* host code */
			/* if possible, position cursor on same row in dataset*/
			if (rownumber == maxrows) {
/* # line 656 "tumain.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 657 "tumain.qsc" */	/* host code */
			}
			else if (rownumber < maxrows) {
			    if (rownumber < newmaxrows)
/* # line 660 "tumain.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,rownumber) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 661 "tumain.qsc" */	/* host code */
			    else
/* # line 662 "tumain.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 663 "tumain.qsc" */	/* host code */
			}
			else 
			    ;	/* resume will put cursor on first row */
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 670 "tumain.qsc" */	/* host code */
			/* get data off current row */
		if (ltcheck(FALSE, nrmltblname, nrmlowner))
		{
/* # line 673 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 674 "tumain.qsc" */	/* host code */
		}
		STcopy(nrmltblname, ltable);
		STcopy(nrmlowner, lowner);
		IITUtbiTableinfo(nrmltblname, nrmlowner, title_prefix, NULL);
		if (tu_emptycats)
		{
			fix_emptycat_form();
		}
		else if (STcompare(nrmltblname, ltable) != 0 ||
			 STcompare(nrmlowner, lowner) != 0)
		{
			count = 0;
			rownumber = 0;
/* # line 689 "tumain.qsc" */	/* unloadtable */
            {
              if (IItbact(_Form,_Tblfld,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,ltable,(char *)
"table");
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,lowner,(char *)
"owner");
                IITBceColEnd();
                {
/* # line 692 "tumain.qsc" */	/* host code */
				count++;
				if (STcompare(ltable, nrmltblname) == 0 &&
				    STcompare(lowner, nrmlowner) == 0)
				{
					rownumber = count;
				}
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 699 "tumain.qsc" */	/* host code */
			tblscroll(rownumber);
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 705 "tumain.qsc" */	/* host code */
		i4	is_table = FALSE;
			/* get data off current row */
		if (ltcheck(FALSE, nrmltblname, nrmlowner))
		{
/* # line 710 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 711 "tumain.qsc" */	/* host code */
		}
		IIUGrqd_Requote_dlm(nrmltblname, tblname);
		IIUGrqd_Requote_dlm(nrmlowner, rrowner);
 		STcopy(nrmltblname, ltable);
 		STcopy(nrmlowner, lowner);
		for (restart = TRUE; restart; )
		{
			restart = FALSE;
/* # line 720 "tumain.qsc" */	/* submenu */
            {
              goto IImuI2;
IIfdB2:
              IIrunmu(0);
              if (IIretval() == 1) {
                {
/* # line 724 "tumain.qsc" */	/* host code */
				is_table = FALSE;
                }
              } else if (IIretval() == 2) {
                {
/* # line 729 "tumain.qsc" */	/* host code */
				is_table = TRUE;
                }
              } else if (IIretval() == 3) {
                {
/* # line 734 "tumain.qsc" */	/* host code */
				FEhelp(ERx("tutblqbf.hlp"), 
					ERget(F_TU002A_QBF));
				restart = TRUE;
                }
              } else if (IIretval() == 4) {
                {
/* # line 741 "tumain.qsc" */	/* host code */
				second_pass_emode = TRUE;
/* # line 742 "tumain.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
                }
              } else {
                if (1) goto IIfdE2;
              } /* IIretval */
              goto IIfdE2;
IImuI2:
              IImuonly();
              if (IIinitmu() == 0) goto IIfdE2;
              if (IInmuact(ERget(F_TU000B_SimpleFields),
              ERget(F_TU000C_SimpleField_expl),2,2,1) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_TblField),ERget(F_TU000D_TableField_expl),
              2,2,2) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Help),ERget(F_TU000E_Help_expl),2,2,3) == 
              0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_End),ERget(F_TU000F_End_expl),2,2,4) == 0
              ) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdB2;
IIfdE2:;
            }
/* # line 744 "tumain.qsc" */	/* host code */
		}
		if (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) >= 0)
		{
			/* then we're operating against 6.5, so synthesize
			** owner.table from components, and fire up qbf
			** on that.  Cast ret of xrt to void since these
			** names were loaded from dbms (assume they're ok)
			*/
#ifdef VMS
                        /* If the table is a delimited table name, we can't use
                        ** IIUIxrt_tbl, as it will enclose the table name in '"'.
                        ** The command line generated to invoke the QBF utility,
                        ** encloses each parameter in '"' also, thus we have a
                        ** conflict. We need to enclose delimited table names in
                        ** '""' (two double quotes), e.g owner.""table name"".
                        */
                    if (rrowner [0] != '"')
                    {
                       if (tblname [0] != '"')
  		           (void) IIUIxrt_tbl( nrmlowner, nrmltblname, owndottbl );
                       else
                           STprintf( owndottbl, "%s.\"\"%s\"\"", nrmlowner,
				nrmltblname );
                    }
		    else
                    {
                       if (tblname [0] != '"')
                           STprintf( owndottbl, "\"\"%s\"\".%s", nrmlowner,
				nrmltblname );
                       else
                           STprintf( owndottbl, "\"\"%s\"\".\"\"%s\"\"",
				nrmlowner, nrmltblname );
                    }
#else
                       (void) IIUIxrt_tbl( nrmlowner, nrmltblname, owndottbl );
#endif
		    mqbfstart( 1, is_table, owndottbl );
		}
		else	/* 6.4 or less */
		{
		    mqbfstart(1, is_table, nrmltblname);
		}
		if (tu_emptycats)
		{
			fix_emptycat_form();
		}
		else if (STcompare(nrmltblname, ltable) != 0 ||
			 STcompare(nrmlowner, lowner) != 0)
		{
			count = 0;
			rownumber = 0;
/* # line 799 "tumain.qsc" */	/* unloadtable */
            {
              if (IItbact(_Form,_Tblfld,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,ltable,(char *)
"table");
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,lowner,(char *)
"owner");
                IITBceColEnd();
                {
/* # line 802 "tumain.qsc" */	/* host code */
				count++;
				if (STcompare(ltable, nrmltblname) == 0 &&
				    STcompare(lowner, nrmlowner) == 0)
				{
					rownumber = count;
				}
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 809 "tumain.qsc" */	/* host code */
			tblscroll(rownumber);
		}
          }
        } else if (IIretval() == 5) {
          {
/* # line 815 "tumain.qsc" */	/* host code */
		i4	table_mode = 0;
			/* get data off current row */
		if (ltcheck(FALSE, nrmltblname, nrmlowner))
		{
/* # line 820 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 821 "tumain.qsc" */	/* host code */
		}
		IIUGrqd_Requote_dlm(nrmltblname, tblname);
		IIUGrqd_Requote_dlm(nrmlowner, rrowner);
		STcopy(nrmltblname, ltable);
		STcopy(nrmlowner, lowner);
		for (restart = TRUE; restart; )
		{
			restart = FALSE;
/* # line 831 "tumain.qsc" */	/* submenu */
            {
              goto IImuI3;
IIfdB3:
              IIrunmu(0);
              if (IIretval() == 1) {
                {
/* # line 835 "tumain.qsc" */	/* host code */
				table_mode = H_COLUMN;
                }
              } else if (IIretval() == 2) {
                {
/* # line 840 "tumain.qsc" */	/* host code */
				table_mode = H_WRAP;
                }
              } else if (IIretval() == 3) {
                {
/* # line 845 "tumain.qsc" */	/* host code */
				table_mode = H_BLOCK;
                }
              } else if (IIretval() == 4) {
                {
/* # line 850 "tumain.qsc" */	/* host code */
				FEhelp(ERx("tutblrep.hlp"),
					ERget(F_TU002B_Report));
				restart = TRUE;
                }
              } else if (IIretval() == 5) {
                {
/* # line 857 "tumain.qsc" */	/* host code */
				second_pass_emode = TRUE;
/* # line 858 "tumain.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
                }
              } else {
                if (1) goto IIfdE3;
              } /* IIretval */
              goto IIfdE3;
IImuI3:
              IImuonly();
              if (IIinitmu() == 0) goto IIfdE3;
              if (IInmuact(ERget(F_TU0012_ColumnMode),
              ERget(F_TU0013_ColumnMode_expl),2,2,1) == 0) goto IIfdE3;
              if (IInmuact(ERget(F_TU0014_WrapMode),
              ERget(F_TU0015_WrapMode_expl),2,2,2) == 0) goto IIfdE3;
              if (IInmuact(ERget(F_TU0010_BlockMode),
              ERget(F_TU0011_BlockMode_expl),2,2,3) == 0) goto IIfdE3;
              if (IInmuact(ERget(FE_Help),ERget(F_TU0018_Help_expl),2,2,4) == 
              0) goto IIfdE3;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE3;
              if (IInmuact(ERget(FE_End),ERget(F_TU0019_End_expl),2,2,5) == 0
              ) goto IIfdE3;
              IIFRInternal(0);
              if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE3;
              if (IIendmu() == 0) goto IIfdE3;
              goto IIfdB3;
IIfdE3:;
            }
/* # line 860 "tumain.qsc" */	/* host code */
		}
		if (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) >= 0)
		{
			/* then we're operating against 6.5, so synthesize
			** owner.table from components, and fire up report
			** on that.  Cast ret of xrt to void since these
			** names were loaded from dbms (assume they're ok)
			*/
#ifdef VMS
                        /* If the table is a delimited table name, we can't use
                        ** IIUIxrt_tbl, as it will enclose the table name in '"'.
                        ** The command line generated to invoke the QBF utility,
                        ** encloses each parameter in '"' also, thus we have a
                        ** conflict. We need to enclose delimited table names in
                        ** '""' (two double quotes), e.g owner.""table name"".
                        */
                    if (rrowner [0] != '"')
                    {
                       if (tblname [0] != '"')
  		           (void) IIUIxrt_tbl( nrmlowner, nrmltblname, owndottbl );
                       else
                           STprintf( owndottbl, "%s.\"\"%s\"\"", nrmlowner,
				nrmltblname );
                    }
		    else
                    {
                       if (tblname [0] != '"')
                           STprintf( owndottbl, "\"\"%s\"\".%s", nrmlowner,
				nrmltblname );
                       else
                           STprintf( owndottbl, "\"\"%s\"\".\"\"%s\"\"",
				nrmlowner, nrmltblname );
                    }
#else
                       (void) IIUIxrt_tbl( nrmlowner, nrmltblname, owndottbl );
#endif
		    reportstart(owndottbl, table_mode);
		}
		else /* 6.4 or less */
		{
		    reportstart(nrmltblname, table_mode);
		}
		if (tu_emptycats)
		{
			fix_emptycat_form();
		}
		else if (STcompare(nrmltblname, ltable) != 0 ||
			 STcompare(nrmlowner, lowner) != 0)
		{
			count = 0;
			rownumber = 0;
/* # line 915 "tumain.qsc" */	/* unloadtable */
            {
              if (IItbact(_Form,_Tblfld,0) == 0) goto IItbE3;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,ltable,(char *)
"table");
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,lowner,(char *)
"owner");
                IITBceColEnd();
                {
/* # line 918 "tumain.qsc" */	/* host code */
				count++;
				if (STcompare(ltable, nrmltblname) == 0 &&
				    STcompare(lowner, nrmlowner) == 0)
				{
					rownumber = count;
				}
                }
              } /* IItunload */
IItbE3:
              IItunend();
            }
/* # line 925 "tumain.qsc" */	/* host code */
			tblscroll(rownumber);
		}
          }
        } else if (IIretval() == 6) {
          {
/* # line 932 "tumain.qsc" */	/* host code */
		/* only do the "Go" menu item if something was passed */
		if ( (itemname[0] != EOS) && (menu_item != NULL) )
		{
				/* get data off current row */
			if (ltcheck(FALSE, nrmltblname, nrmlowner))
			{
/* # line 938 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 939 "tumain.qsc" */	/* host code */
			}
			/* re-compose owner.tablename */
			if (!IIUIxrt_tbl(nrmlowner, nrmltblname, owndottbl))
			{
				/*
				** THIS SHOULD NEVER HAPPEN. This means
				** that either rrowner or tblname was found
				** to be a UI_BOGUS_ID. Since ltcheck()
				** above should have caught this. Should
				** maybe print an error here like ltcheck()?
				*/
/* # line 951 "tumain.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 952 "tumain.qsc" */	/* host code */
			}
			(*(menu_item->rout))(menu_item->mem, owndottbl);
			/* fix for jupbug 2181: */
			if (tu_emptycats)
			{
				fix_emptycat_form();
			}
		}
		else	/* Indicate menuitem is not available by beeping.
			** (user could have selected this by typing FRSkey4.
			*/
			FTbell();
          }
        } else if (IIretval() == 7) {
          {
/* # line 971 "tumain.qsc" */	/* host code */
		FEtabfnd(_Form, _Tblfld);
          }
        } else if (IIretval() == 8) {
          {
/* # line 976 "tumain.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 981 "tumain.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 987 "tumain.qsc" */	/* host code */
		FEhelp(ERx("tutbllst.hlp"), 
			ERget(F_TU002C_Table_Utility));
          }
        } else if (IIretval() == 11) {
          {
/* # line 994 "tumain.qsc" */	/* host code */
		/* TRUE if END, FALSE if QUIT */
		if (menu_item != NULL)
			menu_item->end_not_quit = (enditem[0] != EOS);
		second_pass_emode = TRUE;
/* # line 999 "tumain.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else if (IIretval() == 12) {
          {
/* # line 1005 "tumain.qsc" */	/* host code */
		/* TRUE if END, FALSE if QUIT */
		if (menu_item != NULL)
			menu_item->end_not_quit = (quititem[0] == EOS);
/* # line 1009 "tumain.qsc" */	/* enddisplay */
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
      goto IIfdE1;
IImuI1:
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(create_menuitem,ERget(F_TU0053_ExplCreate),2,2,1) == 0) 
      goto IIfdE1;
      if (IInmuact(destroy_menuitem,ERget(F_TU0054_ExplDestroy),2,2,2) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Examine),ERget(F_TU0055_ExplExamine),2,2,3) == 0) 
      goto IIfdE1;
      if (IInmuact(qbfitem,ERget(F_TU0056_ExplQuery),2,2,4) == 0) goto IIfdE1;
      if (IInmuact(repitem,ERget(F_TU0057_ExplReport),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(itemname,(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,9) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,10) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,10) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,11) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,11) == 0) goto IIfdE1;
      if (IInmuact(quititem,ERget(F_TU0058_ExplQuit),0,0,12) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,0,12) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 1012 "tumain.qsc" */	/* host code */
	/* restore activate/validate status in effect before tblutil called */
/* # line 1013 "tumain.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(37,(char *)0,0,(short *)0,1,30,4,&prev_activate);
        IIstfsio(3,(char *)0,0,(short *)0,1,30,4,&prev_validate);
        IIstfsio(36,(char *)0,0,(short *)0,1,30,4,&next_activate);
      } /* IIiqset */
    }
  }
