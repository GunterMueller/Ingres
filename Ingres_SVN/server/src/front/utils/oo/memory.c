# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<cv.h>
#include	<me.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<generr.h>
#include	<fe.h>
#include	<ug.h>
# include <ui.h>
# include <ooclass.h>
# include <oocat.h>
#include	<oodefine.h>
# include	"ooldef.h"
#include	<oocollec.h>
#include	<oosymbol.h>
#include	<erlq.h>
# include <uigdata.h>
#include	"eroo.h"
/**
** Name:	memory.sc -	Object Module Basic Database Access Routines.
**
** Description:
**	Contains routines used to fetch and flush objects to the database.
**	This file defines:
**
**	OOID fetchDb(OO_OBJECT*)
**		force retrieve of all levels from database
**		by zero-ing iiOOtable entry.
**	void IIOOforget(OO_OBJECT*)
**		erase information that object is
**		in memory.
**	OOID fetchAll(OO_OBJECT*)
**		retrieve all class levels from database
**		(if necessary) and create memory structure .
**	bool checkAt(OO_OBJECT*, OO_CLASS*)
**		Check data bits for all levels up to fetchlevel.
**	check0(OO_OBJECT*) and check0n(i4 id)
**		function versions of check0 and check0n.
**	OOID fetch0(OO_OBJECT*)
**		retrieve level 0 (OO_OBJECT*) info.
**	OOID fetchAt(OO_OBJECT*, OO_CLASS*)
**		retrieve a particular class level from database.
**	OOID flushAt(OO_OBJECT*, OO_CLASS*)
**		flush a particular class level to the database.
**	OOID flush0(OO_OBJECT*)
**		flush level 0 (OO_OBJECT*) info to database.
**	OOID IIflushAll(OO_OBJECT*)
**		flush all class levels to database.
**	OOID OOcheckRef(OO_OBJECT*, reference, i4  off)
**		resolve a reference attribute.
**	OOID fetchRef(OO_OBJECT*, reference)
**		retrieve a reference collection from database.
**	OOID masterOff(OO_CLASS*, i4  off)
**		return master reference located at byte offset.
**	void putstatus(OO_OBJECT*, OO_METHOD*)
**		update database with operation status information.
**	err(char *s, ...)
**		report error to stderr.
**	OOID OOextend(OO_OBJECT*, i4  oldsize, i4  newsize)
**		expand memory allocated for an OO_OBJECT to larger size.
**	OOID detailref(OOID, OO_ATTRIB*)
**		return detail reference from OO_COLLECTION represented by
**		OOID matching OO_ATTRIB attribute.
**	void tooMany(char *class_name, char *instance_name)
**		report error, retrieving more objects of given
**		class and instance name than expected.
**	void tooFew(char *class_name, char *instance_name)
**		report error, retrieving fewer objects of given
**		class and instance name than expected.
**
** History:
**	13-jul-87 (bab) Changed memory allocation to use [FM]Ereqmem.
**	7-july-1988 (danielt) put back REPEATED SELECTS
**	27-oct-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	12/08/88 (dkh) - Fixed venus bug 4033.
**	12/19/89 (dkh) - VMS shared lib changes - Added include of ooldef.h
**      22-oct-90 (sandyd)
**              Fixed #include of local ooldef.h to use "" instead of <>.
**	29-apr-92 (pearl)
**		Add port specific change for nc5_us5.
**		Change var _Delete to _ncr_Delete to avoid name space clash.
**	04-may-92 (pearl)
**	 	Above change rejected.  Cancel nc5_us5 specific definition
**		of _Delete; change it to iiDelete instead.
**	8-feb-93 (blaise)
**		Changed _flush, etc. to ii_flush because of conflict with
**		Microsoft C library.
**	27-jan-93 (blaise)
**		The previous change changed all tabs in this file to spaces;
**		changed back to tabs again.
**      18-oct-1993 (kchin)
**          	Since the return type of OOsnd() and OOsndSelf() have been 
**		changed to PTR, when being called in IIflushAll() and
**		OOcheckRef(), their return types need to be cast to the 
**		proper datatypes.
**	12-oct-93 (daver)
**		Casted parameters to MEcopy() and MEmove() to avoid
**		compiler warnings when these CL routines became prototyped.
**	15-Nov-1993 (tad)
**		Bug #56449
**		Replace %x with %p for pointer values where necessary.
**      06-dec-93 (smc)
**		Bug #58882
**          	Commented lint pointer truncation warning.
**      15-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's changes for axp_osf
**              03-feb-93 (kchin)
**              Change cast of self from i4  to PTR in flushAt() and fetchAt().
**              Cast of self to i4  will truncate the address from 64-bit
**              to 32-bit on Alpha.
**              12-nov-93 (kchin/mikehan)
**              When walking through structure that contains int (4 bytes)
**              and char * (size is 8 bytes on Alpha OSF/1) elements using
**              a "nat *", make sure the pointer element is skipped
**              properly (need to be done in the same way as dealing with f8
**              structure elements on Alpha, ie. skipping twice for f8 
**              members).  This is fixed in fetchAt() routine.
**      03-Feb-96 (fanra01)
**              Changed extern to GLOBALREF.
**      24-mar-1999 (hweho01)
**              Extended the changes for axp_osf to ris_u64 (AIX 64-bit port).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      11-Sep-2000 (hanje04)
**              Extended the changes for axp_osf to axp_lnx (Alpha Linux).
**      22-Jun-2001 (horda03)
**          Rather than fix the size of the iiOOtable as OOTABLESIZE, as no
**          matter how large we make it, someone is bound to require more
**          entries, allow the user to specify a larger size. The
**          environment variable II_OO_TABLE_SIZE has been added, and if
**          defined greater than OOTABLESIZE, the new value will be used.
**          (45781)
**	06-Mar-2003 (hanje04)
**	     Extend axp_osf changes to all 64bit platforms (LP64)
**	24-apr-2003 (somsa01)
**	    Use i8 instead of long for LP64 case.
**/
GLOBALREF OO_OBJECT	**iiOOtable;
bool checkAt();
OOID fetchDb();
VOID IIOOforget();
OOID fetchAll();
OOID fetch0();
OOID fetchAt();
OOID fetchNew();
OOID flush0();
OOID flushAt();
OOID IIflushAll();
OOID OOextend();
OOID detailref();
char *STindex();
char *STalloc();
OOID iiooClook();
char *iiooStrAlloc();
bool checkAt();
static OOID fetchObject();
static OOID flushObject();
static OOID OOwr_seqText();
static char	_Append[]	= ERx("Append");
static char	iiDelete[]	= ERx("Delete");
static char	_Replace[]	= ERx("Replace");
static char	_Retrieve[]	= ERx("Retrieve");
/*
**	fetchDb
**		force retrieve of all levels from database
*/
OOID
fetchDb(self)
register OO_OBJECT	*self;
{
	checkAt(self, OOp(self->class), -1);
	return self->ooid;
}
/*{
** Name:	IIOOforget() -	Remove information that object is
**			in memory.
*/
VOID
IIOOforget(self)
OO_OBJECT	*self;
{
	register i4	p;
	if ( (p = OOhash(self->ooid, (OO_OBJECT *)NULL)) != 0 )
	{
		iiOOtable[p] = iiOOnil;
	}
}
/*
**	fetchAll
**		retrieve all class levels from database
**		(if necessary) and create memory structure
*/
OOID
fetchAll(self)
register OO_OBJECT	*self;
{
	GLOBALREF	i4	LevelMask[];
	register OO_CLASS	*classp = (OO_CLASS *)OOp(self->class);
	register i4	level = classp->level;
	register i4	mask = LevelMask[level];
	/* simple check if all levels fetched */
	/* check each data level (level 0 already fetched) */
	if (level && (self->data.levelbits & mask) != mask) {
		checkAt(self, classp, level);
	}
	return self->ooid;
}
/*
**	checkAt
**		Check data bits for all levels up to fetchlevel.
**		Read level data from database if necessary by
**		directly invoking _fetch operation.
**
**		Return TRUE if at least one level actually
**		has to be fetched, else FALSE.
*/
bool
checkAt(self, class, fetchlevel)
register OO_OBJECT	*self;
register OO_CLASS	*class;
register i4	fetchlevel;
{
    register OO_METHOD	*fetchm;
    register i4	ret = 0;
    register OO_CLASS	*lookclass;
    register bool	isMeta = FALSE;
    OOID		fid;
    OO_CLASS		*fetchobj = (OO_CLASS *)self;
    FUNC_EXTERN OOID	(*Mproc())();
    for (; class->level; class = OOpclass(class->super))
    {
	if (fetchlevel >= 0)
	{
	    if (class->level > fetchlevel)
		continue;
	    if (BTtestF(class->level, self->data.levelbits))
		continue;
	}
	/*
	** look up _fetch operation directly
	** normally _fetch will be the generic param fetchAt()
	** but classes can provide a custom fetch if desired.
	*/
	/* look up _fetch operation starting at class */
	/* (or obj itself if class is a meta-class) */
	isMeta = OOisMeta(class->ooid);
	lookclass = isMeta? (OO_CLASS*)fetchobj: class;
	if ( (fid = iiooClook(lookclass, _fetch, isMeta)) == NULL )
	{
		/* shouldn't happen since defined for OC_OBJECT */
		IIUGerr( E_OO0012_No_fetch_operation, UG_ERR_ERROR, 1,
				class->name
		);
		return -1;
	}
	fetchm = (OO_METHOD*)OOp(fid);
	if (!fetchm->entrypt) {
	    /* find operation in proctab and save */
	    fetchm->entrypt = Mproc(fetchm->ooid);
	}
	if ((*fetchm->entrypt)(self, class->ooid) == nil) {
	    IIUGerr(E_OO0013_Error_from_fetch_of, UG_ERR_ERROR, 3,
		class->name, &self->ooid, self->name
	    );
	    return -1;
	}
	BTsetF(class->level, self->data.levelbits);
	ret = 1;
	if (isMeta)
	{
		fetchobj = (OO_CLASS*)OOp(fetchobj->super);
		isMeta = FALSE;
	}
    }
    return ret;
}
/*
**	fetch0
**		retrieve level 0 (OO_OBJECT) info
**		and create OO_OBJECT structure
**		via 'iiooAlloc()' and fetchAt();
**		(the object OC_CLASS is a slightly special case.)
*/
OOID
fetch0(self)
register OO_OBJECT	*self;
{
	register OOID	id;
	register i4	newsize;
	register i4	probe;
	id = self->ooid;
#ifdef DDEBUG
	D(ERx("fetch0(%d)"), id);
#endif /* DDEBUG */
	/* re-use memory if it exists */
	if ( (probe = OOhash(id, (OO_OBJECT *)NULL)) != 0 )
		return fetchAt(iiOOtable[probe], OC_OBJECT);
	id = iiooAlloc(O1, id);
	id = fetchAt(OOp(id), OC_OBJECT);
	if (id == nil)
	    return nil;
	self = OOp(id);
	/* set dbObject & inDatabase bits since we now know it's true */
	self->data.dbObject = self->data.inDatabase = TRUE;
	if (OOisEq(self->class, OC_OBJECT))
	    return self->ooid;
	if (OOisEq(id, OC_CLASS))
	    newsize = O2->memsize;
	else {
	    newsize = OOpclass(self->class)->memsize;
	}
	return OOextend(self, O1->memsize, newsize);
}
/*{
** Name:	fetchAt() - 
**
** Description: 
**	Fetches an object from a system catalog into a structure
** in memory.  This routine is a companion to fetchObject().  If
** the object to be fetched is a simple object (a select from the ii_objects
** catalog) then fetchAt() calls fetchObject(), which involves a
** hardcoded SQL SELECT statement.  Currently the fetching of a simple 
** object is the only operation used in QBF, and the predominant 
** object fetching operation
** in the other FE code where OO is used.  Therefore, fetchObject
** is called for these operations in order to avoid the use of Dynamic SQL, 
** for performance reasons as well as the clarity of the code.
**
** The fetchAt routine is designed to fetch an arbitrarily complex object
** from a system catalog into its in-memory representation.  It makes 
** use of the fact that objects have
** a canonical representation in memory.  All complex objects are represented
** in memory by a struct which consists of an OBJECT struct plus additional
** structure members.  This routine walks through the in-memory structure
** and sets up the SQL select statement as well as the target addresses for
** the IIUIcao_catOpen() call.  
**
** In order to do this, it calls OOp() to 
** get a pointer to the OO_COLLECTION struct associated with the
** object.  This OO_COLLECTION struct contains an array
** of OO_ATTRIB structs as one of its members.  Each OO_ATTRIB struct in this 
** array has the name, length, and datatype of each structure member of the
** in-memory object.
**
** An object is stored in the database as one tuple in a FE catalog with
** the following caveats:  an object can contain one instance of 'sequenced
** text' which is stored in a different catalog.  An example of this is
** the long_remarks member of the OBJECT struct, which is stored in the
** ii_longremarks catalog.  A structure member can also be a function pointer,
** in which case it is obviously not stored in the database.  
**
** The OO  static initialization code marks
** these two exceptions in the following way: in the OO_ATTRIB struct associated
** with the object structure member, the data type is marked as DB_DMY_TYPE if
** it is a function pointer, DB_MNY_TYPE if it is sequenced text in another
** table.  As fetchAt walks through the in-memory object representation, 
** if one of the structure members is one of these exceptions, it is ignored.
** Sequenced text is fetched using the OOrd_seqText routine, and the function
** pointer field is handled elsewhere in OO.
**
** Inputs:
**	OO_OBJECT	*self;  This is a pointer to some sort of object
**				structure (e.g. OO_OBJECT, OO_CLASS, OO_GRAPH)
**				self->ooid is the OOID of the object to
**				fetched.
**	OOID		cloid;  object id of the class that the object to
**				be fetched is a member of.  (used to determine
**				the FE catalog to fetch from).
**	
**	
**
** Outputs:
**			*self; 	filled in with all members that are stored
**				in the primary FE catalog for this object 
**				class.
**				
** Returns:
**		self->ooid		successful
**		nil			error
** Exceptions:
**		none
**
** Assumptions:
** This routine makes the following assumptions:
**	1)  One can safely set a i4  pointer to the beginning of a structure
** in memory that contains members that are guarenteed to
** be i4, f8, or char pointers and 'walk' through the structure  
** by incrementing the i4  pointer (incrementing twice in the
** case of an f8 member).  The i4  pointer will correctly point at the
** next structure member after each increment.
**	2)  The array of OO_ATTRIB structures that is associated with
** the object class to be fetched contains accurate information about
** the names, sizes, and data types of the columns to be fetched from
** the database.
**	3)  Various types of pointer can be freely cast: e.g. (i4 *) to
** PTR, (i4 *) to (char **)
**
** History:
**	01-dec-1987 (danielt) 
**		changed to call UI dynamic SQL utilities.
**	10/90 (jhw) - Check for 'nil' attributes for class as well as "isEmpty".
**		Bug #30772.
**	1-27-93 (leighb)
**		Allocate "strings".  Putting all that on the stack causes
**		a stack overflow on the PC.
**      22-nov-93 (smc)
**		Bug #58882
**		Fixed non-portable truncating cast of self.
**	11-sep-06 (gupsh01)
**		Added support for ANSI date/time types.
*/
OOID
fetchAt(self, cloid)
register OO_OBJECT *self;
OOID		cloid;
{
	char	format[500];
	char	qual[100];
	register OO_CLASS	*class = (OO_CLASS *)OOp(cloid);
	register char	*f = format;
	register i4	i, num_atts;
	register i4	*p;
	register i4	cvtcnt = 0;
	OOID		id;
	DB_DATA_VALUE	dbdv_array[DB_MAX_COLS];
	DB_DATA_VALUE	*varaddr[DB_MAX_COLS];
	DB_DATA_VALUE	*dvp;
	OO_aCOLLECTION	*att;
	char		*strings;
	char		*s;
	struct {
	    i4  varindex;
	    char  **fieldp;
	}	cvtarray[DB_MAX_COLS];	/* for stalloc'ing strings */
#ifdef DDEBUG
	D(ERx("fetchAt(x%p(%d, %s), x%p(%d, %s))"),
	    self, self->ooid, self->name,
	    class, class->ooid, class->name);
#endif /* DDEBUG */
	if ( class->attributes == nil || OOsnd(class->attributes, _isEmpty, 0) )
		return (self->ooid);
	if (cloid == OC_OBJECT)
	{
		return fetchObject(self);
	}
	strings = s = MEreqmem((u_i4)0, (u_i4)(FE_MAXNAME*DB_MAX_COLS),
					FALSE, NULL);
	p = (i4 *) ((char *)self + 
		(class->super == nil ? OOHDRSIZE
					: (OOpclass(class->super))->memsize));
	for (i= 0,num_atts=0, att=(OO_aCOLLECTION*)OOp(class->attributes); 
			i<att->size; i++, num_atts++)
	{
		register OO_ATTRIB	*a = (OO_ATTRIB *)att->array[i];
		varaddr[i] = &(dbdv_array[i]);
		dvp = varaddr[i];
		switch (a->frmt)
		{
		  case DB_MNY_TYPE:	/* actually (OOID *)() */
		  case DB_DMY_TYPE:	/* actually text in another table */
			p++;
			f--;
			num_atts--;
			break;
		  case DB_INT_TYPE:
		  case DB_FLT_TYPE:
			dvp->db_data = (PTR) p++;
			dvp->db_datatype = (DB_DT_ID) a->frmt;
			dvp->db_length = a->frml;	
			dvp->db_prec = 0;
			if (a->frmt == DB_FLT_TYPE) 
			{
			    p++;	/* skip another word */
			}
			STcopy(a->name, f); 
			f = &f[STlength(f)];
			break;
		  case DB_TXT_TYPE:
		  case DB_DTE_TYPE:
		  case DB_ADTE_TYPE:
		  case DB_TMWO_TYPE:
		  case DB_TMW_TYPE:
		  case DB_TME_TYPE:
		  case DB_TSWO_TYPE:
		  case DB_TSW_TYPE:
		  case DB_TSTMP_TYPE:
		  case DB_INDS_TYPE:
		  case DB_INYM_TYPE:
		  case DB_CHR_TYPE:
		  case DB_CHA_TYPE:
			dvp->db_data = (PTR) s;
			dvp->db_datatype = DB_CHA_TYPE;
			dvp->db_length = a->frml;
			s += a->frml + 1;
			cvtarray[cvtcnt].varindex = i;
#if defined(LP64)
                  /*
                  ** p need to be incremented in the right way,
                  ** incremented by "++" could result in problems
                  ** when trying to skip pointers which have size of
                  ** 8 bytes on Alpha OSF/1, since p is declared
                  ** as "nat *".
                  */
                        cvtarray[cvtcnt++].fieldp = (char **)p;
                        p += sizeof(PTR)/sizeof(i4);
#else /* LP64 */
			cvtarray[cvtcnt++].fieldp = (char **)p++;
#endif /* LP64 */
			STcopy(a->name, f); 
			f = &f[STlength(f)];
			break;
		  default:
			IIUGerr( E_OO0014_Unexpected_attribute, UG_ERR_ERROR, 1,
				a->frmt
			);
		}
		*f++ = ',';
	}
	*--f = EOS;
	id = self->ooid;
#ifdef DDEBUG
	D(ERx("	    format = '%s', id = %d"), format, id);
#endif /* DDEBUG */
        i = 0;
	if ( IIUIrao_relOpen(class->table_name, format, varaddr,
				STprintf(qual,"%s = %d", class->surrogate, id),
				(char *) NULL, num_atts, (bool) TRUE) == OK )
	{
		while ( IIUIraf_relFetch() == OK )
		{
		    /* should never be more than one */
		    if (i++ > 1) 
		    {
			char	buf[16];
			CVna(id, buf);
			tooMany(class->name, buf);
			break;
		    }
		}
		IIUIrac_relClose();
	}
	if ( i > 1 || iiooInqResult(_Retrieve, class->table_name) != OK )
	{
		MEfree(strings);
		return (NULL);
	}
	if (i < 1) 
	{
		char	buf[16];
		CVna(id, buf);
		IIUGerr( E_OO0042_too_few, UG_ERR_ERROR, 0);
		MEfree(strings);
		return NULL;
	}
	for (i = 0; i < cvtcnt; i++) 
	{
		register char	**p = cvtarray[i].fieldp;
		DB_DATA_VALUE	*dbv_ptr = varaddr[cvtarray[i].varindex];
		if ( p != NULL ) 
        	{
#if defined(LP64)
	/* need to skip extra padding bytes inserted by compiler in structure */
                        p = (PTR)ME_ALIGN_MACRO(p,sizeof(PTR));
#endif /* LP64 */
			if ((*p = STalloc(dbv_ptr->db_data)) == NULL )
			{
				IIUGerr( E_OO0015_Error_from_STalloc,
					UG_ERR_ERROR, 0);
			}
		}
	}
	BTsetF(class->level, self->data.levelbits);
	/* set master refs (i.e. Collections)? */
	/* to UNKNOWN or an UnknownRef */
	if ( !OOsnd(class->masterRefs, _isEmpty, 0) )
	{
	    i4		col;
	    char	*pcol;
	    OO_COLLECTION	*m;
	    m = (OO_COLLECTION*)OOp(class->masterRefs);
	    for (i = 0; i < m->size; i++) {
		OO_REFERENCE *r = (OO_REFERENCE *)OOp(m->array[i]);
		pcol = (char *)self + r->moffset;
		MEcopy((PTR)pcol, (u_i2)sizeof(i4), (PTR)&col);
		if (!col) {
		    col = (i4) UNKNOWN;
		    MEcopy((PTR)&col, (u_i2)sizeof(i4), (PTR)pcol);
		}
	    }
	}
	MEfree(strings);
	return id;
}
/*{
** Name:	fetchObject() - 
**
** Description: 
**	Fetches an object from the ii_objects system catalog into an OO_OBJECT
** struct.  This routine is a companion to fetchAt().  If
** the object to be fetched is a simple object (a select from the ii_objects
** catalog) then fetchAt() calls fetchObject(), which involves a
** hardcoded SQL SELECT statement.  Currently the fetching of a simple 
** object is the only operation used in QBF, and the predominant 
** object fetching operation
** in the other FE code where OO is used.  Therefore, fetchObject
** is called for these operations in order to avoid the use of Dynamic SQL, 
** for performance reasons as well as the clarity of the code.
**
**
** Inputs:
**	OO_OBJECT	*self;  This is a pointer to the OO_OBJECT
**				structure that is to be filled in.
**				self->ooid is the OOID of the object to
**				fetched.
**	
**	
**
** Outputs:
**			*self; 	filled in.
**				
** Returns:
**		self->ooid		successful
**		nil			error
** Exceptions:
**		none
**
**
** History:
**	01-dec-1987 (danielt) 
**		changed to call UI dynamic SQL utilities.
**	13-dec-1988 (neil) 
**		Changed to check for generic error as well.
*/
static OOID
fetchObject(self)
  register OO_OBJECT *self;
{
	STATUS	error_num;
	char	buf[20];
  char name[FE_MAXNAME+1];
  char owner[FE_MAXNAME+1];
  char short_remark[OOSHORTREMSIZE+1];
  OOCAT_DATE create_date;
  OOCAT_DATE alter_date;
/* # line 662 "memory.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"memory1",6312,1470);
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"memory1",6312,1470);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, object_class, object_name, object_owner, object_env\
, is_current, short_remark, create_date, alter_date from ii_objects wh\
ere object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&self->ooid);
        IIexDefine(0,(char *)"memory1",6312,1470);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&self->ooid);
      IIgetdomio((short *)0,1,30,4,&self->class);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,owner);
      IIgetdomio((short *)0,1,30,4,&self->env);
      IIgetdomio((short *)0,1,30,4,&self->is_current);
      IIgetdomio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,create_date);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,alter_date);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 676 "memory.sc" */	/* host code */
	if ( FEinqrows() == 0 )
	{
		CVna(self->ooid, buf);
		IIUGerr( E_OO0042_too_few, UG_ERR_ERROR, 0);
		return (OOID) nil;
	}
	if ( (error_num = FEinqerr()) != OK )
	{
		if ( error_num == E_LQ006A_SQMULTSEL ||
				error_num == GE_CARDINALITY )
		{
			CVna(self->ooid, buf);
			tooMany( _ii_objects, buf );
		}
		return (OOID) nil;
	}
	CVlower(name);
	CVlower(owner);
	CVlower(create_date);
	CVlower(alter_date);
	if ( (self->owner = iiooStrAlloc(self, owner)) == NULL ||
		(self->name = iiooStrAlloc(self, name)) == NULL ||
		(self->short_remark = iiooStrAlloc(self, short_remark)) == NULL ||
		(self->create_date = iiooStrAlloc(self, create_date)) == NULL ||
		(self->alter_date = iiooStrAlloc(self, alter_date)) == NULL )
	{ /* allocation error! */
		IIUGerr( E_OO0015_Error_from_STalloc, UG_ERR_ERROR, 0 );
		return (OOID) nil;
	}
	/* set dbObject & inDatabase bits since we now know it's true */
	self->data.dbObject = self->data.inDatabase = TRUE;
	return self->ooid;
}
/*{
** Name:	flushAt() - 
**
** Description: 
**	Flushes an object from a structure in memory into a
** system  catalog (or two catalogs if the object contains sequenced
** text).  This routine is a companion to flushObject().  If
** the object to be flushed is a simple object (an OO_OBJECT structure)
** then flushAt() calls flushObject(), which involves 
** hardcoded SQL statements.  Currently the flushing of a simple 
** object is the only operation used in QBF, and the predominant one
** in the other FE code where OO is used.  Therefore, flushObject()
** is called for these operations in order to avoid the use of Dynamic SQL, 
** for performance reasons as well as the clarity of the code.
**
** The flushAt routine is designed to flush an arbitrarily complex object
** from its in-memory representation into the corresponding system
** catalog(s).  It makes use of the fact that objects have
** a canonical representation in memory.  All complex objects are represented
** in memory by a struct which consists of an OBJECT struct plus additional
** structure members.  This routine walks through the in-memory structure
** and sets up the SQL statement strings as well as the target addresses for
** the IIUIupdate or IIUIinsert() calls.  
**
** In order to do this, it calls OOp() to 
** get a pointer to the OO_COLLECTION struct associated with the
** object.  This OO_COLLECTION struct contains an array
** of OO_ATTRIB structs as one of its members.  Each OO_ATTRIB struct in this 
** array has the name, length, and datatype of each structure member of the
** in-memory object.
**
** An object is stored in the database as one tuple in a FE catalog with
** the following caveats:  an object can contain one instance of 'sequenced
** text' which is stored in a different catalog.  An example of this is
** the long_remarks member of the OBJECT struct, which is stored in the
** ii_longremarks catalog.  A structure member can also be a function pointer,
** in which case it is obviously not stored in the database.  
**
** The OO  static initialization code marks
** these two exceptions in the following way: in the OO_ATTRIB struct associated
** with the object structure member, the data type is marked as DB_DMY_TYPE if
** it is a function pointer, DB_MNY_TYPE if it is sequenced text in another
** table.  
**
** As flushAt walks through the in-memory object representation, 
** if one of the structure members is a function pointer, it is ignored.
** If it is sequenced text, the sequenced text string is stored and
** flushed using the OOwr_seqText() routine.  With the rest of the structure
** members, flushAt() fills in a DB_DATA_VALUE structure which will be passed
** to one of the UI utilities, and in addition it builds the SQL statement
** strings.  If the object was previously in the database, flushAt calls
** IIUIupdate(), otherwise it calls IIUIinsert().
**
** Inputs:
**	OO_OBJECT	*self;  This is a pointer to some sort of object
**				structure (e.g. OO_OBJECT, OO_CLASS, OO_GRAPH)
**				self->ooid is the OOID of the object to
**				flushed.
**	OOID		cloid;  object id of the class that the object to
**				be flushed is a member of.  (used to determine
**				the FE catalog to flush to).
**	
**	
**
** Outputs:
**				
** Returns:
**		self->ooid		successful
**		nil			error
** Exceptions:
**		none
**
** Side effects: 
**		the object is flushed to the database.
**
** Assumptions:
** This routine makes the following assumptions:
**	1)  One can safely set a i4  pointer to the beginning of a structure
** in memory that contains members that are guarenteed to
** be i4, f8, or char pointers and 'walk' through the structure  
** by incrementing the i4  pointer (incrementing twice in the
** case of an f8 member).  The i4  pointer will correctly point at the
** next structure member after each increment.
**	2)  The array of OO_ATTRIB structures that is associated with
** the object class to be flushed contains accurate information about
** the names, sizes, and data types of the columns to be flushed from
** the database.  The data type ids in this OO_ATTRIB array correspond
** to the DB_DT types in dbms.h
**	3)  Various types of pointers can be freely cast: e.g. (i4 *) to
** PTR, (i4 *) to (char **), *(i4 *) to (char *)
**
** History:
**	23-nov-1987 (danielt) 
**		changed to call UI dynamic SQL utilities.
**	10/90 (jhw) - Check for 'nil' attributes for class as well as "isEmpty".
**		Bug #30772.
**	02-dec-93 (smc)
**		Bug #58882
**		Made truncating cast of self a portable PTR.
**	11-sep-06 (gupsh01)
**		Added support for ANSI date/time types.
*/
OOID
flushAt ( self, cloid )
register OO_OBJECT	*self;
OOID			cloid;
{
	register DB_DATA_VALUE	*dvp;
	register OO_CLASS	*class = OOpclass(cloid);
	register i4		i;
	register i4		*p;
	OOID			id;
	OO_aCOLLECTION		*att;
	char		*f;
	/* number of columns to be flushed to primary table */
	i4		num_columns; 
	DB_DATA_VALUE	dbdv[DB_MAX_COLS];
	char		sql_buf[UI_MAX_QBUF];
	struct seqTextStruct {
		OO_ATTRIB	*att;
		char		**data;
	} seqText;
	seqText.att = NULL;
	seqText.data = NULL;
	sql_buf[0] = EOS;
	id = self->ooid;
#ifdef DDEBUG
	D(ERx("flushAt(x%p(%d, %s), x%p(%d, %s))"),
	    self, id, self->name,
	    class, cloid, class->name);
#endif /* DDEBUG */
	if ( !self->data.dbObject || class->attributes == nil
			|| OOsnd(class->attributes, _isEmpty, 0) )
		return id;
	/* if self does not yet have a permanent database id get one now... */
	if ( id == UNKNOWN || OOisTempId(id) )
	{
		id = IIOOnewId();
		if ( OOhash(id, self) == 0 )
		    return nil;
		self->ooid = id;
	}
	iiuicw1_CatWriteOn();	/* for safety's sake ...
				**	all external code should rely on
				**	'OOsave()' to enable catalog write.
				*/
  	if ( cloid == OC_OBJECT )
  		return flushObject(self);
	p = (i4 *) ((PTR)self +
			(class->super == nil ? OOHDRSIZE
					: (OOpclass(class->super))->memsize
			)
		);
	att = (OO_aCOLLECTION*) OOp(class->attributes);
	num_columns = 0;
	for (i = 0 ; i < att->size ; ++i)
	{
		register OO_ATTRIB	*a = (OO_ATTRIB *)att->array[i];
		switch ( a->frmt )
		{
		  case DB_DMY_TYPE:	/* sequenced text in another table */
			if ( seqText.att )
			{
			    IIUGerr( E_OO0016_Only_one_Sequenced_Te,
				UG_ERR_ERROR, 0
			    );
			    return nil;
			}
			seqText.att = a;
			seqText.data = (char **)p;
			/* fall thru into ... */
		  case DB_MNY_TYPE:	/* actually (OOID *)() */
			++p;	/* skip over attribute value */
			continue;
		}
		STcat(sql_buf, a->name); 
		if (self->data.inDatabase)
		{
			STcat(sql_buf,ERx(" = ?"));
		}
		STcat(sql_buf,ERx(", "));
		dvp = &dbdv[num_columns];
		switch (a->frmt) 
		{
		  case DB_INT_TYPE:
			dvp->db_data = (PTR) p++;
			dvp->db_datatype = (DB_DT_ID) a->frmt;
			dvp->db_length = a->frml;	
			dvp->db_prec = 0;
			break;
		  case DB_FLT_TYPE:
			dvp->db_data = (PTR) p;
			dvp->db_datatype = (DB_DT_ID) a->frmt;
			dvp->db_length = a->frml;	
			dvp->db_prec = 0;
			p += sizeof(f8)/sizeof(i4);	/* skip f8 */
			break;
		  case DB_TXT_TYPE:
		  case DB_DTE_TYPE:
		  case DB_ADTE_TYPE:
		  case DB_TMWO_TYPE:
		  case DB_TMW_TYPE:
		  case DB_TME_TYPE:
		  case DB_TSWO_TYPE:
		  case DB_TSW_TYPE:
		  case DB_TSTMP_TYPE:
		  case DB_INDS_TYPE:
		  case DB_INYM_TYPE:
		  case DB_CHR_TYPE:
		  case DB_CHA_TYPE:
#if defined(LP64)
                        dvp->db_data = (PTR) *(i8 *)p;
#else /* LP64  */
			dvp->db_data = (PTR) *p;
#endif /* LP64 */
			dvp->db_datatype = DB_CHA_TYPE;
#if defined(LP64)
                        /*
                        ** Need to align p to the right boundary and cast
                        ** it to long in order to get the right 64-bit address.
                        ** Also, p will be incremented to the right location
                        **                              - kchin 2/4/93
                        */
                        p = (PTR)ME_ALIGN_MACRO(p,sizeof(PTR));
                        dvp->db_length = STlength((char *) (long)*(long *)p);
#else /* LP64 */
			dvp->db_length = STlength((char *) *p++);
#endif /* LP64 */
			dvp->db_prec = 0;
#if defined(LP64)
                        p += sizeof(PTR)/sizeof(i4);  /* skip PTR */
#endif /* LP64 */
			break;
		default:
			IIUGerr( E_OO0017_Unexpected_attribute_, UG_ERR_ERROR,
					1, a->frmt
			);
		}
		++num_columns;
	}
	/* add format for surrogate id */
	STcat(sql_buf, class->surrogate);
	if (self->data.inDatabase)
	{
		STcat(sql_buf,ERx(" = ?"));
	}
	dvp = &dbdv[num_columns++];
	dvp->db_data = (PTR) &self->ooid;
	dvp->db_datatype = DB_INT_TYPE;
	dvp->db_length = sizeof(OOID);	
	dvp->db_prec = 0;
#ifdef DDEBUG
	D(ERx("	    n = %d"), self->ooid);
#endif /* DDEBUG */
	if ( self->data.inDatabase )
	{
		char	where_buf[2*FE_MAXNAME + 3 + 1];
		IIUIupdate( class->table_name, sql_buf, dbdv, 
				STprintf( where_buf, ERx("%s = %d"),
						class->surrogate, self->ooid
				),
				num_columns
		); 
	    	f = ERx("Update");
	}
	else
	{
		IIUIinsert(class->table_name, sql_buf, dbdv, num_columns);
	    	f = ERx("Insert");
	    	if ( seqText.att && *seqText.data == NULL )
		{ /* initialize seqText data to null string */
			*seqText.data = _;
		}
	}
	if ( iiooInqResult(f, class->table_name) != OK )
	    return nil;
	self->data.inDatabase = TRUE;
	if (seqText.att && *seqText.data != NULL)
	{
	    id = OOwr_seqText(id, *seqText.data, _ii_longremarks, _long_remark);
	}
	return id;
}
/*{
** Name:	flushObject() - 
**
** Description: 
**	Flushes an object from an OO_OBJECT struct into the ii_objects
** and ii_longremarks FE catalogs.  This routine is a companion to flushAt().
** If the object to be flushed is a simple object then flushAt() calls 
** flushObject(), which involves hardcoded SQL statements.  Currently 
** the flushing of simple objects is the only operation used in QBF, and the 
** predominant one in the other FE code where OO is used. flushObject()
** is called for these operations in order to avoid the use of Dynamic SQL, 
** for performance reasons as well as for the clarity of the code.
**
**
** Inputs:
**	OO_OBJECT	*self;  This is a pointer to the OO_OBJECT
**				structure that is to be flushed.
**				self->ooid is the OOID of the object to
**				flushed.
**	
**	
**
** Outputs:
**		none
**				
** Returns:
**		self->ooid		successful
**		nil			error
** Exceptions:
**		none
**
** Side effects:
**		The object is flushed to the database.
**
** History:
**	01-dec-1987 (danielt) 
**		changed to call UI dynamic SQL utilities.
**	6/89 (bobm)	update class in case it has been modified.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
static OOID
flushObject ( self )
  register OO_OBJECT *self;
{
  UIDBDATA *uidbdata;
	OOID	ret_val = nil;
	uidbdata = IIUIdbdata();
	if (STlength(self->short_remark) >= OOSHORTREMSIZE)
	{
	    self->short_remark[OOSHORTREMSIZE] = EOS;
	}
	if (self->data.inDatabase)
	{
/* # line 1051 "memory.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"memory2",6313,24626);
      IIputdomio((short *)0,1,32,0,self->name);
      IIputdomio((short *)0,1,30,4,&self->class);
      IIputdomio((short *)0,1,32,0,self->short_remark);
      IIputdomio((short *)0,1,32,0,self->alter_date);
      IIputdomio((short *)0,1,32,0,uidbdata->user);
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"memory2",6313,24626);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_objects set object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,self->name);
        IIwritio(0,(short *)0,1,32,0,(char *)", object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&self->class);
        IIwritio(0,(short *)0,1,32,0,(char *)", short_remark=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,self->short_remark);
        IIwritio(0,(short *)0,1,32,0,(char *)", alter_date=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,self->alter_date);
        IIwritio(0,(short *)0,1,32,0,(char *)", last_altered_by=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,uidbdata->user);
        IIwritio(0,(short *)0,1,32,0,(char *)
", alter_count=alter_count +1 where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&self->ooid);
        IIexDefine(0,(char *)"memory2",6313,24626);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1059 "memory.sc" */	/* host code */
	    ret_val = ( iiooInqResult(ERx("Update"), _ii_objects) != OK ||
		    		FEinqrows() == 0 /* must have been deleted */ )
		    	? nil : self->ooid;
	}
	if (ret_val == nil)
	{
	    /* 
	    ** We might be here either if the object was not in
	    ** the database OR if the object had been deleted from under
	    ** the current user (in the database).  If this is the case,
	    ** then the object is in our in memory cache, but not in the
 	    ** database, so we just try to INSERT it.  
	    */
/* # line 1072 "memory.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"memory3",6314,24666);
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIputdomio((short *)0,1,30,4,&self->class);
      IIputdomio((short *)0,1,32,0,self->name);
      IIputdomio((short *)0,1,30,4,&self->env);
      IIputdomio((short *)0,1,32,0,self->owner);
      IIputdomio((short *)0,1,30,4,&self->is_current);
      IIputdomio((short *)0,1,32,0,self->short_remark);
      IIputdomio((short *)0,1,32,0,self->create_date);
      IIputdomio((short *)0,1,32,0,self->alter_date);
      IIputdomio((short *)0,1,32,0,uidbdata->user);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"memory3",6314,24666);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_objects(object_id, object_class, object_name, object_e\
nv, object_owner, is_current, short_remark, object_language, create_da\
te, alter_date, alter_count, last_altered_by)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&self->ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&self->class);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,self->name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&self->env);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,self->owner);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&self->is_current);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,self->short_remark);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,self->create_date);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,self->alter_date);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,32,0,uidbdata->user);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"memory3",6314,24666);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1091 "memory.sc" */	/* host code */
	    ret_val = ( iiooInqResult(ERx("Insert"), _ii_objects) != OK )
	   		? nil : self->ooid;
	}
	if (ret_val != nil)
	{
	    self->data.inDatabase = TRUE;
	    if (self->long_remark != NULL)
	    {
		 ret_val = OOwr_seqText(self->ooid, self->long_remark,
					_ii_longremarks, _long_remark);
	    }
	}
	return (ret_val);
}
OOID
flush0(self)
register OO_OBJECT	*self;
{
	return flushAt(self, OC_OBJECT);
}
OOID
IIflushAll(self)
register OO_OBJECT	*self;
{
	register OOID		class;
	register OO_CLASS	*classp;
	register bool		wasInDatabase = self->data.inDatabase;
	if ( self == NULL )
	{
		return nil;
	}
	if ( !self->data.dbObject )
		return self->ooid;
	class = self->class;
	/* flush each fetched data level (including level 0) */
	for (; class != nil && (classp = OOpclass(class)); class = classp->super)
	{
		register OOID	ret;
		if (BTtestF(classp->level, self->data.levelbits))
		{
    		    /* lint truncation warning if size of ptr > OOID, 
		       but code valid */
		    ret = (OOID)OOsndSelf(self, ii_flush, class);
		    self->data.inDatabase = wasInDatabase;
		    if (ret == nil)
			return nil;
		}
	}
	self->data.inDatabase = TRUE;
	return self->ooid;
}
/*
**	OOcheckRef
**		resolve a reference attribute
**
** History:
**	11/10/89 (terry) - Modified to report error if attribute reference
**		not found.
*/
OOID
OOcheckRef(self, ref, off)
register OO_OBJECT	*self;
register OOID	ref;
register i4	off;
{
	OOID		fetchRef();
	register char	*p = (char *) self;
	OOID		result;
	OO_CLASS	*classp = (OO_CLASS*)OOp(self->class);
#ifdef DDEBUG
	D(ERx("OOcheckRef(x%p (%d), %d) data = x%p"), self, self->ooid, off, self->data);
#endif /* DDEBUG */
	if (!BTtestF(classp->level, self->data.levelbits))
		OOsndSelf(self, _fetch, self->class, 0);
	MEcopy((PTR)(p + off), (u_i2)sizeof(i4), (PTR)&result);
#ifdef DDEBUG
	D(ERx("\tresult = x%x"));
#endif /* DDEBUG */
	if ( result == 0 )
		return nil;
	else if ( result == OC_UNDEFINED )
	{ /* fetch the collection from the database */
		i4	tmp = ref;
		IIUGerr(E_OO0044_BadRef, UG_ERR_ERROR, 1, (PTR)&tmp);
		return nil;
	}
	else
		return result;
}
  OOID
  fetchRef(self, ref)
  register OO_OBJECT *self;
   register	OOID	ref;
  {
	register i4	i;
	register OOID	newcol;
    register OO_REFERENCE *refp = (OO_REFERENCE *) OOp(ref);
    i4 ooid;
    i4 cnt;
    char *table_name;
    OO_CLASS *detail;
	i4	dml = IIUIdml();
	PTR	target_addr[10];
	char	qual[100];
	detail = OOpclass(refp->detail);
	table_name = detail->table_name;
	if (dml == UI_DML_QUEL)
	{
/* # line 1305 "memory.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(cnt=count(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,detail->surrogate);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,refp->connector);
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
/* # line 1309 "memory.qsc" */	/* host code */
	}
	else
	{
	    target_addr[0] = (PTR)&ooid;
	    IIUIcao_catOpen(table_name,detail->surrogate, target_addr,
	    		STprintf(qual, "%s = %d",refp->connector, self->ooid),
			(char *) NULL
	    );
	    for( cnt = 0; IIUIcaf_catFetch() == OK; )
		    cnt++;
	    IIUIcac_catClose();
	}
        /* lint truncation warning if size of ptr > OOID, but code valid */
	newcol = (OOID)OOsnd(OC_COLLECTION, _new, UNKNOWN, 0, cnt);
	if ( cnt != 0 )
	{
	    i = 0;
	    if (UI_DML_QUEL)
	    {
/* # line 1328 "memory.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(ooid=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,detail->surrogate);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".");
      IIwritio(1,(short *)0,1,32,0,refp->connector);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&ooid);
        if (IIerrtest() != 0) goto IIrtB2;
        {
/* # line 1332 "memory.qsc" */	/* host code */
			OOsnd(newcol, _atPut, i, ooid);
			if (i++ >= cnt) {
			    char buf[80];
			    STprintf(buf, ERx("%s (%d)"), 
					self->name, self->ooid);
			    tooMany(refp->name, buf);
/* # line 1338 "memory.qsc" */	/* endretrieve */
          {
            IIbreak();
            if (1) goto IIrtE2;
          }
/* # line 1339 "memory.qsc" */	/* host code */
			}
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 1341 "memory.qsc" */	/* host code */
	    }
	    else
	    {
		IIUIcao_catOpen(table_name,detail->surrogate, target_addr,
				qual,(char *) NULL);
		while (IIUIcaf_catFetch() == OK)
		{
			OOsnd(newcol, _atPut, i, ooid);
			if (i++ >= cnt) 
			{
			    char buf[80];
			    STprintf(buf, ERx("%s (%d)"), 
					self->name, self->ooid);
			    tooMany(refp->name, buf);
			    break;
			}
		}
		IIUIcac_catClose();
	    }
	    if ( i > cnt || iiooInqResult(_Retrieve, detail->table_name) != OK )
		return nil;
	    if (i < cnt) {
		IIUGerr( E_OO0042_too_few, UG_ERR_ERROR, 0);
		return nil;
	    }
	}
	return newcol;
  }
