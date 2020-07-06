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
# include <ooclass.h>
# include <oocat.h>
# include <oocatlog.h>
# include <uigdata.h>
/**
** Name:	oocatlog.qsc -	Visual Object Catalog Class Methods.
**
** Description:
**	Defines the visual object catalog class methods:
**
**	IIOOctTFldInit() initialize visual object catalog table field.
**	IIOOctRowLoad()	load row in visual object catalog table field.
**	IIOOctInsertRow() insert row in visual object catalog table field.
**	IIOOctPutRow()	put row in visual object catalog table field.
**	IIOOctObjLoad()
**
** History:
**	Revision 6.2  89/01  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF i4	IIuserIndex;
/*{
** Name:	IIOOctTFldInit() -	Initialize Visual Object Catalog
**							Table Field.
** Description:
**	Initializes the table field for the visual object catalog with
**	respect to the FRS.  Specifically, this defines the hidden columns
**	to be added to the visible ones.
**
** Input:
**	self	{OO_CATALOG *}  The visual object catalog.
**	mode	{char *}  The mode to which to initialize the table field,
**				if NULL then default to UPDATE mode.
**
** History:
**	01/89 (jhw) -- Restructured as object method from 'OBcatInit()'.
*/
VOID
  IIOOctTFldInit( self, mode )
  register OO_CATALOG *self;
char			*mode;
  {
    char *rmode;
	rmode = mode != NULL ? mode : ERx("update");
/* # line 67 "oocatlog.qsc" */	/* inittable */
    {
      if (IItbinit(self->c_form,self->c_tfield,rmode) != 0) {
        IIthidecol(_id,(char *)"i4");
        IIthidecol(_class,(char *)"i4");
        IIthidecol(_env,(char *)"i4");
        IIthidecol(_is_current,(char *)"i4");
        IIthidecol(_create_date,(char *)"c25");
        IIthidecol(_altr_date,(char *)"c25");
        IIthidecol(_altr_count,(char *)"i4");
        IIthidecol(_last_altered_by,(char *)"c32");
        IItfill();
      } /* IItbinit */
    }
  }
/* # line 79 "oocatlog.qsc" */	/* host code */
VOID
  IIOOctRowLoad( self, row )
  register OO_CATALOG *self;
  register OO_CATREC *row;
  {
    char *owner = IIOOmapOwner(row->owner);
/* # line 86 "oocatlog.qsc" */	/* loadtable */
    {
      if (IItbact(self->c_form,self->c_tfield,1) != 0) {
        IItcoputio(_id,(short *)0,1,30,4,&row->id);
        IItcoputio(_class,(short *)0,1,30,4,&row->class);
        IItcoputio(_name,(short *)0,1,32,0,row->name);
        IItcoputio(_env,(short *)0,1,30,4,&row->env);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_is_current,(short *)0,1,30,4,&row->is_current);
        IItcoputio(_short_remark,(short *)0,1,32,0,row->short_remark);
        IItcoputio(_create_date,(short *)0,1,32,0,row->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,row->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,4,&row->alter_cnt);
        IItcoputio(_last_altered_by,(short *)0,1,32,0,row->altered_by);
        IITBceColEnd();
      } /* IItbact */
    }
  }
/* # line 101 "oocatlog.qsc" */	/* host code */
VOID
  IIOOctInsertRow( self, obj, rowno )
  register OO_CATALOG *self;
  OO_OBJECT *obj;
  i4 rowno;
  {
/* # line 107 "oocatlog.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,self->c_form,self->c_tfield,rowno) != 0) {
        if (IItinsert() != 0) {
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
/* # line 108 "oocatlog.qsc" */	/* host code */
	if ( rowno < self->c_maxrow )
		++rowno;
	IIOOctPutRow(self, obj, rowno );
  }
/*
** History:
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
VOID
  IIOOctPutRow( self, obj, rowno )
  register OO_CATALOG *self;
  register OO_OBJECT *obj;
  i4 rowno;
  {
    char *owner = IIOOmapOwner(obj->owner);
    char *modifier = IIOOmapOwner(IIUIdbdata()->user);
/* # line 128 "oocatlog.qsc" */	/* putrow */
    {
      if (IItbsetio(3,self->c_form,self->c_tfield,rowno) != 0) {
        IItcoputio(_id,(short *)0,1,30,4,&obj->ooid);
        IItcoputio(_name,(short *)0,1,32,0,obj->name);
        IItcoputio(_class,(short *)0,1,30,4,&obj->class);
        IItcoputio(_short_remark,(short *)0,1,32,0,obj->short_remark);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_env,(short *)0,1,30,4,&obj->env);
        IItcoputio(_is_current,(short *)0,1,30,4,&obj->is_current);
        IItcoputio(_create_date,(short *)0,1,32,0,obj->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,obj->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IItcoputio(_last_altered_by,(short *)0,1,32,0,modifier);
        IITBceColEnd();
      } /* IItbsetio */
    }
  }
