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
# include <ooclass.h>
# include <oocat.h>
#include	<oocatlog.h>
#include	<oosymbol.h>
#include	<oodefine.h>
#include	"eroo.h"
# include <uigdata.h>
/**
** Name:	oocreate.qc -	Object Module Create Frame Routine.
**
** Description:
**	Contains a routine that is the generic create frame for objects
**	to be used when an object must be created before being editted
**	(e.g., applications.)  (Objects that are editted before being
**	save, such as forms, use 'IIOOsvFrame()' to save.)  Defines:
**
**	IIOOcreateFrame()	generic create object frame.
**
** History:
**	Revision 6.2  89/01  wong
**	Modified to use catalog object.
**
**	Revision 6.1  88/11  wong
**	Added custom help-file parameter.
**
**	Revision 6.0  88/01/27  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
char	*UGcat_now();
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _ForgetExpl	F_FE0109_ExplForget
FUNC_EXTERN VOID IIUFfieldHelp();
/*{
** Name:	IIOOcreateFrame() -	Generic OO Create Object Frame.
**
** Description:
**	Display Catalog Create Frame, handling user interface interactions
**	and calling OO routines to create generic Object DB Catalogs
**	(ii_id, ii_objects, ii_longremarks.)  Invoke passed in
**	application-specific display and save functions as appropriate
**	to complete display and/or save tasks.
**
** Input params:
**	id	{OOID *}	// ptr to object id
**	class	{OOID}		// class for object
**	catalog	{OO_CATALOG *}	// catalog form name
**	saveform {char *}	// catalog save form name
**	title	{char *}	// save frame title string
**	ptr	{PTR}		// PTR to application-specific data structure
**	display	{STATUS (*)()}	// application-specific display function
**	save	{STATUS (*)()}	// application-specific save function
**	setup	{STATUS (*)()}	// application-specific setup function
**	menup	{MENUPARM []}	// dynamic menu specification array
**	hlp_file {char *}	// custom help-file name.
**
** Output params:
**	id	{OOID *}	// filled in with new ID for created object
**
** Returns:
**	{STATUS}	OK	// a create actually happened
**			FAIL	// create didn't happen (not nec. an error)
**
** Side Effects:
**	displays Save sequence sub-menu; may invoke Save frame; object
**	may be saved in database.
**
** History:
**	01/88 (jhw) -- Modified from 'IIOOsvFrame()'.
**	01/89 (jhw) -- Modified to use catalog object.
**	08/89 (jhw) -- Added activate on Name field to check for legal INGRES
**		name.  Also, resume on Name field if save confirmation fails.
**      18-oct-1993 (kchin)
**          	Since the return type of OOsnd() has been changed to PTR,
**          	when being called in IIOOcreateFrame(), its return
**          	type needs to be cast to the proper datatype.
**      06-dec-93 (smc)
**		Bug #58882
**              Commented lint pointer truncation warning.
*/
STATUS
IIOOcreateFrame ( id, class, catalog, saveform, title,
			ptr, display, save, setup, menup, hlp_file )
OOID		*id;
OOID		class;
OO_CATALOG	*catalog;
  char *saveform;
  char *title;
PTR		ptr;
STATUS		(*display)();
STATUS		(*save)();
STATUS		(*setup)();
  MENUPARAM *menup;
