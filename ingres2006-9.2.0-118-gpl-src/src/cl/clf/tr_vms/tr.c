/*
** Copyright (c) 1985, 2005 Ingres Corporation
**
*/

/*
NO_OPTIM=rtp_us5
*/

#include    <compat.h>
#include    <gl.h>
#include    <bt.h>
#include    <tr.h>
#include    <cv.h>
#include    <lo.h>
#include    <si.h>
#include    <st.h>
#include    <tm.h>
#include    <me.h>
#include    <starlet.h>

#if !defined(__STDARG_H__)
#include    <varargs.h>
#endif

/**
**
**  Name: TR.C - Tracing compatibilty library routines.
**
**  Description:
**      This file contains the trace routines defined by the CL. 
**      These routines provide tracing primitives used to facilitate 
**      debugging and testing.
**
**          TRdisplay - Write trace output.
**	    TRflush - Flush buffers to disk.
**          TRgettrace - Test trace flag
**          TRmaketrace - Setup a trace vector.
**          TRrequest - Read trace input.
**          TRset_file - Set the trace input/output.
**          TRsettrace - Set trace flag
**
** History:
**      Revision 1.5  88/07/01  10:53:08  mikem
**      remove newline for TM string.
**      
**      Revision 1.4  88/02/16  14:31:21  anton
**      user specifiable prompt for TRrequest
**      
**      Revision 1.3  87/12/31  13:16:18  anton
**      new tr semantics - closer to VMS
**      
**      Revision 1.2  87/11/30  14:39:57  mikem
**      changed to use 6.0 version of STindex
**      
**      Revision 1.1  87/11/10  16:04:41  mikem
**      Initial revision
**      
**      Revision 1.3  87/07/27  10:42:24  mikem
**      Updated to conform to jupiter coding standards.
**      
**      30-sep-1985 (derek)    
**          Created new for 5.0.
**	06-aug-1986 (Derek)
**	    Fix problem with date format.
**      08-aug-1986 (seputis)
**          Added floating point support
**	21-aug-1986 (Derek)
**	    Added indirect fetch of width and precision.
**	16-jul-1987 (mmm)
**	    ported to UNIX - use ifdef's for now to facilitate diffs with vms
**	    system which still seems to be changing.
**	17-Oct-1988 (daveb)
**	    readonly isn't legal.  Make the initialization readable.
**	18-oct-1988 (seputis)
**	    fix floating point problem, pass fmt char correctly
**	30-Nov-1988 (russ)
**	    Modified TRdisplay, TRformat, and the underlying do_format
**	    function to use varargs.  Also, TRdisplay now formats the
**	    data only if it is actually going to be printed.
**	04-Apr-89 (GordonW)
**	    added MECOPY_{VAR/CONST}_MACRO call
**	13-Apr-1989 (fredv)
**	    Added ming hint not to optimize this file for rtp_us5.
**	26-Apr-89 (GordonW)
**	    TRdisplay returns a STATUS
**	02-May-89 (GordonW)
**	    remove mecopy.h include because it is already included in me.h
**      16-Apr-90 (Kimman)
**          Using MEcopy for ds3_ulx because compiler has bug and gets
**          confused with to many defines in SET_ARG.
**	08-Aug-90 (GordonW)
**	    TERMCL changes, change # 20511.
**	01-Nov-90 (anton)
**	    Reentrancy protection added
**	14-dec-90 (mikem) restore lost history comments:
**    	    03-Aug-1990 (mikem)
**          Mainline currently passes "NULL" pointer as a "fcn" when it
**          doesn't want any function executed.  Changed TRformat to check
**          for a NULL function pointer before using it.
**	11-mar-91 (rudyw)
**	    Comment out text following #endif.
**	25-jun-1991 (bryanp)
**	    Don't suppress zero-length lines.
**	27-may-1992 (jnash)
**	    Add ability to append to end of file in TRset_file via
**	    TR_F_APPEND.
**	22-june-1992 (jnash)
**	    TR_NOOVERRIDE_MASK added to TRset_file flag param.
**	12-nov-1992 (seiwald)
**		Unix version ported for Alpha.
**      29-jul-1993 (huffman)
**           AXP integration for 7-26.  (hand done)
**	30-jun-1995 (duursma)
**	    Restored original (VAX) logic for dealing with %? that was somehow
**	    lost during a poorly documented integration of this file.  This 
**	    fixes the problem where infodb and logdump print completely bogus
**	    date/times.  Also commented the fact that ? is a format character.
**	15-aug-1996 (kamal03) intgrated Unix change.
**		11-Jul-1996 (jenjo02)
**	    Fixed printing of '%'. '%%' produced nothing.
**      20-aug-1997 (teresak) integrated Unix change
**	    17-oct-96 (mcgem01)
**          Changed the length of the "format_item" buffer from 128 to 256
**          to facilitate RAAT trace messages.
**      19-may-99 (kinte01)
**          Added casts to quite compiler warnings & change nat to i4
**          where appropriate
**      19-may-99 (kinte01)
**          04-may-1999 (hanch04) - Unix change 441753
**            Change TRformat's print function to pass a PTR not an i4.
**	24-may-2000 (kinte01)
**	    Update for Compaq C 6.2  --- put comment lines around label
**	    on endif preprocessor directive
**	19-jul-2000 (kinte01)
**	    Correct prototype definitions by adding missing includes
**	01-dec-2000	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from VMS CL as the use is no longer allowed
**	04-jun-2001 (kinte01)
**	   Replace STindex with STchr
**	10-dec-2001 (kinte01)
**	   Clean up compiler warnings
**	10-dec-2001 (kinte01)
**	    Update length of "format_item" to 2048 since we have messages 
**	    longer than 128 bytes
**      30-jul-2002 (horda03) Bug 106435
**          Added 'u' as a valid TRdisplay datatype for unsigned data.
**	04-feb-2003 (kinte01) 
**	    Cross-integration from 2.0 forgot to remove longnat and
**	    replace with i4
**	09-jun-2003 (devjo01)
**	    Add support for TR_F_SAFEOPEN. b110302.
**	06-aug-2003 (abbjo03)
**	    Change longnat's in 30-jul-2002 change to i4's.
**	22-apr-2005 (devjo01)
**	    Make write_line a public function "TRwrite" to be compatible
**	    with the UNIX/Windows implementation.
**/

/*}
** Name: CONTEXT - Trace context
**
** Description:
**      This structure contains information about trace files.  This context 
**      is allocated by the caller but interpreted by these routines.
**
** History:
**     30-sep-1985 (derek)
**          Created new for 5.0.
*/

