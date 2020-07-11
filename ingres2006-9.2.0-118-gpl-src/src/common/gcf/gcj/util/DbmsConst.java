/*
** Copyright (c) 1999, 2003 Ingres Corporation All Rights Reserved.
*/

package com.ingres.gcf.util;

/*
** Name: DbmsConst.java
**
** Description:
**	Defines interface providing DBMS Server constants.
**
**  Interfaces
**
**	DbmsConst
**
** History:
**	 8-Sep-99 (gordy)
**	    Created.
**	18-Apr-00 (gordy)
**	    Extracted DBMS constants from EdbcMsgConst.java.
**	10-May-01 (gordy)
**	    Added UCS2 datatypes.
**	20-Feb-01 (gordy)
**	    Added DBMS protocol levels.
**	11-Sep-02 (gordy)
**	    Moved to GCF.
**	31-Oct-02 (gordy)
**	    Added constants for dbmsinfo(), iidbcapabilities and AIF errors.
**	20-Mar-03 (gordy)
**	    Added DBMS_DBCAP_SAVEPOINT.
**	 3-Jul-03 (gordy)
**	    Added DBMS_TYPE_LOGKEY and DBMS_TYPE_TBLKEY.
**	15-Mar-04 (gordy)
**	    Support BIGINT.
**	28-May-04 (gordy)
**	    Add max column lengths for NCS columns.
**	10-May-05 (gordy)
**	    Added DBMS_DBINFO_SERVER.
**	16-Jun-06 (gordy)
**	    ANSI Date/Time support.
**	14-Sep-06 (lunbr01) 
**	    Add DBMS_SQL_LEVEL_IMS_VSAM to identify EDBC IMS & VSAM SQL
**	    restrictions.
**	22-Sep-06 (gordy)
**	    Updated keywords for Ingres and ANSI date types.
**	15-Nov-06 (gordy)
**	    LOB Locator support.
*/


