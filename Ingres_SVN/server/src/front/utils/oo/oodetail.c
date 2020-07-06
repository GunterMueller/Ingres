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
# include <oocatlog.h>
#include	<oodefine.h>
#include	<oosymbol.h>
#include	<eqrun.h>
#include	"eroo.h"
/**
** Name:	oodetail.qsc -	Generic details frame code
**
** Description:
**	This file defines:
**
**	IIOOctMoreInfo()	visual object catalog detail method.
**	iiooMenuItem()
**	iiooMenuFrsKey()
**
** History:
**	4/8/87 (peterk) -- created catalog details code in QBF
**	20-jul-1988 (danielt)
**		fixed jupbug 2991 (don't update catalog form if 
**		detail has been saved, just return TRUE)
**	22-aug-1988 (danielt) 
**		put back update of catalog form.  Although it is
**		up to the caller to reload the catalog frame
**		if the DB has been updated, we still need to 
**		update the catalog tablefield because of the
**		Next and Previous functions.
**	29-sept-1988 (danielt)
**		don't read in the long remark from the long remarks
**		catalog, and don't write it out if it is empty, if
**		we are dealing with an object that is not in the database.
**		(if it has a negative id)
**	07-nov-88 (sylviap)
**		Added the customized help screen parameter to OOdetail.
**	01/89 (jhw) -- Test for non-existent object against 'row->id'.
**		Previous test for 'is_new' was before 'row->id' may have been
**		set.
**	10-may-1990 (fredv)
**		Added NO_OPTIM hint for ris_us5; the optimizer simply
**		dies on this file.
**      18-oct-1993 (kchin)
**          	Since the return type of OOsnd() has been changed to PTR,
**          	when being called in IIOOctMoreInfo(), its return
**          	type needs to be cast to the proper datatype.
**      06-dec-93 (smc)
**		Bug #58882
**          	Commented lint pointer truncation warning.
**  17-mar-1995(angusm)
**      If we change info on MoreDetails as user other than application
**      owner, this causes displayed application owner on previous
**      screen to be changed, giving the impression that a separate
**      copy of the application has been created for this user.
**      (bug 65898)
**	02-jan-97 (mcgem01)
**		lmenup should be an array not a pointer when GLOBALREF'ed.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	16-aug-2001 (toumi01)
**	    speculative i64_aix NO_OPTIM change for beta xlc_r - FIXME !!!
**/
/*
NO_OPTIM=ris_us5 i64_aix
*/
VOID	iiooMenuItem();
VOID	iiooMenuFrsKey();
bool	IIUFcheckEnd();
/*{
** Name:	IIOOctMoreInfo() -	Invoke generic Catalog Details frame
**
** Description:
**	Display Catalog Details Frame, invoking passed in
**	application-specific functions as appropriate.
**
** Input params:
**	self	{OO_CATALOG *}  The visual object catalog.
**	row	{OO_CATREC *} 	Reference to row structure or NULL.
**	detform	{char *} Catalog detail form name.
**	title	{char *}  Detail frame title string.
**	data	{PTR}  Reference to application data structure.
**	STATUS	(*display)();	// application-specific display function
**	STATUS	(*reinit)();	// application-specific reinitialize function
**	STATUS	(*rename)();	// application-specific rename function
**	STATUS	(*destroy)();	// application-specific destroy function
**	STATUS	(*save)();	// application-specific save function
**	MENUPARM menup[];	// dynamic menu specification array
**
** Returns:
**	{bool}		// TRUE if any change in database
**
** Side Effects:
**	display Catalog Details frame; objects may be modified and saved
**	in database; catalog table field may be updated.
**
*/
#define _Help_Expl	F_FE0100_ExplFrameHelp
#define _End_Expl	F_FE0102_ExplEnd
# define        MAXMENUPARAM    10
  GLOBALREF MENUPARAM lmenup[];
