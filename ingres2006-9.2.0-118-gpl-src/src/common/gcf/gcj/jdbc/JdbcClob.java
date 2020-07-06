/*
** Copyright (c) 2007 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.jdbc;

/*
** Name: JdbcClob.java
**
** Description:
**	Defines class which implements the JDBC Clob interface.
**
**	Class acts as a wrapper for Clob objects representing both
**	LOB Locators and cached LOBs.
**
**  Classes:
**
**	JdbcClob
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	 2-Feb-07 (gordy)
**          Support update operations by converting LOB Locators
**          to local cached Clob.
**	 4-May-07 (gordy)
**	    Set class access for reflection.
*/

import	java.io.InputStream;
import	java.io.OutputStream;
import	java.io.Reader;
import	java.io.Writer;
import	java.sql.Clob;
import	java.sql.SQLException;
import	com.ingres.gcf.util.BufferedClob;
import	com.ingres.gcf.util.SqlEx;


/*
** Name: JdbcClob
**
** Description:
**	JDBC driver class which implements the JDBC Clob interface.
**
**      Class acts as a wrapper for Clob objects representing both
**      LOB Locators and cached LOBs.  LOB Locators are not updatable.
**      If an update method is called for an instance which wraps a
**      LOB Locator,  the associated LOB data is cached and subsequent
**      operations are performed on the cached LOB.
**
**  Interface Methods:
**
**	getAsciiStream		Retrieve stream which accesses LOB data.
**	getCharacterStream	Retrieve stream which accesses LOB data.
**	getSubString		Read characters from LOB data.
**	length			Get length of LOB data.
**	position		Search LOB data.
**	setAsciiStream		Write LOB data from stream.
**	setCharacterStream	Write LOB data from stream.
**	setString		Write characters into LOB data.
**	truncate		Set length of LOB data.
**
**  Protected Methods:
**
**	isValidLocator		Blob represents a LOB Locator.
**	getLOB			Get underlying LOB Locator.
**
**  Private Data:
**
**	clob			The underlying DBMS Clob.
**	segSize			Buffered segment size.
**	trace			Tracing output.
**	title			Class title for tracing.
**
**  Private Methods:
**
**	prepareToUpdate		Convert to updatable.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	 2-Feb-07 (gordy)
**	    Added prepareToUpdate() to convert to cached LOB if needed.
**	26-Feb-07 (gordy)
**	    Added segment size and constructor to provide segment size.
*/

public class
JdbcClob
    implements Clob, DrvConst
{

    private Clob		clob = null;
    private int			segSize = DRV_DFLT_SEGSIZE;
    private DrvTrace		trace = null;
    private String		title = null;


/*
** Name: JdbcClob
**
** Description:
**	Class constructor for Character LOB Locators.
**
** Input:
**	clob	Underlying DBMS Clob.
**	trace	Tracing output.
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
**	 4-May-07 (gordy)
**	    Class is exposed outside package, restrict constructor access.
*/

// package access
JdbcClob( DrvClob clob, DrvTrace trace )
{
    this.clob = clob;
    this.trace = trace;
    title = trace.getTraceName() + "-Clob[LOC:" + clob.toString() + "]";
} // JdbcClob


/*
** Name: JdbcClob
**
** Description:
**	Class constructor for Character LOB Locators.
**
** Input:
**	clob		Underlying DBMS Clob.
**	segSize		Buffered segment size.
**	trace		Tracing output.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	26-Feb-07 (gordy)
**	    Created.
**	 4-May-07 (gordy)
**	    Class is exposed outside package, restrict constructor access.
*/

// package access
JdbcClob( DrvClob clob, int segSize, DrvTrace trace )
{
    this( clob, trace );
    this.segSize = segSize;
} // JdbcClob


/*
** Name: JdbcClob
**
** Description:
**	Class constructor for NCS (UCS2) LOB Locators.
**
** Input:
**	clob	Underlying DBMS Clob.
**	trace	Tracing output.
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
**	 4-May-07 (gordy)
**	    Class is exposed outside package, restrict constructor access.
*/

// package access
JdbcClob( DrvNlob clob, DrvTrace trace )
{
    this.clob = clob;
    this.trace = trace;
    title = trace.getTraceName() + "-Clob[LOC:" + clob.toString() + "]";
} // JdbcClob


/*
** Name: JdbcClob
**
** Description:
**	Class constructor for NCS (UCS2) LOB Locators.
**
** Input:
**	clob		Underlying DBMS Clob.
**	segSize		Buffered segment size.
**	trace		Tracing output.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	26-Feb-07 (gordy)
**	    Created.
**	 4-May-07 (gordy)
**	    Class is exposed outside package, restrict constructor access.
*/

// package access
JdbcClob( DrvNlob clob, int segSize, DrvTrace trace )
{
    this( clob, trace );
    this.segSize = segSize;
} // JdbcClob


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
    return( title );
} // toString


