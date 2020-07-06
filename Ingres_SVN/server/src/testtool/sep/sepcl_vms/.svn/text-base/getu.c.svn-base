/* **************************************************************
**
** getu.c
**
**  History:
**
**  26-mar-1993 (donj)
**          Created out of setuser.c for inclusion in SEP.
**  30-mar-1993 (donj)
**          Added tracing feature. Fixed a bug in setusr().
**          Added ALPHA code changes.
**  13-may-1993 (donj)
**          Made some changes for Alpha.
**   5-jan-1994 (donj)
**          MY_pcb$l_jib should be 252 for ALPHA.
**  10-jan-1994 (donj)
**	    Moved constant declarations to sep!main!generic!hdr setu.h
**	    which will be shared between getu.c and setu.c.
**  11-jan-1994 (donj)
**	    Added check for EOS in getusr() looking for the end
**	    of the username.
**	13-mar-2001	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from testtool code as the use is no longer allowed
**	07-jan-2004 (abbjo03)
**	    Use STmove to copy and pad the username. Fix return values.
**
** **************************************************************
*/
#include    <compat.h>
#include    <er.h>
#include    <cm.h>
#include    <me.h>
#include    <si.h>
#include    <st.h>
#include    <ssdef.h>
#include    <starlet.h>
#include    <setu.h>

i4
getu(
i4   length,
char *string)
{
    globalref char        *ctl$gl_pcb ;
    char                  *jib = *(int *)&ctl$gl_pcb[MY_pcb$l_jib] ;
    unsigned short         junk ;
#ifdef ALPHA
    STmove(&jib[12], ' ', 12, string);
#else
    _MOVC5 (length, &jib[12], ' ', 12, string, &junk);
#endif
    return (SS$_NORMAL);
}
char
*getusr()
{
    i4                     getu () ;
    i4                     slen ;
    i4                     ret_val ;
    char                  *cptr1 ;
    char                  *cptr2 ;
    
    cptr1  = (char *)MEreqmem(0,USERNAME_MAX,TRUE,(STATUS *)NULL);
    arg[0] = 2;
    arg[1] = USERNAME_MAX;
    arg[2] = (u_i4)cptr1;
    arg[3] = NULL;

    ret_val = sys$cmkrnl (getu, arg);
    if (ret_val != SS$_NORMAL)
	return ("") ;

    for (cptr2 = cptr1; ((!CMwhite(cptr2))&&(*cptr2 != EOS)); CMnext(cptr2));

    *cptr2 = EOS;

    cptr2  = (char *)MEreqmem(0,USERNAME_MAX,TRUE,(STATUS *)NULL);
    STcopy(cptr1,cptr2);
    MEfree(cptr1);

    return (cptr2);
}
