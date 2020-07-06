
# include	<compat.h>
# include	<gl.h>
# include	<ex.h>
# include	"exi.h"

/*
** Copyright (c) 1985 Ingres Corporation
**
** exstack.c -- EX Local Stack Handling Routines.
**
**	Contains all routines dealing with the exception stack.
**	These routines should be local to EX.
**
**	Content:
**		i_EX_initfunc()
**			Routine to initialize the i_EXsetcontext and
**			i_EXgetcontext static pointers to functions.
**
**		i_EX_getcontext() - static
**			Default get context routine for non-threaded users.
**
**		i_EX_setcontext() - static
**			Default set context routine for non-threaded users.
**
**		i_EXtop()
**			Routine the topmost environment on the stack.
**
**		i_EXpush()
**			Push a new scope on the stack.
**
**		i_EXpop()
**			Pop a scope off the stack.
**
**		i_EXnext()
**			Return next context on the stack.
**
**	History:
**
 * Revision 1.2  90/11/19  11:24:58  source
 * sc
 *
 * Revision 1.2  89/11/16  16:23:42  source
 * sc
 *
 * Revision 1.1  89/09/25  15:16:09  source
 * Initialized as new code.
 *
 * Revision 1.1  89/05/26  15:49:12  source
 * sc
 *
 * Revision 1.1  89/05/11  01:09:42  source
 * sc
 *
 * Revision 1.1  89/01/13  17:12:05  source
 * This is the new stuff.  We go dancing in.
 *
 * Revision 1.1  88/08/05  13:36:26  roger
 * UNIX 6.0 Compatibility Library as of 12:23 p.m. August 5, 1988.
 *
**      Revision 1.2  87/11/25  13:10:48  mikem
**      use new lower case hdr files.
**
**      Revision 1.1  87/11/09  12:59:44  mikem
**      Initial revision
**
 * Revision 1.1  87/07/30  18:04:13  roger
 * This is the new stuff.  We go dancing in.
 *
**		Revision 1.2  86/04/25  17:02:28  daveb
**		Change comments, remove code that was never being
**		compiled in, make routines void since their
**		return value was neither changing nor being checked.
**
**	26-aug-89 (rexl)
**	    Add non-threaded functions to set and get current context,
**	    i_EX_setcontext() and i_EX_getcontext() respectively.
**	    Add statics i_EXsetcontext and i_EXgetcontext, which are
**	    pointers to functions that perform the desired action so that
**	    in a multi-threaded environment one can obtain the current
**	    context from the CS_SCB without knowing any details of the CS_SCB.
**	    Add i_EX_initfunc() so that the set and get functions can be
**	    specified by the user.
**	    i_EXtop(), i_EXpush() and i_EXpop() were modified to use the
**	    function calls instead of the static variable ex_sptr.
**	    ex_sptr is still used by the default, non-threaded, functions
**	    i_EX_setcontext() and i_EX_getcontext().
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	04-oct-95 (duursma)
**	    Incorporated this UNIX CL module into alpha vms CL.
**	    Made changes so we conform to coding standard.
*/

/* Forward declarations */

static	VOID		i_EX_setcontext();
static	EX_CONTEXT	*i_EX_getcontext();

/* Local variables */

static  EX_CONTEXT	*ex_sptr;
static	VOID		 (*i_EXsetcontext)() = i_EX_setcontext;
static	EX_CONTEXT	*(*i_EXgetcontext)() = i_EX_getcontext;

/*
** EX_initfunc(setfunc, getfunc)
** void		(*setfunc)();
** EX_CONTEXT	(*getfunc)();
**
**	Sets the pointers used to set and get the current exception stack
**	pointer.  If NULL, systems defaults to the non-threaded functions
**	i_EXsetcontext() and i_EXgetcontext().
**
** HISTORY
**	26-aug-89 (rexl)
**	    written.
**	02-nov-92 (bonobo/mikem)
**	    su4_us5 port. Updated to VOID instead of void on setfunc().
*/

VOID
i_EX_initfunc( VOID        (*setfunc)(),
	       EX_CONTEXT *(*getfunc)() )

{

	i_EXsetcontext = setfunc ? setfunc : i_EX_setcontext;
	i_EXgetcontext = getfunc ? getfunc : i_EX_getcontext;
	return;
}


/*
** EX_CONTEXT *
** EX_getcontext()
**
**	Return a pointer to the current exception stack in a non-threaded
**	environment.
**
** HISTORY
**	26-aug-89 (rexl)
**	    written.
*/

static EX_CONTEXT *
i_EX_getcontext( void )
{
	return( ex_sptr );
}

/*
** EX_setcontext( context )
** EX_CONTEXT	*context;
**
**	Set the current exception stack pointer in a non-threaded environment.
**
** HISTORY
**	26-aug-89 (rexl)
**	    written.
*/

static VOID
i_EX_setcontext( EX_CONTEXT *context )
{
	ex_sptr = context;
	return;
}


/*
** EX_CONTEXT *
** EXtop()
**
**	Return the topmost scope on the exception stack.
**
** HISTORY
**	12/11/84 Written (sat).
*/

EX_CONTEXT *
i_EXtop( void )
{
	return ((*i_EXgetcontext)());
}

/*
** EXpush(contest)
**	EX_CONTEXT	*context;
**
**	Push the scope on the stack.
**
** HISTORY
**	12/11/84 Written (sat).
**
**      02-sep-93 (smc)
**          Made context cast a PTR to be portable to axp_osf.
**	13-oct-95 (duursma)
**	    Changed that back to u_i4 since that's the type of address_check.
*/
VOID
i_EXpush( EX_CONTEXT *context )
{

	/* If this handler is already here, we're just
	** changing the setjmp return point.
	*/
	if (context->address_check == (u_i4)(*i_EXgetcontext)())
		return;

	context->prev_context = (*i_EXgetcontext)();
	(*i_EXsetcontext)(context);
	return;
}

/*
** EXpop()
**
**	Removes topmost scope from stack.
**
** HISTORY
**	12/11/84 Written (sat)
*/
VOID
i_EXpop( void )
{
	EX_CONTEXT	*context;

	context = (*i_EXgetcontext)();
	(*i_EXsetcontext)(context->prev_context);
	return;
}

EX_CONTEXT	*
i_EXnext( EX_CONTEXT *context )
{
	return(context->prev_context);
}

