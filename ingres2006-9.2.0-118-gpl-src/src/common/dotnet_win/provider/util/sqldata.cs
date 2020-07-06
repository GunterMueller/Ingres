/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;

namespace Ca.Ingres.Utility
{
	/*
	** Name: sqldata.cs
	**
	** Description:
	**	Defines base class which provides support for SQL data values.
	**
	**  Classes:
	**
	**	SqlData		Base class for NULL data.
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
	** Name: SqlData
	**
	** Description:
	**	Utility base class which provides NULL data capabilities
	**	and defines basic data access interface.
	**
	**	This class, in and of itself, only represents NULL typeless
	**	data values and cannot be directly instantiated.  Sub-classes
	**	provide support for specific datatype values.
	**
	**	This class implements a set of getXXX() methods which always
	**	throw conversions exceptions.  Sub-classes override those
	**	methods for which conversion of their specific datatype is
	**	supported.
	**
	**	This class defines routines for handling data truncation but
	**	the routines are only implemented for the non-truncation case.
	**	A separate sub-class should implement truncation support and
	**	act as base class for support of truncatable datatypes.
	**
	**  Public Methods:
	**
	**	toString		String representation of data value.
	**	setNull			Set data value NULL.
	**	isNull			Data value is NULL?
	**	isTruncated		Data value is truncated?
	**	getDataSize		Expected size of data value.
	**	getTruncSize		Actual size of truncated data value.
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
	**	getBytes
	**	getDate
	**	getTime
	**	getTimestamp
	**	getBinaryStream
	**	getAsciiStream
	**	getUnicodeStream
	**	getCharacterStream
	**	getObject
	**
	**  Protected Methods:
	**
	**	setNotNull		Set data value not NULL.
	**
	**  Private Data:
	**
	**	is_null			Data value is NULL?.
	**
	** History:
	**	 5-Sep-03 (gordy)
	**	    Created.
	*/

	internal class
		SqlData : GcfErr
	{

		private bool		is_null = true;


		/*
		** Name: SqlData
		**
		** Description:
		**	Constructor for sub-classes to define initial NULL
		**	state of the data value.
		**
		** Input:
		**	is_null		True if data value is initially NULL.
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

		protected internal
			SqlData( bool is_null )
		{
			this.is_null = is_null;
		} // SqlData


		/*
		** Name: ToString
		**
		** Description:
		**	Returns a string representing the current object.
		**
		**	Overrides super-class method to attempt conversion
		**	of SQL value to string using getString() method.
		**	Since getString() may not be implemented by a sub-
		**	class, an exception from getString() defaults to
		**	using the super-class method.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String	String representation of this SQL data value.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override String 
			ToString()
		{
			String str;

			try { str = getString(); }
			catch( SqlEx ) { str = base.ToString(); }

			return( str );
		} // toString


		/*
		** Name: setNotNull
		**
		** Description:
		**	Set the NULL state of the data value to not NULL.
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

		public virtual void
			setNotNull()
		{
			is_null = false;
			return;
		} // setNotNull


		/*
		** Name: setNull
		**
		** Description:
		**	Set the NULL state of the data value to NULL.
		**
		**	Sub-classes may need to override this method if the
		**	stored data value is dependent on the NULL state.
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

		public virtual void
			setNull()
		{
			is_null = true;
			return;
		} // setNull


		/*
		** Name: isNull
		**
		** Description:
		**	Returns the NULL state of the data value.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	bool		True if data value is NULL.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		internal virtual bool
			isNull()
		{
			return( is_null );
		} // isNull


		/*
		** Name: isTruncated
		**
		** Description:
		**	Returns an indication that the data value was truncated.
		**
		**	This implementation always returns False.  Sub-classes should 
		**	override this method to support truncatable datatypes.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	bool		True if truncated.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public virtual bool
			isTruncated()
		{
			return( false );
		} // isTruncated


		/*
		** Name: getDataSize
		**
		** Description:
		**	Returns the expected size of an untruncated data value.
		**	For datatypes whose size is unknown or varies, -1 is returned.
		**	The returned value is only valid for truncated data values
		**	(isTruncated() returns True).
		**
		**	This implementation always returns -1.  Sub-classes should 
		**	override this method to support truncatable datatypes.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int    Expected size of data value.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public virtual int
			getDataSize()
		{
			return( -1 );
		} // getDataSize


		/*
		** Name: getTruncSize
		**
		** Description:
		**	Returns the actual size of a truncated data value.  For 
		**	datatypes whose size is unknown or varies, -1 is returned.
		**	The returned value is only valid for truncated data values
		**	(isTruncated() returns True).
		**
		**	This implementation always returns 0.  Sub-classes should 
		**	override this method to support truncatable datatypes.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int    Truncated size of data value.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public virtual int
			getTruncSize()
		{
			return( 0 );
		} // getTruncSize


		/*
		** The following routines represent the set of valid accessor
		** methods for the data values supported by SQL.  For this
		** implementation these methods throw data conversion errors.  
		** Sub-classes should override those methods for which conversion 
		** of their datatype is supported.
		*/

		public virtual bool 
			getBoolean()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual byte 
			getByte()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual short 
			getShort()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual int 
			getInt()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual long 
			getLong()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual float 
			getFloat()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual double 
			getDouble()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual Decimal 
			getDecimal()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual String 
			getString()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual byte[] 
			getBytes()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual DateTime 
			getDate( TimeZone tz )
		{  throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual TimeSpan 
			getTime( TimeZone tz )
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual DateTime 
			getTimestamp( TimeZone tz )
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual InputStream 
			getBinaryStream()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual InputStream 
			getAsciiStream()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual InputStream 
			getUnicodeStream()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual Reader 
			getCharacterStream()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }

		public virtual Object 
			getObject()
		{ throw SqlEx.get( ERR_GC401A_CONVERSION_ERR ); }


		/*
		** The following variants of the accessor methods take a length 
		** limit parameter.  The limit is simply ignored by the stub 
		** methods implemented below.  Sub-classes should override when 
		** length limitations are relevent.
		*/

		public virtual String
			getString( int limit )
		{ return( getString() ); }

		public virtual byte[]
			getBytes( int limit )
		{ return( getBytes() ); }

		public virtual Object
			getObject( int limit )
		{ return( getObject() ); }

	} // class SqlData

