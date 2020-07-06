# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<me.h>
#include	<ex.h>
#include	<st.h>
#include	<lo.h>
#include	<si.h>
#include	<ol.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<erug.h>
#include	<fedml.h>
#include	<flddesc.h>
# include <abclass.h>
#include	<abfdbcat.h>
# include <oocat.h>
# include <oocatlog.h>
#include	<oodefine.h>
#include	<feconfig.h>
#include	<abfcnsts.h>
#include	<eqrun.h>
#ifdef IMTDY
#include	<fdesc.h>
#include	<abfrts.h>
#endif
#include	<ilerror.h>
#include	<uigdata.h>
#include	<metafrm.h>
#include	"abclinfo.h"
# include "abcatrow.h"
#include	"abfgolnk.h"
#include	"gncode.h"
#include	"erab.h"
/*
fool ming:
#include	<abclass.h>
#include	<oocat.h>
#include	"abcatrow.h"
*/
/**
** Name:	editapp.qc -		ABF Define Application Frame.
**
** Description:
**	Contains the application definition frame routine for ABF, which allows
**	users to define or modify applications.	 Defines:
**
**	IIABeditApp()	ABF edit application frame.
**
** History:
**	Revision 2.0  82/04  joe
**	Initial revision.
**	12/19/89 (dkh) - VMS shared lib changes - References to IIarStatus
**			 is now through IIar_Status.
**
**	Revision 6.1  88/10  wong
**	Modified to use ACTIVATE FIELD to get source-code directory and to
**	check for its existence only when changed.
**
**	Revision 6.0  87/06  wong
**	Moved 'abchkaname()' to "abchkname.c".
**
**	8-march-91 (blaise)
**	    Integrated PC porting changes.
**	11-march-91 (blaise)
**	    More desktop porting changes.
**	03/28/91 (dkh) - Replaced old inquire_frs syntax with new.
**	24-aug-92 (davel)
**	    Modify catalog object version number check: should be < instead
**	    of !=.
**	03-nov-92 (rudyw)
**	    Fix bug 44739. Modify a call to IIABchkSrcDir to bypass checking
**	    if source directory is writeable by the user during a file edit.
**	    Let source file permissions determine the user editing capability.
**	08-dec-92 (davel)
**	    Replaced IIolLang references with the official OL interface,
**	    OLanguage().
**	30-jun-93 (DonC)
**	    Fix for bug 50610. Add call to new routine IIAMcrCountRef. Upon
**	    deletion of a form object, the object should NOT be deleted until
**	    all other references to the object go away.  An attribute exists
**	    in the FORM_REF structure to track the number of objects that 
**	    reference a given form.  Unfortunately the architecture of ABF
**	    and Vision disallowed proper maintenance of the reference counts
**	    unless every single frame object was touched in a given Vision/ABF
**	    session (not too good). As a result there were numerous bugs that
**	    caused forms not to be linked into images when frames shared form
**	    references.  By calling IIAMcrCountRef, the map off all objects is
**	    scanned for usage of a given form anf the reference count updated
**	    accordingly.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
char		*IIUGdmlStr();
char		*UGcat_now();
char 		*IIUGfmt();
bool 		IIUFver();
bool		IIABchkSrcDir();
APPL_COMP	*iiabGetComp();
COMP_CLASS_INFO	*iiabApCompClass();
STATUS		iiabSvApp();
STATUS		IIABcreateComp();
STATUS		iiabCrVQEntry();
i4  		IIFDlpListPick();
VOID		iiabCtRenameComp();
static STATUS	do_display();
static VOID	de_chain();
static VOID	de_reference();
static VOID	invalidate_obj();
static USER_FRAME	*find_top();
static USER_FRAME	*new_top();
static APPL_COMP	*find_parent();
static USER_FRAME	*add_top();
GLOBALREF bool	IIabWopt;
GLOBALREF bool	IIabVision;
GLOBALREF bool	abFnwarn;
GLOBALDEF ABLNKTST	iiAB_test_image = {0};
static bool	Ref_reentry;
static OOID	Ref_curid;
/*{
** Name:	IIABeditApp() -	ABF Edit Application Frame.
**
** Input:
**	appname		{char *}  Application name.
**	id		{OOID}  Application object ID.
**	noload		{bool}  Expert mode; don't load the catalogs.
**
** Called by:
**	'IIabf()' and 'IIABcatalogs()'.
**
** Side Effects:
**	Will change the ABF catalogs
**
** History:
**	Written 4/15/82 (jrc)
**	08/85 (jhw) -- Made explicit menu execution under "Define" into
**		a sub-menu.
**	02-mar-86 (marian)	bug 8254, 9986, 10346
**		Fixed problems with source code directory when 1) no
**		directory specified, 2) non-existent directory
**		specified and 3) non-writeable directory specified.
**		Changed abdefapp() so it now checks these conditions
**		before creating the application, defining a frame and
**		procedure by calling IIABchkSrcDir().
**	23-jul-86 (marian)	bug 8868
**		Comment out app->modified = TRUE it will be done in
**		IIABccatCreate().
**	02-jun-87 (bobm)  6.0 catalog / name generation changes.
**	06/87 (jhw) -- Set application for exception here (through
**		'abexcapp()') rather than in the catalog utilities,
**		'IIABccatCreate()' or 'IIABgcatGet()' since it is ABF specific.
**	03/16/88 (dkh) - Added EXdelete call to conform to coding spec.
**	04/90 (jhw) - Hacked Rename to handle host-language types by fetching
**		procedure component and re-updating new row.  Bug #21336.
**	6/90 (Mike S)
**		Don't allow edit of application with bad source directory.
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
**	24-aug-92 (davel)
**		Modify catalog object version number check: should be < instead
**		of !=.
**		
*/
GLOBALREF OO_CLASS	*IIam_Class;
GLOBALREF OO_CLASS	*IIam_ProcCl;
GLOBALREF STATUS	*IIar_Status;
static VOID	_a_catInit();
static VOID	_a_catLoad();
static VOID	_a_loadTable();
static VOID	_a_insertRow();
static VOID	_a_putRow();
static C_OO_CATALOG	_catClass =
{
	IIOOct0Init,
	IIOOctEmpty,
	IIOOctCheck,
	IIOOctLoad,
	IIOOctDestroy,
	IIOOctRename,
	IIOOctFetch,
	IIOOctMoreInfo,
	_a_catInit,
	_a_catLoad,
	_a_loadTable,
	_a_insertRow,
	_a_putRow
};
GLOBALDEF AB_CATALOG	iiabAcatalog =
{
	&_catClass,
	ERx("abfobjects"),
	ERx("frames@procs"),
	NULL,			/* loaded with IIamClass at run time */		 
	0, FALSE,
	OC_UNDEFINED, OC_UNDEFINED,
	FALSE ,
	/* func, menuitem               char  explanation           char */
	/* pseudo-menuitem for initialization. has title. */
	{ NULL, F_AB0014_AppEdit_title, NULL, 0, NULL },
	/* menuitem for create. */
	{ NULL, FE_Create, NULL, F_AB0015_CreateObj_expl, NULL },
	/* menuitem for destroy. */
	{ NULL, FE_Destroy, NULL, F_AB0016_DestroyObj_expl, NULL },
	/* menuitem for edit. */
	{ NULL, FE_Edit, NULL, F_AB0017_EditObj_expl, NULL },
	/* menuitem for end. */
	{ NULL, FE_End, NULL, F_FE0102_ExplEnd, NULL },
	/* menuitem for quit. */
	{ NULL, FE_Quit, NULL, F_AB000D_Quit_expl, NULL },
	iiabGetComp,
	ERx("abedtapp.hlp")
};
#define _CreateExpl	F_AB0015_CreateObj_expl
#define _DestroyExpl	F_AB0016_DestroyObj_expl
#define _EditExpl	F_AB0017_EditObj_expl
#define _RenameExpl	F_AB0018_RenameObj_expl
#define _GoExpl		F_AB000B_Go_expl
#define _PartsExpl	F_AB0300_Parts_expl
#define _UtilExpl	F_AB000C_Util_expl
#define	_FindExpl	F_FE0104_ExplFind
#define	_TopExpl	F_FE0105_ExplTop
#define	_BottomExpl	F_FE0106_ExplBottom
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _EndExpl	F_FE0102_ExplEnd
#define _QuitExpl	F_AB000D_Quit_expl
#define _ForgetExpl	F_FE0109_ExplForget
#define _EditAppTitle	F_AB0014_AppEdit_title
#define FE_Defaults	F_AB0006_Defaults
#define _DefltExpl	F_AB0010_Defaults_expl
STATUS
IIABeditApp ( appname, id, noload, newapp, qflag )
char	*appname;
OOID	id;
bool	noload;
bool	newapp;
bool	*qflag;
  {
	STATUS		stat;
	bool		exit_flag;
	USER_FRAME	*current;
	USER_FRAME	*topframe;
	AB_CATALOG	*self;
    APPL app;
	EX_CONTEXT	context;
	AB_CATALOG	*iiabGtCatClass();
	EX		abexchand();
	*qflag = FALSE;
	iiabAcatalog.c_end.m_m_menu = FE_End;
	iiabAcatalog.c_quit.m_m_menu = FE_Quit;
	/* need to stash newapp info too. */
	self = iiabGtCatClass(OC_APPL, noload);
	IIUGmsg(ERget(F_AB00B2_CatMsg), FALSE, 0);
	app.ooid = id;
	app.data.inDatabase = FALSE;
	if (( IIABappFetch(&app, 
			  appname, 
			  (IIabVision ? OC_UNDEFINED : id) , 
			  IIabVision)
		!= OK ) || ( app.name == NULL))		 
	{
		return FAIL;
	}
	if (!IIABchkSrcDir(app.source, FALSE))
	{
		IIAMappFree(&app); 
		return FAIL;
	}
	if (IIABcelCheckEditLock(&app) != OK)
	{
		IIAMappFree(&app);
		return FAIL;
	}
	id = app.ooid;
	/*
	** old version number on application overrides -w command line setting
	*/
	abFnwarn = ( app.version < ACAT_VERSION_REL_60  ||  !IIabWopt );
	if ( EXdeclare(abexchand, &context) != OK )
	{
		if ( *IIar_Status == OK )
			*IIar_Status = FAIL;
	    USER_CANCEL:
		IIAMappFree(&app);
		EXdelete();
		IIABrlRemLock(app.ooid);
		return FAIL;
	}
	MEfill(sizeof(iiAB_test_image), '\0', (PTR)&iiAB_test_image);
	iiabNamesInit();
	self->c_appid = app.ooid;
	/*
	** If vision, we get the FrameFlow menuitem.  Set up to call
	** frame flow diagram initially if we have a metaframe, otherwise
	** the regular catalog screen.
	*/
	if ( !IIabVision )
	{
		topframe = NULL;
	}
	else
	{
		if (app.dml == DB_QUEL)
		{
			IIUGerr(E_AB02F6_WrongDML, 0, 0);
			topframe = NULL;
		}
		else
		{
			topframe = (newapp) ? new_top(&app) : find_top(&app);
			if (topframe == NULL)
			{
				if (iiabCrVQEntry(&app, &iiabAcatalog,
						&topframe) == FAIL) 
				{
					goto USER_CANCEL;
				}
				/* the function above could also possibly
				   return OK (with topframe set), or
				   GO_CATALOG (with topframe null) */
			}
		}
	}
	current = NULL;
	/*
	** do this outside the range of the display loop
	** in do_display()
	*/
	_VOID_ IIABchkSrcDir( app.source, FALSE );
	/*
	** If we are VISION, the first do_display merely sets up the catalog
	** screen and comes back so that things from the frame flow diagram
	** may reference the catalog tablefield.
	**
	** The frame flow diagram returns a boolean indicating whether
	** we are to exit or not.  This was done because this interaction
	** was originally implemented as a toggle between the catalog
	** and the frame flow.  Now, we simply call the catalog display
	** and then reenter the frame flow, making the catlog appear to be
	** underneath the frame flow diagram.
	**
	** for VISION, if there is no topframe, and user does not provide
	** one, we simply display the catalog, degenerating into normal
	** abf functionality.
	*/
	Ref_reentry = TRUE;
	stat = do_display(self,&app,&current,&topframe,FALSE,noload,&exit_flag);
	if (IIabVision)
	{
		if (topframe != NULL)
		{
			i4	call_state = 0;
			while ( stat == OK && call_state == 0 && !exit_flag )
			{
				stat = IIVQffeFrameFlowEntry(&app, topframe,
					&current, &call_state, (PTR) self);
				if ( stat == OK )
				{
					if ( call_state == 0 )
						stat = do_display( self, &app,
								&current,
								&topframe,
								Ref_reentry,
								noload,
								&exit_flag
						);
					else if ( call_state < 0 )
						exit_flag = TRUE;
					/*
					** this will happen if user removes the
					** top frame.  Simply let "end" end them
					** back to the application catalog.
					*/
					if (topframe == NULL)
						break;
				}
			}
		}
		else
		{
			do_display(self, &app, &current, &topframe,
						TRUE, noload, &exit_flag);
		}
	}
	if ( exit_flag )
	{
/* # line 443 "editapp.qsc" */	/* message */
    {
      IImessage(ERget(S_AB0034_Exiting));
    }
/* # line 444 "editapp.qsc" */	/* host code */
		*qflag = TRUE;
	}
	IIABrlRemLock(app.ooid);
	IIAMappFree(&app);
	iiabRmTest(&iiAB_test_image);
	EXdelete();
	return stat;
  }
