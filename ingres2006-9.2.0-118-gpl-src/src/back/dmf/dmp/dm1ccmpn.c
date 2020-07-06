/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <me.h>
#include    <mh.h>
#include    <pc.h>
#include    <tr.h>
#include    <st.h>
#include    <cs.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <adf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <lg.h>
#include    <lk.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmpp.h>
#include    <dmppn.h>
#include    <dm1c.h>
#include    <dm1cn.h>

/**
**
**  Name: DM1CCMPN.C - Normal tuple-level format compression routines.
**
**  Description:
**      This file contains the routines needed to compress 
**      tuples on normal data pages. Since the manipulation 
**      is done via accessor functions, a set needs to be provided for each
**	supported page type. These are then formed into the two accessor
**	structures referenced by the dm1c_getaccessors function:
**
**	dm1cn_plv	- page level vector for CAFS format
**
**	dm1cn_tlv	- tuple level vecor for CAFS format
**
**      The normal (dm1cn_) functions defined in this file are:
**
**	compress	- Compress a record
**	uncompress	- Uncompresses a record
**	compexpand	- how much can a tuple expand when compressed
**	old_get  	- Old tuple get
**	old_uncomp_rec  - Old tuple uncompress
**
**	NB. We use same routines for BTREE index compression.
**
**	old_get and old_uncomp_rec really do not belong here, and
**	may be removed at some time in the future.
**
**	Normal page level accessors are defined in dm1cn.c 
**	
**  History:
**	13-oct-1992 (jnash)
**	    Reduced Logging project.  Created from dm1cn.c to 
**	    isolate compression code from page access.
**	10-nov-1992 (jnash)
**	    Replaced references to DMP_ATTS with DB_ATTS.  
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**		Include <lg.h> to pick up logging system typedefs
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Include <lk.h> to pick up locking system type definitions.
**	    Include <pc.h> before <lk.h> to pick up PID definition
**	31-jan-1994 (jnash)
**	    In dm1cn_uncompress(), prefill output buffer with zeros to 
**	    eliminate rollforwarddb dmve_replace() "tuple_mismatch" 
**	    warnings after modify operations.
**	16-may-1996 (shero03)
**	    Support variable length UDTs.
**	23-may-1994 (andys)
**	    Add cast(s) to quiet qac/lint
**      17-jul-1996 (ramra01 for bryanp)
**          Alter Table support: Add new row_version argument to
**          dm1cn_uncompress, and use it to convert older versions of rows
**          to their current versions while uncompressing them.
**	13-sep-1996 (canor01)
**	    Add buffer to dmpp_uncompress call to pass buffer for data
**	    uncompression.
**	23-sep-1996 (canor01)
**	    Moved global data definitions to dmpdata.c.
**      29-Jan-97 (fanra01)
**          Pass adf_scb as address.  On NT compiler places whole structure on
**          stack.
**      28-May-97 (fanra01)
**          Bug 82393
**          Changed instances where adf_scb was being passed to functions as
**          a structure to a pointer.
**          NT compiler places whole structure on the stack.
**	08-Oct-1997 (shero03)
**	    Found an instance similar to 82393.  The call to adc_compr
**	    requires the addr(dv) - not dv itself.
**	25-Feb-1998 (shero03)
**	    Account for a null coupon.
**	06-Apr-1998 (hanch04/nanpr01)
**	    Performace improvements.  Added case for common types.
**	22-Apr-1998 (hanch04)
**	    Add missing assignment for compress.
**	18-May-1998 (jenjo02)
**	    Repaired some code broken by 06-Apr performance changes.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-may-2001 (stial01)
**          Back out some 06-Apr-1998 Performance improvements (B104806)
**      31-may-2001 (stial01)
**          Fixed cross integration of 28-Aug-2000 change which moved
**          the line 'src += DB_CNTSIZE' out of place.
**       2-Sep-2004 (hanal04) Bug 111474 INGSRV2638 
**          dm1cn_uncompress() was incorrectly materialising data for
**          columns added with defaults.
**      18-feb-2004 (stial01)
**          Fixed incorrect casting of length arguments.
**      18-feb-2005 (srisu02)
**          Fixed a call to MEcopy() as the arguments were in the wrong order
**	21-Apr-2005 (thaju02)
**	    In dm1cn_uncompress, increment src to account for dropped 
**	    nchar/nvarchar cols. (B114367)
**	01-Aug-2005 (sheco02)
**	    Since C2secure was removed for open source, remove the related
**	    code here to fix the x-integration 477914 problem.
**      12-sep-2005 (stial01)
**          dm1cn_uncompress() changes for alter table alter column (b115164)
**      17-jan-2006 (stial01)
**          Fix uncompress of dropped nullable BLOB column (b115642,b115643)
**      27-sep-2006 (stial01)
**          Fixed args to dm1cn_uncompress, use caller adf_cb (b116737)
**/


/*
**  Forward function references (used in accessor definition).
*/

static DB_STATUS    dm1cn_old_uncomp_rec(
			DB_ATTS     **atts_array,
			i4     atts_count,
			char	    *src,
			char	    *dst,
			i4	    dst_maxlen,
			i4     *dst_size);


/*{
** Name: dm1cn_tlv - Normal format tuple level vector of accessors
**
** Description:
**	This the the accessor structure that dm1c_getaccessors will use when
**	passed parameters asking for standard accessors without the
**	special forward compatibility compresion scheme.
**
** History:
**      03_jun_92 (kwatts)
**	    Created.
**	23-sep-1996 (canor01)
**	    Moved to dmpdata.c.
*/

GLOBALREF DMPP_ACC_TLV dm1cn_tlv;


