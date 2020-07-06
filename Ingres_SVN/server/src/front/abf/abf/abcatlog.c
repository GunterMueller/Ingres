# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 2004 Ingres Corporation
**  All rights reserved.
*/
#include	<compat.h>
#include	<cv.h>
#include	<lo.h>
#include	<st.h>
#include	<er.h>
#include	<nm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<uf.h>
#include	<fedml.h>
# include <abclass.h>
# include <oocat.h>
# include <oocatlog.h>
#include	<uigdata.h>
#include	<erfg.h>
#include	"erab.h"
#include	<oodefine.h>
/**
** Name:	abcatlog.qc -	ABF Catalog Frame.
**
** Description:
**	Contains the ABF catalog frame and some related routines.  Defines:
**
**	IIABcatlogs()		ABF catalog frame.
**	IIABrappRename()	Rename an application
**
** History:
**	Revision 6.1  88/03/08  wong
**	Initial revision.
**
**	18-jan-91 (blaise)
**	    Added new functions IIABrappRename(), rename_object() and
**	    get_new_appname(), to change the procedure for renaming an
**	    application. Previously an application was renamed by (i) changing
**	    the object catalog then (ii) creating a new directory for the
**	    application and deleting the old one. This meant that if (ii)
**	    failed then the renaming process was left only half complete. The
**	    procedure now is to get the name for the application, create the
**	    directory $ING_ABFDIR/<newappname>, then, if this is successful,
**	    rename the application in the object catalog and delete
**	    $ING_ABFDIR/<oldappname>. If the procedure fails in any way, delete
**	    the directory $ING_ABFDIR/<newappname> which we've just created.
**	    Bug #34960.
**	19-aug-91 (blaise)
**	    Added missing #include cv.h.
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
**	25-jan-93 (davel)
**		Clear global constant file specification on return from
**		IIABeditApp().
**	01-feb-93 (davel)
**		Correct previous change: clear global constant file 
**		specification before calling IIABeditApp(), IIABrun(), or 
**		IIAButilities().  Introduced static function clear_gconsf().
**	04-mar-93 (kchin) 
**		Included oodefine.h for function declaration of OOp(), 
**		since it returns OO_OBJECT *.  On 64-bit platform, the 
**		default int return value for a function will truncate an 
**		address which is 64-bit long.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	08-feb-2001 (somsa01)
**	    Properly defined the type of IIabWrkDir.
*/
/* defines */
/* Explanations */
#define	_CreateExpl	F_AB0007_Create_expl
#define	_DestroyExpl	F_AB0008_Destroy_expl
#define	_EditExpl	F_AB0009_Edit_expl
#define	_RenameExpl	F_AB000A_Rename_expl
#define _MoreInfoExpl	F_FE0107_ExplMoreInfo
#define	_GoExpl		F_AB000B_Go_expl
#define	_UtilExpl	F_AB000C_Util_expl
#define	_FindExpl	F_FE0104_ExplFind
#define	_TopExpl	F_FE0105_ExplTop
#define	_BottomExpl	F_FE0106_ExplBottom
#define	_HelpExpl	F_FE0100_ExplFrameHelp
#define	_QuitExpl	F_AB000D_Quit_expl
#define _DefExpl	F_AB0010_Defaults_expl
/* Titles */
#define	_CatalogTitle	F_AB0001_Cat_title
#define	_CreateTitle	F_AB0002_AppCreate_title
#define	_DetailTitle	F_AB0003_AppDetail_title
/* globals */
GLOBALREF char	*IIabExename;
GLOBALREF bool	IIabVision;
GLOBALREF LOCATION	IIabWrkDir;
GLOBALREF OO_CLASS	*IIam_Class;		 
/* statics */
static i4	current_rowid = 0;
static VOID	clear_gconsf (i4 rowid);
static STATUS	ap_init (OOID id, PTR data );
static STATUS	ap_create (OOID id, PTR *data );
static STATUS	detail (OOID applid, PTR data );
static VOID	_abCatLoad (OO_CATALOG *self, char *pattern, 
				char *lastname, i4  *prevrow);
