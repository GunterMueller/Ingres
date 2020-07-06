# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1987, 2005 Ingres Corporation
**
*/
# include	<compat.h>
# include	<cv.h>
# include	<lo.h>
# include       <me.h>
# include	<si.h>
# include	<st.h>
# include	<cm.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include <ooclass.h>
# include	<oodefine.h>
# include	"ooldef.h"
# include <oosymbol.h>
# include <oocat.h>
# include	"eroo.h"
/**
** Name:	ooencode.sc - Read/Write Encoded Objects to the database
**
** Description:
**	This file defines:
**
**	ENdecode()	decode a encoded object string into a set
**		of ordinary objects.
**	ENdestroy()	destroy an OC_ENCODING object.
**	ENinit()	initialize an OC_ENCODING object.
**	ENfetch()	read an OC_ENCODING object from the database.
**	ENflush()	write an OC_ENCODING object to the database.
**
** History:
**	Revision 6.4  89/12/19  dave
**	12/19/89 (dkh) - VMS shared lib changes - Added include of ooldef.h
**      22-oct-90 (sandyd)
**              Fixed #include of local ooldef.h to use "" instead of <>.
**      27-jan-93 (leighb)
**              Allocate "buf" instead of using the stack.  
**              (Stack overflowed on the PC.)
**	Revision 4.0  85/08  peterk
**	Initial revision.
**
**	Revision 6.0  87  peterk
**	Updated for release 6.
**
**	Revision 6.1  88/11/09  marian
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**      15-sep-93 (kchin)
**              Changed use of FORCE_ALIGN macro to ME_ALIGN_MACRO in
**		iiooNumDecode().  We should stick to one standard alignment
**		interface in the CL.
**      18-oct-1993 (kchin)
**          	Since the return type of OOsnd() and OOsndSelf() have been 
**		changed to PTR, when being called in ENdecode(), _doVal() 
**		and ENreadfile(), their return types need to be cast to the 
**		proper datatypes.
**
**	Revision 6.4  89/12/19  dave
**	12/19/89 (dkh) - VMS shared lib changes - Added include of ooldef.h
**
**      22-oct-90 (sandyd)
**              Fixed #include of local ooldef.h to use "" instead of <>.
**	2-jun-93 (blaise)
**		From version 6.5 we no longer attempt to convert pre-6.0
**		objects; changed the code accordingly.
**	Revision 4.0  85/08  peterk
**	Initial revision.
**
**      06-dec-93 (smc)
**		Bug #58882
**              Commented lint pointer truncation warning.
**      15-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's change for axp_osf
**              11-mar-93 (kchin)
**              Need to cast p in _doVal() from i4 * to long * when dealing
**              with string pointers, see comments below.
**              The fix is necessary for 64-bit platform like Alpha (axp_osf).
**      24-mar-1999 (hweho01)
**              Extended the change for axp_osf to AIX 64-bit platform
**              (ris_u64).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      11-Sep-2000 (hanje04)
**              Extended the change for axp_osf to Alpha Linux (axp_lnx)
**	24-apr-2003 (somsa01)
**	    Use i8 instead of long for LP64 case.
**	07-sep-2005 (abbjo03)
**	    Replace xxx_FILE constants by SI_xxx.
**    25-Oct-2005 (hanje04)
**        Add prototype for _doVal() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**/
/* local prototypes */
char	*iiooStrAlloc();
i4	IIOOencodedSize();
static i4 _doVal (
		char	**tp,
		i4	*p,
		i4	reloc);
