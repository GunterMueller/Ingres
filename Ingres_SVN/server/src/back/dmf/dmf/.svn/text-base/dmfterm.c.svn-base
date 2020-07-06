/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <pc.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lk.h>
#include    <lg.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmpp.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0s.h>
#include    <dm0m.h>
#include    <dm0p.h>
#include    <dmfinit.h>
/**
** Name: DMFTERM.C - Termindate the DMF working environment used by the 
**		     recovery process, archiver, and recoverdb.
**
** Description:
**      This file contains the function that deallocate the DMF server
**	control block. 
**
**      This file defines:
**    
**      dmf_term() -  Routine to deallocate the DMF server control block.
**
**  History:    $Log-for RCS$
**	Oct-22-1986 (ac)
**	    Created for Jupiter.
**	8-july-1992 (ananth)
**	    Prototyping DMF
**	3-oct-1992 (bryanp)
**	    Re-arrange includes to share transaction ID type definition.
**	14-dec-1992 (rogerk)
**	    Include dm1b.h before dm0l.h for prototype definitions.
**	26-apr-1993 (bryanp)
**	    Include dmfinit.h for prototype definitions. I don't think that
**	    anyone actually calls dmf_term(), so we perhaps should just deep-six
**	    this file.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	20-sep-1993 (andys)
**	     Fix arguments to ule_format call.
**	24-Oct-1996 (jenjo02)
**	    Replaced hcb_mutex with hash_mutex, one for each hash entry.
**	    Added hcb_tq_mutex to protect TCB free queue.
**	17-Jan-1997 (jenjo02)
**	    Replaced svcb_mutex with svcb_dq_mutex (DCB queue) and
**	    svcb_sq_mutex (SCB queue).
**	28-jul-1998 (rigka01)
**          Cross integrate fix for bug 90691 from 1.2 to 2.0 code line
**          In highly concurrent environment, temporary file names may not
**          be unique so improve algorithm for determining names
**          Initialize svcb_DM2F_TempTblCnt_mutex
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	05-Mar-2002 (jenjo02)
**	    Call dms_stop_server() for Sequence Generators.
**      20-sept-2004 (huazh01)
**          Pull out the fix for b90691 as the fix has been 
**          rewritten. 
**          INGSRV2643, b111502.
**	15-Jan-2004 (schka24)
**	    hcb mutex array changed, reflect here.
**	6-Feb-2004 (schka24)
**	    No more temp name generator mutex.
**      05-Aug-2005 (hanje04)
**          Back out change 478041 as it should never have been X-integrated
**          into main in the first place.
**/

/*{
** Name: dmf_term - deallocate the DMF server control block.
**
** Description:
**
**    Function prototypes defined in DM.H.
**
**    This function causes the DMF working environment used by the recovery 
**    processs, archiver and recoverdb to be terminated.
**
** Inputs:
**
** Output:
**	err_code			    reason for error status.
**
**      Returns:
**          E_DB_OK                         Function completed normally. 
**          E_DB_ERROR                      Function completed abnormally.
**
** History:
**	Oct-22-1986 (ac)
**	    Created for Jupiter.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	20-sep-1993 (andys)
**	     Fix arguments to ule_format call.
**	24-Oct-1996 (jenjo02)
**	    Replaced hcb_mutex with hash_mutex, one for each hash entry.
**	    Added hcb_tq_mutex to protect TCB free queue.
**      20-sept-2004 (huazh01)
**          Pull out the fix for b90691 as it has been
**          rewritten. 
**          INGSRV2643, b111502. 
**	15-Aug-2005 (jenjo02)
**	    Implement multiple DMF LongTerm memory pools and
**	    ShortTerm session memory.
**	    Removed dm0m_mrelease of svcb_mem_mutex. 
**	    In SINGLEUSER mode, dm0s semaphores aren't used,
**	    we really ought to clean up the others, too...
*/

DB_STATUS
dmf_term(
i4		*err_code)
{
    DM_SVCB		    *svcb = dmf_svcb;
    i4		    status = E_DB_OK;
    i4		    error, local_error;
    CL_SYS_ERR		    sys_err;
    DMP_HCB		    *hcb;
    i4			    i;

    for (;;)
    {
	/* Destroy/deallocate any remaining DML_SEQ's */
	if ( svcb->svcb_seq )
	    dms_stop_server();

	/*  Deallocate logging context. */

	status = dm0l_deallocate(&svcb->svcb_lctx_ptr, err_code);
	if (status != E_DB_OK)
	    break;

	/* Release the hcb mutexes */
	hcb = svcb->svcb_hcb_ptr;
	dm0s_mrelease(&hcb->hcb_tq_mutex);
	for (i = 0; i < HCB_MUTEX_ARRAY_SIZE; i++)
	{
	    dm0s_mrelease(&hcb->hcb_mutex_array[i]);
	}

	/*  Deallocate the HCB. */

	dm0m_deallocate((DM_OBJECT **)&hcb);

	svcb->svcb_hcb_ptr = 0;

	/*  Deallocate the BMCB. */

	status = dm0p_deallocate(err_code);
	if (status != E_DB_OK)
	    break;

	/*  Release the server lock list. */

	status = LKrelease(LK_ALL, svcb->svcb_lock_list, (LK_LKID *)0, 
            (LK_LOCK_KEY *)0,
	    (LK_VALUE *)0, &sys_err);
	if (status != OK)
	{
	    (VOID) ule_format(E_DM901B_BAD_LOCK_RELEASE, &sys_err, ULE_LOG, NULL,
		    (char *)0, 0L, (i4 *)0, err_code, 1, 
                    0, svcb->svcb_lock_list);
	    break;
	}

	/*  Deallocate mutexes. */

	dm0s_mrelease(&svcb->svcb_dq_mutex);
	dm0s_mrelease(&svcb->svcb_sq_mutex);

	return (E_DB_OK);
    }
    ule_format(*err_code, 0, ULE_LOG, NULL, 0, 0, 0, err_code, 0);
    *err_code = E_DM9412_DMF_TERM;
    return(E_DB_ERROR);
}
