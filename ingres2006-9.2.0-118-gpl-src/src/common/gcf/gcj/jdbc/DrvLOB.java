/*
** Copyright (c) 2007 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.jdbc;

/*
** Name: DrvLOB.java
**
** Description:
**	Defines class which provides basic support methods
**	for accessing DBMS LOB data via a LOB Locator.
**
**  Classes:
**
**	DrvLOB		Locator based DBMS LOB access.
**	ByteStream	InputStream providing access to LOB.
**	CharStream	Reader providing access to LOB.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	30-Jan-07 (gordy)
**	    Backward compatibility for LOB Locators.
**	26-Feb-07 (gordy)
**	    Provided capability for immediate access to LOB data
**	    for better performance under restricted conditions.
**	 4-May-07 (gordy)
**	    Set class access for reflection.
*/

import	java.io.InputStream;
import	java.io.ByteArrayInputStream;
import	java.io.Reader;
import	java.io.StringReader;
import	java.io.IOException;
import	java.sql.Types;
import	java.sql.SQLException;
import	com.ingres.gcf.util.DbmsConst;
import	com.ingres.gcf.util.SqlStream;
import	com.ingres.gcf.util.SqlLongByte;
import	com.ingres.gcf.util.SqlLongChar;
import	com.ingres.gcf.util.SqlLongNChar;
import	com.ingres.gcf.util.SqlEx;


/*
** Name: DrvLOB
**
** Description:
**	JDBC driver class which provides basic support methods
**	for accessing DBMS LOB data via a LOB Locator.
**
**  Public Methods:
**
**	toString		Return object name.
**	getType			Return locator type.
**	getLocator		Return locator value.
**	hasSameDomain		Is locator domain the same.
**	streamClosed		Stream closure callback.
**
**  Protected Data:
**
**	loc_type		DBMS specific variant.
**	locator			DBMS LOB Locator.
**
**	resultType		DBMS type of query result.
**	resultLength		Length of query result.
**	intResult		Integer result value.
**	baResult		Byte array result value.
**	stringResult		String result value.
**	bsResult		Byte stream result value.
**	csResult		Char stream result value.
**
**  Protected Methods:
**
**	length			Get length of LOB.
**	getBytes		Read LOB sub-array.
**	getChars		Read LOB sub-string.
**	position		Search LOB.
**	getByteStream		Returns stream to access LOB.
**	getCharStream		Returns stream to access LOB.
**
**	sendQuery		Send QUERY message.
**	startDesc		Initialize DESC message.
**	descParam		Describe parameter meta-data.
**	sendParam		Send parameter value.
**	initResults		Initialize Query results.
**	readDesc		Overrides DrvObj method.
**	readData		Overrides DrvObj method.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	30-Jan-07 (gordy)
**	    Added descParam() and sendParam() methods for streams.
**	26-Feb-07 (gordy)
**	    Added support for StreamListener interface with streamClosed()
**	    method to complete result processing once stream is consumed.
**	 4-May-07 (gordy)
**	    Class is not exposed outside package, restrict access.
*/

class	// package access
DrvLOB
    extends DrvObj
    implements DbmsConst, SqlStream.StreamListener
{

    /*
    ** Locator info indentifying LOB.
    */
    protected short		loc_type = -1;	// DBMS specific variant
    protected int		locator = 0;	// DBMS LOB Locator

    /*
    ** Query results provided by readDesc(), readData().
    */
    protected short		resultType;
    protected short		resultLength;
    protected long		intResult;
    protected byte[]		baResult;
    protected String		stringResult;
    protected InputStream	bsResult;
    protected Reader		csResult;


/*
** Name: DrvLOB
**
** Description:
**	Class constructor.
**
** Input:
**	conn		DBMS Connection.
**	loc_type	DBMS specific variant.
**	locator		DBMS LOB Locator.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected
DrvLOB( DrvConn conn, short loc_type, int locator )
{
    super( conn );
    this.loc_type = loc_type;
    this.locator = locator;
} // DrvLOB


/*
** Name: toString
**
** Description:
**	Returns a descriptive name for this object.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String	Object name.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public String
toString()
{
    return( Integer.toString( locator ) );
} // toString


/*
** Name: getType
**
** Description:
**	Returns the DBMS specific varient type of the locator.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	short	DBMS type.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public short
getType()
{
    return( loc_type );
} // getType


/*
** Name: getLocator
**
** Description:
**	Returns the LOB Locator value.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	int	LOB Locator.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public int
getLocator()
{
    return( locator );
} // getLocator


/*
** Name: hasSameDomain
**
** Description
**	Returns an indication that the domain of a locator
**	is the same.  
**
**	Locators are only valid within their domain, which is 
**	the connection context in which they were created.
**
** Input:
**	lob	Locator reference to compare.
**
** Output:
**	None.
**
** Returns:
**	boolean	TRUE if domains are the same.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public boolean
hasSameDomain( DrvLOB lob )
{
    return( hasSameDomain( lob.conn ) );
} // hasSameDomain


/*
** Name: hasSameDomain
**
** Description
**	Returns an indication that the domain of a locator
**	is a particular connection.
**
**	Locators are only valid within their domain, which is 
**	the connection context in which they were created.
**
** Input:
**	conn	Target connection context.
**
** Output:
**	None.
**
** Returns:
**	boolean	TRUE if domain matches connection.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public boolean
hasSameDomain( DrvConn conn )
{
    return( this.conn == conn );
} // hasSameDomain


/*
** Name: length
**
** Description:
**	Returns the length of the LOB data referenced
**	by the locator.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	long	Length of LOB.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected long
length()
    throws SQLException
{
    long result = -1;

    msg.lock();
    try
    {
	sendQuery( "select length( ~V )" );

	startDesc( (short)1 );
	descParam();			// Locator
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam();			// Locator
	msg.done( true );

	initResults( DBMS_TYPE_INT );
	readResults();
	result = intResult;
	closeQuery();
    }
    finally
    {
	msg.unlock();
    }

    return( result );
} // length


/*
** Name: getBytes
**
** Description:
**	Extracts a binary array from the LOB data referenced 
**	by the locator.  
**
**	The result can be coerced into a variable length
**	array in the DBMS, which is handled more efficiently
**	in the driver but is limited by the maximum length
**	of varbyte data.
**
** Input:
**	pos	Starting position.
**	length	Length of array.
**	coerce	Coerce to variable-length array.
**
** Output:
**	None.
**
** Returns;
**	byte[]	Extracted byte array.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected byte[]
getBytes( long pos, int length, boolean coerce )
    throws SQLException
{
    byte result[] = null;

    msg.lock();
    try
    {
	sendQuery( coerce
		   ? "select varbyte( substring( ~V from ~V for ~V ), ~V )"
		   : "select substring( ~V from ~V for ~V )" );

	startDesc( (short)(coerce ? 4 : 3) );
	descParam();			// Locator
	descParam( (int)pos );
	descParam( length );
	if ( coerce )  descParam( length );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam();			// Locator
	sendParam( (int)pos );
	sendParam( length );
	if ( coerce )  sendParam( length );
	msg.done( true );

	initResults( coerce ? DBMS_TYPE_VARBYTE : DBMS_TYPE_LONG_BYTE );
	readResults();

	if ( coerce )
	    result = baResult;
	else
	{
	    /*
	    ** Read LOB data and convert to fixed-length array.
	    ** Close stream to ensure data message has been consumed.
	    */
	    result = SqlLongByte.strm2array( bsResult );
	    try { bsResult.close(); }
	    catch( IOException ignore ) {}

	    /*
	    ** Read remaining query results.
	    */
	    readResults();
	}

	closeQuery();
    }
    finally
    {
	msg.unlock();
    }

    return( result );
} // getBytes


