/*
** Copyright (c) 1999, 2007 Ingres Corporation. All Rights Reserved.
*/

using System;
using Ingres.Utility;

namespace Ingres.ProviderInternals
{
	/*
	** Name: msgio.cs
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
	**	    MsgIo  	Establish socket connection.
	**	    MsgOut 	Send TL Connection Request packet.
	**	    MsgIn  	Receive TL Connection Confirm packet.
	**	    MsgConn	General initialization.
	**
	** History:
	**	 7-Jun-99 (gordy)
	**	    Created.
	**	17-Nov-99 (gordy)
	**	    Extracted from DbConn.cs.
	**	21-Apr-00 (gordy)
	**	    Moved to package io.
	**	28-Mar-01 (gordy)
	**	    Separated tracing interface and implementating.
	**	14-Aug-02 (thoda04)
	**	    Ported for .NET Provider.
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
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	**	17-nov-04 (thoda04)
	**	    Be forgiving and treat "(local)" as localhost.
	**	04-may-06 (thoda04)
	**	    Replace the obsolete .NET 1.1 GetHostByName() method with
	**	    new .NET 2.0 System.Net.Dns.GetHostEntry() method.
	**	 3-May-05 (gordy)
	**	    Added public abort() to perform same action (at this level)
	**	    as protected disconnect().
	**	 3-Jul-06 (gordy)
	**	    Allow character-set to be overridden.
	**	29-Dec-06 (lunbr01, thoda04)  Sir 116548
	**	    Allow for IPv6 addresses enclosed in square brackets '[]'.
	**	    Try connecting to ALL IP @s for target host rather than just 1st.
	**	19-sep-07 (thoda04)
	**	    Added isSocketConnected indicator to return health of underlying socket.
	*/


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
	**	connID  	Return connection identifier.
	**	isClosed	Is the connection closed?
	**	isLocal		Is the connection local?
	**	getRemoteHost	Get server machine name.
	**	getRemoteAddr	Get server network address.
	**	getLocalHost	Get local machine name.
	**	getLocalAddr	Get local network address.
	**	setProtocolLevel	Set Messaging Layer protocol level.
	**	getCharSet		Get character-set used for connection.
	**	setCharSet		Set character-set used for connection.
	**
	**  Protected Data:
	**
	**	socket  	Network socket used for the connection.
	**	char_set	Character encoding.
	**	tl_proto	Connection protocol level.
	**	title		Class title for tracing.
	**	trace		Trace output.
	**
	**  Protected Methods:
	**
	**	disconnect	Disconnect from the server.
	**
	**  Private Data:
	**
	**	conn_id      	Connection ID.
	**	connections  	Static counter for connection ID.
	**	msg_proto_lvl	Message Layer protocol level.
	**	tl_proto_lvl 	Transport Layer protocol level.
	**
	**  Private Methods:
	**
	**	close          	Close socket.
	**	getHost        	Extract host name from connection target.
	**	getPort        	Extract port from connection target.
	**	translatePortID	Translate symbolic/numeric port ID to port number.
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
	**	19-Oct-06 (lunbr01)  Sir 116548
	**	    Try connecting to ALL IP @s for target host rather than just 1st.
	*/

	internal class MsgIo : TlConst
	{
		/*
		** Constants
		*/
		public const String DAM_ML_TRACE_ID = "msg";    // DAM-ML trace ID.

		/*
		** The network connection.
		*/
		protected MsgSocket                     socket     = null;
		protected CharSet                       char_set = null;
		private   System.Net.IPAddress          ipAddress  = null;
		private   bool                          socketIsLocal = false;
    
		/*
		** Protocol must be assumed to be lowest level
		** until a higher level is negotiated.
		*/
		private byte  msg_proto_lvl = MSG_PROTO_1;
		private byte  tl_proto_lvl  = DAM_TL_PROTO_1;

		/*
		** Tracing.
		*/
		protected String	title = null;	// Title for tracing
		protected ITrace	trace;       	// Tracing.

