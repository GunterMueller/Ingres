/*
** Copyright (c) 2004, 2007 Ingres Corporation
*/

# include <compat.h>
# include <gl.h>
# include <me.h>
# include <bt.h>
# include <cm.h>
# include <cv.h>
# include <me.h>
# include <pc.h>
# include <st.h>
# include <tm.h>
# include <tmtz.h>

# include <iicommon.h>
# include <adf.h>

# include <iiapi.h>
# include <api.h>
# include <apichndl.h>
# include <apithndl.h>
# include <apishndl.h>
# include <apiihndl.h>
# include <apitname.h>
# include <apiadf.h>
# include <apidatav.h>
# include <apigcap.h>
# include <apimisc.h>
# include <apitrace.h>

/*
** Name: apigcap.c
**
** Description:
**	This file contains functions which create and delete
**	GCA event parameters.
**
**	IIapi_createGCAAck       create GCA_ACK parameters
**	IIapi_createGCAAttention create GCA_ATTENTION parameters
**	IIapi_createGCAClose     create GCA_CLOSE parameters
**	IIapi_createGCAError     create GCA_ERROR parameters
**	IIapi_createGCAFetch     create GCA_FETCH/GCA1_FETCH parameters
**	IIapi_createGCAScroll    create GCA2_FETCH parameters
**	IIapi_createGCAMdassoc   create GCA_MD_ASSOC parameters
**	IIapi_createGCAQuery     create GCA_QUERY parameters
**	IIapi_createGCAParm	 create GCA_QUERY parameters
**	IIapi_createGCAResponse  create GCA_RESPONSE parameters
**	IIapi_createGCASecure    create GCA_SECURE parameters
**	IIapi_createGCAxa	 create GCA_XA_DATA parameters
**	IIapi_createGCATuple     create GCA_TUPLE parameters
**	IIapi_createGCAPrreply   create GCA_PRREPLY parameters
**
** History:
**      15-oct-93 (ctham)
**          creation
**	 11-apr-94 (edg)
**	    Target name in MD_ASSOC message should have any vnode info
**	    stripped out.
**      11-apr-94 (ctham)
**          all string parameters in GCA_MD_ASSOC should have the
**          data types of IIAPI_CHA_TYPE instead of IIAPI_CHR_TYPE.
**      18-apr-94 (ctham)
**          GCA_RUPASS is not accepted in GCA_MD_ASSOC when
**          the protocol level is less than GCA_PROTOCOL_LEVEL_60,
**          corrected in IIapi_createGCAMdassoc().
**      26-apr-94 (ctham)
**          The size of stmtHndl->sh_cursorHndl was incorrectly set
**          in IIapi_createGCAClose() and IIapi_createGCAFetch().  They
**          are now corrected.
**      26-apr-94 (ctham)
**          remove GCA_RUPASS completely from API.
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	 31-may-94 (ctham)
**	     cleaned up handling of GCA names in createDelete().
**	21-Nov-94 (gordy)
**	    Allow for zero length owner name in createDelete().
**	    Renamed IIapi_createGCAQueryWithParm().
**	22-Nov-94 (gordy)
**	    Fix createUpdate handling of cursor IDs.
**	19-Dec-94 (gordy)
**	    Ensure the proper number of procedure parameters are sent.
**	20-Dec-94 (gordy)
**	    Clean up common handle processing.
**	22-Dec-94 (gordy)
**	    Added support for DEFINE repeat query.
**	10-Jan-95 (gordy)
**	    Allow for procedure handle instead of procedure name when
**	    invoking a procedure.
**	11-Jan-95 (gordy)
**	    Cleaning up descriptor handling.
**	17-Jan-95 (gordy)
**	    Conditionalized code for older GCA interface.
**	19-Jan-95 (gordy)
**	    Conditionalized code for older timezone interface.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	13-Feb-95 (gordy)
**	    Disallow transaction statements in queries.
**	16-Feb-95 (gordy)
**	    API now adds query text for GCA dependent forms
**	    utilizing ~Q and ~V.
**	 9-Mar-95 (gordy)
**	    Silence compiler warnings.
**	13-Mar-95 (gordy)
**	    Do not append USING clause for non-dynamic open.
**	10-May-95 (gordy)
**	    Make sure the parameter count is correct when some
**	    parms are not sent due to protocol restrictions.
**	19-May-95 (gordy)
**	    Fixed include statements.
**	14-Jun-95 (gordy)
**	    Connection transaction ID parameters now sent as formatted 
**	    strings.  Updated IIapi_createGCASecure() to new GCD 
**	    structure definitions.
**	20-Jun-95 (gordy)
**	    Added support for sending BLOBs as parameters.
**	11-Jul-95 (gordy)
**	    Fixed problem where putParmParm is being referenced
**	    for pp_moreSegments, but putParmParm is NULL.
**	13-Jul-95 (gordy)
**	    Added missing header file, casts for type correctness.
**	28-Jul-95 (gordy)
**	    Used fixed length types.  Fixed tracing.
**	14-Sep-95 (gordy)
**	    GCD_GCA_ prefix changed to GCD_.
**	21-Dec-95 (gordy)
**	    Added IIapi_createGCAEmpty().  Cleaned up parmNmem tracing.
**	17-Jan-96 (gordy)
**	    Added environment handles.  Added global data structure.
**	 8-Mar-96 (gordy)
**	    GCD_SESSION_PARMS is now a variable length data structure
**	    rather than containing a pointer to a variable length array.
**	15-Mar-96 (gordy)
**	    Added support for compressed varchars.
**	 3-Apr-96 (gordy)
**	    BLOBs may now be sent using the formatted GCA interface.
**	 8-Apr-96 (gordy)
**	    Converted gca_e_element from pointer to variable length
**	    array.  Since we don't send and error message, only send
**	    the element count.
**	22-Apr-96 (gordy)
**	    Finished support for COPY FROM.
**	15-May-96 (gordy)
**	    Use a static location to hold timezone offset when connecting
**	    at a GCA protocol level which doesn't support timezone names.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
**	 9-Jul-96 (gordy)
**	    Added support for autocommit transactions.
**	 9-Sep-96 (gordy)
**	    Added support for GCA_RUPASS, GCA_YEAR_CUTOFF, and GCA1_FETCH.
**	 2-Oct-96 (gordy)
**	    Replaced original SQL state machines which a generic
**	    interface to facilitate additional connection types.
**	    IIAPI_PARMNMEM expanded to hold additional message info.
**	11-Nov-96 (gordy)
**	    Check for successful tag allocation.
**	18-Feb-97 (gordy)
**	    Moved IIapi_createParmNMem() to apimisc.c
**	29-May-97 (gordy)
**	    Split large query texts into multiple GCA_DATA_VALUEs.
**	15-Aug-98 (rajus01)
**	    Added support for security labels..
**	14-Oct-98 (rajus01)
**	    Added support for GCA_PROMPT. Added IIapi_createGCAPrreply().
**	21-may-1999 (hanch04)
**	    Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**	16-Dec-99 (gordy)
**	    Make sure param count handles all conditional parameters.
**      23-May-00 (loera01) Bug 101463
**          Added support for passing global temporary tables argument
**          into a DB procedure (GCA1_INVPROC message).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-Nov-1999 (hanch04)
**          First stage of updating ME calls to use OS memory management.
**          Make sure me.h is included.  Use SIZE_TYPE for lengths.
**	 1-Mar-01 (gordy)
**	    Embedded length in IIAPI_NVCH_TYPE is number of characters,
**	    which must be converted to bytes to determine actual length.
**	23-Mar-01 (gordy)
**	    Need to use timezone selected by application when sending
**	    timezone offset to 6.4 DBMS.
**      28-May-03 loera01 Bug 110318
**          Send the decimal connection parameter only if it's a comma.
**	15-Mar-04 (gordy)
**	    Added connection parameter for I8 width.
**	 6-Oct-04 (gordy)
**	    Use GCA suggested buffer size for message buffer.
**	28-Jun-06 (gordy)
**	    Support procedure OUT parameters (backward support as BYREF).
**	11-Jul-06 (gordy)
**	    Created IIapi_createGCAxa().
**	14-Jul-2006 (kschendel)
**	    Remove u_i2 casts from MEcopy calls.
**	25-Oct-06 (gordy)
**	    Allow locators when requested.
**	15-Mar-07 (gordy)
**	    GCA clean-up.
**	15-Mar-07 (gordy)
**	    Support for scrollable cursors.
**	 8-Aug-07 (gordy)
**	    Added connection parameter for date alias.
*/


/*
** Local variables
**
** A zero length VARCHAR string used as GCA_MD_ASSOC parameter.
*/
static struct { char len1; char len2; char data[1]; } zero_string = { 0, 0, 0 };

/*
** Query text fragments to be added to SQL statements.
*/
static char	*api_qt_repeat = "define query ~Q is ";
static char	*api_qt_where = " where current of ~Q ";
static char	*api_qt_open = "open ~Q cursor for ";
static char	*api_qt_scroll = "open ~Q scroll cursor for ";
static char	*api_qt_using = " using ~V ";
static char	*api_qt_marker = ", ~V ";

#define API_USING_CLAUSE_LENGTH( cnt )	(((cnt) <= 0) ? 0 \
: STlength( api_qt_using ) + (((cnt) - 1) * STlength( api_qt_marker )))


/*
** Buffer space used by a GCA_ID formatted as GCA_DATA_VALUEs.
*/
#define API_GCA_ID_SIZE		(sizeof(GCA_ID) + (3 * sizeof(GCA_DATA_VALUE)))



/*
** Local functions
*/
static II_VOID		put_GCA_int_parm( i4			gca_index, 
					  i4			data_size, 
		  			  PTR			data_value, 
					  GCD_USER_DATA		*gca_data );

static II_VOID		put_GCA_string_parm( i4		gca_index, 
					     char		*data_value,
		     			     GCD_USER_DATA	*gca_data );

static II_BOOL		createQuery( IIAPI_STMTHNDL	*stmtHndl,
				     IIAPI_PUTPARMPARM	*putParmParm,
				     IIAPI_PARMNMEM	*parmNmem );

static II_BOOL		createOpen( IIAPI_STMTHNDL	*stmtHndl,
				    IIAPI_PUTPARMPARM	*putParmParm,
				    IIAPI_PARMNMEM	*parmNmem );

static II_BOOL 		createDelete( IIAPI_STMTHNDL	*stmtHndl,
				      IIAPI_PUTPARMPARM	*putParmParm,
				      IIAPI_PARMNMEM	*parmNmem );

static II_BOOL		createUpdate( IIAPI_STMTHNDL	*stmtHndl,
				      IIAPI_PUTPARMPARM	*putParmParm,
				      IIAPI_PARMNMEM	*parmNmem );

static II_BOOL		createExecProc( IIAPI_STMTHNDL		*stmtHndl,
					IIAPI_PUTPARMPARM	*putParmParm,
					IIAPI_PARMNMEM		*parmNmem );

static II_BOOL		createDefRepeatQuery( IIAPI_STMTHNDL	*stmtHndl,
					      IIAPI_PUTPARMPARM	*putParmParm,
					      IIAPI_PARMNMEM	*parmNmem );

static II_BOOL		createExecRepeatQuery(IIAPI_STMTHNDL	*stmtHndl,
					      IIAPI_PUTPARMPARM	*putParmParm,
					      IIAPI_PARMNMEM	*parmNmem );

static II_BOOL		dynamic_open( char *queryText );

static II_BOOL		append_where_clause( IIAPI_STMTHNDL	*stmtHndl );

static II_BOOL		append_using_clause( IIAPI_STMTHNDL	*stmtHndl );

static II_INT2		calcProcParamCount( II_INT2 		parmCount, 
					    IIAPI_DESCRIPTOR 	*descriptor, 
					    IIAPI_DATAVALUE	*dataValue,
					    II_BOOL		moreSegments,
					    II_LONG		buff_max,
					    II_LONG		reserve,
					    II_BOOL		force );

static II_INT2		calcDataValueCount( II_INT2		parmCount, 
					    IIAPI_DESCRIPTOR	*descriptor,
					    IIAPI_DATAVALUE	*dataValue,
					    II_BOOL		moreSegments, 
					    II_LONG		buff_max,
					    II_LONG 		reserve,
					    II_BOOL		force );

static GCD_DATA_VALUE	*allocDataValues( IIAPI_PARMNMEM	*parmNmem,
					  II_INT2		parmCount );

static II_INT2		calcQueryCount( IIAPI_STMTHNDL 		*stmtHndl,
					II_INT2 		parmCount,
					IIAPI_DESCRIPTOR 	*descriptor,
					IIAPI_DATAVALUE		*dataValue,
					II_BOOL			moreSegments,
					II_LONG			buff_max,
					II_LONG			reserve );

static GCD_DATA_VALUE	*allocQueryBuffer( IIAPI_STMTHNDL	*stmtHndl,
					   IIAPI_PARMNMEM	*parmNmem,
					   II_INT2		parmCount );

static II_BOOL		copyQueryDataValue( II_UINT2		memTag,
					    GCD_DATA_VALUE	*qDataValue,
					    IIAPI_DATAVALUE	*parmData,
					    IIAPI_DESCRIPTOR	*descriptor,
					    II_LONG		parmCount );

static II_BOOL		copyCursorID( II_UINT2			memTag,
				      GCD_DATA_VALUE		*qDataValue,
				      II_LONG			cursorIndex1,
				      II_LONG			cursorIndex2,
				      II_INT2			NameLength,
				      char			*cursorName );

static II_BOOL		copyBLOBSegment( IIAPI_STMTHNDL		*stmtHndl,
					 IIAPI_PARMNMEM		*parmNmem,
					 IIAPI_DESCRIPTOR	*descriptor,
					 IIAPI_DATAVALUE	*dataValue,
					 II_BOOL		moreSegments );

static i4		get_tz_offset( char *tz_name );