/*
** Name: getChars
**
** Description:
**	Extracts a character string from the LOB data referenced 
**	by the locator.  Both long varchar and long nvarchar data
**	is supported, dependent on the type of locator being used.
**
**	The result can be coerced into a variable-length string 
**	in the DBMS, which is handled more efficiently in the 
**	driver but is limited by the maximum length of (n)varchar 
**	data.
**
** Input:
**	pos	Starting position.
**	length	Length of string.
**	coerce	Coerce to variable-length string.
**
** Output:
**	None.
**
** Returns;
**	String	Extracted string.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected String
getChars( long pos, int length, boolean coerce )
    throws SQLException
{
    String result = null;
    boolean ucs2 = (loc_type == DBMS_TYPE_LNLOC);

    msg.lock();
    try
    {
	if ( coerce )
	    sendQuery( "select " + (ucs2 ? "nvarchar" : "varchar") +
		       "( substring( ~V from ~V for ~V ), ~V )" );
	else
	    sendQuery( "select substring( ~V from ~V for ~V )" );

	startDesc( (short)(coerce ? 4 : 3) );
	descParam();			// Locator
	descParam( (int)pos );
	descParam( length );
	if ( coerce )  descParam( length );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam();			// Locator
	sendParam( (int)pos );
	sendParam( length );
	if ( coerce )  sendParam( length );
	msg.done( true );

	initResults( coerce ? (ucs2 ? DBMS_TYPE_NVARCHAR 
				    : DBMS_TYPE_VARCHAR)
			    : (ucs2 ? DBMS_TYPE_LONG_NCHAR 
				    : DBMS_TYPE_LONG_CHAR) );
	readResults();

	if ( coerce )
	    result = stringResult;
	else
	{
	    /*
	    ** Read LOB data and convert into fixed length string.  
	    ** Close stream to ensure data message has been consumed.
	    */
	    if ( ucs2 )
	    {
		result = SqlLongNChar.strm2str( csResult );
		try { csResult.close(); }
		catch( IOException ignore ) {}
	    }
	    else
	    {
		result = SqlLongChar.strm2str( bsResult, msg.getCharSet() );
		try { bsResult.close(); }
		catch( IOException ignore ) {}
	    }

	    /*
	    ** Read remaining query results.
	    */
	    readResults();
	}

	closeQuery();
    }
    finally
    {
	msg.unlock();
    }

    return( result );
} // getChars


/*
** Name: position
**
** Description:
**	Search LOB data referenced by the locator.
**
** Input:
**	pattern	Search pattern.
**	start	Starting position of search.
**
** Output:
**	None.
**
** Returns:
**	long	Position of match.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected long
position( byte pattern[], long start )
    throws SQLException
{
    long result = -1;

    msg.lock();
    try
    {
	sendQuery( "select position( ~V , ~V , ~V )" );

	startDesc( (short)3 );
	descParam( pattern );
	descParam();			// Locator
	descParam( (int)start );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam( pattern );
	sendParam();			// Locator
	sendParam( (int)start );
	msg.done( true );

	initResults( DBMS_TYPE_INT );
	readResults();
	result = intResult;
	closeQuery();
    }
    finally
    {
	msg.unlock();
    }

    return( result );
} // position


/*
** Name: position
**
** Description:
**	Search LOB data referenced by the locator.
**
** Input:
**	pattern	Search pattern.
**	start	Starting position of search.
**
** Output:
**	None.
**
** Returns:
**	long	Position of match.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected long
position( String pattern, long start )
    throws SQLException
{
    long result = -1;

    msg.lock();
    try
    {
	sendQuery( "select position( ~V , ~V , ~V )" );

	startDesc( (short)3 );
	descParam( pattern );
	descParam();			// Locator
	descParam( (int)start );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam( pattern );
	sendParam();			// Locator
	sendParam( (int)start );
	msg.done( true );

	initResults( DBMS_TYPE_INT );
	readResults();
	result = intResult;
	closeQuery();
    }
    finally
    {
	msg.unlock();
    }

    return( result );
} // position


/*
** Name: position
**
** Description:
**	Search LOB data referenced by the locator.
**
** Input:
**	pattern	Search pattern.
**	start	Starting position of search.
**
** Output:
**	None.
**
** Returns:
**	long	Position of match.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected long
position( DrvLOB pattern, long start )
    throws SQLException
{
    long result = -1;

    msg.lock();
    try
    {
	sendQuery( "select position( ~V , ~V , ~V )" );

	startDesc( (short)3 );
	descParam( pattern );
	descParam();			// Locator
	descParam( (int)start );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam( pattern );
	sendParam();			// Locator
	sendParam( (int)start );
	msg.done( true );

	initResults( DBMS_TYPE_INT );
	readResults();
	result = intResult;
	closeQuery();
    }
    finally
    {
	msg.unlock();
    }

    return( result );
} // position


/*
** Name: position
**
** Description:
**	Search LOB data referenced by the locator.
**
** Input:
**	pattern	Search pattern.
**	start	Starting position of search.
**
** Output:
**	None.
**
** Returns:
**	long	Position of match.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	30-Jan-07 (gordy)
**	    Handle stream parameter.
*/