/*
** reentry = FALSE under VISION simply sets up the catalog screen and returns.
** this assures that any of the initialization which has to be done inside
** the display loop is done there, as well as assuring that the catalog screen
** is available for xxxVision() calls.
*/
static OOID	pickGlobClass();
static STATUS
do_display(self,app,curobj,topframe,reentry,noload,eflag)
  AB_CATALOG *self;
  APPL *app;
USER_FRAME **curobj;
USER_FRAME **topframe;
bool reentry;
bool noload;
bool	*eflag;
  {
	AB_CATREC	row;
	*eflag = FALSE;
	/* Note:  Form is displayed in the default FILL mode,
	** but all fields are display or read only.
	*/
/* # line 478 "editapp.qsc" */	/* display */
    {
      if (IIdispfrm(self->c_form,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
        i4 _on;
/* # line 482 "editapp.qsc" */	/* host code */
		i4	def, avail;
		if (! reentry)
		{
			FEdml(&def, &avail);
			_on = ( avail == FEDMLBOTH ) ? 0 : 1;
/* # line 488 "editapp.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,self->c_form,(char *)0, (char *)0) != 0) {
            IIstfsio(54,(char *)"dml",0,(short *)0,1,30,4,&_on);
          } /* IIiqset */
        }
/* # line 490 "editapp.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_name,(short *)0,1,32,0,app->name);
            IIputfldio((char *)"defaultstart",(short *)0,1,32,0,
            app->start_name);
            IIputfldio((char *)"dml",(short *)0,1,32,0,
            ERx(IIUGdmlStr(app->dml)));
          } /* IIfsetio */
        }
/* # line 496 "editapp.qsc" */	/* host code */
			_send(catLoad, self)(self, (char *)NULL, (char*)NULL);
			if (IIabVision)
			{
/* # line 500 "editapp.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 501 "editapp.qsc" */	/* host code */
			}
		}
		if (*curobj != NULL)
		{
			_VOID_ iiabCtFindComp(self,
					(*curobj)->name, (*curobj)->class
			);
		}
		if (! reentry)
/* # line 512 "editapp.qsc" */	/* resume */
        {
          IIresfld(self->c_tfield);
          if (1) goto IIfdB1;
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 517 "editapp.qsc" */	/* host code */
		OO_OBJECT	*obj;
		OOID		id = OC_UNDEFINED;
		if ( IIABchkSrcDir(app->source, TRUE)  &&
				IIABcreateComp( app, &id, self ) == OK  &&
					(obj = OOp(id)) != NULL )
		{
			IIABnelNewEditLock( obj );
			IIABedtComp( app, (APPL_COMP *)obj, self );
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 531 "editapp.qsc" */	/* host code */
		APPL_COMP *obj, *parent;
		bool trash;
		trash = IIABappFetch(app, app->name, OC_UNDEFINED, FALSE); 
		*topframe = find_top(app);
		if ( _send(catCheck, self)( self, &row ) == OK )
		{ /* get it first */
			obj = (*self->c_getcomp) ( app, row.id );
			if ( obj == NULL )
			{
/* # line 542 "editapp.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,self->c_form,self->c_tfield,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 543 "editapp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 544 "editapp.qsc" */	/* host code */
			}
			if (IIABcdlCheckDestroyLock( app, obj,
					FALSE, TRUE, &trash ) != OK)
			{
/* # line 549 "editapp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 550 "editapp.qsc" */	/* host code */
			}
			if (((USER_FRAME *) obj) == *topframe)
			{
				IIUGerr(S_AB0272_TopDestroy,0,0);
				parent = NULL;
			}
			else
			{
				parent = find_parent( app->comps, obj );
				if ( parent != NULL )
					IIUGerr( S_AB0273_ChildDestroy, 0,
						1, parent->name
					);
			}
			if ( _send(catDestroy, self)( self, &row ) == OK )
			{
				COMP_CLASS_INFO *ctmp;
				ctmp = iiabApCompClass(row.class, FALSE);
                                if (row.class == OC_OSLFRAME)
                                   IIAMcrCountRef( app, obj ); 
				de_chain(app,obj);
				if (parent != NULL)
				{
					de_reference((USER_FRAME *)parent, obj);
				}
				if (ctmp->destroy != NULL)
					(*(ctmp->destroy)) ( obj );
				IIOOforget(obj);
			}
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 590 "editapp.qsc" */	/* host code */
		if ( _send(catCheck, self)( self, &row ) == OK )
            {
              APPL_COMP *obj;
			obj = (*self->c_getcomp) ( app, row.id );
			if (obj == NULL)
/* # line 596 "editapp.qsc" */	/* deleterow */
              {
                if (IItbsetio(4,self->c_form,self->c_tfield,-2) != 0) {
                  if (IItdelrow(0) != 0) {
                  } /* IItdelrow */
                } /* IItbsetio */
              }
/* # line 597 "editapp.qsc" */	/* host code */
			else
			{
				if ( STequal(row.name, obj->name) )
				{ /* update row information */
					_send(catPutRow,self)(self,obj,rowCURR);
				}
				else
				{ /* name changed concurrently */
				}
				if ( IIABchkSrcDir(app->source, FALSE ) )
				{
					if (IIABcelCheckEditLock(obj) == OK)
					{
						IIABedtComp( app, obj, self);
						IIABrlRemLock(obj->ooid);
					}
				}
			}
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 621 "editapp.qsc" */	/* host code */
		iiabCtRenameComp(self, app);
          }
        } else if (IIretval() == 5) {
          {
/* # line 627 "editapp.qsc" */	/* host code */
		STATUS	stat = OK;
		char	*start_name;
		OOID	start_class;
		bool	usedef;
		APPL_COMP	*obj;
		/* Get the row information */
                if ( _send(catCheck, self)( self, &row ) != OK )
/* # line 635 "editapp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 637 "editapp.qsc" */	/* host code */
		/* Be sure we have a source directory */
		if ( !IIABchkSrcDir( app->source, FALSE ))
/* # line 639 "editapp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 641 "editapp.qsc" */	/* host code */
		/* Assume we'll start at the current row's component */
		start_name = row.name;
		start_class = row.class;
		usedef = FALSE;
		/* 
		** Get the frame/procedure to start with.  There are three
		** cases:
		**	1.  There is no default starting frame or procedure for
		**	    the application.  Prompt user for whether
		**	    he wants to start from the current row.
		**	2.  The default is different from the current row.  
		**	    Prompt user for which he wants to use.
		**	3.  The current row is the default starting place.
		**	    We'll use it.
		*/
		if (app->start_name == NULL || *app->start_name == EOS)
		{
			/* There is no default */
			if (!IIUFver(ERget(S_AB0005_StartAt), 1, row.name))
/* # line 661 "editapp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 662 "editapp.qsc" */	/* host code */
		}
		else if ( !STequal(app->start_name, row.name) )
		{
			/* 
			** There is a default, but we're not there.
			** Let the user choose between the two.
			*/
			i4  choice;
			char listpckbuf[256];
			bool def_is_proc = app->start_proc != 0;
			bool cur_is_proc = row.class < OC_APPLFRAME ||
					   row.class > OC_APPLFRAME + 99;
			_VOID_ IIUGfmt(listpckbuf, sizeof(listpckbuf), 
				ERget(F_AB02FD_ChooseBuf), 4, 
				ERget(cur_is_proc 
					? F_AB02FE_Procedure : F_AB02FF_Frame),
				row.name,
				ERget(def_is_proc
					? F_AB02FE_Procedure : F_AB02FF_Frame),
				app->start_name
			);
			choice = IIFDlpListPick(ERget(S_AB0006_ChooseStart),
					listpckbuf, 0, -1, -1, 
					ERget(S_AB0006_ChooseStart),
					ERx("abchstrt.hlp"), NULL, NULL);
			switch (choice)
			{
			    case 0:
				/* Use the current component */
				break;
			    case 1:
				/* Use the default component */
				start_name = app->start_name;
				start_class = (app->start_proc) ? OC_APPLPROC :
							          OC_APPLFRAME;
				usedef = TRUE;
				break;
			    default:
				/* User cancelled us */
/* # line 705 "editapp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 706 "editapp.qsc" */	/* host code */
				break;
			}
		}
		/* 
		** If we're using the default, assume we have to build the
		** whole image.
		*/
		if (usedef)
		{
			iiAB_test_image.one_frame = NULL;
			iiAB_test_image.plus_tree = FALSE;
		}
		else
		{
			/* Get the staring component */
			obj = (*self->c_getcomp) ( app, row.id );
			if (obj == NULL)
			{
/* # line 725 "editapp.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,self->c_form,self->c_tfield,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 726 "editapp.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 727 "editapp.qsc" */	/* host code */
			}
			iiAB_test_image.one_frame = obj;
			iiAB_test_image.plus_tree = TRUE;
		}
		if ((stat = IIAMapFetch(app, OC_UNDEFINED, 
					!iiAB_test_image.plus_tree)) 
		       == OK )
			_VOID_ IIABarunApp( app, &iiAB_test_image, start_name,
					    start_class);
		if ( stat == ILE_NOMEM )
/* # line 739 "editapp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 744 "editapp.qsc" */	/* host code */
		OOID	class;
		if ((class = pickGlobClass()) != OC_UNDEFINED)
			IIABcaCatalogEdit( app, class, noload, FALSE );
/* # line 748 "editapp.qsc" */	/* resume */
            {
              IIresfld(self->c_tfield);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 753 "editapp.qsc" */	/* host code */
		if ( IIABdefaults(app->ooid, app) == OK && app->data.dirty )
		{
			_VOID_ IIOOsave( &(app->ooid), (PTR)app,
					iiabSvApp, (STATUS (*)()) NULL
			);
/* # line 758 "editapp.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio((char *)"defaultstart",(short *)0,1,32,0,
                app->start_name);
              } /* IIfsetio */
            }
/* # line 759 "editapp.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 8) {
          {
/* # line 765 "editapp.qsc" */	/* host code */
		if (IIAMapFetch(app, OC_UNDEFINED, TRUE) == OK)
		{
			_VOID_ IIVQdceDisplayCompErrs(app, (OO_OBJECT *)NULL, 
						      FALSE);
		}
          }
        } else if (IIretval() == 9) {
          {
/* # line 774 "editapp.qsc" */	/* host code */
		IIAButilities(app, app->name, ABUT_FRMCAT, noload);
          }
        } else if (IIretval() == 10) {
          {
/* # line 779 "editapp.qsc" */	/* host code */
		_VOID_ IIfind_row(self->c_form, self->c_tfield, _);
          }
        } else if (IIretval() == 11) {
          {
/* # line 784 "editapp.qsc" */	/* host code */
		IIscrollto( _, _, 1 );
          }
        } else if (IIretval() == 12) {
          {
/* # line 789 "editapp.qsc" */	/* host code */
		IIscrollto( _, _, -1 );
          }
        } else if (IIretval() == 13) {
          {
/* # line 796 "editapp.qsc" */	/* host code */
		FEhelp( self->c_helpfile, self->c_init.m_menu );
          }
        } else if (IIretval() == 14) {
          {
/* # line 802 "editapp.qsc" */	/* host code */
		if ( self->c_end.m_menu == NULL )
/* # line 803 "editapp.qsc" */	/* message */
            {
              IImessage(ERget(S_AB0034_Exiting));
            }
/* # line 804 "editapp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 15) {
          {
/* # line 809 "editapp.qsc" */	/* host code */
		if ( self->c_quit.m_menu != NULL )
		{
			*eflag = TRUE;
/* # line 812 "editapp.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 813 "editapp.qsc" */	/* host code */
		}
		FTbell();
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
      if (IInmuact(self->c_create.m_menu,self->c_create.m_expl,2,2,1) == 0) 
      goto IIfdE1;
      if (IInmuact(self->c_destroy.m_menu,self->c_destroy.m_expl,2,2,2) == 0) 
      goto IIfdE1;
      if (IInmuact(self->c_edit.m_menu,self->c_edit.m_expl,2,2,3) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(FE_Rename),ERget(_RenameExpl),2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Go),ERget(_GoExpl),2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(AB_Parts),ERget(_PartsExpl),2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Defaults),ERget(_DefltExpl),2,2,7) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(F_AB02FB_Errors),ERget(F_AB02FC_ErrorsExpl),2,2,8) ==
       0) goto IIfdE1;
      if (IInmuact(ERget(FE_Utilities),ERget(_UtilExpl),2,2,9) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(_FindExpl),2,2,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(_TopExpl),2,2,11) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(_BottomExpl),2,2,12) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,13) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,13) == 0) goto IIfdE1;
      if (IInmuact(self->c_end.m_menu,self->c_end.m_expl,2,2,14) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,14) == 0) goto IIfdE1;
      if (IInmuact(self->c_quit.m_menu,self->c_quit.m_expl,2,2,15) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,15) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 817 "editapp.qsc" */	/* host code */
	if ( ! *eflag && reentry && _send(catCheck,self)(self,&row ) == OK )
		*curobj = (USER_FRAME *) (*self->c_getcomp) ( app, row.id );
	else
	{
		if (! Ref_reentry)
		{
			if (Ref_curid != OC_UNDEFINED)
				*curobj = (USER_FRAME *)
					(*self->c_getcomp)(app,Ref_curid);
			else
				*curobj = NULL;
			Ref_reentry = TRUE;
		}
		else
			*curobj = NULL;
	}
	*topframe = find_top(app);
	return OK;
  }