char		*hlp_file;
{
	STATUS		dbchange = FAIL;
  UIDBDATA *uidbdata;
  GLOBALREF MENUPARAM lmenup[];
	/* set up dynamic menuitems */
	iiooMenuItem(menup, lmenup);
	*id = OC_UNDEFINED;
	uidbdata = IIUIdbdata();
/* # line 131 "oocreate.qsc" */	/* display */
  {
    if (IIdispfrm(saveform,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 134 "oocreate.qsc" */	/* host code */
		/* set up dynamic FRSKEY mappings */
		iiooMenuFrsKey(menup, 0);
		/* set mode of extra fields to be non-updateable */
/* # line 138 "oocreate.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,saveform,(char *)0, (char *)0) != 0) {
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
/* # line 144 "oocreate.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio(_title,(short *)0,1,32,0,title);
          IIputfldio(_name,(short *)0,1,32,0,_iiOOempty);
          IIputfldio(_owner,(short *)0,1,32,0,uidbdata->user);
          IIputfldio(_create_date,(short *)0,1,32,0,ERx(UGcat_now()));
        } /* IIfsetio */
      }
/* # line 151 "oocreate.qsc" */	/* host code */
		/* Object specific attributes */
		if ( display != NULL && (*display)(*id, ptr) != OK )
/* # line 153 "oocreate.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE1;
      }
/* # line 155 "oocreate.qsc" */	/* resume */
      {
        IIresfld(_name);
        if (1) goto IIfdB1;
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
          i4 change;
/* # line 162 "oocreate.qsc" */	/* inquire_frs */
          {
            if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&change,20,_name,0);
            } /* IIiqset */
          }
/* # line 163 "oocreate.qsc" */	/* host code */
		if ( change != 0 )
          {
            char name[FE_MAXNAME+1];
/* # line 167 "oocreate.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,name,_name);
              } /* IIfsetio */
            }
/* # line 168 "oocreate.qsc" */	/* host code */
			if ( *name == EOS )
			{
				IIUGerr( E_OO002A_No_name_specified_,
						UG_ERR_ERROR, 0
				);
/* # line 173 "oocreate.qsc" */	/* resume */
            {
              IIresfld(_name);
              if (1) goto IIfdB1;
            }
/* # line 174 "oocreate.qsc" */	/* host code */
			}
			else if ( FEchkname(name) != OK )
			{
				IIUGerr( E_UG000E_BadName, UG_ERR_ERROR,
						1, name
				);
/* # line 180 "oocreate.qsc" */	/* resume */
            {
              IIresfld(_name);
              if (1) goto IIfdB1;
            }
/* # line 181 "oocreate.qsc" */	/* host code */
			}
          }
/* # line 183 "oocreate.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 189 "oocreate.qsc" */	/* host code */
		char	*srem;
		char	*lrem;
		OOID	newid;
		STATUS	ret;
          char currname[FE_MAXNAME+1];
          char buf[OOLONGREMSIZE+1];
/* # line 196 "oocreate.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,currname,_name);
            } /* IIfsetio */
          }
/* # line 198 "oocreate.qsc" */	/* host code */
		/* Perform basic names checks.  This should have been validated
		** on the name field (which should be lowercase as well,) but
		** double-check just to be sure.
		*/
		if ( iiooCheckName(currname) != OK )
/* # line 203 "oocreate.qsc" */	/* resume */
          {
            IIresfld(_name);
            if (1) goto IIfdB1;
          }
/* # line 205 "oocreate.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,OOLONGREMSIZE+1-1,buf,_short_remark);
            } /* IIfsetio */
          }
/* # line 206 "oocreate.qsc" */	/* host code */
		srem = STtrmwhite(buf) <= 0 ? _iiOOempty : STalloc(buf);
		OOget_long_remark(saveform, buf);
		lrem = STtrmwhite(buf) <= 0 ? _iiOOempty : STalloc(buf);
		if ( *id <= 0 )
		{ /* allocate new DB object */
    			/* lint truncation warning if size of ptr > OOID, 
			   but code valid */
			*id = (OOID)OOsnd( class, _newDb, FALSE, currname, 0,
					uidbdata->user, 0,
					srem, (char *)NULL, (char *)NULL, lrem
			);
			if ( *id == nil )
			{
				dbchange = FAIL;
/* # line 222 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 223 "oocreate.qsc" */	/* host code */
			}
		}
		if ( setup != NULL && (ret = (*setup)(*id, ptr)) != OK )
		{ /* put error message here */
/* # line 228 "oocreate.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 229 "oocreate.qsc" */	/* host code */
		}
    		/* lint truncation warning if size of ptr > OOID, 
	`	   but code valid */
		if ( (newid = (OOID)OOsnd( *id, _confirmName, currname, FALSE )) 
		    == nil )