protected long
position( InputStream pattern, long start )
    throws SQLException
{
    long result = -1;

    msg.lock();
    try
    {
	sendQuery( "select position( ~V , ~V , ~V )" );

	startDesc( (short)3 );
	descParam( pattern );
	descParam();			// Locator
	descParam( (int)start );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam( pattern );
	sendParam();			// Locator
	sendParam( (int)start );
	msg.done( true );

	initResults( DBMS_TYPE_INT );
	readResults();
	result = intResult;
	closeQuery();
    }
    finally
    {
	msg.unlock();
    }

    return( result );
} // position


/*
** Name: position
**
** Description:
**	Search LOB data referenced by the locator.
**
** Input:
**	pattern	Search pattern.
**	start	Starting position of search.
**
** Output:
**	None.
**
** Returns:
**	long	Position of match.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	30-Jan-07 (gordy)
**	    Handle stream parameter.
*/

protected long
position( Reader pattern, long start )
    throws SQLException
{
    long result = -1;

    msg.lock();
    try
    {
	sendQuery( "select position( ~V , ~V , ~V )" );

	startDesc( (short)3 );
	descParam( pattern );
	descParam();			// Locator
	descParam( (int)start );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam( pattern );
	sendParam();			// Locator
	sendParam( (int)start );
	msg.done( true );

	initResults( DBMS_TYPE_INT );
	readResults();
	result = intResult;
	closeQuery();
    }
    finally
    {
	msg.unlock();
    }

    return( result );
} // position


/*
** Name: getByteStream
**
** Description:
**	Returns an InputStream capable of accessing
**	the LOB data referenced by the locator.  
**
**	The stream can provide either immediate or 
**	deferred access.  With immediate access, the
**	LOB data is retrieved directly by this request
**	and the returned stream must be read completely
**	before any other request is permitted on the
**	connection.  With deferred access, no connection
**	request is made and subsequent requests to read
**	the stream will initiate and process independent
**	connection requests to read the LOB data.
**
**	Whether the stream is host character-set (CHAR)
**	or binary (BYTE) data depends on the type of 
**	LOB/locator.  Should not be called for NCHAR
**	locators.
**
** Input:
**	immediate	Request immediate access.
**
** Output:
**	None.
**
** Returns:
**	InputStream	LOB stream.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	26-Feb-07 (gordy)
**	    Added ability to return immediate or deferred streams.
*/

protected InputStream
getByteStream( boolean immediate )
    throws SqlEx
{
    /*
    ** For deferred access, an InputStream with the
    ** capability of sending locator based requests
    ** to the DBMS is returned.
    */
    if ( ! immediate )  return( new ByteStream() );

    /*
    ** For immediate access, the LOB data is retrieved
    ** from the database and the active data stream is
    ** returned.
    **
    ** Connection is unlocked if an exception occurs.
    ** Otherwise, connection will be unlocked once
    ** the result stream is read and the ...
    */
    msg.lock();
    try
    {
	sendQuery( "select ~V " );

	startDesc( (short)1 );
	descParam();			// Locator
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam();			// Locator
	msg.done( true );

	initResults( (loc_type == DBMS_TYPE_LCLOC) 
		     ? DBMS_TYPE_LONG_CHAR : DBMS_TYPE_LONG_BYTE );
	readResults();

	/*
	** Result processing has been interrupted by the
	** LOB data stream.  Set-up callback to finish
	** result processing once the stream has been
	** consumed.  The active stream is then returned.
	*/
	((SqlStream.StreamSource)bsResult).addStreamListener( this, null );
    }
    catch( SqlEx ex )
    {
	msg.unlock();
	throw ex;
    }

    return( bsResult );
} // getByteStream


/*
** Name: getCharStream
**
** Description:
**	Returns a Reader capable of accessing the LOB 
**	data referenced by the locator.
**
**	The stream can provide either immediate or 
**	deferred access.  With immediate access, the
**	LOB data is retrieved directly by this request
**	and the returned stream must be read completely
**	before any other request is permitted on the
**	connection.  With deferred access, no connection
**	request is made and subsequent requests to read
**	the stream will initiate and process independent
**	connection requests to read the LOB data.
**
**	Should only be called for NCHAR locators.
**
** Input:
**	immediate	Request immediate access.
**
** Output:
**	None.
**
** Returns:
**	Reader	LOB stream.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	26-Feb-07 (gordy)
**	    Added ability to return immediate or deferred streams.
*/