typedef struct _CONTEXT
{
    int		    context_id;		/*  Constant to verify argument. */
#define			CONTEXT_ID	'TRIO'
    char	    stdioflag;		/* flags indicateing stdio use in the following array */
    FILE	    *fp_array[3];
#define                 FP_FILE		0
#define			FP_TERMINAL	1
#define			FP_INPUT	2
}   CONTEXT;


/*
**  Definition of static variables and forward static functions.
*/         

static	CONTEXT	    tr_default;

static	i2	    *trace_vector;	/* Current trace vector. */	    
static	char	    tr_buffer[131];	/* TRdisplay output buffer. */
static	i4	    tr_offset;		/* Current length of buffer. */
static  VOID	    do_format();

# ifdef VMS
# define SETCLERR( e, x, y ) ( e->error = x )
# endif /*VMS*/

struct argmng {
    char    loc_buf[1024];
    PTR	    dataptr;		/* area to hold actual value of args */
    PTR	    argptr[256];	/* pointers to args within dataptr */
    i4	    next_arg;		/* next available slot in argptr */
};

/*{
** Name: TRdisplay	- Write trace output.
**
** Description:
**      Characters from the format string are passed to the trace terminal and/or the
**	trace file.  If a format specifier is recognized parameters are read from the
**	argument list and are displayed in the form described by the format specifier.
**	All output is formatted into 132 characters lines and long lines are broken
**	into multiple lines.
**
**	A format specifier is of the following form:
**
**	    % [-] [ width ] [. [ size ] ] directive
**
**		Where width and size are decimal number or '#' or '*'.  The '#'
**		means get the value from the parameter list.  The '*' means
**		get the value indirectly from the parameter list, or get the
**		value from a offset within a structure or array if currently
**		in a structure or array.  The optional leading '-' controls 
**		the automatic deletion of trailing blanks and the corresponding
**		shrinking of the field width.  This is most useful when
**		formatting fixed size text strings with potential for trailing
**		blanks.
** 
**	A formatting directive is one of the following:
**
**	    b	Print base address of structure.  Width means how wide a field to
**		use in displaying the string, if none is given as many characters
**		as needed to display the words are used.  Size is not used.
**
**	    c	Format a text character.  If the character is not printable a 
**		'.' is substituted.  Width and size are ignored.  One parameter
**		is used, this is the character to format.  If this is in a structure
**		the parameter is used as an offset from the base address of the
**		structure.
**
**	    d	Format an integer in decimal.  Width means how wide of a field to
**		use in displaying the number, if none is given as many characters
**		as needed to display the number are used.  Size means how large
**		an integer to get when displaying values in structures or arrays.
**		One parameter is used, this is the number to format.  If this is in
**		a structure then the parameter is used as an offset from the base
**		address of the structure.
**
**          e   The float or double arg is converted in the style [-]d.dddeSdd,
**              where there is one digit before the decimal point and the number
**              of digits after it is equal to the precision;  when the 
**              precision is missing, a default of 3 is used; if the precision
**              is zero, no decimal point appears.  The exponent always contains
**              at least two digits.
**
**          f   The float or double arg is converted to decimal notation in the
**              style [-]ddd.ddd, where the number of digits after the decimal
**              point is equal to the precision specification.  If the precision
**              is missing 3 decimal digits are output; if the precision is
**              explicitly 0, no decimal point appears.
**
**	    g	The float or double arg is converted to decimal notation in the
**		sytle [-]ddd.ddd, where the number of digits after the decimal
**		point is equal to the precision specification.  If the precision
**		is missing, 3 decimal digits are output; if the precision is
**		explicitly 0, no decimal point appears.  If the number
**		is a display of zero, the number is displayed in 'e' format.
** 
**	    g	The float or double arg is converted to decimal notation in the
**		sytle [-]ddd.ddd, where the number of digits after the decimal
**		point is equal to the precision specification.  If the precision
**		is missing, 3 decimal digits are output; if the precision is
**		explicitly 0, no decimal point appears.  If the number
**		is a display of zero, the number is displayed in 'e' format with
**		the decimal points aligned.
** 
**	    s	Format a C character string.  Width means how wide a field to use in
**		displaying the string,  if none is given as many characters as needed to
**		display the string are used.  If size is given it is assumed to be the
**		length of the string, otherwise a null character terminates a string.
**		One parameter is used, this is a pointer to the string to format.  If
**		this is in a structure then the parameter is used as an offset from
**		the base address of the structure to get the pointer.
**
**	    t	Format a text string.  Width means how wide a field to use in
**		displaying the string, if none is given as many characters as needed to
**		display the string are used.  Size is ignored.  The first parameter
**		is the length of the string, the second parameter is a pointer to the
**		string.  There is no special meaning when inside a structure.
**
**	    v	Format a bit field into words. Width means how wide a field to use
**		in displaying the string, if none is given as many characters as needed
**		to display the words are used.  Size is used inside a structure to
**		determine the size of the integer to fetch.  The first parameter is
**		a character string containing words separated by ','.  The words from
**		left to right correspond to the bits from least significant to most
**		significant.  If the bit is on,  the word is copied to the output.
**		The second parameter is the value to use, or the offset from the base
**		address of the structure.
**
**	    w	Format an integer into a word.  Width means how wide a field to use
**		in displaying the string, if none is given as many characters as needed
**		to display the words are used.  Size is used inside a structure to
**		determine the size of the integer to fetch.  The first parameter is a
**		character string containing words separated by ','.  The words from
**		left to right correspond to the numbers 0 through whatever.  The second
**		parameter is the value to use, or the offset from the base address
**		of the structure.
**
**	    x	Format an integer in hexidecimal.  Width means how wide of a field to
**		use in displaying the number,  if none is given as many characters
**		as needed to display the number are used.  Size means how large of an 
**		integer to get when displaying values in structures or arrays.
**		One parameter is used, this is the number to format.  If this is in
**		a structure then the parameter is used as an offset from the base
**		address of the structure.
**
**	    ?   Format the argument as a DB_TAB_TIMESTAMP (not as a SYSTIME)
**
**	    @	Format the current time and date.  Width means how wide of field to
**		use in displaying the time and date.  This takes no parameters.
**
**	    (	Start repeating group.  Width means how many times to repeat the 
**		format items following this until the %).  A repeating group can not
**		be used inside an array or structure.
**
**	    )	End repeating group.  Terminates a repeated format group.
**
**	    <	Backup parameter.  If width is set this backs up the corresponding
**		number of parameters otherwise it backs up one parameter.
**
**	    >	Skip parameter.  If width is set this skips the corresponding number
**		of parameters, otherwise it skips one parameter.
**
**	    *?	Repeat character. Width determines how may times the characters
**		following the * is copied to the output.
**
**	    %	Print a '%'.
**
**	    [	Start array of pointers.  Width is used to tell how many pointers to
**		follow.  The first parameter is the address of the array of pointers.
**		If this is in a structure,  this is an offset to the pointer to the
**		array of pointers.  The format and the parameter are rescanned with
**		the next pointer until all pointers are exhausted.  This can be nested
**		within an array, structure or repeat group.
**
**	    ]	End array of pointers.  Delimits an array of pointers.
**
**	    {	Start array of structures.  Width is used to tell how many
**		structures to display. Size is the size of the structure item so
**		that it can increment to the next item.  The first parameter is the
**		address of the structure.  If this is in a structure this is an
**		offset to the structure to be displayed.  The format and the
**		parameters are rescanned with the next structure address until all
**		structures have been displayed.  This can be nested within an array,
**		structure or repeat group.
**
**	    }	End array of structures.  Delimits an array of structures.
**
** Inputs:
**      format				String describing the formatting actions
**					to perform.
**      parameter			Variable number of arguments to substitite
**					into the output string.
** Outputs:
**	Returns:
**	    OK
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-sep-1985 (derek)
**          Created new for 5.0.
**	06-aug-1986 (Derek)
**	    Fix problem with date format that caused garbage to be
**	    printed.
**      08-aug-1986 (seputis)
**          Added floating point support
**      29-sep-1988 (mikem)
**	    Changed length of "format_item" to 128 bytes from 64 bytes.  When
**	    using the %v format from dmfrcp we overflowed this buffer resulting
**	    in an access violation in do_format().  This should really be 
**	    changed to check for lengths, and return errors when overflow 
**	    happens.  Also the unix version of this file needs to be changed
**	    to use a portable "varargs" method of dealing with variable number
**	    of arguments to TRdisplay().
**
*/
#if defined(__STDARG_H__)
STATUS
TRdisplay(char *f, ... )
#else
STATUS
TRdisplay(va_alist)
va_dcl
#endif  /* __STDARG_H__ */
{
    CONTEXT	    *context = &tr_default;
#if !defined(__STDARG_H__)
    char            *f;
#endif
    va_list	    p;
    i4	    cur_arg = 0;
    struct argmng   marg;
#ifdef MT_CS
    static SHM_SPIN trdislock;
#endif

#if defined(__STDARG_H__)
    va_start(p, f);
#else
    va_start(p);

    f = va_arg(p,char *);
#endif

    if (context->fp_array[FP_TERMINAL] != NULL || context->fp_array[FP_FILE] != NULL)
    {
#ifdef MT_CS
	SHM_GETSPIN(&trdislock);
#endif
	do_format(TRwrite, (i4 *) 0, &f, &p, &cur_arg, tr_buffer, sizeof(tr_buffer), &tr_offset, 0, 0, &marg);

#ifdef MT_CS
	SHM_RELSPIN(&trdislock);
#endif
    }
    va_end(p);
    return(OK);
}

