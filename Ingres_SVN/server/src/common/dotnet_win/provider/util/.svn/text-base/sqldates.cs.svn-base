/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.IO;
using System.Text;

namespace Ca.Ingres.Utility
{
	/*
	** Name: sqldates.cs
	**
	** Description:
	**	Defines class which provides constants and static methods
	**	for processing SQL date/time values.
	**
	**  Classes:
	**
	**	SqlDates
	**
	** History:
	**	12-Sep-03 (gordy)
	**	    Created.
	**	 9-Dec-03 (thoda04)
	**	    Ported to .NET.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	**	14-dec-04 (thoda04) SIR 113624
	**	    Add BLANKDATE=NULL support.
	*/


	/*
	** Name: SqlDates
	**
	** Description:
	**	Utility class which provides constants and static methods
	**	for date/time processing.
	**
	**  Constants:
	**
	**	D_EPOCH			Date epoch value.
	**	T_EPOCH			Time epoch value.
	**	TS_EPOCH		Timestamp epoch value.
	**	D_FMT			Date format template.
	**	T_FMT			Time format template.
	**	TS_FMT			Timestamp format template.
	**	D_LIT_FMT		Date literal format template.
	**	T_LIT_FMT		Time literal format template.
	**	TS_LIT_FMT		Timestamp literal format template.
	**
	**  Public Methods:
	**
	**	getEpochDate		Returns epoch Date value.
	**	getEpochTime		Returns epoch Time value.
	**	getEpochTimestamp	Returns epoch Timestamp value.
	**	parseTime		Parse SQL time values.
	**	formatTime		Format SQL time values.
	**	formatTimeLit		Format SQL time literals.
	**	parseDate		Parse SQL date values.
	**	formatDate		Format SQL date values.
	**	formatDateLit		Format SQL date literals.
	**	parseTimestamp		Parse SQL timestamp values.
	**	formatTimestamp		Format SQL timestamp values.
	**	formatTimestampLit	Format SQL timestamp literals.
	**
	**  Private Data:
	**
	**	tz_gmt			GMT Timezone.
	**	tz_lcl			Local Timezone.
	**	df_gmt			GMT Date formatter.
	**	df_lcl			Local Date formatter.
	**	df_ts_val		Date formatter for Timestamp values.
	**	df_d_val		Date formatter for Date values.
	**	df_t_val		Date formatter for Time values.
	**	df_ts_lit		Date formatter for Timestamp literals.
	**	df_d_lit		Date formatter for Date literals.
	**	df_t_lit		Date formatter for Time literals.
	**	epochDate		Epoch Date value.
	**	epochTime		Epoch Time value.
	**	epochTS			Epoch Timestamp value.
	**
	** History:
	**	12-Sep-03 (gordy)
	**	    Created.
	*/