protected Reader
getCharStream( boolean immediate )
    throws SqlEx
{
    /*
    ** For deferred access, an InputStream with the
    ** capability of sending locator based requests
    ** to the DBMS is returned.
    */
    if ( ! immediate )  return( new CharStream() );

    /*
    ** For immediate access, the LOB data is retrieved
    ** from the database and the active data stream is
    ** returned.
    **
    ** Connection is unlocked if an exception occurs.
    ** Otherwise, connection will be unlocked once
    ** the result stream is read and the ...
    */
    msg.lock();
    try
    {
	sendQuery( "select ~V " );

	startDesc( (short)1 );
	descParam();			// Locator
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam();			// Locator
	msg.done( true );

	initResults( DBMS_TYPE_LONG_NCHAR );
	readResults();

	/*
	** Result processing has been interrupted by the
	** LOB data stream.  Set-up callback to finish
	** result processing once the stream has been
	** consumed.  The active stream is then returned.
	*/
	((SqlStream.StreamSource)csResult).addStreamListener( this, null );
    }
    catch( SqlEx ex )
    {
	msg.unlock();
	throw ex;
    }

    return( csResult );
} // getCharStream


/*
** Name: streamClosed
**
** Description:
**      Stream closure notification method for the 
**	StreamListener interface.
**
**	Result processing was interrupted by a LOB 
**	data stream and processing is resumed to 
**	complete the query.
**	
** Input:
**	ignored		Not used.
**
** Output:
**	None.
**
** Reurns:
**	void.
**
** History:
**	26-Feb-07 (gordy)
**	    Created.
*/

public void
streamClosed( SqlStream ignored )
{
    try
    {
	/*
	** Read remaining query results.
	*/
	readResults();
	closeQuery();
    }
    catch( SqlEx ex )
    {
        /*
        ** The caller of the Stream.Listener interface does
        ** not care about errors we may hit, so we can only
        ** trace the exception.  Errors aren't expected 
	** except in catastrophic circumstances, so the 
	** following should be OK.
        */
	if ( trace.enabled( 1 ) )
	{
	    trace.log( "DrvBlob: error completing result processing" );
	    ex.trace( trace );
	}
    }
    finally
    {
	msg.unlock();
    }

    return;
} // streamClosed


/*
** Name: SendQuery
**
** Description:
**	Format and send QUERY message.
**
** Input:
**	query	Query text.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
sendQuery( String query )
    throws SqlEx
{
    /*
    ** Locators are only supported at protocol level 6
    ** and above, so conditional query features at lower
    ** protocol levels (select loops and query flags)
    ** are assumed to be supported.
    */
    msg.begin( MSG_QUERY );
    msg.write( MSG_QRY_EXQ );
    msg.write( MSG_QP_FLAGS );
    msg.write( (short)4 );
    msg.write( MSG_QF_FETCH_ALL | MSG_QF_AUTO_CLOSE );
    msg.write( MSG_QP_QTXT );
    msg.write( query );
    msg.done( false );
    return;
} // sendQuery


/*
** Name: startDesc
**
** Description:
**	Format initial portion of DESC message.  A descXXX()
**	method call should follow for each descriptor entry.
**	DESC message should be completed with msg.done( false ).
**
** Input:
**	count	Number of descriptor entries.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
startDesc( short count )
    throws SqlEx
{
    msg.begin( MSG_DESC );
    msg.write( count );
    return;
} // startDesc


/*
** Name: descParam
**
** Description:
**	Format DESC entry for this locator object.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
descParam()
    throws SqlEx
{
    short sql_type = (short)Types.NULL;

    switch( loc_type )
    {
    case DBMS_TYPE_LBLOC :  sql_type = (short)Types.LONGVARBINARY;	break;
    case DBMS_TYPE_LCLOC :  sql_type = (short)Types.LONGVARCHAR;	break;
    case DBMS_TYPE_LNLOC :  sql_type = (short)Types.LONGVARCHAR;	break;
    }

    descParam( sql_type, loc_type, (short)0 );
    return;
} // descParam


/*
** Name: descParam
**
** Description:
**	Format DESC entry for external locator object.
**
** Input:
**	lob	External locator object.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
descParam( DrvLOB lob )
    throws SqlEx
{
    short sql_type = (short)Types.NULL;

    switch( lob.loc_type )
    {
    case DBMS_TYPE_LBLOC :  sql_type = (short)Types.LONGVARBINARY;	break;
    case DBMS_TYPE_LCLOC :  sql_type = (short)Types.LONGVARCHAR;	break;
    case DBMS_TYPE_LNLOC :  sql_type = (short)Types.LONGVARCHAR;	break;
    }

    descParam( sql_type, loc_type, (short)0 );
    return;
} // descParam


/*
** Name: descParam
**
** Description:
**	Format DESC entry for integer value.
**
** Input:
**	value	Integer value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
descParam( int value )
    throws SqlEx
{
    descParam( (short)Types.INTEGER, DBMS_TYPE_INT, (short)4 );
    return;
} // descParam


/*
** Name: descParam
**
** Description:
**	Format DESC entry for byte array.
**
** Input:
**	value	Byte array.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
descParam( byte value[] )
    throws SqlEx
{
    int length = value.length;

    if ( length > conn.max_vbyt_len )
	descParam( (short)Types.LONGVARBINARY, DBMS_TYPE_LONG_BYTE, (short)0 );
    else
	descParam( (short)Types.VARBINARY, DBMS_TYPE_VARBYTE, (short)length );
    return;
} // descParam


/*
** Name: descParam
**
** Description:
**	Format DESC entry for string value.
**
** Input:
**	value	String value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
descParam( String value )
    throws SqlEx
{
    int		length = value.length();
    boolean	ucs2 = (loc_type == DBMS_TYPE_LNLOC);
    short	sql_type, dbms_type;

    /*
    ** Select DBMS type based on locator type.
    */
    if ( length > (ucs2 ? conn.max_nvch_len : conn.max_vchr_len) )
    {
	sql_type = (short)Types.LONGVARCHAR;
	dbms_type = ucs2 ? DBMS_TYPE_LONG_NCHAR : DBMS_TYPE_LONG_CHAR;
	length = 0;
    }
    else  if ( ucs2 )
    {
	sql_type = (short)Types.VARCHAR;
	dbms_type = DBMS_TYPE_NVARCHAR;
	length *= 2;
    }
    else
    {
	sql_type = (short)Types.VARCHAR;
	dbms_type = DBMS_TYPE_VARCHAR;
    }

    descParam( sql_type, dbms_type, (short)length );
    return;
} // descParam


