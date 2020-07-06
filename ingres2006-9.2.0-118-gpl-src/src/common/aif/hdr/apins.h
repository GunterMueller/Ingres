/*
** Copyright (c) 2004 Ingres Corporation
*/

/*
** Name: apins.h
**
** Description:
**	Name Server State Machine interfaces.
**
** History:
**	12-Feb-97 (gordy)
**	    Created.
**	25-Mar-97 (gordy)
**	    Added macros for handling changes in Name Server protocol.
**	 3-Jul-97 (gordy)
**	    State machine interface is now through initialization
**	    function which sets the dispatch table entries.
**	22-Oct-04 (gordy)
**	    Return error code from IIapi_validNSDescriptor().
*/

# ifndef __APINS_H__
# define __APINS_H__

# include <apishndl.h>

/*
** Maximum length of a GCN object.
*/
#define API_NS_MAX_LEN		64


/*
** Currently, GCN limits its buffer size
** to attempt to avoid splitting buffers.
*/
# define API_GCN_BUFF_SIZE	1024

/*
** Macros for handling Name Server protocol changes.
**
** In the original Name Server protocol end-of-group
** was indicated by the end-of-data flag and there was
** no way to indicate message continuation (end-of-data
** always assumed to be TRUE.
**
** In the new Name Server protocol end-of-group and
** end-of-data are handled according to GCA standards.
*/
#define NS_EOG( ch, rp )        \
		    ( ((ch)->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_63) \
		      ? ((rp)->gca_end_of_data) \
		      : ((rp)->gca_end_of_group  &&  (rp)->gca_end_of_data) )

#define NS_EOD( ch, rp )        \
		    ( ((ch)->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_63) \
		      ? (TRUE) \
		      : ((rp)->gca_end_of_data) )


/*
** Global functions.
*/

II_EXTERN IIAPI_STATUS		IIapi_ns_init( VOID );
II_EXTERN IIAPI_STATUS		IIapi_ns_cinit( VOID );
II_EXTERN IIAPI_STATUS		IIapi_ns_tinit( VOID );
II_EXTERN IIAPI_STATUS		IIapi_ns_sinit( VOID );

II_EXTERN IIAPI_PARMNMEM	*IIapi_createGCNOper( IIAPI_STMTHNDL * );

II_EXTERN IIAPI_STATUS		IIapi_parseNSQuery(IIAPI_STMTHNDL *, STATUS *);

II_EXTERN II_ULONG		IIapi_validNSDescriptor( IIAPI_STMTHNDL *,
							 II_LONG,
							 IIAPI_DESCRIPTOR * );

II_EXTERN II_BOOL		IIapi_validNSParams( IIAPI_STMTHNDL *,
						     IIAPI_PUTPARMPARM * );

II_EXTERN II_VOID		IIapi_saveNSParams( IIAPI_STMTHNDL *,
						    IIAPI_PUTPARMPARM * );

II_EXTERN IIAPI_STATUS		IIapi_getNSDescriptor( IIAPI_STMTHNDL * );

II_EXTERN II_VOID		IIapi_loadNSColumns( IIAPI_STMTHNDL *,
						     IIAPI_GETCOLPARM *,
						     GCD_GCN_REQ_TUPLE * );

# endif /* __APINS_H__ */
