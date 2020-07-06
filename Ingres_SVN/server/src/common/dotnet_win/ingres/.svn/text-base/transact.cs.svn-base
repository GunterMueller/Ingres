/*
** Copyright (c) 2002, 2006 Ingres Corporation. All Rights Reserved.
*/

	/*
	** Name: transact.cs
	**
	** Description:
	**	Describes a local transaction to the database.
	**
	** Classes:
	**	IngresTransaction.
	**
	** History:
	**	27-Aug-02 (thoda04)
	**	    Created.
	**	27-Feb-04 (thoda04)
	**	    Added additional XML Intellisense comments.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	*/

using System;
using System.Data;
using Ca.Ingres.Utility;

namespace Ca.Ingres.Client
{
/// <summary>
/// Represents a local transaction to be made in an Ingres database.
/// </summary>
//	Allow this type to be visible to COM.
[System.Runtime.InteropServices.ComVisible(true)]
public sealed class IngresTransaction : MarshalByRefObject,
                                      IDbTransaction,
                                      IDisposable
{
	internal IngresTransaction(IngresConnection conn):
		this(conn, IsolationLevel.ReadCommitted)
	{
	}

	internal IngresTransaction(IngresConnection conn, IsolationLevel level)
	{
		_connection     = conn;
		_isolationLevel = level;

		switch(level)
		{
			case IsolationLevel.ReadUncommitted: 
			case IsolationLevel.ReadCommitted: 
			case IsolationLevel.RepeatableRead: 
			case IsolationLevel.Serializable: 
				break;
			default: 
				throw new NotSupportedException(level.ToString());
		}
	}

	/// <summary>
	/// Finalizer for the IngresTransaction object.
	/// </summary>
	~IngresTransaction() // Finalize code
	{
		Dispose(false);
	}

	/// <summary>
	/// Release allocated resources of the Transaction.
	/// </summary>
	public void Dispose()
	{
		Dispose(true);
		GC.SuppressFinalize(this);  // Take us off Finalization queue to prevent
		                            // executing finalization a second time
	}

	private void Dispose(bool disposing)
	{
		/*if disposing == true  then method called by user code
		  if disposing == false then method called by runtime from inside the
				finalizer and we should not reference other objects. */
		if (disposing)
		{
			_connection     = null;
		}
	}

	/*
	** PROPERTIES
	*/

	private IngresConnection _connection;
	/// <summary>
	/// Returns the Connection associated with the Transaction.
	/// </summary>
	public  IDbConnection       Connection
	{
		get { return _connection; }  // Connection object associated w/ txn
	}

	private IsolationLevel _isolationLevel;  // = IsolationLevel.ReadCommitted;
	/// <summary>
	/// Returns the current isolation level associated with the Transaction.
	/// </summary>
	public  IsolationLevel  IsolationLevel
	{
		get { return _isolationLevel; }  // current transaction isolation level
	}

	private  bool _hasAlreadyBeenCommittedOrRolledBack = false;
	internal bool  HasAlreadyBeenCommittedOrRolledBack
	{
		get { return _hasAlreadyBeenCommittedOrRolledBack; }
		set { _hasAlreadyBeenCommittedOrRolledBack = value; }
	}

	/*
	** METHODS
	*/

	/// <summary>
	/// Commits the transaction and removes the transaction from the Connection.
	/// </summary>
	public void Commit()
	{
		// Connection must be already open
		if (_connection == null || _connection.State != ConnectionState.Open)
			throw new InvalidOperationException(
				"The connection is not open.");

		if (HasAlreadyBeenCommittedOrRolledBack)
			throw new InvalidOperationException(
				"The transaction has already been committed or rolled back.");

			try
			{
				_connection.advanConnect.commit();  // Commit
				_connection.Transaction = null;     // Connection has no tx now
			}
			catch( SqlEx ex )  { throw ex.createProviderException(); }

		// invalidate the Transaction object to avoid reusing
		HasAlreadyBeenCommittedOrRolledBack = true;
	}

	/// <summary>
	/// Rolls back the transaction and removes the
	/// transaction from the Connection.
	/// </summary>
	public void Rollback()
	{
		// Connection must be already open
		if (_connection == null ||
			_connection.State != ConnectionState.Open)
				throw new InvalidOperationException(
					"The connection is not open.");

		if (HasAlreadyBeenCommittedOrRolledBack)
			throw new InvalidOperationException(
				"The transaction has already been committed or rolled back.");

			try
			{
				_connection.advanConnect.rollback(); // Rollback
				_connection.Transaction = null;     // Connection has no tx now
			}
			catch( SqlEx ex )  { throw ex.createProviderException(); }

		// invalidate the Transaction object to avoid reusing
		HasAlreadyBeenCommittedOrRolledBack = true;
	}

}  // class
}  // namespace