/*
** Name: descParam
**
** Description:
**	Format DESC entry for byte stream.
**
**	Note: the byte stream is treated as a binary stream.
**	Character streams should use the Reader interface.
**
** Input:
**	value	Byte stream.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	30-Jan-07 (gordy)
**	    Created.
*/

protected void
descParam( InputStream value )
    throws SqlEx
{
    descParam( (short)Types.LONGVARBINARY, DBMS_TYPE_LONG_BYTE, (short)0 );
    return;
} // descParam


/*
** Name: descParam
**
** Description:
**	Format DESC entry for character stream.
**
** Input:
**	value	Character stream.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	30-Jan-07 (gordy)
**	    Created.
*/

protected void
descParam( Reader value )
    throws SqlEx
{
    descParam( (short)Types.LONGVARCHAR, 
	       (loc_type == DBMS_TYPE_LNLOC) ? DBMS_TYPE_LONG_NCHAR 
					     : DBMS_TYPE_LONG_CHAR, 
	       (short)0 );
    return;
} // descParam


/*
** Name: descParam
**
** Description:
**	Format DESC entry.
**
** Input:
**	sql_type	SQL Type.
**	dbms_type	DBMS Type.
**	length		Length.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
descParam( short sql_type, short dbms_type, short length )
    throws SqlEx
{
    msg.write( sql_type );	// SQL type
    msg.write( dbms_type );	// DBMS type
    msg.write( length );	// Length
    msg.write( (byte)0 );	// Precision
    msg.write( (byte)0 );	// Scale
    msg.write( (byte)0 );	// Flags
    msg.write( (short)0 );	// Parameter name
    return;
} // descParam


/*
** Name: sendParam
**
** Description:
**	Send this locator object as a parameter DATA value.
**
**	DATA message should have been initialized with
**	msg.begin( MSG_DATA ) prior to calling this method
**	and should be completed with msg.done( true ).
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
sendParam()
    throws SqlEx
{
    msg.write( (byte)1 );	// Data indicator
    msg.write( locator );	// LOB Locator
    return;
} // sendParam


/*
** Name: sendParam
**
** Description:
**	Send a locator object as a parameter DATA value.
**
**	DATA message should have been initialized with
**	msg.begin( MSG_DATA ) prior to calling this method
**	and should be completed with msg.done( true ).
**
** Input:
**	lob	Locator object to send.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
sendParam( DrvLOB lob )
    throws SqlEx
{
    msg.write( (byte)1 );	// Data indicator
    msg.write( lob.locator );	// LOB Locator
    return;
} // sendParam


/*
** Name: sendParam
**
** Description:
**	Send an integer as a parameter DATA value.
**
**	DATA message should have been initialized with
**	msg.begin( MSG_DATA ) prior to calling this method
**	and should be completed with msg.done( true ).
**
** Input:
**	value	Integer value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
sendParam( int value )
    throws SqlEx
{
    msg.write( (byte)1 );	// Data indicator
    msg.write( value );
    return;
} // sendParam


/*
** Name: sendParam
**
** Description:
**	Send a byte array as a parameter DATA value.
**
**	DATA message should have been initialized with
**	msg.begin( MSG_DATA ) prior to calling this method
**	and should be completed with msg.done( true ).
**
** Input:
**	value	Byte array.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
sendParam( byte value[] )
    throws SqlEx
{
    msg.write( (byte)1 );	// Data indicator

    if ( value.length > conn.max_vbyt_len )
	msg.write( new ByteArrayInputStream( value ) );
    else
	msg.write( value );

    return;
} // sendParam


/*
** Name: sendParam
**
** Description:
**	Send a string as a parameter DATA value.
**
**	DATA message should have been initialized with
**	msg.begin( MSG_DATA ) prior to calling this method
**	and should be completed with msg.done( true ).
**
** Input:
**	value	String value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
sendParam( String value )
    throws SqlEx
{
    boolean	ucs2 = (loc_type == DBMS_TYPE_LNLOC);

    msg.write( (byte)1 );	// Data indicator

    if ( value.length() > (ucs2 ? conn.max_nvch_len : conn.max_vchr_len) )
    {
	Reader rdr = new StringReader( value );

	if ( ucs2 )
	    msg.writeUCS2( rdr );
	else
	    msg.write( rdr );
    }
    else  if ( ucs2 )
	msg.writeUCS2( value );
    else
	msg.write( value );

    return;
} // sendParam


/*
** Name: sendParam
**
** Description:
**	Send a byte stream as a parameter DATA value.
**
**	Note: the byte stream is treated as a binary stream.
**	Character streams should use the Reader interface.
**
**	DATA message should have been initialized with
**	msg.begin( MSG_DATA ) prior to calling this method
**	and should be completed with msg.done( true ).
**
** Input:
**	value	Byte stream.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	30-Jan-07 (gordy)
**	    Created.
*/

protected void
sendParam( InputStream value )
    throws SqlEx
{
    msg.write( (byte)1 );	// Data indicator
    msg.write( value );
    return;
} // sendParam


/*
** Name: sendParam
**
** Description:
**	Send a character stream as a parameter DATA value.
**
**	DATA message should have been initialized with
**	msg.begin( MSG_DATA ) prior to calling this method
**	and should be completed with msg.done( true ).
**
** Input:
**	value	Character stream.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	30-Jan-07 (gordy)
**	    Created.
*/

