/*
** Copyright (c) 2007 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.util;

/*
** Name: BufferedBlob.java
**
** Description:
**	Defines class which implements the JDBC Blob interface
**	as a wrapper for a cached BLOB value.
**
**  Classes:
**
**	BufferedBlob		Blob wrapper for ByteBuffer
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
**	26-Feb-07 (gordy)
**	    Support buffered LOBs.
*/

import	java.io.InputStream;
import	java.io.OutputStream;
import	java.sql.Blob;
import	java.sql.SQLException;
import	com.ingres.gcf.util.SqlEx;


/*
** Name: BufferedBlob
**
** Description:
**	Class which implements the JDBC Blob interface as
**	a wrapper for a cached BLOB value.
**
**	The BLOB data is stored in a ByteBuffer.
**
**  Interface Methods:
**
**	getBinaryStream		Retrieve stream which accesses LOB data.
**	getBytes		Read bytes from LOB data.
**	length			Get length of LOB data.
**	position		Search LOB data.
**	setBinaryStream		Write LOB data from stream.
**	setBytes		Write bytes into LOB data.
**	truncate		Set length of LOB data.
**
**  Private Data:
**
**	buff			Underlying cached BLOB.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
**	26-Feb-07 (gordy)
**	    Added constructor to wrap existing byte buffer.
*/

public class
BufferedBlob
    implements Blob, GcfErr
{

    private ByteBuffer		buff = null;


/*
** Name: BufferedBlob
**
** Description:
**	Class constructor.  Constructs an empty Blob
**	with default segment size.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public
BufferedBlob()
{
    buff = new ByteBuffer();
} // BufferedBlob


/*
** Name: BufferedBlob
**
** Description:
**	Class constructor.  Constructs an empty Blob
**	with requested segment size.
**
** Input:
**	size	Block size.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public
BufferedBlob( int size )
{
    buff = new ByteBuffer( size );
} // BufferedBlob


/*
** Name: BufferedBlob
**
** Description:
**	Class constructor.  Constructs an empty Blob
**	with default segment size and fills with data
**	read from a binary stream.  The binary stream
**	is closed.
**
** Input:
**	stream	Binary stream.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public
BufferedBlob( InputStream stream )
{
    buff = new ByteBuffer( stream );
} // BufferedBlob


/*
** Name: BufferedBlob
**
** Description:
**	Class constructor.  Constructs an empty Blob
**	with requested segment size and fills with data
**	read from a binary stream.  The binary stream
**	is closed.
**
** Input:
**	size	Block size.
**	stream	Binary stream.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public
BufferedBlob( int size, InputStream stream )
{
    buff = new ByteBuffer( size, stream );
} // BufferedBlob

    
/*
** Name: BufferedBlob
**
** Description:
**	Class constructor.  Constructs a Blob as a
**	wrapper for an existing byte buffer.
**
** Input:
**	buff	Byte buffer.
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
*/

public
BufferedBlob( ByteBuffer buff )
{
    this.buff = buff;
} // BufferedBlob


/*
** Name: length
**
** Description:
**	Returns the length of the Blob.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	long	Length of Blob.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public long
length()
    throws SQLException
{
    return( buff.length() );
} // length


/*
** Name: getBinaryStream
**
** Description:
**	Returns a stream capable of reading the Blob data.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	InputStream	Stream which accesses Blob data.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public InputStream
getBinaryStream()
    throws SQLException
{
    return( buff.getIS() );
} // getBinaryStream


/*
** Name: getBytes
**
** Description:
**	Extract portion of Blob.  The number of bytes
**	returned is limited by the length of the Blob.
**	Zero bytes are returned if the target position 
**	is beyond the end of the Blob.
**
** Input:
**	pos	Target position (1 based reference).
**	length	Number of bytes.
**
** Output:
**	None.
**
** Returns:
**	byte[]	Bytes extracted.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public byte[]
getBytes( long pos, int length )
    throws SQLException
{
    /*
    ** Validate parameters.
    */
    if ( pos < 1  ||  length < 0 )
	throw SqlEx.get( ERR_GC4010_PARAM_VALUE );

    /*
    ** Adjust position to 0 base reference within valid
    ** range and limit length to amount of data available.
    */
    long limit = buff.length();
    pos = Math.min( pos - 1, limit );
    length = (int)Math.max( 0L, Math.min( limit - pos, (long)length ) );
    byte data[] = new byte[ length ];

    /*
    ** Due to the validation and adjustments done above,
    ** there should be no problem reading the data, but
    ** make sure the actual length matches requested.
    */
    if ( length > 0  &&  (int)buff.read( pos, data, 0, length ) != length )
	throw SqlEx.get( ERR_GC4010_PARAM_VALUE );

    return( data );
} // getBytes


