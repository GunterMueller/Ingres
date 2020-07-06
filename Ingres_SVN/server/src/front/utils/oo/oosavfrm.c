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
#include	<ug.h>
#include	<erui.h>
# include <ooclass.h>
# include <oocat.h>
#include	<oosymbol.h>
#include	<oodefine.h>
#include	"eroo.h"
/**
** Name:	oosavfrm.qc -	Object Save Frame Module.
**
** Description:
**	Contains routines that are the generic save frame and save sub-menu
**	for objects to be used when an object is editted before being saved
**	(e.g., forms.)  (Objects that are created before being editted, such
**	as applications, use 'OOcreateFrame()' to create them.)  Defines:
**
**	OOsaveSequence()	save (sub)menu sequence.
**	IIOOsvFrame()		save frame.
**
** History:
**	Revision 6.0  87/04  peterk
**	Initial revision.
**	2-mar-1988 (danielt) changed to give error message from the save
**		frame if there was an error with 'IIOOsave()'.
**      18-oct-1993 (kchin)
**          	Since the return type of OOsnd() has been changed to PTR,
**          	when being called in IIOOsvFrame(), its return
**          	type needs to be cast to the proper datatype.
**      06-dec-93 (smc)
**		Bug #58882
**          	Commented lint pointer truncation warning.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
#define _Help_Expl	F_FE0100_ExplFrameHelp
static STATUS save_it();
/*{
** Name:	OOsaveSequence() - generic OO Catalog Save submenu sequence
**
** Description:
**	Provide generic Catalog Save submenu sequence, invoking 'IIOOsave()'
**	generic save routine to directly save the object, or 'IIOOsvFrame()'
**	to bring up generic Save Frame to modify object attributes,
**	as appropriate according to user selection.
**
** Input params:
**	OOID	*id;		// ptr to object id
**	char	*savename;	// new name for object
**	char	*catform;	// catalog form name
**	char	*cattabl;	// catalog form tableField name
**	char	*detform;	// catalog detail form name
**	char	*title;		// detail frame title string
**	PTR	ptr;		// PTR to application-specific data structure
**	i4	(*display)();	// application-specific display function
**	i4	(*save)();	// application-specific save function
**	MENUPARM menup[];	// dynamic menu specification array
**	STATUS  (*setup)(); 	// application-specific setup routine
**	char	*help_scr;	// customized help screen
**	bool	bypass;		// bypass the submenu
**
** Output params:
**	OOID	*id;		// filled in with saved id for object
**
** Returns:
**	STATUS		OK	// a save actually happened
**			FAIL	// save didn't happen (not nec. an error)
**
** Side Effects:
**	displays Save sequence sub-menu; may invoke Save frame; object
**	may be saved in database.
**
** History:
**	4/87 (peterk) - created.
**	07-nov-88 (sylviap) 
**		Added another parameter, a customized help screen, to 
**		OOsaveSequence and 'IIOOsvFrame()'.
**	28-feb-1990 (Mike S)
**		Added yet another parameter, to skip the submenu
**      05-Feb-96 (fanra01)
**              Changed extern to GLOBALREF.
*/
STATUS
OOsaveSequence(id, savename, catform, cattabl, saveform, title, ptr,
			   display, save, menup, setup, help_scr, bypass)
