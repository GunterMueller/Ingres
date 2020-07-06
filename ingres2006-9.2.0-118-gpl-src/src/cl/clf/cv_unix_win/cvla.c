/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cvcl.h>

/**
** Name: CVLA.C - Convert long to ascii.
**
** Description:
**      This file contains the following external routines:
**    
**      CVla()             -  Long to ascii
**
** History:
 * Revision 1.1  90/03/09  09:14:24  source
 * Initialized as new code.
 * 
 * Revision 1.2  90/02/12  09:40:02  source
 * sc
 * 
 * Revision 1.1  89/05/26  15:45:38  source
 * sc
 * 
 * Revision 1.1  89/05/11  01:05:22  source
 * sc
 * 
 * Revision 1.1  89/01/13  17:11:21  source
 * This is the new stuff.  We go dancing in.
 * 
 * Revision 1.1  88/08/05  13:28:48  roger
 * UNIX 6.0 Compatibility Library as of 12:23 p.m. August 5, 1988.
 * 
**      Revision 1.2  87/11/10  12:37:28  mikem
**      Initial integration of 6.0 changes into 50.04 cl to create 
**      6.0 baseline cl
**      
**      Revision 1.3  87/07/27  11:23:39  mikem
**      Updated to conform to jupiter coding standards.
**      
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.      
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	17-jan-1996 (toumi01; from 1.1 axp_osf port) (schte01)
**	    adding <cvcl.h> after <gl.h>. Added routine CVla8 for 8 byte
**	    conversion.
**	04-Feb-2002 (rigka01) bug #106435
**	    Add CVula() which is CVla() plus modifications for unsigned data 
**	24-dec-03 (inkdo01)
**	    Changed long to longlong in cvla8().
**	10-Jan-2004 (schka24)
**	    Let's go for i8, more portable.
**	21-jan-04 (hayek02)
**	    Change the type of the input integer,i, from long to longlong in
**	    CVCla8() - this will allow true 64 bit integers to be converted
**	    to scaii, and the argument will now match the type in cv.h.
**	20-Jul-2004 (lakvi01)
**		SIR #112703, cleaned-up warnings.
**	26-Jul-2004 (schka24)
**	    Fix breakage from above, be more careful with what is cast.
**      26-Jul-2004 (lakvi01)
**          Backed-out the change for SIR 112703 to keep the open-source stable.
**          Will be revisited and submitted at a later date.
**	14-Jul-2005 (schka24)
**	    Remove x-integration dirt.
**/

/* # defines */
/* typedefs */
/* forward references */
/* externs */
/* statics */


/*{
** Name: CVla      - Convert long to ascii.
**
** Description:
**	Long to ascii
**
**	Converts the long integer num to ascii and stores the
**	result in string, which is assumed to be large enough.
**
** Inputs:
**	i				    number to convert.
**
** Output:
**	a				    ascii buffer to fill in.
**
**      Returns:
**	    none. (VOID
**
** History:
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.
*/
VOID
CVla(i, a)
register i4	i;
register char	*a;
{
	register char	*j;
	char		b[12];

	if (i < 0)
	{
		*a++ = '-';
		i = -i;
	}
	j = &b[11];
	*j-- = 0;

	/* Need to accomadate the most negative number (-2147483648).  Since
	** -(-2147483648) = -2147483648 in 2's complement, i is still negative
	** and i % 10 is negative also.  Therefore use -(i % 10) for this case,
	** and divide by -10 to get absolute value of i.
	**
	** Note that more negative numbers wrap around to positive numbers when
	** negated on the VAX as expected, but are mapped to (-2147483648) on 
	** the SUN.  1/23/84 (tpc)
	*/

	if (i < 0)	/* catch -2147483648 */
	{
		*j-- = -(i % 10) + '0' ;
		i /= -10;
	}

	do
	{
		*j-- = i % 10 + '0' ;
		i /= 10;
	} while (i);

	do
	{
		*a++ = *++j;
	} while (*j);
	return;
}

/*
** On axp_osf the following are defined in <machine/machlimits.h>
** LONG_MAX	 9223372036854775807  max value for a long 
** ULONG_MAX    18446744073709551615U  max value for an unsigned long 
*/
VOID
CVla8(i, a)
register i8 	i;
register char	*a;
{
	register char	*j;
	char		b[23];

	if (i < 0)
	{
		*a++ = '-';
		i = -i;
	}
	j = &b[22];
	*j-- = 0;

	/* Need to accomodate the most negative number (-922...5808).  Since
	** -(-922...5808) = -922...5808 in 2's complement, i is still negative
	** and i % 10 is negative also.  Therefore use -(i % 10) for this case,
	** and divide by -10 to get absolute value of i.
	**
	*/

	if (i < 0)	/* catch two-complement -infinity */
	{
		*j-- = -(i % 10) + '0' ;
		i /= -10;
	}

	do
	{
		*j-- = i % 10 + '0' ;
		i /= 10;
	} while (i);

	do
	{
		*a++ = *++j;
	} while (*j);
	return;
}

/*{
** Name: CVula      - Convert unsigned long to ascii.
**
** Description:
**	Unsigned Long to ascii
**
**	Converts the unsigned long integer num to ascii and stores the
**	result in string, which is assumed to be large enough.
**
** Inputs:
**	i				    number to convert.
**
** Output:
**	a				    ascii buffer to fill in.
**
**      Returns:
**	    none. (VOID
**
** History:
**      04-Feb-2002 (rigka01) bug #106435
**          Similar to CVla but CVula is for unsigned data 
*/
VOID
CVula(i, a)
register u_i4 i;
register char	*a;
{
	register char	*j;
	char		b[12];

	j = &b[11];
	*j-- = 0;

	do
	{
		*j-- = i % 10 + '0' ;
		i /= 10;
	} while (i);

	do
	{
		*a++ = *++j;
	} while (*j);
	return;
}

/*{
** Name: CVula8      - Convert unsigned i8 to ascii.
**
** Description:
**      Unsigned i8 to ascii
**
**      Converts the unsigned i8 num to ascii and stores the
**      result in string, which is assumed to be large enough.
**
** Inputs:
**      i	number to convert.
**
** Output:
**      a	ascii buffer to fill in.
**
**      Returns:
**          none. 
**
** History:
**      05-Sep-2004 (zhahu02)
**          Updating for a typo of previous change.
**      21-Feb-2005 (thaju02) 
**          Similar to CVla8 but for unsigned. 
*/
VOID
CVula8(i, a)
register u_i8 	i;
register char	*a;
{
	register char	*j;
	char		b[23];

	j = &b[22];
	*j-- = 0;

	do
	{
		*j-- = i % 10 + '0' ;
		i /= 10;
	} while (i);

	do
	{
		*a++ = *++j;
	} while (*j);
	return;
}
