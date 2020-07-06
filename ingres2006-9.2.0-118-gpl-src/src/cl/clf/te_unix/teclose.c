
# include	<compat.h>
# include	<gl.h>
# include	<te.h>
# include	"TE.h"

/*
**Copyright (c) 2004 Ingres Corporation
**
**	Name:
**		TEclose.c
**
**	Function:
**		Close the terminal
**		
**
**	Arguments:
**
**	Result:
**		Flush the contesnts of the write buffers and close the
**		the terminal.  TEclose does an implicit TErestore(TE_NORMAL).
**
**	Side Effects:
**		None
**
**	History:
**		10/83 - (mmm)
**			written
**
**
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
*/

TEclose()
{
	TEflush();
	TErestore(TE_NORMAL);
}
