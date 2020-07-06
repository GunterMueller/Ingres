# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* static char	Sccsid[] = "@(#)iiqbf.c	30.1	11/14/84"; */
/*
**	IIQBF.QSC  -  This module contains the interface
**		      to mqbf from equel.
**
**	Routines:
**	     iiqbf()
**
**	Written: 2/17/84 (nml) -stolen from qbf
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# define	ARGVSIZ	20
i4
IIqbf(line)
	register char	*line;
{
	register char	**avp;
	i4		argc = 0;
	char		*argv[ARGVSIZ];
	if (!mq_equel)
	{
		mq_equel = TRUE;
		STgetwords(line, &argc, argv);
		for (avp = argv; *avp; avp++)
			argc++;
		mqbfmain(argc, argv);
	}
	mq_equel = FALSE;
	return mq_status;
}