static STATUS	get_new_appname (OO_CATALOG *self, OO_CATREC *row, 
				char *newname);
static STATUS	rename_object();
static READONLY
  char _create[]= ERx("abfcreate"), _srcdir[]= ERx("srcdir"), _dml[]= ERx("dml")
  , _menustyle[]= ERx("menustyle");
STATUS	IIABdefaults();
VOID	IIABrappRename();
i4	IIFGmsmMenuStyleMask();
VOID	IIABscfSetConstantFile();
/*{
** Name:	iiabSvApp() -	Save Application Object.
**
** Description:
**	Saves an application object if it was changed through the ABF
**	Application Defaults frame.  A changed
**	entry has a non-negative application object ID.
**
** Input:
**	applid	{OOID}  Application object ID.
**	app	{APPL *}  Application object.
**
** Returns:
**	{STATUS}  OK or 'IIAMcuCatUpd()' status.
**
** History:
**	03/88 (jhw) -- Written.
*/
STATUS
iiabSvApp ( applid, data )
OOID	applid;
PTR	data;
{
	register APPL	*app = (APPL *)data;
	return ( app->ooid != OC_UNDEFINED && app->data.dirty )
			? IIAMwrApp(app) : OK;
}
/*
** Name:	ap_init() -	Initialize Application Object Entry for Create.
**
** Description:
**	Initializes the application object entry for an application that is
**	being created, and displays some initial values on the Create form.
**
** Input:
**	id	{OOID}  Application object ID.
**	data	{PTR ==> APPL *}  Application object specific data.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	03/88 (jhw) -- Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	9/90 (Mike S) Ifdef source directory creation for hp9_mpe.
**		Porting change 130508 from mrelac.
**	3/91 (Mike S) Initialize "listchoicestyle" field
*/
char	*IIUGdmlStr();
static STATUS
ap_init ( OOID id, PTR data )
{
	if ( id == OC_UNDEFINED && data != NULL )
  {
    register APPL *app = (APPL *)data;
    i4 toggle;
		char		*cp;
		i4		def;
		i4		avail;
		char		buf[MAX_LOC+1];
		LOCATION	loc;
		app->data.inDatabase = FALSE;
		app->owner = IIUIdbdata()->user;
# ifdef CMS
		STcopy(ERx("A:"), cp = buf);	/* use 'A' as default
							source-code directory
							under CMS */
# else
#  ifdef hp9_mpe
		NMgtAt(ERx("HPACCOUNT"), &cp);
#  else
		LOgt(buf, &loc);
		LOtos(&loc, &cp);
#  endif
# endif
		app->source = FEtsalloc( app->data.tag, ( cp != buf ) ? cp : buf );
		/*
		** in Vision, we can only create SQL applications.
		*/
		if (IIabVision)
		{
			app->dml = DB_SQL;
			toggle = TRUE;
/* # line 222 "abcatlog.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,_create,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_menustyle,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 223 "abcatlog.qsc" */	/* putform */
    {
      if (IIfsetio(_create) != 0) {
        IIputfldio(_menustyle,(short *)0,1,32,0,ERget(F_FG0040_TableField));
      } /* IIfsetio */
    }
/* # line 224 "abcatlog.qsc" */	/* host code */
		}
		else
		{
			FEdml(&def, &avail);
			if ( avail == FEDMLSQL )
			{
				app->dml = DB_SQL;
				toggle = TRUE;
			}
			else if ( avail == FEDMLQUEL )
			{
				app->dml = DB_QUEL;
				toggle = TRUE;
			}
			else
			{ /* both, use default or SQL */
				app->dml = ( def == FEDMLQUEL ) ? DB_QUEL : DB_SQL;
				toggle = FALSE;
			}
/* # line 243 "abcatlog.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,_create,(char *)0, (char *)0) != 0) {
        IIstfsio(54,_menustyle,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 244 "abcatlog.qsc" */	/* host code */
		}
/* # line 247 "abcatlog.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,_create,(char *)0, (char *)0) != 0) {
        IIstfsio(53,_dml,0,(short *)0,1,30,4,&toggle);
      } /* IIiqset */
    }
/* # line 249 "abcatlog.qsc" */	/* host code */
		/* Display */
/* # line 250 "abcatlog.qsc" */	/* putform */
    {
      if (IIfsetio(_create) != 0) {
        IIputfldio(_srcdir,(short *)0,1,32,0,app->source);
        IIputfldio(_dml,(short *)0,1,32,0,ERx(IIUGdmlStr(app->dml)));
      } /* IIfsetio */
    }
/* # line 254 "abcatlog.qsc" */	/* host code */
		app->data.dirty = TRUE;
		app->data.levelbits |= ( 1 << IIam_Class->level );		 
  }
	return OK;
}
/*
** Name:	ap_create() -	Create Application Object Catalog Entry.
**
** Description:
**	Writes the application object catalog entry for an application when it
**	is created.  The user-settable values, such as the source directory and
**	default start frame, are fetched from the form.
**
**	This routine is called from the Create frame of the OO module
**	('IIOOcreateFrame()',) which is passed this routine when it is executed.
**
** Input:
**	id	{OOID}  Application object ID.
**	data	{PTR ==> APPL *}  Application object specific data.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/88 (jhw) -- Written.
**	2/90 (Mike S) -- Check for presence of object code directory
**	6/90 (Mike S) -- Check for syntax of source directory
**	07/90 (jhw) -- Added object-code directory location to
**			'IIABcdirCreate()' call.
*/
static STATUS
ap_create ( OOID id, PTR *data )
  {
	register APPL	*app = (APPL *)data;
    char name[FE_MAXNAME+1];
    char source[180+1];
    char dml[5+1];
    char defmenustyle[20];
	STATUS		stat;
	if ( app == NULL || id < 0 )
		return FAIL;
	app->ooid = id;
/* # line 300 "abcatlog.qsc" */	/* getform */
    {
      if (IIfsetio(_create) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,name,_name);
        IIgetfldio((short *)0,1,32,180+1-1,source,_srcdir);
        IIgetfldio((short *)0,1,32,5+1-1,dml,_dml);
        IIgetfldio((short *)0,1,32,19,defmenustyle,_menustyle);
      } /* IIfsetio */
    }
