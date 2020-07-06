/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <erglf.h>
#include    <cs.h>
#include    <gc.h>
#include    <mo.h>
#include    <mu.h>
#include    <qu.h>
#include    <sl.h>
#include    <st.h>
#include    <iicommon.h>
#include    <gca.h>
#include    <gcaint.h>
#include    <gcm.h>
#include    <gcs.h>
#include    <gcr.h>

/**
**
**  Name: GCATERMINATE.C - Terminate GCA communications
**
**  Description:
**
**          gca_terminate - Terminate User/GCA Association
**
**  History:    $Log-for RCS$
**      15-Apr-1987 (berrow)
**          Created.
**      08-Aug-87 (jbowers)
**          Clean up svc_parms queue.
**      11-Aug-87 (jbowers)
**          Added invocation of GCdelexh to delete a previously established 
**	    exit handler.
**	25-Oct-89 (seiwald)
**	    Shortened gcainternal.h to gcaint.h.
**	26-Oct-89 (seiwald)
**	    Changed calls to gca_qenq, etc. to use QUinsert, etc.
**	01-Nov-89 (seiwald)
**	    Fixed call to QUremove() - was taking address of QUEUE pointer.
**	10-Nov-89 (seiwald)
**	    Removed references to the SVC_PARMS queue.
**	25-Jan-90 (seiwald)
**	    Reworked a bit.  Calls gca_free_acbs() to free acb list.
**	30-May-91 (seiwald)
**	    Call gca_free_acbs() before zeroing out IIGCa_static, so that
**	    the usr_dealloc is still defined.
**	07-Jan-93 (edg)
**	    Removed FUNC_EXTERN's (now in gcaint.h) and #include gcagref.h.
**      17-May-1994 (daveb) 59127
**          Fixed semaphore leaks, named sems.  In GCA, change from 
**	    using handed in semaphore routines to MU semaphores; these
**	    can be initialized, named, and removed.  The others can't
**	    without expanding the GCA interface, which is painful and
**	    practically impossible.
**	10-Oct-95 (gordy)
**	    Completion moved to gca_service().
**	20-Nov-95 (gordy)
**	    Added prototypes.  Turn of init flag so IIGCa_static will
**	    be freed in IIGCa_call().
**	21-Nov-95 (gordy)
**	    Added gcr.h for embedded Comm Server configuration.
**	 4-Dec-95 (gordy)
**	    Added support for embedded Name Server configuration.
**	 3-Sep-96 (gordy)
**	    Reorganized GCA global data.
**	 9-Sep-96 (gordy)
**	    Use STprintf() rather than MOulongout() to build the instance
**	    ID.  See gcainit.c for expanation.
**	10-Apr-97 (gordy)
**	    GCA and GC service parameters separated.
**	23-May-97 (thoda04)
**	    Included mo.h header for function prototypes.
**	29-May-97 (gordy)
**	    Shutdown GCS.
**	 2-Oct-98 (gordy)
**	    Moved MIB class definitions to gcm.h.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/



/*{
** Name: gca_terminate	- Terminate User/GCA Association
**
** Description:
**      This routine is used to reset the internal data structures set up
**	when a user (either client or server) made itself known to GCF.
**	
**	All parameters (both input and output) are handled via a pointer to
**	SVC_PARMS structure containing the following elements ...
**
** Inputs:
**	none.
**
** Outputs:
**      gca_status                      Success or failure indication.
**	    E_GC0000_OK			Normal completion
**	    E_GC0007_NO_PREV_INIT	No previous initiation request
**
** Returns:
**	None
**
** Exceptions:
**	None
**
** Side Effects:
**	None.
**
** History:
**      15-Apr-1987 (berrow)
**          Created.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	25-aug-93 (ed)
**	    remove dbdbms.h
**	08-sep-93 (swm)
**	    Cast completion id. parameter to IIGCa_call() to PTR as it is
**	    no longer a i4, to accomodate pointer completion ids.
**	10-Oct-95 (gordy)
**	    Completion moved to gca_service().
**	20-Nov-95 (gordy)
**	    Added prototypes.  Turn of init flag so IIGCa_static will
**	    be freed in IIGCa_call().
**	 4-Dec-95 (gordy)
**	    Added support for embedded Name Server configuration.
**	 3-Sep-96 (gordy)
**	    Reorganized GCA global data.
**	 9-Sep-96 (gordy)
**	    Use STprintf() rather than MOulongout() to build the instance
**	    ID.  See gcainit.c for expanation.
**	10-Apr-97 (gordy)
**	    GCA and GC service parameters separated.
**	29-May-97 (gordy)
**	    Shutdown GCS.
**	11-May-06 (gordy)
**	    Semaphore protect the registration ACB.
*/

VOID
gca_terminate( GCA_SVC_PARMS *svc_parms )
{
    GCA_CB	*gca_cb = (GCA_CB *)svc_parms->gca_cb;
    char	id_buff[16];

    /*
    ** Tell MO that the control block is no longer active.
    */
    STprintf( id_buff, "%d", gca_cb->gca_cb_id );
    MOdetach( GCA_MIB_CLIENT, id_buff );

    /*
    ** Free resources.
    */
    MUr_semaphore( &gca_cb->gca_reg_semaphore );

    /*
    ** Clear the initialization flag so the 
    ** control block will be freed in gca_call().
    */
    gca_cb->gca_initialized = FALSE;

    /*
    ** Increment the term counter and perform
    ** global shutdown if all initializers have
    ** terminated.
    */
    IIGCa_global.gca_terminate++;

    if ( IIGCa_global.gca_initiate <= IIGCa_global.gca_terminate )
    {
	i4 assoc_id;

	/*
	** Shutdown any remaining associations.
	*/
	for( 
	     assoc_id = gca_next_acb( -1 ); 
	     assoc_id >= 0;
	     assoc_id = gca_next_acb( assoc_id ) 
	   )
	{
	    GCA_DA_PARMS	da_parms;
	    STATUS		status;

	    da_parms.gca_association_id = assoc_id;

	    IIGCa_call( GCA_DISASSOC, (GCA_PARMLIST *)&da_parms, 
			GCA_SYNC_FLAG, NULL, -1L, &status );
	}

	/*
	** Cleanup ACB global resources.
	*/
	gca_free_acbs();
	MUr_semaphore( &IIGCa_global.gca_acb_semaphore );

	/* 
	** Call GCterminate to perform system-dependent cleanup 
	*/
	GCterminate( &svc_parms->gc_parms );

#ifdef GCF_EMBEDDED_GCN

	/* 
	** Shutdown the embedded Name Server interface 
	*/
	if ( IIGCa_global.gca_embedded_gcn )  gca_ns_term();

#endif /* GCF_EMBEDDED_GCN */

	/*
	** Shutdown security.
	*/
	IIgcs_call( GCS_OP_TERM, GCS_NO_MECH, NULL );

	IIGCa_global.gca_initialized = FALSE;
    }

    return;
} 

