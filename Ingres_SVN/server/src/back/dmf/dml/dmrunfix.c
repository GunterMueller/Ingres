/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <me.h>
#include    <sl.h>
#include    <sr.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <pc.h>
#include    <tm.h>
#include    <adf.h>
#include    <dmf.h>
#include    <ulf.h>
#include    <sxf.h>
#include    <dmrcb.h>
#include    <dmccb.h>
#include    <dmxcb.h>
#include    <dmtcb.h>
#include    <dmacb.h>
#include    <lg.h>
#include    <lk.h>
#include    <dm.h>
#include    <dml.h>
#include    <dmp.h>
#include    <dmse.h>
#include    <dm1b.h>
#include    <dm2umct.h>
#include    <dm2r.h>
#include    <dm2rlct.h>
#include    <dma.h>
#include    <gwf.h>

/**
** Name: DMRUNFIX.C - Implements the DMF unfix pages operation.
**
** Description:
**      This file contains the functions necessary to
**	unfix any pages still fixed by the DMR_CB's DMP_RCB.
**
**      This file defines:
**
**      dmr_unfix - Unfix pages.
**
** History:
**	09-Apr-2004 (jenjo02)
**	    Invented for partitioning.
**/

/*{
** Name:  dmr_unfix - Unfix pages.
**
**  INTERNAL DMF call format:      status = dmr_unfix(&dmr_cb);
**
**  EXTERNAL call format:          status = dmf_call(DMR_UNFIX,&dmr_cb);
**
** Description:
**	This function unfixes any still-fixed pages in the
**	caller's DMR_CB->DMP_RCB.
**
**	It is typically used, for example, when crossing a
**	partition boundary when doing a table scan.
**
**	It lets the caller leave the table open but without any
**	of its pages fixed in cache.
**
**	Unless the partition's table is either closed or
**	unfixed, the pages remain fixed in cache and when
**	"lots" of partitions are involved in a query, the  
**	cache can quickly become saturated.
**
** Inputs:
**      dmr_cb
**          .type                           Must be set to DMR_RECORD_CB.
**          .length                         Must be at least
**                                          sizeof(DMR_RECORD_CB) bytes.
**          .dmr_access_id                  Record access identifer returned
**                                          from DMT_OPEN that identifies a
**                                          table.
**
** Outputs:
**      dmr_cb
**          .error.err_code                 One of the following error numbers.
**                                          E_DM0000_OK                
**                                          E_DM002A_BAD_PARAMETER
**	    				    E_DM019A_ERROR_UNFIXING_PAGES
**
**      Returns:
**         E_DB_OK                          Function completed normally.
**         E_DB_WARN                        Function completed normally with a 
**                                          termination status which is in
**                                          dmr_cb.err_code.
**         E_DB_ERROR                       Function completed abnormally 
**                                          with a 
**                                          termination status which is in
**                                          dmr_cb.err_code.
**         E_DB_FATAL                       Function completed with a fatal
**                                          error which must be handled
**                                          immediately.  The fatal status is in
**                                          dmr_cb.err_code.
** History:
**	09-Apr-2004 (jenjo02)
**	    Invented for partitioning.
**      04-jan-2005 (stial01)
**          Set RCB_CSRR_LOCK so that locks can get released (b113231)
*/
DB_STATUS
dmr_unfix(
DMR_CB  *dmr_cb)
{
    DMR_CB	*dmr = dmr_cb;
    DMP_RCB	*rcb;
    DB_STATUS	status;
    i4		error;

    if ( (rcb = (DMP_RCB *)dmr->dmr_access_id) &&
	 (dm0m_check((DM_OBJECT *)rcb, (i4)RCB_CB) == E_DB_OK) )
    {
	/* Unfix any pages, if any */
        if (rcb->rcb_iso_level == RCB_CURSOR_STABILITY ||
            rcb->rcb_iso_level == RCB_REPEATABLE_READ)
	    rcb->rcb_state |= RCB_CSRR_LOCK;
	status = dm2r_unfix_pages(rcb, &error);
    }
    else
    {
	ule_format(E_DM00E0_BAD_CB_PTR, (CL_SYS_ERR *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 1,
	    sizeof("record")-1, "record");
	error = E_DM002A_BAD_PARAMETER;
	status = E_DB_ERROR;
    }

    if ( status )
    {
	/* This is pretty bad; abort the transaction */
	rcb->rcb_xcb_ptr->xcb_state |= XCB_TRANABORT;

	if (error > E_DM_INTERNAL)
	{
	    ule_format( error, 0, ULE_LOG , NULL,
		(char * )0, 0L, (i4 *)0, 
		&error, 0);
	    ule_format(E_DM904F_ERROR_UNFIXING_PAGES, 0, ULE_LOG, NULL,
		(char * )0, 0L, (i4 *)0, &error, 3,
		sizeof(DB_DB_NAME), &rcb->rcb_tcb_ptr->tcb_dcb_ptr->dcb_name,
		sizeof(DB_OWN_NAME), &rcb->rcb_tcb_ptr->tcb_rel.relowner,
		sizeof(DB_TAB_NAME), &rcb->rcb_tcb_ptr->tcb_rel.relid
		);
	    error = E_DM019A_ERROR_UNFIXING_PAGES;
	}
    }

    dmr->error.err_code = error;
    return (status);
}
