/*
** Copyright (c) 2003, 2007 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.util;

/*
** Name: SqlLongNChar.java
**
** Description:
**	Defines class which represents an SQL Long NVarchar value.
**
**  Classes:
**
**	SqlLongNChar	An SQL LongNVarchar value.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Added support for parameter types/values in addition to 
**	    existing support for columns.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
**	15-Nov-06 (gordy)
**	    Enhance conversion support.
**	26-Feb-07 (gordy)
**	    Enhanced with Clob compatibility.
*/

import	java.math.BigDecimal;
import	java.io.InputStream;
import	java.io.Reader;
import	java.io.Writer;
import	java.io.IOException;
import	java.util.TimeZone;
import	java.sql.Date;
import	java.sql.Time;
import	java.sql.Timestamp;
import	java.sql.Clob;


/*
** Name: SqlLongNChar
**
** Description:
**	Class which represents an SQL Long NVarchar value.  SQL Long 
**	NVarchar values are potentially large variable length streams.
**
**	Supports conversion to String, Binary, Object.  
**
**	The data value accessor methods do not check for the NULL
**	condition.  The super-class isNull() method should first
**	be checked prior to calling any of the accessor methods.
**
**  Inherited Methods:
**
**	setNull			Set a NULL data value.
**	toString		String representation of data value.
**	closeStream		Close the current stream.
**
**  Public Methods:
**
**	set			Assign a new non-NULL data value.
**	get			Retrieve current data value.
**
**	setBoolean		Data value accessor assignment methods
**	setByte
**	setShort
**	setInt
**	setLong
**	setFloat
**	setDouble
**	setBigDecimal
**	setString
**	setDate
**	setTime
**	setTimestamp
**	setCharacterStream
**
**	getBoolean		Data value accessor retrieval methods
**	getByte
**	getShort
**	getInt
**	getLong
**	getFloat
**	getDouble
**	getBigDecimal
**	getString
**	getDate
**	getTime
**	getTimestamp
**	getAsciiStream
**	getUnicodeStream
**	getCharacterStream
**	getClob
**	getObject
**
**	strm2str		Convert character stream to string.
**
**  Protected Methods:
**
**	cnvtIS2Rdr		Convert InputStream to Reader (Stub).
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Added parameter data value oriented methods.
**	15-Nov-06 (gordy)
**	    Made static method strm2str() public.  Added implementation
**	    which does not limit the string.
**	26-Feb-07 (gordy)
**	    Added getClob().
*/

public class
SqlLongNChar
    extends SqlStream
{

    
/*
** Name: SqlLongNChar
**
** Description:
**	Class constructor.  Data value is initially NULL.
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
**	12-Sep-03 (gordy)
**	    Created.
*/

public
SqlLongNChar()
{
    super();
} // SqlLongNChar


/*
** Name: SqlLongNChar
**
** Description:
**	Class constructor.  Data value is initially NULL.
**	Defines a SqlStream event listener for stream 
**	closure event notification.
**
** Input:
**	listener	Stream listener.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public
SqlLongNChar( StreamListener listener )
{
    super( listener );
} // SqlLongNChar


/*
** Name: set
**
** Description:
**	Assign a new data value.  The data value will be NULL if 
**	the input value is null, otherwise non-NULL.  The input 
**	stream may optionally implement the SqlStream.StreamSource
**	interface.
**
** Input:
**	stream		The new data value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public void
set( Reader stream )
    throws SqlEx
{
    setStream( (Object)stream );
    return;
} // set


/*
** Name: set
**
** Description:
**	Assign a new data value as a copy of a SqlNChar data 
**	value.  The data value will be NULL if the input value 
**	is null, otherwise non-NULL.  
**
** Input:
**	data	SqlNChar data value to copy.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void
set( SqlNChar data )
    throws SqlEx
{
    if ( data == null  ||  data.isNull() )
	setNull();
    else
    {
	/*
	** The character data is stored in a character array.  
	** A character stream will produce the desired output.  
	** Note that we need to follow the SqlNChar convention 
	** and extend the data to the optional limit.
	*/
	data.extend();
	setStream( getCharacter( data.value, 0, data.length ) );
    }
    return;
} // set