OOID
ENdecode(self, reloc)
OO_ENCODING	*self;
i4	reloc;		/* if TRUE relocate permanent ids */
{
	register char	*s = self->estring;
	register char	*p;
	register OO_OBJECT	*obj;
	register i4	len;
	OOID		id;
	char		*t;
	OOID		topObject = nil;
	OOID		class;
	OO_CLASS	*classp;
	i4		version;
	GLOBALREF i4	LevelMask[];
	relocInit();
	if (*s != 'V')
	{
	    /* error - pre 6.0 encoding */
	   IIUGerr(E_OO0051_OLD_VERSION, UG_ERR_ERROR, 0);
	   return nil;
	}
	t = s;
	/* read version stamp */
	_doVal(&t, &version, FALSE);
	while ( t && *t != EOS )
	{
	    /* skip the Object Definition marker '*' */
	    /* read the object id */
	    for ( s = ++t; *t != '{';) t++;
	    *t = EOS;
	    CVan(s, &id);
	    *t++ = '{';
	    if (topObject == nil && id >= 0 && !reloc)
	    {
		topObject = self->object = id;
		obj = OOp(id);
		p = (char *)obj + O1->memsize;
	    }
	    else
	    {
		/* relocate temp id's or perm id's if necessary */
		if (id < 0 || (reloc && id > MAXSYSOBJECT))
		    id = relocId(id);
		/* create obj directly from level 0 info in estring */
		len = _doVal(&t, (i4 *)&class, FALSE);
                /* lint truncation warning if size of ptr > OOID,
                ** but code valid. 
                */
		id = (OOID)OOsnd(class, ERx("alloc"), id, 0);
		obj = OOp(id);
		p = (char *)obj + OOHDRSIZE +len;
		if (topObject == nil)
		{
		    topObject = self->object = id;
		}
	    }
	    classp = OOpclass(obj->class);
	    obj->data.levelbits = LevelMask[classp->level];
	    obj->data.inDatabase = self->data.inDatabase;
	    obj->data.dbObject = self->data.dbObject;
	    /* read the (non-OC_OBJECT) attributes */
	    for (; *t != '}';)
	    {
		len = _doVal(&t, (i4 *)p, reloc);
		p += len;
	    }
	    ++t;
	    while (CMwhite(t))
		CMnext(t);
	}
	return topObject;
}
static i4
_doVal ( tp, p, reloc )
char	**tp;	/* ptr to estring buffer ptr */
i4	*p;	/* destination location for values */
i4	reloc;	/* if TRUE relocate permanent, non-system object ids */
{
	register char	*t = *tp;
	i4		len = sizeof(*p);
	char		control;
#if defined(LP64)
        i8            diff;
#endif
	while (CMspace(t))
	    CMnext(t);
	switch ( control = *t++ )
	{
	    case ';':		/* class level end marker */
	    case '/':		/* class level elision marker */
		len = 0;
		break;
	    case 'N':
		*p = (i4) nil;	/* nil Object (0) */
		break;
	    case 'U':		/* UNKNOWN Object (-1) */
		*p = (i4) UNKNOWN;
		break;
	    case '"':		/* NULL (char *) value */
#if defined(LP64)
                /*
                ** Need to align string pointer to sizeof(PTR) (8-byte on
                ** 64-bit platform) boundary.
                ** Also, need to cast p to (long *) for string pointers
                ** assignments.
                */
                diff = (i8)p;
                p = (i8 *)ME_ALIGN_MACRO(p,sizeof(PTR));
                *(i8 *)p = 0L;
                /*
                ** since there is an adjustment of p using the ME_ALIGN_MACRO,
                ** the return len must be adjusted accordingly as well.
                */
                len = sizeof(PTR) + ((i8)p-diff);
#else /* LP64 */
		*p = (i4) 0;
#endif /* LP64 */
		break;
	    case '\'':		/* 'character string' */
	    {
		register char	*b;
		char            *buf;
		buf = MEreqmem((u_i4)0, (u_i4)(10*ESTRINGSIZE),FALSE,NULL);
		b = buf;
		while ( (*b = *t++) != '\'' )
		{
			if (*b == '\\')
				*b = *t++;
			*b++;
		}
		*b = EOS;
#if defined(LP64)
                /*
                ** Need to align string pointer to sizeof(PTR) (8-byte on
                ** 64-bit platform) boundary.
                ** Also, need to cast p to (long *) for string pointers
                ** assignments, original (i4 *) will truncate the pointers
                ** values.
                */
                diff = (i8)p;
                p = (i8 *)ME_ALIGN_MACRO(p,sizeof(PTR));
                *(i8 *)p = (i8) STalloc(buf);
                /*
                ** since there is an adjustment of p using the ME_ALIGN_MACRO,
                ** the return len must be adjusted accordingly as well.
                */
                len = sizeof(PTR) + ((i8)p-diff);
#else /* LP64 */
		*p = (i4) STalloc(buf);
#endif /* LP64 */
		MEfree(buf);
		break;
	    }
	    case '[':		/* list of Collection contents */
	    {
		register i4	j;
		char		*n;
		OOID		carray[1000];
		n = t;
		for( j = 0 ; *n != ']' ; )
			_doVal(&n, &carray[j++], reloc);
		t = n + 1;
		if ( j == 0 )
			*p = (i4) nil;
		else
		{
			register OOID	col;
    			/* lint truncation warning if size of ptr > OOID, 
			   but code valid */
			col = (OOID)OOsnd(OC_COLLECTION, _new, OC_UNDEFINED, 0, j);
			while ( --j >= 0 )
				OOsnd(col, _atPut, j, carray[j], 0);
			*p = (i4) col;
		}
		break;
	    }
	    default:		/* float, integer, version, object id */
	    {
		register char	*s = t;
		while ( STindex(ERx("OIF'\"[]{}V*/;NU"), t, 0) == NULL )
			++t;
		len = iiooNumDecode(control, s, t, reloc, p);
		break;
	    }
	} /* end switch */
	*tp = t;
	return len;
}
/*{
** Name:	iiooNumDecode() -	Decode Numeric Value.
**
** Description:
**	Decodes a numeric value from an input string.
**
** Inputs:
**	control	{char}  The type of numeric value.
**	start	{char *}  The start of the input buffer.
**	end	{char *}  The end of the input buffer.
**	reloc	{nat}  Whether to relocate or not.
**
** Output:
**	p	{i4 *}  The address of the decoded value.
**
** History:
**	05/90 (jhw)  Abstracted from pre-6.0 and 6.0 decode value routines
**		for JupBug #21433.  Pre-6.0 decode value routine was not
**		aligning f8's.
*/
i4
iiooNumDecode ( control, start, end, reloc, p )
char	control;
char	*start;
char	*end;
i4	reloc;
i4	*p;
{
	char	terminator;
	i4	len;
	terminator = *end;
	*end = EOS;
	switch (control)
	{
	    case 'F':	/* float attribute */
	    {
		register double	*aligned_p;
		aligned_p = (double *) ME_ALIGN_MACRO(p,sizeof(ALIGN_RESTRICT));
		CVaf(start, '.', aligned_p);
		len = (char *)aligned_p - (char *)p + sizeof(double);
		break;
	    }
	    default:	/* integer, version stamp, object id */
	    {
		i4	i;
		CVan(start, &i);
		if ( control == 'O' &&
				( i < -1 || ( reloc && i > MAXSYSOBJECT ) ) )
			i = (i4) relocId(i);
		*p = i;
		len = sizeof(*p);
	    }
	} /* end switch */
	*end = terminator;
	return len;
}
struct	relocStruct {
	OOID	orig;
	OOID	reloc;
};
static struct relocStruct	relocVec[MAXRELOC], *relocEnd = relocVec;
relocInit()
{
	relocEnd = relocVec;
}
OOID
relocId(id)
OOID	id;
{
	register struct relocStruct		*p;
	for (p = relocVec; p < relocEnd; p++)
	    if (p->orig == id)
		return p->reloc;
	++relocEnd;
	p->orig = id;
	return p->reloc = IIOOtmpId();
}
OOID
ENinit(self, objid, inDatabase, estring)
OO_ENCODING	*self;
register OOID	objid;
i4		inDatabase;
char		*estring;
{
	char		objname[50];
	register char	*s;
	s = iiooStrAlloc(self, STprintf(objname, ERx("encoded<%d>"), objid));
	OOsndSuper( self, _init, s, 0, _, 1, s,
			(char *)NULL, (char *)NULL, (char *)NULL, 0
	);
	self->data.dbObject = TRUE;
	self->data.inDatabase = inDatabase;
	self->class = OC_ENCODING;
	self->object = objid;
	self->estring = estring;
	return self->ooid;
}
static i4	_colSize = 0;
/*
** History:
**	01/90 (jhw) -- Added support for B1 catalog size difference.
*/
OOID
ENflush(self, class)
  OO_ENCODING *self;
