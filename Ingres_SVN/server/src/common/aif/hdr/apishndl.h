/*
** Copyright (c) 2004, 2007 Ingres Corporation
*/

/*
** Name: apishndl.h
**
** Description:
**	This file contains the definition of the statement handle.
**
** History:
**      14-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	31-Oct-94 (gordy)
**	    Cleaned up cursor processing.
**	10-Nov-94 (gordy)
**	    Added sh_flags to track statement specific info.
**	20-Dec-94 (gordy)
**	    Cleaned up common handle processing.
**	11-Jan-95 (gordy)
**	    Cleaned up descriptor handling.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	16-Feb-95 (gordy)
**	    Save tuple length in statement handle.
**	13-Jun-95 (gordy)
**	    Implemented GCA receive buffering.
**	20-Jun-95 (gordy)
**	    Added flag for cursor rather than testing cursor handle.
**	    Cursor update will need to save the cursor handle, but
**	    shouldn't be considered the parent of the cursor.  Added
**	    sh_parmSend for send buffering.
**	28-Jul-95 (gordy)
**	    Used fixed length types.
**	23-Feb-96 (gordy)
**	    Replaced copy info fields with copy map.  Removed unused
**	    tuple length field.
**	15-Mar-96 (gordy)
**	    Added compressed varchar flag, IIAPI_SH_COMP_VARCHAR.
**	 3-Apr-96 (gordy)
**	    Added sh_segmentLength for BLOB processing.
**	 2-Oct-96 (gordy)
**	    Simplified function callbacks.
**	31-Jan-97 (gordy)
**	    Added het-net descriptor to statement handle.
**	25-Oct-06 (gordy)
**	    Combined external query flags and statement flags.
**	    Added IIAPI_SH_PARAMETERS to replace qy_parameters.
**	15-Mar-07 (gordy)
**	    Support for scrollable cursors.
*/

# ifndef __APISHNDL_H__
# define __APISHNDL_H__

# include <apithndl.h>
# include <apiihndl.h>
# include <apilower.h>


/*
** Name: IIAPI_STMTHNDL
**
** Description:
**      This data type defines the statement handle
**
** History:
**      14-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	31-Oct-94 (gordy)
**	    Cleaned up cursor processing.
**	10-Nov-94 (gordy)
**	    Added sh_flags to track statement specific info.
**	20-Dec-94 (gordy)
**	    Cleaned up common handle processing.
**	11-Jan-95 (gordy)
**	    Cleaned up descriptor handling.
**	16-Feb-95 (gordy)
**	    Save tuple length in statement handle.
**	13-Jun-95 (gordy)
**	    Added sh_gcaBuff and sh_colFetch for receive buffering.
**	    Added flags for long data types.
**	20-Jun-95 (gordy)
**	    Added flag for cursor rather than testing cursor handle.
**	    Cursor update will need to save the cursor handle, but
**	    shouldn't be considered the parent of the cursor.  Added
**	    sh_parmSend for send buffering.
**	28-Jul-95 (gordy)
**	    Used fixed length types.
**	23-Feb-96 (gordy)
**	    Replaced copy info fields with copy map.  Removed unused
**	    tuple length field.
**	 2-Oct-96 (gordy)
**	    Simplified function callbacks.
**	31-Jan-97 (gordy)
**	    Added het-net descriptor to statement handle.
**	 3-Sep-98 (gordy)
**	    Added function to abort handle.
**	25-Oct-06 (gordy)
**	    Combined external query flags and statement flags.
**	    Added IIAPI_SH_PARAMETERS to replace qy_parameters.
**	15-Mar-07 (gordy)
**	    Added sh_rowCount to track rows requested by position/scroll.
*/

typedef struct IIAPI_STMTHNDL
{
    
    IIAPI_HNDL		sh_header;
    IIAPI_TRANHNDL	*sh_tranHndl;		/* parent */
    
    /*
    ** State table predicates
    */
    IIAPI_QUERYTYPE	sh_queryType;
    IIAPI_IDHNDL	*sh_cursorHndl;
    
    /*
    ** Query flags from IIAPI_QUERYPARM.qy_flags
    **
    ** Internal flags must not conflict with those
    ** defined in iiapi.h 
    */
    II_ULONG		sh_flags;

#define IIAPI_SH_PARAMETERS		0x01000000
#define IIAPI_SH_CURSOR			0x02000000
#define IIAPI_SH_COMP_VARCHAR		0x04000000
#define IIAPI_SH_LOST_TUPLES		0x08000000
#define IIAPI_SH_LOST_SEGMENTS		0x10000000
#define IIAPI_SH_PURGE_SEGMENTS		0x20000000
#define IIAPI_SH_MORE_SEGMENTS		0x40000000

    char		*sh_queryText;
    II_INT2		sh_parmCount;		/* # of sh_parmDescriptor's */
    II_INT2		sh_parmIndex;		/* Next parameter to send */
    II_INT2		sh_parmSend;		/* Number of parms requested */
    IIAPI_DESCRIPTOR	*sh_parmDescriptor;	/* Parameter Descriptor */
    II_INT2		sh_rowCount;		/* # of rows requested */
    II_INT2		sh_colCount;		/* # of sh_descriptor's */
    II_INT2		sh_colIndex;		/* Next column to fetch */
    II_INT2		sh_colFetch;		/* # of columns requested */
    IIAPI_DESCRIPTOR	*sh_colDescriptor;	/* Column Descriptor */
    PTR			sh_cpyDescriptor;	/* GCA copy descriptor */
    IIAPI_COPYMAP	sh_copyMap;		/* Copy to/from map */
    IIAPI_GETQINFOPARM	sh_responseData;	/* statement status info */
    IIAPI_GCA_BUFF	*sh_gcaBuff;		/* GCA buffer for tuples */
    II_LONG		sh_segmentLength;	/* BLOB segment processing */
    
    /*
    ** API Function call info.
    */
    II_BOOL		sh_callback;
    IIAPI_GENPARM	*sh_parm;
    II_BOOL		sh_cancelled;
    IIAPI_GENPARM	*sh_cancelParm;

} IIAPI_STMTHNDL;


/*
** Global functions.
*/

II_EXTERN IIAPI_STMTHNDL	*IIapi_createStmtHndl( IIAPI_TRANHNDL *tranHndl,
						       IIAPI_QUERYPARM *parm );

II_EXTERN II_VOID		IIapi_deleteStmtHndl(IIAPI_STMTHNDL *stmtHndl);

II_EXTERN II_VOID		IIapi_abortStmtHndl( IIAPI_STMTHNDL *stmtHndl,
						     II_LONG errorCode,
						     char *SQLState,
						     IIAPI_STATUS status );

II_EXTERN II_BOOL		IIapi_isStmtHndl( IIAPI_STMTHNDL *stmtHndl );

II_EXTERN IIAPI_STMTHNDL	*IIapi_getStmtHndl( IIAPI_HNDL *handle );

# endif /* __APISHNDL_H__ */
