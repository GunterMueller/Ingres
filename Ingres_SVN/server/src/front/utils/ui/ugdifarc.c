# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	ugdifarch.c
**
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/**
** Name:	ugdifarch.c - Are we running across different architectures.
**
** Description:
**	This file contains a routine to allow programs to check if
**	if it is running across different machine architectures
**	(e.g., pyramid to a vax).
**
** History:
**	10/22/87 (dkh) - Initial version.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
/*{
** Name:	IIUGdaDiffArch - Check if running across different architecture.
**
** Description:
**	This routine checks to see if a frontend program is connected
**	to a backend that is of a different machine architecture.
**
**
** Inputs:
**	None.
**
** Outputs:
**
**	Returns:
**		TRUE	If the frontend and backend are running on
**			different machine architecture.
**		FALSE	If the frontend and backend are running
**			on the same machine architecture.
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	10/22/87 (dkh) - Initial version.
**	19-apr-1989 (danielt)
**		added call to INQUIRE_INGRES
*/
i4
IIUGdaDiffArch()
{
  i4 diff_arch;
/* # line 64 "ugdifarc.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&diff_arch,60);
  }
/* # line 65 "ugdifarc.sc" */	/* host code */
	return(diff_arch);
}
