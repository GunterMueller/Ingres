/*
** Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: COPY.H - Common definitions used for COPY command.
**
** Description:
**      The file contains the definitions shared by the DBMS and FE during
**	execution of the COPY command.
**
** History:
**      01-dec-1986 (rogerk)
**          Written.
**	06-apr-1987 (rogerk)
**	    Added CPY_CONTINUE status which specifies that the user wants to
**	    continue with the copy when errors are encountered. 
**	06-apr-1987 (rogerk)
**	    Took out definition of CPY_VARROWS as it was a FE only concern.
**	29-apr-1991 (Derek)
**	    Added support for EMPTYTABLE clause to copy.
**	26-jul-1993 (rmuth)
**	    Added support for COPY to take a variable number of WITH
**	    options. These are passed around in a DMR_CB.
**/

/*
**  Forward and/or External function references.
*/

/* 
** Defined Constants
*/

#define			CPY_INTO	0
#define			CPY_FROM	1

#define			CPY_MAXROW	(2 * DB_MAXTUP)
#define			CPY_MAXDOMAIN	DB_MAXTUP
#define			CPY_MAX_DELIM	2

#define			CPY_ESCAPE	'\\'
#define			CPY_DUMMY_TYPE	-1
#define			CPY_TEXT_TYPE	-2

#define			CPY_OK			0x00000000
#define			CPY_FAIL		0x00000001
#define			CPY_ERROR		0x00000002
#define			CPY_WARN		0x00000004
#define			CPY_DONE		0x00000008
#define			CPY_NOTARG		0x00000010
#define			CPY_NOTBIN		0x00000020
#define			CPY_VARTUPS		0x00000040
#define			CPY_PROGRAM		0x00000080
#define			CPY_NETWORK		0x00000100
#define			CPY_NOSYNC		0x00000200
#define			CPY_NOABORT		0x00000400
#define			CPY_CONTINUE		0x00000800
#define			CPY_EMPTY		0x00001000
#define			CPY_DMR_CB		0x00002000
