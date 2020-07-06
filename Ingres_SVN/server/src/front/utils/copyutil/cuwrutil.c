# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<si.h>
#include	<tm.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include <ooclass.h>
#include	<oocat.h>
#include	<cu.h>
#include	"ercu.h"
# include <uigdata.h>
/**
** Name:	cuwrutil.sc	- Utility routines to write to copy file.
**
** Description:
**	Contains routines that write records to copy files.
**
**	cu_writeobj	Find an object and write a header.
**	cu_wrobj	Write an object header.
**	cu_wrcomp	Write a component header.
**	cu_wrgbf	Write a GBFSTYLE header.
**	cu_wrhdr	Write a copyfile header.
**	IICUgtLongRemark	fetch long remarks for object.
**
** History:
**	Revision 6.2  89/05  wong
**	Abstracted fetch of long remarks into 'IICUgtLongRemark()' for JupBug
**	5746.
**
**	Revision 6.1  88/11/09  marian
**	Modified column names for extended catalogs to allow them
**	to work with gateways.
**
**	Revision 6.0  87/07/26  joe
**	Initial Version.
**
**	06/17/89 (dkh) - Added declaration for parameters passed into
**			 cu_wrhdr(), as requested by PC group.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-May-2005 (kodse01)
**	    replace %ld with %d for old nat and long nat variables.
**/
char	*IICUrtnRectypeToName();
/*{
** Name:	cu_wrhdr	- Write a copy file header
**
** Description:
**	Each copy file begins with a header.  The header is of
**	the form:
**
**	name\tmajor\tminor\tdate\n
**
**	where name is a name supplied by the caller, major is the value
**	oc IICUMAJOR when the file was written. minor is the value of
**	IICUMINOR when the file was written.  date is the date the file
**	was written.
**
**	On copyin, the value of IICUMAJOR will be check againest
**	major to see if the input file needs to be converted.
**	The value of IICUMINOR will be check againest minor to
**	see if the file was written by a newer version of copy.
**
** Inputs:
**	name		The name to place in the header.
**
**	fp		the file to write to.
**
** History:
**	4-aug-19987 (Joe)
**		Initial Version
**	06/17/89 (dkh) - Added declaration for passed in parameters.
*/
char	*UGcat_now();
VOID
cu_wrhdr ( name, fp )
char	*name;
FILE	*fp;
{
	SIfprintf( fp, ERx("%s\t%d\t%d\t%s\n"),
			name, IICUMAJOR, IICUMINOR, UGcat_now()
	);
}
/*{
** Name:        cu_writeobj - write object header to file if exists
**
** Description:
**      Writes the header to the intermediate file for one object.  Returns
**      a bool indicating whether the objects exists.
**
**	If class is OC_REPORT, then it will search
**	for any kind of report.
**
** Input params:
**      class           object class of object for which to write header.
**	level		The level of this object in the file
**      name            name of the object.
**      fp              file descriptor of the open file to write to.
**      id              pointer to id to have object id returned in.
**                      may be NULL if id is not desired.
**
** Output params:
**      id              pointer to id to have object id returned in.
**
** Returns:
**      OK            object exists and header was written.
**      FAIL           object does not exist and header was not written.
**
** History:
**      22-Apr-87 (rdesmond) - written.
**	26-jul-1987 (Joe)
**		Modified for copyutil.
**	2-Feb-1988  (Joe)
**		For applications, the owner is not important since
**		an application is global to the database.
**		Took out ranges and made repeat queries.
**		Also, took out sort.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
STATUS
cu_writeobj ( class, level, name, fp, id )
OOID	class;
i4	level;
  char *name;
FILE	*fp;
OOID	*id;
{
        bool    found = FALSE;
	STATUS	stat = OK;
  OOID temp_id;
  OOID hiid;
  OOID lowid;
  char *owner;
  char short_remark[OOSHORTREMSIZE+1];
  char long_remark[OOLONGREMSIZE+1];
  char obj_owner[FE_MAXNAME+1];
  UIDBDATA *uidbdata;
	uidbdata = IIUIdbdata();
	owner = (class == OC_APPL) ? "%" : uidbdata->user;
	lowid = class;
	hiid = (class == OC_REPORT) ? OC_RBFREP : lowid;
	if (STcompare(owner, uidbdata->dba) < 0)
	{
/* # line 161 "cuwrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cuwrutil1",6482,4027);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,30,4,&lowid);
      IIputdomio((short *)0,1,30,4,&hiid);
      IIputdomio((short *)0,1,32,0,owner);
      IIputdomio((short *)0,1,32,0,uidbdata->dba);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cuwrutil1",6482,4027);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, object_owner, short_remark from ii_objects where ob\
ject_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&lowid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class<=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&hiid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(object_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,uidbdata->dba);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by object_owner A\
SC");
        IIexDefine(0,(char *)"cuwrutil1",6482,4027);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&temp_id);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,obj_owner);
      IIgetdomio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 169 "cuwrutil.sc" */	/* host code */
		_VOID_ STtrmwhite(obj_owner);
		_VOID_ STtrmwhite(short_remark);
	        found = TRUE;
	        /*
	        ** Will (possibly) get back several records.
	        ** If its an application are looking for,
	        ** or it is the user's version, then we found the
	        ** one we want.  Otherwise we have the dba's, so
	        ** we keep looking for the user's.
	        */
	        if ( class == OC_APPL || STequal(owner, obj_owner) )
	        {
/* # line 181 "cuwrutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 182 "cuwrutil.sc" */	/* host code */
	        }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	}
	else
	{
/* # line 187 "cuwrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cuwrutil2",6483,4267);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,30,4,&lowid);
      IIputdomio((short *)0,1,30,4,&hiid);
      IIputdomio((short *)0,1,32,0,owner);
      IIputdomio((short *)0,1,32,0,uidbdata->dba);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cuwrutil2",6483,4267);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, object_owner, short_remark from ii_objects where ob\
ject_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&lowid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class<=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&hiid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(object_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,uidbdata->dba);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by object_owner D\
ESC");
        IIexDefine(0,(char *)"cuwrutil2",6483,4267);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&temp_id);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,obj_owner);
      IIgetdomio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 195 "cuwrutil.sc" */	/* host code */
		_VOID_ STtrmwhite(obj_owner);
		_VOID_ STtrmwhite(short_remark);
	        found = TRUE;
	        if ( class == OC_APPL || STequal(owner, obj_owner) )
	        {
/* # line 200 "cuwrutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 201 "cuwrutil.sc" */	/* host code */
	        }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	}
	if ( (stat = FEinqerr()) != OK )
	    return stat;
        if ( found )
        {
                long_remark[0] = EOS;
		if ( (stat = IICUgtLongRemark(temp_id, long_remark)) != OK )
			return stat;
		cu_wrobj(class, level, name, short_remark, long_remark, fp);
                if ( id != (OOID *)NULL )
                        *id = temp_id;
                return OK;
        }
        return FAIL;
}
/*{
** Name:	cu_wrobj	- Write an object header.
**
** Description:
**	Writes out a line for a CU_OBJECT type of record.
**
** Inputs:
**	class		The class of the object to write.
**
**	level		The level of this object in the file.
**
**	name		The name of the object.
**
**	short_rem	The short remark.
**
**	long_rem	The long remark.
**
**	fp		The file to write to.
**
** Outputs:
**	Returns:
**		OK is written
**		E_CU0001_UNKNOWN_CLASS
**			if rectype is bad.
**
** History:
**	26-jul-1987 (Joe)
**		Initial Version
*/
STATUS
cu_wrobj(class, level, name, short_rem, long_rem, fp)
OOID	class;
i4	level;
char	*name;
char	*short_rem;
char	*long_rem;
FILE	*fp;
{
    char	*class_str;
    i4	erparam;
    if ((class_str = IICUrtnRectypeToName(class)) == NULL)
    {
	erparam = (i4) class;
	IIUGerr(E_CU0001_UNKNOWN_CLASS, 0, 2, (PTR) &erparam,
					(PTR) ERx("cu_wrobj"));
	return E_CU0001_UNKNOWN_CLASS;
    }
    SIfprintf( fp, ERx("%s:\t%d\t%s\t%s\t%s\n"),
			class_str,
			level,
			name,
			short_rem,
			long_rem
	);
    return OK;
}
/*{
** Name:	cu_wrcomp	- Write out a component header.
**
** Description:
**	Given a rectype, writes a component header for it.
**
** Inputs:
**	rectype		The rectype to write a component header for.
**
**	fp		The file to write to.
**
** Outputs:
**	Returns:
**		OK if written
**		E_CU0001_UNKNOWN_CLASS
**			if rectype is bad.
**
** History:
**	26-jul-1987 (Joe)
**		Initial Version.
*/
STATUS
cu_wrcomp(rectype, fp)
OOID	rectype;
FILE	*fp;
{
    char	*rec_str;
    i4	erparam;
    if ((rec_str = IICUrtnRectypeToName(rectype)) == NULL)
    {
	erparam = (i4) rectype;
	IIUGerr(E_CU0001_UNKNOWN_CLASS, 0, 2, (PTR) &erparam,
					(PTR) ERx("cu_wrcomp"));
	return E_CU0001_UNKNOWN_CLASS;
    }
    SIfprintf(fp, ERx("%s:\n"), rec_str);
    return OK;
}
/*{
** Name:	cu_wrgbf	- Write out a GBF header.
**
** Description:
**	Given a rectype, writes a GBF header for it.
**
** Inputs:
**	rectype		The rectype to write a component header for.
**
**	name		The name of the graph.
**
**	owner		The old two letter owner of the graph.
**
**	fp		The file to write to.
**
** Outputs:
**	Returns:
**		OK if written
**		E_CU0001_UNKNOWN_CLASS
**			if rectype is bad.
**
** History:
**	26-jul-1987 (Joe)
**		Initial Version.
*/
STATUS
cu_wrgbf(rectype, name, owner, fp)
OOID	rectype;
char	*name;
char	*owner;
FILE	*fp;
{
    char	*rec_str;
    i4	erparam;
    if ((rec_str = IICUrtnRectypeToName(rectype)) == NULL)
    {
	erparam = (i4) rectype;
	IIUGerr(E_CU0001_UNKNOWN_CLASS, 0, 2, (PTR) &erparam,
					(PTR) ERx("cu_wrgbf"));
	return E_CU0001_UNKNOWN_CLASS;
    }
    SIfprintf(fp, ERx("%s:\t%s\t%s\n"), rec_str, name, owner);
    return OK;
}
/*{
** Name:	cu_wrenc	- Write out a CU_ENCODING header.
**
** Description:
**	Given a rectype, writes a CU_ENCODING header for it.
**
** Inputs:
**	name		The name of the object.
**
**	rectype		The rectype to write a component header for.
**
**	class		The class of the encoding.
**
**	size		The size of the lines to follow.
**
**	fp		The file to write to.
**
** Outputs:
**	Returns:
**		OK if written
**		E_CU0001_UNKNOWN_CLASS
**			if rectype is bad.
**
** History:
**	14-sep-1987 (Joe)
**		Initial Version.
*/
STATUS
cu_wrenc(name, rectype, class, size, fp)
char	*name;
OOID	rectype;
OOID	class;
i4	size;
FILE	*fp;
{
    char	*rec_str;
    i4	erparam;
    if ((rec_str = IICUrtnRectypeToName(rectype)) == NULL)
    {
	erparam = (i4) rectype;
	IIUGerr(E_CU0001_UNKNOWN_CLASS, 0, 2, (PTR) &erparam,
					(PTR) ERx("cu_wrenc"));
	return E_CU0001_UNKNOWN_CLASS;
    }
    SIfprintf(fp, ERx("%s:\t%s\t%d\t%d\n"), rec_str, name, class, size);
    return OK;
}
/*{
** Name:	cu_wrenc	- Write out an END marker for a CU_ENCODING.
**
** Description:
**	Writes an END marker for a CU_ENCODING.
**
** Inputs:
**	fp		The file to write to.
**
** History:
**	14-sep-1987 (Joe)
**		Initial Version.
*/
VOID
cu_wrend(fp)
FILE	*fp;
{
    SIfprintf(fp, "END\n");
}
/*{
** Name:	IICUgtLongRemark() -	Fetch Long Remark for Object.
**
** Description:
**	Retrieves the long remark for an object from the database.
**
** Input:
**	id	{OOID}  The object ID.
**
** Output:
**	remark	{char [OOLONGREMSIZE+1]}  The buffer to contain the long remark.
**
** History:
**	05/89 (jhw) -- Written.
*/
STATUS
IICUgtLongRemark ( id, remark )
  OOID id;
  char remark[OOLONGREMSIZE+1];
{
        remark[0] = EOS;
/* # line 445 "cuwrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cuwrutil3",6484,12072);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cuwrutil3",6484,12072);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select long_remark from ii_longremarks where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"cuwrutil3",6484,12072);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,OOLONGREMSIZE+1-1,remark);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 449 "cuwrutil.sc" */	/* host code */
		_VOID_ STtrmwhite(remark);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	return FEinqerr();
}