static VOID
  _a_catInit( self, mode )
  AB_CATALOG *self;
char		*mode;
  {
    char *rmode;
	rmode = mode != NULL ? mode : ERx("update");
/* # line 847 "editapp.qsc" */	/* inittable */
    {
      if (IItbinit(self->c_form,self->c_tfield,rmode) != 0) {
        IIthidecol(_id,(char *)"i4");
        IIthidecol(_class,(char *)"i4");
        IIthidecol(_env,(char *)"i4");
        IIthidecol(_owner,(char *)"c32");
        IIthidecol(_is_current,(char *)"i4");
        IIthidecol(_create_date,(char *)"c25");
        IIthidecol(_altr_date,(char *)"c25");
        IIthidecol(_altr_count,(char *)"i4");
        IIthidecol(_last_altered_by,(char *)"c32");
        IItfill();
      } /* IItbinit */
    }
  }
/* # line 860 "editapp.qsc" */	/* host code */
/*
** Name:	_a_catLoad() -	Load ABF Application Object Catalogs.
**
** Description:
**	Loads the frames and procedures table fields in the
**	Application Definition frame
**
** Input:
**	app	{APPL *}  The application currently being defined.
**
** Called by:
**	'IIABeditApp()'
**
** Side Effects:
**	Initializes the object-code symbol table.
**
** History:
**	01/89 (jhw) -- Written.
**	02-nov-1989 (wolf)  FT3270 has not initialized the menu at the point
**		the REDISPLAY is executed; skip it.
*/
static VOID
  _a_catLoad( self, pattern, lastname, prevrow )
  AB_CATALOG *self;
