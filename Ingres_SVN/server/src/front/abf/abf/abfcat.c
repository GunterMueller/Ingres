# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
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
#include	<adf.h>
#include	<afe.h>
#include	<fedml.h>
# include <abclass.h>
#include	<abfdbcat.h>
# include <oocat.h>
#include	<oocatlog.h>
#include	<oodefine.h>
#include	<feconfig.h>
#include	<abfcnsts.h>
#ifdef IMTDY
#include	<fdesc.h>
#include	<abfrts.h>
#endif
#include	<ilerror.h>
# include "abclinfo.h"
# include "abcatrow.h"
#include	"abfgolnk.h"
#include	"abfglobs.h"
#include	"gncode.h"
#include	<dmchecks.h>
#include	"erab.h"
/*
fool ming:
#include	<abclass.h>
#include	<oocat.h>
#include	"abcatrow.h"
*/
/**
** Name:	abfcat.qsc -		ABF Catalog selection utility.
**
** Description:
**	Contains the global-variable definition frame routine for ABF, which 
**	allows users to define or modify globals.	 Defines:
**
**	IIABcaCatalogEdit()	- List components, allow edit operations.
**
** History:
**	6/89 billc - Initial revision.
**	12/19/89 (dkh) - VMS shared lib changes - References to IIarStatus
**			 is now through IIar_Status.
**	11-march-91 (blaise)
**	    Integrated desktop porting changes.
**
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
**
**      6-Feb-92 (blaise)
**	Added include abfglobs.h.
**      26-oct-1992 (donc) Changed call to _send( catDetail ) and the save_comp
**              routine to fill in a RECDEF struct for the IIAMwrRecord call.
**	11-apr-1996 (tsale01)
**		In routine save_comp, the call to IIAMwrRecord should not
**		deref obj as IIAMwrRecord expects 'RECDEF *' as parameter.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
APPL_COMP	*iiabGetComp();
COMP_CLASS_INFO	*iiabApCompClass();
AB_CATALOG	*iiabGtCatClass();
static	STATUS	save_comp();
/*{
** Name:	IIABcaCatalogEdit() -	ABF Catalog Edit utility.
**
** Input:
**      app		{APPL *}  The application.
**      class		{OOID}  The class of objects being edited.
**      noload		{bool}  Expert mode; don't load the catalogs.
**      catalogs	{bool}  Not currently used.
**
** Side Effects:
**	Will change the ABF catalogs
**
** History:
**	Written 6/7/89 (billc)
**	Revision 6.5
**	15-jan-92 (davel)
**		Removed Language field activations.
*/
GLOBALREF STATUS	*IIar_Status;
GLOBALREF char		*IIabExename;
#define _RenameExpl	F_AB0018_RenameObj_expl
#define _MoreInfoExpl	F_FE0107_ExplMoreInfo
#define	_FindExpl	F_FE0104_ExplFind
#define	_TopExpl	F_FE0105_ExplTop
#define	_BottomExpl	F_FE0106_ExplBottom
#define _HelpExpl	F_FE0100_ExplFrameHelp
STATUS
IIABcaCatalogEdit ( app, class, noload, catalogs )
  APPL *app;
