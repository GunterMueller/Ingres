/*
** Copyright (c) 1999, 2006 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.dam;

/*
** Name: MsgConn.java
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
**	 4-Nov-99 (gordy)
**	    Added ability to cancel a query.
**	17-Nov-99 (gordy)
**	    Extracted I/O functionality to DbConnIo.java, DbConnOut.java
**	    and DbConnIn.java.
**	 4-Feb-00 (gordy)
**	    A single thread can dead-lock if it starts another operation
**	    while it already holds the lock.  Save the lock owner and
**	    throw an exception in this case.
**	21-Apr-00 (gordy)
**	    Moved to package io.
**	 6-Sep-02 (gordy)
**	    Character encoding now encapsulated in CharSet class.
**	 1-Oct-02 (gordy)
**	    Moved to GCF dam package.  Renamed as DAM Message Layer
**	    implementation class.  Extracted non-messaging functionality
**	    to jdbc DbConn class.
**	31-Oct-02 (gordy)
**	    Renamed GCF error codes.
**	20-Dec-02 (gordy)
**	    Character set/encoding may be explicitly specified.
**	    Cancel buffer now allocated only when needed.
**       2-Sep-03 (weife01)
**          Bug 110772: Allow non-owner thread to do the unlock.
**	16-Jun-04 (gordy)
**	    Added option key mask to encoding routines.
**	 3-May-05 (gordy)
**	    Removed abort() since nothing needs to be done at this level.
**	 5-Dec-07 (gordy)
**	    Adding support for multiple DAS targets.
*/

import	java.util.Random;
import	com.ingres.gcf.util.CharSet;
import	com.ingres.gcf.util.CompatCI;
import	com.ingres.gcf.util.GcfErr;
import	com.ingres.gcf.util.SqlEx;
import	com.ingres.gcf.util.TraceLog;


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
**  Public Methods:
**
**	close		Close the connection.
**	cancel		Cancel current query.
**	lock		Lock connection.
**	unlock		Unlock connection.
**	encode		Encrypt a string using a key.
**
**  Private Data:
**
**	cncl		Cancel output buffer.
**	lock_obj	Object used to lock() and unlock().
**	lock_active	Connection is locked.
**	lock_thread	Lock owner.
**
**	KS		Static encryption buffers.
**	kbuff
**	rand
**
**  Private Methods:
**
**	disconnect	Disconnect from the server.
**	getHost		Extract host name from connection target.
**	getPort		Extract port from connection target.
**	buildPortList	Build list of port ID's.
**	randomSort	Randomly order the entries in an array.
**
** History:
**	 7-Jun-99 (gordy)
**	    Created.
**	10-Sep-99 (gordy)
**	    Parameterized the initial TL connection data.
**	29-Sep-99 (gordy)
**	    Added methods (lock()/unlock()) and fields to provide
**	    synchronization.
**	 4-Nov-99 (gordy)
**	    Added cancel() method and cncl output buffer.
**	 4-Feb-00 (gordy)
**	    Added a thread reference for owner fo the current lock.
**	 1-Oct-02 (gordy)
**	    Renamed as DAM-ML messaging class.  Extracted data/methods 
**	    not directly associated with the DAM-ML connection to jdbc 
**	    DbConn class.  Moved finalize() here from super-class.
**	16-Jun-04 (gordy)
**	    Seed the random number generator.
**	 2-Feb-05 (gordy)
**	    Added ability to abort connection.
**	 3-May-05 (gordy)
**	    Moved abort() to MsgIo class.
*/