OOID
masterOff(class, off)
register OOID	class;
register i4	off;
{
	register OO_CLASS	*classp;
	register OO_COLLECTION	*m;
	register OO_REFERENCE	*ref;
	register i4	i;
	if (class == nil)
		return nil;
	classp = (OO_CLASS*) OOp(class);
	if ( !OOsnd(classp->masterRefs, _isEmpty, 0) )
	    m = (OO_COLLECTION*) OOp(classp->masterRefs);
	    for (i = 0; i < m->size; i++) {
		    ref = (OO_REFERENCE*) OOp(m->array[i]);
		    if (ref->moffset == off)
			return ref->ooid;
	    }
	return masterOff(classp->super,off);
}
static char	_iihiststat[] = ERx("iihiststat");
static char	_iicurstat[] = ERx("iicurstat");
  putstatus(self, meth)
  OO_OBJECT *self;
  OO_METHOD *meth;
  {
	iiuicw1_CatWriteOn();
/* # line 1403 "memory.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"append _iihiststat(_iicurstat.all)where _iicurstat.id=");
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIsyncup((char *)0,0);
    }
/* # line 1405 "memory.qsc" */	/* host code */
	_VOID_ iiooInqResult(_Append, _iihiststat);
/* # line 1406 "memory.qsc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace _iicurstat(user=");
      IIputdomio((short *)0,1,32,0,self->owner);
      IIwritio(0,(short *)0,1,32,0,(char *)", method=");
      IIputdomio((short *)0,1,30,4,&meth->ooid);
      IIwritio(0,(short *)0,1,32,0,(char *)", date=date(");
      IIputdomio((short *)0,1,32,0,ERx("now"));
      IIwritio(0,(short *)0,1,32,0,(char *)
"), revision=c.revision+1, remarks=");
      IIputdomio((short *)0,1,32,0,meth->name);
      IIwritio(0,(short *)0,1,32,0,(char *)")where _iicurstat.id=");
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIsyncup((char *)0,0);
    }
/* # line 1413 "memory.qsc" */	/* host code */
	iiuicw0_CatWriteOff();
	_VOID_ iiooInqResult(_Replace, _iicurstat);
  }