/*
** Name: set
**
** Description:
**	Assign a new data value as a copy of a SqlNVarChar data 
**	value.  The data value will be NULL if the input value 
**	is null, otherwise non-NULL.  
**
** Input:
**	data	SqlNVarChar data value to copy.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void
set( SqlNVarChar data )
    throws SqlEx
{
    if ( data == null  ||  data.isNull() )
	setNull();
    else
    {
	/*
	** The character data is stored in a character array.  
	** A character stream will produce the desired output.  
	*/
	setStream( getCharacter( data.value, 0, data.length ) );
    }
    return;
} // set


/*
** Name: get
**
** Description:
**	Return the current data value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	Reader	Current value.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public Reader
get()
    throws SqlEx
{
    return( (Reader)getStream() );
} // get


/*
** Name: get
**
** Description:
**	Write the current data value to a Writer stream.
**	The current data value is consumed.  The Writer
**	stream is not closed but will be flushed.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	wtr	Writer to receive data value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void
get( Writer wtr )
    throws SqlEx
{
    copyRdr2Wtr( (Reader)getStream(), wtr );
    return;
} // get


/*
** Name: setBoolean
**
** Description:
**	Assign a boolean value as the non-NULL data value.
**
** Input:
**	value	Boolean value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setBoolean( boolean value ) 
    throws SqlEx
{
    setString( Boolean.toString( value ) );
    return;
}


/*
** Name: setByte
**
** Description:
**	Assign a byte value as the non-NULL data value.
**
** Input:
**	value	Byte value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setByte( byte value ) 
    throws SqlEx
{
    setString( Byte.toString( value ) );
    return;
}


/*
** Name: setShort
**
** Description:
**	Assign a short value as the non-NULL data value.
**
** Input:
**	value	Short value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setShort( short value ) 
    throws SqlEx
{
    setString( Short.toString( value ) );
    return;
}


/*
** Name: setInt
**
** Description:
**	Assign a integer value as the non-NULL data value.
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
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setInt( int value ) 
    throws SqlEx
{
    setString( Integer.toString( value ) );
    return;
} // setInt


/*
** Name: setLong
**
** Description:
**	Assign a long value as the non-NULL data value.
**
** Input:
**	value	Long value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setLong( long value ) 
    throws SqlEx
{
    setString( Long.toString( value ) );
    return;
} // setLong


/*
** Name: setFloat
**
** Description:
**	Assign a float value as the non-NULL data value.
**
** Input:
**	value	Float value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setFloat( float value ) 
    throws SqlEx
{
    setString( Float.toString( value ) );
    return;
} // setFloat


/*
** Name: setDouble
**
** Description:
**	Assign a double value as the non-NULL data value.
**
** Input:
**	value	Double value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setDouble( double value ) 
    throws SqlEx
{
    setString( Double.toString( value ) );
    return;
} // setDouble


/*
** Name: setBigDecimal
**
** Description:
**	Assign a BigDecimal value as the data value.
**	The data value will be NULL if the input value
**	is null, otherwise non-NULL.
**
** Input:
**	value	BigDecimal value (may be null).
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setBigDecimal( BigDecimal value ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else
	setString( value.toString() );

    return;
} // setBigDecimal


/*
** Name: setString
**
** Description:
**	Assign a String value as the data value.
**	The data value will be NULL if the input 
**	value is null, otherwise non-NULL.
**
** Input:
**	value	String value (may be null).
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setString( String value ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else
	setStream( getCharacter( value ) );

    return;
} // setString


/*
** Name: setDate
**
** Description:
**	Assign a Date value as the data value.
**	The data value will be NULL if the input 
**	value is null, otherwise non-NULL.
**
**	A relative timezone may be provided which 
**	is applied to adjust the input such that 
**	it represents the original date/time value 
**	in the timezone provided.  The default is 
**	to use the local timezone.
**
** Input:
**	value	Date value (may be null).
**	tz	Relative timezone, NULL for local.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setDate( Date value, TimeZone tz ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else  if ( tz != null )
	setString( SqlDates.formatDate( value, tz ) );
    else
	setString( SqlDates.formatDate( value, false ) );
    return;
} // setDate


/*
** Name: setTime
**
** Description:
**	Assign a Time value as the data value.
**	The data value will be NULL if the input 
**	value is null, otherwise non-NULL.
**
**	A relative timezone may be provided which 
**	is applied to adjust the input such that 
**	it represents the original date/time value 
**	in the timezone provided.  The default is 
**	to use the local timezone.
**
** Input:
**	value	Time value (may be null).
**	tz	Relative timezone, NULL for local.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setTime( Time value, TimeZone tz ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else  if ( tz != null )
	setString( SqlDates.formatTime( value, tz ) );
    else
	setString( SqlDates.formatTime( value, false ) );
    return;
} // setTime


/*
** Name: setTimestamp
**
** Description:
**	Assign a Timestamp value as the data value.
**	The data value will be NULL if the input 
**	value is null, otherwise non-NULL.
**
**	A relative timezone may be provided which 
**	is applied to adjust the input such that 
**	it represents the original date/time value 
**	in the timezone provided.  The default is 
**	to use the local timezone.
**
** Input:
**	value	Timestamp value (may be null).
**	tz	Relative timezone, NULL for local.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setTimestamp( Timestamp value, TimeZone tz ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else  if ( tz != null )
	setString( SqlDates.formatTimestamp( value, tz ) );
    else
	setString( SqlDates.formatTimestamp( value, false ) );
    return;
} // setTimestamp


/*
** Name: setAsciiStream
**
** Description:
**	Assign a ASCII stream as the data value.
**	The data value will be NULL if the input 
**	value is null, otherwise non-NULL.
**
** Input:
**	value	ASCII stream (may be null).
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setAsciiStream( InputStream value ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else
	setStream( cnvtAscii( value ) );
    return;
} // setAsciiStream


/*
** Name: setUnicodeStream
**
** Description:
**	Assign a Unicode stream as the data value.
**	The data value will be NULL if the input 
**	value is null, otherwise non-NULL.
**
** Input:
**	value	Unicode stream (may be null).
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setUnicodeStream( InputStream value ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else
	setStream( cnvtUnicode( value ) );
    return;
} // setAsciiStream


/*
** Name: setCharacterStream
**
** Description:
**	Assign a character stream as the data value.
**	The data value will be NULL if the input 
**	value is null, otherwise non-NULL.
**
** Input:
**	value	Character stream (may be null).
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public void 
setCharacterStream( Reader value ) 
    throws SqlEx
{
    setStream( value );
    return;
} // setCharacterStream


/*
** Name: getBoolean
**
** Description:
**	Return the current data value as a boolean value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean		Current value converted to boolean.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public boolean 
getBoolean() 
    throws SqlEx
{
    String str = getString().trim();
    return( str.equals("1") ? true : Boolean.valueOf( str ).booleanValue());
} // getBoolean


/*
** Name: getByte
**
** Description:
**	Return the current data value as a byte value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	byte	Current value converted to byte.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public byte 
getByte() 
    throws SqlEx
{
    byte value;
    
    try { value = Byte.parseByte( getString().trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( value );
} // getByte


/*
** Name: getShort
**
** Description:
**	Return the current data value as a short value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	short	Current value converted to short.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public short 
getShort() 
    throws SqlEx
{
    short value;
    
    try { value = Short.parseShort( getString().trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( value );
} // getShort


/*
** Name: getInt
**
** Description:
**	Return the current data value as a integer value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	int	Current value converted to integer.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public int 
getInt() 
    throws SqlEx
{
    int value;
    
    try { value = Integer.parseInt( getString().trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( value );
} // getInt


/*
** Name: getLong
**
** Description:
**	Return the current data value as a long value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	long	Current value converted to long.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public long 
getLong() 
    throws SqlEx
{
    long value;
    
    try { value = Long.parseLong( getString().trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( value );
} // getLong


/*
** Name: getFloat
**
** Description:
**	Return the current data value as a float value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	float	Current value converted to float.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public float 
getFloat() 
    throws SqlEx
{
    float value;
    
    try { value = Float.parseFloat( getString().trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( value );
} // getFloat


/*
** Name: getDouble
**
** Description:
**	Return the current data value as a double value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	double	Current value converted to double.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public double 
getDouble() 
    throws SqlEx
{
    double value;
    
    try { value = Double.parseDouble( getString().trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( value );
} // getDouble


/*
** Name: getBigDecimal
**
** Description:
**	Return the current data value as a BigDecimal value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	BigDecimal	Current value converted to BigDecimal.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public BigDecimal 
getBigDecimal() 
    throws SqlEx
{
    BigDecimal value;
    
    try { value = new BigDecimal( getString().trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( value );
} // getBigDecimal


/*
** Name: getString
**
** Description:
**	Return the current data value as a String value.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String		Current value converted to String.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public String 
getString() 
    throws SqlEx
{
    return( strm2str( getCharacter() ) );
} // getString


/*
** Name: getString
**
** Description:
**	Return the current data value as a String value
**	with maximum size limit.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	limit	Maximum length of result.
**
** Output:
**	None.
**
** Returns:
**	String	Current value converted to String.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public String 
getString( int limit ) 
    throws SqlEx
{
    return( strm2str( getCharacter(), limit ) );
} // getString


/*
** Name: getDate
**
** Description:
**	Return the current data value as a Date value.
**	A relative timezone may be provided which is
**	applied to adjust the final result such that it
**	represents the original date/time value in the
**	timezone provided.  The default is to use the
**	local timezone.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	tz	Relative timezone, NULL for local.
**	
**
** Output:
**	None.
**
** Returns:
**	Date	Current value converted to Date.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public Date 
getDate( TimeZone tz ) 
    throws SqlEx
{
    String str = getString().trim();
    return( (tz == null) ? SqlDates.parseDate( str, false )
			 : SqlDates.parseDate( str, tz ) );
} // getDate


/*
** Name: getTime
**
** Description:
**	Return the current data value as a Time value.
**	A relative timezone may be provided which is
**	applied to adjust the final result such that it
**	represents the original date/time value in the
**	timezone provided.  The default is to use the
**	local timezone.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	tz	Relative timezone, NULL for local.
**
** Output:
**	None.
**
** Returns:
**	Time	Current value converted to Time.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public Time 
getTime( TimeZone tz ) 
    throws SqlEx
{ 
    String str = getString().trim();
    return( (tz == null) ? SqlDates.parseTime( str, false )
			 : SqlDates.parseTime( str, tz ) );
} // getTime


/*
** Name: getTimestamp
**
** Description:
**	Return the current data value as a Timestamp value.
**	A relative timezone may be provided which is
**	applied to adjust the final result such that it
**	represents the original date/time value in the
**	timezone provided.  The default is to use the
**	local timezone.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	tz	Relative timezone, NULL for local.
**
** Output:
**	None.
**
** Returns:
**	Timestamp	Current value converted to Timestamp.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public Timestamp 
getTimestamp( TimeZone tz ) 
    throws SqlEx
{ 
    String str = getString().trim();
    return( (tz == null) ? SqlDates.parseTimestamp( str, false )
			 : SqlDates.parseTimestamp( str, tz ) );
}


/*
** Name: getAsciiStream
**
** Description:
**	Return the current data value as an ASCII stream.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	InputStream	Current value converted to ASCII stream.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public InputStream 
getAsciiStream() 
    throws SqlEx
{ 
    return( getAscii() );
} // getAsciiStream


/*
** Name: getUnicodeStream
**
** Description:
**	Return the current data value as a Unicode stream.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	InputStream	Current value converted to Unicode stream.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public InputStream 
getUnicodeStream() throws SqlEx
{ 
    return( getUnicode() );
} // getUnicodeStream


/*
** Name: getCharacterStream
**
** Description:
**	Return the current data value as a character stream.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	Reader	Current value converted to character stream.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public Reader 
getCharacterStream() 
    throws SqlEx
{ 
    return( getCharacter() );
} // getCharacterStream


/*
** Name: getClob
**
** Description:
**	Returns the current data value buffered in a Clob wrapper.
**
**      Note: the value returned when the data value is
**      NULL is not well defined.
**
** Input:
**      None.
**
** Output:
**      None.
**
** Returns:
**      Clob    Current value buffered and wrapped as a Clob.
**
** History:
**      26-Feb-07 (gordy)
**          Created.
*/

