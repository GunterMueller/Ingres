/*
** Copyright (c) 2003 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.util;

/*
** Name: SqlDecimal.java
**
** Description:
**	Defines class which represents an SQL Decimal value.
**
**  Classes:
**
**	SqlDecimal	An SQL Decimal value.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Added support for parameter types/values in addition to 
**	    existing support for columns.
**	 4-Aug-05 (gordy)
**	    Enforce the Ingres max decimal precision of 31 by rounding
**	    scale digits to reduce the precision.
**	 7-Jun-07 (gordy)  BUG 118470
**	    BigDecimal.toString() output changed with Java 1.5.
*/

import	java.math.BigDecimal;


/*
** Name: SqlDecimal
**
** Description:
**	Class which represents an SQL Decimal value.  
**
**	Supports conversion to boolean, byte, short, integer, long, 
**	float, double, String and Object.
**
**	The data value accessor methods do not check for the NULL
**	condition.  The super-class isNull() method should first
**	be checked prior to calling any of the accessor methods.
**
**	Decimal values are truncatable.  A truncated decimal value
**	results in data value of 0.0.
**
**  Public Methods:
**
**	set			Assign a new data value.
**	get			Retrieve current data value.
**	isTruncated		Data value is truncated?
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
**	getObject
**
**  Protected Methods:
**
**	setScale		Set decimal scale
**
**  Private Data:
**
**	value			The decimal value.
**	truncated		Decimal value is truncted.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
**	 1-Dec-03 (gordy)
**	    Added parameter data value oriented methods.
**	 4-Aug-05 (gordy)
**	    Added maxPrecision constant.
*/