/*
** Name: IIapi_createGCAEmpty
**
** Description:
**	Allocate a GCA parameter block for a GCA message which
**	does not have an associated data object.
**	
** Input:
**	handle			Handle of current query.
**
** Output:
**	None.
**
** Returns:
**	IIAPI_PARMNMEM *	GCA parameter info.
**
** History:
**	21-Dec-95 (gordy)
**	    Created.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAEmpty( IIAPI_HNDL *handle )
{
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAEmpty: creating empty parm\n" );
    
    return( IIapi_createParmNMem() );
}


/*
** Name: IIapi_createGCAAck
**
** Description:
**	This function creates the GCA_ACK parameters.
**
**      stmtHndl   statement handle
**
** Return value:
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAAck( IIAPI_STMTHNDL *stmtHndl )
{
    IIAPI_PARMNMEM	*parmNmem;
    GCD_AK_DATA		*akData;
    STATUS		status;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAAck: creating GCA_ACK parm\n" );
    
    if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	return( NULL );
    
    if ( ! ( akData = (GCD_AK_DATA *)MEreqmem( parmNmem->pm_memTag, 
					       sizeof( GCD_AK_DATA ), 
					       TRUE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCAAck: can't allocate GCA_AK_DATA\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }

    parmNmem->pm_msgType = GCA_ACK;
    parmNmem->pm_parmLen = sizeof(*akData);
    parmNmem->pm_parmBlock = (II_PTR)akData;
    
    return( parmNmem );
}




/*
** Name: IIapi_createGCAAttention
**
** Description:
**	This function creates the GCA_ATTENTION parameters.
**
**      handle    The handle which contains the current memory tag.
**
** Return value:
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAAttention( IIAPI_HNDL *handle )
{
    IIAPI_PARMNMEM	*parmNmem;
    GCD_AT_DATA		*atData;
    STATUS		status;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "createGCAAttention: creating GCA_ATTENTION parm\n" );
    
    if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	return( NULL );
    
    if ( ! ( atData = (GCD_AT_DATA *)MEreqmem( parmNmem->pm_memTag, 
					       sizeof( GCD_AT_DATA ), 
					       FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_crateGCAAttention: can't allocate GCA_AT_DATA\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }

    parmNmem->pm_msgType = GCA_ATTENTION;
    parmNmem->pm_parmLen = sizeof (*atData);
    parmNmem->pm_parmBlock = (II_PTR) atData;
    atData->gca_itype = GCA_INTALL;
    
    return( parmNmem );
}




/*
** Name: IIapi_createGCAClose
**
** Description:
**	This function creates the GCA_CLOSE parameters.
**
**      stmtHndl   The statement handle
**
** Return value:
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      26-apr-94 (ctham)
**          The size of stmtHndl->sh_cursorHndl was incorrectly set
**          in IIapi_createGCAClose() and IIapi_createGCAFetch().  They
**          are now corrected.
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	20-Dec-94 (gordy)
**	    Clean up common handle processing.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAClose( IIAPI_STMTHNDL *stmtHndl )
{
    IIAPI_PARMNMEM	*parmNmem;
    STATUS		status;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAClose: creating GCA_CLOSE parm\n" );
    
    if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	return( NULL );
    
    parmNmem->pm_msgType = GCA_CLOSE;
    parmNmem->pm_parmLen = sizeof( GCD_ID );

    if ( ! ( parmNmem->pm_parmBlock = MEreqmem( parmNmem->pm_memTag,
						parmNmem->pm_parmLen,
						FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCAClose: can't create parmBlock\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }
    
    MEcopy( (II_PTR)&stmtHndl->sh_cursorHndl->id_gca_id,
	    sizeof( GCD_ID ), parmNmem->pm_parmBlock );

    return( parmNmem );
}




/*
** Name: IIapi_createGCAError
**
** Description:
**	This function creates the GCA_ERROR parameters.
**
**      handle    The handle which contains the current memory tag.
**
** Return value:
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	 8-Apr-96 (gordy)
**	    Converted gca_e_element from pointer to variable length
**	    array.  Since we don't send and error message, only send
**	    the element count.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAError( IIAPI_HNDL *handle )
{
    IIAPI_PARMNMEM	*parmNmem = (IIAPI_PARMNMEM *) NULL;
    GCD_ER_DATA		*erData;
    STATUS		status;
    
    IIAPI_TRACE(IIAPI_TR_VERBOSE)
	( "IIapi_createGCAError: creating GCA_ERROR parm\n" );
    
    if ( ! (parmNmem = IIapi_createParmNMem()) )  
	return( NULL );
    
    if ( ! ( erData = (GCD_ER_DATA *)MEreqmem( parmNmem->pm_memTag, 
					       sizeof( GCD_ER_DATA ),
					       FALSE, &status ) ) )
    {
	IIAPI_TRACE(IIAPI_TR_FATAL)
	    ("IIapi_createGCAError: cant' create GCA_ER_DATA\n");
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }
    
    parmNmem->pm_msgType = GCA_ERROR;
    parmNmem->pm_parmLen = (char *)&erData->gca_e_element[0] - (char *)erData;
    parmNmem->pm_parmBlock = (II_PTR)erData;
    
    erData->gca_l_e_element = 0;
    
    return( parmNmem );
}




/*
** Name: IIapi_createGCAFetch
**
** Description:
**	This function creates the GCA_FETCH/GCA1_FETCH parameters.
**
** Input:
**      prefetchRows   number of prefetch rows
**      stmtHndl       statement handle
**
** Returns:
**	IIAPI_PARMNMEM *	A structure containg the parameter 
**				and its memory tag if successful; 
**				otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      26-apr-94 (ctham)
**          The size of stmtHndl->sh_cursorHndl was incorrectly set
**          in IIapi_createGCAClose() and IIapi_createGCAFetch().  They
**          are now corrected.
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	20-Dec-94 (gordy)
**	    Clean up common handle processing.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	28-Jul-95 (gordy)
**	    Use fixed length data types.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
**	 9-Sep-96 (gordy)
**	    Added support for GCA1_FETCH.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAFetch
( 
    II_INT2		prefetchrows, 
    IIAPI_STMTHNDL	*stmtHndl 
)
{
    IIAPI_CONNHNDL	*connHndl;
    IIAPI_PARMNMEM	*parmNmem;
    GCD1_FT_DATA	*ftData;
    STATUS		status;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAFetch: creating GCA_FETCH parm\n" );
    
    if ( ! ( connHndl = IIapi_getConnHndl( (IIAPI_HNDL *)stmtHndl ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "createGCAFetch: can't get connection handle from statement\n" );
	return( NULL );
    }

    if ( ! ( parmNmem = IIapi_createParmNMem() ) )  return( NULL );
    
    if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_62  &&
	 prefetchrows > 1 )
    {
	parmNmem->pm_msgType = GCA1_FETCH;
	parmNmem->pm_parmLen = sizeof( GCD1_FT_DATA );

	if ( ! ( ftData = (GCD1_FT_DATA *)MEreqmem( parmNmem->pm_memTag,
						   parmNmem->pm_parmLen,
						   FALSE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "IIapi_createGCAFetch: can't create GCD1_FT_DATA\n" );
	    IIapi_freeMemTag( parmNmem->pm_memTag );
	    return( NULL );
	}
    
	parmNmem->pm_parmBlock = (II_PTR)ftData;
	ftData->gca_rowcount = (i4)prefetchrows;
	MEcopy( (II_PTR)&stmtHndl->sh_cursorHndl->id_gca_id,
		sizeof( GCD_ID ), (II_PTR)&ftData->gca_cursor_id );
    }
    else
    {
	parmNmem->pm_msgType = GCA_FETCH;
	parmNmem->pm_parmLen = sizeof( GCD_ID );

	if ( ! ( parmNmem->pm_parmBlock = MEreqmem( parmNmem->pm_memTag,
						    parmNmem->pm_parmLen,
						    FALSE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "IIapi_createGCAFetch: can't create GCD_ID\n" );
	    IIapi_freeMemTag( parmNmem->pm_memTag );
	    return( NULL );
	}
    
	MEcopy( (II_PTR)&stmtHndl->sh_cursorHndl->id_gca_id,
		sizeof( GCD_ID ), parmNmem->pm_parmBlock );
    
	if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_2 && 
	     prefetchrows > 1 )
	{
	    char prefetchRowTxt[ GCA_MAXNAME - DB_GW1_MAXNAME ];
	
	    CVna( (i4)prefetchrows, prefetchRowTxt );
	    STmove( prefetchRowTxt, ' ', GCA_MAXNAME - DB_GW1_MAXNAME,
		    ((GCD_ID *)parmNmem->pm_parmBlock)->gca_name + 
			DB_GW1_MAXNAME );
	}
    }
    
    return( parmNmem );
}




/*
** Name: IIapi_createGCAScroll
**
** Description:
**	This function creates the GCA2_FETCH parameters.
**	If the connection protocol is insufficient, only
**	a FETCH NEXT operation is supported using either
**	GCA_FETCH or GCA1_FETCH.  Caller must ensure that
**	the current request meets the protocol level.
**
** Input:
**      stmtHndl	Statement handle
**	reference	Reference point
**	offset		Row offset
**	rowCount	Number of rows
**
** Returns:
**	IIAPI_PARMNMEM *	A structure containg the parameter 
**				and its memory tag if successful; 
**				otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**	15-Mar-07 (gordy)
**	    Created.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAScroll
( 
    IIAPI_STMTHNDL	*stmtHndl,
    II_UINT2		reference,
    II_INT4		offset,
    II_INT2		rowCount 
)
{
    IIAPI_CONNHNDL	*connHndl;
    IIAPI_PARMNMEM	*parmNmem;
    GCD2_FT_DATA	*ftData;
    STATUS		status;
    
    if ( ! ( connHndl = IIapi_getConnHndl( (IIAPI_HNDL *)stmtHndl ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "createGCAScroll: can't get connection handle from statement\n" );
	return( NULL );
    }

    if ( connHndl->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_67 )
        return( IIapi_createGCAFetch( rowCount, stmtHndl ) );

    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAScroll: creating GCA2_FETCH parm\n" );
    
    if ( ! ( parmNmem = IIapi_createParmNMem() ) )  return( NULL );
    
    parmNmem->pm_msgType = GCA2_FETCH;
    parmNmem->pm_parmLen = sizeof( GCD2_FT_DATA );

    if ( ! ( ftData = (GCD1_FT_DATA *)MEreqmem( parmNmem->pm_memTag,
						parmNmem->pm_parmLen,
						FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCAScroll: can't create GCD2_FT_DATA\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }
    
    parmNmem->pm_parmBlock = (II_PTR)ftData;
    ftData->gca_anchor = (u_i4)reference;
    ftData->gca_offset = offset;
    ftData->gca_rowcount = (i4)rowCount;
    MEcopy( (II_PTR)&stmtHndl->sh_cursorHndl->id_gca_id,
	    sizeof( GCD_ID ), (II_PTR)&ftData->gca_cursor_id );
    
    return( parmNmem );
}




/*
** Name: IIapi_createGCAMdassoc
**
** Description:
**	This function creates the GCA_MD_ASSOC parameters.
**
**      handle    The handle which contains the current memory tag.
**
** Return value:
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**	 11-apr-94 (edg)
**	    Target name in MD_ASSOC message should have any vnode info
**	    stripped out.
**      11-apr-94 (ctham)
**          all string parameters in GCA_MD_ASSOC should have the
**          data types of IIAPI_CHA_TYPE instead of IIAPI_CHR_TYPE.
**      18-apr-94 (ctham)
**          GCA_RUPASS is not accepted in GCA_MD_ASSOC when
**          the protocol level is less than GCA_PROTOCOL_LEVEL_60,
**          corrected in IIapi_createGCAMdassoc().
**      26-apr-94 (ctham)
**          remove GCA_RUPASS completely from API.
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	14-Nov-94 (gordy)
**	    Cleanup GCA_MD_ASSOC processing.
**	17-Jan-95 (gordy)
**	    Conditionalized code for older GCA interface.
**	19-Jan-95 (gordy)
**	    Conditionalized code for older timezone interface.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	10-May-95 (gordy)
**	    Make sure the parameter count is correct when some
**	    parms are not sent due to protocol restrictions.
**	14-Jun-95 (gordy)
**	    Transaction IDs now sent as formatted strings.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	19-Jan-96 (gordy)
**	    Added global data structure.
**	 8-Mar-96 (gordy)
**	    GCD_SESSION_PARMS is now a variable length data structure
**	    rather than containing a pointer to a variable length array.
**	15-May-96 (gordy)
**	    Use a static location to hold timezone offset when connecting
**	    at a GCA protocol level which doesn't support timezone names.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
**	 9-Sep-96 (gordy)
**	    Added support for GCA_RUPASS, GCA_YEAR_CUTOFF.
**	16-Dec-99 (gordy)
**	    Make sure param count handles all conditional parameters.
**	23-Mar-01 (gordy)
**	    Need to use timezone selected by application when sending
**	    timezone offset to 6.4 DBMS.
**      28-May-03 (loera01) Bug 110318
**          Do not send GCA_DECIMAL unless the decimal value is a comma.
**	15-Mar-04 (gordy)
**	    Added connection parameter for I8 width.
**	 8-Aug-07 (gordy)
**	    Added connection parameter for date alias.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAMdassoc( IIAPI_CONNHNDL	*connHndl )
{
    IIAPI_SESSIONPARM	*sessionParm = connHndl->ch_sessionParm;
    IIAPI_PARMNMEM	*parmNmem = NULL;
    GCD_SESSION_PARMS	*mdAssocParm;
    STATUS		status;
    II_LONG		idx, mask;
    i4			parmCnt, i;

    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAMdassoc: creating MD_ASSOC parm\n" );
    
    if ( ! sessionParm )
    {
	/*
	** This is an internal error and should not occur.
	*/
	return( NULL );
    }
    /*
    ** Count the number of parameters in GCA_MD_ASSOC
    */
    parmCnt = BTcount( (char *)&sessionParm->sp_mask1,
		       sizeof(II_LONG) * BITSPERBYTE );
    parmCnt += BTcount( (char *)&sessionParm->sp_mask2,
			sizeof(II_LONG) * BITSPERBYTE );
    if ( sessionParm->sp_mask2 & IIAPI_SP_MISC_PARM )
	parmCnt += sessionParm->sp_misc_parm_count - 1;	/* -1 for BTcount() */
    if ( sessionParm->sp_mask2 & IIAPI_SP_GW_PARM )
	parmCnt += sessionParm->sp_gw_parm_count - 1;	/* -1 for BTcount() */
    
    /*
    **  Only send commas as decimal parameters.  Ignore all other settings.
    */
    if ( sessionParm->sp_mask1 & IIAPI_SP_DECIMAL &&
        STbcompare( sessionParm->sp_decimal, 0, ",\0", 0, TRUE ) )
    { 
        parmCnt--;
    }

    if ( connHndl->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_65 )
    {
	if ( sessionParm->sp_mask2 & IIAPI_SP_I8WIDTH )  parmCnt--;
    }

    if ( connHndl->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_62 )
    {
	if ( sessionParm->sp_mask2 & IIAPI_SP_RUPASS )  parmCnt--;
	if ( sessionParm->sp_mask2 & IIAPI_SP_YEAR_CUTOFF )  parmCnt--;
    }

    if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60 )
    {
	if ( sessionParm->sp_mask1 & IIAPI_SP_TIMEZONE )  
	    parmCnt--;
    }
    else
    {
	if ( sessionParm->sp_mask1 & IIAPI_SP_DECFLOAT )  parmCnt--;

	if ( sessionParm->sp_mask2 & IIAPI_SP_SL_TYPE )  parmCnt--;

	if ( sessionParm->sp_mask2 & IIAPI_SP_CAN_PROMPT )  parmCnt--;

	if ( (sessionParm->sp_mask1 & IIAPI_SP_TIMEZONE)  &&
	     connHndl->ch_partnerProtocol <= GCA_PROTOCOL_LEVEL_3 )
	    parmCnt--;

	if ( sessionParm->sp_mask1 & IIAPI_SP_TIMEZONE_NAME )
	    if ( sessionParm->sp_mask1 & IIAPI_SP_TIMEZONE  ||
	         connHndl->ch_partnerProtocol <= GCA_PROTOCOL_LEVEL_3 )
		parmCnt--;

	if ( sessionParm->sp_mask2 & IIAPI_SP_STRTRUNC )  parmCnt--;
    }

    if ( connHndl->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_67 )
    {
	if ( sessionParm->sp_mask2 & IIAPI_SP_DATE_ALIAS )  parmCnt--;
    }

    /*
    ** create user parameter buffers
    */
    if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	return( NULL );
    
    /*
    ** GCD_SESSION_PARMS is a variable length structure which
    ** already includes 1 GCD_USER_DATA entry.
    */
    parmNmem->pm_msgType = GCA_MD_ASSOC;
    parmNmem->pm_parmLen = sizeof( GCD_SESSION_PARMS ) +
			   sizeof( GCD_USER_DATA ) * (parmCnt - 1);

    if ( ! (parmNmem->pm_parmBlock = MEreqmem( parmNmem->pm_memTag, 
					       parmNmem->pm_parmLen, 
					       FALSE, &status )) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCAMdassoc: can't create GCD_SESSION_PARMS\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }
    
    mdAssocParm = (GCD_SESSION_PARMS *)parmNmem->pm_parmBlock;
    mdAssocParm->gca_l_user_data = parmCnt;
    
    /*
    ** Put values into GCA parameter buffers.
    ** Reset parmCnt and use as GCA buffer index.
    */
    for( 
	 mask = sessionParm->sp_mask1, idx = 0x00000001, parmCnt = 0;
	 mask  &&  idx;
	 mask &= ~idx, idx <<= 1
       )
	switch( mask & idx )
	{
	    case IIAPI_SP_EXCLUSIVE :
		put_GCA_int_parm( GCA_EXCLUSIVE, 
				  sizeof( sessionParm->sp_exclusive ),
				  (II_PTR)&sessionParm->sp_exclusive,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_VERSION :
		put_GCA_int_parm( GCA_VERSION, sizeof(sessionParm->sp_version),
				  (II_PTR)&sessionParm->sp_version,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_APPLICATION :
		put_GCA_int_parm( GCA_APPLICATION, 
				  sizeof( sessionParm->sp_application ),
				  (II_PTR)&sessionParm->sp_application,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_QLANGUAGE :
		put_GCA_int_parm( GCA_QLANGUAGE, 
				  sizeof( sessionParm->sp_qlanguage ),
				  (II_PTR)&sessionParm->sp_qlanguage,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_CWIDTH :
		put_GCA_int_parm( GCA_CWIDTH, sizeof( sessionParm->sp_cwidth ),
				  (II_PTR)&sessionParm->sp_cwidth,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_TWIDTH :
		put_GCA_int_parm( GCA_TWIDTH, sizeof( sessionParm->sp_twidth ),
				  (II_PTR)&sessionParm->sp_twidth,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_I1WIDTH :
		put_GCA_int_parm( GCA_I1WIDTH, sizeof(sessionParm->sp_i1width),
				  (II_PTR)&sessionParm->sp_i1width,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_I2WIDTH :
		put_GCA_int_parm( GCA_I2WIDTH, sizeof(sessionParm->sp_i2width),
				  (II_PTR)&sessionParm->sp_i2width,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_I4WIDTH :
		put_GCA_int_parm( GCA_I4WIDTH, sizeof(sessionParm->sp_i4width),
				  (II_PTR)&sessionParm->sp_i4width,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_F4WIDTH :
		put_GCA_int_parm( GCA_F4WIDTH, sizeof(sessionParm->sp_f4width),
				  (II_PTR)&sessionParm->sp_f4width,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_F4PRECISION :
		put_GCA_int_parm( GCA_F4PRECISION, 
				  sizeof( sessionParm->sp_f4precision ),
				  (II_PTR)&sessionParm->sp_f4precision,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_F8WIDTH :
		put_GCA_int_parm( GCA_F8WIDTH, sizeof(sessionParm->sp_f8width),
				  (II_PTR)&sessionParm->sp_f8width,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_F8PRECISION :
		put_GCA_int_parm( GCA_F8PRECISION, 
				  sizeof( sessionParm->sp_f8precision ),
				  (II_PTR)&sessionParm->sp_f8precision,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_SVTYPE :
		put_GCA_int_parm( GCA_SVTYPE, sizeof( sessionParm->sp_svtype ),
				  (II_PTR)&sessionParm->sp_svtype,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_NLANGUAGE :
		put_GCA_int_parm( GCA_NLANGUAGE, 
				  sizeof( sessionParm->sp_nlanguage ),
				  (II_PTR)&sessionParm->sp_nlanguage,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_MPREC :
		put_GCA_int_parm( GCA_MPREC, sizeof( sessionParm->sp_mprec ),
				  (II_PTR)&sessionParm->sp_mprec,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_MLORT :
		put_GCA_int_parm( GCA_MLORT, sizeof( sessionParm->sp_mlort ),
				  (II_PTR)&sessionParm->sp_mlort,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_DATE_FORMAT :
		put_GCA_int_parm( GCA_DATE_FORMAT, 
				  sizeof( sessionParm->sp_date_format ),
				  (II_PTR)&sessionParm->sp_date_format,
				  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_TIMEZONE :
		/*
		** Don't send this if at wrong protocol level.
		*/
		if ( connHndl->ch_partnerProtocol > GCA_PROTOCOL_LEVEL_3  &&
		     connHndl->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_60 )
		{
		    put_GCA_int_parm( GCA_TIMEZONE, 
				      sizeof( sessionParm->sp_timezone ),
				      (II_PTR)&sessionParm->sp_timezone,
				      &mdAssocParm->gca_user_data[parmCnt++] );
		}
		break;

	    case IIAPI_SP_TIMEZONE_NAME :
		/*
		** Only send this parameter at the correct GCA protocol
		** level.  If at lower protocol level and IIAPI_SP_TIMEZONE
		** is NOT set, then convert to older protocol.
		*/
		if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60 )
		{
		    put_GCA_string_parm(GCA_TIMEZONE_NAME, 
					sessionParm->sp_timezone_name,
					&mdAssocParm->gca_user_data[parmCnt++]);
		}
		else  if ( ! ( sessionParm->sp_mask1 & IIAPI_SP_TIMEZONE )  &&
			   connHndl->ch_partnerProtocol > GCA_PROTOCOL_LEVEL_3 )
	        {
		    /*
		    ** We need a static location to hold the
		    ** timezone info.  Since we checked that
		    ** IIAPI_SP_TIMEZONE is not currently set,
		    ** we can use the associated session parm.
		    */
		    sessionParm->sp_timezone = 
			get_tz_offset( sessionParm->sp_timezone_name );

		    put_GCA_int_parm( GCA_TIMEZONE, 
				      sizeof( sessionParm->sp_timezone ),
				      (II_PTR)&sessionParm->sp_timezone,
				      &mdAssocParm->gca_user_data[parmCnt++] );
		}
		break;

	    case IIAPI_SP_DBNAME :
		put_GCA_string_parm( GCA_DBNAME, sessionParm->sp_dbname,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_IDX_STRUCT :
		put_GCA_string_parm( GCA_IDX_STRUCT, sessionParm->sp_idx_struct,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_RES_STRUCT :
		put_GCA_string_parm( GCA_RES_STRUCT, sessionParm->sp_res_struct,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_EUSER :
		put_GCA_string_parm( GCA_EUSER, sessionParm->sp_euser,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_MATHEX :
		put_GCA_string_parm( GCA_MATHEX, sessionParm->sp_mathex,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_F4STYLE :
		put_GCA_string_parm( GCA_F4STYLE, sessionParm->sp_f4style,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_F8STYLE :
		put_GCA_string_parm( GCA_F8STYLE, sessionParm->sp_f8style,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_MSIGN :
		put_GCA_string_parm( GCA_MSIGN, sessionParm->sp_msign,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_DECIMAL :
               
                if (!STbcompare( sessionParm->sp_decimal, 0, ",\0", 0, TRUE ) )
                {
    		    put_GCA_string_parm( GCA_DECIMAL, sessionParm->sp_decimal,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
                }
		break;

	    case IIAPI_SP_APLID :
		put_GCA_string_parm( GCA_APLID, sessionParm->sp_aplid,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_GRPID :
		put_GCA_string_parm( GCA_GRPID, sessionParm->sp_grpid,
                                     &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_DECFLOAT :
		if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60 )
		    put_GCA_string_parm(GCA_DECFLOAT, 
					sessionParm->sp_decfloat, 
				        &mdAssocParm->gca_user_data[parmCnt++]);
		break;
	}

    for( 
	 mask = sessionParm->sp_mask2, idx = 0x00000001;
	 mask  &&  idx;
	 mask &= ~idx, idx <<= 1
       )
	switch( mask & idx )
	{
	    case IIAPI_SP_RUPASS :
		if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_62 )
		    put_GCA_string_parm(GCA_RUPASS, sessionParm->sp_rupass,
					&mdAssocParm->gca_user_data[parmCnt++]);
		break;

	    case IIAPI_SP_MISC_PARM :
		for( i = 0; i < sessionParm->sp_misc_parm_count; i++ )
		    put_GCA_string_parm(GCA_MISC_PARM, 
					sessionParm->sp_misc_parm[ i ],
					&mdAssocParm->gca_user_data[parmCnt++]);
		break;

	    case IIAPI_SP_STRTRUNC :
		if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60 )
		    put_GCA_string_parm(GCA_STRTRUNC, 
					sessionParm->sp_strtrunc,
                                        &mdAssocParm->gca_user_data[parmCnt++]);
		break;

	    case IIAPI_SP_XUPSYS :
		put_GCA_int_parm( GCA_XUPSYS, sizeof( sessionParm->sp_xupsys ),
				  (II_PTR)&sessionParm->sp_xupsys,
                                  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_SUPSYS :
		put_GCA_int_parm( GCA_SUPSYS, sizeof( sessionParm->sp_supsys ),
				  (II_PTR)&sessionParm->sp_supsys,
                                  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_WAIT_LOCK :
		put_GCA_int_parm( GCA_WAIT_LOCK, 
				  sizeof( sessionParm->sp_wait_lock ),
				  (II_PTR)&sessionParm->sp_wait_lock,
                                  &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_GW_PARM :
		for( i = 0; i < sessionParm->sp_gw_parm_count; i++ )
		    put_GCA_string_parm(GCA_GW_PARM, 
					sessionParm->sp_gw_parm[ i ],
					&mdAssocParm->gca_user_data[parmCnt++]);
		break;

	    case IIAPI_SP_RESTART :
		put_GCA_string_parm( GCA_RESTART, sessionParm->sp_restart,
				     &mdAssocParm->gca_user_data[parmCnt++] );
		break;

	    case IIAPI_SP_SL_TYPE :
		if( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60)
		    put_GCA_int_parm( GCA_SL_TYPE, 
				      sizeof( sessionParm->sp_sl_type ),
				      (II_PTR)&sessionParm->sp_sl_type,
				      &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_CAN_PROMPT :
		if( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60)
		    put_GCA_int_parm( GCA_CAN_PROMPT, 
				      sizeof( sessionParm->sp_can_prompt ),
				      (II_PTR)&sessionParm->sp_can_prompt,
				      &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_XA_RESTART :
		put_GCA_string_parm( GCA_XA_RESTART, sessionParm->sp_xa_restart,
				     &mdAssocParm->gca_user_data[parmCnt++] );
		break;

	    case IIAPI_SP_YEAR_CUTOFF :
		if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_62 )
		    put_GCA_int_parm( GCA_YEAR_CUTOFF, 
				      sizeof( sessionParm->sp_year_cutoff ),
				      (II_PTR)&sessionParm->sp_year_cutoff,
				      &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_I8WIDTH :
		if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_65 )
		    put_GCA_int_parm( GCA_I8WIDTH, 
				      sizeof(sessionParm->sp_i8width),
				      (II_PTR)&sessionParm->sp_i8width,
				      &mdAssocParm->gca_user_data[ parmCnt++ ] );
		break;

	    case IIAPI_SP_DATE_ALIAS :
		if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_67 )
		    put_GCA_string_parm(GCA_DATE_ALIAS, 
					sessionParm->sp_date_alias,
					&mdAssocParm->gca_user_data[parmCnt++]);
		break;
	}

    return( parmNmem );
}




/*
** Name: IIapi_createGCAQuery
**
** Description:
**	This function creates the GCA_QUERY parameters.
**
** Input:
**      parmNmem	Parameter and memory control block (may be NULL).
**      queryType	The type of query
**      queryText	The query text to be sent in GCA_QUERY, may be NULL.
**	allow_xact	Are transaction statements permitted
**      handle		General header for any handle 
**      parmCount	Number of initial parameters for the query
**
** Returns:
**	IIAPI_PARMNMEM *	Input value (if not NULL) or new value.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	11-Jan-95 (gordy)
**	    Cleaning up descriptor handling.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	13-Feb-95 (gordy)
**	    Disallow transaction statements in queries.
**	16-Feb-95 (gordy)
**	    API now adds query text for GCA dependent forms
**	    utilizing ~Q and ~V.
**	20-Jun-95 (gordy)
**	    Moved trasformation of query text to query dependent functions.
**	28-Jul-95 (gordy)
**	    Used fixed length types.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	15-Mar-96 (gordy)
**	    Added support for compressed varchars.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
**	 9-Jul-96 (gordy)
**	    Added allow_xact parameter for autocommit transactions.
**	29-May-97 (gordy)
**	    Permit queryText parameter to be NULL.  This will permit 
**	    the caller to package queryText as multiple GCA_DATA_VALUEs 
**	    if too long. If not NULL, queryText is sent as a single 
**	    GCA_DATA_VALUE.  
**	25-Oct-06 (gordy)
**	    Allow locators when requested.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAQuery 
(
    IIAPI_PARMNMEM	*parmNmem,
    IIAPI_QUERYTYPE	queryType,
    char		*queryText,
    II_BOOL		allow_xact,
    IIAPI_HNDL		*handle,
    II_INT2		parmCount
)
{
    GCD_Q_DATA		*qData;
    STATUS		status;
    II_BOOL		allocated = FALSE;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAQuery: creating GCA_QUERY parm\n" );
    
    if ( ! parmNmem )
    {
	if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	    return( NULL );

	parmNmem->pm_msgType = GCA_QUERY;
	allocated = TRUE;
    }
    
    /*
    ** The GCD_Q_DATA structure contains 1 GCD_DATA_VALUE.
    ** Allocate additional values as needed for the query
    ** text and parameters.
    */
    if ( queryText )  parmCount++;
    parmNmem->pm_parmLen = sizeof(GCD_Q_DATA) +
			   ((parmCount - 1) * sizeof(GCD_DATA_VALUE));
    
    if ( ! ( qData = (GCD_Q_DATA *)MEreqmem( parmNmem->pm_memTag, 
					     parmNmem->pm_parmLen, 
					     FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCAQuery: can't allocate GCA_Q_DATA\n" );

	if ( allocated )  IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }

    parmNmem->pm_parmBlock = (II_PTR)qData;
    qData->gca_language_id = DB_SQL;
    qData->gca_query_modifier = GCA_NAMES_MASK | GCA_COMP_MASK;

    if ( queryType == IIAPI_QT_SELECT_SINGLETON )
	qData->gca_query_modifier |= GCA_SINGLE_MASK;

    {
    	IIAPI_CONNHNDL	*connHndl;
    	
	if ( ! allow_xact  &&  (connHndl = IIapi_getConnHndl( handle ))  &&
    	     connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60 )
	    qData->gca_query_modifier |= GCA_Q_NO_XACT_STMT;
    }

    {
    	IIAPI_STMTHNDL	*stmtHndl = IIapi_getStmtHndl( handle );

	if ( stmtHndl  &&  stmtHndl->sh_flags & IIAPI_QF_LOCATORS )
	    qData->gca_query_modifier |= GCA_LOCATOR_MASK;
    }

    if ( queryText )
    {
	qData->gca_qdata[0].gca_type = DB_QTXT_TYPE;
	qData->gca_qdata[0].gca_precision = 0;
	qData->gca_qdata[0].gca_l_value = STlength( queryText ) + 1;
	qData->gca_qdata[0].gca_value = 
				MEreqmem( parmNmem->pm_memTag, 
					  qData->gca_qdata[0].gca_l_value, 
					  FALSE, &status );

	if ( ! qData->gca_qdata[0].gca_value )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "IIapi_createGCAQuery: can't allocate text buffer\n" );

	    if ( allocated )  IIapi_freeMemTag( parmNmem->pm_memTag );
	    return( NULL );
	}

	STcopy( queryText, qData->gca_qdata[0].gca_value );
    }

    return( parmNmem );
}




/*
** Name: IIapi_createGCAParm
**
** Description:
**	This function creates the GCA_QUERY message with parameters.
**
** Input:
**      stmtHndl	Statement handle of query.
**      putParmParm	IIapi_putParam() parameters (may be NULL).
**
** Return value:
**	IIAPI_PARMNMEM *	Memory buffer information, NULL if failure.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	21-Nov-94 (gordy)
**	    Renamed.
**	22-Dec-94 (gordy)
**	    Added support for DEFINE repeat query.
**	11-Jan-95 (gordy)
**	    Cleaning up descriptor handling.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	16-Feb-95 (gordy)
**	    API now adds query text for GCA dependent forms
**	    utilizing ~Q and ~V.
**	20-Jun-95 (gordy)
**	    Added support for sending BLOBs as parameters.
**	28-Jul-95 (gordy)
**	    Used fixed length types.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAParm
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTPARMPARM	*putParmParm
)
{
    IIAPI_PARMNMEM	*parmNmem;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAParm: creating GCA_QUERY parm\n" );
    
    if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	return( NULL );
    
    parmNmem->pm_msgType = GCA_QUERY;	/* Default message type */

    switch( stmtHndl->sh_queryType )
    {
	case IIAPI_QT_OPEN:
	    if ( ! createOpen( stmtHndl, putParmParm, parmNmem ) )
		goto allocFail;
	    break;

	case IIAPI_QT_CURSOR_DELETE:
	    if ( ! createDelete( stmtHndl, putParmParm, parmNmem ) )
		goto allocFail;
	    break;

	case IIAPI_QT_CURSOR_UPDATE:
	    if ( ! createUpdate( stmtHndl, putParmParm, parmNmem ) )
		goto allocFail;
	    break;

	case IIAPI_QT_DEF_REPEAT_QUERY:
	    if ( ! createDefRepeatQuery( stmtHndl, putParmParm, parmNmem ) )
		goto allocFail;
	    break;

	case IIAPI_QT_EXEC_REPEAT_QUERY:
	    if (! createExecRepeatQuery( stmtHndl, putParmParm, parmNmem ) )
		goto allocFail;
	    break;

	case IIAPI_QT_EXEC_PROCEDURE:
	    if ( ! createExecProc( stmtHndl, putParmParm, parmNmem ) )
		goto allocFail;
	    break;

	default:
	    if ( ! createQuery( stmtHndl, putParmParm, parmNmem ) )
		goto allocFail;
	    break;
    }
    
    /*
    ** Cursor updates can create a kludgy condition
    ** where all the parameters have been sent, but
    ** the cursor ID (which comes last) still needs
    ** to be sent.  In this case sh_parmSend will
    ** still be set, even when sh_parmIndex appears
    ** to indicate end-of-message.
    */
    parmNmem->pm_endFlag = 
	(! putParmParm) ? TRUE 
	    : ( (stmtHndl->sh_parmIndex >= stmtHndl->sh_parmCount &&
	        ! stmtHndl->sh_parmSend) ? TRUE : FALSE );

    return( parmNmem );
    
 allocFail:
    
    IIapi_freeMemTag( parmNmem->pm_memTag );
    
    return( NULL );
}



/*
** Name: IIapi_createGCAPrreply
**
** Description:
**	This function creates the GCA_PRREPLY parameters.
**
** Input:
**	connHndl		Connection handle.
**
** Output:
**	None.
**
** Return value:
**	IIAPI_PARMNMEM *	GCA parameter info.
**
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** History:
**	14-Oct-98 (rajus01)
**	    Created.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAPrreply( IIAPI_CONNHNDL	*connHndl )
{

    IIAPI_ENVHNDL	*envHndl= connHndl->ch_envHndl;
    IIAPI_PROMPTPARM 	*promptParm = 
				(IIAPI_PROMPTPARM *)connHndl->ch_prmptParm;
    IIAPI_PARMNMEM      *parmNmem = NULL;
    GCD_PRREPLY_DATA	*prRplyData;
    STATUS		status;

    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	    ( "IIapi_createGCAPrreply: creating PRREPLY data\n" );

    if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	return( NULL );

    parmNmem->pm_msgType = GCA_PRREPLY;
    parmNmem->pm_parmLen = sizeof( GCD_PRREPLY_DATA );

    if ( ! (parmNmem->pm_parmBlock = MEreqmem( parmNmem->pm_memTag,
					       parmNmem->pm_parmLen,
					       FALSE, &status )) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL ) 
	( "IIapi_createGCAPrreply: can't create GCD_PRREPLY_DATA\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL);
    }

    prRplyData = (GCD_PRREPLY_DATA *)parmNmem->pm_parmBlock;
    prRplyData->gca_l_prvalue = 1;

    if( promptParm->pd_flags & IIAPI_PR_NOECHO )
	prRplyData->gca_prflags |= GCA_PR_NOECHO;

    if( promptParm->pd_rep_flags & IIAPI_REPLY_TIMEOUT ) 
    	prRplyData->gca_prflags |= GCA_PR_TIMEOUT; 

    prRplyData->gca_prvalue = (GCD_DATA_VALUE *)MEreqmem( parmNmem->pm_memTag,
			  prRplyData->gca_l_prvalue * sizeof(
			  GCD_DATA_VALUE),
			  FALSE, &status );

    if ( ! prRplyData->gca_prvalue )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
		( "IIapi_createGCAPrreply: can't allocate text buffer\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }

    if( promptParm->pd_rep_len )
    {
	prRplyData->gca_prvalue[0].gca_type = DB_CHA_TYPE;
	prRplyData->gca_prvalue[0].gca_precision = 0;
	prRplyData->gca_prvalue[0].gca_l_value = promptParm->pd_rep_len;
	prRplyData->gca_prvalue[0].gca_value =
		(II_CHAR *)MEreqmem( parmNmem->pm_memTag,
			  prRplyData->gca_prvalue[0].gca_l_value,
			  FALSE, &status );
	if ( ! prRplyData->gca_prvalue[0].gca_value )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCAPrreply: can't allocate text buffer for reply\n");
	    IIapi_freeMemTag( parmNmem->pm_memTag );
	    return( NULL );
	}

	MEcopy( promptParm->pd_reply, promptParm->pd_rep_len,
			prRplyData->gca_prvalue[0].gca_value );
    }

    return( parmNmem );
}


/*
** Name: IIapi_createGCAResponse
**
** Description:
**	This function creates the GCA_RESPONSE parameters.
**
**      stmtHndl   statement handle
**
** Return value:
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAResponse( IIAPI_STMTHNDL *stmtHndl )
{
    IIAPI_PARMNMEM	*parmNmem = (IIAPI_PARMNMEM *) NULL;
    GCD_RE_DATA		*rspData;
    STATUS		status;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAResponse: creating GCA_RESPONSE parm\n" );
    
    if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	return( NULL );
    
    if ( ! ( rspData = (GCD_RE_DATA *)MEreqmem( parmNmem->pm_memTag, 
					        sizeof(GCD_RE_DATA), 
						TRUE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCAResponse: can't allocate GCA_RE_DATA\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }

    parmNmem->pm_msgType = GCA_RESPONSE;
    parmNmem->pm_parmLen = sizeof(*rspData);
    parmNmem->pm_parmBlock = (II_PTR)rspData;
    rspData->gca_rqstatus = GCA_OK_MASK;
    rspData->gca_rowcount = GCA_NO_ROW_COUNT;
    
    return( parmNmem );
}




/*
** Name: IIapi_createGCASecure
**
** Description:
**	This function creates the GCA_SECURE parameters.
**
**      tranHndl  transaction handle
**
** Return value:
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	14-Jun-95 (gordy)
**	    Updated to new GCD structure definitions.
**	28-Jul-95 (gordy)
**	    Use fixed length types.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCASecure( IIAPI_TRANHNDL *tranHndl)
{
    IIAPI_PARMNMEM	*parmNmem;
    STATUS		status;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCASecure: creating GCA_SECURE parm\n" );
    
    if ( ! tranHndl->th_tranName )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCASecure: no distributed xact name handle\n" );
	return( NULL );
    }

    if ( ! ( parmNmem = IIapi_createParmNMem() ) )
	return( NULL );
    
    if ( tranHndl->th_tranName->tn_tranID.ti_type == IIAPI_TI_XAXID )
    {
	GCD_XA_TX_DATA	*iiXAXid;

	if ( ! ( iiXAXid = (GCD_XA_TX_DATA *)MEreqmem( parmNmem->pm_memTag,
						       sizeof(GCD_XA_TX_DATA), 
						       FALSE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "IIapi_createGCASecure: can't allocate GCA XA XID\n" );
	    IIapi_freeMemTag( parmNmem->pm_memTag );
	    return( NULL );
	}

	parmNmem->pm_msgType = GCA_XA_SECURE;
	parmNmem->pm_parmLen = sizeof( GCD_XA_TX_DATA );
	parmNmem->pm_parmBlock = (II_PTR)iiXAXid;
	iiXAXid->gca_xa_type = 0;
	iiXAXid->gca_xa_precision = 0;
	iiXAXid->gca_xa_l_value = sizeof( GCD_XA_DIS_TRAN_ID );
	MEcopy( (PTR)&tranHndl->th_tranName->tn_tranID.ti_value.xaXID,
		sizeof( IIAPI_XA_DIS_TRAN_ID ), (PTR)&iiXAXid->gca_xa_id );
    }
    else
    {
	GCD_TX_DATA		*iiGCAXid;
	IIAPI_II_DIS_TRAN_ID	*iiTranID;

	iiTranID = &tranHndl->th_tranName->tn_tranID.ti_value.iiXID;
	    
	if ( ! ( iiGCAXid = (GCD_TX_DATA *)MEreqmem( parmNmem->pm_memTag,
						     sizeof(GCD_TX_DATA), 
						     FALSE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "IIapi_createGCASecure: can't allocate GCA XID\n" );
	    IIapi_freeMemTag( parmNmem->pm_memTag );
	    return( NULL );
	}

	parmNmem->pm_msgType = GCA_SECURE;
	parmNmem->pm_parmLen = sizeof( GCD_TX_DATA );
	parmNmem->pm_parmBlock = (II_PTR)iiGCAXid;
	iiGCAXid->gca_tx_id.gca_index[0] = iiTranID->ii_tranID.it_highTran;
	iiGCAXid->gca_tx_id.gca_index[1] = iiTranID->ii_tranID.it_lowTran;
	iiGCAXid->gca_tx_type = GCA_DTX;

	/*
	** The back end ignores the gca_name value.  We
	** could send what the application gave us, but
	** doing what LIBQ does is probably a better idea.
	*/
	iiGCAXid->gca_tx_id.gca_name[ 0 ] = '?';
	iiGCAXid->gca_tx_id.gca_name[ 1 ] = '\0';
    }
    
    return( parmNmem );
}




/*
** Name: IIapi_createGCAxa
**
** Description:
**	This function creates the GCA_XA_DATA parameters.
**
** Input:
**	msg_type	GCA message type.
**	xa_xid		XA transaction ID.
**	xa_flags	XA operation flags.
**
** Output:
**	None.
**
** Returns:
**      IIAPI_PARMNMEM	A structure containg the parameter and 
**			its memory tag (NULL if error).
** History:
**	11-Jul-06 (gordy)
**	    Created.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCAxa( II_LONG msg_type, 
		   IIAPI_XA_DIS_TRAN_ID *xa_xid, II_ULONG xa_flags )
{
    IIAPI_PARMNMEM	*parmNmem;
    GCD_XA_DATA		*xa_data;
    STATUS		status;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCAxa: creating GCA_XA_DATA parm\n" );
    
    if ( ! (parmNmem = IIapi_createParmNMem()) )  return( NULL );
    
    if ( ! (xa_data = (GCD_XA_DATA *)MEreqmem( 
	    parmNmem->pm_memTag, sizeof( GCD_XA_DATA ), TRUE, &status )) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "IIapi_createGCAxa: can't allocate GCA_XA_DATA\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }

    parmNmem->pm_msgType = msg_type;
    parmNmem->pm_parmLen = sizeof( GCD_XA_DATA );
    parmNmem->pm_parmBlock = (II_PTR)xa_data;
    xa_data->gca_xa_flags = xa_flags;
    xa_data->gca_xa_xid.formatID = xa_xid->xa_tranID.xt_formatID;
    xa_data->gca_xa_xid.gtrid_length = xa_xid->xa_tranID.xt_gtridLength;
    xa_data->gca_xa_xid.bqual_length = xa_xid->xa_tranID.xt_bqualLength;
    xa_data->gca_xa_xid.branch_seqnum = xa_xid->xa_branchSeqnum;
    xa_data->gca_xa_xid.branch_flag = xa_xid->xa_branchFlag;
    MEcopy( (PTR)xa_xid->xa_tranID.xt_data,
	    min( xa_xid->xa_tranID.xt_gtridLength +
	         xa_xid->xa_tranID.xt_bqualLength, GCA_XA_XID_MAX ),
	    (PTR)xa_data->gca_xa_xid.data );
    
    return( parmNmem );
}




/*
** Name: IIapi_createGCATuple
**
** Description:
**	This function creates the GCA_TUPLE parameters.
**
**      stmtHndl      statement handle
**      columnCount   column count
**      columnData    array of column data
**      moreSegment   flag indicating more BLOB segment
**
** Return value:
**      parmNmem  a structure containg the parameter and its memory tag
**                if successful; otherwise, it returns NULL.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     use hd_memTag for memory allocation.
**	11-Jan-95 (gordy)
**	    Cleaning up descriptor handling.
**	23-Jan-95 (gordy)
**	    Cleaned up error handling.
**	28-Jul-95 (gordy)
**	    Fixed tracing.
**	22-Apr-96 (gordy)
**	    Finished support for COPY FROM.
**	24-May-96 (gordy)
**	    Removed hd_memTag, replaced by new tagged memory support.
**	 1-Mar-01 (gordy)
**	    Embedded length in IIAPI_NVCH_TYPE is number of characters,
**	    which must be converted to bytes to determine actual length.
**	 6-Oct-04 (gordy)
**	    Must send at least one column, even if it exceeds the max
**	    tuple length normally used.  Rather than use a fixed tuple
**	    length, calculate the length of the current column-set.
*/

II_EXTERN IIAPI_PARMNMEM *
IIapi_createGCATuple 
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTCOLPARM	*putColParm
)
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    IIAPI_PARMNMEM	*parmNmem;
    IIAPI_DESCRIPTOR	*descr;
    IIAPI_DATAVALUE	*value;
    STATUS		status;
    char		*tuple;
    II_LONG		tupmax;
    II_LONG		length; 
    II_INT2		count; 
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "IIapi_createGCATuple: creating GCA_TUPLE parm\n" );
    
    if ( ! (parmNmem = IIapi_createParmNMem()) )  return( NULL );
    
    /*
    ** Determine length of tuple buffer for current set of
    ** columns.  This is simply the sum of the lengths of 
    ** the columns in GCA format with the following three 
    ** exceptions:
    **
    ** 1) The column-set contains a BLOB.
    ** 2) The tuple length exceeds the GCA buffer length.
    ** 3) A single column exceeds the GCA buffer length
    **    (a subset of exception 2).
    **
    ** The buffer length given one of the three exceptions
    ** is determined as follows:
    **
    ** 1) Use the GCA buffer length to hold BLOB segments.
    ** 2) The sum of column lengths preceeding exception
    **    column.  See exception 3 if no preceding columns.
    ** 3) The length of the column even though it exceeds
    **    the GCA buffer length.
    */
    tupmax = 0;
    descr = &stmtHndl->sh_colDescriptor[ stmtHndl->sh_colIndex ];
    value = &putColParm->pc_columnData[ putColParm->pc_columnCount - 
					stmtHndl->sh_colFetch ];

    for( count = stmtHndl->sh_colFetch; count; count--, descr++, value++ )
    {
	if ( IIapi_isBLOB( descr->ds_dataType ) )		/* Ex 1 */
	{
	    tupmax = connHndl->ch_sizeAdvise;
	    break;
	}

	/*
	** Compressed VARCHAR requires special length calculations.
	** For a description, see the compressed VARCHAR data handling
	** section below.
	*/
	if ( ! (stmtHndl->sh_flags & IIAPI_SH_COMP_VARCHAR)  ||
	     ! IIapi_isVAR( descr->ds_dataType ) )
	    length = IIapi_getGCALength( descr );
	else
	{
	    IIAPI_DESCRIPTOR	descriptor;

	    if ( descr->ds_nullable  &&  value->dv_null )
		length = 0;
	    else
	    {
		II_UINT2 len;

		MEcopy( value->dv_value, sizeof( len ), (PTR)&len );
		length = (IIapi_isUCS2(descr->ds_dataType) ? len * 2 : len);
	    }

	    descriptor.ds_dataType = descr->ds_dataType;
	    descriptor.ds_nullable = descr->ds_nullable;
	    descriptor.ds_length = sizeof( II_UINT2 ) + length;
	    descriptor.ds_precision = descr->ds_precision;
	    descriptor.ds_scale = descr->ds_scale;

	    length = IIapi_getGCALength( &descriptor );
	}

	if ( (tupmax + length) > connHndl->ch_sizeAdvise )	/* Ex 2 & 3 */
	{
	    if ( ! tupmax )  tupmax = length;			/* Ex 3 */
	    break;
	}

	/*
	** Sum length of column-set.
	*/
	tupmax += length;
    }

    if ( ! (tuple = MEreqmem( parmNmem->pm_memTag, tupmax, FALSE, &status )) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )( "getTuple: can't allocate tuple\n" );
	IIapi_freeMemTag( parmNmem->pm_memTag );
	return( NULL );
    }

    parmNmem->pm_msgType = GCA_TUPLES;
    parmNmem->pm_parmLen = 0;
    parmNmem->pm_parmBlock = (II_PTR)tuple;
    
    IIAPI_TRACE( IIAPI_TR_DETAIL )
	("IIapi_createGCATuple: %d columns starting at %d, %d total columns\n",
	 (II_LONG)stmtHndl->sh_colFetch, (II_LONG)stmtHndl->sh_colIndex, 
	 (II_LONG)stmtHndl->sh_colCount );
    
    descr = &stmtHndl->sh_colDescriptor[ stmtHndl->sh_colIndex ];
    value = &putColParm->pc_columnData[ putColParm->pc_columnCount - 
					stmtHndl->sh_colFetch ];

    for( ; stmtHndl->sh_colFetch;
	 stmtHndl->sh_colFetch--, stmtHndl->sh_colIndex++, descr++, value++ )
    {
	if ( IIapi_isBLOB( descr->ds_dataType ) )
	{
	    II_BOOL	moreSegments, done;

	    /*
	    ** Check to see if there are more segments after
	    ** the current one.  The moreSegments flag is
	    ** only meaningful for the last column in the
	    ** request.
	    */
	    moreSegments = (putColParm->pc_moreSegments  &&  
			    stmtHndl->sh_colFetch <= 1);

	    /*
	    ** Format the current segment.
	    */
	    length = tupmax - parmNmem->pm_parmLen;
	    done = IIapi_cnvtBLOB2GCATuple( stmtHndl, descr, value,
					    moreSegments, tuple, &length );
	    tuple += length;
	    parmNmem->pm_parmLen += length;

	    /*
	    ** The current segment is incomplete in two cases:
	    ** the segment was split, or the end-of-segments
	    ** indicator needs to be sent (an artifact of query
	    ** parameter processing).  Check for the possibility
	    ** that we have hit the second case (the only way
	    ** to know for sure is peek at some BLOB processing
	    ** data which is considered private to the data
	    ** handling routines).
	    */
	    if ( ! done  &&  ! moreSegments )
	    {
		/*
		** The last segment was either split, or we
		** need to send the end-of-segments indicator.
		** A second conversion call will have no effect
		** in the former case and may finish the BLOB
		** in the latter.
		*/
		length = tupmax - parmNmem->pm_parmLen;
		done = IIapi_cnvtBLOB2GCATuple( stmtHndl, descr, value,
						moreSegments, tuple, &length );
		tuple += length;
		parmNmem->pm_parmLen += length;
	    }

	    /*
	    ** We are done if we split the current segment,
	    ** if there are more segments to follow, or if
	    ** this is the last column in the request.  The
	    ** first two cases require special handling,
	    ** while the last is handled via standard loop
	    ** processing.
	    */
	    if ( ! done )
	    {
		/*
		** Since we have not finished the segment,
		** we neither decrement the column count,
		** nor increment the column index so that
		** when we are called again we will finish
		** the remainder of the current segment.
		*/
		break;
	    }
	    else  if ( moreSegments )
	    {
		/*
		** We are done with the current segment and
		** request, so decrement the column count.  
		** Since we don't increment the column index, 
		** we will continue processing this column 
		** when called with the next segment.
		*/
		stmtHndl->sh_colFetch--;
		break;
	    }
	}
	else  if ( stmtHndl->sh_flags & IIAPI_SH_COMP_VARCHAR  &&
		   IIapi_isVAR( descr->ds_dataType ) )
	{
	    IIAPI_DESCRIPTOR	descriptor;
	    II_UINT2		len = 0;

	    /*
	    ** Determine the compressed length of the 
	    ** data value.  Normally, NULL data values 
	    ** are ignored, but for compressed we need 
	    ** to make sure the embedded length is 0.
	    */
	    if ( descr->ds_nullable  &&  value->dv_null )
	    {
		if ( (parmNmem->pm_parmLen + sizeof( len )) > tupmax )
		    break;

		MEcopy( (PTR)&len, sizeof( len ), tuple );
		length = 0;
	    }
	    else
	    {
		MEcopy( value->dv_value, sizeof( len ), (PTR)&len );
		length = (IIapi_isUCS2(descr->ds_dataType) ? len * 2 : len);
	    }

	    /*
	    ** Now build a new descriptor which represents
	    ** the variable length data as a type which 
	    ** contains no padding (compressed format) and
	    ** process in the same manner as non-compressed
	    ** values.
	    */
	    descriptor.ds_dataType = descr->ds_dataType;
	    descriptor.ds_nullable = descr->ds_nullable;
	    descriptor.ds_length = sizeof( len ) + length;
	    descriptor.ds_precision = descr->ds_precision;
	    descriptor.ds_scale = descr->ds_scale;

	    length = IIapi_getGCALength( &descriptor );

	    if ( (parmNmem->pm_parmLen + length) > tupmax )
	    {
		/*
		** We have filled the tuple buffer.  Exit
		** the loop to send what has been processed
		** so far.  We will be called again to send
		** what remains.
		*/
		break;
	    }

	    IIapi_cnvtDataValue2GCATuple( &descriptor, value, tuple );

	    tuple += length;
	    parmNmem->pm_parmLen += length;
	}
	else
	{
	    length = IIapi_getGCALength( descr );

	    if ( (parmNmem->pm_parmLen + length) > tupmax )
	    {
		/*
		** We have filled the tuple buffer.  Exit
		** the loop to send what has been processed
		** so far.  We will be called again to send
		** what remains.
		*/
		break;
	    }

	    IIapi_cnvtDataValue2GCATuple( descr, value, tuple );

	    tuple += length;
	    parmNmem->pm_parmLen += length;
	}
    }
    
    /*
    ** Check to see if we have completed the current tuple.
    */
    if ( stmtHndl->sh_colIndex >= stmtHndl->sh_colCount )
	stmtHndl->sh_colIndex = 0;              /* Reset for next tuple */
    else
	parmNmem->pm_endFlag = FALSE;		/* Clear end-of-data flag */

    return( parmNmem );
}




/*
** Name: put_GCA_int_parm
**
** Description:
**	Builds a GCA_MD_ASSOC integer parameter.  The data value
**	in the IIAPI_SESSIONPARM structure is referenced since the
**	values will not change for the duration of the GCA_MD_ASSOC
**	processing.
**
** History:
**	14-Nov-94 (gordy)
**	    Created.
**	22-Oct-04 (gordy)
**	    Zero GCA precision.
*/

static II_VOID
put_GCA_int_parm( i4  gca_index, i4  data_size, 
		  PTR data_value, GCD_USER_DATA *gca_user_data )
{
    gca_user_data->gca_p_index = gca_index;
    gca_user_data->gca_p_value.gca_type = IIAPI_INT_TYPE;
    gca_user_data->gca_p_value.gca_precision = 0;
    gca_user_data->gca_p_value.gca_l_value = data_size;
    gca_user_data->gca_p_value.gca_value = data_value;

    return;
}


/*
** Name: put_GCA_string_parm
**
** Description:
**	Builds a GCA_MD_ASSOC string parameter.  The data value
**	in the IIAPI_SESSIONPARM structure is referenced since the
**	values will not change for the duration of the GCA_MD_ASSOC
**	processing.
**
** History:
**	14-Nov-94 (gordy)
**	    Created.
**	22-Oct-04 (gordy)
**	    Zero GCA precision.
*/

static II_VOID
put_GCA_string_parm( i4  gca_index, char *data_value,
		     GCD_USER_DATA *gca_user_data )
{
    gca_user_data->gca_p_index = gca_index;
    gca_user_data->gca_p_value.gca_precision = 0;
    gca_user_data->gca_p_value.gca_l_value = STlength( data_value );

    if ( gca_user_data->gca_p_value.gca_l_value )
    {
	gca_user_data->gca_p_value.gca_type = IIAPI_CHA_TYPE;
	gca_user_data->gca_p_value.gca_value = data_value;
    }
    else
    {
	/*
	** Ingres does not like zero length CHAR strings,
	** so send a zero length VARCHAR string.
	*/
	gca_user_data->gca_p_value.gca_type = IIAPI_VCH_TYPE;
	gca_user_data->gca_p_value.gca_l_value = sizeof( zero_string );
	gca_user_data->gca_p_value.gca_value = (II_PTR)&zero_string;
    }

    return;
}


/*
** Name: createQuery
**
** Description:
**	This group of functions create GCA_QUERY parameter blocks.
**
**      stmtHndl     statement handle
**      putParmParm  parameter block for IIapi_putParms()
**      parmNmem     parameter memory tag
**
** Return value:
**      status    TRUE if the function succeeds, FALSE otherwise.
**
** Re-entrancy:
**	This function does not update shared memory.
**
** History:
**      30-sep-93 (ctham)
**          creation
**      17-may-94 (ctham)
**          change all prefix from CLI to IIAPI.
**	 31-may-94 (ctham)
**	     cleaned up handling of GCA names.
**	21-Nov-94 (gordy)
**	    Allow for zero length owner name.
**	22-Nov-94 (gordy)
**	    Fix cursor ID handling.
**	22-Dec-94 (gordy)
**	    Added support for DEFINE repeat query.
**	10-Jan-95 (gordy)
**	    Allow for procedure handle instead of procedure name.
**	11-Jan-95 (gordy)
**	    Cleaning up descriptor handling.
**	17-Jan-95 (gordy)
**	    Conditionalize code for older GCA interface.
**	16-Feb-95 (gordy)
**	    API now adds query text for GCA dependent forms
**	    utilizing ~Q and ~V.
**	20-Jun-95 (gordy)
**	    Added support for sending BLOBs as parameters.
**	11-Jul-95 (gordy)
**	    Save pp_moreSegments in local variable under same
**	    conditions that pp_parmCount is saved.
**	 6-Oct-04 (gordy)
**	    Use GCA suggested buffer size for message buffer.
*/

static II_BOOL
createQuery 
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTPARMPARM	*putParmParm,
    IIAPI_PARMNMEM	*parmNmem
)
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    GCD_DATA_VALUE	*qDataValue;
    II_INT2		parmCount;
    IIAPI_DESCRIPTOR	*descriptor;
    IIAPI_DATAVALUE	*parmData;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )( "createQuery: creating GCA_QUERY\n" );
    
    if ( ! putParmParm )
    {
	/*
	** Simple query, no parameters.
	*/
        if ( ! allocQueryBuffer( stmtHndl, parmNmem, (II_INT2)0 ) )  
	    return( FALSE );
    }
    else
    {
	parmCount = stmtHndl->sh_parmSend;
        descriptor = &stmtHndl->sh_parmDescriptor[ stmtHndl->sh_parmIndex ];
	parmData = &putParmParm->pp_parmData[ putParmParm->pp_parmCount - 
					      stmtHndl->sh_parmSend ];

	/*
	** Check to see if initial query processing is 
	** required.  If not, just need to send current 
	** parameters.
	*/
	if ( ! stmtHndl->sh_parmIndex  &&  stmtHndl->sh_queryText )
        {
	    /*
	    ** Perform any query text transformations required.
	    */
	    switch( stmtHndl->sh_queryType )
	    {
		case IIAPI_QT_EXEC :
		    if ( ! append_using_clause( stmtHndl ) )
			return( FALSE );
		    break;
	    }

	    parmCount = calcQueryCount( stmtHndl, parmCount, 
					descriptor, parmData, 
					putParmParm->pp_moreSegments, 
					connHndl->ch_sizeAdvise, 0 );

	    if ( ! ( qDataValue = allocQueryBuffer( stmtHndl, 
						    parmNmem, parmCount ) ) )
		return( FALSE );
	}
	else  if ( IIapi_isBLOB( descriptor->ds_dataType ) )
	{
	    /*
	    ** Check to see if the BLOB can be sent with a
	    ** single segment, or must be sent in multiple
	    ** segment.  If we have already started sending
	    ** multiple segments, continue doing so.
	    */
	    if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )
		parmCount = 0;
	    else
		parmCount = calcDataValueCount( parmCount, 
						descriptor, parmData, 
						putParmParm->pp_moreSegments, 
						connHndl->ch_sizeAdvise, 
						0, FALSE );

	    if ( parmCount )
	    {
		/*
		** The BLOB (and perhaps additional parameters)
		** can be sent as a single segment data value.
		*/
		if ( ! ( qDataValue = allocDataValues(parmNmem, parmCount) ) )
		    return( FALSE );
	    }
	    else
	    {
		/*
		** The BLOB must be sent as a multi-segment
		** data value.
		*/
		if ( ! copyBLOBSegment(stmtHndl, parmNmem, descriptor, 
				       parmData, putParmParm->pp_moreSegments) )
		    return( FALSE );
	    }
	}
	else
	{
	    parmCount = calcDataValueCount( parmCount, descriptor, parmData, 
					    putParmParm->pp_moreSegments, 
					    connHndl->ch_sizeAdvise, 0, TRUE );

	    if ( ! ( qDataValue = allocDataValues( parmNmem, parmCount ) ) )
		return( FALSE );
	}

	if ( parmCount )
	{
	    if ( ! copyQueryDataValue( parmNmem->pm_memTag, qDataValue, 
				       parmData, descriptor, parmCount ) )
	    {
		IIAPI_TRACE( IIAPI_TR_FATAL )
		    ( "createQuery: can't copy gca data value\n" );
		return( FALSE );
	    }

	    stmtHndl->sh_parmSend -= parmCount;
	    stmtHndl->sh_parmIndex += parmCount;
	}
    }
    
    return( TRUE );
}




static II_BOOL
createOpen 
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTPARMPARM	*putParmParm,
    IIAPI_PARMNMEM	*parmNmem
)
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    GCD_DATA_VALUE	*qDataValue;
    IIAPI_DESCRIPTOR	*descriptor = NULL;
    IIAPI_DATAVALUE	*parmData = NULL;
    II_BOOL		moreSegments = FALSE;
    II_INT2		parmCount = 0;
    II_LONG		len;
    char		*ptr, cursor_name[ GCA_MAXNAME + 1 ];
    STATUS		status;

    IIAPI_TRACE( IIAPI_TR_VERBOSE )( "createOpen: creating GCA_QUERY\n" );
    
    /*
    ** Since cursor name is an optional parameter,
    ** its possible that there are no parameters.
    */
    if ( putParmParm )
    {
	parmCount = stmtHndl->sh_parmSend;
	parmData = &putParmParm->pp_parmData[ putParmParm->pp_parmCount - 
					      stmtHndl->sh_parmSend ];
	descriptor = &stmtHndl->sh_parmDescriptor[ stmtHndl->sh_parmIndex ];
	moreSegments = putParmParm->pp_moreSegments;
    }

    /*
    ** Check to see if initial query processing is 
    ** required.  If not, just need to send current 
    ** parameters.
    */
    if ( ! stmtHndl->sh_parmIndex  &&  stmtHndl->sh_queryText )
    {
	char *open = (stmtHndl->sh_flags & IIAPI_QF_SCROLL) 
		     ? api_qt_scroll : api_qt_open;

	/*
	** Perform query text transformation.  First,
	** dynamic OPEN CURSOR requires a USING clause.
	*/
	if ( dynamic_open( stmtHndl->sh_queryText )  &&
	     ! append_using_clause( stmtHndl ) )
	    return( FALSE );

	/*
	** Then pre-pend the OPEN CURSOR text.
	*/
	len = STlength(open) + STlength(stmtHndl->sh_queryText) + 1;

	if ( ! ( ptr = MEreqmem( 0, len, FALSE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "createOpen: can't allocate text buffer\n" );
	    return( FALSE );
	}

	STpolycat( 2, open, stmtHndl->sh_queryText, ptr );
	MEfree( stmtHndl->sh_queryText );
	stmtHndl->sh_queryText = ptr;

	/*
	** Check to see if the application 
	** has provided the cursor name.
	*/
	if ( parmCount  &&  descriptor[0].ds_columnType == IIAPI_COL_SVCPARM )
	{
	    /*
	    ** Use the applications cursor name and
	    ** remove from parameter set.
	    */
	    len = putParmParm->pp_parmData[0].dv_length;
	    ptr = putParmParm->pp_parmData[0].dv_value;

	    stmtHndl->sh_parmSend--;
	    stmtHndl->sh_parmIndex++;
	    parmCount--;
	    parmData++;
	    descriptor++;
	}
	else
	{
	    IIAPI_CONNHNDL *connHndl;

	    /*
	    ** Create a unique cursor name.  We should
	    ** not have a problem getting the connection
	    ** handle (in a perfect world), but watch out
	    ** just the same.
	    */
	    connHndl = IIapi_getConnHndl( (IIAPI_HNDL *)stmtHndl );
	    STprintf( cursor_name, "IIAPICURSOR%d", 
		      connHndl ? connHndl->ch_cursorID++ : (i4)stmtHndl );
	    ptr = cursor_name;
	    len = STlength( cursor_name );
	}

	/*
	** Cursor name is sent as a 3 part GCA_ID 
	** in addition to any other parameters.  
	*/
	parmCount = calcQueryCount( stmtHndl, parmCount, descriptor, parmData, 
				    moreSegments, connHndl->ch_sizeAdvise, 
				    API_GCA_ID_SIZE );

	if ( ! ( qDataValue = allocQueryBuffer( stmtHndl, 
						parmNmem, parmCount + 3 ) ) )
	    return( FALSE );
    
	if ( ! copyCursorID( parmNmem->pm_memTag, 
			     qDataValue, 0, 0, (II_INT2)len, ptr ) )
	    return( FALSE );

	qDataValue += 3;
    }
    else  if ( IIapi_isBLOB( descriptor->ds_dataType ) )
    {
	/*
	** Check to see if the BLOB can be sent with a
	** single segment, or must be sent in multiple
	** segment.  If we have already started sending
	** multiple segments, continue doing so.
	*/
	if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )
	    parmCount = 0;
	else
	    parmCount = calcDataValueCount( parmCount, descriptor, 
					    parmData, moreSegments, 
					    connHndl->ch_sizeAdvise, 
					    0, FALSE );

	if ( parmCount )
	{
	    /*
	    ** The BLOB (and perhaps additional parameters)
	    ** can be sent as a single segment data value.
	    */
	    if ( ! ( qDataValue = allocDataValues( parmNmem, parmCount ) ) )
		return( FALSE );
	}
	else
	{
	    /*
	    ** The BLOB must be sent as a multi-segment
	    ** data value.
	    */
	    if ( ! copyBLOBSegment( stmtHndl, parmNmem, descriptor, 
				    parmData, moreSegments ) )
		return( FALSE );
	}
    }
    else
    {
	parmCount = calcDataValueCount( parmCount, descriptor, parmData,
					moreSegments, connHndl->ch_sizeAdvise,
					0, TRUE );

	if ( ! ( qDataValue = allocDataValues( parmNmem, parmCount ) ) )
	    return( FALSE );
    }
    
    /*
    ** Now send the remaining parameters.
    */
    if ( parmCount )
    {
	if ( ! copyQueryDataValue( parmNmem->pm_memTag, qDataValue, 
			       	   parmData, descriptor, parmCount ) )
	    return( FALSE );
    
	stmtHndl->sh_parmSend -= parmCount;
	stmtHndl->sh_parmIndex += parmCount;
    }

    return( TRUE );
}




static II_BOOL
createUpdate 
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTPARMPARM	*putParmParm,
    IIAPI_PARMNMEM	*parmNmem
)
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    GCD_DATA_VALUE	*qDataValue;
    II_INT2		svcparms;
    II_INT2		parmCount;
    IIAPI_DATAVALUE	*parmData;
    IIAPI_DESCRIPTOR	*descriptor;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "createUpdate: creating GCA_QUERY for cursor update\n" );
    
    /*
    ** Watch out for the kludgy condition where sh_parmSend
    ** is set, but sh_parmIndex is at end of parameters.  This
    ** indicates that all the parameters have been sent, but
    ** the cursor ID remains to be sent.  It is caused by the
    ** last parameter being a BLOB.  In this case, most of the
    ** values set below are invalid (but they will not be used).
    */
    parmCount = stmtHndl->sh_parmSend;
    parmData = &putParmParm->pp_parmData[ putParmParm->pp_parmCount - 
					  stmtHndl->sh_parmSend ];
    descriptor = &stmtHndl->sh_parmDescriptor[ stmtHndl->sh_parmIndex ];

    /*
    ** The cursor ID is sent as a 3 part GCA_ID
    ** after all the other parameters are sent,
    ** so we need to determine if the current
    ** parameter set is the last.
    */
    svcparms = (stmtHndl->sh_parmIndex + parmCount >= stmtHndl->sh_parmCount) 
		? 3 : 0;

    /*
    ** Check to see if initial query processing is 
    ** required.  If not, just need to send current 
    ** parameters.
    */
    if ( ! stmtHndl->sh_parmIndex  &&  stmtHndl->sh_queryText )
    {
	IIAPI_STMTHNDL	*cursorHndl;

	/*
	** Perform query text transformation.
	*/
	if ( ! append_where_clause( stmtHndl ) )
	    return( FALSE );

	/*
	** The cursor handle parameter is actually 
	** a statement handle containing the cursor 
	** handle.  Save it in our statement handle 
	** (but don't set the cursor flag since we 
	** didn't open the cursor).  Remove cursor
	** handle from the parameter set.
	*/
	MEcopy( parmData[ 0 ].dv_value, sizeof(II_PTR), (II_PTR)&cursorHndl );
	stmtHndl->sh_cursorHndl = cursorHndl->sh_cursorHndl;
	stmtHndl->sh_parmSend--;
	stmtHndl->sh_parmIndex++;
	parmCount--;
	parmData++;
	descriptor++;

	/*
	** The cursor ID is sent as a 3 part GCA_ID
	** after all the other parameters are sent,
	*/
	parmCount = calcQueryCount( stmtHndl, parmCount, descriptor, 
				    parmData, putParmParm->pp_moreSegments,
				    connHndl->ch_sizeAdvise,
				    svcparms ? API_GCA_ID_SIZE : 0 );

	/*
	** If we can't send the last parameter with the initial
	** query info, don't send the cursor ID either.
	*/
	if ( svcparms  &&  parmCount < stmtHndl->sh_parmSend )
	    svcparms = 0;

	if ( ! ( qDataValue = allocQueryBuffer( stmtHndl, parmNmem, 
					        parmCount + svcparms ) ) )
	    return( FALSE );
    }
    else  if ( stmtHndl->sh_parmIndex >= stmtHndl->sh_parmCount )
    {
	/*
	** The inconsistent values indicate that we have
	** reached a special case where all parameters have
	** been sent, but the cursor ID remains to be sent.  
	** This occurs when the last parameter is a BLOB 
	** (see below).  Send only the cursor ID, and clean
	** up the inconsistent values.
	*/
	parmCount = 0;
	stmtHndl->sh_parmSend--;

	if ( ! ( qDataValue = allocDataValues( parmNmem, svcparms ) ) )
	    return( FALSE );
    }
    else  if ( IIapi_isBLOB( descriptor->ds_dataType ) )
    {
	/*
	** Check to see if the BLOB can be sent with a
	** single segment, or must be sent in multiple
	** segment.  If we have already started sending
	** multiple segments, continue doing so.
	*/
	if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )
	    parmCount = 0;
	else
	    parmCount = calcDataValueCount( parmCount, descriptor, parmData, 
					    putParmParm->pp_moreSegments, 
					    connHndl->ch_sizeAdvise,
					    svcparms ? API_GCA_ID_SIZE : 0, 
					    FALSE );

	if ( parmCount )
	{
	    /*
	    ** The BLOB (and perhaps additional parameters)
	    ** can be sent as a single segment data value.
	    **
	    ** If we can't send the last parameter, 
	    ** don't send the cursor ID either.
	    */
	    if ( svcparms  &&  parmCount < stmtHndl->sh_parmSend )
		svcparms = 0;

	    if ( ! ( qDataValue = allocDataValues( parmNmem, 
						   parmCount + svcparms ) ) )
		return( FALSE );
	}
	else
	{
	    /*
	    ** The BLOB must be sent as a multi-segment
	    ** data value.
	    */
	    if ( ! copyBLOBSegment( stmtHndl, parmNmem, descriptor, 
				    parmData, putParmParm->pp_moreSegments ) )
		return( FALSE );

	    /*
	    ** Kludge for handling cursor ID when last
	    ** parameter is a BLOB.  We can't send the
	    ** ID with the BLOB segment, so we keep the
	    ** request count set.  The resulting values
	    ** of sh_parmIndex and sh_parmSend are a
	    ** signal that only the cursor ID needs to
	    ** be sent (even if they are inconsistent).
	    */
	    if ( svcparms && stmtHndl->sh_parmIndex >= stmtHndl->sh_parmCount )
		stmtHndl->sh_parmSend++;

	    svcparms = 0;
	}
    }
    else
    {
	/*
	** The cursor ID is sent as a 3 part GCA_ID
	** after all the other parameters are sent,
	*/
	parmCount = calcDataValueCount( parmCount, descriptor, parmData,
					putParmParm->pp_moreSegments,
					connHndl->ch_sizeAdvise,
					svcparms ? API_GCA_ID_SIZE : 0, TRUE );

	/*
	** If we can't send the last parameter, 
	** don't send the cursor ID either.
	*/
	if ( svcparms  &&  parmCount < stmtHndl->sh_parmSend )
	    svcparms = 0;

	if ( ! (qDataValue = allocDataValues(parmNmem, parmCount + svcparms)) )
	    return( FALSE );
    }

    if ( parmCount )
    {
	if ( ! copyQueryDataValue( parmNmem->pm_memTag, qDataValue, 
				   parmData, descriptor, parmCount ) )
	return( FALSE );
    
	stmtHndl->sh_parmSend -= parmCount;
	stmtHndl->sh_parmIndex += parmCount;
    }

    /*
    ** Send the cursor ID after all other parameters.
    */
    if ( svcparms )
    {
	GCD_ID	*cursorID = &stmtHndl->sh_cursorHndl->id_gca_id;

	if ( ! copyCursorID( parmNmem->pm_memTag, &qDataValue[ parmCount ],
			     cursorID->gca_index[0], cursorID->gca_index[1],
			     sizeof(cursorID->gca_name), cursorID->gca_name ) )
	{
	    return( FALSE );
	}

	stmtHndl->sh_cursorHndl = NULL;
    }

    return( TRUE );
}




static II_BOOL
createDelete
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTPARMPARM	*putParmParm,
    IIAPI_PARMNMEM	*parmNmem
)
{
    GCD_DATA_VALUE	*qDataValue;
    IIAPI_STMTHNDL	*cursorHndl;
    GCD_ID		*cursorID;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "createDelete: creating GCA_QUERY for cursor delete\n" );
    
    /*
    ** Need to transform query text.
    */
    if ( ! append_where_clause( stmtHndl ) )
	return( FALSE );

    /*
    ** The cursor handle is the only parameter, 
    ** Its actually a statement handle containing
    ** the cursor handle.
    */
    MEcopy( putParmParm->pp_parmData[ 0 ].dv_value, 
	    sizeof(II_PTR), (II_PTR)&cursorHndl );
    cursorID = &cursorHndl->sh_cursorHndl->id_gca_id;
    stmtHndl->sh_parmSend--;
    stmtHndl->sh_parmIndex++;
	
    /*
    ** The cursor ID is sent as three data values.  No other
    ** parameters are sent.
    */
    if ( ! ( qDataValue = allocQueryBuffer(stmtHndl, parmNmem, (II_INT2)3) ) )
	return( FALSE );
    
    if ( ! copyCursorID( parmNmem->pm_memTag, qDataValue,
			 cursorID->gca_index[0], cursorID->gca_index[1],
			 sizeof(cursorID->gca_name), cursorID->gca_name ) )
    {
	return( FALSE );
    }

    return( TRUE );
}




static II_BOOL
createExecProc
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTPARMPARM	*putParmParm,
    IIAPI_PARMNMEM	*parmNmem
)
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    II_INT2		parmCount, svcparms;
    IIAPI_DATAVALUE	*parmData;
    IIAPI_DESCRIPTOR	*descriptor;
    GCD_P_PARAM		*procParm;
    IIAPI_IDHNDL	*idHndl;
    STATUS		status;
    II_BOOL		success;
    char		*ptr;
    i4			i;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "createExecProc: creating GCA_INVPROC/GCA1_INVPROC\n" );
    
    parmCount = stmtHndl->sh_parmSend;
    parmData = &putParmParm->pp_parmData[ putParmParm->pp_parmCount - 
					  stmtHndl->sh_parmSend ];
    descriptor = &stmtHndl->sh_parmDescriptor[ stmtHndl->sh_parmIndex ];

    if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60 )
	parmNmem->pm_msgType = GCA1_INVPROC;
    else
	parmNmem->pm_msgType = GCA_INVPROC;

    /*
    ** Check to see if initial query processing is 
    ** required.  If not, just need to send current 
    ** parameters.  Since one parameter is required,
    ** testing for start of parameters is sufficient.
    */
    if ( ! stmtHndl->sh_parmIndex )
    {
	/*
	** The procedure name/ID is required, but the
	** procedure owner is optional so determine
	** the number of parameters which are not
	** actual procedure parameters and remove
	** them from the parameter set.
	*/
	svcparms =  (stmtHndl->sh_parmCount > 1  &&  
		     descriptor[1].ds_columnType == IIAPI_COL_SVCPARM) ? 2 : 1;

	stmtHndl->sh_parmSend -= svcparms;
	stmtHndl->sh_parmIndex += svcparms;
        parmCount -= svcparms;
	parmData += svcparms;
	descriptor += svcparms;

	if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_60 )
	{
	    GCD1_IP_DATA *ipData1;
	
	    /*
	    ** There is 1 GCD_P_PARAM in GCD_IP_DATA
	    ** so only allocate additional as needed.  Also,
	    ** if no procedure parameters then don't even
	    ** send the one in GCD_IP_DATA.
	    */
	    parmCount = calcProcParamCount( parmCount, descriptor, parmData,
					    putParmParm->pp_moreSegments,
					    connHndl->ch_sizeAdvise,
					    sizeof( GCA1_IP_DATA ) - 
					    sizeof( GCA_P_PARAM ), FALSE );

	    parmNmem->pm_parmLen = sizeof( GCD1_IP_DATA ) +
				   ((parmCount - 1) * sizeof(GCD_P_PARAM));
	
	    if ( ! (ipData1 = (GCD1_IP_DATA *)
			      MEreqmem(parmNmem->pm_memTag, 
				       parmNmem->pm_parmLen, TRUE, &status)) )
	    {
		IIAPI_TRACE( IIAPI_TR_FATAL )
		    ( "createExecProc: can't allocate GCA1_IP_DATA\n" );
		return( FALSE );
	    }

	    parmNmem->pm_parmBlock = (II_PTR)ipData1;
	
	    if (stmtHndl->sh_parmDescriptor[0].ds_dataType == IIAPI_HNDL_TYPE)
	    {
		MEcopy( putParmParm->pp_parmData[0].dv_value, 
			sizeof(II_PTR), (II_PTR)&idHndl );
		MEcopy( (II_PTR)&idHndl->id_gca_id, 
			sizeof( GCD_ID ), (II_PTR)&ipData1->gca_id_proc );
	    }
	    else
	    {
		/*
		** copy procedure name
		*/
		MEmove( putParmParm->pp_parmData[0].dv_length,
			(char *)putParmParm->pp_parmData[0].dv_value,
			' ', GCA_MAXNAME, ipData1->gca_id_proc.gca_name );
	    }
	    
	    /*
	    ** copy optional procedure owner - may be zero length.
	    */
	    if ( svcparms < 2 )
		ipData1->gca_proc_own.gca_l_name = 0;
	    else
		ipData1->gca_proc_own.gca_l_name =
					putParmParm->pp_parmData[1].dv_length;

	    if ( ! ipData1->gca_proc_own.gca_l_name )
	    {
		ipData1->gca_proc_own.gca_name = NULL;
	    }
	    else  if ( ! ( ipData1->gca_proc_own.gca_name = (char *)
			   MEreqmem( parmNmem->pm_memTag,
				     ipData1->gca_proc_own.gca_l_name, 
				     FALSE, &status ) ) )
	    {
		IIAPI_TRACE( IIAPI_TR_FATAL )
		    ("createExecProc: can't allocate procedure owner name\n");
		return( FALSE );
	    }
	    else
	    {
		MEcopy( putParmParm->pp_parmData[1].dv_value,
			ipData1->gca_proc_own.gca_l_name,
			ipData1->gca_proc_own.gca_name );
	    }

	    ipData1->gca_proc_mask = GCA_PARAM_NAMES_MASK;

	    if ( stmtHndl->sh_flags & IIAPI_QF_LOCATORS )
	    	ipData1->gca_proc_mask |= GCA_IP_LOCATOR_MASK;

	    procParm = ipData1->gca_param;
	}
	else
	{
	    GCD_IP_DATA	*ipData;
	
	    /*
	    ** There is 1 GCD_P_PARAM in GCD_IP_DATA
	    ** so only allocate additional as needed.  Also,
	    ** if no procedure parameters then don't even
	    ** send the one in GCD_IP_DATA.
	    */
	    parmCount = calcProcParamCount( parmCount, descriptor, parmData,
					    putParmParm->pp_moreSegments,
					    connHndl->ch_sizeAdvise,
					    sizeof( GCA_IP_DATA ) - 
					    sizeof( GCA_P_PARAM ), FALSE );

	    parmNmem->pm_parmLen = sizeof( GCD_IP_DATA ) +
				   ((parmCount - 1) * sizeof(GCD_P_PARAM));
	
	    if ( ! (ipData = (GCD_IP_DATA *)MEreqmem( parmNmem->pm_memTag, 
						      parmNmem->pm_parmLen, 
						      TRUE, &status )) )
	    {
		IIAPI_TRACE( IIAPI_TR_FATAL )
		    ( "createExecProc: can't allocate GCA_IP_DATA\n" );
		return( FALSE );
	    }

	    parmNmem->pm_parmBlock = (II_PTR)ipData;
	
	    if (stmtHndl->sh_parmDescriptor[0].ds_dataType == IIAPI_HNDL_TYPE)
	    {
		MEcopy( putParmParm->pp_parmData[0].dv_value, 
			sizeof(II_PTR), (II_PTR)&idHndl );
		MEcopy( (II_PTR)&idHndl->id_gca_id, 
			sizeof( GCD_ID ), (II_PTR)&ipData->gca_id_proc );
	    }
	    else
	    {
		/*
		** copy procedure name
		*/
		ptr = ipData->gca_id_proc.gca_name;

		if ( svcparms >= 2 )
		{
		    /*
		    ** Combine owner and procedure name by
		    ** prepending  '<owner>.' to where
		    ** the procedure name will be placed.
		    */
		    MEcopy( putParmParm->pp_parmData[ 1 ].dv_value,
			    putParmParm->pp_parmData[ 1 ].dv_length, ptr );

		    ptr += putParmParm->pp_parmData[ 1 ].dv_length;
		    *(ptr++) = '.';
		}

		MEmove( putParmParm->pp_parmData[ 0 ].dv_length,
			putParmParm->pp_parmData[ 0 ].dv_value, 
			' ', 
			(sizeof(ipData->gca_id_proc.gca_name) - 
					(ptr - ipData->gca_id_proc.gca_name)), 
			ptr );
	    }

	    ipData->gca_proc_mask = GCA_PARAM_NAMES_MASK;

	    if ( stmtHndl->sh_flags & IIAPI_QF_LOCATORS )
	    	ipData->gca_proc_mask |= GCA_IP_LOCATOR_MASK;

	    procParm = ipData->gca_param;
	}
    }
    else  if ( IIapi_isBLOB( descriptor->ds_dataType ) )
    {
	/*
	** Check to see if the BLOB can be sent with a
	** single segment, or must be sent in multiple
	** segment.  If we have already started sending
	** multiple segments, continue doing so.
	*/
	if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )
	    parmCount = 0;
	else
	    parmCount = calcProcParamCount( parmCount, descriptor, parmData, 
					    putParmParm->pp_moreSegments, 
					    connHndl->ch_sizeAdvise, 0, FALSE );

	if ( parmCount )
	{
	    /*
	    ** The BLOB (and perhaps additional parameters)
	    ** can be sent as a single segment data value.
	    */
	    parmNmem->pm_parmLen = sizeof( GCD_P_PARAM ) * parmCount;
	
	    if ( ! ( procParm = (GCD_P_PARAM *)MEreqmem( parmNmem->pm_memTag, 
						         parmNmem->pm_parmLen, 
							 TRUE, &status ) ) )
	    {
		IIAPI_TRACE( IIAPI_TR_FATAL )
		    ( "createExecProc: can't allocate GCA_P_PARAM\n" );
		return( FALSE );
	    }

	    parmNmem->pm_parmBlock = (II_PTR)procParm;
	}
	else
	{
	    /*
	    ** The BLOB must be sent as a multi-segment
	    ** data value.
	    */
	    if ( ! copyBLOBSegment( stmtHndl, parmNmem, descriptor, 
				    parmData, putParmParm->pp_moreSegments ) )
		return( FALSE );
	}
    }
    else
    {
	parmCount = calcProcParamCount( parmCount, descriptor, parmData, 
					putParmParm->pp_moreSegments, 
					connHndl->ch_sizeAdvise, 0, TRUE );

	parmNmem->pm_parmLen = sizeof( GCD_P_PARAM ) * parmCount;
	
	if ( ! ( procParm = (GCD_P_PARAM *)MEreqmem( parmNmem->pm_memTag, 
						     parmNmem->pm_parmLen, 
						     TRUE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "createExecProc: can't allocate GCA_P_PARAM\n" );
	    return( FALSE );
	}

	parmNmem->pm_parmBlock = (II_PTR)procParm;
    }

    /*
    ** Now send the procedure parameters.
    */
    for( i = 0; i < parmCount; i++ )
    {
	if ( ! descriptor[ i ].ds_columnName )
	{
	    procParm[ i ].gca_parname.gca_l_name = 0;
	    procParm[ i ].gca_parname.gca_name = NULL;
	}
	else
	{
	    procParm[ i ].gca_parname.gca_l_name =
		STlength( descriptor[ i ].ds_columnName );
	    procParm[ i ].gca_parname.gca_name =
		STtalloc( parmNmem->pm_memTag, descriptor[ i ].ds_columnName );
	}
	    
	procParm[ i ].gca_parm_mask = 0;

	switch( descriptor[ i ].ds_columnType )
	{
	case IIAPI_COL_PROCOUTPARM :
	    /*
	    ** Output parameters backward supported as BYREF.
	    */
	    if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_66 )
		procParm[ i ].gca_parm_mask |= GCA_IS_OUTPUT;
	    else
		procParm[ i ].gca_parm_mask |= GCA_IS_BYREF;
	    break;

	case IIAPI_COL_PROCINOUTPARM :
	    procParm[ i ].gca_parm_mask |= GCA_IS_BYREF;
	    break;

	case IIAPI_COL_PROCGTTPARM :
 	    procParm[ i ].gca_parm_mask |= GCA_IS_GTTPARM;
	    break;
	}

	success = IIapi_cnvtDataValue2GDV( parmNmem->pm_memTag, 
					   &descriptor[ i ], &parmData[ i ], 
					   &procParm[ i ].gca_parvalue );

	if ( ! success )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "createExecProc: can't create parvalue\n" );
	    return( FALSE );
	}
    }
    
    stmtHndl->sh_parmSend -= parmCount;
    stmtHndl->sh_parmIndex += parmCount;

    return( TRUE );
}




