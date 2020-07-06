# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<oodefine.h>
# include <oosymbol.h>
# include <oocat.h>
# include	"eroo.h"
#include        <iisqlda.h>
/**
** Name:	obdestro.sc -	Object Destroy Method.
**
** Description:
**	Contains the routine that is the method to destroy an object.  Defines:
**
**	OBdestroy()
**
** History:
**	29-apr-92 (pearl)
**		Add port specific change for nc5_us5.
**		Change var _Delete to _ncr_Delete to avoid name space clash.
**	04-may-92 (pearl)
**	 	Above change rejected.  Cancel nc5_us5 specific definition
**		of _Delete; change it to iiDelete instead.
**	15-Nov-1993 (tad)
**		Bug #56449
**		Replace %x with %p for pointer values where necessary.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
OOID	iioCOfirst();
OOID	iioCOnext();
VOID		IIOBpromote();
OO_OBJECT	*IIOBdemote();
static char	iiDelete[]	= ERx("Delete");
static bool	anyRestricted();
static bool	anyCascades();
bool
OBdestroy (self, norefcheck)
  register OO_OBJECT *self;
i4		norefcheck;	/* if TRUE don't check referential integrity */
{
  register OO_CLASS *class;
  char delete_buf[200];
	/* check all References for DELETE RESTRICTED */
	if ( !norefcheck && !anyRestricted(self) )
	    goto undo;
	/* check for NULLIFIES and CASCADES */
	if ( !norefcheck && !anyCascades(self) )
	    goto undo;
	/* delete the object from database (if it has attributes) */
	class = OOpclass(self->class);
	if (!OOsnd(class->attributes, _isEmpty))
	{
	    register OO_ATTRIB		*att;
	    register OO_COLLECTION	*col;
	    _VOID_ STprintf(delete_buf,"DELETE FROM %s WHERE %s = ?",
				class->table_name, class->surrogate
		);
	    iiuicw1_CatWriteOn();
/* # line 80 "obdestro.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"oodo_s1",(char *)0,0,delete_buf);
  }
/* # line 81 "obdestro.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"oodo_s1",1);
    IIputdomio((short *)0,1,30,4,&self->ooid);
    IIsyncup((char *)0,0);
  }
/* # line 83 "obdestro.sc" */	/* host code */
	    _VOID_ iiooInqResult(iiDelete, class->table_name);
/* # line 85 "obdestro.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id=");
    IIputdomio((short *)0,1,30,4,&self->ooid);
    IIsyncup((char *)0,0);
  }
/* # line 86 "obdestro.sc" */	/* host code */
	    _VOID_ iiooInqResult(iiDelete, _ii_longremarks);
	    iiuicw0_CatWriteOff();
	}
	if ( IIOBdemote(self) != NULL )
	{
	    bool	ret;
	    ret = (bool)OOsndSelf(self, _destroy, norefcheck);
	    IIOBpromote(self, class);
	    if ( !ret )
		goto undo;
	}
	return TRUE;
undo:
	return FALSE;
}
static bool
anyRestricted(self)
register OO_OBJECT	*self;
{
	OO_CLASS		*class = OOpclass(self->class);
	register OO_COLLECTION	*col;
	register OOID		refid;
	register OO_REFERENCE	*ref;
#ifdef DDEBUG
	D(ERx("anyRestricted(x%p)"), self);
#endif
	col = (OO_COLLECTION *)OOp(class->masterRefs);
	for ( refid = iioCOfirst(col) ; ! iioCOxatEnd(col) ;
			refid = iioCOnext(col) )
	{
		ref = (OO_REFERENCE*)OOp(refid);
		if ( ref->mdelete == RESTRICTED )
		{ /* check if REFERENCE set is empty */
			OOID	second;
			second = OOcheckRef(self, refid, ref->moffset);
			if ( !OOsnd(second, _isEmpty) )
			{
	/*	Destroy of %s %d (%s) failed: DELETE RESTRICTED on %s	*/
				IIUGerr(E_OO0022_Delete_restricted, 0, 3,
				    OOsnd(self->class, _name),
				    self->ooid, self->name,
				    OOsnd(ref, _name));
				return FALSE;
			}
		}
	} /* end for */
	col = (OO_COLLECTION *)OOp(class->detailRefs);
	for ( refid = iioCOfirst(col) ; ! iioCOxatEnd(col) ;
			refid = iioCOnext(col) )
	{
		ref = (OO_REFERENCE*) OOp(refid);
		if ( ref->ddelete == RESTRICTED )
		{ /* check if REFERENCE set is empty */
			OOID	second;
			second = OOcheckRef(self, refid, ref->doffset);
			if ( !OOsnd(second, _isEmpty) )
			{
	/*	Destroy of %s %d (%s) failed: DELETE RESTRICTED on %s	*/
				IIUGerr(E_OO0022_Delete_restricted, 0, 3,
				OOsnd(self->class, _name),
				    self->ooid, self->name,
				    ref->connector);
				return FALSE;
			}
		}
	}
	return TRUE;
}
static bool
anyCascades(self)
register OO_OBJECT	*self;
{
	register OOID		refid;
	register OO_REFERENCE	*ref;
	register OO_CLASS	*class = OOpclass(self->class);
	register OO_COLLECTION	*col;
	register OOID		obj;
#ifdef DDEBUG
	D(ERx("anyCascades(x%p)"), self);
#endif
	col = (OO_COLLECTION *) OOp(class->masterRefs);
	for ( refid = iioCOfirst(col) ; ! iioCOxatEnd(col) ;
			refid = iioCOnext(col) )
	{
		register i4	mdel;
		ref = (OO_REFERENCE*) OOp(refid);
		mdel = ref->mdelete;
		if (mdel != NULLIFIES && mdel != CASCADES)
		    continue;
		obj = OOcheckRef(self, refid, ref->moffset);
		if ( !OOsnd(obj, _isEmpty) && ref->detail )
		{
		    if (mdel == NULLIFIES)
			OOsnd(obj, _do, _setRefNull, ref);
		    else
		    {	/* CASCADES */
			if (!OOsnd(obj, _destroy))
			    return FALSE;
		    }
		}
	} /* end for */
	col = (OO_COLLECTION *) OOp(class->detailRefs);
	for ( refid = iioCOfirst(col) ; ! iioCOxatEnd(col) ;
			refid = iioCOnext(col) )
	{
		ref = (OO_REFERENCE*) OOp(refid);
		if ( ref->ddelete == CASCADES )
		{
		    obj = OOcheckRef(self, refid, ref->doffset);
		    if ( !OOsnd(obj, _isEmpty) && ref->master )
		    {
			if (!OOsnd(obj, _destroy))
			    return FALSE;
		    }
		}
	}
	return TRUE;
}
/*{
** Name:	IIOBdemote() -	Demote an Object to its Super.
**
**		i.e. "peel" current class layer off
**		returning same object as object of super
*/
OO_OBJECT *
IIOBdemote ( obj )
register OO_OBJECT	*obj;
{
	if ( obj != NULL && obj->class )
	{
		OO_CLASS	*class = OOpclass(obj->class);
		if (class->super != nil)
			obj->class = class->super;
		else
			return NULL;
	}
	return obj;
}
/*{
** Name:	IIOBpromote() -	Promote an Object back to a Class.
*/
VOID
IIOBpromote ( obj, class )
OO_OBJECT	*obj;
OO_CLASS	*class;
{
	if ( obj != NULL )
		obj->class = class->ooid;
}
