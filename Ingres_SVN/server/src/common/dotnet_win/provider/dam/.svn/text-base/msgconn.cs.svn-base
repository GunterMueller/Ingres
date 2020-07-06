/*
** Copyright (c) 1999, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using Ca.Ingres.Utility;

namespace Ca.Ingres.ProviderInternals
{
	/*
	** Name: msgconn.cs
	**
	** Description:
	**	Defines class representing a connection to a Data Access Server.
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
	**	MsgConn		Extends MsgIn.
	**
	** History:
	**	 7-Jun-99 (gordy)
	**	    Created.
	**	10-Sep-99 (gordy)
	**	    Parameterized the initial TL connection data.
	**	13-Sep-99 (gordy)
	**	    Implemented error code support.
	**	22-Sep-99 (gordy)
	**	    Added character set/encoding.
	**	29-Sep-99 (gordy)
	**	    Added methods (lock()/unlock()) and fields to provide
	**	    synchronization.
	**	 2-Nov-99 (gordy)
	**	    Added methods setDbmsInfo() and getDbmsInfo() to DbConn.
	**	 4-Nov-99 (gordy)
	**	    Added ability to cancel a query.
	**	12-Nov-99 (gordy)
	**	    Allow gmt or local timezone to support gateways.
	**	17-Nov-99 (gordy)
	**	    Extracted I/O functionality to DbConnIo.cs, DbConnOut.cs
	**	    and DbConnIn.cs.
	**	 4-Feb-00 (gordy)
	**	    A single thread can dead-lock if it starts another operation
	**	    while it already holds the lock.  Save the lock owner and
	**	    throw an exception in this case.
	**	21-Apr-00 (gordy)
	**	    Moved to package io.
	**	19-May-00 (gordy)
	**	    Added public field for select_loops and make additional
	**	    validation check during unlock().
	**	19-Oct-00 (gordy)
	**	    Added getXactID().
	**	 3-Nov-00 (gordy)
	**	    Removed timezone fields/methods and replaced with public
	**	    boolean field: use_gmt_tz.
	**	20-Jan-01 (gordy)
	**	    Added msg_protocol_level.
	**	 5-Feb-01 (gordy)
	**	    Coalesce statement IDs using a statement ID cache with the
	**	    query text as key which is cleared at transaction boundaries.
	**	10-May-01 (gordy)
	**	    Added support for UCS2 datatypes.
	**	20-Aug-01 (gordy)
	**	    Added support for default cursor mode.
	**	20-Feb-02 (gordy)
	**	    Added fields for DBMS protocol level and Ingres/gateway distinction.
	**	20-Aug-02 (thoda04)
	**	    Ported for .NET Provider.
	**	 6-Sep-02 (gordy)
	**	    Character encoding now encapsulated in CharSet class.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	**	 6-jul-04 (gordy; ported by thoda04)
	**	    Added option key mask to encoding routines.
	*/
	
	
	/*
	** Name: MsgConn
	**
	** Description:
	**	Class representing a Data Access Server connection using
	**	the Data Access Messaging protocol (DAM).  
	**
	**	When reading/writing messages, the caller must provide
	**	multi-threaded protection for each message.  Since the 
	**	DAM communication channel is not multi-session, the 
	**	caller should provide multi-threaded protection from the 
	**	first message sent in a request until the last response 
	**	message has been received.  The methods lock() and unlock()
	**	permit the caller to synchronize multi-threaded access
	**	to the communication channel.
	**
	**  Public Data:
	**
	**	CRSR_DBMS    	    Cursor mode determined by DBMS.
	**	CRSR_READONLY	    Readonly cursor mode.
	**	CRSR_UPDATE  	    Updatable cursor mode.
	**
	**	msg_protocol_level	Negotiated message protocol level.
	**	use_gmt_tz        	Is connection using GMT timezone?
	**	ucs2_supported    	Is UCS2 data supported?
	**	select_loops      	Are select loops enabled?
	**	cursor_mode       	Default cursor mode.
	**
	**  Public Methods:
	**
	**	close      	Close the connection.
	**	cancel     	Cancel current query.
	**	LockConnection  	Lock connection.
	**	UnLockConnection	Unlock connection.
	**	encode     	Encrypt a string using a key.
	**	endXact    	Declare end of transaction.
	**	getUniqueID	Return a new unique identifier.
	**	getStmtID  	Return a statement ID for a query.
	**	setDbmsInfo	Save DBMS information.
	**	getDbmsInfo	Retrieve DBMS information.
	**
	**  Private Data:
	**
	**	cncl       	Cancel output buffer.
	**	dbmsInfo   	DBMS information keys and values.
	**	stmtID     	Statement ID cache.
	**	tran_id    	Transaction ID.
	**	obj_id     	Object ID.
	**	lock_obj   	Object used to lock() and unlock().
	**	lock_active	Connection is locked.
	**	lock_thread_hashcode	Lock owner.
	**
	**	KS         	Static encryption buffers.
	**	kbuff
	**	rand
	**
	**  Private Methods:
	**
	**	disconnect	Disconnect from the server.
	**
	** History:
	**	 7-Jun-99 (gordy)
	**	    Created.
	**	10-Sep-99 (gordy)
	**	    Parameterized the initial TL connection data.
	**	29-Sep-99 (gordy)
	**	    Added methods (lock()/unlock()) and fields to provide
	**	    synchronization.
	**	 2-Nov-99 (gordy)
	**	    Added methods setDbmsInfo() and getDbmsInfo().
	**	 4-Nov-99 (gordy)
	**	    Added cancel() method and cncl output buffer.
	**	12-Nov-99 (gordy)
	**	    Added methods to permit configuration of timezone.
	**	 4-Feb-00 (gordy)
	**	    Added a thread reference for owner fo the current lock.
	**	19-May-00 (gordy)
	**	    Added public select_loops status field.
	**	31-May-00 (gordy)
	**	    Make date formatters public.
	**	19-Oct-00 (gordy)
	**	    Added getXactID().
	**	 3-Nov-00 (gordy)
	**	    Removed timezone fields/methods and replaced with public
	**	    boolean field, use_gmt_tz, indicating if connection uses
	**	    GMT timezone (default is to use GMT).
	**	20-Jan-01 (gordy)
	**	    Added msg_protocol_level now that there are more than 1 level.
	**	 5-Feb-01 (gordy)
	**	    Added stmtID as statment ID cache for the new method
	**	    getStmtID().
	**	10-May-01 (gordy)
	**	    Added public flag, ucs2_supported, for UCS2 support.
	**	20-Aug-01 (gordy)
	**	    Added default for connection cursors, cursor_mode, and
	**	    related constants: CRSR_DBMS, CRSR_READONLY, CRSR_UPDATE.
	**	20-Feb-02 (gordy)
	**	    Added db_protocol_level and is_ingres to handle differences
	**	    in DBMS protocol levels and gateways.
	**	20-Aug-02 (thoda04)
	**	    Ported for .NET Provider.
	**	 6-jul-04 (gordy; ported by thoda04)
	**	    Seed the random number generator.
	*/
	
	internal class MsgConn : MsgIn
	{
		/*
		** Output buffer used to cancel the current query (interrupt the
		** server).  The connection may be locked for the current query,
		** so synchronization of the output buffer cannot be done using
		** that mechanism.  Using a separate buffer avoids potential
		** conflicts with operations in the regular message buffer.
		*/
		private OutBuff	    cncl = null;

		/*
		** The connection must be locked from the start
		** of a message until the last of the response.
		** BLOB handling causes any synchronized method
		** or block to terminate prematurely, so methods
		** to lock and unlock the connection are provided.
		** A lock object is used for synchronization and
		** a boolean defines the state of the lock.
		*/
		private bool                    lock_active = false;
		private int                     lock_thread_hashcode = 0;
		private System.Object           lock_obj = new System.Object();
		private int                     lock_nest_count;
		private bool                    lock_nesting_allowed;
		
		/*
		** Buffers used by encode() (require synchronization).
		*/
		private static byte[] KS    = new byte[CI.KS_SIZE];
		private static byte[] kbuff = new byte[CI.CRYPT_SIZE];
		private static System.Random rand =  // seed the random num generator
			new System.Random( unchecked((int)DateTime.Now.Ticks) );
		
		
		/*
		** Name: MsgConn
		**
		** Description:
		**	Class constructor.  Opens a socket connection to target
		**	host and initializes the I/O buffers.  Sends the DAM-ML
		**	connection parameters and returns the response.
		**
		**	The DAM-ML connection parameters are formatted as an
		**	array of bytes.  The byte array reference is provided as
		**	the first entry in a multi-dimensional array parameter, 
		**	msg_cp.  The input array reference is sent to the 
		**	server and the response is placed in a new byte array 
		**	whose reference is returned in place of the input
		**	reference, msg_cp.
		**
		** Input:
		**	target 	Hostname and port.
		**	msg_cp  	Message layer parameter info.
		**
		** Output:
		**	msg_cp  	DAM-ML connection parameter response.
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	10-Sep-99 (gordy)
		**	    Parameterized the initial TL connection data.
		**	17-Nov-99 (gordy)
		**	    Extracted I/O functionality to DbConnIo, DbConnOut, DbConnIn.
		*/

		/// <summary>
		/// Class constructor.  Opens a socket connection to target
		/// host and initializes the I/O buffers.  Sends the DAM-ML
		/// connection parameters and returns the response.
		/// The DAM-ML connection parameters are formatted as an
		/// array of bytes.  The byte array reference is provided as
		/// the first entry in a multi-dimensional array parameter,
		/// msg_cp.  The input array reference is sent to the
		/// server and the response is placed in a new byte array
		/// whose reference is returned in place of the input
		/// reference, msg_cp.
		/// </summary>
		/// <param name="target"></param>
		/// <param name="msg_cp"></param>
		/// <param name="char_set"></param>
		public MsgConn(String target, ref byte[] msg_cp, CharSet char_set):
		                 base(target, ref        msg_cp,         char_set)
		{
			title = "Msg[" + ConnID + "]";
		} // MsgConn
		
		
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
		**	    Extracted I/O functionality to DbConnIo, DbConnOut, DbConnIn.*/
		
		protected internal override void  disconnect()
		{
			/*
			** We don't set the I/O buffer reference to null
			** here so that we don't have to check it on each
			** use.  I/O buffer functions will continue to work
			** until a request results in a stream I/O request,
			** in which case an exception will be thrown by the
			** I/O buffer.
			**
			** We must, however, test the reference for null
			** since we may be called by the constructor with
			** a null cancel buffer.
			*/
			if (cncl != null)
			{
				try { cncl.close(); }
				catch( Exception ) {}
				finally { cncl = null; }
			}
			
			base.disconnect();
			return ;
		} // disconnect
		
		
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
		**	    Extracted I/O functionality to DbConnIo, DbConnOut, DbConnIn.*/
		
		protected internal override void  close()
		{
			lock(this)
			{
				if (!isClosed())
				{
					base.close();
					disconnect();
				}
				return ;
			}
		} // close
		
		
		/*
		** Name: cancel
		**
		** Description:
		**	Issues an interrupt to the Data Access Server which
		**	will attempt to cancel any active query.
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
		**	 4-Nov-99 (gordy)
		**	    Created.
		**	26-Dec-02 (gordy)
		**	    Allocate cncl buffer on first request.
		*/
		
		public virtual void  cancel()
		{
			lock(this)
			{
				/*
				** Allocate cancel buffer on first request.
				*/
				if ( cncl == null )
					try 
					{
						cncl = new OutBuff(socket.GetStream(), ConnID, 16);
						cncl.TL_ProtocolLevel = this.TL_ProtocolLevel;
					}
					catch( Exception ex )
					{
						if ( trace.enabled( 1 ) )  
							trace.write( title + ": error creating cancel buffer: " + 
								ex.Message );
						disconnect();
						throw SqlEx.get( ERR_GC4001_CONNECT_ERR, ex );
					}

				try
				{
					if ( trace.enabled( 2 ) )
						trace.write( title + ": interrupt network connection" );
					cncl.begin( DAM_TL_INT, 0 );
					cncl.flush();
				}
				catch( SqlEx ex )
				{
					disconnect();
					throw ex;
				}

				return;
			}
		} // cancel
		
		
		/*
		** Name: LockConnection
		**
		** Description:
		**	Set a lock which will block all other threads until the
		**	connection is unlocked.  The lock is released by invoking.
		**	unlock().  A thread must not invoke lock() twice without
		**	invoking unlock() in between.
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
		**	 4-Feb-00 (gordy)
		**	    A single thread can dead-lock if it starts another operation
		**	    while it already holds the lock.  Save the lock owner and
		**	    throw an exception in this case.
		**	19-May-00 (gordy)
		**	    Changed trace messages.*/
		
		public virtual void  LockConnection()
		{
			LockConnection(false);  // by default, no nested LockConnection's
		}

		public virtual void  LockConnection(bool nesting_allowed)
		{
			lock(lock_obj)
			{
				while (lock_active)  // if already locked by someone
				{
					// if recursively locked by the same thread,
					// make sure it was intentional as in the case
					// of the DTCEnlistment's TxResourceAsyncThread.
					if (lock_thread_hashcode ==
						System.Threading.Thread.CurrentThread.GetHashCode())
					{
						if (lock_nesting_allowed)
						{
							lock_nest_count++;
							return;
						}

						if (trace.enabled(1))  // unintentional nested locking!
							trace.write(title + ".lock(): locked, current " +
								lock_thread_hashcode.ToString());
						throw SqlEx.get( ERR_GC4005_SEQUENCING );
					}
					
					try
					{
						System.Threading.Monitor.Wait(lock_obj);
						// wait for a Pulse from another
						// thread's UnlockConnection
					}
					catch (System.Exception /* ignore */)
					{
					}
				}
				
				// we have the lock!
				lock_thread_hashcode =
					System.Threading.Thread.CurrentThread.GetHashCode();
				lock_active = true;
				lock_nesting_allowed = nesting_allowed;
				lock_nest_count = 1;

				if (trace.enabled(5))
					trace.write(title + ".lock(): locked by " +
						lock_thread_hashcode.ToString());
			}  // end lock(lock_obj)
			
			return ;
		}
		
		
		/*
		** Name: UnlockConnection
		**
		** Description:
		**	Clear the lock set by LockConnection() and activate blocked threads.
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
		**	19-May-00 (gordy)
		**	    Check for additional invalid unlock conditions.*/
		
		public virtual void  UnlockConnection()
		{
			lock(lock_obj)
			{
				if (lock_thread_hashcode == 0 || !lock_active)
				{
					if (trace.enabled(1))
						trace.write(title + ".unlock(): not locked!, current " +
							System.Threading.Thread.CurrentThread.ToString());
					lock_thread_hashcode = 0;
					lock_active = false;
				}
				else if (lock_thread_hashcode !=
					System.Threading.Thread.CurrentThread.GetHashCode())
				{
					if (trace.enabled(1))
						trace.write(title + ".unlock(): not owner, current " +
							System.Threading.Thread.CurrentThread.ToString() +
							", owner " + lock_thread_hashcode.ToString());
				}
				else
				{
					if (trace.enabled(5))
						trace.write(title + ".unlock(): owner " +
							lock_thread_hashcode.ToString());

					// if we had nested LockConnection calls then just return;
					if (--lock_nest_count > 0)
						return;

					// get ready to release the next in the waiting queue
					lock_active = false;
					lock_thread_hashcode = 0;
					lock_nest_count = 0;
					lock_nesting_allowed = false;

					// move the next in the waiting queue to ready queue
					// to get it to ready to come out of its wait
					System.Threading.Monitor.Pulse(lock_obj);
				}
			}  // end lock(lock_obj)
			
			return ;
		}
		
		
		/*
		** Name: encode
		**
		** Description:
		**	Encode a string using a key.  The key and string are translated
		**	to the Server character set prior to encoding.  An optional key
		**	mask, CompatCI.CRYPT_SIZE in length, will be combined with the 
		**	key if provided.
		**
		** Input:
		**	key 	    Encryption key.
		**	mask	    Key mask, may be NULL.
		**	data	    Data to be encrypted
		**
		** Output:
		**	None.
		**
		** Returns:
		**	byte []	    Encrypted data.
		**
		** History:
		**	22-Sep-99 (gordy)
		**	    Created.
		**	 6-Sep-02 (gordy)
		**	    Character encoding now encapsulated in CharSet class.
		**	 6-jul-04 (gordy; ported by thoda04)
		**	    Added key mask.
		*/
		
//		public virtual byte[] encode(String key, String data)
		public virtual byte[] toBytes(  // obfuscate name
			String key, byte[] mask, String data)
			{
			byte[] buff;
			
			try
			{
				// encode password using obfuscated encode name method
//				buff = encode(   CharSet.getBytes( key ),
//				                 CharSet.getBytes( data ) );
				buff = toBytes( CharSet.getBytes( key ), mask,
				                 CharSet.getBytes( data ) );
			}
			catch (Exception ex)
			{
				throw SqlEx.get( ERR_GC401E_CHAR_ENCODE, ex );	// Should not happen!
			}

			return (buff);
		}
		
		
		/*
		** Name: encode
		**
		** Description:
		**	Applies the Driver semantics to convert the data and key 
		**	into form acceptable to CI and encodes the data using the 
		**	key provided.  An optional key mask, CompatCI.CRYPT_SIZE
		**	in length, will be combined with the key if provided.
		**
		** Input:
		**	key 	    Encryption key.
		**	mask	    Key mask, may be NULL.
		**	data	    Data to be encrypted.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	byte []	    Encrypted data.
		**
		** History:
		**	 6-May-99 (gordy)
		**	    Created.
		**	22-Sep-99 (gordy)
		**	    Converted parameters to byte arrays for easier processing
		**	    here and to support character set/encoding at a higher level.
		**	21-Apr-00 (gordy)
		**	    Extracted from class AdvanObject.
		**	 6-jul-04 (gordy; ported by thoda04)
		**	    Added key mask.
		*/
		
//		public static byte[] encode(byte[] key, byte[] data)
		public static byte[] toBytes(  // obfuscate name
			byte[] key, byte[] mask, byte[] data)
			{
			byte[] buff = null;
			int i, j, m, n, blocks;
			
			lock(KS)
			{
				/*
				** The key schedule is built from a single CRYPT_SIZE
				** byte array.  We use the input key to build the byte
				** array, truncating or duplicating as necessary.
				*/
				 for (m = n = 0; m < CI.CRYPT_SIZE; m++)
				{
					if (n >= key.Length)
						n = 0;
					kbuff[ m ] = (byte)(key[ n++ ] ^ 
						(mask != null ? mask[ m ] : (byte)0));
				 }
				
				CI.set(kbuff, KS);
				
				/*
				** The data to be encoded must be padded to a multiple
				** of CRYPT_SIZE bytes.  Random data is used for the
				** pad, so for strings the null terminator is included
				** in the data.  A random value is added to each block
				** so that a given key/data pair does not encode the
				** same way twice.  
				**
				** The total number of blocks can be calculated from 
				** the string length plus the null terminator divided 
				** into CRYPT_SIZE blocks (with one random byte): 
				**	((len+1) + (CRYPT_SIZE-2)) / (CRYPT_SIZE-1)
				** which can be simplified as: 
				**	(len / (CRYPT_SIZE - 1) + 1
				*/
				blocks = (data.Length / (CI.CRYPT_SIZE - 1)) + 1;
				buff = new byte[blocks * CI.CRYPT_SIZE];
				
				 for (i = m = n = 0; i < blocks; i++)
				{
					buff[m++] = (byte) (rand.Next( 256 ));
					
					 for (j = 1; j < CI.CRYPT_SIZE; j++)
						if (n < data.Length)
							buff[m++] = data[n++];
						else if (n > data.Length)
							buff[m++] = (byte) (rand.Next( 256 ));
						else
						{
							buff[m++] = 0; // null terminator
							n++;
						}
				}
				
				// encode using obfuscated encode name method
//				CI.encode  (buff, 0, buff.Length, KS, buff, 0);
				CI.toString(buff, 0, buff.Length, KS, buff, 0);
			}
			
			return (buff);
		} // encode
		
		

	}  // class MsgConn
}