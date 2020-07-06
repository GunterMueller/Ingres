
/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: DM1CH.H - Definitions of common constants and functions.
**
** Description:
**      This header describes the visible standard accessors so that they
**	can also be used in other formats. These are the compression
**	accessors.
**
** History:
**  27-oct-1994 (shero03)
**      Created.
**  22-jul-1996 (ramra01)
**      Add row_version argument to dmpp_uncompress prototype.
**  06-sep-1996 (canor01)
**	Add buffer argument to dm1ch_uncompress prototype.
**  13-aug-99 (stephenb)
**	alter def for dm1ch_uncompress, buffer should be a char** since it is
**	deallocated in the called function.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	16-mar-2004 (gupsh01)
**	    Added rcb_adf_cb to the parameter list of dm1ch_uncompress to support
**	    alter table alter column.
**/

FUNC_EXTERN DB_STATUS  dm1ch_compress(
			    DB_ATTS		**atts_array,
			    i4		atts_count,
			    char		*rec,
			    i4		rec_size,
			    char 	      	*crec,
			    i4		*crec_size);

FUNC_EXTERN DB_STATUS  dm1ch_uncompress(
			    DB_ATTS		**atts_array,
			    i4    		 atts_count,
			    char		*src,
			    char		*dst,
			    i4		dst_maxlen,
			    i4    		 *dst_size,
			    i4 		record_type,
			    DM_TID		*tid,
			    char		**buffer,
			    i4		row_version,
			    PTR		rcb_adf_cb);

FUNC_EXTERN i4       dm1ch_compexpand(
			    DB_ATTS		**atts,
			    i4			att_cnt);