OOID	class;
bool	noload;
bool	catalogs;
  {
	STATUS		stat = OK;
    AB_CATALOG *self;
    char *_moreinfo;
    char *_rename;
    char *form_mode;
	EX_CONTEXT	context;
	EX	abexchand();
	self = iiabGtCatClass( class, noload );
	if ( EXdeclare(abexchand, &context) != OK )
	{
		EXdelete();
		if ( *IIar_Status == OK )
			*IIar_Status = FAIL;
		return FAIL;
	}
	self->c_appid = app->ooid;
	self->c_ooid = OC_UNDEFINED;
	_rename = ( self->class->catRename != NULL ) ? ERget(FE_Rename) : NULL;
	_moreinfo = (self->class->catDetail != NULL) ? ERget(FE_MoreInfo) : NULL;
	form_mode = "READ";
/* # line 150 "abfcat.qsc" */	/* display */
    {
      if (IIdispfrm(self->c_form,form_mode) == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 153 "abfcat.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_name,(short *)0,1,32,0,app->name);
          } /* IIfsetio */
        }
/* # line 155 "abfcat.qsc" */	/* host code */
		if ( self->c_init.m_func != NULL )
			_VOID_ (*self->c_init.m_func)( self, (APPL_COMP*)NULL);
		_send(catLoad, self)(self, (char *)NULL, (char*)NULL);
/* # line 160 "abfcat.qsc" */	/* resume */
        {
          IIresfld(self->c_tfield);
          if (1) goto IIfdB1;
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 165 "abfcat.qsc" */	/* host code */
		/* if there's a catalog type with a create method, call it.  */
		if ( self->c_create.m_func != NULL )
		{
			_VOID_ (*self->c_create.m_func) ( app, class, self);
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 174 "abfcat.qsc" */	/* host code */
		AB_CATREC	row;
		APPL_COMP 	*comp;
		bool		disallow;
		if ( _send(catCheck, self)( self, &row ) == OK )
		{ /* get it first */
			self->c_ooid = row.id;
			comp = (*self->c_getcomp) ( app, row.id );
			if (comp == NULL)
			{
/* # line 187 "abfcat.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,self->c_form,self->c_tfield,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 188 "abfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 189 "abfcat.qsc" */	/* host code */
			}
			if (comp->class == OC_RECORD)
			{
				/* See if the record is in use */
				disallow = FALSE;
				IIAMxdsDepSearch(comp, IIAM_zdcDelCheck, 
						(PTR)&disallow);
				if (disallow)
				{
					/* It's in use */
/* # line 200 "abfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 201 "abfcat.qsc" */	/* host code */
				}
			}
			if ( _send(catDestroy, self) ( self, &row ) == OK )
			{
				COMP_CLASS_INFO *ctmp;
				OO_OBJECT       *obj = OOp(row.id);
				/* if there's a component type with a destroy
				** method, call it. 
				*/
				ctmp = iiabApCompClass(row.class, FALSE);
				if (ctmp != NULL && ctmp->destroy != NULL)
					(*(ctmp->destroy)) ( obj );
				/* if there's a catalog type with a destroy
				** method, call it. 
				*/
				if (self->c_destroy.m_func != NULL)
				{
					_VOID_ (*self->c_destroy.m_func) 
							( self, obj );
				}
				IIOOforget(obj);
			}
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 232 "abfcat.qsc" */	/* host code */
		AB_CATREC	row;
		if ( _send(catCheck, self)( self, &row ) == OK )
            {
			APPL_COMP	*comp;
			self->c_ooid = row.id;
			comp = (*self->c_getcomp) ( app, row.id );
			if ( comp == NULL )
/* # line 241 "abfcat.qsc" */	/* deleterow */
              {
                if (IItbsetio(4,self->c_form,self->c_tfield,-2) != 0) {
                  if (IItdelrow(0) != 0) {
                  } /* IItdelrow */
                } /* IItbsetio */
              }
/* # line 242 "abfcat.qsc" */	/* host code */
			else if (self->c_edit.m_func != NULL)
				_VOID_ (*self->c_edit.m_func) ( self, comp );
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 249 "abfcat.qsc" */	/* host code */
		AB_CATREC	row;
		char            oldname[FE_MAXNAME+1];
		APPL_COMP	*comp;
		if ( _send(catCheck, self)( self, &row ) != OK )
/* # line 254 "abfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 256 "abfcat.qsc" */	/* host code */
		/* Get the component. */
		self->c_ooid = row.id;
		comp = (*self->c_getcomp) ( app, row.id );
		if (comp == NULL)
		{
/* # line 261 "abfcat.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,self->c_form,self->c_tfield,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 262 "abfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 263 "abfcat.qsc" */	/* host code */
		}
		if (comp->class == OC_RECORD)
		{
			bool disallow = FALSE;
			/* See if the record is in use */
			IIAMxdsDepSearch(comp, IIAM_zdcDelCheck, (PTR)&disallow);
			if (disallow)
			{
				/* It's in use */
/* # line 274 "abfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 275 "abfcat.qsc" */	/* host code */
			}
		}
		if ( _send(catRename, self)( self, &row, oldname, app->ooid) 
		    == OK)
		{
			/* Fix up the dependency structures */
			IIAMxrcRenameComp(app, comp, oldname);
		}
          }
        } else if (IIretval() == 5) {
          {
/* # line 288 "abfcat.qsc" */	/* host code */
		AB_CATREC	row;
		/* 
		** At the moment, only records have this menuitem.
		** Other global types are edited on their edit screen,
		** but Editing a record displays the attributes catalog.
		*/
		if ( _send(catCheck, self)( self, &row ) == OK )
		{
			COMP_CLASS_INFO *ctmp = iiabApCompClass(row.class, FALSE);
			char		title[63+1];
			IIUGfmt( title, sizeof(title)-1, ERget(ctmp->detail),
					1, IIabExename
			);
			IIARiaForm( _iidetail);
                        /* DonC (bug 43050): pass the app data (typedef APPL) to
		        ** save_comp. save_comp then passes the information on
			** to IIAMwrRecord through a RECDEF structure as is
			** proper.
		        */
			_VOID_ _send(catDetail, self)( self, (OO_CATREC *)NULL,
					_iidetail, title, app,
					(STATUS (*)())NULL, (STATUS (*)())NULL,
					(STATUS (*)())NULL, (STATUS (*)())NULL,
					save_comp, (MENUPARAM *)NULL,
					ERx("oodetail.hlp")
			);
		}
          }
        } else if (IIretval() == 6) {
          {
/* # line 322 "abfcat.qsc" */	/* host code */
		_VOID_ IIfind_row(self->c_form, self->c_tfield, _);
          }
        } else if (IIretval() == 7) {
          {
/* # line 327 "abfcat.qsc" */	/* host code */
		IIscrollto( _, _, 1 );
          }
        } else if (IIretval() == 8) {
          {
/* # line 332 "abfcat.qsc" */	/* host code */
		IIscrollto( _, _, -1 );
          }
        } else if (IIretval() == 9) {
          {
/* # line 339 "abfcat.qsc" */	/* host code */
		FEhelp( self->c_helpfile, self->c_init.m_menu );
          }
        } else if (IIretval() == 10) {
          {
/* # line 345 "abfcat.qsc" */	/* host code */
		if (self->c_end.m_func != NULL)
		{
			_VOID_ (*self->c_end.m_func) (self, (APPL_COMP*)NULL );
		}
		if ( self->c_end.m_menu == NULL )
/* # line 350 "abfcat.qsc" */	/* message */
            {
              IImessage(ERget(S_AB0034_Exiting));
            }
/* # line 352 "abfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 358 "abfcat.qsc" */	/* host code */
		if (self->c_quit.m_func != NULL)
		{
			_VOID_ (*self->c_quit.m_func) (self, (APPL_COMP*)NULL);
		}
		if ( self->c_quit.m_menu != NULL )
/* # line 363 "abfcat.qsc" */	/* message */
            {
              IImessage(ERget(S_AB0034_Exiting));
            }
/* # line 365 "abfcat.qsc" */	/* breakdisplay */
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
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(self->c_create.m_menu,self->c_create.m_expl,2,2,1) == 0) 
      goto IIfdE1;
      if (IInmuact(self->c_destroy.m_menu,self->c_destroy.m_expl,2,2,2) == 0) 
      goto IIfdE1;
      if (IInmuact(self->c_edit.m_menu,self->c_edit.m_expl,2,2,3) == 0) goto 
      IIfdE1;
      if (IInmuact(_rename,ERget(_RenameExpl),2,2,4) == 0) goto IIfdE1;
      if (IInmuact(_moreinfo,ERget(_MoreInfoExpl),2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(_FindExpl),2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(_TopExpl),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(_BottomExpl),2,2,8) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,9) == 0) goto IIfdE1;
      if (IInmuact(self->c_end.m_menu,self->c_end.m_expl,2,2,10) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,10) == 0) goto IIfdE1;
      if (IInmuact(self->c_quit.m_menu,self->c_quit.m_expl,2,2,11) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,11) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 368 "abfcat.qsc" */	/* host code */
	self->c_ooid = OC_UNDEFINED;
	EXdelete();
	return stat;
  }
/*{
** Name:	iiabGtCatClass() -	Return Application Catalog Edit
**						Object Class.
** Description:
**	Return a reference to the edit class descriptor of an application
**	catalog object.  
**
** Inputs:
**	class	{OOID}  The ID of the class of the object.
**
** Returns:
**	{AB_CATALOG *}  The reference to the edit class descriptor.
**
** History:
**	08/89 (billc) -- Written.
*/
GLOBALREF AB_CATALOG	iiabAcatalog;
GLOBALREF AB_CATALOG	iiabCcatalog;
GLOBALREF AB_CATALOG	iiabGcatalog;
GLOBALREF AB_CATALOG	iiabMcatalog;
GLOBALREF AB_CATALOG	iiabRcatalog;
static AB_CATALOG	*_cat_list[] = {
	&iiabAcatalog,
	&iiabCcatalog,
	&iiabGcatalog,
	&iiabRcatalog,
	&iiabMcatalog,
	NULL
};
static VOID	_initmenu();
AB_CATALOG *
iiabGtCatClass ( class , noload )
OOID	class;
bool 	noload;
  {
	register i4	i;
	for ( i = 0 ; _cat_list[i] != NULL ; ++i )
	{
    register AB_CATALOG *cp;
		cp = _cat_list[i];
		if ( cp->c_class->ooid != class )
			continue;
		if ( !cp->c_finited )
		{
    char bufr[63+1];
			IIARiaForm( cp->c_form );
			IIUGfmt( bufr, sizeof(bufr)-1, 
				ERget(cp->c_init.m_m_menu), 1,IIabExename
			);
			cp->c_init.m_menu = STalloc(bufr);
			/* initialize menuitems */
			_initmenu(&cp->c_init);
			_initmenu(&cp->c_create);
			_initmenu(&cp->c_destroy);
			_initmenu(&cp->c_edit);
			_initmenu(&cp->c_end);
			_initmenu(&cp->c_quit);
/* # line 442 "abfcat.qsc" */	/* putform */
    {
      if (IIfsetio(cp->c_form) != 0) {
        IIputfldio((char *)"title",(short *)0,1,32,0,cp->c_init.m_menu);
      } /* IIfsetio */
    }
/* # line 444 "abfcat.qsc" */	/* host code */
			_send(catInit, cp)( cp, cp->c_class, cp->c_form, 
				cp->c_tfield, noload
			);
			cp->c_finited = TRUE;
		}
		break;
	}
	return _cat_list[i];
  }
static VOID
_initmenu ( mp )
register AB_CMENU	*mp;
{
	if ( mp->m_m_menu != 0 && mp->m_menu == NULL)
		mp->m_menu = ERget(mp->m_m_menu);
	if ( mp->m_m_expl != 0 && mp->m_expl == NULL)
		mp->m_expl = ERget(mp->m_m_expl);
}
/* 
** Save a component to the database.  This is called if the short or long
** remark is edited in the MoreInfo Screen.
*/
static STATUS
save_comp(ooid, data)
/*ARGSUSED*/
OOID	ooid;
PTR	data;
{
	STATUS	status = FAIL;
	/* DonC: Bug 43050, IIAMwrRecord requires the invoker to pass a
	** RECDEF structure with appl.ooid entry defined.  To do otherwise
	** causes a seg vio
	*/
	register RECDEF *obj = (RECDEF*) OOp(ooid);
	obj->appl = (APPL*) data; 
	if (obj != NULL)
	{
		switch(obj->class)
		{
		    /* Currently, only Record Types have a MoreInfo menuitem */
		    case OC_RECORD:
			status = IIAMwrRecord(obj);
			break;
		}
	}
	return status;
}
