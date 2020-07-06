# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<cv.h>		/* 6-x_PC_80x86 */
#include	<st.h>
#include	<si.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<qg.h>
#include	<mqtypes.h>
#include	"mqglobs.h"
# include <uigdata.h>
#include	"mqerrs.h"
#include	"erqf.h"
/**
** Name:	getqname.sc -	Fetch Query Objects ID Module.
**
** Description:
**	Contains routines that check that the query defn. (QDEF) that the user
**	wishes to retrieve from the mqbf catalogs exists.
**
**	Routines:
**	     mqgetqname()  -prompt for name of qdef and make sure
**			    it exists.
**	     mqgetfname()  -prompt for name of form (ie -qbfname) and
**			    make sure it exists.
**
** History:
**	Written: 11/21/83 (nml)
**	05/21/87 (dkh) - Integrated VMS change.	 Passing pointer to an
**		OOID to OOidcheck().
**	07-dec-87 (kenl)
**		Turned module into a .QSC file in order to incorporate
**		the new header files: mqbf.qsh and mqsymbol.qsh.  Added
**		code to deal with case-sensitivity issues of backend
**		object names.  Changed QUEL RETRIEVE to SQL SELECT.
**	29-jan-1988 (peter)
**		Get rid of RANGE statements and use REPEAT query.
**	14-apr-1988 (danielt) added commit after call to OOidCheck.
**	21-july-1988 (sylviap) 
**		Added commit after call to OOidCheck.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	05-dec-89 (kenl)
**		Routine mqgetqname() will no longer produce an error if the
**		mq_editjoin flag is set (we're editing a JoinDef).  The
**		JoinDef may not exist and we want this condition handled
**		by the code for the -J flag.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	3-feb-1994 (rdrane)
**		Retrieve the table owner associated with the QBFname so
**		we won't think that a table owned by another user is not
**		accessable, or worse confuse it with one that we own.
**		b57675.
**	4-mar-94 (donc for rdrane)  
**		Need to check that the owner of the table associated with
**		the QBFname is not NULL before attempting to copy it into
**		qdef->mqcr.owner.  
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
static OOID	Joindef_class = OC_JOINDEF;
bool
mqgetqname ( qdef, uprompt )
MQQDEF	*qdef;
bool	uprompt;
{
	char	buf[FE_PROMPTSIZE+1];
	if (uprompt)
	{
		IIUFask( ERget(S_QF0042_JoinDef_Name_), FALSE, buf, 0 );
		buf[FE_MAXNAME] = EOS;
		if ( STtrmwhite(buf) <= 0 )
		{
			return FALSE;
		}
		CVlower(buf);
		STcopy(buf, qdef->mqcr.name);
	}
	else
	{
		STcopy(qdef->mqcr.name, buf);
		/* make sure that the QBFName is in lowercase */
		CVlower(buf);
	}
	qdef->mqcr.id = -1;
	if ( IIOOidCheck(&Joindef_class, &qdef->mqcr.id,
			buf, (char *)NULL) == OK )
	{
		return (mq_qflag = TRUE);
	}
	if (!mq_lookup && !mq_editjoin)
		FDerror(NOQUERYD, 1, uprompt ? buf : qdef->mqcr.name);
	return FALSE;
}
bool
mqgetfname ( qdef, uprompt )
MQQDEF	*qdef;
bool	uprompt;
{
	bool	found;
  i2 qown_null;
  i4 type;
  char buf[FE_PROMPTSIZE+1];
  char qname[FE_MAXNAME + 1];
  char qowner[FE_MAXNAME + 1];
  char fname[FE_MAXNAME + 1];
  char owner[FE_MAXNAME + 1];
  UIDBDATA *uidbdata;
	uidbdata = IIUIdbdata();
	/*
	**  As of 11/6/87, this routine is NEVER called with the argument
	**  uprompt set to TRUE.
	*/
	if ( uprompt )
	{
		IIUFask( ERget(S_QF0043_QBFName_), FALSE, buf, 0 );
		buf[FE_MAXNAME] = EOS;
		if ( STtrmwhite(buf) <= 0 )
		{
			*qbf_name = EOS;
			return FALSE;
		}
		CVlower(buf);
	}
	else
	{
		STcopy(qbf_name, buf);
		/* make sure that the QBFName is in lowercase */
		CVlower(buf);
	}
	found = FALSE;
/* # line 147 "getqname.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"getqname1",7184,30974);
      IIputdomio((short *)0,1,32,0,buf);
      IIputdomio((short *)0,1,32,0,uidbdata->user);
      IIputdomio((short *)0,1,32,0,uidbdata->dba);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"getqname1",7184,30974);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select q.relname, q.relowner, q.frname, q.qbftype, o.object_owner fro\
m ii_qbfnames q, ii_objects o where o.object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(o.object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->user);
        IIwritio(0,(short *)0,1,32,0,(char *)"or o.object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->dba);
        IIwritio(0,(short *)0,1,32,0,(char *)")and o.object_id=q.objec\
t_id");
        IIexDefine(0,(char *)"getqname1",7184,30974);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,qname);
      IIgetdomio(&qown_null,1,32,FE_MAXNAME + 1-1,qowner);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,fname);
      IIgetdomio((short *)0,1,30,4,&type);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,owner);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 156 "getqname.sc" */	/* host code */
	{
		if ( !found || STequal(uidbdata->user, owner) )
		{
			found = TRUE;
			_VOID_ STtrmwhite(qname); 
			_VOID_ STtrmwhite(fname);
			STcopy(qname, qdef->mqcr.name); 
			qdef->mqcr.owner[0] = EOS; 
			if  (qown_null != -1)
			{
				STcopy(qowner, qdef->mqcr.owner); 
			}
			STcopy(fname, mq_frame); 
			mq_forms = TRUE; 
			if (type == 0)
				onetbl = TRUE; 
		} 
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if ( !found )
	{
		if (!mq_lookup)
			FDerror(NOQBFNM, 1, buf);
		return FALSE;
	}
	if ( type != 0 )
	{ /* check for JoinDef */
		qdef->mqcr.id = -1;
		if ( IIOOidCheck(&Joindef_class, &qdef->mqcr.id,
					qdef->mqcr.name, (char *)NULL) != OK )
		{
			if (!mq_lookup)
				FDerror(NOQUERYD, 1, qdef->mqcr.name);
			return FALSE;
		}
	}
	return TRUE;
}