public Clob
getClob()
    throws SqlEx
{
    return( new BufferedClob( getCharacter() ) );
}


/*
** Name: getObject
**
** Description:
**	Return the current data value as an Binary object.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	Object	Current value converted to Binary.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public Object 
getObject() 
    throws SqlEx
{
    return( getString() );
} // getObject


/*
** Name: getObject
**
** Description:
**	Return the current data value as an Binary object
**	with maximum size limit.
**
**	Note: the value returned when the data value is 
**	NULL is not well defined.
**
** Input:
**	limit	Maximum length of result.
**
** Output:
**	None.
**
** Returns:
**	Object	Current value converted to Binary.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public Object 
getObject( int limit ) 
    throws SqlEx
{
    return( getString( limit ) );
} // getObject


/*
** Name: cnvtIS2Rdr
**
** Description:
**	Converts the byte string input stream into a Reader stream.
**
**	This class uses Reader streams, so implemented as stub.
**
** Input:
**	stream	    Input stream.
**
** Output:
**	None.
**
** Returns:
**	Reader		Converted Reader stream.
**
** History:
**	12-Sep-03 (gordy)
*/

protected Reader
cnvtIS2Rdr( InputStream stream )
    throws SqlEx
{
    return( (Reader)null );	// Stub, should not be called.
} // cnvtIS2Rdr


