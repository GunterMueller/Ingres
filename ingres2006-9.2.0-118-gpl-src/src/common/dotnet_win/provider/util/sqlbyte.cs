/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.IO;
using System.Text;


namespace Ca.Ingres.Utility
{
	/*
	** Name: sqlbyte.cs
	**
	** Description:
	**	Defines class which represents an SQL Binary value.
	**
	**  Classes:
	**
	**	SqlByte		An SQL Binary value.
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
	** Name: SqlByte
	**
	** Description:
	**	Class which represents an SQL Binary value.  SQL Binary 
	**	values are fixed length.
	**
	**	Supports conversion to String, Object and streams.  
	**
	**	This class implements the ByteArray interface as the means
	**	to set the binary value.  The maximum length and capacity
	**	of the array is determined by the fixed size of the binary
	**	value.  
	**
	**	The binary value may be set by first clearing the array and 
	**	then using the put() method to set the value.  Segmented 
	**	input values are handled by successive calls to put().  The 
	**	clear() method also clears the NULL setting.
	**
	**	The data value accessor methods do not check for the NULL
	**	condition.  The super-class isNull() method should first
	**	be checked prior to calling any of the accessor methods.
	**	Truncated input is automatically extended with 0 bytes to
	**	the fixed length of the binary value.
	**
	**  Interface Methods:
	**
	**	capacity		Determine capacity of the array.
	**	ensureCapacity		Set minimum capacity of the array.
	**	limit			Set or determine maximum capacity of the array.
	**	length			Determine the current length of the array.
	**	clear			Set array length to zero.
	**	get			Copy bytes out of the array.
	**	put			Copy bytes into the array.
	**
	**  Public Methods:
	**
	**	bin2str			Convert byte array to hex string.
	**	getString		Data value accessor methods
	**	getBytes
	**	getBinaryStream
	**	getAsciiStream
	**	getUnicodeStream
	**	getCharacterStream
	**	getObject
	**
	**  Protected Data:
	**
	**	value       	The binary value.
	**	value_length	The current length.
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
		SqlByte : SqlData, IByteArray
	{

		protected byte[]	value = null;
		protected int   	value_length = 0;

    
		/*
		** Name: SqlByte
		**
		** Description:
		**	Class constructor.  Data value is initially NULL.
		**
		** Input:
		**	count		The length of the binary value.
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
			SqlByte( int count ) : this()
		{
			this.value = new byte[ count ];
		} // SqlByte


		/*
		** Name: SqlByte
		**
		** Description:
		**	Default constructor for binary datatypes.
		**	Data value is initially NULL.
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
			SqlByte() : base( true )
		{
		} // SqlByte


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
		**	and the length of the binary value, this implementation
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

		public virtual void
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
		**	and the length of the binary value, setting the limit
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

		public void
			limit( int size )
		{
			if ( size >= 0  &&  (value == null  ||  value.Length != size) )
			{
				byte[] ba = new byte[ size ];
				if ( value_length > size )  value_length = size;
				if ( value != null  &&  value_length > 0 )
					Array.Copy( value, 0, ba, 0, value_length );
				value = ba;
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

		public void
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
		**	Append a byte value to the current array data.  The portion
		**	of the input which would cause the array to grow beyond the
		**	size limit (if set) is ignored.  The number of bytes actually 
		**	appended is returned.
		**
		** Input:
		**	value	The byte to append.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Number of bytes appended.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int 
			put( byte value )
		{
			/*
			** Is there room for the byte.
			*/
			if ( value_length >= limit() )  return( 0 );

