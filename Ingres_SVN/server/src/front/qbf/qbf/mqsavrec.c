# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
/*
**	MQSAVEREC.QSC -
**
**	History:
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
mqsaverec(tbl, reclen, status, tfirst, mq_qdef)
RELINFO	*tbl;			/* individual relation info */ 
i4	reclen;			/* length of record to be written */
int	status;			/* status of record */
i4	tfirst;			/* indicates if this is first tbl for the rec*/
MQQDEF  *mq_qdef;
{
	mqwrtrec(tbl, reclen, status, tfirst);
}