/*{
** Name: TRformat	- Format text into memory.
**
** Description:
**      This function will format text into a memory buffer and then call a 
**	caller provided function to dispose of the buffer contents.
**
**	The use function is called like:
**	    (*fcn)(arg, length, buffer)
**
** Inputs:
**      fcn                             Function to call back.
**      arg                             Argument to pass to fcn.
**      buffer                          Pointer to buffer.
**      l_buffer                        Length of buffer.
**      format                          Format string.
**      parameter                       Parameter list.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      02-jun-1987 (Derek)
**          Created.
**	14-dec-90 (mikem) restore lost history comments:
**    	    03-Aug-1990 (mikem)
**          Mainline currently passes "NULL" pointer as a "fcn" when it
**          doesn't want any function executed.  Changed TRformat to check
**          for a NULL function pointer before using it.
**	30-jun-1995 (duursma)
**	    Restored original (VAX) logic for dealing with %? that was somehow
**	    lost during a poorly documented integration of this file.  This 
**	    fixes the problem where infodb and logdump print completely bogus
**	    date/times.  Also commented the fact that ? is a format character.
**	10-dec-2001 (kinte01)
**	    Update length of "format_item" to 2048 since we have messages 
**	    longer than 128 bytes
*/
VOID
# if 0
TRformat(fcn, arg, buffer, l_buffer, format, va_alist)
i4		    (*fcn)();
i4		    *arg;
char		    *buffer;
i4		    l_buffer;
char		    *format;
va_dcl
# else
# if defined(__STDARG_H__)
TRformat(i4 (*fcn)(PTR, i4, char *), ...)
# else
TRformat(va_alist)
va_dcl
# endif
# endif
{
#if !defined(__STDARG_H__)
    i4              (*fcn)();
# endif
    PTR		    arg;
    char	    *buffer;
    i4		    l_buffer;
    char	    *f;
    va_list	    p;
    i4		    offset = 0;
    i4		    cur_arg = 0;
    struct argmng   marg;

#if defined(__STDARG_H__)

    va_start(p, fcn);
# else
    va_start(p);
    fcn =	(i4 (*)()) va_arg(p,PTR);
# endif
    arg =  	va_arg(p,PTR);
    buffer =	va_arg(p,char *);
    l_buffer =	va_arg(p,i4);
    f =		va_arg(p,char *);

    do_format(fcn, arg, &f, &p, &cur_arg, buffer, l_buffer, &offset, 0, 0,
		&marg);
    if (fcn && offset)
	(*fcn)(arg, offset, buffer);

    va_end(p);
}

static VOID
do_format(fcn, arg, format, parameter, pcur_arg, buffer, buffer_length, buffer_offset, base, type, marg)
i4		    (*fcn)();
PTR		    *arg;
char		    **format;
va_list		    *parameter;
i4		    *pcur_arg;
char		    *buffer;
i4		    buffer_length;
i4		    *buffer_offset;
PTR		    base;
i4		    type;
#define			IN_REPEAT   1
#define			IN_ARRAY    2
#define			IN_STRUCT   3
struct argmng	    *marg;
{
    char	    *f = *format;
    va_list	    p = *parameter;
    i4	    cur_arg = *pcur_arg;
    char	    format_item[2048];
    i4	    offset = *buffer_offset;
    i4	    length = buffer_length;
    i4	    format_length;
    char	    *format_buffer;
    PTR		    base_address;
    i4	    width;
    bool	    noprecision;	    /* TRUE if no precision specified */
    i4	    precision;
    i4	    directive;
    i4	    trim;
    i4	    move_amount;
    i4	    save_cur_arg;
    i4	    save_offset;
    char	    *save_f;
    i4	    save_length;
    i4	    number;
    char	    *string;