/* # line 303 "abcatlog.qsc" */	/* host code */
	if ( app->name == NULL || !STequal(app->name, name) )
	{
		if ( FEchkname(name) != OK )
		{
			IIUGerr(E_UG000E_BadName, UG_ERR_ERROR, 1, name);
			return FAIL;
		}
		app->name = FEtsalloc(app->data.tag, name);
	}
	_VOID_  STtrmwhite(source);
	if (!IIABchkSrcDir(source, TRUE))
	{
		return FAIL;
	}
	else
	{
		app->source = FEtsalloc(app->data.tag, source);
#ifdef CMS
		source[1] = ':';
		source[2] = EOS;
		CVupper(source);
#endif
	}
	app->comps = NULL;
	app->executable = _;
	app->start_name = _;
	app->link_file = _;
	app->batch = app->start_proc = FALSE;
	app->roleid = _;
	app->password = (char *)NULL;
	app->flags = 0;
	if (IIabVision)
	{
		i4 ms_mask;
		/* Set mask for default menu style */
		ms_mask = IIFGmsmMenuStyleMask(defmenustyle);
		app->flags &= ~APC_MS_MASK;
		app->flags |= ms_mask;
	}
	if ( app->dml != IIUGstrDml(dml) )
	{
		i4	def;
		i4	avail;
		FEdml(&def, &avail);
		if ( avail != FEDMLQUEL && avail != FEDMLSQL )
			app->dml = IIUGstrDml(dml);
	}
	/* 
	** If anything's going to fail, it's probably the directory create.
	** That's also easy for the user to back out, so let's do it first.
	*/
	if ((stat = IIABcdirCreate(app->name, &IIabWrkDir)) != OK)
		return stat;
	if ((stat = IIAMwrApp(app)) != OK )
	{
		abproerr(ERx("CreateApp"), BADAPPEND, (char *)NULL);
	}
	return stat;
  }