protected void
sendParam( Reader value )
    throws SqlEx
{
    msg.write( (byte)1 );	// Data indicator

    if ( loc_type == DBMS_TYPE_LNLOC )
	msg.writeUCS2( value );
    else
	msg.write( value );
    return;
} // sendParam


/*
** Name: closeQuery
**
** Description:
**	Closes an active query.  This method is a NO-OP
**	if the query was automatically closed at the
**	completion of initial processing.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
closeQuery()
    throws SqlEx
{
    /*
    ** Only close statement if it wasn't automatically
    ** closed during initial processing.
    */
    if ( (rslt_flags & MSG_RF_CLOSED) == 0 )
    {
	msg.begin( MSG_CURSOR );
	msg.write( MSG_CUR_CLOSE );
	msg.write( MSG_CUR_STMT_ID );
	msg.write( (short)8 );
	msg.write( (int)((rslt_val_stmt >> 32) & 0xffffffff) );
	msg.write( (int)(rslt_val_stmt & 0xffffffff) );
	msg.done( true );
	readResults();
    }

    return;
} // closeQuery


/*
** Name: initResults
**
** Description:
**	Initialize query results prior to calling readResults().  
**	The expected result type is saved to be used as validation 
**	during result processing.
**
** Input:
**	dbms_type	Expected result type.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected void
initResults( short dbms_type )
{
    /*
    ** Clear the MSG results, save the result type 
    ** info, and initialize the result values.  Note 
    ** that the result byte stream is not initialized 
    ** so that the stream can be re-used.
    */
    clearResults();
    resultType = dbms_type;
    resultLength = -1;
    intResult = -1;
    baResult = null;
    stringResult = null;

    return;
} // initResults


/*
** Name: readDesc
**
** Description:
**	Read DESC message describing query results.  This method 
**	is called indirectly by calling readResults() to read 
**	query results.
**
**	Only a single result value is permitted.  The result type 
**	must match the type provided in the preceding call to 
**	initResults().  The length of the result value is stored 
**	in resultLength.
**
**	Note: this method must return a JdbcRSMD reference to 
**	satisfy the super-class, but simply returns null.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	JdbcRSMD	NULL!
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected JdbcRSMD
readDesc()
    throws SqlEx
{
    short	count = msg.readShort();

    if ( count != 1 )
	throw SqlEx.get( ERR_GC401A_CONVERSION_ERR );

    msg.readShort();			// SQL type
    short dbms_type = msg.readShort();	// DBMS type
    resultLength = msg.readShort();	// Length
    msg.readByte();			// Precision
    msg.readByte();			// Scale
    msg.readByte();			// Flags
    short len = msg.readShort();	// Column name length
    if ( len > 0 )  msg.skip( len );	// Column name

    if ( dbms_type != resultType )
	throw SqlEx.get( ERR_GC401A_CONVERSION_ERR );

    return( null );
} // readDesc


/*
** Name: readData
**
** Description:
**	Read a query result DATA value.  This method is called 
**	indirectly by calling readResults() to read query results.
**
**	Only a single value is permitted.  The value type is 
**	determined by the result type provided in the preceeding 
**	call to initResults() and verified by readDesc().
**
**	The following table shows the supported result types,
**	where the associated value is stored and it's Java type:
**
**	DBMS_TYPE_INT		intResult	long
**	DBMS_TYPE_VARBYTE	baResult	byte[].
**	DBMS_TYPE_VARCHAR	stringResult	String
**	DBMS_TYPE_NVARCHAR	stringResult	String
**	DBMS_TYPE_LONG_BYTE	bsResult	InputStream
**	DBMS_TYPE_LONG_CHAR	bsResult	InputStream
**	DBMS_TYPE_LONG_NCHAR	csResult	Reader
**
**	If the result is a LOB value, the associated stream is 
**	initialized but no LOB data is read.  Message processing 
**	is interrupted and the result stream should be used 
**	immediately to read the LOB data.  Once the LOB data has 
**	been read, readResults() should be called again to finish 
**	reading the query results.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean		Interrupt processing.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

protected boolean
readData()
    throws SqlEx
{
    boolean	interrupt = false;

    /*
    ** A NULL result value is not expected.
    */
    if ( msg.readByte() == 0 )		// Should not happen!
	throw SqlEx.get( ERR_GC401A_CONVERSION_ERR );
    
    switch( resultType )
    {
    case DBMS_TYPE_INT :
	switch( resultLength )
	{
	case 1 :  intResult = msg.readByte();	break;
	case 2 :  intResult = msg.readShort();	break;
	case 4 :  intResult = msg.readInt();	break;
	case 8 :  intResult = msg.readLong();	break;
	default :  throw SqlEx.get( ERR_GC401A_CONVERSION_ERR );
	}
	break;

    case DBMS_TYPE_VARBYTE :
	baResult = msg.readBytes();
	break;

    case DBMS_TYPE_VARCHAR :
	stringResult = msg.readString();
	break;

    case DBMS_TYPE_NVARCHAR :
	stringResult = msg.readUCS2();
	break;

    case DBMS_TYPE_LONG_BYTE :
    case DBMS_TYPE_LONG_CHAR :
	/*
	** Get a byte stream to read the character LOB data
	** (re-use prior byte stream if available).
	*/
	if ( bsResult == null )
	    bsResult = msg.readByteStream();
	else
	    bsResult = msg.readByteStream( bsResult );

	/*
	** No LOB data has been read at this point.
	** Interrupt result processing to permit the
	** LOB data to be read.  Result processing
	** will continue once the LOB data is read.
	*/
	interrupt = true;
	break;

    case DBMS_TYPE_LONG_NCHAR :
	/*
	** Get a character reader to read the LOB data
	** re-use prior reader if available).
	*/
	if ( csResult == null )
	    csResult = msg.readUCS2Reader();
	else
	    csResult = msg.readUCS2Reader( csResult );

	/*
	** No LOB data has been read at this point.
	** Interrupt result processing to permit the
	** LOB data to be read.  Result processing
	** will continue once the LOB data is read.
	*/
	interrupt = true;
	break;

    default :  	// Should not happen!
	throw SqlEx.get( ERR_GC401A_CONVERSION_ERR );
    }
 
    return( interrupt );
} // readData