    i4	    l_tmp;
    i4	    *lp_tmp;
    f8		    f_tmp;
    PTR             p_tmp;
    u_i4            ul_tmp;

	/*
	** If type is 0, we are being called from TR{format|display}, not
	** recursively.  So, we need to set marg->next_arg to 0 to begin a new
	** argument list.
	*/
	if (type == 0)
	{
		marg->next_arg = 0;
		marg->dataptr  = marg->loc_buf;
	}

	/*
	** Macro to set a variable to the value of an arg.
	** If the arg has already been seen, get it from the
	** data space pointed to by marg->dataptr via cur_arg and
	** the marg->argptr array.  Otherwise, use varargs to get the
	** value, stash the data in the data space pointed to by
	** marg->dataptr, set the next pointer in marg->argptr to the address
	** of this space, and then increment the counters cur_arg
	** and marg->next_arg.
	*/

# define SET_ARG(VAR,VALIST,TYPE) \
	if (cur_arg < marg->next_arg) \
	{ \
		VAR = *((TYPE *)marg->argptr[cur_arg++]); \
	} \
	else \
	{ \
		marg->dataptr = ME_ALIGN_MACRO(marg->dataptr,sizeof(TYPE)); \
		VAR = va_arg(VALIST,TYPE); \
		MECOPY_CONST_MACRO((PTR)&VAR,sizeof(TYPE),marg->dataptr); \
		marg->argptr[marg->next_arg++] = marg->dataptr; \
		marg->dataptr += sizeof(TYPE); \
		cur_arg++; \
	}

