/*
**	VMS Exit handler defintion
*/

typedef struct _exh$
{
	struct _exh$	*exh$gl_link;
	long		(*exh$g_func)();
	long		exh$l_argcount;
	long		*exh$gl_value;
	long		exh$l_status;
}	$EXHDEF;
