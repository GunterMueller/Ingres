/*
**	Copyright (c) 2004 Ingres Corporation
*/
#include    <srcl.h>

/**CL_SPEC
** Name:	SR.h	- Define SR function externs
**
** Specification:
**
** Description:
**	Contains SR function externs
**
** History:
**	2-jun-1993 (ed)
**	    initial creation to define func_externs in glhdr
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

FUNC_EXTERN STATUS  SRclose(
			SR_IO		*f,
			i4		delete_flag,
			CL_SYS_ERR	*err_code
);

FUNC_EXTERN STATUS  SRopen(
			SR_IO           *f,
			char            *path,
			u_i4		pathlength,
			char            *filename,
			u_i4	        filelength,
			i4		pagesize,
			u_i4		create_flag,
			i4         n,
			CL_SYS_ERR	*err_code
);

FUNC_EXTERN STATUS  SRread(
			SR_IO	        *f,
			i4         page,
			char            *buf,
			CL_SYS_ERR	*err_code
);

FUNC_EXTERN STATUS  SRwrite(
			SR_IO	        *f,
			i4         page,
			char            *buf,
			CL_SYS_ERR	*err_code
);

