# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<me.h>
# include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<cf.h>
# include	<ug.h>
# include   <uigdata.h>
# include	<erfe.h>
# include	"ercf.h"
# include <ooclass.h>
FUNC_EXTERN	char	*UGcat_now();
/**
** Name:	cfaddobj.sc - add a form-related object to the database.
**
** Description:
**	These procedures are used to add forms, qbfnames, and joindefs to
**	the database, using oo_save and the catalogs specific to the objects
**	being added.  This file defines:
**
**	cf_addobj	add an object to the object catalog(s).
**	cf_adddet	add the definition of the object to its appropriate
**			catalog(s).
**
** History:
**	28-Apr-87 (rdesmond) - written.
**	03-nov-87 (danielt) - changed all QUEL to a mixture of conversion
**				routines (to eliminate INGRES function calls
**				in database statements) and vanilla SQL.
**	21-apr-88 (sylviap)
**		Added SQL COMMIT after OO call to release the locks on the
**		system catalogs.
**	09/24/88 (dkh) - Fixed to correctly copy the qtype for a joindef
**	14-oct-88 (danielt)
**		removed SQL COMMIT
**	08-dec-88 (marian)
**		Extended catalog column name changes.
**	04/08/91 (dkh) - Fixed bug 36573.  More bullet proofing for
**			 corrupted copyform files.
**	08-jun-92 (leighb) DeskTop Porting Change:
**		Changed 'errno' to 'errnum' cuz 'errno' is a macro in WIN/NT.
**	08/31/93 (dkh) - Fixed up compiler complaints due to prototyping.
**      11-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's change (from 6.4) for axp_osf
**              10/15/93 (kchin)
**              Cast argument to PTR when calling IIOOidWrite(), this
**              is due to the change of datatype of variable parameters
**              to PTR in IIOOidWrite().  The change hereis made in
**              cf_addobj().
**	12-mar-1996 (angusm)
**		Change cf_adddet() to reset owner name for joindef detail
**		records. (bug 74841)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*{
** Name:	cf_addobj - add a form-related object to the object catalog(s).
**
** Description:
**	For the object whose object catalog attributes are passed in, delete
**	an object with the same class, name and owner, if it exists, and
**	enter the description of the new object in the object catalog(s)
**	(currently ii_objects and ii_longremarks).  Return the id of the
**	new object.
**
** Input params:
**	class		object class of the object add (OOID).
**	name		name of the object.
**	usercode	usercode of the object.
**	short_remark	short remark for the object.
**	long_remark	long remark for the object.
**	curr_id		pointer to i4 in which to return the id.
**			may be set to NULL if id is not desired.
**
** Output params:
**	curr_id		pointer to i4 in which to return the id.
**
** Returns:
**	OK		added object successfully.
**	FAIL		failed to add object successfully.
**
** Exceptions:
**	none.
**
** Side Effects:
**
** History:
**	28-Apr-87 (rdesmond) - written.
**      15-mar-88 (danielt) - added calls to UGcat_now instead of "now".
**
*/
STATUS
cf_addobj(class, name, usercode, short_remark, long_remark, curr_id)
i4	class;
char	*name;
char	*usercode;
char	*short_remark;
char	*long_remark;
i4	*curr_id;
{
	i4	id = -1;
	char	*date_ptr = STalloc(UGcat_now());
	/* If object exists, it must be deleted first */
	if (cf_objexist(class, name, usercode, &id))
		cf_delobj(class, id);
	/* Save object in object catalog and get new id */
	id = -1;
	if ( IIOOidWrite(class, &id, name, (PTR)0, usercode, (PTR)0, short_remark,
				date_ptr, date_ptr, long_remark) != OK )
	{
		return(FAIL);
	}
	else
	{
		/* set id for return */
		if (curr_id != (i4 *)NULL)
			*curr_id = id;
		return(OK);
	}
}
/*{
** Name:	cf_adddet - add one tuple of the definition of a form-related
**				object to the database.
**
** Description:
**	Given a record containing tab separated fields with the values for one
**	tuple of a catalog for a form-related object, given the record type,
**	and given the object id, insert the tuple into the appropriate catalog.
**	The fields of the record are character fields.	Numeric entities are
**	represented in ASCII format.  The fields of each record are
**	positionally identified.
**
** Input params:
**	rec_type	the record type of the input record, as follows:
**				RT_QBFDET	ii_qbfnames record.
**				RT_JDEFDET	ii_joindefs record.
**				RT_FORMDET	ii_forms record.
**				RT_FIELDDET	ii_fields record.
**				RT_TRIMDET	ii_trim record.
**	inbuf		a pointer to the input record.
**	curr_id		the id of the object to be added.
**
** Output params:
**	none.
**
** Returns:
**	OK		added tuple successfully.
**	FAIL		failed to add tuple successfully.
**
** Exceptions:
**	none.
**
** Side Effects:
**
** History:
**	28-Apr-87 (rdesmond) - written.
**	03-nov-87 (danielt) - changed all QUEL to a mixture of conversion
**				routines (to eliminate INGRES function calls
**				in database statements) and vanilla SQL.
**	05-Nov-87 (rdesmond) - Changed token[] definition to 27 elements.
**	12/23/89 (dkh) - Added support for owner.table.
**	12-mar-1996 (angusm)
**		bug 74841: joindefs copied in as another owner
**		need to have field qinfo5 reset, else they are unusable:
**		joindef 'master' record has new owner, but 'detail' records
**		preserve original owner.
*/
STATUS
cf_adddet(rec_type, inbuf, curr_id)
i4	rec_type;
char	*inbuf;
  i4 curr_id;
{
  char *token[40];
  i4 i4array[40];
  i4 errnum;
  i2 qinf5_null;
  i2 relown_null;
	ER_MSGID	type_num;
	STATUS		retstat;
	register i4	i;
	/*
	**  Clear out the token and i4array so we can support
	**  6.4 versions of the ii_qbfnames and ii_joindefs
	**  catalogs.
	*/
	if (rec_type == RT_QBFDET || rec_type == RT_JDEFDET)
	{
		MEfill(sizeof(token), '\0', token);
		MEfill(sizeof(i4array), '\0', i4array);
	}
	/* parse input string */
	_VOID_ cf_gettok(inbuf);
	for( i = 0 ; (token[i] = cf_gettok((char *)NULL)) != NULL ; ++i)
		;
	retstat = OK;
	if (rec_type == RT_FORMDET && i != FORMDET_COUNT)
	{
		type_num = FE_form;
		retstat = FAIL;
	}
	else if (rec_type == RT_FIELDDET && i != FIELDDET_COUNT)
	{
		type_num = FE_Field;
		retstat = FAIL;
	}
	else if (rec_type == RT_TRIMDET && i != TRIMDET_COUNT)
	{
		type_num = FE_Trim;
		retstat = FAIL;
	}
	if (retstat != OK)
	{
		IIUGerr(E_CF0026_CORRUPTED_FILE, UG_ERR_FATAL, 1,
			ERget(type_num));
		return(retstat);
	}
	iiuicw1_CatWriteOn();
	switch ( rec_type )
	{
		case RT_QBFDET:
			if ( token[2] != NULL )
				IIUGatoi(token[2], (PTR)&i4array[2], sizeof(i4));
			else
				i4array[2] = 0;
			/*
			**  If token[3] is NULL, then either we are
			**  converting from a pre-6.4 version or
			**  there was no "relowner" value copied out.
			**  In either case, we set the null indicator
			**  so a NULL value is placed into the "relowner"
			**  column of the ii_qbfnames catalog.
			*/
			relown_null =
			    (token[3] == NULL || *(token[3]) == EOS) ? -1 : 0;
/* # line 251 "cfaddobj.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_qbfnames(object_id, relname, frname, qbftype, relowner\
)values(");
    IIputdomio((short *)0,1,30,4,&curr_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[0]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[1]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[2]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio(&relown_null,1,32,0,token[3]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 256 "cfaddobj.sc" */	/* host code */
			break;
		case RT_JDEFDET:
			if (token[2] != NULL)
			{
				IIUGatoi(token[0], (PTR)&i4array[0],sizeof(i4));
			}
			/*
			**  If token[5] is NULL, then either we are
			**  converting from a pre-6.4 version or
			**  there was no "qinfo5" value copied out.
			**  In either case, we set the null indicator
			**  so a NULL value is placed into the "qinfo5"
			**  column of the ii_joindefs catalog.
			*/
			qinf5_null =
			    (token[5] == NULL || *(token[5]) == EOS) ? -1 : 0;
			/*
			** reset owner name in qinfo5 field if joindef
			** being copied in under another user id
			*/
			if ( (token[5] != NULL) && (STcompare(token[5], IIuiUser) != 0) )
				 STcopy(IIuiUser, token[5]);
/* # line 278 "cfaddobj.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_joindefs(object_id, qtype, qinfo1, qinfo2, qinfo3, qin\
fo4, qinfo5)values(");
    IIputdomio((short *)0,1,30,4,&curr_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[0]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[1]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[2]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[3]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[4]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio(&qinf5_null,1,32,0,token[5]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 284 "cfaddobj.sc" */	/* host code */
			break;
		case RT_FORMDET:
			IIUGatoi(token[0],(PTR) &i4array[0],sizeof(i4));
			IIUGatoi(token[1],(PTR) &i4array[1],sizeof(i4));
			IIUGatoi(token[2],(PTR) &i4array[2],sizeof(i4));
			IIUGatoi(token[3],(PTR) &i4array[3],sizeof(i4));
			IIUGatoi(token[4],(PTR) &i4array[4],sizeof(i4));
			IIUGatoi(token[5],(PTR) &i4array[5],sizeof(i4));
			IIUGatoi(token[6],(PTR) &i4array[6],sizeof(i4));
			IIUGatoi(token[7],(PTR) &i4array[7],sizeof(i4));
			IIUGatoi(token[8],(PTR) &i4array[8],sizeof(i4));
			IIUGatoi(token[9],(PTR) &i4array[9],sizeof(i4));
			IIUGatoi(token[10],(PTR) &i4array[10],sizeof(i4));
			IIUGatoi(token[11],(PTR) &i4array[11],sizeof(i4));
			IIUGatoi(token[12],(PTR) &i4array[12],sizeof(i4));
			IIUGatoi(token[13],(PTR) &i4array[13],sizeof(i4));
			IIUGatoi(token[14],(PTR) &i4array[14],sizeof(i4));
			IIUGatoi(token[15],(PTR) &i4array[15],sizeof(i4));
/* # line 302 "cfaddobj.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_forms(object_id, frmaxcol, frmaxlin, frposx, frposy, f\
rfldno, frnsno, frtrimno, frversion, frscrtype, frscrmaxx, frscrmaxy, \
frscrdpix, frscrdpiy, frflags, fr2flags, frtotflds)values(");
    IIputdomio((short *)0,1,30,4,&curr_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[0]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[1]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[2]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[3]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[4]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[5]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[6]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[7]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[8]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[9]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[10]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[11]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[12]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[13]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[14]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[15]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 338 "cfaddobj.sc" */	/* host code */
			break;
		case RT_FIELDDET:
  			IIUGatoi(token[0],(PTR) &i4array[0],sizeof(i4));
  			IIUGatoi(token[2],(PTR) &i4array[2],sizeof(i4));
  			IIUGatoi(token[3],(PTR) &i4array[3],sizeof(i4));
  			IIUGatoi(token[4],(PTR) &i4array[4],sizeof(i4));
  			IIUGatoi(token[5],(PTR) &i4array[5],sizeof(i4));
  			IIUGatoi(token[6],(PTR) &i4array[6],sizeof(i4));
  			IIUGatoi(token[7],(PTR) &i4array[7],sizeof(i4));
  			IIUGatoi(token[8],(PTR) &i4array[8],sizeof(i4));
  			IIUGatoi(token[9],(PTR) &i4array[9],sizeof(i4));
  			IIUGatoi(token[10],(PTR) &i4array[10],sizeof(i4));
  			IIUGatoi(token[11],(PTR) &i4array[11],sizeof(i4));
  			IIUGatoi(token[12],(PTR) &i4array[12],sizeof(i4));
  			IIUGatoi(token[14],(PTR) &i4array[14],sizeof(i4));
  			IIUGatoi(token[15],(PTR) &i4array[15],sizeof(i4));
  			IIUGatoi(token[16],(PTR) &i4array[16],sizeof(i4));
  			IIUGatoi(token[17],(PTR) &i4array[17],sizeof(i4));
  			IIUGatoi(token[18],(PTR) &i4array[18],sizeof(i4));
  			IIUGatoi(token[19],(PTR) &i4array[19],sizeof(i4));
  			IIUGatoi(token[20],(PTR) &i4array[20],sizeof(i4));
  			IIUGatoi(token[25],(PTR) &i4array[25],sizeof(i4));
  			IIUGatoi(token[26],(PTR) &i4array[26],sizeof(i4));
/* # line 361 "cfaddobj.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_fields(object_id, flseq, fldname, fldatatype, fllength\
, flprec, flwidth, flmaxlin, flmaxcol, flposy, flposx, fldatawidth, fl\
datalin, fldatacol, fltitle, fltitcol, fltitlin, flintrp, fldflags, fl\
d2flags, fldfont, fldptsz, fldefault, flformat");
    IIwritio(0,(short *)0,1,32,0,(char *)
", flvalmsg, flvalchk, fltype, flsubseq)values(");
    IIputdomio((short *)0,1,30,4,&curr_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[0]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[1]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[2]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[3]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[4]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[5]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[6]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[7]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[8]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[9]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[10]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[11]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[12]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[13]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[14]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[15]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[16]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[17]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[18]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[19]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[20]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[21]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[22]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[23]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[24]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[25]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[26]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 419 "cfaddobj.sc" */	/* host code */
			break;
		case RT_TRIMDET:
  			IIUGatoi(token[0],(PTR) &i4array[0],sizeof(i4));
  			IIUGatoi(token[1],(PTR) &i4array[1],sizeof(i4));
  			IIUGatoi(token[3], (PTR) &i4array[3],sizeof(i4));
  			IIUGatoi(token[4], (PTR) &i4array[4],sizeof(i4));
  			IIUGatoi(token[5], (PTR) &i4array[5],sizeof(i4));
  			IIUGatoi(token[6], (PTR) &i4array[6],sizeof(i4));
/* # line 427 "cfaddobj.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_trim(object_id, trim_col, trim_lin, trim_trim, trim_fl\
ags, trim2_flags, trim_font, trim_ptsz)values(");
    IIputdomio((short *)0,1,30,4,&curr_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[0]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[1]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,token[2]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[3]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[4]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[5]);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i4array[6]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 439 "cfaddobj.sc" */	/* host code */
			break;
	} /* end switch */
	iiuicw0_CatWriteOff();
/* # line 442 "cfaddobj.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 443 "cfaddobj.sc" */	/* host code */
	return (errnum > 0) ? FAIL : OK;
}
