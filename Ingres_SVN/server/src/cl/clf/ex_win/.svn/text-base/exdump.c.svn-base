#include <compat.h>

/*
**      Copyright (c) 1996, Ingres Corporation
**
**  Name: EXdump - Dumping routines
**
**  Description:
**      This module contains EXdump a routine for causing a dump of the current
**      process and EXdumpInit a routine for initialising it
**
**  History:
**	20-mar-96 -- File branched from UNIX.  Code still to be written.
**	16-jul-98 (mcgem01)
**	    Add a stub for the EXdumpInit function.
**      06-aug-1999 (mcgem01)
**          Replace nat and longnat with i4.
**		       
*/

/*
**  Name: EXdump - Collect evidence for current process
**
**  Description:
**      This routine causes any required evidence (as defined by the problem 
**      management database) to be collected.
**
**      If EXdumpInit has been called then a list of all errors for which no
**      evidence is required is available and this is scanned to filter out
**      any errors which require no evidence (saving a call to the problem 
**      management server)
**
**      If EXdumpInit has not been called then no dump is taken
**
**  Inputs:
**      u_i4 error        The reason we are dumping (an INGRES error
**                                 number or exception)
**
**  Side effects:
**     All signals are blocked while the dump is being taken
**     The entire server is suspended while the process is being dumped
**
**  History:
*/
VOID
EXdump(error,stack)
u_i4 error;
u_i4 *stack;
{
}

VOID
EXdumpInit()
{
}
