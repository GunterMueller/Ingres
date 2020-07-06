/*
**	Copyright (c) 2004 Ingres Corporation
*/

# ifndef ID_HDR_INCLUDED
# define ID_HDR_INCLUDED 1
#include    <idcl.h>

/**CL_SPEC
** Name:	ID.h	- Define ID function externs
**
** Specification:
**
** Description:
**	Contains ID function externs
**
** History:
**	2-jun-1993 (ed)
**	    initial creation to define func_externs in glhdr
**	31-mar-1997 (canor01)
**	    Protect against multiple inclusion of id.h.
**	27-oct-1998 (canor01)
**	    Add prototypes for UUID functions.
**	18-nov-1998 (canor01)
**	    Add IDuuid_from_string.
**	24-oct-2000 (somsa01)
**	    Added IDname_service for NT.
**	30-Mar-2005 (kodse01)
**	    Added prototype for IDsetsys.
**	13-Apr-2005 (hanje04)
**	    BUG 112987
**	    Add prototype for IDename
**/

#ifndef IDgroup
FUNC_EXTERN VOID    IDgroup(
	    GID		*gid
);
#endif

FUNC_EXTERN VOID    IDname(
	    char	**name
);

FUNC_EXTERN VOID    IDename(
	    char	**name
);

#ifdef NT_GENERIC
FUNC_EXTERN STATUS  IDname_service(
	    char	**name
);
#endif

#ifndef IDuser
FUNC_EXTERN VOID    IDuser(
	    MID		*uid
);
#endif

# ifndef IDuuid_create
FUNC_EXTERN STATUS  IDuuid_create(
	    UUID	*uuid
);
# endif

# ifndef IDuuid_to_string
FUNC_EXTERN STATUS  IDuuid_to_string(
	    UUID	*uuid,
	    char	*string
);
# endif

# ifndef IDuuid_from_string
FUNC_EXTERN STATUS  IDuuid_from_string(
	    char	*string,
	    UUID	*uuid
);
# endif

# ifndef IDuuid_compare
FUNC_EXTERN STATUS  IDuuid_compare(
	    UUID	*uuid1,
	    UUID	*uuid2
);
# endif

# ifndef IDsetsys
FUNC_EXTERN VOID IDsetsys(
          char *SysLocationVariable,
          char *SysLocationSubdirectory,
          char *SysProductName,
          char *SysDBMSName,
          char *SysVarPrefix,
          char *SysCfgPrefix,
          char *SysCatPrefix,
          char *SysAdminUser,
          char *SysSyscatOwner
);
# endif

# endif /* ! ID_HDR_INCLUDED */