/*
** Name: ByteStream
**
** Description:
**	Class providing LOB data access as a byte stream
**	via a DBMS LOB locator.
**
**  Overriden Methods
**
**	close			Close stream.
**	mark			Mark current position.
**	markSupported		Is mark method supported.
**	read			Read bytes.
**	reset			Set position to mark.
**	skip			Skip bytes.
**
**  Private Data
**
**	position		Current position in LOB.
**	mark			Marked position in LOB.
**	end_of_data		Has end-of-data been reached?
**	ba			Byte buffer for read method.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

private class
ByteStream
    extends InputStream
    implements DbmsConst
{

    private int		position = 1;
    private int		mark = -1;
    private boolean	end_of_data = false;
    private byte	ba[] = new byte[1];


/*
** Name: close
**
** Description:
**	Close stream and free resources.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public void
close()
    throws IOException
{
    /*
    ** Disable future read, skip, and reset requests.
    */
    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".close()" );
    position = mark = -1;
    end_of_data = true;
    return;
} // close


/*
** Name: markSupported
**
** Description:
**	Are mark() and reset() supported?
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean		TRUE if mark() supported.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public boolean
markSupported()
{
    return( true );
} // markSupported


/*
** Name: mark
**
** Description:
**	Mark current position in stream.
**
** Input:
**	readLimit	Max required number of bytes to save.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public void
mark( int readLimit )
{
    /*
    ** Since no data is buffered, only need to save 
    ** current position (limit is meaningless).
    */
    mark = position;
    return;
} // mark


/*
** Name: reset
**
** Description:
**	Repositions stream to previously marked position.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public void
reset()
    throws IOException
{
    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".reset()" );
    if ( position <= 0 )  throw new IOException( "Stream closed" );
    if ( mark <= 0 )  mark = 1;

    if ( trace.enabled( 5 ) )  trace.write( tr_id + ": position to " + mark );
    position = mark;
    end_of_data = false;
    return;
} // reset


/*
** Name: skip
**
** Description:
**	Skip over and discard bytes from stream.
**
** Input:
**	bytes	Number of bytes to skip
**
** Output:
**	None.
**
** Returns:
**	long	Number of bytes skipped.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public long
skip( long bytes )
    throws IOException
{
    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".skip(" + bytes + ")" );

    /*
    ** No data is buffered, so we simply move the current
    ** position forward.  Note that we don't know the
    ** actual LOB length, so we can't limit the number
    ** of bytes skipped.  Other than not returning the
    ** correct value, skipping past the end should not
    ** cause a problem since the next read will discover
    ** the end-of-data condition.
    */
    if ( end_of_data  ||  position <= 0  ||  bytes <= 0 )
	return( 0 );

    if ( trace.enabled( 5 ) )  
	trace.write( tr_id + ": skip " + bytes + " at " + position );

    position += bytes;
    return( bytes );
} // skip


/*
** Name: read
**
** Description:
**	Reads a single byte from the stream.  Returns
**	-1 when no data available.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	int	Data byte or -1.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public int
read()
    throws IOException
{
    return( (read( ba, 0, 1 ) == 1) ? (int)ba[0] & 0xff : -1 );
} // read


/*
** Name: read
**
** Description:
**	Read bytes into byte array from stream.  Returns
**	the number of bytes actually read or -1 if end-
**	of-data has been reached.
**
** Input:
**	ba	Target byte array.
**	offset	Position in target array.
**	length	Number of bytes to read.
**
** Output:
**	ba	Data bytes.
**
** Returns:
**	int	Number of bytes read or -1.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public int
read( byte ba[], int offset, int length )
    throws IOException
{
    int actual = 0;

    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".read(" + length + ")" );

    /*
    ** Check exceptional conditions.
    */
    if ( ba == null )  throw new NullPointerException();
    if ( offset < 0  ||  length < 0  ||  
	 (offset + length) < 0  || (offset + length) > ba.length )
	throw new IndexOutOfBoundsException();
    if ( position < 0 )  throw new IOException( "Stream closed" );

    /*
    ** No data if at end-of-data.
    */
    if ( end_of_data )  return( -1 );
    if ( length == 0 )  return( 0 );

    try { msg.lock(); }
    catch( SqlEx sqlEx )
    { throw new IOException( sqlEx.getMessage() ); }
	
    try
    {
	sendQuery( "select substring( ~V from ~V for ~V )" );

	startDesc( (short)3 );
	descParam();			// Locator
	descParam( (int)position );
	descParam( length );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam();			// Locator
	sendParam( (int)position );
	sendParam( length );
	msg.done( true );

	/*
	** The LOB data stream type depends on the target
	** LOB type which can be determined by the locator
	** type being used as a reference.
	*/
	initResults( (loc_type == DBMS_TYPE_LCLOC) 
		     ? DBMS_TYPE_LONG_CHAR : DBMS_TYPE_LONG_BYTE );
	readResults();

	/*
	** Read LOB data, close stream to ensure data message 
	** has been consumed, and process rest of results.
	*/
	actual = bsResult.read( ba, offset, length );

	try { bsResult.close(); }
	catch( IOException ignore ) {}

	readResults();
	closeQuery();
    }
    catch( SqlEx sqlEx )
    {
	close();
	throw new IOException( sqlEx.getMessage() );
    }
    catch( IOException ex )
    {
	close();
	throw ex;
    }
    finally
    {
	msg.unlock();
    }

    if ( actual <= 0 )  
    {
	actual = -1;
	end_of_data = true;
    }
    else
    {
	if ( trace.enabled( 5 ) )  
	    trace.write( tr_id + ": read " + actual + " at " + position );
	position += actual;
	if ( actual < length )  end_of_data = true;
    }

    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".read:" + actual );
    return( actual );
} // read


} // class ByteStream