/* VARARGS1 */
err(msgid, a1, a2, a3, a4, a5, a6)
ER_MSGID	msgid;
char	*a1, *a2, *a3, *a4, *a5, *a6;
{
	IIUGerr(msgid, 0, 6, a1, a2, a3, a4, a5, a6);
}
OOID
OOextend(self, oldsize, newsize)
OO_OBJECT	*self;
register i4	oldsize, newsize;
{
	OO_OBJECT	*newp;
	STATUS		stat;
	if ((newp = (OO_OBJECT *)MEreqmem((u_i4)0, (u_i4)newsize, FALSE,
	    &stat)) == NULL)
	{
		IIUGerr( E_OO001A_Error_from_MEreqmem, UG_ERR_ERROR, 1, &stat );
		return nil;
	}
	MEmove((u_i2)oldsize, (PTR)self, (char)0, (u_i2)newsize, (PTR)newp);
	_VOID_ OOhash(newp->ooid, newp);
	return newp->ooid;
}
OOID
detailref(dr, f)
register OOID	dr;
register OO_ATTRIB	*f;
{
	register OO_COLLECTION	*c;
	register OO_REFERENCE	*d;
	register i4		i;
	if (dr != nil)
	{
	    c = (OO_COLLECTION*) OOp(dr);
	    for (i = 0; i < c->size; i++) {
		OOsnd(c->array[i], _fetchAll, 0);
		d = (OO_REFERENCE*) OOp(c->array[i]);
		if ( STequal(f->name, d->connector) )
		{
			return d->ooid;
		}
	    }
	}
	return nil;
}
tooMany(which, name)
char	*which;
char *name;
{
	IIUGerr( E_OO001B_Too_many_retrieved, UG_ERR_ERROR, 2, which, name );
}
tooFew(which, name)
char	*which;
char	*name;
{
	IIUGerr( E_OO001C_Too_few_retrieved, UG_ERR_ERROR, 2, which, name );
}
OOID
OOalter_date(id, alter_date)
  OOID id;
  char *alter_date;
{
	OO_OBJECT	*self = OOp(id);
	*alter_date = EOS;
/* # line 1433 "memory.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"memory4",6315,17306);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"memory4",6315,17306);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select alter_date from ii_objects where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"memory4",6315,17306);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,alter_date);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1437 "memory.sc" */	/* host code */
	if ( iiooInqResult(_Retrieve, _ii_objects) != OK )
	{
	    return (nil);
	}
	self->alter_date = alter_date;
	return (id);
}
/*{
** Name:	OOrd_seqText() - read sequenced text from DB
**
** Description:
**
** Input params:
**	OOID	id;
**	char	buffer[];	at least OOLONGREMSIZE+1 chars
**	char	*table;
**	char	*colm;
**
** Output params:
**	char	buffer[];	buffer is filled in
**
** History:
*/
OOID
OOrd_seqText(id, buffer, table, colm)
  OOID id;
  char *buffer;
