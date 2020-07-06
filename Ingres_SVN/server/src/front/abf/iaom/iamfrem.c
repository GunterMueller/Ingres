# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1986, 2004 Ingres Corporation
*/
#include <compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include <fe.h>
# include	<er.h>
#include "iamstd.h"
/**
** Name:	iamfrem.c - remove fid from database.
**
** Description:
**	remove frame from database.
*/
/*{
** Name:	IIAMfrFidRem - remove frame id
**
** Description:
**	Removes frame id and associated IL from the database.  Valid both to
**	remove an already encoded frame, or one that has been noted
**	in the ii_objects table through IIAMfaFidAdd, but not encoded yet.
**
** Inputs:
**	fid	frame to remove
**
** Side Effects:
**	updates ii_objects, ii_encodings table
**
** Returns:
**	OK
**	ILE_FAIL DB failure
**
** History:
**
**	1/88 (bobm) note that abf no longer uses return value anywhere,
**		so we can save a retrieve by simplifying return information.
**	18-aug-88 (kenl)
**		Changed QUEL to SQL.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
STATUS
IIAMfrFidRem(fid)
  i4 fid;
{
  i4 errchk;
	/*
	** delete the encoding record first.  Having just an
	** object table record is still a consistent state.
	*/
	iiuicw1_CatWriteOn();
/* # line 65 "iamfrem.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamfrem1",6614,29918);
      IIputdomio((short *)0,1,30,4,&fid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamfrem1",6614,29918);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where encode_object=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&fid);
        IIexDefine(0,(char *)"iamfrem1",6614,29918);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 66 "iamfrem.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errchk,2);
  }
/* # line 67 "iamfrem.sc" */	/* host code */
	if (errchk == 0)
	{
/* # line 69 "iamfrem.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamfrem2",6615,29213);
      IIputdomio((short *)0,1,30,4,&fid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamfrem2",6615,29213);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&fid);
        IIexDefine(0,(char *)"iamfrem2",6615,29213);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 70 "iamfrem.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errchk,2);
  }
/* # line 71 "iamfrem.sc" */	/* host code */
	}
	iiuicw0_CatWriteOff();
	if (errchk != 0)
		return (ILE_FAIL);
	return (OK);
}
