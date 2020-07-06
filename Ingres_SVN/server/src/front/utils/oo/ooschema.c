# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<me.h>
# include	<cm.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<st.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include <ooclass.h>
# include	<oodefine.h>
# include	"ooldef.h"
# include <oosymbol.h>
# include	"eroo.h"
/**
** Name:	ooschema.qc - routines dealing with dynamic class
**				schema re-definition.
**
** Description:
**	The routines in this file are not normally loaded at runtime
**	in conventional ObjectUtility-based applications.  They
**	used by ObjectUtility tools and special programs such as
**	'classout'.
**
**	This file defines:
**
** History:
**	13-jul-87 (bab) Changed memory allocation to use [FM]Ereqmem.
**	12/19/89 (dkh) - VMS shared lib changes - Added include of ooldef.h
**      22-oct-90 (sandyd)
**              Fixed #include of local ooldef.h to use "" instead of <>.
**	8-feb-93 (blaise)
**		Changed _flush, etc. to ii_flush because of conflict with
**		Microsoft C library.
**	27-jan-93 (blaise)
**		The previous change changed all tabs in this file to spaces;
**		changed back to tabs again.
**      18-oct-1993 (kchin)
**          	Since the return type of OOsnd() and OOsndSuper() have been 
**		changed to PTR, when being called, their return types need to 
**		be cast to the proper datatypes.
**	25-Aug-1993 (fredv)
**		Included <st.h>.
**	15-Nov-1993 (tad)
**		Bug #56449
**		Replace %x with %p for pointer values where necessary.
**      06-dec-93 (smc)
**		Bug #58882
**          	Commented lint pointer truncation warning.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FUNC_EXTERN char	*STalloc();
static STATUS		fetchMasters(), fetchDetails(), fetchAttribs();
static OO_REFERENCE	*allocRefsCol();
  static char _attribute[]= ERx("attribute");
  static char _attid[]= ERx("attid");
  static char _attrelid[]= ERx("attrelid");
  static char _iiakindof[]= ERx("iiakindof");
  static char _xid[]= ERx("id");
  static char _xclass[]= ERx("class");
  static char _xname[]= ERx("name");
  static char *O1table, *O2table, *O3table, *O4table;
  Cdestroy(self)
  OO_CLASS *self;
  {
	if (OOsndSuper(self, _destroy) == FALSE)
		return(FALSE);
/* # line 84 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIsyncup((char *)0,0);
    }
/* # line 85 "ooschema.qsc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_class);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIsyncup((char *)0,0);
    }
/* # line 87 "ooschema.qsc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,self->table_name);
      IIsyncup((char *)0,0);
    }
/* # line 89 "ooschema.qsc" */	/* host code */
	return(TRUE);
  }
  OOID
  Cfetch(p)
  OO_CLASS *p;
  {
    i4 n;
    i4 cnt = 0;
    char methname[51];
    char procname[51];
	register i4	i;
    OO_METHOD *s;
#ifdef DDEBUG
	D(ERx("Cfetch(x%p)"), p);
#endif /* DDEBUG */
	O1table = O1->table_name;
	O2table = O2->table_name;
	O3table = O3->table_name;
	O4table = O4->table_name;
	n = fetchAt(p, OC_CLASS);
	p = (OO_CLASS*)OOp(n);
	p->subClasses = UNKNOWN;	/* init as UNKNOWN reference */
	p->instances = UNKNOWN;		/* init as UNKNOWN reference */
	/* retrieve methods info */
/* # line 119 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIsyncup((char *)0,0);
    }
/* # line 120 "ooschema.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(cnt=count(");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id where ");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)".mclass=");
      IIputdomio((short *)0,1,30,4,&n);
      IIwritio(0,(short *)0,1,32,0,(char *)"))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&cnt);
        if (IIerrtest() != 0) goto IIrtB1;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE1:;
    }
/* # line 121 "ooschema.qsc" */	/* host code */
	if ( cnt != 0 )
	{
		STATUS	stat;
                /* Lint truncation warning if size of ptr > OOID,
                ** but code valid. 
                */
		p->methods = (OOID)OOsnd(OC_COLLECTION, _new, UNKNOWN, cnt);
		if ((s = (OO_METHOD *)MEreqmem((u_i4)0,
		    (u_i4)(cnt * sizeof(OO_METHOD)), FALSE, &stat)) == NULL)
		{
		    IIUGerr(E_OO0030_MEreqmem_err_Cfetch, UG_ERR_ERROR, 1, &stat);
		    return nil;
		}
		i = 0;
/* # line 137 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIsyncup((char *)0,0);
    }
/* # line 138 "ooschema.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(ooid=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id, methname=");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_name);
      IIwritio(0,(short *)0,1,32,0,(char *)", procname=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)".procname, argcount=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)".argcount, defaultperm=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)".defaultperm, keepstatus=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)".keepstatus, fetchlevel=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)".fetchlevel, classmeth=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)".classmeth)where ");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)".mclass=");
      IIputdomio((short *)0,1,30,4,&n);
      IIwritio(0,(short *)0,1,32,0,(char *)"and ");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id=");
      IIwritio(1,(short *)0,1,32,0,O4table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&s[i].ooid);
        IIgetdomio((short *)0,1,32,50,methname);
        IIgetdomio((short *)0,1,32,50,procname);
        IIgetdomio((short *)0,1,30,4,&s[i].argcount);
        IIgetdomio((short *)0,1,30,4,&s[i].defaultperm);
        IIgetdomio((short *)0,1,30,4,&s[i].keepstatus);
        IIgetdomio((short *)0,1,30,4,&s[i].fetchlevel);
        IIgetdomio((short *)0,1,30,4,&s[i].classmeth);
        if (IIerrtest() != 0) goto IIrtB2;
        {
/* # line 149 "ooschema.qsc" */	/* host code */
			if (i >= cnt) {
				tooMany(O4->name, p->name);
/* # line 151 "ooschema.qsc" */	/* endretrieve */
          {
            IIbreak();
            if (1) goto IIrtE2;
          }
/* # line 152 "ooschema.qsc" */	/* host code */
			}
			OOhash(s[i].ooid, &s[i]);
			s[i].data.levelbits = 03;	/* levels 0,1 */
			s[i].class = OC_METHOD;
			if ((s[i].name = STalloc(methname)) == NULL) {
			    IIUGerr( E_OO0031_STalloc_err_Cfetch, UG_ERR_ERROR,
					0
				);
			    return nil;
			}
			s[i].mclass = n;
			s[i].entrypt = 0;
			if ((s[i].procname = STalloc(procname)) == NULL) {
			    IIUGerr(E_OO0032_STalloc_err_Cfetch, UG_ERR_ERROR,
					0
				);
			    return nil;
			}
			OOsnd(p->methods, _atPut, i, s[i].ooid);
			i++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 173 "ooschema.qsc" */	/* host code */
		if ( i > cnt || iiooInqResult(ERx("RETRIEVE"), O4table) != OK )
			return nil;
		if (i < cnt) {
			tooFew(O4->name, p->name);
			return nil;
		}
	}
	else
	    p->methods = nil;
	/* retrieve references info */
	if (fetchMasters(p) != OK || fetchDetails(p) != OK)
		return nil;
	/* retrieve attribute info */
	if (fetchAttribs(p) != OK)
		return nil;
	BTsetF(1, p->data.levelbits);	/* level 1 */
	return(p->ooid);
  }
