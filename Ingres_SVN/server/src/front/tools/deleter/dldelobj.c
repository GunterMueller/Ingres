# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<cv.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<er.h>
# include	<si.h>
# include	<st.h>
# include <ooclass.h>
# include	<ug.h>
# include	<erde.h>
/*	Defines */
# define	MAX_OBJECTS	2000	/* max number of objects matching name */
/*	Externs */
FUNC_EXTERN	STATUS	dl_app();
FUNC_EXTERN	STATUS	dl_gen();
/*	TypeDefs */
typedef	struct
{
	char	objcode;		/* One char code for app type */
	OOID	lowid;			/* Low id for match */
	OOID	hiid;			/* High id for match */
	ER_MSGID	objname;	/* Name of object for messages */
	STATUS	(*delproc)();		/* Address of delete procedure */
} OBJCODE;
static	READONLY OBJCODE objcodes[] =
{
    	{'a',	OC_APPL,	OC_APPL,	F_DE0003_App,	dl_app},
	{'f',	OC_FORM,	OC_FORM,	F_DE0004_Form,	dl_gen},
	{'g',	OC_GRAPH,	OC_GRAPH,	F_DE0005_Graph,	dl_gen},
	{'j',	OC_JOINDEF,	OC_JOINDEF,	F_DE0006_Jdef,	dl_gen}, 
	{'q',	OC_QBFNAME,	OC_QBFNAME,	F_DE0007_Qbfnm,	dl_gen},
	{'r',	OC_REPORT,	OC_RBFREP,	F_DE0008_Rep,	dl_gen},
	{' ',	0,		0}
};
/*
** Name:	dldlobj.qc	- delete an object.
**
** Description:
**	Given an object type, name and owner, get a full list of 
**	matching objects into an array, and then call underlying routines
**	to actually delete the object.
**
** Inputs:
**	obj_type	object type ('r' 'f' 'a' 'g' 'q' or 'j')
**	xobj_name	object name (may include wildcard)
**	xusername	owner name (may be * if -c specified)
**	silent		TRUE to suppress status messages.
**
** Outputs:
**	Returns:
**		0 - normal completion.
**		FAIL - any problem.
**
** History:
**	13-mar-1985 (prs)
**		written.
**	11-nov-1987 (peter)
**		Update for 6.0.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	09/23/89 (dkh) - Porting changes integration.
**	15-jan-90 (kenl)
**		Changed QUEL to SQL.  LIKE is now used for the pattern 
**		matching of command line arguments.  SQL pattern matching
**		characters must now be used instead of QUEL.
**      05-mar-1990 (mgw)
**              Changed #include <erde.h> to #include "erde.h" since this is
**              a local file and some platforms need to make the destinction.
**	7/90 (bobm)
**		Integrate ug bug fix for wild cards.  This is an in-line
**		conversion of QUEL "*" to SQL "%", which may be irrelevent
**		given kenl's fix, but I can't see it hurting anything either.
**	21-mar-1991 (bobm)
**		Integrate desktop changes.
**      06-jan-1992 (rdrane)
**              Changed #include "erde.h" back to #include <erde.h> since it's
**              no longer a local file (shared with the front/misc/deleter
**		production DELETER).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
STATUS
dl_delobj(obj_type,xobj_name,xusername,dbdirn,silent)
char	*obj_type;
  char *xobj_name;
  char *xusername;
char	*dbdirn;
bool	silent;
{
	static	char	*oname[MAX_OBJECTS+1];	/* number of objects passing */
	static	char	*oowner[MAX_OBJECTS+1];	/* owners of objects */
  static OOID oclass[MAX_OBJECTS+1];
  static OOID oid[MAX_OBJECTS+1];
  char m_object[FE_MAXNAME+1];
  char m_owner[FE_MAXNAME+1];
  char msg_name[25];
  OOID xlowclass;
  OOID xhiclass;
	i4	numobjects;		/* number of objects retrieved */
	i4	i;			/* utility counter */
	OBJCODE	*whichobj;		/* PTR to object rec for type */
	STATUS	retstat;
	i2	tag;			/* memory tag */
	/* Depending on the object type, get list of objects 
	** from correct catalog */
	for (i=0; ; i++)
	{	/* get the right object record */
		whichobj = &objcodes[i];
		if (whichobj->objcode == ' ')
		{	/* Did not match the classes listed. */
			IIUGerr(E_DE0001_NoClass, UG_ERR_ERROR, 1,
				(PTR) obj_type);
			return FAIL;
		}
		if (whichobj->objcode == *obj_type)
			break;		/* Found it */
	}
	if (STequal(xobj_name, "*"))
		xobj_name = "%";
	if (STequal(xusername, "*"))
		xusername = "%";
	/* first get the set of matching objects from object catalog */
	tag = FEbegintag();
	xlowclass = whichobj->lowid;
	xhiclass = whichobj->hiid;
	STtrmwhite(xobj_name);
	CVlower(xobj_name);
	STtrmwhite(xusername);
	CVlower(xusername);
	numobjects = 0;
