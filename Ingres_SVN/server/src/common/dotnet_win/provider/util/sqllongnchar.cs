/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.IO;
using System.Text;

namespace Ca.Ingres.Utility
{


	/*
	** Name: sqllongnchar.cs
	**
	** Description:
	**	Defines class which represents an SQL LongNVarchar value.
	**
	**  Classes:
	**
	**	SqlLongNChar	An SQL LongNVarchar value.
	**
	** History:
	**	12-Sep-03 (gordy)
	**	    Created.
	**	 9-Dec-03 (thoda04)
	**	    Ported to .NET.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	*/



	/*
	** Name: SqlLongNChar
	**
	** Description:
	**	Class which represents an SQL LongNVarchar value.  SQL LongNVarchar 
	**	values are potentially large variable length streams.
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
	**  Protected Methods:
	**
	**	cnvtIS2Rdr		Convert InputStream to Reader (Stub).
	**
	**  Private Methods:
	**
	**	strm2str		Convert character stream to string.
	**
	** History:
	**	12-Sep-03 (gordy)
	**	    Created.
	*/

	internal class
		SqlLongNChar : SqlStream
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
			SqlLongNChar() : base()
		{
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

		internal
			SqlLongNChar( IStreamListener listener ) : base( listener )
		{
		} // SqlLongNChar


			/*
			** Name: set
			**
			** Description:
			**	Assign a new non-NULL data value.  The input stream
			**	must also implement the SqlStream.IStreamSource
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
			{
				base.set( (IStreamSource)stream );
				return;
			} // set


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

		public override bool
			getBoolean() 
		{
			String str = strm2str( getCharacter(), -1 ).Trim();
			return( str.Equals("1") ? true : false);
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
		*/

		public override byte 
			getByte() 
		{
			return( Byte.Parse( strm2str( getCharacter(), -1 ).Trim() ) );
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
		*/

		public override short 
			getShort() 
		{
			return( Int16.Parse( strm2str( getCharacter(), -1 ).Trim() ) );
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
		*/

		public override int 
			getInt() 
		{
			return( Int32.Parse( strm2str( getCharacter(), -1 ).Trim() ) );
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
		*/

		public override long 
			getLong() 
		{
			return( Int64.Parse( strm2str( getCharacter(), -1 ).Trim() ) );
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
		*/

		public override float 
			getFloat() 
		{
			return( Single.Parse( strm2str( getCharacter(), -1 ).Trim() ) );
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
		*/

		public override double 
			getDouble() 
		{
			return( Double.Parse( strm2str( getCharacter(), -1 ).Trim() ) );
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
		*/

		public override Decimal 
			getDecimal() 
		{
			return( Decimal.Parse( strm2str( getCharacter(), -1 ).Trim() ) );
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
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public Decimal 
			getBigDecimal( int scale ) 
		{
			Decimal bd = Decimal.Parse( strm2str( getCharacter(), -1 ).Trim() );
			return bd;
//			return( bd.setScale( scale, BigDecimal.ROUND_HALF_UP ) );
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

		public override String 
			getString() 
		{
			return( strm2str( getCharacter(), -1 ) );
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

		public override String 
			getString( int limit ) 
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

		public override DateTime 
			getDate( TimeZone tz ) 
		{
			String str = strm2str( getCharacter(), -1 ).Trim();
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
			String str = strm2str( getCharacter(), -1 ).Trim();
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
			String str = strm2str( getCharacter(), -1 ).Trim();
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

		public override InputStream 
			getAsciiStream() 
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

		public override InputStream 
			getUnicodeStream()
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

		public override Reader 
			getCharacterStream() 
		{ 
			return( getCharacter() );
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
		**	12-Sep-03 (gordy)
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

		public override Object 
			getObject( int limit ) 
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

		protected override Reader
			cnvtIS2Rdr( InputStream stream )
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
		**	limit	Maximum size of result, negative for no limit
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
		*/

		private static String
			strm2str( Reader inReader, int limit )
		{
			char[]           cb = new char[ 8192 ];
			StringBuilder    sb = new StringBuilder();
			int              len;

			try
			{
				while( (limit < 0  ||  sb.Length < limit)  &&
					(len = inReader.read( cb, 0, cb.Length )) >= 0 )
				{
					if ( limit >= 0 )  len = Math.Min( len, limit - sb.Length );
					if ( len > 0 )  sb.Append( cb, 0, len );
				}
			}
			catch( IOException )
			{
				throw SqlEx.get( ERR_GC4007_BLOB_IO );
			}
			finally
			{
				try { inReader.close(); }
				catch( IOException ) {}
			}

			return( sb.ToString() );
		} // strm2str


	} // class SqlLongNChar
}  // namespace
