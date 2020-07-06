/*
** Copyright (c) 2003 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.util;

/*
** Name: SqlString.java
**
** Description:
**	Defines class which represents an SQL string value.
**
**  Classes:
**
**	SqlChar		An SQL Char value.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Added support for parameter types/values in addition to 
**	    existing support for columns.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

import	java.math.BigDecimal;
import	java.io.InputStream;
import	java.io.Reader;
import	java.util.TimeZone;
import	java.sql.Date;
import	java.sql.Time;
import	java.sql.Timestamp;


/*
** Name: SqlString
**
** Description:
**	Class which represents an SQL string value.  SQL string 
**	values are represented by Java Strings
**
**	Supports conversion to boolean, byte, short, int, long, float, 
**	double, BigDecimal, Date, Time, Timestamp, Object and streams.  
**
**	The data value accessor methods do not check for the NULL
**	condition.  The super-class isNull() method should first
**	be checked prior to calling any of the accessor methods.
**
**  Public Methods:
**
**	set			Assign a new data value.
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
**	getObject
**
**  Private Data:
**
**	value			String value.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Added parameter data value oriented methods.  Default
**	    functionality moved to base class.
*/

public class
SqlString
    extends SqlData
{

    private String		value = null;

    
/*
** Name: SqlString
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
**	 5-Sep-03 (gordy)
**	    Created.
*/

public
SqlString()
{
    super( true );
} // SqlString


/*
** Name: SqlString
**
** Description:
**	Class constructor for non-NULL data values.
**
** Input:
**	value	Initial string value.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
*/

public
SqlString( String value )
{
    super( false );
    this.value = value;
} // SqlString


/*
** Name: Set
**
** Description:
**	Assign a new non-NULL data value.
**
** Input:
**	value	New string value.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
*/

public void
set( String value )
{
    setNotNull();
    this.value = value;
    return;
} // set


/*
** Name: set
**
** Description:
**	Assign a new data value as a copy of an existing 
**	SQL data object.  If the input is NULL, a NULL 
**	data value results.
**
** Input:
**	data	The SQL data to be copied.
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
set( SqlString data )
{
    if ( data == null  ||  data.isNull() )
	setNull();
    else
    {
	setNotNull();
	value = data.value;
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
**	String	Current value.
**
** History:
**	 1-Dec-03 (gordy)
**	    Created.
*/

public String 
get() 
{
    return( value );
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
    {
	setNotNull();
	this.value = value;
    }
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
**	 5-Sep-03 (gordy)
**	    Created.
*/

public boolean 
getBoolean() 
    throws SqlEx
{
    String str = value.trim();
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
**	 5-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public byte 
getByte() 
    throws SqlEx
{
    byte val;
    
    try { val = Byte.parseByte( value.trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( val );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public short 
getShort() 
    throws SqlEx
{
    short val;
    
    try { val = Short.parseShort( value.trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( val );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public int 
getInt() 
    throws SqlEx
{
    int val;
    
    try { val = Integer.parseInt( value.trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( val );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public long 
getLong() 
    throws SqlEx
{
    long val;
    
    try { val = Long.parseLong( value.trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( val );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public float 
getFloat() 
    throws SqlEx
{
    float val;
    
    try { val = Float.parseFloat( value.trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( val );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public double 
getDouble() 
    throws SqlEx
{
    double val;
    
    try { val = Double.parseDouble( value.trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( val );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	24-May-06 (gordy)
**	    Convert parsing exception to SQL conversion exception.
*/

public BigDecimal 
getBigDecimal() 
    throws SqlEx
{
    BigDecimal val;
    
    try { val = new BigDecimal( value.trim() ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    return( val );
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
**	String	Current value converted to String.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
*/

public String 
getString() 
    throws SqlEx
{
    return( value );
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
**	limit	Maximum size of result.
**
** Output:
**	None.
**
** Returns:
**	String	Current value converted to String.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
*/

public String 
getString( int limit ) 
    throws SqlEx
{
    return( (limit < value.length()) ? value.substring( 0, limit ) : value );
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
    return( (tz == null) ? SqlDates.parseDate( value.trim(), false )
			 : SqlDates.parseDate( value.trim(), tz ) );
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
    return( (tz == null) ? SqlDates.parseTime( value.trim(), false )
			 : SqlDates.parseTime( value.trim(), tz ) );
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
    return( (tz == null) ? SqlDates.parseTimestamp( value.trim(), false )
			 : SqlDates.parseTimestamp( value.trim(), tz ) );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Utilize helper function in base class.
*/

public InputStream 
getAsciiStream() 
    throws SqlEx
{ 
    return( getAscii( value ) );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Utilize helper function in base class.
*/

public InputStream 
getUnicodeStream() 
    throws SqlEx
{
    return( getUnicode( value ) );
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
**	 5-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Utilize helper function in base class.
*/

public Reader 
getCharacterStream() 
    throws SqlEx
{ 
    return( getCharacter( value ) );
} // getCharacterStream


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
**	 5-Sep-03 (gordy)
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
**	limit	Maximum size of result.
**
** Output:
**	None.
**
** Returns:
**	Object	Current value converted to Binary.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
*/

public Object 
getObject( int limit ) 
    throws SqlEx
{
    return( getString( limit ) );
} // getObject


} // class SqlString
