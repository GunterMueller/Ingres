/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <tm.h>
#include    <cs.h>
#include    <me.h>
#include    <pc.h>
#include    <sr.h>
#include    <sl.h>
#include    <tr.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lg.h>
#include    <lk.h>
#include    <adf.h>
#include    <adp.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dmccb.h>
#include    <dmrcb.h>
#include    <dmtcb.h>
#include    <dmucb.h>
#include    <dmxcb.h>
#include    <dm.h>
#include    <dml.h>
#include    <dmp.h>
#include    <dmxe.h>
#include    <dm2t.h>
#include    <dm1b.h>
#include    <dmpp.h>
#include    <dm1p.h>
#include    <di.h>
#include    <sxf.h>
#include    <dma.h>
#include    <dmse.h>
#include    <dm2umct.h>
#include    <dm2umxcb.h>
#include    <dm2u.h>
#include    <dmftrace.h>
#include    <dmu.h>
#include    <dmpecpn.h>

/**
**
** Name: DMUATBL.C - Functions needed to modify the column definitions
**
** Description:
**      This file contains the functions necessary to alter a table's columns.
**    
**      dmu_atable()       -  Routine to perform the ALTER TABLE operation.
**
** History:
**	18-oct-1993 (bryanp)
**	    Created.
**	27-Feb-1997 (jenjo02)
**	    Extended <set transaction statement> to support <transaction access mode>
**	    (READ ONLY | READ WRITE) per SQL-92.
**	    Added E_DM006A_TRAN_ACCESS_CONFLICT.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	18-Jan-2001 (jenjo02)
**	    Subvert calls to dma_write_audit() if not C2SECURE.
**      29-nov-2005 (horda03) Bug 48659/INGSRV 2716
**          A TX can now be started with a non-journaled BT record. This
**          leaves the XCB_DELAYBT flag set but sets the XCB_NONJNL_BT
**          flag. Need to write a journaled BT record if the update
**          will be journaled.
**/

