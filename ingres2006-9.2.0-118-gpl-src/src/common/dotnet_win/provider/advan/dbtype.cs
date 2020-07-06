/*
** Copyright (c) 2002, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;

	/*
	** Name: dbtype.cs
	**
	** Description:
	**	Defines the data types for Ingres providers.
	**
	**
	** History:
	**	26-Aug-02 (thoda04)
	**	    Created.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	*/
	
	

namespace Ca.Ingres.Utility
{
	/*
	** Description:
	**	Defines the provider independent data types and utility methods.
	**
	** History:
	**	14-Nov-02 (thoda04)
	**	    Created.
	*/
	
	
	internal enum ProviderType
	{
		                          // codes are same as ODBC
		Binary           =  (-2),
		Boolean          =  (-7), // should never happen on retrieval
		VarBinary        =  (-3),
		LongVarBinary    =  (-4),
		Char             =    1,
		VarChar          =   12,
		LongVarChar      =  (-1),
		TinyInt          =  (-6),
		BigInt           =  (-5),
		Decimal          =    3,
		Numeric          =    2,  // always treated as decimal
		SmallInt         =    5,
		Integer          =    4,
		Real             =    7,  // synonym of Single
		Single           =    7,  // synonym of Real
		Double           =    8,
		DateTime         =   93,
		NChar            = (-95),
		NVarChar         = (-96),
		LongNVarChar     = (-97),
		Other            = 1111,
		DBNull           =    0,
	} // ProviderType

	internal class ProviderTypeMgr
	{
		internal static string GetDataTypeName(ProviderType i)
		{
			switch(i)
			{
				case ProviderType.BigInt:
					return IdMap.get(8, DbmsConst.intMap);   // integer8
				case ProviderType.Integer:
					return IdMap.get(4, DbmsConst.intMap);   // integer
				case ProviderType.SmallInt:
					return IdMap.get(2, DbmsConst.intMap);   // smallint
				case ProviderType.TinyInt:
					return IdMap.get(1, DbmsConst.intMap);   // tinyint

				case ProviderType.Double:
					return IdMap.get(8, DbmsConst.floatMap); // float8
				case ProviderType.Real:
					return IdMap.get(4, DbmsConst.floatMap); // real

				case ProviderType.Binary:
					return IdMap.get(DbmsConst.DBMS_TYPE_BYTE,
						DbmsConst.typeMap);                  // byte
				case ProviderType.Char:
					return IdMap.get(DbmsConst.DBMS_TYPE_CHAR,
						DbmsConst.typeMap);                  // char
				case ProviderType.DateTime:
					return IdMap.get(DbmsConst.DBMS_TYPE_DATE,
						DbmsConst.typeMap);                  // date
				case ProviderType.Decimal:
					return IdMap.get(DbmsConst.DBMS_TYPE_DECIMAL,
						DbmsConst.typeMap);                  // decimal
				case ProviderType.LongNVarChar:
					return IdMap.get(DbmsConst.DBMS_TYPE_LONG_NCHAR,
						DbmsConst.typeMap);                  // long nvarchar
				case ProviderType.LongVarBinary:
					return IdMap.get(DbmsConst.DBMS_TYPE_LONG_BYTE,
						DbmsConst.typeMap);                  // long byte
				case ProviderType.LongVarChar:
					return IdMap.get(DbmsConst.DBMS_TYPE_LONG_CHAR,
						DbmsConst.typeMap);                  // long varchar
				case ProviderType.NChar:
					return IdMap.get(DbmsConst.DBMS_TYPE_NCHAR,
						DbmsConst.typeMap);                  // nchar
				case ProviderType.NVarChar:
					return IdMap.get(DbmsConst.DBMS_TYPE_NVARCHAR,
						DbmsConst.typeMap);                  // nvarchar
				case ProviderType.VarBinary:
					return IdMap.get(DbmsConst.DBMS_TYPE_VARBYTE,
						DbmsConst.typeMap);                  // byte varying
				case ProviderType.VarChar:
					return IdMap.get(DbmsConst.DBMS_TYPE_VARCHAR,
						DbmsConst.typeMap);                  // varchar
				default:
					return "unknownDbType" + i.ToString();
			}
		}  // GetDataTypeName


		internal static Type GetNETType(ProviderType i)
		{
			switch(i)
			{
				case ProviderType.BigInt:
					return typeof(System.Int64);          // bigint
				case ProviderType.Integer:
					return typeof(System.Int32);          // integer
				case ProviderType.SmallInt:
					return typeof(System.Int16);          // smallint
				case ProviderType.TinyInt:
					return typeof(System.SByte);          // tinyint

				case ProviderType.Double:
					return typeof(System.Double);         // float8
				case ProviderType.Real:
					return typeof(System.Single);         // real

				case ProviderType.Binary:
					return  typeof(System.Byte[]);        // byte
				case ProviderType.Char:
					return  typeof(String);        // char
				case ProviderType.DateTime:
					return  typeof(System.DateTime);      // date
				case ProviderType.Decimal:
					return  typeof(System.Decimal);       // decimal
				case ProviderType.LongNVarChar:
					return  typeof(String);        // long nvarchar
				case ProviderType.LongVarBinary:
					return  typeof(System.Byte[]);        // long byte
				case ProviderType.LongVarChar:
					return  typeof(String);        // long varchar
				case ProviderType.NChar:
					return  typeof(String);        // nchar
				case ProviderType.NVarChar:
					return  typeof(String);        // nvarchar
				case ProviderType.VarBinary:
					return  typeof(System.Byte[]);        // byte varying
				case ProviderType.VarChar:
					return  typeof(String);        // varchar
				default:
					return typeof(System.Object);
			}
		}