/*
** Name:	detail() -	Reset Application Object.
**
** Description:
**	Reset application object for detail frame by setting application
**	object ID to UNKNOWN.
**
**	Used by 'catDetail' method of AB_CATALOG class.
**
** Input:
**	applid	{OOID}  Application object ID.
**	app	{APPL *}  Application object.
**
** Returns:
**	{STATUS}  OK.
**
** History:
**	03/88 (jhw) -- Written.
*/
static STATUS
detail ( OOID applid, PTR data )
{
	register APPL	*app = (APPL *)data;
	if ( app->ooid != OC_UNDEFINED )
	{
		if ( app->ooid != applid )
			IIOOforget(app);
		app->ooid = OC_UNDEFINED;
		app->data.dirty = app->data.inDatabase = FALSE;
		app->data.levelbits = 0;
	}
	return OK;
}
static VOID
_abCatLoad ( OO_CATALOG *self, char *pattern, char *lastname, i4  *prevrow )
{
	IIOOctObjLoad(self, FALSE, pattern, (char *)NULL, lastname, &prevrow);
}
static bool	detailf = FALSE;
static bool	savef = FALSE;
static bool	catf = FALSE;
/*{
** Name:	IIABcatalogs() -	ABF Applications Catalog Frame.
**
** Description:
**	Displays the catalog of ABF applications and supports various operations
**	on the applications.
**
** Inputs:
**	expert	{bool}  Whether to run in expert mode.
**
** History:
**	03/88 (jhw)  First revision.
**	02/89 (jhw)  Completed.
*/
VOID
IIABcatalogs ( expert )
bool	expert;
  {
	bool	qflag;
    char tbuf[120];
	/* AB_CATALOG sub-class of OO_CATALOG */
	static READONLY C_OO_CATALOG	_abcatalog =
	{
		IIOOct0Init,		/* catInit */
		IIOOctEmpty,		/* catEmpty */
		IIOOctCheck,		/* catCheck */
		IIOOctLoad,		/* catLoad */
		IIOOctDestroy,		/* catDestroy */
		IIOOctRename,		/* catRename */
		IIOOctFetch,		/* catFetch */
		IIOOctMoreInfo,		/* catDetail */
		IIOOctTFldInit,		/* catTFldInit */
		_abCatLoad,		/* catTFldLoad */
		IIOOctRowLoad,		/* catRowLoad */
		IIOOctInsertRow,	/* catInsertRow */
		IIOOctPutRow		/* catPutRow */
	};
	static READONLY 
    char *_CatForm =	_iicatalog;
    GLOBALREF char _cattable[];
	GLOBALREF char		_iidetail[];
	u_i4		tag = FEgettag();
	OO_CATALOG	catalog;
	if (!catf)
	{
		IIARiaForm(_CatForm);
		catf = TRUE;
	}
	catalog.class = &_abcatalog;
	_send(catInit, &catalog)( &catalog, IIam_Class, _CatForm, _cattable, expert );
/* # line 470 "abcatlog.qsc" */	/* display */
    {
      if (IIdispfrm(_CatForm,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 474 "abcatlog.qsc" */	/* host code */
		IIUGfmt(tbuf,119,ERget(_CatalogTitle),1,IIabExename);
/* # line 475 "abcatlog.qsc" */	/* putform */
        {
          if (IIfsetio(_CatForm) != 0) {
            IIputfldio((char *)"title",(short *)0,1,32,0,tbuf);
          } /* IIfsetio */
        }
/* # line 477 "abcatlog.qsc" */	/* host code */
		if ( expert )
			_send(catEmpty, &catalog)( &catalog );
		else
		{
			_send(catLoad, &catalog)( &catalog, (char *)NULL, (char *)NULL );
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 487 "abcatlog.qsc" */	/* host code */
		/*
		** Note:  This activation can occur only when in
		** `expert' mode when the form mode is not READ.
		**
		**	assert( expert && mode == FILL )
		*/
            char name[FE_MAXNAME+1];
            OOID id;
/* # line 496 "abcatlog.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_CatForm,_cattable,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,name,_name);
                IItcogetio((short *)0,1,30,4,&id,_id);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 497 "abcatlog.qsc" */	/* host code */
		if ( *name == EOS )
		{
			_send(catEmpty, &catalog)( &catalog );
		}
		else if ( expert && id <= 0 )
		{
			_send(catCheck, &catalog)( &catalog, (OO_CATREC *)NULL );
		}
		else
		{
/* # line 507 "abcatlog.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 508 "abcatlog.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 514 "abcatlog.qsc" */	/* host code */
		APPL	newappl;
		OOID	id = OC_UNDEFINED;
		OO_CATREC	row;
		if (!savef)
		{
			IIARiaForm(_create);
			savef = TRUE;
		}
		qflag = FALSE;
		newappl.ooid = OC_UNDEFINED;
		newappl.name = NULL;
		newappl.class = OC_APPL;
		newappl.data.tag = tag;
		newappl.data.dbObject = TRUE;
		newappl.data.inDatabase = FALSE;
		newappl.data.levelbits = 1;	/* set level 0 */
		IIUGfmt(tbuf,119,ERget(_CreateTitle),1,IIabExename);
		if ( IIOOcreateFrame(&id, OC_APPL, &catalog, _create,
				tbuf, (PTR)&newappl,
				ap_init, ap_create, (STATUS (*)())NULL,
				(MENUPARAM *)NULL, ERx("abcreate.hlp")
			) == OK )
		{
			if ( _send(catCheck, &catalog)( &catalog, &row ) == OK )
			{
				clear_gconsf(row.id);
				_VOID_ IIABeditApp( row.name, row.id, expert,
							TRUE, &qflag 
				);
			}
		}
		if ( newappl.ooid != OC_UNDEFINED )
			IIOOforget(&newappl);
		FEfree(tag);
		if (qflag)
		{
/* # line 552 "abcatlog.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 553 "abcatlog.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 558 "abcatlog.qsc" */	/* host code */
		OO_CATREC	row;
		APPL		ta;
		bool		x;
		if ( _send(catCheck, &catalog)( &catalog, &row ) == OK )
		{
			/*
			** this is questionable - we dummy up an
			** object for IIABcdlCheckDestroyLock()
			*/
			ta.class = OC_APPL;
			ta.ooid = row.id;
			ta.name = row.name;
			ta.comps = NULL;
			if (IIABcdlCheckDestroyLock(&ta,&ta,FALSE,TRUE,&x) != OK)
			{
/* # line 575 "abcatlog.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 576 "abcatlog.qsc" */	/* host code */
			}
			if ( _send(catDestroy,&catalog)(&catalog,&row ) == OK )
				IIABddirDestroy(row.name);
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 585 "abcatlog.qsc" */	/* host code */
		OO_CATREC	row;
		if ( _send(catCheck, &catalog)( &catalog, &row ) == OK )
		{
			clear_gconsf(row.id);
			_VOID_ IIABeditApp( row.name, row.id, expert, FALSE, &qflag);
			if (qflag)
			{
/* # line 593 "abcatlog.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 594 "abcatlog.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 5) {
          {
/* # line 600 "abcatlog.qsc" */	/* host code */
		OO_CATREC	row;
		IIABrappRename(&catalog, &row);
          }
        } else if (IIretval() == 6) {
          {
/* # line 608 "abcatlog.qsc" */	/* host code */
		static MENUPARAM	_menup[] = {
						{ NULL, -1, IIABdefaults },
						{ NULL, 0, NULL }
					};
		APPL	appl;
		char	title[80];
		if ( !detailf )
		{
			IIARiaForm(_iidetail);
			_menup[0].mpstring = ERget(F_AB0006_Defaults);
			/*_menup[0].mpexpl = ERget(_DefExpl);*/
			detailf = TRUE;
		}
		appl.class = OC_APPL;
		appl.ooid = OC_UNDEFINED;
		appl.data.tag = tag;
		appl.data.dbObject = TRUE;
		appl.data.inDatabase = FALSE;
		appl.data.levelbits = 0;
		_VOID_ IIUGfmt(title, sizeof(title), ERget(_DetailTitle), 
				1, IIabExename);
		_VOID_ _send(catDetail, &catalog)( &catalog, (OO_CATREC *)NULL,
				_iidetail, title, (PTR)&appl,
				(STATUS (*)())NULL, detail, (STATUS (*)())NULL,
				(STATUS (*)())NULL, iiabSvApp,
				_menup, ERx("abdetail.hlp")
		);
		if ( appl.ooid != OC_UNDEFINED )
			IIOOforget(&appl);
		FEfree(tag);
          }
        } else if (IIretval() == 7) {
          {
/* # line 646 "abcatlog.qsc" */	/* host code */
		OO_CATREC	row;
		if ( _send(catCheck, &catalog)( &catalog, &row ) == OK )
		{
			clear_gconsf(row.id);
			IIABrun(row.name, row.id, (char *)NULL);
		}
          }
        } else if (IIretval() == 8) {
          {
/* # line 657 "abcatlog.qsc" */	/* host code */
		OO_CATREC	row;
		APPL		app;
            i4 numrows;
/* # line 661 "abcatlog.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_CatForm,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrows,32,_cattable,0);
              } /* IIiqset */
            }
/* # line 662 "abcatlog.qsc" */	/* host code */
		if (numrows > 0)
		{
			if ( _send(catCheck, &catalog)( &catalog, &row ) == OK )
			{
				app.ooid = row.id;
				app.data.inDatabase = FALSE;
				clear_gconsf(row.id);
				IIAButilities(&app, row.name, ABUT_APPCAT, 
					      expert);
			}
		}
		else
		{
			/* No applications */
			clear_gconsf(row.id);
			IIAButilities((APPL *)NULL, ERx(""), ABUT_APPCAT, 
				      expert);
		}
          }
        } else if (IIretval() == 9) {
          {
/* # line 684 "abcatlog.qsc" */	/* host code */
		_VOID_ IIfind_row(_CatForm, _cattable, _);
          }
        } else if (IIretval() == 10) {
          {
/* # line 689 "abcatlog.qsc" */	/* host code */
		IIscrollto( _, _, 1 );
          }
        } else if (IIretval() == 11) {
          {
/* # line 694 "abcatlog.qsc" */	/* host code */
		IIscrollto( _, _, -1 );
          }
        } else if (IIretval() == 12) {
          {
/* # line 701 "abcatlog.qsc" */	/* host code */
		FEhelp( ERx(IIabVision ? "vqcatlog.hlp" : "abcatlog.hlp"),
			tbuf
		);
          }
        } else if (IIretval() == 13) {
          {
/* # line 710 "abcatlog.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 14) {
          {
/* # line 716 "abcatlog.qsc" */	/* breakdisplay */
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
      goto IIfdE1;
IImuI1:
      if (IITBcaClmAct(_cattable,(char *)"name",0,1) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Create),ERget(_CreateExpl),0,0,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Destroy),ERget(_DestroyExpl),2,2,3) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(FE_Edit),ERget(_EditExpl),2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Rename),ERget(_RenameExpl),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_MoreInfo),ERget(_MoreInfoExpl),2,2,6) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(FE_Go),ERget(_GoExpl),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,ERget(_GoExpl),2,2,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Utilities),ERget(_UtilExpl),2,2,8) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(_FindExpl),0,0,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(_TopExpl),0,0,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(_BottomExpl),0,0,11) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,0,12) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,ERget(_HelpExpl),0,0,12) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_QuitExpl),0,0,13) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,13) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),ERget(_QuitExpl),0,0,14) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,0,14) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 719 "abcatlog.qsc" */	/* host code */
	return;
  }
