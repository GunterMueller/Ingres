# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include 	<lqgdata.h>
/**
** Name:    feinginq.qc -	Front-End Utility Inquire Query Status Module.
**
** Description:
**	Contains routines that return the most recent query completion status.
**	Defines:
**
**	FEinqrows()	return number of rows affected for last query.
**	FEinqerr()	return DBMS query status for last query.
**	FEchkerr()	return DBMS query status for last query and OS error.
**
** History:
**	Revision 4.1  86/03/13  marian
**	Added FEchkerr().
**
**	Revision 3.0  84/06/01  neil
**	Initial revision.
**	19-feb-1988 (danielt) SQLized database statements
**
**	11/02/90 (dkh) - Replaced IILIBQgdata with IILQgdata().
**	10-oct-91 (seg)
**		Changed "errno" to "errnum" to avoid naming conflict with
**		c runtime library
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*{
** Name:    FEinqrows() -	Return Number of Rows Affected for Last Query.
**
** Description:
**	Returns the number of rows affected by the most recent query.
**
** Returns:
**	{nat}  Number of rows affected by the most recent query.
**
** History:
**	01-jun-1984 -	Written (ncg)
**	19-feb-1988 (danielt) SQLized database statements
*/
i4
FEinqrows()
{
  i4 rcount;
/* # line 61 "feinginq.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&rcount,8);
  }
/* # line 62 "feinginq.sc" */	/* host code */
    return rcount;
}
/*{
** Name:    FEinqerr() -	Return DBMS Status for Last Query.
**
** Description:
**	Returns the DBMS status of the most recent query.
**
** Returns:
**	{nat}  The DBMS status of the most recent query.
**
** History:
**	01-jun-1984 -	Written (ncg)
**	19-feb-1988 (danielt) SQLized database statements
**	10-oct-91 (seg)
**		Changed "errno" to "errnum" to avoid naming conflict with
**		c runtime library
*/
i4
FEinqerr()
{
  i4 errnum;
/* # line 86 "feinginq.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 87 "feinginq.sc" */	/* host code */
    return errnum;
}  
/*{
** Name:    FEchkerr() -	Return DBMS Status of Last Query and OS Status.
**
** Description:
**	Returns the number of rows affected by the most recent query and
**	the internal LIBQ OS status.
**
** Output:
**	ingerr	{nat *}  The DBMS status of the last query.
**	oserr	{nat *}  The internal LIBQ OS status.
**
** History:
**	13-mar-1986 (marian)	bug # 7572  Written.
*/
VOID
FEchkerr (ingerr, oserr)
  i4 *ingerr;
  i4 *oserr;
{
/* # line 110 "feinginq.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,ingerr,2);
  }
/* # line 111 "feinginq.sc" */	/* host code */
        *oserr = IILQgdata()->os_errno;
}
