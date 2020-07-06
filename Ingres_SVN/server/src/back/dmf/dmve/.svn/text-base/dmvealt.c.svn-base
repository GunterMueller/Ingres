/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <di.h>
#include    <pc.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <tr.h>
#include    <lg.h>
#include    <lk.h>
#include    <adf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmtcb.h>
#include    <dm2t.h>
#include    <dmve.h>
#include    <dmpp.h>
#include    <dm0c.h>
#include    <dm1b.h>
#include    <dm0l.h>

/**
**
**  Name: DMVEALTER.C - The backout of an alter table operation.
**
**  Description:
**	This file contains the routine for the backout of an alter table
**	operation.  There is no work required for roll-forward as the
**	actual updates to the system catalogs are logged separately.
**
**          dmve_alter - The recovery of an alter table operation.
**
**
**  History:    
**      22-jun-87 (rogerk)
**          Created new for Jupiter.
**	07-july-1992 (jnash)
**	    Add DMF function prototyping 
**	3-oct-1992 (bryanp)
**	    Re-arrange includes to share transaction ID type definition.
**	29-nov-1992 (jnash)
**	    Reduced logging project.  Add CLR handling.
**	14-dec-1992 (rogerk)
**	    Include dm1b.h before dm0l.h for prototype definitions.
**	30-feb-1993 (rmuth)
**	    Prototyping DI, need to include di.h
**	15-mar-1993 (jnash & rogerk)
**	    Reduced Logging - Phase IV:
**	    Changed log record format: added database_id, tran_id and LSN
**		in the log record header.  The LSN of a log record is now
**		obtained from the record itself rather than from the dmve
**		control block.
**	    Check dmve->dmve_logging to determine if logging required.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	15-may-1993 (rmuth)
**	    Readonly table support: dm2t_open the table for 
**	    DM2T_A_OPEN_NOACESS mode so that we can purge both READONLY
**	    and NOREADONLY tables.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (rogerk)
**	    Include respective CL headers for all cl calls.
**	23-aug-1993 (rogerk)
**	  - Broke main body of function into dmv_remodify and dmv_unmodify.
**	  - Added rollforward handling.
**	18-oct-1993 (rogerk)
**	    Added dmve_unreserve_space call to release logfile space
**	    reserved for logforces in the purge tcb call.
**	15-apr-1994 (chiku)
**	    Bug56702: return logfull indication.
**      20-may-95 (hanch04)
**          Added include dmtcb.h for ndmfapi
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      14-jul-2003
**          Added include adf.h
**/

static DB_STATUS dmv_realter(
	DMVE_CB         *dmve,
	DM0L_ALTER      *log_rec,
	i4         *err_code);


static DB_STATUS dmv_unalter(
	DMVE_CB         *dmve,
	DM0L_ALTER      *log_rec,
	i4         *err_code);



/*{
** Name: dmve_alter - UNDO recovery of an alter table opration.
**
** Description:
**	This function purges the TCB for the altered table during recovery of
**	an alter table function.  All changes to the system catalogs to perform
**	the alter are logged and recovered individually.
**
** Inputs:
**	dmve_cb
**	    .dmve_log_rec	    The alter table operation log record.
**	    .dmve_action	    Should only be DMVE_UNDO.
**	    .dmve_dcb_ptr	    Pointer to DCB.
**
** Outputs:
**	dmve_cb
**	    .dmve_error.err_code    The reason for error status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	22-jun-87 (rogerk)
**          Created for Jupiter.
**	6-Oct-88 (Edhsu)
**	    Fix error mapping bug
**	29-nov-1992 (jnash)
**	    Reduced logging project.  Add CLR handling and half-hearted
**	    attempt at partial location recovery.
**	15-mar-1993 (jnash)
**	    Check dmve->dmve_logging to determine if logging required.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Replace all uses of DM_LOG_ADDR with LG_LA or LG_LSN.
**	15-may-1993 (rmuth)
**	    Readonly table support: dm2t_open the table for 
**	    DM2T_A_OPEN_NOACESS mode so that we can purge both READONLY
**	    and NOREADONLY tables.
**	23-aug-1993 (rogerk)
**	  - Broke main body of function into dmv_remodify and dmv_unmodify.
*/
DB_STATUS
dmve_alter(
DMVE_CB		*dmve_cb)
{
    DMVE_CB		*dmve = dmve_cb;
    DM0L_ALTER		*log_rec = (DM0L_ALTER *)dmve_cb->dmve_log_rec;
    i4		recovery_action;
    i4		error;
    DB_STATUS		status = E_DB_OK;
	
    for (;;)
    {
	if (log_rec->dua_header.length > sizeof(DM0L_ALTER) || 
	    log_rec->dua_header.type != DM0LALTER ||
	    dmve->dmve_action == DMVE_REDO)
	{
	    error = E_DM9601_DMVE_BAD_PARAMETER;
	    break;
	}

	/*
	** Call appropriate recovery action depending on the recovery type
	** and record flags.  CLR actions are always executed as an UNDO
	** operation.  Code within UNDO recognizes the CLR and does not
	** write another CLR log record.
	*/
	recovery_action = dmve->dmve_action;
	if (log_rec->dua_header.flags & DM0L_CLR)
	    recovery_action = DMVE_UNDO;

        switch (recovery_action)
	{
	case DMVE_DO:
	    status = dmv_realter(dmve, log_rec, &error);
	    break;

	case DMVE_UNDO:
	    status = dmv_unalter(dmve, log_rec, &error);
	    break;
	}

	if (status != E_DB_OK)
	    break;

	return(E_DB_OK);
    }

    /*
    ** Error handling
    */

    ule_format(error, (CL_ERR_DESC *)0, ULE_LOG, NULL,
	(char *)0, (i4)0, (i4 *)0, &error, 0);
    dmve->dmve_error.err_code = E_DM961B_DMVE_ALTER;

    return(E_DB_ERROR);
}

