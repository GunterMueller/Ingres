/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: DMFGW.H - DMF Gateway Support Program definitions.
**
** Description:
**      This file contains the structures and constants used by the gateway
**	support routines.
**
** History:
**      06-jul-1992 (ananth)
**          Created for prototyping DMF.
**	2-oct-1992 (bryanp/robf)
**	    Added char_array argument to dmf_gwt_register.
**      17-dec-1992 (schang)
**          add dest_rcb argument to dmf_gwt_remove
**	10-sep-93 (swm)
**	    Changed session_id type from i4 to CS_SID in function
**	    interfaces, to match recent CL interface revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	08-Jan-2001 (jenjo02)
**	    Correctly cast "session_id (GW_SESSION*)" to PTR instead
**	    of CS_SID.
*/

/* Function prototype definitions. */

FUNC_EXTERN DB_STATUS dmf_gwf_init(
    i4	*gateway_active,
    i4	*gw_xacts,
    i4	*err_code);

FUNC_EXTERN DB_STATUS dmf_gws_term(
    PTR	session_id,
    i4	*err_code);

FUNC_EXTERN DB_STATUS dmf_gwt_register(
    PTR		    	    session_id,
    DB_TAB_NAME		    *table_name,
    DB_OWN_NAME		    *table_owner,
    DB_TAB_ID		    *table_id,
    i4		    attr_count,
    DM2T_ATTR_ENTRY	    **attr_list,
    DM_DATA		    *gwchar_array,
    DM_PTR		    *gwattr_array,
    DML_XCB		    *xcb,
    DMU_FROM_PATH_ENTRY	    *source,
    i4		    gw_id,
    i4		    *tuple_count,
    i4		    *page_count,
    DM_DATA		    *char_array,
    i4		    *err_code);

FUNC_EXTERN DB_STATUS dmf_gwt_remove(
    i4	    gateway_id,
    PTR	    	    session_id,
    DB_TAB_ID	    *table_id,
    DB_TAB_NAME	    *table_name,
    DB_OWN_NAME	    *table_owner,
    DMP_RCB         *dest_rcb,
    DML_XCB	    *xcb,
    i4	    *err_code);

FUNC_EXTERN DB_STATUS dmf_gwt_open(
    DMP_RCB *rcb,
    i4 *err_code);

FUNC_EXTERN DB_STATUS dmf_gwt_close(
    DMP_RCB *rcb,
    i4 *err_code);

FUNC_EXTERN DB_STATUS dmf_gwr_position(
    DMP_RCB *rcb,
    i4 *err_code);

FUNC_EXTERN DB_STATUS dmf_gwr_get(
    DMP_RCB	*rcb,
    DM_TID	*tid,
    char	*record,
    i4	flag,
    i4	*err_code);

FUNC_EXTERN DB_STATUS dmf_gwr_put(
    DMP_RCB	*rcb,
    char	*record,
    i4	record_size,
    DM_TID	*tid,
    i4	*err_code);

FUNC_EXTERN DB_STATUS dmf_gwr_replace(
    DMP_RCB	*rcb,
    char	*oldrecord,
    i4	oldlength,
    char	*newrecord,
    i4	newlength,
    DM_TID	*oldtid,
    DM_TID	*newtid,
    i4	*err_code);

FUNC_EXTERN DB_STATUS dmf_gwr_delete(
    DMP_RCB	*rcb,
    DM_TID	*tid,
    i4	*err_code);

FUNC_EXTERN DB_STATUS dmf_gwx_begin(
    DML_SCB	*scb,
    i4	*err_code);

FUNC_EXTERN DB_STATUS dmf_gwx_commit(
    DML_SCB	*scb,
    i4	*err_code);

FUNC_EXTERN DB_STATUS dmf_gwx_abort(
    DML_SCB	*scb,
    i4	*err_code);