OOID	class;
{
	register i4	len;
  register char *p;
  i4 seq;
	if ( _colSize == 0 )
	{
		if ( (_colSize = IIOOencodedSize()) <= 0 )
			return nil;
	}
	p = self->estring;
	/* destroy existing encoded object if nec. */
	if (self->data.inDatabase)
	    OOsndSelf(self, _destroy, 0);
	/* output encoded object tuples, piece by piece */
	len = STlength(p);
	iiuicw1_CatWriteOn();
	for ( seq = 0; ; ++seq )
	{
	    char	csav;
	    if ( len > _colSize )
	    {
		csav = p[_colSize];
		p[_colSize] = EOS;
	    }
/* # line 435 "ooencode.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ooencode1",6308,17647);
      IIputdomio((short *)0,1,30,4,&self->object);
      IIputdomio((short *)0,1,30,4,&seq);
      IIputdomio((short *)0,1,32,0,p);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ooencode1",6308,17647);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_encodings(object_id, encode_object, encode_sequence, e\
ncode_estring)values(0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&self->object);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&seq);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,p);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ooencode1",6308,17647);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 438 "ooencode.sc" */	/* host code */
	    if ( len <= _colSize )
		break;
	    p[_colSize] = csav;
	    p += _colSize;
	    len	-= _colSize;
	}
	iiuicw0_CatWriteOff();
	self->data.inDatabase = TRUE;
	return self->ooid;
}
OOID
ENfetch(self, class)
  register OO_ENCODING *self;
