/******************************************************************************
**
**	Copyright (c) 1983, 1991 Ingres Corporation
**
******************************************************************************/

# include	<compat.h>
# include	<pc.h>

/******************************************************************************
**
** PCunique -- generate a unique character string for a given user.
**
******************************************************************************/
VOID
PCunique(numb)
char	*numb;
{
    ultoa(GetCurrentProcessId(), numb, 16);
}
