/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/

/**
** Name:	abfglobs.h -	ABF Global Data Declarations.
**
** Defines all the global variables used by abf
**
** History:
** 	6-Feb-1992 (blaise)
**		Added GLOBALREFS for IIAM_zdcDelCheck, IIAM_zccCompChange,
** 		IIAM_zdcDateChange, IIAM_zraRecAttCheck, IIAM_zctCompTree,
**		previously referenced in dmchecks.h, moved here because
**		they're used exclusively by abf.
** 	6-Feb-1992 (blaise)
**		Moved definition of IIAMABFIAOM back to dmchecks.h
*/

/*
** FLAGS
*/

GLOBALREF bool	IIabWopt;		/* Ignore warning option */
GLOBALREF bool	IIabC50;		/* 5.0 compatibility flag */

/*
** VERSIONS
*/
GLOBALREF i2	osNewVersion;	/* non-zero if new version of OSL installed */

/*
**	References to Globals in abmain.c that are loaded at startup time
**	with the obvious corresponding value so that IAOM's data is not 
**	referenced inside ABF.
**	
**	History:
**
**	02-dec-91 (leighb) DeskTop Porting Change: Created
*/

GLOBALREF PTR IIAM_zdcDelCheck;			 
GLOBALREF PTR IIAM_zccCompChange;		 
GLOBALREF PTR IIAM_zdcDateChange;		 
GLOBALREF PTR IIAM_zraRecAttCheck;		 
GLOBALREF PTR IIAM_zctCompTree;			 

