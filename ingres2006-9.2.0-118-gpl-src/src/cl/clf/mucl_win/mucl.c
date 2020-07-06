/*
** Copyright (C) 1996, 2002 Ingres Corporation All Rights Reserved.
*/

# include <compat.h>
# include <mu.h>

/*
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
**	12-dec-96 (mcgem01)
**	    Ported to Windows NT.
**	25-jun-2002 (somsa01)
**	    Add MUcl_sid (Self ID of caller) to help resolve 108015.
*/

/*
** Forward references
*/

static MU_CL_I_SEM_FUNC	MUcl_isem;
static MU_CL_R_SEM_FUNC	MUcl_rsem;
static MU_CL_P_SEM_FUNC	MUcl_psem;
static MU_CL_V_SEM_FUNC	MUcl_vsem;
static MU_SID_FUNC	MUcl_sid;

/*
** CL mutex function interface
*/

static MU_CL_SEM_FUNCS	cl_sem_funcs = 
{
    MUcl_isem,
    NULL,		/* Named mutexs not supported */
    MUcl_rsem,
    MUcl_psem,
    MUcl_vsem,
    MUcl_sid
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

    CS_synch_init( &sem->mu_synch );
    if (sem->mu_synch == NULL)
	status = FAIL;
    
    return( status );
}



/*
*/

static STATUS
MUcl_rsem( MU_CL_SEMAPHORE *sem )
{
    STATUS	status = OK;

    if (!CloseHandle (sem->mu_synch))
	status = FAIL; 

    return( status );
}



/*
*/

static STATUS
MUcl_psem( MU_CL_SEMAPHORE *sem )
{
    STATUS	status = OK;

    if (WaitForSingleObject (sem->mu_synch, INFINITE) == WAIT_FAILED)
    {
	status = GetLastError();
    }

    return( status );
}



/*
*/

static STATUS
MUcl_vsem( MU_CL_SEMAPHORE *sem )
{
    STATUS	status = OK;

    if (!ReleaseMutex (sem->mu_synch))
	status = FAIL;

    return( status );
}



/*
*/

static MU_SID
MUcl_sid( void )
{
    MU_SID	sid;

    sid = (MU_SID)CS_get_thread_id();

    return(sid);
}