			/*
			** Save byte and update array length.
			*/
			ensure( value_length + 1 );
			this.value[ value_length++ ] = value;
			return( 1 );
		}


		/*
		** Name: put
		**
		** Description:
		**	Append a byte array to the current array data.  The portion
		**	of the input which would cause the array to grow beyond the
		**	size limit (if set) is ignored.  The number of bytes actually 
		**	appended is returned.
		**
		** Input:
		**	bytes	    The byte array to append.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	    Number of bytes appended.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			put( byte[] value )
		{
			return( put( value, 0, value.Length ) );
		} // put


		/*
		** Name: put
		**
		** Description:
		**	Append a portion of a byte array to the current array data.  
		**	The portion of the input which would cause the array to grow 
		**	beyond the size limit (if set) is ignored.  The number of 
		**	bytes actually appended is returned.
		**
		** Input:
		**	bytes	    Array containing bytes to be appended.
		**	offset	    Start of portion to append.
		**	count 	    Number of bytes to append.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	    Number of bytes appended.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			put( byte[] value, int offset, int count )
		{
			/*
			** Determine how many bytes to copy.
			*/
			int unused = limit() - this.value_length;
			if ( count > unused )  count = unused;
    
			/*
			** Copy bytes and update array length.
			*/
			ensure( this.value_length + count );
			Array.Copy( value, offset, this.value, this.value_length, count );
			this.value_length += count;
			return( count );
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
		**	byte[]	Copy of the current array.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public byte[] 
			get()
		{
			byte[] bytes = new byte[ value_length ];
			if ( value_length > 0 )  Array.Copy( value, 0, bytes, 0, value_length );
			return( bytes );
		} // get


		/*
		** Name: get
		**
		** Description:
		**	Copy bytes out of the array.  Copying starts with the first
		**	byte of the array.  The number of bytes copied is the minimum
		**	of the current array length and the length of the output array.
		**
		** Input:
		**	None.
		**
		** Output:
		**	buff	Byte array to receive output.
		**
		** Returns:
		**	int	Number of bytes copied.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			get( byte[] value )
		{
			return( get( 0, value.Length, value, 0 ) );
		} // get


		/*
		** Name: get
		**
		** Description:
		**	Copy a portion of the array.  Copying starts at the position
		**	indicated.  The number of bytes copied is the minimum of the
		**	length requested and the number of bytes in the array starting
		**	at the requested position.  If position is greater than the
		**	current length, no bytes are copied.  The output array must
		**	have sufficient space.  The actual number of bytes copied is
		**	returned.
		**
		** Input:
		**	position	Starting byte to copy.
		**	count   	Number of bytes to copy.
		**	offset  	Starting position in output array.
		**
		** Output:
		**	buff		Byte array to recieve output.
		**
		** Returns:
		**	int		Number of bytes copied.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public int
			get( int position, int count, byte[] value, int offset )
		{
			/*
			** Determine how many bytes to copy.
			*/
			int avail = (position >= this.value_length) ?
				0 : this.value_length - position;
			if ( count > avail )  count = avail;
    
			/*
			** Copy bytes and update array length.
			*/
			if ( count > 0 )  
				Array.Copy( this.value, position, value, offset, count );
			return( count );
		} // get


		/*
		** Name: ensure
		**
		** Description:
		**	Set minimum capacity of the array.
		**
		**	Since the array length determines capacity, size limit,
		**	and the length of the binary value, this implementation
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

		protected internal virtual void
			ensure( int capacity )
		{
			return;
		} // ensure


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

		public override String 
			getString() 
		{
			extend();
			return( bin2str( value, 0, value_length ) );
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
			extend();
			return( bin2str( value, 0, Math.Min( value_length, limit ) ) );
		} // getString


		/*
		** Name: getBytes
		**
		** Description:
		**	Return the current data value as a byte array.
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
		**	byte[]		Current value converted to byte array.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override byte[] 
			getBytes() 
		{
			extend();
			return( get() );
		}


		/*
		** Name: getBytes
		**
		** Description:
		**	Return the current data value as a byte array
		**	with a maximum size limit.
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
		**	byte[]	Current value converted to byte array.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override byte[] 
			getBytes( int limit ) 
		{
			extend();
			byte[] bytes = new byte[ Math.Min( value_length, limit ) ];
			get( 0, bytes.Length, bytes, 0 );
			return( bytes );
		}


		/*
		** Name: getBinaryStream
		**
		** Description:
		**	Return the current data value as a binary stream.
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
		**	InputStream	Current value converted to binary stream.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override InputStream 
			getBinaryStream() 
		{
			extend();
			return( new ByteArrayInputStream( value, 0, value_length ) );
		} // getBinaryStream


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
			extend();
			String str = bin2str( value, 0, value_length );
			byte[] bytes;

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
			extend();
			String str = bin2str( value, 0, value_length);
			byte[] bytes;

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
			extend();
			return( new Reader( bin2str( value, 0, value_length ) ) );
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
			return( getBytes() );
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
			return( getBytes( limit ) );
		} // getObject


		/*
		** Name: extend
		**
		** Description:
		**	Check input length and extend with 0 bytes if necessary.
		**	This routine may be called multiple times with no side-
		**	effects.  
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
				value[ value_length ] = 0;
			return;
		} // extend


		/*
		** Name: bin2str
		**
		** Description:
		**	Convert a byte array to a hex string.
		**
		** Input:
		**	bytes	Byte array.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String	Hex string representation.
		**
		** History:
		**	31-Jan-99 (rajus01)
		**	    Created.
		**	 5-Sep-03 (gordy)
		**	    Generalize parameters to allows sub-arrays.
		*/

		public static String
			bin2str( byte[] bytes, int offset, int count )
		{
			StringBuilder	sb = new StringBuilder( count * 2 );
			String		str;

			for( ; count > 0; offset++, count-- )
			{
				str = toHexString( bytes[ offset ]);
				if ( str.Length == 1 ) sb.Append( '0' );
				sb.Append( str );
			}

			return( sb.ToString() );	
		} // bin2str


	} // class SqlByte
}  // namespace
