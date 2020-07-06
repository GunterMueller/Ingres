/*

NO_OPTIM = usl_us5

**Copyright (c) 2004 Ingres Corporation
*/

# include	<compat.h>
# include	<cs.h>
# include	<gl.h>
# include	<cm.h>
# include	<cv.h>
# include	<ex.h>
# include	<st.h>
# include	<me.h>
# include	<clconfig.h>
# include	<meprivate.h>

/**
** Name: CVAF.C - Functions used to convert ascii to float.
**
** Description:
**      This file contains the following external routines:
**    
**      CVaf()     	   -  convert ascii to float.
**      CVexp10()     	   -  return 10**n
**
** History:
 * Revision 1.2  91/01/14  09:27:08  source
 * sc
 * 
 * Revision 1.1  90/03/09  09:14:22  source
 * Initialized as new code.
 * 
 * Revision 1.2  90/02/12  09:39:21  source
 * sc
 * 
 * Revision 1.1  89/05/26  15:45:18  source
 * sc
 * 
 * Revision 1.1  89/05/11  01:05:02  source
 * sc
 * 
 * Revision 1.5  89/02/22  14:03:29  source
 * sc
 * 
 * Revision 1.4  89/02/17  20:22:56  source
 * sc
 * 
 * Revision 1.3  89/02/15  09:51:03  source
 * sc
 * 
 * Revision 1.1  89/01/13  17:11:19  source
 * This is the new stuff.  We go dancing in.
 * 
 * Revision 1.1  88/08/05  13:28:42  roger
 * UNIX 6.0 Compatibility Library as of 12:23 p.m. August 5, 1988.
 * 
**      Revision 1.4  88/05/02  12:37:40  bruceb
**      ifdef out the remaining EXdelete call.
**      
**      Revision 1.3  87/11/17  16:38:50  mikem
**      changed to not use CH anymore
**      
**      Revision 1.2  87/11/10  12:36:46  mikem
**      Initial integration of 6.0 changes into 50.04 cl to create 
**      6.0 baseline cl
**      
**      Revision 1.3  87/07/27  11:23:09  mikem
**      Updated to conform to jupiter coding standards.
**      
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.      
**	02-may-88 (bruceb)
**	    Ifdef-out the last of the EXdelete calls.
**
**	14-feb-89 (markd)
** 	     Changed CVexp such that it calculates the exponent
**	     first and then multiplies the exponent by the
**	     mantissa.  This produces much more accurate results
**
**	 15-feb-89 (markd)
**	     Added tweaks for more performance to CVexp based on
**	     suggestions by arana.
**	17-feb-89 (arana)
**	    Oops... Coding standards don't allow register storage
**	    class for f8's.  Removed from local var "ten" in CVexp10.
**      15-may-90 (jkb)
**          add EXmath to CVaf to force floating point
**          exceptions to be returned from the 387 co-processor when
**          they occur, rather than with the next floating point operation
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	08-jun-1995 (canor01)
**	    provide thread-local storage for cv_errno in MCT server
**	09-jun-95 (johnst)
**	    Add NO_OPTIM = usl_us5 to avoid f4, f8 normalisation bug on 
**	    Unixware 2.0 when using -K pentium optimisation flag.
**	03-jun-1996 (canor01)
**	    modified thread-local storage calls for operating system
**	    thread support.
**      22-nov-1996 (canor01)
**          Changed names of CL-specific thread-local storage functions
**          from "MEtls..." to "ME_tls..." to avoid conflict with new
**          functions in GL.  Included <meprivate.h> for function prototypes.
**	07-mar-1997 (canor01)
**	    Allow functions to be called from programs not linked with
**	    threading libraries.
**	29-may-1997 (canor01)
**	    Clean up compiler warnings.
**      27-apr-1999 (hanch04)
**          replace STindex with STchr
**      28-Jul-2003 (ashco01)
**          Bug:110588 - Check for exponent underflow in CVaf() should 
**          include compensation for number of fractional digits to 
**          prevent SIGFPE on underflow of exponent during normalization
**          within CVexp10().
**	28-Oct-2003 (malsa02) Bug 110778
**	    Check for trailing tabs (along with space characters) in 
**	    input string. For instance '12345.666<TAB>'
**/


