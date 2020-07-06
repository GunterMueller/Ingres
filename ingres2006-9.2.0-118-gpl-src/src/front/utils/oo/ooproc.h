/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/

/**
** Name:	ooproc.h -	OC_OBJECT Procedure Declarations.
**
** Description:
**	Generated automatically by classout.
**
** History:
**	Revision 4.0  86/01  peterk
**	Initial revision.
**	Revision 6.2  89/07  wong
**	Added 'iioCOalloc()', 'iiooStrAlloc()' and 'iiooMemAlloc()'.  Removed
**	Graph method routine declarations, which are in "graphics!graf!grproc.h"
**
**	8-feb-93 (blaise)
**		Changed _flush, etc. to ii_flush because of conflict with
**		Microsoft C library.
**/
OOID	iioCOalloc();
OOID	iioCOat();
OOID	iioCOxatEnd();
OOID	iioCOpatPut();
OOID	iioCOcurrentName();
OOID	iioCOdo();
OOID	iioCOfirst();
OOID	iioCOinit();
OOID	iioCOisEmpty();
OOID	iioCOnext();
OOID	iioCOprint();
OOID	iiooAlloc();
char	*iiooStrAlloc();
OO_OBJECT	*iiooMemAlloc();
OOID	Cdestroy();
OOID	Cfetch();
OOID	iiooClook();
OOID	CnameOk();
OOID	iiooCnew();
OOID	iiooC0new();
OOID	iiooCDbnew();
OOID	iiooCperform();
OOID	iiobCheckName();
OOID	iiobConfirmName();
OOID	Crename();
OOID	CsubClass();
OOID	iiooCwithName();
OOID	ENdecode();
OOID	ENdestroy();
OOID	ENfetch();
OOID	ENflush();
OOID	ENinit();
OOID	ENinit();
OOID	ENreadfile();
OOID	ENwritefile();
OOID	Minit();
OOID	MinitDb();
bool	iiobAuthorized();
OOID	OBclass();
OOID	OBcopy();
OOID	OBcopyId();
OOID	OBdestroy();
OOID	OBedit();
OOID	OBencode();
OOID	OBinit();
OOID	OBinit();
OOID	OBinit();
OOID	OBisEmpty();
OOID	OBisNil();
OOID	OBmarkChange();
char	*OBname();
OOID	OBnoMethod();
OOID	OBprint();
OOID	OBrename();
OOID	OBsetPermit();
OOID	OBsetRefNull();
OOID	OBsubResp();
OOID	OBtime();
OOID	REinit();
OOID	REinitDb();
OOID	fetch0();
OOID	fetchAll();
OOID	fetchAt();
OOID	fetchDb();
OOID	flush0();
OOID	IIflushAll();
OOID	flushAt();