bool
IIOOctMoreInfo ( self, row, detfrm, title, data, display,
				reinit, rename, destroy, save, menup, help_scr )
  register OO_CATALOG *self;
  register OO_CATREC *row;
  char *detfrm;
  char *title;
PTR			data;
STATUS			(*display)(), (*reinit)(), (*rename)(),
			(*destroy)(), (*save)();
MENUPARAM		*menup;
char			*help_scr;
  {
	OO_CATREC	rowrec;
	bool		dbchange = FALSE;
	bool		cont = TRUE;
	/* as a temporary measure for FE consistency, turn off
	** Rename and Destroy menuitems in Details frame.
	*/
	rename = destroy = NULL;
	/* set up dynamic menuitems before start of display loop */
	iiooMenuItem( menup, lmenup );
	if ( row == NULL )
		row = &rowrec;
	while (cont)
    {
      char *_Rename, *_Destroy, *_Save;
      char *_mode;
	    bool	authorized;
	    OO_OBJECT	object;
	    if ( _send(catCheck, self)(self, row) != OK )
		break;
	    cont = FALSE;	/* Next, Prev will set to TRUE */
	    /*
	    ** check authorization to update object.
	    IIOOobMake(&obj, row);
	    */
	    object.ooid = row->id;
	    object.class = row->class;
	    object.name = row->name;
	    object.owner = row->owner;
	    if ( authorized = (bool)( OOsndSelf(&object, _authorized) &&
			save != NULL ) )
	    { /* can update */
		_Save = ERget(FE_Save);
		_mode = ERx("fill");
	    }
	    else
	    { /* no update, read only */
		_Save = _;
		_mode = ERx("read");
	    }
	    _Rename = (authorized && rename != NULL) ? ERget(FE_Rename) : _;
	    _Destroy = (authorized && destroy != NULL) ? ERget(FE_Destroy) : _;
/* # line 187 "oodetail.qsc" */	/* display */
      {
        if (IIdispfrm(detfrm,_mode) == 0) goto IIfdE1;
        goto IImuI1;
IIfdI1:;
        if (IIfsetio((char *)0) == 0) goto IIfdE1;
        {
/* # line 190 "oodetail.qsc" */	/* host code */
		/* set up dynamic FRSKEY mappings */
		iiooMenuFrsKey(menup, 5);
		/*
		** If "Save" is being turned off, de-activate FRSkey8
		** as well (set last arg of IInfrskey to 0).
		**
		** Note:  The form should already be read only, but make
		** the 'short_remark' and 'long_remark' fields DISPLAY ONLY.
		*/
		if (_Save == _)
		{
			IInfrskact(8, (char *)0, 2, 2, 0);
/* # line 203 "oodetail.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,detfrm,(char *)0, (char *)0) != 0) {
              IIstfsio(53,_short_remark,0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IIstfsio(53,_long_remark,0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 207 "oodetail.qsc" */	/* host code */
		}
		else
		{ /* turn them on ... */
/* # line 210 "oodetail.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,detfrm,(char *)0, (char *)0) != 0) {
              IIstfsio(53,_short_remark,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
              IIstfsio(53,_long_remark,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 214 "oodetail.qsc" */	/* host code */
		}
		/* set mode of extra fields to be non-updateable */
/* # line 217 "oodetail.qsc" */	/* set_frs */
          {
            if (IIiqset(2,0,detfrm,(char *)0, (char *)0) != 0) {
              IIstfsio(53,(char *)"xfield1",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IIstfsio(53,(char *)"xfield2",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IIstfsio(53,(char *)"xfield3",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
              IIstfsio(53,(char *)"xfield4",0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 224 "oodetail.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio(_title,(short *)0,1,32,0,title);
              IIputfldio(_name,(short *)0,1,32,0,row->name);
              IIputfldio(_owner,(short *)0,1,32,0,row->owner);
              IIputfldio(_short_remark,(short *)0,1,32,0,row->short_remark);
              IIputfldio(_create_date,(short *)0,1,32,0,row->create_date);
              IIputfldio(_altr_date,(short *)0,1,32,0,row->alter_date);
            } /* IIfsetio */
          }
/* # line 233 "oodetail.qsc" */	/* host code */
		/* retrieve long remarks and display them */
		if ( row->id <= 0 )
/* # line 235 "oodetail.qsc" */	/* clear */
          {
            IIfldclear(_long_remark);
          }
/* # line 236 "oodetail.qsc" */	/* host code */
		else
		{
			OOrd_seqText(row->id, row->long_remark,
					_ii_longremarks, _long_remark
			);
			OOput_long_remark(detfrm, row->long_remark);
		}
		/* Object specific attributes */
		if ( display != NULL && (*display)(row->id, data) != OK )
		{
/* # line 247 "oodetail.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 248 "oodetail.qsc" */	/* host code */
		}
        }
IIfdB1:
        while (IIrunform() != 0) {
          if (IIretval() == 1) {
            {
              i4 record, maxrec;
/* # line 255 "oodetail.qsc" */	/* host code */
		/* check for form changes before going to different object */
		if ( !IIUFcheckEnd() )
/* # line 257 "oodetail.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 259 "oodetail.qsc" */	/* host code */
		/* scroll to next row of catform table field */
/* # line 261 "oodetail.qsc" */	/* getrow */
              {
                if (IItbsetio(2,self->c_form,self->c_tfield,-2) != 0) {
                  IItcogetio((short *)0,1,30,4,&record,(char *)"_RECOR\
D");
                  IITBceColEnd();
                } /* IItbsetio */
              }
/* # line 262 "oodetail.qsc" */	/* inquire_frs */
              {
                if (IIiqset(3,0,self->c_form,(char *)0, (char *)0) != 0) {
                  IIiqfsio((short *)0,1,30,4,&maxrec,32,self->c_tfield,0);
                } /* IIiqset */
              }
/* # line 265 "oodetail.qsc" */	/* host code */
		if (++record > maxrec)
			record = 1;
/* # line 267 "oodetail.qsc" */	/* scroll */
              {
                if (IItbsetio(1,self->c_form,self->c_tfield,-3) != 0) {
                  IItbsmode((char *)"to");
                  if (IItscroll(0,record) != 0) {
                  } /* IItscroll */
                } /* IItbsetio */
              }
/* # line 268 "oodetail.qsc" */	/* host code */
		if ( reinit == NULL  ||  (*reinit)(row->id, data) == OK )
			cont = TRUE;
/* # line 270 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 2) {
            {
              i4 record, maxrec;
/* # line 276 "oodetail.qsc" */	/* host code */
		/* check for form changes before going to different object */
		if ( !IIUFcheckEnd() )
/* # line 278 "oodetail.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 280 "oodetail.qsc" */	/* host code */
		/* scroll to previous row of catform table field */
/* # line 281 "oodetail.qsc" */	/* getrow */
              {
                if (IItbsetio(2,self->c_form,self->c_tfield,-2) != 0) {
                  IItcogetio((short *)0,1,30,4,&record,(char *)"_RECOR\
D");
                  IITBceColEnd();
                } /* IItbsetio */
              }
/* # line 282 "oodetail.qsc" */	/* inquire_frs */
              {
                if (IIiqset(3,0,self->c_form,(char *)0, (char *)0) != 0) {
                  IIiqfsio((short *)0,1,30,4,&maxrec,32,self->c_tfield,0);
                } /* IIiqset */
              }
/* # line 285 "oodetail.qsc" */	/* host code */
		if ( --record < 1 )
			record = maxrec;
/* # line 287 "oodetail.qsc" */	/* scroll */
              {
                if (IItbsetio(1,self->c_form,self->c_tfield,-3) != 0) {
                  IItbsmode((char *)"to");
                  if (IItscroll(0,record) != 0) {
                  } /* IItscroll */
                } /* IItbsetio */
              }
/* # line 288 "oodetail.qsc" */	/* host code */
		if ( reinit == NULL  ||  (*reinit)(row->id, data) == OK )
			cont = TRUE;
/* # line 290 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 3) {
            {
/* # line 294 "oodetail.qsc" */	/* host code */
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
/* # line 304 "oodetail.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 305 "oodetail.qsc" */	/* host code */
		}
# endif
		if ( _send(catRename, self)(self, row, (char *)NULL) == OK  &&
				(*rename)(row->id, data) == OK )
		{
			dbchange = TRUE;
/* # line 312 "oodetail.qsc" */	/* putform */
              {
                if (IIfsetio((char *)0) != 0) {
                  IIputfldio(_name,(short *)0,1,32,0,row->name);
                  IIputfldio(_altr_date,(short *)0,1,32,0,row->alter_date);
                } /* IIfsetio */
              }
/* # line 315 "oodetail.qsc" */	/* host code */
		}
            }
          } else if (IIretval() == 4) {
            {
/* # line 319 "oodetail.qsc" */	/* host code */
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
/* # line 329 "oodetail.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 330 "oodetail.qsc" */	/* host code */
		}
# endif
		if ( _send(catDestroy, self)(self, row) == OK  &&
				(*destroy)(row->id, data) == OK )
		{
			dbchange = TRUE;
/* # line 337 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
/* # line 338 "oodetail.qsc" */	/* host code */
		}
            }
          } else if (IIretval() == 5) {
            {
/* # line 342 "oodetail.qsc" */	/* host code */
		OOID	newid;
              char currname[FE_MAXNAME+1];
              i4 formchange;
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
/* # line 356 "oodetail.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 357 "oodetail.qsc" */	/* host code */
		}
# endif
		/* 
		** check to see if the object is owned by the
		** user.  If not, copy the object and give it
		** a new ID.
		*/
/* # line 366 "oodetail.qsc" */	/* getform */
              {
                if (IIfsetio((char *)0) != 0) {
                  IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,currname,_name);
                } /* IIfsetio */
              }
/* # line 367 "oodetail.qsc" */	/* host code */
		if ( iiooCheckName(currname) != OK )
/* # line 368 "oodetail.qsc" */	/* resume */
              {
                IIresfld(_name);
                if (1) goto IIfdB1;
              }
/* # line 370 "oodetail.qsc" */	/* host code */
		/* Note:  If the name was changed and the save is confirmed,
		** then the object needs to be saved as a new object (or
		** overwrites an existing object.)  Check the ID returned by
		** the confirmName method and get the long remarks, which will
		** need to be saved as well even though they didn't change.
		*/
                /* Lint truncation warning if size of ptr > OOID,
                ** but code valid. 
                */
		if ( (newid = (OOID)OOsnd(row->id, _confirmName, currname, FALSE)) 
		    == nil )
		{
/* # line 382 "oodetail.qsc" */	/* resume */
              {
                IIresfld(_name);
                if (1) goto IIfdB1;
              }
/* # line 383 "oodetail.qsc" */	/* host code */
		}
		/* instantiate an object from the current row
		** information plus any changes in the remarks;
		*/
/* # line 388 "oodetail.qsc" */	/* getform */
              {
                if (IIfsetio((char *)0) != 0) {
                  IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,
                  row->short_remark,_short_remark);
                } /* IIfsetio */
              }
/* # line 390 "oodetail.qsc" */	/* inquire_frs */
              {
                if (IIiqset(2,0,detfrm,(char *)0, (char *)0) != 0) {
                  IIiqfsio((short *)0,1,30,4,&formchange,20,_long_remark,0);
                } /* IIiqset */
              }
/* # line 391 "oodetail.qsc" */	/* host code */
		if ( formchange || newid != row->id )
		{
			OOget_long_remark(detfrm, row->long_remark);
			if ( STtrmwhite(row->long_remark) <= 0 )
			{
				if ( row->id < 0 )
					row->long_remark[0] = EOS;
				else
					STcopy(ERx(" "), row->long_remark);
			}
		}
		else
		{
			row->long_remark[0] = EOS;	/* do not update */
		}
		row->id = newid;
		if ( OOcatObject(self->c_class->class, row) != OK )
		{
			IIUGerr( E_OO0024_Cant_instantiate_obje,
				UG_ERR_ERROR, 0 
			);
/* # line 413 "oodetail.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 414 "oodetail.qsc" */	/* host code */
		}
		if ( IIOOsave(&row->id, data, save, (STATUS (*)())NULL) == OK )
		{
              OO_OBJECT *obj = OOp(row->id);
 			/* Update row on main catalog form:
			** This is necessary for the Next and Previous menu-items,
			** even if the catalog is re-read if the DBMS has been
			** updated.
			*/
			if ( !STequal(currname, row->name) )
			{ /* can't happen yet because name can't change */
				_send(catInsertRow, self)( self, obj,
 						FEtbsins( self->c_form,
							self->c_tfield,
							currname, _name, TRUE
						)
 				);
			}
			else
              {
				_send(catPutRow, self)( self, obj, rowCURR );
                /*
                ** bug 65898 - this has the effect of changeing
                ** the DISPLAYED application's owner in the
                ** previous screen (although not the ACTUAL
                ** application's pwner) if a user other than the owner
                ** has changed fields in this screen. Reset
                ** field directly.
                */
/* # line 446 "oodetail.qsc" */	/* putrow */
                {
                  if (IItbsetio(3,self->c_form,self->c_tfield,-2) != 0) {
                    IItcoputio(_owner,(short *)0,1,32,0,row->owner);
                    IITBceColEnd();
                  } /* IItbsetio */
                }
              }
/* # line 452 "oodetail.qsc" */	/* host code */
			dbchange = TRUE;
			/* set "form changed" to FALSE */
/* # line 454 "oodetail.qsc" */	/* set_frs */
              {
                if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                  IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                } /* IIiqset */
              }
/* # line 456 "oodetail.qsc" */	/* host code */
		}
            }
          } else if (IIretval() == 6) {
            {
/* # line 460 "oodetail.qsc" */	/* host code */
		if ((*lmenup[0].mpaction)(row->id, data) != OK)
/* # line 461 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 7) {
            {
/* # line 465 "oodetail.qsc" */	/* host code */
		if ((*lmenup[1].mpaction)(row->id, data) != OK)
/* # line 466 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 8) {
            {
/* # line 470 "oodetail.qsc" */	/* host code */
		if ((*lmenup[2].mpaction)(row->id, data) != OK)
/* # line 471 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 9) {
            {
/* # line 475 "oodetail.qsc" */	/* host code */
		if ((*lmenup[3].mpaction)(row->id, data) != OK)
/* # line 476 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 10) {
            {
/* # line 480 "oodetail.qsc" */	/* host code */
		if ((*lmenup[4].mpaction)(row->id, data) != OK)
/* # line 481 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 11) {
            {
/* # line 485 "oodetail.qsc" */	/* host code */
		if ((*lmenup[5].mpaction)(row->id, data) != OK)
/* # line 486 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 12) {
            {
/* # line 490 "oodetail.qsc" */	/* host code */
		if ((*lmenup[6].mpaction)(row->id, data) != OK)
/* # line 491 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 13) {
            {
/* # line 495 "oodetail.qsc" */	/* host code */
		if ((*lmenup[7].mpaction)(row->id, data) != OK)
/* # line 496 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 14) {
            {
/* # line 500 "oodetail.qsc" */	/* host code */
		if ((*lmenup[8].mpaction)(row->id, data) != OK)
/* # line 501 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 15) {
            {
/* # line 505 "oodetail.qsc" */	/* host code */
		if ((*lmenup[9].mpaction)(row->id, data) != OK)
/* # line 506 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
            }
          } else if (IIretval() == 16) {
            {
/* # line 512 "oodetail.qsc" */	/* host code */
		FEhelp( ( help_scr == NULL || *help_scr == EOS )
				? ERx("oodetail.hlp") : help_scr, title
		);
            }
          } else if (IIretval() == 17) {
            {
/* # line 520 "oodetail.qsc" */	/* host code */
		/* check for form changes before ending */
		if ( IIUFcheckEnd() )
/* # line 522 "oodetail.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE1;
              }
/* # line 523 "oodetail.qsc" */	/* host code */
		else
/* # line 524 "oodetail.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
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
        if (IInmuact(ERget(FE_Next),(char *)0,2,2,1) == 0) goto IIfdE1;
        if (IInmuact(ERget(FE_Previous),(char *)0,2,2,2) == 0) goto IIfdE1;
        if (IInmuact(_Rename,(char *)0,2,2,3) == 0) goto IIfdE1;
        if (IInmuact(_Destroy,(char *)0,2,2,4) == 0) goto IIfdE1;
        if (IInmuact(_Save,(char *)0,2,2,5) == 0) goto IIfdE1;
        IIFRInternal(0);
        if (IInfrskact(8,(char *)0,2,2,5) == 0) goto IIfdE1;
        if (IInmuact(lmenup[0].mpstring,(char *)0,2,2,6) == 0) goto IIfdE1;
        if (IInmuact(lmenup[1].mpstring,(char *)0,2,2,7) == 0) goto IIfdE1;
        if (IInmuact(lmenup[2].mpstring,(char *)0,2,2,8) == 0) goto IIfdE1;
        if (IInmuact(lmenup[3].mpstring,(char *)0,2,2,9) == 0) goto IIfdE1;
        if (IInmuact(lmenup[4].mpstring,(char *)0,2,2,10) == 0) goto IIfdE1;
        if (IInmuact(lmenup[5].mpstring,(char *)0,2,2,11) == 0) goto IIfdE1;
        if (IInmuact(lmenup[6].mpstring,(char *)0,2,2,12) == 0) goto IIfdE1;
        if (IInmuact(lmenup[7].mpstring,(char *)0,2,2,13) == 0) goto IIfdE1;
        if (IInmuact(lmenup[8].mpstring,(char *)0,2,2,14) == 0) goto IIfdE1;
        if (IInmuact(lmenup[9].mpstring,(char *)0,2,2,15) == 0) goto IIfdE1;
        if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),0,2,16) == 0) goto IIfdE1;
        IIFRInternal(0);
        if (IInfrskact(1,(char *)0,0,2,16) == 0) goto IIfdE1;
        if (IInmuact(ERget(FE_End),ERget(_End_Expl),0,2,17) == 0) goto IIfdE1;
        IIFRInternal(0);
        if (IInfrskact(3,(char *)0,0,2,17) == 0) goto IIfdE1;
        if (IIendmu() == 0) goto IIfdE1;
        goto IIfdI1;
IIfdE1:;
        IIendfrm();
      }
    }
/* # line 530 "oodetail.qsc" */	/* host code */
	return dbchange;
  }
/*{
** Name:	iiooMenuItem() -	set up dynamic menuitems
**
** Description:
**	Sets up dynamic menuitems, based
**	on a source array of menuitem specifications, by loading
**	a destination data structure used lexically in the calling
**	routines to declare DISPLAY loop menuitems.   This routine
**	should be called BEFORE the start of the DISPLAY loop.
**
**	The scrmenu array should be terminated by a MENUPARAM
**	structure whose members are all NULL or 0;
**
**	A companion routine, 'iiooMenuFrsKey()', may be called AFTER
**	the start of the DISPLAY loop (e.g. within the INITIALIZE
**	block) in order to associate FRSkey mappings with the
**	dynamic menuitems.
**
** Inputs:
**	MENUPARAM	srcmenu[];	// array of input menuitem
**					   specs.
**			   .mpstring	// menuitme label (or NULL).
**			   .mpfrskey	// FRSkey mapping.
**			   .mpaction	// menuitem action routine.
**
**	MENUPARAM	destmenu[];	// array of menuitem specs
**					   used lexically in DISPLAY.
** Outputs:
**	MENUPARAM	destmenu;	// loaded from srcmenu.
**
** Returns:
**	VOID
**
** History:
**	5/12/87 (peterk) - created for 'OOdetail()' and 'OOsave()';
**	7/6/87 (peterk) - broke 'OOmenuparam()' into 2 routines,
**		'OOmenuitem()' (this one), and 'OOmenufrskey()' (see below)
**		since they need to be called in slightly different
**		places w.r.t. the DISPLAY loop.
**	sept-26-1988 (danielt)
**		removed commits (performance changes)
*/
VOID
iiooMenuItem ( srcmenu, destmenu )
MENUPARAM	srcmenu[];	/* input menu array spec */
MENUPARAM	destmenu[];	/* declared menu array in frame */
{
	register i4	i = 0;
	/* set up dynamic menuitems and FRSKEY mappings */
	if (srcmenu != NULL)
	    for (i = 0; i < MAXMENUPARAM; i++)
	    {
		if (srcmenu[i].mpstring == NULL)
		    break;
		destmenu[i].mpstring = srcmenu[i].mpstring;
		destmenu[i].mpaction = srcmenu[i].mpaction;
	    }
	for (; i < MAXMENUPARAM; i++)
	{
	    destmenu[i].mpstring = _;
	}
}
/*{
** Name:	iiooMenuFrsKey() -	set up dynamic FRSkey mappings
**
** Description:
**	Sets up dynamic FRSkey mappings, based
**	on a source array of menuitem specifications, by loading
**	a destination data structure used lexically in the calling
**	routines to declare DISPLAY loop menuitems.   This routine
**	should be called at the start of the INITIALIZE block of
**	the DISPLAY loop.  It is used in association with the
**	companion routine 'iiooMenuItem()' (see above.)
**
**	The scrmenu array should be terminated by a MENUPARAM
**	structure whose members are all NULL or 0;
**
** Inputs:
**	MENUPARAM	srcmenu[];	// array of input menuitem
**					   specs.
**			   .mpstring	// menuitme label (or NULL).
**			   .mpfrskey	// FRSkey mapping.
**			   .mpaction	// menuitem action routine.
**
**	i4		after;		// number of fixed menuitems
**					   in caller's DISPLAY loop
**					   preceeding start of
**					   dynamic items.
**
** Outputs:
**	(none)
**
** Returns:
**	VOID
**
** History:
**	5/12/87 (peterk) - created for 'OOdetail()' and 'OOsave()';
**	7/6/87 (peterk) - broke OOmenuparam into 2 routines,
**		'OOmenuitem()' (see above), and 'OOmenufrskey()' (this one)
**		since they need to be called in slightly different
**		places w.r.t. the DISPLAY loop.
*/
VOID
iiooMenuFrsKey ( srcmenu, after )
MENUPARAM	srcmenu[];	/* input menu array spec */
i4		after;		/* offset position of dynamic menuitems */
{
	register i4	i = 0;
	/* set up dynamic FRSKEY mappings */
	if (srcmenu != NULL)
	    for (i = 0; i < MAXMENUPARAM; i++)
	    {
		if (srcmenu[i].mpstring == NULL)
		    break;
		if (srcmenu[i].mpfrskey > 0)
		    IInfrskact(srcmenu[i].mpfrskey, (char *)0, 2, 2, i+1+after);
	    }
}
