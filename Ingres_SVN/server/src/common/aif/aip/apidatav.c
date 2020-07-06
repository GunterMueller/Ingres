/*
** Copyright (c) 2004, 2007 Ingres Corporation
*/

# include <compat.h>
# include <gl.h>
# include <me.h>
# include <iicommon.h>
# include <adf.h>
# include <adp.h>

# include <iiapi.h>
# include <api.h>
# include <apidatav.h>
# include <apienhnd.h>
# include <apimisc.h>
# include <apitrace.h>

/*
** Name: apidatav.c
**
** Description:
**	This file contains functions which converts data value and
**	descriptor between GCA and API.  The API descriptor is
**	represented by IIAPI_DESCRIPTOR and the API data value is
**	represented by IIAPI_DATAVALUE.  The GCA descriptor and data
**	value are represented by GCD_DATA_VALUE/GCD_GCD_DB_DATA.  
**	The descriptor and data parameters of API and GCA may have
**	different semantic, the differences are listed below.
**
**	parameter	API			GCA
**					            
**	data type	data type of data	data type +
**						nullable flag
**					            
**	length		[variable length] +	[variable length +]
**			length of data		length of data +
**						[null byte]
**					            
**	null flag	II_BOOLean in		included in the data buffer
**			IIAPI_DATAVALUE
**					            
**	nullable flag	II_BOOLean in		included in data type
**                      IIAPI_DESCRIPTOR
**    					            
**	data		[variable length] +	[variable length] +
**			data value		data value +
**						[null flag]
**
**	Note that the length for variable length data in IIAPI_DESCRIPTOR
**	includes any trailing padding, while the length in IIAPI_DATAVALUE
**	is just the actual text length (+ 2 byte length).
**
**	IIapi_getAPILength		Get length of GCA data in API terms.
**	IIapi_getGCALength		Get length of GCA data in GCA terms.
**	IIapi_getBLOBLength		Get length of BLOB in GCA terms.
**	IIapi_cnvtDescr2GDV		copy API descriptor to GCA data value.
**	IIapi_cnvtDataValue2GDV		copy API data value to GCA data value.
**	IIapi_cnvtDataValue2GCATuple	copy API data value to GCA tuple.
**	IIapi_cnvtBLOB2GCATuple		copy BLOB to GCA tuple.
**	IIapi_cnvtGDV2Descr		copy GCA data value to API descriptor.
**	IIapi_cnvtGDV2DataValue		copy GCA data value to API data value.
**	IIapi_loadColumns		copy GCA tuple to API data values.
**	IIapi_checkBLOBSegment		check for end-of-segments indicator.
**
** History:
**      04-may-94 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 7-Oct-94 (gordy)
**	    Fixed handling of variable length types.
**	11-Nov-94 (gordy)
**	    Return internal length in buffer with variable length data.
**	 9-Dec-94 (gordy)
**	    Don't clear NULL byte if non-nullable.
**	 9-Mar-95 (gordy)
**	    Silence compiler warnings.
**	10-Mar-95 (gordy)
**	    Make sure dv_null is FALSE if not null.
**	19-May-95 (gordy)
**	    Fixed include statements.
**	22-May-95 (gordy)
**	    Fixed problem with variable length types.
**	13-Jun-95 (gordy)
**	    Initial support for BLOBs.
**	20-Jun-95 (gordy)
**	    Additional support for sending BLOBs as parameters.
**	11-Jul-95 (gordy)
**	    Conditionalize BLOB support for 6.4 platforms.
**	13-Jul-95 (gordy)
**	    Add missing header files, casts for type correctness.
**	28-Jul-95 (gordy)
**	    Use fixed length types.  Fixed tracing.
**	14-Sep-95 (gordy)
**	    GCD_GCA_ prefix changed to GCD_.
**	17-Jan-96 (gordy)
**	    Added environment handles.
**	16-Feb-96 (gordy)
**	    Removed IIAPI_SDDATA structure and associated functions.
**	15-Mar-96 (gordy)
**	    Added support for compressed varchars with IIapi_getCompLength()
**	    and IIapi_cnvtComp2DataValue().  Two byte data lengths should
**	    be unsigned.
**	27-Mar-96 (gordy)
**	    Made compressed varchar routines local and renamed.
**	 3-Apr-96 (gordy)
**	    BLOB segments may now be sent using the formatted GCA interface.
**	22-Apr-96 (gordy)
**	    Finished support for COPY FROM.
**	20-May-96 (gordy)
**	    Changes in GCA buffering result in the BLOB end-of-segments
**	    indicator being available after reading a data segment, so
**	    check for end of blob after satisfying a segment request.
**	18-Sep-96 (gordy)
**	    Pad/fill data with appropriate values.
**	 2-Oct-96 (gordy)
**	    Added new tracing levels.
**	18-Mar-97 (gordy)
**	    BLOB processing now fills the applications data value buffer
**	    rather than just returning what segment portion may be available.
**	    This will provide more consistent response to the application
**	    and fewer calls to IIapi_getColumns().
**      21-Aug-98 (rajus01)
**          Added environment parameter to IIapi_cnvtGDVDescr().
**	    BLOB segment length can be set through IIapi_setEnvParam().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-Nov-1999 (hanch04)
**          First stage of updating ME calls to use OS memory management.
**          Make sure me.h is included.  Use SIZE_TYPE for lengths.
**	 1-Mar-01 (gordy)
**	    Added support for Unicode datatypes: IIAPI_NCHA_TYPE,
**	    IIAPI_NVCH_TYPE, IIAPI_LNVCH_TYPE.
**	 3-Aug-01 (thoda04)
**	    Added more trace info for IIapi_loadColumns.
**	12-Jul-02 (gordy)
**	    Fixed handling of long column values in cnvtComp2DataValue.
**	    Added cnvtGDV2DataValue to handle long fixed column values.
**	31-May-06 (gordy)
**	    New ANSI date/type types utilize precision.
**	14-Jul-2006 (kschendel)
**	    Remove u_i2 casts from MEcopy calls.
**	25-Oct-06 (gordy)
**	    Added support for Locators.  General cleanup to reduce duplication.
**	15-Mar-07 (gordy)
**	    Support scrollable cursors.
*/


/*
** Local functions.
*/
static	II_VOID	load_columns( IIAPI_STMTHNDL *, 
			      IIAPI_GETCOLPARM *, IIAPI_GCA_BUFF * );

static	II_BOOL	load_blob( IIAPI_STMTHNDL *, IIAPI_GETCOLPARM *,
			   IIAPI_DESCRIPTOR *, IIAPI_GCA_BUFF *,
			   IIAPI_DATAVALUE * );

static	II_BOOL	cnvtGDV2DataValue( IIAPI_DESCRIPTOR *, IIAPI_GCA_BUFF *, 
				   IIAPI_DATAVALUE *, II_BOOL );

static	II_BOOL	cnvtBLOB2DataValue( IIAPI_STMTHNDL *, IIAPI_DESCRIPTOR *,
				    IIAPI_GCA_BUFF *, IIAPI_DATAVALUE * );

static	II_BOOL	cnvtBLOB2GCATuple( IIAPI_DESCRIPTOR *, IIAPI_DATAVALUE *,
				   u_i2 *, II_BOOL, II_BOOL, char *, II_LONG *);

static	u_i2	formatBLOBhdr( i4, u_i1 * );

static	u_i2	formatBLOBseg( u_i2, u_i2, u_i1 *, u_i1 * );

static	u_i2	formatBLOBend( u_i1 * );


/*
** The following macros take GCA values for type
** and length, and expect the data to point to a
** buffer which can hold the GCA form of the data.
**
** Can be used on LOB and compressed variable
** length data if length is specific for value.
*/
#define IIAPI_ISNULL(type,length,data)  \
	((type) > 0 ? FALSE  \
		  : (*(((II_CHAR *)(data)) + (length) - 1) & ADF_NVL_BIT  \
			? TRUE : FALSE) )

#define IIAPI_SETNULL(length,data)  \
	(*(((II_CHAR *)(data)) + (length) - 1) = (II_CHAR)ADF_NVL_BIT)

#define IIAPI_SETNOTNULL(length,data)  \
	(*(((II_CHAR *)(data)) + (length) - 1) = 0)

/*
** The following macros take the API values for type
** and length (as stored in the descriptor), and expect
** the data to point to a buffer which can hold the
** GCA form of the data.
*/
#define IIAPI_ISNULLDESC(descriptor,data)  \
	( (descriptor)->ds_nullable  \
	    ? (*((II_CHAR *)(data) + IIapi_getGCALength(descriptor) - 1)  \
								& ADF_NVL_BIT \
		? TRUE : FALSE)  \
	    : FALSE )

