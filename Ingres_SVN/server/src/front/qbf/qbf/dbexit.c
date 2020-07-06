# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* static char	Sccsid[] = "@(#)dbexit.qc	30.1	11/14/84"; */
/*
**	DBEXIT.QSC   -  This module performs MQBF ingres 
**			exit functions.
**
**	Routines:
**	     mqdbexit()
**
**	Written: 2/15/84 (nml)
**		 9/30/86 (sandyd) - Saturn changes: use FEing_exit().
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
FUNC_EXTERN	VOID	FEing_exit();
mqdbexit()
{
	/* first end forms stuff */
/* # line 32 "dbexit.qsc" */	/* endforms */
  {
    IIendforms();
  }
/* # line 34 "dbexit.qsc" */	/* host code */
	/* now, exit from INGRES */
	FEing_exit();
}