		/*
		** Instance ID and connection count.
		*/
		private int       	    conn_id     = -1;	// connection ID
		private static int	    connections = 0; 	// Number of connections


		/*
		** Name: MsgIo
		**
		** Description:
		**	Class constructor.  Opens a socket connection to target host.
		**
		** Input:
		**	host_id		Hostname and port.
		**
		** Output:
		**	None.
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	17-Nov-99 (gordy)
		**	    Extracted base functionality from DbConn.
		**	28-Mar-01 (gordy)
		**	    Separated tracing interface and implementating.
		**	 1-Oct-02 (gordy)
		**	    Renamed as DAM-ML messaging class.
		**	17-nov-04 (thoda04)
		**	    Be forgiving and treat "(local)" as localhost.
		**	04-may-06 (thoda04)
		**	    Replace the obsolete .NET 1.1 GetHostByName() method with
		**	    new .NET 2.0 System.Net.Dns.GetHostEntry() method.
		*/

		protected internal MsgIo(String target)
		{
			String host = null;
			int    port;
			int    i;
			
			conn_id = connections++;
			title = "MsgConn[" + conn_id + "]";
			trace = new Tracing(DAM_ML_TRACE_ID);

			System.Net.IPHostEntry ipHostInfo = null;
			System.Net.IPEndPoint  ipEndPoint = null;
			socket                            = null;
			ipAddress                         = null;
			socketIsLocal                     = false;

			try
			{
				host = getHost(target);
				port = getPort(target);
				
				if (trace.enabled(2))
					trace.write(title + ": connecting to " + host + ":" + port);
			
				if (host == null  ||  host.Length==0  ||    // if local machine
					host.ToLower(System.Globalization.CultureInfo.InvariantCulture)
						== "(local)")
				{
					ipHostInfo = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName());
					socketIsLocal = true;
				}
				else
					ipHostInfo = System.Net.Dns.GetHostEntry(host);


				for (i = 0; i < ipHostInfo.AddressList.Length; i++ )  // loop addresses
				{
					ipAddress = ipHostInfo.AddressList[i];

					if (trace.enabled(3))
					{
						string address = ipAddress.ToString() + "  ";
						address += ipAddress.AddressFamily.ToString();
						if (ipAddress.AddressFamily ==
							System.Net.Sockets.AddressFamily.InterNetwork)
							address += "(IPv4)";
						else if (ipAddress.AddressFamily ==
							System.Net.Sockets.AddressFamily.InterNetworkV6)
						{
							address += "(IPv6),";
							address += "ScopeId=" + ipAddress.ScopeId.ToString();
						}
						trace.write(title + ": host address " + address +
							"; port=" + port);
					}

					ipEndPoint = new System.Net.IPEndPoint(ipAddress, port);
					socket = new MsgSocket();
					try
					{
						socket.Connect(ipEndPoint);
						return;  // success!
					}
					catch (Exception ex)
					{
						if (trace.enabled(1))
							trace.write(title + ": connection failed to address[" + i + "]=" + ipAddress + " - " + ex.Message);
						if (i == (ipHostInfo.AddressList.Length - 1))
							throw;   // continue throw of last exception up chain
						             // else try next address on list.
					}
				}  // end for loop through ipHostInfo.AddressList

				if (i == ipHostInfo.AddressList.Length)
				{  // should not have reached this point but we will bullet-proof
					trace.write(title + ": connection error - all addresses failed");
					throw SqlEx.get(ERR_GC4001_CONNECT_ERR);
				}
			}  // end try block
			catch (System.Net.Sockets.SocketException ex)
			{
				if (trace.enabled(1))
					trace.write(title +
						": error connecting to host - (SocketException) " +
						ex.Message);
				SqlEx pex = (SqlEx)
					SqlEx.get( ERR_GC4001_CONNECT_ERR, ex );
					// "Unable to establish connection due to communications error."
				if (ipAddress == null)  // is it the GetHostByName( )
					pex = SqlEx.get(pex, SqlEx.get(
						"SocketException thrown " +
						"by System.Net.Dns.GetHostByName(" +
						(host==null?"<null>":("\""+host+"\"")) + ") " +
						"in resolving the hostname",
							"08001",ex.ErrorCode));
				else  // must be TcpClient( ) call
					pex = SqlEx.get(pex, SqlEx.get(
						"SocketException thrown " +
						"by System.Net.Sockets.TcpClient( ) " +
						"while connecting to host " +
						(target==null?"<null>":("\""+target+"\".")),
							"08001",ex.ErrorCode));

				socket                            = null;
				ipAddress                         = null;

				throw pex;
			}
			catch (System.Exception ex)
			{
				if (trace.enabled(1))
					trace.write(title + ": error connecting to host - " + ex.Message);

				socket                            = null;
				ipAddress                         = null;

				throw SqlEx.get( ERR_GC4001_CONNECT_ERR, ex );
			}
		}  // MsgIo


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
			if (socket != null)
			{
				if (trace.enabled(2))
					trace.write(title + ": closing network connection");

				try { socket.Close(); }
				catch (Exception /* ignore */) { }
				finally
				{
					socket = null;
					ipAddress = null;
				}
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
		
		protected internal virtual void  disconnect()
		{
			close();
			return;
		}  // disconnect


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
		
		public virtual bool isClosed()
		{
			return (socket == null);
		}  // isClosed


		/*
		** Name: isSocketConnected
		**
		** Description:
		**	Returns an indication that the underly socket connection is connected.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	boolean	    True if socket connection is connected, False otherwise.
		**
		** History:
		**	19-sep-07 (thoda04)
		**	    Created.
		*/

		/// <summary>
		/// Return an indication that the underly physical socket connection is still connected.
		/// </summary>
		/// <returns></returns>
		public virtual bool isSocketConnected()
		{
			if (socket == null)
				return false;
			return (socket.Connected);
		}  // isSocketConnected


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

		public bool isLocal()
		{
			return( (socket == null) ? false : this.socketIsLocal);
//				socket.getInetAddress().equals( socket.getLocalAddress() ) );
		} // isLocal


		/*
		** Name: connID property
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

		/// <summary>
		/// The numeric connection identifier.
		/// </summary>
		public int ConnID
		{
			get {return (conn_id); }
		}  // connID


		/*
		** Name: RemoteHost
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

		public String RemoteHost
		{
			get { return socket.RemoteHostName; }
		} // RemoteHost


		/*
		** Name: RemoteAddress
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

		public String RemoteAddress
		{
			get { return( socket.RemoteAddress.ToString() ); }
		} // RemoteAddress


		/*
		** Name: LocalHost
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

		public String LocalHost
		{
			get { return( socket.LocalHostName ); }
		} // LocalHost


		/*
		** Name: LocalAddress
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

		public String LocalAddress
		{
			get { return( socket.LocalAddress.ToString() ); }
		} // LocalAddress


		/*
		** Name: ML_ProtocolLevel property
		**
		** Description:
		**	Get/Set the Messaging Layer protocol level.  This level is used
		**	to build message packets.  Message content must still be
		**	controlled by the class user's.
		**
		** Input:
		**	value	Message Layer protocol level.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	None.
		**
		** History:
		**	20-Dec-02 (gordy)
		**	    Created.
		*/

		public byte ML_ProtocolLevel
		{
			get { return this.msg_proto_lvl ; }
			set { this.msg_proto_lvl = value; }
		} // setProtocolLevel


		/*
		** Name: TL_ProtocolLevel property
		**
		** Description:
		**	Get/Set the Transport Layer protocol level.  This level is used
		**	to build message packets.  Message content must still be
		**	controlled by the class user's.
		**
		** Input:
		**	value	Message Layer protocol level.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	None.
		**
		** History:
		**	20-Dec-02 (gordy)
		**	    Created.
		*/

		public byte TL_ProtocolLevel
		{
			get { return this.tl_proto_lvl ; }
			set { this.tl_proto_lvl = value; }
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
			return (char_set);
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
		setCharSet(CharSet char_set)
		{
			this.char_set = char_set;
			return;
		} // setCharSet


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
		** Output:
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

		/// <summary>
		/// Extracts and returns the name of the host from the target.
		/// </summary>
		/// <param name="target"></param>
		/// <returns>Name of the host from the target.</returns>
		private String getHost(String target)
		{
			int index;
			int start = 0;
			if (target.StartsWith("["))
			{
				start = 1;
				index = target.IndexOf(']');
				if (index < 0)
				{
					if (trace.enabled(1))
						trace.write(title + ": right bracket ']' missing at end of IPv6 @ in '" + target + "'");
					throw SqlEx.get(ERR_GC4000_BAD_URL);
				}
			}
			else
				index = target.IndexOf(':');

			return (index < 0 ? target : target.Substring(start, index - start));
		}  // getHost


		/*
		** Name: getPort
		**
		** Description:
		**	Extracts and returns the port number from the target string:
		**
		**	    <host>:<port>
		**
		**	The port may be specified as a numeric string or in standard
		**	Ingres installation ID format such as II0.
		**
		** Input:
		**	target
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	    Port number.
		**
		** History:
		**	 7-Jun-99 (gordy)
		**	    Created.
		**	 1-Oct-02 (gordy)
		**	    Extracted symbolic port translation to separate method.
		**	19-Oct-06 (lunbr01)  Sir 116548
		**	    Allow for IPv6 addresses enclosed in square brackets '[]'.
		*/

		private int getPort(String target)
		{
			String port;
			int portnum, index;

			if (target.StartsWith("["))
			{
				index = target.IndexOf("]:");
				if (index >= 0)
					index++;
			}
			else
				index = target.IndexOf(':', 0);
			
			if (index < 0 || target.Length <= ++index)
			{
				if (trace.enabled(1))
					trace.write(title + ":port number missing '" + target + "'");
				port = "II7";  // default
			}
			else
				port = target.Substring(index);
			
			if ((portnum = translatePortID(port)) < 0)
			{
				if (trace.enabled(1))
					trace.write(title + ": invalid port '" + port + "'");
				throw SqlEx.get( ERR_GC4000_BAD_URL );
			}

			return (portnum);
		}  // getPort


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
		*/

		private int  translatePortID( String port )
		{
			int portnum;

			/*
			** Check for Ingres Installation ID format: 
			** [a-zA-Z][a-zA-Z0-9]{[0-7]}
			*/
			if (port.Length == 2 || port.Length == 3)
			{
				char c0 = port[0];
				char c1 = port[1];
				char c2 = (port.Length == 3)?port[2]:'0';
				
				if (System.Char.IsLetter(c0)        &&
					System.Char.IsLetterOrDigit(c1) &&
					System.Char.IsDigit(c2))
				{
					c0 = System.Char.ToUpper(c0,
						System.Globalization.CultureInfo.InvariantCulture);
					c1 = System.Char.ToUpper(c1,
						System.Globalization.CultureInfo.InvariantCulture);
					
					return (1 << 14 | (c0 & 0x1f) << 9 | (c1 & 0x3f) << 3 | (c2 & 0x07));
				}
			}

			try
			{
				portnum = System.Int32.Parse(port);
			}
			catch (Exception)
			{
				portnum = -1;;
			}

			return (portnum);
		} // translatePortID

		/*
		** Name: QuerySocketHealth
		**
		** Description:
		**	Query the MSG's underly socket to test if it is still connected.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	boolean	    True if socket connection is connected, False otherwise.
		**
		** History:
		**	19-sep-07 (thoda04)
		**	    Created.
		*/

		/// <summary>
		/// Test and return an indication that the underly physical socket connection is still connected.
		/// </summary>
		/// <returns></returns>
		public virtual bool QuerySocketHealth()
		{
			if (socket == null)
				return false;

			return socket.QuerySocketHealth();
		}  // QuerySocketHealth


	}  // class MsgIo



	/*
	** Name: MsgSocket
	**
	** Description:
	**	Class representing a socket connection to a Data Access Server.
	**	This class gives us access to the underlying Socket client under
	**	the TcpClient object.
	**
	**  Public Properties:
	**
	**	LocalAddress  	Returns IPAddress of local side of socket connection.
	**	RemoteAddress 	Returns IPAddress of remote side of socket connection.
	**	LocalHostName 	Returns the host name of the local machine.
	**	RemoteHostName	Returns the host name of the remote machine.
	**
	** History:
	**	12-Nov-03 (thoda04)
	**	    Created.
	*/
	
	internal class MsgSocket : System.Net.Sockets.TcpClient
	{

		/*
		** Name: LocalAddress
		**
		** Description:
		**	Returns the IPAddress of the local side of the socket connection.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	IPAddress	IPAddress of the local side of the socket connection.
		**
		** History:
		**	12-Nov-03 (thoda04)
		**	    Created.
		*/

		public System.Net.IPAddress LocalAddress
		{
			get
			{
				System.Net.Sockets.Socket socket = base.Client;
				System.Net.IPEndPoint ipEndPoint =
					(System.Net.IPEndPoint)socket.LocalEndPoint;
				return ipEndPoint.Address;
			}
		}


		/*
		** Name: RemoteAddress
		**
		** Description:
		**	Returns the IPAddress of the remote side of the socket connection.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	IPAddress	IPAddress of the remote side of the socket connection.
		**
		** History:
		**	12-Nov-03 (thoda04)
		**	    Created.
		*/

		public System.Net.IPAddress RemoteAddress
		{
			get
			{
				System.Net.Sockets.Socket socket = base.Client;
				System.Net.IPEndPoint ipEndPoint =
					(System.Net.IPEndPoint)socket.RemoteEndPoint;
				return ipEndPoint.Address;
			}
		}


		/*
		** Name: LocalHostName
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
		**	12-Nov-03 (thoda04)
		**	    Created.
		*/

		public string LocalHostName
		{
			get
			{
				try
				{
				System.Net.IPAddress ipAddress   = this.LocalAddress;
				System.Net.IPHostEntry hostentry =
					System.Net.Dns.GetHostEntry(ipAddress);

				string hostname = hostentry.HostName;
				return hostname;
				}
				catch (Exception)
				{
					return null;
				}
			}
		}


		/*
		** Name: RemoteHostName
		**
		** Description:
		**	Returns the host name of the remote machine.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String	Remote host name or NULL if error.
		**
		** History:
		**	12-Nov-03 (thoda04)
		**	    Created.
		*/

		public string RemoteHostName
		{
			get
			{
				try
				{
					System.Net.IPAddress ipAddress   = this.RemoteAddress;
					System.Net.IPHostEntry hostentry =
						System.Net.Dns.GetHostEntry(ipAddress);

					string hostname = hostentry.HostName;
					return hostname;
				}
				catch (Exception)
				{
					return null;
				}
			}
		}

		public InputStream getInputStream()
		{
			return new InputStream(base.GetStream());
		}


		/*
		** Name: QuerySocketHealth
		**
		** Description:
		**	Send a zero length message to test if socket is still connected.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Bool	Socket is Connected or not.
		**
		** History:
		**	19-Sep-07 (thoda04)
		**	    Created to test for severed socket into and out of connection pool.
		*/

		/// <summary>
		/// Query the underlying socket connection health to test if Connected.
		/// </summary>
		/// <returns>True if the underlying socket is Connected.</returns>
		public bool QuerySocketHealth()
		{
			if (Connected == false) // if socket already knows it's dead, then return
			   return false;

			bool blocking = this.Client.Blocking;  // save state of socket.Blocking

			try
			{
				// don't block execution if can't complete immediately
				this.Client.Blocking = false;

				byte[] emptymsg = new byte[1];
				Client.Send(emptymsg, 0,  // ping the socket with zero-length msg
					System.Net.Sockets.SocketFlags.None);
			}
			catch (Exception)
			{
			}
			finally
			{
				this.Client.Blocking = blocking; // restore state of socket.Blocking
			}

			return Connected;
		}

	}  // class MsgSocket
}