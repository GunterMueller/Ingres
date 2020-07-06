/*
** Copyright (c) 1997 Ingres Corporation
*/

# include   <compat.h>
# include   <gl.h>
# include   <clconfig.h>
# include   <systypes.h>
# include   <cs.h>
# include   <me.h>
# include   <meprivate.h>
# include   <qu.h>

/*
**  Name: METLS.C - Thread-local storage routines
**
**  Description:
**	This module contains the code which implements 
**	thread-local storage.
**
**
**  History:
**	25-mar-1997 (canor01)
**	    Created.
*/

static bool ME_tlsmtxinit = FALSE;
static HANDLE ME_tlsmtx = NULL;

GLOBALREF ME_TLS_KEY	ME_tlskeys[];

/*
** Name:        ME_tls_createkey    - create a thread-local storage index key
**
** Description:
**      Create a unique key to identify a particular index into
**      thread-local storage.
**
** Inputs:
**      None
**
** Outputs:
**	key		the key created
**	status		
**
**      Returns:
**          OK  - success
**          !OK - failure
**
**  History:
**      25-mar-1997 (canor01)
**          Created.
*/
VOID
ME_tls_createkey( ME_TLS_KEY *key, STATUS *status)
{
    SECURITY_ATTRIBUTES sa;
    ME_TLS_KEY		i;

    /* first time in, create mutex for TLS index array */
    if ( ME_tlsmtxinit == FALSE )
    {
	iimksec (&sa);
	ME_tlsmtx = CreateMutex(&sa, FALSE, NULL);
	ME_tlsmtxinit = TRUE;
    }
    *key = TlsAlloc();
    if ( *key == 0xFFFFFFFF )
        *status = FAIL;
    else 
	*status = OK;

    /*
    ** key was created successfully.
    ** now insert it into an array of active keys
    */
    WaitForSingleObject( ME_tlsmtx, INFINITE );
    for ( i = 0; i < ME_MAX_TLS_KEYS; i++ )
    {
	if ( ME_tlskeys[i] == *key )
	    break;
	if ( ME_tlskeys[i] == (ME_TLS_KEY)0 )
        {
	    ME_tlskeys[i] = *key;
	    break;
	}
    }
    ReleaseMutex( ME_tlsmtx );
}

/*
** Name:        ME_tls_destroyall     - Destroy a thread's local storage
**
** Description:
**      Destroy the thread-local storage associated with a
**      all keys for a particular thread.
**
** Inputs:
**      destructor - pointer to a function to de-allocate the memory.
**
** Outputs:
**
**      Returns:
**          OK  - success
**          !OK - failure
**
**  History:
**	25-mar-1997 (canor01)
**	    Created.
*/
VOID
ME_tls_destroyall( STATUS *status )
{
    ME_TLS_KEY	key;
    STATUS	ret;

    *status = OK;

    /* keys begin at 1, keytab array begins at zero */
    WaitForSingleObject( ME_tlsmtx, INFINITE );
    for ( key = 0; key < ME_MAX_TLS_KEYS; key++ )
    {
        if ( ME_tlskeys[key] == (ME_TLS_KEY)0 )
	    break;
	ME_tls_destroy( ME_tlskeys[key], &ret );
	if ( ret != OK )
	    *status = ret;
    }
    ReleaseMutex( ME_tlsmtx );

    return;
}