/*
** History:
**      08-mar-2007 (smeke01) BUG 115434
**          GChostname() now handles the conversion between 1-based
**          count of elements and 0-based count of elements, so caller
**          no longer needs to decrement by 1.
*/
static II_BOOL
createDefRepeatQuery
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTPARMPARM	*putParmParm,
    IIAPI_PARMNMEM	*parmNmem
)
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    GCD_DATA_VALUE	*qDataValue;
    IIAPI_DESCRIPTOR	*descriptor = NULL;
    IIAPI_DATAVALUE	*parmData = NULL;
    II_BOOL		moreSegments = FALSE;
    II_INT2		parmCount = 0;
    II_LONG		gca_id1, gca_id2;
    STATUS		status;
    II_LONG		len;
    char		*ptr, name[ GCA_MAXNAME ];
    char		host[ GCA_MAXNAME ], process[ GCA_MAXNAME ];
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "createDefRepeatQuery: creating GCA_DEFINE\n" );
    
    parmNmem->pm_msgType = GCA_DEFINE;

    /*
    ** Since query ID is optional, it is possible 
    ** that there are no parameters and putParmParm 
    ** is NULL.
    */
    if ( putParmParm )
    {
	parmCount = stmtHndl->sh_parmSend;
	parmData = &putParmParm->pp_parmData[ putParmParm->pp_parmCount - 
					      stmtHndl->sh_parmSend ];
	descriptor = &stmtHndl->sh_parmDescriptor[ stmtHndl->sh_parmIndex ];
	moreSegments = putParmParm->pp_moreSegments;
    }

    /*
    ** Check to see if initial query processing is 
    ** required.  If not, just need to send current 
    ** parameters.
    */
    if ( ! stmtHndl->sh_parmIndex  &&  stmtHndl->sh_queryText )
    {
	/*
	** Perform query text transformation.
	** Pre-pend the DEFINE QUERY text.
	*/
	len = STlength( api_qt_repeat ) + 
	      STlength( stmtHndl->sh_queryText ) + 1;

	if ( ! ( ptr = MEreqmem( 0, len, FALSE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "createDefRepeatQuery: can't allocate text buffer\n" );
	    return( FALSE );
	}

	STpolycat( 2, api_qt_repeat, stmtHndl->sh_queryText, ptr );
	MEfree( stmtHndl->sh_queryText );
	stmtHndl->sh_queryText = ptr;

	/*
	** Check to see if the application has
	** provided the repeat query ID.
	*/
	if ( parmCount && descriptor[0].ds_columnType == IIAPI_COL_SVCPARM )
	{
	    /*
	    ** Use the applications ID and
	    ** remove from parameter set.
	    */
	    MEcopy( putParmParm->pp_parmData[0].dv_value, 
		    sizeof( gca_id1 ), (II_PTR)&gca_id1 );

	    MEcopy( putParmParm->pp_parmData[1].dv_value, 
		    sizeof( gca_id2 ), (II_PTR)&gca_id2 );

	    len = putParmParm->pp_parmData[2].dv_length;
	    ptr = putParmParm->pp_parmData[2].dv_value;

	    stmtHndl->sh_parmSend -= 3;
	    stmtHndl->sh_parmIndex += 3;
	    parmCount -= 3;
	    parmData += 3;
	    descriptor += 3;
	}
	else
	{
	    SYSTIME uniqueID;

	    /*
	    ** Try to create a run time unique ID.
	    ** The time from TMnow() should be 
	    ** unique in this process, and will 
	    ** most likely be unique globally.
	    ** Use system info to decrease window
	    ** in which conflicts may occur.
	    */
	    TMnow( &uniqueID );
	    gca_id1 = uniqueID.TM_secs;
	    gca_id2 = uniqueID.TM_msecs;
	    GChostname( host, sizeof(host) );
	    PCunique( process );
	    STpolycat( 4, "IIAPI_", host, "_", process, name );
	    ptr = name;
	    len = STlength( ptr );
	}

	/*
	** The repeat query ID is sent as a 3 part 
	** GCA_ID in addition to any other parameters.
	*/
	parmCount = calcQueryCount( stmtHndl, parmCount, descriptor, parmData, 
				    moreSegments, connHndl->ch_sizeAdvise,
				    API_GCA_ID_SIZE );

	if ( ! ( qDataValue = allocQueryBuffer( stmtHndl, 
						parmNmem, parmCount + 3 ) ) )
	    return( FALSE );

	if ( ! copyCursorID(parmNmem->pm_memTag, 
			    qDataValue, gca_id1, gca_id2, (II_INT2)len, ptr) )
	    return( FALSE );

	qDataValue += 3;
    }
    else  if ( IIapi_isBLOB( descriptor->ds_dataType ) )
    {
	/*
	** Check to see if the BLOB can be sent with a
	** single segment, or must be sent in multiple
	** segment.  If we have already started sending
	** multiple segments, continue doing so.
	*/
	if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )
	    parmCount = 0;
	else
	    parmCount = calcDataValueCount( parmCount, descriptor, 
					    parmData, moreSegments, 
					    connHndl->ch_sizeAdvise,
					    0, FALSE );

	if ( parmCount )
	{
	    /*
	    ** The BLOB (and perhaps additional parameters)
	    ** can be sent as a single segment data value.
	    */
	    if ( ! ( qDataValue = allocDataValues( parmNmem, parmCount ) ) )
		return( FALSE );
	}
	else
	{
	    /*
	    ** The BLOB must be sent as a multi-segment
	    ** data value.
	    */
	    if ( ! copyBLOBSegment( stmtHndl, parmNmem, descriptor, 
				    parmData, moreSegments ) )
		return( FALSE );
	}
    }
    else
    {
	parmCount = calcDataValueCount( parmCount, descriptor, 
					parmData, moreSegments, 
					connHndl->ch_sizeAdvise,
					0, TRUE );

	if ( ! ( qDataValue = allocDataValues( parmNmem, parmCount ) ) )
	    return( FALSE );
    }

    /*
    ** Now send the rest of the parameters.
    */
    if ( parmCount )
    {
	if ( ! copyQueryDataValue( parmNmem->pm_memTag, qDataValue, 
				   parmData, descriptor, parmCount ) )
	return( FALSE );

	stmtHndl->sh_parmSend -= parmCount;
	stmtHndl->sh_parmIndex += parmCount;
    }

    return( TRUE );
}




