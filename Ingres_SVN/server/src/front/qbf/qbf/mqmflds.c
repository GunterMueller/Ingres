# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* static char	Sccsid[] = "@(#)mq_mflds.qc	30.1	12/20/84";	*/
/*
**	MQMFLDS.QSC -
**
**	Routines:
**	     mq_mflds()  - count number of fields matching
**			   attributes in qdef
**
**	Written: 2/21/84 (nml)
**
**	History:
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**      	09/26/90 (emerson) Don't display logical keys (bug 8593).
**
**	Copyright (c) 2004 Ingres Corporation
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
i4
mq_mflds(qdef)
MQQDEF	*qdef;
{
	register i4	i, cntr;
	ATTRIBINFO	*att;
	cntr = 0;
	for (i = 0; i < qdef->mqnumatts; i++)
	{
		att = qdef->mqatts[i];
		if (att->formfield[0] != EOS && att->lkeyinfo == 0)
			cntr++;
	}
	return(cntr);
}