OOID	*id;
char	*savename;
char	*catform;
char	*cattabl;
char	*saveform;
char	*title;
PTR	ptr;
i4	(*display)();
i4	(*save)();
MENUPARAM	*menup;
STATUS	(*setup)();
char	*help_scr;
bool	bypass;
{
	OO_OBJECT	*obj = OOp(*id);
	OOID		class = obj->class;
	if ( !obj->data.inDatabase )
	{
		/* It's a new object.  Do the EditInfo frame */
		return IIOOsvFrame( id, savename, catform, cattabl,
					saveform, title, ptr,
					display, save, menup, setup, help_scr
		);
	}
	else if (bypass)
	{
		/* Skip the submenu and save it */
		 return save_it(id, ptr, save, setup);
	}
	/* Go through the submenu */
	for (;;)
	{
/* # line 140 "oosavfrm.qsc" */	/* submenu */
  {
    goto IImuI1;
IIfdB1:
    IIrunmu(0);
    if (IIretval() == 1) {
      {
/* # line 143 "oosavfrm.qsc" */	/* host code */
		return IIOOsvFrame( id, savename, catform, cattabl,
					saveform, title, ptr,
					display, save, menup, setup, help_scr
		);
      }
    } else if (IIretval() == 2) {
      {
/* # line 150 "oosavfrm.qsc" */	/* host code */
		 return save_it(id, ptr, save, setup);
      }
    } else if (IIretval() == 3) {
      {
/* # line 155 "oosavfrm.qsc" */	/* host code */
		return FAIL;
      }
    } else if (IIretval() == 4) {
      {
/* # line 159 "oosavfrm.qsc" */	/* host code */
		FEhelp(ERx("oosavseq.hlp"), title);
      }
    } else {
      if (1) goto IIfdE1;
    } /* IIretval */
    goto IIfdE1;
IImuI1:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_EditInfo),(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Save),(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(8,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Forget),(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdB1;
IIfdE1:;
  }
/* # line 161 "oosavfrm.qsc" */	/* host code */
	}
}
/*
**	save_it
**
**	Save the object 
*/
static STATUS
save_it(id, ptr, save, setup)
OOID	*id;
PTR	ptr;
i4	(*save)();
STATUS	(*setup)();
{
	STATUS	ret;
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
		return (OK);
	}
	else 
	{
# endif
	    	if ( setup != NULL && (ret = (*setup)(*id, ptr)) != OK )
	    	{ /* put error message here */
			return ret;
	    	}
	    	return IIOOsave( id, ptr, save );
# ifdef DGC_AOS
	}
# endif
}
/*{
** Name:	IIOOsvFrame() - display generic OO Catalog Save frame
**
** Description:
**	Display Catalog Save Frame, handling user interface interactions
**	and calling OO routines to update generic Object DB Catalogs
**	(ii_id, ii_objects, ii_longremarks.)  Invoke passed in
**	application-specific display and save functions as appropriate
**	to complete display and/or save tasks.
**
** Input params:
**	OOID	*id;		// ptr to object id
**	char	*savename;	// new name for object
**	char	*catform;	// catalog form name
**	char	*cattabl;	// catalog form tableField name
**	char	*saveform;	// catalog save form name
**	char	*title;		// save frame title string
**	PTR	ptr;		// PTR to application-specific data structure
**	i4	(*display)();	// application-specific display function
**	i4	(*save)();	// application-specific save function
**	MENUPARM menup[];	// dynamic menu specification array
**	STATUS  (*setup)(); 	// application-specific setup routine
**
** Output params:
**	OOID	*id;		// filled in with saved id for object
**
** Returns:
**	{STATUS}	OK	// a save actually happened
**			FAIL	// save didn't happen (not nec. an error)
**
** Side Effects:
**	displays Save sequence sub-menu; may invoke Save frame; object
**	may be saved in database.
**
** History:
**	4/87 (peterk) - created.
**	2-mar-1988 (danielt) changed to give error message from the save
**		frame if there was an error with 'IIOOsave()'.
*/
bool	IIUFcheckEnd();
char	*iiooStrAlloc();
STATUS
IIOOsvFrame ( id, savename, catform, cattabl, saveform, title, ptr,
			display, save, menup, setup, help_scr )
OOID	*id;
  char *savename;
  char *catform;
  char *cattabl;
  char *saveform;
  char *title;
PTR	ptr;
i4	(*display)();
i4	(*save)();
  MENUPARAM *menup;
