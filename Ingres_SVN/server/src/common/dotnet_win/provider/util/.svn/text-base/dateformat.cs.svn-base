/*
** Copyright (c) 2002, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;

namespace Ca.Ingres.Utility
{
	
	/*
	** Name: dateformat.cs
	**
	** Description:
	**	DateTime formating routines.
	**
	** History:
	**	24-Jul-02 (thoda04)
	**	    Created.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	**	26-oct-04 (thoda04) Bug 113349
	**	    Use the datetime's timezone, not the current datetime,
	**	    when converting the datetime to the correct local time.
	**	    The daylight saving time might be different between
	**	    the datetime value and the current time-of-day value.
	*/
	
	
internal class DateFormat
{
	private string DateFormatPattern;
	private readonly System.TimeSpan localTimeZone = System.TimeSpan.Zero;
	private readonly System.TimeSpan gmtTimeZone;  // will have timezone from GMT
	// For example:
	//    If you are in EST, gmtTimeZone == -5.
	//    If you are in PST, gmtTimeZone == -8.
	private System.TimeZone tz;  // local TimeZone or null (for GMT)



	public DateFormat(string format) : this(format, "GMT")
	{
	}

	public DateFormat(string format, string newZone)
	{
		if (newZone == null  ||
			(newZone != "GMT"  &&
			 newZone != "LOCAL"))
			throw new ArgumentException();

		DateFormatPattern = format;
		gmtTimeZone = 
			System.TimeZone.CurrentTimeZone.GetUtcOffset(
			         System.DateTime.Now);  // timezone from GMT

		if (newZone == "LOCAL")
		{
			TimeZone = localTimeZone;
			tz = System.TimeZone.CurrentTimeZone;
		}
		else
			TimeZone = gmtTimeZone;
	}

	private System.TimeSpan _UtcOffset;  // timezone offset from server's timezone
	                      // if Ingres and EST or PST then
	                      // TimeZone has values for -5 and -8 zones respectively.
	                      // If non-Ingres server uses local time always then
	                      // TimeZone has value 0 always.
	public System.TimeSpan TimeZone
	{
		get { return _UtcOffset; }
		set { _UtcOffset = value; }
	} // TimeZone

	public System.TimeSpan LocalTimeZone
	{
		get { return localTimeZone; }
	} // LocalTimeZone

	public System.TimeSpan GMTTimeZone
	{
		get { return gmtTimeZone; }
	} // GMTTimeZone

	public string Format
	{
		get { return DateFormatPattern; }
	} // Format

	public DateTime ToUtc(DateTime datetime)
	{
		if (TimeZone == System.TimeSpan.Zero)
			return datetime;

		return (datetime.ToUniversalTime());
//		return (datetime - TimeZone);
	}

	public string ToUtcString(DateTime datetime)
	{
		return ToUtc(datetime).ToString(this.Format);
	}

	public string ToUtcString(string datetimestr)
	{
		if (TimeZone == System.TimeSpan.Zero)
			return datetimestr;

		DateTime datetime = System.DateTime.Parse(datetimestr);
		return ToUtc(datetime).ToString(this.Format);
	}

	/*
	 * Adjust datetime from server's timezone to client's timezone.
	 */
	public DateTime ToLocal(DateTime datetime)
	{
		if (TimeZone == System.TimeSpan.Zero)
			return datetime;

		return (datetime.ToLocalTime());
//		return (datetime + TimeZone);
	}

	/*
	 * Adjust datetime from server's timezone to client's timezone
	 * and return as string.
	 */
	public string ToLocalString(DateTime datetime)
	{
		return ToLocal(datetime).ToString(this.Format);
	}

	/*
	 * Adjust datetime from client's timezone to server's timezone.
	 */
	public DateTime ToRemote(DateTime datetime)
	{
		if (TimeZone == System.TimeSpan.Zero)
			return datetime;

		return (datetime.ToUniversalTime());
//		return (datetime - TimeZone);
	}

	/*
			 * Adjust datetime from client's timezone to server's timezone
	 * and return as string.
	 */
	public string ToRemoteString(DateTime datetime)
	{
		return ToRemote(datetime).ToString(this.Format);
	}

	public string ToMillisecondString(DateTime datetime)
	{
		return datetime.ToString(this.Format.Substring(0,20)) +
			   String.Format("{0:000}", datetime.Millisecond);
	}

	public DateTime Parse(string datetimestr)
	{
		DateTime datetime = System.DateTime.Parse(datetimestr);
		// adjust for timezone difference between here and server
		return ToLocal(datetime);
	}

	public void setTimeZone( System.TimeZone timezone )
	{
		tz = timezone;
		this.TimeZone = (tz == null)? gmtTimeZone : localTimeZone;
	}

	/// <summary>
	/// Parse the string in the context of the TimeZone from setTimeZone.
	/// </summary>
	/// <param name="datetimestr"></param>
	/// <returns></returns>
	[CLSCompliant(false)]
	public DateTime parse( string datetimestr )
	{
		return Parse(datetimestr);
	}

	[CLSCompliant(false)]
	public String format( DateTime date )
	{
		if (this.TimeZone == gmtTimeZone)
			return this.ToUtcString( date );
		return this.ToLocalString( date );
	}

	[CLSCompliant(false)]
	public String format( TimeSpan time )
	{
		return time.ToString();
	}

}  // DateFormat class

}  // namespace
