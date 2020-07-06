/*
** Copyright (c) 2004 Ingres Corporation
*/

/*
** Name: apidatav.h
**
** Description:
**	This file contains the definition of the GCA/API data 
**	and descriptor management functions.
**
** History:
**      04-may-04 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 7-Oct-94 (gordy)
**	    Fixed handling of variable length types.
**	11-Nov-94 (gordy)
**	    Include length indicator with variable length data.
**	13-Jun-95 (gordy)
**	    Added IIapi_loadColumns(), IIapi_cnvtBLOB2DataValue()
**	    and IIapi_checkBLOBSegment().
**	20-Jun-95 (gordy)
**	    Added routines to support sending of BLOBs.
**	28-Jul-95 (gordy)
**	    Use fixed length types.
**	14-Sep-95 (gordy)
**	    GCD_GCA_ prefix changed to GCD_.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.  Added routines
**	    to handle compressed varchars.
**	27-Mar-96 (gordy)
**	    Compressed varchar routines made local.
**	 2-Oct-96 (gordy)
**	    Move IIAPI_MAX_SEGMENT_LEN here from api.h.
**      21-Aug-98 (rajus01)
**          Added environment handle to IIapi_cnvtGDVDescr().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	25-Oct-06 (gordy)
**	    Added environment handle to IIapi_getAPILength().
*/

# ifndef __APIDATAV_H__
# define __APIDATAV_H__

# include <apienhnd.h>
# include <apishndl.h>
# include <apilower.h>


/*
** Max length of a long datatype segment.  Currently
** the DBMS will only send a segment of DB_MAXTUP
** length.
*/

# define IIAPI_MAX_SEGMENT_LEN	DB_MAXTUP


/*
** Global functions.
*/

II_EXTERN II_UINT2	IIapi_getAPILength( IIAPI_ENVHNDL *, 
					    IIAPI_DT_ID, II_LONG, char * );

II_EXTERN II_UINT2	IIapi_getGCALength( IIAPI_DESCRIPTOR *descriptor );

II_EXTERN II_UINT2	IIapi_getBLOBLength( IIAPI_DESCRIPTOR *descriptor,
					     IIAPI_DATAVALUE *dataValue );

II_EXTERN II_VOID	IIapi_cnvtDescr2GDV( IIAPI_DESCRIPTOR *descriptor,
					     GCD_DATA_VALUE *gcaDataValue );

II_EXTERN II_BOOL	IIapi_cnvtDataValue2GDV( II_UINT2 memTag,
						 IIAPI_DESCRIPTOR *descriptor,
						 IIAPI_DATAVALUE *dataValue,
						 GCD_DATA_VALUE *gcaDataValue );

II_EXTERN II_VOID	IIapi_cnvtDataValue2GCATuple( IIAPI_DESCRIPTOR *descr,
						      IIAPI_DATAVALUE *data,
						      II_PTR gcaTuple );

II_EXTERN II_BOOL	IIapi_cnvtBLOB2GCATuple( IIAPI_STMTHNDL *stmtHndl,
						 IIAPI_DESCRIPTOR *descriptor,
						 IIAPI_DATAVALUE *dataValue,
						 II_BOOL moreSegments,
						 char *buffer,
						 II_LONG *buffer_length );

II_EXTERN II_BOOL	IIapi_cnvtGDV2Descr( IIAPI_ENVHNDL  *envHndl, 
					     IIAPI_DT_ID gcaDataType,
					     II_LONG gcaLength,
					     i4  gcaPrecision,
					     II_INT2 columnNameLength,
					     char *columnName,
					     IIAPI_DESCRIPTOR *descriptor );

II_EXTERN II_VOID	IIapi_cnvtGDV2DataValue( IIAPI_DESCRIPTOR *descriptor,
						 char *gcaValue,
						 IIAPI_DATAVALUE *dataValue );

II_EXTERN II_VOID	IIapi_loadColumns( IIAPI_STMTHNDL *stmtHndl,
					   IIAPI_GETCOLPARM *getColParm,
					   IIAPI_GCA_BUFF *rcvParm );

II_EXTERN II_VOID	IIapi_checkBLOBSegment( IIAPI_STMTHNDL *stmtHndl,
						IIAPI_DESCRIPTOR *descriptor,
						IIAPI_GCA_BUFF *gcaBuff );

# endif /* __APIDATAV_H__ */

