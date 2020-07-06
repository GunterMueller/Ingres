/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

namespace Ca.Ingres.Utility
{
	/*
	** Name: gcferr.cs
	**
	** Description:
	**	Defines interface providing GCF error codes and messages.
	**
	**  classes
	**
	**	GcfErr
	**
	** History:
	**	10-Sep-99 (gordy)
	**	    Created.
	**	29-Sep-99 (gordy)
	**	    Added support for BLOBs: E_JD000B_BLOB_IO,
	**	    E_JD000C_BLOB_ACTIVE, E_JD000D_BLOB_DONE.
	**	26-Oct-99 (gordy)
	**	    Added ODBC escape parsing: E_JD000E_UNMATCHED.
	**	16-Nov-99 (gordy)
	**	    Added query timeouts: E_JD000F_TIMEOUT, and the
	**	    associated API error E_AP0009_QUERY_CANCELLED.
	**	20-Dec-99 (rajus01)
	**	    Implemented DatabaseMetaData: E_JD0010_CATALOG_UNSUPPORTED,
	**	    E_JD0011_SQLTYPE_UNSUPPORTED.
	**	21-Dec-99 (gordy)
	**	    Added generic unsupported E_JD0012_UNSUPPORTED.
	**	25-Jan-00 (rajus01)
	**	    Added ResultsSet closed E_JD0013_RESULTSET_CLOSED.
	**	31-Jan-00 (rajus01)
	**	    Added column not found E_JD0014_INVALID_COLUMN_NAME.
	**	 4-Feb-00 (gordy)
	**	    Added sequencing error E_JD0015_SEQUENCING.
	**	13-Jun-00 (gordy)
	**	    Added procedure call syntax error E_JD0016_CALL_SYNTAX.
	**	15-Nov-00 (gordy)
	**	    Added support for 2.0 extensions.
	**	31-Jan-01 (gordy)
	**	    Added invalid date format E_JD0019_INVALID_DATE.
	**      29-Aug-01 (loera01) SIR 105641
	**          Added invalid output parameter: E_JD0020_INVALID_OUT_PARAM.
	**          Added no result set returned: E_JD0021_NO_RESULT_SET.
	**          Added result set not permitted: E_JD0022_RESULT_SET_NOT_PERMITTED.
	**	11-Sep-02 (gordy)
	**	    Moved to GCF.  Renamed to remove specific product reference.
	**	    Translated error codes into new GCF error codes.
	**	31-Oct-02 (gordy)
	**	    Changed naming conventions to be more compatible with Ingres.
	**	    Added error code constants.
	**	26-Dec-02 (gordy)
	**	    Added GC4009_BAD_CHARSET.
	**	14-Feb-03 (gordy)
	**	    Added GC401F_XACT_STATE, GC4020_NO_PARAM.
	**	 9-Dec-03 (thoda04)
	**	    Ported to .NET.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	*/