    for (;;)
    {
	/*  Check for end of format string. */

	if (*f == 0)
	    break;

	/* Check for embedded new-line. */

	if (*f == '\n')
	{
	    if (fcn)
	    	(*fcn)(arg, offset, buffer);
	    offset = 0;
	    f++;
	    continue;
	}

	/*  If non-specifier then put in buffer. */

	if (*f != '%')
	{
	    if (offset >= length)
	    {
		/*  Write the line and signal a continuation. */

		if (fcn)
		    (*fcn)(arg, length, buffer);
		offset = 0;
	    }
	    buffer[offset++] = *f++;
	    continue;
	}

	/*  Parse a format specifier. */

	trim = width = precision = 0;
        noprecision = TRUE;
	if (*++f == '~')
	{
	    trim = TRUE;
	    f++;
	}
	if (*f == '#')
	{
	    SET_ARG(width,p,i4);
	    f++;
	}
	else if (*f == '*')
	{
	    if (type > IN_REPEAT)
	    {
		SET_ARG(l_tmp,p,i4);
		width = *((i4 *)((char *)base + l_tmp));
	    }
	    else
	    {
		SET_ARG(lp_tmp,p,i4 *);
		width = *lp_tmp;
	    }
	    f++;
	}
	else
	    while (*f >= '0' && *f <= '9')
		width = width * 10 + *f++ - '0';
	if (*f == '.')
	{	
            noprecision = FALSE;	    /* precision for floating pt */
	    if (*++f == '#')
	    {
		SET_ARG(precision,p,i4);
		f++;
	    }
	    else if (*f == '*')
	    {
		if (type > IN_REPEAT)
		{
		    SET_ARG(l_tmp,p,i4);
		    precision = *((i4 *)((char *)base + l_tmp));
		}
		else
		{
		    SET_ARG(lp_tmp,p,i4 *);
		    precision = *lp_tmp;
		}
		f++;
	    }
	    else
		while (*f >= '0' && *f <= '9')
		    precision = precision * 10 + *f++ - '0';
	}
	directive = *f++;
		
	/*  Now execute the directive. */

	switch (directive)
	{
	case '%':
	    buffer[offset] = directive;
	    offset++;
	    continue;

	case '[':
	    SET_ARG(base_address,p,PTR);
	    if (type > IN_REPEAT)
		/* base_address is a structure offset */
		base_address = (PTR)((SCALARP)base + (SCALARP) base_address);
	    save_offset = offset;
	    do
	    {
	        save_f = f;
	        save_cur_arg = cur_arg;
	        do_format(fcn, arg, &save_f, &p, &save_cur_arg, buffer, length,
		    &save_offset, *(i4 *)base_address, IN_ARRAY, marg);
		base_address += sizeof (i4 *);
	    } while (--width > 0);
	    f = save_f;
	    cur_arg = save_cur_arg;
	    offset = save_offset;
	    continue;

	case ']':
	case '}':
	case ')':
	    if ((type == IN_REPEAT && directive == ')') ||
		(type == IN_STRUCT && directive == '}') ||
		(type == IN_ARRAY && directive == ']'))
	    {
		*format = f;
		*parameter = p;
		*pcur_arg = cur_arg;
		*buffer_offset = offset;
		return;
	    }
	    continue;

	case '<':
	    if (width)
	        cur_arg -= width;
	    else
	        cur_arg--;
	    if (cur_arg < 0)
		cur_arg = 0;
	    continue;

	case '>':
	    /*
	    ** This "feature" exhibits a high degree of
	    ** bogosity, and should be removed.
	    */
	    if (width)
	    {
		int i;

		for (i=0; i < width; i++)
		{
		    SET_ARG(l_tmp,p,i4);
		}
	    }
	    else
	    {
		SET_ARG(l_tmp,p,i4);
	    }
	    continue;

	case '{':
	    SET_ARG(base_address,p,PTR);
	    if (type > IN_REPEAT)
		/* base_address is a structure offset */
		base_address = (PTR)((SCALARP)base + (SCALARP) base_address);
	    save_offset = offset;
	    do
	    {
	        save_f = f;
	        save_cur_arg = cur_arg;
	        do_format(fcn, arg, &save_f, &p, &save_cur_arg, buffer, length,
		    &save_offset, base_address, IN_STRUCT, marg);
		base_address += precision;
	    } while (--width > 0);
	    f = save_f;
	    cur_arg = save_cur_arg;
	    offset = save_offset;
	    continue;

	case '(':
	    save_cur_arg = cur_arg;
	    save_offset = offset;
	    do
	    {
	        save_f = f;
	        do_format(fcn, arg, &save_f, &p, &save_cur_arg, buffer, length,
		    &save_offset, base_address, IN_REPEAT, marg);
		base_address += precision;
	    } while (--width > 0);
	    f = save_f;
	    cur_arg = save_cur_arg;
	    offset = save_offset;
	    continue;

	case '*':
	    if (width > length - offset)
	        width = length - offset;
	    MEfill(width, *f++, &buffer[offset]);
	    offset += width;
	    continue;

	case '?':
	    {   
	        struct { int str_len; char  *str_str; } format_desc =
		    { sizeof(format_item), format_item };
		char	    *time;

		if (type > IN_REPEAT)
		{
		    SET_ARG(l_tmp,p,i4);
		    time = ((char *)base + l_tmp);
		}
		else
		{
		    SET_ARG(time,p,char *);
		}
		format_length = 0;
	        sys$asctim(&format_length, &format_desc, time, 0);
		format_buffer = format_item;
	    }
	    break;

	case '@':
	    {   
		SYSTIME		time;

		TMnow(&time);
		format_length = 0;
		TMstr(&time, format_item);
		format_length = STlength(format_item);
		format_buffer = format_item;
	    }
	    break;

	case 'c':
	    format_length = 1;
	    if (type > IN_REPEAT)
	    {
		SET_ARG(l_tmp,p,i4);
	        format_item[0] = *((char *)base + l_tmp);
	    }
	    else
	    {
		SET_ARG(format_item[0],p,i4);
	    }
	    if (format_item[0] < 32 || format_item[0] > 126)
	        format_item[0] = '.';
	    format_buffer = format_item;
	    break;

	case 'd':
	    format_buffer = format_item;
	    if (type > IN_REPEAT)
	    {
	        if (precision == 1)
		{
		    SET_ARG(l_tmp,p,i4);
		    CVla(*((char *)((char *)base + l_tmp)), format_buffer);
		}
		else if (precision == 2)
		{
		    SET_ARG(l_tmp,p,i4);
		    CVla(*((short *)((char *)base + l_tmp)), format_buffer);
		}
		else
		{
		    SET_ARG(l_tmp,p,i4);
		    CVla(*((i4 *)((char *)base + l_tmp)), format_buffer);
		}
	    }
	    else
	    {
		SET_ARG(l_tmp,p,i4);
	        CVla(l_tmp, format_buffer);
	    }
	    for (format_length = 0; format_buffer[format_length]; format_length++)
	        ;
	    break;

 	case 'e':
	case 'f':
	case 'g':
	case 'n':
	    {
		i4                   wid;          /* float width field */
		i4                   prec;	    /* decimal field */
		i2                    real_width;   /* width of result */

	        format_buffer = format_item;
                if (noprecision)
		    prec = 3;
		else
		    prec = precision;
                if (!(wid = width))
		    wid = 20;
	        if (type > IN_REPEAT)
		{
		    SET_ARG(l_tmp,p,i4);
	            CVfa(*((f8 *)((char *)base + l_tmp)), wid, prec, *(f-1), 
		         '.', format_buffer, &real_width);
		}
                else
		{
		    SET_ARG(f_tmp,p,f8);
	            CVfa(f_tmp, wid, prec, *(f-1), '.', format_buffer, 
			&real_width);
		}
                format_length = real_width;
	        break;
	    }

	case 'x':
	case 'b':

	    format_buffer = format_item;
	    if (directive == 'b')
	        CVptrax(base, format_buffer);
	    else if (type > IN_REPEAT)
	    {
	        if (precision == 1)
		{
		    SET_ARG(l_tmp,p,i4);
		    CVlx(*((char *)((char *)base + l_tmp)), format_buffer);
		}
		else if (precision == 2)
		{
		    SET_ARG(l_tmp,p,i4);
		    CVlx(*((short *)((char *)base + l_tmp)), format_buffer);
		}
		else
		{
		    SET_ARG(l_tmp,p,i4);
		    CVlx(*((i4 *)((char *)base + l_tmp)), format_buffer);
		}
	    }
	    else
	    {
		SET_ARG(l_tmp,p,i4);
	        CVlx(l_tmp, format_buffer);
	    }
	    for (format_length = 0; format_buffer[format_length]; format_length++)
		    ;
	    break;

        case 'p':

            format_buffer = format_item;
            if (type > IN_REPEAT)
            {
                SET_ARG(l_tmp,p,i4);
                CVptrax(*((PTR *)((char *)base + l_tmp)), format_buffer);
            }
            else
            {
                SET_ARG(p_tmp, p, PTR);
                CVptrax(p_tmp, format_buffer);
            }
            for (format_length = 0; format_buffer[format_length];
                 format_length++)
                    ;
            break;

	case 's':
	    width = -width;
	    if (type > IN_REPEAT)
	    {
		SET_ARG(l_tmp,p,i4);
	        format_buffer = ((char *)base + l_tmp);
	    }
	    else
	    {
		SET_ARG(format_buffer,p,char *);
	    }
	    format_length = precision;
	    if (precision == 0)
	        for (format_length = 0; format_buffer[format_length]; format_length++)
	            ;
	    break;
	    
	case 't':
	    width = -width;
	    SET_ARG(format_length,p,i4);
	    SET_ARG(format_buffer,p,char *);
	    if (trim)
		while (format_length)
		    if (format_buffer[format_length - 1] != ' ')
			break;
		    else
			format_length--;
	    break;

	case 'u':
	    format_buffer = format_item;
	    if (type > IN_REPEAT)
	    {
	        if (precision == 1)
		{
		    SET_ARG(ul_tmp,p,i4);
		    CVula(*((char *)((char *)base + ul_tmp)), format_buffer);
		}
		else if (precision == 2)
		{
		    SET_ARG(ul_tmp,p,i4);
		    CVla(*((short *)((char *)base + ul_tmp)), format_buffer);
		}
		else
		{
		    SET_ARG(ul_tmp,p,i4);
		    CVla(*((i4 *)((char *)base + ul_tmp)), format_buffer);
		}
	    }
	    else
	    {
		SET_ARG(ul_tmp,p,i4);
	        CVla(ul_tmp, format_buffer);
	    }
	    for (format_length = 0; format_buffer[format_length]; format_length++)
	        ;
	    break;

	case 'v':
	    width = -width;
	    SET_ARG(string,p,char *);
	    if (type > IN_REPEAT)
	    {
	        if (precision == 1)
		{
		    SET_ARG(l_tmp,p,i4);
	            number = *(char *)((char *)base + l_tmp);
		}
	        else if (precision == 2)
		{
		    SET_ARG(l_tmp,p,i4);
	            number = *(short *)((char *)base + l_tmp);
		}
	        else
		{
		    SET_ARG(l_tmp,p,i4);
	            number = *(i4 *)((char *)base + l_tmp);
		}
	    }
	    else
	    {
		SET_ARG(number,p,i4);
	    }
	    format_buffer = format_item;
	    format_length = 0;
	    save_length = 0;
	    do
	    {
	        format_buffer[format_length++] = *string;
	        if (*string == 0 || *string == ',')
	        {
		    if ((number & 1) == 0)
		        format_length = save_length;
		    else
		        save_length = format_length;
		    if ((number >>= 1) == 0)
			break;
		}
	    } while (*string++);
	    if (format_length &&
		(format_buffer[format_length - 1] == ',' ||
		    format_buffer[format_length - 1] == 0))
	    {
		    format_length--;
	    }
	    break;

	case 'w':
	    width = -width;
	    SET_ARG(string,p,char *);
	    if (type > IN_REPEAT)
	    {
	        if (precision == 1)
		{
		    SET_ARG(l_tmp,p,i4);
	            number = *(char *)((char *)base + l_tmp);
		}
	        else if (precision == 2)
		{
		    SET_ARG(l_tmp,p,i4);
	            number = *(short *)((char *)base + l_tmp);
		}
	        else
		{
		    SET_ARG(l_tmp,p,i4);
	            number = *(i4 *)((char *)base + l_tmp);
		}
	    }
	    else
	    {
		SET_ARG(number,p,i4);
	    }
	    format_buffer = string;
	    format_length = 0;
	    do
	    {
	        if (*string == 0 || *string == ',')
	        {
		    if (number == 0)
		        break;
		    number--;
		    format_buffer = string + 1;
		    format_length = 0;
		    continue;
		}
		format_length++;
	    } while (*string++);
	    break;

	default:
	    continue;
	}

	/* Copy result to output buffer. */

	if (width >= 0)
	{
	    save_length = width;
	    if (width == 0)
		save_length = format_length;
	    if (save_length < format_length)
		format_length = save_length;
	    save_length -= format_length;
	    
	    /*  Move leading pad characters to buffer. */

	    while (save_length)
	    {
		move_amount = save_length;
		if (move_amount > length - offset)
		    move_amount = length - offset;
		MEfill(move_amount, ' ', &buffer[offset]);
		save_length -= move_amount;
		offset += move_amount;
		if (offset >= length)
		{
		    if (fcn)
		    	(*fcn)(arg, length, buffer);
		    offset = 0;
		}
	    }
	}
	else
	{
	    save_length = -width;
	    if (save_length < format_length)
		format_length = save_length;
	    save_length -= format_length;
	}

	/*  Move formatted data to buffer. */

	while (format_length)
	{
	    move_amount = format_length;
	    if (move_amount > length - offset)
	        move_amount = length - offset;
	    MECOPY_VAR_MACRO(format_buffer, move_amount, &buffer[offset]);
	    format_length -= move_amount;
	    format_buffer += move_amount;
	    offset += move_amount;
	    if (offset >= length)
	    {
		if (fcn)
		    (*fcn)(arg, length, buffer);
	        offset = 0;
	    }
	}

	/*  Move trailing pad characters to buffer. */

	while (save_length)
	{
	    move_amount = save_length;
	    if (move_amount > length - offset)
	        move_amount = length - offset;
	    MEfill(move_amount, ' ', &buffer[offset]);
	    save_length -= move_amount;
	    offset += move_amount;
	    if (offset >= length)
	    {
		if (fcn)
		    (*fcn)(arg, length, buffer);
	        offset = 0;
	    }
	}

	/* Continue with next character from format. */
    }

