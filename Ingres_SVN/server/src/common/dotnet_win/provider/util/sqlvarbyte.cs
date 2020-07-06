/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.IO;
using System.Text;

namespace Ca.Ingres.Utility
{

	/*
	** Name: sqlvarbyte.cs
	**
	** Description:
	**	Defines class which represents an SQL VarBinary value.
	**
	**  Classes:
	**
	**	SqlVarByte	An SQL VarBinary value.
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
	** Name: SqlVarByte
	**
	** Description:
	**	Class which represents an SQL VarBinary value.  SQL VarBinary 
	**	values are variable length.
	**
	**	Supports conversion to String, Object and streams.  
	**
	**	This class implements the ByteArray interface as the means
	**	to set the binary value.  The maximum length of the array is 
	**      determined by the maximum size of the binary value.  Capacity
	**	varies as needed to handle the actual binary value size.
	**
	**	The binary value may be set by first clearing the array and 
	**	then using the put() method to set the value.  Segmented 
	**	input values are handled by successive calls to put().  The 
	**	clear() method also clears the NULL setting.
	**
	**	The data value accessor methods do not check for the NULL
	**	condition.  The super-class isNull() method should first
	**	be checked prior to calling any of the accessor methods.
	**
	**  Inherited Methods:
	**
	**	capacity		Determine capacity of the array.
	**	ensureCapacity		Set minimum capacity of the array.
	**	limit			Set or determine maximum capacity of the array.
	**	length			Determine the current length of the array.
	**	clear			Set array length to zero.
	**	get			Copy bytes out of the array.
	**	put			Copy bytes into the array.
	**	getString		Data value accessor methods
	**	getBytes
	**	getBinaryStream
	**	getAsciiStream
	**	getUnicodeStream
	**	getCharacterStream
	**	getObject
	**
	**  Interface Methods:
	**
	**	limit			Set or determine maximum capacity of the array.
	**
	**  Protected Data:
	**
	**	size			Maximum length.
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
		SqlVarByte :  SqlByte, IByteArray
	{

		private int			size = -1;

    
		/*
		** Name: SqlVarByte
		**
		** Description:
		**	Class constructor.  Data value is initially NULL.
		**
		** Input:
		**	size		The maximum size of the binary value.
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
			SqlVarByte( int size ) : base()
		{
			this.size = size;
		} // SqlVarByte


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

		public override int
			limit()
		{
			return( size );
		} // limit


		/*
		** Name: limit
		**
		** Description:
		**	Set the maximum size of the array.  The array wll be
		**	truncated if the current length is greater than the
		**	new maximum size.  A negative size is ignored.
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

		public new void
			limit( int size )
		{
			if ( size >= 0 )  
			{
				this.size = size;
				if ( value_length > size )  value_length = size;
			}
			return;
		} // limit


		/*
		** Name: ensure
		**
		** Description:
		**	Allocates or expands the value array to ensure sufficient
		**	space is available.  Capacity is limited to maximum size
		**	of the binary value.
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
		**	13-Oct-03 (gordy)
		**	    Forgot to reset value after copy into temp buffer.
		*/

		protected internal override void
			ensure( int capacity )
		{
			if ( capacity > size )  capacity = size;

			if ( value == null )
				value = new byte[ capacity ];
			else  if ( value.Length < capacity )
			{
				byte[] ba = new byte[ capacity ];
				if ( value_length > 0 )
					Array.Copy( value, 0, ba, 0, value_length );
				value = ba;
			}
			return;
		} // ensure


		/*
		** Name: extend
		**
		** Description:
		**	Check input length and extend with 0 bytes if necessary.
		**	This routine may be called multiple times with no side-
		**	effects.
		**
		**	Variable length arrays are never extended, so do nothing.
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

		protected override void
			extend()
		{
			return;
		} // extend


	} // class SqlVarByte
}  // namespace
