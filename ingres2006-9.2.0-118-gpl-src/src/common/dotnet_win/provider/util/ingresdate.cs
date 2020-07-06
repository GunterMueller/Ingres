/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.IO;
using System.Text;

namespace Ca.Ingres.Utility
{

	/*
	** Name: ingresdate.cs
	**
	** Description:
	**	Defines class which represents an Ingres Date value.
	**
	**  Classes:
	**
	**	IngresDate	An Ingres Date value.
	**
	** History:
	**	12-Sep-03 (gordy)
	**	    Created.
	**	 9-Dec-03 (thoda04)
	**	    Ported to .NET.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	*/


	/*
	** Name: IngresDate
	**
	** Description:
	**	Class which represents an Ingres Date value.  Ingres Date
	**	values combine aspects of SQL Date, Time, and Timestamp
	**	values while also supporting empty dates and intervals.
	**
	**	Supports conversion to String, Date, Time, Timestamp, and 
	**	Object.  
	**
	**	The data value accessor methods do not check for the NULL
	**	condition.  The super-class isNull() method should first
	**	be checked prior to calling any of the accessor methods.
	**
	**	Ingres dates are truncatable.  Both empty dates and date
	**	only values are considered truncated.  A truncated Ingres
	**	date produces date/time epoch values where applicable.
	**
	**  Public Methods:
	**
	**	set			Assign a new non-NULL data value.
	**	isInterval		Data value is an interval?
	**	isTruncated		Data value is truncated?
	**	getDataSize		Expected size of data value.
	**	getTruncSize		Actual size of truncated data value.
	**	getString		Data value accessor methods
	**	getDate
	**	getTime
	**	getTimestamp
	**	getObject
	**
	**  Protected Data:
	**
	**	value			Date value.
	**	use_gmt			Use GMT for DBMS date/time values.
	**	interval		Is value an interval (valid after getXXX()).
	**
	** History:
	**	12-Sep-03 (gordy)
	**	    Created.
	*/

	internal class
		IngresDate : SqlData
	{

		private String		value = null;
		private bool  		use_gmt = false;
		private bool  		interval = false;
	
    
		/*
		** Name: IngresDate
		**
		** Description:
		**	Class constructor.  Data value is initially NULL.
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
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public
			IngresDate( bool use_gmt ) : base( true )
		{
			this.use_gmt = use_gmt;
		} // IngresDate


		/*
		** Name: set
		**
		** Description:
		**	Assign a new data value.  If the input is NULL,
		**	a NULL data value results.
		**
		** Input:
		**	value		The new data value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public void
			set( String value )
		{
			if ( value == null )
				setNull();
			else  
			{
				setNotNull();
				this.value = value;
			}

			interval = false;
			return;
		} // set


		/*
		** Name: isInterval
		**
		** Description:
		**	Returns an indication that the data value is an interval.
		**
		**	Note that since intervals may not be detected until there
		**	is an attempt to parse the data value, this method should
		**	only be considered accurate after calling a getXXX() method.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	booolean	True if known to be interval, False otherwise.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public bool
			isInterval()
		{
			return( ! isNull()  &&  interval );
		} // isInterval


		/*
		** Name: isTruncated
		**
		** Description:
		**	Returns an indication that the data value was truncated.
		**
		**	Note, we don't want to indicate truncation for intervals,
		**	but the interval indicator is not valid until after a
		**	call to a getXXX() method has been made.  Also, intervals
		**	may be longer than timestamps, so the truncated length
		**	may actually be longer than the expected length.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	boolean		True if truncated.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override bool
			isTruncated()
		{
			return( ! isNull()  &&  ! interval  && 
				value.Length != SqlDates.TS_FMT.Length );
		} // isTruncated


		/*
		** Name: getDataSize
		**
		** Description:
		**	Returns the expected size of an untruncated data value.
		**	For datatypes whose size is unknown or varies, -1 is returned.
		**	The returned value is only valid for truncated data values
		**	(isTruncated() returns True).
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int    Expected size of data value.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override int
			getDataSize()
		{
			return( SqlDates.TS_FMT.Length );
		} // getDataSize


		/*
		** Name: getTruncSize
		**
		** Description:
		**	Returns the actual size of a truncated data value.  For 
		**	datatypes whose size is unknown or varies, -1 is returned.
		**	The returned value is only valid for truncated data values
		**	(isTruncated() returns True).
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int    Truncated size of data value.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override int
			getTruncSize()
		{
			return( value.Length );
		} // getTruncSize



		/*
		** Name: getString
		**
		** Description:
		**	Return the current data value as a String value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Current value converted to String.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override String 
			getString() 
		{
			String str;
    
			/*
			** Ingres dates are overloaded with 'empty' date,
			** date only, timestamp and interval values.  The
			** raw data string is returned for empty dates and
			** intervals.  Date only values and timestamps are
			** parsed/formatted to validate and set timezone.  
			** Intervals will cause an exception if an attempt 
			** is made to parse the value or will be detected
			** by a mis-match in expected string lengths.
			*/
			try
			{
				if ( value.Length == 0 )				// Empty date
				{
					/*
					** Return the empty date string.
					*/
					str = value;
				}
				else  if ( value.Length == SqlDates.D_FMT.Length )	// Date only
				{
					/*
					** Do conversion to check for valid format (in
					** case this is an interval).  Ingres dates are
					** indepedent of timezone, so use local TZ.
					*/
					DateTime dt = SqlDates.parseDate( value, false );
					str = SqlDates.formatDate( dt, false );
				}
				else  if ( value.Length == SqlDates.TS_FMT.Length )	// Timestamp
				{
					/*
					** Convert to GMT using TZ for current connection
					** and then to local time using local TZ.
					*/
					DateTime ts = SqlDates.parseTimestamp( value, use_gmt );
					str = SqlDates.formatTimestamp( ts, false );
				}
				else							// Interval
				{
					/*
					** Return the interval string and produce a warning.
					*/
					interval = true;
					str = value;
				}
			}
			catch( SqlEx )
			{
				/*
				** Any parsing error is assumed to be caused by an interval.
				*/
				interval = true;
				str = value;
			}
    
			return( str );
		} // getString