    *format = f;
    *parameter = p;
    *pcur_arg = cur_arg;
    *buffer_offset = offset;
    return;
}

/*{
** Name: TRflush	- Flush buffers to disk.
**
** Description:
**      This routine will flush any trace output buffers to disk.
**
** Inputs:
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      18-nov-1986 (Derek)
**          Created.
[@history_template@]...
*/
VOID
TRflush()
{
    FILE	*fp;

    if ((fp = tr_default.fp_array[FP_TERMINAL]) != NULL)
	SIflush(fp);
    if ((fp = tr_default.fp_array[FP_FILE]) != NULL)
	SIflush(fp);
}

/*{
** Name: TRgettrace	- Test a trace flag.
**
** Description:
**      Test a specific word and/or bit of a word that was previously 
**      initialized by calling TRmaketrace.  TRgettrace uses the vector
**	that was used in the last call to TRmaketrace.  If no calls have
**	been made to TRmaketrace, then it always returns FALSE.
**
** Inputs:
**      word                            The index of the word to set.
**      bit                             The bit within the wword to set or
**					and bit if bit == -1.
**
** Outputs:
**	Returns:
**	    bool			TRUE if bit was set
**					False if bit wasn't set.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-sep-1985 (derek)
**          Created new for 5.0.
*/
bool
TRgettrace(word,bit)
i4	word;
i4	bit;
{
	if (trace_vector == 0)
	    return (FALSE);
	if (bit < 0)
	    return (trace_vector[word] != 0);
	return (((trace_vector[word] >> bit) & 1) != 0);
}

