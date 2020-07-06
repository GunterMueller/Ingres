/*
** Copyright (c) 2003 Ingres Corporation All Rights Reserved.
*/

package	com.ingres.gcf.util;

/*
** Name: SqlDates.java
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
**	19-Jun-06 (gordy)
**	    ANSI Date/Time data type support
**	22-Sep-06 (gordy)
**	    Added methods for handling ANSI time zone offsets.
*/

import	java.text.DateFormat;
import	java.text.SimpleDateFormat;
import	java.util.Hashtable;
import	java.util.Vector;
import	java.util.TimeZone;
import	java.util.SimpleTimeZone;
import	java.sql.Date;
import	java.sql.Time;
import	java.sql.Timestamp;


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
**	TZ_FMT			Timezone standard format.
**
**  Public Methods:
**
**	getEpochDate		Returns epoch Date value.
**	getEpochTime		Returns epoch Time value.
**	getEpochTimestamp	Returns epoch Timestamp value.
**	parseOffset		Parse Timezone offset.
**	formatOffset		Format Timezone Offset.
**	getTZ			Returns timezone for UTC offset.
**	formatTZ		UTC offset of TZ in standard format.
**	parseFrac		Parse fractional seconds.
**	formatFrac		Format fractional seconds.
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
**	tzCache			Timezone cache.
**	tzoCache		Timezone offset cache.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
**	19-Jun-06 (gordy)
**	    ANSI Date/Time data type support: TZ_FMT, tzOffset(),
**	    parseFrac(), and formatFrac().
**	22-Sep-06 (gordy)
**	    Added parseOffset(), formatOffset(), and getTZ(). 
**	    Rename tzOffset() to formatTZ().  Cache timezone 
**	    offsets: added tzoCache, tzCache.
*/