/*
** Name: DbmsConst
**
** Description:
**	Interface defining Ingres DBMS constants.
**
**  Constants:
**
**	DBMS_API_PROTO_0	Base DBMS API protocol level.
**	DBMS_API_PROTO_1	DBMS API protocol: decimal, byte, varbyte
**	DBMS_API_PROTO_2	DBMS API protocol: nchar, nvarchar
**	DBMS_API_PROTO_3	DBMS API protocol: bigint
**	DBMS_API_PROTO_4	DBMS API protocol: ANSI date/time
**	DBMS_API_PROTO_5	DBMS_API protocol: LOB Locators
**
**	DBMS_COL_MAX		Max length of CHAR/VARCHAR column.
**	DBMS_OI10_IDENT_LEN	Current identifier length,
**	DBMS_ING64_IDENT_LEN	Old identifier length.
**	DBMS_TBLS_INSEL		Max tables in select.
**
**	DBMS_DBINFO_VERSION	Entries for dbmsinfo().
**	DBMS_DBINFO_SERVRE
**
**	DBMS_DBCAP_DBMS_TYPE	Entries for iidbcapabilities.
**	DBMS_DBCAP_ING_SQL_LVL
**	DBMS_DBCAP_OPEN_SQL_LVL
**	DBMS_DBCAP_STD_CAT_LVL
**	DBMS_DBCAP_NAME_CASE
**	DBMS_DBCAP_DELIM_CASE
**	DBMS_DBCAP_ESCAPE
**	DBMS_DBCAP_ESCAPE_CHAR
**	DBMS_DBCAP_IDENT_CHAR
**	DBMS_DBCAP_NULL_SORTING
**	DBMS_DBCAP_OSQL_DATES
**	DBMS_DBCAP_OUTER_JOIN
**	DBMS_DBCAP_OWNER_NAME
**	DBMS_DBCAP_SAVEPOINT
**	DBMS_DBCAP_SQL92_LEVEL
**	DBMS_DBCAP_UCS2_TYPES
**	DBMS_DBCAP_UNION
**	DBMS_DBCAP_MAX_SCH_NAME
**	DBMS_DBCAP_MAX_USR_NAME
**	DBMS_DBCAP_MAX_TAB_NAME;
**	DBMS_DBCAP_MAX_COL_NAME
**	DBMS_DBCAP_MAX_BYT_LIT
**	DBMS_DBCAP_MAX_CHR_LIT
**	DBMS_DBCAP_MAX_BYT_COL
**	DBMS_DBCAP_MAX_VBY_COL
**	DBMS_DBCAP_MAX_CHR_COL
**	DBMS_DBCAP_MAX_VCH_COL
**	DBMS_DBCAP_MAX_NCHR_COL
**	DBMS_DBCAP_MAX_NVCH_COL
**	DBMS_DBCAP_MAX_ROW_LEN
**	DBMS_DBCAP_MAX_COLUMNS
**	DBMS_DBCAP_MAX_STMTS
**
**	DBMS_TYPE_INGRES	Values for iidbcapabilities.
**	DBMS_SQL_LEVEL_IMS_VSAM
**	DBMS_SQL_LEVEL_OI10
**	DBMS_SQL_LEVEL_ANSI_DT
**	DBMS_NAME_CASE_LOWER
**	DBMS_NAME_CASE_MIXED
**	DBMS_NAME_CASE_UPPER
**	DBMS_NULL_SORT_FIRST
**	DBMS_NULL_SORT_LAST
**	DBMS_NULL_SORT_HIGH
**	DBMS_NULL_SORT_LOW
**	DBMS_SQL92_NONE
**	DBMS_SQL92_ENTRY
**	DBMS_SQL92_FIPS
**	DBMS_SQL92_INTERMEDIATE
**	DBMS_SQL92_FULL
**	DBMS_UNION_ALL
**
**	DBMS_TYPE_IDATE		Datatypes
**	DBMS_TYPE_ADATE
**	DBMS_TYPE_MONEY
**	DBMS_TYPE_TMWO
**	DBMS_TYPE_TMTZ
**	DBMS_TYPE_TIME
**	DBMS_TYPE_TSWO
**	DBMS_TYPE_DECIMAL
**	DBMS_TYPE_LOGKEY
**	DBMS_TYPE_TBLKEY
**	DBMS_TYPE_TSTZ
**	DBMS_TYPE_TS
**	DBMS_TYPE_CHAR
**	DBMS_TYPE_VARCHAR
**	DBMS_TYPE_LONG_CHAR
**	DBMS_TYPE_LCLOC
**	DBMS_TYPE_BYTE
**	DBMS_TYPE_VARBYTE
**	DBMS_TYPE_LONG_BYTE
**	DBMS_TYPE_LBLOC
**	DBMS_TYPE_NCHAR
**	DBMS_TYPE_NVARCHAR
**	DBMS_TYPE_LONG_NCHAR
**	DBMS_TYPE_LNLOC
**	DBMS_TYPE_INT
**	DBMS_TYPE_FLOAT
**	DBMS_TYPE_C
**	DBMS_TYPE_INTYM
**	DBMS_TYPE_INTDS
**	DBMS_TYPE_TEXT
**	DBMS_TYPE_LONG_TEXT
**
**	AIF_ERR_CLASS		AIF Error code info.
**	E_AIF_MASK
**	E_AP0009_QUERY_CANCELLED
**
**  Public Data:
**
**	typeMap			Mapping table for most DBMS datatypes.
**	intMap			Mapping table for DBMS integer types.
**	floatMap		Mapping table for DBMS float types.
**
** History:
**	 8-Sep-99 (gordy)
**	    Created.
**	11-Nov-99 (rajus01)
**	    Implement DatabaseMetaData: Add DBMS_NAME_LEN, DBMS_TBLS_INSEL. 
**	10-May-01 (gordy)
**	    Added UCS2 datatypes: DBMS_TYPE_NCHAR, DBMS_TYPE_NVARCHAR,
**	    DBMS_TYPE_LONG_NCHAR.
**	20-Feb-01 (gordy)
**	    Added DBMS protocol levels: DBMS_PROTO_[0,1,2].
**	31-Oct-02 (gordy)
**	    Added constants for dbmsinfo(), iidbcapabilities and AIF errors.
**	20-Mar-03 (gordy)
**	    Added DBMS_DBCAP_SAVEPOINT.
**	 3-Jul-03 (gordy)
**	    Added DBMS_TYPE_LOGKEY and DBMS_TYPE_TBLKEY.
**	29-Jan-04 (fei wei) Bug # 111723; Startrak # EDJDBC94
**	    Added  dataTypeMap.
**	15-Mar-04 (gordy)
**	    Added DBMS_API_PROTO_3 for BIGINT support.
**	28-May-04 (gordy)
**	    Add max column lengths for NCS columns.
**	10-May-05 (gordy)
**	    Added dbmsinfo key DBMS_DBINFO_SERVER.
**	16-Jun-06 (gordy)
**	    ANSI Date/Time support.
**	15-Nov-06 (gordy)
**	    LOB Locator support.
*/