char	*table, *colm;
{
	i4	ret_code;
  i4 seqno;
	PTR		target_addr[2];
	char		target_list[FE_MAXNAME*2 + 3];
	*buffer = EOS;
	if ( OOisTempId(id) || ( OOhash(id, (OO_OBJECT *)NULL) != 0 &&
					!(OOp(id))->data.inDatabase ) )
	{ /* object isn't in database so no need to read */
		return id;
	}
	/*
	** If the string to be fetched is in the ii_longremarks
	** catalog (99% of the calls to this routine), then
	** use a hard-coded SELECT statement.
	*/
	if ( table == _ii_longremarks || STequal(table, _ii_longremarks) )
	{
/* # line 1487 "memory.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"memory5",6316,17426);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"memory5",6316,17426);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select remark_sequence, long_remark from ii_longremarks where object_\
id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"memory5",6316,17426);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&seqno);
      IIgetdomio((short *)0,1,32,0,buffer);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 1492 "memory.sc" */	/* host code */
		    	/* simple-field implementation for now */
			if (seqno > 0)
			{
				break;
			}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	}
	else /* some other sequenced text table */
	{
		char	qual[FE_MAXNAME*4];
		target_addr[0] = (PTR) &seqno;
		target_addr[1] = (PTR) buffer;
		STcopy(ERx("encode_sequence, "),target_list);
		STcat(target_list, colm);
		if ( IIUIcao_catOpen(table, target_list, target_addr,
					STprintf(qual, ERx("object_id = %d"), 
					id), (char *)NULL) != OK )
		{
			IIUIcac_catClose();
			return (nil);
		}
		while ((ret_code = IIUIcaf_catFetch()) != E_UI0002_EOS)
		{
	    		/* simple-field implementation for now */
			if (ret_code != OK || seqno > 0)
			{
				break;
			}
		}
		IIUIcac_catClose();
	}
	if ( iiooInqResult(_Retrieve, table) != OK )
	{
		return (nil);
	}
	return (id);
}
/*{
** Name:	OOwr_seqText() - write sequenced text to DB
**
** Description:
**
** Input params:
**	OOID	id;
**	char	buffer[];	at least OOLONGREMSIZE+1 chars
**	char	*table;
**	char	*colm;
**
** Side Effects:
**	Database is updated.
**
** History:
**	12-nov-1987 (danielt) SQLized the database access
**	27-ock-1988 (marian)
**		Removed else statement and changed else if to else.  This
**		routine will only be called against ii_longremarks and
**		ii_encodings.
*/
static OOID
OOwr_seqText ( id, buffer, table, colm )
  OOID id;
  char *buffer;
