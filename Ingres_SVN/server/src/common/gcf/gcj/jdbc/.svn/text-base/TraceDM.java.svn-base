/*
** Copyright (c) 2001, 2002 Ingres Corporation All Rights Reserved.
*/

package com.ingres.gcf.jdbc;

/*
** Name: TraceDM.java
**
** Description:
**	Defines class which implements the DrvTrace interface for
**	both internal driver tracing and JDBC DriverManager tracing. 
**
**  Classes
**
**	TraceDM
**
** History:
**       6-May-99 (gordy)
**          Created.
**	 2-May-00 (gordy)
**	    Extracted general tracing to package util.
**	15-Nov-00 (gordy)
**	    Added support for JDBC 2.0 extensions.
**	16-Mar-01 (gordy)
**	    Separated into interface and implementation.
**	31-Oct-02 (gordy)
**	    Adapted for generic GCF driver.
*/

import	java.sql.DriverManager;
import	com.ingres.gcf.util.Tracing;
import	com.ingres.gcf.util.TraceLog;


/*
** Name: TraceDM
**
** Description:
**	Implements DrvTrace interface to support both internal 
**	driver tracing and JDBC DriverManager tracing.
**
**  Interface Methods:
**
**	getTraceName	Returns tracing instance name.
**	enabled		Is tracing enabled?
**	log		Log message to all trace logs.
**
**  Private Data:
**
**	name		Tracing instance name.
**	dm_20_interface	Is the JDBC 2.0 DriverManager interface available.
**
**  Private Methods:
**
**	dm_enabled	Is DriverManager logging enabled.
**
** History:
**	 2-May-00 (gordy)
**	    Created.
**	 2-May-00 (gordy)
**	    Extracted general tracing to package util.
**	15-Nov-00 (gordy)
**	    Support JDBC 2.0 extensions.  Added static initializer, 
**	    dm_20_interface, and dm_enabled() to handle the new 
**	    DriverManager logging interface and deprecated 1.2 
**	    interface.
**	16-Mar-01 (gordy)
**	    Separate interface and implementation.
**	31-Oct-02 (gordy)
**	    Added name for tracing ID instead of interface constant,
**	    getTraceName() to return tracing ID.
*/

class
TraceDM
    extends Tracing
    implements DrvTrace
{

    private String		name = "JDBC";	// Tracing instance name

    /*
    ** Indicates if the DriverManager 2.0 interface is supported.
    */
    private static boolean	dm_20_interface = false;


/*
** Name: <class initializer>
**
** Description:
**	Determine if the JDBC 2.0 DriverManager logging interface
**	is available since the 1.2 interface has been deprecated.
**
** History:
**	15-Nov-00 (gordy)
**	    Created.
*/

static // Class Initializer
{
    try
    {
	DriverManager.getLogWriter();
	dm_20_interface = true;
    }
    catch( Throwable ignore ) {}
} // static


/*
** Name: TraceDM
**
** Description:
**	Class constructor.  Set tracing level based on tracing ID.
**
** Input:
**	log	Internal trace log.
**	name	Tracing instance name.
**	id	Tracing ID.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	 2-May-00 (gordy)
**	    Created.
**	31-Oct-02 (gordy)
**	    Added trace log and name for configuration by driver
**	    implementation wrappers.
*/

public
TraceDM( TraceLog log, String name, String id )
{
    super( log, id );
    this.name = name;
    return;
} // TraceDM


/*
** Name: getTraceName
**
** Description:
**	Returns a string which describes this tracing instance.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	String	Tracing name.
**
** History:
**	31-Oct-02 (gordy)
**	    Created.
*/

public String 
getTraceName()
{
    return( name );
} // getTraceName


/*
** Name: enabled
**
** Description:
**	Returns True if DriverManager tracing is active or internal 
**	driver tracing is enabled (at any level)
**	.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean	    True if tracing enabled.
**
** History:
**	 2-May-00 (gordy)
**	    Created.
**	31-Oct-02 (gordy)
**	    Super-class no longer implements driver multi-log methods.
*/

public boolean
enabled()
{
    return( super.enabled( 1 )  ||  dm_enabled() );
} // enabled


/*
** Name: log
**
** Description:
**	Writes trace output to both the JDBC DriverManager log
**	(if active) and internal driver trace log.
**
** Input:
**	str	Trace message.
**
** Output:
**	None.
**
** Returns:
**	void
**
** History:
**	 2-May-00 (gordy)
**	    Created.
**	15-Nov-00 (gordy)
**	    Call driver manager unconditionally to avoid problems
**	    with deprecated interface.
**	31-Oct-02 (gordy)
**	    Super-class no longer implements driver multi-log methods.
*/

public void
log( String str )
{
    DriverManager.println( str );
    write( str );
    return;
} // log


/*
** Name: dm_enabled
**
** Description:
**	Determine if DriverManager logging is enabled.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	boolean	    TRUE if enabled, FALSE otherwise.
**
** History:
**	15-Nov-00 (gordy)
**	    Created.
*/

private boolean
dm_enabled()
{
    boolean enabled = false;

    try
    {
	if ( dm_20_interface )
	    enabled = (DriverManager.getLogWriter() != null);
	else
	    enabled = (DriverManager.getLogStream() != null);
    }
    catch( Exception ignore ) {}

    return( enabled );
}


} // class TraceDM
