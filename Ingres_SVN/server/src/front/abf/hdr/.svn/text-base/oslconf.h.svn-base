/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/

/**
** Name:    oslconf.h -		OSL Parser Configuration Definitions File.
**
** Description:
**	Contains definitions and declarations for the OSL parser that allow
**	it to be configured to support either QUEL or SQL as a data manipulation
**	language.  (This allows much the same code to be used for OSL/QUEL and
**	OSL/SQL.)  This file also includes some global definitions for the OSL
**	parser.
**
** History:
**	Revision 6.0  87/05  wong
**	Added 'osQuote' and DML configuration definitions.
**    25-july-97 (rodjo04)
**	bug 82837.  Added Two macros  Is_on(), and  Reset_on(). 
**	Is_on(char * ) will return True if char * is = 'on'.
**	Reset_on() will set on_clause to zero. 
**  12-Sep-97 (rodjo04)
**  Adjusted macros Is_on() and Check_join() so that 
**  STbcompare is passed a length of zero for both strings
**  so that the entire string will be compared.
**  10-mar-2000 (rodjo04) BUG 98775.
**      Removed above change (bug 82837).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/

GLOBALCONSTREF i4	osldml;

/*
** These are the same as DB_QUEL and DB_SQL in dbms.h, but we don't 
** want to force inclusion of that file.  Dangerous, but works.
*/
#define	OSLQUEL	1
#define OSLSQL	2
#define QUEL	(osldml == OSLQUEL)
#define SQL	(osldml == OSLSQL)

GLOBALCONSTREF char	osQuote[];

/*
** Constant to signify start of special OSL comment
*/
# define	OSCOMCHAR	'#'

#define	    OSBUFSIZE	256		/* internal buffer size */