/*{
** Name: dmv_realter	- REDO Recovery of a table alter operation
**
** Description:
**
**	This routine is used during REDO recovery of an alter of a table.  
**
**	The updates of the alter operation are redon by re-applying the
**	system catalog updates via recovery of their respective log records.
**
**	This routine purges the table TCB.
**
** Inputs:
**	dmve			Recovery context block.
**	log_rec			Alter log record.
**
** Outputs:
**	err_code		The reason for error status.
**
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	23-aug-1993 (rogerk)
**	    Routine created from dmve_alter code.
*/
static DB_STATUS
dmv_realter(
DMVE_CB         *dmve,
DM0L_ALTER 	*log_rec, 
i4		*err_code)
{
    LG_LSN		*log_lsn = &log_rec->dua_header.lsn; 
    LG_LSN		lsn;
    i4		dm0l_flags;
    DB_STATUS		status = E_DB_OK;

    for (;;)
    {
	/*
	** Purge the TCB so it can be re-built in its altered state.
	*/
	status = dm2t_purge_tcb(dmve->dmve_dcb_ptr, &log_rec->dua_tbl_id,
			(DM2T_PURGE | DM2T_NORELUPDAT), dmve->dmve_log_id, 
			dmve->dmve_lk_id, &dmve->dmve_tran_id, 
			dmve->dmve_db_lockmode, err_code);
	if (status != E_DB_OK)
	    break;

	return(E_DB_OK);
    }

    ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
	(char *)0, (i4)0, (i4 *)0, err_code, 0);
    *err_code = E_DM9688_REDO_ALTER;

    return(E_DB_ERROR);
}

/*{
** Name: dmv_unalter	- UNDO Recovery of a table alter operation
**
** Description:
**
**	This routine is used during UNDO recovery of an alter of a table.  
**
**	The updates of the alter operation are rolled back by undoing the
**	system catalog updates via recovery of their respective log records.
**
**	This routine purges the table TCB (and notifies other servers of
**	the structure change by bumping the cache lock).
**
** Inputs:
**	dmve			Recovery context block.
**	log_rec			Alter log record.
**
** Outputs:
**	err_code		The reason for error status.
**
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	23-aug-1993 (rogerk)
**	    Routine created from dmve_alter code.
**	18-oct-1993 (rogerk)
**	    Added dmve_unreserve_space call to release logfile space
**	    reserved for logforces in the purge tcb call.
**	15-apr-1994 (chiku)
**	    Bug56702: return logfull indication.
*/
static DB_STATUS
dmv_unalter(
DMVE_CB         *dmve,
DM0L_ALTER 	*log_rec, 
i4		*err_code)
{
    LG_LSN		*log_lsn = &log_rec->dua_header.lsn; 
    LG_LSN		lsn;
    i4		dm0l_flags;
    DB_STATUS		status = E_DB_OK;

    for (;;)
    {
	/*
	** Alter was aborted.  Purge the table descriptor in case
	** someone (presumably this transaction) has done a build_tcb 
	** on the table in its altered state.
	*/
	status = dm2t_purge_tcb(dmve->dmve_dcb_ptr, &log_rec->dua_tbl_id,
			(DM2T_PURGE | DM2T_NORELUPDAT), dmve->dmve_log_id, 
			dmve->dmve_lk_id, &dmve->dmve_tran_id, 
			dmve->dmve_db_lockmode, err_code);
	if (status != E_DB_OK)
	    break;

	/*
	** Write the CLR if necessary.
	*/
	if ((dmve->dmve_logging) &&
	    ((log_rec->dua_header.flags & DM0L_CLR) == 0))
	{
	    dm0l_flags = (log_rec->dua_header.flags | DM0L_CLR);

	    status = dm0l_alter(dmve->dmve_log_id, dm0l_flags, 
				&log_rec->dua_tbl_id, &log_rec->dua_name, 
				&log_rec->dua_owner, log_rec->dua_count,
				log_rec->dua_action, log_lsn, &lsn, err_code);
	    if (status != E_DB_OK) {
		/*
		 * Bug56702: return logfull indication.
		 */
		dmve->dmve_error.err_data = *err_code;
		break;
	    }

	    /*
	    ** Release log file space allocated for the logforce done
	    ** in the purge tcb call above.
	    */
	    dmve_unreserve_space(dmve, 1);
	}

	return(E_DB_OK);
    }

    ule_format(*err_code, (CL_ERR_DESC *)0, ULE_LOG, NULL, 
	(char *)0, (i4)0, (i4 *)0, err_code, 0);
    *err_code = E_DM9689_UNDO_ALTER;

    return(E_DB_ERROR);
}
