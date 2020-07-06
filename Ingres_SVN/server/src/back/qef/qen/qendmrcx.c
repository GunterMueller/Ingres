/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <ex.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <adf.h>
#include    <ade.h>
#include    <dmf.h>
#include    <dmrcb.h>
#include    <dmtcb.h>
#include    <qefmain.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>
#include    <dmmcb.h>
#include    <dmucb.h>
#include    <tm.h>

#include    <dudbms.h>
#include    <qefqeu.h>
#include    <qeuqcb.h>
#include    <rqf.h>
#include    <tpf.h>
#include    <qefkon.h>
#include    <qefqry.h>
#include    <qefcat.h>
#include    <sxf.h>
#include    <qefprotos.h>

/**
**
**  Name: QENDMRCX.C - execute compiled expressions on behalf of DMF
**
**  Description:
**      DMF allows for one function and one parameter to that function
**  to be passed to DMF that will be executed to further qualify tuples. 
**  The routines in this file perform the qualification.
**  
**          qen_dmr_cx - execute a compiled expression for DMR
**
**
**  History:    $Log-for RCS$
**      4-aug-86 (daved)
**          written
**	20-sep-88 (puree)
**	    minor modification to qen_dmr_cx for better program structure.
**	08-dec-92 (anitap)
**	    Included <qsf.h> and <psfparse.h> for CREATE SCHEMA.
**      14-sep-93 (smc)
**          Added <cs.h> for CS_SID.
[@history_template@]...
**/


/*{
** Name: QEN_DMR_CX	- execute compiled expression to qualify a tuple.
**
** Description:
**      This routine is called by DMF to qualify a tuple using the compiled
**	expression specified in the DSH (dsh_qe_adf).  The pointer to the
**	tuple is passed to this routine by DMF.  The ade_execute_cx is called
**	to execute the qualification function as usual.  The value returned
**	to DMF is the disposition of the tuple.
**
**	NOTICE:
**	THIS ROUTINE DOES NOT RETURN AN ERROR CODE TO THE CALLER.  IF AN
**      ERROR OCCURS, THE ONLY ACTION TAKEN IS TO SET THE DISPOSITION CODE
**	TO DMR_F_ABORT.
**
**	Also notice that this qualifier routine assumes that there is
**	a CX to run, whose QEN_ADF was stashed in the DSH.
**	If there is no CX to run, we shouldn't get here.
**
** Inputs:
**	dsh			    current DSH struct
**	  .dsh_qe_adf	    	    current QEF_ADF - initilizer for ADE_EXCB
**		.qen_pos	    index into DSH->DSH_CBS where ADE_EXCB
**				    is located
**		.qen_input	    location in base array for the tuple
**	  .dsh_qefcb		    qef session control block
**	    .qef_adf_cb		    adf session control block
**	dmr_tuple		    ptr to the data tuple
**
** Outputs:
**      none
**	Returns:
**	    DMR_F_NEXT		    tuple doesn't qualify - get next one.
**	    DMR_F_RETURN	    tuple qualifies - return this tuple
**	    DMR_F_ABORT		    abort the search - a serious problem
**				    was encountered.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      4-aug-86 (daved)
**          written
**	11-aug-88 (puree)
**	    issue EXdelete on EXDECLARE return.
**	20-sep-88 (puree)
**	    Use the for-loop/break to assure s single exit point.
**	08-aug-90 (davebf)
**	    handle dsh->dsh_qe_adf being null
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	21-jul-1993 (rog)
**	    EXdeclare() returns STATUS, so test against OK, not E_DB_OK.
**	24-feb-04 (inkdo01)
**	    Add parm to qef_adf_error() for || query thread safety.
**	25-feb-04 (inkdo01)
**	    Changed 1st parm from QEF_CB to DSH for thread safety.
**	24-aug-04 (inkdo01)
**	    Place row addr in global base array when requested.
**	13-Dec-2005 (kschendel)
**	    Don't stack QEF's signal handler.  We're really still in DMF
**	    here, let DMF handle any traps.  (We don't do anything
**	    especially qef-ish here anyway.)
**	    Remove "no CX" test since callers will now never ask to
**	    get here if there's no qualifier to run.
**	14-Dec-2005 (kschendel)
**	    Move ADF CB pointer to DSH.
**	1-Nov-2006 (kschendel) b119034
**	    As much as I despise doing this, restore the signal handler
**	    foolishness.  DMF/GWF don't know what to do with overflows
**	    and other ADF exceptions.  This slows large scans by several
**	    percentage points, and really needs fixed a different way.
**	11-Oct-2007 (kibro01) Correct bug number 119304 -> 119034
*/
DB_STATUS
qen_dmr_cx(
QEE_DSH		*dsh,
PTR		dmr_tuple )
{
    QEN_ADF		*qen_adf = dsh->dsh_qe_adf;
    ADE_EXCB		*ade_excb;
    DB_STATUS		status;
    EX_CONTEXT		ex_context;

    /* FIXME FIXME we need to use QEF's signal handler only because
    ** DMF and/or GWF don't know what to do with ADF exceptions, and
    ** they barf horribly.
    ** EXdeclare takes a remarkable amount of time, not sure why, and
    ** a better answer is badly needed.  (most likely, teach dmf/qef
    ** to do better with ADF exceptions.)
    */

    if (EXdeclare(qef_handler, &ex_context) != OK)
    {
	return (DMR_F_ABORT);
    }
    /* all returns must EXdelete first... */

    /* set the base for the input tuple */
    ade_excb = (ADE_EXCB*) dsh->dsh_cbs[qen_adf->qen_pos];
    if (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY)
	dsh->dsh_row[qen_adf->qen_input] = dmr_tuple;
    else ade_excb->excb_bases[qen_adf->qen_input + ADE_ZBASE] = dmr_tuple;

    /* we will use the main segment */
    ade_excb->excb_seg = ADE_SMAIN;

    /* qualify the tuple */
    status = ade_execute_cx(dsh->dsh_adf_cb, ade_excb);
    if (status != E_DB_OK)
    {
	if (qef_adf_error(&dsh->dsh_adf_cb->adf_errcb, status, 
		dsh->dsh_qefcb, &dsh->dsh_error) != E_DB_OK)
	{
	    EXdelete();
	    return (DMR_F_ABORT);
	}
    }
    else 
    {
	if (ade_excb->excb_value == ADE_TRUE)
	{
	    EXdelete();
	    return (DMR_F_RETURN);
	}
    }

    EXdelete();
    return (DMR_F_NEXT);
}