/*
** Name: isValidLocator
**
** Description:
**	Returns an indication that this Clob represents a
**	DBMS LOB Locator associated with the given connection.
**
** Input:
**	conn	Target connection.
**
** Output:
**	None.
**
** Returns:
**	boolean	TRUE if LOB Locator associated with connection.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	 2-Feb-07 (gordy)
**          Make sure underlying Blob is LOB Locator.
*/

boolean	// package access required
isValidLocator( DrvConn conn )
{
    return( (clob instanceof DrvLOB) 
	    ? ((DrvLOB)clob).hasSameDomain( conn ) : false );
} // isValidLocator


/*
** Name: getLOB
**
** Description:
**	Returns the underlying LOB Locator object.
**
**      If underlying Clob is not a LOB Locator, NULL is returned.
**	This method should only be called when isValidLocator()
**	returns TRUE.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	DrvLOB	LOB Locator or NULL.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**       2-Feb-07 (gordy)
**          Make sure underlying Blob is LOB Locator.
*/

DrvLOB	// package access required
getLOB()
{
    return( (clob instanceof DrvLOB) ? (DrvLOB)clob : null );
} // getLOB


/*
** Name: length
**
** Description:
**	Returns the length of the Clob.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	long	Length of Clob.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public long
length()
    throws SQLException
{
    if ( trace.enabled() )  trace.log( title + ".length()" );
    long length = clob.length();
    if ( trace.enabled() )  trace.log( title + ".length: " + length );
    return( length );
} // length


/*
** Name: getAsciiStream
**
** Description:
**	Returns a stream capable of reading and converting
**	the Clob data into ASCII.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	InputStream	Stream which access Clob data.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public InputStream
getAsciiStream()
    throws SQLException
{
    if ( trace.enabled() )  trace.log( title + ".getAsciiStream()" );
    return( clob.getAsciiStream() );
} // getAsciiStream


/*
** Name: getCharacterStream
**
** Description:
**	Returns a stream capable of reading the Clob data.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	Reader	Stream which access Clob data.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public Reader
getCharacterStream()
    throws SQLException
{
    if ( trace.enabled() )  trace.log( title + ".getCharacterStream()" );
    return( clob.getCharacterStream() );
} // getCharacterStream


/*
** Name: getSubString
**
** Description:
**	Extract portion of Clob.
**
** Input:
**	pos	Position of characters to be extracted.
**	length	Number of characters to be extracted.
**
** Output:
**	None.
**
** Returns:
**	String	Characters extracted.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public String
getSubString( long pos, int length )
    throws SQLException
{
    if ( trace.enabled() )  
	trace.log( title + ".getSubString(" + pos + "," + length + ")" );

    String str = clob.getSubString( pos, length );

    if ( trace.enabled() )  
	trace.log( title + ".getSubString:" + str.length() + " characters" );
    return( str );
} // getSubString


/*
** Name: position
**
** Description:
**	Search Clob for matching pattern.  If no match
**	is found, -1 is returned.
**
** Input:
**	pattern	Character pattern to be matched.
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

public long
position( String pattern, long start )
    throws SQLException
{
    if ( trace.enabled() )  trace.log( title + ".position(" + start + ")" );
    long pos = clob.position( pattern, start );
    if ( trace.enabled() )  trace.log( title + ".position: " + pos );
    return( pos );
} // position


/*
** Name: position
**
** Description:
**	Search Clob for matching pattern.  If no match
**	is found, -1 is returned.
**
** Input:
**	pattern	Clob pattern to be matched.
**	start	Starting position of search.
**
** Output:
**	None.
**
** Returns:
**	long	Position of match or -1.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public long
position( Clob pattern, long start )
    throws SQLException
{
    if ( trace.enabled() )  
	trace.log( title + ".position(" + pattern + "," + start + ")" );

    /*
    ** If pattern Clob is an instance of this class,
    ** then we need to use the underlying LOB object
    ** directly to allow determination of locator
    ** validity.
    */
    if ( pattern == null )  throw new NullPointerException();
    if ( pattern instanceof JdbcClob )  pattern = ((JdbcClob)pattern).clob;
    long pos = clob.position( pattern, start );

    if ( trace.enabled() )  trace.log( title + ".position: " + pos );
    return( pos );
} // position


