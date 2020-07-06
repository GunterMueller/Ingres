# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* static char	Sccsid[] = "@(#)mqtest.c	30.1	11/14/84";	*/
/*
**	MQTEST.QSC  -   This module performs MQBF testing
**			functions.
**
**	Routines:
**	     mqtest()
**	     mqtestexit()
**
**	Stolen from qbftest: 2/15/84 (nml)
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include	<er.h>
# include	"erqf.h"
static FILE	*mq_ofile = NULL;
mqtest()
{
	LOCATION	loc;
	if (mq_itest != NULL)
	{
		FDrcvalloc(mq_itest);
	}
	if (mq_otest[0] != EOS)
	{
		if (LOfroms(FILENAME & PATH, mq_otest, &loc) != OK
			|| SIopen(&loc, ERx("w"), &mq_ofile) != OK)
		{
			mq_otest[0] = EOS;
		}
		else
		{
			FDdmpinit(mq_ofile);
		}
	}
}
mqtestexit()
{
	if (mq_itest != NULL)
		FDrcvclose(FALSE);
	if (mq_ofile != NULL)
		SIclose(mq_ofile);
}