char		*pattern;
char		*lastname;
i4		*prevrow;
  {
	register i4	nrows = 0;
	bool		sys_conflicts = FALSE;
  AB_CATREC row;
  char pattern_buf[FE_MAXNAME*DB_MAX_COLS];
  char symbol[FE_MAXNAME+1];
  char source[FE_MAXNAME+1];
  OOID applid;
	if ( pattern == NULL || *pattern == EOS )
		STcopy( ERx("%"), pattern_buf );
	else
	{ /* change the pattern matching to SQL */
		i4	sql_wild;
		STcopy( pattern, pattern_buf );
		IIUGwildcard( pattern_buf, &sql_wild );
	}
	applid = self->c_appid;
/* # line 971 "editapp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"editapp1",6921,3550);
      IIputdomio((short *)0,1,32,0,pattern_buf);
      IIputdomio((short *)0,1,30,4,&applid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"editapp1",6921,3550);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select ii_objects.object_id, object_class, object_name, object_owner,\
 object_env, is_current, short_remark, create_date, alter_date, '', al\
ter_count, last_altered_by, abf_arg1, '', abf_symbol, abf_source from \
ii_objects, ii_abfobjects where ii_objects.");
        IIwritio(0,(short *)0,1,32,0,(char *)"object_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,pattern_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and ii_objects.object_id=ii_abfobjects.object_id and ii_abfobjects.ap\
plid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&applid);
        IIwritio(0,(short *)0,1,32,0,(char *)
"order by object_name, object_owner");
        IIexDefine(0,(char *)"editapp1",6921,3550);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&(row).id);
      IIgetdomio((short *)0,1,30,4,&(row).class);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,(row).name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(row).owner);
      IIgetdomio((short *)0,1,30,4,&(row).env);
      IIgetdomio((short *)0,1,30,4,&(row).is_current);
      IIgetdomio((short *)0,1,32,OOSHORTREMSIZE+1-1,(row).short_remark);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,(row).create_date);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,(row).alter_date);
      IIgetdomio((short *)0,1,32,OOLONGREMSIZE+1-1,(row).long_remark);
      IIgetdomio((short *)0,1,30,4,&(row).alter_cnt);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(row).altered_by);
      IIgetdomio((short *)0,1,32,TYPE_LEN+1-1,(row).type);
      IIgetdomio((short *)0,1,32,Y_N_LEN+1-1,(row).nullable);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,symbol);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,source);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 983 "editapp.sc" */	/* host code */
	{
		if ( ( row.class > OC_APPLFRAME &&
				row.class < OC_APPLFRAME + 100 )  ||
			row.class == OC_OSLPROC || row.class == OC_DBPROC )
		{
			STlcopy ( OOpclass(row.class)->name, row.type,
					sizeof(row.type)-1
			);
			if ( STtrmwhite(symbol) > 0 )
			{
				IIGNonOldName( ( row.class > OC_APPLFRAME &&
						row.class < OC_APPLFRAME + 100 )
							? AGN_OFRAME :AGN_OPROC,
						symbol
				);
			}
			if ( abFnwarn && ( row.class == OC_OSLPROC  ||
						row.class == OC_DBPROC ) &&
					IIAFckFuncName(row.name) )
			{
				sys_conflicts = TRUE;
			}
			if ( STtrmwhite(source) > 0 )
			{
				iiabOldFileName(source);
			}
		}
		else if ( row.class == OC_HLPROC )
		{
			if ( STtrmwhite(row.type) > 0 )
			{
				register i4	i;
				register char	*lang_str;
				for (i = 0 ; i < OLOSL ; ++i)
				{
					if ( (lang_str = OLanguage(i)) == NULL)
						continue;
					if ( STbcompare(lang_str, 0, row.type, 
								0, TRUE) == 0 )
					{
						STcopy(lang_str, row.type);
						break;
					}
				}
			}
			_VOID_ STprintf( row.type+STlength(row.type),
						ERx(" %s"), IIam_ProcCl->name		 
			);
			if ( STtrmwhite(source) > 0 )
			{
				iiabOldFileName(source);
			}
		}
		else
		{
			if ( row.class == OC_AFORMREF )
				IIGNonOldName(AGN_FORM, symbol);
			continue;
		}
		_send(catRowLoad, self)( self, &row );
		if ( ++nrows == self->c_maxrow  && ! IIabVision )
#ifdef FT3270
			;
#else
/* # line 989 "editapp.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 990 "editapp.qsc" */	/* host code */
#endif
		if ( lastname != NULL && STequal(lastname, row.name) &&
				prevrow != NULL )
			*prevrow = nrows;
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if ( FEinqerr() != OK )
		EXsignal(EXDECLARE, 0);
	if ( sys_conflicts )
	{ /* system procedre name conflicts */
		iiabReportConflicts(self->c_form, self->c_tfield);
	}
  }
static VOID
  _a_loadTable( self, csp )
  AB_CATALOG *self;
  AB_CATREC *csp;
  {
    char *owner = IIOOmapOwner(csp->owner);
/* # line 1013 "editapp.qsc" */	/* loadtable */
    {
      if (IItbact(self->c_form,self->c_tfield,1) != 0) {
        IItcoputio(_id,(short *)0,1,30,4,&csp->id);
        IItcoputio(_class,(short *)0,1,30,4,&csp->class);
        IItcoputio(_name,(short *)0,1,32,0,csp->name);
        IItcoputio(_env,(short *)0,1,30,4,&csp->env);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_is_current,(short *)0,1,30,4,&csp->is_current);
        IItcoputio(_short_remark,(short *)0,1,32,0,csp->short_remark);
        IItcoputio(_create_date,(short *)0,1,32,0,csp->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,csp->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,4,&csp->alter_cnt);
        IItcoputio(_last_altered_by,(short *)0,1,32,0,csp->altered_by);
        IItcoputio((char *)"type",(short *)0,1,32,0,csp->type);
        IITBceColEnd();
      } /* IItbact */
    }
  }