static II_BOOL
createExecRepeatQuery
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PUTPARMPARM	*putParmParm,
    IIAPI_PARMNMEM	*parmNmem
)
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    IIAPI_IDHNDL	*idHndl;
    II_INT2		parmCount;
    IIAPI_DESCRIPTOR	*descriptor;
    IIAPI_DATAVALUE	*parmData;
    GCD_DATA_VALUE	*qDataValue;
    STATUS		status;
    
    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "createExecRepeatQuery: creating GCA_INVOKE\n" );
    
    parmNmem->pm_msgType = GCA_INVOKE;
    parmCount = stmtHndl->sh_parmSend;
    parmData = &putParmParm->pp_parmData[ putParmParm->pp_parmCount - 
					  stmtHndl->sh_parmSend ];
    descriptor = &stmtHndl->sh_parmDescriptor[ stmtHndl->sh_parmIndex ];

    /*
    ** Check to see if initial query processing is 
    ** required.  If not, just need to send current 
    ** parameters.  Since one parameter is required,
    ** testing for start of parameters is sufficient.
    */
    if ( ! stmtHndl->sh_parmIndex )
    {
	GCD_IV_DATA	*ivData;

	/*
	** Process the repeat query ID and remove
	** handle from the parameter set.
	*/
	MEcopy( putParmParm->pp_parmData[0].dv_value, 
		sizeof(II_PTR), (II_PTR)&idHndl );
    
	stmtHndl->sh_parmSend--;
	stmtHndl->sh_parmIndex++;
        parmCount--;
	parmData++;
	descriptor++;

	/*
	** There is 1 GCD_DATA_VALUE in GCD_IV_DATA
	** so only allocate additional as needed.  Also,
	** if no parameters then don't even send the one in 
	** GCD_IV_DATA.
	*/
	parmCount = calcDataValueCount( parmCount, descriptor, parmData,
					putParmParm->pp_moreSegments,
					connHndl->ch_sizeAdvise, 
					sizeof( GCA_IV_DATA ) - 
					sizeof( GCA_DATA_VALUE ), FALSE );

	parmNmem->pm_parmLen = sizeof( GCD_IV_DATA ) +
			       (sizeof(GCD_DATA_VALUE) * (parmCount - 1));
    
	if ( ! ( ivData = (GCD_IV_DATA *)MEreqmem( parmNmem->pm_memTag, 
						   parmNmem->pm_parmLen, 
						   TRUE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "createExecRepeatQuery: can't allocate GCA_IV_DATA\n" );
	    return( FALSE );
	}

	parmNmem->pm_parmBlock = (II_PTR)ivData;
	qDataValue = ivData->gca_qparm;
	MEcopy( (II_PTR)&idHndl->id_gca_id, 
		sizeof( GCD_ID ), (II_PTR)&ivData->gca_qid );
    }
    else  if ( IIapi_isBLOB( descriptor->ds_dataType ) )
    {
	/*
	** Check to see if the BLOB can be sent with a
	** single segment, or must be sent in multiple
	** segment.  If we have already started sending
	** multiple segments, continue doing so.
	*/
	if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )
	    parmCount = 0;
	else
	    parmCount = calcDataValueCount( parmCount, descriptor, parmData, 
					    putParmParm->pp_moreSegments, 
					    connHndl->ch_sizeAdvise, 
					    0, FALSE );

	if ( parmCount )
	{
	    /*
	    ** The BLOB (and perhaps additional parameters)
	    ** can be sent as a single segment data value.
	    */
	    if ( ! ( qDataValue = allocDataValues( parmNmem, parmCount ) ) )
		return( FALSE );
	}
	else
	{
	    /*
	    ** The BLOB must be sent as a multi-segment
	    ** data value.
	    */
	    if ( ! copyBLOBSegment( stmtHndl, parmNmem, descriptor, 
				    parmData, putParmParm->pp_moreSegments ) )
		return( FALSE );
	}
    }
    else
    {
	parmCount = calcDataValueCount( parmCount, descriptor, parmData,
					putParmParm->pp_moreSegments, 
					connHndl->ch_sizeAdvise, 0, TRUE );

	if ( ! ( qDataValue = allocDataValues( parmNmem, parmCount ) ) )
	    return( FALSE );
    }

    /*
    ** Now send the current parameter set.
    */
    if ( parmCount )
    {
	if ( ! copyQueryDataValue( parmNmem->pm_memTag, qDataValue,
				   parmData, descriptor, parmCount ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "createExecRepeatQuery: can't copy gca data value\n" );
	    return( FALSE );
	}

	stmtHndl->sh_parmSend -= parmCount;
	stmtHndl->sh_parmIndex += parmCount;
    }

    return( TRUE );
}



