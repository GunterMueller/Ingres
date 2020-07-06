/*
** Copyright (c) 2004, 2007 Ingres Corporation
*/

/*
** Name: apigcap.h
**
** Description:
**	This file contains the definition of the GCA parameter
**      management functions.
**
** History:
**      10-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	14-Nov-94 (gordy)
**	    Cleaned up processing of GCA_MD_ASSOC messages.
**	21-Nov-94 (gordy)
**	    Rename IIapi_createGCAQueryWithParm().
**	28-Jul-95 (gordy)
**	    Use fixed length types.  Removed unused structure definition.
**	21-Dec-95 (gordy)
**	    Added IIapi_createGCAEmpty().
**	22-Apr-96 (gordy)
**	    Finished support for COPY FROM.
**	 9-Jul-96 (gordy)
**	    Added support for autocommit transactions.
**	 9-Sep-96 (gordy)
**	    Added msgType parameter to IIapi_createGCAFetch to support
**	    GCA1_FETCH messages.
**	 6-Oct-98 (rajus01)
**	    Added IIapi_createGCAPrreply(). 
**	11-Jul-06 (gordy)
**	    Added IIapi_createGCAxa().
**	15-Mar-07 (gordy)
**	    Support for scrollable cursors.
*/

# ifndef __APIGCAP_H__
# define __APIGCAP_H__

# include <apichndl.h>
# include <apithndl.h>
# include <apishndl.h>
# include <apilower.h>


/*
** Global Functions.
*/

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAEmpty( IIAPI_HNDL *handle );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAAttention( IIAPI_HNDL *handle );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAClose( IIAPI_STMTHNDL *stmtHndl );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAFetch( II_INT2 prefetchrows,
					        IIAPI_STMTHNDL *stmtHndl );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAScroll( IIAPI_STMTHNDL *stmtHndl,
						 II_UINT2 reference,
						 II_INT4 offset, 
						 II_INT2 rowCount );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAParm( IIAPI_STMTHNDL *stmtHndl,
					       IIAPI_PUTPARMPARM *putParmParm );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAError( IIAPI_HNDL *handle );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAMdassoc( IIAPI_CONNHNDL *connHndl );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAResponse( IIAPI_STMTHNDL *stmtHndl );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAAck( IIAPI_STMTHNDL *stmtHndl );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAQuery( IIAPI_PARMNMEM *parmNmem,
					        IIAPI_QUERYTYPE queryType,
					        char *queryText,
						II_BOOL allow_xact,
					        IIAPI_HNDL *handle,
					        II_INT2 parmCount );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCASecure( IIAPI_TRANHNDL *tranHndl );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCATuple( IIAPI_STMTHNDL *stmtHndl,
						IIAPI_PUTCOLPARM *putColParm );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAPrreply( IIAPI_CONNHNDL *connHndl );

II_EXTERN IIAPI_PARMNMEM *IIapi_createGCAxa( II_LONG msg_type,
					     IIAPI_XA_DIS_TRAN_ID *xa_xid,
					     II_ULONG xa_flags );

# endif /* __APIGCAP_H__ */