		/*
		** Name: getDate
		**
		** Description:
		**	Return the current data value as a Date value.
		**	A relative timezone may be provided which is
		**	applied to adjust the final result such that it
		**	represents the original date/time value in the
		**	timezone provided.  The default is to use the
		**	local timezone.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	tz	Relative timezone, NULL for local.
		**	
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Date	Current value converted to Date.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public override DateTime 
			getDate( TimeZone tz ) 
		{ 
			/*
			** Ingres dates are overloaded with 'empty' date,
			** date only, timestamp and interval values.  The
			** first three types are handled explicitly below.
			** Intervals will either cause an exception while
			** attempting to parse the value or as the default
			** action for an unrecognized format.
			*/
			try
			{
				if ( value.Length == 0 )  				// Empty date
				{
					/*
					** Ingres permits zero length date literals or 'empty' 
					** dates.  Since does not have any corresponding 
					** date/time concept, we use the date epoch.  If
					** no timezone is provided, we can return the local
					** epoch constant.  Otherwise, the epoch value for the
					** requested timezone must be generated.
					*/
					return( (tz == null) ? SqlDates.getEpochDate()
						: SqlDates.parseDate(SqlDates.D_EPOCH, tz) );
				}
				else  if ( value.Length == SqlDates.D_FMT.Length )	// Date only
				{
					/*
					** The date is stored in GMT such as to have a 0 time for
					** the target TZ (requested/local).
					*/
					return( (tz == null ) ? SqlDates.parseDate( value, false )
						: SqlDates.parseDate( value, tz ) );
				}
				else  if ( value.Length == SqlDates.TS_FMT.Length )	// Timestamp
				{
					/*
					** Remove the time component but retain correct date:
					**
					** 1.  Convert to GMT Timestamp using TZ for current connection.
					** 2.  Format as date only using local TZ to get local date.  
					** 3.  Generate Date value using requested/local timezone.
					*/
					DateTime date = SqlDates.parseTimestamp( value, use_gmt );
					String str = SqlDates.formatDate( date, false );
					return( (tz == null ) ? SqlDates.parseDate( str, false )
						: SqlDates.parseDate( str, tz ) );
				}
				else							// Interval
				{
					/*
					** Can't support intervals with Date objects.
					*/
					throw SqlEx.get( ERR_GC401B_INVALID_DATE );
				}
			}
			catch( SqlEx ex )
			{
				/*
				** Any parsing error is assumed to be caused by an interval.
				*/
				interval = true;
				throw ex;
			}
		} // getDate


		/*
		** Name: getTime
		**
		** Description:
		**	Return the current data value as a Time value.
		**	A relative timezone may be provided which is
		**	applied to adjust the final result such that it
		**	represents the original date/time value in the
		**	timezone provided.  The default is to use the
		**	local timezone.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	tz	Relative timezone, NULL for local.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Time	Current value converted to Time.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public override TimeSpan 
			getTime( TimeZone tz ) 
		{ 
			/*
			** Ingres dates are overloaded with 'empty' date,
			** date only, timestamp and interval values.  The
			** first three types are handled explicitly below.
			** Intervals will either cause an exception while
			** attempting to parse the value or as the default
			** action for an unrecognized format.
			*/
			try
			{
				if (  value.Length == 0  ||				// Empty date
					value.Length == SqlDates.D_FMT.Length )	// Date only
				{
					/*
					** There is no time component, so create a time EPOCH value.
					** If no timezone is provided, we can return the local epoch 
					** constant.  Otherwise, the epoch value for the requested 
					** timezone must be generated.
					*/
					return( (tz == null) ? SqlDates.getEpochTime()
						: SqlDates.parseTime(SqlDates.T_EPOCH, tz) );
				}
				else  if ( value.Length == SqlDates.TS_FMT.Length )	// Timestamp
				{
					/*
					** Remove the date component but retain correct time:
					**
					** 1.  Convert to GMT timestamp using TZ for current connection.
					** 2.  Re-format as time only using local TZ to get local time.
					** 3.  Generate Time value using requested/local TZ.
					*/
					DateTime ts = SqlDates.parseTimestamp( value, use_gmt );
					String str = SqlDates.formatTime( ts, false );
					return( (tz == null) ? SqlDates.parseTime( str, false )
						: SqlDates.parseTime( str, tz ) );
				}
				else							// Interval
				{
					/*
					** Can't support intervals with Time objects.
					*/
					throw SqlEx.get( ERR_GC401B_INVALID_DATE );
				}
			}
			catch( SqlEx ex )
			{
				/*
				** Any parsing error is assumed to be caused by an interval.
				*/
				interval = true;
				throw ex;
			}
		} // getTime


		/*
		** Name: getTimestamp
		**
		** Description:
		**	Return the current data value as a Timestamp value.
		**	A relative timezone may be provided which is
		**	applied to adjust the final result such that it
		**	represents the original date/time value in the
		**	timezone provided.  The default is to use the
		**	local timezone.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	tz		Relative timezone, NULL for local.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Timestamp	Current value converted to Timestamp.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public override DateTime
			getTimestamp( TimeZone tz ) 
		{
			/*
			** Ingres dates are overloaded with 'empty' date,
			** date only, timestamp and interval values.  The
			** first three types are handled explicitly below.
			** Intervals will either cause an exception while
			** attempting to parse the value or as the default
			** action for an unrecognized format.
			*/
			try
			{
				if (  value.Length == 0 )				// Empty date
				{
					/*
					** Create a timstamp EPOCH value.  If no timezone is 
					** provided, we can return the local epoch constant.  
					** Otherwise, the epoch value for the requested 
					** timezone must be generated.
					*/
					return( (tz == null) ? SqlDates.getEpochTimestamp()
						: SqlDates.parseTimestamp( SqlDates.TS_EPOCH, tz ) );
				}
				else  if ( value.Length == SqlDates.D_FMT.Length )	// Date only
				{
					/*
					** There is no time component, so convert to timestamp with
					** a 0 time component for the requested/local timezone.
					*/
					DateTime date = (tz == null) 
						? SqlDates.parseDate( value, false )
						: SqlDates.parseDate( value, tz );
					return( date );
				}
				else  if ( value.Length == SqlDates.TS_FMT.Length )	// Timestamp
				{
					/*
					** Convert to GMT timestamp using TZ for current connection.
					*/
					DateTime ts = SqlDates.parseTimestamp( value, use_gmt );
	
					if ( tz != null )
					{
						/*
						** Effectively, we need to apply time difference
						** between local and requested timezones.  First,
						** apply local TZ to get local timestamp.  Then
						** apply requested TZ to get desired GMT value.
						*/
						String str = SqlDates.formatTimestamp( ts, false );
						ts = SqlDates.parseTimestamp( str, tz );
					}
	
					return( ts );
				}
				else	   						// Interval
				{
					/*
					** Can't support intervals with Timestamp objects.
					*/
					throw SqlEx.get( ERR_GC401B_INVALID_DATE );
				}
			}
			catch( SqlEx ex )
			{
				/*
				** Any parsing error is assumed to be caused by an interval.
				*/
				interval = true;
				throw ex;
			}
		} // getTimestamp


		/*
		** Name: getObject
		**
		** Description:
		**	Return the current data value as a Timestamp object.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Object	Current value converted to Timestamp.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public override Object 
			getObject() 
		{
			return( getTimestamp( null ) );
		} // getObject


	} // class IngresDate
}  // namespace