STATUS	(*setup)();
char	*help_scr;
{
  register OO_OBJECT *obj;
	STATUS			dbchange = FAIL;
	bool			is_new = ( *id < 0 );
  char buf[OOLONGREMSIZE+1];
  GLOBALREF MENUPARAM lmenup[];
	if ((obj = OOp(*id)) == NULL)
	{
		IIUGerr( E_OO0028_Object_not_found, UG_ERR_ERROR, 1, id );
		return FAIL;
	}
	{
	    /* Vifred/RBF need an explicit FTclear() to clear screen
	    ** before displaying Save Frame since they are not in
	    ** normal forms mode.
	    */
	    FTclear();
	    /* set up dynamic menuitems */
	    iiooMenuItem(menup, lmenup);
/* # line 287 "oosavfrm.qsc" */	/* display */
  {
    if (IIdispfrm(saveform,(char *)"u") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
      char *srem, *cre, *alt;
/* # line 292 "oosavfrm.qsc" */	/* host code */
		/* set up dynamic FRSKEY mappings */
		iiooMenuFrsKey(menup, 0);
		/* set mode of extra fields to be non-updateable */
/* # line 296 "oosavfrm.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,saveform,(char *)0, (char *)0) != 0) {
          IIstfsio(21,(char *)"xfield1",0,(short *)0,1,32,0,(char *)"q");
          IIstfsio(21,(char *)"xfield2",0,(short *)0,1,32,0,(char *)"q");
          IIstfsio(21,(char *)"xfield3",0,(short *)0,1,32,0,(char *)"q");
          IIstfsio(21,(char *)"xfield4",0,(short *)0,1,32,0,(char *)"q");
        } /* IIiqset */
      }
/* # line 302 "oosavfrm.qsc" */	/* host code */
		srem = obj->short_remark != NULL ? obj->short_remark: _;
		cre = obj->create_date != NULL ? obj->create_date: _;
		alt = obj->alter_date != NULL ? obj->alter_date: _;
/* # line 305 "oosavfrm.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio(_title,(short *)0,1,32,0,title);
          IIputfldio(_name,(short *)0,1,32,0,savename);
          IIputfldio(_owner,(short *)0,1,32,0,obj->owner);
          IIputfldio(_short_remark,(short *)0,1,32,0,srem);
          IIputfldio(_create_date,(short *)0,1,32,0,cre);
          IIputfldio(_altr_date,(short *)0,1,32,0,alt);
        } /* IIfsetio */
      }
/* # line 314 "oosavfrm.qsc" */	/* host code */
		/* retrieve long remarks and display them */
		if (!is_new)
		{
		    OOrd_seqText(*id, buf, _ii_longremarks, _long_remark);
		    obj->long_remark = iiooStrAlloc(obj, buf);
		    OOput_long_remark(saveform, obj->long_remark);
		}
		else
		{
		    OOput_long_remark(saveform, _);
		}
		/* Object specific attributes */
		if ( display != NULL && (*display)(*id, ptr) != OK)
/* # line 328 "oosavfrm.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE2;
      }
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 333 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[0].mpaction)(*id, ptr) != OK)
/* # line 334 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 338 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[1].mpaction)(*id, ptr) != OK)
/* # line 339 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 343 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[2].mpaction)(*id, ptr) != OK)
/* # line 344 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 348 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[3].mpaction)(*id, ptr) != OK)
/* # line 349 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 353 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[4].mpaction)(*id, ptr) != OK)
/* # line 354 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 358 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[5].mpaction)(*id, ptr) != OK)
/* # line 359 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 363 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[6].mpaction)(*id, ptr) != OK)
/* # line 364 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 368 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[7].mpaction)(*id, ptr) != OK)
/* # line 369 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 373 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[8].mpaction)(*id, ptr) != OK)
/* # line 374 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 10) {
        {
/* # line 378 "oosavfrm.qsc" */	/* host code */
		if ((*lmenup[9].mpaction)(*id, ptr) != OK)
/* # line 379 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 11) {
        {
/* # line 383 "oosavfrm.qsc" */	/* host code */
		OOID	newid;
		STATUS	ret;
          i4 longchange;
          char currname[FE_MAXNAME+1];
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
/* # line 398 "oosavfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 399 "oosavfrm.qsc" */	/* host code */
		}
# endif
/* # line 402 "oosavfrm.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,currname,_name);
            } /* IIfsetio */
          }
/* # line 404 "oosavfrm.qsc" */	/* host code */
		/* Perform basic names checks.  This should have been validated
		** on the name field (which should be lowercase as well,) but
		** double-check just to be sure.
		*/
		if ( iiooCheckName(currname) != OK )
/* # line 409 "oosavfrm.qsc" */	/* resume */
          {
            IIresfld(_name);
            if (1) goto IIfdB2;
          }