/*
** Name: dynamic_open
**
** Description:
**	Peaks at the query text to determine if this is
**	a select statement or a prepared statement name.
**
** Input:
**	queryText	Text of query.
**
** Output:
**	none
**
** Returns:
**	II_BOOL		TRUE if dynamic, FALSE if select statement.
**
** History:
**	13-Mar-95 (gordy)
**	    Created.
*/

static II_BOOL
dynamic_open( char *queryText )
{
    II_BOOL dynamic = TRUE;

    /*
    ** Skip leading white space.
    */
    while( *queryText  &&  CMwhite( queryText ) )  CMnext( queryText );

    /*
    ** A select statement may be nested in parenthesis
    ** or will start with keyword SELECT.
    */
    if ( *queryText == '(' ||
	 ( STlength( queryText ) > 7  &&
	   STncasecmp( queryText, "select ", 7 ) == 0 ) )
	dynamic = FALSE;
    
    return( dynamic );
}



/*
** Name: append_where_clause
**
** Description:
**	Modifies the query text by appending ' WHERE CURRENT OF ~Q '.
**
** Input:
**	stmtHndl	Statement handle of query.
**
** Output:
**	None.
**
** Returns:
**	II_BOOL		TRUE if success, FALSE if memory allocation failure.
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
*/