public class
SqlDates
    implements GcfErr
{

    /*
    ** Date/Time constants for EPOCH values, JDBC literals, and
    ** Ingres literals.  
    **
    ** Note: TZ_FMT is provided to show the standard TZ format
    ** but isn't an actual usable format.
    */
    public static final String	D_EPOCH		= "1970-01-01";
    public static final String	T_EPOCH		= "00:00:00";
    public static final String	TS_EPOCH	= D_EPOCH + " " + T_EPOCH;
    public static final String	D_FMT		= "yyyy-MM-dd";
    public static final String	T_FMT		= "HH:mm:ss";
    public static final String	TS_FMT		= D_FMT + " " + T_FMT;
    public static final String	D_LIT_FMT	= "yyyy_MM_dd";
    public static final String	T_LIT_FMT	= "HH:mm:ss";
    public static final String	TS_LIT_FMT	= D_LIT_FMT + " " + T_LIT_FMT;
    public static final String	TZ_FMT		= "+HH:mm";

    /*
    ** Common timezones and date/time formatters required by parsing and
    ** formatting methods.  There are two fixed timezones for GMT and the
    ** local machine timezone.  There are formatters for timestamp, date, 
    ** and time values as well as literals.  
    **
    ** Use of any formatter requires synchronization on the formatters 
    ** themselves.
    */
    private static TimeZone	tz_gmt = TimeZone.getTimeZone( "GMT" );
    private static TimeZone	tz_lcl = TimeZone.getDefault();
    private static DateFormat	df_ts_val = new SimpleDateFormat( TS_FMT );
    private static DateFormat	df_d_val = new SimpleDateFormat( D_FMT );
    private static DateFormat	df_t_val = new SimpleDateFormat( T_FMT );
    private static DateFormat	df_ts_lit = new SimpleDateFormat( TS_LIT_FMT );
    private static DateFormat	df_d_lit = new SimpleDateFormat( D_LIT_FMT );
    private static DateFormat	df_t_lit = new SimpleDateFormat( T_LIT_FMT );
    private static Date		epochDate = null;
    private static Time		epochTime = null;
    private static Timestamp	epochTS = null;
    private static Hashtable	tzCache = new Hashtable();
    private static Vector	tzoCache = new Vector();


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

public static Date
getEpochDate()
    throws SqlEx
{
    if ( epochDate == null )  epochDate = parseDate( D_EPOCH, tz_lcl );
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

public static Time
getEpochTime()
    throws SqlEx
{
    if ( epochTime == null )  epochTime = parseTime( T_EPOCH, tz_lcl );
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

public static Timestamp
getEpochTimestamp()
    throws SqlEx
{
    if ( epochTS == null )  epochTS = parseTimestamp( TS_EPOCH, tz_lcl );
    return( epochTS );
} // getEpochTimestamp


/*
** Name: parseOffset
**
** Description:
**	Parses standard timezone offset "+HH:mm" and returns
**	offset in minutes.
**
** Input:
**	tzOffset	Standard timezone offset.
**
** Output:
**	None.
**
** Returns:
**	int		Offset in minutes.
**
** History:
**	22-Sep-06 (gordy
**	    Created.
*/

public static int
parseOffset( String tzOffset )
    throws SqlEx
{
    /*
    ** Validate offset format.
    */
    if (
	 tzOffset.length() != TZ_FMT.length()  ||
	 (tzOffset.charAt(0) != '+'  &&  tzOffset.charAt(0) != '-')  ||
	 ! Character.isDigit( tzOffset.charAt(1) )  ||
	 ! Character.isDigit( tzOffset.charAt(2) )  ||
	 tzOffset.charAt(3) != ':'  ||
	 ! Character.isDigit( tzOffset.charAt(4) )  ||
	 ! Character.isDigit( tzOffset.charAt(5) )
       )
	throw SqlEx.get( ERR_GC401B_INVALID_DATE );

    int hours = Character.digit( tzOffset.charAt(1), 10 ) * 10 +
		Character.digit( tzOffset.charAt(2), 10 );
    int minutes = Character.digit( tzOffset.charAt(4), 10 ) * 10 +
		  Character.digit( tzOffset.charAt(5), 10 );
    int offset = hours * 60 + minutes;
    if ( tzOffset.charAt(0) == '-' )  offset = -offset;

    return( offset );
} // parseOffset


/*
** Name: formatOffset
**
** Description:
**	Formats a timesone offset into standard format: "+HH:mm".
**
** Input:
**	offset		Timezone offset in minutes.
**
** Output:
**	None.
**
** Returns:
**	String		Formatted offset.
**
** History:
**	22-Sep-06 (gordy)
**	    Created.
*/

private static char digits[] = {'0','1','2','3','4','5','6','7','8','9'};

public static String
formatOffset( int offset )
{
    String tzOffset = null;

    /*
    ** Since only a small number of timezones are expected
    ** to be active, a simple vector is used to map offsets.
    ** The vector is not synchronized because it is only
    ** extended and duplicate entries will only cause an
    ** insignificant degredation in performance.
    */
    for( int i = 0; i < tzoCache.size(); i++ )
    {
	IdMap tzo = (IdMap)tzoCache.get(i);

	if ( tzo.equals( offset ) )
	{
	    tzOffset = tzo.toString();
	    break;
	}
    }

    if ( tzOffset == null )
    {	
	boolean neg = (offset < 0);
	if ( neg )  offset = -offset;
	int hour = offset / 60;
	int minute = offset % 60;

	char format[] = new char[ 6 ];
	format[ 0 ] = neg ? '-' : '+';
	format[ 1 ] = digits[ (hour / 10) % 10 ];
	format[ 2 ] = digits[ hour % 10 ];
	format[ 3 ] = ':';
	format[ 4 ] = digits[ (minute / 10) % 10 ];
	format[ 5 ] = digits[ minute % 10 ];

	if ( neg )  offset = -offset;	// Restore original offset
	tzOffset = new String( format );
	tzoCache.add( new IdMap( offset, tzOffset ) );
    }

    return( tzOffset );
} // formatOffset


/*
** Name: getTZ
**
** Description:
**	Returns a timezone for a given TZ offset.
**
** Input:
**	offset	Timezone offset in minutes.
**
** Output:
**	None.
**
** Returns:
**	TimeZone	Timezone.
**
** History:
**	22-Sep-06 (gordy)
**	    Created.
*/

public static TimeZone
getTZ( int offset )
    throws SqlEx
{
    return( getTZ( formatOffset( offset ) ) );
} // getTZ


/*
** Name: getTZ
**
** Description:
**	Parses standard TZ offset format "+HH:mm" and returns
**	a timezone with the same TZ offset.
**
** Input:
**	tzOffset	TZ offset.
**
** Output:
**	None.
**
** Returns:
**	TimeZone	Timezone.
**
** History:
**	22-Sep-06 (gordy)
**	    Created.
*/

public static TimeZone
getTZ( String tzOffset )
    throws SqlEx
{
    /*
    ** Only a small number timezones are expected to be active.
    ** Timezones and offsets are cached for quick access.
    */
    TimeZone tz = (TimeZone)tzCache.get( tzOffset );

    if ( tz == null )
    {
	/*
	** Construct timezone for offset
	*/
	int offset = parseOffset( tzOffset );
	tz = new SimpleTimeZone( offset * 60000, "GMT" + tzOffset );
	tzCache.put( tzOffset, tz );
    }

    return( tz );
} // getTZ


/*
** Name: formatTZ
**
** Description:
**	Returns the offset from UTC for a date/time value 
**	in the local default timezone.  Standard TZ offset
**	format is "+HH:mm".
**
** Input:
**	value		Date/time value.
**
** Output:
**	None.
**
** Returns:
**	String		TZ offset.
**
** History:
**	19-Jun-06 (gordy)
**	    Created.
**	22-Sep-06 (gordy)
**	    Renamed.
*/

public static String
formatTZ( java.util.Date value )
{
    return( formatTZ( tz_lcl, value.getTime() ) );
} // formatTZ


/*
** Name: formatTZ
**
** Description:
**	Returns the offset from UTC for a date/time value 
**	in a specific timezone.  Standard TZ offset format
**	is "+HH:mm".
**
** Input:
**	tz		Target timezone.
**	value		Date/time value.
**
** Output:
**	None.
**
** Returns:
**	String		UTC offset.
**
** History:
**	19-Jun-06 (gordy)
**	    Created.
**	22-Sep-06 (gordy)
**	    Renamed.  
*/

public static String
formatTZ( TimeZone tz, java.util.Date value )
{
    return( formatTZ( tz, value.getTime() ) );
} // formatTZ


/*
** Name: formatTZ
**
** Description:
**	Returns the offset from UTC for a milli-second time
**	value in the default timezone.  Standard TZ
**	offset format is "+HH:mm".
**
** Input:
**	millis		Milli-second time value.
**
** Output:
**	None.
**
** Returns:
**	String		UTC offset.
**
** History:
**	22-Sep-06 (gordy)
**	    Created.
*/

public static String
formatTZ( long millis )
{
    return( formatTZ( tz_lcl, millis ) );
} // formatTZ


/*
** Name: formatTZ
**
** Description:
**	Returns the offset from UTC for a date/time value
**	in a specific timezone.  Standard TZ offset format
**	is "+HH:mm".
**
** Input:
**	tz		Target timezone.
**	millis		Milli-second time value.
**
** Output:
**	None.
**
** Returns:
**	String		UTC ofset.
**
** History:
**	22-Sep-06 (gordy)
**	    Created.  Cache offset strings.
*/

public static String
formatTZ( TimeZone tz, long millis )
{
    return( formatOffset( tz.getOffset( millis ) / 60000 ) );
} // formatTZ


/*
** Name: parseFrac
**
** Description:
**	Parses a Fractional seconds timestamp component
**	in the format ".ddddddddd" and returns the number
**	of nano-seconds.  At a minimum, the decimal point
**	must be present.  Any number of decimal digits 
**	may be present.  Any digits beyond nano-seconds
**	are ignored.
**
** Input:
**	frac	Fractional seconds string.
**
** Output:
**	None.
**
** Returns:
**	int	Nano-seconds
**
** History:
**	19-Jun-06 (gordy)
**	    Created.
*/

private static char zeros[] = {'0','0','0','0','0','0','0','0','0'};

public static int
parseFrac( String frac )
    throws SqlEx
{
    int nanos = 0;

    /*
    ** Validate minimum format.
    */
    if ( frac.length() < 1  ||  frac.charAt( 0 ) != '.' )
	throw SqlEx.get( ERR_GC401B_INVALID_DATE );

    /*
    ** Transform into standard format by truncating extra 
    ** digits or appending trailing zeros as needed.
    */
    StringBuffer sb = new StringBuffer( frac );
    if ( sb.length() > 10 )  sb.setLength( 10 );
    if ( sb.length() < 10 )  sb.append( zeros, 0, 10 - sb.length() );

    /*
    ** Extract number of nano-seconds.
    */
    try { nanos = Integer.parseInt( sb.substring( 1 ) ); }
    catch( NumberFormatException nfe )
    { throw SqlEx.get( ERR_GC401B_INVALID_DATE ); }

    return( nanos );
} // parseFrac


/*
** Name: formatFrac
**
** Description:
**	Formats nano-seconds into a fractional seconds
**	timestamp component in the format ".dddddddddd".
**	Trailing zeros are truncated, but at a minimum
**	".0" will be returned.
**
** Input:
**	nanos	Nano-seconds
**
** Output:
**	None.
**
** Returns:
**	String	Fractional seconds string.
**
** History:
**	19-Jun-06 (gordy)
**	    Created.
*/

public static String
formatFrac( int nanos )
{
    /*
    ** Handle minimal case.
    */
    if ( nanos == 0 )  return( ".0" );
    if ( nanos < 0 )  nanos = -nanos;		// Shouldn't happen!

    StringBuffer sb = new StringBuffer();
    String	 frac = Integer.toString( nanos );

    /*
    ** Build-up the fractional seconds component.
    ** Prepend zeros as necessary to get correct
    ** magnitude.  
    */
    sb.append( '.' );
    if ( frac.length() < 9 )  sb.append( zeros, 0, 9 - frac.length() );
    sb.append( frac );

    /*
    ** Remove trailing zeros.  There shouldn't be any
    ** excess digits, but skip/truncate just in case.
    ** Note: there is at least 1 non-zero digit since
    ** the 0 case was handled above.
    */
    int length = 10;
    while( sb.charAt( length - 1 ) == '0' )  length--;
    sb.setLength( length );

    return( sb.toString() );
} // formatFrac


/*
** Name: parseTime
**
** Description:
**	Parse a string containing a JDBC time value using GMT or
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
**	Time		JDBC Time.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public static Time
parseTime( String str, boolean use_gmt ) 
    throws SqlEx
{ 
    return( parseTime( str, use_gmt ? tz_gmt : tz_lcl ) ); 
}


/*
** Name: parseTime
**
** Description:
**	Parse a string containing a JDBC time value using an 
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
**	Time		JDBC Time.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public static Time
parseTime( String str, TimeZone tz )
    throws SqlEx
{
    java.util.Date date;
    
    if ( str.length() != T_FMT.length() )
	throw SqlEx.get( ERR_GC401B_INVALID_DATE );

    synchronized( df_t_val )
    {
	df_t_val.setTimeZone( tz );
	try { date = df_t_val.parse( str ); }
	catch( Exception ex )
	{ throw SqlEx.get( ERR_GC401B_INVALID_DATE ); }
    }
    
    return( new Time( date.getTime() ) );
} // parseTime


/*
** Name: formatTime
**
** Description:
**	Format a Java date value into a time string using GMT or
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
formatTime( java.util.Date date, boolean use_gmt )
{ 
    return( formatTime( date, use_gmt ? tz_gmt : tz_lcl ) ); 
} // formatTime


/*
** Name: formatTime
**
** Description:
**	Format a Java date value into a time string using an 
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
formatTime( java.util.Date date, TimeZone tz )
{
    String str;
    
    synchronized( df_t_val )
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
**	Format a Java date value into a time literal using GMT or
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
formatTimeLit( java.util.Date date, boolean use_gmt )
{ 
    return( formatTimeLit( date, use_gmt ? tz_gmt : tz_lcl ) ); 
} // formatTimeLit


/*
** Name: formatTimeLit
**
** Description:
**	Format a Java date value into a time literal using an 
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
formatTimeLit( java.util.Date date, TimeZone tz )
{
    String str;
    
    synchronized( df_t_lit )
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
**	Parse a string containing a JDBC date value using GMT or
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
**	Date		JDBC Date.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public static Date
parseDate( String str, boolean use_gmt ) 
    throws SqlEx
{ 
    return( parseDate( str, use_gmt ? tz_gmt : tz_lcl ) ); 
} // parseDate


/*
** Name: parseDate
**
** Description:
**	Parse a string containing a JDBC date value using an 
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
**	Date		JDBC Date.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public static Date
parseDate( String str, TimeZone tz )
    throws SqlEx
{
    java.util.Date date;
    
    if ( str.length() != D_FMT.length() )
	throw SqlEx.get( ERR_GC401B_INVALID_DATE );

    synchronized( df_d_val )
    {
	df_d_val.setTimeZone( tz );
	try { date = df_d_val.parse( str ); }
	catch( Exception ex )
	{ throw SqlEx.get( ERR_GC401B_INVALID_DATE ); }
    }
    
    return( new Date( date.getTime() ) );
} // parseDate


/*
** Name: formatDate
**
** Description:
**	Format a Java date value into a date string using GMT or
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
formatDate( java.util.Date date, boolean use_gmt )
{ 
    return( formatDate( date, use_gmt ? tz_gmt : tz_lcl ) ); 
} // formatDate


/*
** Name: formatDate
**
** Description:
**	Format a Java date value into a date string using an 
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
formatDate( java.util.Date date, TimeZone tz )
{
    String str;
    
    synchronized( df_d_val )
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
**	Format a Java date value into a date literal using GMT or
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
formatDateLit( java.util.Date date, boolean use_gmt )
{ 
    return( formatDateLit( date, use_gmt ? tz_gmt : tz_lcl ) ); 
} // formatDateLit


/*
** Name: formatDateLit
**
** Description:
**	Format a Java date value into a date literal using an 
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
formatDateLit( java.util.Date date, TimeZone tz )
{
    String str;
    
    synchronized( df_d_lit )
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
**	Parse a string containing a JDBC timestamp value using GMT or
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
**	Timestamp	JDBC Timestamp.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public static Timestamp
parseTimestamp( String str, boolean use_gmt ) 
    throws SqlEx
{ 
    return( parseTimestamp( str, use_gmt ? tz_gmt : tz_lcl ) ); 
} // parseTimestamp


/*
** Name: parseTimestamp
**
** Description:
**	Parse a string containing a JDBC timestamp value using an 
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
**	Timestamp	JDBC Timestamp.
**
** History:
**	12-Sep-03 (gordy)
**	    Created.
*/

public static Timestamp
parseTimestamp( String str, TimeZone tz )
    throws SqlEx
{
    java.util.Date date;
    
    if ( str.length() != TS_FMT.length() )
	throw SqlEx.get( ERR_GC401B_INVALID_DATE );

    synchronized( df_ts_val )
    {
	df_ts_val.setTimeZone( tz );
	try { date = df_ts_val.parse( str ); }
	catch( Exception ex )
	{ throw SqlEx.get( ERR_GC401B_INVALID_DATE ); }
    }
    
    return( new Timestamp( date.getTime() ) );
} // parseTimestamp


/*
** Name: formatTimestamp
**
** Description:
**	Format a Java date value into a timestamp string using GMT or
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
formatTimestamp( java.util.Date date, boolean use_gmt )
{ 
    return( formatTimestamp( date, use_gmt ? tz_gmt : tz_lcl ) ); 
} // formatTimestamp


/*
** Name: formatTimestamp
**
** Description:
**	Format a Java date value into a timestamp string using an
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
formatTimestamp( java.util.Date date, TimeZone tz )
{
    String str;
    
    synchronized( df_ts_val )
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
**	Format a Java date value into a timestamp literal using GMT or 
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
formatTimestampLit( java.util.Date date, boolean use_gmt )
{ 
    return( formatTimestampLit( date, use_gmt ? tz_gmt : tz_lcl ) ); 
} // formatTimestampLit


/*
** Name: formatTimestampLit
**
** Description:
**	Format a Java date value into a timestamp literal using an 
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
formatTimestampLit( java.util.Date date, TimeZone tz )
{
    String str;
    
    synchronized( df_ts_lit )
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