	/*
	** Name: GcfErr
	**
	** Description:
	**	Interface defining GCF error codes and messages.
	**
	**  Constants
	**
	**	GCF_ERR_CLASS				GCF error class.
	**	E_GCF_MASK				GCF error code mask.
	**
	**	E_GC4000_BAD_URL			GCF error codes.
	**	E_GC4001_CONNECT_ERR
	**	E_GC4002_PROTOCOL_ERR
	**	E_GC4003_CONNECT_FAIL
	**	E_GC4004_CONNECTION_CLOSED
	**	E_GC4005_SEQUENCING
	**	E_GC4006_TIMEOUT
	**	E_GC4007_BLOB_IO
	**	E_GC4008_SERVER_ABORT
	**	E_GC4009_BAD_CHARSET
	**	E_GC4010_PARAM_VALUE
	**	E_GC4011_INDEX_RANGE
	**	E_GC4012_INVALID_COLUMN_NAME
	**	E_GC4013_UNMATCHED
	**	E_GC4014_CALL_SYNTAX
	**	E_GC4015_INVALID_OUT_PARAM
	**	E_GC4016_RS_CHANGED
	**	E_GC4017_NO_RESULT_SET
	**	E_GC4018_RESULT_SET_NOT_PERMITTED
	**	E_GC4019_UNSUPPORTED
	**	E_GC401A_CONVERSION_ERR
	**	E_GC401B_INVALID_DATE
	**	E_GC401C_BLOB_DONE
	**	E_GC401D_RESULTSET_CLOSED
	**	E_GC401E_CHAR_ENCODE
	**	E_GC401F_XACT_STATE
	**	E_GC4020_NO_PARAM
	**
	**	ERR_GC4000_BAD_URL			GCF error info objects.
	**	ERR_GC4001_CONNECT_ERR
	**	ERR_GC4002_PROTOCOL_ERR
	**	ERR_GC4003_CONNECT_FAIL
	**	ERR_GC4004_CONNECTION_CLOSED
	**	ERR_GC4005_SEQUENCING
	**	ERR_GC4006_TIMEOUT
	**	ERR_GC4007_BLOB_IO
	**	ERR_GC4008_SERVER_ABORT
	**	ERR_GC4009_BAD_CHARSET
	**	ERR_GC4010_PARAM_VALUE
	**	ERR_GC4011_INDEX_RANGE
	**	ERR_GC4012_INVALID_COLUMN_NAME
	**	ERR_GC4013_UNMATCHED
	**	ERR_GC4014_CALL_SYNTAX
	**	ERR_GC4015_INVALID_OUT_PARAM
	**	ERR_GC4016_RS_CHANGED
	**	ERR_GC4017_NO_RESULT_SET
	**	ERR_GC4018_RESULT_SET_NOT_PERMITTED
	**	ERR_GC4019_UNSUPPORTED
	**	ERR_GC401A_CONVERSION_ERR
	**	ERR_GC401B_INVALID_DATE
	**	ERR_GC401C_BLOB_DONE
	**	ERR_GC401D_RESULTSET_CLOSED
	**	ERR_GC401E_CHAR_ENCODE
	**	ERR_GC401F_XACT_STATE
	**	ERR_GC4020_NO_PARAM
	**
	** History:
	**	10-Sep-99 (gordy)
	**	    Created.
	**	29-Sep-99 (gordy)
	**	    Added support for BLOBs: E_JD000B_BLOB_IO,
	**	    E_JD000C_BLOB_ACTIVE, E_JD000D_BLOB_DONE.
	**	26-Oct-99 (gordy)
	**	    Added ODBC escape parsing: E_JD000E_UNMATCHED.
	**	16-Nov-99 (gordy)
	**	    Added query timeouts: E_JD000F_TIMEOUT, and the
	**	    associated API error E_AP0009_QUERY_CANCELLED.
	**	20-Dec-99 (rajus01)
	**	    Implemented DatabaseMetaData: E_JD0010_CATALOG_UNSUPPORTED,
	**	    E_JD0011_SQLTYPE_UNSUPPORTED.
	**	21-Dec-99 (gordy)
	**	    Added generic unsupported E_JD0012_UNSUPPORTED.
	**	25-Jan-00 (rajus01)
	**	    Added ResultsSet closed E_JD0013_RESULTSET_CLOSED.
	**	31-Jan-00 (rajus01)
	**	    Added column not found E_JD0014_INVALID_COLUMN_NAME.
	**	 4-Feb-00 (gordy)
	**	    Added sequencing error E_JD0015_SEQUENCING.
	**	13-Jun-00 (gordy)
	**	    Added procedure call syntax error E_JD0016_CALL_SYNTAX.
	**	15-Nov-00 (gordy)
	**	    Added E_JD0017_RS_CHANGED, E_JD0018_UNKNOWN_TIMEZONE.
	**	31-Jan-01 (gordy)
	**	    Added invalid date format E_JD0019_INVALID_DATE.
	**	11-Sep-02 (gordy)
	**	    Renamed to remove specific product reference.
	**	    Translated error codes into new GCF error codes.
	**	31-Oct-02 (gordy)
	**	    Added error code constants (E_GCXXXX_*) to be more consistent
	**	    with Ingres error coding standards.  The error message objects
	**	    now are of the form ERR_GCXXXX_*.
	**	26-Dec-02 (gordy)
	**	    Added GC4009_BAD_CHARSET.
	**	14-Feb-03 (gordy)
	**	    Added GC401F_XACT_STATE, GC4020_NO_PARAM.
	*/

	internal class GcfErr

	{
		const int	GCF_ERR_CLASS		= 12;
		const int	E_GCF_MASK   		= GCF_ERR_CLASS << 16;