/*
** Name: dmpp_compress - Compress a record, given pointers to attributes
**
** Description:
**      Compress the record into work area provided.
**
**	NOTE! This routine is the BTREE one that everyone uses now
**
**	Compression Algorithms:
**	    Nullable datatypes - nullable datatypes are compressed by writing
**			one byte null descriptor (indicating whether the value
**			is NULL) to the front of the compressed value.  If the
**			value is NULL, then no further data is written.  If the
**			value is not NULL, then the data value itself is
**			compressed and written following the null descriptor.
**
**	    C		- copied up to the last non-blank character and then
**			a NULL byte ('\0') is written.
**	    CHAR	- number of bytes to the last non-blank character is
**			calculated.  Two-byte length desciptor is written
**			followed by the string up to this length.
**	    VARCHAR, TEXT - Two byte length descriptor is written giving the
**			length indicated in the VARCHAR or TEXT datatype,
**			followed by this many bytes from the data portion of
**			the datatype.
**	    INT, FLOAT, DATE, other - copied as is: no compression.
**
** Inputs:
**      atts_array                      Pointer to a list of pointers to
**                                      attribute descriptors.
**      atts_count                      Number of entries in atts_array.
**      
**      rec                             Pointer to record to compress.
**      rec_size                        Uncompressed record size. 
**      crec                            Pointer to an area to return compressed
**                                      record.
**
** Outputs:
**      crec_size		        Pointer to an area where compressed
**                                      record size can be returned.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR			Tuple could not be compressed.
**
**	7-mar-1991 (bryanp)
**	    Created for the Btree index compression project.
**	13-oct-1992 (jnash)
**	    Make att_ln i2 to quiet compiler.
**	16-may-1996 (shero03)
**	    Support variable length UDTs.
**      29-Jan-97 (fanra01)
**          Pass adf_scb as address.
**	08-Oct-1997 (shero03)
**	    Found an instance similar to 82393.  The call to adc_compr
**	    requires the addr(dv) - not dv itself.
**	25-Feb-1998 (shero03)
**	    Account for a null coupon.
**	03-Oct-1999 (wanfr01)
** 	    Partial cross-integration of change 435281 - performance improvement
**	    to dm1cn_compress and dm1cn_umcompress
**	30-Aug-2000 (thaju02)
**	    If columns have been previous dropped via alter table, then
**	    avoid attempting to compress for dropped column. (b102455)
**	06-Apr-1998 (hanch04/nanpr01)
**	    Performace improvements.  Added case for common types.
**	18-May-1998 (jenjo02)
**	    Repaired some code broken by 06-Apr performance changes.
**	09-Feb-2005 (gupsh01)
**	    Added support for nchar/nvarchar.
**	26-Apr-2005 (gupsh01)
**	    Fix the nchar case.
**	21-Jun-2005 (gupsh01)
**	    Fix processing for nchar case. NCHAR lengths are number of
**	    unicode characters.
**	03-Aug-2006 (gupsh01)
**	    Added ANSI date/time types.
*/
DB_STATUS	
dm1cn_compress(
			DB_ATTS         **atts_array,
			i4         atts_count,
			char            *rec,
			i4         rec_size,
			char            *crec,
			i4         *crec_size)
{
    register char	*ptr;
    register char	*dst;
    register DB_ATTS	*att;
    register char	*ptr2;
    register char       *src;
    register i2		type;
    register i2		abs_type;
    u_i2	char_ln;
    i2		att_ln;
    i4     i;
    i4	error;
    i4		dt_bits;
    i4     compr_len;     
    ADF_CB      adf_scb;
    i4		adi_dtinfo_called;

    src = rec;
    dst = crec;

    for (i = 0; i < atts_count; i++)
    {
	att = atts_array[i];

        att_ln = att->length;
	adi_dtinfo_called = FALSE;

	type = att->type;
	abs_type = (type < 0) ? -type: type;

	/*
	** If nullable datatype, write byte to front of data value that
	** indicates whether it is NULL or not.  If it is NULL, then go
	** on to next column;  otherwise, fall through to compress the
	** actual data value.
	** A null coupon is passed anyway.
	*/

        if (att->ver_dropped > att->ver_added)
            continue;

	if (type < 0)
	{
	    *dst = src[--att_ln];
	    if (*dst++ & ADF_NVL_BIT)
	    {
		/* Null data. Obtain info about the data type */
		adf_scb.adf_errcb.ad_ebuflen = 0;
		adf_scb.adf_errcb.ad_errmsgp = 0;
		adf_scb.adf_maxstring = DB_MAXSTRING;
		adi_dtinfo(&adf_scb, abs_type, &dt_bits);

		adi_dtinfo_called = TRUE;

		if ((dt_bits & AD_PERIPHERAL) == 0)
		{
		    src += att->length;
		    continue;
		}
	    }
	}

	switch(abs_type)
	{
	case DB_CHR_TYPE:
	case DB_CHA_TYPE:
	    /*
	    ** Look for last non-blank character in the string.  The string
	    ** will be written out to this point.
	    */
	    ptr = src + att_ln - 1;
            for (; ptr >= src; ptr--)
            {
                if (*ptr != ' ')
                    break;
            }
	    if (abs_type == DB_CHR_TYPE)
	    {
		/*
		** C type ...
		** Convert all nulls to blanks.  There should not be any
		** nulls in the character string but sometimes they seem
		** to get there (usually from the copy command which doesn't
		** screen data the way it should). By converting to blanks
		** we avoid errors when decompressing the data since it is
		** assummed that character fields are null terminated.
		*/
		for (ptr2 = src; ptr2 <= ptr;)
		{
		    if (!(*dst++ = *ptr2++))
			*(dst - 1) = ' ';
		}
		*dst++ = '\0';
	    }
	    else
	    {
		/*
		** CHAR type ...
		** Calculate the current length and copy that into the
		** destination string as a two-byte length id. Then copy
		** the data portion into the destination string.
		*/
		ptr++;
		char_ln = ptr - src;
		I2ASSIGN_MACRO(char_ln, *(u_i2 *)dst);
		dst += sizeof(char_ln);
		MEcopy(src, char_ln, dst);
		dst += char_ln;
	    }
	    break;

	case DB_NCHR_TYPE:
	    /*
	    ** Look for last non-blank character in the string.  The string
	    ** will be written out to this point.
	    */
	    {
                u_i2	*usrc;
                u_i2	*psave;
                u_i2	*uend;

	        usrc = (u_i2 *)src;
                                  
	        psave = usrc + att_ln/sizeof(UCS2) - 1;
                for (; psave >= usrc; psave--)
                {
                    if (*psave != U_BLANK)
                        break;
	        }
	        psave++;
	        char_ln = psave - usrc; 
	        I2ASSIGN_MACRO(char_ln, *(u_i2 *)dst);
	        dst += sizeof(char_ln);
	        MEcopy(src, char_ln * sizeof(UCS2), dst);
	        dst += char_ln * sizeof(UCS2);
	    }
	    break;

	case DB_VCH_TYPE:
	case DB_TXT_TYPE:
	    I2ASSIGN_MACRO(((DB_TEXT_STRING *)src)->db_t_count, att_ln);
	    att_ln += DB_CNTSIZE;

	    /*
	    ** Verify that length descriptor is valid before writing it out.
	    ** Ideally, this check is not needed if we guaranteed all data
	    ** values were checked before being passed to the low levels of the
	    ** system.
	    */
	    if (att_ln <= (att->length))
	    {
		MEcopy(src, att_ln, dst);
		dst += att_ln;
		break;
	    }

	    /*
	    ** If we get here then we have a bad datatype - the length
	    ** descriptor has an impossible value.
	    */
	    ule_format(E_DM9388_BAD_DATA_LENGTH, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, &error, 1,
		0, (i4)att_ln);
	    ule_format(E_DM9389_BAD_DATA_VALUE, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, &error, 0);
	    return (E_DB_ERROR);

        case DB_NVCHR_TYPE:
        {
	    I2ASSIGN_MACRO(((DB_TEXT_STRING *)src)->db_t_count, att_ln);
            att_ln = att_ln * sizeof(UCS2) + DB_CNTSIZE;

            /* Check that length descriptor is valid */
	    if (att_ln <= (att->length))
	    {
		MEcopy(src, att_ln, dst);
		dst += att_ln;
		break;
	    }

	    /*
	    ** If we get here then we have a bad datatype - the length
	    ** descriptor has an impossible value.
	    */
	    ule_format(E_DM9388_BAD_DATA_LENGTH, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, &error, 1,
		0, (i4)att_ln);
	    ule_format(E_DM9389_BAD_DATA_VALUE, (CL_SYS_ERR *)0, ULE_LOG, NULL,
		(char *)0, (i4)0, (i4 *)0, &error, 0);
	    return (E_DB_ERROR);
        }
        break;

	case DB_INT_TYPE:
	case DB_FLT_TYPE:
	case DB_DTE_TYPE:
        case DB_ADTE_TYPE:
        case DB_TMWO_TYPE:
        case DB_TMW_TYPE:
        case DB_TME_TYPE:
        case DB_TSWO_TYPE:
        case DB_TSW_TYPE:
        case DB_TSTMP_TYPE:
        case DB_INDS_TYPE:
        case DB_INYM_TYPE:
	case DB_MNY_TYPE:
	case DB_DEC_TYPE:
	case DB_LOGKEY_TYPE:
	case DB_TABKEY_TYPE:
             MEcopy(src, att_ln, dst);
             dst += att_ln;
	    break;
	
	default:    /* non-NULL INT, FLT, DTE, MNY */
            if (abs_type >= ADI_DT_CLSOBJ_MIN )
	    {
		/* If we haven't already, get the dt_bits from adi_dtinfo() */
		if (adi_dtinfo_called == FALSE)
		{
		    adf_scb.adf_errcb.ad_ebuflen = 0;
		    adf_scb.adf_errcb.ad_errmsgp = 0;
		    adf_scb.adf_maxstring = DB_MAXSTRING;
		    adi_dtinfo(&adf_scb, abs_type, &dt_bits);
		}
		/* If UDT see if it is a VL UDT */
	        if (dt_bits & AD_VARIABLE_LEN)
	        {
		    /* Call the UDTs compress routine */
		    DB_DATA_VALUE tmp_dv;

		    tmp_dv.db_datatype = type;
		    tmp_dv.db_data = src;
		    tmp_dv.db_length = att_ln;

		    adc_compr(&adf_scb, &tmp_dv, TRUE, dst, &compr_len);
		    dst += (u_i2)compr_len;
		    break;
	        }
            }

	    MEcopy(src, att_ln, dst);
            dst += att_ln;
	    break;
        }
	src += att->length;	/* position source at the next attribute */
    }
    *crec_size = dst - crec;	/* calculate resulting tuple length */
    return (E_DB_OK);
}