public class
SqlDecimal
    extends SqlData
{

    private BigDecimal		value = null;
    private boolean		truncated = false;

    static private final int	maxPrecision	= 31;	/* Max Precision */


/*
** Name: SqlDecimal
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
SqlDecimal()
{
    super( true );
} // SqlDecimal


/*
** Name: set
**
** Description:
**	Assign a new data value.  If the input is NULL,
**	a NULL data value results.
**
** Input:
**	value		The new data value.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
*/

public void
set( String value )
{
    if ( value == null )
    {
	setNull();
	truncated = false;
    }
    else  if ( value.length() > 0 )
    {
	this.value = new BigDecimal( value );
	setNotNull();
	truncated = false;
    }
    else
    {
	this.value = new BigDecimal( 0.0 );
	setNotNull();
	truncated = true;
    }
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
set( SqlDecimal data )
{
    if ( data == null  ||  data.isNull() )
    {
	setNull();
	truncated = false;
    }
    else
    {
	setNotNull();
	value = data.value;
	truncated = data.truncated;
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
**	 7-Jun-07 (gordy)
**	    BigDecimal.toString() output changed with Java 1.5.  Use
**	    new toPlainString() method which produces original format.
*/

public String 
get() 
{
    return( value.toPlainString() );
} // get


/*
** Name: isTruncated
**
** Description:
**	Returns an indication that the data value was truncated.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean		True if truncated.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
*/

public boolean
isTruncated()
{
    return( truncated );
} // isTruncated


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
    setNotNull();
    truncated = false;
    this.value = BigDecimal.valueOf( value ? 1L : 0L );
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
    setNotNull();
    truncated = false;
    this.value = BigDecimal.valueOf( (long)value );
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
    setNotNull();
    truncated = false;
    this.value = BigDecimal.valueOf( (long)value );
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
    setNotNull();
    truncated = false;
    this.value = BigDecimal.valueOf( (long)value );
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
    setNotNull();
    truncated = false;
    this.value = BigDecimal.valueOf( value );
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
**	 4-Aug-05 (gordy)
**	    Adjust precision to valid range.
*/

public void 
setFloat( float value ) 
    throws SqlEx
{
    BigDecimal temp;

    try { temp = new BigDecimal( (double)value ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    this.value = checkPrecision( temp );
    setNotNull();
    truncated = false;
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
**	 4-Aug-05 (gordy)
**	    Adjust precision to valid range.
*/

public void 
setDouble( double value ) 
    throws SqlEx
{
    BigDecimal temp;

    try { temp = new BigDecimal( value ); }
    catch( NumberFormatException ex )
    { throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

    this.value = checkPrecision( temp );
    setNotNull();
    truncated = false;
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
**	 4-Aug-05 (gordy)
**	    Adjust precision to valid range.
*/

public void 
setBigDecimal( BigDecimal value ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else
    {
	this.value = checkPrecision( value );
	setNotNull();
    }
    
    truncated = false;
    return;
} // setBigDecimal


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
**	scale	Number of decimal places.
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
**	 4-Aug-05 (gordy)
**	    Adjust precision to valid range.
*/

public void 
setBigDecimal( BigDecimal value, int scale ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else
    {
	value = value.setScale( scale, BigDecimal.ROUND_HALF_UP );
	this.value = checkPrecision( value );
	setNotNull();
    }
    
    truncated = false;
    return;
} // setBigDecimal


/*
** Name: setScale
**
** Description:
**	Set the scale of the DECIMAL value.
**
** Input:
**	scale	Number of decimal places.
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
**	 4-Aug-05 (gordy)
**	    Adjust precision to valid range.
*/

protected void
setScale( int scale )
    throws SqlEx
{
    if ( value != null )
	if ( value.scale() < scale )	/* Raising scale raises precision */
	    value = checkPrecision( value.setScale(scale, 
						   BigDecimal.ROUND_HALF_UP) );
	else
	    value = value.setScale( scale, BigDecimal.ROUND_HALF_UP );
    return;
} // setScale


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
**	 4-Aug-05 (gordy)
**	    Adjust precision to valid range.
*/

public void 
setString( String value ) 
    throws SqlEx
{
    if ( value == null )
	setNull();
    else
    {
	BigDecimal temp;

	try { temp = new BigDecimal( value.trim() ); }
	catch( NumberFormatException ex )
	{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }
	
	this.value = checkPrecision( temp );
	setNotNull();
    }
    
    truncated = false;
    return;
} // setString


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
    return( (value.signum() == 0) ? false : true );
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
*/

public byte 
getByte() 
    throws SqlEx
{
    return( value.byteValue() );
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
*/

public short 
getShort() 
    throws SqlEx
{
    return( value.shortValue() );
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
*/

public int 
getInt() 
    throws SqlEx
{
    return( value.intValue() );
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
*/

public long 
getLong() 
    throws SqlEx
{
    return( value.longValue() );
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
*/

public float 
getFloat() 
    throws SqlEx
{
    return( value.floatValue() );
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
*/

public double 
getDouble() 
    throws SqlEx
{
    return( value.doubleValue() );
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
*/

public BigDecimal 
getBigDecimal() 
    throws SqlEx
{
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
**	 5-Sep-03 (gordy)
**	    Created.
**	 7-Jun-07 (gordy)
**	    BigDecimal.toString() output changed with Java 1.5.  Use
**	    new toPlainString() method which produces original format.
*/

public String 
getString() 
    throws SqlEx
{
    return( value.toPlainString() );
} // getString


/*
** Name: getObject
**
** Description:
**	Return the current data value as a BigDecimal object.
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
**	Object	Current value converted to BigDecimal.
**
** History:
**	 5-Sep-03 (gordy)
**	    Created.
*/

public Object 
getObject() 
    throws SqlEx
{
    return( getBigDecimal() );
} // getObject


/*
** Name: checkPrecision
**
** Description:
**	Verifies that precision of a BigDecimal value is within
**	the range supported by the Ingres decimal data-type.
**	Returns original value if precision is OK.  Returns an 
**	updated value if precision exceeds range and value can 
**	be adjusted (scaled) with minimal change, otherwise an
**	exception is thrown.
**
** Input:
**	value	BigDecimal to check.
**
** Output:
**	None.
**
** Returns:
**	BigDecimal	Original or updated value.
**
** History:
**	 4-Aug-05 9gordy)
**	    Created.
*/

static private BigDecimal
checkPrecision( BigDecimal value )
    throws SqlEx
{
    /*
    ** Precision determined by number of digits in the 
    ** unscaled decimal value (watch out for sign).
    */
    int precision = value.unscaledValue().toString().length() -
		    ((value.signum() < 0) ? 1 : 0);
    int	scale = value.scale();

    /*
    ** Scale can exceed precision determined above due
    ** to zero's  immediately following decimal point.
    */
    int digits = Math.max( precision, scale ) - maxPrecision;

    if ( digits > 0 )
    {
	/*
	** Precision can be adjusted by truncating decimal
	** digits (reducing scale by rounding).  Conversion
	** exception is thrown if insufficient scale is
	** available (can't change number of integer digits).
	*/
	if ( scale >= digits )
	    // TODO: set truncation indication
	    value = value.setScale( scale - digits, BigDecimal.ROUND_HALF_UP );
	else
	    throw SqlEx.get( ERR_GC401A_CONVERSION_ERR );
    }

    return( value );
} // checkPrecision


} // class SqlDecimal