#define IIAPI_SETNULLDESC(descriptor,data)  \
	(*((II_CHAR *)(data) + IIapi_getGCALength(descriptor) - 1)  \
							= (II_CHAR)ADF_NVL_BIT)

#define IIAPI_SETNOTNULLDESC(descriptor,data)  \
	(*((II_CHAR *)(data) + IIapi_getGCALength(descriptor) - 1) = 0) 



/*
** Name: IIapi_getAPILength
**
** Description:
**	This function returns the length of a data value as 
**	represented within the API depending on its GCA data 
**	type.  
**
**	For variable length types, the length of the actual 
**	data will be returned if a data value is provided.
**	Otherwise, the maximum length is returned.
**
**	For LOB types, a data value is expected to be a data
**	segment (non-NULL LOB, not including the data segment
**	indicator).  Otherwise, the maximum segment length is 
**	returned.
**
**	Compressed variable length data values are not supported.
**
** Input:
**	envHndl		Environment handle (may be NULL).
**	dataType	Data type of the value.
**	length		Maximum length of the data value.
**	buffer		Data value (may be NULL).
**
** Output:
**	None.
**
** Returns:
**	II_INT2		Length of value in API format.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      03-may-94 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 7-Oct-94 (gordy)
**	    Fixed handling of variable length types.  Renamed.
**	11-Nov-94 (gordy)
**	    Return internal length in buffer with variable length data.
**	22-May-95 (gordy)
**	    Don't deref ptr as arg to IIAPI_ISNULL macro.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.
**	25-Oct-06 (gordy) Support LOBs and Locators.  Added envHndl param.
*/

II_EXTERN II_UINT2
IIapi_getAPILength 
(
    IIAPI_ENVHNDL	*envHndl,
    IIAPI_DT_ID		dataType,
    II_LONG		dataLength,
    char		*buffer
)
{
    II_LONG length;

    switch( abs( dataType ) )
    {
    case IIAPI_LVCH_TYPE :
    case IIAPI_LBYTE_TYPE :
    case IIAPI_LNVCH_TYPE :
	if ( buffer != NULL )
	{
	    II_UINT2	seg_len;

	    MECOPY_CONST_MACRO( buffer, sizeof( seg_len ), (PTR)&seg_len );
	    length = sizeof( seg_len ) + 
		     (IIapi_isUCS2( dataType ) ? seg_len * 2 : seg_len);
	}
	else  if ( IIapi_isEnvHndl( envHndl )  &&
	           (envHndl->en_usrParm.up_mask1 & IIAPI_UP_MAX_SEGMENT_LEN) )
	    length = envHndl->en_usrParm.up_max_seglen;
	else
	    length = IIAPI_MAX_SEGMENT_LEN;
	break;

    case IIAPI_LCLOC_TYPE :
    case IIAPI_LBLOC_TYPE :
    case IIAPI_LNLOC_TYPE :
	length = sizeof( ADP_LOCATOR );
	break;

    default :
    {
	II_UINT2 var_len;

	if ( buffer == NULL  ||  ! IIapi_isVAR( dataType )  ||
	     IIAPI_ISNULL( dataType, dataLength, buffer ) )
	    length = (dataType < 0) ? dataLength - 1 : dataLength;
	else
	{
	    MECOPY_CONST_MACRO( buffer, sizeof( var_len ), (PTR)&var_len );
	    length = sizeof( var_len ) + 
		     (IIapi_isUCS2( dataType ) ? var_len * 2 : var_len);
	}
	break;
    }
    }

    return( (II_UINT2)length );
}



/*
** Name: IIapi_getGCALength
**
** Description:
**	This function returns the length of a data value as 
**	represented within GCA (including null indicator and
**	variable data length) depending on its API data type.  
**
**	For LOBs, only the fixed sized portion of the variable
**	segmented format is represented.
**
**	For variable length types which may be compressed, the
**	length returned is the maximum length.
**
** Input:
**	descriptor	API data descriptor.
**
** Output:
**	None.
**
** Return value:
**	II_UINT2	Length of the value in GCA format.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**	 7-Oct-94 (gordy)
**	    Created.
**	11-Nov-94 (gordy)
**	    Return internal length in buffer with variable length data.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.
**	25-Oct-06 (gordy)
**	    Support LOBs (fixed portion) and Locators.
*/

II_EXTERN II_UINT2
IIapi_getGCALength( IIAPI_DESCRIPTOR *descriptor )
{
    II_UINT2	length;

    switch( descriptor->ds_dataType )
    {
    case IIAPI_LVCH_TYPE :
    case IIAPI_LBYTE_TYPE :
    case IIAPI_LNVCH_TYPE :	
	/*
	** For LOBs, the fixed sized portion of the 
	** variable length segmented format is used.
	** There isn't a specific ADF definition, but
	** it is everything except the NULL indicator
	** byte in a NULL LOB value.
	*/
	length = ADP_NULL_PERIPH_SIZE - 1;	/* Exclude NULL indicator */
	break;

    case IIAPI_LCLOC_TYPE :
    case IIAPI_LBLOC_TYPE :
    case IIAPI_LNLOC_TYPE :
	length = ADP_LOC_PERIPH_SIZE;
	break;

    default :
	length = descriptor->ds_length;
	break;
    }

    if ( descriptor->ds_nullable )  length++;	/* NULL indicator byte */
    return( length );
}


/*
** Name: IIapi_getBLOBLength
**
** Description:
**	Returns the length of a BLOB as formatted into one
**	segment.  This function does not handle multi-segment
**	BLOBs.
**
** Input:
**	descriptor	API data descriptor.
**	dataValue	API data value.
**
** Output:
**	None.
**
** Return value:
**	II_UINT2	Length of the BLOB in GCA format.
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
**	28-Jul-95 (gordy)
**	    Use fixed length types.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.
**	25-Oct-06 (gordy)
**	    Returns size of single segment.
*/

II_EXTERN II_UINT2
IIapi_getBLOBLength( IIAPI_DESCRIPTOR *descriptor, IIAPI_DATAVALUE *dataValue )
{
    II_UINT2	length = 0;
    u_i2	seg_len;

    if ( ! (descriptor->ds_nullable  &&  dataValue->dv_null)  &&
         dataValue->dv_length > sizeof( seg_len ) )
    {
	MEcopy( (PTR)dataValue->dv_value, sizeof( seg_len ), (PTR)&seg_len );

        if ( seg_len ) 
	    length += formatBLOBseg( seg_len, 
			IIapi_isUCS2( descriptor->ds_dataType ) ? 2 : 1, 
			NULL, NULL );
    }

    return( length );
}


/*
** Name: IIapi_cnvtDescr2GDV
**
** Description:
**	This function copies the content of the API descriptor
**	into the format of the GCA Data Value descriptor.
**
**	For LOBs, the minimum fixed size is used.
**
** Input:
**	descriptor	API descriptor
**
** Output:
**	gcaDataValue	GCA data value
**
** Returns:
**	II_BOOL		TRUE if success, FALSE if memory allocation failure.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**     02-may-94 (ctham)
**         creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 7-Oct-94 (gordy)
**	    Fixed handling of variable length types.
**	11-Nov-94 (gordy)
**	    Return internal length in buffer with variable length data.
**	 9-Dec-94 (gordy)
**	    Don't clear NULL byte if non-nullable.
**	20-Jun-95 (gordy)
**	    Renamed to more closely reflect functionality.
**	31-May-06 (gordy)
**	    New ANSI date/type types utilize precision.
*/

II_EXTERN II_VOID
IIapi_cnvtDescr2GDV
(
    IIAPI_DESCRIPTOR	*descriptor,
    GCD_DATA_VALUE	*gcaDataValue
)
{
    gcaDataValue->gca_type = descriptor->ds_nullable 
			   ? -descriptor->ds_dataType : descriptor->ds_dataType;

    gcaDataValue->gca_l_value = IIapi_getGCALength( descriptor );

    if ( descriptor->ds_dataType == IIAPI_DEC_TYPE )
	gcaDataValue->gca_precision = 
	    DB_PS_ENCODE_MACRO(descriptor->ds_precision, descriptor->ds_scale);
    else
	gcaDataValue->gca_precision = descriptor->ds_precision;
    
    return;
}