		const int	E_GC4000_BAD_URL            = E_GCF_MASK | 0x4000;
		const int	E_GC4001_CONNECT_ERR        = E_GCF_MASK | 0x4001;
		const int	E_GC4002_PROTOCOL_ERR       = E_GCF_MASK | 0x4002;
		const int	E_GC4003_CONNECT_FAIL       = E_GCF_MASK | 0x4003;
		const int	E_GC4004_CONNECTION_CLOSED  = E_GCF_MASK | 0x4004;
		const int	E_GC4005_SEQUENCING         = E_GCF_MASK | 0x4005;
		const int	E_GC4006_TIMEOUT            = E_GCF_MASK | 0x4006;
		const int	E_GC4007_BLOB_IO            = E_GCF_MASK | 0x4007;
		public
		const int	E_GC4008_SERVER_ABORT       = E_GCF_MASK | 0x4008;
		const int	E_GC4009_BAD_CHARSET        = E_GCF_MASK | 0x4009;

		const int	E_GC4010_PARAM_VALUE        = E_GCF_MASK | 0x4010;
		const int	E_GC4011_INDEX_RANGE        = E_GCF_MASK | 0x4011;
		const int	E_GC4012_INVALID_COLUMN_NAME= E_GCF_MASK | 0x4012;
		const int	E_GC4013_UNMATCHED          = E_GCF_MASK | 0x4013;
		const int	E_GC4014_CALL_SYNTAX        = E_GCF_MASK | 0x4014;
		const int	E_GC4015_INVALID_OUT_PARAM  = E_GCF_MASK | 0x4015;
		const int	E_GC4016_RS_CHANGED         = E_GCF_MASK | 0x4016;
		const int	E_GC4017_NO_RESULT_SET      = E_GCF_MASK | 0x4017;
		const int	E_GC4018_RESULT_SET_NOT_PERMITTED= E_GCF_MASK | 0x4018;
		const int	E_GC4019_UNSUPPORTED        = E_GCF_MASK | 0x4019;
		const int	E_GC401A_CONVERSION_ERR     = E_GCF_MASK | 0x401A;
		const int	E_GC401B_INVALID_DATE       = E_GCF_MASK | 0x401B;
		const int	E_GC401C_BLOB_DONE          = E_GCF_MASK | 0x401C;
		const int	E_GC401D_RESULTSET_CLOSED   = E_GCF_MASK | 0x401D;
		const int	E_GC401E_CHAR_ENCODE        = E_GCF_MASK | 0x401E;
		const int	E_GC401F_XACT_STATE         = E_GCF_MASK | 0x401F;
		const int	E_GC4020_NO_PARAM           = E_GCF_MASK | 0x4020;

		const int	E_GC4201_CONNECTION_STRING_BAD_KEY      = E_GCF_MASK | 0x4201;
		const int	E_GC4202_CONNECTION_STRING_BAD_VALUE    = E_GCF_MASK | 0x4202;
		const int	E_GC4203_CONNECTION_STRING_BAD_QUOTE    = E_GCF_MASK | 0x4203;
		const int	E_GC4204_CONNECTION_STRING_DUP_EQUAL    = E_GCF_MASK | 0x4204;
		const int	E_GC4205_CONNECTION_STRING_MISSING_KEY  = E_GCF_MASK | 0x4205;
		const int	E_GC4206_CONNECTION_STRING_MISSING_QUOTE= E_GCF_MASK | 0x4206;