		public static System.Data.DbType GetDbType(ProviderType i)
		{
			switch(i)
			{
				case ProviderType.BigInt:
					return  System.Data.DbType.Int64;         // bigint
				case ProviderType.Integer:
					return  System.Data.DbType.Int32;         // integer
				case ProviderType.SmallInt:
					return  System.Data.DbType.Int16;         // smallint
				case ProviderType.TinyInt:
					return  System.Data.DbType.SByte;         // tinyint

				case ProviderType.Double:
					return  System.Data.DbType.Double;        // float8
				case ProviderType.Real:
					return  System.Data.DbType.Single;        // real

				case ProviderType.Binary:
					return  System.Data.DbType.Binary;        // byte
				case ProviderType.Char:
					return  System.Data.DbType.String;        // char
				case ProviderType.DateTime:
					return  System.Data.DbType.DateTime;      // date
				case ProviderType.Decimal:
					return  System.Data.DbType.Decimal;       // decimal
				case ProviderType.LongNVarChar:
					return  System.Data.DbType.String;        // long nvarchar
				case ProviderType.LongVarBinary:
					return  System.Data.DbType.Binary;        // long byte
				case ProviderType.LongVarChar:
					return  System.Data.DbType.String;        // long varchar
				case ProviderType.NChar:
					return  System.Data.DbType.String;        // nchar
				case ProviderType.NVarChar:
					return  System.Data.DbType.String;        // nvarchar
				case ProviderType.VarBinary:
					return  System.Data.DbType.Binary;        // byte varying
				case ProviderType.VarChar:
					return  System.Data.DbType.String;        // varchar
				default:
					return System.Data.DbType.Object;
			}
		}

		public static ProviderType GetProviderType(System.Data.DbType i)
		{
			switch(i)
			{
				case System.Data.DbType.AnsiString:
					return ProviderType.VarChar;         // varchar
				case System.Data.DbType.AnsiStringFixedLength:
					return ProviderType.Char;            // char

				case System.Data.DbType.Binary:
					return ProviderType.VarBinary;       // byte varying

				case System.Data.DbType.Byte:
					return ProviderType.TinyInt;        // keep as integer

				case System.Data.DbType.Boolean:
				case System.Data.DbType.SByte:
					return ProviderType.TinyInt;         // tinyint

				case System.Data.DbType.Currency:
				case System.Data.DbType.Decimal:
					return ProviderType.Decimal;         // decimal

				case System.Data.DbType.Date:
				case System.Data.DbType.Time:
				case System.Data.DbType.DateTime:
					return ProviderType.DateTime;        // date

				case System.Data.DbType.Double:
					return ProviderType.Double;          // float8
				case System.Data.DbType.Single:
					return ProviderType.Real;            // real

				case System.Data.DbType.Int16:
					return ProviderType.SmallInt;        // smallint
				case System.Data.DbType.Int32:
					return ProviderType.Integer;         // integer
				case System.Data.DbType.Int64:
					return ProviderType.BigInt;          // bigint

				case System.Data.DbType.String:
					return ProviderType.NVarChar;        // nvarchar
				case System.Data.DbType.StringFixedLength:
					return ProviderType.NChar;           // nchar

				case System.Data.DbType.UInt16:
					return ProviderType.Integer;         // unsigned smallint

				case System.Data.DbType.UInt32:          // unsigned 32 bit
				case System.Data.DbType.UInt64:          // unsigned 64 bit
				case System.Data.DbType.VarNumeric:
					return ProviderType.Decimal;

				default:
					return ProviderType.Other;
			}
		}

		internal static bool isLong(ProviderType providerType)
		{
			switch(providerType)
			{
				case ProviderType.LongNVarChar:   // long nvarchar
				case ProviderType.LongVarBinary:  // long byte
				case ProviderType.LongVarChar:    // long varchar
					return true;
			}
			return false;
		}

		internal static bool hasPrecision(ProviderType providerType)
		{
			switch(providerType)
			{
				case ProviderType.BigInt:      // BigInt
				case ProviderType.Integer:     // integer
				case ProviderType.SmallInt:    // smallint
				case ProviderType.TinyInt:     // tinyint
				case ProviderType.Double:      // float8
				case ProviderType.Real:        // real
				case ProviderType.DateTime:    // date
				case ProviderType.Decimal:     // decimal
					return true;
			}
			return false;
		}

	} // class
}  // namespace


