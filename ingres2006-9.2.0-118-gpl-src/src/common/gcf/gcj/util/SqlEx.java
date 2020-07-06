/*
** Copyright (c) 1999, 2002 Ingres Corporation All Rights Reserved.
*/

package com.ingres.gcf.util;

/*
** Name: SqlEx.java
**
** Description:
**	Defines the SQL exception class and an error information 
**	class used to define the errors associated with exceptions.
**
**  Classes:
**
**	SqlEx		Extends java.sql.SQLException
**	ErrInfo
**
** History:
**	10-Sep-99 (gordy)
**	    Created.
**	18-Nov-99 (gordy)
**	    Made ErrInfo a nested class.
**	20-Apr-00 (gordy)
**	    Moved to package util.  Converted from interface to class.
**	12-Apr-01 (gordy)
**	    Tracing changes require tracing to be passed as parameter.
**	11-Sep-02 (gordy)
**	    Moved to GCF.  Renamed to remove specific product reference.
**	    Extracted SQLWarning methods to SqlWarn class.
*/

import java.sql.SQLException;


/*
** Name: SqlEx
**
** Description:
**	Class which extends standard SQL exception class to provide
**	construction from resource references and other useful sources.
**
**  Public Methods:
**
**	get		Factory methods for generating SqlEx exceptions.
**	trace		Write exception info to trace log.
**
** History:
**	10-Sep-99 (gordy)
**	    Created.
**	18-Nov-99 (gordy)
**	    Made ErrInfo a nested class.
**	20-Apr-00 (gordy)
**	    Converted from interface to class, added constructors,
**	    and implemented factory methods get().
**	11-Sep-02 (gordy)
**	    Renamed to remove specific product reference.
**	    Extracted SQLWarning methods to SqlWarn class.
*/

public class
SqlEx
    extends SQLException
{


/*
** Name: get
**
** Description:
**	Generate an SqlEx exception from an ErrInfo constant.
**
** Input:
**	err	    ErrInfo constant.
**
** Output:
**	None.
**
** Returns:
**	SqlEx	    The new exception object.
**
** History:
**	20-Apr-00 (gordy)
**	    Created.
**	11-Sep-02 (gordy)
**	    Renamed class to remove specific product reference.
*/

public static SqlEx
get( ErrInfo err )
{
    String msg;
    
    try { msg = ErrRsrc.getResource().getString( err.id ); }
    catch( Exception ignore ){ msg = err.name; }

    return( new SqlEx( msg, err.sqlState, err.code ) );
} // get


/*
** Name: get
**
** Description:
**	Generate an SqlEx exception from an ErrInfo constant
**	and another SqlEx exception.  The existing SqlEx
**	exception is chained to the SqlEx exception created
**	from the error information.
**
** Input:
**	err	    ErrInfo constant.
**	ex	    Associated exception,
**
** Output:
**	None.
**
** Returns:
**	SqlEx	    The new exception object.
**
** History:
**	20-Apr-00 (gordy)
**	    Created.
**	11-Sep-02 (gordy)
**	    Renamed class to remove specific product reference.
*/

public static SqlEx
get( ErrInfo err, SqlEx ex )
{
    SqlEx ex_list = get( err );
    ex_list.setNextException( ex );
    return( ex_list );
} // get


/*
** Name: get
**
** Description:
**	Generate an SqlEx exception from an ErrInfo constant
**	and a general exception.  The general exception is
**	converted into an SqlEx exception and chained to the
**	SqlEx exception created from the error information.
**
** Input:
**	err	    ErrInfo constant
**	ex	    Associated exception.
**
** Ouptut:
**	None.
**
** Returns:
**	SqlEx	    The new exception object.
**
** History:
**	20-Apr-00 (gordy)
**	    Created.
**	11-Sep-02 (gordy)
**	    Renamed class to remove specific product reference.
*/

public static SqlEx
get( ErrInfo err, Exception ex )
{
    SqlEx   sqlEx;
    String  msg = ex.getMessage();
    
    if ( msg == null )  msg = ex.toString();
    sqlEx = (msg == null) ? get( err ) : get( err, new SqlEx( msg ) );
    
    return( sqlEx );
} // get


/*
** Name: SqlEx
**
** Description:
**	Class constructor.  SQL State is defaulted to NULL,
**	error code is defaulted to 0.
**
** Input:
**	msg	    Detailed message for exception.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	20-Apr-00 (gordy)
**	    Created.
**	11-Sep-02 (gordy)
**	    Renamed to remove specific product reference.
*/

public
SqlEx( String msg )
{
    super( msg );
    return;
} // SqlEx


/*
** Name: SqlEx
**
** Description:
**	Class constructor.
**
** Input:
**	msg	    Detailed message for exception.
**	sqlState    Associated SQL State.
**	err	    Associated error code.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	20-Apr-00 (gordy)
**	    Created.
**	11-Sep-02 (gordy)
**	    Renamed to remove specific product reference.
*/

public
SqlEx( String msg, String sqlState, int err )
{
    super( msg, sqlState, err );
    return;
} // SqlEx


/*
** Name: trace
**
** Description:
**	Writes the exception information to the trace log.
**	Any chained exceptions are also traced.
**
** Input:
**	trace	Tracing output.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	20-Apr-00 (gordy)
**	    Created.
**	12-Apr-01 (gordy)
**	    Tracing changes require tracing to be passed as parameter.
**	11-Sep-02 (gordy)
**	    Renamed class to remove specific product reference.
*/

public void
trace( Trace trace )
{
    for( SqlEx ex = this; ex != null; ex = (SqlEx)ex.getNextException() )
    {
	trace.write( "Exception: " + ex.getSQLState() + ", 0x" + 
		     Integer.toHexString( ex.getErrorCode() ) );
	trace.write( "  Message: " + ex.getMessage() );
    }
    return;
}


/*
** Name: ErrInfo
**
** Description:
**	Class containing information for a single error code.
**
**  Public Data
**
**	code	    Numeric error code.
**	sqlState    Associated SQL State.
**	id	    Resource key.
**	name	    Full name.
**
** History:
**	10-Sep-99 (gordy)
**	    Created.
*/

public static class
ErrInfo
{
    public  int	    code;
    public  String  sqlState;
    public  String  id;
    public  String  name;

/*
** Name: ErrInfo
**
** Description:
**	Class constructor.
**
** Input:
**	code	    Error code.
**	sqlState    SQL State.
**	id	    Resource key.
**	name	    Error name.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	10-Sep-99 (gordy)
**	    Created.
*/

public
ErrInfo( int code, String sqlState, String id, String name )
{
    this.code = code;
    this.sqlState = sqlState;
    this.id = id;
    this.name = name;
} // ErrInfo


} // class ErrInfo

} // class SqlEx
