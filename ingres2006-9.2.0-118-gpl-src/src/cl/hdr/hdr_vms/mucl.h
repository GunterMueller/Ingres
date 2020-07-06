/*
** Copyright (C) 1996,2000 Ingres Corporation All Rights Reserved.
*/

# include <compat.h>
# include <cs.h>

/**
** Name:	mucl.h	- CL private interface for MU mutex functions.
**
** Description:
**	Defines the CL private interface to support the GL public
**	interface of the MU mutex module.  This interface permits
**	semaphore support in multi-threaded application environments.
**	Support for multi-threaded servers is provided through CS.
**
** History:
**	14-Nov-96 (gordy)
**	    Created.
**	12-dec-1996 (canor01)
**	    Make slightly more generic using existing CL macros for
**	    ease in porting.
**	01-dec-2000	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from VMS CL as the use is no longer allowed
**/

/*
** Datatype used by the CL semaphore functions.  This
** must be a structure to permit forward referencing
** in the MU public header file for definition of the
** CL private interface.  On systems which do not
** support multi-threaded applications, this may have
** a single, unused element.
*/

struct II_MU_CL_SEMAPHORE
{

# ifdef OS_THREADS_USED

    CS_SYNCH		mu_synch;

# else /* OS_THREADS_USED */

    i4 		mu_unused;	/* Unused - threads not supported */

# endif /* OS_THREADS_USED */

};

/*
** Macro function to provide access to the CL semaphore 
** functions data structure.  On non-threaded systems, 
** the macro may simply return NULL.  The macro may also 
** resolve to an actual function call, in which case the 
** function must match the following prototype:
**
** FUNC_EXTERN MU_CL_SEM_FUNCS *MUcl_init( VOID );
*/

# ifdef OS_THREADS_USED

FUNC_EXTERN	MU_CL_SEM_FUNCS		*MUcl_init( VOID );
# define	MU_CL_SEM_INIT()	MUcl_init()

# else /* OS_THREADS_USED */

# define 	MU_CL_SEM_INIT()	((MU_CL_SEM_FUNCS *)NULL)

# endif /* OS_THREADS_USED */


