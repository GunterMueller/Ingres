# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** Includes and variable declarations
*/
#include <compat.h>
#include <dbms.h>
#include <fe.h>
#include <ui.h>
#include <uigdata.h>
#include "dba.h"
#include "ingpriv.h"
  static int privmask = 0;
/*
** 	ingprivs.qc - this file contains the routines which get the
**		user's INGRES permissions from the dbdb.  This is used
**		to check the user's capabilities when they select such
**		menuitems as REMOVE.
**
**	This file defines:
**		getingpriv(user) - given a specified user, this routine
**			grabs the user privilege mask from the dbdb, table
**			user, columns name and status.  
**		chkingpriv(priv) - checks if a user has the specified
**			privilege.
*/
/*
** Forward references
*/
VOID getingpriv();
i4  chkingpriv();
/*
**	getingpriv - this routine will get the user's ingres privilege
**		mask out of the INGRES dbdb (assume that the dbdb is open
**		already).  This routine must be called when the dbdb is open
**		and must be called before calling chkingpriv().
**
**	Input: user - a pointer to the user name to get privmask for
**
**	Output: none
**
**	Side Effects: none, if the dbdb is open
**
**	History
**	25-oct-88 (tmt)
**	    if in garp mode, we want to get the privmask from the users file in
**	    sys_ingres:[ingres.files]users.
**	25-jan-89 (tmt)
**	    converted to R6 lockinfo
**	13-nov-95 (pchang)
**	    Translate user name in accordance with database name case (B72029 &
**	    B72536).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
VOID
getingpriv(user)
  char *user;
{
  char l_user[FE_MAXNAME + 1];
  int numrows;
    if (IIUIdbdata()->name_Case == UI_UPPERCASE)
	CVupper(user);
    else
	CVlower(user);
    /*
    **  Get the mask out of the database.  If the specified user is
    **  not an INGRES user, the privmask will be set to zero
    */
/* # line 83 "ingprivs.qsc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(l_user=iiuser.name, privmask=iiuser.status)where iiuser.name\
=");
    IIputdomio((short *)0,1,32,0,user);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,l_user);
      IIgetdomio((short *)0,1,30,4,&privmask);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 86 "ingprivs.qsc" */	/* inquire_equel */
  {
    IIeqiqio((short *)0,1,30,4,&numrows,(char *)"rowcount");
  }
/* # line 87 "ingprivs.qsc" */	/* host code */
    if (numrows < 1)
	privmask = 0;	/* set to 0 because user not found */
    return;
}
/*
**	chkingpriv - this routine will check the passed priv
**		against the master privmask.  The getingpriv() routine
**		must be called prior to this routine to get the user's
**		ingres priv mask.  OK will be returned if the priv
**		mask contains the passed priv, FAIL if otherwise.
**
**	Input - priv contains the privilege to check for.
**
**	Output - none
**
**	Returns - OK if success
**		  FAIL if user doesn't have the specified priv
*/
i4
chkingpriv(priv)
i4  priv;
{
    /*
    **  Check the requested privilege against the previously retrieved
    **  priv mask NOTE: getingpriv MUST be called before this routine
    **  is called.
    */
    return((priv & privmask) ? OK : FAIL);
}