/* # line 145 "dldelobj.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, object_class, object_name, object_owner from ii_obj\
ects where object_class>=");
    IIputdomio((short *)0,1,30,4,&xlowclass);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_class<=");
    IIputdomio((short *)0,1,30,4,&xhiclass);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_name like ");
    IIputdomio((short *)0,1,32,0,xobj_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner like ");
    IIputdomio((short *)0,1,32,0,xusername);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&oid[numobjects]);
      IIgetdomio((short *)0,1,30,4,&oclass[numobjects]);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,m_object);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,m_owner);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 156 "dldelobj.sc" */	/* host code */
		STtrmwhite(m_object);
		STtrmwhite(m_owner);
		if (numobjects > MAX_OBJECTS)
		{
			i = MAX_OBJECTS;
			IIUGerr(E_DE0006_TooMany, UG_ERR_ERROR, 
				(i4) 4, (PTR) &i, 
				(PTR) ERget(whichobj->objname),
				(PTR) xobj_name,
				(PTR) xusername);
/* # line 166 "dldelobj.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 167 "dldelobj.sc" */	/* host code */
		}
		oname[numobjects] = 
			(char *) FEreqmem((u_i4)tag, 
					(u_i4) (STlength(m_object)+1),
					FALSE, NULL);
		STcopy(m_object, oname[numobjects]);
		oowner[numobjects] = 
			(char *) FEreqmem((u_i4)tag, 
					(u_i4) (STlength(m_owner)+1),
					FALSE, NULL);
		STcopy(m_owner, oowner[numobjects]);
		numobjects++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	tag = FEendtag();
	if (numobjects <= 0)
	{	/* No matches found. Give message and return. */
		IIUGmsg(ERget(S_DE0008_NoMatch), FALSE, (i4) 3,
			(PTR) ERget(whichobj->objname),
			(PTR) xobj_name,
			(PTR) xusername);
		_VOID_ FEfree(tag);
		return(FAIL);
	}
	/* Now call the delete routines to actually delete
	** each object.
	*/
	for (i=0; i<numobjects; i++)
	{
		if (!silent)
		{
			IIUGmsg(ERget(S_DE0007_Status), FALSE, (i4) 3,
				(PTR) ERget(whichobj->objname), 
				(PTR) oname[i],
				(PTR) oowner[i]);
		}
		/* Call delete procedure */
		retstat = (*whichobj->delproc) 
			(oname[i], oowner[i], oclass[i], oid[i], dbdirn);
		if (retstat != OK)
		{
			i = retstat;
			IIUGerr(E_DE0009_GenError, UG_ERR_ERROR, (i4) 4,
				(PTR) ERget(whichobj->objname),
				(PTR) oname[i],
				(PTR) oowner[i],
				(PTR) &i);
		}
	}
	_VOID_ FEfree(tag);
	return (0);
}
