# include	<compat.h>
# include	<excl.h>

FUNC_EXTERN VOID i_EXpush();

/*
**
**      History:
**
**           25-jun-1993 (huffman)
**               Change include file references from xx.h to xxcl.h.
**
**	     04-oct-1995 (duursma)
**		 Performance improvements:
**		 No more calls to lib$get_invo...
**		 Instead, use UNIX CL method of linking EX_CONTEXT blocks
**		 together.
**	     07-may-1998 (kinte01)
**		 Initialize previous_context pointer to 0,
**        	 else EXdelete() can SIGBUS ->server death on
**        	 non-0 uninitialized pointer. (bug 89411)
**	19-jul-2000 (kinte01)
**	   Add missing external function references
*/

STATUS 
EXsetup(i4 (*handler()), EX_CONTEXT *context)
{
        context->prev_context = 0;

	context->handler_address = handler;
	context->address_check = (u_i4)context;

	i_EXpush(context);

	return OK;
}