/*
** Name: IIapi_cnvtDataValue2GDV
**
** Description:
**	This function copies the content of the API descriptor and
**	data value into the format of the GCA Data Value.  The GCA
**	data value buffer is allocated.
**
**	This function provides limited support for LOBs: NULL
**	values or formatted as a single segment.
**
** Input:
**	memTag		Tag for memory allocation.
**	descriptor	API descriptor
**	dataValue	API data value
**
** Output:
**	gcaDataValue	GCA data value
**
** Returns:
**	II_BOOL		TRUE if success, FALSE if memory allocation failure.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**     02-may-94 (ctham)
**         creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 7-Oct-94 (gordy)
**	    Fixed handling of variable length types.
**	11-Nov-94 (gordy)
**	    Return internal length in buffer with variable length data.
**	 9-Dec-94 (gordy)
**	    Don't clear NULL byte if non-nullable.
**	20-Jun-95 (gordy)
**	    Renamed to more closely reflect functionality.  Add support
**	    for BLOBs.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.
**	 3-Apr-96 (gordy)
**	    BLOB segments may now be sent using the formatted GCA interface.
**	18-Sep-96 (gordy)
**	    Pad/fill data with appropriate values.
**	31-May-06 (gordy)
**	    New ANSI date/type types utilize precision.
**	25-Oct-06 (gordy)
**	    Support Locators, extract formatting to localize operations.
*/

II_EXTERN II_BOOL
IIapi_cnvtDataValue2GDV
(
    II_UINT2		memTag,
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_DATAVALUE	*dataValue,
    GCD_DATA_VALUE	*gcaDataValue
)
{
    II_UINT2	length;
    STATUS	status;

    IIapi_cnvtDescr2GDV( descriptor, gcaDataValue );

    /*
    ** Allocate space for the data value.  For LOBs,
    ** provide additional space for single segment.
    */
    length = gcaDataValue->gca_l_value;
    if ( IIapi_isBLOB( descriptor->ds_dataType ) )
	length += IIapi_getBLOBLength( descriptor, dataValue );

    if ( ! (gcaDataValue->gca_value = 
			(char *)MEreqmem( memTag, length, FALSE, &status )) )
	return( FALSE );
    
    IIapi_cnvtDataValue2GCATuple( descriptor, 
    				  dataValue, (II_PTR)gcaDataValue->gca_value );
    return( TRUE );
}



/*
** Name: IIapi_cnvtDataValue2GCATuple
**
** Description:
**	This function copies the content of an API data value
**	into the GCA format used for tuples.  Caller must ensure 
**	sufficient space, as indicated by IIapi_getGCALength(), 
**	exists for the formatted value.
**
**	This function provides limited support for LOBs: NULL
**	values or formatted as a single segment.
**
** Input:
**     descriptor	API descriptor
**     dataValue	API data value
**     gcaValue		GCA tuple
**
** Output:
**	None.
**
** Returns:
**	VOID
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      04-may-94 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 7-Oct-94 (gordy)
**	    Fixed handling of variable length types.
**	11-Nov-94 (gordy)
**	    Return internal length in buffer with variable length data.
**	 9-Dec-94 (gordy)
**	    Don't clear NULL byte if non-nullable.
**	22-Apr-96 (gordy)
**	    Finished support for COPY FROM.
**	18-Sep-96 (gordy)
**	    Pad/fill data with appropriate values.
**	25-Oct-06 (gordy)
**	    Support NULL/single segment LOBs, Locators.
*/

II_EXTERN II_VOID
IIapi_cnvtDataValue2GCATuple 
(
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_DATAVALUE	*dataValue,
    II_PTR		gcaValue
)
{
    II_UINT2 length;

    if ( descriptor->ds_nullable  &&  dataValue->dv_null )
    {
	length = IIapi_getGCALength( descriptor );
	MEfill( length, '\0', gcaValue );
	IIAPI_SETNULL( length, gcaValue );
        return;
    }

    switch( descriptor->ds_dataType )
    {
    case IIAPI_LVCH_TYPE :
    case IIAPI_LBYTE_TYPE :
    case IIAPI_LNVCH_TYPE :
    {
	u_i2 seg_len;

	length = formatBLOBhdr( ADP_P_GCA_L_UNK, (u_i1 *)gcaValue );

	MEcopy( (PTR)dataValue->dv_value, sizeof( seg_len ), (PTR)&seg_len );
        if ( seg_len )  length += formatBLOBseg( seg_len, 
				IIapi_isUCS2(descriptor->ds_dataType) ? 2 : 1, 
				(u_i1 *)dataValue->dv_value + sizeof(seg_len),
				(u_i1 *)gcaValue + length );

	length += formatBLOBend( (u_i1 *)gcaValue + length );
    	break;
    }
    case IIAPI_LCLOC_TYPE :
    case IIAPI_LBLOC_TYPE :
    case IIAPI_LNLOC_TYPE :
	length = formatBLOBhdr( ADP_P_LOC_L_UNK, (u_i1 *)gcaValue );
    	MEcopy( dataValue->dv_value, 
		sizeof( ADP_LOCATOR ), (PTR)((u_i1 *)gcaValue + length) );
    	length += sizeof( ADP_LOCATOR );
    	break;

    default :
	length = min( descriptor->ds_length, dataValue->dv_length );
	MEcopy( dataValue->dv_value, length, gcaValue );

	if ( length < descriptor->ds_length )
	    if ( descriptor->ds_dataType != IIAPI_NCHA_TYPE )
		MEfill( descriptor->ds_length - length, 
			( descriptor->ds_dataType == IIAPI_CHA_TYPE  ||
			  descriptor->ds_dataType == IIAPI_CHR_TYPE ) 
			  ? ' ' : '\0', 
			(PTR)((u_i1 *)gcaValue + length) );
	    else
	    {
		u_i2	pad = 0x0020;	/* UCS-2 space */
		u_i1	*ptr;

		for( 
		     ptr = (u_i1 *)gcaValue + length; 
		     length < descriptor->ds_length; 
		     length += sizeof(pad), ptr += sizeof(pad) 
		   )
		    MECOPY_CONST_MACRO( (PTR)&pad, sizeof(pad), (PTR)ptr );
	    }

        length = descriptor->ds_length;
    	break;
    }

    if ( descriptor->ds_nullable )  IIAPI_SETNOTNULL( length + 1, gcaValue );
    return;
}



/*
** Name: IIapi_cnvtBLOB2GCATuple
**
** Description:
**	Formats a single segment of a BLOB in GCA format.  Adds BLOB
**	headers and trailers as needed.  Returns TRUE when the current
**	segment has been fully processed.  If moreSegments is TRUE, a
**	new input segment should be obtained.  Otherwise, the BLOB is
**	completed.  Returns FALSE when additional formatting must be
**	done on the current segment.  In either case, the output buffer
**	should be flushed.
**
** Input:
**	stmtHndl	Statement handle.
**	descriptor	API data descriptor.
**	dataValue	API data value.
**	moreSegments	TRUE if there are more segments to follow.
**	buffer		Tuple buffer.
**	length		Length remaining in tuple buffer.
**
** Output:
**	length		Length used by segment in tuple buffer.
**
** Returns:
**	II_BOOL		TRUE if segment completely processed.
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
**	 3-Apr-96 (gordy)
**	    BLOB segments may now be sent using the formatted GCA interface.
*/

II_EXTERN II_BOOL
IIapi_cnvtBLOB2GCATuple
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_DATAVALUE	*dataValue,
    II_BOOL		moreSegments,
    char 		*buffer,
    II_LONG		*length
)
{
    II_BOOL	first_segment = FALSE;
    II_BOOL	last_segment = FALSE;
    u_i2	seg_len;

    /*
    ** Initialize processing.  The segment length 
    ** in the statement handle is used as follows:
    **
    **	0   : Start of new input segment.
    **  > 0 : Data remaining in current segment.
    **	< 0 : BLOB completed, need end-of-segment indicator.
    **
    ** When starting a new BLOB, the segment length is set 
    ** for the first input segment.  Otherwise, a 0 value 
    ** indicates that we need to get the length of the 
    ** new input segment and a non-zero value indicates
    ** additional work is required on the current segment.
    */
    if ( ! (stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS) )  
    {
	first_segment = TRUE;

	/*
	** Validate the size of the current segment and
	** existence of trailing segments.
	*/
	if ( ! (descriptor->ds_nullable  &&  dataValue->dv_null)  &&
	     dataValue->dv_length >= sizeof( seg_len ) )
	{
	    MEcopy( (PTR)dataValue->dv_value, sizeof(seg_len), (PTR)&seg_len );
	    stmtHndl->sh_segmentLength = seg_len;
	}
	else
	{
	    moreSegments = FALSE;
	    stmtHndl->sh_segmentLength = 0;
	}
    }
    else  if ( stmtHndl->sh_segmentLength == 0  &&  
	       dataValue->dv_length >= sizeof( seg_len ) )
    {
	MEcopy( (PTR)dataValue->dv_value, sizeof(seg_len), (PTR)&seg_len );
	stmtHndl->sh_segmentLength = seg_len;
    }

    /*
    ** We only want to process the end-of-segment indicator
    ** when all data segments have been completed.
    */
    if ( ! moreSegments  &&  stmtHndl->sh_segmentLength <= 0 )  
	last_segment = TRUE;

    seg_len = max( 0, stmtHndl->sh_segmentLength );

    /*
    ** We only set last_segment TRUE when there is no
    ** data segment, so we don't care if the segment
    ** is split.  We use the returned segment length
    ** to determine when the current segment is done.
    */
    cnvtBLOB2GCATuple( descriptor, dataValue, &seg_len,
		       first_segment, last_segment, buffer, length );

    stmtHndl->sh_segmentLength = seg_len;

    /*
    ** Set our current processing state.
    */
    if ( last_segment  &&  *length )
    {
	/*
	** We be done.  Segment length should be 0 at
	** this point, but make sure so that we return
	** the correct indication to the caller.
	*/
	stmtHndl->sh_flags &= ~IIAPI_SH_MORE_SEGMENTS;
	stmtHndl->sh_segmentLength = 0;
    }
    else  
    {
	/*
	** We only want to set the segment processing flag
	** when the header has been successfully processed.
	*/
	if ( first_segment  &&  *length )
	    stmtHndl->sh_flags |= IIAPI_SH_MORE_SEGMENTS;

	/*
	** If we are done with the current segment and
	** there are no more segments to follow, set
	** the current segment length negative as a way
	** of indicating that there is more to be done
	** (processing the end-of-segments indicator).
	*/
	if ( ! moreSegments  &&  ! stmtHndl->sh_segmentLength )
	    stmtHndl->sh_segmentLength = -1;
    }

    /*
    ** We return TRUE when we have completed the
    ** current segment.  If there is segment data
    ** remaining or the end-of-segments indicator
    ** needs to be processed, return FALSE.
    */
    return( ! stmtHndl->sh_segmentLength );
}