/* # line 1029 "editapp.qsc" */	/* host code */
static VOID
  _a_insertRow( self, obj, rowno )
  AB_CATALOG *self;
  register OO_OBJECT *obj;
  i4 rowno;
  {
    i4 numrows;
/* # line 1037 "editapp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,self->c_form,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&numrows,30,self->c_tfield,0);
      } /* IIiqset */
    }
/* # line 1039 "editapp.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,self->c_form,self->c_tfield,rowno) != 0) {
        if (IItinsert() != 0) {
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
/* # line 1041 "editapp.qsc" */	/* host code */
	if ( rowno < numrows )
		++rowno;
	_a_putRow( self, obj, rowno );
  }
static VOID
  _a_putRow( self, obj, rowno )
  AB_CATALOG *self;
  register OO_OBJECT *obj;
  i4 rowno;
  {
    char *owner = IIOOmapOwner(obj->owner);
    char type[TYPE_LEN+1];
	if ( obj->class == OC_HLPROC )
	{
		register HLPROC	*proc = (HLPROC *)obj;
		register char	*lang_str;
		if ( (lang_str = OLanguage(proc->ol_lang)) == NULL)
			lang_str = ERx("");
		_VOID_ STprintf( type, ERx("%s %s"), lang_str,
				IIam_ProcCl->name	 
		);
	}
	else
	{
		STlcopy ( OOpclass(obj->class)->name, type, sizeof(type)-1 );
	}
/* # line 1072 "editapp.qsc" */	/* putrow */
    {
      if (IItbsetio(3,self->c_form,self->c_tfield,rowno) != 0) {
        IItcoputio(_id,(short *)0,1,30,4,&obj->ooid);
        IItcoputio(_class,(short *)0,1,30,4,&obj->class);
        IItcoputio(_name,(short *)0,1,32,0,obj->name);
        IItcoputio(_env,(short *)0,1,30,4,&obj->env);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_is_current,(short *)0,1,30,4,&obj->is_current);
        IItcoputio(_short_remark,(short *)0,1,32,0,obj->short_remark);
        IItcoputio(_create_date,(short *)0,1,32,0,obj->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,obj->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IItcoputio(_last_altered_by,(short *)0,1,32,0,owner);
        IItcoputio((char *)"type",(short *)0,1,32,0,type);
        IITBceColEnd();
      } /* IItbsetio */
    }
  }
