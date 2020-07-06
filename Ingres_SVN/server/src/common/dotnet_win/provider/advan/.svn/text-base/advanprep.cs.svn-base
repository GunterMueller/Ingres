/*
** Copyright (c) 1999, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.Collections;
using System.IO;
using Ca.Ingres.Utility;

namespace Ca.Ingres.ProviderInternals
{
	/*
	** Name: advanprep.cs
	**
	** Description:
	**	Defines class which implements the PreparedStatement interface.
	**
	**  Classes:
	**
	**	AdvanPrep
	**
	** History:
	**	19-May-99 (gordy)
	**	    Created.
	**	 8-Sep-99 (gordy)
	**	    Created new base class for class which interact with
	**	    the server and extracted common data and methods.
	**	    Synchronize entire request/response with server.
	**	13-Sep-99 (gordy)
	**	    Implemented error code support.
	**	29-Sep-99 (gordy)
	**	    Added classes for BLOB streams and implemented support
	**	    for BLOBs.  Use DbConn lock()/unlock() methods for
	**	    synchronization to support BLOB streams.
	**	26-Oct-99 (gordy)
	**	    Implemented ODBC escape processing.
	**	12-Nov-99 (gordy)
	**	    Use configured date formatter.
	**	16-Nov-99 (gordy)
	**	    Added query timeouts.
	**	18-Nov-99 (gordy)
	**	    Made Param, AscRdr, and UniRdr nested classes.
	**	13-Dec-99 (gordy)
	**	    Added fetch limit.
	**	 2-Feb-00 (gordy)
	**	    Send short streams as VARCHAR or VARBINARY.
	**	 7-Feb-00 (gordy)
	**	    For the NULL datatype or NULL data values, send the generic
	**	    NULL datatype and the nullable indicator.
	**	13-Jun-00 (gordy)
	**	    Added support for derived classes (advanCall specfically).
	**	 4-Oct-00 (gordy)
	**	    Standardized internal tracing.
	**	19-Oct-00 (gordy)
	**	    Changes in transaction state result in prepared statements
	**	    being lost.  The transaction state is now tracked and the
	**	    query re-prepared when the state changes.
	**	 1-Nov-00 (gordy)
	**	    Added support for 2.0 extensions.  Added ChrRdr class.
	**	15-Nov-00 (gordy)
	**	    Extracted Param class to create a stand-alone Paramset class.
	**	 8-Jan-01 (gordy)
	**	    Extracted additional parameter handling to ParamSet.  Moved
	**	    the BLOB processing classes.  Added support for batch processing.
	**	23-Jan-01 (gordy)
	**	    Allocate batch list on first access.
	**	 5-Feb-01 (gordy)
	**	    Coalesce statement IDs to preserve resources in the DBMS.
	**	 3-Mar-01 (gordy)
	**	    Centralize call to clearResults in exec().
	**	11-Apr-01 (gordy)
	**	    Replaced timezone specific date formatters with a single
	**	    date formatter which, when synchronized, may be set to
	**	    whatever timezone is required.  Do not include time with 
	**	    DATE datatypes.
	**	16-Apr-01 (gordy)
	**	    Optimize usage of Result-set Meta-data.
	**	10-May-01 (gordy)
	**	    Added support for UCS2 datatypes.
	**	 1-Jun-01 (gordy)
	**	    Removed conversion of BLOB to fixed length type for short
	**	    lengths.  requires long fixed length to BLOB conversion,
	**	    but not the other way around.  Since BLOBs can only be
	**	    specified explicitly, we now send in format requested.
	**	13-Jun-01 (gordy)
	**	    2.1 spec requires UTF-8 encoding for unicode streams.
	**	20-Jun-01 (gordy)
	**	    Pass cursor name to result-set according to spec.
	**	20-Aug-01 (gordy)
	**	    Support UPDATE concurrency.  Send READONLY flag to server.
	**	15-Feb-02 (gordy)
	**	    Null parameter values should be mapped to a SQL NULL.
	**	20-Feb-02 (gordy)
	**	    Decimal not supported by gateways at proto level 0.
	**	 6-Sep-02 (gordy)
	**	    When describing character parameters, lengths should be
	**	    for format sent to server (which is not necessarily the
	**	    character length of the parameter).
	**	31-Oct-02 (gordy)
	**	    Adapted for generic GCF driver.
	**	19-Feb-03 (gordy)
	**	    Updated to 3.0.
	**	15-Apr-03 (gordy)
	**	    Added connection timezones separate from date formatters.
	**	26-Jun-03 (wansh01)
	**	    Changed send_desc() and send_param() for Type.Binary to handle 
	**	    null value properly. 
	**	 7-Jul-03 (gordy)
	**	    Added Ingres timezone config which affects date/time literals.
	**	 4-Aug-03 (gordy)
	**	    New result-set for updatable cursor.  Simplified pre-fetch 
	**	    calculations.
	**	20-Aug-03 (gordy)
	**	    Add statement AUTO_CLOSE optimization.
	**	12-Sep-03 (gordy)
	**	    Replaced timezones with GMT indicator and SqlDates utility.
	**	 6-Oct-03 (gordy)
	**	    First block of rows may now be fetched with query response.
	**	25-Feb-04 (thoda04)
	**	    Convert SqlEx exception to an IngresException.
	**	15-Mar-04 (gordy)
	**	    Implemented BIGINT support, default to DECIMAL when not supported.
	**	    BOOLEAN now implemented as always using alternate storage format.
	**	12-May-04 (thoda04)
	**	    For blobs, use write(StreamReader), not write(Reader). 	
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	**	13-Oct-05 (thoda04) B115418
	**	    CommandBehavior.SchemaOnly should not call database and
	**	    needs to return an empty RSLT and non-empty RSMD metadata.
	**	    Added getResultSetEmpty() method.
	*/



	/*
	** Name: AdvanPrep
	**
	** Description:
	**	driver class which implements the PreparedStatement 
	**	interface.
	**
	**  Interface Methods:
	**
	**	executeQuery	    Execute a statement with result set.
	**	executeUpdate	    Execute a statement with row count.
	**	execute		    Execute a statement and indicate result.
	**	addBatch	    Add parameters to batch set.
	**	executeBatch	    Execute statement for each parameter set.
	**	close		    Close the statement.
	**	getMetaData	    Retrieve the ResultSet meta-data.
	**	getParameterMetaData	Retrieves parameter meta-data.
	**	clearParameters	    Release parameter resources.
	**	setNull		    Set parameter to NULL.
	**	setBoolean	    Set a boolean parameter value.
	**	setByte		    Set a byte parameter value.
	**	setShort	    Set a short parameter value.
	**	setInt		    Set an int parameter value.
	**	setLong		    Set a long parameter value.
	**	setFloat	    Set a float parameter value.
	**	setDouble	    Set a double parameter value.
	**	setDecimal	    Set a Decimal parameter value.
	**	setString	    Set a String parameter value.
	**	setBytes	    Set a byte array parameter value.
	**	setDate		    Set a Date parameter value.
	**	setTime		    Set a Time parameter value.
	**	setTimestamp	    Set a Timestamp parameter value.
	**	setBinaryStream	    Set a binary stream parameter value.
	**	setAsciiStream	    Set an ASCII stream parameter value.
	**	setUnicodeStream    Set a Unicode stream parameter value.
	**	setCharacterStream  Set a character stream parameter value.
	**	setObject	    Set a generic object parameter value.
	**	setArray	    Set an Array parameter value.
	**	setBlob		    Set a Blob parameter value.
	**	setClob		    Set a Clob parameter value.
	**	setRef		    Set a Ref parameter value.
	**	setURL		    Set a URL parameter value.
	**
	**  Protected Methods:
	**
	**	send_desc	    Send DESC message.
	**	send_params	    Send DATA message.
	**	useAltStorage	    Use ALT storage format for parameter type.
	**	paramMap	    Map extern param index to internal index.
	**
	**  Protected Data:
	**
	**	paramSet		    Parameter info.
	**
	**  Private Methods:
	**
	**	prepare		    Prepare the query.
	**	exec		    Execute a prepared statement.
	**
	**  Private Data:
	**
	**	query_text	    Processed query text.
	**	qry_concur	    Query concurrency.
	**	xactID		    Transaction ID of prepared statement.
	**	stmt_name	    Statement name.
	**	rsmd		    Result set meta-data for statement.
	**	max_char_len	    Max char string length.
	**	max_vchr_len	    Max varchar string length.
	**	max_byte_len	    Max byte string length.
	**	max_vbyt_len	    Max varbyte string length.
	**	empty		    Empty RSMD for non-select statements.
	**
	** History:
	**	19-May-99 (gordy)
	**	    Created.
	**	17-Dec-99 (gordy)
	**	    Added max_vchr_len to adapt to actual DBMS maximum.
	**	13-Jun-00 (gordy)
	**	    Added constructor for derived classes, a descriptor
	**	    flag value for 'set' parameters, overriding methods
	**	    for advanStmt 'execute' methods which aren't supported,
	**	    and support for parameter names and flags.
	**	19-Oct-00 (gordy)
	**	    Added query_text, xactID and extracted prepare() from
	**	    the constructor so that changes in transaction state
	**	    can be tracked and the query re-prepared when necessary.
	**	 1-Nov-00 (gordy)
	**	    Support 2.0 extensions.  Added empty, getMetaData(), 
	**	    addBatch(), setNull(), setDate(), setTime(), setTimestamp(),
	**	    setCharacterStream(), setArray(), setBlob(), setClob(), setRef().
	**	15-Nov-00 (gordy)
	**	    Extracted Param class to create a stand-alone Paramset class.
	**	    Removed ARRAY_INC and param_cnt, setFlags(), setName().  
	**	    Parameters now implemented as ParamSet rather than Param array.
	**	 8-Jan-01 (gordy)
	**	    Extracted additional methods to ParamSet class.  Moved the
	**	    BLOB processing classes.  Default parameter flags now saved
	**	    in ParamSet.  Added addBatch(), executeBatch().
	**	16-Apr-01 (gordy)
	**	    Maintain a single RSMD (reloading as necessary).  
	**	    Don't allocate empty RSMD until needed.
	**	20-Aug-01 (gordy)
	**	    Add qry_concur to hold query text concurrency for execution.
	**	31-Oct-02 (gordy)
	**	    Adapted for generic GCF driver.  Added max_char_len, max_byte_len,
	**	    max_vbyt_len rather than use max_vchr_len for all types.
	**	25-Feb-03 (gordy)
	**	    Added close method to clear parameter set, useAltStorage() to
	**	    determine how parameters are stored in the parameter set,
	**	    getParameterMetaData().
	*/

	class
		AdvanPrep : AdvanStmt

	{

		protected ParamSet	paramSet;

		private String	query_text = null;
		private int		qry_concur = DrvConst.DRV_CRSR_DBMS;
		private int		xactID;
		private String	stmt_name = null;
		private AdvanRSMD	rsmd = null;
		private int		max_char_len = DBMS_COL_MAX;
		private int		max_vchr_len = DBMS_COL_MAX;
		private int		max_byte_len = DBMS_COL_MAX;
		private int		max_vbyt_len = DBMS_COL_MAX;

		/*
		** Empty RSMD for non-select statements.
		*/
//		private static	AdvanRSMD empty = null;
    

		/*
		** Name: AdvanPrep
		**
		** Description:
		**	Class constructor.  Primary constructor for this class
		**	and derived classes which prepare the SQL text.
		**
		** Input:
		**	conn	    Associated connection.
		**	query	    Query text.
		**	rs_type	    Default ResultSet type.
		**	rs_concur   Cursor concurrency.
		**	rs_hold	    Default ResultSet holdability.
		**
		** Output:
		**	None.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 8-Sep-99 (gordy)
		**	    Synchronize on DbConn.
		**	29-Sep-99 (gordy)
		**	    Use DbConn lock()/unlock() methods for
		**	    synchronization to support BLOB streams.
		**	26-Oct-99 (gordy)
		**	    Implemented ODBC escape processing.
		**	12-Nov-99 (gordy)
		**	    Use configured date formatter.
		**	17-Dec-99 (gordy)
		**	    Check DBMS max varch length.
		**	13-Jun-00 (gordy)
		**	    Extracted functionality for derived classes to
		**	    new protected constructor.
		**	 4-Oct-00 (gordy)
		**	    Create unique ID for standardized internal tracing.
		**	19-Oct-00 (gordy)
		**	    Query prepare code extracted to prepare().  Process
		**	    the query text then call new method to prepare query.
		**	 1-Nov-00 (gordy)
		**	    Changed parameters for 2.0 extensions.
		**	31-Oct-01 (gordy)
		**	    Timezone now passed to AdvanSQL.
		**	31-Oct-02 (gordy)
		**	    Adapted for generic GCF driver.
		**	19-Feb-03 (gordy)
		**	    Added ResultSet holdability.
		**	15-Apr-03 (gordy)
		**	    Added connection timezones separate from date formatters.
		**	 7-Jul-03 (gordy)
		**	    Added Ingres timezone config which affects date/time literals.
		**	12-Sep-03 (gordy)
		**	    Replaced timezones with GMT indicator and SqlDates utility.
		*/

		public
			AdvanPrep( DrvConn conn, String query, int rs_type, int rs_concur, int rs_hold ) :
			this( conn, rs_type, rs_concur, rs_hold )
		{
			title = trace.getTraceName() + "-PreparedStatement[" + inst_id + "]";
			tr_id = "Prep[" + inst_id + "]";

			if ( trace.enabled() )  trace.log( title + ": '" + query + "'" );

			try
			{
				SqlParse sql = new SqlParse( query, timeLiteralsInGMT() );
				query_text = sql.parseSQL( parse_escapes );
				qry_concur = sql.getConcurrency();
			}
			catch( SqlEx ex )
			{
				if ( trace.enabled() )  
					trace.log( title + ": error parsing query text" );
				if ( trace.enabled( 1 ) )  ex.trace( trace );
				throw ex;
			}

			prepare();
			return;
		} // AdvanPrep


		/*
		** Name: AdvanPrep
		**
		** Description:
		**	Class constructor.  Common functionality for this class
		**	and derived classes which don't actually prepare the
		**	SQL text.
		**
		** Input:
		**	conn	    Associated connection.
		**	rs_type	    Default ResultSet type.
		**	rs_concur   Cursor concurrency.
		**	rs_hold	    Default ResultSet holdability.
		**
		** Output:
		**	None.
		**
		** History:
		**	13-Jun-00 (gordy)
		**	    Created.
		**	 1-Nov-00 (gordy)
		**	    Changed parameters for 2.0 extensions.
		**	 8-Jan-01 (gordy)
		**	    Set max varchar length in parameter set.
		**	31-Oct-02 (gordy)
		**	    Added max lengths for other column types.
		**	19-Feb-03 (gordy)
		**	    Added ResultSet holdability.  Pass CharSet to ParamSet.
		**	12-Sep-03 (gordy)
		**	    Replaced timezones with GMT indicator and SqlDates utility.
		*/

		protected
			AdvanPrep( DrvConn conn, int rs_type, int rs_concur, int rs_hold ) :
			base( conn, rs_type, rs_concur, rs_hold )
		{

			try
			{
				String value;

				if ( (value = conn.dbCaps.getDbCap( DBMS_DBCAP_MAX_CHR_COL )) != null )  
					max_char_len = Int32.Parse( value );
				if ( (value = conn.dbCaps.getDbCap( DBMS_DBCAP_MAX_VCH_COL )) != null )  
					max_vchr_len = Int32.Parse( value );
				if ( (value = conn.dbCaps.getDbCap( DBMS_DBCAP_MAX_BYT_COL )) != null )  
					max_byte_len = Int32.Parse( value );
				if ( (value = conn.dbCaps.getDbCap( DBMS_DBCAP_MAX_VBY_COL )) != null )  
					max_vbyt_len = Int32.Parse( value );
			}
			catch( Exception ) {}

			paramSet = new ParamSet(conn, timeValuesInGMT(), msg.CharSet );
			return;
		} // AdvanPrep


		/*
		** Name: executeQuery
		**
		** Description:
		**	Execute an SQL prepared statement and return a result set.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	ResultSet   A new ResultSet object.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 8-Jan-01 (gordy)
		**	    Pass parameter set to exec() to support batch processing.
		**	 3-Mar-01 (gordy)
		**	    Centralize call to clearResults in exec().
		*/

		public AdvanRslt
			executeQuery()
		{
			if ( trace.enabled() )  trace.log( title + ".executeQuery()" );

			if ( rsmd == null )  
			{
				if ( trace.enabled() )  
					trace.log( title + ".executeQuery(): statement is not a query" );
				clearResults();
				throw SqlEx.get( ERR_GC4017_NO_RESULT_SET );
			}

			exec( paramSet );
			if ( trace.enabled() )  trace.log(title + ".executeQuery(): " + resultSet);
			return( resultSet );
		} // executeQuery


		/*
		** Name: executeUpdate
		**
		** Description:
		**	Execute an SQL prepared statement and return a row count.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	    Row count or 0.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 8-Jan-01 (gordy)
		**	    Pass parameter set to exec() to support batch processing.
		**	 3-Mar-01 (gordy)
		**	    Centralize call to clearResults in exec().
		*/

		public int
			executeUpdate()
		{
			if ( trace.enabled() )  trace.log( title + ".executeUpdate()" );

			if ( rsmd != null )  
			{
				if ( trace.enabled() )  
					trace.log( title + ".executeUpdate(): statement is a query" );
				clearResults();
				throw SqlEx.get( ERR_GC4018_RESULT_SET_NOT_PERMITTED );
			}

			exec( paramSet );
			int count = rslt_rowcount ? rslt_val_rows : 0;
			if ( trace.enabled() )  trace.log( title + ".executeUpdate(): " + count );
			return( count );
		} // executeUpdate


		/*
		** Name: execute
		**
		** Description:
		**	Execute an SQL prepared statement and return an indication 
		**	as to the type of the result.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	boolean	    True if the result is a ResultSet, false otherwise.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 8-Jan-01 (gordy)
		**	    Pass parameter set to exec() to support batch processing.
		**	 3-Mar-01 (gordy)
		**	    Centralize call to clearResults in exec().
		*/

		public virtual bool
			execute()
		{
			if ( trace.enabled() )  trace.log( title + ".execute()" );

			exec( paramSet );
			bool isRS = (resultSet != null);

			if ( trace.enabled() )  trace.log( title + ".execute(): " + isRS );
			return( isRS );
		} // execute


		/*
		** The following methods override Statement methods which are 
		** not allowed for PreparedStatement objects.
		*/

		protected internal override AdvanRslt
			executeQuery( String sql )
		{
			if ( trace.enabled() )  trace.log(title + ".executeQuery('" + sql + "')");
			throw SqlEx.get( ERR_GC4019_UNSUPPORTED );
		} // executeQuery

		protected internal override int
			executeUpdate( String sql )
		{
			if ( trace.enabled() )  trace.log(title + ".executeUpdate('" + sql + "')");
			throw SqlEx.get( ERR_GC4019_UNSUPPORTED );
		} // executeUpdate

		protected internal override bool 
			execute( String sql )
		{
			if ( trace.enabled() )  trace.log( title + ".execute('" + sql + "' )" );
			throw SqlEx.get( ERR_GC4019_UNSUPPORTED );
		} // execute



		/*
		** Name: addBatch
		**
		** Description:
		**	Add parameters to batch set.  
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
		**	11-Jan-01 (gordy)
		**	    Created.
		**	23-Jan-01 (gordy)
		**	    Allocate batch list on first access.
		**	31-Oct-02 (gordy)
		**	    Super-class now provides newBatch() to initialize batch list.
		*/

		internal virtual void
			addBatch()
		{
			if ( trace.enabled() )  trace.log( title + ".addBatch()" );

			if ( rsmd != null )  
			{
				if ( trace.enabled() )  
					trace.log( title + ".addBatch(): statement is a query" );
				throw SqlEx.get( ERR_GC4018_RESULT_SET_NOT_PERMITTED );
			}
    
			if ( batch == null )  newBatch();
			lock( batch ) { batch.Add( paramSet.clone() ); }
			return;
		} // addBatch


		/*
		** Name: executeBatch
		**
		** Description:
		**	Execute the prepared statement for each parameter set
		**	in the batch.
		**
		**	Synchronizes on the batch list so no other thread
		**	can do batch operations during execution.  The
		**	connection is locked for each individual query in
		**	the batch, so other threads can intermix other
		**	queries during the batch execution.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int []	    Array of update counts for each query.
		**
		** History:
		**	11-Jan-01 (gordy)
		**	    Created.
		**	23-Jan-01 (gordy)
		**	    Allocate batch list on first access.
		**	 3-Mar-01 (gordy)
		**	    Centralize call to clearResults in exec().
		**	31-Oct-02 (gordy)
		**	    Check for NULL batch instead of allocating.
		**	25-Feb-03 (gordy)
		**	    Use SUCCESS_NO_INFO rather than explicit integer literal.
		*/

		protected internal override int []
			executeBatch()
		{
			int[] results;

			if ( trace.enabled() )  trace.log( title + ".executeBatch()" );
			if ( batch == null )  return( new int[ 0 ] );

			lock( batch )
			{
				int count = batch.Count;
				results = new int[ count ];

				/*
				** Execute each individual query in the batch.
				*/
				for( int i = 0; i < count; i++ )
					try
					{
						ParamSet ps = (ParamSet)batch[0];
						batch.RemoveAt(0);
						if ( trace.enabled() )  
							trace.log( title + ".executeBatch[" + i + "] " );

						exec( ps );
						results[ i ] = rslt_rowcount ? rslt_val_rows
							: SUCCESS_NO_INFO;

						if ( trace.enabled() )  
							trace.log( title + ".executeBatch[" + i + "] = " 
								+ results[ i ] );
					}
					catch (SqlEx /* ex */)
					{
						/*
						** Return only the successful query results.
						*/
						int[] temp = new int[i];
						if (i > 0)
							Array.Copy(results, 0, temp, 0, i);
						
						batch.Clear();
						throw;
					}
					catch (Exception /* ex */)
					{
						// Shouldn't happen
						/*
						** Return only the successful query results.
						*/
						int[] temp = new int[i];
						if (i > 0)
							Array.Copy(results, 0, temp, 0, i);
						results = temp;
						break;
					}
	
				batch.Clear();
			}

			return( results );
		} // executeBatch


		/*
		** Name: prepare
		**
		** Description:
		**	Prepares the query text and assigns a unique statement
		**	name and transaction ID.  If the query is a SELECT, a
		**	result-set meta-data object is also produced.
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
		**	19-Oct-00 (gordy)
		**	    Extracted from constructor.
		**	 5-Feb-01 (gordy)
		**	    Coalesce statement IDs.
		**	31-Oct-02 (gordy)
		**	    Adapted for generic GCF driver.  
		*/

		private void
			prepare()
		{
			xactID = conn.getXactID();
			stmt_name = conn.getStmtID( query_text, stmt_prefix );
    
			msg.LockConnection();
			try
			{
				msg.begin( MSG_QUERY );
				msg.write( MSG_QRY_PREP );
				msg.write( MSG_QP_STMT_NAME );
				msg.write( stmt_name );
				msg.write( MSG_QP_QTXT );
				msg.write( query_text );
				msg.done( true );

				rsmd = readResults( true );
			}
			catch( SqlEx ex )
			{
				if ( trace.enabled() )  
					trace.log( title + ": error preparing query" );
				if ( trace.enabled( 1 ) )  ex.trace( trace );
				throw ex;
			}
			finally 
			{ 
				msg.UnlockConnection(); 
			}

			return;
		} // prepare


		/*
		** Name: exec
		**
		** Description:
		**	Execute a prepared statement.
		**
		** Input:
		**	param_set	Parameters
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 8-Sep-99 (gordy)
		**	    Synchronize on DbConn.
		**	29-Sep-99 (gordy)
		**	    Use DbConn lock()/unlock() methods for
		**	    synchronization to support BLOB streams.
		**	15-Nov-99 (gordy)
		**	    Pass max row count and column length to result set.
		**	16-Nov-99 (gordy)
		**	    Added query timeouts.
		**	13-Dec-99 (gordy)
		**	    Added fetch limit.
		**	19-Oct-00 (gordy)
		**	    Re-prepare the statement if transaction state changed.
		**	 8-Jan-01 (gordy)
		**	    Parameter set passed as parameter to support batch processing.
		**	 3-Mar-01 (gordy)
		**	    Centralize call to clearResults in exec().
		**	16-Apr-01 (gordy)
		**	    RSMD may change between prepare and execute (nasty!).  We 
		**	    can't do anything about the application seeing the change, 
		**	    but we must use the execute RSMD to read the result-set.
		**	10-May-01 (gordy)
		**	    Only use execute RSMD if one actually arrives.
		**	20-Jun-01 (gordy)
		**	    Pass cursor name to result-set according to spec:
		**	    NULL if READONLY and not provided by application.
		**	20-Aug-01 (gordy)
		**	    Send READONLY flag to server.  Warn if cursor mode changed
		**	    to READONLY.
		**	31-Oct-02 (gordy)
		**	    Adapted for generic GCF driver. 
		**	25-Feb-03 (gordy)
		**	    Check that all contiguous parameters have been provided
		**	    (we don't know actual dynamic parameter count, so we can't
		**	    tell if sufficient parameters have been provided). 
		**	 4-Aug-03 (gordy)
		**	    Created neew result-set class for updatable cursors.
		**	20-Aug-03 (gordy)
		**	    Send AUTO_CLOSE flag for query statements.
		**	 6-Oct-03 (gordy)
		**	    Fetch first block of rows for read-only cursors.
		*/

		private void
			exec( ParamSet param_set )
		{
			/*
			** All dynamic parameters must be set.  We check that all
			** contiguous parameters have been set, the DBMS will check
			** that sufficient parameters have been provided.
			*/
			int param_cnt = param_set.getCount();
			clearResults();

			for( int param = 0; param < param_cnt; param++ )
				if ( ! param_set.isSet( param ) )
				{
					if ( trace.enabled( 1 ) )
						trace.write( tr_id + ": parameter not set: " + (param + 1) );
					throw SqlEx.get( ERR_GC4020_NO_PARAM );
				}

			/*
			** Is the current driver transaction ID different
			** than the ID obtained when the query was prepared?
			** If so, the prepared statement was lost with the
			** change in transaction state and we need to prepare
			** the query again.
			*/
			if ( xactID != conn.getXactID() )  prepare();

			msg.LockConnection();
			try
			{
				if ( rsmd == null )
				{
					/*
					** Non-query statement (execute).
					*/
					msg.begin( MSG_QUERY );
					msg.write( MSG_QRY_EXS );
					msg.write( MSG_QP_STMT_NAME );
					msg.write( stmt_name );
					msg.done( (param_cnt <= 0) );

					if ( param_cnt > 0 )
					{
						send_desc( param_set );
						send_params( param_set );
					}

					readResults( timeout, true );
				}
				else
				{
					/*
					** Select statement (open a cursor).  Generate a 
					** cursor ID if not provided by the application.
					*/
					int		concurrency = getConcurrency( qry_concur );
					short	flags = 0;
					bool	needEOG = true;
					String	stmt = stmt_name;
					String	cursor = (crsr_name != null) 
						? crsr_name : conn.getUniqueID( crsr_prefix );

					/*
					** Parser removes FOR READONLY clause because it isn't
					** a part of Ingres SELECT syntax.  Tell server that
					** cursor should be READONLY (kludge older protocol
					** by appending clause to statement name).
					*/
					if ( concurrency == DrvConst.DRV_CRSR_READONLY )
						if ( conn.msg_protocol_level < MSG_PROTO_2 )
							stmt += " for readonly";
						else  if ( conn.msg_protocol_level < MSG_PROTO_3 )
							flags = (short)((ushort)flags |
								MSG_QF_READONLY);
						else
						{
							flags = (short)((ushort)flags |
								MSG_QF_READONLY | MSG_QF_FETCH_FIRST);
							needEOG = false;
						}
		    
					if ( conn.msg_protocol_level >= MSG_PROTO_3 )
						flags = (short)((ushort)flags |
							MSG_QF_AUTO_CLOSE);
		    
					msg.begin( MSG_QUERY );
					msg.write( MSG_QRY_OCS );
	    
					if ( flags != 0 )
					{
						msg.write( MSG_QP_FLAGS );
						msg.write( (short)2 );
						msg.write(  flags );
					}
	    
					msg.write( MSG_QP_CRSR_NAME );
					msg.write( cursor );
					msg.write( MSG_QP_STMT_NAME );
					msg.write( stmt );
					msg.done( (param_cnt <= 0) );

					if ( param_cnt > 0 )
					{
						send_desc( param_set );
						send_params( param_set );
					}

					/*
					** Check to see if a new result-set descriptor
					** has been received.  If so, it may be different
					** than the previous descriptor and will be more
					** accurate (save only if present).
					*/
					AdvanRSMD new_rsmd = readResults( timeout, needEOG );
					if ( new_rsmd != null )  rsmd = new_rsmd;

					/*
					** The cursor name is passed to the result-set 
					** for updatable cursors or if provided by the 
					** application (2.1 API spec).
					*/
					if ( (rslt_flags & MSG_RF_READ_ONLY) == 0 )  
					{
						resultSet = new RsltUpd( conn, this, 
							rsmd, rslt_val_stmt, cursor );
						if ( msg.moreMessages() )  readResults( timeout, true );
					}
					else
					{
						if ( rs_concur == DrvConst.DRV_CRSR_UPDATE )
							warnings.Add( SqlEx.get( ERR_GC4016_RS_CHANGED ));

						resultSet = new RsltCurs( conn, this, rsmd, rslt_val_stmt, 
							crsr_name, getPreFetchSize(),
							msg.moreMessages() );
					}
				}
			}
			catch( SqlEx ex )
			{
				if ( trace.enabled() )  
					trace.log( title + ".execute(): error executing query" );
				if ( trace.enabled( 1 ) )  ex.trace( trace );
				throw ex;
			}
			finally 
			{ 
				msg.UnlockConnection(); 
			}

			return;
		} // exec


		/*
		** Name: send_desc
		**
		** Description:
		**	Send a MSG_DESC message for the current set of parameters.
		**
		** Input:
		**	param_set	Parameters.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	17-Dec-99 (gordy)
		**	    Conversion to BLOB now determined by DBMS max varchar len.
		**	 2-Feb-00 (gordy)
		**	    Send short streams as VARCHAR or VARBINARY.
		**	 7-Feb-00 (gordy)
		**	    For the NULL datatype or NULL data values, send the generic
		**	    NULL datatype and the nullable indicator.
		**	13-Jun-00 (gordy)
		**	    Nullable byte converted to flags, parameter names supported
		**	    for database procedures (AdvanCall).
		**	15-Nov-00 (gordy)
		**	    Extracted Param class to create a stand-alone Paramset class.
		**	 8-Jan-01 (gordy)
		**	    Parameter set passed as parameter to support batch processing.
		**	10-May-01 (gordy)
		**	    Now pass DBMS type to support alternate formats (UCS2 for now).
		**	    Character arrays also supported.
		**	 1-Jun-01 (gordy)
		**	    Removed conversion of BLOB to fixed length type for short lengths.
		**      13-Aug-01 (loera01) SIR 105521
		**          Set the MSG_DSC_GTT flag if the parameter is a global temp
		**          table.
		**	20-Feb-02 (gordy)
		**	    Decimal not supported by gateways at proto level 0.  Send as float.
		**	 6-Sep-02 (gordy)
		**	    Use formatted length of CHAR and VARCHAR parameters rather than
		**	    character length.
		**	31-Oct-02 (gordy)
		**	    Adapted for generic GCF driver.
		**	19-Feb-03 (gordy)
		**	    Replaced BIT with BOOLEAN.  Skip unset parameters (default
		**	    procedure parameters).  ALT character parameters now stored
		**	    in communication format.
		**	24-June-03 (wansh01)
		**	    for binary type with 0 length send as varbinary.    
		**	15-Mar-04 (gordy)
		**	    BIGINT now full implemented with same alternate format as DECIMAL
		**	    (type is forced to DECIMAL when not supported).  BOOLEAN should
		**	    always be alternate, but force to alternate just in case.
		*/

		protected void
			send_desc( ParamSet param_set )
		{
			lock(this)
			{
				/*
									   ** Count the number of paramters which are set.
									   */
				int	param_cnt = param_set.getCount();
				int set_cnt = 0;

				for( int param = 0; param < param_cnt; param++ )
					if ( param_set.isSet( param ) )  set_cnt++;

				msg.begin( MSG_DESC );
				msg.write( (short)set_cnt );

				for( int param = 0; param < param_cnt; param++ )
				{
					if ( ! param_set.isSet( param ) )  continue;	// Skip unset params.

					Object	value = param_set.getValue( param );
					ushort	pflags = param_set.getFlags( param );
					 // alt=true if need to convert to char and not use UCS2 storage
					bool	alt = (pflags & ParamSet.PS_FLG_ALT) != 0;

					ProviderType	type = param_set.getType( param );
					short	dbms = 0;
					String	name = param_set.getName( param );
					int 	length = 0;
					byte	prec = 0;
					byte	scale = 0;
					byte	flags = 0;

					switch( type )
					{
						case ProviderType.DBNull :
							dbms = DBMS_TYPE_LONG_TEXT;
							flags |= MSG_DSC_NULL;
							break;

						case ProviderType.Boolean : 
							if ( alt )
							{
								type = ProviderType.TinyInt;
								dbms = DBMS_TYPE_INT;
								length = 1;
							}
							else
							{
								/*
								** BOOLEAN not supported by messaging protocol.
								** Shouldn't reach this point (should always be
								** alt), but just in case: force to alternate.
								*/
								type = ProviderType.TinyInt;
								dbms = DBMS_TYPE_INT;
								length = 1;
							}
							break;
	
						case ProviderType.TinyInt :
							dbms = DBMS_TYPE_INT;
							length = 1;
							break;

						case ProviderType.SmallInt :
							dbms = DBMS_TYPE_INT;
							length = 2;
							break;

						case ProviderType.Integer :
							dbms = DBMS_TYPE_INT;
							length = 4;
							break;

						case ProviderType.Real :
							dbms = DBMS_TYPE_FLOAT;
							length = 4;
							break;

						case ProviderType.Double :
							dbms = DBMS_TYPE_FLOAT;
							length = 8;
							break;

						case ProviderType.BigInt :
							if ( alt )
							{
								type = ProviderType.Double;
								dbms = DBMS_TYPE_FLOAT;
								length = 8;
							}
							else
							{
								dbms = DBMS_TYPE_INT;
								length = 8;
							}
							break;


						case ProviderType.Decimal :
							if ( conn.is_ingres ||
								 conn.db_protocol_level >= DBMS_API_PROTO_1 )
							{
								dbms = DBMS_TYPE_DECIMAL;
								prec = 15;

								if ( value != null )
								{
									Decimal  dec = (Decimal)value;
									String	str = dec.ToString();

									prec = (byte) (str.Length -
										(Math.Sign(dec) < 0?1:0) -
										(Scale(dec) > 0?1:0));
									scale= (byte) Scale(dec);
								}
							}
							else
							{
								type = ProviderType.Double;
								dbms = DBMS_TYPE_FLOAT;
								length = 8;
							}
							break;
	
						case ProviderType.DateTime :
							dbms = DBMS_TYPE_DATE;
							break;

						case ProviderType.Char :
						case ProviderType.NChar :
							type = ProviderType.Char;
							/*
							** Determine the byte length of the string as
							** formatted for transmission to the Server.
							** Nulls are sent as zero length strings.  
							*/
							if ( value == null )
								length = 0;
							else  if ( alt ) // true if need to convert to char
								length = ((byte[])value).Length;
							else
								length = msg.getUCS2ByteLength( (char[])value );

							/*
							** Long character arrays are sent as BLOBs.
							** Zero length arrays must be sent as VARCHAR.
							** UCS2 used when supported unless alternate 
							** format is requested.
							*/
							if ( length > 0  &&  length <= max_char_len )
								dbms = alt ? DBMS_TYPE_CHAR : DBMS_TYPE_NCHAR;
							else  if ( length == 0 )
							{
								type = ProviderType.VarChar;
								dbms = alt ? DBMS_TYPE_VARCHAR : DBMS_TYPE_NVARCHAR;
							}
							else
							{
								type = ProviderType.LongVarChar;
								dbms = alt ? DBMS_TYPE_LONG_CHAR : DBMS_TYPE_LONG_NCHAR;
								length = 0;
							}
							break;

						case ProviderType.VarChar :
						case ProviderType.NVarChar :
							type = ProviderType.VarChar;
							/*
							** Determine the byte length of the string as
							** formatted for transmission to the Server.
							** Nulls are described as zero length strings.  
							*/
							if ( value == null )
								length = 0;
							else  if ( alt )
								length = ((byte[])value).Length;
							else
								length = msg.getUCS2ByteLength( (String)value );

							/*
							** Long strings are sent as BLOBs.  UCS2 used 
							** when supported unless alternate format is 
							** requested.
							*/
							if ( length <= max_vchr_len )
								dbms = alt ? DBMS_TYPE_VARCHAR : DBMS_TYPE_NVARCHAR;
							else
							{
								type = ProviderType.LongVarChar;
								dbms = alt ? DBMS_TYPE_LONG_CHAR : DBMS_TYPE_LONG_NCHAR;
								length = 0;
							}
							break;

						case ProviderType.LongVarChar :
						case ProviderType.LongNVarChar :
							type = ProviderType.LongVarChar;
							/*
							** UCS2 used when supported unless alternate 
							** format is requested.
							*/
							dbms = alt ? DBMS_TYPE_LONG_CHAR : DBMS_TYPE_LONG_NCHAR;
							length = 0;
							break;

						case ProviderType.Binary :
							/*
							** Long arrays are sent as BLOBs.
							*/
							length = (value == null) ? 0 : ((byte[])value).Length;

							if (  length > 0  &&  length <= max_byte_len )
								dbms = DBMS_TYPE_BYTE;
							else  if ( length == 0 ) 
							{
								type = ProviderType.VarBinary;
								dbms = DBMS_TYPE_VARBYTE;
							}
							else	
							{
								type = ProviderType.LongVarBinary;
								dbms = DBMS_TYPE_LONG_BYTE;
								length = 0;
							}
							break;

						case ProviderType.VarBinary :
							/*
							** Long arrays are sent as BLOBs.
							*/
							length = (value == null) ? 0 : ((byte[])value).Length;

							if ( length <= max_vbyt_len )
								dbms = DBMS_TYPE_VARBYTE;
							else
							{
								type = ProviderType.LongVarBinary;
								dbms = DBMS_TYPE_LONG_BYTE;
								length = 0;
							}
							break;

						case ProviderType.LongVarBinary :
							dbms = DBMS_TYPE_LONG_BYTE;
							length = 0;
							break;
					}

					if ( value == null )  flags |= MSG_DSC_NULL;

					/*
					** Support parameter flags which may be used by sub-classes.
					*/
					if ( (pflags & ParamSet.PS_FLG_PROC_PARAM) != 0 )  
						flags |= MSG_DSC_PIN;
	
					if ( (pflags & ParamSet.PS_FLG_PROC_BYREF) != 0 )  
						flags |= MSG_DSC_POUT;

					if ( (pflags & ParamSet.PS_FLG_PROC_GTT) != 0 )  
						flags |= MSG_DSC_GTT;

					msg.write( (short)type );
					msg.write( (short)dbms );
					msg.write( (short)length );
					msg.write( (byte)prec );
					msg.write( (byte)scale );
					msg.write( (byte)flags );
	
					if ( name == null )
						msg.write( (short)0 );	// no name.
					else
						msg.write( name );
				}

				msg.done( false );
				return;
			}
		} // send_desc


		/*
		** Name: send_params
		**
		** Description:
		**	Send a MSG_DATA message for the current set of parameters.
		**
		**	VARCHAR and VARBINARY represent all non-stream
		**	input strings and byte arrays.  Since a single
		**	parameter has fixed length, these are converted
		**	to CHAR and BINARY when sent to the server.  If
		**	their lengths exceed the max for non-BLOBs, they
		**	are converted to LONGVARCHAR or LONGVARBINARY.
		**
		** Input:
		**	param_set	Parameters.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	29-Sep-99 (gordy)
		**	    Implemented support for BLOBs.
		**	12-Nov-99 (gordy)
		**	    Use configured date formatter.
		**	17-Dec-99 (gordy)
		**	    Conversion to BLOB now determined by DBMS max varchar len.
		**	 2-Feb-00 (gordy)
		**	    Send short streams as VARCHAR or VARBINARY.
		**	15-Nov-00 (gordy)
		**	    Extracted Param class to create a stand-alone Paramset class.
		**	 8-Jan-01 (gordy)
		**	    Parameter set passed as parameter to support batch processing.
		**	10-May-01 (gordy)
		**	    Support UCS2 as alternate format for character types.  Char
		**	    arrays fully supported as separate BINARY and VARBINARY types.
		**	 1-Jun-01 (gordy)
		**	    Removed conversion of BLOB to fixed length type for short lengths.
		**	20-Feb-02 (gordy)
		**	    Decimal not supported by gateways at proto level 0.  Send as float.
		**	 6-Sep-02 (gordy)
		**	    Use formatted length of CHAR and VARCHAR parameters rather than
		**	    character length.
		**	31-Oct-02 (gordy)
		**	    Adapted for generic GCF driver.  
		**	19-Feb-03 (gordy)
		**	    Replaced BIT with BOOLEAN.  Skip unset parameters (default
		**	    procedure parameters).  ALT character parameters now stored
		**	    in communication format.
		**	26-Jun-03 (gordy)
		**	    Missed an ALT storage class.
		**	27-June-03 (wansh01)
		**	    Handle binary type with 0 length as varbinary. 	
		**	12-May-04 (thoda04)
		**	    For blobs, use write(StreamReader), not write(Reader). 	
		*/

		protected  void
			send_params( ParamSet param_set )
		{
			lock(this)
			{
				int param_cnt = param_set.getCount();
				msg.begin( MSG_DATA );

				for( int param = 0; param < param_cnt; param++ )
				{
					if ( ! param_set.isSet( param ) )
						continue;	// Skip unset params.

					Object	obj = param_set.getValue( param );
					ProviderType type = param_set.getType( param );
					bool	alt =
						(param_set.getFlags(param) & ParamSet.PS_FLG_ALT) != 0;
					int	length;

					if ( type == ProviderType.DBNull  || obj == null )
					{
						msg.write( (byte)0 );   // NULL
						continue;
					}

					msg.write( (byte)1 );   // Not NULL

					switch( type )
					{
						case ProviderType.Boolean :
							msg.write( (byte)(((bool)obj)? 1 : 0) ); break;
						case ProviderType.TinyInt :
							msg.write( unchecked((Byte)((SByte)obj)) ); break;
						case ProviderType.SmallInt :
							msg.write( ((Int16)obj) );               break;
						case ProviderType.Integer :
							msg.write( ((Int32)obj) );               break;
						case ProviderType.Real :
							msg.write( ((Single)obj) );              break;
						case ProviderType.Double :
							msg.write(((Double)obj) );               break;

#if DATE_AND_TIME_NOT_SUPPORTED_IN_NET
//						case ProviderType.DATE :
//						case ProviderType.TIME :
#endif
						case ProviderType.DateTime :
							msg.write( (String)obj );		break;

						case ProviderType.BigInt :
							if ( alt )
								msg.write( Convert.ToDouble((Int64)obj) );
							else
								msg.write( (long)obj );
							break;

						case ProviderType.Decimal :    
							if ( conn.is_ingres ||
								 conn.db_protocol_level >= DBMS_API_PROTO_1 )
								msg.write( ((Decimal)obj).ToString() );
							else
								msg.write( Convert.ToDouble((Decimal)obj) );
							break;

						case ProviderType.Char :
						case ProviderType.NChar :
							type = ProviderType.Char;
							/*
							** Determine the byte length of the string as
							** formatted for transmission to the Server.
							*/
							if ( alt )
								length = ((byte[])obj).Length;
							else
								length = msg.getUCS2ByteLength( (char[])obj );

							/*
							** Long character arrays are sent as BLOBs.
							** Zero length arrays must be sent as VARCHAR.
							** UCS2 used when supported unless alternate 
							** format is requested.
							*/
							if ( length > 0  &&  length <= max_char_len )
								if ( alt )
									msg.write( (byte[])obj, 0, length );
								else
									msg.writeUCS2( (char[])obj, 0, ((char[])obj).Length );
							else  if ( length == 0 )	// Zero length CHAR sent as VARCHAR
								if ( alt )
									msg.write( (byte[])obj );
								else
									msg.writeUCS2( (char[])obj );
							else  if ( alt )
								msg.write( new ByteArrayInputStream( (byte[])obj ) );
							else
								msg.writeUCS2( new CharArrayReader( (char[])obj ) );
							break;

						case ProviderType.VarChar :
						case ProviderType.NVarChar :
							type = ProviderType.VarChar;
							/*
							** Determine the byte length of the string as
							** formatted for transmission to the Server.
							*/
							if ( alt )
								length = ((byte[])obj).Length;
							else
								length = msg.getUCS2ByteLength( (String)obj );

							/*
							** Long strings are sent as BLOBs.  UCS2 used 
							** when supported unless alternate format is 
							** requested.
							*/
							if ( length <= max_vchr_len )
								if ( alt )
									msg.write( (byte[])obj );
								else
									msg.writeUCS2( (String)obj );
							else  if ( alt )
								msg.write( new ByteArrayInputStream( (byte[])obj ) );
							else
								msg.writeUCS2( new System.IO.StringReader( (String)obj ) );
							break;

						case ProviderType.LongVarChar :
						case ProviderType.LongNVarChar :
							type = ProviderType.LongVarChar;
							if ( alt )
								msg.write( (StreamReader)obj );
							else
								msg.writeUCS2( (StreamReader)obj );
							break;

						case ProviderType.Binary :
							if ( ((byte[])obj).Length > 0  &&  
								((byte[])obj).Length <= max_byte_len )
								msg.write( (byte[])obj, 0, ((byte[])obj).Length );
							else  if ( ((byte[])obj).Length == 0 )
								msg.write( (byte[])obj );
							else
								msg.write( new ByteArrayInputStream( (byte[])obj ) );
							break;

						case ProviderType.VarBinary :
							if ( ((byte[])obj).Length <= max_vbyt_len )	
								msg.write( (byte[])obj );
							else
								msg.write( new ByteArrayInputStream( (byte[])obj ) );
							break;

						case ProviderType.LongVarBinary :
							msg.write( (InputStream)obj );
							break;
					}
				}

				msg.done( true );
				return;
			}
		} // send_params


		/*
		** Name: close
		**
		** Description:
		**	Close a statement and release associated resources.
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
		**	25-Feb-03 (gordy)
		**	    Created.
		*/

		public override void
			close()
		{
			base.close();
			paramSet.clear( true );
			return;
		} // close


		/*
		** Name: getMetaData
		**
		** Description:
		**	Retrieves the ResultSet meta-data describing the ResultSet
		**	returned by the prepared statement.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	ResultSetMetaData   The ResultSet meta-data.
		**
		** History:
		**	 1-Nov-00 (gordy)
		**	    Created.
		**	16-Apr-01 (gordy)
		**	    Allocate empty RSMD only when needed.
		*/

		internal AdvanRSMD
			getMetaData()
		{
			if ( trace.enabled() )  trace.log( title + ".getMetaData(): " + rsmd );
			if ( rsmd == null)
				return new AdvanRSMD( 0, trace );

			return rsmd;
		} // getMetadata


		/*
		** Name: getParameterMetaData
		**
		** Description:
		**	Returns meta-data object describing the input parameters.
		**
		**	Ingres does not support DESCRIBE INPUT.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	ParameterMetaData   Parameter meta-data.
		**
		** History:
		**	19-Feb-03 (gordy)
		**	    Created.
		*/

//		public ParameterMetaData
//			getParameterMetaData()
//		{
//			if ( trace.enabled() )  trace.log( title + ".getParameterMetaData()" );
//			throw SqlEx.get( ERR_GC4019_UNSUPPORTED );
//		} // getParameterMetaData


		/*
		** Name: getResultSetEmpty
		**
		** Description:
		**	Returns an empty result set with metdata (rsmd) filled in.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	RsltData   An empty result set with metdata (rsmd) filled in.
		**
		** History:
		**	13-Oct-05 (thoda04)
		**	    Created.
		*/
		
		internal RsltData getResultSetEmpty()
		{
			RsltData  resultset = new RsltData(conn, getMetaData(), null);
			return resultset;
		}


		/*
		** Name: clearParameters
		**
		** Description:
		**	Clear stored parameter values and free resources.
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
		**	19-May-99 (gordy)
		**	    Created.
		*/

		public void
			clearParameters()
		{
			lock(this)
			{
				if ( trace.enabled() )  trace.log( title + ".clearParameters()" );
				paramSet.clear( false );
				return;
			}
		} // clearParameters


		/*
		** Name: setNull
		**
		** Description:
		**	Set parameter to be a NULL value.  The parameter is assigned
		**	a NULL value of the requested SQL type.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	sqlType		Column SQL type (ProviderType).
		**	typeName	Name of SQL type (ignored).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 1-Nov-00 (gordy)
		**	    Added typeName (ignored) parameter.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		**	19-Feb-03 (gordy)
		**	    Check type for alternate storage format.
		*/

		public void
			setNull( int paramIndex, ProviderType sqlType, String typeName )
		{
			if ( trace.enabled() )  trace.log( title + ".setNull( " + paramIndex + 
										", " + sqlType + ", " + typeName + " )" );

			paramSet.setObject( paramMap( paramIndex ), null, 
				sqlType, 0, useAltStorage( sqlType ) );
			return;
		} // setNull


		/*
		** Name: setBoolean
		**
		** Description:
		**	Set parameter to a boolean value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Parameter value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		**	19-Feb-03 (gordy)
		**	    Replaced BIT with BOOLEAN.  
		*/

		public void
			setBoolean( int paramIndex, bool value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setBoolean( " + paramIndex + ", " + value + " )" );

			paramSet.set(paramMap(paramIndex), ProviderType.Boolean, value, false);
			return;
		} // setBoolean

		/*
		** Name: setByte
		**
		** Description:
		**	Set parameter to a byte value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Byte value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		*/

		public void
			setByte( int paramIndex, byte value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setByte( " + paramIndex + ", " + value + " )" );
    
			paramSet.set( paramMap(paramIndex), ProviderType.TinyInt, value, false );
			return;
		} // setByte

		/*
		** Name: setShort
		**
		** Description:
		**	Set parameter to a short value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Short value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		*/

		public void
			setShort( int paramIndex, short value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setShort( " + paramIndex + ", " + value + " )" );
    
			paramSet.set(paramMap(paramIndex), ProviderType.SmallInt, value, false);
			return;
		} // setShort

		/*
		** Name: setInt
		**
		** Description:
		**	Set parameter to an int value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Int value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		*/

		public void
			setInt( int paramIndex, int value )
		{
			if ( trace.enabled() )  
				trace.log( title + " setInt( " + paramIndex + ", " + value + " )" );

			paramSet.set(paramMap(paramIndex), ProviderType.Integer, value,false);
			return;
		} // setInt

		/*
		** Name: setLong
		**
		** Description:
		**	Set parameter to a long value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Long value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		*/

		public void
			setLong( int paramIndex, long value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setLong( " + paramIndex + ", " + value + " )" );

			paramSet.set( paramMap(paramIndex), ProviderType.BigInt, value, false );
			return;
		} // setLong

		/*
		** Name: setFloat
		**
		** Description:
		**	Set parameter to a float value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		*/

		public void
			setFloat( int paramIndex, float value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setFloat( " + paramIndex + ", " + value + " )" );

			paramSet.set( paramMap(paramIndex), ProviderType.Real, value, false );
			return;
		} // setFloat

		/*
		** Name: setDouble
		**
		** Description:
		**	Set parameter to a double value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Double value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		*/

		public void
			setDouble( int paramIndex, double value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setDouble( " + paramIndex + ", " + value + " )" );

			paramSet.set( paramMap(paramIndex), ProviderType.Double, value, false );
			return;
		} // setDouble

		/*
		** Name: setDecimal
		**
		** Description:
		**	Set parameter to a Decimal value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Decimal value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		*/

		public void
			setDecimal( int paramIndex, Decimal value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setDecimal( " + paramIndex + ", " + value + " )" );

			paramSet.set( paramMap( paramIndex ), ProviderType.Decimal, value, false );
			return;
		} // setDecimal

		/*
		** Name: setString
		**
		** Description:
		**	Set parameter to a String value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		String value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		**	19-Feb-03 (gordy)
		**	    Check for alternate storage format.
		*/

		public void
			setString( int paramIndex, String value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setString( " + paramIndex + ", " + value + " )" );

			if ( useAltStorage( ProviderType.VarChar ) )
				paramSet.setObject(paramMap(paramIndex), value, ProviderType.VarChar, -1, true);
			else
				paramSet.set( paramMap( paramIndex ), ProviderType.VarChar, value, false );
			return;
		} // setString

		/*
		** Name: setBytes
		**
		** Description:
		**	Set parameter to a byte array value.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Byte array value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		*/

		public void
			setBytes( int paramIndex, byte[] value )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setBytes( " + paramIndex + ", " + value + " )" );

			paramSet.set( paramMap( paramIndex ), ProviderType.VarBinary, value, false );
			return;
		} // setBytes


