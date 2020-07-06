/*
** Copyright (c) 2004 Ingres Corporation
** All Rights Reserved.
*/

# include    <compat.h>
# include    <gl.h>
# include    <systypes.h>
# include    <stdarg.h>
# include    <st.h>

/*
**    STlpolycat - Multiple string concatenate with length check.
**
**    Concatenate N null-terminated strings into space supplied by the user
**    and null terminate.  Return pointer.
**    Differs from STpolycat in that the caller also supplies the size
**    of the result area (size NOT to include the trailing null slot).
**    Use STlpolycat for safe concatenation that prevents stack smash,
**    variable overrun, etc.
**
** Usage:
**	char *ptr = STlpolycat(N, len, c1, c2, ... cN, &result[0]);
**	i4 N;  -- Number of input strings to concatenate
**	i4 len;  -- Length of result not including trailing null
**	char *cn;  -- Input strings
**	char result[];  -- Where to put result
**	Returns &result[0], ie pointer to result
**
** History:
**	14-Jun-2004 (schka24)
**	    Copy from STpolycat for safe string handling
*/

/*VARARGS2*/
char *
STlpolycat( i4  n, i4 len, char *c1, char *c2, ... )
{
    register i4     N = n;
    register char    *charstr;
    register char    *resultstr;
    char        *result;
    va_list        args;
    
    /* Find the result string */

    if ( N == 0 )
        return c1;
    if ( N == 1 )
        resultstr = c2;
    N -= 2; /* start counting after c2 */

    if ( N >= 0 )
    {
        va_start( args, c2 );
        while ( N-- )
            resultstr = va_arg( args, char * );
        resultstr = va_arg( args, char * );
        va_end( args );    
    }

    result = resultstr;

    /* Do the concatenation */

    N = n;
    while (--len >= 0 && *c1 != '\0')
        *resultstr++ = *c1++;
    if ( N > 1 )
    {
        while (--len >= 0 && *c2 != '\0')
            *resultstr++ = *c2++;
    }
    N -= 2;

    if ( N > 0 )
    {
        va_start( args, c2 );
        while ( N-- )
        {
            charstr = va_arg( args, char * );
            while (--len >= 0 && *charstr != '\0')
                *resultstr++ = *charstr++;
        }
        va_end( args );
    }

    *resultstr = '\0';
    
    return (result);
}