static II_BOOL
append_where_clause( IIAPI_STMTHNDL *stmtHndl )
{
    i4		length;
    char	*text;
    STATUS	status;
    II_BOOL	success = TRUE;

    length = STlength( stmtHndl->sh_queryText ) + STlength( api_qt_where ) + 1;

    if ( ! ( text = MEreqmem( 0, length, FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "append_where_clause: can't allocate text buffer\n" );
	success = FALSE;
    }
    else
    {
	STpolycat( 2, stmtHndl->sh_queryText, api_qt_where, text );
	MEfree( stmtHndl->sh_queryText );
	stmtHndl->sh_queryText = text;
    }

    return( success );
}



/*
** Name: append_using_clause
**
** Description:
**	Modifies the query text by appending USING ~V . . .
**	Determines actual number of query parameters from
**	parameter descriptor.  No changes are made if no
**	actual query parameters.
**
** Input:
**	stmtHndl	Statement handle of query.
**
** Output:
**	None.
**
** Returns:
**	II_BOOL		TRUE if success, FALSE if memory allocation failure.
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
*/

static II_BOOL
append_using_clause( IIAPI_STMTHNDL *stmtHndl )
{
    i4		i, length;
    II_INT2	parmCount;
    char	*text;
    STATUS	status;
    II_BOOL	success = TRUE;

    /*
    ** We need to know the actual number of query parameters.  
    ** Since some parameters are required by the API and we 
    ** don't require all parameters to be sent at the same time,
    ** there is no single value available with the needed info.  
    ** So we calculate the value by subtracting the number of API 
    ** required parameters (which are all grouped at the start of 
    ** the parameters, if present at all) from the total number of 
    ** parameters.
    */
    for( 
	 i = 0, parmCount = stmtHndl->sh_parmCount;
	 i < stmtHndl->sh_parmCount  &&  
	 stmtHndl->sh_parmDescriptor[ i ].ds_columnType == IIAPI_COL_SVCPARM;
	 i++, parmCount--
       );

    /*
    ** Nothing to do if no query parameters.
    */
    if ( parmCount > 0 )
    {
	length = STlength( stmtHndl->sh_queryText ) + 
		 API_USING_CLAUSE_LENGTH( parmCount ) + 1;

	if ( ! ( text = MEreqmem( 0, length, FALSE, &status ) ) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "append_using_clause: can't allocate text buffer\n" );
	    success = FALSE;
	}
	else
	{
	    STpolycat( 2, stmtHndl->sh_queryText, api_qt_using, text );
	    while( --parmCount )  STcat( text, api_qt_marker );

	    MEfree( stmtHndl->sh_queryText );
	    stmtHndl->sh_queryText = text;
	}
    }

    return( success );
}



