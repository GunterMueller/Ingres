/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.IO;
using System.Text;

namespace Ca.Ingres.Utility
{

	/*
	** Name: sqlnchar.cs
	**
	** Description:
	**	Defines class which represents an SQL NChar value.
	**
	**  Classes:
	**
	**	SqlNChar	An SQL NChar value.
	**
	** History:
	**	 5-Sep-03 (gordy)
	**	    Created.
	**	 9-Dec-03 (thoda04)
	**	    Ported to .NET.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	*/



	/*
	** Name: SqlNChar
	**
	** Description:
	**	Class which represents an SQL NChar value.  SQL NChar values 
	**	are fixed length UCS2 based strings.  
	**
	**	Supports conversion to boolean, byte, short, int, long, float, 
	**	double, BigDecimal, Date, Time, Timestamp, Object and streams.  
	**
	**	This class implements the CharArray interface as the means
	**	to set the string value.  The maximum length and capacity
	**	of the array is determined by the fixed size of the string
	**	value.  
	**
	**	The string value may be set by first clearing the array and 
	**	then using the put() method to set the value.  Segmented 
	**	input values are handled by successive calls to put().  The 
	**	clear() method also clears the NULL setting.
	**
	**	The data value accessor methods do not check for the NULL
	**	condition.  The super-class isNull() method should first
	**	be checked prior to calling any of the accessor methods.
	**	Truncated input is automatically extended with space chars
	**	to the fixed length of the string value.
	**
	**  Interface Methods:
	**
	**	capacity		Determine capacity of the array.
	**	ensureCapacity		Set minimum capacity of the array.
	**	limit			Set or determine maximum capacity of the array.
	**	length			Determine the current length of the array.
	**	clear			Set array length to zero.
	**	get			Copy characters out of the array.
	**	put			Copy characters into the array.
	**
	**  Public Methods:
	**
	**	getBoolean		Data value accessor methods
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
	**  Protected Data:
	**
	**	value			String value.
	**	length			Current length.
	**
	**  Protected Methods:
	**
	**	ensure			Ensure minimum capacity.
	**	extend			Check input, extend if necessary.
	**
	** History:
	**	 5-Sep-03 (gordy)
	**	    Created.
	*/

	internal class
		SqlNChar :  SqlData, ICharArray
	{

		protected char[]	value = null;
		protected int   	value_length = 0;
	
    
		/*
		** Name: SqlNChar
		**
		** Description:
		**	Class constructor.  Data value is initially NULL.
		**
		** Input:
		**	length		The length of the string value.
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
			SqlNChar( int length ) : this()
		{
			this.value = new char[ length ];
		} // SqlNChar


		/*
		** Name: SqlNChar
		**
		** Description:
		**	Default constructor for UCS2 string datatypes.
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

		protected
			SqlNChar() : base( true )
		{
		} // SqlNChar


		/*
		** Name: capacity
		**
		** Description:
		**	Returns the current capacity of the array.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Current capacity.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			capacity()
		{
			return( value == null ? 0 : value.Length );
		} // capacity


		/*
		** Name: ensureCapacity
		**
		** Description:
		**	Set minimum capacity of the array.
		**
		**	Since the array length determines capacity, size limit,
		**	and the length of the string value, this implementation
		**	ignores any suggested mimimum capacity.
		**
		** Input:
		**	capacity	Minimum capacity.
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
			ensureCapacity( int capacity )
		{
			ensure( capacity );
			return;
		} // ensureCapacity


		/*
		** Name: limit
		**
		** Description:
		**	Determine the current maximum size of the array.
		**	If a maximum size has not been set, -1 is returned.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Maximum size or -1.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public virtual int
			limit()
		{
			return( value == null ? -1 : value.Length );
		} // limit


		/*
		** Name: limit
		**
		** Description:
		**	Set the maximum size of the array.  The array wll be
		**	truncated if the current length is greater than the
		**	new maximum size.
		**
		**	Since the array length determines capacity, size limit,
		**	and the length of the string value, setting the limit
		**	also effectively ensures the capacity.  A negative size
		**	is ignored.
		**
		** Input:
		**	size	Maximum size.
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

		public virtual void
			limit( int size )
		{
			if ( size >= 0  &&  (value == null  ||  value.Length != size) )
			{
				char[] ca = new char[ size ];
				if ( value_length > size )  value_length = size;
				if ( value != null  &&  value_length > 0 )
					Array.Copy( value, 0, ca, 0, value_length );
				value = ca;
			}
			return;
		} // limit


		/*
		** Name: limit
		**
		** Description:
		**	Set the maximum size of the array and optionally ensure
		**	the mimimum capacity.  
		**
		**	Note that limit(n, true) is equivilent to limit(n)
		**	followed by ensureCapacity(n) and limit(n, false) is
		**	the same as limit(n).
		**
		** Input:
		**	size	Maximum size.
		**	ensure	True if capacity should be ensured as max size.
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

		public virtual void
			limit( int size, bool do_ensure )
		{
			limit( size );
			if ( do_ensure )  ensure( size );
			return;
		} // limit


		/*
		** Name: length
		**
		** Description:
		**	Returns the current length of the array.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Current length.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			length()
		{
			return( value_length );
		} // length


		/*
		** Name: clear
		**
		** Description:
		**	Sets the length of the array to zero.  Also clears NULL setting.
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
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public void
			clear()
		{
			setNotNull();
			value_length = 0;
			return;
		} // clear


		/*
		** Name: put
		**
		** Description:
		**	Append a character value to the current array data.  The 
		**	portion of the input which would cause the array to grow 
		**	beyond the size limit (if set) is ignored.  The number of 
		**	characters actually appended is returned.
		**
		** Input:
		**	value	The character to append.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Number of characters appended.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int 
			put( char value )
		{
			/*
			** Is there room for the character.
			*/
			if ( value_length >= limit() )  return( 0 );

			/*
			** Save character and update array length.
			*/
			ensure( value_length + 1 );
			this.value[ value_length++ ] = value;
			return( 1 );
		}