/*
** Name: IIapi_cnvtGDV2Descr
**
** Description:
**	This function copies the content of a GCA descriptor to
**	a API descriptor.
**
**	This function can handle long data types.
**
** Input:
**	gcaDataType		GCA data type.
**	gcaLength		GCA data length.
**	gcaPrecision		GCA data precision.
**	columnNameLength	Length of column name.
**	columnName		Column name.
**
** Output:
**	descriptor		API descriptor.
**
** Return:
**	II_BOOL			TRUE if success, FALSE if allocation failure.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**     02-may-94 (ctham)
**         creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 7-Oct-94 (gordy)
**	    Fixed handling of variable length types.
**	13-Jun-95 (gordy)
**	    BLOB GCA info does not provide actual or segment length.
**	    Set BLOB API descriptors to max segment length.
**	28-Jul-95 (gordy)
**	    Use fixed length types.
**	21-Oct-99 (rajus01)
**	    Changed BLOB segment length flag indicator from
**	    IIAPI_EP_MAX_SEGMENT_LEN to IIAPI_UP_MAX_SEGMENT_LEN.
**	31-May-06 (gordy)
**	    New ANSI date/type types utilize precision.
*/

II_EXTERN II_BOOL
IIapi_cnvtGDV2Descr
(	
    IIAPI_ENVHNDL	*envHndl,
    IIAPI_DT_ID		gcaDataType,
    II_LONG	  	gcaLength,
    i4			gcaPrecision,
    II_INT2		columnNameLength,
    char		*columnName,
    IIAPI_DESCRIPTOR	*descriptor
)
{
    descriptor->ds_columnType = IIAPI_COL_TUPLE;
    descriptor->ds_dataType = abs( gcaDataType );
    descriptor->ds_nullable = gcaDataType < 0;
    descriptor->ds_length = IIapi_getAPILength( envHndl, gcaDataType,
    						gcaLength, NULL );

    if ( descriptor->ds_dataType == IIAPI_DEC_TYPE )
    {
	descriptor->ds_precision = DB_P_DECODE_MACRO( gcaPrecision );
	descriptor->ds_scale = DB_S_DECODE_MACRO( gcaPrecision );
    }
    else
    {
	descriptor->ds_precision = gcaPrecision;
        descriptor->ds_scale = 0;
    }

    if ( columnNameLength )
    {
	STATUS status;
    
	if ( ! ( descriptor->ds_columnName = (char *)
	         MEreqmem( 0, columnNameLength + 1, TRUE, &status ) ) )
	    return( FALSE );

	MEcopy( columnName, columnNameLength, descriptor->ds_columnName );
    }

    return( TRUE );
}



/*
** Name: IIapi_cnvtGDV2DataValue
**
** Description:
**	This function copies an entire GCA data value into API format.  
**	Caller must allocate data value buffer at least as large as the 
**	length in the descriptor.
**
**	This function does not handle LOBs, locators, partial or
**	compressed variable length data values.
**
** Input:
**	descriptor	descriptor of data value
**	buffer		data value
**
** Output:
**	dataValue	API buffer to store data value
**
** Returns:
**	Void.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      03-may-94 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 7-Oct-94 (gordy)
**	    Fixed handling of variable length types.
**	10-Mar-95 (gordy)
**	    Make sure dv_null is FALSE if not null.
**	13-Jun-95 (gordy)
**	    Do not allocate data value buffers.  Caller must
**	    assure that dv_value is large enough for data.
**	    Return type changed to void.
*/

II_EXTERN II_VOID
IIapi_cnvtGDV2DataValue 
(
    IIAPI_DESCRIPTOR	*descriptor,
    char		*gcaValue,
    IIAPI_DATAVALUE	*dataValue
)
{
    if ( IIAPI_ISNULLDESC( descriptor, gcaValue ) )
    {
	dataValue->dv_null = TRUE;
	dataValue->dv_length = 0;
    	return;
    }

    dataValue->dv_null = FALSE;

    /*
    ** This may seem like a circular way of doing
    ** things, but IIapi_getAPILength() will handle
    ** variable length data types by looking at the
    ** actual data rather than just the descriptor.
    ** Since IIapi_getAPILength() requires the GCA
    ** data length and all we have is the max API 
    ** length in the descriptor, we also have to
    ** call IIapi_getGCALength().
    */
    dataValue->dv_length = 
	    IIapi_getAPILength( NULL, descriptor->ds_nullable 
				      ? -descriptor->ds_dataType 
				      : descriptor->ds_dataType,
				IIapi_getGCALength( descriptor ), gcaValue );

    MEcopy( gcaValue, dataValue->dv_length, dataValue->dv_value );
    return;
}



/*
** Name: load_columns
**
** Description:
**	Copy data values from GCA tuple to API data values.  This
**	routine handles Binary Large OBject columns, compressed 
**	variable length columns, and fixed length columns.  This
**	routine only reads columns for the current row.  Multi-row
**	requests must call this routine for each row.
**
** Input:
**	stmtHndl	Statement Handle.
**	getColParm	IIapi_getColumns() parameters.
**	gcaBuff		GCA Receive buffer.
**
** Output:
**	None.
**
** Returns:
**
** History:
**	13-Jun-95 (gordy)
**	    Created.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	15-Mar-96 (gordy)
**	    Added support for compressed varchars.
**	27-Mar-96 (gordy)
**	    Extracted from IIapi_loadColumns() for single row processing.
**	20-May-96 (gordy)
**	    Changes in GCA buffering result in the BLOB end-of-segments
**	    indicator being available after reading a data segment, so
**	    check for end of blob after satisfying a segment request.
**	18-Mar-97 (gordy)
**	    Extracted buffer processing for BLOBs to facilitate segment
**	    concatenation.
**	12-Jul-02 (gordy)
**	    Check for column values which are split across GCA buffers.
**	    Call cnvtGDV2DataValue for long data values.
**	25-Oct-06 (gordy)
**	    Use cnvtGDV2DataValue for all non-LOB values.
*/

static II_VOID
load_columns
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_GETCOLPARM	*getColParm,
    IIAPI_GCA_BUFF	*gcaBuff
)
{
    IIAPI_DESCRIPTOR	*descr;
    IIAPI_DATAVALUE	*value;
    i4			length;
    u_i2		len;
    bool		done;

    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_loadColumns: %d columns starting with %d, %d total columns\n",
	  (II_LONG)stmtHndl->sh_colFetch, 
	  (II_LONG)stmtHndl->sh_colIndex, 
	  (II_LONG)stmtHndl->sh_colCount );

    descr = &stmtHndl->sh_colDescriptor[ stmtHndl->sh_colIndex ];
    value = &getColParm->gc_columnData[ (getColParm->gc_rowsReturned * 
					 getColParm->gc_columnCount) +
					 getColParm->gc_columnCount -
					 stmtHndl->sh_colFetch ];

    /*
    ** Process columns until all done or input buffer exhausted.
    */
    for( ; stmtHndl->sh_colFetch; 
	 stmtHndl->sh_colFetch--, stmtHndl->sh_colIndex++, descr++, value++ )
    {
	/*
	** Blobs are broken into segments of variable lengths
	** and require special handling to load segments using 
	** internal data value info.
	*/
	if ( IIapi_isBLOB( descr->ds_dataType ) )
	    done = load_blob( stmtHndl, getColParm, descr, gcaBuff, value );
	else
	    done = cnvtGDV2DataValue( descr, gcaBuff, value, 
		(stmtHndl->sh_flags & IIAPI_SH_COMP_VARCHAR) ? TRUE : FALSE );

	if ( ! done )  break;	/* Need more data for current column */

	IIAPI_TRACE( IIAPI_TR_VERBOSE )
	    ( "IIapi_loadColumns: loaded data for column %d\n",
	      (II_LONG)stmtHndl->sh_colIndex );
    }

    return;
}



