/*
** Copyright (c) 1999, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using Ca.Ingres.Utility;

namespace Ca.Ingres.ProviderInternals
{
	/*
	** Name: msgout.cs
	**
	**	Defines class providing capability to send messages to 
	**	a Data Access Server.
	**
	**	The Data Access Messaging (DAM) protocol classes present 
	**	a single unified interface, through inheritance, for access 
	**	to a Data Access Server.  They are divided into separate 
	**	classes to group related functionality for easier management.  
	**	The order of inheritance is determined by the initial actions
	**	required during initialization of the connection:
	**
	**	    MsgIo	Establish socket connection.
	**	    MsgOut	Send TL Connection Request packet.
	**	    MsgIn	Receive TL Connection Confirm packet.
	**	    MsgConn	General initialization.
	**
	**  Classes:
	**
	**	MsgOut
	**	ByteSegOS
	**
	** History:
	**	 7-Jun-99 (gordy)
	**	    Created.
	**	10-Sep-99 (gordy)
	**	    Parameterized the initial TL connection data.
	**	13-Sep-99 (gordy)
	**	    Implemented error code support.
	**	29-Sep-99 (gordy)
	**	    Added Segmentor class to process BLOBs as streams.  
	**	    Added methods (write(InputStream) and write(Reader)) 
	**	    to process BLOBs.  Moved check() functionality to begin().
	**	17-Nov-99 (gordy)
	**	    Extracted output functionality from DbConn.cs.
	**	21-Apr-00 (gordy)
	**	    Moved to package io.
	**	12-Apr-01 (gordy)
	**	    Pass tracing to exception.
	**	10-May-01 (gordy)
	**	    Added support for UCS2 datatypes.  Renamed segmentor class.
	**	31-May-01 (gordy)
	**	    Fixed UCS2 BLOB support.
	**	14-Aug-02 (thoda04)
	**	    Ported for .NET Provider.
	**	 6-Sep-02 (gordy)
	**	    Character encoding now encapsulated in CharSet class.
	**	20-Dec-02 (gordy)
	**	    Header ID protocol level dependent.  Communicate TL protocol
	**	    level to rest of messaging system.
	**	15-Jan-03 (gordy)
	**	    Added MSG layer ID connection parameter.
	**	25-Feb-03 (gordy)
	**	    CharSet now supports character arrays.
	**	15-Mar-04 (gordy)
	**	    Support SQL BIGINT and .NET long values.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	*/
	
	
	/*
	** Name: MsgOut
	**
	** Description:
	**	Class providing methods for sending messages to a Data
	**	Access Server.  Provides methods for building messages
	**	of the Message Layer of the Data Access Messaging protocol 
	**	(DAM-ML).  Drives the Transport Layer (DAM-TL) using the 
	**	buffered output class OutBuff to send TL  packets to the 
	**	server.
	**
	**	When sending messages, the caller must provide multi-
	**	threaded protection for each message.
	**
	**  Public Methods:
	**
	**	begin            	Start a new output message.
	**	getByteLength    	Returns length of encoded string.
	**	write            	Append data to output message.
	**	getUCS2ByteLength	Returns length of UCS2 encoded string.
	**	writeUCS2        	Append char data as UCS2 to output message.
	**	done             	Finish the current output message.
	**
	**  Protected Methods:
	**
	**	disconnect	Disconnect from the server.
	**	close		Close the server connection.
	**	setBuffSize	Set the I/O buffer size.
	**
	**  Private Data:
	**
	**	out		Output buffer.
	**	segOS		BLOB output.
	**	out_msg_id	Current output message ID.
	**	out_hdr_pos	Message header buffer position.
	**	out_hdr_len	Size of message header.
	**	char_buff	Character buffer.
	**
	**  Private Methods:
	**
	**	split		Send current output message and continue message.
	**
	** History:
	**	 7-Jun-99 (gordy)
	**	    Created.
	**	10-Sep-99 (gordy)
	**	    Parameterized the initial TL connection data.
	**	29-Sep-99 (gordy)
	**	    Added methods (write(InputStream) and write(Reader)) 
	**	    to process BLOBs.  Moved check() functionality to begin().
	**	17-Nov-99 (gordy)
	**	    Extracted output functionality from DbConn.
	**	10-May-01 (gordy)
	**	    Added write() methods for byte and character arrays and
	**	    writeUCS2() methods for UCS2 data.  Added character buffers,
	**	    ca1 and char_buff, for write() methods.
	**	 6-Sep-02 (gordy)
	**	    Character encoding now encapsulated in CharSet class.  Added
	**	    methods to return the encoded length of strings.
	**	 6-Sep-02 (gordy)
	**	    Character encoding now encapsulated in CharSet class.  Added
	**	    methods to return the encoded length of strings.
	**	 1-Oct-02 (gordy)
	**	    Renamed as DAM-ML messaging class.  Moved tl_proto and 
	**	    char_set here from super-class.
	**	20-Dec-02 (gordy)
	**	    Communicate TL protocol level to rest of messaging system
	**	    by adding setIoProtoLvl().  Moved tl_proto and char_set back
	**	    to super-class to localize all internal data.
	**	15-Mar-04 (gordy)
	**	    Added write methods for long and SqlBigInt values.
	*/
	
	internal class MsgOut : MsgIo
	{
		/*
		** Output buffer and BLOB stream.
		*/
		private OutBuff outBuff;  // = null;
		private ByteSegOS segOS;  // = null;

		/*
		** Current message info
		*/
		private byte   out_msg_id;  // = 0;
		private int    out_hdr_pos; // = 0;
		private int    out_hdr_len; // = 0;
		private char[] char_buff = new char[8192];
			

		
		
		/*
		** Name: MsgOut
		**
		** Description:
		**	Class constructor.  Initializes the output buffer.  
		**	Sends the DAM-TL Connection Request packet including
		**	the DAM-ML connection parameters.
		**
		** Input:
		**	target  	Hostname and port.
		**	msg_cp    	DAM-ML connection parameters., may be NULL.
		**
		** Output:
		**	None.
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	10-Sep-99 (gordy)
		**	    Parameterized the initial TL connection data.
		**	17-Nov-99 (gordy)
		**	    Extracted output functionality from DbConn.
		**	 1-Oct-02 (gordy)
		**	    Renamed as DAM Message Layer.  Removed connection info 
		**	    length parameter (use array length).  Added trace log 
		**	    parameter.
		**	15-Jan-03 (gordy)
		**	    Added MSG layer ID connection parameter.
		*/
		
		/// <summary>
		/// Sends the DAM-TL Connection Request packet including
		/// the DAM-ML connection parameters.
		/// </summary>
		/// <param name="target">Hostname, semicolon, and port.</param>
		/// <param name="msg_cp">DAM-ML connection parameters.</param>
		protected internal MsgOut(String target, byte[] msg_cp) :
		                       base(target)
		{
			try
			{
				outBuff = new OutBuff(socket.GetStream(),
				                       ConnID, 1 << DAM_TL_PKT_MIN);
			}
			catch (System.Exception ex)
			{
				if (trace.enabled(1))
					trace.write(title + ": error creating output buffer: " + ex.Message);
				disconnect();
				throw SqlEx.get( ERR_GC4001_CONNECT_ERR, ex );
			}
			
			try
			{
				if (trace.enabled(2))
					trace.write(title + ": sending Connection Request");
				
				/*
				** Send the Connection Request message with our desired
				** protocol level and buffer size.
				*/
				outBuff.begin(DAM_TL_CR, 12);
				outBuff.write(DAM_TL_CP_PROTO);
				outBuff.write((byte) 1);
				outBuff.write(DAM_TL_PROTO_DFLT);
				outBuff.write(DAM_TL_CP_SIZE);
				outBuff.write((byte) 1);
				outBuff.write(DAM_TL_PKT_MAX);
				outBuff.write(DAM_TL_CP_MSG_ID);
				outBuff.write((byte) 4);
				outBuff.write(DAM_TL_MSG_ID);  // "DMML"

				if (msg_cp != null && msg_cp.Length > 0)
				{
					outBuff.write(DAM_TL_CP_MSG);
					
					if (msg_cp.Length < 255)
						outBuff.write((byte) msg_cp.Length);
					else
					{
						outBuff.write((byte) 255);
						outBuff.write((short) msg_cp.Length);
					}
					
					outBuff.write(msg_cp, 0, msg_cp.Length);
				}
				
				outBuff.flush();
			}
			catch (SqlEx)
			{
				if (trace.enabled(1))
					trace.write(title + ": error formatting/sending parameters");
				disconnect();
				throw;
			}
		}  // MsgOut
		
		
		/*
		** Name: disconnect
		**
		** Description:
		**	Disconnect from server and free all I/O resources.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	17-Nov-99 (gordy)
		**	    Extracted output functionality from DbConn.
		*/

		/// <summary>
		/// Disconnect from server and free all I/O resources.
		/// </summary>
		protected internal override void  disconnect()
		{
			/*
			** We don't set the output buffer references to null
			** here so that we don't have to check it on each
			** use.  I/O buffer functions will continue to work
			** until a request results in a stream I/O request,
			** in which case an exception will be thrown by the
			** I/O buffer.
			**
			** We must, however, test the reference for null
			** since we may be called by the constructor with
			** a null output buffer.
			*/
			if (outBuff != null)
			{
				try
				{
					outBuff.close();
				}
				catch (Exception /* ignore */)
				{
				}
			}
			
			base.disconnect();
			return ;
		}  // disconnect


		/*
		** Name: close
		**
		** Description:
		**	Close the connection with the server.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	17-Nov-99 (gordy)
		**	    Extracted output functionality from DbConn.
		*/

		/// <summary>
		/// Close the connection with the server.
		/// </summary>
		protected internal virtual void  close()
		{
			try
			{
				outBuff.clear(); // Clear partial messages from buffer.
				outBuff.begin(DAM_TL_DR, 0);
				outBuff.flush();
				if (trace.enabled(2))
					trace.write(title + ": Disconnect Request sent");
			}
			catch (Exception /* ignore */)
			{
			}
			
			return ;
		}  // close
		
		
		/*
		** Name: BuffSize property
		**
		** Description:
		**	Set the I/O buffer size.
		**
		** Input:
		**	size	    Size of I/O buffer.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	17-Nov-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Set the I/O buffer size.
		/// </summary>
		protected internal virtual int BuffSize
		{
			set { outBuff.BuffSize = value; }
		} // BuffSize


		/*
		** Name: TL_ProtocolLevel property
		**
		** Description:
		**	Get/Set the DAM-TL protocol level.
		**
		** History:
		**	20-Dec-02 (gordy)
		**	    Created; Ported to .NET as a property (thoda04).
		*/

		/// <summary>
		/// Set the DAM-TL protocol level into the output buffer.
		/// </summary>
		public new byte TL_ProtocolLevel
		{
			get { return base.TL_ProtocolLevel; }
			set
			{
				base.TL_ProtocolLevel    = value;
				outBuff.TL_ProtocolLevel = value;
			}
		} // TL_ProtocolLevel


		/*
		** Name: begin
		**
		** Description:
		**	Begin a new message.  Builds a message header for
		**	an initially empty message.  The caller should lock
		**	this MsgConn object from the time this method is
		**	called until the request is complete.
		**
		** Input:
		**	msg_id	    Message ID.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	29-Sep-99 (gordy)
		**	    Replaced check() with inline test.
		**	20-Dec-02 (gordy)
		**	    Header ID protocol level dependent.
		*/

		/// <summary>
		/// Begin a new message.  Builds a message header for
		/// an initially empty message.  The caller should lock
		/// this MsgConn object from the time this method is
		/// called until the request is complete.
		/// </summary>
		/// <param name="msg_id">Message ID.</param>
		public virtual void  begin(byte msg_id)
		{
			if (isClosed())
				throw SqlEx.get( ERR_GC4004_CONNECTION_CLOSED );
			if (out_hdr_pos > 0)
				done(false);
			/* Complete current message */
			
			if (trace.enabled(3))
				trace.write(title + ": beginning message " +
				            IdMap.map(msg_id, MsgConst.msgMap));
			
			/*
			** Reserve space in output buffer for header.
			** Note that we don't do message concatenation
			** at this level.  We rely on messages being
			** concatenated by the output buffer if not
			** explicitly flushed.
			*/
			try
			{
				outBuff.begin(DAM_TL_DT, 8); /* Begin new message */
				out_msg_id = msg_id;
				out_hdr_pos = outBuff.position();
				
				//outBuff.write(MSG_ID); /* Eyecatcher */
				outBuff.write( (ML_ProtocolLevel >= MSG_PROTO_3)
					? MSG_HDR_ID_2    // "DMML"  Eyecatcher
					: MSG_HDR_ID_1 ); // "JDBC"
				outBuff.write((short) 0); /* Data length */
				outBuff.write(msg_id); /* Message ID */
				outBuff.write(MSG_HDR_EOD); /* Flags */
				
				out_hdr_len = outBuff.position() - out_hdr_pos;
			}
			catch (SqlEx ex)
			{
				disconnect();
				throw ex;
			}
			
			return ;
		}  // begin
		
		/*
		** Name: write (byte)
		**
		** Description:
		**	Append a byte value to current output message.  If buffer
		**	overflow would occur, the message is split.
		**
		** Input:
		**	valueByte	    The byte value to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a byte value to current output message.  If buffer
		/// overflow would occur, the message is split.
		/// </summary>
		/// <param name="valueByte"></param>
		public virtual void  write(byte valueByte)
		{
			if (outBuff.avail() < 1)
				split();
			outBuff.write(valueByte);
			return ;
		}  // write
		
		/*
		** Name: write (short)
		**
		** Description:
		**	Append a short value to current output message.  If buffer
		**	overflow would occur, the message is split.
		**
		** Input:
		**	valueShort	    The short value to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a short value to current output message.  If buffer
		/// overflow would occur, the message is split.
		/// </summary>
		/// <param name="valueShort">The short value to be appended.</param>
		public virtual void  write(short valueShort)
		{
			if (outBuff.avail() < 2)
				split();
			outBuff.write(valueShort);
			return ;
		}  // write
		

		/*
		** Name: write (int)
		**
		** Description:
		**	Append an integer value to current output message.  If buffer
		**	overflow would occur, the message is split.
		**
		** Input:
		**	valueInt	    The integer value to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append an integer value to current output message.  If buffer
		/// overflow would occur, the message is split.
		/// </summary>
		/// <param name="valueInt">The integer value to be appended.</param>
		public virtual void  write(int valueInt)
		{
			if (outBuff.avail() < 4)
				split();
			outBuff.write(valueInt);
			return ;
		}  // write


		/*
		** Name: write
		**
		** Description:
		**	Append a long value to current output message.  If buffer
		**	overflow would occur, the message is split.
		**
		** Input:
		**	value	    The long value to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	15-Mar-04 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a long 64-bit value to current output message.  If buffer
		/// overflow would occur, the message is split.
		/// </summary>
		/// <param name="value"></param>
		public void
			write( long value )
		{
			if ( outBuff.avail() < 8 )  split();
			outBuff.write( value );
			return;
		} // write


		/*
		** Name: write (float)
		**
		** Description:
		**	Append a float value to current output message.  If buffer
		**	overflow would occur, the message is split.
		**
		** Input:
		**	valueFloat	    The float value to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a float value to current output message.  If buffer
		/// overflow would occur, the message is split.
		/// </summary>
		/// <param name="valueFloat">The float value to be appended.</param>
		public virtual void  write(float valueFloat)
		{
			if (outBuff.avail() < DAM_TL_F4_LEN)
				split();
			outBuff.write(valueFloat);
			return ;
		}  // write


		/*
		** Name: write (double)
		**
		** Description:
		**	Append a double value to current output message.  If buffer
		**	overflow would occur, the message is split.
		**
		** Input:
		**	valueDouble	    The double value to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a double value to current output message.  If buffer
		/// overflow would occur, the message is split.
		/// </summary>
		/// <param name="valueDouble">The double value to be appended.</param>
		public virtual void  write(double valueDouble)
		{
			if (outBuff.avail() < DAM_TL_F8_LEN)
				split();
			outBuff.write(valueDouble);
			return ;
		}
		// write


		/*
		** Name: write (byte[])
		**
		** Description:
		**	Append a byte array to current output message.  If buffer
		**	overflow would occur, the message (and array) is split.
		**	The byte array is preceded by a two byte length indicator.
		**
		** Input:
		**	byteArray	    The byte array to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	22-Sep-99 (gordy)
		**	    Send entire array, preceded by the length.
		*/

		/// <summary>
		/// Append a byte array to current output message.  If buffer
		/// overflow would occur, the message (and array) is split.
		/// </summary>
		/// <param name="byteArray">The byte array to be appended.</param>
		public virtual void  write(byte[] byteArray)
		{
			write((short) byteArray.Length);
			write(byteArray, 0, byteArray.Length);
			return ;
		}  // write


		/*
		** Name: write (byte[])
		**
		** Description:
		**	Append a byte sub-array to current output message.  If buffer
		**	overflow would occur, the message (and array) is split.
		**
		** Input:
		**	byteArray	    The byte array containing sub-array to be appended.
		**	offset   	    Start of sub-array.
		**	length   	    Length of sub-array.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	10-May-01 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a byte sub-array to current output message.  If buffer
		/// overflow would occur, the message (and array) is split.
		/// </summary>
		/// <param name="byteArray">The byte array containing sub-array
		/// to be appended.</param>
		/// <param name="offset">Start of sub-array.</param>
		/// <param name="length">Length of sub-array.</param>
		public virtual void  write(byte[] byteArray, int offset, int length)
		{
			 for (int end = offset + length; offset < end; )
			{
				if (outBuff.avail() <= 0)
					split();
				length = System.Math.Min(outBuff.avail(), end - offset);
				outBuff.write(byteArray, offset, length);
				offset += length;
			}
			
			return ;
		}  // write


		/*
		** Name: getByteLength
		**
		** Description:
		**	Returns the length in bytes of the provided string 
		**	when formatted for transmission to the Server.
		**
		** Input:
		**	str	String
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Formatted length in bytes.
		**
		** History:
		**	 6-Sep-02 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Returns the length in bytes of the provided string
		/// when formatted for transmission to the Server.
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		public int
			getByteLength( String str )
		{
			return( CharSet.getByteLength( str ) );
		} // getByteLength


		/*
		** Name: write (string)
		**
		** Description:
		**	Append a string value to current output message.  If buffer
		**	overflow would occur, the message (and string) is split.
		**	The string is preceded by a two byte length indicator.
		**
		** Input:
		**	valueString	    The string value to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	22-Sep-99 (gordy)
		**	    Added character set/encoding.
		**	 6-Sep-02 (gordy)
		**	    Character encoding now encapsulated in CharSet class.
		*/

		/// <summary>
		/// Append a string value to current output message.  If buffer
		/// overflow would occur, the message (and string) is split.
		/// </summary>
		/// <param name="valueString"></param>
		public virtual void  write(String valueString)
		{
			byte[] ba;
			
			try { ba = CharSet.getBytes( valueString ); }
			catch( Exception ex )  // Should not happen!
			{
				throw SqlEx.get(
					title + ": character encoding failed.  " + ex.Message,
					"HY000", 0);
			}

			write( ba );
			return;
		}  // write


		/*
		** Name: getByteLength
		**
		** Description:
		**	Returns the length in bytes of the provided character
		**	array when formatted for transmission to the Server.
		**
		** Input:
		**	charArray	Character array
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Formatted length in bytes.
		**
		** History:
		**	 6-Sep-02 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Returns the length in bytes of the provided character
		/// array when formatted for transmission to the Server.
		/// </summary>
		/// <param name="charArray"></param>
		/// <returns>Length in bytes.</returns>
		public int getByteLength( char[] charArray )
		{
			String  str = new String( charArray );
			return( CharSet.getByteLength( str ) );
		} // getByteLength


		/*
		** Name: write (char[])
		**
		** Description:
		**	Append a character array to current output message.  If 
		**	buffer overflow would occur, the message (and array) is 
		**	split.  The character array is preceded by a two byte 
		**	length indicator.
		**
		** Input:
		**	charArray	    The character array to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	10-May-01 (gordy)
		**	    Created.
		**	 6-Sep-02 (gordy)
		**	    Character encoding now encapsulated in CharSet class.
		*/

		/// <summary>
		/// Append a character array to current output message.  If
		/// buffer overflow would occur, the message (and array) is split.
		/// </summary>
		/// <param name="charArray"></param>
		public virtual void  write(char[] charArray)
		{
			byte[]    ba;

			try { ba = CharSet.getBytes( charArray ); }
			catch( Exception ex )  // Should not happen!
			{
				throw SqlEx.get(
					title + ": character encoding failed.  " + ex.Message,
					"HY000", 0);
			}

			write( ba );
			return;
		}  // write 


		/*
		** Name: write (char[], int, int)
		**
		** Description:
		**	Append a character sub-array to current output message.  
		**	If buffer overflow would occur, the message (and array) 
		**	is split.
		**
		** Input:
		**	charArray	    The character array with sub-array to be appended.
		**	offset   	    Start of sub-array.
		**	length   	    Length of sub-array.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	10-May-01 (gordy)
		**	    Created.
		**	 6-Sep-02 (gordy)
		**	    Character encoding now encapsulated in CharSet class.
		**	25-Feb-03 (gordy)
		**	    CharSet now supports character arrays.
		*/

		/// <summary>
		/// Append a character sub-array to current output message. 
		/// If buffer overflow would occur, the message (and array) is split.
		/// </summary>
		/// <param name="charArray">The character array with sub-array
		/// to be appended.</param>
		/// <param name="offset">Start of sub-array.</param>
		/// <param name="length">Length of sub-array.</param>
		public virtual void  write(char[] charArray, int offset, int length)
		{
			byte[] ba;
			
			try { ba = CharSet.getBytes( charArray, offset, length ); }
			catch( Exception ex )  // Should not happen!
			{
				throw SqlEx.get(
					title + ": character encoding failed.  " + ex.Message,
					"HY000", 0);
			}
			
			write(ba, 0, ba.Length);
			return ;
		}  // write


		/*
		** Name: getUCS2ByteLength
		**
		** Description:
		**	Returns the length in bytes of the provided string
		**	when formatted as UCS2 for transmission to the Server.
		**
		** Input:
		**	str	String
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Formatted length in bytes.
		**
		** History:
		**	 6-Sep-02 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Returns the length in bytes of the provided string
		/// when formatted as UCS2 for transmission to the Server.
		/// </summary>
		/// <param name="str"></param>
		/// <returns>Formatted length in bytes.</returns>
		public int getUCS2ByteLength( String str )
		{
			return( str.Length * 2 );
		} // getUCS2ByteLength


		/*
		** Name: writeUCS2 (string)
		**
		** Description:
		**	Append a string value to current output message.  If buffer
		**	overflow would occur, the message (and string) is split.
		**	The UCS2 array is preceded by a two byte length indicator.
		**
		** Input:
		**	valueString	    The string value to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 10-May-01 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a string value to current output message.  If buffer
		/// overflow would occur, the message (and string) is split.
		/// </summary>
		/// <param name="valueString"></param>
		public virtual void  writeUCS2(String valueString)
		{
			int length = valueString.Length;
			write((short) length);
			
			 for (int offset = 0; offset < length; offset++)
				write((short) valueString[offset]);
			return ;
		}  // writeUCS2


		/*
		** Name: getUCS2ByteLength
		**
		** Description:
		**	Returns the length in bytes of the provided character array
		**	when formatted as UCS2 for transmission to the Server.
		**
		** Input:
		**	charArray	Character array.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Formatted length in bytes.
		**
		** History:
		**	 6-Sep-02 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Returns the length in bytes of the provided character array
		/// when formatted as UCS2 for transmission to the Server.
		/// </summary>
		/// <param name="charArray"></param>
		/// <returns>Formatted length in bytes.</returns>
		public int getUCS2ByteLength( char[] charArray )
		{
			return( charArray.Length * 2 );
		} // getUCS2ByteLength


		/*
		** Name: writeUCS2 (char[])
		**
		** Description:
		**	Append a character array to current output message.  If 
		**	buffer overflow would occur, the message (and array) is 
		**	split.  The UCS2 array is preceded by a two byte length 
		**	indicator.
		**
		** Input:
		**	charArray	    The character array to be appended.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	10-May-01 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a character array to current output message.  If
		/// buffer overflow would occur, the message (and array) is split.
		/// </summary>
		/// <param name="charArray"></param>
		public virtual void  writeUCS2(char[] charArray)
		{
			write((short) charArray.Length);
			write(charArray, 0, charArray.Length);
			return ;
		}  // writeUCS2


		/*
		** Name: writeUCS2 (char[])
		**
		** Description:
		**	Append a character sub-array to current output message.  
		**	If buffer overflow would occur, the message (and array) 
		**	is split.
		**
		** Input:
		**	charArray	    The character array with sub-array to be appended.
		**	offset   	    Start of sub-array.
		**	length   	    Length of sub-array.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	10-May-01 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Append a character sub-array to current output message. 
		/// If buffer overflow would occur, the message (and array) is split.
		/// </summary>
		/// <param name="charArray">The character array with sub-array
		/// to be appended.</param>
		/// <param name="offset">Start of sub-array.</param>
		/// <param name="length">Length of sub-array.</param>
		public virtual void  writeUCS2(char[] charArray, int offset, int length)
		{
			 for (int end = offset + length; offset < end; offset++)
				write((short) charArray[offset]);
			return ;
		}  // writeUCS2


		/*
		** Name: writeUCS2 (StreamReader)
		**
		** Description:
		**	Send an input Unicode stream to the server as a series of
		**	segments.  Segments are sized to fit the amount of space
		**	remaining in the buffer.  Additional segments will end
		**	the current message and begin a new message with the
		**	same message ID.  The last segment is left in the buffer
		**	so the caller may append additional data.
		**
		**	A ByteSegOS (special local class) is used to write the
		**	segment format of the bufferred output stream.
		**
		** Input:
		**	in	    Input Unicode stream.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	10-May-01 (gordy)
		**	    Created.
		**	31-May-01 (gordy)
		**	    Close request moved from finally() block to exception
		**	    block so input stream won't be closed on first read().
		**	    Increased buffering capacity.*/

		/// <summary>
		/// Send an input Unicode stream to the server as a series of
		/// segments.  Segments are sized to fit the amount of space
		/// remaining in the buffer.  Additional segments will end
		/// the current message and begin a new message with the
		/// same message ID.  The last segment is left in the buffer
		/// so the caller may append additional data.
		/// </summary>
		/// <param name="inputStreamRdr">Input Unicode stream.</param>
		public virtual void  writeUCS2(System.IO.TextReader inputStreamRdr)
		{
			byte msg_id = out_msg_id;
			int len_pos, length;
			short seg_len;
			
			if (trace.enabled(3))
				trace.write(title + ": starting CLOB");
			
			do 
			{
				/*
				** A CLOB segment requires a minimum of four bytes:
				** two for the length indicator and two bytes for
				** one character of data.
				*/
				if (outBuff.avail() < 4)
				{
					done(false); // Finish current message
					begin(msg_id); // Begin new message of same type.
				}
				
				/*
				** Write an initial length indicator of 0 (since
				** the segment has not been written yet) and save
				** the length indicator position for later updating.
				*/
				len_pos = outBuff.position();
				seg_len = 0;
				outBuff.write(seg_len);
				
				/*
				** Now try to write a segment which fills the current
				** message buffer.  Update length indicator with the
				** actual segment length written.
				*/
				while ((length = outBuff.avail()) >= 2)
				{
					length = System.Math.Min(length / 2, char_buff.Length);
					
					try
					{
						length = inputStreamRdr.Read(char_buff, 0, length);
					}
					catch (System.Exception ex)
					{
						try
						{
							inputStreamRdr.Close();
						}
						catch (System.Exception /* ignore */)
						{
						}
						throw SqlEx.get( ERR_GC4007_BLOB_IO, ex );
					}
					
					if (length < 1)
						break;
					seg_len = (short) (seg_len + (short) length);
					
					 for (int i = 0; i < length; i++)
						outBuff.write((short) char_buff[i]);
				}
				
				if (seg_len > 0)
				{
					outBuff.write(len_pos, seg_len);
					if (trace.enabled(4))
						trace.write(title + ": wrote " +
						            seg_len + " of " + seg_len);
				}
			}
			while (seg_len > 0);
			
			if (trace.enabled(3))
				trace.write(title + ": closing CLOB");
			try
			{
				inputStreamRdr.Close();
			}
			catch (System.Exception /* ignore */)
			{
			}
			return ;
		}  // writeUCS2


		/*
		** Name: write
		**
		** Description:
		**	Send an input Unicode stream to the server as a series of
		**	segments.  Segments are sized to fit the amount of space
		**	remaining in the buffer.  Additional segments will end
		**	the current message and begin a new message with the
		**	same message ID.  The last segment is left in the buffer
		**	so the caller may append additional data.
		**
		**	An OutputStreamWriter provides the conversion from Unicode
		**	to the server character set and a ByteSegOS (special local
		**	class) converts between the OutputStream format used by
		**	OuputStreamWriter and the segment format of the bufferred
		**	output stream.
		**
		** Input:
		**	in	    Input Unicode stream.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	29-Sep-99 (gordy)
		**	    Created.
		**	 6-Sep-02 (gordy)
		**	    Character encoding now encapsulated in CharSet class.
		*/

		/// <summary>
		/// An OutputStreamWriter provides the conversion from Unicode
		/// to the server character set and a ByteSegOS (special local
		/// class) converts between the OutputStream format used by
		/// OuputStreamWriter and the segment format of the bufferred
		/// output stream.
		/// </summary>
		/// <param name="inputStreamRdr">Input Unicode stream.</param>
		public virtual void  write(System.IO.TextReader inputStreamRdr)
		{
			System.IO.StreamWriter os;
			
			if (segOS == null)
				segOS = new ByteSegOS(this, outBuff, trace);
			segOS.begin(out_msg_id); // begin a new BLOB.
			
			try { os = CharSet.getOSW( segOS ); }
			catch( Exception ex )  // Should not happen!
			{
				throw SqlEx.get(
					title + ": character encoding failed.  " + ex.Message,
					"HY000", 0);
			}

			try
			{
				int length;
				
				while ((length = inputStreamRdr.Read(char_buff, 0, char_buff.Length))
				           >  0)
				{
					os.Write(char_buff, 0, length);
				}
				
				os.Flush();
			}
			catch (System.Exception ex)
			{
				throw SqlEx.get( ERR_GC4007_BLOB_IO, ex);
			}
			finally
			{
				try { inputStreamRdr.Close(); } catch (Exception /* ign */)  { }
				try { os.Close();             } catch (Exception /* ign */)  { }
				segOS.end(); // append the end-of-segments indicator.
			}
			
			return ;
		}  // write


		/*
		** Name: write
		**
		** Description:
		**	Send an input byte stream to the server as a series of
		**	segments.  Segments are sized to fit the amount of space
		**	remaining in the buffer.  Additional segments will end
		**	the current message and begin a new message with the
		**	same message ID.  The last segment is left in the buffer
		**	so the caller may append additional data.
		**
		** Input:
		**	in	    Input byte stream.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	29-Sep-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Send an input byte stream to the server as a series of
		/// segments.  Segments are sized to fit the amount of space
		/// remaining in the buffer.  Additional segments will end
		/// the current message and begin a new message with the
		/// same message ID.  The last segment is left in the buffer
		/// so the caller may append additional data.
		/// </summary>
		/// <param name="inputStream">Input byte stream.</param>
		public virtual void  write(System.IO.Stream inputStream)
		{
			byte msg_id = out_msg_id;
			short seg_len;
			int len_pos;
			
			if (trace.enabled(3))
				trace.write(title + ": starting BLOB");
			
			do 
			{
				/*
				** A BLOB segment requires a minimum of three bytes:
				** two for the length indicator and one byte of data.
				*/
				if (outBuff.avail() < 3)
				{
					done(false); // Finish current message
					begin(msg_id); // Begin new message of same type.
				}
				
				/*
				** Write an initial length indicator of 0 (since
				** the segment has not been written yet) and save
				** the length indicator position for later updating.
				*/
				len_pos = outBuff.position();
				outBuff.write((short) 0);
				
				/*
				** Now try to write a segment which fills the current
				** message buffer.  Update length indicator with the
				** actual segment length written.
				*/
				if ((seg_len = outBuff.write(inputStream)) > 0)
				{
					outBuff.write(len_pos, seg_len);
					if (trace.enabled(4))
						trace.write(title + ": wrote " + seg_len + " of " + seg_len);
				}
			}
			while (seg_len > 0);
			
			if (trace.enabled(3))
				trace.write(title + ": closing BLOB");
			try
			{
				inputStream.Close();
			}
			catch (System.Exception /* ignore */)
			{
			}
			return ;
		}  // write


		/*
		** Name: done
		**
		** Description:
		**	Finish the current message and optionally send
		**	it to the server.
		**
		** Input:
		**	flush	    True if message should be sent to server.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// Finish the current message and optionally send
		/// it to the server.
		/// </summary>
		/// <param name="flush">True if message should be sent to server.</param>
		public virtual void  done(bool flush)
		{
			if (out_hdr_pos > 0)
			{
				// Update the message header.
				short length = (short) (outBuff.position() -
				                        (out_hdr_pos + out_hdr_len));
				
				if (trace.enabled(2))
					trace.write(title + ": sending message " +
					            IdMap.map(out_msg_id, MsgConst.msgMap) +
					            " length " + length +
					            " EOD" + (flush?" EOG":""));
				
				outBuff.write(out_hdr_pos + 4, length);
				outBuff.write(out_hdr_pos + 7,
				             (byte) (MSG_HDR_EOD | 
				                     (flush?MSG_HDR_EOG:(byte)0)));
			}
			
			if (flush)
				try
				{
					outBuff.flush();
				}
				catch (SqlEx ex)
				{
					disconnect();
					throw ex;
				}
			
			out_msg_id = 255;
			out_hdr_pos = 0;
			out_hdr_len = 0;
			
			return ;
		}  // done


		/*
		** Name: split
		**
		** Description:
		**	The current output message is sent to the server and 
		**	a message continuation is initialized.
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
		**	 7-Jun-99 (gordy)
		**	    Created.
		*/

		/// <summary>
		/// The current output message is sent to the server and
		/// a message continuation is initialized.
		/// </summary>
		private void  split()
		{
			if (out_hdr_pos > 0)
			{
				short length =
					(short) (outBuff.position() - (out_hdr_pos + out_hdr_len));
				
				if (trace.enabled(3))
					trace.write(title + ": splitting message " +
						IdMap.map(out_msg_id, MsgConst.msgMap) +
						" length " + length);
				
				// Update the message header.
				try
				{
					outBuff.write(out_hdr_pos + 4, length);
					outBuff.write(out_hdr_pos + 7, (byte) 0);
					outBuff.flush();
					
					outBuff.begin(DAM_TL_DT, 8); /* Begin continued message */
					out_hdr_pos = outBuff.position();
					outBuff.write( (ML_ProtocolLevel >= MSG_PROTO_3)
						? MSG_HDR_ID_2    // "DMML"  Eyecatcher
						: MSG_HDR_ID_1 ); // "JDBC"
					outBuff.write((short) 0); /* Data length */
					outBuff.write(out_msg_id); /* Message ID */
					outBuff.write(MSG_HDR_EOD); /* Flags */
					out_hdr_len = outBuff.position() - out_hdr_pos;
				}
				catch (SqlEx ex)
				{
					disconnect();
					throw ex;
				}
			}
			
			return ;
		}  // split


		/*
		** Name: ByteSegOS
		**
		** Description:
		**	Class for converting byte stream into segmented output stream.
		**
		**	An OutputStreamWriter object is required for conversion of 
		**	Unicode BLOBs into server character encoded byte streams.  
		**	OutputStreamWriter requires an OutputStream to process the
		**	resulting byte stream.  This class extends OutputStream to
		**	provide segmentation of the converted BLOB, bufferring the
		**	stream as needed.
		**
		**	This class is designed for repeated usage.  The begin() and
		**	end() methods provide the means for a single class instance
		**	to process multiple serial BLOBs on the same connection.
		**
		**	This class is not static because it needs access to MsgOut
		**	begin(), done() and disconnect() methods.
		**
		**  Public Methods
		**
		**	begin   	Initialize processing of a BLOB.
		**	write   	Write bytes in BLOB stream.
		**	end     	Complete processing of a BLOB.
		**
		**  Private Data
		**
		**	title   	Class title for tracing.
		**	trace   	Tracing.
		**	dbc_out 	Db Connection output.
		**	out     	Bufferred output stream.
		**	msg_id  	Message ID of message containing the BLOB.
		**	li_pos  	Position of segment length indicator in output buffer.
		**	seg_len 	Length of the current segment.
		**	ba      	Temporary storage for writing single byte.
		**
		** History:
		**	29-Sep-99 (gordy)
		**	    Created.
		**	17-Nov-99 (gordy)
		**	    Made class nested (inner).
		**	10-May-01 (gordy)
		**	    Class renamed to match changes in DbConnIn.  Made class 
		**	    static and added dbc_out for access to DB connection.
		*/

		/// <summary>
		/// Class for converting byte stream into segmented output stream.
		/// An OutputStreamWriter object is required for conversion of
		/// Unicode BLOBs into server character encoded byte streams. 
		/// OutputStreamWriter requires an IO.Stream to process the
		/// resulting byte stream.  This class extends IO.Stream to
		/// provide segmentation of the converted BLOB, bufferring the
		/// stream as needed.
		/// </summary>

		private class ByteSegOS : System.IO.Stream
		{
			private String title; // Class title for tracing.
			private ITrace trace;        // Tracing.
			
			private MsgOut msg_out;      // Db Connection output.
			private OutBuff outBuff = null; // Buffered output stream.
			private byte msg_id = 255;      // Current message ID.
			
			private int li_pos    = -1; // Position of length indicator.
			private short seg_len =  0; // Current segment length.
			private byte[] ba;          // Temp storage.
			
			
			/*
			** Name: ByteSegOS
			**
			** Description:
			**	Class constructor.
			**
			** Input:
			**	msg_out	Message layer output.
			**	outBuff	Buffered output stream.
			**	trace  	Tracing.
			**
			** Output:
			**	None.
			**
			** Returns:
			**	None.
			**
			** History:
			**	29-Sep-99 (gordy)
			**	    Created.
			**	17-Nov-99 (gordy)
			**	    Removed msg parameter when class nested.
			**	10-May-01 (gordy)
			**	    Added msg_out parameter when class made static.*/

			/// <summary>
			/// Class constructor for converting byte stream into
			/// segmented output stream.
			/// </summary>
			/// <param name="msg_out">Message layer output.</param>
			/// <param name="outBuff">Buffered output stream.</param>
			/// <param name="trace">Tracing</param>
			public ByteSegOS(MsgOut msg_out, OutBuff outBuff, ITrace trace)
			{
				title = "ByteSegOS[" + msg_out.ConnID + "]";
				this.trace = trace;
				this.msg_out = msg_out;
				this.outBuff = outBuff;

				ba = new byte[1];
			}  // BytesSegOS


			/*
			** Name: begin segment
			**
			** Description:
			**	Initialize the segmentor for a new series of BLOB segments.
			**	The current message ID must be provided since segmentation 
			**	of the BLOB may require the current message to be completed 
			**	and a new message of the same type to be started.
			**
			** Input:
			**	msg_id	    ID of current message.
			**
			** Output:
			**	None.
			**
			** Returns:
			**	void.
			**
			** History:
			**	29-Sep-99 (gordy)
			**	    Created.
			*/

			/// <summary>
			/// Initialize the segmentor for a new series of BLOB segments.
			/// The current message ID must be provided since segmentation
			/// of the BLOB may require the current message to be completed
			/// and a new message of the same type to be started.
			/// </summary>
			/// <param name="msg_id">ID of current message.</param>
			public virtual void  begin(byte msg_id)
			{
				if (trace.enabled(3))
					trace.write(title + ": start of BLOB");
				this.msg_id = msg_id;
				li_pos = - 1;
				seg_len = 0;
				return ;
			}  // begin


			/*
			** Name: write (int) into segment
			**
			** Description:
			**	Write a single byte to the segment stream.
			**
			** Input:
			**	b	Byte to be output.
			**
			** Output:
			**	None.
			**
			** Returns:
			**	void.
			**
			** History:
			**	29-Sep-99 (gordy)
			**	    Created.
			*/

			/// <summary>
			/// Write a single byte to the segment stream.
			/// </summary>
			/// <param name="b"></param>
			public  void  WriteByte(int b)
			{
				ba[0] = (byte) b;
				Write(ba, 0, 1);
				return ;
			}  // write


			/*
			** Name: write byte[] into segment
			**
			** Description:
			**	Write a byte sub-array to the segment stream.  The remainder
			**	of the current message buffer is used to build a segment.
			**	When the buffer is filled, the current message is completed
			**	and a new message of the same type is started.  A non-zero
			**	length segment is always left in the current output buffer.
			**
			** Input:
			**	b     	Byte array.
			**	offset	Starting byte to be output.
			**	length	Number of bytes to be output.
			**
			** Output:
			**	None.
			**
			** Returns:
			**	void.
			**
			** History:
			**	29-Sep-99 (gordy)
			**	    Created.
			**	12-Apr-01 (gordy)
			**	    Pass tracing to exception.*/

			/// <summary>
			/// Write a byte sub-array to the segment stream.  The remainder
			/// of the current message buffer is used to build a segment.
			/// When the buffer is filled, the current message is completed
			/// and a new message of the same type is started.  A non-zero
			/// length segment is always left in the current output buffer.
			/// </summary>
			/// <param name="b">Byte array.</param>
			/// <param name="offset">Starting byte to be output.</param>
			/// <param name="length">Number of bytes to be output.</param>
			public override void  Write(byte[] b, int offset, int length)
			{
				try
				{
					if (trace.enabled(3))
						trace.write(title + ": writing " + length + " bytes");
					
					while (length > 0)
					{
						int len;
						
						if (outBuff.avail() < 3)
						{
							if (seg_len > 0  &&  trace.enabled(4))
								trace.write(title +
								": BLOB segment length " + seg_len);

							li_pos = - 1;
							msg_out.done(false);   // Finish current message
							msg_out.begin(msg_id); // Begin new message of same type.
						}
						
						if (li_pos < 0)
						{
							li_pos = outBuff.position();
							seg_len = 0;
							outBuff.write((short) 0);
						}
						
						len = System.Math.Min(outBuff.avail(), length);
						outBuff.write(b, offset, len);
						offset += len;
						length -= len;
						seg_len = (short) (seg_len + len);
						outBuff.write(li_pos, seg_len);
					}  // end while
				}
				catch (SqlEx ex)
				{
					if (trace.enabled(1))
					{
						trace.write(title + ": exception writing data");
						ex.trace(trace);
					}
					
					msg_out.disconnect();
					throw; // new IOException(ex.Message);
				}
				
				return ;
			}  // write


			/*
			** Name: close segment
			**
			** Description:
			**	Close the output stream.
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
			**	29-Sep-99 (gordy)
			**	    Created.
			**	12-Apr-01 (gordy)
			**	    Pass tracing to exception.
			*/

			/// <summary>
			/// Close the output stream.
			/// </summary>
			public override void  Close()
			{
				try { end(); }
				catch (SqlEx ex)
				{
					if (trace.enabled(1))
					{
						trace.write(title + ": exception writing end-of-segments");
						ex.trace(trace);
					}
					
					msg_out.disconnect();
					throw; // new IOException(ex.Message);
				}
				
				return ;
			}  // Close


			/*
			** Name: end segment
			**
			** Description:
			**	End the current output stream of segments.  The end-
			**	of-segments indicator is appended to the output buffer.
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
			**	29-Sep-99 (gordy)
			**	    Created.
			*/

			/// <summary>
			/// End the current output stream of segments.  The
			/// end-of-segments indicator is appended to the output buffer.
			/// </summary>
			public virtual void  end()
			{
				if (msg_id != 255)
				{
					
					if (outBuff.avail() < 2)
					{
						if (seg_len > 0  &&  trace.enabled(4))
							trace.write(title +
								": BLOB segment length " + seg_len);

						msg_out.done(false);
						msg_out.begin(msg_id);
					}
					
					msg_id  = 255;
					li_pos  =  -1;
					seg_len =   0;
					outBuff.write((short) 0);

					if (trace.enabled(3))
						trace.write(title + ": end of BLOB");
				}
				
				return ;
			}  // end


			/*
			** Name: miscellaneous properties and methods for ByteSegOS stream
			**
			** Description:
			**	System.IO.Stream abstract class has properties and 
			**	methods that need not be implemented for the ByteSegOS
			**	output stream but do need to be defined.
			**
			** History:
			**	19-Aug-02 (thoda04)
			**	    Created for .NET Provider.
			*/
			
			public override bool CanRead
			{
				get { return false; }
			}  // CanRead

			public override bool CanSeek
			{
				get { return false; }
			}  // CanSeek

			public override bool CanWrite
			{
				get { return true; }
			}  // CanWrite

			public override long Length
			{
				get { throw new NotSupportedException(
				         "ByteSegOS stream is a write-only stream");
				    }
			}  // Length

			public override long Position
			{
				get
				{
					throw new NotSupportedException(
						  "ByteSegOS stream does not support seeking");
				}
				set
				{
					throw new NotSupportedException(
						"ByteSegOS stream does not support seeking");
				}
			}  // Position

			public override void Flush()
			{
			}  // Flush

			public override int Read(byte[] byteArray, int offset, int count)
			{
				throw new NotSupportedException(
					"ByteSegOS stream does not support reading");
			}  // Read

			public override long Seek(long offset, System.IO.SeekOrigin origin)
			{
				throw new NotSupportedException(
					"ByteSegOS stream does not support seek");
			}  // Seek

			public override void SetLength(long length)
			{
				throw new NotSupportedException(
					"ByteSegOS stream does not support seek");
			}  // SetLength

		}  // class ByteSegOS
	}  // class MsgOut
}