/* # line 1088 "editapp.qsc" */	/* host code */
/*
** Name - pickGlobClass
**
** Description:
**	Select, from a listpick, what kind of global data component to edit.
**
** Returns:
**	OOID - class selected.
*/
static READONLY 
  char _GlobSelectForm[]= ERx("abfglobselect"), _GlobTfld[]= ERx("gltypes");
static bool	_Gl_inited = FALSE;
static i4	_glchoice();
GLOBALREF char *IIabExename;
static OOID
  pickGlobClass()
  {
	OOID	rval = OC_UNDEFINED;
    char title[63+1];
	if (!_Gl_inited)
	{
		IIARiaForm(_GlobSelectForm);
/* # line 1115 "editapp.qsc" */	/* inittable */
    {
      if (IItbinit(_GlobSelectForm,_GlobTfld,(char *)"u") != 0) {
        IIthidecol((char *)"class",(char *)"i4");
        IItfill();
      } /* IItbinit */
    }
/* # line 1117 "editapp.qsc" */	/* host code */
		IIUGfmt( title, sizeof(title)-1, ERget(F_AB0302_Comp_title),
				1, IIabExename
			);
/* # line 1120 "editapp.qsc" */	/* putform */
    {
      if (IIfsetio(_GlobSelectForm) != 0) {
        IIputfldio(_title,(short *)0,1,32,0,title);
      } /* IIfsetio */
    }
/* # line 1122 "editapp.qsc" */	/* loadtable */
    {
      if (IItbact(_GlobSelectForm,_GlobTfld,1) != 0) {
        IItcoputio((char *)"gtype",(short *)0,1,32,0,ERget(AB_Globals));
        IItcoputio((char *)"description",(short *)0,1,32,0,
        ERget(F_AB0304_Glob_expl));
        IItcoputio((char *)"class",(short *)0,1,30,sizeof(OC_GLOBAL),
        (void *)IILQint(OC_GLOBAL));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1127 "editapp.qsc" */	/* loadtable */
    {
      if (IItbact(_GlobSelectForm,_GlobTfld,1) != 0) {
        IItcoputio((char *)"gtype",(short *)0,1,32,0,ERget(AB_Consts));
        IItcoputio((char *)"description",(short *)0,1,32,0,
        ERget(F_AB0356_Const_expl));
        IItcoputio((char *)"class",(short *)0,1,30,sizeof(OC_CONST),
        (void *)IILQint(OC_CONST));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1132 "editapp.qsc" */	/* loadtable */
    {
      if (IItbact(_GlobSelectForm,_GlobTfld,1) != 0) {
        IItcoputio((char *)"gtype",(short *)0,1,32,0,ERget(AB_Records));
        IItcoputio((char *)"description",(short *)0,1,32,0,
        ERget(F_AB0334_Rec_expl));
        IItcoputio((char *)"class",(short *)0,1,30,sizeof(OC_RECORD),
        (void *)IILQint(OC_RECORD));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1137 "editapp.qsc" */	/* host code */
		_Gl_inited = TRUE;
	}
	else
	{
/* # line 1141 "editapp.qsc" */	/* getform */
    {
      if (IIfsetio(_GlobSelectForm) != 0) {
        IIgetfldio((short *)0,1,32,63+1-1,title,_title);
      } /* IIfsetio */
    }
/* # line 1142 "editapp.qsc" */	/* host code */
	}
	_VOID_ IIFRrlpRunListPick(_GlobSelectForm, _GlobTfld,
			LPGR_FLOAT, LPGR_FLOAT, title, ERx("abcomps.hlp"), 
			_glchoice, (PTR) &rval
		);
	return rval;
  }
static i4
  _glchoice(dat,ret,res,idx)
PTR	dat;
i4	ret;
bool	*res;
i4	idx;
  {
    OOID *rval = (OOID *) dat;
	if (ret != -1)
	{
/* # line 1162 "editapp.qsc" */	/* getrow */
    {
      if (IItbsetio(2,_GlobSelectForm,_GlobTfld,-2) != 0) {
        IItcogetio((short *)0,1,30,4,rval,(char *)"class");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1163 "editapp.qsc" */	/* host code */
	}
	return ret;
  }
VOID
iiabCtRenameComp ( self, app )
AB_CATALOG	*self;
APPL		*app;
{
	char		oldname[FE_MAXNAME+1];
	AB_CATREC	row;
	if ( _send(catRename, self)(self, &row, oldname, app->ooid) == OK )
	{
		APPL_COMP	*obj = (APPL_COMP *)OOp(row.id);
		/* Fix up the dependency structures */
		if (obj != NULL)
			IIAMxrcRenameComp(app, obj, oldname);
		if ( obj != NULL && IIabVision )
		{
			if ( !app->start_proc
					&& STequal(oldname, app->start_name) )
			{
				app->start_name = obj->name;
				app->data.dirty = TRUE;
				_VOID_ IIOOsave( &(app->ooid), (PTR)app,
							iiabSvApp
				);
			}
			obj = find_parent( app->comps, obj );
			while (obj != NULL)
			{
				invalidate_obj( obj );
				obj = find_parent(obj->_next, obj);
			}
		}
		if ( row.class == OC_HLPROC )
		{
			if ( obj != NULL && !BTtestF(IIam_ProcCl->level,		 
						obj->data.levelbits) )
			{ /* fetch level-1 info for host-language procedure
				** and update row for type description.
				*/
				_send(catPutRow, self)( self,
					(*self->c_getcomp)(app, row.id),
					rowCURR
				);
			}
			/*
			** perhaps pessimistic, but if this host-lang
			** procedures resides in a library, then the
			** interpreter is no good.
			*/
			iiabRiRmInterp();
		}
	}
}
i4
  iiabCtFindComp( self, name, class )
  AB_CATALOG *self;
char		*name;
OOID		class;
  {
    OOID tclass;
    i4 rec;
    char tname[FE_MAXNAME+1];
	rec = 0;
/* # line 1235 "editapp.qsc" */	/* unloadtable */
    {
      if (IItbact(self->c_form,self->c_tfield,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,30,4,&tclass,_class);
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,tname,_name);
        IItcogetio((short *)0,1,30,4,&rec,(char *)"_RECORD");
        IITBceColEnd();
        {
/* # line 1238 "editapp.qsc" */	/* host code */
		if (STequal(tname,name) && tclass == class)
		{
/* # line 1240 "editapp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,self->c_form,self->c_tfield,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,rec) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 1241 "editapp.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 1242 "editapp.qsc" */	/* host code */
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 1245 "editapp.qsc" */	/* host code */
	return rec;
  }
/*{
** Name - IIABcotChangeObjectType
**
** Description:
**	Change an existing object to one of another type.  This is
**	used by INGRES/VISION, which initially always presents the
**	user with a MENU frame, which is changed to another type of
**	object.  Unavoidably, a NEW pointer is returned, and the old
**	one is no longer a valid address.
**
** Inputs:
**	app - application.
**	oobj - old object.
**	class - class of new object.
**	ctx - catalog context.
**
** Outputs:
**	aobj - new object.
**
**	Returns:
**		STATUS.
*/
STATUS
IIABcotChangeObjectType(app,oobj,class,ctx,aobj)
APPL *app;
OO_OBJECT *oobj;
OOID class;
PTR ctx;
OO_OBJECT **aobj;
{
  OO_CATALOG *catalog;
	register USER_FRAME	*parent;
	STATUS			rc;
	OOID			fid;
	/*
	** catch same class as a special case, and avoid a lot of work.
	*/
	if (class == oobj->class)
	{
		*aobj = oobj;
		return OK;
	}
	*aobj = NULL;
	parent = (USER_FRAME *)find_parent( app->comps, oobj );
	catalog = (OO_CATALOG *) ctx;
	rc = OK;
	/*
	** the various types that are USER_FRAME structures can be interchanged.
	** Do so, NULLing or adding a metaframe pointer if neccesary.
	*/
	switch(oobj->class)
	{
	  case OC_OSLFRAME:
	  case OC_MUFRAME:
	  case OC_APPFRAME:
	  case OC_UPDFRAME:
	  case OC_BRWFRAME:
		fid = ((USER_FRAME *) oobj)->fid;
		switch(class)
		{
		case OC_OSLFRAME:
			((USER_FRAME *) oobj)->mf = NULL;
			*aobj = oobj;
			break;
		case OC_MUFRAME:
		case OC_APPFRAME:
		case OC_UPDFRAME:
		case OC_BRWFRAME:
			if (((USER_FRAME *) oobj)->mf == NULL)
				IIAMamAttachMeta(oobj);
			*aobj = oobj;
			break;
		default:
			break;
		}
		break;
	case OC_OSLPROC:
		fid = ((_4GLPROC *) oobj)->fid;
		break;
	default:
		fid = OC_UNDEFINED;
		break;
	}
	/*
	** If we cannot simply change the class,  Destroy the object,
	** and create a new one.
	*/
	if (*aobj == NULL)
	{
		COMP_CLASS_INFO	*ctmp;
		char		tname[FE_MAXNAME+1];
/* # line 1345 "editapp.qsc" */	/* message */
  {
    IImessage(ERget(S_AB0274_Changing));
  }
/* # line 1347 "editapp.qsc" */	/* host code */
		/*
		** remove the old catalog row.  In order to do this using the
		** proper method, we have to scroll the tablefield to the
		** correct row, then fetch the row information.  We do this
		** BEFORE actually placing a new class in the object, so that
		** we can be sure of finding the old one.  We directly delete
		** the tablefield row in order to avoid the OO "destroy?"
		** prompt, and underlying DB call.
		*/
		if ( catalog != NULL
			    && iiabCtFindComp(catalog, oobj->name, oobj->class)
				> 0 )
		{
/* # line 1360 "editapp.qsc" */	/* deleterow */
  {
    if (IItbsetio(4,catalog->c_form,catalog->c_tfield,-2) != 0) {
      if (IItdelrow(0) != 0) {
      } /* IItdelrow */
    } /* IItbsetio */
  }
/* # line 1361 "editapp.qsc" */	/* host code */
		}
		de_chain(app,oobj);
		STcopy(oobj->name,tname);
		ctmp = iiabApCompClass(oobj->class, FALSE);
		if (ctmp->destroy != NULL)
			(*(ctmp->destroy))( oobj );
		/*
		** call IIAMcdCatDel directly to avoid object manager
		** prompt and messages.  This leaves our object cached
		** in OO, but since there will no longer be any record
		** of it, that shouldn't hurt anything.
		*/
		IIAMcdCatDel( app->ooid, oobj->ooid, fid, oobj->class );
		rc = IIABmdoMakeDefaultObj(app,tname,class,ctx,aobj);
	}
	else
	{
		oobj->class = class;
		if ( catalog != NULL )
			_send(catPutRow, catalog)( catalog, oobj, rowCURR );
	}
	/*
	** If it has a parent, change the child pointer, and rewrite
	** the menu dependencies.
	*/
	while (rc == OK && parent != NULL)
	{
		METAFRAME	*mf;
		register MFMENU	**m;
		register int	i;
		m = (mf = parent->mf)->menus;
		for ( i = mf->nummenus ; --i >= 0 ; ++m )
			if ((*m)->apobj == (OO_OBJECT *) oobj)
				(*m)->apobj = *aobj;
		iiuicw1_CatWriteOn();
		IIAMwmdWriteMenuDep(mf,TRUE);
		iiuicw0_CatWriteOff();
		parent = (USER_FRAME *)find_parent( parent->_next, oobj );
	}
	return OK;
}
/*{
** Name - IIABdcvDestroyCompVision
**
** Description:
**	Entry to Destroy component from INGRES/VISION.  This entry deletes
**	silently, and does not prompt.  It does fix up the references in
**	the parent metaframe.
**
** Inputs:
**	appl - application.
**	comp - component.
**	ctx - context pointer given to frame flow entry.
**		or NULL for calls from iiabCompile via "go" or "image".
**
** Outputs:
**	None.
**
** Returns:
**	None.
**
** History:
**	24-apr-91 (blaise)
**		Change function type to VOID, since it doesn't return anything.
*/
VOID
IIABdcvDestroyCompVision( appl, comp, ctx)
APPL		*appl;
OO_OBJECT	*comp;
PTR		*ctx;
  {
    OO_CATALOG *catalog;
	register USER_FRAME	*parent;
	COMP_CLASS_INFO		*ctmp;
	OOID			fid;
	switch(comp->class)
	{
	 case OC_OSLFRAME:
	 case OC_MUFRAME:
	 case OC_APPFRAME:
	 case OC_UPDFRAME:
	 case OC_BRWFRAME:
		fid = ((USER_FRAME *) comp)->fid;
		break;
	 case OC_OSLPROC:
		fid = ((_4GLPROC *) comp)->fid;
		break;
	 default:
		fid = OC_UNDEFINED;
		break;
	}
	parent = (USER_FRAME *) find_parent( appl->comps, comp );
	while ( parent != NULL )
	{
		de_reference( parent, comp );
		parent = (USER_FRAME *)find_parent( parent->_next, comp );
	}
	catalog = (OO_CATALOG *) ctx;
	if ( catalog != NULL
		    && iiabCtFindComp(catalog,comp->name,comp->class) > 0 )
	{
/* # line 1474 "editapp.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,catalog->c_form,catalog->c_tfield,-2) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 1475 "editapp.qsc" */	/* host code */
	}
	de_chain(appl,comp);
	ctmp = iiabApCompClass(comp->class, FALSE);
	if (ctmp->destroy != NULL)
		(*(ctmp->destroy))( comp );
	IIAMcdCatDel( appl->ooid, comp->ooid, fid, comp->class );
  }
/*{
** Name: IIABnsrNewShortRemark
**
** Description:
**	called from IIABucvUpdCompVision to assure that
**	changes by Vision are reflected in the frame catalog.
**	The short remarks column is the only visible one.
**
**	Note that this will fail if an attempt is made using ucv to
**	change the type of an object, since the catalog frame is keyed
**	by class.
**
** Inputs:
**	app	application
**	comp	component being updated.
**	ctx	context pointer passed to vision
**
*/
VOID
IIABnsrNewShortRemark(app, comp, ctx)
APPL *app;
  OO_OBJECT *comp;
PTR ctx;
  {
    OO_CATALOG *catalog;
	catalog = (OO_CATALOG *) ctx;
	if ( catalog == NULL
			|| iiabCtFindComp(catalog,comp->name,comp->class) == 0 )
		return;
/* # line 1517 "editapp.qsc" */	/* putrow */
    {
      if (IItbsetio(3,catalog->c_form,catalog->c_tfield,-2) != 0) {
        IItcoputio(_short_remark,(short *)0,1,32,0,comp->short_remark);
        IITBceColEnd();
      } /* IItbsetio */
    }
  }
/* # line 1522 "editapp.qsc" */	/* host code */
/*{
** Name: find_top
**
** Description:
**	Local routine to find top frame of application for frame
**	flow diagram.  Only metaframe type frames may qualify -
**	return NULL if there is no starting frame OR if it is not
**	of the proper type.
*/
static USER_FRAME *
find_top(app)
register APPL *app;
{
	register APPL_COMP *obj;
	if ( app->start_proc || app->start_name == NULL
				|| *app->start_name == EOS )
		return NULL;
	for (obj = (APPL_COMP *) app->comps; obj != NULL; obj = obj->_next)
	{
		switch (obj->class)
		{
		case OC_MUFRAME:
		case OC_APPFRAME:
		case OC_UPDFRAME:
		case OC_BRWFRAME:
			if (STequal(app->start_name,obj->name))
			{
				register APPL_COMP	*parent;
				parent = find_parent( app->comps, obj );
				while (parent != NULL)
				{
					de_reference((USER_FRAME *)parent, obj);
					parent = find_parent(parent->_next,obj);
				}
				return (USER_FRAME *)obj;
			}
		} /* end switch */
	}
	return NULL;
}
/*{
** Name: new_top
**
** Description:
**	Local routine to add top frame to a new vision application.
*/
static USER_FRAME *
new_top(app)
register APPL *app;
{
	char bufr[FE_MAXNAME+1];
	USER_FRAME *obj;
	IIVQmfnMakeFrameName(app, app->name, bufr);
	if (IIABmdoMakeDefaultObj(app, bufr, OC_MUFRAME,
					(PTR) &iiabAcatalog, &obj) != OK)
		return NULL;
	obj->short_remark = app->short_remark;
	if (IIABucvUpdCompVision(app, obj, (PTR) &iiabAcatalog) != OK)
		return NULL;
	app->start_name = obj->name;
	app->data.dirty = TRUE;
	_VOID_ IIOOsave( &(app->ooid), (PTR)app, iiabSvApp );
	return obj;
}
/*
** Name: de_chain
**
** Description:
**	Local routine to remove object from application component
**	chain.  Stricter maintenance of this list is needed for Vision.
*/
static VOID
de_chain(app,obj)
register APPL *app;
APPL_COMP *obj;
{
	register APPL_COMP *op, *pp;
	pp = NULL;
	for (op = (APPL_COMP *) app->comps; op != NULL; op = (pp=op)->_next)
		if (op == obj)
			break;
	if (op != NULL)
	{
		if (pp == NULL)
			app->comps = (OO_OBJECT *) op->_next;
		else
			pp->_next = op->_next;
	}
}
/*
** Name: find_parent
**
** Description:
**	Local routine to find parent metaframes (if any) for a given object.
**	Called with first = TRUE to initiate search, FALSE to continue
**	previous search.
*/
static APPL_COMP *
find_parent ( start, cobj )
APPL_COMP		*start;
register APPL_COMP	*cobj;
{
	register APPL_COMP	*obj = start;
	for ( ; obj != NULL ; obj = obj->_next )
	{
		switch (obj->class)
		{
		  case OC_MUFRAME:
		  case OC_APPFRAME:
		  case OC_UPDFRAME:
		  case OC_BRWFRAME:
		  {
			METAFRAME	*mf = ((USER_FRAME *)obj)->mf;
			if ( mf != NULL )
			{
				register MFMENU	**m;
				register i4	i;
				m = mf->menus;
				for ( i = mf->nummenus ; --i >= 0 ; ++m )
					if ((*m)->apobj == (OO_OBJECT *) cobj)
					{
						return obj;
					}
			}
			break;
		  }
		} /* end switch */
	}
	return NULL;
}
/*
** Name: de_reference
**
** Description:
**	Local routine to remove menu reference from parent metaframe.
*/
static VOID
de_reference ( parent, obj )
USER_FRAME		*parent;
register APPL_COMP	*obj;
{
	METAFRAME	*mf;
	register MFMENU	**m;
	register i4	i;
	m = (mf = parent->mf)->menus;
	for ( i = mf->nummenus ; --i >= 0 ; ++m )
	{
		if ((*m)->apobj == (OO_OBJECT *) obj)
		{
			/*
			** not using MEcopy - I'm paranoid about
			** overlapping addresses
			*/
			while( --i >= 0 )
			{
				*m = *(m+1);
				++m;
			}
			--(mf->nummenus);
			invalidate_obj(parent);
			break;
		}
	} /* end for */
}
/*
** local routine to invalidate metaframe type objects by
** flagging the source for regeneration.  Also, updates the
** menu dependencies in the catalogs so they reflect reality again.
*/
static VOID
invalidate_obj ( obj )
register USER_FRAME	*obj;
{
	if (obj->mf != NULL)
		IIAMwmdWriteMenuDep(obj->mf,TRUE);
	obj->mf->state |= MFST_DOGEN|MFST_DOFORM;
	IIAMufqUserFrameQuick(obj);
}
/*{
** Name - IIABvrVisionRefresh
**
** Description:
**	Entry to refresh application component list from INGRES/VISION.
**	Reinitializes the frame catalog as well.
**
**	NOTE:
**
**	This is sheer hackery.  This sets up some global status in
**	THIS routine with the understanding that FrameFlowEntry is going to
**	return after calling this.
**
** Inputs:
**	appl - application.
**	ctx - context pointer given to frame flow entry.
**	comp - current component - returned possibly "relocated".
**
** Outputs:
**	None.
**
**	Returns:
**		STATUS
*/
STATUS
IIABvrVisionRefresh( appl, ctx, cur )
APPL		*appl;
PTR		ctx;
OO_OBJECT	*cur;
{
	AB_CATALOG	*cat;
	char		tmp[FE_MAXNAME+1];
	bool		trash;
	OO_OBJECT	*trashcur;
	OO_OBJECT	*trashtop;
	cat = (AB_CATALOG *) ctx;
	if (cur != NULL)
		Ref_curid = cur->ooid;
	else
		Ref_curid = OC_UNDEFINED;
	Ref_reentry = FALSE;
	STcopy(appl->name,tmp);
	IIUGmsg(ERget(F_AB00B2_CatMsg), FALSE, 0);
	IIAMappFree(appl);
	IIABappFetch(appl, tmp, OC_UNDEFINED, TRUE);
}