/*{
** Name:	IIABrappRename() - 	Rename an application
**
** Description:
**	Get the name for the application, create the directory
**	$ING_ABFDIR/<newappname>, then, if this is successful, rename the
**	application in the object catalog and delete $ING_ABFDIR/<oldappname>.
**	If the procedure fails in any way, delete the directory
**	$ING_ABFDIR/<newappname> which we've just created.
**
** Inputs:
**	self	{OO_CATALOG *}	The visual object catalog
**	row	{OO_CATREC *}	Catalog row object for row
**
** Outputs:
**
** Returns:
**
** Side Effects:
**
** History:
**	10-jan-91 (blaise)
**		Initial revision
*/
VOID
IIABrappRename (self, row)
register OO_CATALOG	*self;
OO_CATREC		*row;
{
	char	oldname[FE_MAXNAME+1];
	char	newname[FE_MAXNAME+1];
	/* Get new name for application */
	if (get_new_appname(self, row, newname) == OK)
	{
		/* Create the new directory for the application */
		if ((newname[0] != EOS) &&
			(IIABcdirCreate(newname, &IIabWrkDir) == OK))
		{
			/* Change the name in the object catalog */
			if (rename_object(self, row, newname, oldname) == OK)
			{
				/* delete the old application directory */
				IIABddirDestroy(oldname);
			}
			else
			{
				/* delete the directory we've just created */
				IIABddirDestroy(newname);
			}
		}
	}
}
/*{
** Name:	get_new_appname() -	Get new name for an application
**
** Description:
**	Gets row from catalog tablefield, checks owner authorisation, prompts
**	for new application name, checks validity of new name.
**
** Inputs:
**	self	{OO_CATALOG *}	The visual object catalog
**	row	{OO_CATREC *}	Catalog row object for row
**
** Outputs:
**	newname	{char[FE_MAXNAME+1]}	The new name for the application
**
** Returns:
**	rstatus	{STATUS}	OK if operation succeeds
**				FAIL if operation fails
**
** Side Effects:
**
** History:
**	10-jan-91 (blaise)
**		Initial revision (adapted from IIOOctRename())
**	23-jul-91 (blaise)
**		If, when prompted for a new application name, the user enters a
**		blank string or a new name which is the same as the old one,
**		return FAIL to ensure we don't attempt to rename the
**		application. Bug #37881.
**
*/
static STATUS
get_new_appname (OO_CATALOG *self, OO_CATREC *row, char *newname)
{
	OO_CATREC	rowbuf;
	STATUS		rstatus = OK;
	if ( row == NULL )
		row = &rowbuf;
	if ( _send(catCheck, self)(self, row) == OK )
	{ /* got row from catalog */
		char	ans[FE_MAXNAME+1];
		OOID	class;
		char	*owner = (*row->owner == EOS)
					     ? IIUIdbdata()->user : row->owner;
		/* Check owner authorisation */
		class = row->class;
		if ( IIOOidCheck(&class, &row->id, row->name, owner) != OK )
		{
			IIUGerr(E_AB0150_NotOwner, UG_ERR_ERROR,
					2, OOsnd(class, _name), row->name);
			rstatus = FAIL;
		}
		else
		{
			/* Prompt for new application name */
			IIUFprompt(ERget(S_AB0151_Enter_new_name_prompt), 1,
				(bool)FALSE, ans, sizeof(ans)-1, 0);
			/*
			** If the user enters a blank string or a new name
			** which is the same as the old one, we won't attempt
			** to rename the application.
			*/
			if ((ans[0] == EOS) ||
				(CVlower(ans), STequal(row->name, ans)))
			{
				rstatus = FAIL;
			}
			else
			{
				if ( FEchkname(ans) != OK )
				{
				    IIUGmsg(ERget(E_AB0152_Not_a_legal_name),
					TRUE, 2, ans, OOsnd(class, _name));
				    rstatus = FAIL;
				}
				else
				{
					STcopy(ans, newname);
				}
			}
		}
	}
	else
	{
		rstatus = FAIL;
	}
	return rstatus;
}
/*{
** Name:	rename_object() -	Rename object in current row
**
** Description:
**	Gets row from catalog tablefield, invokes Rename operation on object
**	and updates catalog tablefield display.
**
** Inputs:
**	self	{OO_CATALOG *}		The visual object catalog
**	row	{OO_CATREC *}		Catalog row object for row
**	newname	{char[FE_MAXNAME+1]}	The new name for the application
**
** Output:
**	oldname	{char[FE_MAXNAME+1]}	The old name of the object
**
** Returns:
**	rstatus	{STATUS}	OK if operation succeeds
**				FAIL if operation fails
**
** Side Effects:
**	Object may be fetched from database and allocated.
**	Display may be updated.
** 
** History:
**	10-jan-91 (blaise)
**		Initial revision (adapted from IIOOctRename())
*/
static STATUS
  rename_object(self, row, newname, oldname)
  register OO_CATALOG *self;
