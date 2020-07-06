# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
/**
** Name:	vqffldlp.qsh
**
** Description:
**	Definitions needed by vqffldlp.qsc and its clients.  These are the
**	routines which do a listpick of field and column names on a frame's
**	form.
**
** History:
**	10/90 (Mike S) created
**	13-dec-1991 (kevinm)
** 		Declared iivqLP0form[], iivqLP1tblfld[], iivqLP2actCol[],
**		iivqLP3flagsCol, iivqLP4flags2Col, iivqLP5fldTypeCol to
**		be GLOBALCONSTREF to be consistent with other places it is
**		declared.
**/
  GLOBALCONSTREF char iivqLP0form[];
  GLOBALCONSTREF char iivqLP1tblfld[];
  GLOBALCONSTREF char iivqLP2actCol[];
  GLOBALCONSTREF char iivqLP3flagsCol[];
  GLOBALCONSTREF char iivqLP4flags2Col[];
  GLOBALCONSTREF char iivqLP5fldTypeCol[];
FUNC_EXTERN bool IIVQcdfCheckDerivedField();
