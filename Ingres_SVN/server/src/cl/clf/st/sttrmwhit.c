/*
**Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/

# include	<compat.h>
# include	<gl.h>
# include	<cm.h>
# include	<st.h>

/**
** Name:	sttrmwhit.c -	String Module Trim White Space Routine.
**
** Description:
**	Delete all trailing whitespace of string.  EOS terminate.
**	Return new length.  Defines:
**
**	STtrmwhite()	trim white space from string.
**
** History:
**		2-15-83   - Written (jen)
**
**		11/84 (jhw) -- "CH" macros should not be used with arguments
**			that have side effects.  Re-wrote default C routine loop
**			to reflect this fact.
**
**		12/84 (jhw) -- Added conditional compilation to force compiling
**				of the C default routine for testing.  Also,
**				fixed bug introduced by last change (should be
**				"len-- > 0" in the test so that "string" points
**				past beginning for all blanks, and returns 0
**				length when "len < 0".)
**
**		08/86 (cmr) -- Removed buggy VAX assembly-language version.
**
**		10/87 (bobm) -- effectively reimplemented to use CM.  Old
**				version scanned backwards.
**
**		10/87 (bobm) -- integrate 5.0 (correct arg types)
**		11/04/87 (dkh) - Fixed to initialize "len".
**		03/89 (jhw)  Check that some white space is to be trimmed
**			before terminating with EOS.  This allows constant
**			strings to be passed to this routine without causing
**			an access violation.
**		30-nov-92 (pearl)
**			Add #ifdef for "CL_BUILD_WITH_PROTOS" and prototyped 
**			function headers.
**		08-feb-93 (pearl)
**			Add #include of st.h.
**      8-jun-93 (ed)
**              Changed to use CL_PROTOTYPED
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	11-aug-93 (ed)
**	    unconditional prototypes
**      21-apr-1999 (hanch04)
**          replace i4 with size_t
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

size_t
STtrmwhite(
	register char	*string)
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
	while (*string != EOS)
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