static STATUS
  fetchAttribs(p)
  OO_CLASS *p;
  {
    OO_ATTRIB *a;
    i4 cnt;
    char name[13];
	register i4	i = 0;
	/* search attribute table for fields */
/* # line 206 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIsyncup((char *)0,0);
    }
/* # line 207 "ooschema.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(cnt=count(");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_attid);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_attrelid);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,32,0,p->table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"and ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".attname!=");
      IIputdomio((short *)0,1,32,0,p->surrogate);
      IIwritio(0,(short *)0,1,32,0,(char *)"))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&cnt);
        if (IIerrtest() != 0) goto IIrtB3;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE3:;
    }
/* # line 209 "ooschema.qsc" */	/* host code */
	if (cnt) {
	    STATUS stat;
	    /* Lint truncation warning if size of ptr > OOID,
	    ** but code valid. 
	    */
	    p->attributes = (OOID)OOsnd(OC_COLLECTION, _new, UNKNOWN, cnt);
	    if ((a = (OO_ATTRIB *)MEreqmem((u_i4)0,
		(u_i4)(cnt * sizeof(OO_ATTRIB)), FALSE,
		&stat)) == NULL)
	    {
		IIUGerr( E_OO0033_MEreqmem_err_Cfetch, UG_ERR_ERROR, 1, &stat );
		return FAIL;
	    }
	    i = 0;
/* # line 225 "ooschema.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(attnum=");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_attid);
      IIwritio(0,(short *)0,1,32,0,(char *)", name=trim(");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".attname), frmt=");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".attfrmt, frml=");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".attfrml)where ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_attrelid);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,32,0,p->table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"and ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".attname!=");
      IIputdomio((short *)0,1,32,0,p->surrogate);
      IIwritio(0,(short *)0,1,32,0,(char *)"and ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)
".attowner=usercode sort by attnum");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&a[i].attnum);
        IIgetdomio((short *)0,1,32,12,name);
        IIgetdomio((short *)0,1,30,4,&a[i].frmt);
        IIgetdomio((short *)0,1,30,4,&a[i].frml);
        if (IIerrtest() != 0) goto IIrtB4;
        {
/* # line 234 "ooschema.qsc" */	/* host code */
		if((a[i].name = STalloc(name)) == NULL) {
		    IIUGerr( E_OO0034_STalloc_err_Cfetch, UG_ERR_ERROR, 0 );
		    return FAIL;
		}
		OOsnd(p->attributes, _atPut, i, &a[i]);
		i++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE4:;
    }
/* # line 241 "ooschema.qsc" */	/* host code */
	    if ( iiooInqResult(ERx("RETRIEVE"), ERx("attribute")) != OK )
		return FAIL;
	}
	else
	    p->attributes = nil;
	return OK;
  }
   static OO_REFERENCE*
  allocRefsCol(class, which, pp, components, cntp)
  OOID class;
  char *which;