/*
** Name: calcProcParamCount
**
** Description:
**	Calculates how many of the given parameter set
**	can be formatted into a GCA buffer (reserving
**	some space for additional info) as GCA_P_PARAMs.
**	If force is true, the caller must assure that the
**	first parameter in the set is not a BLOB.
**
** Input:
**	parmCount	Number of parameters in set.
**	descriptor	API parameter descriptors.
**	dataValue	API data values.
**	moreSegments	TRUE if last parm is BLOB with additional segments.
**	buff_max	Max GCA buffer length.
**	reserve		Length of additional overhead for GCA info.
**	force		TRUE if at least one parm should be sent.
**
** Output:
**	None.
**
** Returns:
**	II_INT2		Number of parms that will fit (<= parmCount).
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
**	 3-Apr-96 (gordy)
**	    BLOBs may now be sent using the formatted GCA interface.
**	    Only NULL BLOBs will be processed with the other columns.
**	 6-Oct-04 (gordy)
**	    Buffer size now passed as parameter.
**	25-Oct-06 (gordy)
**	    IIapi_getGCALength() now handles NULL LOBs.
*/

static II_INT2
calcProcParamCount
( 
    II_INT2 		parmCount, 
    IIAPI_DESCRIPTOR 	*descriptor, 
    IIAPI_DATAVALUE	*dataValue,
    II_BOOL		moreSegments,
    II_LONG		buff_max,
    II_LONG 		reserve,
    II_BOOL		force
)
{
    II_INT2 count;

    /*
    ** First check to see if we should even
    ** bother attempting to send a parameter.
    */
    if ( parmCount < 1 )  return( 0 );

    if ( reserve >= buff_max )  
	count = 0;
    else
    {
	buff_max -= reserve;

	for( count = 0; count < parmCount; count++ )
	{
	    II_LONG length;

	    /*
	    ** NULL LOBs can be processed with other parameters.  
	    ** Otherwise, process LOB separately.
	    */
	    if ( IIapi_isBLOB( descriptor[ count ].ds_dataType )  &&
		 ! (descriptor[ count ].ds_nullable  &&
		    dataValue[ count ].dv_null) )
		break;

	    /*
	    ** Determine space required for parameter
	    ** formatted as a GCA_P_PARAM.  Remove
	    ** required space from buffer if it fits,
	    ** otherwise, no more parameters can be sent.
	    */
	    length = sizeof( GCA_P_PARAM ) +
			(descriptor[ count ].ds_columnName 
			 ? STlength( descriptor[count].ds_columnName ) : 0) +
			IIapi_getGCALength( &descriptor[ count ] );

	    if ( length > buff_max )  break;
	    buff_max -= length;
	}
    }

    /*
    ** When requested, we must send one parameter
    ** even if it is longer than our self-imposed
    ** limit.  If the first parameter in the set 
    ** is a BLOB, the caller should have processed 
    ** it using a different mechanism.
    */
    if ( force  &&  ! count  &&  ! IIapi_isBLOB( descriptor[0].ds_dataType ) )
	count++;

    return( count );
}


/*
** Name: calcDataValueCount
**
** Description:
**	Calculates how many of the given parameter set
**	can be formatted into a GCA buffer (reserving
**	some space for additional info) as GCA_DATA_VALUEs.
**	If force is true, the caller must assure that the
**	first parameter in the set is not a BLOB.
**
** Input:
**	parmCount	Number of parameters in set.
**	descriptor	API parameter descriptors.
**	dataValue	API data value.
**	moreSegments	TRUE if last parm is BLOB with additional segments.
**	buff_max	Max length of GCA buffer.
**	reserve		Length of additional overhead for GCA info.
**	force		TRUE if at least one parm should be sent.
**
** Output:
**	None.
**
** Returns:
**	II_INT2		Number of parms that will fit (<= parmCount).
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
**	 3-Apr-96 (gordy)
**	    BLOBs may now be sent using the formatted GCA interface.
**	    Only NULL BLOBs will be processed with the other columns.
**	 6-Oct-04 (gordy)
**	    Buffer size now passed as parameter.
**	25-Oct-06 (gordy)
**	    IIapi_getGCALength() now handles NULL LOBs.
*/