/*
** Name: position
**
** Description:
**	Search Blob for matching pattern.  If no match
**	is found, -1 is returned.
**
** Input:
**	pattern	Byte pattern to be matched.
**	start	Starting position of search (1 based reference).
**
** Output:
**	None.
**
** Returns:
**	long	Position of match (1 based reference) or -1.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public long
position( byte[] pattern, long start )
    throws SQLException
{
    if ( pattern == null  ||  start < 1 )
	throw SqlEx.get( ERR_GC4010_PARAM_VALUE );

    /*
    ** Adjust start and match position for 0 based reference.
    */
    start = buff.find( pattern, start - 1 );
    return( (start >= 0) ? start + 1 : -1 );
} // position


/*
** Name: position
**
** Description:
**	Search Blob for matching pattern.  If no match
**	is found, -1 is returned.
**
** Input:
**	pattern	Blob pattern to be matched.
**	start	Starting position of search (1 based reference).
**
** Output:
**	None.
**
** Returns:
**	long	Position of match (1 based reference) or -1.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public long
position( Blob pattern, long start )
    throws SQLException
{
    long length;

    /*
    ** An extermely long pattern is not expected, but
    ** check to make sure the downcast won't lose bits.
    */
    if ( pattern == null  ||  (length = pattern.length()) > Integer.MAX_VALUE )
	throw SqlEx.get( ERR_GC4010_PARAM_VALUE );
    return( position( pattern.getBytes( 1, (int)length ), start ) );
} // position


/*
** Name: setBytes
**
** Description:
**	Writes bytes into Blob at requested position.
**
** Input:
**	pos	Position in Blob to write bytes (1 based reference).
**	bytes	Bytes to be written.
**
** Output:
**	None.
**
** Returns:
**	int	Number of bytes written.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public int
setBytes( long pos, byte bytes[] )
    throws SQLException
{
    if ( bytes == null )  throw SqlEx.get( ERR_GC4010_PARAM_VALUE );
    return( setBytes( pos, bytes, 0, bytes.length ) );
} // setBytes


/*
** Name: setBytes
**
** Description:
**	Writes bytes into Blob at requested position.
**
** Input:
**	pos	Position in Blob to write bytes (1 based reference).
**	bytes	Bytes to be written.
**	offset	Offset of bytes to be written (0 based reference).
**	len	Number of bytes to be written.
**
** Output:
**	None.
**
** Returns:
**	int	Number of bytes written.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public int
setBytes( long pos, byte bytes[], int offset, int len )
    throws SQLException
{
    /*
    ** Validate parameters.
    */
    if ( bytes == null  ||  len < 0  ||
	 pos < 1  ||  pos > buff.length() + 1  ||
	 offset < 0  ||  offset > bytes.length )
	throw SqlEx.get( ERR_GC4010_PARAM_VALUE );

    /*
    ** Adjust target position to 0 based reference.
    */
    return( (int)buff.write( pos - 1, bytes, offset, len ) );
} // setBytes


/*
** Name: setBinaryStream
**
** Description:
**	Returns a stream capable of writing to the Blob at
**	the requested position.
**
** Input:
**	pos		Position to write stream (1 based reference).
**
** Output:
**	None.
**
** Returns:
**	OutputStream	Stream which writes to Blob.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public OutputStream
setBinaryStream( long pos )
    throws SQLException
{
    OutputStream os;

    /*
    ** NULL is returned if the target position is invalid.
    ** Adjust target position to 0 based reference.
    */
    if ( (os = buff.getOS( pos - 1 )) == null )
	throw SqlEx.get( ERR_GC4010_PARAM_VALUE );

    return( os );
} // setBinaryStream


/*
** Name: truncate
**
** Description:
**	Truncate Blob to requested length.
**
** Input:
**	len	Length at which Blob should be truncated.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 2-Feb-07 (gordy)
**	    Created.
*/

public void
truncate( long len )
    throws SQLException
{
    if ( len < 0 )  throw SqlEx.get( ERR_GC4010_PARAM_VALUE );
    buff.truncate( len );
    return;
} // truncate


} // class BufferedBlob