		internal readonly static SqlEx.ErrInfo	ERR_GC4000_BAD_URL = 
			new SqlEx.ErrInfo( E_GC4000_BAD_URL, "50000", 
			ErrRsrc.E_GC4000, "E_GC4000_BAD_URL" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4001_CONNECT_ERR = 
			new SqlEx.ErrInfo( E_GC4001_CONNECT_ERR, "08001", 
			ErrRsrc.E_GC4001, "E_GC4001_CONNECT_ERR" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4002_PROTOCOL_ERR = 
			new SqlEx.ErrInfo( E_GC4002_PROTOCOL_ERR, "40003", 
			ErrRsrc.E_GC4002, "E_GC4002_PROTOCOL_ERR" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4003_CONNECT_FAIL = 
			new SqlEx.ErrInfo( E_GC4003_CONNECT_FAIL, "40003", 
			ErrRsrc.E_GC4003, "E_GC4003_CONNECT_FAIL" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4004_CONNECTION_CLOSED = 
			new SqlEx.ErrInfo( E_GC4004_CONNECTION_CLOSED, "08003", 
			ErrRsrc.E_GC4004, "E_GC4004_CONNECTION_CLOSED" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4005_SEQUENCING = 
			new SqlEx.ErrInfo( E_GC4005_SEQUENCING, "5000R", 
			ErrRsrc.E_GC4005, "E_GC4005_SEQUENCING" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4006_TIMEOUT = 
			new SqlEx.ErrInfo( E_GC4006_TIMEOUT, "5000R", 
			ErrRsrc.E_GC4006, "E_GC4006_TIMEOUT" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4007_BLOB_IO = 
			new SqlEx.ErrInfo( E_GC4007_BLOB_IO, "50000", 
			ErrRsrc.E_GC4007, "E_GC4007_BLOB_IO" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4008_SERVER_ABORT = 
			new SqlEx.ErrInfo( E_GC4008_SERVER_ABORT, "40003", 
			ErrRsrc.E_GC4008, "E_GC4008_SERVER_ABORT" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4009_BAD_CHARSET = 
			new SqlEx.ErrInfo( E_GC4009_BAD_CHARSET, "50000", 
			ErrRsrc.E_GC4009, "E_GC4009_BAD_CHARSET" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4010_PARAM_VALUE = 
			new SqlEx.ErrInfo( E_GC4010_PARAM_VALUE, "22023", 
			ErrRsrc.E_GC4010, "E_GC4010_PARAM_VALUE" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4011_INDEX_RANGE = 
			new SqlEx.ErrInfo( E_GC4011_INDEX_RANGE, "50000", 
			ErrRsrc.E_GC4011, "E_GC4011_INDEX_RANGE" );

		internal readonly static SqlEx.ErrInfo 	ERR_GC4012_INVALID_COLUMN_NAME = 
			new SqlEx.ErrInfo( E_GC4012_INVALID_COLUMN_NAME, "5000R", 
			ErrRsrc.E_GC4012, "E_GC4012_INVALID_COLUMN_NAME" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4013_UNMATCHED = 
			new SqlEx.ErrInfo( E_GC4013_UNMATCHED, "42000", 
			ErrRsrc.E_GC4013, "E_GC4013_UNMATCHED" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4014_CALL_SYNTAX = 
			new SqlEx.ErrInfo( E_GC4014_CALL_SYNTAX, "42000", 
			ErrRsrc.E_GC4014, "E_GC4014_CALL_SYNTAX" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4015_INVALID_OUT_PARAM = 
			new SqlEx.ErrInfo( E_GC4015_INVALID_OUT_PARAM, "22023", 
			ErrRsrc.E_GC4015, "E_GC4015_INVALID_OUT_PARAM" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4016_RS_CHANGED = 
			new SqlEx.ErrInfo( E_GC4016_RS_CHANGED, "22023", 
			ErrRsrc.E_GC4016, "E_GC4016_RS_CHANGED" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4017_NO_RESULT_SET = 
			new SqlEx.ErrInfo( E_GC4017_NO_RESULT_SET, "07005", 
			ErrRsrc.E_GC4017, "E_GC4017_NO_RESULT_SET" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4018_RESULT_SET_NOT_PERMITTED = 
			new SqlEx.ErrInfo( E_GC4018_RESULT_SET_NOT_PERMITTED, "21000", 
			ErrRsrc.E_GC4018, "E_GC4018_RESULT_SET_NOT_PERMITTED" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4019_UNSUPPORTED = 
			new SqlEx.ErrInfo( E_GC4019_UNSUPPORTED, "5000R", 
			ErrRsrc.E_GC4019, "E_GC4019_UNSUPPORTED" );

		internal readonly static SqlEx.ErrInfo	ERR_GC401A_CONVERSION_ERR = 
			new SqlEx.ErrInfo( E_GC401A_CONVERSION_ERR, "07006", 
			ErrRsrc.E_GC401A, "E_GC401A_CONVERSION_ERR" );

		internal readonly static SqlEx.ErrInfo	ERR_GC401B_INVALID_DATE = 
			new SqlEx.ErrInfo( E_GC401B_INVALID_DATE, "22007", 
			ErrRsrc.E_GC401B, "E_GC401B_INVALID_DATE" );

		internal readonly static SqlEx.ErrInfo	ERR_GC401C_BLOB_DONE = 
			new SqlEx.ErrInfo( E_GC401C_BLOB_DONE, "50000", 
			ErrRsrc.E_GC401C, "E_GC401C_BLOB_DONE" );

		internal readonly static SqlEx.ErrInfo	ERR_GC401D_RESULTSET_CLOSED = 
			new SqlEx.ErrInfo( E_GC401D_RESULTSET_CLOSED, "5000R", 
			ErrRsrc.E_GC401D, "E_GC401D_RESULTSET_CLOSED" );

		internal readonly static SqlEx.ErrInfo	ERR_GC401E_CHAR_ENCODE = 
			new SqlEx.ErrInfo( E_GC401E_CHAR_ENCODE, "07006", 
			ErrRsrc.E_GC401E, "E_GC401E_CHAR_ENCODE" );

		internal readonly static SqlEx.ErrInfo	ERR_GC401F_XACT_STATE = 
			new SqlEx.ErrInfo( E_GC401F_XACT_STATE, "25000", 
			ErrRsrc.E_GC401F, "E_GC401F_XACT_STATE" );

		internal readonly static SqlEx.ErrInfo	ERR_GC4020_NO_PARAM = 
			new SqlEx.ErrInfo( E_GC4020_NO_PARAM, "07001", 
			ErrRsrc.E_GC4020, "E_GC4020_NO_PARAM" );



		internal readonly static SqlEx.ErrInfo	ERR_GC4201_CONNECTION_STRING_BAD_KEY =
			new SqlEx.ErrInfo(E_GC4201_CONNECTION_STRING_BAD_KEY, "42000",
			ErrRsrc.E_GC4201, "E_GC4201_CONNECTION_STRING_BAD_KEY");

		internal readonly static SqlEx.ErrInfo	ERR_GC4202_CONNECTION_STRING_BAD_VALUE =
			new SqlEx.ErrInfo(E_GC4202_CONNECTION_STRING_BAD_VALUE, "42000",
			ErrRsrc.E_GC4202, "E_GC4202_CONNECTION_STRING_BAD_VALUE");

		internal readonly static SqlEx.ErrInfo	ERR_GC4203_CONNECTION_STRING_BAD_QUOTE =
			new SqlEx.ErrInfo(E_GC4203_CONNECTION_STRING_BAD_QUOTE, "42000",
			ErrRsrc.E_GC4203, "E_GC4203_CONNECTION_STRING_BAD_QUOTE");

		internal readonly static SqlEx.ErrInfo	ERR_GC4204_CONNECTION_STRING_DUP_EQUAL =
			new SqlEx.ErrInfo(E_GC4204_CONNECTION_STRING_DUP_EQUAL, "42000",
			ErrRsrc.E_GC4204, "E_GC4204_CONNECTION_STRING_DUP_EQUAL");

		internal readonly static SqlEx.ErrInfo	ERR_GC4205_CONNECTION_STRING_MISSING_KEY =
			new SqlEx.ErrInfo(E_GC4205_CONNECTION_STRING_MISSING_KEY, "42000",
			ErrRsrc.E_GC4205, "E_GC4205_CONNECTION_STRING_MISSING_KEY");

		internal readonly static SqlEx.ErrInfo	ERR_GC4206_CONNECTION_STRING_MISSING_QUOTE =
			new SqlEx.ErrInfo(E_GC4206_CONNECTION_STRING_MISSING_QUOTE, "42000",
			ErrRsrc.E_GC4206, "E_GC4206_CONNECTION_STRING_MISSING_QUOTE");


		internal readonly static SqlEx.ErrInfo	ERR_AP0009_QUERY_CANCELLED =
			new SqlEx.ErrInfo(DbmsConst.E_AP0009_QUERY_CANCELLED, "5000R",
			ErrRsrc.E_AP0009, "E_AP0009_QUERY_CANCELLED");


		internal readonly static System.Globalization.CultureInfo InvariantCulture =
			System.Globalization.CultureInfo.InvariantCulture;

		internal static string ToInvariantLower(string str)
		{
			if (str == null)
				return null;
			return str.ToLower(InvariantCulture);
		}

		internal static string ToInvariantUpper(string str)
		{
			if (str == null)
				return null;
			return str.ToUpper(InvariantCulture);
		}

		internal static char ToInvariantLower(char c)
		{
			return System.Char.ToLower(c, InvariantCulture);
		}

		internal static char ToInvariantUpper(char c)
		{
			return System.Char.ToUpper(c, InvariantCulture);
		}

		public static string toHexString(int val)
		{
			return toHexStringTrim(val.ToString("x8"));
		}

		public static string toHexString(short val)
		{
			return toHexStringTrim(val.ToString("x4"));
		}

		public static string toHexString(byte val)
		{
			return val.ToString("x2");
		}

		/// <summary>
		/// Trim leading zeros from the string.
		/// </summary>
		/// <param name="str"></param>
		private static string toHexStringTrim(string str)
		{
			int len = str.Length - 2;
			int i;
			for (i = 0; i < len; i+=2)
			{
				if (str[i] == '0'  &&  str[i+1] == '0')
					continue;
			}
			return str.Substring(i, str.Length - i);
		}


	} // GcfErr

}  // namespace
