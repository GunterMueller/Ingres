/*
** Copyright (c) 2004 Ingres Corporation
*/
# ifndef RPU_H_INCLUDED
# define RPU_H_INCLUDED
# include <compat.h>

/**
** Name:	rpu.h - Replicator facility utility routines
**
** Description:
**	Contains defines, declarations and prototypes for using the
**	Replicator facility utility routines.
**
** History:
**	16-dec-96 (joea)
**		Created based on rpu.h in replicator library.
**	23-jul-98 (abbjo03)
**		Remove DLMNAME_LENGTH, EDNM_SLITERAL, RPing_ver_ok and getdecch.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

/* flag values for RPdb_error_check */

# define DBEC_SINGLE_ROW	(1 << 0)
# define DBEC_ZERO_ROWS_OK	(1 << 1)


/* RPdb_error_check error information */

typedef struct
{
	i4	errorno;
	i4	rowcount;
} DBEC_ERR_INFO;


/* return values for RPdb_error_check */

# define DBEC_ERROR		1
# define DBEC_DEADLOCK		2


/* edit_type values for RPedit_name */

# define EDNM_ALPHA	1
# define EDNM_DELIMIT	2
# define EDNM_SLITERAL	3


/* function prototypes */

FUNC_EXTERN STATUS RPdb_error_check(i4 flags, DBEC_ERR_INFO *errinfo);
FUNC_EXTERN bool RPvalid_name(char *name);
FUNC_EXTERN char *RPedit_name(i4 edit_type, char *name, char *edited_name);
FUNC_EXTERN void RPgetwords(char *delims, char *string, i4  *count,
	char **words);

# endif
