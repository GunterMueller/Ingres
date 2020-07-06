/*
** Copyright (c) 2007 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.jdbc;

/*
** Name: RsltScroll.java
**
** Description:
**	Defines class which implements the JDBC ResultSet interface
**	and supports scrollable cursors.
**
**  Classes
**
**	RsltScroll
**
** History:
**	 6-Apr-07 (gordy)
**	    Created.
**	20-Jul-07 (gordy)
**	    Row class now encapsulates a ResultSet row.
*/

import	java.sql.ResultSet;
import	com.ingres.gcf.util.SqlEx;


/*
** Name: RsltScroll
**
** Description:
**	JDBC driver class which implements the JDBC ResultSet interface
**	and adds support for scrollable cursors.
**
**	Overriding the positioned load() method utilizes scrolling
**	support built into the super classes.
**
**  Overridden Methods:
**
**	load			Load next row-set.
**
** History:
**	 6-Apr-07 (gordy)
**	    Created.
**	20-Jul-07 (gordy)
**	    Super-class now tracks number of rows in result-set
**	    and provides most of the capabilities to do scrolling.
*/

public class
RsltScroll
    extends RsltCurs
{


/*
** Name: RsltScroll
**
** Description:
**	Class constructor.
**
**	Current implementation enforces single row per fetch.
**	Pre-fetching blocks of rows is an intended future
**	enhancement, so pre-fetch size and pre-load request
**	are accepted as parameters.
**
** Input:
**	conn		Associated connection.
**	stmt		Associated statement.
**	rsmd		ResultSet meta-data.
**	stmt_id		Statement ID.
**	cursor		Cursor name.
**	preFetch	Number of rows to pre-fetch.
**	preLoad		Load initial row cache.
**
** Output:
**	None.
**
** Returns:
**	None.
**
** History:
**	 6-Apr-07 (gordy)
**	    Created.
**	20-Jul-07 (gordy)
**	    Pre-fetching (and pre-loading) now supported.
*/

RsltScroll
( 
    DrvConn	conn,
    JdbcStmt	stmt, 
    JdbcRSMD	rsmd, 
    long	stmt_id,
    String	cursor,
    int		preFetch,
    boolean	preLoad
)
    throws SqlEx
{
    /*
    ** Delay pre-load until fully initialized.
    */
    super( conn, stmt, rsmd, stmt_id, cursor, preFetch, false );

    rs_type = ResultSet.TYPE_SCROLL_INSENSITIVE;
    tr_id = "Scroll[" + inst_id + "]";
    if ( preLoad )  preLoad();

} // RsltScroll


/*
** Name: load
**
** Description:
**	Position the result set to a specific row and optionally load 
**	the column values.  
**
** Input:
**	reference	Reference position.
**	offset		Row offset from reference.
**	load		TRUE to load row, FALSE to just position.
**
** Output:
**	None.
**
** Returns:
**	void.
**
** History:
**	 6-Apr-07 (gordy)
**	    Created.
**	20-Jul-07 (gordy)
**	    Super-class now provides loading for scrolling along
**	    with pre-fetch cacheing of rows.
*/

protected void
load( int reference, int offset, boolean load )
    throws SqlEx
{
    if ( trace.enabled( 3 ) )  trace.write( tr_id + ".load(" + reference + 
					    "," + offset + "," + load + ")" );

    /*
    ** Pre-fetching is only done for NEXT operations.
    */
    int rows = (reference == ROW  &&  offset == 1) ? fetchSize() : 1;

    try { load( reference, offset, load ? rows : 0 ); }
    catch( SqlEx ex )
    {
	if ( trace.enabled() )
	    trace.log( title + ": error positioning/loading row" );
	if ( trace.enabled( 1 ) )  ex.trace( trace );
	try { shut(); }  catch( SqlEx ignore ) {}
	throw ex;
    }

    return;
} // load


} // class RsltScroll
