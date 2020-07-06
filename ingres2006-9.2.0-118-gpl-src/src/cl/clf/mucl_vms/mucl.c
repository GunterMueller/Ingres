/*
** Copyright (C) 1996 Ingres Corporation All Rights Reserved.
*/

# include <compat.h>
# include <gl.h>
# include <mu.h>

/**
** Name:	mucl.c	- CL private interface for MU mutex functions.
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
**      12-dec-1996 (canor01)
**          Make slightly more generic using existing CL macros for
**          ease in porting.
**	21-jan-1999 (canor01)
**	    Remove erglf.h.
**/

/*
** Forward references
*/

static MU_CL_I_SEM_FUNC	MUcl_isem;
static MU_CL_R_SEM_FUNC	MUcl_rsem;
static MU_CL_P_SEM_FUNC	MUcl_psem;
static MU_CL_V_SEM_FUNC	MUcl_vsem;

/*
** CL mutex function interface
*/

static MU_CL_SEM_FUNCS	cl_sem_funcs = 
{
    MUcl_isem,
    NULL,		/* Named mutexs not supported */
    MUcl_rsem,
    MUcl_psem,
    MUcl_vsem
};



/*
*/

MU_CL_SEM_FUNCS	*
MUcl_init( VOID )
{
    return( &cl_sem_funcs );
}


/*
*/

static STATUS
MUcl_isem( MU_CL_SEMAPHORE *sem )
{
    STATUS	status = OK;

# ifdef OS_THREADS_USED
    if ( CS_synch_init( &sem->mu_synch ) )
	status = FAIL;
# endif /* OS_THREADS_USED */
    
    return( status );
}



/*
*/

static STATUS
MUcl_rsem( MU_CL_SEMAPHORE *sem )
{
    STATUS	status = OK;

# ifdef OS_THREADS_USED
    if ( CS_synch_destroy( &sem->mu_synch ) )
	status = FAIL;
# endif /* OS_THREADS_USED */

    return( status );
}



/*
*/

static STATUS
MUcl_psem( MU_CL_SEMAPHORE *sem )
{
    STATUS	status = OK;

# ifdef OS_THREADS_USED
    if ( CS_synch_lock ( &sem->mu_synch ) )
	status = FAIL;
# endif /* OS_THREADS_USED */

    return( status );
}



/*
*/

static STATUS
MUcl_vsem( MU_CL_SEMAPHORE *sem )
{
    STATUS	status = OK;

# ifdef OS_THREADS_USED
    if ( CS_synch_unlock ( &sem->mu_synch ) )
	status = FAIL;
# endif /* OS_THREADS_USED */

    return( status );
}
