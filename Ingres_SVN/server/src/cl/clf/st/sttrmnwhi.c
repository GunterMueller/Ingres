/*
**Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/

# include	<compat.h>
# include	<gl.h>
# include	<cm.h>
# include	<st.h>

/**
** Name:	sttrmnwhi.c -	String Module Trim n White Spaces Routine.
**
** Description:
**	Delete all trailing whitespace of string up to n characters.
**	EOS terminate.
**	Return new length.  Defines:
**
**	STtrmnwhite()	trim white space from string up to n characters.
**
** History:
**		6-june-95 (hanch04)
**		    Added from sttrmwhite()
**		8-jun-95 (hanch04)
**		    change max to max_len
**
**      21-apr-1999 (hanch04)
**          replace i4 with size_t
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

size_t
STtrmnwhite(
	register char	*string,
	register size_t    max_len)
{
	register size_t	len;
	register char	*nw;
	register size_t	nwl;

	/*
	** after the loop, nw points to the first character beyond
	** the last non-white character.  Done this way because you
	** can't reverse scan a string with CM efficiently
	*/
	len = 0;
	nwl = 0;
	nw = string;
	while (*string != EOS && len < max_len)
	{
		len += CMbytecnt(string);
		if (!CMwhite(string))
		{
			CMnext(string);
			nw = string;
			nwl = len;
		}
		else
			CMnext(string);
	}

	if ( nw != string )
		*nw = EOS;
	return nwl;
}