OO_CATREC		*row;
char			*newname;
char			*oldname;
  {
	OO_CATREC	rowbuf;
	STATUS		rstatus = OK;
	OOID		class = row->class;
	char		*owner = row->owner;
		IIUGmsg(ERget(S_AB0153_Rename_status), FALSE,
			3, OOsnd(class, _name), row->name, newname);
		if (IIOOidRename(&class, &row->id, row->name, owner,
						STalloc(newname)) == OK)
		{
			STcopy(UGcat_now(), row->alter_date);
			if (oldname != NULL)
			{
				STcopy(row->name, oldname);
			}
			STcopy(newname, row->name);
			if (self->c_noload)
			{
				_send(catEmpty, self)(self);
			}
			else
			{
/* # line 926 "abcatlog.qsc" */	/* deleterow */
    {
      if (IItbsetio(4,self->c_form,self->c_tfield,-2) != 0) {
        if (IItdelrow(0) != 0) {
        } /* IItdelrow */
      } /* IItbsetio */
    }
/* # line 928 "abcatlog.qsc" */	/* host code */
				_send(catInsertRow, self)(self, OOp(row->id),
					FEtbsins( self->c_form, self->c_tfield,
					newname, _name, TRUE));
			}
		}
		else
		{
			rstatus = FAIL;
		}
		return rstatus;
  }
/*{
** Name:	clear_gconsf() -	Clear global constant file specification
**					if app id has changed.
**
** Description:
**	CLear previous global constant file specification if specified object id
**	differs from previous object ID, which is kept in static variable
**	"current_rowid".
**
** Inputs:
**	rowid	{i4}		The new application object ID.
**
** Outputs:
**	NONE
**
** Returns:
**
** Side Effects:
**	Update static current_rowid to new application object ID.
**
** History:
**	01-feb-93 (davel)
**		Initial revision.
*/
static VOID	
clear_gconsf(i4 rowid)
{
    if (rowid != current_rowid)
    {
	current_rowid = rowid;
	IIABscfSetConstantFile((char *)NULL);
    }
    return;
}