register OOID	*pp;
i4	components;	/* if TRUE allocate memory for collection
			** components as well.
			*/
i4	*cntp;
  {
    i4 cnt;
	OO_REFERENCE	*r;
	STATUS		stat;
/* # line 264 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIsyncup((char *)0,0);
    }
/* # line 265 "ooschema.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(cnt=count(");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id where ");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,which);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,30,4,&class);
      IIwritio(0,(short *)0,1,32,0,(char *)"))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&cnt);
        if (IIerrtest() != 0) goto IIrtB5;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE5:;
    }
/* # line 267 "ooschema.qsc" */	/* host code */
	/* Lint truncation warning if size of ptr > OOID,
	** but code valid. 
	*/
	*pp = cnt? (OOID)OOsnd(OC_COLLECTION, _new, UNKNOWN, cnt): nil;
	if (components && cnt)
	{
	    if ((r = (OO_REFERENCE *)MEreqmem((u_i4)0,
		(u_i4)(cnt * sizeof(OO_REFERENCE)), FALSE, &stat)) == NULL)
	    {
		IIUGerr( E_OO0035_MEreqmem_err_Cfetch, UG_ERR_ERROR, &stat );
	    }
	}
	*cntp = cnt;
	return r;
  }
static STATUS
fetchMasters(class)
OO_CLASS	*class;
{
  i4 classn = class->ooid;
	register OOID	col;
  register OO_REFERENCE *r;
  i4 cnt;
  char name[51], conn[13];
	register i4	i;
	char		*_masterRefs = ERx("masterRefs");
	/* retrieve master references info */
	r = allocRefsCol(classn, ERx("master"), &class->masterRefs, TRUE, &cnt);
	col = class->masterRefs;
	if (col != nil ) {
		i = 0;
/* # line 301 "ooschema.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
    IIwritio(1,(short *)0,1,32,0,O1table);
    IIwritio(0,(short *)0,1,32,0,(char *)"=");
    IIwritio(1,(short *)0,1,32,0,O1table);
    IIsyncup((char *)0,0);
  }
/* # line 302 "ooschema.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(ooid=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)"._id, name=");
    IIwritio(1,(short *)0,1,32,0,O1table);
    IIwritio(0,(short *)0,1,32,0,(char *)".");
    IIwritio(1,(short *)0,1,32,0,_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", master=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)".master, mdelete=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)".mdelete, moffset=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)".moffset, conn=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)".connector, detail=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)".detail, ddelete=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)".ddelete, doffset=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)".doffset)where ");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)".master=");
    IIputdomio((short *)0,1,30,4,&classn);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ");
    IIwritio(1,(short *)0,1,32,0,O1table);
    IIwritio(0,(short *)0,1,32,0,(char *)"._id=");
    IIwritio(1,(short *)0,1,32,0,O3table);
    IIwritio(0,(short *)0,1,32,0,(char *)"._id");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&r[i].ooid);
      IIgetdomio((short *)0,1,32,50,name);
      IIgetdomio((short *)0,1,30,4,&r[i].master);
      IIgetdomio((short *)0,1,30,4,&r[i].mdelete);
      IIgetdomio((short *)0,1,30,4,&r[i].moffset);
      IIgetdomio((short *)0,1,32,12,conn);
      IIgetdomio((short *)0,1,30,4,&r[i].detail);
      IIgetdomio((short *)0,1,30,4,&r[i].ddelete);
      IIgetdomio((short *)0,1,30,4,&r[i].doffset);
      if (IIerrtest() != 0) goto IIrtB6;
      {
/* # line 314 "ooschema.qsc" */	/* host code */
			if (i >= cnt) {
				tooMany(_masterRefs, class->name);
/* # line 316 "ooschema.qsc" */	/* endretrieve */
        {
          IIbreak();
          if (1) goto IIrtE6;
        }
/* # line 317 "ooschema.qsc" */	/* host code */
			}
			OOhash(r[i].ooid, (OO_OBJECT*)&r[i]);
			r[i].data.levelbits = 03;	/* levels 0,1 */
			r[i].class = OC_REFERENCE;
			if ((r[i].name = STalloc(name)) == NULL) {
			    IIUGerr(E_OO0036_STalloc_err_fetchMast, UG_ERR_ERROR,
					0
				);
			    return FAIL;
			}
			if ((r[i].connector = STalloc(conn)) == NULL) {
			    IIUGerr(E_OO0037_STalloc_err_fetchMast, UG_ERR_ERROR,
					0
				);
			    return FAIL;
			}
			OOsnd(col, _atPut, i, r[i].ooid);
			i++;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
/* # line 336 "ooschema.qsc" */	/* host code */
		if ( i > cnt || iiooInqResult(ERx("RETRIEVE"), O3table) != OK )
			return FAIL;
		if (i < cnt) {
			tooFew(_masterRefs, class->name);
			return FAIL;
		}
	}
	return OK;
}
static STATUS
fetchDetails(class)
OO_CLASS	*class;
  {
    OOID classn;
	register OOID	col;
    i4 cnt, ooid;
	register i4	i;
	char	*_detailRefs = ERx("detailRefs");
	/* retrieve detail reference ooid's */
	classn = class->ooid;
	(VOID) allocRefsCol(classn, ERx("detail"), &class->detailRefs, FALSE, &cnt);
	col = class->detailRefs;
	if (col != nil ) {
		i = 0;
/* # line 363 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIsyncup((char *)0,0);
    }
/* # line 364 "ooschema.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(ooid=");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id)where ");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)".detail=");
      IIputdomio((short *)0,1,30,4,&classn);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&ooid);
        if (IIerrtest() != 0) goto IIrtB7;
        {
/* # line 368 "ooschema.qsc" */	/* host code */
			if (i >= cnt) {
				tooMany(_detailRefs, class->name);
/* # line 370 "ooschema.qsc" */	/* endretrieve */
          {
            IIbreak();
            if (1) goto IIrtE7;
          }
/* # line 371 "ooschema.qsc" */	/* host code */
			}
			OOsnd(col, _atPut, i, ooid);
			i++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE7:;
    }
/* # line 375 "ooschema.qsc" */	/* host code */
		if ( i > cnt || iiooInqResult(ERx("RETRIEVE"), O3table) != OK )
			return FAIL;
		if (i < cnt) {
			tooFew(_detailRefs, class->name);
			return FAIL;
		}
	}
	return OK;
  }
  CnameOk(self, name)
  OO_CLASS *self;
  char *name;
  {
    i4 anyOther;
	O1table = O1->table_name;
	O2table = O2->table_name;
	if (!CMupper(name)) {
		IIUGerr( E_OO0038_NameMustBeCapitalized, UG_ERR_ERROR, 0 );
		return FALSE;
	}
	/* check name uniqueness */
/* # line 398 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,O2table);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,O2table);
      IIsyncup((char *)0,0);
    }
/* # line 399 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIsyncup((char *)0,0);
    }
/* # line 400 "ooschema.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(anyOther=any(");
      IIwritio(1,(short *)0,1,32,0,O2table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id where ");
      IIwritio(1,(short *)0,1,32,0,O2table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id=");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)"._id and ");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,32,0,name);
      IIwritio(0,(short *)0,1,32,0,(char *)"))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&anyOther);
        if (IIerrtest() != 0) goto IIrtB8;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE8:;
    }
/* # line 402 "ooschema.qsc" */	/* host code */
	if (anyOther) {
		IIUGerr( E_OO0039_Class_already_exists, UG_ERR_ERROR, 1, name );
		return FALSE;
	}
	return TRUE;
  }
