# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1990, 2004 Ingres Corporation
*/
#include	<compat.h>
/**
** Name:	iamdbses.sc -	IAM DBMS Session Set-up Module.
**
** Description:
**	Contains routines used to set-up and clear locking state for the
**	DBMS session.  Defines:
**
**	IIAMsetDBsession()	set DBMS session lock mode state.
**	IIAMclrDBsession()	clear DBMS session lock mode state.
**
** History:
**	Revision 6.3/03/00  90/04  wong
**	Initial revision.
**/
bool	IIUIdcn_ingres();
VOID
IIAMsetDBsession()
{
	if ( IIUIdcn_ingres() )
	{ /* ... for INGRES only ... */
/* # line 29 "iamdbses.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where readlock=nolock");
    IIsyncup((char *)0,0);
  }
/* # line 30 "iamdbses.sc" */	/* host code */
	}
}
VOID
IIAMclrDBsession()
{
	if ( IIUIdcn_ingres() )
	{ /* ... for INGRES only ... */
/* # line 38 "iamdbses.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where readlock=system");
    IIsyncup((char *)0,0);
  }
/* # line 39 "iamdbses.sc" */	/* host code */
	}
}
