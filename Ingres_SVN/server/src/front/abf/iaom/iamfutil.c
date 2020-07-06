# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1989, 2004 Ingres Corporation
*/
# include <compat.h>
# include <cv.h>		/* 6-x_PC_80x86 */
# include <gl.h>
# include <sl.h>
# include <iicommon.h>
# include <fe.h>
# include <er.h>
# include <cm.h>
# include <st.h>
# include <ug.h>
# include <ooclass.h>
# include <oocat.h>
# include	<uigdata.h>
# include "eram.h"
# include <abfdbcat.h>
/**
** Name:	iamfutil.c -	Forms utilities
**
** Description:
**
**	Forms catalog utilities called from abf, mainly for VISION use.
**
**		IIAMgoiGetObjInfo
**		IIAMfoiFormInfo
**		IIAMforFormRead
**		IIAMufnUniqFormName
**		IIAMggdGetGenDate
**/
static char	*uniq_sub();
/*{
** Name - IIAMgoiGetObjInfo	- get information about an object
**
** Description:
**	Return the date and id associated with a object, and an empty string
**	and OC_UNDEFINED if the object doesn't exist.  
**	OK is returned  for non-existent object.
**	failure is returned only if DB access fails.
**
** Inputs:
**	name - object name
**	class - object class
**	dbuf - char buffer to hold date string
**	ooid_p - pointer to OOID.
**
** Outputs:
**	dbuf - filled in date, or empty string.
**	ooid_p - pointer to OOID, or OC_UNDEFINED.
**
** Returns:
**	STATUS
**
** History:
**	7/89 (bobm)	written
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
STATUS
IIAMgoiGetObjInfo(name, class, dbuf, ooid_p)
  char *name;
  OOID class;
char	*dbuf;
OOID	*ooid_p;
{
  char ad[OODATESIZE+1];
  char cd[OODATESIZE+1];
  char own[FE_MAXNAME+1];
  OOID id;
	OOID	frid = OC_UNDEFINED;
	bool	found = FALSE;
	i4	seq = 0;
	*dbuf = EOS;
	*ooid_p = OC_UNDEFINED;
/* # line 94 "iamfutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamfutil1",6609,9387);
      IIputdomio((short *)0,1,30,4,&class);
      IIputdomio((short *)0,1,32,0,name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamfutil1",6609,9387);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select alter_date, create_date, object_owner, object_id from ii_objec\
ts where object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&class);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,name);
        IIexDefine(0,(char *)"iamfutil1",6609,9387);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,ad);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,cd);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,own);
      IIgetdomio((short *)0,1,30,4,&id);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 100 "iamfutil.sc" */	/* host code */
	{
		if ( STtrmwhite(own) > 0)
		{
			++seq;
			/*
			** if we have an empty alter date, use the create date
			*/
			if (ad[0] == EOS)
				STcopy(cd, ad);
			if (STequal(own, IIUIdbdata()->user))
			{
				STcopy(ad, dbuf);
				frid = id;
				found = TRUE;
/* # line 115 "iamfutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 116 "iamfutil.sc" */	/* host code */
			}
			else if (STequal(own, IIUIdbdata()->dba))
			{
				frid = id;
				found = TRUE;
				STcopy(ad, dbuf);
			}
			else if ( frid == OC_UNDEFINED )
			{
				frid = id;
			}
		}
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if ( FEinqerr() != OK )
		return FAIL;
	if ( !found && seq > 1 )
		frid = OC_UNDEFINED;
	*ooid_p = frid;
	return OK;
}
/*{
** Name:	IIAMfoiFormInfo - get information about a form.
**
** Description:
**	Instance of IIAMgoiGetObjInfo.  See description above.
**
*/
STATUS
IIAMfoiFormInfo(fname, dbuf, ooid_p)
char    *fname;
char    *dbuf;
OOID    *ooid_p;
{
	return IIAMgoiGetObjInfo(fname, OC_FORM, dbuf, ooid_p);
}
/*{
** Name - IIAMforFormRead	- Read forms fields.
**
** Description:
**	Reads the fields of a given form and calls a function for action.
**
** Inputs:
**	fname - the form name.
**	frid - form id.
**	p    - PTR, for use of client.  We just pass it along.
**	func - function to call on each field.
**
** Returns:
**	STATUS
**
** History:
**	1/90 (billc)	written
*/
STATUS
IIAMforFormRead (fname, frid, p, func)
char	*fname;
  OOID frid;
