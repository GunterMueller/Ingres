/*
**  Setu.c
**
** Here is a fragment of code that implements a setuser command.  The function
** setu gets the PCB address from a process global symbol.  The main function,
** setusr, calls the setu function via kernel mode.  This requires VAXC 3.0 or
** above and the link command must include:
**		sys$system:sys.stb/sel,sys$system:sysdef.stb/sel
**
**  History:
**	22-mar-1993 (donj)
**	    Created from testtool!sep!septool_vms!setuser.c.
**  30-mar-1993 (donj)
**          Added tracing feature and ALPHA code changes.
**  13-may-1993 (donj)
**	    Made some changes for Alpha.
**      15-oct-1993 (donj)
**          Make function prototyping unconditional.
**  10-jan-1994 (donj)
**          Moved constant declarations to sep!main!generic!hdr setu.h
**          which will be shared between getu.c and setu.c.
**	13-mar-2001	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from testtool code as the use is no longer allowed
**	15-may-2003 (abbjo03)
**	    In setu(), use STmove to copy and pad the user name, because STlcopy
**	    tacks on a null character.
**	06-jan-2004 (abbjo03)
**	    Provide a return value in setu() to prevent bogus warnings when
**	    qasetuser is invoked. Correct calls to SIprintf() which were
**	    supposed to call SIfprintf().
*/
#include    <compat.h>
#include    <er.h>
#include    <si.h>
#include    <st.h>
#include    <ssdef.h>
#include    <starlet.h>
#include    <setu.h>

i4
setusr(
char	*future_user)
{
    char                   *present_user ;
    i4                      ret_val = SS$_NORMAL;
    i4                      setu () ;

    present_user = getusr();

    if (tracing)
    {
	SIfprintf(traceptr,
	    "setusr> Inside setusr, present_user = <%s>, future_user = <%s>\n",
	    present_user, future_user);
	SIflush(traceptr);
    }

    while (STbcompare(present_user,0,future_user,0,TRUE) != 0)
    {
	arg[0] = 2;
	arg[1] = STlength(future_user);
	arg[2] = (u_i4)future_user;
	arg[3] = NULL;
	ret_val = sys$cmkrnl (setu, arg);
	if (ret_val != SS$_NORMAL)
	    return (ret_val);

	present_user = getusr();
    }

    if (tracing)
    {
	SIfprintf(traceptr,
	    "setusr> Leaving setusr, present_user = <%s>, future_user = <%s>\n",
	    present_user, future_user);
	SIflush(traceptr);
    }

    return (ret_val);
}


i4
setu(
i4	length,
char	*string)
{
    globalref char        *ctl$gl_pcb ;
    char                  *jib = *(int *)&ctl$gl_pcb[MY_pcb$l_jib] ;
    unsigned short         junk ;

    if (tracing)
    {
	SIfprintf(traceptr,"setu> Inside setu, string = <%s>\n",
	    string);
	SIflush(traceptr);
    }
#ifdef ALPHA
    STmove(string, ' ', 12, &jib[12]);
#else
    _MOVC5 (length, string, ' ', 12, &jib[12], &junk);
#endif

    if (tracing)
    {
	SIfprintf(traceptr,"setu> Leaving setu.\n");
	SIflush(traceptr);
    }

    return SS$_NORMAL;
}
