/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.IO;
using System.Text;

namespace Ca.Ingres.Utility
{

	/*
	** Name: sqllongchar.cs
	**
	** Description:
	**	Defines class which represents an SQL LongVarchar value.
	**
	**  Classes:
	**
	**	SqlLongChar	An SQL LongVarchar value.
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
	** Name: SqlLongChar
	**
	** Description:
	**	Class which represents an SQL LongVarchar value.  SQL 
	**	LongVarchar values are potentially large variable length 
	**	streams.
	**
	**	Supports conversion to String, Binary, Object.  
	**
	**	The data value accessor methods do not check for the NULL
	**	condition.  The super-class isNull() method should first
	**	be checked prior to calling any of the accessor methods.
	**
	**	Extends the SqlLongNChar class by requiring a character-
	**	set which defines the conversion between bytes strea and 
	**	character stream and implementing the cnvtIS2Rdr method 
	**	to wrap the InputStream with a Reader to perform the 
	**	charactger-set conversion.
	**
	**  Inherited Methods:
	**
	**	setNull			Set a NULL data value.
	**	toString		String representation of data value.
	**	closeStream		Close the current stream.
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
	**  Public Methods:
	**
	**	set			Assign a new non-NULL data value.
	**
	**  Protected Mehtods:
	**
	**	cnvtIS2Rdr		Convert InputStream to Reader.
	**
	**  Private Data:
	**
	**	charSet			Character-set.
	**
	** History:
	**	12-Sep-03 (gordy)
	**	    Created.
	*/

	internal class
		SqlLongChar : SqlLongNChar
	{

		private CharSet		charSet = null;

    
		/*
		** Name: SqlLongChar
		**
		** Description:
		**	Class constructor.  Data value is initially NULL.
		**
		** Input:
		**	charSet		Character-set of byte stream.
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
			SqlLongChar( CharSet charSet ) : base( )
		{
			this.charSet = charSet;
		} // SqlLongChar


		/*
		** Name: SqlLongChar
		**
		** Description:
		**	Class constructor.  Data value is initially NULL.
		**	Defines a SqlStream event listener for stream 
		**	closure event notification.
		**
		** Input:
		**	charSet		Character-set of byte stream.
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
			SqlLongChar( CharSet charSet, IStreamListener listener ) : base( listener )
		{
			this.charSet = charSet;
		} // SqlLongChar


		/*
		** Name: set
		**
		** Description:
		**	Assign a new non-NULL data value.
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
			set( InputStream stream )
		{
			base.set( (IStreamSource)stream );
			return;
		} // set


		/*
		** Name: cnvtIS2Rdr
		**
		** Description:
		**	Converts the byte string input stream into a Reader
		**	stream using the associated character-set.
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
			try 
			{ 
				return( charSet.getISR( stream ) ); 
			}
			catch( Exception )			// Should not happen!
			{
				throw SqlEx.get( ERR_GC401E_CHAR_ENCODE );
			}
		} // cnvtIS2Rdr


	} // class SqlLongChar
}  // namespace