/* Name: dmu_atable - Alter table (add or drop column to a table).
**
**
** EXTERNAL call format:          status = dmf_call(DMU_ALT_TABLE,&dmu_cb);
**
** Description:
**
**	The dmu_atable function handles the alter table command.
**	The table should be owned by the user however, it is presumed
**	that this has been verified by the caller.
**	
**	This function will error if a column that is part of an index
**	is dropped.  It will also only work on base tables.
**	
**	Functions performed by this routine are dependent upon the 
**	characteristics passed in.  An ADD column is performed if the
**	value of the DMU_ALTER_TYPE characteristic is DM_C_ADD_ALTER.  
**	A DROP column is performed if value of the DMU_ALTER_TYPE 
**	characteristic is DM_C_DROP_ALTER.
**	The "cascade" option of the "alter table table_name drop col cascade"
**	command is performed if the DMU_CASCADE characteristic is input with the
**	value of DMU_C_ON.
**	
**	If the "CASCADE" option is enabled then any indexes dependent upon
**	the dropped column will automatically be dropped.  The caller should
**	have guaranteed that any views, integrity constraints, or grants 
**	dependent upon the dropped column.
**	
**	Note: For B1 secure server, this routine must make sure
**	that the security label of the requester is exactly the
**	same as the security label on the table.  If is not 
**	identical, the operation is rejected with the error
**	E_DB0125_DDL_SECURITY_ERROR.
**	
** Inputs:
**	dmu_cb 
**	     .type                           Must be set to DMU_UTILITY_CB.
**	     .length                         Must be at least sizeof(DMU_CB).
**	     .dmu_flags_mask                 Must be zero.
**	     .dmu_tran_id                    Must be the current transaction 
**					     identifier returned from the 
**					     begin transaction operation.
**	     .dmu_tbl_id                     Internal name of table to be 
**					     altered.
**	     .dmu_char_array.data_address    Pointer to an area used to input
**					     and array of entries of type
**					     DMU_CHAR_ENTRY.
**					     See below for description of 
**					     <dmu_char_array> entries.
**	     .dmu_char_array.data_in_size    Length of char_array in bytes.
**	
**	
**	     <dmu_char_array> entries are of type DMU_CHAR_ENTRY and
**	     must have following format:
**	     char_id                         Must be one of the dmu 
**					     characteristics:
**					     DMU_ALTER_TYPE
**					     DMU_CASCADE
**	
**	     char_value                      The value to associate with above
**					     characteristic.
**	
** Outputs:
**	dmu_cb
**	 .dmu_tup_cnt                    Set to -1.
**	
**	 .error.err_code                 One of the following error numbers.
**					 E_DM0000_OK                
**					 E_DM0006_BAD_ATTR_FLAG
**					 E_DM0007_BAD_ATTR_NAME
**					 E_DM0009_BAD_ATTR_SIZE
**					 E_DM0008_BAD_ATTR_PRECISION
**					 E_DM000A_BAD_ATTR_TYPE
**					 E_DM000B_BAD_CB_LENGTH
**					 E_DM000C_BAD_CB_TYPE
**					 E_DM000D_BAD_CHAR_ID
**					 E_DM000E_BAD_CHAR_VALUE
**					 E_DM0010_BAD_DB_ID
**					 E_DM001A_BAD_FLAG
**					 E_DM002A_BAD_PARAMETER
**					 E_DM003B_BAD_TRAN_ID
**					 E_DM0042_DEADLOCK
**					 E_DM0045_DUPLICATE_KEY
**					 E_DM004A_INTERNAL_ERROR
**					 E_DM004B_LOCK_QUOTA_EXCEEDED
**					 E_DM0054_NONEXISTENT_TABLE
**					 E_DM005D_TABLE_ACCESS_CONFLICT
**					 E_DM005E_CANT_UPDATE_SYSCAT
**					 E_DM0064_USER_INTR
**					 E_DM0065_USER_ABORT
**	    				 E_DM006A_TRAN_ACCESS_CONFLICT
**					 E_DM009F_ILLEGAL_OPERATION
**					 E_DM0100_DB_INCONSISTENT
**					 E_DM0103_TUPLE_TOO_WIDE
**					 E_DM010C_TRAN_ABORTED
**					 E_DM0125_DDL_SECURITY_ERROR
**	
**	.error.err_data                  Set to attribute in error by 
**					 returning index into attribute list.
**	
** Returns:
**	E_DB_OK
**	E_DB_ERROR
**
** History:
**	01-sep-1995 ( athda01)
**	    Created.
**	16-oct-1996 (ramra01)
**	    Support adding and dropping columns that use
**	    extended catalogs such as long varchar and long varbyte.
**	    Pass down relstat information to dm2u routines.
**	27-Feb-1997 (jenjo02)
**	    Extended <set transaction statement> to support <transaction access mode>
**	    (READ ONLY | READ WRITE) per SQL-92.
**	    Added E_DM006A_TRAN_ACCESS_CONFLICT.
**	17-dec-1999 (gupsh01)
**	    Added breaks in the switch statement to enable correct error 
**	    reporting.  
**	17-Apr-2001 (jenjo02)
**	    Repaired misplaced test for status==E_DB_OK, return;
**	6-Feb-2004 (schka24)
**	    Get rid of DMU statement count and its limit.
**	13-Apr-2004 (gupsh01)
**	    Added alter table alter column support.
**	13-dec-04 (inkdo01)
**	    Added support for attr_collID and db_collID.
**      04-Apr-2005 (chopr01) (b113295)
**          Initialized variables table_owner and table_name
**	11-Nov-2005 (jenjo02)
**	    Replaced dmx_show() with the more robust 
**	    dmxCheckForInterrupt() to standardize external
**	    interrupt handling.
*/