	/// <summary>
	/// Class representing an input stream of bytes.
	/// </summary>
	internal class InputStream : System.IO.MemoryStream
	{
		public InputStream() :
			base()
		{}

		public InputStream(byte[] buf) :
			base(buf)
		{}

		public InputStream(byte[] buf, int maxlength) :
			this(buf, 0, (maxlength>0)?
				System.Math.Min(buf.Length, maxlength) : buf.Length )
		{}

		public InputStream(byte[] buf, int offset, int count) :
			base(buf, offset, count)
		{}

		[CLSCompliant(false)]
		public virtual int read()
		{
			byte[] buffer = new byte[1];

			int countread = this.Read(buffer, 0, 1);
			if (countread == -1)
				return (-1);
			return (int)buffer[0];
		}

		[CLSCompliant(false)]
		public virtual int read(byte[] buffer)
		{
			return this.Read(buffer, 0, buffer.Length);
		}

		[CLSCompliant(false)]
		public virtual int read(byte[] buffer, int offset, int count)
		{
			int countread = base.Read(buffer, offset, count);
			if (countread == 0)
				return (-1);
			return countread;
		}

		[CLSCompliant(false)]
		public virtual void close()
		{
			base.Close();
		}
	}

	/// <summary>
	/// Class to read a byte stream and convert it into a character stream
	/// using an encoding.
	/// </summary>
	internal class InputStreamReader : Reader
	{
		private System.IO.StreamReader inStream;

		public InputStreamReader(
			InputStream stream)
		{
			inStream = new System.IO.StreamReader(stream);
		}

		public InputStreamReader(
			InputStream stream, System.Text.Encoding encoding)
		{
			inStream = new System.IO.StreamReader(stream, encoding);
		}

		public InputStreamReader(
			InputStream stream, String charSetName)
		{
			System.Text.Encoding encoding;
			if (charSetName == "UTF-8")
				encoding = System.Text.Encoding.UTF8;
			else if (charSetName == "ASCII"  ||  charSetName == "US-ASCII" )
				encoding = System.Text.Encoding.ASCII;
			else throw new ArgumentOutOfRangeException(
				"charSetName", "Must be \"UTF-8\" or \"ASCII\".");

			inStream = new System.IO.StreamReader(stream, encoding);
		}

		/// <summary>
		/// Read a single character.
		/// </summary>
		/// <returns></returns>
		public override int read()
		{
			return inStream.Read();
		}

		public override int read(char[] buffer, int index, int count)
		{
			int countread = inStream.Read(buffer, index, count);
			if (countread == 0)
				return (-1);
			return countread;
		}

		public override void close()
		{
			inStream.Close();
		}

	}

	internal class ByteArrayInputStream : InputStream
	{
		public ByteArrayInputStream(byte[] buf) :
			base(buf)
		{}

		public ByteArrayInputStream(byte[] buf, int maxlength) :
			base(buf, 0, (maxlength>0)?
				System.Math.Min(buf.Length, maxlength) : buf.Length )
	{}

		public ByteArrayInputStream(byte[] buf, int offset, int count) :
			base(buf, offset, count)
		{}
	}

	/// <summary>
	/// Class for reading character streams.
	/// </summary>
	internal class Reader : System.IO.StringReader
	{
		public Reader() : base(String.Empty)
		{}

		public Reader(String s) : base(s)
		{}

		/// <summary>
		/// Read and return the next character, or -1 if end of stream.
		/// </summary>
		/// <returns>The next character from the stream or
		/// -1 if end of stream.</returns>
		[CLSCompliant(false)]
		public virtual int read()
		{
			return base.Read();
		}

		/// <summary>
		/// Read next characters into the char array.
		/// </summary>
		/// <returns>The number of characters read from the stream or
		/// -1 if end of stream.</returns>
		[CLSCompliant(false)]
		public virtual int read(char[] buffer)
		{
			return this.read(buffer, 0, buffer.Length);
		}

		[CLSCompliant(false)]
		public virtual int read(char[] buffer, int index, int count)
		{
			int countread = base.Read(buffer, index, count);
			if (countread == 0)
				return -1;
			return countread;
		}

		[CLSCompliant(false)]
		public virtual void close()
		{
			base.Close();
		}



	}
}  // namespace
