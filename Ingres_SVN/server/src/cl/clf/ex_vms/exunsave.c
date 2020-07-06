# include	<compat.h>
# include	<excl.h>
# include	<builtins.h>
# include	"exi.h"

/*
** Name: EXunsave_handler - Deletes the current contexts' handler.
**
** Description:
**	Pops the exception stack of all LARGER or EQUAL scopes.
**
** History:
**           27-may-93 (walt)
**               Added __TRAPB() call to make sure that all exceptions have been
**               handled before deleting the handler from our list of remembered
**               handlers.
**           25-jun-1993 (huffman)
**               Change include file references from xx.h to xxcl.h.
**	    10-oct-1995 (duursma)
**		Use new Unix CL routines i_EXtop() and i_EXpop() instead of
**		playing with very expensive lib$get_curr... and lib$get_prev...
**		routines to walk the stack.
*/

int
EXunsave_handler()
{
    /*  Make sure all exceptions have happened before deleting handler.  */

    __TRAPB();

    if (i_EXtop())
	i_EXpop();

    return (OK);
}