static II_INT2
calcDataValueCount
( 
    II_INT2 		parmCount, 
    IIAPI_DESCRIPTOR 	*descriptor, 
    IIAPI_DATAVALUE	*dataValue,
    II_BOOL		moreSegments,
    II_LONG		buff_max,
    II_LONG 		reserve,
    II_BOOL		force
)
{
    II_INT2 count;

    /*
    ** First check to see if we should even
    ** bother attempting to send a parameter.
    */
    if ( parmCount < 1 )  return( 0 );

    if ( reserve >= buff_max )  
	count = 0;
    else
    {
	buff_max -= reserve;

	for( count = 0; count < parmCount; count++ )
	{
	    II_LONG length;

	    /*
	    ** NULL LOBs can be processed with other parameters.  
	    ** Otherwise, process LOB separately.
	    */
	    if ( IIapi_isBLOB( descriptor[ count ].ds_dataType )  &&
		 ! (descriptor[ count ].ds_nullable  &&
		    dataValue[ count ].dv_null) )
	    	break;

	    /*
	    ** Determine space required for parameter
	    ** formatted as a GCA_DATA_VALUE.  Remove
	    ** required space from buffer if it fits,
	    ** otherwise, no more parameters can be sent.
	    */
	    length = sizeof( GCA_DATA_VALUE ) + 
			IIapi_getGCALength( &descriptor[ count ] );

	    if ( length > buff_max )  break;
	    buff_max -= length;
	}
    }

    /*
    ** When requested, we must send one parameter
    ** even if it is longer than our self-imposed
    ** limit.  If the first parameter in the set 
    ** is a BLOB, the caller should have processed 
    ** it using a different mechanism.
    */
    if ( force  &&  ! count  &&  ! IIapi_isBLOB( descriptor[0].ds_dataType ) )
	count++;

    return( count );
}


/*
** Name: allocDataValues
**
** Description:
**	Allocates a GCA data value array.
**
** Input:
**	parmNmem	Parameter Memory Info
**	parmCount	Number of parameters.
**
** Output:
**	None.
**
** Returns:
**	GCD_DATA_VALUE *	Data value array.
**
** History:
**	20-Jun-95 (gordy)
**	    Extracted from allocQueryBuffer().
*/

static GCD_DATA_VALUE *
allocDataValues
(
   IIAPI_PARMNMEM	*parmNmem,
   II_INT2		parmCount
)
{
    GCD_DATA_VALUE	*qDataValue;
    STATUS		status;
    
    parmNmem->pm_parmLen = parmCount * sizeof(GCD_DATA_VALUE);
    
    if ( ! ( qDataValue = (GCD_DATA_VALUE *)MEreqmem( parmNmem->pm_memTag, 
						      parmNmem->pm_parmLen, 
						      FALSE, &status ) ) )
    {
	IIAPI_TRACE(IIAPI_TR_FATAL)
	    ( "allocDataValues: can't allocate GCD_DATA_VALUE\n" );
	return( NULL );
    }

    parmNmem->pm_parmBlock = (II_PTR)qDataValue;

    return( qDataValue );
}



/*
** Name: calcQueryCount
**
** Description:
**	Calculates how many of the given parameter set
**	can be formatted into the GCA buffer length
**	as GCA_DATA_VALUEs in the initial GCA_QUERY
**	message which includes the query text as 1st
**	parameter.
**
** Input:
**	stmtHndl	Statement handle of query.
**	parmCount	Number of parameters in set.
**	descriptor	API parameter descriptors.
**	dataValue	API data values.
**	moreSegments	TRUE if last parm is BLOB with additional segments.
**	buff_max	Max GCA buffer length.
**	reserve		Length of additional overhead for GCA parms.
**
** Output:
**	None.
**
** Returns:
**	II_INT2		Number of parms that will fit (<= parmCount).
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
**	 6-Oct-04 (gordy)
**	    Buffer size now passed as parameter.
*/

static II_INT2
calcQueryCount
( 
    IIAPI_STMTHNDL	*stmtHndl,
    II_INT2 		parmCount, 
    IIAPI_DESCRIPTOR 	*descriptor,
    IIAPI_DATAVALUE	*dataValue,
    II_BOOL		moreSegments,
    II_LONG		buff_max,
    II_LONG		reserve
)
{
    /*
    ** The GCA_QUERY overhead includes the initial GCA_QUERY data
    ** object and the query text sent as the first parameter.
    */
    reserve += sizeof(GCA_Q_DATA) + STlength( stmtHndl->sh_queryText );

    parmCount = calcDataValueCount( parmCount, descriptor, 
				    dataValue, moreSegments, 
				    buff_max, reserve, FALSE );

    return( parmCount );
}


/*
** Name: allocQueryBuffer
**
** Description:
**	Allocates a buffer for a GCA_QUERY message and optional
**	parameters.  A pointer to the start of the parameters in
**	the data value array is returned.
**
** Input:
**	stmtHndl	Statement Handle of Query.
**	parmNmem	Parameter Memory info, may be unallocated.
**	parmCount	Number of optional parameters.
**	svcparms	Number of GCA parameters.
**
** Output:
**	parmNmem	Parameter Memory info if unallocated upon entry.
**
** Returns:
**	GCD_DATA_VALUE *	Start of parameters data values.
**
** History:
**	11-Jan-95 (gordy)
**	    Cleaning up descriptor handling.
**	20-Jun-95 (gordy)
**	    Extracted data value allocation to allocDataValues().
**	 9-Jul-96 (gordy)
**	    Transaction statements not permitted for these queries.
**	29-May-97 (gordy)
**	    Split large query texts into multiple GCA_DATA_VALUEs.
*/

static GCD_DATA_VALUE *
allocQueryBuffer
(
   IIAPI_STMTHNDL	*stmtHndl,
   IIAPI_PARMNMEM	*parmNmem,
   II_INT2		parmCount
)
{
    GCD_Q_DATA	*qData;
    char	*queryText = stmtHndl->sh_queryText;
    i4		queryLen = STlength( queryText ) + 1;
    i4		qryCount = ((queryLen + DB_MAXSTRING - 1) / DB_MAXSTRING);
    i4		i;
    STATUS	status;

    /*
    ** Send the full GCA_QUERY message
    ** with the query text as the first
    ** data value.
    */
    if ( ! ( parmNmem = IIapi_createGCAQuery( parmNmem, 
					      stmtHndl->sh_queryType,
					      NULL, FALSE,
					      (IIAPI_HNDL *)stmtHndl, 
					      parmCount + qryCount ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "allocQueryBuffer: can't create query\n" );
	return( NULL );
    }
	
    qData = ((GCD_Q_DATA *)parmNmem->pm_parmBlock);

    for( i = 0; i < qryCount; i++ )
    {
	qData->gca_qdata[i].gca_type = DB_QTXT_TYPE;
	qData->gca_qdata[i].gca_precision = 0;
	qData->gca_qdata[i].gca_l_value = min( queryLen, DB_MAXSTRING );
	qData->gca_qdata[i].gca_value = 
				MEreqmem( parmNmem->pm_memTag, 
					  qData->gca_qdata[i].gca_l_value, 
					  FALSE, &status );

	if ( ! qData->gca_qdata[i].gca_value )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "allocQueryBuffer: can't allocate text buffer\n" );
	    return( NULL );
	}

	MEcopy( (PTR)queryText, qData->gca_qdata[i].gca_l_value,
				qData->gca_qdata[i].gca_value );
	queryText += qData->gca_qdata[i].gca_l_value;
	queryLen -= qData->gca_qdata[i].gca_l_value;
    }

    /*
    ** We are done with the query text.
    */
    MEfree( stmtHndl->sh_queryText );
    stmtHndl->sh_queryText = NULL;

    /*
    ** Query text is sent as first data values.
    ** Return pointer to next available entry.
    */
    return( &qData->gca_qdata[qryCount] );
}



/*
** Name: copyQueryDataValue
**
** Description:
**	Copies parameter values into the query data value array.
**
** History:
**	11-Jan-95 (gordy)
**	    Cleaning up descriptor handling.
*/

static II_BOOL
copyQueryDataValue 
(
    II_UINT2		memTag,
    GCD_DATA_VALUE	*qDataValue,
    IIAPI_DATAVALUE	*parmData,
    IIAPI_DESCRIPTOR	*descriptor,
    II_LONG		parmCount
)
{
    i4		i;
    II_BOOL	success = TRUE;

    for( i = 0; success  &&  i < parmCount; i++ )
	success = IIapi_cnvtDataValue2GDV( memTag, &descriptor[ i ], 
					   &parmData[i], &qDataValue[i] );

    return( success );
}



/*
** Name: copyCursorID
**
** Description:
**	Copies the three part cursor ID into the query data value array.
**
** History:
**	11-Jan-95 (gordy)
**	    Cleaning up descriptor handling.
*/

static II_BOOL
copyCursorID 
(
    II_UINT2		memTag,
    GCD_DATA_VALUE	*qDataValue,
    II_LONG		cursorIndex1,
    II_LONG		cursorIndex2,
    II_INT2		cursorNameLength,
    char		*cursorName
)
{
    STATUS	status;
    
    qDataValue[0].gca_type = IIAPI_INT_TYPE;
    qDataValue[0].gca_precision = 0;
    qDataValue[0].gca_l_value = sizeof(II_LONG);

    if ( ! ( qDataValue[0].gca_value =
	     MEreqmem( memTag, sizeof(II_LONG), FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )( "copyCursorID: can't create GCA_ID\n" );
	return( FALSE );
    }

    *((II_LONG *)qDataValue[0].gca_value) = cursorIndex1;
    
    qDataValue[1].gca_type = IIAPI_INT_TYPE;
    qDataValue[1].gca_precision = 0;
    qDataValue[1].gca_l_value = sizeof(II_LONG);

    if ( ! ( qDataValue[1].gca_value =
	     MEreqmem( memTag, sizeof(II_LONG), FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )( "copyCursorID: can't create GCA_ID\n" );
	return( FALSE );
    }

    *((II_LONG *)qDataValue[1].gca_value) = cursorIndex2;
    
    qDataValue[2].gca_type = IIAPI_CHA_TYPE;
    qDataValue[2].gca_precision = 0;
    qDataValue[2].gca_l_value = GCA_MAXNAME;

    if ( ! ( qDataValue[2].gca_value =
	     MEreqmem( memTag, GCA_MAXNAME, FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )( "copyCursorID: can't create GCA_ID\n" );
	return( FALSE );
    }

    MEcopy( cursorName, cursorNameLength, qDataValue[2].gca_value );
    MEfill( GCA_MAXNAME - cursorNameLength, ' ',
	    qDataValue[2].gca_value + cursorNameLength );
    
    return( TRUE );
}


/*
** Name: copyBLOBSegment
**
** Description:
**	Allocates a data buffer and formats a BLOB segment.
**	Updates the parameter processing values in the 
**	statement handle.
**
** Input:
**	stmtHndl	Statement handle of query.
**	parmNmem	Parameter Memory Info
**	descriptor	API data descriptor.
**	dataValue	API data value.
**	moreSegments	IIapi_putParms() segment indicator.
**
** Output:
**
** Returns:
**	II_BOOL		TRUE if success, FALSE if memory allocation failure.
**
** History:
**	20-Jun-95 (gordy)
**	    Created.
**	 3-Apr-96 (gordy)
**	    BLOB segments may now be sent using the formatted interface.
**	 6-Oct-04 (gordy)
**	    Use the GCA suggested buffer size rather than the (now out-
**	    of-date) DBMS max tuple length.
**	28-Jun-06 (gordy)
**	    Support procedure OUT parameters (backward support as BYREF).
*/

static II_BOOL
copyBLOBSegment
(
    IIAPI_STMTHNDL	*stmtHndl,
    IIAPI_PARMNMEM	*parmNmem,
    IIAPI_DESCRIPTOR	*descriptor,
    IIAPI_DATAVALUE	*dataValue,
    II_BOOL		moreSegments
)
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    PTR			ptr;
    II_LONG		mlen;
    II_LONG		blen;
    II_BOOL		done;
    STATUS		status;

    /*
    ** We send at most the GCA suggested buffer size 
    ** to the server.  This may be shortened if the
    ** the segment is shorter.  The segment will be 
    ** split if longer.
    */
    parmNmem->pm_parmLen = connHndl->ch_sizeAdvise;
    
    if ( ! ( parmNmem->pm_parmBlock = MEreqmem( parmNmem->pm_memTag, 
					        parmNmem->pm_parmLen, 
						FALSE, &status ) ) )
    {
	IIAPI_TRACE( IIAPI_TR_FATAL )
	    ( "copyBLOBSegment: can't allocate BLOB buffer\n" );
	return( FALSE );
    }

    if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )
	mlen = 0;	/* No GCA message header, just segment */
    else
    {
	GCD_DATA_VALUE	*gdv;

	/*
	** This is the first segment of the BLOB.  Need
	** to send the GCA_DATA_VALUE info.  If this is a
	** procedure parameter, the GCA_DATA_VALUE info
	** is preceded by the GCA_P_PARAM info.
	*/
	if ( 
	     descriptor->ds_columnType != IIAPI_COL_PROCINPARM  &&
	     descriptor->ds_columnType != IIAPI_COL_PROCOUTPARM  &&
	     descriptor->ds_columnType != IIAPI_COL_PROCINOUTPARM &&
  	     descriptor->ds_columnType != IIAPI_COL_PROCGTTPARM 
	   )
	{
	    mlen = sizeof( GCD_DATA_VALUE );
	    gdv = (GCD_DATA_VALUE *)parmNmem->pm_parmBlock;
	}
	else
	{
	    GCD_P_PARAM	*pp = (GCD_P_PARAM *)parmNmem->pm_parmBlock;

	    pp->gca_parname.gca_l_name = STlength( descriptor->ds_columnName );

	    if ( ! (pp->gca_parname.gca_name = 
				MEreqmem( parmNmem->pm_memTag, 
					  pp->gca_parname.gca_l_name + 1, 
					  FALSE, &status )) )
	    {
		IIAPI_TRACE( IIAPI_TR_FATAL )
		    ( "copyBLOBSegment: can't allocate parameter name\n" );
		return( FALSE );
	    }

	    STcopy( descriptor->ds_columnName, pp->gca_parname.gca_name );
	    pp->gca_parm_mask = 0;

	    /*
	    ** Output parameters backward supported as BYREF.
	    */
	    switch( descriptor->ds_columnType )
	    {
	    case IIAPI_COL_PROCOUTPARM :
		if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_66 )
		    pp->gca_parm_mask |= GCA_IS_OUTPUT;
		else
		    pp->gca_parm_mask |= GCA_IS_BYREF;
		break;

	    case IIAPI_COL_PROCINOUTPARM :
		pp->gca_parm_mask |= GCA_IS_BYREF;
		break;

	    case IIAPI_COL_PROCGTTPARM :
 	        pp->gca_parm_mask |= GCA_IS_GTTPARM;
		break;
	    }

	    mlen = sizeof( GCD_P_PARAM );
	    gdv = &pp->gca_parvalue;
	}

	IIapi_cnvtDescr2GDV( descriptor, gdv );
	gdv->gca_value = (char *)parmNmem->pm_parmBlock + mlen;
    }

    /*
    ** Determine location of BLOB segment in the
    ** message buffer (immediatly following the
    ** GCA mesage, if any) and space remaining.
    */
    ptr = (char *)parmNmem->pm_parmBlock + mlen;
    blen = parmNmem->pm_parmLen - mlen;

    /*
    ** This is the last segment if this is not
    ** the last column in the set.  Otherwise,
    ** the application has indicated if this is
    ** the last segment.
    */
    if ( stmtHndl->sh_parmSend > 1 )  moreSegments = FALSE;

    done = IIapi_cnvtBLOB2GCATuple( stmtHndl, descriptor, 
				    dataValue, moreSegments, 
				    (char *)ptr, &blen );

    /*
    ** The message length is the GCA message size, if any.
    ** Otherwise, it is the formatted segment length.
    */
    parmNmem->pm_parmLen = mlen ? mlen : blen;

    /*
    ** If the segment was split we leave the
    ** parameter as if it wasn't processed:
    ** the first part of the segment will be
    ** sent and we'll come back to finish the
    ** remainder.
    */
    if ( done )
    {
	/*
	** We have at least finished this part
	** of the request.  We may have also
	** finished the parameter.
	*/
	stmtHndl->sh_parmSend--;
	if ( ! moreSegments )  stmtHndl->sh_parmIndex++;
    }

    return( TRUE );
}


/*
** Name: get_tz_offset
**
** Description:
**	Given a timezone name, returns the timezone offset
**	from GMT in minutes.  The offset is in the format
**	used by the GCA_MD_ASSOC parameter GCA_TIMEZONE.
**
** Input:
**	tz_name		Timezone name.
**
** Output:
**	None.
**
** Returns:
**	i4		Timezone offset.
**
** History:
**	23-Mar-01 (gordy)
**	    Created.
*/

static i4
get_tz_offset( char *tz_name )
{
    II_LONG	tmsecs;
    PTR		tz_cb;
    i4		offset;
    STATUS	status;

    /*
    ** TMtz_search requires a GMT or time local to the 
    ** timezone.  We don't have either of these, but we 
    ** can get GMT time from our local time by adjusting 
    ** for our gmt offset.
    */
    if ( (tmsecs = TMsecs()) > MAXI4 )  tmsecs = MAXI4;
    offset = TMtz_search( IIapi_static->api_tz_cb, TM_TIMETYPE_LOCAL, tmsecs );
    tmsecs -= offset;

    /*
    ** Get the timezone control block for the requested timezone.
    */
    status = TMtz_lookup( tz_name, &tz_cb );
    if ( status == TM_TZLKUP_FAIL )
    {
	MUp_semaphore( &IIapi_static->api_semaphore );
	status = TMtz_load( tz_name, &tz_cb );
	MUv_semaphore( &IIapi_static->api_semaphore );
    }

    if ( status != OK )  
    {
	/*
	** We have already gotten our local timezone
	** offset, so go ahead and use that instead.
	*/
	IIAPI_TRACE( IIAPI_TR_ERROR )
	    ( "get_tz_offset: invalid timezone '%s'\n", tz_name );
    }
    else
    {
	/*
	** Now, get the timezone offset of the requested timezone,
	** using the current GMT time.
	*/
        offset = TMtz_search( tz_cb, TM_TIMETYPE_GMT, tmsecs );
    }

    /*
    ** Adjust to format expected by GCA_MD_ASSOC.
    */
    return( -offset / 60 );
}