/* # defines */
# define	AFTER_DP	   1
# define	NEGATIVE_NUMBER	   2
# define	GOT_MANTISSA	   4

# ifdef	IEEE_FLOAT
# define	MAXEXPONENT	 308
# define	MINEXPONENT	-308

# define	SIGNIF_DIGITS	  15

# else

# define	MAXEXPONENT	  38
# define	MINEXPONENT	 -38

# define	SIGNIF_DIGITS	  16
# endif

/* typedefs */
/* forward references */

VOID	CVexp10();

/* externs */
# ifdef OS_THREADS_USED
GLOBALREF ME_TLS_KEY	cv_errno_key;
# endif /* OS_THREADS_USED */

GLOBALREF i4		cv_errno;

/* statics */



/*{
** Name: CVaf	- Convert ascii to floating.
**
** Description:
**    Converts the string 'str' to floating point and stores the
**    result into the cell pointed to by 'val'.
**
**    Returns zero for ok, negative one for syntax error, and
**    positive one for overflow.
**    (actually, it doesn't check for overflow)
**
**    The syntax which it accepts is pretty much what you would
**    expect.  Basically, it is:
**    	{<sp>} [+|-] {<sp>} {<digit>} [.{digit}] {<sp>} [<exp>]
**    where <exp> is "e" or "E" followed by an integer, <sp> is a
**    space character, <digit> is zero through nine, [] is zero or
**    one, and {} is zero or more.
**
** Inputs:
**      string                          String containing the number.
**	decchar				Decimal character to use
**
** Outputs:
**      result                          Where to store the result.
**
**      Returns:
**	    OK
**	    CV_UNDERFLOW
**	    CV_OVERFLOW
**	    CV_SYNTAX
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History: 
**	08/04/85 -- (jrc)
**		added decchar for international support.
**	02-12-1986 (joe)
**		Took the INGRES 3.0 CL version to INGRES 4.0 CL and
**		added international support.  The first 4.0 version
**		was not correct.
**	06-03-86 (eda) bug # 9145
**		Removed statement that was setting v=1.0 if there was no
**		mantissa.  The case where no mantissa is found in the string,
**		it can assumed to be zero.
**      10-09-86 (nar) bug #10172 
**		When CVexp10 is called to divide by 10 to get fraction value
**	        for number converted from ascii, precision is lost.  
**	09-11-86 (greg)
**		Merge VMS and UNIX versions to pick up bug fixes from both sides
**		Specifically 10272, 9145 and 7867.
**	10-16-86 (linda)
**		Take out fix for bug #10272 on Unix side; it loses precision on
**		this architecture.
**	01-oct-1985 (derek)
**          Created new for Jupiter.
**	08-sep-86 (thurston)
**	    Re-did Jeff's decimal parameter integration to be more efficient.
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.
**
**		Revision 1.2  86/04/29  09:37:08  daveb
**		remove unused variable "problem"
**
**		Revision 1.2  86/02/06  13:37:45  joe
**		Fix for bug 7867.  Don't accept . if decchar is set and
**		is not '.'.
** 
**		Revision 30.3  85/09/30  20:31:39  cgd
**		New version of file from ibs integration.  Includes ifdef'ed
**		exception handling code for UNIX, and does away with the need
**		to call MHexp10 (either the MH routine or the 3B specific
**		routine at the end of the old cvaf.c) since simple *=10.0 or
**		/=10.0 operations are used by a new routine CVexp10() only if
**		[UNDER,OVER]FLOW will not occur.
**		Also includes international support in the form of a new
**		parameter decchar.
**      28-July-2003 (ashco01)
**          Bug:110588 - Check for exponent underflow in CVaf() should
**          include compensation for number of fractional digits to
**          prevent SIGFPE on underflow of exponent during normalization
**          within CVexp10().
*/
STATUS
CVaf(char *str, char decchar, f8 *val)
{
	register char	*p = str;
	register i4	cnt = 0;
	STATUS		status = OK;
	i4		frac_digits = 0;
	i4		exponent = 0;
	int		digits = 0;
	i4		zero_cnt;	/* number of consecutive zeros found */
	i4		exp_adj = 0;	/* number to increase exponent */
	double		v = 0.0;
	int		normalizer = 0;	/* exponent multiplier to
					**	normalize data
					*/
	char		buf[256];	/* In case str is readonly */
	char		dbuf[4];	/* In case str is readonly */
# ifdef UNIX
	bool		exception_raised;
	extern	i4	CVexhandler();
	EX_CONTEXT	context;
	i4		*cv_errno_ptr = NULL;
# ifdef OS_THREADS_USED
	static  int	once;
# endif /* OS_THREADS_USED */
# endif /* UNIX */

# ifdef UNIX
# ifdef OS_THREADS_USED
	if (!once)
	{
	    ME_tls_createkey( &cv_errno_key, &status );
	    if ( status )
		cv_errno_ptr = &cv_errno;
	    once++;
	}
	if ( cv_errno_key == 0 )
	    cv_errno_key = -1;
# endif /* OS_THREADS_USED */
	/* Exception handling for over and underflow
	**
	**	Exception handler will set cv_errno. It will then return
	**	to line following the EXdeclare.  If it does catch an ex-
	**	ception and returns the CVaf.c will exit with the error number.
	*/
# ifdef OS_THREADS_USED
	if ( cv_errno_key == -1 )
	{
	    cv_errno_ptr = &cv_errno;
	}
	else
	{
	    if ( cv_errno_ptr == NULL )
	        ME_tls_get( cv_errno_key, (PTR *)&cv_errno_ptr, &status );
	    if ( status != OK )
	    {
	        cv_errno_ptr = &cv_errno;
	        status = OK;
	    }
	    if ( cv_errno_ptr == NULL )
	    {
		    cv_errno_ptr = (i4 *) MEreqmem( 0, sizeof(i4), 0, NULL );
		    ME_tls_set( cv_errno_key, (PTR)cv_errno_ptr, &status );
	    }
	}
	if ( cv_errno_ptr )
	    *cv_errno_ptr = 0;
# else  /* OS_THREADS_USED */
	cv_errno_ptr = &cv_errno;
	*cv_errno_ptr = 0;	/* reset error number */
# endif /* OS_THREADS_USED */
	exception_raised = FALSE;
	EXdeclare(CVexhandler, &context);

	if (exception_raised)
	{
		/* the handler caught an exception -- return the error */
		EXdelete();
		if (cv_errno_ptr)
		    return(*cv_errno_ptr);
		else
		    return(FAIL);
	}

	/* set flag so that above "if" statement will work if it is reentered */
	exception_raised = TRUE;
# endif /* UNIX */

# ifdef DESKTOP
__try {
# endif /* DESKTOP */
	if (STlength(str) > 255)
	{
# ifdef UNIX
		EXdelete();
# endif
		return(CV_SYNTAX);
	}

	STcopy(str, buf);

 	if (decchar != '\0')
 	{
		dbuf[0] = decchar;
		dbuf[1] = EOS;
 		if ((p = STchr(buf, *dbuf)) != NULL)
		{
 			*p = '.';
		}
 		else if (decchar != '.' && STchr(buf, '.') != NULL)
 		{
# ifdef UNIX
 			EXdelete();
# endif
 			return CV_SYNTAX;
 		}
 	}

	p = buf;
	str = buf;

	/*	scan past blanks collecting the sign and stop after
	**	first non-blank after sign
	*/

	while (*p++ == ' ')
		;

	if (*--p == '-' || *p == '+')
	{
		if (*p++ != '+')
			status |= NEGATIVE_NUMBER;

		while (*p++ == ' ')
			;

		--p;
	}

	while(CMdigit(p) && *p == '0')
		p++;

	/*	collect the number: remember number of digits
	**	after . and suppress trailing zero's
	*/

	for (cnt = 0;; p++, status |= GOT_MANTISSA)
	{
		if (CMdigit(p))
		{
			if (*p != '0' && (digits + frac_digits) < SIGNIF_DIGITS)
			{
				/* If this number is less than one, remember the
				** number of leading zeros after the exponent.
				** These will not be multiplied here. They will
				** be subtracted from the exponent.
				*/
				if (cnt && v == 0.0 && (status & AFTER_DP))
				{
					normalizer = -cnt;
					zero_cnt = 0;
					cnt = 0;
				}
				else
				{
					for (zero_cnt = cnt;
					     cnt > 0;
					     v *= 10.0, cnt--)
					{
						;
					}
				}
			}
			else
			{
				if (!(status & AFTER_DP))
					exp_adj++;

				cnt++;

				continue;
			}

			if (!(status & AFTER_DP))
			{
				exp_adj = 0;
				digits += zero_cnt + 1;
			}
			else
			{
				/* subtract from frac those zeros in the
				**	whole number
				*/
				frac_digits += zero_cnt + 1 - exp_adj;
				exp_adj = 0;
			}

			v = v * 10.0 + (*p - '0');
		}
		else if (*p == '.')
		{
			if (status & AFTER_DP)
			{
				break;
			}
			/* add in the trailing whole number zeros. Subtracted
			** out if no number > 0 in the fraction
			*/
			digits += cnt;
			status |= AFTER_DP;
		}
		else
		{
			if ((status & AFTER_DP) == 0)
			{
				/* add in the trailing whole number zeros.
				** Subtracted out if no number > 0 in the
				** fraction
				*/
				digits += cnt;
			}
			break;
		}
	}

	/* skip blanks and tabs before possible exponent */

	/* 
	**	(bug 110778)
	*/

	for(; ((*p == ' ') || (*p == '\t')) ; p++)
		;
	p++;

	/* test for exponent */

	if (*--p == 'e' || *p == 'E')
	{
		if (CVal(++p, &exponent))
		{
# ifdef UNIX
			EXdelete();
# endif
			return (CV_SYNTAX);
		}
/*
**		This is bug fix 9145 from VMS.
**
**		if ((status & GOT_MANTISSA) == 0)
**			v = 1.0;
*/
	}
	else if (*p != '\0')
	{
# ifdef UNIX
		EXdelete();
# endif
		return (CV_SYNTAX);
	}

	exponent += exp_adj;
	digits -= exp_adj;
	/* size of implied exponent. 10e20 is really 1e21
	** if negative normalizer, digits is zero
	*/
	normalizer += digits - 1;

	/* If the normalizer is less than 0, then digits is zero.
	** If the number were greater than 1, digits whould be greater
	** than 1 and the normalizer would be greater than 1.
	*/
	if (normalizer < 0)
	{
		exponent += normalizer;
		frac_digits -= 1;	/* we have added to the exponent.
					** Don't subtract as much later
					*/
		normalizer = 0;
	}

	/* exponent + number of decimals to normalize number */
	if (
		(exponent + normalizer) > MAXEXPONENT
		||
		(
			(exponent + normalizer) == MAXEXPONENT 
			&& 
			v > 1.0
		)
	   )
	{
# ifdef UNIX	
		EXdelete();
# endif
		return (CV_OVERFLOW);
	}

	/* If x > 1e-38 then its OK. After normalization the mantissa is
	** always greater than 1. Therefore, we only need to check the exponent.
	*/
	/* Include fractional digits in underflow check */
	if ((exponent + normalizer - frac_digits) < MINEXPONENT)	
	{
# ifdef UNIX
		EXdelete();
# endif
		return (CV_UNDERFLOW);
	}

	/* compensate for fractional digits */
	if (exponent - frac_digits)
		CVexp10((i4) exponent - frac_digits, &v);

	/* store the result and exit */
	if (status & NEGATIVE_NUMBER)
		v = -v;

	*val = v;
# ifdef DESKTOP
}
__except (EXCEPTION_EXECUTE_HANDLER)
{
	switch (GetExceptionCode())
	{
		case EXCEPTION_FLT_OVERFLOW:
			return CV_OVERFLOW;
		case EXCEPTION_FLT_UNDERFLOW:
			return CV_UNDERFLOW;
		default:
			return FAIL;
	}
}
# endif /* DESKTOP */
# ifdef UNIX
	EXmath(EX_OFF);
	EXdelete();
# endif
	return (OK);
}

VOID
CVexp10(i4 exp, f8 *val)
{
	f8	ten = 10.0;
	f8	exponent = 1.0;
	if (exp > 0)
	{
		do
		{
			exponent *= ten;
		}
		while (--exp > 0);
		*val *= exponent;
	}
	else
	{
		while (exp++ < 0)
		{
			exponent *= ten;
		}
		*val /= exponent;
	}
}