		/*
		** Name: put
		**
		** Description:
		**	Append a character array to the current array data.  The 
		**	portion of the input which would cause the array to grow 
		**	beyond the size limit (if set) is ignored.  The number of 
		**	characters actually appended is returned.
		**
		** Input:
		**	chars	The character array to append.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Number of characters appended.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			put( char[] value )
		{
			return( put( value, 0, value.Length ) );
		} // put


		/*
		** Name: put
		**
		** Description:
		**	Append a portion of a character array to the current array 
		**	data.  The portion of the input which would cause the array 
		**	to grow beyond the size limit (if set) is ignored.  The 
		**	number of characters actually appended is returned.
		**
		** Input:
		**	chars	Array containing characters to be appended.
		**	offset	Start of portion to append.
		**	length	Number of bytes to append.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Number of characters appended.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			put( char[] value, int offset, int length )
		{
			/*
			** Determine how many bytes to copy.
			*/
			int unused = limit() - this.value_length;
			if ( length > unused )  length = unused;
    
			/*
			** Copy bytes and update array length.
			*/
			ensure( this.value_length + length );
			Array.Copy( value, offset, this.value, this.value_length, length );
			this.value_length += length;
			return( length );
		} // put


		/*
		** Name: get
		**
		** Description:
		**	Returns a copy of the array.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	char[]	Copy of the current array.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public char[] 
			get()
		{
			char[] chars = new char[ value_length ];
			if ( value_length > 0 )  Array.Copy( value, 0, chars, 0, value_length );
			return( chars );
		} // get


		/*
		** Name: get
		**
		** Description:
		**	Copy characters out of the array.  Copying starts with the first
		**	character of the array.  The number of characters copied is the 
		**	minimum of the current array length and the length of the output 
		**	array.
		**
		** Input:
		**	None.
		**
		** Output:
		**	buff	Character array to receive output.
		**
		** Returns:
		**	int	Number of characters copied.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			get( char[] value )
		{
			return( get( 0, value.Length, value, 0 ) );
		} // get


		public int
			get( int position, int length, char[] value, int offset )
		{
			/*
			** Determine how many bytes to copy.
			*/
			int avail = (position >= this.value_length) ? 0 : this.value_length - position;
			if ( length > avail )  length = avail;
    
			/*
			** Copy bytes and update array length.
			*/
			if ( length > 0 )
				Array.Copy( this.value, position, value, offset, length );
			return( length );
		} // get


		/*
		** Name: ensure
		**
		** Description:
		**	Set minimum capacity of the array.
		**
		**	Since the array length determines capacity, size limit,
		**	and the length of the string value, this implementation
		**	ignores any suggested mimimum capacity.
		**
		** Input:
		**	capacity	Required capacity.
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

		protected virtual void
			ensure( int capacity )
		{
			return;
		} // ensure


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

		public override bool
			getBoolean() 
		{
			String str = getString( value_length ).Trim();
			return( str.Equals("1") ? true : false );
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

		public override byte 
			getByte() 
		{
			return( Byte.Parse( getString( value_length ).Trim() ) );
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

		public override short 
			getShort() 
		{
			return( Int16.Parse( getString( value_length ).Trim() ) );
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

		public override int 
			getInt() 
		{
			return( Int32.Parse( getString( value_length ).Trim() ) );
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

		public override long 
			getLong() 
		{
			return( Int64.Parse( getString( value_length ).Trim() ) );
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

		public override float 
			getFloat() 
		{
			return( Single.Parse( getString( value_length ).Trim() ) );
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

		public override double 
			getDouble() 
		{
			return( Double.Parse( getString( value_length ).Trim() ) );
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

		public override Decimal 
			getDecimal() 
		{
			return( Decimal.Parse( getString( value_length ).Trim() ) );
		} // getBigDecimal


		/*
		** Name: getBigDecimal
		**
		** Description:
		**	Return the current data value as a BigDecimal value
		**	with a specific scale.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	scale		Desired scale.
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

		public Decimal 
			getBigDecimal( int scale ) 
		{
			Decimal bd = Decimal.Parse( getString( value_length ).Trim() );
			return bd;
			//    return( bd.setScale( scale, BigDecimal.ROUND_HALF_UP ) );
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
		*/

		public override String 
			getString() 
		{
			extend();
			return( new String(value, 0, value_length ) );
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

		public override String 
			getString( int limit ) 
		{
			/*
			** Only need to extend if max size is greater than current length.
			*/
			if ( limit > value_length )  extend();
    
			/*
			** Extended size may be less or greater than max size.
			*/
			return( new String( value, 0, Math.Min( value_length, limit ) ) );
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

		public override DateTime
			getDate( TimeZone tz ) 
		{ 
			String str = getString( value_length ).Trim();
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

		public override TimeSpan
			getTime( TimeZone tz ) 
		{ 
			String str = getString( value_length ).Trim();
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

		public override DateTime 
			getTimestamp( TimeZone tz ) 
		{ 
			String str = getString( value_length ).Trim();
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
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override InputStream 
			getAsciiStream() 
		{ 
			String	str = getString();
			byte[]	bytes;
    
			try
			{
				System.Text.Encoding ASCIIEncoding = System.Text.Encoding.ASCII;
				bytes = ASCIIEncoding.GetBytes(str);
			}
			catch( Exception )		// Should not happen!
			{ throw SqlEx.get( ERR_GC401E_CHAR_ENCODE ); }
    
			return( new ByteArrayInputStream( bytes ) );
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
		*/

		public override InputStream 
			getUnicodeStream()
		{ 
			String	str = getString();
			byte[]	bytes;

			try
			{
				System.Text.Encoding UTF8Encoding = System.Text.Encoding.UTF8;
				bytes = UTF8Encoding.GetBytes(str);
			}
			catch( Exception )		// Should not happen!
			{ throw SqlEx.get( ERR_GC401E_CHAR_ENCODE ); }

			return( new ByteArrayInputStream( bytes ) );
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
		*/

		public override Reader 
			getCharacterStream() 
		{ 
			return( new Reader( getString() ) );
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

		public override Object 
			getObject() 
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

		public override Object 
			getObject( int limit ) 
		{
			return( getString( limit ) );
		} // getObject


		/*
		** Name: extend
		**
		** Description:
		**	Check input length and extend with space characters if necessary.
		**	This routine may be called multiple times with no side-effects.  
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
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		protected virtual void
			extend()
		{
			for( ; value_length < value.Length; value_length++ )
				value[ value_length ] = ' ';
			return;
		} // extend


	} // class SqlNChar
}  // namespace