#if DATE_AND_TIME_NOT_SUPPORTED_IN_NET
		/*
		** Name: setDate
		**
		** Description:
		**	Set parameter to a Date value.
		**
		**	If the connection is not using the GMT timezone, the timezone
		**	associated with the calendar parameter (or local timezone) is 
		**	used.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Date value.
		**	cal		Calendar for timezone, may be NULL.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 1-Nov-00 (gordy)
		**	    Convert to string using connection timezone.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		**	11-Apr-01 (gordy)
		**	    Ingres supports (to some extent) dates without time, so do not
		**	    include time with DATE datatypes.  Replaced timezone specific 
		**	    date formatters with a single date formatter which, when 
		**	    synchronized, may be set to whatever timezone is required.
		**	15-Apr-03 (gordy)
		**	    Abstracted date formatters.
		**	12-Sep-03 (gordy)
		**	    Replaced timezones with GMT indicator and SqlDates utility.
		*/

		private void
			setDate( int paramIndex, DateTime value )
		{
			String str = null;

			if ( trace.enabled() )  trace.log( title + ".setDate( " + 
										paramIndex + ", " + value + " )" );
			if ( value != null )
			{
				/*
				** The date is stored in GMT such as to have a 0 time for
				** the local TZ.  Ingres dates are not associated with a
				** TZ.  Gateways add a 0 time to dates which is assumed to
				** be in the client TZ.  The date in the local TZ is used.
				*/
				str = SqlDates.formatDate( value, false );
			}

			paramSet.set( paramMap( paramIndex ), ProviderType.DATE, str, false );
			return;
		} // setDate


		/*
		** Name: setTime
		**
		** Description:
		**	Set parameter to a Time value.
		**
		**	If the connection is not using the GMT timezone, the timezone
		**	associated with the calendar parameter is used.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Time value.
		**	cal		Calendar for timezone, may be null.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 1-Nov-00 (gordy)
		**	    Convert to string using connection timezone.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		**	11-Apr-01 (gordy)
		**	    Replaced timezone specific date formatters with a single
		**	    date formatter which, when synchronized, may be set to
		**	    whatever timezone is required.
		**	15-Apr-03 (gordy)
		**	    Abstracted date formatters.
		**	21-Jul-03 (gordy)
		**	    Apply calendar generically before connection based processing.
		**	12-Sep-03 (gordy)
		**	    Replaced timezones with GMT indicator and SqlDates utility.
		*/

		private void
			setTime( int paramIndex, TimeSpan value )
		{
			String  str = null;

			if ( trace.enabled() )  trace.log( title + ".setTime( " + 
										paramIndex + ", " + value + " )" );
			if ( value != null )
			{
				/*
				** Produce the correct time value for the current connection.
				**
				** Ingres only partially supports time only values and adds
				** the current date to such values.  Spec specifies that the
				** date portion for time values should be set to the date
				** epoch 1970-01-01.  When the current date has a different
				** daylight savings offset than the epoch, a one hour offset
				** can occur because of the different GMT offsets applied by
				** .Net and Ingres.
				*/
				str = SqlDates.formatTimestamp( value, timeValuesInGMT() );
			}

			paramSet.set( paramMap( paramIndex ), ProviderType.TIME, str, false );
			return;
		} // setTime