/*{
** Name: TRmaketrace	- Setup trace vector
**
** Description:
**      Initilize a trace vector based on the flags that are passed to 
**	this routine.  The address of the trace vector is remembered for
**	future calls to TRgetrace or TRsettrace. 
**
**	Trace flags have syntax:
**	[-t | -tn1 | -tn2.n3 | -tn4/n5 ]
**	where t is whatever single character was set by argument tflag
**	of tTrace.  The four options mean set all flags in the trace vector,
**	set word n1, set bit n3 in word n2, and set words n4 through n5.
**
** Inputs:
**      argv                            An array of characters to scan for
**					trace arguments.
**      tflag                           The characters that must follow the
**					'-' in trace arguements.
**      tsize                           The size of the trace vector in i2's.
**	tonoff				Wheter to turn the bits on or off.
**
** Outputs:
**      tvect                           Sets bits in tvect determined by the 
**					arguments passed.  The name of tvect
**					if remembered as the default.
**	Returns:
**	    STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-sep-1985 (derek)
**          Created new for 5.0.
**	14-nov-1991 (bryanp)
**	    Return STATUS to match CL spec.
*/
STATUS
TRmaketrace(
char	**argv,
char	tflag,
i4	tsize,
i4	*tvect,
i4	tonoff)
{
    char		**ap;
    char		*p;
    i4			low_word;
    i4			high_word;
    i4			low_bit;
    i4			high_bit;
    i4			i;

    for (ap = argv; *ap != NULL; ap++)
    {
        if ((*ap)[0] != '-' || (*ap)[1] != tflag)
	    continue;

	trace_vector = (i2 *) tvect;
	p = *ap;

	while (*p)
	{
	    /* Search for n. */

	    for (low_word = 0; *p >= '0' && *p <= '9'; p++)
		low_word = low_word * 10 + *p - '0';

	    /* If '.' then search for 'm'. */
	    
	    if (*p == '.')
		for (low_bit = 0, p++; *p >= '0' && *p <= '9'; p++)
		    low_bit = low_bit * 10 + *p - '0';
	    else
		low_bit = -1;
	    high_word = low_word;
	    high_bit = low_bit;

	    /* If '/' then search for high n.m. */

	    if (*p == '/')
	    {
		for (high_word = 0; *p >= '0' && *p <= '9'; p++)
		    high_word = high_word * 10 + *p - '0';
		if (*p == '.')
		    for (high_bit = 0, p++; *p >= '0' && *p <= '9'; p++)
			high_bit = high_bit * 10 + *p - '0';
		else
		    high_bit = -1;
	    }

	    /* Set bits low_word.low_bit through high_word.high_bit. */

	    for (i = low_word; i <= high_word; i++)
	    {
		if (i == low_word)
		    tvect[i] |= tonoff ? (-1 << low_bit) : 
			~ (-1 << low_bit);
		else if (i == high_word)
		    tvect[i] |= tonoff ? ~(-1 << high_bit) :
			(-1 << high_bit);
		else if (tonoff)
		    tvect[i] = -1;
		else tvect[i] = 0;
	    }
	}
    }
    return (OK);
}

/*{
** Name: TRset_file	- Set trace input/output.
**
** Description:
**      Changes the trace system log file and terminal output file. 
**      The log file contains a log of all inputs and output of the 
**      trace system.  The terminal output contains only data written 
**      to the output.
**
** Inputs:
**      flag                            Flag to determine the action type.
**					Either: TR_F_OPEN, TR_F_CLOSE,
**					TR_T_OPEN, TR_T_CLOSE, TR_I_OPEN,
**					TR_I_CLOSE.
**      filename                        Pointer to the filename.
**      name_length                     The length of the filename.
**
** Outputs:
**	tr_context			Updates context if it was passed.
**      err_code                        Pointer to location to store error.
**	Returns:
**	    STATUS			CL error status.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-sep-1985 (derek)
**          Created new for 5.0.
**	27-may-1992 (jnash)
**	    Add ability to append to end of file via TR_F_APPEND.
**	22-june-1992 (jnash)
**	    TR_NOOVERRIDE_MASK added.
**	09-jun-2003 (devjo01)
**	    Add support for TR_F_SAFEOPEN. b110302.
*/
STATUS
TRset_file(flag, filename, name_length, err_code)
i4                flag;
char               *filename;
i4                name_length;
CL_ERR_DESC        *err_code;
{
    CONTEXT	    *context = &tr_default;
    i4		    file_index;
    int		    create = FALSE;
    int		    status;
    char	    *mode;
    LOCATION	    loc;
    int		    nooverride = 0;
    char	    locbuf[MAX_LOC];
    char	    signaturebuf[TR_SAFE_STRING_LEN+2];
    i4		    count;

    /*	 Check for bad parameters. */

    if (err_code == 0)
	return (TR_BADPARAM);
    SETCLERR(err_code, 0, 0);

    nooverride = flag & TR_NOOVERRIDE_MASK;
    flag &= ~TR_NOOVERRIDE_MASK;

    /* Determine the file to work with. */

    switch (flag)
    {
    case TR_T_CLOSE:
    case TR_T_OPEN:
	file_index = FP_TERMINAL;
	break;

    case TR_F_OPEN:
    case TR_F_SAFEOPEN:
    case TR_F_CLOSE:
    case TR_F_APPEND:
	file_index = FP_FILE;
	break;

    case TR_I_OPEN:
    case TR_I_CLOSE:
	file_index = FP_INPUT;
	break;

    default:
	return (TR_BADPARAM);
    }

    /* Determine the action to perform. */

    switch (flag)
    {
    case TR_T_OPEN:
    case TR_F_OPEN:
    case TR_I_OPEN:
    case TR_F_SAFEOPEN:
	/*
	**  If this is an open and a file in already open, fall through
	**  the the close code.
	*/

	if (context->fp_array[file_index] == NULL)
	    break;
	else if (nooverride)
	    return(TR_NOT_OPENED);

	create = TRUE;

    case TR_I_CLOSE:
    case TR_F_CLOSE:
    case TR_T_CLOSE:
	/*  Close the file */

	if (context->fp_array[file_index] == NULL)
	{
	    return (TR_BADCLOSE);
	}
	if (BTtest(file_index, &context->stdioflag))
	{
	    BTclear(file_index, &context->stdioflag);
	    SIflush(context->fp_array[file_index]);
	}
	else
	{
	    status = SIclose(context->fp_array[file_index]);
	    if (status != OK)
	    {
		return (TR_BADCLOSE);
	    }
	}
	context->fp_array[file_index] = NULL;
	if (create == FALSE)
	    return (OK);
	break;

    case TR_F_APPEND:
	/* Append creates a new file unless nooverride requested */
	if ( (context->fp_array[file_index] != NULL) && nooverride )
	    return (TR_NOT_OPENED);
    }

    /* Must create the file now */

    /* Either an open, create or append depending on usage. */

    if (STscompare(filename, name_length, "stdio", 0) == 0)
    {
	switch (flag)
	{
	case TR_T_OPEN:
	case TR_F_OPEN:
	case TR_F_SAFEOPEN:
	    context->fp_array[file_index] = stdout;
	    break;
	case TR_I_OPEN:
	    context->fp_array[file_index] = stdin;
	    break;
	}
	BTset(file_index, &context->stdioflag);
    }
    else
    {
	STlcopy(filename, locbuf, name_length);
	LOfroms(FILENAME&PATH, locbuf, &loc);
	switch (flag)
	{
	case TR_I_OPEN:
	case TR_F_SAFEOPEN:
	    mode = "r";
	    break;
	case TR_F_APPEND:
	    mode = "a";
	    break;
	default:
	    mode = "w";
	}
	status = SIopen(&loc, mode, &(context->fp_array[file_index]));

	if (TR_F_SAFEOPEN == flag)
	{
	    if (OK == status)
	    {
		/* File exists, check if it is a trace log */
		signaturebuf[TR_SAFE_STRING_LEN+1] = '\0';
		if ( OK != SIread(context->fp_array[file_index],
				TR_SAFE_STRING_LEN+1, &count, signaturebuf) ||
				(TR_SAFE_STRING_LEN+1) != count ||
				0 != STcmp(signaturebuf + 1, TR_SAFE_STRING) )
		{
		    status = TR_BADOPEN;
		}
		SIclose(context->fp_array[file_index]);
		context->fp_array[file_index] = NULL;
	    }
	    else
	    {
		/* It's OK for file not to exist. */
		status = OK;
	    }
	}

	if (status != OK)
	{
	    if ( 'r' == *mode )
		return TR_BADOPEN;
	    else
		return TR_BADCREATE;
	}

	if (TR_F_SAFEOPEN == flag)
	{
	    status = SIopen(&loc, "w", &(context->fp_array[file_index]));
	    if ( status )
		return TR_BADCREATE;
	    /* Put signature line into file. */
	    TRdisplay( "%s %@\n", TR_SAFE_STRING );
	}
    }

    return (OK);
}   