/*
** Name: dmpp_uncompress - Uncompresses a record, given pointers to attributes
**
** Description:
**    This routine uncompresses a record and returns the record
**    and size in bytes.
**
**	NOTE! This routine is the BTREE version used all the time now.
**
** Inputs:
**      atts_array                      Pointer to a list of pointers to
**                                      attribute descriptors.
**      atts_count                      Value indicating number of attributes
**      src                             Pointer to an area containing record
**                                      to uncompress.
**	dst_maxlen			Size of the destination area. If, during
**					the uncompress operation, we detect
**					that we are exceeding the destination
**					area size, we abort the uncompress
**					operation and return an error.
**      record_type			Compression flag:
**					   TCB_C_OLD - Old style compression
**					   (only one of concern)
**	tid				Required only if record_type = TCB_C_OLD
**
** Outputs:
**      dst                             Pointer to an area to return
**                                      uncompressed record.
**      dst_size                        Size of uncompressed record.
**      
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR			Bad tuple encountered.
**
**	Exceptions:
**	    none
**
** Side Effects:
**          none.
**
** History:
**	7-mar-1991 (bryanp)
**	    Created for the Btree index compression project.
**	13-oct-1992 (jnash)
**	    Reduced logging project.  As part of separating
**	    compression from the page accessor code, added record_type
**	    and tid params, calling dm1cn_old_uncomp_rec if old style
**	    compression.   
**	31-jan-1994 (jnash)
**	    Prefill output buffer with zeros to eliminate rollforwarddb 
**	    dmve_replace() "tuple_mismatch" warnings after modify
**	    operations.
**	23-may-1994 (andys)
**	    Add cast(s) to quiet qac/lint
**      17-jul-1996 (ramra01 for bryanp)
**          Alter Table support: Add new row_version argument to
**          dm1cn_uncompress, and use it to convert older versions of rows
**          to their current versions while uncompressing them.
**	25-Feb-1998 (shero03)
**	    Account for a null coupon.
**	03-Oct-1999 (wanfr01)
** 	    Partial cross-integration of change 435281 - performance improvement
**	    to dm1cn_compress and dm1cn_umcompress
**	18-Aug-2000 (thaju02)
**	    If DB_CHA_TYPE, DB_TXT_TYPE, DB_VCH_TYPE column has been dropped,
**	    increment src pointer by DB_CNTSIZE to account for two-byte 
**	    length identifier. (b101362)
**	28-Aug-2000 (thaju02)
**	    If nullable column has been dropped, increment src pointer 
**	    to account for null indicator. Also modified code to account
**	    for UDT dropped column. (b102446)
**	06-Apr-1998 (hanch04/nanpr01)
**	    Performace improvements.  Added case for common types.
**	18-May-1998 (jenjo02)
**	    Fix code broken by 06-Apr change. 
**	    Selectively zero-fill otherwise unfilled destination fields
**	    instead of zero-filling the entire destination beforehand.
**	12-aug-99 (stephenb)
**	    Alter parameters, buffer is now char** since it may be 
**	    freed by called function.
**       2-Sep-2004 (hanal04) Bug 111474 INGSRV2638 
**          Added code to insert default value for not null with default
**          added columns. 
**	09-Feb-2005 (gupsh01)
**	    Added support for nchar/nvarchar.
**	21-Apr-2005 (thaju02)
**	    Increment src to account for dropped nchar/nvarchar cols. 
**	    (B114367)
**	21-Jun-2005 (gupsh01)
**	    Fix processing for nchar case. NCHAR lengths are number of
**	    unicode characters.
**	03-aug-2006 (gupsh01)
**	    Added support for new ANSI date/time types.
**	29-Sep-2006 (wanfr01)
**	    Bug 116727
**	    cv_att and save_dst need to be nulled for each att's
**	    processing to avoid misuse of previous attribute info.
**	    also restore saved dst prior to processing next row.
*/
DB_STATUS	
dm1cn_uncompress(
			DB_ATTS     **atts_array,
			i4     atts_count,
			register char	    *src,
			register char	    *dst,
			i4	    dst_maxlen,
			i4     *dst_size,
			i4	    record_type,
			DM_TID	    *tid,
			char        **buffer,
			i4	    row_version,	
			PTR	    rcb_adf_cb)
{
    register DB_ATTS	*att;
    register i2		type;
    register i2		abs_type;
    u_i2	chr_ln;
    u_i2	att_ln;
    i4	i;
    i4	width;
    i4	error;
    i4		dt_bits;
    i4		adi_dtinfo_called;
    i4	compr_len;
    ADF_CB	*adf_scb = (ADF_CB *)rcb_adf_cb;
    char	*tempbuf = (char *)0;
    char	*save_dst;
    DB_ATTS	*att_from = (DB_ATTS *)0;
    DB_ATTS	*cv_att = (DB_ATTS *)0;
    i4  		err_code;
    DB_DATA_VALUE	fromvalue;
    DB_DATA_VALUE	tovalue;
    STATUS		stat;

    if (record_type == TCB_C_OLD)
    {
    	return(dm1cn_old_uncomp_rec(atts_array, atts_count, src, 
	    dst, dst_maxlen, dst_size));
    }

    width = 0;

    for (i = 0; i < atts_count; i++)
    {
	att = atts_array[i];

	att_ln = att->length;
	type = att->type;
	abs_type = (type < 0) ? -type: type;

	if ( (width + att->length > dst_maxlen) &&
	    (!att->ver_dropped) )
	    return (E_DB_ERROR);		/* buffer overflow */

	if ( (att->ver_dropped) ||
	    (att->ver_dropped > att->ver_added) )
	{
	    if (att->ver_dropped > att->ver_added)
	    {
	    if (att->ver_dropped > row_version) 
	    {
		if (att->ver_altcol)
		{
		    if ( (row_version >= att->ver_added) &&
			(att->ver_dropped == att->ver_altcol) )
		    {
			att_from = att;
		    }
		    continue;
		}

		if (att_from)
		{
		    /* Skip correct version of att */
		    att = att_from;
		    att_from = (DB_ATTS *)0;
		    att_ln = att->length;
		    type = att->type;
		    abs_type = (type < 0) ? -type: type;
		}

		if (type < 0)
		{
		    if (*src++ & ADF_NVL_BIT)
		    {
			/* Source is NULL, check for AD_PERIPHERAL */
			adi_dtinfo(adf_scb, abs_type, &dt_bits);

			adi_dtinfo_called = TRUE;

			if ((dt_bits & AD_PERIPHERAL) == 0)
			    continue;

			/* A null coupon is passed through */
		    }
		    att_ln--;
		}

		if (abs_type == DB_CHR_TYPE)
		{
		    chr_ln = STlength((char *)src);
		    src += chr_ln + 1;
		    att_from = (DB_ATTS *)0;
		    continue;
		}

		if ((abs_type == DB_CHA_TYPE) || (abs_type == DB_TXT_TYPE) 
		    || (abs_type == DB_VCH_TYPE)
		    || (abs_type == DB_NCHR_TYPE))
		{
		    I2ASSIGN_MACRO(*(u_i2 *)src, chr_ln);
		    src += DB_CNTSIZE;
		    if (chr_ln)
		    {	
		      if (abs_type == DB_NCHR_TYPE)
		        chr_ln = chr_ln * sizeof(UCS2);

		      src += chr_ln;
		    }
		    att_from = (DB_ATTS *)0;
		    continue;
		}
		else if (abs_type == DB_NVCHR_TYPE)
		{
		    I2ASSIGN_MACRO(*(u_i2 *)src, chr_ln);
		    chr_ln = chr_ln * sizeof(UCS2) + DB_CNTSIZE;
		    if (chr_ln)
			src += chr_ln;
		    att_from = (DB_ATTS *)0;
		    continue;
		}

		if (type >= ADI_DT_CLSOBJ_MIN )
		{
		/* If UDT see if it is a VL UDT */
		    if (adi_dtinfo_called == FALSE)
		    {
			adi_dtinfo(adf_scb, abs_type, &dt_bits);
		    }
		    if (dt_bits & AD_VARIABLE_LEN)
		    {
		    /* Call the UDTs compress routine */
			DB_DATA_VALUE tmp_dv;

			tmp_dv.db_datatype = type;
			tmp_dv.db_data = (char *)src;
			tmp_dv.db_length = att_ln;

			adc_compr(adf_scb, &tmp_dv, FALSE, dst, &compr_len);
			att_ln = (u_i2)compr_len;
		    }
		}

		src += att_ln;
		att_from = (DB_ATTS *)0;
	    }
	    }
	    continue;
	}

	if ( !att_from && (att->ver_added > row_version) &&
	   (att->ver_added > att->ver_dropped) )
	{
              /*
              ** In this case, the select returns a column for which no
              ** data exists.  Set the null flag to indicate that this is
              ** null.  If variable-length datatype, put zeros in
              ** the first two bytes to indicate zero length.
              */
              if (type > 0)
              {
                /* If added column is "not null with default", project 0 or
                ** blank into row buffer, depending on data type. */
                switch (type)
                {
                    case DB_VCH_TYPE:
                    case DB_TXT_TYPE:
                    case DB_LTXT_TYPE:
                    case DB_VBYTE_TYPE:
                    case DB_VBIT_TYPE:
                    {
                        MEfill( sizeof(u_i2), (u_char) '\0', (PTR) dst);
                        break;
                    }
                    case DB_INT_TYPE:
                    case DB_BIT_TYPE:
                    case DB_BYTE_TYPE:
                    case DB_LOGKEY_TYPE:
                    case DB_TABKEY_TYPE:
                    {
                        MEfill( att->length, (u_char) 0, (PTR) dst);
                        break;
                    }
                    case DB_FLT_TYPE:
                    case DB_MNY_TYPE:
                    {
                        f8      f8val = 0.0;
                        MEcopy((PTR)&f8val, att->length, (PTR) dst);
                        break;
                    }
                    case DB_CHA_TYPE:
                    case DB_CHR_TYPE:
                    {
                        MEfill( att->length, (u_char)' ', (PTR) dst);
                        break;
                    }
                    case DB_DTE_TYPE:
        	    case DB_ADTE_TYPE:
        	    case DB_TMWO_TYPE:
        	    case DB_TMW_TYPE:
        	    case DB_TME_TYPE:
        	    case DB_TSWO_TYPE:
        	    case DB_TSW_TYPE:
        	    case DB_TSTMP_TYPE:
        	    case DB_INDS_TYPE:
        	    case DB_INYM_TYPE:
                    {
                        /* Note: this depends on the fact that the internal
                        ** representation of a "blank" date is 12 bytes of 0's.
                        ** If this changes, adudate.h from common!hdr must be
                        ** included in this module and the AD_DATENTRNL
                        ** structure will be needed for the initialization. */
                        MEfill( att->length, (u_char) 0, (PTR) dst);
                        break;
                    }
                    case DB_DEC_TYPE:
                    {
                        MEfill( att->length, (u_char) 0, (PTR) dst);
                        dst[att->length-1] = MH_PK_PLUS;
                                                /* add packed decimal plus sign
*/
                        break;
                    }
                }
 
              }         /* end of not null with default */
              else
              {
                /* If nullable, set null indicator and (if var text) init
                ** column length to 0. */
                dst[(att->length - 1)] = ADF_NVL_BIT;
                switch(type)
                {
                    case -DB_VCH_TYPE:
                    case -DB_TXT_TYPE:
                    case -DB_LTXT_TYPE:
                    case -DB_VBYTE_TYPE:
                    {
                        MEfill( sizeof(u_i2), '\0', (PTR) dst);
                        break;
                    }
                    case -DB_LVCH_TYPE:
                    case -DB_LBYTE_TYPE:
                    {
                        MEfill( (att->length - 1), '\0', (PTR) dst);
                        break;
                    }
                    default:
                    {
                          break;
                    }
                }
              }         /* end of nullable */
 
              dst += att->length;
              width += att->length;
              continue;
	}

	if (att_from)
	{
	    cv_att = att;
	    att = att_from;
	    att_from = (DB_ATTS *)0;
	    att_ln = att->length;
	    type = att->type;
	    abs_type = (type < 0) ? -type: type;
	    tempbuf = (PTR)MEreqmem (0, att->length, TRUE, NULL);
	    save_dst = dst;
	    dst = tempbuf;
	}
	else
	{
	    cv_att = (DB_ATTS *)0;
	    save_dst = NULL;
	}

	adi_dtinfo_called = FALSE;

	if (type < 0)
	{
	    /*
	    ** place the null byte in the trailing byte field,
	    ** check if the NULL value is set; if it is move
	    ** the destination and the width calculation up
	    ** and go on to the next attribute, if it is not
	    ** set then unwind the data as in non-null cases. 
	    ** A null coupon is passed through.
	    */

	    dst[--att_ln] = *src;
	    if (save_dst)
	        save_dst[att_ln] = *src;
	    if (*src++ & ADF_NVL_BIT)
	    {
		/* Source is NULL, check for AD_PERIPHERAL */
		adi_dtinfo(adf_scb, abs_type, &dt_bits);

		adi_dtinfo_called = TRUE;

		if ((dt_bits & AD_PERIPHERAL) == 0)
		{
		    if (save_dst)
			dst = save_dst;
		    MEfill(att_ln, '\0', (char *)dst);
		    dst += att->length;
		    width += att->length;
		    continue;
		}
	    }
	}

	switch(abs_type) {

	case DB_CHR_TYPE:
	    /*
	    ** copy the source into the destination and
	    ** pad with blanks, then move the source past
	    ** this attribute to the next one.
	    */
	    chr_ln = STlength((char *)src);
	    MEmove(chr_ln, (PTR)src, ' ', att_ln, (PTR)dst);
	    src += chr_ln + 1;
	    break;

	case DB_CHA_TYPE:
	    /*
	    ** get the length of the CHAR field, move source
	    ** past the length id, copy the CHAR data into
	    ** the destination, move the source past the used
	    ** characters, blank pad the CHAR field.
	    */
	    I2ASSIGN_MACRO(*(u_i2 *)src, chr_ln);

	    /* Check that length descriptor is valid */
	    if (chr_ln <= att_ln)
	    {
		src += DB_CNTSIZE;
		MEmove(chr_ln, (PTR)src, ' ', att_ln, (PTR)dst);
		src += chr_ln;
		break;
	    }
	    else
	    {
		/*
		** Row has a bad length descriptor - it cannot be uncompressed.
		*/
		ule_format(E_DM9388_BAD_DATA_LENGTH, (CL_SYS_ERR *)0, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0, &error, 1,
		    0, (i4)chr_ln);
		ule_format(E_DM9389_BAD_DATA_VALUE, (CL_SYS_ERR *)0, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0, &error, 0);
		if (tempbuf)
		    MEfree (tempbuf);
		return (E_DB_ERROR);
	    }

	case DB_NCHR_TYPE:
            /*
            ** get the length of the NCHAR field, move source
            ** past the length id, copy the NCHAR data into
            ** the destination, move the source past the used
            ** characters, blank pad the NCHAR field.
            */
            I2ASSIGN_MACRO(*(u_i2 *)src, chr_ln);
	    chr_ln *= sizeof(UCS2);

            /* Check that length descriptor is valid */
            if (chr_ln <= att_ln)
            {
               u_i2	*psave;
               u_i2	*uend;

               src += DB_CNTSIZE;
               MEcopy((PTR)src, chr_ln, (PTR)dst);
               psave = (u_i2 *)dst + chr_ln/sizeof(UCS2);
               uend = (u_i2 *)dst + att_ln/sizeof(UCS2);

               for (; psave <= uend; psave++)
                   *psave = U_BLANK;

               src += chr_ln;
               break;
            }
            else
            {
              /*
              ** Row has a bad length descriptor - it cannot be uncompressed.
              */
              ule_format(E_DM9388_BAD_DATA_LENGTH, (CL_SYS_ERR *)0, ULE_LOG,
                   NULL, (char *)0, (i4)0, (i4 *)0, &error, 1,
                   0, (i4)chr_ln);
              ule_format(E_DM9389_BAD_DATA_VALUE, (CL_SYS_ERR *)0, ULE_LOG,
                  NULL, (char *)0, (i4)0, (i4 *)0, &error, 0);
	      if (tempbuf)
		  MEfree (tempbuf);
              return (E_DB_ERROR);
            }

        case DB_TXT_TYPE:
	case DB_VCH_TYPE:
	    /*
	    ** copy the source into the destination however
	    ** many bytes are specified by the two-byte length
	    ** identifier, move the source to the next field.
	    */
	    I2ASSIGN_MACRO(((DB_TEXT_STRING *)src)->db_t_count, chr_ln);
	    chr_ln += DB_CNTSIZE;

	    /* Check that length descriptor is valid */
	    if (chr_ln <= att_ln)
	    {
                MEcopy((char *)src, chr_ln, (char *)dst);
                src += chr_ln;
		break;
	    }
	    else
	    {
		/*
		** Row has a bad length descriptor - it cannot be uncompressed.
		*/
		ule_format(E_DM9388_BAD_DATA_LENGTH, (CL_SYS_ERR *)0, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0, &error, 1,
		    0, (i4)chr_ln - DB_CNTSIZE);
		ule_format(E_DM9389_BAD_DATA_VALUE, (CL_SYS_ERR *)0, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0, &error, 0);
		if (tempbuf)
		    MEfree (tempbuf);
		return (E_DB_ERROR);
	    }
	    break;

 	case DB_NVCHR_TYPE:
        {
	    u_i2	nvchr_len;
	    char	*nvptr;

	    I2ASSIGN_MACRO(((DB_TEXT_STRING *)src)->db_t_count, nvchr_len);
            chr_ln = nvchr_len * sizeof(UCS2) + DB_CNTSIZE;

            /* Check that length descriptor is valid */
            if (chr_ln <= att_ln)
            {
                MEcopy((char *)src, chr_ln, (char *)dst);
                src += chr_ln;
                break;
            }
            else
            {
                /*
                ** Row has a bad length descriptor - it cannot be uncompressed.
                */
                ule_format(E_DM9388_BAD_DATA_LENGTH, (CL_SYS_ERR *)0, ULE_LOG,
                    NULL, (char *)0, (i4)0, (i4 *)0, &error, 1,
                    0, (i4)chr_ln - DB_CNTSIZE);
                ule_format(E_DM9389_BAD_DATA_VALUE, (CL_SYS_ERR *)0, ULE_LOG,
                    NULL, (char *)0, (i4)0, (i4 *)0, &error, 0);
		if (tempbuf)
		    MEfree (tempbuf);
                return (E_DB_ERROR);
            }
        }
        break;

	case DB_INT_TYPE:
	case DB_FLT_TYPE:
	case DB_DTE_TYPE:
        case DB_ADTE_TYPE:
        case DB_TMWO_TYPE:
        case DB_TMW_TYPE:
        case DB_TME_TYPE:
        case DB_TSWO_TYPE:
        case DB_TSW_TYPE:
        case DB_TSTMP_TYPE:
        case DB_INDS_TYPE:
        case DB_INYM_TYPE:
	case DB_MNY_TYPE:
	case DB_DEC_TYPE:
	case DB_LOGKEY_TYPE:
	case DB_TABKEY_TYPE:
            MEcopy((char *)src, att_ln, (char *)dst);
	    src += att_ln;
	    break;
	
	default:
            if (abs_type >= ADI_DT_CLSOBJ_MIN )
	    {
		/* If UDT see if it is a VL UDT */
		if (adi_dtinfo_called == FALSE)
		{
		    adi_dtinfo(adf_scb, abs_type, &dt_bits);
		}
	        if (dt_bits & AD_VARIABLE_LEN)
	        {
		    /* Call the UDTs compress routine */
		    DB_DATA_VALUE tmp_dv;

		    tmp_dv.db_datatype = abs_type;
		    tmp_dv.db_data = (char *)src;
		    tmp_dv.db_length = att_ln;

		    adc_compr(adf_scb, &tmp_dv, FALSE, dst, &compr_len);
		    src += (u_i2)compr_len;
		    break;
		 }
	    }
	    
	    MEcopy((char *)src, att_ln, (char *)dst);
	    src += att_ln;
	    break;
	}

	if (cv_att)
	{
	    dst = save_dst;

	    fromvalue.db_datatype = att->type;
	    fromvalue.db_length = att->length;
	    fromvalue.db_prec = att->precision;
	    fromvalue.db_data = tempbuf;
	    fromvalue.db_collID = att->collID;

	    tovalue.db_datatype = cv_att->type;
	    tovalue.db_length = cv_att->length;
	    tovalue.db_prec = cv_att->precision;
	    tovalue.db_data = (PTR)MEreqmem (0, tovalue.db_length, TRUE, NULL);
	    tovalue.db_collID = cv_att->collID;

	    stat = adc_cvinto (adf_scb, &fromvalue, &tovalue);
	    if (stat != E_DB_OK)
	    {
	       err_code = E_DM019D_ROW_CONVERSION_FAILED;
	       ule_format(err_code, (CL_SYS_ERR *)0, 
		    ULE_LOG, NULL, (char *)0,
		    (i4)0, (i4 *)0, &err_code, 0);
	       MEfree (tovalue.db_data);
	       MEfree (tempbuf);
	       tempbuf = (char *)0;
	       return (err_code);
	    }

	    MEcopy ( (char *)tovalue.db_data, tovalue.db_length, dst);
	    MEfree (tovalue.db_data);
	    MEfree (tempbuf);
	    tempbuf = (char *)0;

	    att = cv_att;
	    cv_att = (DB_ATTS *)0;
	}

    	dst += att->length;
	width += att->length;
    }
    /* Now fill that part of the destination buffer that wasn't used */
    if (dst_maxlen - width)
	MEfill(dst_maxlen - width, '\0', (char *)dst);

    *dst_size = width;
    return (E_DB_OK);
}

