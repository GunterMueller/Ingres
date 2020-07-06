# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<uf.h>
#include	<ug.h>
# include <abclass.h>
#include	<uigdata.h>
#include	<stdprmpt.h>
#include	"erab.h"
/**
** Name:	abutilty.qc -	ABF Application Utilities Sub-Menu.
**
** Description:
**	Contains the routine used to execute the ABF Utilities Sub-Menu,
**	which gives access to the ABF Build Application Image frame, etc.
**	Defines:
**
**	IIAButilities()	ABF utilities frame.
**
** History:
**	Revision 6.2  88/12  wong
**	Initial revision.
**	12/19/89 (dkh) - VMS shared lib changes - References to IIarDbname
**			 is now through IIar_Dbname.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
GLOBALREF char	**IIar_Dbname;
GLOBALREF bool  IIabVision;
/*{
** Name:	IIAButilities() -	ABF Utilities Sub-Menu.
**
** Description:
**	Runs a sub-menu that allows the user to select several utility
**	functions.  Its arguments are all passed through to the utility
**	functions, except for whether INGRES/Menu is offered as an option.
**	The caller indicatates to this function and children who the 
**	caller is.. this indicates whether we load and/or free the 
**	application components.
**
**	if the application is NULL, we were called from an empty applications
**	catalog, and only a subset of the menuitmes are active.
**
** Input:
**	app	{APPL *}  An application object.
**	appname	{char *}  The application name.
**	caller  {nat}     indication of who is calling us
**	expert {bool}  Expert mode.
**
** Called by:
**	'IIABcatalogs()' and 'IIABeditApp()'.
**
** History:
**	12/88 (jhw) -- Written.
**	28-aug-1990 (Joe)
**	    Changed from IIUIgdata to IIUIdbdata().  Also, replaced
**	    STequal of user and dba with reference to isdba field.
**	10/90 (Mike S) Allow calls from empty applications catalog
*/
#define _ImageExpl	F_AB000E_Image_expl
#define _IngMenuExpl	F_AB000F_IngMenu_expl
#define _ConsFileExpl	F_AB00E2_ConsFile_expl
#define _ShellExpl	F_FE010A_ExplShell
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _EndExpl	F_FE0102_ExplEnd
#define AB_IngMenu	F_AB0004_IngMenu
VOID
IIAButilities ( app, appname, caller, expert )
APPL	*app;
char	*appname;
i4	caller;
bool	expert;
{
  char *_shell;
  char *_clup;
  char *imageItem = NULL;
  char *gconsfItem = NULL;
  char *reconcileItem = NULL;
  char *reportItem = NULL;
  char *locksItem = NULL;
	i4		cc;
	ER_MSGID	cumsg;
	_clup = IIUIdbdata()->isdba ? ERget(F_AB00AF_CleanMenu) : NULL;
#ifndef DGC_AOS
#ifndef CMS
#ifdef VMS
	_shell = ERget(FE_Spawn);
#else
	_shell = ERget(FE_Shell);
#endif
#else
	_shell = ERget(FE_CMSSubset);
#endif
#else
	_shell = ERget(FE_CLI);
#endif
	/* Set up menuitmes which are invalid from an empty catalog */
	if (app != NULL)
	{
		imageItem = ERget(FE_Image);
		gconsfItem = ERget(F_AB00E1_ConsFile_menu);
		reportItem = ERget(F_AB0082_AppReport);
		locksItem = ERget(F_AB00AC_LockMenu);
		/* If we're in Vision, add the "Reconcile" menuitem */
		if (IIabVision)
			reconcileItem = ERget(F_AB0085_Reconcile);
	}
	for (;;)
/* # line 131 "abutilty.qsc" */	/* submenu */
  {
    goto IImuI1;
IIfdB1:
    IIrunmu(0);
    if (IIretval() == 1) {
      {
/* # line 135 "abutilty.qsc" */	/* host code */
		IIABimage(app, appname, caller);
		break;
      }
    } else if (IIretval() == 2) {
      {
/* # line 143 "abutilty.qsc" */	/* message */
        {
          IImessage(ERget(F_AB0084_AppReport_msg));
        }
/* # line 145 "abutilty.qsc" */	/* host code */
		/* we must read the application components, if not
		   called from the application flow diagram */
		if (  caller == ABUT_AFD
		   || IIABappFetch(app, appname, OC_UNDEFINED, TRUE) == OK
		   )
		{
			/* display the report */
			IIVQdarDisplayAppReport(app);
			/* free the components if necessary, only free if
			   we were called from the application catalog */
			if (caller == ABUT_APPCAT)
			{
				IIAMappFree(app);
			}
		}
		break;
      }
    } else if (IIretval() == 3) {
      {
/* # line 167 "abutilty.qsc" */	/* host code */
		IIIMnaive(*IIar_Dbname, "", TRUE, expert);
		break;
      }
    } else if (IIretval() == 4) {
      {
/* # line 173 "abutilty.qsc" */	/* call */
        {
          IIutsys(0,(char *)"system",(char *)0);
          IIutsys(1,(char *)"command",(char *)"");
          IIutsys(2,(char *)0,(char *)0);
        }
/* # line 174 "abutilty.qsc" */	/* host code */
		break;
      }
    } else if (IIretval() == 5) {
      {
/* # line 179 "abutilty.qsc" */	/* host code */
		IIABlaLockAdm(OC_UNDEFINED);
		break;
      }
    } else if (IIretval() == 6) {
      {
/* # line 187 "abutilty.qsc" */	/* host code */
		/* we must read the application components, if not
		   called from the application flow diagram */
		if (caller != ABUT_AFD)
		{
			IIUGmsg(ERget(F_AB00B2_CatMsg), FALSE, 0);
			if (IIABappFetch(app, appname, OC_UNDEFINED, TRUE) 
			     != OK)
				break;
		}
		/* do table reconciliation */
		IIVQraReconcileApp(app, caller != ABUT_APPCAT);
		/* free the components if necessary, only free if
		   we were called from the application catalog */
		if (caller == ABUT_APPCAT)
		{
			IIAMappFree(app);
		}
		break;
      }
    } else if (IIretval() == 7) {
      {
/* # line 211 "abutilty.qsc" */	/* host code */
		cc = IIUFccConfirmChoice(CONF_GENERIC, ERx(""), ERx(""),
                	ERget(F_AB00BB_CHelp), ERx("abfclup.hlp"),
			F_AB00B7_CTitle, F_AB00B8_Yexp, F_AB00B9_Nexp,
			ERx(""), FALSE);
		if (cc == CONFCH_YES)
		{
/* # line 217 "abutilty.qsc" */	/* message */
        {
          IImessage(ERget(F_AB00BA_Cmessage));
        }
/* # line 218 "abutilty.qsc" */	/* host code */
			if (IIUIdgDeleteGarbage(0,TRUE,&cc) != OK)
			{
				cumsg = E_AB00A7_CErrs;
			}
			else
			{
				if (cc == 0)
					cumsg = S_AB00A8_CNone;
				else
					cumsg = S_AB00A9_CWork;
			}
			/*
			** Yes, if cc = 1, the message will be ungrammitical.
			** It is highly unlikely that there will be 1 row.
			*/
			IIUGerr(cumsg, UG_ERR_ERROR, 1, (PTR) &cc);
		}
/* # line 236 "abutilty.qsc" */	/* redisplay */
        {
          IIredisp();
        }
      }
    } else if (IIretval() == 8) {
      {
/* # line 241 "abutilty.qsc" */	/* host code */
		IIABgcfGlobalConstantFile(app, appname);
/* # line 242 "abutilty.qsc" */	/* redisplay */
        {
          IIredisp();
        }
      }
    } else if (IIretval() == 9) {
      {
/* # line 247 "abutilty.qsc" */	/* host code */
		FEhelp(ERx("abutilty.hlp"), ERget(F_AB0005_UtilitiesTitle));
      }
    } else if (IIretval() == 10) {
      {
/* # line 252 "abutilty.qsc" */	/* host code */
		break;
      }
    } else {
      if (1) goto IIfdE1;
    } /* IIretval */
    goto IIfdE1;
IImuI1:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(imageItem,ERget(_ImageExpl),2,2,1) == 0) goto IIfdE1;
    if (IInmuact(reportItem,ERget(F_AB0083_AppReport_expl),2,2,2) == 0) goto 
    IIfdE1;
    if (IInmuact(ERget(AB_IngMenu),ERget(_IngMenuExpl),2,2,3) == 0) goto IIfdE1;
    if (IInmuact(_shell,ERget(_ShellExpl),2,2,4) == 0) goto IIfdE1;
    if (IInmuact(locksItem,ERget(F_AB00AD_LockExpl),2,2,5) == 0) goto IIfdE1;
    if (IInmuact(reconcileItem,ERget(F_AB0086_Reconcile_expl),2,2,6) == 0) 
    goto IIfdE1;
    if (IInmuact(_clup,ERget(F_AB00B0_CleanExpl),2,2,7) == 0) goto IIfdE1;
    if (IInmuact(gconsfItem,ERget(_ConsFileExpl),2,2,8) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,9) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,10) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,10) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdB1;
IIfdE1:;
  }
/* # line 254 "abutilty.qsc" */	/* host code */
}
