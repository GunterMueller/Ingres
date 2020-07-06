/*
**	Copyright (c) 2004 Ingres Corporation
*/
# ifndef CM_HDR_INCLUDED
# define CM_HDR_INCLUDED 1
#include    <cmcl.h>

/**CL_SPEC
** Name:	CM.h	- Define CM function externs
**
** Specification:
**
** Description:
**	Contains CM function externs
**
** History:
**	2-jun-1993 (ed)
**	    initial creation to define func_externs in glhdr
**	31-mar-1997 (canor01)
**	    Protect against multiple inclusion of cm.h.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	14-may-2002 (somsa01)
**	    Added CMset_locale().
**      25-Jun-2002 (fanra01)
**          Sir 108122
**          Add prototypes
**	14-Jan-2004 (gupsh01)
**	    Added the file location in input parameter, it can now
**	    be CM_COLLATION_LOC for $II_SYSTEM/ingres/files/collation
**	    or CM_UCHARMAPS_LOC for $II_SYSTEM/ingres/filese/ucharmaps
**	    for unicode and local encoding mapping files.
**	    Added prototype for CM_getcharset() function.
**      22-Mar-2004 (hweho01)
**          1) Added prototype CMread_attr(), 
**          2) Enabled CMvalidusername() and CMvalidhostname() to Unix.
**	14-Jun-2004 (schka24)
**	    Added prototypes for charset functions.
**      18-Nov-2004 (Gordon.Thorpe@ca.com and Ralph.Loen@ca.com)
**          Added new CMgetDefCS() function to simplify export of the
**          CMdefcs global reference in Windows.
**	26-Apr-2007 (gupsh01)
**	    Added CM_ischarsetUTF8() to identify if the II_CHARSETxx
**	    is set to UTF8.
**/

FUNC_EXTERN STATUS      CMclose_col(
#ifdef	CL_PROTOTYPED
	CL_ERR_DESC *	syserr,
	i4		loctype		
#endif
);
FUNC_EXTERN STATUS      CMdump_col(
#ifdef	CL_PROTOTYPED
	char		*colname, 
	PTR		tablep, 
	i4		tablen, 
	CL_ERR_DESC	*syserr,
	i4		loctype		
#endif
);
FUNC_EXTERN STATUS      CMopen_col(
#ifdef	CL_PROTOTYPED
	char		*colname, 
	CL_ERR_DESC	*syserr,
	i4		loctype		
#endif
);
FUNC_EXTERN STATUS      CMread_col(
#ifdef	CL_PROTOTYPED
	char		*bufp, 
	CL_ERR_DESC	*syserr
#endif
);
FUNC_EXTERN STATUS      CMset_attr(
#ifdef	CL_PROTOTYPED
	char		*name, 
	CL_ERR_DESC	*err
#endif
);
FUNC_EXTERN STATUS      CMwrite_attr(
#ifdef	CL_PROTOTYPED
	char		*name, 
	CMATTR		*attr, 
	CL_ERR_DESC	*syserr
#endif
);
FUNC_EXTERN char *      CMunctrl(
#ifdef	CL_PROTOTYPED
	char		*str
#endif
);
FUNC_EXTERN STATUS      CMset_locale();

FUNC_EXTERN STATUS      CM_getcharset(
#ifdef	CL_PROTOTYPED
	char		*pcs
#endif
);

#ifndef CMoper
FUNC_EXTERN u_char *    CMoper(
#ifdef	CL_PROTOTYPED
	char		*str
#endif
);
#endif

# ifndef CMread_attr
FUNC_EXTERN STATUS      CMread_attr(
#ifdef	CL_PROTOTYPED
	char		*name, 
	char		*filespec, 
	CL_ERR_DESC	*err
#endif
);
# endif /* CMread_attr */

FUNC_EXTERN void	CMget_charset_name(
#ifdef	CL_PROTOTYPED
	char		*charset
#endif
);

FUNC_EXTERN STATUS	CMset_charset(
#ifdef	CL_PROTOTYPED
	CL_ERR_DESC	*cl_err
#endif
);

# ifndef CMvalidhostname
FUNC_EXTERN bool
CMvalidhostname(
# ifdef	CL_PROTOTYPED
	char* hostname
# endif /* CL_PROTOTYPED */
);
# endif /* CMvalidhostname */

# ifndef CMvalidusername
FUNC_EXTERN bool
CMvalidusername(
# ifdef	CL_PROTOTYPED
	char* username
# endif /* CL_PROTOTYPED */
);
# endif /* CMvalidusername */

# if defined(NT_GENERIC)
# ifndef CMvalidpath
FUNC_EXTERN bool
CMvalidpath(
# ifdef	CL_PROTOTYPED
	char* path
# endif /* CL_PROTOTYPED */
);
# endif /* CMvalidpath */

# ifndef CMvalidpathname
FUNC_EXTERN bool
CMvalidpathname(
# ifdef	CL_PROTOTYPED
	char* path
# endif /* CL_PROTOTYPED */
);
# endif /* CMvalidpathname */

# endif /* NT_GENERIC */

FUNC_EXTERN i4 
CMgetDefCS();

FUNC_EXTERN bool 
CM_ischarsetUTF8();

FUNC_EXTERN u_i2 
CM_getUTF8property();

# endif /* ! CM_HDR_INCLUDED */