/*{
** Name: dmpp_compexpand -- how much can a tuple expand when compressed?
**
** Description:
**	Though the obvious goal of compression is to shrink actual tuple
**	size, in all our compression algorithms there is potential growth
**	in the worst case.  Given the list of attributes for a relation,
**	this accessor returns the amount by which a tuple may expand in the
**	worst case.  If the uncompressed tuple width plus this worst-case
**	compression expansion is larger than the maximum tuple width,
**	the table cannot be modified to a compressed format.
**
** Inputs:
**	atts	- Pointer to DB_ATTS pointers array 
**
**	    C		- copied up to the last non-blank character and then
**			a NULL byte ('\0') is written.
**	    CHAR	- number of bytes to the last non-blank character is
**			calculated.  Two-byte length desciptor is written
**			followed by the string up to this length.
**	    VARCHAR, TEXT - Two byte length descriptor is written giving the
**			length indicated in the VARCHAR or TEXT datatype,
**			followed by this many bytes from the data portion of
**			the datatype.
**	    INT, FLOAT, DATE, other - copied as is: no compression.
**
** Inputs:
**      atts_array                      Pointer to an array of attribute 
**                                      descriptors.
**      atts_count                      Number of entries in atts_array.
**      
**      rec                             Pointer to record to compress.
**      rec_size                        Uncompressed record size. 
**      crec                            Pointer to an area to return compressed
**                                      record.
**
** Outputs:
**      crec_size		        Pointer to an area where compressed
**                                      record size can be returned.
**	Returns:
**
**	    E_DB_OK
**	    E_DB_ERROR			Tuple could not be compressed.
**
**	Exceptions:
**	    none
**
** Side Effects:
**          none.
**
** History:
**      08-nov-85 (jennifer)
**          Converted for Jupiter.
**	28-sep-88 (sandyh)
**	    Added support for CHAR, VARCHAR, and nullable datatypes.
**	08-mar-89 (arana)
**	  Changed MEcopy of u_i2 to I2ASSIGN_MACRO.
**	29-may-89 (rogerk)
**	    Added checks for bad data values so we don't write bad data
**	    to the database that cannot be uncompressed.
**	    Changed from being VOID routine to being DB_STATUS.
**	    Took out IFDEF's for byte alignment.
**	10-jul-89 (jas)
**	    Changed to multiple routines for multiple page format support.
**	03-aug-89 (kimman)
**	    Used MECOPY_VAR_MACRO instead of MEcopy.
*/
/*
** "Traditional" page format implementation:
**
** "c" format attributes can grow by 1 byte (the terminating null when the
** attribute is of maximum length); "char" format attributes can grow by
** 2 bytes (the 2-byte length when the attribute is of maximum length).
*/
i4	
dm1cn_compexpand(
			DB_ATTS		**atts,
			i4		att_cnt)
{
    register i4  expansion = 0;
    register i4  abs_type, i;
    register DB_ATTS	*att;

    for (i=0; i<att_cnt; i++)
    {
	att = atts[i];

	if (att->ver_dropped)
	   continue;

	abs_type = (att->type < 0) ? -att->type : att->type;
	if (abs_type == DB_CHR_TYPE)
	    ++expansion;
	else if (abs_type == DB_CHA_TYPE)
	    expansion += DB_CNTSIZE;
    }
    return expansion;
}

