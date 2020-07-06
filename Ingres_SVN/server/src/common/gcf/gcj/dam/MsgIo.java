/*
** Copyright (c) 1999, 2007 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.dam;

/*
** Name: MsgIo.java
**
** Description:
**	Defines base class representing a connection with a Data
**	Access Server.
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
**  Classes
**
**	MsgIo
**
** History:
**	 7-Jun-99 (gordy)
**	    Created.
**	17-Nov-99 (gordy)
**	    Extracted from DbConn.java.
**	21-Apr-00 (gordy)
**	    Moved to package io.
**	28-Mar-01 (gordy)
**	    Separated tracing interface and implementating.
**	 6-Sep-02 (gordy)
**	    Character encoding now encapsulated in CharSet class.
**	 1-Oct-02 (gordy)
**	    Moved to GCF dam package.  Renamed as DAM Message
**	    Layer implementation class.
**	29-Oct-02 (wansh01)
**	    Added public method isLocal to determine if connection is local.	
**	31-Oct-02 (gordy)
**	    Renamed GCF error codes.
**	20-Dec-02 (gordy)
**	    Track messaging protocol level.
**	15-Jan-03 (gordy)
**	    Added local/remote host name/address methods.
**	26-Feb-03 (gordy)
**	    Added getCharSet().
**	 3-May-05 (gordy)
**	    Added public abort() to perform same action (at this level)
**	    as protected disconnect().
**	 3-Jul-06 (gordy)
**	    Allow character-set to be overridden.
**	19-Oct-06 (lunbr01)  Sir 116548
**	    Allow for IPv6 addresses enclosed in square brackets '[]'.
**	    Try connecting to ALL IP @s for target host rather than just 1st.
**	 5-Dec-07 (gordy)
**	    Adding support for multiple server targets.
*/

import	java.net.InetAddress;
import	java.net.Socket;
import	java.util.Random;
import	com.ingres.gcf.util.GcfErr;
import	com.ingres.gcf.util.SqlEx;
import	com.ingres.gcf.util.CharSet;
import	com.ingres.gcf.util.Trace;
import	com.ingres.gcf.util.Tracing;
import	com.ingres.gcf.util.TraceLog;


/*
** Name: MsgIo
**
** Description:
**	Class representing a connection to a Data Access Server.
**	Provides functionality for establishing and dropping the 
**	connection.
**
**  Constants
**
**	DAM_ML_TRACE_ID	Message Layer trace ID.
**
**  Public Methods:
**
**	abort			Abort network connection.
**	connID			Return connection identifier.
**	isClosed		Is the connection closed?
**	isLocal			Is the connection local?
**	getRemoteHost		Get server machine name.
**	getRemoteAddr		Get server network address.
**	getLocalHost		Get local machine name.
**	getLocalAddr		Get local network address.
**	setProtocolLevel	Set Messaging Layer protocol level.
**	getCharSet		Get character-set used for connection.
**	setCharSet		Set character-set used for connection.
**
**  Protected Data:
**
**	socket			Network socket used for the connection.
**	char_set		Character encoding.
**	msg_proto_lvl		Message Layer protocol level.
**	tl_proto_lvl		Transport Layer protocol level.
**	title			Class title for tracing.
**	trace			Trace output.
**
**  Protected Methods:
**
**	connect			Connect to target server.
**	disconnect		Shutdown the physical connection.
**
**  Private Data:
**
**	conn_id			Connection ID.
**	connections		Static counter for generating connection IDs.
**
**  Private Methods:
**
**	close			Close socket.
**	translatePortID		Translate port ID to port number.
**
** History:
**	 7-Jun-99 (gordy)
**	    Created.
**	22-Sep-99 (gordy)
**	    Added character set/encoding.
**	17-Nov-99 (gordy)
**	    Extracted base level functionality from DbConn.
**	 6-Sep-02 (gordy)
**	    Character encoding now encapsulated in CharSet class.
**	 1-Oct-02 (gordy)
**	    Renamed as DAM-ML messaging class.  Added DAM-ML trace ID.
**	    Moved tl_proto, char_set and finalize() to sub-class.
**	29-Oct-02 (wansh01)
**	    Added public method isLocal to determine if connection is local.
**	20-Dec-02 (gordy)
**	    Track messaging protocol level.  Added msg_proto_lvl and 
**	    setProtocolLevel().  Restored tl_proto_lvl and char_set to
**	    make this class the central repository for all data within
**	    the messaging classes.
**	15-Jan-03 (gordy)
**	    Added getRemoteHost(), getRemoteAddr(), getLocalHost(), and
**	    getLocalAddr().
**	26-Feb-03 (gordy)
**	    Added getCharSet().
**	 3-May-05 (gordy)
**	    Extracted disconnect() functionality to private method close()
**	    so that it may be shared.  Added public method abort() which
**	    calls close() to drop the network connection.
**	 3-Jul-06 (gordy)
**	    Added setCharSet().
**	 5-Dec-07 (gordy)
**	    Extract connection functionality from constructor to connect().
**	    Moved target parsing methods getHost() and getPort() to MsgConn.
*/