/*
** Name: CharStream
**
** Description:
**	Class providing LOB data access as a character
**	Reader via a DBMS LOB locator.
**
**  Overriden Methods
**
**	close			Close stream.
**	mark			Mark current position.
**	markSupported		Is mark method supported?
**	read			Read characters.
**	reset			Position to mark.
**	skip			Skip characters.
**
**  Private Data
**
**	position		Current position in LOB.
**	mark			Marked position in LOB.
**	end_of_data		Has end-of-data been reached.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

private class
CharStream
    extends Reader
    implements DbmsConst
{

    private int		position = 1;
    private int		mark = -1;
    private boolean	end_of_data = false;


/*
** Name: close
**
** Description:
**	Close stream and free resources.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public void
close()
    throws IOException
{
    /*
    ** Disable future read, skip, and reset requests.
    */
    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".close()" );
    position = mark = -1;
    end_of_data = true;
    return;
} // close


/*
** Name: markSupported
**
** Description:
**	Are mark() and reset() supported?
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean		TRUE if mark() supported.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public boolean
markSupported()
{
    return( true );
} // markSupported


/*
** Name: mark
**
** Description:
**	Mark current position in stream.
**
** Input:
**	readLimit	Max required number of bytes to save.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public void
mark( int readLimit )
    throws IOException
{
    if ( position < 0 )  throw new IOException( "Stream closed" );

    /*
    ** Since no data is buffered, only need to save 
    ** current position (limit is meaningless).
    */
    mark = position;
    return;
} // mark


/*
** Name: reset
**
** Description:
**	Repositions stream to previously marked position.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public void
reset()
    throws IOException
{
    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".reset()" );
    if ( position <= 0 )  throw new IOException( "Stream closed" );
    if ( mark <= 0 )  mark = 1;

    if ( trace.enabled( 5 ) )  trace.write( tr_id + ": position to " + mark );
    position = mark;
    end_of_data = false;
    return;
} // reset


/*
** Name: skip
**
** Description:
**	Skip over and discard characters from stream.
**
** Input:
**	chars	Number of characters to skip
**
** Output:
**	None.
**
** Returns:
**	long	Number of characters skipped.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public long
skip( long chars )
    throws IOException
{
    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".skip(" + chars + ")" );

    /*
    ** No data is buffered, so we simply move the current
    ** position forward.  Note that we don't know the
    ** actual LOB length, so we can't limit the number
    ** of characters skipped.  Other than not returning the
    ** correct value, skipping past the end should not
    ** cause a problem since the next read will discover
    ** the end-of-data condition.
    */
    if ( end_of_data  ||  position <= 0  ||  chars <= 0 )
	return( 0 );

    if ( trace.enabled( 5 ) )  
	trace.write( tr_id + ": skip " + chars + " at " + position );

    position += chars;
    return( chars );
} // skip


/*
** Name: read
**
** Description:
**	Read characters into char array from stream.  Returns
**	the number of characters actually read or -1 if end-
**	of-data has been reached.
**
** Input:
**	ca	Target character array.
**	offset	Position in target array.
**	length	Number of characters to read.
**
** Output:
**	ca	Characters.
**
** Returns:
**	int	Number of characters read or -1.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public int
read( char ca[], int offset, int length )
    throws IOException
{
    int actual = 0;

    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".read(" + length + ")" );

    /*
    ** Check exceptional conditions.
    */
    if ( ca == null )  throw new NullPointerException();
    if ( offset < 0  ||  length < 0  ||  
	 (offset + length) < 0  || (offset + length) > ca.length )
	throw new IndexOutOfBoundsException();
    if ( position < 0 )  throw new IOException( "Stream closed" );

    /*
    ** No data if at end-of-data.
    */
    if ( end_of_data )  return( -1 );
    if ( length == 0 )  return( 0 );

    try { msg.lock(); }
    catch( SqlEx sqlEx )
    { throw new IOException( sqlEx.getMessage() ); }
	
    try
    {
	sendQuery( "select substring( ~V from ~V for ~V )" );

	startDesc( (short)3 );
	descParam();			// Locator
	descParam( (int)position );
	descParam( length );
	msg.done( false );

	msg.begin( MSG_DATA );
	sendParam();			// Locator
	sendParam( (int)position );
	sendParam( length );
	msg.done( true );

	initResults( DBMS_TYPE_LONG_NCHAR );
	readResults();

	/*
	** Read LOB data, close stream to ensure data message 
	** has been consumed, and process rest of results.
	*/
	actual = csResult.read( ca, offset, length );

	try { csResult.close(); }
	catch( IOException ignore ) {}

	readResults();
	closeQuery();
    }
    catch( SqlEx sqlEx )
    {
	close();
	throw new IOException( sqlEx.getMessage() );
    }
    catch( IOException ex )
    {
	close();
	throw ex;
    }
    finally
    {
	msg.unlock();
    }

    if ( actual <= 0 )  
    {
	actual = -1;
	end_of_data = true;
    }
    else
    {
	if ( trace.enabled( 5 ) )  
	    trace.write( tr_id + ": read " + actual + " at " + position );
	position += actual;
	if ( actual < length )  end_of_data = true;
    }

    if ( trace.enabled( 4 ) )  trace.write( tr_id + ".read: " + actual );
    return( actual );
} // read


} // class CharStream


} // class DrvLOB