PTR	p;
STATUS	(*func)();
{
  char nbuf[FE_MAXNAME+1];
  i4 length;
  i2 type;
  i2 scale;
  i4 seq;
  i4 kind;
	STATUS	stat = OK;
	if (frid == OC_UNDEFINED)
		return FAIL;
	/*
	** Step through the form retrieving all the fields
	*/
/* # line 202 "iamfutil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select fldname, fldatatype, fllength, flprec, fltype, flsubseq from i\
i_fields where object_id=");
    IIputdomio((short *)0,1,30,4,&frid);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by flsubseq");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nbuf);
      IIgetdomio((short *)0,1,30,2,&type);
      IIgetdomio((short *)0,1,30,4,&length);
      IIgetdomio((short *)0,1,30,2,&scale);
      IIgetdomio((short *)0,1,30,4,&kind);
      IIgetdomio((short *)0,1,30,4,&seq);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 209 "iamfutil.sc" */	/* host code */
	{
		DB_DATA_VALUE   dbv;
		STtrmwhite(nbuf);
		dbv.db_datatype = type;
		dbv.db_length = length;
		dbv.db_data = NULL;
		dbv.db_prec = scale;
		if ((stat = (*func)(fname, nbuf, p, kind, &dbv)) != OK)
/* # line 219 "iamfutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 220 "iamfutil.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	return stat != OK ? stat : FEinqerr();
}
/*{
** Name - IIAMufnUniqFormName
**
** Description:
**	Return a unique name for a form to be attached to a metaframe
**	type.  We make sure the name is unique with reference to all
**	OC_FORM's and OC_AFORMREF's in the database.  The latter is
**	important because VISION may not actually generate the form for
**	a long time, but abf will put an AFORMREF record in the DB
**	shortly after generating the name.
**
** Inputs:
**	name - frame name for a starting point
**	tag - storage tag for generated name
**
** Returns:
**	pointer to name, or NULL for failure.
**
** History:
**	7/89 (bobm)	written
*/
char *
IIAMufnUniqFormName(name,tag)
char *name;
TAGID tag;
{
	char *nm;
	nm = uniq_sub(name,tag,1);
	if (nm == NULL)
		IIUGerr(E_AM0027_Form_name);
	return nm;
}
/*
** The real work for name generation.  What is basically going on here:
**
**	If the name all by itself works, simply use it.  Otherwise
**	we make numeric suffixes of length ndig (numbers with fewer
**	than ndig digits may appear, zero padded to ndig).  If all the
**	purely numeric suffixes of this length are taken, we bump
**	ndig by one and recurse.  Yes, this is a tail recursion which
**	we could unwind, but this makes it easy to distinguish not
**	having an empty slot at a given number of digits from failure.
**	Also, since this involves DB access, the price for a recursive
**	call is pretty well swamped.  Will never go very many levels,
**	so I don't care about the stack buffers either.  We cut off
**	at 4 levels so that we'll be OK with 16 bit nat's.  This gives
**	us 11111 unique names for a given frame name, which ought to
**	be enough.
**
**	The scheme:
**
**		<name> <name>0 .... <name>9	ndig = 1
**		<name>00 .... <name>99		ndig = 2
**		<name>000 .... <name>999	ndig = 3
*/
static char *
uniq_sub(name,tag,ndig)
char *name;
TAGID tag;
i4  ndig;
{
  char nbuf[FE_MAXNAME+1];
  char pat[FE_MAXNAME+2];
	char	ntrunc[FE_MAXNAME+1];
	i4	lastnum;
	i4	hinum;
	i4	num;
	char	*suffix;
	bool	name_ok;
	if (ndig > 4)
		return NULL;
	/*
	** this is for safety.  VISION actually generates short
	** frame names, so ntrunc should = name.
	*/
	STcopy(name,ntrunc);
	ntrunc[FE_MAXNAME-ndig] = EOS;
	/*
	** largest number that can fit in our suffix
	*/
	STcopy(ERx("9999"),nbuf);
	nbuf[ndig] = EOS;
	_VOID_ CVan(nbuf,&hinum);
	STprintf(pat,ERx("%s%%"),ntrunc);
	suffix = nbuf + STlength(ntrunc);
	name_ok = TRUE;
	lastnum = -1;
	/*
	** we use the "order by" to assure that the name by itself
	** comes first, and that the type of suffixes we generate
	** (ndig digit numbers with leading 0's) come out sorted in relation
	** to each other.  This allows us to quit as soon as we find
	** an empty gap.
	**
	** lastnum starts out -1, so that if no ndig-length pure numerics
	** exist, we use 0.  If we finish the select loop with
	** lastnum = hinum, all the possible suffixes are used, and we need
	** to go to more digits.
	*/
/* # line 338 "iamfutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamfutil2",6610,6124);
      IIputdomio((short *)0,1,32,0,pat);
      IIputdomio((short *)0,1,30,sizeof(OC_FORM),(void *)IILQint(OC_FORM));
      IIputdomio((short *)0,1,30,sizeof(OC_AFORMREF),(void *)IILQint(
      OC_AFORMREF));
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamfutil2",6610,6124);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_name from ii_objects where object_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,pat);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(OC_FORM),(void *)IILQint(OC_FORM));
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(OC_AFORMREF),(void *)IILQint(
        OC_AFORMREF));
        IIwritio(0,(short *)0,1,32,0,(char *)")order by object_name");
        IIexDefine(0,(char *)"iamfutil2",6610,6124);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nbuf);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 345 "iamfutil.sc" */	/* host code */
	{
		if (*suffix == EOS)
			name_ok = FALSE;
		else
		{
			if (name_ok)
			{
/* # line 352 "iamfutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 353 "iamfutil.sc" */	/* host code */
			}
			if (STlength(suffix) == ndig && CVan(suffix,&num) == OK)
			{
				/* quit if we found a gap */
				if ((num - lastnum) > 1)
				{
/* # line 359 "iamfutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 360 "iamfutil.sc" */	/* host code */
				}
				lastnum = num;
			}
		}
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	if ( FEinqerr() != OK )
		return NULL;
	if (name_ok)
		return STtalloc(tag,ntrunc);
	if (lastnum >= hinum)
		return uniq_sub(name,tag,ndig+1);
	++lastnum;
	STprintf(pat,ERx("%%s%%0%dd"),ndig);
	STprintf(nbuf,pat,ntrunc,lastnum);
	return STtalloc(tag,nbuf);
}
/*
** Name - IIAMggdGetGenDate
**
** Description:
**	Get the current database value of the generation date of an objects
**	source file from the ii_abfobjects system catalog. 
**
** Inputs:
**	aid - application id of the object.
**	oid - object id.
**	gdate - Pointer to location to hold gendate value.
**
** Returns:
**	STATUS
**
** History:
**	24-nov-93 (rudyw)
**		Written to fix bug 51091. Possibility existed for using routine
**		IIAMcgCatGet (iamcget.c) instead but it had memory implications.
**		Needed to add include of abfdbcat.h to get ABFARG_SIZE.
*/
STATUS
IIAMggdGetGenDate(aid,oid,gdate)
  i4 aid;
  i4 oid;
char    *gdate;
{
  char buf[ABFARG_SIZE+1];
	STATUS rval;
	char *datepart;
/* # line 420 "iamfutil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select abf_arg6 from ii_abfobjects where applid=");
    IIputdomio((short *)0,1,30,4,&aid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_id=");
    IIputdomio((short *)0,1,30,4,&oid);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,ABFARG_SIZE+1-1,buf);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 425 "iamfutil.sc" */	/* host code */
	rval=FEinqerr();
	if ( rval != OK )
		return rval;
	/*
	** If a value has been returned and there is a date part
	**  Copy the datepart to input pointer location.
	*/
	datepart = STindex(buf,"@",ABFARG_SIZE);
	if ( datepart == NULL || *(datepart+1) == '\0' )
		return FAIL;
	datepart++;
	STcopy(datepart, gdate);
	return OK;
}
