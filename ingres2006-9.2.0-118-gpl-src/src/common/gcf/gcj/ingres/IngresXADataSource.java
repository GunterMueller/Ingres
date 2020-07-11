/*
** Copyright (c) 2001, 2003 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.jdbc;

/*
** Name: IngresXADataSource.java
**
** Description:
**	Defines class which implements the JDBC XADataSource interface.
**
**  Classes
**
**	IngresXADataSource
**
** History:
**	31-Jan-01 (gordy)
**	    Created.
**	31-Oct-02 (gordy)
**	    Implemented as wrapper for new generic JDBC data-source.
**	15-Jul-03 (gordy)
**	    Added runtime configuration.
*/

import	java.io.Serializable;
import	java.sql.SQLException;
import	javax.sql.XADataSource;
import	javax.naming.Reference;
import	javax.naming.Referenceable;
import	javax.naming.NamingException;
import	com.ingres.gcf.jdbc.JdbcXADS;
import	com.ingres.gcf.util.Config;
import	com.ingres.gcf.util.TraceLog;


/*
** Name: IngresXADataSource
**
** Description:
**	The Ingres JDBC XADataSource class.  Extends and customizes the 
**	generic JDBC XADataSource class JdbcXADS.
**
**  Overridden Methods:
**
**	loadConfig		Provide config properties to super-class.
**	loadDriverName		Provide Ingres driver name to super-class.
**	loadProtocolID		Provide Ingres protocol ID to super-class.
**	loadTraceName		Provide Ingres tracing name to super-class.
**	loadTraceLog		Provide tracing log to super-class.
**
**  Interface Methods:
**
**	getReference		Get JNDI Reference to IngresXADataSource object.
**
** History:
**	31-Jan-01 (gordy)
**	    Created.
**	31-Oct-02 (gordy)
**	    Implemented as wrapper for new generic JDBC data-source.
**	15-Jul-03 (gordy)
**	    Replaced setDriverIdentity() with load methods.
*/

public class
IngresXADataSource
    extends JdbcXADS
    implements	XADataSource, Serializable, Referenceable, IngConst
{


/*
** Name: <class initializer>
**
** Description:
**	Initializes tracing.
**
** History:
**	31-Oct-02 (gordy)
**	    Created.
**	15-Jul-03 (gordy)
**	    Added runtime configuration.
*/

static 
{
    /*
    ** Initialize Ingres driver tracing.
    */
    IngConfig.setDriverVers( driverMajorVersion, driverMinorVersion );

} // <class initializer>


/*
** Name: IngresXADataSource
**
** Description:
**	Class constructor.
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
**	31-Jan-01 (gordy)
**	    Created.
**	31-Oct-02 (gordy)
**	    Initialize and configure generic super-class.
**	15-Jul-03 (gordy)
**	    Super-class now loads identity information via load methods.
*/

public
IngresXADataSource()
{
} // IngresXADataSource


/*
** Name: IngresXADataSource
**
** Description:
**	Class constructor used by class object factor IngresXAFactory.
**
** Input:
**	ref	Reference to use for initialization.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	31-Oct-02 (gordy)
**	    Created.
**	15-Jul-03 (gordy)
**	    Don't need to call other constructor.
*/

// package access.
IngresXADataSource( Reference ref ) 
{
    initInstance( ref );	// Restore instance properties.
} // IngresXADataSource


/*
** The following methods implement the abstract methods of the
** super-class to provide driver identity information.
**
**	loadConfig		Configuration properties.
**	loadDriverName		Full name of driver.
**	loadProtocolID		Protocol accepted by driver.
**	loadTraceName		Name of driver for tracing.
**	loadTraceLog		Tracing log.
**
** History:
**	15-Jul-03 (gordy)
**	    Replaced setDriverIdentity() with load methods.
*/

protected Config
loadConfig()
{ return( IngConfig.getConfig() ); }

protected String
loadDriverName()
{ return( ING_DRIVER_NAME ); }

protected String
loadProtocolID()
{ return( ING_PROTOCOL_ID ); }

protected String
loadTraceName()
{ return( ING_TRACE_NAME ); }

protected TraceLog
loadTraceLog()
{ return( IngTrace.getTraceLog() ); }


/*
** Name: getReference
**
** Description:
**	Returns a JNDI Reference to this object and associated 
**	factory class.  Encodes property values in Reference 
**	which may be restored by using the class constructor 
**	that takes a Reference parameter.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	Reference   A JNDI Reference.
**
** History:
**	31-Jan-01 (gordy)
**	    Created.
**	31-Oct-02 (gordy)
**	    Adapted for Ingres data-source.
**	15-Jul-03 (gordy)
**	    Get class name directly from class rather than constant.
*/

public Reference
getReference()
    throws NamingException
{
    Reference	ref = new Reference( this.getClass().getName(),
				     IngresXAFactory.class.getName(), null );
    initReference( ref );
    return( ref );
} // getReference


} // class IngresXADataSource