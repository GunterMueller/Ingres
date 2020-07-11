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
#include    <lg.h>
#include    <lk.h>
#include    <adf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmtcb.h>
#include    <dm2t.h>
#include    <dm2r.h>
#include    <dm1b.h>
#include    <dmve.h>
#include    <dmpp.h>
#include    <dm0l.h>

/**
**
**  Name: DMVEUNDO.C - Undo Recovery entry point.
**
**  Description:
**
**          dmve_undo - Apply undo recovery for given log record.
**
**  History:
**	14-dec-1992 (rogerk)
**	    Written for 6.5 recovery.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	21-jun-1993 (rogerk)
**	    Replaced extraneous comment with this one.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	23-aug-93 (jnash)
**	    Add E_DM967A_DMVE_UNDO error msg to provide LSN after failure.
**	18-oct-93 (jnash)
**	    Fix typo in E_DM967A_DMVE_UNDO error msg.
**	18-oct-93 (jrb)
**	    Added DM0LEXTALTER handing for MLSorts.
**      20-may-95 (hanch04)
**          Added include dmtcb.h for ndmfapi
**	17-oct-1996 (shero03)
**	    Added support for Rtree rtdel, rtput, and rtrep
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	29-Apr-2004 (gorvi01)
**		Added case for DM0LDELLOCATION.
**/

/*
** Forward Declarations
*/


/*{
** Name: dmve_undo - Apply undo recovey for given log record.
**
** Description:
**
** Inputs:
**	dmve_cb
**	    .dmve_log_rec	    The log record of the btree delete operation
**	    .dmve_action	    Should be DMVE_DO, DMVE_REDO, or DMVE_UNDO.
**	    .dmve_lg_addr	    The log address of the log record.
**	    .dmve_dcb_ptr	    Pointer to DCB.
**	    .dmve_tran_id	    The physical transaction id.
**	    .dmve_lk_id		    The transaction lock list id.
**	    .dmve_log_id	    The logging system database id.
**	    .dmve_db_lockmode	    The lockmode of the database. Should be 
**				    DM2T_X or DM2T_S.
**
** Outputs:
**	dmve_cb
**	    .dmve_error.err_code    The reason for error status.
**
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	14-dec-1992 (rogerk)
**	    Written for 6.5 recovery.
**	23-aug-93 (jnash)
**	    Add E_DM967A_DMVE_UNDO error msg to provide LSN after failure.
**	18-oct-93 (jnash)
**	    Fix typo in E_DM967A_DMVE_UNDO error msg.
**	18-oct-93 (jrb)
**	    Added DM0LEXTALTER handing for MLSorts.
*/
DB_STATUS
dmve_undo(
DMVE_CB		*dmve)
{
    DM0L_HEADER		*record = (DM0L_HEADER *) dmve->dmve_log_rec;
    DB_STATUS		status = E_DB_OK;
    i4		error;


    /*
    ** Skip records not needed for UNDO processing:
    **
    **     - Before Images logged for Online Backup.
    **     - CLR records.
    */

    if (record->flags & (DM0L_CLR | DM0L_DUMP))
	return (E_DB_OK);

    /*
    ** Process record by type.
    */
    switch (record->type)
    {
    case DM0LBI:
	status = dmve_bipage(dmve);
	break;

    case DM0LPUT:
	status = dmve_put(dmve);
	break;
    
    case DM0LDEL:
	status = dmve_del(dmve);
	break;
    
    case DM0LREP:
	status = dmve_rep(dmve);
	break;

    case DM0LASSOC:
	status = dmve_assoc(dmve);
	break;

    case DM0LALLOC:
	status = dmve_alloc(dmve);
	break;

    case DM0LDEALLOC:
	status = dmve_dealloc(dmve);
	break;

    case DM0LEXTEND:
	status = dmve_extend(dmve);
	break;

    case DM0LFRENAME:
	status = dmve_frename(dmve);
	break;

    case DM0LMODIFY:
	status = dmve_modify(dmve);
	break;

    case DM0LFCREATE:
	status = dmve_fcreate(dmve);
	break;

    case DM0LSM0CLOSEPURGE:
	status = dmve_sm0_closepurge(dmve);
	break;

    case DM0LSM1RENAME:
	status = dmve_sm1_rename(dmve);
	break;

    case DM0LSM2CONFIG:
	status = dmve_sm2_config(dmve);
	break;

    case DM0LLOCATION:
	status = dmve_location(dmve);
	break;

    case DM0LEXTALTER:
	status = dmve_ext_alter(dmve);
	break;

    case DM0LCREATE:
	status = dmve_create(dmve);
	break;

    case DM0LINDEX:
	status = dmve_index(dmve);
	break;

    case DM0LRELOCATE:
	status = dmve_relocate(dmve);
	break;

    case DM0LDESTROY:
	status = dmve_destroy(dmve);
	break;

    case DM0LALTER:
	status = dmve_alter(dmve);
	break;

    case DM0LLOAD:
	status = dmve_load(dmve);
	break;

    case DM0LCRDB:
	status = dmve_crdb(dmve);
	break;

    case DM0LDMU:
	status = dmve_dmu(dmve);
	break;

    case DM0LOVFL:
	status = dmve_ovfl(dmve);
	break;

    case DM0LNOFULL:
	status = dmve_nofull(dmve);
	break;

    case DM0LFMAP:
	status = dmve_fmap(dmve);
	break;

    case DM0LBTPUT:
	status = dmve_btput(dmve);
	break;

    case DM0LBTDEL:
	status = dmve_btdel(dmve);
	break;

    case DM0LBTSPLIT:
	status = dmve_btsplit(dmve);
	break;

    case DM0LBTOVFL:
	status = dmve_btovfl(dmve);
	break;

    case DM0LBTFREE:
	status = dmve_btfree(dmve);
	break;

    case DM0LBTUPDOVFL:
	status = dmve_btupdovfl(dmve);
	break;

    case DM0LRTPUT:
	status = dmve_rtput(dmve);
	break;

    case DM0LRTDEL:
	status = dmve_rtdel(dmve);
	break;

    case DM0LRTREP:
	status = dmve_rtrep(dmve);
	break;

    case DM0LDISASSOC:
	status = dmve_disassoc(dmve);
	break;

    case DM0LDELLOCATION:
	status = dmve_del_location(dmve);
	break;

    default:
	/*
	** Log record type that has no associated UNDO action.
	*/
	break;
    }

    if (status != E_DB_OK)
    {
	ule_format(dmve->dmve_error.err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
	    (char *)0, (i4)0, (i4 *)0, &error, 0);
	ule_format(E_DM967A_DMVE_UNDO, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	    (char *)0, (i4)0, (i4 *)0, &error, 2, 
	    0, record->lsn.lsn_high, 0,  record->lsn.lsn_low);
	dmve->dmve_error.err_code = E_DM9639_DMVE_UNDO;
    }

    return(status);
}