public class
MsgConn 
    extends MsgIn
    implements TlConst, GcfErr
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
    private boolean	    lock_active = false;
    private Thread	    lock_thread = null;
    private Object	    lock_obj = new Object();

    /*
    ** Buffers used by encode() (require synchronization).
    */
    private static byte	    KS[] = new byte[ CompatCI.KS_SIZE ];
    private static byte	    kbuff[] = new byte[ CompatCI.CRYPT_SIZE ];
    private static Random   rand = new Random( System.currentTimeMillis() );


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
**	msg_cp[0].  The input array reference is sent to the 
**	server and the response is placed in a new byte array 
**	whose reference is returned in place of the input
**	reference, msg_cp[0].
**
** Input:
**	target		Hostname and port.
**	msg_cp		DAM-ML connection parameters.
**	log		Trace log.
**
** Output:
**	msg_cp		DAM-ML connection parameter response.
**
** History:
**	 7-Jun-99 (gordy)
**	    Created.
**	10-Sep-99 (gordy)
**	    Parameterized the initial TL connection data.
**	17-Nov-99 (gordy)
**	    Extracted I/O functionality to DbConnIo, DbConnOut, DbConnIn.
**	 1-Oct-02 (gordy)
**	    Renamed as DAM Message Layer.  Replaced connection info 
**	    parameters with multi-dimensional array.  Added trace log 
**	    parameter.
**	20-Dec-02 (gordy)
**	    Added char_set parameter for connection character encoding.
**	    Moved cncl buffer allocation to point where used.
**	 2-Feb-05 (gordy)
**	    Added abort() to unconditionally close connection.
**	 3-Jul-06 (gordy)
**	    Character-set may now be overridden using setCharSet(),
**	    so dropped char_set parameter.
**	 5-Dec-07 (gordy)
**	    Separated connection logic from super-class constructors
**	    to facilitate multiple server targets.
*/

public
MsgConn( String target, byte msg_cp[][], TraceLog log )
    throws SqlEx
{
    super( log );
    title = "Msg[" + connID() + "]";

    String	host;
    String	ports[];
    SqlEx	last_ex = null;

    try
    {
	host = getHost( target );
	ports = getPort( target );
    }
    catch( SqlEx sqlEx )
    {
	throw sqlEx;
    }
    catch( Exception ex )
    {
	if ( trace.enabled( 1 ) )  
	    trace.write( title + ": URL parsing error - " + ex.getMessage() );
	throw SqlEx.get( ERR_GC4000_BAD_URL, ex );
    }

    randomSort( ports );

    for( int i = 0; i < ports.length; i++ )
    {
	if ( ports[i].length() < 1 )  continue;

	try
	{
            connect( host, ports[i] );
            sendCR( msg_cp[0] );
            msg_cp[0] = recvCC();
	    return;			// Connection established
	}
	catch( SqlEx ex )
	{
	    last_ex = ex;
	}
    }

    if ( last_ex == null )  
    {
	/*
	** The only reason that no exception would be generated
	** when no connection is established is a missing port ID.
	*/
	if ( trace.enabled( 1 ) )
	    trace.write( title + ": No port specified in URL" );
	last_ex = SqlEx.get( ERR_GC4000_BAD_URL );
    }

    throw last_ex;
} // MsgConn


/*  
** Name: finalize
**
** Description:
**	Class destructor.  Close connection if still open.
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
*/

protected void 
finalize() 
    throws Throwable 
{
    disconnect();
    super.finalize();
    return;
} // finalize


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
**	    Extracted I/O functionality to DbConnIo, DbConnOut, DbConnIn.
*/