public interface
DbmsConst
{

    /*
    ** Server/DBMS API protocol levels.
    */
    byte	DBMS_API_PROTO_0	= 0;	// Base level.
    byte	DBMS_API_PROTO_1	= 1;	// Added decimal, byte, varbyte
    byte	DBMS_API_PROTO_2	= 2;	// Added nchar, nvarchar
    byte	DBMS_API_PROTO_3	= 3;	// Added bigint
    byte	DBMS_API_PROTO_4	= 4;	// Added ANSI date/time
    byte	DBMS_API_PROTO_5	= 5;	// Added LOB Locators

    /*
    ** Misc constants.
    */
    short	DBMS_COL_MAX		= 2000;	// Max col length.
    short	DBMS_OI10_IDENT_LEN	= 32;	// Max identifier length,
    short	DBMS_ING64_IDENT_LEN	= 20;	// Old identifier length.
    short	DBMS_TBLS_INSEL		= 30;	// Max tables in select.

    /*
    ** Entries for dbmsinfo()
    */
    String	DBMS_DBINFO_VERSION	= "_version";
    String	DBMS_DBINFO_SERVER	= "ima_server";

    /*
    ** Entries for iidbcapabilities.
    */
    String	DBMS_DBCAP_DBMS_TYPE	= "DBMS_TYPE";
    String	DBMS_DBCAP_ING_SQL_LVL	= "INGRES/SQL_LEVEL";
    String	DBMS_DBCAP_OPEN_SQL_LVL	= "OPEN/SQL_LEVEL";
    String	DBMS_DBCAP_STD_CAT_LVL	= "STANDARD_CATALOG_LEVEL";
    String	DBMS_DBCAP_NAME_CASE	= "DB_NAME_CASE";
    String	DBMS_DBCAP_DELIM_CASE	= "DB_DELIMITED_CASE";
    String	DBMS_DBCAP_ESCAPE	= "ESCAPE";
    String	DBMS_DBCAP_ESCAPE_CHAR	= "ESCAPE_CHAR";
    String	DBMS_DBCAP_IDENT_CHAR	= "IDENT_CHAR";
    String	DBMS_DBCAP_NULL_SORTING	= "NULL_SORTING";
    String	DBMS_DBCAP_OSQL_DATES	= "OPEN_SQL_DATES";
    String	DBMS_DBCAP_OUTER_JOIN	= "OUTER_JOIN";
    String	DBMS_DBCAP_OWNER_NAME	= "OWNER_NAME";
    String	DBMS_DBCAP_SAVEPOINT	= "SAVEPOINTS";
    String	DBMS_DBCAP_SQL92_LEVEL	= "SQL92_LEVEL";
    String	DBMS_DBCAP_UCS2_TYPES	= "NATIONAL_CHARACTER_SET";
    String	DBMS_DBCAP_UNION	= "UNION";
    String	DBMS_DBCAP_MAX_SCH_NAME	= "SQL_MAX_SCHEMA_NAME_LEN";
    String	DBMS_DBCAP_MAX_USR_NAME	= "SQL_MAX_USER_NAME_LEN";
    String	DBMS_DBCAP_MAX_TAB_NAME	= "SQL_MAX_TABLE_NAME_LEN";
    String	DBMS_DBCAP_MAX_COL_NAME	= "SQL_MAX_COLUMN_NAME_LEN";
    String	DBMS_DBCAP_MAX_BYT_LIT	= "SQL_MAX_BYTE_LITERAL_LEN";
    String	DBMS_DBCAP_MAX_CHR_LIT	= "SQL_MAX_CHAR_LITERAL_LEN";
    String	DBMS_DBCAP_MAX_BYT_COL	= "SQL_MAX_BYTE_COLUMN_LEN";
    String	DBMS_DBCAP_MAX_VBY_COL	= "SQL_MAX_VBYT_COLUMN_LEN";
    String	DBMS_DBCAP_MAX_CHR_COL	= "SQL_MAX_CHAR_COLUMN_LEN";
    String	DBMS_DBCAP_MAX_VCH_COL	= "SQL_MAX_VCHR_COLUMN_LEN";
    String	DBMS_DBCAP_MAX_NCHR_COL	= "SQL_MAX_NCHAR_COLUMN_LEN";
    String	DBMS_DBCAP_MAX_NVCH_COL	= "SQL_MAX_NVCHR_COLUMN_LEN";
    String	DBMS_DBCAP_MAX_ROW_LEN	= "SQL_MAX_ROW_LEN";
    String	DBMS_DBCAP_MAX_COLUMNS	= "MAX_COLUMNS";
    String	DBMS_DBCAP_MAX_STMTS	= "SQL_MAX_STATEMENTS";

    /*
    ** Value for iidbcapabilities.
    */
    String	DBMS_TYPE_INGRES	= "INGRES";
    int		DBMS_SQL_LEVEL_IMS_VSAM = 601;
    int		DBMS_SQL_LEVEL_OI10	= 605;
    int		DBMS_SQL_LEVEL_ANSI_DT	= 910;
    String	DBMS_NAME_CASE_LOWER	= "LOWER";
    String	DBMS_NAME_CASE_MIXED	= "MIXED";
    String	DBMS_NAME_CASE_UPPER	= "UPPER";
    String	DBMS_NULL_SORT_FIRST	= "FIRST";
    String	DBMS_NULL_SORT_LAST	= "LAST";
    String	DBMS_NULL_SORT_HIGH	= "HIGH";
    String	DBMS_NULL_SORT_LOW	= "LOW";
    String	DBMS_SQL92_NONE		= "NONE";
    String	DBMS_SQL92_ENTRY	= "ENTRY";
    String	DBMS_SQL92_FIPS		= "FIPS-IEF";
    String	DBMS_SQL92_INTERMEDIATE	= "INTERMEDIATE";
    String	DBMS_SQL92_FULL		= "FULL";
    String	DBMS_UNION_ALL		= "ALL";

    /*
    ** Datatypes
    */
    short	DBMS_TYPE_IDATE		= 3;
    short	DBMS_TYPE_ADATE		= 4;
    short	DBMS_TYPE_MONEY		= 5;
    short	DBMS_TYPE_TMWO		= 6;
    short	DBMS_TYPE_TMTZ		= 7;
    short	DBMS_TYPE_TIME		= 8;
    short	DBMS_TYPE_TSWO		= 9;
    short	DBMS_TYPE_DECIMAL	= 10;
    short	DBMS_TYPE_LOGKEY	= 11;
    short	DBMS_TYPE_TBLKEY	= 12;
    short	DBMS_TYPE_TSTZ		= 18;
    short	DBMS_TYPE_TS		= 19;
    short	DBMS_TYPE_CHAR		= 20;
    short	DBMS_TYPE_VARCHAR	= 21;
    short	DBMS_TYPE_LONG_CHAR	= 22;
    short	DBMS_TYPE_BYTE		= 23;
    short	DBMS_TYPE_VARBYTE	= 24;
    short	DBMS_TYPE_LONG_BYTE	= 25;
    short	DBMS_TYPE_NCHAR		= 26;
    short	DBMS_TYPE_NVARCHAR	= 27;
    short	DBMS_TYPE_LONG_NCHAR	= 28;
    short	DBMS_TYPE_LNLOC		= 29;
    short	DBMS_TYPE_INT		= 30;
    short	DBMS_TYPE_FLOAT		= 31;
    short	DBMS_TYPE_C		= 32;
    short	DBMS_TYPE_INTYM		= 33;
    short	DBMS_TYPE_INTDS		= 34;
    short	DBMS_TYPE_LBLOC		= 35;
    short	DBMS_TYPE_LCLOC		= 36;
    short	DBMS_TYPE_TEXT		= 37;
    short	DBMS_TYPE_LONG_TEXT	= 41;

    IdMap	typeMap[] =
    {
	new IdMap( DBMS_TYPE_IDATE,	"ingresdate" ),
	new IdMap( DBMS_TYPE_ADATE,	"ansidate" ),
	new IdMap( DBMS_TYPE_MONEY,	"money" ),
	new IdMap( DBMS_TYPE_TMWO,	"time without time zone" ),
	new IdMap( DBMS_TYPE_TMTZ,	"time with time zone" ),
	new IdMap( DBMS_TYPE_TIME,	"time with local time zone" ),
	new IdMap( DBMS_TYPE_TSWO,	"timestamp without time zone" ),
	new IdMap( DBMS_TYPE_DECIMAL,	"decimal" ),
	new IdMap( DBMS_TYPE_LOGKEY,	"object_key" ),
	new IdMap( DBMS_TYPE_TBLKEY,	"table_key" ),
	new IdMap( DBMS_TYPE_TSTZ,	"timestamp with time zone" ),
	new IdMap( DBMS_TYPE_TS,	"timestamp with local time zone" ),
	new IdMap( DBMS_TYPE_CHAR,	"char" ),
	new IdMap( DBMS_TYPE_VARCHAR,	"varchar" ),
	new IdMap( DBMS_TYPE_LONG_CHAR,	"long varchar" ),
	new IdMap( DBMS_TYPE_LCLOC,	"long varchar" ),
	new IdMap( DBMS_TYPE_BYTE,	"byte" ),
	new IdMap( DBMS_TYPE_VARBYTE,	"byte varying" ),
	new IdMap( DBMS_TYPE_LONG_BYTE,	"long byte" ),
	new IdMap( DBMS_TYPE_LBLOC,	"long byte" ),
	new IdMap( DBMS_TYPE_NCHAR,	"nchar" ),
	new IdMap( DBMS_TYPE_NVARCHAR,	"nvarchar" ),
	new IdMap( DBMS_TYPE_LONG_NCHAR,"long nvarchar" ),
	new IdMap( DBMS_TYPE_LNLOC,	"long nvarchar" ),
	new IdMap( DBMS_TYPE_C,		"c" ),
	new IdMap( DBMS_TYPE_INTYM,	"interval year to month" ),
	new IdMap( DBMS_TYPE_INTDS,	"interval day to second" ),
	new IdMap( DBMS_TYPE_TEXT,	"text" ),
	new IdMap( DBMS_TYPE_LONG_TEXT,	"long text" ),
    };

    IdMap	intMap[] =
    {
	new IdMap( 1,	"integer1" ),
	new IdMap( 2,	"smallint" ),
	new IdMap( 4,	"integer" ),
	new IdMap( 8,	"bigint" ),
    };

    IdMap	floatMap[] =
    {
	new IdMap( 4,	"float4" ),
	new IdMap( 8,	"float" ),
    };

    IdMap   dataTypeMap[] =
    {
	new IdMap( DBMS_TYPE_INT,	"tinyint"),
	new IdMap( DBMS_TYPE_INT,	"smallint"),
	new IdMap( DBMS_TYPE_INT,	"integer"),
	new IdMap( DBMS_TYPE_INT,	"int"),
	new IdMap( DBMS_TYPE_INT,	"bigint"),
	new IdMap( DBMS_TYPE_FLOAT,	"real"),
	new IdMap( DBMS_TYPE_FLOAT,	"float"),
	new IdMap( DBMS_TYPE_FLOAT,	"double precision"),
	new IdMap( DBMS_TYPE_FLOAT,	"double p"),
	new IdMap( DBMS_TYPE_DECIMAL,	"decimal" ),
	new IdMap( DBMS_TYPE_DECIMAL,	"numeric" ),
	new IdMap( DBMS_TYPE_CHAR,	"char"),
	new IdMap( DBMS_TYPE_CHAR,	"character"),
	new IdMap( DBMS_TYPE_VARCHAR,	"varchar"),
	new IdMap( DBMS_TYPE_LONG_CHAR,	"long varchar"),
	new IdMap( DBMS_TYPE_NCHAR,	"nchar" ),
	new IdMap( DBMS_TYPE_NVARCHAR,	"nvarchar" ),
	new IdMap( DBMS_TYPE_LONG_NCHAR,"long nvarchar" ),
	new IdMap( DBMS_TYPE_BYTE,	"byte"),
	new IdMap( DBMS_TYPE_VARBYTE,	"varbyte"),
	new IdMap( DBMS_TYPE_VARBYTE,	"byte varying"),
	new IdMap( DBMS_TYPE_LONG_BYTE,	"long byte"),
	new IdMap( DBMS_TYPE_C,		"c"),
	new IdMap( DBMS_TYPE_TEXT,	"text"),
	new IdMap( DBMS_TYPE_MONEY,	"money"),
	new IdMap( DBMS_TYPE_IDATE,	"date"),
	new IdMap( DBMS_TYPE_IDATE,	"ingresdate" ),
	new IdMap( DBMS_TYPE_ADATE,	"ansidate" ),
	new IdMap( DBMS_TYPE_TIME,	"time with local time zone" ),
	new IdMap( DBMS_TYPE_TMWO,	"time without time zone" ),
	new IdMap( DBMS_TYPE_TMTZ,	"time with time zone" ),
	new IdMap( DBMS_TYPE_TS,	"timestamp with local time zone" ),
	new IdMap( DBMS_TYPE_TSWO,	"timestamp without time zone" ),
	new IdMap( DBMS_TYPE_TSTZ,	"timestamp with time zone" ),
	new IdMap( DBMS_TYPE_VARCHAR,	"interval year to month" ),
	new IdMap( DBMS_TYPE_VARCHAR,	"interval day to second" ),
    };

    /*
    ** AIF Error codes
    */
    int		AIF_ERR_CLASS		= 201;
    int		E_AIF_MASK		= AIF_ERR_CLASS << 16;
    int		E_AP0009_QUERY_CANCELLED	= E_AIF_MASK | 0x0009;


} // interface DbmsConst