/* # line 235 "oocreate.qsc" */	/* resume */
          {
            IIresfld(_name);
            if (1) goto IIfdB1;
          }
/* # line 237 "oocreate.qsc" */	/* host code */
		*id = newid;
		if ( IIOOsave( id, ptr, save ) == OK )
		{
          OO_OBJECT *obj = OOp(*id);
/* # line 242 "oocreate.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio(_altr_date,(short *)0,1,32,0,obj->alter_date);
              IIputfldio(_create_date,(short *)0,1,32,0,obj->create_date);
            } /* IIfsetio */
          }
/* # line 246 "oocreate.qsc" */	/* set_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 248 "oocreate.qsc" */	/* host code */
			/* update row on main catalog form */
			_send(catInsertRow, catalog)( catalog, obj,
				FEtbsins( catalog->c_form, catalog->c_tfield,
					currname, _name, FALSE
				)
			);
			dbchange = OK;
/* # line 256 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 257 "oocreate.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 263 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 269 "oocreate.qsc" */	/* host code */
		if ((*lmenup[0].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 270 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 274 "oocreate.qsc" */	/* host code */
		if ((*lmenup[1].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 275 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 279 "oocreate.qsc" */	/* host code */
		if ((*lmenup[2].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 280 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 284 "oocreate.qsc" */	/* host code */
		if ((*lmenup[3].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 285 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 289 "oocreate.qsc" */	/* host code */
		if ((*lmenup[4].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 290 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 294 "oocreate.qsc" */	/* host code */
		if ((*lmenup[5].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 295 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 10) {
        {
/* # line 299 "oocreate.qsc" */	/* host code */
		if ((*lmenup[6].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 300 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 11) {
        {
/* # line 304 "oocreate.qsc" */	/* host code */
		if ((*lmenup[7].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 305 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 12) {
        {
/* # line 309 "oocreate.qsc" */	/* host code */
		if ((*lmenup[8].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 310 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 13) {
        {
/* # line 314 "oocreate.qsc" */	/* host code */
		if ((*lmenup[9].mpaction)(*id, ptr, &dbchange) != OK)
/* # line 315 "oocreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 14) {
        {
/* # line 322 "oocreate.qsc" */	/* host code */
		IIUFfieldHelp();
        }
      } else if (IIretval() == 15) {
        {
/* # line 328 "oocreate.qsc" */	/* host code */
		FEhelp( ( hlp_file != NULL ) ? hlp_file : ERx("oocreate.hlp"),
			title
		);
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIFRafActFld(_name,0,1) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_OK),(char *)0,1,1,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,1,1,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Forget),ERget(_ForgetExpl),0,0,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,3) == 0) goto IIfdE1;
    if (IInmuact(lmenup[0].mpstring,(char *)0,0,0,4) == 0) goto IIfdE1;
    if (IInmuact(lmenup[1].mpstring,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact(lmenup[2].mpstring,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact(lmenup[3].mpstring,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IInmuact(lmenup[4].mpstring,(char *)0,2,2,8) == 0) goto IIfdE1;
    if (IInmuact(lmenup[5].mpstring,(char *)0,2,2,9) == 0) goto IIfdE1;
    if (IInmuact(lmenup[6].mpstring,(char *)0,2,2,10) == 0) goto IIfdE1;
    if (IInmuact(lmenup[7].mpstring,(char *)0,2,2,11) == 0) goto IIfdE1;
    if (IInmuact(lmenup[8].mpstring,(char *)0,2,2,12) == 0) goto IIfdE1;
    if (IInmuact(lmenup[9].mpstring,(char *)0,2,2,13) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,14) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,14) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,0,15) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,15) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 333 "oocreate.qsc" */	/* host code */
	return dbchange;
}