/*
** Name: IIapi_loadColumns
**
** Description:
**	Copy data values from GCA tuple to API data values.  This
**	routine handles Binary Large OBject columns, compressed 
**	variable length columns, and fixed length columns.  Multi-
**	row fetchs are also supported.
**
** Input:
**	stmtHndl	Statement Handle.
**	getColParm	IIapi_getColumns() parameters.
**	gcaBuff		GCA Receive buffer.
**
** Output:
**	None.
**
** Returns:
**
** History:
**	13-Jun-95 (gordy)
**	    Created.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	15-Mar-96 (gordy)
**	    Added support for compressed varchars.
**	27-Mar-96 (gordy)
**	    Extracted individual column handling to load_columns().
**	15-Mar-07 (gordy)
**	    Track the number of rows returned for a cursor fetch.
*/

II_EXTERN II_VOID
IIapi_loadColumns
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_GETCOLPARM	*getColParm,
    IIAPI_GCA_BUFF	*gcaBuff
)
{
    /*
    ** If no data available or request complete, we be done!
    */
    if ( gcaBuff->data_len < 1  ||  ! stmtHndl->sh_colFetch  ||
	 getColParm->gc_rowsReturned >= getColParm->gc_rowCount )
    {
	IIAPI_TRACE( IIAPI_TR_DETAIL )
	    ( "IIapi_loadColumns: nothing to do\n" );
	return;
    }

    IIAPI_TRACE( IIAPI_TR_DETAIL )
	( "IIapi_loadColumns: converting tuple data to API format\n" );
    
    /*
    ** Loop for each row in the request.
    */
    do
    {
	/*
	** Process columns in the current row.  The current
	** column request should not span rows, but may be
	** fewer than the remaining columns in the row.  If
	** this is a multi-row fetch, we will reset the
	** column request for the next row after processing
	** the current row.  The current row request will
	** either be satisfied, or the tuple data will be
	** exhausted.
	*/
	load_columns( stmtHndl, getColParm, gcaBuff );

	/*
	** If there are still columns to be processed in
	** the current request, exit to get more data.
	*/
	if ( stmtHndl->sh_colFetch )  break;

	/*
	** We have completed the request for the current row.
	** We bump the number of rows returned since we either
	** completed a row or completed a partial fetch of a
	** row.  We will either exit at this point or move
	** on to the next row.
	*/
	getColParm->gc_rowsReturned++;

	/*
	** Check to see if we have completed a row.
	*/
	if ( stmtHndl->sh_colIndex >= stmtHndl->sh_colCount )
	{
	    /*
	    ** For cursors, decrement the remaining row count now
	    ** that a row has been completed.
	    */
	    if ( stmtHndl->sh_flags & IIAPI_SH_CURSOR )
		stmtHndl->sh_rowCount--;

	    /*
	    ** Set the column index for the start of the next row.
	    ** If there are more rows in the request, set the 
	    ** column fetch count for the next row.
	    */
	    stmtHndl->sh_colIndex = 0;

	    if ( getColParm->gc_rowsReturned < getColParm->gc_rowCount )
		stmtHndl->sh_colFetch = stmtHndl->sh_colCount;
	}

    } while( stmtHndl->sh_colFetch );

    return;
}



/*
** Name: IIapi_checkBLOBSegment
**
** Description:
**	Checks the GCA buffer for the next segment indicator
**	(MORE_SEGMENTS flag must be set).  If it is the 
**	end-of-segments indicator, removes the indicator and 
**	(optional) null byte from the buffer and turns off the 
**	MORE_SEGMENTS flag.  Otherwise, nothing is done.
**
**	The null byte value is ignored (if present).  The null 
**	byte is only significant when the BLOB header is im-
**	mediatly followed by an end-of-segments indicator.  
**	This case is completely handled by cnvtBLOB2DataValue() 
**	and so this routine should never be called when the null 
**	indicator is significant.
**
**	This routine does nothing if segment purging is active.
**
** Input:
**	stmtHndl	Statement Handle.
**	descriptor	API descriptor of BLOB.
**	gcaBuff		GCA tuple buffer.
**
** Output:
**	None.
**
** Returns:
**	Void.
**
** History:
**	13-Jun-95 (gordy)
**	    Created.
*/

II_EXTERN II_VOID
IIapi_checkBLOBSegment
( 
    IIAPI_STMTHNDL	*stmtHndl, 
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_GCA_BUFF	*gcaBuff 
)
{
    i4  indicator;
    i4  length = sizeof( indicator );

    /*
    ** We only check when we know it is safe to do so.
    ** We must be reading segments, but not purging.
    ** and we require the request to be complete to
    ** avoid scenarios to complicated to explain or
    ** properly handle.
    */
    if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS  &&
         ! ( stmtHndl->sh_flags & IIAPI_SH_PURGE_SEGMENTS )  &&
	 ! stmtHndl->sh_colFetch  &&  gcaBuff->data_len >= length )
    {
	/*
	** Peak at the indicator.
	*/
	MECOPY_CONST_MACRO( gcaBuff->data, length, (PTR)&indicator );

	if ( ! indicator )
	{
	    /*
	    ** We have the end-of-segments indicator.
	    ** Only consume it if the (optional) null
	    ** byte is also available (if not nullable,
	    ** the test is redundant).  The null byte
	    ** value is ignored (see description).
	    */
	    if ( descriptor->ds_nullable )  length++;

	    if ( gcaBuff->data_len >= length )
	    {
		IIAPI_TRACE( IIAPI_TR_VERBOSE )
		    ( "IIapi_checkBLOBSegment: found end-of-segments\n" );

		gcaBuff->data += length;
		gcaBuff->data_len -= length;

		/*
		** Since we have now finished the BLOB,
		** advance the column index to the next
		** column (wrap if finished all columns).
		** Since we check for sh_colFetch to be 
		** 0, we assume the gc_rowsReturned is
		** set properly and we are not in the
		** midst of a multi-row fetch.
		*/
		if ( ++stmtHndl->sh_colIndex >= stmtHndl->sh_colCount )
		    stmtHndl->sh_colIndex = 0;
		stmtHndl->sh_flags &= ~IIAPI_SH_MORE_SEGMENTS;
	    }
	}
    }

    return;
}


/*
** Name: load_blob
**
** Description:
**	Processes BLOB segments in the GCA input buffer until the BLOB
**	is completed, the output data value buffer is full, or the GCA
**	input buffer is exhausted.  Returns TRUE when the BLOB is
**	completed.
**
**	If the output data value buffer gets filled and the current
**	column request is complete, standard column processing is
**	performed and FALSE is returned so that the processing is 
**	not duplicated.
**
** Input:
**	stmtHndl	Statement Handle.
**	getColParm	IIapi_getColumns() parameters.
**	descriptor	API descriptor of datavalue.
**	gcaBuff		GCA tuple buffer containing data.
**
** Output:
**	dataValue	API datavalue buffer.
**
** Returns:
**	II_BOOL		TRUE if BLOB completed, FALSE if need more data.
**
** History:
**	18-Mar-97 (gordy)
**	    Created.
*/