DB_STATUS
dmu_atable(DMU_CB    *dmu_cb)
{
    DMU_CB		*dmu = dmu_cb;
    DML_XCB		*xcb;
    DML_ODCB		*odcb;
    i4		operation;
    i4		cascade;
    i4		error, local_error;
    DB_STATUS		status;
    i4             db_lockmode;
    DB_OWN_NAME		table_owner;
    DB_TAB_NAME		table_name;
    u_i4		setrelstat2 = 0;
    i4		i;
    i4		mask;
    i4		char_count, attr_count;
    DMU_ATTR_ENTRY	**attr_entry;
    DMU_CHAR_ENTRY	*char_entry;
    DB_STATUS		s;
    ADF_CB		adf_cb;
    i4		relstat = 0;
    DB_DATA_VALUE	adc_dv1;
    i4			bits;
    SXF_ACCESS		access;
    i4		message;
    i4			gateway = 0;

    dmu->dmu_tup_cnt = DM_NO_TUPINFO;
    status = E_DB_ERROR;
    table_owner = dmu->dmu_owner;
    table_name = dmu->dmu_table_name;
    for (;;)
    {
	/* dmu_flags_mask may be set to DMU_VGRANT_OK */
	error = E_DM001A_BAD_FLAG;
	mask = ~(DMU_VGRANT_OK|DMU_INTERNAL_REQ);
        if (dmu->dmu_flags_mask != 0 && (dmu->dmu_flags_mask & mask) != 0)
	    break;

	error = E_DM003B_BAD_TRAN_ID;
	xcb = (DML_XCB*) dmu->dmu_tran_id;
	
	if (dm0m_check((DM_OBJECT *)xcb, (i4)XCB_CB) != E_DB_OK)
	    break;

	/* Check for external interrupts */
	if ( xcb->xcb_scb_ptr->scb_ui_state )
	    dmxCheckForInterrupt(xcb, &error);

	if (xcb->xcb_state & XCB_USER_INTR)
	{
	    error = E_DM0065_USER_INTR;
	    break;
	}
	if (xcb->xcb_state & XCB_FORCE_ABORT)
	{
	    error = E_DM010C_TRAN_ABORTED;
	    break;
	}
	if (xcb->xcb_state & XCB_ABORT)
	{
	    error = E_DM0064_USER_ABORT;
	    break;
	}	    


	error = E_DM0010_BAD_DB_ID;
	odcb = (DML_ODCB*) dmu->dmu_db_id;
	if (dm0m_check((DM_OBJECT *)odcb, (i4)ODCB_CB) != E_DB_OK)
	    break;
	if (xcb->xcb_scb_ptr != odcb->odcb_scb_ptr)
	    break;

	/*  Check that this is a update transaction on the database 
        **  that can be updated. */

	if (odcb != xcb->xcb_odcb_ptr)
	{
	    error = E_DM005D_TABLE_ACCESS_CONFLICT;
	    break;
	}

	if (xcb->xcb_x_type & XCB_RONLY)
	{
	    error = E_DM006A_TRAN_ACCESS_CONFLICT;
	    break;
	}

	db_lockmode = DM2T_X;

	/*  Check for characteristics. */
	cascade = 0;
	operation = 0;

	if (dmu->dmu_char_array.data_address && 
		dmu->dmu_char_array.data_in_size)
	{
	    char_entry = (DMU_CHAR_ENTRY*) dmu->dmu_char_array.data_address;
	    char_count = dmu->dmu_char_array.data_in_size / 
			    sizeof(DMU_CHAR_ENTRY);

	    error = 0;

	    for (i = 0; i < char_count; i++)
	    {
		switch (char_entry[i].char_id)
		{

		case DMU_ALTER_TYPE:
		    operation = (char_entry[i].char_value);
		    if (operation != DMU_C_ADD_ALTER &&
			operation != DMU_C_DROP_ALTER &&
			operation != DMU_C_ALTCOL_ALTER)
		    {
			dmu->error.err_data = i;
			error = E_DM006D_BAD_OPERATION_CODE;
		    }
		    break;

		case DMU_CASCADE:
		    cascade = (char_entry[i].char_value == DMU_C_ON);
		    break;

		default:
		    dmu->error.err_data = i;
		    error = E_DM000D_BAD_CHAR_ID;
		}
		if (error)
		    break;
	    }
	    if (error)
		break;
	}

	if (!operation)

	{
	   error = E_DM002A_BAD_PARAMETER;
	   break;
	}

	/* Check the attribute values. */

	if (dmu->dmu_attr_array.ptr_address && 
	    (dmu->dmu_attr_array.ptr_size == sizeof(DMU_ATTR_ENTRY))
	    )
	{
	    attr_entry = (DMU_ATTR_ENTRY**) dmu->dmu_attr_array.ptr_address;
	    attr_count = dmu->dmu_attr_array.ptr_in_count;
	    if (attr_count != 1)
	    {
	       dmu->error.err_data = attr_count;
	       error = E_DM002A_BAD_PARAMETER;
	       break;
	    }
	     
	    dmu->error.err_data = 0;

	    if ((operation == DMU_C_ADD_ALTER) ||
	        (operation == DMU_C_ALTCOL_ALTER))
	    {

	       error = E_DM0009_BAD_ATTR_SIZE;
	       adf_cb.adf_errcb.ad_ebuflen = 0;
	       adf_cb.adf_errcb.ad_errmsgp = 0;
	       adf_cb.adf_maxstring = DB_MAXSTRING;
               adc_dv1.db_datatype = attr_entry[0]->attr_type;
               adc_dv1.db_length   = attr_entry[0]->attr_size;
               adc_dv1.db_prec     = attr_entry[0]->attr_precision;
               adc_dv1.db_collID   = attr_entry[0]->attr_collID;
	       s = adc_lenchk(&adf_cb, FALSE, &adc_dv1, NULL);
	       if (s != E_DB_OK)
	       {
		   if (adf_cb.adf_errcb.ad_errcode == E_AD2004_BAD_DTID)
		       error = E_DM000A_BAD_ATTR_TYPE;
		   break;
    	       }	    	
	       s = adi_dtinfo(&adf_cb, attr_entry[0]->attr_type, &bits);
	       if (s != E_DB_OK)
	       {
		   error = adf_cb.adf_errcb.ad_errcode;
		   break;
	       }
	       if (bits & AD_PERIPHERAL)
	       {
		    /*
		    ** If peripheral datatypes are found,
		    **	1) mark the attribute as peripheral, and
		    **	2) mark the relation as having extensions.
		    **
		    **	The `having extensions' is necessary because DMF uses
		    **	table keys to keep the peripheral datatypes in sync with
		    **	the base tuple.  A table key in the peripheral
		    **	datatype's coupon is used as the key in the table
		    **	extension.
		    */
		    
		   attr_entry[0]->attr_flags_mask |= DMU_F_PERIPHERAL;
		   relstat |= DM2U_HAS_EXTENSIONS;
	       }
		   /* check for appropriate attribute flags */
	       if (attr_entry[0]->attr_flags_mask & ~DMU_F_LEGAL_ATTR_BITS)
	       {
		   error = E_DM0006_BAD_ATTR_FLAGS;
		   break;
	       }
	    }
	}
	else
	{
	     error = E_DM002A_BAD_PARAMETER;
	     break;
	}

    	/*
    	** If this is the first write operation for this transaction,
    	** then we need to write the begin transaction record.
    	*/

    	if ((xcb->xcb_flags & XCB_DELAYBT) != 0 )
    	{
	   status = dmxe_writebt(xcb, TRUE, &error);
	   if (status != E_DB_OK)
	   {
	      xcb->xcb_state |= XCB_TRANABORT;
	      break;
	   }
    	}

    	/* Calls the physical layer to process the rest of the alter table */

    	status = dm2u_atable(odcb->odcb_dcb_ptr, xcb,
			     &dmu->dmu_tbl_id, operation, cascade, 
			     (DM2T_ATTR_ENTRY **)attr_entry, relstat,
			     db_lockmode, &table_name, &table_owner, &error);

    	/*
    	**  Audit alteration of Table 
    	*/

    	if ( status == E_DB_OK && dmf_svcb->svcb_status & SVCB_C2SECURE )
    	{
           access=SXF_A_SUCCESS;

           if (gateway) 
           {
	      access |= SXF_A_REGISTER;
	      message = I_SX2043_GW_REGISTER;
           }
           else
           {
	      access |= SXF_A_ALTER;
	      message = I_SX2815_ALTER;
           }
					
           status = dma_write_audit(
				    SXF_E_TABLE,
				    access,
				    dmu->dmu_table_name.db_tab_name,
				    sizeof(dmu->dmu_table_name.db_tab_name),
				    &dmu->dmu_owner, 
				    message,
				    FALSE, /* Not force */
				    &dmu->error.err_code, NULL);

	}
        if (status == E_DB_OK)
	    return (E_DB_OK);
	break;
    }		/* end of for ;; */

    if (error > E_DM_INTERNAL)
    {
	ule_format(error, (CL_SYS_ERR *)0, ULE_LOG, NULL, (char * )0,
	           (i4)0, (i4 *)0, &local_error, 0);
	error = E_DM010A_ERROR_ALTERING_TABLE;
    }

    switch (error)
    {
	case E_DM004B_LOCK_QUOTA_EXCEEDED:
	case E_DM0112_RESOURCE_QUOTA_EXCEED:
	case E_DM010A_ERROR_ALTERING_TABLE:
	case E_DM0136_GATEWAY_ERROR:
	case E_DM006A_TRAN_ACCESS_CONFLICT:
	     xcb->xcb_state |= XCB_STMTABORT;break;

	case E_DM0042_DEADLOCK:
	case E_DM004A_INTERNAL_ERROR:
	case E_DM0100_DB_INCONSISTENT:
	     xcb->xcb_state |= XCB_TRANABORT;break;

	case E_DM0065_USER_INTR:
	     xcb->xcb_state |= XCB_USER_INTR;break;

	case E_DM010C_TRAN_ABORTED:
	     xcb->xcb_state |= XCB_FORCE_ABORT;break;
	default:break;
    }

    dmu->error.err_code = error;
    return (E_DB_ERROR);
}
