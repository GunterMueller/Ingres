# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1986, 2004 Ingres Corporation
*/
#include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
#include	<cm.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include <ooclass.h>
# include <uigdata.h>
#include	<ilerror.h>
#include	"eram.h"
/**
** Name:	iamfadd.sc - fid add
**
** Description:
**	Get a new frame number from the database
**
** History:
**	Revision 6.2  88/08  kenl
**	Changed QUEL to SQL.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	02/89  wong  Removed 'new_id()' and changed to call 'IIOOnewId()'.
**
**	Revision 6.0  86/08  bobm
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-May-2005 (kodse01)
**	    replace %ld with %d for old nat and long nat variables.
*/
/*{
** Name:	IIAMfaFidAdd() -
**
** Description:
**	Get a new unique frame identification number from the
**	database.
**
** Inputs:
**	fname	name of frame
**	frem	remarks to associate with frame
**	fown	owner of frame		( no longer used )
**
** Outputs:
**	fid	new number for use
**
**	return:
**		OK		success
**		ILE_FRDWR	ingres failure
**
** Side Effects:
**	updates ii_id table, ii_objects table
**
** History:
**	8/86 (rlm)	written
**	18-aug-88 (kenl)
**		Changed QUEL to SQL.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
*/
#ifdef PCINGRES
GLOBALREF i4	Iamtrycnt;	/* retry query count */
#endif
STATUS
IIAMfaFidAdd(fname,frem,fown,fid)
char	*fname;
  char *frem;
char	*fown;
  OOID *fid;
{
  i4 errchk;
  i4 rows;
  char *fidnm;
  char *nowstr;
  UIDBDATA *uidbdata;
# ifdef PCINGRES
	i4	iamerr1100();
	i4	(*oldprinterr)();
	i4	(*IIseterr())();	/* function returning "pointer to
					   function returning nat" */
# endif
	char	*iiam29fid_name();
	char	*UGcat_now();
	OOID	IIOOnewId();
	/*
	** new_id() handles locking to assure unique id's for simultaneous
	** processes.	Since the id is the key, we don't have to worry
	** about locking the ii_objects table append.
	*/
	if ( (*fid = IIOOnewId()) <= OC_UNDEFINED )
		return ILE_FRDWR;
	fidnm = iiam29fid_name(*fid);
	nowstr = UGcat_now();
#ifdef PCINGRES
	/* the following append seems to often generate error 1100 in backend so
	   catch here and try to silently recover by rerunning the query.  The
	   backend will probably have cleaned up enough to successfully run
	   the query this time */
	oldprinterr = IIseterr(iamerr1100);
	Iamtrycnt = 0;
    TRYAGAIN:
#endif
	uidbdata = IIUIdbdata();
	iiuicw1_CatWriteOn();
/* # line 135 "iamfadd.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamfadd1",6609,19950);
      IIputdomio((short *)0,1,30,4,fid);
      IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE));
      IIputdomio((short *)0,1,32,0,fidnm);
      IIputdomio((short *)0,1,32,0,uidbdata->dba);
      IIputdomio((short *)0,1,32,0,frem);
      IIputdomio((short *)0,1,32,0,nowstr);
      IIputdomio((short *)0,1,32,0,nowstr);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamfadd1",6609,19950);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_objects(object_id, object_class, object_name, object_o\
wner, object_env, is_current, short_remark, object_language, create_da\
te, alter_date, alter_count, last_altered_by)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,fid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE
        ));
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,fidnm);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->dba);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,frem);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,nowstr);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,nowstr);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, '')");
        IIexDefine(0,(char *)"iamfadd1",6609,19950);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 141 "iamfadd.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errchk,2);
    IILQisInqSqlio((short *)0,1,30,4,&rows,8);
  }
/* # line 143 "iamfadd.sc" */	/* host code */
	iiuicw0_CatWriteOff();
#ifdef PCINGRES
	/* if unable to append for whatever reason, delete the old
	   object and try to insert again */
	if (Iamtrycnt++ == 0 && rows == 0)
	{
		iiuicw1_CatWriteOn();
/* # line 152 "iamfadd.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from ii_objects where\
 id=");
    IIputdomio((short *)0,1,30,4,fid);
    IIsyncup((char *)0,0);
  }
/* # line 153 "iamfadd.sc" */	/* host code */
		iiuicw0_CatWriteOff();
		goto TRYAGAIN;
	}
	_VOID_ IIseterr(oldprinterr);
#endif
	if ( errchk != 0 || rows == 0 )
		return ILE_FRDWR;
	return OK;
}
/*
** 'iiam29fid_name()' generates a name to use when adding a new IL object to
** the database.  A new call wipes out the result from the old - use
** only as a temporary.	 'iiam35fid_unname()' decodes ID from name, and is
** thus intimately dependent on the format set by 'iiam29fid_name()'.
*/
static char	Fid_n[FE_MAXNAME+1] ZERO_FILL;
char *
iiam29fid_name ( id )
OOID	id;
{
	return STprintf(Fid_n, ERx("%d"), id);
}
OOID
iiam35fid_unname ( name )
char	*name;
{
	OOID	id;
	CVal( !CMdigit(name) ? name+3 : name, &id );
	return id;
}