/* # line 411 "oosavfrm.qsc" */	/* host code */
		/* Note:  If the name was changed and the save is confirmed,
		** then the object needs to be saved as a new object (or
		** overwrites an existing object.)  Check the ID returned by
		** the confirmName method and get the long remarks, which will
		** need to be saved as well even though they didn't change.
		*/
    		/* Lint truncation warning if size of ptr > OOID, 
		** but code valid. 
		*/
		if ( (newid = (OOID)OOsnd(*id, _confirmName, currname, FALSE)) 
		    == nil )
		{
/* # line 423 "oosavfrm.qsc" */	/* resume */
          {
            IIresfld(_name);
            if (1) goto IIfdB2;
          }
/* # line 424 "oosavfrm.qsc" */	/* host code */
		}
		obj = OOp(newid);
/* # line 428 "oosavfrm.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,OOLONGREMSIZE+1-1,buf,_short_remark);
            } /* IIfsetio */
          }
/* # line 429 "oosavfrm.qsc" */	/* host code */
		obj->short_remark =
			STtrmwhite(buf) <= 0 ? _ : iiooStrAlloc(obj, buf);
/* # line 432 "oosavfrm.qsc" */	/* inquire_frs */
          {
            if (IIiqset(2,0,saveform,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&longchange,20,_long_remark,0);
            } /* IIiqset */
          }
/* # line 434 "oosavfrm.qsc" */	/* host code */
		if ( longchange == 1 || obj->ooid != *id )
		{
			OOget_long_remark(saveform, buf);
			/* Note:  If Long Remark is not empty force deletion
			** from DB by setting it to " ".  JupBug #9163.
			*/
			obj->long_remark = ( STtrmwhite(buf) <= 0 )
						? " " : iiooStrAlloc(obj, buf);
		}
		else
		{
			obj->long_remark = (char *) NULL;
		}
		*id = newid;
		if ( setup != NULL && (ret = (*setup)(*id, ptr)) != OK )
		{ /* put error message here */
			dbchange = ret;
/* # line 453 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
/* # line 454 "oosavfrm.qsc" */	/* host code */
		}
		if ( IIOOsave( id, ptr, save ) == OK )
		{
			obj = OOp(*id);
/* # line 459 "oosavfrm.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio(_altr_date,(short *)0,1,32,0,obj->alter_date);
              IIputfldio(_create_date,(short *)0,1,32,0,obj->create_date);
            } /* IIfsetio */
          }
/* # line 463 "oosavfrm.qsc" */	/* host code */
			/* set "form changed" to FALSE */
/* # line 464 "oosavfrm.qsc" */	/* set_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 466 "oosavfrm.qsc" */	/* host code */
			/* update row on main catalog form */
			OBinsertrow(obj, catform, cattabl,
					FEtbsins( catform, cattabl, currname,
							_name, TRUE
					)
			);
			dbchange = OK;
/* # line 473 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
/* # line 474 "oosavfrm.qsc" */	/* host code */
		}
		else
		{
			IIUGerr(E_OO003F_err_writing_db, 0, 0);
		}
        }
      } else if (IIretval() == 12) {
        {
/* # line 483 "oosavfrm.qsc" */	/* host code */
		/* check for form changes before ending */
		if ( IIUFcheckEnd() )
/* # line 485 "oosavfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 13) {
        {
/* # line 491 "oosavfrm.qsc" */	/* host code */
		FEhelp( help_scr == NULL ? ERx("oosave.hlp") : help_scr, title);
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
    if (IInmuact(lmenup[0].mpstring,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact(lmenup[1].mpstring,(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact(lmenup[2].mpstring,(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact(lmenup[3].mpstring,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IInmuact(lmenup[4].mpstring,(char *)0,2,2,5) == 0) goto IIfdE2;
    if (IInmuact(lmenup[5].mpstring,(char *)0,2,2,6) == 0) goto IIfdE2;
    if (IInmuact(lmenup[6].mpstring,(char *)0,2,2,7) == 0) goto IIfdE2;
    if (IInmuact(lmenup[7].mpstring,(char *)0,2,2,8) == 0) goto IIfdE2;
    if (IInmuact(lmenup[8].mpstring,(char *)0,2,2,9) == 0) goto IIfdE2;
    if (IInmuact(lmenup[9].mpstring,(char *)0,2,2,10) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Save),(char *)0,2,2,11) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(8,(char *)0,2,2,11) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Forget),(char *)0,0,2,12) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,2,12) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),0,2,13) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,13) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 494 "oosavfrm.qsc" */	/* host code */
	}
	return dbchange;
}
