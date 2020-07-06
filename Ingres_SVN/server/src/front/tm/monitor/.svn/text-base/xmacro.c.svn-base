# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<si.h>  

/*
** Copyright (c) 2004 Ingres Corporation
**
**  XMACRO -- construct and execute a macro
**
**	Builds an argument string from the given parameters
**	enclosing the string in '{}'. The resulting macro is then
**	evaluated and a pointer to the resulting string returned.
**
**	Parameters:
**		arg -- variable number of string arguments terminated
**			with a NULL argument.
**
**	Returns:
**		pointer to string resulting from macro evaluation
**
**	History:
**		3/4/81 (peb) -- first written for VMS.
**
*/

FUNC_EXTERN	char	*mcall();

char *
xmacro(arg)
char	*arg;
{
	char		buf[512];
	register char	**p;

	p = &arg;
	*buf = '\0';
	for ( ; *p != NULL; p++)
		STcat(buf, *p);
	return (mcall(buf));
}