#endif


#if IsNotNeededForDotNET
		/*
		** Name: setTimestamp
		**
		** Description:
		**	Set parameter to a Timestamp value.
		**
		**	If the connection is not using the GMT timezone, the timezone
		**	associated with the calendar parameter is used.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	value		Timestamp value.
		**	cal		Calendar for timezone, may be NULL.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 1-Nov-00 (gordy)
		**	    Convert to string using connection timezone.
		**	 8-Jan-01 (gordy)
		**	    Set method moved to ParamSet.
		**	11-Apr-01 (gordy)
		**	    Replaced timezone specific date formatters with a single
		**	    date formatter which, when synchronized, may be set to
		**	    whatever timezone is required.
		**	15-Apr-03 (gordy)
		**	    Abstracted date formatters.
		**	21-Jul-03 (gordy)
		**	    Apply calendar generically before connection based processing.
		**	12-Sep-03 (gordy)
		**	    Replaced timezones with GMT indicator and SqlDates utility.
		*/

		private void
			setTimestamp( int paramIndex, DateTime value )
		{
			String  str = null;

			if ( trace.enabled() )  trace.log( title + ".setTimestamp( " + 
										paramIndex + ", " + value + " )" );
			/*
			** Produce the correct timestamp value for the current connection.
			*/
			str = SqlDates.formatTimestamp( value, timeValuesInGMT() );

			paramSet.set( paramMap( paramIndex ), ProviderType.DateTime, str, false );
			return;
		} // setTimestamp
