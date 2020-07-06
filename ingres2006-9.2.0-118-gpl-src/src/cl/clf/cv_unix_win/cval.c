/*
**Copyright (c) 2004 Ingres Corporation
*/

#include	<compat.h>
#include	<gl.h>
#include	<cv.h>
#include	<me.h>

#ifdef hpb_us5
#include	<inttypes.h>
#endif
/*
**  Name: CVAL.C - Functions used to convert ascii to long
**
**  Description:
**      This file contains the following external routines:
**    
**      CVal()             -  convert ascii to long.
**
**  History:
**      Revision 1.3  87/11/17  16:54:13  mikem
**      changed to not use CH anymore
**      
**      Revision 1.3  87/07/27  11:23:19  mikem
**      Updated to conform to jupiter coding standards.
**      
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.      
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	17-jan-1996 (toumi01; from 1.1 axp_osf port) (schte01)
**	    added routine CVal8 for 8 byte conversion.
**	29-may-1996 (toumi01)
**	    Make CVal8 axp_osf platform-conditional and replace inclusion
**	    of <stdlib.h> with declaration "extern long int atol ();" to
**	    avoid dragging in other definitions (such as "abs()") should
**	    this code be used in the future for other 64-bit platforms.
**	    Delete unnecessary inclusion of <cm.h>.
**      22-mar-1999 (hweho01)
**          Use CVal8 for ris_u64 (AIX 64 bit platform).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	08-feb-2001 (somsa01)
**	    Define CVal8() for all LP64 platforms as well.
**      16-Nov-2000 (ahaal01)
**          Use CVal8 for rs4_us5 (AIX 32 bit platform).
**      04-Nov-2002 (hweho01)
**          Added change for AIX hybrid build.
**	14-oct-2003 (somsa01)
**	    On Windows 64-bit, use _atoi64() in CVal8(). Also, changed
**	    result type to be OFFSET_TYPE for CVal8 as well.
**	2-Jan-2004 (schka24)
**	    Use i8 for CVal8.
**	27-Jan-2004 (fanra01)
**	    Use strtol function on windows.
**	27-Jan-2004 (fanra01)
**	    Use _strtoi64 instead of strtol.
**	    Forward reference for stroll taken from standard C header.
**	    long long is not accepted on earlier versions of the MS C compiler.
**	04/01/2004 (gupsh01) 
**	    Fix CVal8, which now checks if the conversion was successful, 
**	    else returns CV_SYNTAX.
**	02-apr-2004 (somsa01)
**	    Use __strtoll() on HP-UX.
**	04/08/2004 (gupsh01)
**	    Fixed CVal8, it does not cover the case where input string 
**	    is character '0', which will be converted to 0 in the result.
**	04/08/2004 (gupsh01, schka24 )
**	    Fixed CVal8 again to take care of more general case. 
**	16-apr-2004 (somsa01)
**	    Include inttypes.h on HP, so that we properly pick up the
**	    definition of __strtoll().
*/

/* # defines */
/* typedefs */
/* forward references */
/* externs */
/* statics */


/*{
** Name:	CVal - CHARACTER STRING TO 32-BIT INTEGER
**		       CONVERSION
**
** Description:
**    ASCII CHARACTER STRING TO 32-BIT INTEGER CONVERSION
**
**	  Convert a character string pointed to by `a' to an
**	i4 pointed to by `i'. A valid string is of the form:
**
**		{<sp>} [+|-] {<sp>} {<digit>} {<sp>}
**
** Inputs:
**	a	char pointer, contains string to be converted.
**
** Outputs:
**	i	i4 pointer, will contain the converted long value
**
**   Returns:
**	OK:		succesful conversion; `i' contains the
**			integer
**	CV_OVERFLOW:	numeric overflow; `i' is unchanged
**	CV_UNDERFLOW:	numeric underflow; 'i' is unchanged
**	CV_SYNTAX:	syntax error; `i' is unchanged
**	
** History:
**	03/09/83 -- (mmm)
**		brought code from vms and modified.   
**	19-june-84 (fhc) -- made work after absent history broke it
**	19-mar-87 (mgw)	Bug # 11825
**		Made it work as it does in Unix and IBM so that in particular,
**		it will now return CV_SYNTAX for "5 3 4" instead of
**		converting it to 534.
**	31-mar-87 (mgw)
**		Made it work for MINI4 (= -MAXI4 -1).
**	10-jan-89 (daveb)
**		Take (bj-icl)'s 17-nov-89 mod to use unsigned when checking
**		for overflow:
**	11-may-90 (blaise)
**		Integrated changes from ug into 63p library.
**	21-mar-91 (jrb)
**		Bug 36133. A negative number with leading zeros was causing
**		this routine to incorrectly return with CV_UNDERFLOW.
*/
STATUS
CVal(a, i)
register char	*a;
i4		*i;
{
	register i4	sign;	/* flag to indicate the sign */
	register unsigned long	x;	/* holds the integer beeing formed */
	char		c;

	sign = 0;

	/* skip leading blanks */
	for (; *a == ' '; a++)
		;

	/* check for sign */
	switch (*a)
	{
	  case '-':
		sign = -1;

	  case '+':
		while (*++a == ' ')
			;
	}

	/* at this point everything had better be numeric */
	for (x = 0; *a <= '9' && *a >= '0'; a++)
	{
		if (sign == 0)	/* positive value */
		{
			/* check for overflow */

			if (x > MAXI4 / 10 || 
				(x = x * 10 + (*a - '0')) > MAXI4 )
			{
				/* positive overflow */
				return (CV_OVERFLOW);
			}
		}
		else	/* (sign == -1) negative value */
		{
			/* check for underflow */

			if ( (long)x < MINI4 / 10 || 
			     ((x = x * 10 - (*a - '0')) < (unsigned long) MINI4
			     &&  x != 0) )
			{
				/* negative overflow */
				return (CV_UNDERFLOW);
			}
		}
	}

	/* eaten all the numerics; better be all blanks */
	while (c = *a++)
		if(c != ' ')			/* syntax error */
			return (CV_SYNTAX);

	*i = x;

	return (OK);		/* successful termination */
}

STATUS
CVal8(str, result)
register char   *str;
i8		*result;
{
char c;
char   *endstr;

# if defined(NT_GENERIC) || defined(NT_IA64) || defined(NT_AMD64)
    *result = _strtoi64( (const char *) str, &endstr, 10 );
# elif defined(LP64) || defined(axp_osf) || defined(ris_u64)
    *result = strtol( (const char *) str, &endstr, 10 );
# elif defined(hpb_us5)
    *result = __strtoll( (const char *) str, &endstr, 10 );
# else  /* NT_GENERIC */    
    *result = strtoll( (const char *) str, &endstr, 10 );
# endif /* NT_GENERIC */    
    
    if (endstr == str)
      return (CV_SYNTAX);
    else
      /* terminator character must be null or space */
      while ( (c = *endstr++) != '\0')
        if (c != ' ')
	  return (CV_SYNTAX);

     return (OK);		/* successful termination */
}