protected void
disconnect()
{
    /*
    ** Shutdown the cancel buffer, if allocated.
    */
    if ( cncl != null )
    {
	try { cncl.close(); }
	catch( Exception ignore ) {}
	finally { cncl = null; }
    }

    super.disconnect();
    return;
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
**	    Extracted I/O functionality to DbConnIo, DbConnOut, DbConnIn.
*/

public synchronized void
close()
{
    if ( ! isClosed() )
    {
	super.close();
	disconnect();
    }
    return;
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

public synchronized void
cancel()
    throws SqlEx
{
    /*
    ** Allocate cancel buffer on first request.
    */
    if ( cncl == null )
	try 
	{
	    cncl = new OutBuff( socket.getOutputStream(), 
				connID(), 16, trace.getTraceLog() );
	    cncl.setProtoLvl( tl_proto_lvl );
	}
	catch( Exception ex )
	{
	    if ( trace.enabled( 1 ) )  
		trace.write( title + ": error creating cancel buffer: " + 
			     ex.getMessage() );
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
} // cancel


/*
** Name: lock
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
**	    Changed trace messages.
*/

public void
lock()
    throws SqlEx
{
    synchronized( lock_obj )
    {
	while( lock_active )  
	{
	    if ( lock_thread == Thread.currentThread() )
	    {
		if ( trace.enabled( 1 ) )
		    trace.write( title + ".lock(): connection locked by " + 
				 Thread.currentThread() );
	    	throw SqlEx.get( ERR_GC4005_SEQUENCING );
	    }

	    try { lock_obj.wait(); }
	    catch( Exception ignore ) {}
	}

	lock_thread = Thread.currentThread();
	lock_active = true;
	if ( trace.enabled( 5 ) )
	    trace.write( title + ".lock(): owner " + lock_thread );
    }

    return;
}


/*
** Name: unlock
**
** Description:
**	Clear the lock set by lock() and activate blocked threads.
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
**	    Check for additional invalid unlock conditions.
**	 2-Sep-03 (weife01)
**	    Bug 110772: Allow non-owner thread to do the unlock.
*/

public void
unlock()
{
    synchronized( lock_obj )
    {
	if ( lock_thread == null  ||  ! lock_active )
	{
	    if ( trace.enabled( 5 ) )
		trace.write( title + ".unlock(): conn not locked!" );
	    lock_thread = null;
	    lock_active = false;
	}
	else  
	{
	    if ( trace.enabled( 5 ) )
	    {
		trace.write( title + ".unlock(): owner " + lock_thread );

		if ( lock_thread != Thread.currentThread() )
		    trace.write( title + ".unlock(): current thread " + 
				 Thread.currentThread() );
            }

	    lock_active = false;
	    lock_thread = null;
	    lock_obj.notify();
	}
    }

    return;
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
**	key	    Encryption key.
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
**	16-Jun-04 (gordy)
**	    Added key mask.
*/

public byte []
encode( String key, byte mask[], String data )
    throws SqlEx
{
    byte buff[];

    try { buff = encode( char_set.getBytes( key ), mask,
			 char_set.getBytes( data ) ); }
    catch( Exception ex )
    {
        throw SqlEx.get( ERR_GC401E_CHAR_ENCODE );	// Should not happen!
    }

    return( buff );
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
**	key	    Encryption key.
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
**	    Extracted from class EdbcObject.
**	16-Jun-04 (gordy)
**	    Added key mask.
*/

public static byte []
encode( byte key[], byte mask[], byte data[] )
{
    byte    buff[] = null;
    int	    i, j, m, n, blocks;

    synchronized( KS )
    {
	/*
	** The key schedule is built from a single CRYPT_SIZE
	** byte array.  We use the input key to build the byte
	** array, truncating or duplicating as necessary.
	*/
	for( m = n = 0; m < CompatCI.CRYPT_SIZE; m++ )
	{
	    if ( n >= key.length )  n = 0;
	    kbuff[ m ] = (byte)(key[ n++ ] ^ (mask != null ? mask[ m ] : 0));
	}

	CompatCI.setkey( kbuff, KS );

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
	blocks = (data.length / (CompatCI.CRYPT_SIZE - 1)) + 1;
	buff = new byte[ blocks * CompatCI.CRYPT_SIZE ];

	for( i = m = n = 0; i < blocks; i++ )
	{
	    buff[ m++ ] = (byte)rand.nextInt( 256 );

	    for( j = 1; j < CompatCI.CRYPT_SIZE; j++ )
		if ( n < data.length )
		    buff[ m++ ] = data[ n++ ];
		else  if ( n > data.length )
		    buff[ m++ ] = (byte)rand.nextInt( 256 );
		else
		{
		    buff[ m++ ] = 0;	// null terminator
		    n++;
		}
	}

	CompatCI.encode( buff, 0, buff.length, KS, buff, 0 );
    }

    return( buff );
} // encode


/*
** Name: getHost
**
** Description:
**	Extracts and returns the name of the host from the target
**	string:
**
**	    <host>[:<port>]
**
** Input:
**	target
**
** Ouptut:
**	None.
**
** Returns:
**	String	    Host name.
**
** History:
**	 7-Jun-99 (gordy)
**	    Created.
**	19-Oct-06 (lunbr01)  Sir 116548
**	    Allow for IPv6 addresses enclosed in square brackets '[]'.
*/

private String
getHost( String target )
    throws SqlEx
{
    int     index;
    int     start = 0;

    if ( target.startsWith( "[" ) )
    {
	if ( (index = target.indexOf( ']' )) < 0 )
	{
	    if ( trace.enabled( 1 ) )
	        trace.write( title + ": right bracket ']' missing " +
				     "in IPv6 address '" + target + "'" );
	    throw SqlEx.get( ERR_GC4000_BAD_URL );
	}

	start = 1;
    }
    else
	index = target.indexOf( ':' );

    return( index < 0 ? target : target.substring( start, index ) );
} // getHost


/*
** Name: getPort
**
** Description:
**	Extracts and returns the port ID list from the target string:
**
**	    <host>:<port>{,<port>}
**
** Input:
**	target		Host/port list target string.
**
** Output:
**	None.
**
** Returns:
**	String []	Port ID list.
**
** History:
**	 7-Jun-99 (gordy)
**	    Created.
**	 1-Oct-02 (gordy)
**	    Extracted symbolic port translation to separate method.
**	19-Oct-06 (lunbr01)  Sir 116548
**	    Allow for IPv6 addresses enclosed in square brackets '[]'.
**	 5-Dec-07 (gordy)
**	    List of port ID's now supported and returned as individual
**	    entries in an array.  Ports are no longer translated into
**	    numeric values.
*/

private String []
getPort( String target )
    throws SqlEx
{
    int	index = 0;

    /*
    ** Skip past IPv6 host address if present.
    */
    if ( target.startsWith( "[" ) )
    {
	if ( (index = target.indexOf( ']' )) < 0 )
	{
	    if ( trace.enabled( 1 ) )
	        trace.write( title + ": right bracket ']' missing " +
				     "in IPv6 address '" + target + "'" );
	    throw SqlEx.get( ERR_GC4000_BAD_URL );
	}
    }

    index = target.indexOf( ':', index );

    if ( index < 0 )
    {
	if ( trace.enabled( 1 ) )
	    trace.write( title + ": port ID missing '" + target + "'" );
	throw SqlEx.get( ERR_GC4000_BAD_URL );
    }

    return( buildPortList( target, index + 1, 0 ) );
} // getPort


/*
** Name: buildPortList
**
** Description:
**	Parses a comma separated list of port ID's and
**	returns an array with an entry for each port
**	in the list:
**
**	    <port>{,<port>}
**
**	This method is called recursively to process each
**	individual port.  Initial call should pass 0 for
**	starting port count.
**
** Input:
**	target	Comma separated port list.
**	start	Offset in target of the current port to process.
**	count	Number of ports preceding current port.
**
** Output:
**	None.
**
** Returns:
**	String []	Array of port ID's.
**
** History:
**	 5-Dec-07 (gordy)
**	    Created.
*/

private String []
buildPortList( String target, int start, int count )
{
    String	ports[];
    int		index;

    /*
    ** Ports may be specified in a comma separated list.
    ** Check to see if the last port has been reached
    ** or if there are additional ports.
    */
    if ( (index = target.indexOf( ',', start )) >= 0 )
    {
	/*
	** Recursively process the tail of the port list.
	*/
	ports = buildPortList( target, index + 1, count + 1 );

	/*
	** Save current entry.
	*/
        ports[ count ] = target.substring( start, index );
    }
    else
    {
	/*
	** This is the last, or only, port entry.
	** Allocate the port list array including
	** room for all preceding ports, and save
	** this, the final, port in the list.
	*/
	ports = new String[ count + 1 ];
	ports[ count ] = target.substring( start );
    }

    return( ports );
} // buildPortList


/*
** Name: randomSort
**
** Description:
**	Randomly orders the entries in an array.
**
** Input:
**	strings	String array to be randomly sorted.
**
** Output:
**	strings	Entries in array may be moved.
**
** Returns:
**	void
**
** History:
**	 5-Dec-07 (gordy)
**	    Created.
*/

private static void
randomSort( String strings[] )
{

    switch( strings.length )
    {
    case 0 :
    case 1 : /* Nothing to do */	break;

    case 2 :
    {
	/*
	** For just two entries, randomly switch or leave alone.
	*/
	Random	rand = new Random();

	if ( rand.nextBoolean() )
	{
	    String tmp = strings[0];
	    strings[0] = strings[1];
	    strings[1] = tmp;
	}
	break;
    }
    default:
    {
	/*
	** For everyone else, step through all entries and
	** randomly switch with some other entry.
	*/
	Random	rand = new Random();

	for( int i = 0; i < strings.length; i++ )
	{
	    int j = rand.nextInt( strings.length );

	    /*
	    ** Nothing to do if target is same as source.
	    ** Another option would be to continue getting
	    ** targets until the target is different from
	    ** the source (force a swap).
	    */ 
	    if ( i != j )
	    {
		String tmp = strings[i];
		strings[i] = strings[j];
		strings[j] = tmp;
	    }
	}
	break;
    }
    }

    return;
} // randomSort


} // class MsgConn