#endif


		/*
		** Name: setBinaryStream
		**
		** Description:
		**	Set parameter to a binary stream.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	stream		Binary stream.
		**	length		Length of stream in bytes.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 2-Feb-00 (gordy)
		**	    Send short streams as VARCHAR or VARBINARY.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Stream classes moved to ParamSet.
		**	 1-Jun-01 (gordy)
		**	    No longer need wrapper class for binary InputStream.
		*/

		public void
			setBinaryStream( int paramIndex, InputStream stream, int length )
		{
			if ( trace.enabled() )  trace.log( title + ".setBinaryStream( " + 
										paramIndex + ", " + length + " )" );

			paramSet.set( paramMap( paramIndex ), ProviderType.LongVarBinary, stream, false );
			return;
		} // setBinaryStream


		/*
		** Name: setAsciiStream
		**
		** Description:
		**	Set parameter to an ASCII character stream.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	stream		ASCII stream.
		**	length		Length of stream in bytes.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 2-Feb-00 (gordy)
		**	    Send short streams as VARCHAR or VARBINARY.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Stream classes moved to ParamSet.
		**	 1-Jun-01 (gordy)
		**	    No longer need wrapper class for Reader, so use
		**	    wrapper class to convert InputStream to Reader.
		**	19-Feb-03 (gordy)
		**	    Check for alternate storage format.
		*/

		public void
			setAsciiStream( int paramIndex, InputStream stream, int length )
		{
			if ( trace.enabled() )  trace.log( title + ".setAsciiStream( " + 
										paramIndex + ", " + length + " )" );

			if ( stream == null )
				paramSet.set( paramMap( paramIndex ), ProviderType.LongVarChar, null, 
					useAltStorage( ProviderType.LongVarChar ) );
			else
				paramSet.set( paramMap( paramIndex ), ProviderType.LongVarChar, 
					new InputStreamReader(stream), useAltStorage(ProviderType.LongVarChar) );
    
			return;
		} // setAsciiStream

		/*
		** Name: setUnicodeStream
		**
		** Description:
		**	Set parameter to a Unicode character stream.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	stream		Unicode stream.
		**	length		Length of stream in bytes.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 2-Feb-00 (gordy)
		**	    Send short streams as VARCHAR or VARBINARY.
		**	13-Jun-00 (gordy)
		**	    Map parameter index.
		**	 8-Jan-01 (gordy)
		**	    Stream classes moved to ParamSet.
		**	13-Jun-01 (gordy)
		**	    2.1 spec requires UTF-8 encoding for unicode streams.
		**	19-Feb-03 (gordy)
		**	    Check for alternate storage format.
		*/

		public void
			setUnicodeStream( int paramIndex, InputStream stream, int length )
		{
			if ( trace.enabled() )  trace.log( title + ".setUnicodeStream( " + 
										paramIndex + ", " + length + " )" );

			if ( stream == null )
				paramSet.set( paramMap( paramIndex ), ProviderType.LongVarChar, null, 
					useAltStorage( ProviderType.LongVarChar ) );
			else
			{
				try
				{
					paramSet.set( paramMap( paramIndex ), ProviderType.LongVarChar, 
						new InputStreamReader( stream, "UTF-8" ), 
						useAltStorage( ProviderType.LongVarChar ) );
				}
				catch( Exception ex )
				{
					throw SqlEx.get( ERR_GC401A_CONVERSION_ERR, ex ); 
				}
			}

			return;
		} // setUnicodeStream


		/*
		** Name: setCharacterStream
		**
		** Description:
		**	Set parameter to a character stream.
		**
		** Input:
		**	paramIndex	Parameter index.
		**	reader		Character stream.
		**	length		Length of stream in bytes.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	 2-Nov-00 (gordy)
		**	    Created.
		**	 8-Jan-01 (gordy)
		**	    Stream classes moved to ParamSet.
		**	 1-Jun-01 (gordy)
		**	    No longer need wrapper class for Reader.
		**	19-Feb-03 (gordy)
		**	    Check for alternate storage format.
		*/

		public void
			setCharacterStream( int paramIndex, Reader reader, int length )
		{
			if ( trace.enabled() )  trace.log( title + ".setCharacterStream( " + 
										paramIndex + ", " + length + " )" );
    
			paramSet.set( paramMap( paramIndex ), ProviderType.LongVarChar, reader, 
				useAltStorage( ProviderType.LongVarChar ) );
			return;
		} // setCharacterStream


		/*
		** Name: setObject
		**
		** Description:
		**	Set parameter to a generic object with the requested
		**	SQL type and scale.  A scale value of -1 indicates that the
		**	scale of the value should be used.
		**
		**	Special handling of certain objects may be requested
		**	by setting sqlType to OTHER.  If the object does not have
		**	a processing alternative, the value will processed as if
		**	setObject() with no SQL type was called (except that the
		**	scale value provided will be used for DECIMAL values).
		**
		** Input:
		**	index		Parameter index returned by paramMap().
		**	value		object.
		**	sqlType		Target SQL type.
		**	scale		Number of digits following decimal point, or -1.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 8-Jan-01 (gordy)
		**	    Set methods moved to ParamSet.
		**	10-May-01 (gordy)
		**	    Alternate datatype formats supported through OTHER.
		**	19-Feb-03 (gordy)
		**	    Check for default alternate storage format.
		*/

		public void
			setObject(
				int paramIndex,
				Object value,
				ProviderType sqlType,
				int scale,
				int size )
		{
			if ( trace.enabled() )  
				trace.log( title + ".setObject( " + 
					paramIndex + ", " + sqlType + ", " + scale + " )" );

			if ( sqlType == ProviderType.Other )
				paramSet.setObject( paramMap( paramIndex ), value, 
					ParamSet.getSqlType( value ), scale, true, size );
			else
				paramSet.setObject( paramMap( paramIndex ), value, 
					sqlType, scale, useAltStorage( sqlType ), size );
			return;
		}


		/*
		** Data save methods which are simple covers 
		** for the primary data save methods.
		*/

		public void 
			setNull( int paramIndex, ProviderType sqlType )
		{ setNull( paramIndex, sqlType, null ); }

		public void 
			setObject( int paramIndex, Object value, ProviderType sqlType )
		{ setObject( paramIndex, value, sqlType, -1, 0 ); }

		public void
			setObject( int paramIndex, Object value )
		{ setObject( paramIndex, value, ParamSet.getSqlType( value ), -1, 0 ); }



		/*
		** Name: useAltStorage
		**
		** Description:
		**	Determine if the alternate storage format should be used
		**	for a given SQL type.
		**
		** Input:
		**	sqlType	    Target SQL type.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	boolean	    TRUE if alternate storage should be used.
		**
		** History:
		**	25-Feb-03 (gordy)
		**	    Created.
		**	15-Mar-04 (gordy)
		**	    Implemented BIGINT support, default to DECIMAL when not supported.
		**	    BOOLEAN now implemented as always using alternate storage format.
		*/

		protected bool
			useAltStorage( ProviderType sqlType )
		{
			switch( sqlType )
			{
				case ProviderType.Boolean :
					/*
					** BOOLEAN always uses alternate storage format TINYINT, 
					** at least until supported by the messaging protocol.
					*/
					return( true );		// TODO: support BOOLEAN

				case ProviderType.BigInt :
					/*
					** BIGINT is supported starting with API protocol level 3.
					** The preferred alternate storage format is DECIMAL, but
					** DECIMAL has it's own alternate format.  BIGINT uses the
					** same alternate format as DECIMAL and is forced to DECIMAL
					** when not supported.
					*/
					if ( conn.db_protocol_level >= DBMS_API_PROTO_3 )
						return( false );
					// utilize DECIMAL alternate logic
					goto case ProviderType.Decimal;

				case ProviderType.Numeric :
				case ProviderType.Decimal :
					/*
					** The messaging protocol indicates that DECIMAL values
					** will be converted to VARCHAR automatically if not 
					** supported by the DBMS.  This works well with Ingres, 
					** (which supports char to numeric coercion), but does
					** not work with gateways.  The alternate storage format
					** is DOUBLE, which is used for non-Ingres servers which
					** don't support DECIMAL.
					*/
					return( ! conn.is_ingres  &&  
						conn.db_protocol_level < DBMS_API_PROTO_1 );

				case ProviderType.Other : return( true );

				case ProviderType.Char :    // always convert to ASCII chars
				case ProviderType.VarChar :
				case ProviderType.LongVarChar :
					return( true );

				case ProviderType.NChar :
				case ProviderType.NVarChar :
				case ProviderType.LongNVarChar :
					return( ! conn.ucs2_supported );
			}

			return( false );
		} // useAltStorage


		/*
		** Name: paramMap
		**
		** Description:
		**	Determines if a given parameter index is valid
		**	and maps the external index to the internal index.
		**	Ensures the internal parameter array is prepared
		**	to hold a parameter for the given index.
		**
		** Input:
		**	index	    External parameter index >= 1.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	    Internal parameter index >= 0.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	13-Jun-00 (gordy)
		**	    Use System method to copy array and Param method
		**	    to reset the parameter values.
		*/

		protected internal virtual int
			paramMap( int index )
		{
			if ( index < 0 )  throw SqlEx.get( ERR_GC4011_INDEX_RANGE );
			return( index );
		} // paramMap


		/*
		** Name: readDesc
		**
		** Description:
		**	Read a query result data descriptor message.  Overrides the
		**	method in AdvanStmt.  Maintains a single RSMD.  Subsequent
		**	descriptor messages are loaded into the existing RSMD.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	AdvanRSMD    Query result data descriptor.
		**
		** History:
		**	16-Apr-01 (gordy)
		**	    Created.
		*/

		protected internal override AdvanRSMD
			readDesc()
		{
			if ( rsmd == null )  
				rsmd = AdvanRSMD.load( conn );
			else
				rsmd.reload( conn );

			return( rsmd );
		} // readDesc


		/*
		** Name: Scale
		**
		** Description:
		**	Get the scale of a decimal number.
		**
		** Input:
		**	decValue	Decimal value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	byte    	scale of the decimal number (0 to 28).
		**
		** History:
		**	16-Sep-02 (thoda04)
		**	    Created.
		*/

		private byte Scale(decimal decValue)
		{
			int[] decBits = System.Decimal.GetBits(decValue);
			//decBits[0:2] = low, middle, and high 32-bits of 96-bit integer.
			//decBits[3] = sign and scale of the decimal number.
			//	bits  0:15 = zero and are unused
			//	bits 16:23 = scale
			//	bits 24:30 = zero and are unused
			//	bit  31    = sign

			return (byte)(decBits[3]>>16);
		}

	} // class AdvanPrep

}  // namespace