static II_BOOL
load_blob
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_GETCOLPARM	*getColParm,
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_GCA_BUFF	*gcaBuff,
    IIAPI_DATAVALUE	*dataValue
)
{
    if ( ! (stmtHndl->sh_flags & IIAPI_SH_PURGE_SEGMENTS ) )
    {
	/*
	** Process the BLOB segments in the input buffer
	** until we empty the input buffer, the end of
	** the BLOB is reached, or we fill the output
	** buffer.
	*/
	do
	{
	    if ( ! cnvtBLOB2DataValue(stmtHndl,descriptor,gcaBuff,dataValue) )
		return( FALSE );

	    if ( ! (stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS) )
		return( TRUE );

	} while( dataValue->dv_length < descriptor->ds_length );

	/*
	** We have satisfied the request if this is the
	** last column of a single row request.  Other-
	** wise we must purge the rest of the BLOB before
	** processing the rest of the request.
	*/
	if ( stmtHndl->sh_colFetch <= 1  &&  getColParm->gc_rowCount <= 1 )
	{
	    /*
	    ** There is a (very small) chance that we
	    ** we filled the output buffer and read the
	    ** last input segment at the same time.  In
	    ** this case the end-of-segments indicator
	    ** has not been processed and may or may not
	    ** be in the input buffer.
	    **
	    ** Normally we would just return TRUE at this
	    ** point, but the special case complicates
	    ** things.  If the end-of-segments indicator
	    ** is in the input buffer, we want to process
	    ** it so that the application can be told
	    ** that this is the last segment.  The check
	    ** can be made by calling IIapi_checkBLOBSegment(),
	    ** but some special conditions must be handled.
	    **
	    ** IIapi_checkBLOBSegment() requires the request
	    ** to be complete and will change the column index
	    ** if the end-of-segments indicator is processed.
	    ** The result of all this is that we must do the
	    ** work to finish the column processing and return
	    ** FALSE so that the caller will not duplicate the
	    ** work.
	    */
	    stmtHndl->sh_colFetch--;
	    IIapi_checkBLOBSegment( stmtHndl, descriptor, gcaBuff );
	    return( FALSE );
	}

	stmtHndl->sh_flags |= IIAPI_SH_PURGE_SEGMENTS;
	IIAPI_TRACE( IIAPI_TR_VERBOSE )
	    ( "cnvtBLOB2DataValue: begin purging segments\n" );
    }

    /*
    ** Purge the input buffer of BLOB segments
    ** until we find the end of the blob or we
    ** empty the input buffer.
    */
    while( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )
	if ( ! cnvtBLOB2DataValue( stmtHndl, descriptor, gcaBuff, dataValue ) )
	    return( FALSE );
    
    /*
    ** We must be done with the BLOB.
    */
    return( TRUE );
}



/*
** Name: cnvtGDV2DataValue
**
** Description:
**	This function copies a GCA data value into API format, even
**	those which may be split across multiple GCA receive buffers.  
**	Caller must allocate data value buffer at least as large as 
**	the length in the descriptor.
**
**	This function does not handle long data types.
**
** Input:
**	descriptor	API descriptor of data value
**	gcaBuff		GCA buffer.
**	dataValue	API buffer with partial data value.
**	compressed	TRUE if variable length values are compressed.
**
** Output:
**	dataValue	API buffer to store data value
**
** Returns:
**	II_BOOL		TRUE if value processed, FALSE if need more data.
**
** History:
**	12-Jul-02 (gordy)
**	    Created.
**	25-Oct-06 (gordy)
**	    Support compressed variable length data types and locators.
*/

static II_BOOL
cnvtGDV2DataValue
(
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_GCA_BUFF	*gcaBuff,
    IIAPI_DATAVALUE	*dataValue,
    II_BOOL		compressed
)
{
    i4 data_len, avail_len;

    /*
    ** Check that any minimum required length 
    ** is available and determine GCA data length.
    */
    switch( descriptor->ds_dataType )
    {
    case IIAPI_VCH_TYPE :
    case IIAPI_VBYTE_TYPE :
    case IIAPI_NVCH_TYPE :
    case IIAPI_TXT_TYPE :
    case IIAPI_LTXT_TYPE :
	if ( ! compressed )
	    data_len = descriptor->ds_length;
	else
	{
	    u_i2 len;

	    /*
	    ** The embeded length indicator is required.  It may
	    ** have been already loaded (dv_length > 0) or may
	    ** be available in the input buffer.  Otherwise, wait
	    ** until it is available.
	    */
	    if ( dataValue->dv_length )
		MEcopy( dataValue->dv_value, sizeof( len ), (PTR)&len );
	    else  if ( gcaBuff->data_len >= sizeof( len ) )  
		MEcopy( (PTR)gcaBuff->data, sizeof( len ), (PTR)&len );
	    else
	    {
		IIAPI_TRACE( IIAPI_TR_VERBOSE )
		    ( "cnvtGDV2DataValue: need embedded length (%d bytes, %d available)\n",
		      sizeof( len ), gcaBuff->data_len );
		return( FALSE );
	    }

	    /*
	    ** Get the actual length of the compressed column.
	    */
	    data_len = sizeof( len ) + 
		       (IIapi_isUCS2(descriptor->ds_dataType) ? len * 2 : len);
	}
    	break;

    case IIAPI_LCLOC_TYPE :
    case IIAPI_LBLOC_TYPE :
    case IIAPI_LNLOC_TYPE :
	/*
	** The ADF PERIPHERAL header is not used
	** and needs to be skipped.  To ensure 
	** it is read correctly, wait until the 
	** entire ADP locator object is available.
	*/
	if ( gcaBuff->data_len < (ADP_HDR_SIZE + sizeof(ADP_LOCATOR)) )
	{
	    IIAPI_TRACE( IIAPI_TR_VERBOSE )
		( "cnvtGDV2DataValue: need LOCATOR (%d bytes, %d available)\n",
		  ADP_HDR_SIZE + sizeof(ADP_LOCATOR), gcaBuff->data_len );
	    return( FALSE );
	}

	/*
	** With the full ADP locator object available,
	** skip the header and set remaining data length.
	*/
	gcaBuff->data += ADP_HDR_SIZE;
	gcaBuff->data_len -= ADP_HDR_SIZE;
	data_len = sizeof( ADP_LOCATOR );
    	break;

    default :
	data_len = descriptor->ds_length;
    	break;
    }

    /*
    ** Determine amount of data available to be loaded:
    ** smaller of amount in input buffer and amount not 
    ** yet loaded.
    */
    avail_len = min( gcaBuff->data_len, data_len - dataValue->dv_length );

    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "cnvtGDV2DataValue: loading %d of %d bytes (%d loaded)\n",
	  avail_len, data_len, (II_LONG)dataValue->dv_length );

    /*
    ** Load available data in input buffer.
    */
    if ( avail_len )
    {
	MEcopy( (PTR)gcaBuff->data, avail_len, 
		(PTR)((char *)dataValue->dv_value + dataValue->dv_length) );
	gcaBuff->data += avail_len;
	gcaBuff->data_len -= avail_len;
	dataValue->dv_length += avail_len;
    }

    /*
    ** Check to see if all data has been loaded.
    */
    if ( dataValue->dv_length < data_len )
    {
	IIAPI_TRACE( IIAPI_TR_VERBOSE )
	    ( "cnvtGDV2DataValue: insufficient data: need %d bytes\n",
	      data_len - dataValue->dv_length );
	return( FALSE );
    }

    /*
    ** For nullable columns, check the NULL byte.
    */
    if ( descriptor->ds_nullable )
    {
	/*
	** Check to see if NULL byte is in input buffer.
	*/
	if ( ! gcaBuff->data_len )  
	{
	    IIAPI_TRACE( IIAPI_TR_VERBOSE )
		( "cnvtGDV2DataValue: need NULL byte (1 byte, 0 available\n" );
	    return( FALSE );
	}

	if ( ! (*gcaBuff->data & ADF_NVL_BIT) )
	    dataValue->dv_null = FALSE;
	else
	{
	    dataValue->dv_null = TRUE;
	    dataValue->dv_length = 0;	/* Force to 0 since set above */
	}

	gcaBuff->data++;	/* Remove NULL byte from input buffer */
	gcaBuff->data_len--;
    }

    if ( IIapi_isVAR( descriptor->ds_dataType )  &&  ! dataValue->dv_null )
    {
	/*
	** Variable length types only return the length of
	** the actual data, not their fully padded length.
	*/
	u_i2 len;

	MEcopy( dataValue->dv_value, sizeof( len ), (PTR)&len );
	dataValue->dv_length = sizeof( len ) +
		    (IIapi_isUCS2( descriptor->ds_dataType ) ? len * 2 : len);
    }

    return( TRUE );
}



/*
** Name: cnvtBLOB2DataValue
**
** Description:
**	Converts BLOB data values from GCA tuple format to API format.
**	A single input BLOB segment (or portion) is appended to the 
**	output segment.  The output segment is initialized on the first
**	call to this routine for a new BLOB.
**	
**	If starting a new BLOB (as indicated by no MORE_SEGMENTS flag),
**	reads the ADP_PERIPHERAL header and sets the MORE_SEGMENTS flag.
**	Reads a segment indicator.  If end-of-segments, completes the
**	processing of the BLOB and clears the MORE_SEGMENTS flag.  Other-
**	wise, appends the available portion of the next segment to the
**	output segment.
**
**	Returns FALSE if insufficient data to process header or segment.
**	Additional data may remain in input buffer after processing.
**
** Input:
**	stmtHndl	Statement Handle.
**	descriptor	API descriptor of datavalue.
**	gcaBuff		GCA tuple buffer containing data.
**
** Output:
**	dataValue	API datavalue buffer.
**
** Returns:
**	II_BOOL		FALSE if need more data, TRUE if successful.
**
** History:
**	13-Jun-95 (gordy)
**	    Created.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.
**	18-Mar-97 (gordy)
**	    Added the ability to append segments to support 
**	    re-segmentation to fill the output buffer.
*/

