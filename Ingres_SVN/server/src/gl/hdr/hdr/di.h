/*
**	Copyright (c) 2004 Ingres Corporation
*/
#include    <qu.h>
#include    <dicl.h>

/**CL_SPEC
** Name:	DI.h	- Define DI function externs
**
** Specification:
**
** Description:
**	Contains DI function externs
**
** History:
**	2-jun-1993 (ed)
**	    initial creation to define func_externs in glhdr
**	03-mar-1999 (walro03)
**	    Include qu.h for definition of QUEUE needed in dicl.h.
**	29-Jun-1999 (jenjo02)
**	    Added function prototypes for DIgather_write(), 
**	    DIgather_setting().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	09-Apr-2001 (jenjo02)
**	    Added gw_io, gw_pages to DIgather_write() prototype.
**/

FUNC_EXTERN STATUS  DIalloc(
			DI_IO         *f,
			i4	      n,
			i4	      *page,
			CL_SYS_ERR    *err_code
);

FUNC_EXTERN STATUS  DIclose(
			DI_IO          *f,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIcreate(
			DI_IO          *f,
			char           *path,
			u_i4          pathlength,
			char           *filename,
			u_i4          filelength,
			i4            pagesize,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIdelete(
			DI_IO          *f,
			char           *path,
			u_i4          pathlength,
			char           *filename,
			u_i4          filelength,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIdircreate(
			DI_IO          *f,
			char           *path,
			u_i4          pathlength,
			char           *dirname,
			u_i4          dirlength,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIdirdelete(
			DI_IO          *f,
			char           *path,
			u_i4          pathlength,
			char           *dirname,
			u_i4          dirlength,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS  DIflush(
			DI_IO          *f,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIforce(
			DI_IO          *f,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIlistdir(
			DI_IO          *f,
			char           *path,
			u_i4          pathlength,
			STATUS         (*func)(),
			PTR            arg_list,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIlistfile(
			DI_IO          *f,
			char           *path,
			u_i4          pathlength,
			STATUS         (*func)(),
			PTR            arg_list,
			CL_SYS_ERR     *err_code
);
FUNC_EXTERN STATUS  DIopen(
			DI_IO          *f,
			char           *path,
			u_i4          pathlength,
			char           *filename,
			u_i4          filelength,
			i4            pagesize,
			i4            mode,
			u_i4      flags,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS  DIread(
			DI_IO	       *f,
			i4        *n,
			i4        page,
			char           *buf,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIrename(
			DI_IO	       *di_io_unused,
			char           *path,
			u_i4          pathlength,
			char           *oldfilename,
			u_i4          oldlength,
			char           *newfilename,
			u_i4          newlength,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS DIsense(
			DI_IO          *f,
			i4        *page,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS  DIwrite(
			DI_IO	       *f,
			i4            *n,
			i4        page,
			char           *buf,
			CL_SYS_ERR     *err_code
);

FUNC_EXTERN STATUS 	DIgalloc(
			DI_IO               *f,
			i4             n,
			i4             *page,
			CL_ERR_DESC         *err_code );



FUNC_EXTERN STATUS 	DIguarantee_space(
			DI_IO               *f,
			i4             start_pageno,
			i4             number_pages,
			CL_ERR_DESC         *err_code );

FUNC_EXTERN i4		DIgather_setting(void);

FUNC_EXTERN STATUS 	DIgather_write(
			i4		op,
			char		*gather,
			DI_IO		*f,
			i4		*n,
			i4		page,
			char		*buf,
			VOID		(*evcomp)(),
			PTR		closure,
			i4		*queued_count,
			u_i4		*gw_pages,
			u_i4		*gw_io,
			CL_ERR_DESC     *err_code );
