/*
** Copyright (c) 2001, 2007 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.jdbc;

/*
** Name: JdbcCPVirt.java
**
** Description:
**	Defines class which implements the JDBC Connection interface
**	as a virtual cover for a pooled connection.
**
**  Classes
**
**	JdbcCPVirt
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
**	12-Apr-01 (gordy)
**	    Exceptions are now self tracing.
**	31-Oct-02 (gordy)
**	    Adapted for generic GCF driver.
**	14-Feb-03 (gordy)
**	    Updated to JDBC 3.0.
**	 4-May-07 (gordy)
**	    Set class access for reflection.
*/

import	java.util.Map;
import	java.sql.Connection;
import	java.sql.Statement;
import	java.sql.PreparedStatement;
import	java.sql.CallableStatement;
import	java.sql.DatabaseMetaData;
import	java.sql.Savepoint;
import	java.sql.SQLException;
import	java.sql.SQLWarning;
import	javax.sql.PooledConnection;
import	javax.sql.ConnectionEvent;
import	javax.sql.ConnectionEventListener;
import	com.ingres.gcf.util.SqlEx;
import	com.ingres.gcf.util.GcfErr;


/*
** Name: JdbcCPVirt
**
** Description:
**	JDBC driver class which implements the JDBC Connection interface 
**	as a virtual cover for a pooled connection.
**
**	This class wraps the interface methods of the associated connection 
**	to provide connection close request and error event notification to 
**	a single event listener.
**
**  Interface Methods:
**
**	isClosed		Determine if connection is closed.
**	close			Close the connection.
**	getWarnings		Retrieve SQLWarning list.
**	clearWarnings		Clear SQLWarning list.
**	createStatement		Create a Statement object.
**	prepareStatement	Create a PreparedStatement object.
**	prepareCall		Create a CallableStatement object
**	getHoldability		Retrieve ResultSet holdability.
**	setHoldability		Set ResultSet holdability.
**	nativeSQL		Translate SQL into native DBMS SQL.
**	getMetaData		Create a DatabaseMetaData object.
**	getCatalog		Retrieve the DBMS catalog.
**	setCatalog		Set the DBMS catalog.
**	getTypeMap		Retrieve the type map.
**	setTypeMap		Set the type map.
**	isReadOnly		Determine readonly state.
**	setReadOnly		Set readonly state
**	getTransactionIsolation	Determine the transaction isolation level.
**	setTransactionIsolation	Set the transaction isolation level.
**	getAutoCommit		Determine autocommit state.
**	setAutoCommit		Set autocommit state.
**	commit			Commit current transaction.
**	rollback		Rollback current transaction.
**	setSavepoint		Create savepoint.
**	releaseSavepoint	Remove savepoint.
**
**  Protected Data:
**
**	trace			DataSource tracing.
**	title			Class tracing title.
**	tr_id			Class tracing ID.
**	inst_id			Instance ID.
**	conn			JDBC connection.
**
**  Private Data:
**
**	pool			Pooled connection owner.
**	listener		Connection event listener.
**	inst_count		Class instance counter.
**
**  Private Methods:
**
**	checkConn		Check for active connection.
**	closeEvent		Raise a connection closed event.
**	errorEvent		Raise a connection error event.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
**	31-Oct-02 (gordy)
**	    Adapted for generic GCF driver.
**	14-Feb-03 (gordy)
**	    Updated to JDBC 3.0: rollback( savepoint ), setSavepoint(), 
**	    releaseSavepoint(), getHoldability(), setHoldability().
**	20-Jul-06 (gordy)
**	    Change connection to be protected.
**	 4-May-07 (gordy)
**	    Class is exposed outside package, permit access.
*/