static II_BOOL
cnvtBLOB2DataValue 
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_GCA_BUFF	*gcaBuff,
    IIAPI_DATAVALUE	*dataValue
)
{
    u_i2	char_size = IIapi_isUCS2( descriptor->ds_dataType ) ? 2 : 1;
    i4		indicator = 0;
    i4		length;
    u_i2	seg_len;

    /*
    ** The output buffer needs to be initialized with a 
    ** zero length segment.  Subsequent processing then 
    ** only needs to append input segments to the current 
    ** output segment.  The initial output data value 
    ** length is set to 0 (see IIapi_getColumns()) so 
    ** that we can detect when initialization is required.  
    ** Any non-zero value indicates an existing segment.
    */
    if ( ! dataValue->dv_length )
    {
	seg_len = 0;
	dataValue->dv_null = FALSE;
	dataValue->dv_length = sizeof( seg_len );
	MECOPY_CONST_MACRO( (PTR)&seg_len, 
			    sizeof( seg_len ), dataValue->dv_value );
    }

    /*
    ** The MORE_SEGMENTS flag gets set once the BLOB
    ** header is read.  If it is not set, we need to
    ** read the header.
    */
    if ( ! ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS ) )
    {
	/*
	** BLOB processing assumes that if this is a NULL or
	** zero length BLOB, the entire BLOB is processed with
	** the header information.  In addition to the header,
	** we need to have access to the first segment indicator
	** and possibly the NULL byte.  Note that if the BLOB
	** is nullable but not NULL, the additional byte will
	** come from the segment length of the first segment.
	*/
	length = ADP_HDR_SIZE + sizeof( indicator ) + 
		 (descriptor->ds_nullable ? 1 : 0);

	if ( gcaBuff->data_len < length )
	{
	    IIAPI_TRACE( IIAPI_TR_VERBOSE )
		( "cnvtBLOB2DataValue: need BLOB header for column %d\n",
		  (II_LONG)stmtHndl->sh_colIndex );
	    return( FALSE );
	}

	/*
	** Remove the header from the input buffer.  I 
	** guess we could actually look at the header 
	** info, but not sure what to do with it.  At 
	** this point we begin processing segments.
	*/
	gcaBuff->data += ADP_HDR_SIZE;
	gcaBuff->data_len -= ADP_HDR_SIZE;
	stmtHndl->sh_flags |= IIAPI_SH_MORE_SEGMENTS;

	IIAPI_TRACE( IIAPI_TR_VERBOSE )
	    ( "cnvtBLOB2DataValue: processed BLOB header for column %d\n",
	      (II_LONG)stmtHndl->sh_colIndex );
    }

    /*
    ** Process the segment indicator.
    */
    length = sizeof( indicator );

    if ( gcaBuff->data_len >= length )
    {
	/*
	** To consume the indicator, must have sufficient remaining
	** data to satisfy the request.  There are three cases:
	**
	** Indicator value	Nullable	Bytes Needed
	** ---------------	--------	------------
	**        0		   N		0 
	**        0		   Y		1 (NULL byte)
	**        1		  Y/N		3 (segment length + data)
	*/
	MECOPY_CONST_MACRO( gcaBuff->data, length, (PTR)&indicator );
	length += indicator ? sizeof( seg_len ) + char_size
			    : (descriptor->ds_nullable ? 1 : 0);
    }

    if ( gcaBuff->data_len < length )
    {
	IIAPI_TRACE( IIAPI_TR_VERBOSE )
	    ("cnvtBLOB2DataValue: need segment indicator for column %d\n",
	     (II_LONG)stmtHndl->sh_colIndex);
	return( FALSE );
    }

    /*
    ** Remove the indicator from the input buffer.
    */
    gcaBuff->data += sizeof( indicator );
    gcaBuff->data_len -= sizeof( indicator );

    /*
    ** If the indicator is 0, this is the end of the
    ** data segments and the BLOB.  Otherwise a
    ** segment follows.
    */
    if ( ! indicator )
    {
	IIAPI_TRACE( IIAPI_TR_VERBOSE )
	    ( "cnvtBLOB2DataValue: found end-of-segments indicator\n" );

	/*
	** During purging we ignore the NULL byte since
	** the data value was initialized when purging
	** started.
	*/
	if ( ! (stmtHndl->sh_flags & IIAPI_SH_PURGE_SEGMENTS) )
	    dataValue->dv_null = 
		descriptor->ds_nullable 
		    ? IIAPI_ISNULL(-descriptor->ds_dataType, 1, gcaBuff->data)
		    : FALSE;

	/*
	** Remove NULL byte from input buffer and
	** terminate BLOB segment processing.
	*/
	if ( descriptor->ds_nullable )
	{
	    gcaBuff->data++;
	    gcaBuff->data_len--;
	}

	stmtHndl->sh_flags &= ~(IIAPI_SH_MORE_SEGMENTS |
				IIAPI_SH_PURGE_SEGMENTS);
    }
    else
    {
	/*
	** Read the segment length.  Limit the length
	** to what is available in the input buffer.
	*/
	MECOPY_CONST_MACRO( gcaBuff->data, sizeof( seg_len ), (PTR)&seg_len );
	gcaBuff->data += sizeof( seg_len );
	gcaBuff->data_len -= sizeof( seg_len );
	length = min( seg_len, gcaBuff->data_len / char_size ) * char_size;

	if ( stmtHndl->sh_flags & IIAPI_SH_PURGE_SEGMENTS )
	{
	    /*
	    ** Remove the available portion of
	    ** the segment from the input buffer.
	    */
	    gcaBuff->data += length;
	    gcaBuff->data_len -= length;
	    stmtHndl->sh_flags |= IIAPI_SH_LOST_SEGMENTS;

	    IIAPI_TRACE( IIAPI_TR_VERBOSE )
		( "cnvtBLOB2DataValue: discarding segment of %d bytes\n",
		  length );
	}
	else
	{
	    u_i2 new_len;

	    /*
	    ** We further limit the segment length to be
	    ** processed by the space available in the
	    ** output buffer.
	    */
	    length = min(length, descriptor->ds_length - dataValue->dv_length);
	    length -= length % char_size;	/* Whole characters only */

	    /*
	    ** Append input segment data to output segment.
	    */
	    MEcopy( gcaBuff->data, length, 
		    (PTR)((char *)dataValue->dv_value + dataValue->dv_length) );
	    gcaBuff->data += length;
	    gcaBuff->data_len -= length;
	    dataValue->dv_length += length;

	    /*
	    ** Update the output embedded segment length.
	    */
	    new_len = (dataValue->dv_length - sizeof( new_len )) / char_size;
	    MECOPY_CONST_MACRO( (PTR)&new_len, 
				sizeof( new_len ), dataValue->dv_value );

	    IIAPI_TRACE( IIAPI_TR_VERBOSE )
		( "cnvtBLOB2DataValue: processed segment of %d bytes\n",
		  length );
	}

	/*
	** If the entire input segment was not consumed,
	** patch what remains to be a valid BLOB segment.
	** Create a segment indicator and segment length
	** to represent the remaining data.  There is 
	** room in the input buffer for this info to be 
	** pre-pended to what remains since the same 
	** info was removed from the input buffer above.
	*/
	if ( (length /= char_size) != seg_len )
	{
	    seg_len -= length;				/* Segment length */
	    gcaBuff->data -= sizeof( seg_len );
	    gcaBuff->data_len += sizeof( seg_len );
	    MECOPY_CONST_MACRO((PTR)&seg_len, sizeof(seg_len), gcaBuff->data);

	    gcaBuff->data -= sizeof( indicator );	/* Indicator */
	    gcaBuff->data_len += sizeof( indicator );
	    MECOPY_CONST_MACRO( (PTR)&indicator, 
				sizeof( indicator ), gcaBuff->data );

	    IIAPI_TRACE( IIAPI_TR_VERBOSE )
		( "cnvtBLOB2DataValue: split segment, %d bytes remain\n",
		  (II_LONG)seg_len );
	}
    }

    return( TRUE );
}