char	*table;
char	*colm;
{
  char sql_buf[FE_MAXNAME*2 + 20];
	bool  	is_longremarks = FALSE;
	if ( table == _ii_longremarks || STequal(table, _ii_longremarks) )
	{
		if (STlength(buffer) >= OOLONGREMSIZE)
		{
			buffer[OOLONGREMSIZE] = EOS;
		}
		is_longremarks = TRUE;
	}
	if (OOisTempId(id))
	{
	    IIUGerr( E_OO001D_Cant_write_sequenced, UG_ERR_ERROR, 0 );
	    return nil;
	}
	if (buffer == NULL)
	    return id;
	if (is_longremarks)
	{
/* # line 1581 "memory.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 1582 "memory.sc" */	/* host code */
	}
	else 
	{
/* # line 1586 "memory.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 1587 "memory.sc" */	/* host code */
	}
	if ( iiooInqResult(iiDelete, table) != OK )
	{
		return nil;
	}
	if (is_longremarks)
	{
/* # line 1594 "memory.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"memory6",6317,14638);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,32,0,buffer);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"memory6",6317,14638);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_longremarks(object_id, remark_sequence, long_remark, r\
emark_language)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
        IIexDefine(0,(char *)"memory6",6317,14638);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1598 "memory.sc" */	/* host code */
	}
	else 
	{
/* # line 1601 "memory.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"memory7",6318,18062);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,32,0,buffer);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"memory7",6318,18062);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_encodings(object_id, object, encode_sequence, encode_e\
string)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"memory7",6318,18062);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1605 "memory.sc" */	/* host code */
	}
	if ( iiooInqResult(_Append, table) != OK )
	{
		return nil;
	}
	return id;
}