	internal class
		SqlDates : GcfErr
	{

		/*
		** Date/Time constants for EPOCH values, literals, and
		** Ingres literals.
		*/
		internal const String	D_EPOCH		= "9999-12-31";
		internal const String	T_EPOCH		= "23:59:59";
		internal const String	TS_EPOCH	= D_EPOCH + " " + T_EPOCH;
		internal const String	D_FMT		= "yyyy-MM-dd";
		internal const String	T_FMT		= "HH:mm:ss";
		internal const String	TS_FMT		= D_FMT + " " + T_FMT;
		internal const String	D_LIT_FMT	= "yyyy_MM_dd";
		internal const String	T_LIT_FMT	= "HH:mm:ss";
		internal const String	TS_LIT_FMT	= D_LIT_FMT + " " + T_LIT_FMT;

		/*
		** Common timezones and date/time formatters required by parsing and
		** formatting methods.  There are two fixed timezones for GMT and the
		** local machine timezone.  There are formatters for timestamp, date, 
		** and time values as well as literals.  
		**
		** Use of any formatter requires synchronization on the formatters 
		** themselves.
		*/
		private static TimeZone  	tz_gmt   = null; //TimeZone..getTimeZone( "GMT" );
		private static TimeZone  	tz_lcl   = TimeZone.CurrentTimeZone;
		private static DateFormat	df_ts_val= new DateFormat( TS_FMT );
		private static DateFormat	df_d_val = new DateFormat( D_FMT );
		private static DateFormat	df_t_val = new DateFormat( T_FMT );
		private static DateFormat	df_ts_lit= new DateFormat( TS_LIT_FMT );
		private static DateFormat	df_d_lit = new DateFormat( D_LIT_FMT );
		private static DateFormat	df_t_lit = new DateFormat( T_LIT_FMT );
		private static DateTime  	epochDate = DateTime.MinValue;
		private static TimeSpan  	epochTime = TimeSpan.MinValue;
		private static DateTime  	epochTS   = DateTime.MinValue;


		/*
		** Name: getEpochDate
		**
		** Description:
		**	Returns epoch Date value relative to local timezone.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Date	Epoch date value.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static DateTime
			getEpochDate()
		{
			if (epochDate == DateTime.MinValue )
				epochDate = parseDate( D_EPOCH, tz_lcl );
			return( epochDate );
		} // getEpochDate


		/*
		** Name: getEpochTime
		**
		** Description:
		**	Returns epoch Time value relative to local timezone.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Time	Epoch Time value.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static TimeSpan
			getEpochTime()
		{
			if (epochTime == TimeSpan.MinValue )
				epochTime = parseTime( T_EPOCH, tz_lcl );
			return( epochTime );
		} // getEpochTime


		/*
		** Name: getEpochTimestamp
		**
		** Description:
		**	Returns epoch Timestamp value relative to local timezone.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Timestamp	Epoch Timestamp value.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static DateTime
			getEpochTimestamp()
		{
			if (epochTS == DateTime.MinValue )
				epochTS = parseTimestamp( TS_EPOCH, tz_lcl );
			return( epochTS );
		} // getEpochTimestamp


		/*
		** Name: parseTime
		**
		** Description:
		**	Parse a string containing a time value using GMT or
		**	local timezone.
		**
		** Input:
		**	str		Time value.
		**	use_gmt		True for GMT, False for local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Time		Time.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static TimeSpan
			parseTime( String str, bool use_gmt ) 
		{ 
			return( parseTime( str, use_gmt ? tz_gmt : tz_lcl ) ); 
		}


		/*
		** Name: parseTime
		**
		** Description:
		**	Parse a string containing a time value using an 
		**	explicit timezone.
		**
		** Input:
		**	str		Time value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Time		Time.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static TimeSpan
			parseTime( String str, TimeZone tz )
		{
			DateTime date;

			lock( df_t_val )
			{
				df_t_val.setTimeZone( tz );
				try { date = df_t_val.parse( str ); }
				catch( Exception )
				{ throw SqlEx.get( ERR_GC401B_INVALID_DATE ); }
			}

			return( date.TimeOfDay );
		} // parseTime


		/*
		** Name: formatTime
		**
		** Description:
		**	Format a date value into a time string using GMT or
		**	local timezone.
		**
		** Input:
		**	date		Jave date value.
		**	use_gmt		Use GMT or local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted time.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatTime( DateTime date, bool use_gmt )
		{ 
			return( formatTime( date, use_gmt ? tz_gmt : tz_lcl ) ); 
		} // formatTime


		/*
		** Name: formatTime
		**
		** Description:
		**	Format a date value into a time string using an 
		**	explicit timezone.
		**
		** Input:
		**	date		Jave date value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted time.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatTime( DateTime date, TimeZone tz )
		{
			String str;
    
			lock( df_t_val )
			{
				df_t_val.setTimeZone( tz );
				str = df_t_val.format( date );
			}
    
			return( str );
		} // formatTime


		/*
		** Name: formatTimeLit
		**
		** Description:
		**	Format a date value into a time literal using GMT or
		**	local timezone.
		**
		** Input:
		**	date		Jave date value.
		**	use_gmt		Use GMT or local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted time literal.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatTimeLit( TimeSpan date, bool use_gmt )
		{ 
			return( formatTimeLit( date, use_gmt ? tz_gmt : tz_lcl ) ); 
		} // formatTimeLit


		/*
		** Name: formatTimeLit
		**
		** Description:
		**	Format a date value into a time literal using an 
		**	explicit timezone.
		**
		** Input:
		**	date		Jave date value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted time literal.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatTimeLit( TimeSpan date, TimeZone tz )
		{
			String str;

			lock( df_t_lit )
			{
				df_t_lit.setTimeZone( tz );
				str = df_t_lit.format( date );
			}

			return( str );
		} // formatTimeLit


		/*
		** Name: parseDate
		**
		** Description:
		**	Parse a string containing a date value using GMT or
		**	local timezone.
		**
		** Input:
		**	str		Date value.
		**	use_gmt		True for GMT, Falst for local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Date		Date.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static DateTime
			parseDate( String str, bool use_gmt ) 
		{ 
			return( parseDate( str, use_gmt ? tz_gmt : tz_lcl ) ); 
		} // parseDate


		/*
		** Name: parseDate
		**
		** Description:
		**	Parse a string containing a date value using an 
		**	explicit timezone.
		**
		** Input:
		**	str		Date value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Date		Date.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static DateTime
			parseDate( String str, TimeZone tz )
		{
			DateTime date;

			lock( df_d_val )
			{
				df_d_val.setTimeZone( tz );
				try { date = df_d_val.parse( str ); }
				catch( Exception )
				{ throw SqlEx.get( ERR_GC401B_INVALID_DATE ); }
			}

			return( date.Date );
		} // parseDate


		/*
		** Name: formatDate
		**
		** Description:
		**	Format a date value into a date string using GMT or
		**	local timezone.
		**
		** Input:
		**	date		Jave date value.
		**	use_gmt		True for GMT, False for local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted date.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatDate( DateTime date, bool use_gmt )
		{ 
			return( formatDate( date, use_gmt ? tz_gmt : tz_lcl ) ); 
		} // formatDate


		/*
		** Name: formatDate
		**
		** Description:
		**	Format a date value into a date string using an 
		**	explicit timezone.
		**
		** Input:
		**	date		Jave date value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted date.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatDate( DateTime date, TimeZone tz )
		{
			String str;

			lock( df_d_val )
			{
				df_d_val.setTimeZone( tz );
				str = df_d_val.format( date );
			}

			return( str );
		} // formatDate


		/*
		** Name: formatDateLit
		**
		** Description:
		**	Format a date value into a date literal using GMT or
		**	local timezone.
		**
		** Input:
		**	date		Jave date value.
		**	use_gmt		True for GMT, False for local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted date literal.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatDateLit( DateTime date, bool use_gmt )
		{ 
			return( formatDateLit( date, use_gmt ? tz_gmt : tz_lcl ) ); 
		} // formatDateLit


		/*
		** Name: formatDate
		**
		** Description:
		**	Format a date value into a date literal using an 
		**	explicit timezone.
		**
		** Input:
		**	date		Jave date value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted date literal.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatDateLit( DateTime date, TimeZone tz )
		{
			String str;
    
			lock( df_d_lit )
			{
				df_d_lit.setTimeZone( tz );
				str = df_d_lit.format( date );
			}
    
			return( str );
		} // formatDateLit


		/*
		** Name: parseTimestamp
		**
		** Description:
		**	Parse a string containing a timestamp value using GMT or
		**	local timezone.
		**
		** Input:
		**	str		Timestamp value.
		**	use_gmt		True for GMT, False for local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Timestamp	Timestamp.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static DateTime
			parseTimestamp( String str, bool use_gmt ) 
		{ 
			return( parseTimestamp( str, use_gmt ? tz_gmt : tz_lcl ) ); 
		} // parseTimestamp


		/*
		** Name: parseTimestamp
		**
		** Description:
		**	Parse a string containing a timestamp value using an 
		**	explicit timezone.
		**
		** Input:
		**	str		Timestamp value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Timestamp	Timestamp.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static DateTime
			parseTimestamp( String str, TimeZone tz )
		{
			DateTime date;

			lock( df_ts_val )
			{
				df_ts_val.setTimeZone( tz );
				try { date = df_ts_val.parse( str ); }
				catch( Exception )
				{ throw SqlEx.get( ERR_GC401B_INVALID_DATE ); }
			}

			return( date );
		} // parseTimestamp


		/*
		** Name: formatTimestamp
		**
		** Description:
		**	Format a date value into a timestamp string using GMT or
		**	local timezone.
		**
		** Input:
		**	date		Jave date value.
		**	use_gmt		True for GMT, False for local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted timestamp.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatTimestamp( DateTime date, bool use_gmt )
		{ 
			return( formatTimestamp( date, use_gmt ? tz_gmt : tz_lcl ) ); 
		} // formatTimestamp


		/*
		** Name: formatTimestamp
		**
		** Description:
		**	Format a date value into a timestamp string using an
		**	explicit timezone.
		**
		** Input:
		**	date		Jave date value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted timestamp.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatTimestamp( DateTime date, TimeZone tz )
		{
			String str;
    
			lock( df_ts_val )
			{
				df_ts_val.setTimeZone( tz );
				str = df_ts_val.format( date );
			}
    
			return( str );
		} // formatTimestamp


		/*
		** Name: formatTimestampLit
		**
		** Description:
		**	Format a date value into a timestamp literal using GMT or 
		**	local timezone.
		**
		** Input:
		**	date		Jave date value.
		**	use_gmt		True for GMT, False for local timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted timestamp literal.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatTimestampLit( DateTime date, bool use_gmt )
		{ 
			return( formatTimestampLit( date, use_gmt ? tz_gmt : tz_lcl ) ); 
		} // formatTimestampLit


		public static String
			formatTimestampLit( TimeSpan time, bool use_gmt )
		{
			DateTime date = new DateTime(1970, 1, 1) + time;
			string s = formatTimestampLit( date, use_gmt ? tz_gmt : tz_lcl );
			date = DateTime.Parse(s);
			return date.TimeOfDay.ToString();
		} // formatTimestampLit


		/*
		** Name: formatTimestampLit
		**
		** Description:
		**	Format a date value into a timestamp literal using an 
		**	explicit timezone.
		**
		** Input:
		**	date		Jave date value.
		**	tz		Timezone.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Formatted timestamp literal.
		**
		** History:
		**	12-Sep-03 (gordy)
		**	    Created.
		*/

		public static String
			formatTimestampLit( DateTime date, TimeZone tz )
		{
			String str;

			lock( df_ts_lit )
			{
				df_ts_lit.setTimeZone( tz );
				str = df_ts_lit.format( date );
			}

			return( str );
		} // formatTimestampLit


		/*
		** Name: SqlDates
		**
		** Description:
		**	Constructor: removes ability to instantiate.
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

		private
			SqlDates()
		{
		} // SqlDates


	} // class SqlDates
}  // namespace