/*
** Name: setString
**
** Description:
**	Writes string into Clob at requested position.
**
** Input:
**	pos	Position in Clob to write string.
**	str	String to be written.
**
** Output:
**	None.
**
** Returns:
**	int	Number of characters written.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public int
setString( long pos, String str )
    throws SQLException
{
    return( setString( pos, str, 0, str.length() ) );
} // setString


/*
** Name: setString
**
** Description:
**	Writes characters into Clob at requested position.
**
** Input:
**	pos	Position in Clob to write characters.
**	str	Characters to be written.
**	offset	Offset of characters to be written.
**	len	Number of characters to be written.
**
** Output:
**	None.
**
** Returns:
**	int	Number of characters written.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	 2-Feb-07 (gordy)
**	    Convert to updatable.
*/

public int
setString( long pos, String str, int offset, int len )
    throws SQLException
{
    if ( trace.enabled() )  
	trace.log( title + ".setString(" + pos + "," + len + ")" );

    prepareToUpdate();
    int length = clob.setString( pos, str, offset, len );

    if ( trace.enabled() )  trace.log( title + ".setString: " + length );
    return( length );
} // setString


/*
** Name: setAsciiStream
**
** Description:
**	Returns a stream capable of writing ASCII data
**	to the Clob at the requested position.
**
** Input:
**	pos		Position to write stream.
**
** Output:
**	None.
**
** Returns:
**	OutputStream	Stream which writes to Clob.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	 2-Feb-07 (gordy)
**	    Convert to updatable.
*/

public OutputStream
setAsciiStream( long pos )
    throws SQLException
{
    if ( trace.enabled() )  trace.log(title + ".setAsciiStream(" + pos + ")");
    prepareToUpdate();
    return( clob.setAsciiStream( pos ) );
} // setAsciiStream


/*
** Name: setCharacterStream
**
** Description:
**	Returns a stream capable of writing to the Clob at
**	the requested position.
**
** Input:
**	pos	Position to write stream.
**
** Output:
**	None.
**
** Returns:
**	Writer	Stream which writes to Clob.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
**	 2-Feb-07 (gordy)
**	    Convert to updatable.
*/

public Writer
setCharacterStream( long pos )
    throws SQLException
{
    if ( trace.enabled() )  
	trace.log( title + ".setCharacterStream(" + pos + ")" );
    prepareToUpdate();
    return( clob.setCharacterStream( pos ) );
} // setCharacterStream


/*
** Name: truncate
**
** Description:
**	Truncate Clob to requested length.
**
** Input:
**	len	Length at which Clob should be truncated.
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
**	 2-Feb-07 (gordy)
**	    Convert to updatable.
*/

public void
truncate( long len )
    throws SQLException
{
    if ( trace.enabled() )  trace.log( title + ".truncate(" + len + ")" );
    prepareToUpdate();
    clob.truncate( len );
    return;
} // truncate


/*
** Name: prepareToUpdate
**
** Description:
**      Converts the underlying Clob to be updatable, if necessary.
**
**      LOB Locators are not updatable.  If underlying Clob is a
**      LOB Locator, the associated LOB data is cached and used
**      for subsequent operations.  Otherwise, no change is made.
**
** Input:
**      None.
**
** Output:
**      None.
**
** Returns:
**      void
**
** History:
**       2-Nov-07 (gordy)
**          Created.
**	26-Feb-07 (gordy)
**	    Set segment size.  Use more effecient access method.
*/

private void
prepareToUpdate()
    throws SqlEx
{
    if ( clob instanceof DrvClob )
    {
	clob = new BufferedClob( segSize, ((DrvClob)clob).get() );
	title = trace.getTraceName() + "-Clob[cache]";
    }
    else  if ( clob instanceof DrvNlob )
    {
	clob = new BufferedClob( segSize, ((DrvNlob)clob).get() );
	title = trace.getTraceName() + "-Clob[cache]";
    }

    return;
} // prepareToUpdate()


} // class JdbcClob
