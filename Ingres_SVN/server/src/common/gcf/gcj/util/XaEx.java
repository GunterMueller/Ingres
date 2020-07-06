/*
** Copyright (c) 2006 Ingres Corporation All Rights Reserved.
*/

package com.ingres.gcf.util;

/*
** Name: XaEx.java
**
** Description:
**	Defines the XA exception class.
**
**  Classes:
**
**	XaEx		Extends SqlEx
**
** History:
**	20-Jul-06 (gordy)
**	    Created.
*/


/*
** Name: XaEx
**
** Description:
**	Class which extends SQL exception class to provide
**	XA error support.
**
**  Public Methods:
**
**	trace		Write exception info to trace log.
**
** History:
**	20-Jul-06 (gordy)
**	    Created.
*/

public class
XaEx
    extends SqlEx
{


/*
** Name: XaEx
**
** Description:
**	Class constructor.
**
** Input:
**	err	    Associated error code.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	20-Jul-06 (gordy)
**	    Created.
*/

public
XaEx( int err )
{
    super( null, null, err );
    return;
} // XaEx


/*
** Name: trace
**
** Description:
**	Writes the exception information to the trace log.
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
**	20-Jul-06 (gordy)
**	    Created.
*/

public void
trace( Trace trace )
{
    trace.write( "XA Exception: " + getErrorCode() );
    return;
}


} // class XaEx