class
MsgIo
    implements GcfErr, TlConst, MsgConst
{

    /*
    ** Constants
    */
    public static final	String	DAM_ML_TRACE_ID	= "msg";    // DAM-ML trace ID.

    /*
    ** The network connection.
    */
    protected Socket	socket		= null;
    protected CharSet	char_set	= null;
    
    /*
    ** Protocol must be assumed to be lowest level
    ** until a higher level is negotiated.
    */
    protected byte	msg_proto_lvl	= MSG_PROTO_1;
    protected byte	tl_proto_lvl	= DAM_TL_PROTO_1;

    /*
    ** Tracing.
    */
    protected String	title		= null;	// Title for tracing
    protected Tracing	trace;			// Tracing.

    /*
    ** Instance ID and connection count.
    */
    private int		conn_id		= -1;	// connection ID
    private static int	connections	= 0;	// Number of connections


/*
** Name: MsgIo
**
** Description:
**	Class constructor.
**
** Input:
**	log		Trace log.
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
**	17-Nov-99 (gordy)
**	    Extracted base functionality from DbConn.
**	28-Mar-01 (gordy)
**	    Separated tracing interface and implementation.
**	 1-Oct-02 (gordy)
**	    Renamed as DAM Message Layer.  Added trace log parameter.
**	 5-Dec-07 (gordy)
**	    Extracted connect() functionality.
*/

protected
MsgIo( TraceLog log )
    throws SqlEx
{
    conn_id = connections++;
    trace = new Tracing( log, DAM_ML_TRACE_ID );
    title = "MsgIo[" + conn_id + "]";
} // MsgIo


/*
** Name: connect
**
** Description:
**	Opens a socket connection to target host.
**
** Input:
**	host		Host name or address.
**	portID		Symbolic or numeric port ID.
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
**	19-Oct-06 (lunbr01)  Sir 116548
**	    Try connecting to ALL IP @s for target host rather than just 1st.
**	 5-Dec-07 (gordy)
**	    Extracted from constructor.
*/

protected void
connect( String host, String portID )
    throws SqlEx
{
    InetAddress	addr[];
    Exception	ex_last = null;

    if ( trace.enabled( 2 ) )  
	trace.write( title + ": opening network connection '" + 
			     host + "', '" + portID + "'" );

    try { addr = InetAddress.getAllByName( host ); }
    catch( Exception ex )
    {
	if ( trace.enabled( 1 ) )  
	    trace.write( title + ": Error resolving host address - " + 
				 ex.getMessage() );
	throw SqlEx.get( ERR_GC4001_CONNECT_ERR, ex );
    }

    int port = translatePortID( portID );

    for( int i = 0; i < addr.length; i++ )
    {
	String hostAddr = addr[i].getHostAddress();

	if ( trace.enabled( 5 )  &&  ! host.equalsIgnoreCase( hostAddr ) )
	    trace.write( title + ": " + host + " => " + hostAddr );

	try
	{
	    socket = new Socket( addr[i], port );
	    if ( trace.enabled( 3 ) )
		trace.write(title + ": connected to " + hostAddr + "," + port);
	    return;
	}
	catch( Exception ex )
	{
	    if ( trace.enabled( 1 ) )  
		trace.write( title + ": connection failed to " + hostAddr + 
				     "," + port + " - " + ex.getMessage() );
	    ex_last = ex;
	}
    }

    trace.write( title + ": all addresses have failed" );
    throw SqlEx.get( ERR_GC4001_CONNECT_ERR, ex_last );
} // connect


/*
** Name: close
**
** Description:
**	Close socket used for network connection.
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
**	 3-May-05 (gordy)
**	    Created.
*/

private void
close()
{
    if ( socket != null )
    {
	if ( trace.enabled( 2 ) )  
	    trace.write( title + ": closing network connection" );

	try { socket.close(); }
	catch( Exception ignore ) {}
	finally { socket = null; }
    }

    return;
} // close


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
*/

protected void
disconnect()
{
    close();
    return;
} // disconnect


/*
** Name: abort
**
** Description:
**	Abort the network connection.
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
**	 3-May-05 (gordy)
**	    Created.
*/

public void
abort()
{
    close();
    return;
}


/*
** Name: isClosed
**
** Description:
**	Returns an indication that the connection is closed.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean	    True if connection is closed, False otherwise.
**
** History:
**	 7-Jun-99 (gordy)
**	    Created.
*/

public boolean
isClosed()
{
    return( socket == null );
} // isClosed


/*
** Name: isLocal
**
** Description:
**	Returns an indication that the connection is local.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean	    True if connection is local, False otherwise.
**
** History:
**	 29-Oct-02 (wansh01)
**	    Created.
*/

public boolean
isLocal()
{
    return( (socket == null) ? false : 
	    socket.getInetAddress().equals( socket.getLocalAddress() ) );
} // isLocal


/*
** Name: connID
**
** Description:
**	Returns the numeric connection identifier.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	int	Connection identifier.
**
** History:
**	 7-Jun-99 (gordy)
**	    Created.
*/

public int
connID()
{
    return( conn_id );
} // connID


/*
** Name: getRemoteHost
**
** Description:
**	Returns the host name of the server machine.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String	Server host name or NULL if error.
**
** History:
**	15-Jan-03 (gordy)
**	    Created.
*/

public String
getRemoteHost()
{
    String host;

    try { host = socket.getInetAddress().getHostName(); }
    catch( Exception ex ) { host = null; }

    return( host );
} // getRemoteHost


/*
** Name: getRemoteAddr
**
** Description:
**	Returns the network address of the server machine.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String	Server network address or NULL if error.
**
** History:
**	15-Jan-03 (gordy)
**	    Created.
*/

public String
getRemoteAddr()
{
    return( socket.getInetAddress().getHostAddress() );
} // getRemoteAddr


/*
** Name: getLocalHost
**
** Description:
**	Returns the host name of the local machine.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String	Local host name or NULL if error.
**
** History:
**	15-Jan-03 (gordy)
**	    Created.
*/

public String
getLocalHost()
{
    String host;

    try { host = InetAddress.getLocalHost().getHostName(); }
    catch( Exception ex ) { host = null; }

    return( host );
} // getLocalHost


/*
** Name: getLocalAddr
**
** Description:
**	Returns the network address of the local machine.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String	Local network address or NULL if error.
**
** History:
**	15-Jan-03 (gordy)
**	    Created.
*/

public String
getLocalAddr()
{
    String host;

    try { host = InetAddress.getLocalHost().getHostAddress(); }
    catch( Exception ex ) { host = null; }

    return( host );
} // getLocalAddr


/*
** Name: setProtocolLevel
**
** Description:
**	Set the Messaging Layer protocol level.  This level is used
**	to build message packets.  Message content must still be
**	controlled by the class user's.
**
** Input:
**	msg_proto_lvl	Message Layer protocol level.
**
** Ouput:
**	None.
**
** Returns:
**	byte	    Previous protocol level.
**
** History:
**	20-Dec-02 (gordy)
**	    Created.
*/

public byte
setProtocolLevel( byte msg_proto_lvl )
{
    byte prev = this.msg_proto_lvl;
    this.msg_proto_lvl = msg_proto_lvl;
    return( prev );
} // setProtocolLevel


/*
** Name: getCharSet
**
** Description:
**	Returns the CharSet used to encode strings for the connection.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	CharSet	    Connection CharSet.
**
** History:
**	26-Feb-03 (gordy)
**	    Created.
*/

public CharSet
getCharSet()
{
    return( char_set );
} // getCharSet


/*
** Name: setCharSet
**
** Description:
**	Set the CharSet used to encode strings for the connection.
**
** Input:
**	char_set	CharSet to be used.
**
** Output:
**	None.
**
** Returns:
**	void
**
** History:
**	 3-Jul-06 (gordy)
**	    Created.
*/

public void
setCharSet( CharSet char_set )
{
    this.char_set = char_set;
    return;
} // setCharSet


/*
** Name: translatePortID
**
** Description:
**	Translates a symbolic or numeric port ID to a port number.
**	A negative value is returned if an error occurs.
**
** Input:
**	port	Port ID.
**
** Ouptut:
**	None.
**
** Returns:
**	int	Port number.
**
** History:
**	 1-Oct-02 (gordy)
**	    Extracted from getPort().
**	 5-Dec-07 (gordy)
**	    Throws exception if symbolic port ID cannot be translated
**	    into numeric value.
*/

private int
translatePortID( String port )
    throws SqlEx
{
    int portnum;

    /*
    ** Check for Ingres Installation ID format: 
    ** [a-zA-Z][a-zA-Z0-9]{[0-7]}
    */
    if ( port.length() == 2  ||  port.length() == 3 )
    {
	char c0 = port.charAt( 0 );
	char c1 = port.charAt( 1 );
	char c2 = (port.length() == 3) ? port.charAt( 2 ) : '0';

	if ( 
	     Character.isLetter( c0 )  &&  
	     Character.isLetterOrDigit( c1 )  &&
	     Character.isDigit( c2 ) 
	   )
	{
	    c0 = Character.toUpperCase( c0 );
	    c1 = Character.toUpperCase( c1 );

	    portnum = 1 << 14 | (c0 & 0x1f) << 9 | 
				(c1 & 0x3f) << 3 | (c2 & 0x07);

	    if ( trace.enabled( 5 ) )  
		trace.write( title + ": " + port + " => " + portnum );

	    return( portnum );
	}
    }

    try { portnum = Integer.parseInt( port ); }
    catch( Exception ex ) 
    {
	if ( trace.enabled( 1 ) )
	    trace.write( title + ": invalid port ID '" + port + "'" );
	throw SqlEx.get( ERR_GC4000_BAD_URL );
    }

    return( portnum );
} // translatePortID


} // class MsgIo