public class
JdbcCPVirt
    implements	Connection, GcfErr
{

    /*
    ** Tracing
    */
    protected	DrvTrace		trace = null;
    protected	String			title = null;
    protected	String			tr_id = "CPVirt";
    protected	int			inst_id = 0;

    /*
    ** Connection info
    */
    protected	Connection		conn = null;
    private	PooledConnection	pool = null;
    private	ConnectionEventListener	listener = null;

    /*
    ** Instance counter to generate instance IDs.
    */
    private static int			inst_count = 0;


/*
** Name: JdbcCPVirt
**
** Description:
**	Class constructor.
**
** Input:
**	conn	    JDBC connection.
**	pool	    Pooled connection owner.
**	listener    Connection event listener.
**
** Output:
**	None.
**
**
** Returns:
**	None.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
**	31-Oct-02 (gordy)
**	    Adapted for generic GCF driver.
**	 4-May-07 (gordy)
**	    Class is exposed outside package, restrict constructor access.
*/

// package access
JdbcCPVirt
( 
    Connection		    conn,
    PooledConnection	    pool,
    ConnectionEventListener listener,
    DrvTrace		    trace
)
{
    this.conn = conn;
    this.pool = pool;
    this.listener = listener;
    this.trace = trace;
    inst_id = inst_count++;
    title = trace.getTraceName() + "-CPVConnection[" + inst_id + "]";
    tr_id += "[" + inst_id + "]";
} // JdbcCPVirt


/*
** Name: toString
**
** Description:
**	Returns a string identfier for this instance.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String	    Instance identifier.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public String
toString()
{
    return( title );
} // toString


/*
** Name: isClosed
**
** Description:
**	Determine if the connection is closed.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean	    True if connection closed, false otherwise.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public boolean
isClosed()
    throws SQLException
{
    return( (conn != null) ? conn.isClosed() : true );
} // isClosed


/*
** Name: close
**
** Description:
**	Close the connection.
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
**	29-Jan-01 (gordy)
**	    Created.
*/

public void
close()
    throws SQLException
{
    if ( trace.enabled() )  trace.log( title + ".close()" );
    if ( conn != null )  closeEvent();
    return;
} // close


/*
** Name: getWarnings
**
** Description:
**	Retrieve the connection warning/message list.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	SQLWarning  SQL warning/message list, may be NULL.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public SQLWarning
getWarnings()
    throws SQLException
{
    return( (conn != null) ? conn.getWarnings() : null );
} // getWarnings


/*
** Name: clearWarnings
**
** Description:
**	Clear the connection warning/message list.
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
**	29-Jan-01 (gordy)
**	    Created.
*/

public void
clearWarnings()
    throws SQLException
{
    if ( conn != null )  conn.clearWarnings();
    return;
} // clearWarnings


/*
** Name: createStatement
**
** Description:
**	Creates a JDBC Statement object associated with the
**	connection.
**
** Input:
**	type	    ResultSet type.
**	concurrency ResultSet concurrency.
**	holdability ResultSet holdability
**
** Output:
**	None.
**
** Returns:
**	Statement   A new Statement object.
**
** History:
**	19-Feb-03 (gordy)
**	    Created.
*/

public Statement
createStatement()
    throws SQLException
{
    Statement stmt;
    checkConn( ".createStatement()" );

    try { stmt = conn.createStatement(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // createStatement


public Statement
createStatement( int type, int concurrency )
    throws SQLException
{
    Statement stmt;
    checkConn( ".createStatement()" );

    try { stmt = conn.createStatement( type, concurrency ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // createStatement


public Statement
createStatement( int type, int concurrency, int holdability )
    throws SQLException
{
    Statement stmt;
    checkConn( ".createStatement()" );

    try { stmt = conn.createStatement( type, concurrency, holdability ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // createStatement


/*
** Name: prepareStatement
**
** Description:
**	Creates a JDBC PreparedStatement object associated with the
**	connection.
**
** Input:
**	sql	    Query text.
**	type	    ResultSet type.
**	concurrency ResultSet concurrency.
**	holdability ResultSet holdability
**
** Output:
**	None.
**
** Returns:
**	PreparedStatement   A new PreparedStatement object.
**
** History:
**	19-Feb-03 (gordy)
**	    Created.
*/

public PreparedStatement
prepareStatement( String sql )
    throws SQLException
{
    PreparedStatement stmt;
    checkConn( ".prepareStatement()" );

    try { stmt = conn.prepareStatement( sql ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareStatement


public PreparedStatement
prepareStatement( String sql, int type, int concurrency )
    throws SQLException
{
    PreparedStatement stmt;
    checkConn( ".prepareStatement()" );

    try { stmt = conn.prepareStatement( sql, type, concurrency ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareStatement


public PreparedStatement
prepareStatement( String sql, int type, int concurrency, int holdability )
    throws SQLException
{
    PreparedStatement stmt;
    checkConn( ".prepareStatement()" );

    try { stmt = conn.prepareStatement(sql, type, concurrency, holdability); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareStatement


public PreparedStatement
prepareStatement( String sql, int autoGeneratedKeys )
    throws SQLException
{
    PreparedStatement stmt;
    checkConn( ".prepareStatement()" );

    try { stmt = conn.prepareStatement( sql, autoGeneratedKeys ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareStatement


public PreparedStatement
prepareStatement( String sql, int columnIndexes[] )
    throws SQLException
{
    PreparedStatement stmt;
    checkConn( ".prepareStatement()" );

    try { stmt = conn.prepareStatement( sql, columnIndexes ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareStatement


public PreparedStatement
prepareStatement( String sql, String columnNames[] )
    throws SQLException
{
    PreparedStatement stmt;
    checkConn( ".prepareStatement()" );

    try { stmt = conn.prepareStatement( sql, columnNames ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareStatement


/*
** Name: prepareCall
**
** Description:
**	Creates a JDBC CallableStatement object associated with the
**	connection.
**
** Input:
**	sql	    Execute procedure statement.
**	type	    ResultSet type.
**	concurrency ResultSet concurrency.
**	holdability ResultSet holdability
**
** Output:
**	None.
**
** Returns:
**	CallableStatement   A new CallableStatement object.
**
** History:
**	19-Feb-03 (gordy)
**	    Created.
*/

public CallableStatement 
prepareCall( String sql )
    throws SQLException
{
    CallableStatement stmt;
    checkConn( ".prepareCall()" );

    try { stmt = conn.prepareCall( sql ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareCall


public CallableStatement 
prepareCall( String sql, int type, int concurrency )
    throws SQLException
{
    CallableStatement stmt;
    checkConn( ".prepareCall()" );

    try { stmt = conn.prepareCall( sql, type, concurrency ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareCall


public CallableStatement 
prepareCall( String sql, int type, int concurrency, int holdability )
    throws SQLException
{
    CallableStatement stmt;
    checkConn( ".prepareCall()" );

    try { stmt = conn.prepareCall( sql, type, concurrency, holdability ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( stmt );
} // prepareCall


/*
** Name: getHoldability
**
** Description:
**	Returns the current default ResultSet holdability.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	int	Holdability.
**
** History:
**	19-Feb-03 (gordy)
**	    Created.
*/

public int
getHoldability()
    throws SQLException
{
    int holdability;
    checkConn( ".getHoldability()" );

    try { holdability = conn.getHoldability(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( holdability );
} // getHoldability


/*
** Name: setHoldability
**
** Description:
**	Set the default ResultSet holdability:
**
**	Ingres does not support holdable cursors.
**
** Input:
**	holdability	ResultSet holdability
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	19-Feb-03 (gordy)
**	    Created.
*/

public void
setHoldability( int holdability )
    throws SQLException
{
    checkConn( ".setHoldability()" );

    try { conn.setHoldability( holdability ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // setHoldability


/*
** Name: nativeSQL
**
** Description:
**	Translates the input SQL query and returns the query in
**	native DBMS grammer.
**
** Input:
**	sql	SQL query.
**
** Output:
**	None.
**
** Returns:
**	String	Native DBMS query.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public String
nativeSQL( String sql )
    throws SQLException
{
    checkConn( ".nativeSQL()" );

    try { sql = conn.nativeSQL( sql ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( sql );
} // nativeSQL


/*
** Name: getMetaData
**
** Description:
**	Creates a JDBC DatabaseMetaData object associated with the
**	connection.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	DatabaseMetaData    A new DatabaseMetaData object.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public DatabaseMetaData
getMetaData()
    throws SQLException
{
    DatabaseMetaData metaData;
    checkConn( ".getMetaData()" );

    try { metaData = conn.getMetaData(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( metaData );
} // getMetaData


/*
** Name: getCatalog
**
** Description:
**	Retrieve the DBMS catalog.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String	    The DBMS catalog.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public String
getCatalog()
    throws SQLException
{
    String catalog;
    checkConn( ".getCatalog()" );

    try { catalog = conn.getCatalog(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( catalog );
} // getCatalog


/*
** Name: setCatalog
**
** Description:
**	Set the DBMS catalog.
**
** Input:
**	catalog	    DBMS catalog.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public void
setCatalog( String catalog )
    throws SQLException
{
    checkConn( ".setCatalog()" );

    try { conn.setCatalog( catalog ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // setCatalog


/*
** Name: getTypeMap
**
** Description:
**	Retrieve the type map.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	Map	Type map.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public Map
getTypeMap()
    throws SQLException
{
    Map map;
    checkConn( ".getTypeMap()" );

    try { map = conn.getTypeMap(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( map );
} // getTypeMap


/*
** Name; setTypeMap
**
** Description:
**	Set the type map.
**
** Input:
**	map	Type map.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	29-Jan-01 (gordy)
*/

public void
setTypeMap( Map map )
    throws SQLException
{
    checkConn( ".setTypeMap()" );

    try { conn.setTypeMap( map ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // setTypeMap


/*
** Name: isReadOnly
**
** Description:
**	Determine the readonly state of a connection.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean	    True if connection is readonly, false otherwise.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public boolean
isReadOnly()
    throws SQLException
{
    boolean readOnly;
    checkConn( ".isReadOnly()" );

    try { readOnly = conn.isReadOnly(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( readOnly );
} // isReadOnly


/*
** Name: setReadOnly
**
** Description:
**	Set the readonly state of a connection.
**
** Input:
**	readOnly    True for readonly, false otherwise.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public synchronized void
setReadOnly( boolean readOnly )
    throws SQLException
{
    checkConn( ".setReadOnly()" );

    try { conn.setReadOnly( readOnly ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // setReadOnly


/*
** Name: getTransactionIsolation
**
** Description:
**	Determine the transaction isolation level.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	int	Transaction isolation level.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public int
getTransactionIsolation()
    throws SQLException
{
    int level;
    checkConn( ".getTransactionIsolation()" );

    try { level = conn.getTransactionIsolation(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( level );
} // getTransactionIsolation


/*
** Name: setTransactionIsolation
**
** Description:
**	Set the transaction isolation level.  The JDBC Connection
**	interface defines the following transaction isolation
**	levels:
**
**	    TRANSACTION_NONE
**	    TRANSACTION_READ_UNCOMMITTED
**	    TRANSACTION_READ_COMMITTED
**	    TRANSACTION_REPEATABLE_READ
**	    TRANSACTION_SERIALIZABLE
**
** Input:
**	level	Transaction isolation level.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public synchronized void
setTransactionIsolation( int level )
    throws SQLException
{
    checkConn( ".setTransactionIsolation()" );

    try { conn.setTransactionIsolation( level ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // setTransactionIsolation


/*
** Name: getAutoCommit
**
** Description:
**	Determine the autocommit state.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean	    True if autocommit is ON, false if OFF.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public boolean
getAutoCommit()
    throws SQLException
{
    boolean autoCommit;
    checkConn( ".getAutoCommit()" );

    try { autoCommit = conn.getAutoCommit(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( autoCommit );
} // getAutoCommit


/*
** Name: setAutoCommit
**
** Description:
**	Set the autocommit state.
**
** Input:
**	autocommit  True to set autocommit ON, false for OFF.
**
** Output:
**	None.
**
** Returns:
**	void
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

public void
setAutoCommit( boolean autoCommit )
    throws SQLException
{
    checkConn( ".setAutoCommit()" );

    try { conn.setAutoCommit( autoCommit ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // setAutoCommit


/*
** Name: commit
**
** Description:
**	Commit the current transaction
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
**	29-Jan-01 (gordy)
**	    Created.
*/

public void
commit()
    throws SQLException
{
    checkConn( ".commit()" );

    try { conn.commit(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // commit


/*
** Name: rollback
**
** Description:
**	Rollback the current transaction
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
**	29-Jan-01 (gordy)
**	    Created.
*/

public void
rollback()
    throws SQLException
{
    checkConn( ".rollback()" );

    try { conn.rollback(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // rollback


/*
** Name: rollback
**
** Description:
**	Rollback to savepoint.
**
** Input:
**	savepoint   Target savepoint.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	14-Feb-03 (gordy)
**	    Created.
*/

public void
rollback( Savepoint savepoint )
    throws SQLException
{
    checkConn( ".rollback()" );

    try { conn.rollback( savepoint ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // rollback


/*
** Name: setSavepoint
**
** Description:
**	Creates an unnamed savepoint.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	Savepoint	The unnamed savepoint.
**
** History:
**	14-Feb-03 (gordy)
**	    Created.
*/

public Savepoint
setSavepoint()
    throws SQLException
{
    Savepoint sp;
    checkConn( ".setSavepoint()" );

    try { sp = conn.setSavepoint(); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( sp );
} // setSavepoint


/*
** Name: setSavepoint
**
** Description:
**	Creates a named savepoint.
**
** Input:
**	name	Savepoint name.
**
** Output:
**	None.
**
** Returns:
**	Savepoint	The named savepoint.
**
** History:
**	14-Feb-03 (gordy)
**	    Created.
*/

public Savepoint
setSavepoint( String name )
    throws SQLException
{
    Savepoint sp;
    checkConn( ".setSavepoint()" );

    try { sp = conn.setSavepoint( name ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return( sp );
} // setSavepoint


/*
** Name: releaseSavepoint
**
** Description:
**	Remove savepoint from transaction.  Ingres does not allow
**	savepoints to be deleted, so we just remove the savepoint
**	from our list, effectively making the savepoint unusable.
**
** Input:
**	savepoint   Target savepoint.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	14-Feb-03 (gordy)
**	    Created.
*/

public void
releaseSavepoint( Savepoint savepoint )
    throws SQLException
{
    checkConn( ".releaseSavepoint()" );

    try { conn.releaseSavepoint( savepoint ); }
    catch( SQLException ex )
    {
	if ( conn.isClosed() )  errorEvent( ex );
	throw ex;
    }

    return;
} // releaseSavepoint


/*
** Name: checkConn
**
** Description:
**	Make sure the connection is active.
**
** Input:
**	name	Callers name for tracing.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
*/

protected void
checkConn( String name )
    throws SqlEx
{
    if ( conn == null )
    {
	if ( trace.enabled( 1 ) )
	    trace.write( tr_id + name + ": virtual connection closed!" );
	throw SqlEx.get( ERR_GC4004_CONNECTION_CLOSED );
    }
    return;
} // checkConn


/*
** Name: closeEvent
**
** Description:
**	Raise a connection closed event.
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
**	29-Jan-01 (gordy)
**	    Created.
*/

private void
closeEvent()
{
    if ( trace.enabled( 2 ) )
	trace.write( tr_id + ": closing virtual connection" );

    if ( listener != null )
	listener.connectionClosed( new ConnectionEvent( pool ) );

    listener = null;
    pool = null;
    conn = null;
    return;
} // closeEvent


/*
** Name: errorEvent
**
** Description:
**	Raise a connection error event.
**
** Input:
**	ex	SQLException.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	29-Jan-01 (gordy)
**	    Created.
**	12-Apr-01 (gordy)
**	    Exceptions are now self tracing.
*/

private void
errorEvent( SQLException ex )
{
    if ( trace.enabled() )
    {
	trace.log( title + ": virtual connection error" );
	if ( trace.enabled( 1 ) )  ((SqlEx)ex).trace( trace );
    }

    if ( listener != null )
	listener.connectionErrorOccurred( new ConnectionEvent( pool, ex ) );

    listener = null;
    pool = null;
    conn = null;
    return;
} // errorEvent


} // class JdbcCPVirt