/*
** Name: cnvtBLOB2GCATuple
**
** Description:
**	Formats BLOB segments into a buffer.  If segment is
**	too long for the buffer, it will be truncated for
**	the current segment and split with the remaining 
**	length returned for further processing.  The segment 
**	length passed in, and the segment length embedded in 
**	the data value are used to determine the actual point 
**	in the current segment of the data when splitting 
**	occurs.  If a segment length of 0 is returned, the
**	next segment should be obtained and its length used
**	in the subsequent call to this routine.  Otherwise,
**	the current segment should not be changed and the
**	value returned used in the subsequent call (once
**	the output buffer has been flushed).
**
**	The BLOB header is formatted if first_segment is TRUE.
**	A data segment is formatted if segment length is not
**	zero.  Note that the segment length is not the byte
**	length of the data, but the number of characters that
**	are available (for UCS2 encoding).  
**
**	The end-of-segments indicator is formatted if 
**	last_segment is TRUE.  The caller must handle the 
**	case where last_segment was TRUE on entry and TRUE 
**	is returned indicating the segment was split.  In 
**	this case the end-of-segments indicator was not 
**	processed (even if the remaining segment length is 
**	zero) and this routine should be called again with 
**	last_segment TRUE.
**
** Input:
**	descriptor	API data descriptor.
**	dataValue	API data value.
**	segment_length	Length remaining in current segment (chars).
**	first_segment	TRUE if this is first segment of BLOB.
**	last_segment	TRUE if this is last segment of BLOB.
**	buffer		Result buffer.
**	buffer_length	Length of result buffer (bytes).
**
** Output:
**	segment_length	Length remaining in current segment.
**	buffer_length	Length of formatted segment.
**
** Returns:
**	II_BOOL		TRUE if segment was split.
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.
**	 3-Apr-96 (gordy)
**	    BLOB segments may now be sent using the formatted GCA interface.
**	25-Oct-06 (gordy)
**	    Extracted LOB formatting to separate functions for re-use.
*/

static II_BOOL
cnvtBLOB2GCATuple
(
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_DATAVALUE	*dataValue,
    u_i2		*segment_length,
    II_BOOL		first_segment,
    II_BOOL		last_segment,
    char		*buffer,
    II_LONG		*buffer_length
)
{
    u_i2	hdr_len = 0;
    u_i2 	seg_len = 0;
    u_i2	end_len = 0;
    u_i2	char_size = IIapi_isUCS2( descriptor->ds_dataType ) ? 2 : 1;
    II_LONG	used = 0;
    II_LONG	blen = *buffer_length;
    II_BOOL	split = FALSE;

    /*
    ** Calculate the minimum space required.
    ** First segment requires a LOB header.
    ** A data segment requires at least one 
    ** character of data.  The last segment 
    ** is followed by an end-of-segments 
    ** indicator and possibly a NULL indicator.
    */
    if ( first_segment )  hdr_len = formatBLOBhdr( 0, NULL );
    if ( *segment_length )  seg_len = formatBLOBseg( 1, char_size, NULL, NULL );
    if ( last_segment )  
	end_len =formatBLOBend( NULL ) + (descriptor->ds_nullable ? 1 : 0);

    /*
    ** Make sure there is room for everything requested.
    */
    if ( (hdr_len + seg_len + end_len) > blen )
    {
	*buffer_length = 0;
	return( FALSE );
    }

    if ( first_segment )
    {
	u_i2 len = formatBLOBhdr( ADP_P_GCA_L_UNK, (u_i1 *)buffer );
	buffer += len;
	blen -= len;
	used += len;
    }

    if ( *segment_length )
    {
	u_i2	len;
	u_i1	*ptr;

	/*
	** Determine how much of the current segment
	** will fit in the current buffer allowing
	** room for the trailing end-of-segments
	** indicator if requested.  If the current
	** segment won't fit, we fill the buffer
	** with what will fit.
	*/
	len = formatBLOBseg( *segment_length, char_size, NULL, NULL );

	if ( len <= (blen - end_len) )
	    seg_len = *segment_length;
	else
	{
	    /*
	    ** Determine how much of the data segment will
	    ** fit in buffer allowing space for segment
	    ** overhead.
	    */
	    II_LONG byte_length = *segment_length * char_size;
	    seg_len = (blen - (len - byte_length)) / char_size;

	    /*
	    ** It's possible that the entire segment will fit
	    ** when the end-of-segments indicator is not
	    ** included.  We send the entire segment, but
	    ** still indicate a split segment condition
	    ** so that the end-of-segments indicator can
	    ** be included in the subsequent buffer.
	    */
	    seg_len = min( seg_len, *segment_length );
	    split = TRUE;

	    IIAPI_TRACE( IIAPI_TR_VERBOSE )
		    ( "cnvtBLOB2GCATuple: splitting segment (%d, %d)\n",
		      (i4)seg_len, (i4)(*segment_length - seg_len) );
	}

	/*
	** Segment Data.  The location of the data
	** depends on what we may have already taken
	** from the current segment.  The difference
	** between the passed in segment length and
	** the embedded segment length indicates what
	** has already been processed.
	*/
	MEcopy( (PTR)dataValue->dv_value, sizeof( hdr_len ), (PTR)&len );
	ptr = (u_i1 *)dataValue->dv_value + sizeof( len ) + 
	      (len - *segment_length) * char_size;

	len = formatBLOBseg( seg_len, char_size, ptr, buffer );
	buffer += len;
	used += len;
	*segment_length -= seg_len;
    }

    /*
    ** Don't process end-of-segments indicator
    ** if the segment was split (since the rest
    ** of the segment is sent as an additional
    ** segment).
    */
    if ( last_segment  &&  ! split )
    {
	/*
	** End-of-segments indicator.
	*/
	u_i2 len = formatBLOBend( (u_i1 *)buffer );
	buffer += len;
	used += len;

	/*
	** Null byte.
	*/
	if ( descriptor->ds_nullable )
	{
	    if ( dataValue->dv_null )
		IIAPI_SETNULL( 1, buffer );
	    else
		IIAPI_SETNOTNULL( 1, buffer );

	    buffer++;
	    used++;
	}
    }

    *buffer_length = used;
    return( split );
}



/*
** Name: formatBLOBhdr
**
** Description:
**	Builds a GCA LOB data value header.  Buffer (if provided)
**	is assumed to be large enough to hold output.  Space
**	required for header may be determined by setting buffer
**	to NULL.
**
** Input:
**	tag	ADF PERIPHERAL tag.
**	buffer	Output buffer (may be NULL).
**
** Output:
**	None.
**
** Returns:
**	u_i2	Size of header.
**
** History:
**	25-Oct-06 (gordy)
**	    Created.
*/

static u_i2
formatBLOBhdr( i4 tag, u_i1 *buffer )
{
    if ( buffer )
    {
	ADP_PERIPHERAL adp;

	adp.per_tag = tag;
	adp.per_length0 = 0;
	adp.per_length1 = 0;
	MEcopy( (PTR)&adp, ADP_HDR_SIZE, (PTR)buffer );
    }

    return( ADP_HDR_SIZE );
}



/*
** Name: formatBLOBseg
**
** Description:
**	Builds a GCA LOB data segment.  Buffer (if provided)
**	is assumed to be large enough to hold output.  Space
**	required for segment may be determined by setting buffer
**	to NULL.
**
** Input:
**	seg_len		Number of characters.
**	char_size	Size of characters.
**	data		Input buffer.
**	buffer		Output buffer (may be NULL).
**
** Output:
**	None.
**
** Returns:
**	u_i2		Size of segment.
**
** History:
**	25-Oct-06 (gordy)
**	    Created.
*/

static u_i2
formatBLOBseg( u_i2 seg_len, u_i2 char_size, u_i1 *data, u_i1 *buffer )
{
    u_i4 indicator = 1;
    u_i4 byte_len = seg_len * char_size;

    if ( buffer )
    {
    	MEcopy( (PTR)&indicator, sizeof( indicator ), (PTR)buffer );
	buffer += sizeof( indicator );
    	MEcopy( (PTR)&seg_len, sizeof( seg_len ), (PTR)buffer );
    	buffer += sizeof( seg_len );
    	MEcopy( (PTR)data, byte_len, (PTR)buffer );
    }

    return( sizeof( indicator ) + sizeof( seg_len ) + byte_len );
}



/*
** Name: formatBLOBend
**
** Description:
**	Builds a GCA LOB end-of-segments indicator.  Buffer 
**	if provided) is assumed to be large enough to hold 
**	output.  Space required for end-of-segments may be 
**	determined by setting buffer to NULL.
**
** Input:
**	buffer		Output buffer (may be NULL).
**
** Output:
**	None.
**
** Returns:
**	u_i2		Size of end-of-segments.
**
** History:
**	25-Oct-06 (gordy)
**	    Created.
*/

static u_i2
formatBLOBend( u_i1 *buffer )
{
    u_i4 indicator = 0;

    if ( buffer )  MEcopy( (PTR)&indicator, sizeof( indicator ), (PTR)buffer );
    return( sizeof( indicator ) );
}