/*
** Name: strm2str
**
** Description:
**	Read a character input stream and convert to a string object.  
**	The stream is closed.
**
** Input:
**	in	Character input stream.
**
** Output:
**	None.
**
** Returns:
**	String	The stream as a string.
**
** History:
**	15-Nov-06 (gordy)
**	    Created.
*/

public static String
strm2str( Reader in )
    throws SqlEx
{
    char	 cb[] = new char[ 8192 ];	// TODO: make dynamic?
    StringBuffer sb = new StringBuffer();
    int		 len;

    try
    {
	while( (len = in.read( cb, 0, cb.length )) >= 0 )
	    if ( len > 0 )  sb.append( cb, 0, len );
    }
    catch( IOException ex )
    {
	throw SqlEx.get( ERR_GC4007_BLOB_IO );
    }
    finally
    {
	try { in.close(); }
	catch( IOException ignore ) {}
    }

    return( sb.toString() );
} // strm2str


/*
** Name: strm2str
**
** Description:
**	Read a character input stream and convert to a string object.  
**	The stream is closed.
**
** Input:
**	in	Character input stream.
**	limit	Maximum length of string.
**
** Output:
**	None.
**
** Returns:
**	String	The stream as a string.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	15-Nov-06 (gordy)
**	    Made public.
*/

public static String
strm2str( Reader in, int limit )
    throws SqlEx
{
    char	 cb[] = new char[ 8192 ];	// TODO: make dynamic?
    StringBuffer sb = new StringBuffer();
    int		 len;

    try
    {
	while( sb.length() < limit  &&
	       (len = in.read( cb, 0, cb.length )) >= 0 )
	{
	    len = Math.min( len, limit - sb.length() );
	    if ( len > 0 )  sb.append( cb, 0, len );
	}
    }
    catch( IOException ex )
    {
	throw SqlEx.get( ERR_GC4007_BLOB_IO );
    }
    finally
    {
	try { in.close(); }
	catch( IOException ignore ) {}
    }

    return( sb.toString() );
} // strm2str


} // class SqlLongNChar