OOID	class;
{
	char    *buf;
  char *estring;
  i4 seq;
	estring = buf = MEreqmem((u_i4)0,(u_i4)(10*ESTRINGSIZE),FALSE,NULL);
/* # line 461 "ooencode.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ooencode2",6309,19707);
      IIputdomio((short *)0,1,30,4,&self->object);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ooencode2",6309,19707);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select encode_sequence, encode_estring from ii_encodings where encode\
_object=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&self->object);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by encode_sequence");
        IIexDefine(0,(char *)"ooencode2",6309,19707);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&seq);
      IIgetdomio((short *)0,1,32,0,estring);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 467 "ooencode.sc" */	/* host code */
	    estring += STtrmwhite(estring);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	self->estring = iiooStrAlloc(self, buf);
	self->data.dbObject = TRUE;
	self->data.inDatabase = TRUE;
	MEfree(buf);
	return self->ooid;
}
OOID
ENdestroy(self)
  register OO_ENCODING *self;
{
	if (self->data.inDatabase)
	{
		iiuicw1_CatWriteOn();
/* # line 485 "ooencode.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where encode_object=");
    IIputdomio((short *)0,1,30,4,&self->object);
    IIsyncup((char *)0,0);
  }
/* # line 486 "ooencode.sc" */	/* host code */
		iiuicw0_CatWriteOff();
	}
	return self->ooid;
}
OOID
ENreadfile(self, file)
OO_CLASS	*self;
LOCATION	*file;
{
	register OOID	enid;
	register OO_OBJECT	*en;
	char            *buf;
	FILE		*f;
	i4		count;
	if (SIfopen(file, ERx("r"), SI_VAR, 0, &f) != OK)
	    return nil;
	buf = MEreqmem((u_i4)0, (u_i4)(10*ESTRINGSIZE), FALSE, NULL);
	SIread(f, 10*ESTRINGSIZE, &count, buf);
	buf[count] = EOS;
	SIclose(f);
        /* lint truncation warning if size of ptr > OOID, but code valid */
	enid = (OOID)OOsndSelf(self, _new, -1, 0, FALSE, iiooStrAlloc(self, buf), 0);
	en = OOp(enid);
	en->data.inDatabase = FALSE;
	en->data.dbObject = TRUE;
	MEfree(buf);
	return enid;
}
STATUS
ENwritefile(self, file)
OO_ENCODING	*self;
LOCATION	*file;
{
	FILE	*f;
	STATUS	stat;
	if ((stat = SIfopen(file, ERx("w"), SI_VAR, 0, &f))  == OK)
	{
	    stat = SIwrite(STlength(self->estring), self->estring, &stat, f);
	    SIputc('\n', f);
	    SIclose(f);
	}
	return stat;
}
