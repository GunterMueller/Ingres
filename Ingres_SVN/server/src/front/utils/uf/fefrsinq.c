# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
/**
** Name:    fefrsinq.qc - 	Front-End FRS Inquire Module.
**
** Description:
**	Contains a routine that returns the last FRS error.  Defines:
**
**	FEinqferr()	get last FRS error number.
**
** History:
**	Revision 3.0  84/06/01  neil
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*{
** Name:    FEinqferr()	-	Get Last FRS Error Number.
**
** Description:
**	Return the last FRS error found.
**
** Returns:
**	{nat}	Error number or 0 (if none).
**
** History:
**	01-jun-1984 -	Written (ncg)
**	10-oct-91 (seg)
**		Changed "errno" to "errnum" to avoid naming conflict with
**		c runtime library
*/
i4
FEinqferr()
  {
    i4 errnum;
/* # line 50 "fefrsinq.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&errnum,1,(char *)0,0);
      } /* IIiqset */
    }
/* # line 52 "fefrsinq.qsc" */	/* host code */
    return errnum;
  }
