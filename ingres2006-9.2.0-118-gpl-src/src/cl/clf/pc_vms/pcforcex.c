/*
**    Copyright (c) 1992, 2000 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <pc.h>
#include    <starlet.h>

/*
** Name: PCFORCEX.C	    - The PCforce_exit() routine
**
** Description:
**	This file contains code to implement PCforce_exit.
**
** History:
**	8-oct-1992 (bryanp)
**	    Created for the new logging and locking system.
**      16-jul-93 (ed)
**	    added gl.h
**	18-oct-1993 (bryanp)
**	    Try using $delprc rather than $forcex. $forcex doesn't seem to stop
**	    servers quickly enough -- they continue running even after they've
**	    been forced to exit, and this panics the shared buffer manager nuke
**	    routines in LG.
**	19-jul-2000 (kinte01)
**	   Correct prototype definitions by adding missing includes
**	01-dec-2000	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from VMS CL as the use is no longer allowed
*/

/*
** Name: PCforce_exit	    - force specified process to exit.
**
** Description:
**	This routine causes the specified process to exit.
**
** INputs:
**	pid		    - Process ID of specified process
**
** Outputs:
**	err_code	    - reason for error.
**
** Returns:
**	OK
**	other
**
** History:
**	8-oct-1992 (bryanp)
**	    Created.
*/
STATUS
PCforce_exit(PID pid, CL_ERR_DESC *err_code)
{
    i4	stat;
    STATUS	status = OK;

    sys$delprc(&pid, (PTR)0);
    return (OK);

#if 0
    stat = sys$forcex(&pid, (PTR)0, (i4)0 );

    if ((stat & 1) == 0)
    {
	err_code->error = stat;
	status = FAIL;
    }

    return (status);
#endif
}