/*{
** Name: TRrequest	- Read from trace input file.
**
** Description:
**      Read a line of input from the trace input file.  If there
**	if a log file, the input line is logged to the log file
**	preceded by a comment designator.  Input records that have
**	comment designators are skipped.
**
** Inputs:
**      record                          Record buffer address.
**      record_size                     Size of record buffer.
**      prompt                          Prompt to use, 0 gets default.
**
** Outputs:
**      read_count                      The number of bytes read.
**	Returns:
**	    STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-sep-1985 (derek)
**          Created new for 5.0.
**	17-Oct-1988 (daveb)
**	    readonly isn't legal.  Make the initialization readable.
**	24-jan-1989 (roger)
**	    Added CL_ERR_DESC usage.  Also, SIgetrec only fails w/ ENDFILE.
*/
STATUS
TRrequest(record, record_size, read_count, err_code, prompt)
char		   *record;
i4                record_size;
i4                *read_count;
CL_ERR_DESC        *err_code;
char               *prompt;
{
    FILE	    *infp;
    FILE	    *outfp;
    CONTEXT	    *context = &tr_default;
    char	    log_buffer[256];
    int		    status;
    i4	    count;
    char	    *cp;
    char	    newline = '\n';
    static 	    char *prompt_buffer = "TRrequest> ";

    /*	Check for bad paramaters. */

    if (err_code == 0)
	return(TR_BADPARAM);
    SETCLERR(err_code, 0, 0);

    if (record == 0 || record_size == 0 || read_count == 0)
	return (TR_BADPARAM);

    /*	Normalize the record size.  */

    if (record_size > sizeof(log_buffer) - 1)
	record_size = sizeof(log_buffer) - 1;

    infp = context->fp_array[FP_INPUT];

    if (infp == NULL)
    {
	return (TR_BADREAD);
    }

    if (prompt == 0)
    {
	prompt = prompt_buffer;
    }

    /*	Read until a record which is not a comment is found. */

    do
    {
	if ((outfp = context->fp_array[FP_TERMINAL]) != NULL)
	{
	    SIwrite(STlength(prompt), prompt, &count, outfp);
	    SIflush(outfp);
	}
	status = SIgetrec(record, record_size, infp);
	if (status == ENDFILE)
	{
	    return (TR_ENDINPUT);
	}
	if ((cp = STchr(record, newline)) != NULL)
		*cp = '\0';
    }	while (record[0] == '!');

    /*	If there is a log file, write the input to the log file. */

    *read_count = STlength(record);
    if ((outfp = context->fp_array[FP_FILE]) != NULL)
    {
	SIwrite(*read_count, record, &count, outfp);
    }
    return (OK);
}

/*{
** Name: TRsettrace	- Set a trace flag.
**
** Description:
**      Sets or clears trace flags set in the default trace vector 
**      setup by TRmaketrace.  If no trace vector has been setup the
**	the call is ignored.
**
** Inputs:
**      word                            The index of the word to set.
**      bit                             The bit within the word to set, or
**					all bits if bit == -1.
**      set                             The value to set either 0 or 1.
**
** Outputs:
**	Returns:
**	    STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	30-sep-1985 (derek)
**          Created for new 5.0.
**	14-nov-1991 (bryanp)
**	    REturn status to match CL spec.
**      14-Jul-1993 (daveb)
**          Return status when trace_vector is 0.  Was sending back junk.
*/
STATUS
TRsettrace(word, bit, set)
i4	word;
i4	bit;
bool	set;
{

    if (trace_vector == 0)
	return(OK);
    if (bit < 0)
    {
	trace_vector[word] = -1;
	if (set == FALSE)
	    trace_vector[word] = 0;
    }
    else
        trace_vector[word] &=  (~(1 << bit)) | ((set & 1) << bit);

    return (OK);
}

/*
** Name: TRwrite	- Write line to trace file.
**
** Description:
**      Internal routine to write the line to the trace output file and/or
**	the trace log file or neither.
** 
** Inputs:
**      buffer                          Address of line to print.
**	length				Length of line to print.
**
** Outputs:
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	20-nov-1985 (derek)
**          Created new for jupiter.
**	25-jun-1991 (bryanp)
**	    Don't suppress zero-length lines.
*/
STATUS
TRwrite(arg, length, buffer)
PTR		    arg;
i4		    length;
char                *buffer;
{
    i4		count;
    CONTEXT		*context = &tr_default;
    FILE		*fp;
    STATUS		status = FAIL;

    if ((fp = context->fp_array[FP_TERMINAL]) != NULL)
    {
	if (length > 0)
	    SIwrite(length, buffer, &count, fp);
	SIwrite(1, "\n", &count, fp);
	SIflush(fp);
	status = OK;
    }
    if ((fp = context->fp_array[FP_FILE]) != NULL)
    {
	SIwrite(1, "!", &count, fp);
	if (length > 0)
	    SIwrite(length, buffer, &count, fp);
	SIwrite(1, "\n", &count, fp);
	SIflush(fp);
	status = OK;
    }
    return( status );
}
