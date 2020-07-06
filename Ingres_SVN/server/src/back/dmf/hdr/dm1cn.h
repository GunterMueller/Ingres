
/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: DM1CN.H - Definitions of common constants and functions.
**
** Description:
**      This header describes the visible standard accessors so that they
**	can also be used in other formats. These are the compression
**	accessors.
**
** History: 
**	16-jul-1992 (kwatts)
**	    MPF project. Created.
**	16-sep-1992 (jnash)
**	    Reduced logging project.
**	     -  Add FUNC_EXTERNs for dm1cn_format, dm1cn_load, dm1cn_isnew,
**		dm1cn_reclen, dm1cn_get_offset, used by dm1cs.c.  
**	     -  Add record_type and tid param's to dm1cn_uncompress, required
**		to support old style compression within dm1cn_uncompress().
**	22-oct-1992 (rickh)
**	    Replaced all references to DMP_ATTS with DB_ATTS.
**	03-nov-1992 (jnash))
**	    Add dm1cn_reallocate FUNC_EXTERN.
**	24-july-1997(angusm)
**	    Add dmpp_dput().
**	06-mar-1996 (stial01 for bryanp)
**	    Add page_size argument to dm1cn_format, dm1cn_reallocate,
**		dm1cn_reclen prototypes.
**	06-may-1996 (thaju02)
**	    Add page_size argument to dm1cn_isnew.
**	20-may-1996 (ramra01)
**	    Added new param to the load accessor
**      22-jul-1996 (ramra01 for bryanp)
**          Add row_version argument to dm1cn_uncompress.
**          Add current_table_version(tup_info) argument to dm1cn_load.
**	06-sep-1996 (canor01)
**	    Add buffer argument to dm1cn_uncompress prototype.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          load accessor: changed DMPP_TUPLE_INFO param to table_version
**      05-oct-98 (stial01)
**          Updated dmpp_dput parameters, Added dmpp_put_hdr
**	13-aug-99 (stephenb)
**	    Alter def for dm1cn_uncomress.
**	05-dec-99 (nanpr01)
**	    Fixed up the prototypes for performance.
**	23-Jun-2000 (jenjo02)
**	    Added fill_option to dmpp_format() prototype, set by caller
**	    to DM1C_ZERO_FILL if dmpp_format should MEfill the page,
**	    DM1C_ZERO_FILLED if the page has already been MEfilled.
**	    The page overhead space is always zero-filled.
**      30-oct-2000 (stial01)
**          Changed prototype for dmpp_isnew page accessor
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      01-feb-2001 (stial01)
**          Arg changes for page accessors (Variable Page Type SIR 102955)
**      09-mar-2001 (stial01)
**          Changes for Variable Page Type SIR 102955
**      28-may-2003 (stial01)
**          Added line_start parameter to dmpp_allocate accessor
**      10-feb-2004 (stial01)
**          Support for 256K rows, rows spanning pages.
**      27-sep-2006 (stial01)
**          Fixed prototype for dm1cn_uncompress()
**/

FUNC_EXTERN DB_STATUS   dm1cn_allocate(
			    DMPP_PAGE               *data,
			    DMP_RCB                 *r,
			    char		    *record,
			    i4                      record_size,
			    DM_TID                  *tid,
			    i4			    line_start);
FUNC_EXTERN bool	dm1cn_isempty(
			    DMPP_PAGE               *data,
			    DMP_RCB                 *r);
FUNC_EXTERN DB_STATUS	dm1cn_check_page(
			    DM1U_CB                 *dm1u,
			    DMPP_PAGE               *page,
			    i4                      page_size);
FUNC_EXTERN VOID	dm1cn_delete(
			    i4		    page_type,	
			    i4		    page_size,
			    DMPP_PAGE	    *page,
			    i4              update_mode,
			    i4         reclaim_space,
			    DB_TRAN_ID      *tran_id,
			    DM_TID          *tid,
			    i4              record_size);

