# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name: impstatic.sc
**
## History:
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "imp.h"
# include "impcommon.h"
  int imaSession = IMA_SESSION;
  int iidbdbSession = IIDBDB_SESSION;
  int userSession = USER_SESSION;
  int inFormsMode = FALSE;
  int iidbdbConnection = FALSE;
  int dbConnection = FALSE;
  int timeVal = 0;
  GcnEntry *GcnListHead = (GcnEntry *) NULL;
  SessEntry *SessListHead = (SessEntry *) NULL;
  char *currentVnode;