/*{
** Name: dm1cn_old_uncomp_rec - Uncompresses a record in old compression format.
**
** Description:
**    This routine uncompresses a record and returns the record
**    and size in bytes.
**
** Inputs:
**      atts_array                      Pointer to an array of attribute 
**                                      descriptors.
**      atts_count                      Value indicating number of attributes
**                                      in attribute array.
**      src                             Pointer to an area containing record
**                                      to uncompress.
**	dst_maxlen			Size of the destination area. If, during
**					the uncompress operation, we detect
**					that we are exceeding the destination
**					area size, we abort the uncompress
**					operation and return an error.
**
** Outputs:
**      dst                             Pointer to an area to return
**                                      uncompressed record.
**      dst_size                        Size of uncompressed record.
**      
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR			Bad tuple encountered.
**
**	Exceptions:
**	    none
**
** Side Effects:
**          none.
**
** History:
**      08-nov-85 (jennifer)
**          Converted for Jupiter.
**	29-may-89 (rogerk)
**	    Changed from VOID to DB_STATUS.
**	    Added checks for invalid tuple lengths in compressed tuples
**	    to prevent us from uncompressing tuple all over memory.
**	    Took out IFDEF's for byte alignment.
**	7-mar-1991 (bryanp)
**	    Made this routine static.
**	    Added dst_maxlen argument and implemented a new sanity
**	    check to detect buffer overflow.
**	13-oct-1992 (jnash)
**	    Make chr_ln i2 to quiet compiler.
**	18-May-1998 (jenjo02)
**	    Repaired for loop (att++) broken by 06-Apr performance changes.
*/
static DB_STATUS    
dm1cn_old_uncomp_rec(
			DB_ATTS     **atts_array,
			i4     atts_count,
			char	    *src,
			char	    *dst,
			i4	    dst_maxlen,
			i4     *dst_size)
{
    DB_ATTS	*att;
    i4	i;
    i4	width;
    i4	error;
    i2		chr_ln;

    /* For each domain, copy and blank pad if char. */

    width = 0;

    for (i = 0; i < atts_count; i++)
    {
	att = atts_array[i];

	if (width + att->length > dst_maxlen)
	    return (E_DB_ERROR);		/* buffer overflow */

        if (att->type == DB_CHR_TYPE)
	{
            STmove(src, ' ', att->length, dst);
	    src += STlength(src) + 1;
	}
        else if (att->type == DB_TXT_TYPE)
        {
	    I2ASSIGN_MACRO(((DB_TEXT_STRING *)src)->db_t_count, chr_ln);
	    chr_ln += DB_CNTSIZE;

	    /* Check that length descriptor is valid */
	    if (chr_ln <= (att->length))
	    {
		MEcopy(src, chr_ln, dst);
		src += chr_ln;
	    }
	    else
	    {
		/*
		** Row has a bad length descriptor - it cannot be uncompressed.
		*/
		ule_format(E_DM9388_BAD_DATA_LENGTH, (CL_SYS_ERR *)0, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0, &error, 1,
		    0, (i4)chr_ln - DB_CNTSIZE);
		ule_format(E_DM9389_BAD_DATA_VALUE, (CL_SYS_ERR *)0, ULE_LOG,
		    NULL, (char *)0, (i4)0, (i4 *)0, &error, 0);
		return (E_DB_ERROR);
	    }
        }
        else
	{
            MEcopy(src, att->length, dst);
	    src += att->length;
        }
	
	dst += att->length;
	width = width + att->length;
        att++;
    }

    *dst_size =width;
    return (E_DB_OK);
}