FUNC_EXTERN DB_STATUS	dm1cn_compress(
			    DB_ATTS	**atts_array,
			    i4	atts_count,
			    char       	*rec,
			    i4	rec_size,
			    char       	*crec,
			    i4	*crec_size);

FUNC_EXTERN DB_STATUS	dm1cn_uncompress(
			    DB_ATTS    **atts_array,
			    i4     atts_count,
			    char	*src,
			    char	*dst,
			    i4	dst_maxlen,
			    i4     *dst_size,
			    i4 	record_type,
			    DM_TID 	*tid,
			    char	**buffer,
			    i4		row_version,
			    PTR		rcb_adf_cb);

FUNC_EXTERN i4		dm1cn_compexpand(
			    DB_ATTS	**atts,
			    i4		att_cnt);

FUNC_EXTERN VOID	dm1cn_format(
			    i4		    page_type,
			    i4		    page_size,
			    DMPP_PAGE       *page,
			    DM_PAGENO       pageno,
			    i4              stat,
			    i4		    fill_option);
FUNC_EXTERN DB_STATUS  dm1cn_get(
			    i4		    page_type,
			    i4		    page_size,
			    DMPP_PAGE	    *page,
			    DM_TID          *tid,
			    i4      *record_size,
			    char            **record,
			    i4		    *row_version,
			    u_i4	    *row_low_tran,
			    DMPP_SEG_HDR    *seg_hdr);

FUNC_EXTERN i4          dm1cn_get_offset(
			    i4		    page_type,
			    i4		    page_size,
			    DMPP_PAGE       *page,
			    i4             item);
FUNC_EXTERN i4		dm1cn_getfree(
			    DMPP_PAGE       *page,
			    i4              page_size);

FUNC_EXTERN i4 	dm1cn_isnew(
			    DMP_RCB	    *r,
			    DMPP_PAGE       *page,
			    i4             line_num);

FUNC_EXTERN i4		dm1cn_ixsearch(
			    i4		    page_type,
			    i4		    page_size,
			    DMPP_PAGE       *page,
			    DB_ATTS         **keyatts,
			    i4              keys_given,
			    char            *key,
			    bool            partialkey,
			    i4              direction,
			    i4              relcmptlvl,
			    ADF_CB          *adf_cb);
FUNC_EXTERN DB_STATUS    dm1cn_load(
			    i4		    page_type,
			    i4		    page_size,
			    DMPP_PAGE       *data,
			    char            *record,
			    i4         record_size,
			    i4         record_type,
			    i4         fill,
			    DM_TID          *tid,
			    u_i2            current_table_version,
			    DMPP_SEG_HDR    *seg_hdr);
FUNC_EXTERN VOID dm1cn_put(
			    i4		    page_type,
			    i4		    page_size,
			    DMPP_PAGE	    *page,
			    i4      update_mode,
			    DB_TRAN_ID      *tran_id,
			    DM_TID          *tid,
			    i4              record_size,
			    char            *record,
			    i4              record_type,
			    u_i2            current_table_version,
			    DMPP_SEG_HDR    *seg_hdr);

FUNC_EXTERN DB_STATUS	dm1cn_reallocate(
			    i4		page_type,
			    i4		page_size,
			    DMPP_PAGE	*page,
			    i4		line_num,
			    i4		reclen);

FUNC_EXTERN VOID         dm1cn_reclen(
			    i4		page_type,
			    i4		page_size,
			    DMPP_PAGE	*page,
			    i4         lineno,
			    i4         *record_size);

FUNC_EXTERN i4		 dm1cn_tuplecount(
			    DMPP_PAGE       *page,
			    i4              page_size);
                        
FUNC_EXTERN i4	         dm1cn_dput(
			    DMP_RCB	*rcb,
			    DMPP_PAGE	*page, 
			    DM_TID	*tid,
			    i4		*err_code);