/* # line 144 "oocatlog.qsc" */	/* host code */
/*{
** Name:	IIOOctObjLoad() -	Load Objects Into a
**						Catalog Form Table Field.
** Description:
**	Loads the descriptive attributes (including the name) of objects into a
**	catalog form table field.  This method is the general, high performance
**	case when database objects of a certain class, or range of classes, are
**	to be displayed on the catalog form.  (Its performance is high because
**	it uses a repeat query that is shared by many of the Front-Ends, and so
**	should be used if at all possible in preference to redefining this
**	method.)
**
**	It will retrieve either all objects (if the owner is not specified;
**	this corresponds to a global namespace,) or only those objects that
**	are owned by the owner or the DBA and will resolve any object name
**	ambiguities in favor of displaying the object owned by the owner.
**
** Inputs:
**	self		{OO_CATALOG *}  The catalog form object.
**	subClasses	{bool}  Whether to display sub-classes.
**	pattern		{char *}  Object names, which may be a QUEL pattern.
**	owner		{char *}  The owner of the object to fetch, or all
**					if empty or NULL.
**	lastname	{char *}  The name of the object on the row on which
**					the cursor was previously located.
**	prevcursor	{nat *}  Row corresponding to the previous cursor row.
**
** History:
**	10-22-1987 (danielt)
**		Written (adapted from 'OBcatForm()'.)
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
static READONLY char	_FetchAll[] = ERx("%");
VOID
IIOOctObjLoad ( self, subClasses, pattern, owner, lastname, prevcursor )
OO_CATALOG	*self;
bool		subClasses;
  char *pattern;
  char *owner;
char		*lastname;
i4		*prevcursor;
{
	i4		rows;
	register i4	i,
			j;
  i4 high, low;
  OO_CATREC cs[2], *csp[2];
  UIDBDATA *uidbdata;
	char	pattern_buf[FE_MAXNAME*DB_MAX_COLS+1];
	uidbdata = IIUIdbdata();
	if ( pattern == NULL || *pattern == EOS )
		pattern = _FetchAll;
	else
	{ /* change the pattern matching to SQL */
		i4	sql_wild;
		STcopy(pattern, pattern_buf);
		IIUGwildcard(pattern = pattern_buf, &sql_wild);
	}
	if ( subClasses )
	{
		low = self->c_class->ooid/100 * 100;
		high  = low + 99;
	}
	else
	{
		high = low = self->c_class->ooid;
	}
	if ( owner == NULL || *owner == EOS )
		owner = _FetchAll;
	csp[0] = &cs[0];
	csp[1] = &cs[1];
	rows = 0;
	/*
	** Retrieve catalog rows.
	**
	** Order by both name and owner, the latter so that user and DBA
	** objects will appear in lexical order as indicated by the
	** 'IIuserIndex' (0 user precedes DBA; as setup by 'IIOOinit()').
	** This is then used to distinguish user from DBA objects if the
	** namespace is scoped by owner, otherwise just fetch all objects.
	** (PeterK implemented this algorithm and J. Wong added the fetch
	** all logic.)
	*/
	i = 0;
/* # line 251 "oocatlog.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"oocatlog1",6310,30204);
      IIputdomio((short *)0,1,32,0,pattern);
      IIputdomio((short *)0,1,30,4,&low);
      IIputdomio((short *)0,1,30,4,&high);
      IIputdomio((short *)0,1,32,0,owner);
      IIputdomio((short *)0,1,32,0,uidbdata->dba);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"oocatlog1",6310,30204);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, object_class, object_name, object_owner, object_env\
, is_current, short_remark, create_date, alter_date, '', alter_count, \
last_altered_by from ii_objects where object_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,pattern);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&low);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class<=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&high);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(object_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,uidbdata->dba);
        IIwritio(0,(short *)0,1,32,0,(char *)
")order by object_name, object_owner");
        IIexDefine(0,(char *)"oocatlog1",6310,30204);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&(*csp[i]).id);
      IIgetdomio((short *)0,1,30,4,&(*csp[i]).class);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,(*csp[i]).name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(*csp[i]).owner);
      IIgetdomio((short *)0,1,30,4,&(*csp[i]).env);
      IIgetdomio((short *)0,1,30,4,&(*csp[i]).is_current);
      IIgetdomio((short *)0,1,32,OOSHORTREMSIZE+1-1,(*csp[i]).short_remark);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,(*csp[i]).create_date);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,(*csp[i]).alter_date);
      IIgetdomio((short *)0,1,32,OOLONGREMSIZE+1-1,(*csp[i]).long_remark);
      IIgetdomio((short *)0,1,30,4,&(*csp[i]).alter_cnt);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(*csp[i]).altered_by);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 261 "oocatlog.sc" */	/* host code */
		if ( i == 0 && owner != _FetchAll )
			++i;
		else
		{
			if ( owner == _FetchAll )
				j = 0;
			else if ( STequal(cs[0].name, cs[1].name) )
			{
				i = 0;
				j = IIuserIndex;
			}
			else 
			{
				OO_CATREC	*ctmp;
				/* swap */
				ctmp = csp[0];
				csp[0] = csp[1];
				csp[1] = ctmp;
				i = j = 1;
			}
			_send(catRowLoad, self)( self, csp[j] );
#ifdef	FT3270
			++rows;
#else
			/* Give feedback to user:  display catalog
			** when it has a display full.
			*/
			if ( ++rows == self->c_maxrow )
/* # line 287 "oocatlog.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 288 "oocatlog.qsc" */	/* host code */
#endif
			/* check whether this corresponds to the object/row on
			** which the cursor was previously located ...
			*/
			if ( lastname != NULL && STequal(lastname, csp[j]->name) )
			{
				*prevcursor = rows;
			}
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if ( i > 0 )
	{
		_send(catRowLoad, self)( self, csp[0] );
		++rows;
		if ( lastname != NULL && STequal(lastname, csp[0]->name) )
		{
			*prevcursor = rows;
		}
	}
}