OOID
Crename(self, newname)
register OO_CLASS	*self;
register char	*newname;
{
	if (OOsndSelf(self, _nameOk, newname) == FALSE)
		return nil;
	else
	     /* Lint truncation warning if size of ptr > OOID,
	     ** but code valid. 
	     */
	     return (OOID)OOsndSuper(self, _rename, newname);
}
OOID
  CsubClass(self, name, remarks, table, surrogate, keepstatus)
  OO_CLASS *self;
  char *name;
   char		*remarks, *table, *surrogate;
   i4		keepstatus;
  {
    register OO_CLASS *aClass;
	register OOID	aClassid;
	if (OOsndSelf(self, _nameOk, name) == FALSE)
		return nil;
	/* Lint truncation warning if size of ptr > OOID,
	** but code valid. 
	*/
	aClassid = (OOID)OOsnd(self->class, _newDb, FALSE, name, 0, _, 1, remarks,
	    NULL, NULL, NULL);
	aClass = (OO_CLASS*)OOp(aClassid);
	aClass->super = self->ooid;
	aClass->table_name = table;
	aClass->surrogate = surrogate;
	aClass->keepstatus = keepstatus;
	aClass->level = self->level + 1;
	aClass->hasarray = self->hasarray;
	/* retrieve of size of new part of struct for class */
/* # line 452 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIsyncup((char *)0,0);
    }
/* # line 453 "ooschema.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(memsize=4*(max(");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_attid);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_attrelid);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,32,0,aClass->table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"and ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".attowner=usercode)-1+coun\
t(");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_attid);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_attrelid);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,32,0,aClass->table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"and ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".attowner=usercode and ");
      IIwritio(1,(short *)0,1,32,0,_attribute);
      IIwritio(0,(short *)0,1,32,0,(char *)".attfrmt=31)))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&aClass->memsize);
        if (IIerrtest() != 0) goto IIrtB9;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE9:;
    }
/* # line 459 "ooschema.qsc" */	/* host code */
	aClass->memsize += self->memsize;
	/*
	BTsetF(1, aClass->data.levelbits);
	*/
	OOsnd(aClass, ii_flushAll);
	/* record akindof hierarchy for new class */
/* # line 466 "ooschema.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIsyncup((char *)0,0);
    }
/* # line 467 "ooschema.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append to ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,_xclass);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,30,4,&aClass->ooid);
      IIwritio(0,(short *)0,1,32,0,(char *)", ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)".akindof)where ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,_class);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIsyncup((char *)0,0);
    }
/* # line 469 "ooschema.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append to ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,_xclass);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,30,4,&aClass->ooid);
      IIwritio(0,(short *)0,1,32,0,(char *)", ");
      IIwritio(1,(short *)0,1,32,0,_iiakindof);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,30,4,&aClass->ooid);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 471 "ooschema.qsc" */	/* host code */
	return aClassid;
  }
