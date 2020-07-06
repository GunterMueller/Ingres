/*
** Copyright (c) 1993, 2006 Ingres Corporation
*/

#include <compat.h>
#ifndef VMS
#include <systypes.h>
#endif
#include <sql.h>                    /* ODBC Core definitions */
#include <sqlext.h>
#include <sqlstate.h>               /* ODBC driver sqlstate codes */
#include <sqlcmd.h>


#include <cm.h>
#include <cv.h> 
#include <me.h>
#include <nm.h> 
#include <erodbc.h>
#include <qu.h>
#include <st.h> 
#include <tr.h>
#include <iiapi.h>

#include <sqlca.h>
#include <idmsxsq.h>                /* control block typedefs */
#include <idmsutil.h>               /* utility stuff */
#include "caidoopt.h"               /* connect options */
#include "idmsoinf.h"               /* information definitions */
#include "idmsoids.h"               /* string ids */
#include <idmseini.h>               /* ini file keys */
#include "idmsodbc.h"               /* ODBC conversion routines            */

#include "idmsocvt.h"               /* ODBC driver definitions             */

#define DEBUGGING_64_CATALOG        FALSE

/*
** Name: CATALOG.C
**
** Description:
**	Catalog routines for ODBC driver.
**
** History:
**	12-jan-1993 (rosda01)
**	    Initial coding
**	18-aug-1993 (rosda01)
**	    Use szTableOwner for SCHEMA in all functions
**	19-apr-1994 (rosda01)
**	    Rewrote to handle all IDMS derived DBMS's
**	14-mar-1995 (rosda01)
**	    ODBC 2.0 upgrade...
**	21-jun-1996 (rosda01)
**	    Win32 port
**	28-jan-1997 (thoda04)
**	    Added changes for Ingres
**	24-feb-1997 (thoda04)
**	    Added SQLColumns support
**	04-mar-1997 (thoda04)
**	    Bug fixes to DB2 SQLColumns support
**	13-mar-1997 (thoda04)
**	    Added SQLSpecialColumns and SQLStatistics
**	27-mar-1997 (thoda04)
**	    Removed CASQLSets baggage. 
**	01-apr-1997 (thoda04)
**	    Added SQLProcedure and SQLProcedureColumns.
**	14-apr-1997 (thoda04)
**	    Convert int literals back to int from float.
**	01-may-1997 (thoda04)
**	    Lower the case of catalog objects if INGRES.
**	12-may-1997 (thoda04)
**	    Support SQLProcedureColumns only for serverclass=INGRES.
**	05-jun-1997 (thoda04)
**	    Support SQLProcedureColumns in Gateways.
**	16-jun-1997 (thoda04)
**	    Free memory and bug clean-up.
**	27-jun-1997 (thoda04)
**	    Don't free memory to be freed in FreeSqlStr.
**	30-jun-1997 (thoda04)
**	    Break up query for SQLSpecialColumns if DB2
**	    due to DB2 limit of 15 tables references.
**	05-aug-1997 (thoda04)
**	    Map ingdatatype into typename if needed.
**	05-aug-1997 (thoda04)
**	    Break up SQLSpecialColumns query for VSAM/IMS
**	23-sep-1997 (thoda04)
**	    SQLStatistics - return "noname" unique primary key
**	09-oct-2001 (thoda04)
**	    SQLColumns - return columns in CREATE TABLE order
**	12-oct-1997 (thoda04)
**	    Add debugging code for odbc_malloc
**	05-nov-1997 (thoda04)
**	    SQLSpecialColumns should return primary key also
**	17-nov-1997 (thoda04)
**	    DB_NAME_CASE from iicapabilities support
**	06-dec-1997 (tenge01)
**	    convert C run-time functions to CL functions 
**	18-dec-1997 (thoda04)
**	    Get length of table_name for CatParm LIKE clause
**	09-jan-1998 (tenje01)
**	    CatScale, set scale for money to 2
**	14-jan-1998 (thoda04)
**	   Return correct scale for MONEY datatype
**	26-feb-1998 (thoda04)
**	    Pass a valid pstmt for CatConnect and SetTransaction
**	15-apr-1998 (thoda04)
**	    Return correct TYPE_NAME for REAL and SMALLINT
**	06-jan-1999 (thoda04)
**	    Moved sqldaDefault from ENV to DBC for thread-safeness.
**	13-jan-1999 (thoda04)
**	    Determine max name lengths (e.g. max_table_name_length).
**	09-feb-1999 (thoda04)
**	    Check that descriptor count is not zero before using it.
**	22-feb-1999 (thoda04)
**	    Add support for SQLPrimaryKeys.
**	25-feb-1999 (thoda04)
**	    Ported to UNIX.
**	15-jun-1999 (thoda04)
**	    SQLColumns should allow all null parameters.
**	20-aug-1999 (thoda04)
**	    Add support for SQLForeignKeys.
**	15-nov-1999 (thoda04)
**	    Added option to return blank date as NULL.
**	12-jan-2000 (thoda04)
**	    Added option to return date 1582-01-01 as NULL.
**	10-feb-2000 (thoda04)
**	    If DB2, DCOM, etc., use the hard-coded table_len,...
**	31-mar-2000 (thoda04)
**	    Add CatGetDBconstants for szDBAname.
**	25-apr-2000 (thoda04)
**	    Make separate catalog session optional
**	24-may-2000 (thoda04)
**	    Additional isServerClassINGRES conversions
**	    for user-defined server class support
**	02-oct-2000 (thoda04)
**	    Moved "username" query to CatGetDBconstants
**	12-oct-2000 (thoda04)
**	    Added option to return NULL for TABLE_SCHEM
**	07-nov-2000 (thoda04)
**	    Convert nat's to i4
**	19-mar-2001 (weife01)
**	    Fix for Bug#104251. SQLPrimaryKeys and SQLForeignKeys
**	    does not use iikeys catalog when server is Ingres 6.4
**	12-jul-2001 (somsa01)
**	    Cleaned up 64-bit compiler warnings.
**	10-aug-2001 (thoda04)
**	    Add support for SQL_ATTR_METADATA_ID==TRUE to skip search
**	    for wild-card chars and take the catalog function parms
**	    literally.
**	08-oct-2001 (thoda04)
**	    Return SQL_TIMESTAMP rather than SQL_TYPE_TIMESTAMP if
**	    2.x application
**	29-oct-2001 (thoda04)
**	    Corrections to 64-bit compiler warnings change.
**	15-oct-2001 (weife01)
**	    Fix for Bug#105954. Underscores in table name in SQLColumns()
**	    have to be escaped if the server is IDMS
**	05-nov-2001 (weife01)
**	    Fix for Bug#106281. DBAname was overwritten by
**	    SystemOwnername in CatGetDBconstants().
**	16-jan-2002 (thoda04)
**	    Add support for DisableCatUnderscore search char.
**      30-jul-2002 (weife01) SIR #108411
**          Made change to functions SQLGetInfo and SQLGetTypeInfo to support DBMS 
**          change to allow the VARCHAR limit to 32k; added internal function GetSQLMaxLen.
**  21-oct-2002 (weife01) bug #109014
**          Deleted the white space in CatParm when do LIKE caluse, so the underscore
**          pattern match will work for table_name or column_name has lenght of 32 CHAR.
**      14-feb-2003 (loera01) SIR 109643
**          Minimize platform-dependency in code.  Make backward-compatible
**          with earlier Ingres versions.
**     17-oct-2003 (loera01)
**          Changed "IIsyc_" routines to "odbc_".
**     04-dec-2003 (loera01)
**          Changed "STMT_" status flags to "CATLG_".
**     29-may-2003 (weife01) Bug 110148
**          Fix internal data mapping for gateway.
**	23-apr-2004 (somsa01)
**	    Include systypes.h to fix compile errors on HP.
**     10-nov-2004 (Ralph.Loen@ca.com) Bug 113429
**          SQLTables() should return with SQLSTATE error IM001 (not supported)
**          if only catalogs are requested.
**     15-nov-2004 (Ralph.Loen@ca.com) Bug 113462
**          Use IItrace global block to handle tracing.
**     26-apr-2007 (fei.wei@ingres.com) Bug118193
**          Strip off double quote around parameter for catalog function 
**          in CatUnquote().
**     21-jul-2006 (Ralph Loen)  SIR 116410
**          Add support for SQLColumnPrivileges().
**     26-jul-2006 (Ralph Loen) SIR 116417
**          Add gateway support to SQLTablePrivileges() and 
**          SQLColumnPrivileges().  This includes VSAM and IMS.  RMS is already
**          supported.
**     07-aug-2006 (Ralph Loen)  SIR 116410
**          Port SQLColumnPrivileges() to non-Windows.
**     10-Aug-2006 (Ralph Loen) SIR 116477
**          Add support for ISO dates, times, and intervals (first phase).
**     10-Nov-2006 (Ralph Loen) SIR 116477
**          In CatFetchColumn(), return Level 2 types for times and
**          dates.  In CatFetchBuffer(), add ISO types to the
**          CATLG_PROCCOL case.
**     30-Aug-2007 (Ralph Loen) Bug 119045
**          Return SQL_SUCCESS in SQLTablePrivileges_InternalCall() and
**          SQLColumnPrivileges_InternalCall() if an empty result set is
**          returned from the catalog query--not SQL_NO_DATA.
**     09-Jan-2008 (Ralph Loen) SIR 119723 
**          Initialize qry_flags to be compatible with the new API
**          specification.
*/

/*
**  Table type flag bits:
*/
#define TYPE_ALIAS      0x0001          /* reserved */
#define TYPE_SYNONYM    0x0002          /* reserved */
#define TYPE_SYSTEM     0x0004
#define TYPE_TABLE      0x0008
#define TYPE_VIEW       0x0010
#define TYPE_ERROR      0x8000
#define TYPE_USER       (TYPE_TABLE + TYPE_VIEW)
#define TYPE_ALL        (TYPE_USER /* + TYPE_SYSTEM*/)

#define LEN_SQL         10000           /* length of syntax buffer */
#define OBJECT_NAME_SIZE 32

/*
**  Internal typedefs and function prototypes:
*/
typedef enum
{
    PARM_INVALID, PARM_NULL, PARM_EMPTY, PARM_EQUAL, PARM_LIKE, PARM_USER
}
PARM;

#ifdef CATALOG_CACHE_ENABLED
static BOOL      CacheInit   (LPDBC, LPSTMT);
static RETCODE   CacheOpen   (LPDBC, LPSTMT);
static RETCODE   CacheSqlda  (LPDBC, LPSTMT);
#endif  /* CATALOG_CACHE_ENABLED */

static LPSTR     CatAlloc    (LPSTMT);
static BOOL      CatConnect  (LPSTMT);
static CHAR      CatEscape   (LPSTMT);
static RETCODE   CatExecDirect (LPSTMT, CHAR  *, UWORD);
static SWORD     CatFetchColumn  (LPSTMT, CHAR  *, UWORD, UWORD, UWORD, UWORD, UWORD,
                                            UWORD, UWORD, UWORD);
static void                CatInt(LPSTMT, CHAR  *, UWORD, UWORD);
#define CatInt2(a, b, c)   CatInt(a, b, c, SQL_SMALLINT)
#define CatInt4(a, b, c)   CatInt(a, b, c, SQL_INTEGER)
static void      CatNull     (LPSTMT, CHAR  *, UWORD);
static PARM      CatParm     (LPDBC, LPSTR, SWORD, UCHAR  *, OFFSET_TYPE, CHAR);
static PARM      CatParmName (LPDBC, LPSTR, UCHAR  *, OFFSET_TYPE);
static PARM                CatParmForceToUser(PARM parm, LPDBC pdbc, LPSTR szOwner);
static void      CatRemove   (LPSTR, LPCSTR);
static void      CatScale    (LPDESCFLD, SWORD  *, SDWORD  *);
static LPSTR     CatStrtok   (LPSTR, size_t);
static void      CatTrim     (LPSTMT, CHAR  *, UWORD);
static UWORD     CatType     (LPSTMT, CHAR  *, SDWORD);
static size_t    CatUnquote  (CHAR  *  *, size_t);
static void      CatWhere    (LPSTR, PARM, LPCSTR, LPCSTR, CHAR);
static void      CatWhereStr (LPSTR, PARM, LPCSTR, LPCSTR, CHAR);
static void      CatWhereIn  (LPSTR, UWORD, LPCSTR, LPCSTR);
static BOOL   CatQueryOpen       (LPSTMT, IIAPI_QUERYPARM *, LPSTR);
static BOOL   CatQueryDescriptor (LPSTMT, IIAPI_QUERYPARM *, IIAPI_GETDESCRPARM*);
static BOOL   CatQueryGetColumns (LPSTMT, IIAPI_QUERYPARM *, IIAPI_GETDESCRPARM*,
                                                                          IIAPI_GETCOLPARM *);
static BOOL   CatQueryClose      (LPSTMT, IIAPI_QUERYPARM *, IIAPI_GETDESCRPARM*,
                                                                          IIAPI_GETCOLPARM *);

BOOL odbc_getResult( IIAPI_GENPARM  *genParm, LPSQLCA psqlca, II_LONG cQueryTimeout);
BOOL odbc_getQInfo( LPSTMT pstmt );
BOOL odbc_close( LPSTMT pstmt );


/*
**  Work areas for identifier parameter names:
**
**  Allocated in default data segment in Win16 to save on stack space,
**  which is allocated by the app, and can be smaller than we would like.
**  These are allocated on the stack for each function in Win32.  Note
**  that 16 bit process in Win95 still use the app's stack, so some apps
**  might not work with the Win32 version of this driver in that environment,
**  but what else can we do?
*/
#ifdef WIN16
static char szOwner[128];
static char szTable[65];
static char szColumn[65];
static char szProc[65];
static char szTemp[128];
static char szPkOwner[128];
static char szPkTable[65];
static char szFkOwner[128];
static char szFkTable[65];
#endif

/*
**  General query constants:
*/
static const char ALL[]     = " ALL ";
static const char AND[]     = " AND ";
static const char COMMA[]   = ", ";
static const char DELIM[]   = "\".\"";
static const char DQUOTE[]  = "\"";
static const char EQUAL[]   = " = ";
static const char ESCAPE[]  = " ESCAPE ";
static const char LIKE[]    = " LIKE ";
static const char LPAREN[]  = " ( ";
static const char OR[]      = " OR ";
static const char PERCENT[] = "%";
static const char RPAREN[]  = " ) ";
static const char SELECT[]  = " SELECT * FROM ";
static const char SQUOTE[]  = "'";
static const char UNION[]   = " UNION ";
static const char WHERE[]   = " WHERE ";
static const char UNDERSCORE[] = "_";

typedef struct tDATATYPE_INGDATATYPE   /* Conversion table for datatype -> ingdatatype */
{
    char    * szdatatype;
    short     ingdatatype;
} DATATYPE_INGDATATYPE;

static const DATATYPE_INGDATATYPE Datatype_To_Ingdatatype_Table[] = 
{
 "CHAR",     IIAPI_CHA_TYPE,
 "INTEGER",  IIAPI_INT_TYPE,
 "VARCHAR",  IIAPI_VCH_TYPE,
 "SMALLINT", IIAPI_INT_TYPE,
 "LONG VAR", IIAPI_LVCH_TYPE,
 "C",        IIAPI_CHA_TYPE,
 "TEXT",     IIAPI_TXT_TYPE,
 "DATE",     IIAPI_DTE_TYPE,
 "DECIMAL",  IIAPI_DEC_TYPE,
 "INT",      IIAPI_INT_TYPE,
 "FLOAT",    IIAPI_FLT_TYPE,
 "REAL",     IIAPI_FLT_TYPE,
 "DOUBLE P", IIAPI_FLT_TYPE,
 "CHARACTE", IIAPI_CHA_TYPE,
 "BYTE",     IIAPI_BYTE_TYPE,
 "BYTE VAR", IIAPI_CHA_TYPE,
 "INGRESDATE", IIAPI_DTE_TYPE,
 "ANSIDATE", IIAPI_DATE_TYPE,
 "TIME WITH LOCAL TIME ZONE", IIAPI_TIME_TYPE,
 "TIME WITH TIME ZONE", IIAPI_TMTZ_TYPE,
 "TIME WITHOUT TIME ZONE", IIAPI_TMWO_TYPE,
 "TIMESTAMP WITH LOCAL TIME ZONE", IIAPI_TS_TYPE,
 "TIMESTAMP WITH TIME ZONE", IIAPI_TSTZ_TYPE,
 "TIMESTAMP WITHOUT TIME ZONE", IIAPI_TSWO_TYPE,
 "INTERVAL YEAR TO MONTH", IIAPI_INTYM_TYPE,
 "INTERVAL DAY TO SECOND", IIAPI_INTDS_TYPE,
# ifdef IIAPI_NCHA_TYPE
 "NCHAR",    IIAPI_NCHA_TYPE,
 "NVARCHAR", IIAPI_NVCH_TYPE,
 "MONEY",    IIAPI_MNY_TYPE
# else
   "MONEY",    IIAPI_MNY_TYPE
# endif
};


#define SIZEOF_Datatype_To_Ingdatatype_Table (sizeof(Datatype_To_Ingdatatype_Table) / \
                                              sizeof(Datatype_To_Ingdatatype_Table[0]))



static const STRINGID CatStringID =
{
    "table_owner",           /* fOwnerName       Owner name column (for most tables) */
    "table_name",            /* fTableName       TABLE table name column */
    "c.table_owner",         /* fColumnOwner     COLUMN table table name column */
    "c.table_name",          /* fColumnTable     COLUMN table table name column */
    "c.column_name",         /* fColumnName      COLUMN table column name column */
    "table_name",            /* fIndexTable      INDEX table table name column */
    "C.NULLS = 0 AND ",      /* fNullable        SQLSpecialColumns nullable predicate */
    "UPDATE STATISTICS FOR ",/* fUpStats         SQLStatistics update stats command */
    "i.base_owner",          /* fIndexTableOwner Joined index table owner column name */
    "i.base_name",           /* fIndexTableName  Joined index table table column name */
    "UNIQUE = 1",            /* fUnique          SQLStatistics unique predicate */
    "procedure_owner",       /* fProcOwner       Procedure owner name */
    "procedure_name",        /* fProcName;       Procedure name */
    "param_name"             /* fProcColName     Procedure column name */
};


/*
**  SQLColumns
**
**  Return column definitions from the catalog:
**
**  On entry: pstmt           -->statement control block.
**            szTableQualifier   ignored.
**            cbTableQualifier   ignored.
**            szTableOwner    -->owner (schema) name.
**            cbTableOwner     = length of owner name.
**            szTableName     -->table name.
**            cbTableName      = length of table name.
**            szColumnName    -->column name.
**            cbColumnName     = length of column name.
**
**  On exit:
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
*/

#define QRY_COLS_QUALIFIER    1
#define QRY_COLS_OWNER        2
#define QRY_COLS_TABLE        3
#define QRY_COLS_COLUMN       4
#define QRY_COLS_DATA_TYPE    5
#define QRY_COLS_TYPE_NAME    6
#define QRY_COLS_PRECISION    7
#define QRY_COLS_LENGTH       8
#define QRY_COLS_SCALE        9
#define QRY_COLS_RADIX       10
#define QRY_COLS_NULLABLE    11
#define QRY_COLS_REMARKS     12
#define QRY_COLS_DEFAULT     13  /* begin 3.x columns */
#define QRY_COLS_VERBOSETYPE 14
#define QRY_COLS_SUBTYPE     15
#define QRY_COLS_OCTETLENGTH 16
#define QRY_COLS_POSITION    17
#define QRY_COLS_ISNULLABLE  18

#define CAT_COLUMNS \
   "SELECT ' ', "                      /* TABLE_CAT   */ \
          "table_owner, "              /* TABLE_SCHEM */     \
          "table_name, "               /* TABLE_NAME  */     \
          "column_name, "              /* COLUMN_NAME */     \
          "column_ingdatatype, "       /* DATA_TYPE (ODBC type code  */ \
          "column_datatype, "          /* TYPE_NAME (human readable) */ \
          "column_length, "            /* COLUMN_SIZE */ \
          "column_length, "            /* BUFFER_LENGTH */ \
          "column_scale, "             /* DECIMAL_DIGITS */ \
          "10, "                       /* RADIX */ \
          "column_nulls, "             /* NULLABLE  */ \
          "' ', "                      /* REMARKS   */ \
          "' ', "                      /* COLUMN_DEF   begin 3.x cols */ \
          "column_ingdatatype, "       /* SQL_DATA_TYPE */ \
          "0, "                        /* DATETIME_SUB  */ \
          "column_length, "            /* CHAR_OCTET_LENGTH */ \
          "column_sequence, "          /* ORDINAL_POSITION  */ \
          "'YES' "                     /* IS_NULLABLE   */ \
     "FROM iicolumns c " \
     "WHERE \0"
/*         OWNER LIKE <szOwner>  AND                -- or */
/*         OWNER = <szOwner> AND */
/*         TABLENAME LIKE <szTable> AND             -- or */
/*         TABLENAME = <szTable> AND */
/*         COLNAME LIKE <szColumn>                  -- or */
/*         COLNAME = <szColumn> */
#define CAT_COLUMNS_ORDER_BY  " ORDER BY table_owner, table_name, column_sequence"
/*    ORDER BY TABLE_OWNER, TABLE_NAME" */

RETCODE SQL_API SQLColumns(
    SQLHSTMT    hstmt,
    UCHAR  * szTableQualifier,
    SWORD       cbTableQualifier,
    UCHAR  * szTableOwner,
    SWORD       cbTableOwner,
    UCHAR  * szTableName,
    SWORD       cbTableName,
    UCHAR  * szColumnName,
    SWORD       cbColumnName)
{  return SQLColumns_InternalCall(
                hstmt,
                szTableQualifier, cbTableQualifier,
                szTableOwner,     cbTableOwner,
                szTableName,      cbTableName,
                szColumnName,     cbColumnName);
}

RETCODE SQL_API SQLColumns_InternalCall(
    SQLHSTMT    hstmt,
    UCHAR  * szTableQualifier,
    SWORD       cbTableQualifier,
    UCHAR  * szTableOwner,
    SWORD       cbTableOwner,
    UCHAR  * szTableName,
    SWORD       cbTableName,
    UCHAR  * szColumnName,
    SWORD       cbColumnName)
{
    LPSTMT      pstmt = (LPSTMT)hstmt;
    RETCODE     rc;
    CHAR        bEscape;
    PARM        parmOwner;
    PARM        parmTable;
    PARM        parmColumn;
    LPDBC       pdbc;
    const STRINGID*  pstringid = &CatStringID;
    LPSTR       szSqlStr  = NULL;
    UDWORD      fOptions;           /* saved DBC options */
    DECLTEMP (szOwner[128])         /* Win32 only */
    DECLTEMP (szTable[65])
    DECLTEMP (szColumn[65])

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLCOLUMNS, pstmt,
            szTableQualifier, cbTableQualifier, szTableOwner, cbTableOwner,
            szTableName, cbTableName, szColumnName, cbColumnName);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;

    /*
    **  Set up parms to stuff into syntax string:
    **
    **  1. Get driver's ESCAPE character, if supported.
    **  2. Edit length (allowing for ESCAPE characters, if any).
    **  3. Ensure null terminated.
    **  4. Scan for search patterns, and pad with trailing
    **     blanks or wild cards to make LIKE clause work.
    */
    bEscape    = CatEscape (pstmt);

    fOptions = pdbc->fOptions;   /* save DBC connection option flags */

    /* if the server is IDMS, make sure the underscore is escaped */
    if (isServerClassIDMS(pdbc))
        pdbc->fOptions |= OPT_DISABLEUNDERSCORE;

    parmOwner  = CatParm (pdbc, szOwner,  32, szTableOwner, cbTableOwner, bEscape);
    parmTable  = CatParm (pdbc, szTable,  32, szTableName,  cbTableName,  bEscape);
    parmColumn = CatParm (pdbc, szColumn, 32, szColumnName, cbColumnName, bEscape);

    pdbc->fOptions = fOptions;   /* restore DBC connection option flags */

    if (parmOwner  == PARM_INVALID ||
        parmTable  == PARM_INVALID ||
        parmColumn == PARM_INVALID)
        return ErrUnlockStmt (SQL_HY090, pstmt); /* invalid string or buffer length */

    parmOwner = CatParmForceToUser(parmOwner, pdbc, szOwner);
        
    /*
    **  Caller has some selection criteria, so do a real query:
    */
    {
        /*
        **  Set up transient retrieval connection to catalog:
        */
        if (!CatConnect (pstmt))
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

        /*
        **  Start building SQL query syntax:
        **
        **  1. Allocate buffer (SQLExecDirect frees it).
        **  2. Load user defined resource containing catalog specific query.
        */
        szSqlStr = CatAlloc (pstmt);
        if (!szSqlStr)
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

        STcat (szSqlStr, CAT_COLUMNS);

        /*
        **  Add predicates, if any, to query:
        **
        **  1. Load user defined resource containing predicate string ids.
        **  2. Add owner, table, and column predicates as specified.
        **  3. Remove final AND stuck on end by CatWhere.
        **  4. Free stringid table resource.
        */
        if (parmOwner != PARM_NULL || parmTable != PARM_NULL || parmColumn != PARM_NULL)
        {
            CatWhere (szSqlStr, parmOwner,  pstringid->fColumnOwner, szOwner,  bEscape);
            CatWhere (szSqlStr, parmTable,  pstringid->fColumnTable, szTable,  bEscape);
            CatWhere (szSqlStr, parmColumn, pstringid->fColumnName,  szColumn, bEscape);
        }

        CatRemove (szSqlStr, WHERE);   /* in case no selection was desired */
        CatRemove(szSqlStr, AND);

        /*
        **  Add sort clause:
        */
        STcat (szSqlStr, CAT_COLUMNS_ORDER_BY);

        /*
        **  Prepare the query and open the cursor on the catalog session:
        */
        rc = CatExecDirect (pstmt, szSqlStr, CATLG_COLUMNS);

    }
    UnlockStmt (pstmt);
    return rc;
}

/*
**  SQLForeignKeys                           Level 2
**
**  Returns a list of foreign keys in the specified table (columns in the
**         specified table that refer to primary keys in other tables),
**    or
**  returns a list of foreign keys in other tables that refer to the primary
**         key in the specified table.
**
**  On entry: pstmt           -->statement control block.
**            szPkTableQualifier-->ignored.
**            cbPkTableQualifier = ignored.
**            szPkTableOwner    -->primary key owner (schema) name.
**            cbPkTableOwner     = length of primary key owner name.
**            szPkTableName     -->primary key table name.
**            cbPkTableName      = length of primary key table name.
**            szFkTableQualifier-->ignored.
**            cbFkTableQualifier = ignored.
**            szFkTableOwner    -->foreign key owner (schema) name.
**            cbFkTableOwner     = length of foreign key owner name.
**            szFkTableName     -->foreign key table name.
**            cbFkTableName      = length of foreign key table name.
**
**  On exit:  Result set is ready to SQLFetch if no errors.
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
**
**  Notes:    
*/

#define QRY_FKEY_PKQUALIFIER  1
#define QRY_FKEY_PKOWNER      2
#define QRY_FKEY_PKTABLE      3
#define QRY_FKEY_PKCOLUMN     4
#define QRY_FKEY_FKQUALIFIER  5
#define QRY_FKEY_FKOWNER      6
#define QRY_FKEY_FKTABLE      7
#define QRY_FKEY_FKCOLUMN     8
#define QRY_FKEY_KEY_SEQ      9
#define QRY_FKEY_UPDATE_RULE  10
#define QRY_FKEY_DELETE_RULE  11
#define QRY_FKEY_FK_NAME      12
#define QRY_FKEY_PK_NAME      13

/*   return 0 rows for gateways since they don't
     have the iikeys, iiconstraints, and iiref_constraints views */
#define CAT_FOREIGN_KEYS_NONINGRES \
   "SELECT ' ', " \
          "table_owner, " \
          "table_name, " \
          "column_name, " \
          "' ', " \
          "table_owner, " \
          "table_name, " \
          "column_name, " \
          "0, " \
          "0, " \
          "0, " \
          "' ', " \
          "' ' " \
     "FROM iicolumns " \
     "WHERE " \
          "table_name ='ii+odbc+nonsense'" \
/*         schema_name = <szOwner> AND */
/*         table_name  = <szTable>     */

#define CAT_FOREIGN_KEYS_INGRES \
   "SELECT distinct ' ', " \
          "PKiikeys.schema_name, " \
          "PKiikeys.table_name, " \
          "PKiikeys.column_name, " \
          "' ', " \
          "FKiikeys.schema_name, " \
          "FKiikeys.table_name, " \
          "FKiikeys.column_name, " \
          "FKiikeys.key_position, " \
          "0, " \
          "0, " \
          "FKiikeys.constraint_name, " \
          "PKiikeys.constraint_name " \
     "FROM iikeys  PKiikeys, " \
          "iiconstraints, " \
          "iiref_constraints, " \
          "iikeys  FKiikeys " \
     "WHERE " \
          "iiconstraints.constraint_type = 'R' AND " \
          "iiconstraints.constraint_name = iiref_constraints.ref_constraint_name AND " \
          "PKiikeys.constraint_name = iiref_constraints.unique_constraint_name AND " \
          "FKiikeys.constraint_name = iiref_constraints.ref_constraint_name AND " \
          "PKiikeys.key_position    = FKiikeys.key_position AND " \
/*         PKiikeys.schema_name = <szOwner> AND */
/*         PKiikeys.table_name  = <szTable> AND */
/*         FKiikeys.schema_name = <szOwner> AND */
/*         FKiikeys.table_name  = <szTable>     */

RETCODE SQL_API SQLForeignKeys(
    SQLHSTMT    hstmt,
    UCHAR  * szPkTableQualifier,    SWORD  cbPkTableQualifier,
    UCHAR  * szPkTableOwner,        SWORD  cbPkTableOwner,
    UCHAR  * szPkTableName,         SWORD  cbPkTableName,
    UCHAR  * szFkTableQualifier,    SWORD  cbFkTableQualifier,
    UCHAR  * szFkTableOwner,        SWORD  cbFkTableOwner,
    UCHAR  * szFkTableName,         SWORD  cbFkTableName)
{   return SQLForeignKeys_InternalCall( hstmt,
                szPkTableQualifier,           cbPkTableQualifier,
                szPkTableOwner,               cbPkTableOwner,
                szPkTableName,                cbPkTableName,
                szFkTableQualifier,           cbFkTableQualifier,
                szFkTableOwner,               cbFkTableOwner,
                szFkTableName,                cbFkTableName);
}

RETCODE SQL_API SQLForeignKeys_InternalCall(
    SQLHSTMT    hstmt,
    UCHAR  * szPkTableQualifier,    SWORD  cbPkTableQualifier,
    UCHAR  * szPkTableOwner,        SWORD  cbPkTableOwner,
    UCHAR  * szPkTableName,         SWORD  cbPkTableName,
    UCHAR  * szFkTableQualifier,    SWORD  cbFkTableQualifier,
    UCHAR  * szFkTableOwner,        SWORD  cbFkTableOwner,
    UCHAR  * szFkTableName,         SWORD  cbFkTableName)
{
    LPSTMT      pstmt = (LPSTMT)hstmt;
    RETCODE     rc;
    PARM        parmPkOwner;
    PARM        parmPkTable;
    PARM        parmFkOwner;
    PARM        parmFkTable;
    LPSTR       szSqlStr;
    LPDBC       pdbc;
    DECLTEMP (szPkOwner[65])      /* Win32 only */
    DECLTEMP (szPkTable[65])      /* Use static globals in Win16 */
    DECLTEMP (szFkOwner[65])
    DECLTEMP (szFkTable[65])

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    pdbc = pstmt->pdbcOwner;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLFOREIGNKEYS, pstmt,
            szPkTableQualifier, cbPkTableQualifier,
            szPkTableOwner,     cbPkTableOwner,
            szPkTableName,      cbPkTableName,
            szFkTableQualifier, cbFkTableQualifier,
            szFkTableOwner,     cbFkTableOwner,
            szFkTableName,      cbFkTableName);

 /*  if (!isServerClassINGRES(pdbc))*/                  /* only ingres will */
 /*       return ErrUnlockStmt (SQL_IM001, pstmt);  */  /* handle our cat queries */

    if (pstmt->fStatus & STMT_OPEN)          /* err if result set open already*/
        return ErrUnlockStmt (SQL_24000, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */

    if (!CatConnect (pstmt))
    {   UnlockStmt (pstmt);
        return SQL_ERROR;
    }

/* debugging code to code around MS ODBCTEST SQLForeignKeys bug */
/*
szPkTableOwner = szFkTableOwner;
szPkTableName  = szFkTableName;
cbPkTableOwner = cbFkTableOwner;
cbPkTableName  = cbFkTableName;

szFkTableOwner = NULL;
szFkTableName  = NULL;
cbFkTableOwner = 0;
cbFkTableName  = 0;
*/

    /*
    **  Edit parms:
    **
    **  1. Edit length, punt if error.
    **  2. Ensure null terminated and save in local work field.
    */
    parmPkOwner = CatParmName (pdbc, szPkOwner, szPkTableOwner, cbPkTableOwner);
    parmPkTable = CatParmName (pdbc, szPkTable, szPkTableName,  cbPkTableName);
    parmFkOwner = CatParmName (pdbc, szFkOwner, szFkTableOwner, cbFkTableOwner);
    parmFkTable = CatParmName (pdbc, szFkTable, szFkTableName,  cbFkTableName);

    if (parmPkOwner == PARM_INVALID || parmPkTable == PARM_INVALID ||
        parmFkOwner == PARM_INVALID || parmFkTable == PARM_INVALID)
        return ErrState (SQL_HY090, pstmt); /* invalid string or buffer length */

    if (parmPkTable == PARM_NULL  &&  parmFkTable == PARM_NULL)
        return ErrState (SQL_HY009, pstmt);    /* if both null then err */

    if (parmPkTable != PARM_NULL)
        parmPkOwner = CatParmForceToUser(parmPkOwner, pdbc, szPkOwner);
    if (parmFkTable != PARM_NULL)
        parmFkOwner = CatParmForceToUser(parmFkOwner, pdbc, szFkOwner);

    /*
    **  Build SQL query syntax:
    */
    szSqlStr = CatAlloc (pstmt);
    if (szSqlStr == NULL)
    {   UnlockStmt (pstmt);
        return (SQL_ERROR);
    }

    if (isServerClassINGRES(pdbc)&&!isOld6xRelease(pdbc) /* Ingres 6.4 does not have iikeys catalog*/
        ||  DEBUGGING_64_CATALOG)
       {
        STcopy     (CAT_FOREIGN_KEYS_INGRES, szSqlStr);
        CatWhereStr  (szSqlStr, parmPkOwner, "PKiikeys.schema_name", szPkOwner, 0);
        CatWhereStr  (szSqlStr, parmPkTable, "PKiikeys.table_name",  szPkTable, 0);
        CatWhereStr  (szSqlStr, parmFkOwner, "FKiikeys.schema_name", szFkOwner, 0);
        CatWhereStr  (szSqlStr, parmFkTable, "FKiikeys.table_name",  szFkTable, 0);
        CatRemove (szSqlStr, AND);
       }
    else
        STcopy     (CAT_FOREIGN_KEYS_NONINGRES, szSqlStr);  /* return 0 rows */


    if (parmPkTable != PARM_NULL)  /* if PK, order by PK names, key_seq */
        STcat(szSqlStr, " ORDER BY 2, 3, 9");
    else                             /* else order by FK names, key_seq */
        STcat(szSqlStr, " ORDER BY 6, 7, 9");


    rc = CatExecDirect (pstmt, szSqlStr, CATLG_FKEY);

/*  szSqlStr will be freed by ResetStmt,FreeSqlStr*/
    UnlockStmt (pstmt);
    return rc;
}  /* end SQLForeignKeys */



/*
**  SQLPrimaryKeys                           Level 2
**
**  Returns the column name(s) that comprise the primary key for a table.
**
**  On entry: pstmt           -->statement control block.
**            szTableQualifier-->ignored.
**            cbTableQualifier = ignored.
**            szTableOwner    -->owner (schema) name.
**            cbTableOwner     = length of owner name.
**            szTableName     -->table name.
**            cbTableName      = length of table name.
**
**  On exit:  Result set is ready to SQLFetch if no errors.
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
**
**  Notes:    
*/

#define QRY_PKEY_QUALIFIER  1
#define QRY_PKEY_OWNER      2
#define QRY_PKEY_TABLE      3
#define QRY_PKEY_COLUMN     4
#define QRY_PKEY_KEY_SEQ    5
#define QRY_PKEY_PK_NAME    6

#define CAT_PRIMARY_KEYS \
   "SELECT distinct ' ', " \
          "iikeys.schema_name, " \
          "iikeys.table_name, " \
          "iikeys.column_name, " \
          "iikeys.key_position, " \
          "iikeys.constraint_name " \
     "FROM iikeys, " \
          "iiconstraints " \
     "WHERE " \
          "iiconstraints.constraint_type = 'P'  AND " \
          "iikeys.constraint_name = iiconstraints.constraint_name  AND " \
/*         iikeys.schema_name = <szOwner> AND */
/*         iikeys.table_name  = <szTable>     */

#define CAT_PRIMARY_KEYS1 \
   "SELECT key_id, "                    /* number of the alternate key */ \
          "' ', "                       /* TABLE_CAT   */ \
          "table_owner, "               /* TABLE_SCHEM */ \
          "table_name, "                /* TABLE_NAME  */ \
          "column_name, "               /* COLUMN_NAME */ \
          "key_sequence AS KEY_SEQ "    /* KEY_SEQ */     \
     "FROM iialt_columns " \
     "WHERE key_sequence <> 0 AND \0"
/*          c.table_owner = <szOwner> AND */
/*          c.table_name  = <szTable>     */

#define CAT_PRIMARY_KEYS2 \
   "SELECT ' ', "                       /* TABLE_CAT */ \
          "c.table_owner, "             /* TABLE_SCHEM */ \
          "c.table_name, "              /* TABLE_NAME  */ \
          "c.column_name, "             /* COLUMN_NAME */ \
          "c.key_sequence AS KEY_SEQ "  /* KEY_SEQ */ \
     "FROM iitables t, iicolumns c " \
     "WHERE t.table_name = c.table_name AND " \
           "t.table_owner = c.table_owner AND " \
           "t.unique_rule  = 'U' AND c.key_sequence <> 0 AND \0"
/*          table_owner = <szOwner> AND */
/*          table_name  = <szTable>     */

RETCODE SQL_API SQLPrimaryKeys(
    SQLHSTMT    hstmt,
    UCHAR  * szTableQualifier,    SWORD  cbTableQualifier,
    UCHAR  * szTableOwner,        SWORD  cbTableOwner,
    UCHAR  * szTableName,         SWORD  cbTableName)
{   return SQLPrimaryKeys_InternalCall( hstmt,
                szTableQualifier,           cbTableQualifier,
                szTableOwner,               cbTableOwner,
                szTableName,                cbTableName);
}

RETCODE SQL_API SQLPrimaryKeys_InternalCall(
    SQLHSTMT    hstmt,
    UCHAR  * szTableQualifier,    SWORD  cbTableQualifier,
    UCHAR  * szTableOwner,        SWORD  cbTableOwner,
    UCHAR  * szTableName,         SWORD  cbTableName)
{
    LPSTMT      pstmt = (LPSTMT)hstmt;
    RETCODE     rc;
    PARM        parmOwner;
    PARM        parmTable;
    CHAR  *  szSqlStr;
    OFFSET_TYPE lenSqlStr;
    LPDBC       pdbc;
    const STRINGID*  pstringid = &CatStringID;
    IIAPI_QUERYPARM     queryParm={0};
    IIAPI_GETDESCRPARM  getDescrParm={0};
    IIAPI_GETCOLPARM    getColParm={0};
    IIAPI_DESCRIPTOR   *pDescriptor;
    IIAPI_STATUS        gp_status;
    II_LONG     len;
    II_INT4     key_id;
    DECLTEMP (szOwner[128])     /* Win32 only */
    DECLTEMP (szTable[65])      /* Use static globals in Win16 */
    DECLTEMP (szTemp[65])

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLPRIMARYKEYS, pstmt,
            szTableQualifier, cbTableQualifier, szTableOwner, cbTableOwner,
            szTableName, cbTableName);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;

    if (!CatConnect (pstmt))
    {
        UnlockStmt (pstmt);
        return SQL_ERROR;
    }

    /*
    **  Edit parms:
    **
    **  1. Edit length, punt if error.
    **  2. Ensure null terminated and save in local work field.
    */
    parmOwner = CatParmName (pdbc, szOwner, szTableOwner, cbTableOwner);
    parmTable = CatParmName (pdbc, szTable, szTableName,  cbTableName);

    if (parmOwner == PARM_INVALID || 
        parmTable == PARM_INVALID ||
        parmTable == PARM_NULL)
        return ErrState (SQL_HY090, pstmt); /* invalid string or buffer length */

    parmOwner = CatParmForceToUser(parmOwner, pdbc, szOwner);

    /*
    **  Build SQL query syntax:
    **
    **  1. Allocate buffer (SQLExecDirect frees it).
    **  2. Load user defined resource containing table of string id's.
    **  3. Load user defined resource containing catalog specific query.
    **  4. Add owner and table predicates (required).
    **  5. Free string id table.
    */
    szSqlStr = CatAlloc (pstmt);
    if (szSqlStr == NULL)
    {
        UnlockStmt (pstmt);
        return (SQL_ERROR);
    }

    if (isServerClassINGRES(pdbc)&&!isOld6xRelease(pdbc)   /* Ingres 6.4 does not have iikeys */
        ||  DEBUGGING_64_CATALOG)
    {
    /*   first try to get the primary key from iikeys/iiconstraints */
    STcopy     (CAT_PRIMARY_KEYS, szSqlStr);
    CatWhereStr  (szSqlStr, parmOwner, "iikeys.schema_name", szOwner, 0);
    CatWhereStr  (szSqlStr, parmTable, "iikeys.table_name",  szTable, 0);
    CatRemove (szSqlStr, AND);

    STcat(szSqlStr, " ORDER BY 2, 3, 5");

    if (    (rc=CatQueryOpen      (pstmt, &queryParm, szSqlStr))==TRUE)
        if ((rc=CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))==TRUE)
             rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, &getColParm);
    if (!rc) /* if error occurred, close the query and free up resources */
       {CatQueryClose(pstmt, &queryParm, &getDescrParm, NULL);  /* free up */
        UnlockStmt (pstmt);
        return (SQL_ERROR);
       }
    gp_status=getColParm.gc_genParm.gp_status;  /* save the end-of-data status */
    CatQueryClose(pstmt, &queryParm, &getDescrParm, &getColParm);  /* close and free up */
    if (gp_status == IIAPI_ST_SUCCESS)     /* if a row found - success! */
        goto SQLPrimaryKeys_ExecDirect; /* reexecute the the string and    */
                                        /*   make it the active result set */
    }  /* end if (isServerClassINGRES(pdbc)) */


    /*   second, try to get the primary key (or first unique key)
         from iialt_columns */
    STcopy     (CAT_PRIMARY_KEYS1, szSqlStr);

    CatWhere  (szSqlStr, parmOwner, pstringid->fOwnerName, szOwner, 0);
    CatWhere  (szSqlStr, parmTable, pstringid->fTableName, szTable, 0);
    CatRemove (szSqlStr, AND);

    lenSqlStr = (OFFSET_TYPE)STlength(szSqlStr); /* length of SQL string up to ORDER BY */
    STcat(szSqlStr, " ORDER BY 1, 2, 3, 5");

    if (    (rc=CatQueryOpen      (pstmt, &queryParm, szSqlStr))==TRUE)
        if ((rc=CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))==TRUE)
             rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, &getColParm);
    if (!rc) /* if error occurred, close the query and free up resources */
       {CatQueryClose(pstmt, &queryParm, &getDescrParm, NULL);  /* free up */
        UnlockStmt (pstmt);
        return (SQL_ERROR);
       }
    gp_status=getColParm.gc_genParm.gp_status;  /* save the end-of-data status */
    if (gp_status == IIAPI_ST_SUCCESS)
       {
        pDescriptor=getDescrParm.gd_descriptor;
        len = pDescriptor->ds_length;
        if      (len == 4)
            key_id =*((II_INT4*)getColParm.gc_columnData[0].dv_value);
        else if (len == 2)
            key_id =*((II_INT2*)getColParm.gc_columnData[0].dv_value);
        else
            key_id =*((II_INT1*)getColParm.gc_columnData[0].dv_value);
        STprintf(szTemp," AND key_id = %d ORDER BY 2, 3, 5", key_id);
        MEfill(7, ' ', szSqlStr+7);         /* remove the "key_id" field */
        STcopy(szTemp, szSqlStr+lenSqlStr); /* rebuild the end of the SQL */
       }
    CatQueryClose(pstmt, &queryParm, &getDescrParm, &getColParm);  /* close and free up */
    if (gp_status == IIAPI_ST_SUCCESS)     /* if a row found - success! */
        goto SQLPrimaryKeys_ExecDirect; /* reexecute the the string and    */
                                        /*   make it the active result set */


          /* there is no unique primary key; try the table's physical key */
    STcopy     (CAT_PRIMARY_KEYS2, szSqlStr);

    CatWhere  (szSqlStr, parmOwner, pstringid->fColumnOwner, szOwner, 0);
    CatWhere  (szSqlStr, parmTable, pstringid->fColumnTable, szTable, 0);
    CatRemove (szSqlStr, AND);

    STcat(szSqlStr, " ORDER BY 2, 3, 5");

  SQLPrimaryKeys_ExecDirect:

    rc = CatExecDirect (pstmt, szSqlStr, CATLG_PKEY);

/*  szSqlStr will be freed by ResetStmt,FreeSqlStr*/
    UnlockStmt (pstmt);
    return rc;
}  /* end SQLPrimaryKeys */



/*
**  SQLProcedures                               Level 2
**  
**  Returns the list of procedure names stored in a specific data source.
**
**  On entry: pstmt           -->statement control block.
**            szProcQualifier -->qualifier name.   (ignored)
**            cbProcQualifier = length of qualifier name.
**            szProcOwner     -->owner (schema) name search string.
**            cbProcOwner     = length of owner name.
**            szProcName      -->table name search string.
**            cbProcName      = length of table name.
**
**  On exit:
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
**
**  Notes:
**            Arguments szProcOwner and szProcName can contain 
**            search pattern characters like "%" and "_".
**
**            SELECT NULL  AS PROCEDURE_QUALIFIER,
**                   PROCEDURE_OWNER,
**                   PROCEDURE_NAME,
**                   NULL  AS NUM_INPUT_PARAMS,
**                   NULL  AS NUM_OUTPUT_PARAMS,
**                   NULL  AS NUM_RESULT_SETS,
**                   NULL  AS REMARKS
**               FROM IIPROCEDURES
**               ORDER BY 2,3
**
*/

#define QRY_PROC_QUALIFIER  1
#define QRY_PROC_OWNER      2
#define QRY_PROC_NAME       3
#define QRY_PROC_NUMIN      4
#define QRY_PROC_NUMOUT     5
#define QRY_PROC_NUMRES     6
#define QRY_PROC_REMARKS    7
#define QRY_PROC_TYPE       8

#define CAT_PROC \
   "SELECT DISTINCT " \
          "' ', "                      /* PROCEDURE_QUALIFIER */ \
          "procedure_owner, "          /* PROCEDURE_OWNER    */  \
          "procedure_name, "           /* PROCEDURE_NAME     */  \
          "0, "                        /* NUM_INPUT_PARAMS   */  \
          "0, "                        /* NUM_OUTPUT_PARAMS  */  \
          "0, "                        /* NUM_RESULT_SETS    */  \
          "' ', "                      /* REMARKS   */           \
          "0 "                         /* PROCEDURE_TYPE    */   \
     "FROM iiprocedures " \
     "WHERE \0"
/*         PROCEDURE_OWNER LIKE <szOwner> AND    -- or */
/*         PROCEDURE_OWNER    = <szOwner> AND */
/*         PROCEDURE_NAME  LIKE <szTable> AND    -- or */
/*         PROCEDURE_NAME     = <szTable> AND */
/*    ORDER BY 2,3" */

#define QRY_COLUMNP_TABLE_CAT 1
#define QRY_COLUMNP_TABLE_SCHEM 2
#define QRY_COLUMNP_TABLE_NAME 3
#define QRY_COLUMNP_COLUMN_NAME 4
#define QRY_COLUMNP_GRANTOR 5
#define QRY_COLUMNP_GRANTEE 6
#define QRY_COLUMNP_PRIVILEGE 7
#define QRY_COLUMNP_IS_GRANTABLE 8

static const struct tcolumnPrivInfo
{
    char    Name[33];
    i2      fIngApiType;
    i4      OctetLength;
    i2      Nullable;
} columnPrivInfo[]
=
{
    "TABLE_CAT",      IIAPI_VCH_TYPE, 3, SQL_NULLABLE,
    "TABLE_SCHEM",    IIAPI_VCH_TYPE, 33, SQL_NULLABLE,
    "TABLE_NAME",     IIAPI_VCH_TYPE, 33, SQL_NO_NULLS,
    "COLUMN_NAME",    IIAPI_VCH_TYPE, 33, SQL_NO_NULLS,
    "GRANTOR",        IIAPI_VCH_TYPE, 33, SQL_NULLABLE,
    "GRANTEE",        IIAPI_VCH_TYPE, 33, SQL_NULLABLE,
    "PRIVILEGE",      IIAPI_VCH_TYPE, 10, SQL_NULLABLE,
    "IS_GRANTABLE",   IIAPI_VCH_TYPE, 6, SQL_NULLABLE
};

#define CAT_COLUMNPRIV "SELECT " \
    "object_owner, " \
    "object_name, " \
    "permit_grantor, " \
    "permit_user, " \
    "text_segment " \
    "FROM iipermits " \
    "WHERE object_type = 'T' AND " \
    "text_segment LIKE '%(%' AND " \
    "object_name NOT LIKE 'ii%' AND \0" 

#define CAT_COLUMNPRIV_GW "SELECT " \
    "object_owner, " \
    "object_name, " \
    "'_SYSTEM', " \
    "permit_user, " \
    "text_segment " \
    "FROM iipermits " \
    "WHERE object_type = 'T' AND " \
    "text_segment LIKE '%(%' AND " \
    "object_name NOT LIKE 'ii%' AND \0" 

#define QRY_TABLEP_TABLE_CAT 1
#define QRY_TABLEP_TABLE_SCHEM 2
#define QRY_TABLEP_TABLE_NAME 3
#define QRY_TABLEP_GRANTOR 4
#define QRY_TABLEP_GRANTEE 5
#define QRY_TABLEP_PRIVILEGE 6
#define QRY_TABLEP_IS_GRANTABLE 7

static const struct ttablePrivInfo
{
    char    Name[33];
    i2      fIngApiType;
    i4      OctetLength;
    i2      Nullable;
} tablePrivInfo[]
=
{
    "TABLE_CAT",      IIAPI_VCH_TYPE, 3, SQL_NULLABLE,
    "TABLE_SCHEM",    IIAPI_VCH_TYPE, 33, SQL_NULLABLE,
    "TABLE_NAME",     IIAPI_VCH_TYPE, 33, SQL_NO_NULLS,
    "GRANTOR",        IIAPI_VCH_TYPE, 33, SQL_NULLABLE,
    "GRANTEE",        IIAPI_VCH_TYPE, 33, SQL_NULLABLE,
    "PRIVILEGE",      IIAPI_VCH_TYPE, 10, SQL_NULLABLE,
    "IS_GRANTABLE",   IIAPI_VCH_TYPE, 6, SQL_NULLABLE
};

#define CAT_TABLEPRIV "SELECT " \
    "object_owner, " \
    "object_name, " \
    "permit_grantor, " \
    "permit_user, " \
    "text_segment " \
    "FROM iipermits " \
    "WHERE object_type = 'T' " \
    "AND text_segment NOT LIKE '%(%' " \
    "AND object_name NOT LIKE 'ii%' AND \0" 

#define CAT_TABLEPRIV_GW "SELECT " \
    "object_owner, " \
    "object_name, " \
    "'_SYSTEM', " \
    "permit_user, " \
    "text_segment " \
    "FROM iipermits " \
    "WHERE object_type = 'T' " \
    "AND text_segment NOT LIKE '%(%' " \
    "AND object_name NOT LIKE 'ii%' AND \0" 

RETCODE SQL_API SQLProcedures(
    SQLHSTMT    hstmt,
    UCHAR  * szProcQualifier,    SWORD  cbProcQualifier,
    UCHAR  * szProcOwner,        SWORD  cbProcOwner,
    UCHAR  * szProcName,         SWORD  cbProcName)
{   return SQLProcedures_InternalCall( hstmt,
                szProcQualifier,           cbProcQualifier,
                szProcOwner,               cbProcOwner,
                szProcName,                cbProcName);
}

RETCODE SQL_API SQLProcedures_InternalCall(
    SQLHSTMT    hstmt,
    UCHAR  * szProcQualifier,    SWORD  cbProcQualifier,
    UCHAR  * szProcOwner,        SWORD  cbProcOwner,
    UCHAR  * szProcName,         SWORD  cbProcName)
{
    LPSTMT      pstmt = (LPSTMT)hstmt;
    RETCODE     rc;
    CHAR        bEscape;
    PARM        parmOwner;
    PARM        parmProc;
    LPDBC       pdbc;
    const STRINGID*  pstringid = &CatStringID;
    CHAR   * szSqlStr    = NULL;

    DECLTEMP (szOwner[128])    /* Win32 only */
    DECLTEMP (szProc[65])      /* Use static globals in Win16 */

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLPROCEDURES, pstmt,
            szProcQualifier, cbProcQualifier, szProcOwner, cbProcOwner,
            szProcName, cbProcName);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;

    if (!CatConnect (pstmt))
    {
        UnlockStmt (pstmt);
        return SQL_ERROR;
    }

    /*
    **  Set up parms to stuff into syntax string:
    **
    **  1. Get driver's ESCAPE character, if supported.
    **  2. Edit length (allowing for ESCAPE characters, if any).
    **  3. Ensure null terminated.
    **  4. Scan for search patterns, and pad with trailing
    **     blanks or wild cards to make LIKE clause work.
    **
    */
    bEscape   = CatEscape (pstmt);
    parmOwner = CatParm (pdbc, szOwner,  32, szProcOwner, cbProcOwner, bEscape);
    parmProc  = CatParm (pdbc, szProc,   32, szProcName,  cbProcName,  bEscape);

    if (parmOwner == PARM_INVALID || 
        parmProc  == PARM_INVALID)
        return ErrUnlockStmt (SQL_HY090, pstmt); /* invalid string or buffer length */

    parmOwner = CatParmForceToUser(parmOwner, pdbc, szOwner);

    pstmt->fCommand = CMD_SELECT;      /* avoid parsing command */

    /*
    **  Build SQL query syntax:
    **
    **  - Allocate buffer (SQLExecDirect frees it).
    **  - Load user defined resource containing catalog specific query.
    **  - Add owner and name predicates (required).
    **  - Add catalog specific not null predicate, if needed.
    **  - Free string id table.
    */
    szSqlStr = CatAlloc (pstmt);
    if (szSqlStr == NULL)
    {
        UnlockStmt (pstmt);
        return (SQL_ERROR);
    }

    STcat     (szSqlStr, CAT_PROC);

    CatWhere  (szSqlStr, parmOwner, pstringid->fProcOwner, szOwner, 0);
    CatWhere  (szSqlStr, parmProc,  pstringid->fProcName,  szProc, 0);

    CatRemove (szSqlStr, AND);
    CatRemove (szSqlStr, WHERE);

    STcat(szSqlStr, " ORDER BY 2, 3, 4, 7");

    rc = CatExecDirect (pstmt, szSqlStr, CATLG_PROC);

/*  MEfree (szSqlStr); szSqlStr will be freed by ResetStmt,FreeSqlStr */
    UnlockStmt (pstmt);
    return rc;

}


/*
**  SQLProcedureColumns                         Level 2
**  
**  Returns the list of procedure input/output parameters.
**
**  On entry: pstmt           -->statement control block.
**            szProcQualifier -->qualifier name.   (ignored)
**            cbProcQualifier = length of qualifier name.
**            szProcOwner     -->owner (schema) name search string.
**            cbProcOwner     = length of owner name.
**            szProcName      -->table name search string.
**            cbProcName      = length of table name.
**            szColumnName    -->column name search string.
**            cbColumnName    = length of column name.
**
**  On exit:
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
**
**  Notes:
**            Arguments szProcOwner, szProcName, and szColumnName can contain 
**            search pattern characters like "%" and "_".
**
**            SELECT DISTINCT
**                   ' ',                     AS PROCEDURE_QUALIFIER
**                   procedure_owner,         AS PROCEDURE_OWNER
**                   procedure_name,          AS PROCEDURE_NAME
**                   param_name,              AS COLUMN_NAME
**                   1,                       AS COLUMN_TYPE (always SQL_PARAM_INPUT)
**                   param_datatype_code,     AS DATA_TYPE (ODBC type code) (-1 if DB2)
**                   param_datatype,          AS TYPE_NAME (human readable)
**                   param_length,            AS COLUMN_SIZE
**                   param_length,            AS BUFFER_LENGTH
**                   param_scale,             AS DECIMAL_DIGITS
**                   10,                      AS NUM_PREC_RADIX
**                   param_nulls,             AS NULLABLE
**                   ' '                      AS REMARKS
**               FROM iiproc_params
**               WHERE 
**                   PROCEDURE_OWNER LIKE <szOwner> AND    -- or
**                   PROCEDURE_OWNER    = <szOwner> AND
**                   PROCEDURE_NAME  LIKE <szTable> AND    -- or
**                   PROCEDURE_NAME     = <szTable> AND
**                   param_name      LIKE <szColumn>            -- or
**                   param_name         = <szColumn>
**               ORDER BY 2,3"
**
*/

#define QRY_PROCCOL_QUALIFIER   1
#define QRY_PROCCOL_OWNER       2
#define QRY_PROCCOL_PROC        3
#define QRY_PROCCOL_COLUMN      4
#define QRY_PROCCOL_COLUMNTYPE  5
#define QRY_PROCCOL_DATA_TYPE   6
#define QRY_PROCCOL_TYPE_NAME   7
#define QRY_PROCCOL_PRECISION   8
#define QRY_PROCCOL_LENGTH      9
#define QRY_PROCCOL_SCALE       10
#define QRY_PROCCOL_RADIX       11
#define QRY_PROCCOL_NULLABLE    12
#define QRY_PROCCOL_REMARKS     13
#define QRY_PROCCOL_DEFAULT     14  /* begin 3.x columns */
#define QRY_PROCCOL_VERBOSETYPE 15
#define QRY_PROCCOL_SUBTYPE     16
#define QRY_PROCCOL_OCTETLENGTH 17
#define QRY_PROCCOL_POSITION    18
#define QRY_PROCCOL_ISNULLABLE  19

#define CAT_PROCCOL \
   "SELECT DISTINCT " \
          "' ', "                   /* PROCEDURE_QUALIFIER */ \
          "procedure_owner, "       /* PROCEDURE_OWNER    */  \
          "procedure_name, "        /* PROCEDURE_NAME     */  \
          "param_name, "            /* COLUMN_NAME */         \
          "param_sequence, "        /* COLUMN_TYPE (always SQL_PARAM_INPUT=1) */ \
          "param_datatype_code, "   /* DATA_TYPE (ODBC type code) (-1 if DB2) */ \
          "param_datatype, "        /* TYPE_NAME (human readable) */ \
          "param_length, "          /* COLUMN_SIZE */ \
          "param_length, "          /* BUFFER_LENGTH  */ \
          "param_scale, "           /* DECIMAL_DIGITS */ \
          "10, "                    /* NUM_PREC_RADIX */ \
          "param_nulls, "           /* NULLABLE  */ \
          "' ', "                   /* REMARKS   */ \
          "' ', "                   /* COLUMN_DEF   begin 3.x cols */ \
          "param_datatype_code, "   /* SQL_DATA_TYPE */ \
          "0, "                     /* DATETIME_SUB  */ \
          "param_length, "          /* CHAR_OCTET_LENGTH */ \
          "param_sequence, "        /* ORDINAL_POSITION  */ \
          "'YES' "                  /* IS_NULLABLE   */ \
     "FROM iiproc_params " \
     "WHERE \0"
/*         PROCEDURE_OWNER LIKE <szOwner> AND    -- or */
/*         PROCEDURE_OWNER    = <szOwner> AND */
/*         PROCEDURE_NAME  LIKE <szTable> AND    -- or */
/*         PROCEDURE_NAME     = <szTable> AND */
/*         param_name      LIKE <szColumn>            -- or */
/*         param_name         = <szColumn> */
/*    ORDER BY 2,3,5" */


/*
**  SQLProcedureColumns using Gateway's catalog:
*/
#define CAT_PROCCOL_GATEWAY \
   "SELECT DISTINCT " \
          "' ', "                   /* PROCEDURE_QUALIFIER */ \
          "proc_owner, "            /* PROCEDURE_OWNER    */  \
          "proc_name, "             /* PROCEDURE_NAME     */  \
          "param_name, "            /* COLUMN_NAME */         \
          "param_sequence, "        /* COLUMN_TYPE (always SQL_PARAM_INPUT) */     \
          "param_type, "            /* DATA_TYPE (ODBC type code) (-1 if DB2) */   \
          "'                                ', "   /* TYPE_NAME (human readable) */\
          "param_length, "          /* COLUMN_SIZE */ \
          "param_length, "          /* BUFFER_LENGTH  */ \
          "param_scale, "           /* DECIMAL_DIGITS */ \
          "10, "                    /* NUM_PREC_RADIX */ \
          "param_null, "            /* NULLABLE  */ \
          "' ', "                   /* REMARKS   */ \
          "' ', "                   /* COLUMN_DEF   begin 3.x cols */ \
          "param_datatype_code, "   /* SQL_DATA_TYPE */ \
          "0, "                     /* DATETIME_SUB  */ \
          "param_length, "          /* CHAR_OCTET_LENGTH */ \
          "param_sequence, "        /* ORDINAL_POSITION  */ \
          "'YES' "                  /* IS_NULLABLE   */ \
     "FROM IIGWPROCPARAMS " \
     "WHERE \0"
/*         PROCEDURE_OWNER LIKE <szOwner> AND    -- or */
/*         PROCEDURE_OWNER    = <szOwner> AND          */
/*         PROCEDURE_NAME  LIKE <szTable> AND    -- or */
/*         PROCEDURE_NAME     = <szTable> AND          */
/*         param_name      LIKE <szColumn>       -- or */
/*         param_name         = <szColumn>             */
/*    ORDER BY 2,3,5" */


RETCODE SQL_API SQLProcedureColumns(
    SQLHSTMT    hstmt,
    UCHAR  * szProcQualifier,    SWORD  cbProcQualifier,
    UCHAR  * szProcOwner,        SWORD  cbProcOwner,
    UCHAR  * szProcName,         SWORD  cbProcName,
    UCHAR  * szColumnName,       SWORD  cbColumnName)
{   return SQLProcedureColumns_Internal( hstmt,
                szProcQualifier,         cbProcQualifier,
                szProcOwner,             cbProcOwner,
                szProcName,              cbProcName,
                szColumnName,            cbColumnName);
}

RETCODE SQL_API SQLProcedureColumns_Internal(
    SQLHSTMT    hstmt,
    UCHAR  * szProcQualifier,    SWORD  cbProcQualifier,
    UCHAR  * szProcOwner,        SWORD  cbProcOwner,
    UCHAR  * szProcName,         SWORD  cbProcName,
    UCHAR  * szColumnName,       SWORD  cbColumnName)
{
    LPSTMT      pstmt = (LPSTMT)hstmt;
    RETCODE     rc;
    CHAR        bEscape;
    PARM        parmOwner;
    PARM        parmProc;
    PARM        parmColumn;
    LPDBC       pdbc;
    char       *p;
    const STRINGID*  pstringid = &CatStringID;
    LPSTR       szSqlStr  = NULL;
    BOOL        isGateway = FALSE;
    DECLTEMP (szOwner[128])         /* Win32 only */
    DECLTEMP (szProc[65])
    DECLTEMP (szColumn[65])

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLPROCEDURECOLUMNS, pstmt,
            szProcQualifier, cbProcQualifier, szProcOwner, cbProcOwner,
            szProcName, cbProcName, szColumnName, cbColumnName);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;

    if (!isServerClassAnIngresEngine(pdbc))  /* Proc Columns only for INGRES engines*/
        isGateway=TRUE;

    if (!CatConnect (pstmt))
    {
        UnlockStmt (pstmt);
        return SQL_ERROR;
    }

    /*
    **  Set up parms to stuff into syntax string:
    **
    **  1. Get driver's ESCAPE character, if supported.
    **  2. Edit length (allowing for ESCAPE characters, if any).
    **  3. Ensure null terminated.
    **  4. Scan for search patterns, and pad with trailing
    **     blanks or wild cards to make LIKE clause work.
    */
    bEscape    = CatEscape (pstmt);
    parmOwner  = CatParm (pdbc, szOwner,  32, szProcOwner, cbProcOwner, bEscape);
    parmProc   = CatParm (pdbc, szProc,   32, szProcName,  cbProcName,  bEscape);
    parmColumn = CatParm (pdbc, szColumn, 32, szColumnName, cbColumnName, bEscape);

    if (parmOwner  == PARM_INVALID || 
        parmProc   == PARM_INVALID ||
        parmColumn == PARM_INVALID)
        return ErrUnlockStmt (SQL_HY090, pstmt); /* invalid string or buffer length */

    parmOwner = CatParmForceToUser(parmOwner, pdbc, szOwner);

    pstmt->fCommand = CMD_SELECT;      /* avoid parsing command */

    /*
    **  Build SQL query syntax:
    **
    **  - Allocate buffer (SQLExecDirect frees it).
    **  - Load user defined resource containing catalog specific query.
    **  - Add owner and name predicates (required).
    **  - Add catalog specific not null predicate, if needed.
    */
    szSqlStr = CatAlloc (pstmt);
    if (szSqlStr == NULL)
    {
        UnlockStmt (pstmt);
        return (SQL_ERROR);
    }

    STcat (szSqlStr, isGateway?CAT_PROCCOL_GATEWAY:CAT_PROCCOL);

    CatWhere (szSqlStr, parmOwner,  pstringid->fProcOwner,    szOwner,  bEscape);
    CatWhere (szSqlStr, parmProc,   pstringid->fProcName,     szProc,   bEscape);
    CatWhere (szSqlStr, parmColumn, pstringid->fProcColName,  szColumn, bEscape);

    if (isGateway)    /* if gateway, hack to the right column names */
       { 
         if ((p = STstrindex(szSqlStr, " procedure_owner ", 0, FALSE) ) != NULL)
                        MEcopy(" proc_owner      ", 17, p);
         if ((p = STstrindex(szSqlStr, " procedure_name ", 0, FALSE) ) != NULL)
                        MEcopy(" proc_name      ", 16, p);
       }

    CatRemove (szSqlStr, AND);
    CatRemove (szSqlStr, WHERE);

    STcat(szSqlStr, " ORDER BY 2, 3, 5");

    rc = CatExecDirect (pstmt, szSqlStr, CATLG_PROCCOL);

/*  MEfree (szSqlStr); szSqlStr will be freed by ResetStmt,FreeSqlStr */
    UnlockStmt (pstmt);
    return rc;

}


/*
**  SQLSpecialColumns
**
**  Return information about columns that uniquely identify a row.
**
**  IDMS does not support any automatically updated columns.
**  The szTableQualifier parm is ignored since it is not used to identify
**  the table in IDMS.  No error is returned if it is passed becuase
**  IDMS does support it as an attribute of the schema.
**
**  On entry: pstmt           -->statement control block.
**            fColType         = type of column to return.
**            szTableQualifier   ignored.
**            cbTableQualifier   ignored.
**            szTableOwner    -->owner (schema) name.
**            cbTableOwner     = length of owner name.
**            szTableName     -->table name.
**            cbTableName      = length of table name.
**            fScope           = minimum scope of rowid.
**            fNullable        = whether columns can be nullable.
**
**  On exit:  Result set is ready to SQLFetch if no errors.
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
*/
#define QRY_SPEC_SCOPE     1
#define QRY_SPEC_COLUMN    2
#define QRY_SPEC_DATA_TYPE 3
#define QRY_SPEC_TYPE_NAME 4
#define QRY_SPEC_PRECISION 5
#define QRY_SPEC_LENGTH    6
#define QRY_SPEC_SCALE     7
#define QRY_SPEC_PSEUDO    8

#define CAT_SPECIAL_COLUMNS_TID \
   "SELECT DISTINCT " \
          "0 AS SCOPE, "  /* SCOPE (SQL_SCOPE_CURROW) */ \
          "'tid', "       /* COLUMN_NAME */              \
          "30, "          /* DATA_TYPE (IIAPI_INT_TYPE)*/\
          "'INTEGER  ', " /* TYPE_NAME */ \
          "10, "          /* COLUMN_SIZE */ \
          "4, "           /* BUFFER_LENGTH */  \
          "0, "           /* DECIMAL_DIGITS */  \
          "2 "            /* PSEUDO_COLUMN (SQL_PC_PSEUDO) */ \
   "FROM iitables " \
   "WHERE table_type = 'T' AND \0"
/*         TABLE_OWNER = <szOwner> AND   */
/*         TABLE_NAME  = <szTable> AND   */

/*
**  SQLSpecialColumns using unique index keys:
*/
#define CAT_SPECIAL_COLUMNS \
   "SELECT DISTINCT "  \
          "2 AS SCOPE, "               /* SCOPE (SQL_SCOPE_SESSION) */  \
          "c.column_name, "            /* COLUMN_NAME */                \
          "c.column_ingdatatype, "     /* DATA_TYPE (ODBC type code  */ \
          "c.column_datatype, "        /* TYPE_NAME (human readable) */ \
          "c.column_length, "          /* COLUMN_SIZE */ \
          "c.column_length, "          /* BUFFER_LENGTH  */ \
          "c.column_scale, "           /* DECIMAL_DIGITS */ \
          "1 "                         /* PSEUDO_COLUMN (SQL_PC_NOT_PSEUDO) */ \
     "FROM iicolumns c, iiindex_columns ic, iiindexes i " \
     "WHERE c.table_name = i.base_name AND "     \
           "c.table_owner = i.base_owner AND "   \
           "ic.index_name = i.index_name AND "   \
           "ic.index_owner = i.index_owner AND " \
           "ic.column_name = c.column_name AND " \
           "i.unique_rule  = 'U' AND "           \
           "$"  /* marker to allow driver to chop off column_null exclusion */ \
           "c.column_nulls  <> 'Y' AND \0"
/*         i.base_owner = <szOwner> AND   */
/*         i.base_name  = <szTable> AND   */


/*
**  SQLSpecialColumns using primary key (defined in iicolumns):
*/
#define CAT_SPECIAL_COLUMNS_PRIMARY_KEY \
   "SELECT DISTINCT "  \
          "2 AS SCOPE, "               /* SCOPE (SQL_SCOPE_SESSION) */  \
          "c.column_name, "            /* COLUMN_NAME */                \
          "c.column_ingdatatype, "     /* DATA_TYPE (ODBC type code  */ \
          "c.column_datatype, "        /* TYPE_NAME (human readable) */ \
          "c.column_length, "          /* COLUMN_SIZE */ \
          "c.column_length, "          /* BUFFER_LENGTH  */ \
          "c.column_scale, "           /* DECIMAL_DIGITS */ \
          "1 "                         /* PSEUDO_COLUMN (SQL_PC_NOT_PSEUDO) */ \
     "FROM iitables t, iicolumns c "            \
     "WHERE t.table_name = c.table_name AND "   \
           "t.table_owner = c.table_owner AND " \
           "t.unique_rule  = 'U' AND c.key_sequence <> 0 AND \0"
/*          c.table_owner = <szOwner> AND */
/*          c.table_name  = <szTable> */


/*
**  SQLSpecialColumns using unique index keys (DB2 and other gateways).
**  DB2 has a limit of 15 tables.  So we break up the query.
*/
#define CAT_SPECIAL_COLUMNS_GATEWAY1 \
   "SELECT DISTINCT " \
          "i.index_name, "             /* index name  */ \
          "i.index_owner, "            /* index owner */ \
          "i.base_name, "              /* table name  */ \
          "i.base_owner "              /* table owner */ \
     "FROM iiindexes i " \
     "WHERE i.unique_rule  = 'U' AND \0"
/*         i.base_owner = <szOwner> AND   */
/*         i.base_name  = <szTable> AND   */

#define CAT_SPECIAL_COLUMNS_GATEWAY2 \
   "SELECT DISTINCT " \
          "2 AS SCOPE, "               /* SCOPE (SQL_SCOPE_SESSION) */  \
          "c.column_name, "            /* COLUMN_NAME */                \
          "c.column_ingdatatype, "     /* DATA_TYPE (ODBC type code  */ \
          "c.column_datatype, "        /* TYPE_NAME (human readable) */ \
          "c.column_length, "          /* COLUMN_SIZE */ \
          "c.column_length, "          /* BUFFER_LENGTH  */ \
          "c.column_scale, "           /* DECIMAL_DIGITS */ \
          "1 "                         /* PSEUDO_COLUMN (SQL_PC_NOT_PSEUDO) */ \
     "FROM iicolumns c, iiindex_columns ic "     \
     "WHERE c.column_name = ic.column_name AND " \
           "$"  /* marker to allow driver to chop off column_null exclusion */ \
           "c.column_nulls  <> 'Y' AND \0"
/*          ic.index_name =  <i.index_name>  AND */
/*          ic.index_owner = <i.index_owner> AND */
/*          c.table_name =   <i.base_name>   AND */
/*          c.table_owner =  <i.base_owner>  AND */


RETCODE SQL_API SQLSpecialColumns(
    SQLHSTMT    hstmt,
    UWORD       fColType,
    UCHAR  * szTableQualifier,
    SWORD       cbTableQualifier,
    UCHAR  * szTableOwner,
    SWORD       cbTableOwner,
    UCHAR  * szTableName,
    SWORD       cbTableName,
    UWORD       fScope,
    UWORD       fNullable)
{   return SQLSpecialColumns_InternalCall(
                hstmt,
                fColType,
                szTableQualifier, cbTableQualifier,
                szTableOwner,     cbTableOwner,
                szTableName,      cbTableName,
                fScope,
                fNullable);
}

RETCODE SQL_API SQLSpecialColumns_InternalCall(
    SQLHSTMT    hstmt,
    UWORD       fColType,
    UCHAR  * szTableQualifier,
    SWORD       cbTableQualifier,
    UCHAR  * szTableOwner,
    SWORD       cbTableOwner,
    UCHAR  * szTableName,
    SWORD       cbTableName,
    UWORD       fScope,
    UWORD       fNullable)
{
    LPSTMT      pstmt = (LPSTMT)hstmt;
    BOOL        rc;
    RETCODE     retcode;
    int         i;
    BOOL        bNeedToBreakUpQuery=FALSE;
    PARM        parmOwner;
    PARM        parmTable;
    CHAR   * szSqlStr;
    CHAR   * TempszSqlStr=NULL;
    CHAR   * s;
    CHAR   * basename;
    CHAR   * baseowner;
    CHAR   * indexname;
    CHAR   * indexowner;
    LPDBC       pdbc;
    const STRINGID*  pstringid = &CatStringID;
    IIAPI_QUERYPARM     queryParm={0};
    IIAPI_GETDESCRPARM  getDescrParm={0};
    IIAPI_GETCOLPARM    getColParm={0};
    IIAPI_DESCRIPTOR   *pDescriptor;
    IIAPI_STATUS        gp_status;
    DECLTEMP (szOwner[128])     /* Win32 only */
    DECLTEMP (szTable[65])      /* Use static globals in Win16 */

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLSPECIALCOLUMNS, pstmt, fColType,
            szTableQualifier, cbTableQualifier, szTableOwner, cbTableOwner,
            szTableName, cbTableName, fScope, fNullable);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;

    /* DB2 has a limit of 15 tables.  The normal 3-way join of the
       iicolumns, iiindex_columns, iiindexes views pushes it over the limit.
       VSAM/IMS have a gateway bug with 3-way join (puts gateway into loop).*/
    if (isServerClassDB2(pdbc)      ||
        isServerClassVSAM(pdbc)     ||
        isServerClassIMS(pdbc))
             bNeedToBreakUpQuery=TRUE;

    if (!CatConnect (pstmt))
    {
        UnlockStmt (pstmt);
        return SQL_ERROR;
    }

    /*
    **  Edit parms:
    **
    **  1. Edit length, punt if error.
    **  2. Ensure null terminated and save in local work field.
    */
    parmOwner = CatParmName (pdbc, szOwner, szTableOwner, cbTableOwner);
    parmTable = CatParmName (pdbc, szTable, szTableName,  cbTableName);

    if (parmOwner == PARM_INVALID || 
        parmTable == PARM_INVALID ||
        parmTable == PARM_NULL)
        return ErrState (SQL_HY090, pstmt); /* invalid string or buffer length */

    parmOwner = CatParmForceToUser(parmOwner, pdbc, szOwner);

    /*
    **  Build SQL query syntax:
    **
    **  1. Allocate buffer (SQLExecDirect frees it).
    **  2. Load user defined resource containing table of string id's.
    **  3. Load user defined resource containing catalog specific query.
    **  4. Add owner and table predicates (required).
    **  6. Add catalog specific not null predicate, if needed.
    **  7. Force no results if not SQL_BEST_ROW_ID.
    **  8. Free string id table.
    */
    szSqlStr = CatAlloc (pstmt);
    if (szSqlStr == NULL)
    {
        UnlockStmt (pstmt);
        return (SQL_ERROR);
    }

    /*
    **  If user wants SQL_BEST_ROW_ID for scope of a transaction, use TID.
    **  If user wants SQL_ROWVER (what columns
    **  are automatcally updated (none)) then build a null query.
    */
    if (fColType != SQL_BEST_ROWID  ||  
        (fScope == SQL_SCOPE_CURROW  &&  (isServerClassINGRES(pdbc) || DEBUGGING_64_CATALOG)))
    {
        STcat     (szSqlStr, CAT_SPECIAL_COLUMNS_TID);

        CatWhere  (szSqlStr, parmOwner, pstringid->fOwnerName, szOwner, 0);
        CatWhere  (szSqlStr, parmTable, pstringid->fTableName, szTable, 0);

        if (fColType != SQL_BEST_ROWID)     /* if SQL_ROWVER (or anything else) */ 
            STcat (szSqlStr, "table_type <> table_type");       /*   no rows  */
        else
            CatRemove (szSqlStr, AND);

        retcode = CatExecDirect (pstmt, szSqlStr, CATLG_SPECCOL);

/*      MEfree (szSqlStr); szSqlStr will be freed by ResetStmt,FreeSqlStr */
        UnlockStmt (pstmt);
        return retcode;
    }

    /*
    **  If we're still here, the user wants SQL_BEST_ROW_ID for  
    **  more than transaction scope (use iicolumns or iiindexes, iiindex_columns
    **  to find unique columns).
    */

              /* first try using the unique primary key if there is one */
    STcat     (szSqlStr, CAT_SPECIAL_COLUMNS_PRIMARY_KEY);
    CatWhere  (szSqlStr, parmOwner, pstringid->fColumnOwner, szOwner, 0);
    CatWhere  (szSqlStr, parmTable, pstringid->fColumnTable, szTable, 0);
    CatRemove (szSqlStr, AND);

    if (    (rc=CatQueryOpen      (pstmt, &queryParm, szSqlStr))==TRUE)
        if ((rc=CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))==TRUE)
             rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, &getColParm);
    if (!rc) /* if error occurred, close the query and free up resources */
       {CatQueryClose(pstmt, &queryParm, &getDescrParm, NULL);  /* free up */
        UnlockStmt (pstmt);
        return (SQL_ERROR);
       }
    gp_status=getColParm.gc_genParm.gp_status;  /* save the end-of-data status */
    CatQueryClose(pstmt, &queryParm, &getDescrParm, &getColParm);  /* close and free up */
    if (gp_status != IIAPI_ST_NO_DATA)     /* if a row found - success! */
       {goto SQLSpecialColumns_ExecDirect; /* reexecute the the string and    */
       }                                   /*   make it the active result set */


          /* there is no unique primary key; try the secondary indexes */
    CMcpychar("\0",szSqlStr); /* wipe the string syntax slate clean    */
    if (bNeedToBreakUpQuery)  /* if DB2 or other gateway that needs break-up */
       STcat (szSqlStr, CAT_SPECIAL_COLUMNS_GATEWAY1);
    else 
       STcat (szSqlStr, CAT_SPECIAL_COLUMNS);

    if ((s = STindex(szSqlStr, "$", 0)) != NULL)
       {if (fNullable == SQL_NO_NULLS)  /* if nullable columns not wanted     */
             CMcpychar(" ",s);          /*   allow columns_null check         */
        else CMcpychar("\0",s);         /* else allow columns_null = anything */
       }

    CatWhere  (szSqlStr, parmOwner, pstringid->fIndexTableOwner, szOwner, 0);
    CatWhere  (szSqlStr, parmTable, pstringid->fIndexTableName,  szTable, 0);

    CatRemove (szSqlStr, AND);

    /*
    **  Prepare the query and open the cursor on catalog connection:
    */
    if (bNeedToBreakUpQuery)  /* if DB2 or other gateway that needs break-up */
    {
    /* DB2 has a limit of 15 tables.  The normal 3-way join of the
       iicolumns, iiindex_columns, iiindexes views pushes it over the limit */

    TempszSqlStr = MEreqmem((u_i2)0, 1000, TRUE, NULL);

    STcat (szSqlStr, CAT_SPECIAL_COLUMNS_GATEWAY2);
       /* set up the second part of the query */

    if ((s = STindex (TempszSqlStr, "$", 0)) != NULL)
       {if (fNullable == SQL_NO_NULLS)  /* if nullable columns not wanted     */
             CMcpychar(" ",s);          /*   allow columns_null check         */
        else CMcpychar("\0",s);         /* else allow columns_null = anything */
       }

    if (    (rc=CatQueryOpen      (pstmt, &queryParm, szSqlStr))==TRUE)
        if ((rc=CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))==TRUE)
             rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, &getColParm);
    if (!rc) /* if error occurred, close the query and free up resources */
       {CatQueryClose(pstmt, &queryParm, &getDescrParm, NULL);  /* free up */
        UnlockStmt (pstmt);
        return (SQL_ERROR);
       }

    pDescriptor=getDescrParm.gd_descriptor;

    indexname =getColParm.gc_columnData[0].dv_value;
    indexowner=getColParm.gc_columnData[1].dv_value;
    basename  =getColParm.gc_columnData[2].dv_value;
    baseowner =getColParm.gc_columnData[3].dv_value;
    
    do        /* get the first unique index, use it as key for iiindex_columns*/
    {
        char *stringlist[] = {
                              "ic.index_name = \'",
                              "ic.index_owner = \'",
                              "c.table_name =  \'",
                              "c.table_owner =  \'"};
        if ( getColParm.gc_genParm.gp_status == IIAPI_ST_NO_DATA )
        {
            STcat (TempszSqlStr, 
              "c.column_name <> c.column_name AND c.column_name = 'x'"); /* no rows */
            break;
        }

        for (i=0; i < getColParm.gc_columnCount; i++)
            { s = getColParm.gc_columnData[i].dv_value;
              if (pDescriptor++->ds_dataType == IIAPI_VCH_TYPE)  /* if varchar returned */
                  s +=2;            		  /* bump past length    */
              STtrmwhite(s);                 /* strip trailing spaces from names */
              STcat(TempszSqlStr, stringlist[i]);  /* ic.index_name = ' */
              STcat(TempszSqlStr, s);              /* ic.index_name = 'myindex */
              STcat(TempszSqlStr, "\' AND ");      /* ic.index_name = 'myindex' AND */
            }
        CatRemove (TempszSqlStr, AND);
    } while(0); /* end convenient loop */

    CatQueryClose(pstmt, &queryParm, &getDescrParm, &getColParm);  /* close and free up */
    
    STcopy(TempszSqlStr,szSqlStr);
    }  /* end if (bNeedToBreakUpQuery) */

    if (TempszSqlStr)
       MEfree(TempszSqlStr);

  SQLSpecialColumns_ExecDirect:
    retcode = CatExecDirect (pstmt, szSqlStr, CATLG_SPECCOL);

/*  MEfree (szSqlStr); szSqlStr will be freed by ResetStmt,FreeSqlStr*/
    UnlockStmt (pstmt);
    return retcode;
}  /* end SQLSpecialColumns */


/*
**  SQLStatistics
**
**  Return statistics for a table and its indexes.
**
**  1. If fAccuracy = SQL_ENSURE execute UPDATE STATISTICS
**     (this CAN cause a time out, if the table is very large).
**     The user must have ALTER authority on the table to use
**     this option.
**
**  2. Prepare and execute a query from the catalog.  The user must
**     have SELECT authority on the SYSTEM schema.  Note that the
**     szTableQualifier parm is ignored, since it is not used by IDMS to
**     identify a table, but is actually assocated with the schema.  An error
**     is not returned if it is used, since it is returned.
**
**  On entry: pstmt           -->statement control block.
**            szTableQualifier-->ignored.
**            cbTableQualifier = ignored.
**            szTableOwner    -->owner (schema) name.
**            cbTableOwner     = length of owner name.
**            szTableName     -->table name.
**            cbTableName      = length of table name.
**            fUnique          = type of index.
**            fAccuracy        = Importannce of cardinality.
**
**  On exit:  Result set is ready to SQLFetch if no errors.
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
*/
#define QRY_STAT_QUALIFIER  1
#define QRY_STAT_OWNER      2
#define QRY_STAT_TABLE      3
#define QRY_STAT_UNIQUE     4
#define QRY_STAT_IQUALIFIER 5
#define QRY_STAT_INDEX      6
#define QRY_STAT_TYPE       7
#define QRY_STAT_SEQUENCE   8
#define QRY_STAT_COLUMN     9
#define QRY_STAT_COLLATION  10
#define QRY_STAT_CARD       11
#define QRY_STAT_PAGES      12
#define QRY_STAT_FILTER     13

#define CAT_STATS_TABLE \
   "SELECT ' ', "                                 /* TABLE_CAT       */ \
          "table_owner, "                         /* TABLE_SCHEM     */ \
          "table_name, "                          /* TABLE_NAME      */ \
          "'        ', "                          /* NON_UNIQUE      */ \
          "'        ', "                          /* INDEX_QUALIFIER */ \
          "'                                ', "  /* INDEX_NAME      */ \
          "0, "                                   /* TYPE (SQL_TABLE_STAT) */ \
          "0, "                                   /* SEQ_IN_INDEX    */ \
          "'                                ', "  /* COLUMN_NAME     */ \
          "' ', "                                 /* COLLATION       */ \
          "0, "                                   /* CARDINALITY     */ \
          "0, "                                   /* PAGES           */ \
          "' ' "                                  /* FILTER_CONDITION*/ \
     "FROM iitables " \
	 "WHERE system_use <> 'S' AND table_type = 'T' AND " \
           "table_name NOT LIKE 'ii%' AND table_name NOT LIKE 'II%' AND " \
           "table_name NOT LIKE 'sys%' AND table_name NOT LIKE 'SYS%' AND \0"
/*          TABLE_OWNER = <szOwner> AND */
/*          TABLE_NAME  = <szTable> */

#define CAT_STATS_INDEX_CU \
   " UNION "  \
   "SELECT ' ', "                  /* TABLE_CAT       */  \
          "i.base_owner, "         /* TABLE_SCHEM     */  \
          "i.base_name, "          /* TABLE_NAME      */  \
          "i.unique_rule, "        /* NON_UNIQUE      remap char8 to smallint*/ \
          "i.index_owner, "        /* INDEX_QUALIFIER */  \
          "i.index_name, "         /* INDEX_NAME      */  \
          "3, "                    /* TYPE  (SQL_INDEX_OTHER) */ \
          "ic.key_sequence, "      /* SEQ_IN_INDEX    */  \
          "ic.column_name, "       /* COLUMN_NAME     */  \
          "ic.sort_direction, "    /* COLLATION       remap char8 to char1*/ \
          "0, "                    /* CARDINALITY     */  \
          "0, "                    /* PAGES           */  \
          "' ' "                   /* FILTER_CONDITION*/  \
     "FROM iiindexes i, iiindex_columns ic "     \
     "WHERE i.index_name = ic.index_name AND "   \
           "i.index_owner = ic.index_owner AND " \
           "$"  /* marker to allow driver to chop off unique_rule exclusion */ \
           "i.unique_rule  = 'U' AND \0"
/*         i.base_owner = <szOwner> AND   */
/*         i.base_name  = <szTable> AND   */

#define CAT_STATS_INDEX_OU  /* primary key fields */  \
   " UNION "      \
   "SELECT ' ', "                  /* TABLE_CAT       */ \
          "c.table_owner, "        /* TABLE_SCHEM     */ \
          "c.table_name, "         /* TABLE_NAME      */ \
          "t.unique_rule, "        /* NON_UNIQUE      remap char8 to smallint*/ \
          "' ', "                  /* INDEX_QUALIFIER */ \
          "'None', "               /* INDEX_NAME      */ \
          "3, "                    /* TYPE  (SQL_INDEX_OTHER) */ \
          "c.key_sequence, "       /* SEQ_IN_INDEX    */ \
          "c.column_name, "        /* COLUMN_NAME     */ \
          "c.sort_direction, "     /* COLLATION       remap char8 to char1*/ \
          "0, "                    /* CARDINALITY     */ \
          "0, "                    /* PAGES           */ \
          "' ' "                   /* FILTER_CONDITION*/ \
     "FROM iitables t, iicolumns c "            \
     "WHERE t.table_name = c.table_name AND "   \
           "t.table_owner = c.table_owner AND " \
           "t.unique_rule  = 'U' AND c.key_sequence <> 0 AND \0"
/*          TABLE_OWNER = <szOwner> AND */
/*          TABLE_NAME  = <szTable> */

#define CAT_STATS_ORDER_BY     " ORDER BY 4, 7, 5, 6, 8"

RETCODE SQL_API SQLStatistics(
    SQLHSTMT    hstmt,
    UCHAR  * szTableQualifier,
    SWORD       cbTableQualifier,
    UCHAR  * szTableOwner,
    SWORD       cbTableOwner,
    UCHAR  * szTableName,
    SWORD       cbTableName,
    UWORD       fUnique,
    UWORD       fAccuracy)
{   return SQLStatistics_InternalCall(
                hstmt,
                szTableQualifier, cbTableQualifier,
                szTableOwner,     cbTableOwner,
                szTableName,      cbTableName,
                fUnique,
                fAccuracy);
}

RETCODE SQL_API SQLStatistics_InternalCall(
    SQLHSTMT    hstmt,
    UCHAR  * szTableQualifier,
    SWORD       cbTableQualifier,
    UCHAR  * szTableOwner,
    SWORD       cbTableOwner,
    UCHAR  * szTableName,
    SWORD       cbTableName,
    UWORD       fUnique,
    UWORD       fAccuracy)
{
    LPSTMT      pstmt = (LPSTMT)hstmt;
    RETCODE     rc;
    PARM        parmOwner;
    PARM        parmTable;
    BOOL        fQuery;
    CHAR   * szSqlStr;
    CHAR   * s;
    LPDBC       pdbc;
    const STRINGID*  pstringid = &CatStringID;
    DECLTEMP (szOwner[128])     /* Win32 only */
    DECLTEMP (szTable[65])      /* Use static globals in Win16 */

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLSTATISTICS, pstmt,
            szTableQualifier, cbTableQualifier, szTableOwner, cbTableOwner,
            szTableName, cbTableName, fUnique, fAccuracy);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;

    if (!CatConnect (pstmt))
    {
        UnlockStmt (pstmt);
        return SQL_ERROR;
    }

    /*
    **  Set up parms to stuff into syntax string:
    **
    **  1. Edit length.
    **  2. Ensure null terminated.
    */
    parmOwner = CatParmName (pdbc, szOwner, szTableOwner, cbTableOwner);
    parmTable = CatParmName (pdbc, szTable, szTableName,  cbTableName);

    if (parmOwner == PARM_INVALID || 
        parmTable == PARM_INVALID ||
        parmTable == PARM_NULL)
        return ErrState (SQL_HY090, pstmt); /* invalid string or buffer length */

    parmOwner = CatParmForceToUser(parmOwner, pdbc, szOwner);

    fQuery = *szTable;

    /*
    **  Build SQL query syntax:
    **
    **  1. Allocate buffer (SQLExecDirect frees it).
    **  2. Load user defined resource containing table statistics query.
    **  3. Add owner and table predicates (required).
    **  4. Add other stuff, see details that follow.
    **  5. Add sort clause.
    **  6. Free string id table.
    */
    szSqlStr = CatAlloc (pstmt);

    STcat (szSqlStr, CAT_STATS_TABLE);
    if (fQuery)
    {
        CatWhere  (szSqlStr, parmOwner, pstringid->fOwnerName, szOwner, 0);
        CatWhere  (szSqlStr, parmTable, pstringid->fTableName, szTable, 0);
        CatRemove (szSqlStr, AND);

        s = szSqlStr + STlength(szSqlStr); /* ptr to start of INDEX_CU string*/
        STcat (szSqlStr, CAT_STATS_INDEX_CU);

        if ((s = STindex (s, "$", 0)) != NULL) 
                                           /* scan INDEX_CU string for marker */
           {if (fUnique == SQL_INDEX_ALL)  /* if all indexes wanted       */
                 CMcpychar("\0",s);        /*   chop off unique_rule check*/
            else CMcpychar(" ",s);         /* else keep test for unique   */
           }

        CatWhere  (szSqlStr, parmOwner, pstringid->fIndexTableOwner, szOwner, 0);
        CatWhere  (szSqlStr, parmTable, pstringid->fIndexTableName,  szTable, 0);
        CatRemove (szSqlStr, AND);

        s = szSqlStr + STlength(szSqlStr); /* ptr to start of INDEX_OU string*/
        STcat (szSqlStr, CAT_STATS_INDEX_OU);

        CatWhere  (szSqlStr, parmOwner, pstringid->fColumnOwner, szOwner, 0);
        CatWhere  (szSqlStr, parmTable, pstringid->fColumnTable, szTable, 0);
        CatRemove (szSqlStr, AND);

        STcat     (szSqlStr, CAT_STATS_ORDER_BY);
    }
    else
    {
        STcat (szSqlStr,                       /* no rows if no table name */
                 "table_type <> table_type AND table_name ='x'");
    }

    /*
    **  Prepare the query and open the cursor on catalog connection:
    */
    rc = CatExecDirect (pstmt, szSqlStr, CATLG_STATS);

/*  MEfree (szSqlStr); szSqlStr will be freed by ResetStmt,FreeSqlStr */
    UnlockStmt (pstmt);
    return (rc);
}


/*
**  SQLTables
**
**  Return schema or table names from the SQL catalog
**  and an associated network dictionary.
**
**  On entry: pstmt           -->statement control block.
**            szTableQualifier-->qualifier name.
**            cbTableQualifier = length of qualifier name.
**            szTableOwner    -->owner (schema) name.
**            cbTableOwner     = length of owner name.
**            szTableName     -->table name.
**            cbTableName      = length of table name.
**            szTableType     -->table type.
**            cbTableType      = length of table type.
**
**  On exit:
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
*/

#define QRY_TABL_QUALIFIER  1
#define QRY_TABL_OWNER      2
#define QRY_TABL_TABLE      3
#define QRY_TABL_TYPE       4
#define QRY_TABL_REMARKS    5

#define CAT_TABLES_OWNER \
     "SELECT DISTINCT "\
     "' ', table_owner AS TABLE_OWNER, ' ', ' ', ' ' "\
     "FROM iitables                     ORDER BY TABLE_OWNER\0"

#define CAT_TABLES_TABLE  \
    "SELECT ' ', "        \
           "table_owner, "\
           "table_name, " \
           "'TABLE       ' AS TABLE_TYPE, "\
           "' ' "\
     "FROM iitables "\
   " WHERE system_use <> 'S' AND (table_type  IN ('T', 'P')) AND "           \
           "table_name NOT LIKE 'ii%' AND table_name NOT LIKE 'II%' AND "    \
           "table_name NOT LIKE 'sys%' AND table_name NOT LIKE 'SYS%' AND \0"
/*          OWNER LIKE <szOwner> AND                -- or */
/*          OWNER = <szOwner> AND                   -- can't = 'SYSTEM' */
/*          NAME LIKE <szTable>  '\'                -- or */
/*          NAME = <szTable> */

#define CAT_TABLES_USESYSTABLE   \
    "SELECT ' ', "               \
           "table_owner, "       \
           "table_name, "        \
           "'TABLE       ' AS TABLE_TYPE, " \
           "' ' " \
      "FROM iitables " \
   " WHERE system_use <> 'S' AND (table_type  IN ('T', 'P')) AND " \
           "table_name NOT LIKE 'ii%' AND table_name NOT LIKE 'II%' AND "
/*          OWNER LIKE <szOwner> AND                -- or */
/*          OWNER = <szOwner> AND                   -- can't = 'SYSTEM' */
/*          NAME LIKE <szTable>  '\'                -- or */
/*          NAME = <szTable> */

#define CAT_TABLES_SYSTEM  \
    "SELECT ' ', "         \
           "table_owner, " \
           "table_name, "  \
           "'SYSTEM TABLE' AS TABLE_TYPE, " \
           "' ' "      \
      "FROM iitables " \
     "WHERE (system_use = 'S' OR "   /* warning:  gateways always set system_use=' ' */ \
           "table_name LIKE 'ii%' OR table_name LIKE 'II%' OR " \
           "table_name LIKE 'sys%' OR table_name LIKE 'SYS%') AND " \
           "table_type <> 'I' AND \0"     /* not sure if right but views are included */
/*          OWNER LIKE <szOwner> AND                -- or (seems silly, */
/*          OWNER = <szOwner> AND                   --     but its needed) */
/*          NAME LIKE <szTable>                     -- or */
/*          NAME = <szTable> */

#define CAT_TABLES_VIEW   \
    "SELECT ' ', "        \
           "table_owner, "\
           "table_name, " \
           "'VIEW        ' AS TABLE_TYPE, "\
           "' ' "\
      "FROM iitables "\
   " WHERE system_use <> 'S' AND table_type = 'V' AND "\
           "table_name NOT LIKE 'ii%' AND table_name NOT LIKE 'II%' AND "\
           "table_name NOT LIKE 'sys%' AND table_name NOT LIKE 'SYS%' AND \0"
/*          OWNER LIKE <szOwner> AND                -- or */
/*          OWNER = <szOwner> AND                   -- can't = 'SYSTEM' */
/*          NAME LIKE <szTable>  '\'                -- or */
/*          NAME = <szTable> */

#define CAT_USE_SYSTABLES_VIEW \
    "SELECT ' ', "             \
           "table_owner, "     \
           "table_name, "      \
           "'VIEW        ' AS TABLE_TYPE, " \
           "' ' "      \
      "FROM iitables " \
   " WHERE system_use <> 'S' AND table_type = 'V' AND " \
           "table_name NOT LIKE 'ii%' AND table_name NOT LIKE 'II%' AND \0"
/*          OWNER LIKE <szOwner> AND                -- or */
/*          OWNER = <szOwner> AND                   -- can't = 'SYSTEM' */
/*          NAME LIKE <szTable>  '\'                -- or */
/*          NAME = <szTable> */

#define CAT_TABLES_ORDER_BY    " ORDER BY 4, 2, 3"

static const struct tdescTableTypeInfo
 {   char          Name[33];           /* column name */
     i2            fIngApiType;        /* Ingres API data type */
     SQLINTEGER    OctetLength;        /* length in bytes of data */
     SQLSMALLINT   Nullable;           /* SQL_NULLABLE if columns can have NULLs.
                                          SQL_NO_NULLS if columns has no NULLs. */
 } descTableTypeInfo[]
=
 {
  "TABLE_CAT",       IIAPI_VCH_TYPE,  3, SQL_NULLABLE,
  "TABLE_SCHEM",     IIAPI_VCH_TYPE,  3, SQL_NULLABLE,
  "TABLE_NAME",      IIAPI_VCH_TYPE,  3, SQL_NULLABLE,
  "TABLE_TYPE",      IIAPI_VCH_TYPE, 20, SQL_NULLABLE,
  "REMARKS",         IIAPI_VCH_TYPE,  3, SQL_NULLABLE,
 };

static const char * dataTableTypeInfo[] =
 {
  "table",
  "view"
 };

RETCODE SQL_API SQLTables(
    SQLHSTMT    hstmt,
    UCHAR  * szTableQualifier,
    SWORD       cbTableQualifier,
    UCHAR  * szTableOwner,
    SWORD       cbTableOwner,
    UCHAR  * szTableName,
    SWORD       cbTableName,
    UCHAR  * szTableType,
    SWORD       cbTableType)
{   
    return SQLTables_InternalCall(
                hstmt,
                szTableQualifier, cbTableQualifier,
                szTableOwner,     cbTableOwner,
                szTableName,      cbTableName,
                szTableType,      cbTableType);
}

RETCODE SQL_API SQLTables_InternalCall(
    SQLHSTMT    hstmt,
    UCHAR  * szTableQualifier,
    SWORD       cbTableQualifier,
    UCHAR  * szTableOwner,
    SWORD       cbTableOwner,
    UCHAR  * szTableName,
    SWORD       cbTableName,
    UCHAR  * szTableType,
    SWORD       cbTableType)
{
    LPSTMT      pstmt = (LPSTMT)hstmt;
    RETCODE     rc;
    CHAR        bEscape;
    PARM        parmQual;
    PARM        parmOwner;
    PARM        parmTable;
    UWORD       fType;
    LPDBC       pdbc;
    BOOL        fAccessible;
    const STRINGID*  pstringid = &CatStringID;
    CHAR   * szSqlStr    = NULL;
    CHAR   * p;
    i4          i;
    i2          dsCount;
    LPDESC      pIRD;           /* -> Implementation Row Descriptor Header */
    LPDESC      pARD;           /* -> Application Row Descriptor Header */
    LPDESCFLD   pird;           /* -> Implementation Row Descriptor column */

    DECLTEMP (szOwner[128])     /* Win32 only */
    DECLTEMP (szTable[65])      /* Use static globals in Win16 */
    DECLTEMP (szColumn[65])

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLTABLES, pstmt,
            szTableQualifier, cbTableQualifier, szTableOwner, cbTableOwner,
            szTableName, cbTableName, szTableType, cbTableType);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;
    fAccessible = pstmt->fOptions & OPT_ACCESSIBLE;

    /*
    **  Set up parms to stuff into syntax string:
    **
    **  1. Get driver's ESCAPE character, if supported.
    **  2. Edit length (allowing for ESCAPE characters, if any).
    **  3. Ensure null terminated.
    **  4. Scan for search patterns, and pad with trailing
    **     blanks or wild cards to make LIKE clause work.
    **  5. Figure out type of request (OWNERS or TABLES).
    **
    **  Note that we only care about qualifier to check for special
    **  case enumerations (i.e. list of owners, types).
    */
    bEscape   = CatEscape (pstmt);
    parmQual  = CatParm (pdbc, szColumn, 32, szTableQualifier, cbTableQualifier, bEscape);
    parmOwner = CatParm (pdbc, szOwner,  32, szTableOwner, cbTableOwner, bEscape);
    parmTable = CatParm (pdbc, szTable,  32, szTableName,  cbTableName,  bEscape);

    if (parmOwner == PARM_INVALID || parmTable == PARM_INVALID)
        return ErrUnlockStmt (SQL_HY090, pstmt); /* invalid string or buffer length */

    /*
    ** Ingres doesn't support catalog schemas, so if this is the only 
    ** item requested, return "not supported" error.
    */
    if (szTableQualifier &&  *szTableQualifier &&
        parmOwner == PARM_EMPTY && parmTable == PARM_EMPTY)
        return ErrUnlockStmt (SQL_IM001, pstmt);
    else
    if (parmQual  == PARM_NULL  &&
        parmOwner == PARM_EMPTY && parmTable == PARM_EMPTY)
        return ErrUnlockStmt (SQL_HYC00, pstmt); /* optional feature not implemented */

    if (parmOwner == PARM_NULL  &&
        parmQual  == PARM_EMPTY && parmTable == PARM_EMPTY)
        pstmt->fCatalog = CATLG_OWNERS;
    else
    if (szTableType && *szTableType == '%' && parmOwner == PARM_EMPTY &&
        parmQual  == PARM_EMPTY && parmTable == PARM_EMPTY)
        pstmt->fCatalog = CATLG_TYPES;
    else
        pstmt->fCatalog = CATLG_TABLES;

    parmOwner = CatParmForceToUser(parmOwner, pdbc, szOwner);

    pstmt->fCommand = CMD_SELECT;      /* avoid parsing command */

    /*
    **  Build query based on request type:
    */
    switch (pstmt->fCatalog)
    {
    case CATLG_CATALOGS:              /* return 0 rows for catalogs */
        if (!CatConnect (pstmt))     /* connect to catalog if not already done */
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

        szSqlStr = CatAlloc (pstmt); /* Allocate syntax buffer */
        if (!szSqlStr)
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

        STcat (szSqlStr, CAT_TABLES_TABLE);
        STcat (szSqlStr, "table_type <> table_type AND table_name = 'ii+odbc'");
        break;

    case CATLG_OWNERS:
        /*
        **  Enumerate table owners from either
        **  catalog table directly or via ACCESSIBLE TABLES view:
        **
        **  1. Establish separate catalog connection.
        **  2. Allocate syntax buffer (SQLExecDirect will free it).
        **  3. Load query syntax.
        **  4. Add ACCESSIBLE TABLES view name if needed.
        */
        if (!CatConnect (pstmt))
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

        szSqlStr = CatAlloc (pstmt);
        if (!szSqlStr)
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

        if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)  /*if CatSchemaNULL=Y */
        {                                            /*   return 0 rows   */
            STcat (szSqlStr, CAT_TABLES_TABLE);
            STcat (szSqlStr, "table_type <> table_type AND table_name = 'ii+odbc'");
            break;
        }
        else
            STcat (szSqlStr, CAT_TABLES_OWNER);
        break;

    case CATLG_TYPES:
        /*
        **  Enumerate table types:
        **
        **  1. "Prepare" the constant query
        **     Load Implementation Row Descriptor (IRD).
        **  2. Pretend to "execute" the constant query
        **     Load Fetch buffer.
        **  3. Set cursor as if we opened it and to end of fetch,
        **     as if we already fetched a buffer.
        */
        dsCount = (i2)(sizeof(descTableTypeInfo)/sizeof(descTableTypeInfo[0]));

        pIRD = pstmt->pIRD;               /* pIRD->Implementation Row Descriptor */
        pARD = pstmt->pARD;               /* pARD->Application Row Descriptor */
        ResetDescriptorFields(pIRD, 0, pIRD->Count);  /* clear all desc fields */
        /*
        **  Ensure that IRD and ARD row descriptors are large enough.
        **  If too few then re-allocate.
        */
        if (dsCount > pIRD->CountAllocated)
            if (ReallocDescriptor(pIRD, dsCount) == NULL)
               {
                rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
                UnlockStmt (pstmt);
                return rc;
               }

        if (dsCount > pARD->CountAllocated)
            if (ReallocDescriptor(pARD, dsCount) == NULL)
               {
                rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
                UnlockStmt (pstmt);
                return rc;
               }

        pIRD->Count = dsCount;
        pird = pIRD->pcol + 1;    /* pird -> 1st IRD field (after bookmark) */

        for (i=0; i < dsCount; i++, pird++)
            {   /* copy descTypeInfo struct into IRD descriptor */
             STcopy(descTableTypeInfo[i].Name, pird->Name);
             pird->fIngApiType = descTableTypeInfo[i].fIngApiType;
             pird->OctetLength = descTableTypeInfo[i].OctetLength;
             pird->Nullable    = descTableTypeInfo[i].Nullable;
             SetDescDefaultsFromType(pdbc, pird);
                         /* Set precision, scale, and other defaults*/
            }

        rc = PrepareConstant (pstmt);
        if (rc != SQL_SUCCESS)
        {
            UnlockStmt (pstmt);
            return rc;
        }

        pstmt->prowCurrent = NULL;
        pstmt->crowBuffer  = sizeof(dataTableTypeInfo)/
                             sizeof(dataTableTypeInfo[0]);

        pstmt->prowCurrent = 
          pstmt->pFetchBuffer = 
             MEreqmem(0, pstmt->cbrow * (pstmt->crowBuffer+1), TRUE, NULL);  

        if (!pstmt->prowCurrent)
        {
            rc = ErrState (SQL_HY000, pdbc, F_OD0063_IDS_ERR_RESOURCE, 
                                 IDU_TYPES, RT_CAT_TABLES_TYPES);
            UnlockStmt (pstmt);
            return rc;
        }

        pstmt->fStatus |= STMT_OPEN + STMT_EOF;

        p = pstmt->prowCurrent;

        for (i=0; (UWORD)i < pstmt->crowBuffer; 
	    i++)  /* loop through the data types */
            {
             char * dp;   /* data pointer in tuple for the column */
             i4   * np;   /* null indicator pointer in tuple for the column */
             i2   * i2p;  /* varchar length pointer in tuple for the column */

             p += pstmt->cbrow;       /* p->tuple to fill in */

             GetTuplePtrs(pstmt, p, QRY_TABL_QUALIFIER, &dp, &np);
             *np = -1;          /* mark as null */

             GetTuplePtrs(pstmt, p, QRY_TABL_OWNER,     &dp, &np);
             *np = -1;          /* mark as null */

             GetTuplePtrs(pstmt, p, QRY_TABL_TABLE,     &dp, &np);
             *np = -1;          /* mark as null */

             GetTuplePtrs(pstmt, p, QRY_TABL_TYPE,      &dp, &np);
             i2p = (i2*) dp;    /* i2p->varchar length in column's data area */
             *i2p = (i2)STlength(dataTableTypeInfo[i]);
             STcopy(dataTableTypeInfo[i], dp+2);

             GetTuplePtrs(pstmt, p, QRY_TABL_REMARKS,   &dp, &np);
             *np = -1;          /* mark as null */
            }  /* end for loop */
     

        UnlockStmt (pstmt);
        return SQL_SUCCESS;

    case CATLG_TABLES:
        /*
        **  First decode type options:
        */
        fType = CatType (pstmt, (CHAR*)szTableType, cbTableType);
        if (fType == TYPE_ERROR)
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

#ifdef CATALOG_CACHE_ENABLED
        /*
        **  If we are cacheing SQLTables result sets
        **  check that a result set has been saved and that
        **  its selection criteria matches the current request.
        **
        **  If it does, use it and we're done here,
        **  otherwise attempt to initialize cache,
        **  save the current selection criteria unless
        **  another STMT is saving to cache, and continue.
        */
        if (pdbc->fOptions & OPT_CACHE)
        {
            if (pdbc->fStatus & DBC_CACHE &&
                fType == pdbc->fCacheType &&
                STcompare (szOwner, pdbc->szCacheOwner) == 0 &&
                STcompare (szTable, pdbc->szCacheTable) == 0)
            {
                rc = CacheOpen (pdbc, pstmt);
                UnlockStmt (pstmt);
                return rc;
           }
            else
            {
                if (CacheInit (pdbc, pstmt))
                {
                    pdbc->fCacheType = fType;
                    STcopy (szOwner, pdbc->szCacheOwner);
                    STcopy (szTable, pdbc->szCacheTable);
                }
            }
        }
#endif  /* CATALOG_CACHE_ENABLED */

        /*
        **  The queries that we build contain predicates based on
        **  owner = or <> 'SYSTEM'.  If the caller has specified an
        **  owner parameter equivalent with no search arguments, we
        **  will try to add a predicate based on owner also.  There
        **  is code in IDMS R12.0 that returns an error when these
        **  two predicates cancel each other out (it would seem better
        **  to just return no rows).  This code avoids that.
        */
        if (parmOwner == PARM_EQUAL)
            if (STcompare (szOwner, "SYSTEM") == 0)
            {
                fType &= ~TYPE_USER;
                parmOwner = PARM_NULL;
            }
            else
                fType &= ~TYPE_SYSTEM;

        /*
        **  Get ready:
        **
        **  1. Establish separate catalog connection.
        **  2. Allocate syntax buffer (SQLExecDirect will free it).
        */
        if (!CatConnect (pstmt))
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

        szSqlStr = CatAlloc (pstmt);
        if (!szSqlStr)
        {
            UnlockStmt (pstmt);
            return SQL_ERROR;
        }

        /*
        **  Build query using accessible view or catalog table:
        **
        **
        **  No valid type specified (but something in type parm),
        **  force a query that returns no rows.
        */
        if (!fType)
        {
            STcat (szSqlStr, CAT_TABLES_TABLE);
            STcat (szSqlStr, "table_type <> table_type AND table_name = 'ii+odbc'");
            break;
        }

        /*
        **  System tables specified:
        **
        **  1. Load query for system tables.
        **  2. Add owner predicate if specified.
        **  3. Add table predicate if specified.
        **  4. Strip last AND from query.   .
        */
        if (fType & TYPE_SYSTEM)
        {
            STcat     (szSqlStr, CAT_TABLES_SYSTEM);
            CatWhere  (szSqlStr, parmOwner, pstringid->fOwnerName, szOwner, bEscape);
            CatWhere  (szSqlStr, parmTable, pstringid->fTableName, szTable, bEscape);
            CatRemove (szSqlStr, AND);
        }
        /*
        **  User tables specified:
        **
        **  1. Add union if needed.
        **  2. Load query for user defined tables.
        **  3. Add owner predicate if specified.
        **  4. Add table predicate if specified.
        **  5. Strip last AND from query.   .
        **  6. Add similar query for network defined records if IDMS.
        */
        if (fType & TYPE_TABLE)
        {   char *  szStartStr;

            if (*szSqlStr) 
                STcat (szSqlStr, (char *) UNION);

            szStartStr = szSqlStr + STlength(szSqlStr);
                /* remember start point of string */

            if (pdbc->fOptions & OPT_USESYSTABLES)
                STcat (szSqlStr, CAT_TABLES_USESYSTABLE); 
            else
                STcat (szSqlStr, CAT_TABLES_TABLE);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)  /* if NULL TABLE_SCHEM */
               MEcopy("SELECT DISTINCT ' ',' ',", 24, szStartStr);
                    /* massage the string to DISTINCT tables with no owners */

            CatWhere  (szSqlStr, parmOwner, pstringid->fOwnerName, szOwner, bEscape);
            CatWhere  (szSqlStr, parmTable, pstringid->fTableName, szTable, bEscape);
            CatRemove (szSqlStr, AND);
        }
        
        /*
        **  Views specified:
        **
        **  1. Add union if needed.
        **  2. Load query for views.
        **  3. Add owner predicate if specified.
        **  4. Add table predicate if specified.
        **  5. Strip last AND from query.   .
        */
        if (fType & TYPE_VIEW)
        {   char *  szStartStr;

            if (*szSqlStr) 
                STcat (szSqlStr, (char*)UNION);

            szStartStr = szSqlStr + STlength(szSqlStr);
                /* remember start point of string */

            if (pdbc->fOptions & OPT_USESYSTABLES)
                STcat(szSqlStr, CAT_USE_SYSTABLES_VIEW);
            else
                STcat(szSqlStr, CAT_TABLES_VIEW);

            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)  /* if NULL TABLE_SCHEM */
               MEcopy("SELECT DISTINCT ' ',' ',", 24, szStartStr);
                    /* massage the string to DISTINCT tables with no owners */

            CatWhere  (szSqlStr, parmOwner, pstringid->fOwnerName, szOwner, bEscape);
            CatWhere  (szSqlStr, parmTable, pstringid->fTableName, szTable, bEscape);
            CatRemove (szSqlStr, AND);
        }
 
        /*
        **  Add sort clause:
        */
        STcat (szSqlStr, CAT_TABLES_ORDER_BY);
    } /* end switch (pstmt->fCatalog)  */

    /*
    **  Execute the query on the catalog session:
    */
    rc = CatExecDirect (pstmt, szSqlStr, pstmt->fCatalog);

#ifdef CATALOG_CACHE_ENABLED
    /*
    **  If we are saving this result set, save the SQLDA too:
    */
    if (pstmt->fStatus & STMT_CACHE_PUT && rc == SQL_SUCCESS)
        rc = CacheSqlda (pdbc, pstmt);
#endif  /* CATALOG_CACHE_ENABLED */

/*  MEfree (szSqlStr); szSqlStr will be freed by ResetStmt,FreeSqlStr */
    UnlockStmt (pstmt);
    return rc;
}

/*
** Name: 	SQLTablePrivileges - Return table columns and privileges.
** 
** Description: 
**              SQLTablePrivileges returns a list of tables and the 
**              privileges associated with each table. 
** 
** Inputs:
**              hstmt                   StatementHandle.
**              szCatalogName           Catalog name.
**              cbCatalogName           Length of szCatalogName. 
**              szSchemaName            Schema name.
**              cbSchemaName            Length of szSchemaName. 
**              szTableName             Table name. 
**              cbTableName             Length of TableName. 
**
** Outputs: 
**              None.
**
** Returns: 
**              SQL_SUCCESS
**              SQL_SUCCESS_WITH_INFO
**              SQL_ERROR
**              SQL_INVALID_HANDLE
**
** Exceptions: 
**       
**              N/A 
** 
** Side Effects: 
** 
**              Generates a result set.
**
** History: 
**    12-jul-06 (Ralph Loen)
**      Created.
**    30-aug-07 (Ralph Loen) Bug 119045
**      Return SQL_SUCCESS rather than SQL_NO_DATA if an empty result set
**      is returned from the catalog query.
*/ 

RETCODE SQL_API SQLTablePrivileges(
    SQLHSTMT         hstmt,
    SQLCHAR        *szCatalogName,
    SQLSMALLINT      cbCatalogName,
    SQLCHAR        *szSchemaName,
    SQLSMALLINT      cbSchemaName,
    SQLCHAR        *szTableName,
    SQLSMALLINT      cbTableName)
{
    return SQLTablePrivileges_InternalCall(
        hstmt,
        szCatalogName,
        cbCatalogName,
        szSchemaName,
        cbSchemaName,
        szTableName,
        cbTableName);
}

RETCODE SQL_API SQLTablePrivileges_InternalCall(
    SQLHSTMT         hstmt,
    SQLCHAR        *szCatalogName,
    SQLSMALLINT      cbCatalogName,
    SQLCHAR        *szSchemaName,
    SQLSMALLINT      cbSchemaName,
    SQLCHAR        *szTableName,
    SQLSMALLINT      cbTableName)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    LPDBC   pdbc;
    CHAR    *szSqlStr    = NULL;
    PARM    parmOwner;
    PARM    parmTable;
    CHAR    szTable[65];
    CHAR    szSchema[128];
    RETCODE rc;
    IIAPI_QUERYPARM     queryParm={0};
    IIAPI_GETDESCRPARM  getDescrParm={0};
    IIAPI_GETCOLPARM    getColParm={0};
    IIAPI_GETCOLPARM    *pgetColParm=NULL;
    UWORD               *pmaxlength=NULL;
    CHAR                *catalog = " ";
    CHAR                *ownerName;   
    CHAR                *tableName;
    CHAR                *grantor;
    CHAR                *grantee;
    CHAR                *privilege;
    CHAR                *grant_text, *grant_textVCH;
    LPDESC      pIRD;           /* -> Implementation Row Descriptor Header */
    LPDESC      pARD;           /* -> Application Row Descriptor Header */
    LPDESCFLD   pird;           /* -> Implementation Row Descriptor column */
    QUEUE       *q;
    i2          rowCount=0;
    CHAR        *p;
    i4 i;
    i2 columnCount = sizeof(tablePrivInfo) / sizeof(tablePrivInfo[0]);
    TABLEPRIV_ROW *tabpriv_row;

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLTABLEPRIVILEGES, pstmt,
            szCatalogName, cbCatalogName, szSchemaName, cbSchemaName,
			szTableName, cbTableName);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    /*
    ** Only "thick" gateways have an IIPERMITS catalog.
    */
    if (!isServerClassAnIngresEngine(pstmt->pdbcOwner))
        return ErrUnlockStmt (SQL_HYC00, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;

    if (!CatConnect (pstmt))
    {
        UnlockStmt (pstmt);
        return SQL_ERROR;
    }

    QUinit(&pstmt->baseTabpriv_q);

    pstmt->fCommand = CMD_SELECT;      /* avoid parsing command */

    /*
    **  Build SQL query syntax:
    **
    **  - Allocate buffer (SQLExecDirect frees it).
    **  - Load user defined resource containing catalog specific query.
    **  - Add owner and name predicates (required).
    */
    szSqlStr = CatAlloc (pstmt);
    if (szSqlStr == NULL)
    {
        UnlockStmt (pstmt);
        return (SQL_ERROR);
    }

    if (isServerClassINGRES(pdbc) || isServerClassRMS(pdbc))
        STcat(szSqlStr, CAT_TABLEPRIV);
    else
        STcat(szSqlStr, CAT_TABLEPRIV_GW);

    parmOwner  = CatParm (pdbc, szSchema,  32, szSchemaName, cbSchemaName, 1);
    parmTable  = CatParm (pdbc, szTable,  32, szTableName, cbTableName, 1);

    if (parmOwner  == PARM_INVALID ||
        parmTable == PARM_INVALID)
        return ErrUnlockStmt (SQL_HY090, pstmt); /* invalid string or buffer length */

    if (szSchemaName)
        CatWhere  (szSqlStr, parmOwner, "object_owner", szSchema, 0);
    if (szTableName)
        CatWhere  (szSqlStr, parmTable, "object_name", szTable, 0);
    CatRemove(szSqlStr, AND);
    STcat(szSqlStr, " ORDER BY 2, 3, 5");

    /*
    ** Since the result set returned from the select on IIPERMITS
    ** is inadequate for returning directly to the application, 
    ** the result set must be converted to an internally-generated
    ** result set.  The "text_segment" column is of particular interest
    ** because it has information on the privilege granted and whether or
    ** not the grant option is specified.
    **
    ** The core algorithm is as follows:
    **     1.  Execute the select against IIPERMITS and fetch the
    **         data.
    **     2.  Create a linked list entry for each column found, and
    **         populate the list with the other columns from IIPERMITS.
    **     3.  Internally create a standard catalog result set.
    **     4.  Populate the result set with information derived from the
    **         link list.
    */    
    if (CatQueryOpen(pstmt, &queryParm, szSqlStr))
        if (CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))
            rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, 
                pgetColParm=&getColParm);
    else
        rc = 0;
    
    while (rc && getColParm.gc_genParm.gp_status != IIAPI_ST_NO_DATA)
    {
        CHAR *pSave;
        CHAR *p1;
        i2 len;

        grant_text=(CHAR*)getColParm.gc_columnData[4].dv_value + sizeof(i2);
        ownerName=(CHAR*)getColParm.gc_columnData[0].dv_value;
        STtrmwhite(ownerName);
        tableName=(CHAR*)getColParm.gc_columnData[1].dv_value;
        STtrmwhite(tableName);
        
        /*
        ** if the table owner is the same as the grantor, the value of
        ** grantor is "_SYSTEM", according to the specs.  This is always
        ** the case for VSAM and IMS.  Note that RMS is enough of an 
        ** Ingres engine to get the same treatment as Ingres.
        */
        if (isServerClassINGRES(pdbc) || isServerClassRMS(pdbc))
        {
            grantor = (CHAR*)getColParm.gc_columnData[2].dv_value;
            if (!STbcompare(ownerName,STlength(ownerName),grantor, 
                STlength(ownerName), TRUE)) 
                grantor = "_SYSTEM";
        }
        else
            /*
            ** VSAM and RMS already return "_SYSTEM", because WITH GRANT
            ** OPTION isn't done there.
            */
            grantor = (CHAR*)getColParm.gc_columnData[2].dv_value + sizeof(i2);

        STtrmwhite(grantor);
        
        grantee=(CHAR*)getColParm.gc_columnData[3].dv_value;
        STtrmwhite(grantee);
        grant_textVCH=(CHAR*)getColParm.gc_columnData[4].dv_value;

        p = STindex(grant_text, " ", 0);
        while (CMwhite(p)) CMnext(p);
        pSave = p1 = STindex(p, " ", 0);
        len = p1 - p;
        if (len <= 0)
            continue;
        privilege = MEreqmem(0, len + sizeof(char) + sizeof(i2), TRUE, 
            NULL);
        MEcopy(p, len, privilege + sizeof(i2));
        *(i2*)privilege = (i2)len;

        rowCount++;
        tabpriv_row = (TABLEPRIV_ROW *) MEreqmem( (u_i2) 0,
            (u_i4)sizeof(TABLEPRIV_ROW), TRUE, (STATUS *) NULL);

        tabpriv_row->catalog = MEreqmem(0, STlength(catalog) + 
            sizeof(i2) + sizeof(char), TRUE, NULL);
        MEcopy(catalog, STlength(catalog), tabpriv_row->catalog + 
            sizeof(i2));
        *(i2 *)tabpriv_row->catalog = STlength(catalog);
                    
        tabpriv_row->ownerName = MEreqmem(0, STlength(ownerName) + 
            sizeof(i2) + sizeof(char), TRUE, NULL);
        MEcopy(ownerName, STlength(ownerName), tabpriv_row->ownerName + 
            sizeof(i2));
        *(i2 *)tabpriv_row->ownerName = STlength(ownerName);
        
        tabpriv_row->tableName = MEreqmem(0, STlength(tableName) + 
            sizeof(i2) + sizeof(char), TRUE, NULL);
        MEcopy(tableName, STlength(tableName), tabpriv_row->tableName + 
            sizeof(i2));
        *(i2 *)tabpriv_row->tableName = STlength(tableName);
        
        tabpriv_row->grantor = MEreqmem(0, STlength(grantor) + 
            sizeof(i2) + sizeof(char), TRUE, NULL);
        MEcopy(grantor, STlength(grantor), tabpriv_row->grantor + 
            sizeof(i2));
        *(i2 *)tabpriv_row->grantor = STlength(grantor);
        
        tabpriv_row->grantee = MEreqmem(0, STlength(grantee) + 
            sizeof(i2) + sizeof(char), TRUE, NULL);
        MEcopy(grantee, STlength(grantee), tabpriv_row->grantee + 
            sizeof(i2));
        *(i2 *)tabpriv_row->grantee = STlength(grantee);
        
        tabpriv_row->privilege = privilege;
        if (STstrindex(grant_text,"with grant option",0,FALSE))
        {
            tabpriv_row->is_grantable = MEreqmem(0, STlength("YES") + 
                sizeof(i2) + sizeof(char), TRUE, NULL);
            MEcopy("YES", STlength("YES"), tabpriv_row->is_grantable + 
                sizeof(i2));
            *(i2 *)tabpriv_row->is_grantable = STlength("YES");
        }
        else
        {
            tabpriv_row->is_grantable = MEreqmem(0, STlength("NO") + 
                sizeof(i2) + sizeof(char), TRUE, NULL);
            MEcopy("NO", STlength("NO"), tabpriv_row->is_grantable + 
                sizeof(i2));
            *(i2 *)tabpriv_row->is_grantable = STlength("NO");
        }
        
        QUinsert((QUEUE *)tabpriv_row, &pstmt->baseTabpriv_q);

        rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, pgetColParm);
    } /* endif row found! */

    /*
    ** Dispose of resources generated by the select statement.
    */
    CatQueryClose(pstmt, &queryParm, &getDescrParm, pgetColParm);

    /*
    **  Enumerate table types:
    **
    **  1. "Prepare" the constant query
    **     Load Implementation Row Descriptor (IRD).
    **  2. Pretend to "execute" the constant query
    **  3. Load Fetch buffer.
    **  4. Set cursor as if we opened it and to end of fetch,
    **     as if we already fetched a buffer.
    */
    
    pIRD = pstmt->pIRD;               /* pIRD->Implementation Row Descriptor */
    pARD = pstmt->pARD;               /* pARD->Application Row Descriptor */
    ResetDescriptorFields(pIRD, 0, pIRD->Count);  /* clear all desc fields */
    /*
    **  Ensure that IRD and ARD row descriptors are large enough.
    **  If too few then re-allocate.
    */
    if (columnCount > pIRD->CountAllocated)
        if (ReallocDescriptor(pIRD, rowCount) == NULL)
        {
            rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
            UnlockStmt (pstmt);
            return rc;
        }

    if (columnCount > pARD->CountAllocated)
        if (ReallocDescriptor(pARD, columnCount) == NULL)
        {
            rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
            UnlockStmt (pstmt);
            return rc;
        }

    pIRD->Count = columnCount;
    pird = pIRD->pcol + 1;    /* pird -> 1st IRD field (after bookmark) */

    /* 
    ** Set precision, scale, and other defaults
    */
    for (i=0; i < columnCount; i++, pird++) 
    {  
        STcopy(tablePrivInfo[i].Name, pird->Name);
        pird->fIngApiType = tablePrivInfo[i].fIngApiType;
        pird->OctetLength = tablePrivInfo[i].OctetLength;
        pird->Nullable    = tablePrivInfo[i].Nullable;
        SetDescDefaultsFromType(pdbc, pird);
    }

    rc = PrepareConstant (pstmt);
    if (rc != SQL_SUCCESS)
    {
        UnlockStmt (pstmt);
        return rc;
    }

    pstmt->prowCurrent = NULL;
    pstmt->crowBuffer  = rowCount;

    pstmt->prowCurrent = 
      pstmt->pFetchBuffer = 
         MEreqmem(0, pstmt->cbrow * (pstmt->crowBuffer+1), TRUE, NULL);  

    if (!pstmt->prowCurrent)
    {
        rc = ErrState (SQL_HY000, pdbc, F_OD0063_IDS_ERR_RESOURCE, 
                             IDU_TYPES, RT_CAT_TABLES_TYPES);
        UnlockStmt (pstmt);
        return rc;
    }

    pstmt->fStatus |= STMT_OPEN + STMT_EOF;

    /*
    ** All of the columns in the generated result set are varchars, so
    ** prefix the two-byte length to the string value.  Null pointers
    ** remain at zero because the columns are never null.
    */
    p = pstmt->prowCurrent;

    pstmt->tabpriv_count = rowCount;
    for (i = 0, q = pstmt->baseTabpriv_q.q_prev; 
        i < pstmt->tabpriv_count; i++, q = q->q_prev )
    {
        char * dp;   /* data pointer in tuple for the column */
        i4   * np;   /* null indicator pointer in tuple for the column */
        i2   * i2p;  /* varchar length pointer in tuple for the column */
         
        tabpriv_row = (TABLEPRIV_ROW *)q;
        p += pstmt->cbrow;       /* p->tuple to fill in */
        
        GetTuplePtrs(pstmt, p, QRY_TABLEP_TABLE_CAT, &dp, &np);
        i2p = (i2*) tabpriv_row->catalog;  
        MEcopy(tabpriv_row->catalog, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_TABLEP_TABLE_SCHEM, &dp, &np);
        i2p = (i2*) tabpriv_row->ownerName;    /* i2p->varchar length */
        MEcopy(tabpriv_row->ownerName, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_TABLEP_TABLE_NAME, &dp, &np);
        i2p = (i2*) tabpriv_row->tableName;   
        MEcopy(tabpriv_row->tableName, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_TABLEP_GRANTOR, &dp, &np);
        i2p = (i2*) tabpriv_row->grantor;   
        MEcopy(tabpriv_row->grantor, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_TABLEP_GRANTEE, &dp, &np);
        i2p = (i2*) tabpriv_row->grantee;  
        MEcopy(tabpriv_row->grantee, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_TABLEP_PRIVILEGE, &dp, &np);
        i2p = (i2*) tabpriv_row->privilege; 
        MEcopy(tabpriv_row->privilege, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_TABLEP_IS_GRANTABLE, &dp, &np);
        i2p = (i2*) tabpriv_row->is_grantable;
        MEcopy(tabpriv_row->is_grantable, *i2p + sizeof(i2) + sizeof(char), dp);
    }  /* end for loop */
    
    UnlockStmt (pstmt);

    return SQL_SUCCESS;
}

/*
** Name:        SQLColumnPrivileges - Return table columns and privileges.
**
** Description:
**              SQLColumnPrivileges returns a list of columns and
**              associated privileges for the specified table.
** Inputs:
**              hstmt                   StatementHandle.
**              szCatalogName           Catalog name.
**              cbCatalogName           Length of szCatalogName.
**              szSchemaName            Schema name.
**              cbSchemaName            Length of szSchemaName.
**              szTableName             Table name.
**              cbTableName             Length of TableName.
**              szColumnName            String search pattern for column names.
**              cbColumnName            Length of szColumnName.
**
** Outputs:
**              None.
**
** Returns:
**              SQL_SUCCESS
**              SQL_SUCCESS_WITH_INFO
**              SQL_ERROR
**              SQL_INVALID_HANDLE
**
** Exceptions:
**
**              N/A
**
** Side Effects:
**
**              Generates a result set.
**
** History:
**    21-jul-06 (Ralph Loen)  SIR 116410
**      Created.
**    30-aug-07 (Ralph Loen) Bug 119045
**      Return SQL_SUCCESS rather than SQL_NO_DATA if an empty result set
**      is returned from the catalog query.
*/

RETCODE SQL_API SQLColumnPrivileges(
    SQLHSTMT           hstmt,
    SQLCHAR            *szCatalogName,
    SQLSMALLINT        cbCatalogName,
    SQLCHAR            *szSchemaName,
    SQLSMALLINT        cbSchemaName,
    SQLCHAR            *szTableName,
    SQLSMALLINT        cbTableName,
    SQLCHAR            *szColumnName,
    SQLSMALLINT        cbColumnName)
{
    return SQLColumnPrivileges_InternalCall(
        hstmt,
        szCatalogName,
        cbCatalogName,
        szSchemaName,
        cbSchemaName,
        szTableName,
        cbTableName,
        szColumnName,
        cbColumnName);
}

RETCODE SQL_API SQLColumnPrivileges_InternalCall(
    SQLHSTMT         hstmt,
    SQLCHAR        *szCatalogName,
    SQLSMALLINT      cbCatalogName,
    SQLCHAR        *szSchemaName,
    SQLSMALLINT      cbSchemaName,
    SQLCHAR        *szTableName,
    SQLSMALLINT      cbTableName,
    SQLCHAR        *szColumnName,
    SQLSMALLINT      cbColumnName)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    LPDBC   pdbc;
    CHAR    *szSqlStr    = NULL;
    PARM    parmOwner;
    PARM    parmTable;
    PARM    parmColumn;
    CHAR    szTable[65];
    CHAR    szSchema[128];
    CHAR    szColumn[128];
    RETCODE rc;
    IIAPI_QUERYPARM     queryParm={0};
    IIAPI_GETDESCRPARM  getDescrParm={0};
    IIAPI_GETCOLPARM    getColParm={0};
    IIAPI_GETCOLPARM    *pgetColParm=NULL;
    UWORD               *pmaxlength=NULL;
    CHAR                *catalog = " ";
    CHAR                *ownerName;   
    CHAR                *tableName;
    CHAR                *grantor;
    CHAR                *grantee;
    CHAR                *privilege;
    CHAR                *nextColPrivilege;
    CHAR                *columnName;
    CHAR                *nextColName;
    CHAR                *grant_text, *grant_textVCH;
    LPDESC      pIRD;           /* -> Implementation Row Descriptor Header */
    LPDESC      pARD;           /* -> Application Row Descriptor Header */
    LPDESCFLD   pird;           /* -> Implementation Row Descriptor column */
    QUEUE       *q;
    i2          rowCount=0;
    CHAR        *p;
    i4 i;
    i2 columnCount = sizeof(columnPrivInfo) / sizeof(columnPrivInfo[0]);
    BOOL colPresent = FALSE;    
    COLPRIV_ROW *colpriv_row;

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLCOLUMNPRIVILEGES, pstmt,
            szCatalogName, cbCatalogName, szSchemaName, cbSchemaName,
            szTableName, cbTableName, szColumnName, cbColumnName);

    QUinit(&pstmt->baseColpriv_q);

    if (pstmt->fStatus & STMT_OPEN)
        return ErrUnlockStmt (SQL_24000, pstmt);

    /*
    ** Only Ingres (thick) gateways support column privileges.
    */
    if (!isServerClassAnIngresEngine(pstmt->pdbcOwner))
        return ErrUnlockStmt (SQL_HYC00, pstmt);

    /*
    **  Table name argument cannot be NULL.
    */
    if (szTableName == NULL || cbTableName == 0)
        return ErrUnlockStmt (SQL_HY009, pstmt); 

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    pdbc = pstmt->pdbcOwner;

    if (!CatConnect (pstmt))
    {
        UnlockStmt (pstmt);
        return SQL_ERROR;
    }

    pstmt->fCommand = CMD_SELECT;      /* avoid parsing command */

    /*
    **  Build SQL query syntax:
    **
    **  - Allocate buffer (SQLExecDirect frees it).
    **  - Load user defined resource containing catalog specific query.
    **  - Add owner and name predicates (required).
    */
    szSqlStr = CatAlloc (pstmt);
    if (szSqlStr == NULL)
    {
        UnlockStmt (pstmt);
        return (SQL_ERROR);
    }

    if (isServerClassINGRES(pdbc) || isServerClassRMS(pdbc))
        STcat(szSqlStr, CAT_COLUMNPRIV);
    else
        STcat(szSqlStr, CAT_COLUMNPRIV_GW); 

    parmOwner  = CatParm (pdbc, szSchema,  32, szSchemaName, cbSchemaName, 1);
    parmTable  = CatParm (pdbc, szTable,  32, szTableName, cbTableName, 1);
    parmColumn  = CatParm (pdbc, szColumn,  32, szColumnName, cbColumnName, 1);

    if (parmOwner  == PARM_INVALID ||
        parmTable == PARM_INVALID)
        return ErrUnlockStmt (SQL_HY090, pstmt); /* invalid string or buffer length */
    else
    {
        /*
        ** Schema and table name arguments cannot have search patterns.
        */
        parmOwner = PARM_EQUAL;
        parmTable = PARM_EQUAL;
    }

    if (szSchemaName)
        CatWhere  (szSqlStr, parmOwner, "object_owner", szSchema, 0);
    if (szTableName)
        CatWhere  (szSqlStr, parmTable, "object_name", szTable, 0);
    CatRemove(szSqlStr, AND);
    STcat(szSqlStr, " ORDER BY 2, 3");

    /*
    ** Since the result set returned from the select on IIPERMITS
    ** is inadequate for returning directly to the application, 
    ** the result set must be converted to an internally-generated
    ** result set.  The "text_segment" column is of particular interest
    ** because it has information on the privilege granted, whether or
    ** not the grant option is specified, and the target columns.
    ** The core algorithm is as follows:
    **     1.  Execute the select against IIPERMITS and fetch the
    **         data.
    **     2.  Extract the column name(s) from the text_segment column.
    **     3.  Create a linked list entry for each column found, and
    **         populate the list with the other columns from IIPERMITS.
    **     4.  Internally create a standard catalog result set.
    **     5.  Populate the result set with information derived from the
    **         link list.
    */    
    if (CatQueryOpen(pstmt, &queryParm, szSqlStr))
        if (CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))
            rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, 
                pgetColParm=&getColParm);
    else
        rc = 0;
    
    while (rc && getColParm.gc_genParm.gp_status != IIAPI_ST_NO_DATA)
    {
        CHAR *pSave;
        CHAR *p1, *p2, *p3;
        i2 len, privLen;

        grant_text=(CHAR*)getColParm.gc_columnData[4].dv_value + sizeof(i2);
        if (STindex (grant_text, "(", 0 ))
        {
            ownerName=(CHAR*)getColParm.gc_columnData[0].dv_value;
            STtrmwhite(ownerName);
            tableName=(CHAR*)getColParm.gc_columnData[1].dv_value;
            STtrmwhite(tableName);
        
            /*
            ** if the table owner is the same as the grantor, the value of
            ** grantor is "_SYSTEM", according to the specs.  This is always
            ** the case for VSAM and IMS.  Note that RMS is enough of an 
            ** Ingres engine to get the same treatment as Ingres.
            */
            if (isServerClassINGRES(pdbc) || isServerClassRMS(pdbc))
            {
                grantor = (CHAR*)getColParm.gc_columnData[2].dv_value;
                if (!STbcompare(ownerName,STlength(ownerName),grantor, 
                    STlength(ownerName), TRUE)) 
                    grantor = "_SYSTEM";
            }
            else
                /*
                ** VSAM and RMS already return "_SYSTEM", because WITH GRANT
                ** OPTION isn't done there.
                */
                grantor = (CHAR*)getColParm.gc_columnData[2].dv_value + 
                    sizeof(i2);
    
            STtrmwhite(grantor);
            
            grantee=(CHAR*)getColParm.gc_columnData[3].dv_value;
            STtrmwhite(grantee);
            grant_textVCH=(CHAR*)getColParm.gc_columnData[4].dv_value;

            p = STindex(grant_text, " ", 0);
            while (CMwhite(p)) CMnext(p);
            pSave = p1 = STindex(p, " ", 0);
            privLen = p1 - p;
            if (privLen <= 0)
                continue;
            privilege = MEreqmem(0, privLen + sizeof(char) + sizeof(i2), TRUE, 
                NULL);
            MEcopy(p, privLen, privilege + sizeof(i2));
            *(i2*)privilege = (i2)privLen;
            while (CMwhite(p)) CMnext(p);
            p = STindex(pSave, "(", 0);
            CMnext(p);

            /*
            ** If a comma is present in the query text, multiple columns are
            ** referenced in the GRANT.  First extract and analyze all 
            ** columns referenced that are followed by commas.
            */
            p1 = STindex(p, ",", 0);
            while(p1)
            {
                colPresent = FALSE;

                if (parmColumn == PARM_NULL)
                    colPresent = TRUE;
 
                len = p1 - p;
                nextColName = MEreqmem(0, len + 
                    sizeof(char) + sizeof(i2), TRUE, NULL);
                MEcopy(p, len, nextColName + sizeof(i2));
                *(i2 *)nextColName = (i2)len;
                p2 = nextColName  + 2;
                if (!cbColumnName || !szColumnName)
                    colPresent = TRUE;
                else if (!STbcompare(p2, 0, szColumn, 0, FALSE))    
                    colPresent = TRUE;
                else
                {
                    p3 = STindex(szColumn, PERCENT, cbColumnName);
                    if (!p3 && !(pstmt->pdbcOwner->fOptions & 
                        OPT_DISABLEUNDERSCORE))
                        p3 = STindex(szColumn, UNDERSCORE, cbColumnName);
                    if (p3 > szColumn)
                    {
                        len = STlength(szColumn) - STlength(p3);
                        if (!STbcompare(p2, len, szColumn, len, FALSE))
                            colPresent = TRUE;
                    }
                    else if (p3)
                    {
                        CMnext(p3);
                        if (STstrindex(p2, p3, 0, FALSE))  
                            colPresent = TRUE;
                    }
                }
                if (!colPresent)
                {
                    MEfree((PTR)nextColName);
                    CMnext(p1);
                    p = p1;
                    while(CMwhite(p)) CMnext(p);
                    p1 = STindex(p1, ",", 0);
                    continue;
                }
                rowCount++;
                colpriv_row = (COLPRIV_ROW *) MEreqmem( (u_i2) 0,
                    (u_i4)sizeof(COLPRIV_ROW), TRUE, (STATUS *) NULL);

                colpriv_row->columnName = nextColName;

                colpriv_row->catalog = MEreqmem(0, STlength(catalog) + 
                    sizeof(i2) + sizeof(char), TRUE, NULL);
                MEcopy(catalog, STlength(catalog), colpriv_row->catalog + 
                    sizeof(i2));
                *(i2 *)colpriv_row->catalog = STlength(catalog);
                
                colpriv_row->ownerName = MEreqmem(0, STlength(ownerName) + 
                    sizeof(i2) + sizeof(char), TRUE, NULL);
                MEcopy(ownerName, STlength(ownerName), colpriv_row->ownerName + 
                    sizeof(i2));
                *(i2 *)colpriv_row->ownerName = STlength(ownerName);
                
                colpriv_row->tableName = MEreqmem(0, STlength(tableName) + 
                    sizeof(i2) + sizeof(char), TRUE, NULL);
                MEcopy(tableName, STlength(tableName), colpriv_row->tableName + 
                    sizeof(i2));
                *(i2 *)colpriv_row->tableName = STlength(tableName);
                
                colpriv_row->grantor = MEreqmem(0, STlength(grantor) + 
                    sizeof(i2) + sizeof(char), TRUE, NULL);
                MEcopy(grantor, STlength(grantor), colpriv_row->grantor + 
                    sizeof(i2));
                *(i2 *)colpriv_row->grantor = STlength(grantor);
                
                colpriv_row->grantee = MEreqmem(0, STlength(grantee) + 
                    sizeof(i2) + sizeof(char), TRUE, NULL);
                MEcopy(grantee, STlength(grantee), colpriv_row->grantee + 
                    sizeof(i2));
                *(i2 *)colpriv_row->grantee = STlength(grantee);
                
                nextColPrivilege = MEreqmem(0, privLen + sizeof(char) 
                    + sizeof(i2), TRUE, NULL);
                MEcopy(privilege, privLen + sizeof(char) + sizeof(i2), 
                    nextColPrivilege);
                colpriv_row->privilege = nextColPrivilege;

                if (STstrindex(grant_text,"with grant option",0,FALSE))
                {
                    colpriv_row->is_grantable = MEreqmem(0, STlength("YES") + 
                        sizeof(i2) + sizeof(char), TRUE, NULL);
                    MEcopy("YES", STlength("YES"), colpriv_row->is_grantable + 
                        sizeof(i2));
                    *(i2 *)colpriv_row->is_grantable = STlength("YES");
                }
                else
                {
                    colpriv_row->is_grantable = MEreqmem(0, STlength("NO") + 
                        sizeof(i2) + sizeof(char), TRUE, NULL);
                    MEcopy("NO", STlength("NO"), colpriv_row->is_grantable + 
                        sizeof(i2));
                    *(i2 *)colpriv_row->is_grantable = STlength("NO");

                }

                QUinsert((QUEUE *)colpriv_row, &pstmt->baseColpriv_q);
                CMnext(p1);
                p = p1;
                while(CMwhite(p)) CMnext(p);
                p1 = STindex(p1, ",", 0);
            } /* while (p1) */

            /*
            ** Fall-though for single column references and the last 
            ** column reference.
            */
	    while(CMwhite(pSave)) CMnext(pSave);

            p1 = STindex(pSave, ")", 0);
            p2 = STindex(p, " ", 0);
			if (p2 < p1)
				p1 = p2; 
            len = p1 - p;
            columnName = MEreqmem(0, len +
                sizeof(char) + sizeof(i2), TRUE, NULL);
            MEcopy(p, len, columnName + sizeof(i2));
            *(i2 *)columnName = (i2)len;
            p2 = columnName  + 2;
    
            if (parmColumn == PARM_NULL)
                colPresent = TRUE;
            else
                colPresent = FALSE;
            
            if (!cbColumnName || !szColumn)
                colPresent = TRUE;
            else if (!STbcompare(p2, 0, szColumn, 0, FALSE))    
                colPresent = TRUE;
            else
            {
                p3 = STindex(szColumn, PERCENT, cbColumnName);
                if (!p3 && !(pstmt->pdbcOwner->fOptions & 
                    OPT_DISABLEUNDERSCORE))
                    p3 = STindex(szColumn, UNDERSCORE, cbColumnName);
                if (p3 > szColumn)
                {
                    len = STlength(szColumn) - STlength(p3);
                    if (!STbcompare(p2, len, szColumn, len, FALSE))
                        colPresent = TRUE;
                }
                else if (p3)
                {
                    CMnext(p3);
                    if (STstrindex(p2, p3, 0, FALSE))  
                        colPresent = TRUE;
                }
            }
            if (!colPresent)
            {
                MEfree((PTR)columnName);
                rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, 
                    pgetColParm);
                continue;
            }

            rowCount++;
            colpriv_row = (COLPRIV_ROW *) MEreqmem( (u_i2) 0,
                (u_i4)sizeof(COLPRIV_ROW), TRUE, (STATUS *) NULL);

            colpriv_row->columnName = columnName;
            
            colpriv_row->catalog = MEreqmem(0, STlength(catalog) + 
                sizeof(i2) + sizeof(char), TRUE, NULL);
            MEcopy(catalog, STlength(catalog), colpriv_row->catalog + 
                sizeof(i2));
            *(i2 *)colpriv_row->catalog = STlength(catalog);
                        
            colpriv_row->ownerName = MEreqmem(0, STlength(ownerName) + 
                sizeof(i2) + sizeof(char), TRUE, NULL);
            MEcopy(ownerName, STlength(ownerName), colpriv_row->ownerName + 
                sizeof(i2));
            *(i2 *)colpriv_row->ownerName = STlength(ownerName);
            
            colpriv_row->tableName = MEreqmem(0, STlength(tableName) + 
                sizeof(i2) + sizeof(char), TRUE, NULL);
            MEcopy(tableName, STlength(tableName), colpriv_row->tableName + 
                sizeof(i2));
            *(i2 *)colpriv_row->tableName = STlength(tableName);
            
            colpriv_row->grantor = MEreqmem(0, STlength(grantor) + 
                sizeof(i2) + sizeof(char), TRUE, NULL);
            MEcopy(grantor, STlength(grantor), colpriv_row->grantor + 
                sizeof(i2));
            *(i2 *)colpriv_row->grantor = STlength(grantor);
            
            colpriv_row->grantee = MEreqmem(0, STlength(grantee) + 
                sizeof(i2) + sizeof(char), TRUE, NULL);
            MEcopy(grantee, STlength(grantee), colpriv_row->grantee + 
                sizeof(i2));
            *(i2 *)colpriv_row->grantee = STlength(grantee);
            
            colpriv_row->privilege = privilege;
            if (STstrindex(grant_text,"with grant option",0,FALSE))
            {
                colpriv_row->is_grantable = MEreqmem(0, STlength("YES") + 
                    sizeof(i2) + sizeof(char), TRUE, NULL);
                MEcopy("YES", STlength("YES"), colpriv_row->is_grantable + 
                    sizeof(i2));
                *(i2 *)colpriv_row->is_grantable = STlength("YES");
            }
            else
            {
                colpriv_row->is_grantable = MEreqmem(0, STlength("NO") + 
                    sizeof(i2) + sizeof(char), TRUE, NULL);
                MEcopy("NO", STlength("NO"), colpriv_row->is_grantable + 
                    sizeof(i2));
                *(i2 *)colpriv_row->is_grantable = STlength("NO");
            }
        
            QUinsert((QUEUE *)colpriv_row, &pstmt->baseColpriv_q);
        } /* if (STindex (grant_text, "(", 0 )) */

        rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, pgetColParm);
    } /* endif row found! */

    /*
    ** Dispose of resources generated by the select statement.
    */
    CatQueryClose(pstmt, &queryParm, &getDescrParm, pgetColParm);

    /*
    **  Enumerate table types:
    **
    **  1. "Prepare" the constant query
    **     Load Implementation Row Descriptor (IRD).
    **  2. Pretend to "execute" the constant query
    **  3. Load Fetch buffer.
    **  4. Set cursor as if we opened it and to end of fetch,
    **     as if we already fetched a buffer.
    */
    
    pIRD = pstmt->pIRD;               /* pIRD->Implementation Row Descriptor */
    pARD = pstmt->pARD;               /* pARD->Application Row Descriptor */
    ResetDescriptorFields(pIRD, 0, pIRD->Count);  /* clear all desc fields */
    /*
    **  Ensure that IRD and ARD row descriptors are large enough.
    **  If too few then re-allocate.
    */
    if (columnCount > pIRD->CountAllocated)
        if (ReallocDescriptor(pIRD, rowCount) == NULL)
        {
            rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
            UnlockStmt (pstmt);
            return rc;
        }

    if (columnCount > pARD->CountAllocated)
        if (ReallocDescriptor(pARD, columnCount) == NULL)
        {
            rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
            UnlockStmt (pstmt);
            return rc;
        }

    pIRD->Count = columnCount;
    pird = pIRD->pcol + 1;    /* pird -> 1st IRD field (after bookmark) */

    /* 
    ** Set precision, scale, and other defaults
    */
    for (i=0; i < columnCount; i++, pird++) 
    {  
        STcopy(columnPrivInfo[i].Name, pird->Name);
        pird->fIngApiType = columnPrivInfo[i].fIngApiType;
        pird->OctetLength = columnPrivInfo[i].OctetLength;
        pird->Nullable    = columnPrivInfo[i].Nullable;
        SetDescDefaultsFromType(pdbc, pird);
    }

    rc = PrepareConstant (pstmt);
    if (rc != SQL_SUCCESS)
    {
        UnlockStmt (pstmt);
        return rc;
    }

    pstmt->prowCurrent = NULL;
    pstmt->crowBuffer  = rowCount;

    pstmt->prowCurrent = 
      pstmt->pFetchBuffer = 
         MEreqmem(0, pstmt->cbrow * (pstmt->crowBuffer+1), TRUE, NULL);  

    if (!pstmt->prowCurrent)
    {
        rc = ErrState (SQL_HY000, pdbc, F_OD0063_IDS_ERR_RESOURCE, 
                             IDU_TYPES, RT_CAT_TABLES_TYPES);
        UnlockStmt (pstmt);
        return rc;
    }

    pstmt->fStatus |= STMT_OPEN + STMT_EOF;

    /*
    ** All of the columns in the generated result set are varchars, so
    ** prefix the two-byte length to the string value.  Null pointers
    ** remain at zero because the columns are never null.
    */
    p = pstmt->prowCurrent;

    pstmt->colpriv_count = rowCount;
    for (i = 0, q = pstmt->baseColpriv_q.q_prev; 
        i < pstmt->colpriv_count; i++, q = q->q_prev )
    {
        char * dp;   /* data pointer in tuple for the column */
        i4   * np;   /* null indicator pointer in tuple for the column */
        i2   * i2p;  /* varchar length pointer in tuple for the column */
         
        colpriv_row = (COLPRIV_ROW *)q;
        p += pstmt->cbrow;       /* p->tuple to fill in */
        GetTuplePtrs(pstmt, p, QRY_COLUMNP_TABLE_CAT, &dp, &np);
        i2p = (i2*) colpriv_row->catalog;  
        MEcopy(colpriv_row->catalog, *i2p + sizeof(i2) + sizeof(char), dp);
        
        GetTuplePtrs(pstmt, p, QRY_COLUMNP_TABLE_SCHEM, &dp, &np);
        i2p = (i2*) colpriv_row->ownerName;    /* i2p->varchar length */
        MEcopy(colpriv_row->ownerName, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_COLUMNP_TABLE_NAME, &dp, &np);
        i2p = (i2*) colpriv_row->tableName;   
        MEcopy(colpriv_row->tableName, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_COLUMNP_COLUMN_NAME, &dp, &np);
        i2p = (i2*) colpriv_row->columnName; 
        MEcopy(colpriv_row->columnName, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_COLUMNP_GRANTOR, &dp, &np);
        i2p = (i2*) colpriv_row->grantor;   
        MEcopy(colpriv_row->grantor, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_COLUMNP_GRANTEE, &dp, &np);
        i2p = (i2*) colpriv_row->grantee;  
        MEcopy(colpriv_row->grantee, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_COLUMNP_PRIVILEGE, &dp, &np);
        i2p = (i2*) colpriv_row->privilege; 
        MEcopy(colpriv_row->privilege, *i2p + sizeof(i2) + sizeof(char), dp);

        GetTuplePtrs(pstmt, p, QRY_COLUMNP_IS_GRANTABLE, &dp, &np);
        i2p = (i2*) colpriv_row->is_grantable;
        MEcopy(colpriv_row->is_grantable, *i2p + sizeof(i2) + sizeof(char), dp);
    }  /* end for loop */
    
    UnlockStmt (pstmt);

    return SQL_SUCCESS;
}

#ifdef CATALOG_CACHE_ENABLED
/*
**  CacheFree
**
**  Free all cached data:
**
**  1. Free cached SQLDA.
**  2. Free all cached fetch buffers.
**  3. Clear DBC cache control fields.
**
**  On entry: pdbc-->Data Base Connection block
**
**  Returns:  nothing.
*/
void CacheFree(
    LPDBC   pdbc)
{
    LPCACHE pcache;
    LPCACHE pcacheNext;

    TRACE_INTL (pdbc, "CacheFree");

    MEfree ((PTR)pdbc->psqlda);

    for (pcache = pdbc->pcache; pcache; pcache = pcacheNext)
    {
        pcacheNext = pcache->pcacheNext;
        MEfree ((PTR)pcache);
    }
    pdbc->fStatus &= ~DBC_CACHE;
    pdbc->psqlda   = NULL;
    pdbc->pcache   = NULL;

    return;
}

/*
**  CacheInit
**
**  If no STMT in DBC is using the cache:
**
**  1. Dump partial cache.
**  2. Tell SQLFetch to save fetch buffers.
**
**  On entry: pdbc -->Data Base Connection block
**            pstmt-->Statement block
**
**  Returns:  TRUE  if cache initialized
**            FALSE if cache in use
*/
static BOOL   CacheInit(
    LPDBC   pdbc,
    LPSTMT  pstmt)
{
    LPSTMT  pstmtC;

    TRACE_INTL (pdbc, "CacheInit");

    for (pstmtC = pdbc->pstmtFirst; pstmtC; pstmtC = pstmtC->pstmtNext)
        if (pstmtC->fStatus & STMT_CACHE)
            return FALSE;

    CacheFree (pdbc);
    pstmt->fStatus |= STMT_CACHE_PUT;

    return TRUE;
}


/*
**  CacheGet
**
**  Get next cached fetch buffer.
**
**  On entry: pdbc  -->Data Base Connection block
**            pstmt -->Statment block
**            psqlca-->SQLCA
**
**  On exit:  SQLCA contains results of fetch.
**
**  Returns:  -->first row in buffer
**            NULL if no more buffers
*/
CHAR  *  CacheGet(
    LPDBC   pdbc,
    LPSTMT  pstmt)
{
    TRACE_INTL (pdbc, "CacheGet");

    pstmt->sqlca.SQLMCT  = 0;                           /* no messages, ever */

    if (pstmt->pcache)
        pstmt->pcache = pstmt->pcache->pcacheNext;      /* -->next cache buffer */
    else
        pstmt->pcache = pdbc->pcache;                   /* -->first cache buffer*/

    if (pstmt->pcache)
    {
        pstmt->sqlca.SQLCODE = 0;                      /* success                 */
        pstmt->sqlca.SQLNRP  = pstmt->pcache->crow;    /* number of rows in buffer*/
        return (CHAR   *)(pstmt->pcache + 1);       /* -->first row in buffer  */
    }
    else
    {
        pstmt->sqlca.SQLCODE = 100;                    /* end of fetch     */
        pstmt->sqlca.SQLNRP  = 0;                      /* no rows in buffer*/
        return NULL;                                   /* nothing to return*/
    }
}


/*
**  CacheOpen
**
**  Simulate SQLExecDirect to "prepare" a query and "open" the cursor
**  to retrieve a result set saved in cache.
**
**  On entry: pdbc -->Data Base Connection block
**            pstmt-->Statement block
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
*/
static RETCODE   CacheOpen(
    LPDBC   pdbc,
    LPSTMT  pstmt)
{
    size_t      size;
    RETCODE     rc;
    LPSQLDA     psqlda = &pdbc->sqldaDefault;

    TRACE_INTL (pdbc, "CacheOpen");

    /*
    **  Copy saved SQLDA describing cached result  to set default SQLDA
    **  and prepare a "constant" query to finish setting up statement's
    **  SQLDA and column fetch control blocks:
    */
    psqlda->SQLD = pdbc->psqlda->SQLD;
    size = sizeof (SQLVAR_T) * (size_t)pdbc->psqlda->SQLD;
    MEcopy (pdbc->psqlda->SQLVAR, size, psqlda->SQLVAR );

    rc = PrepareConstant (pstmt);

    /*
    **  If prepare worked (it should have):
    **
    **  1. Mark cursor as open.
    **  2. Prime fetch to start at first cached buffer.
    */
    if (rc == SQL_SUCCESS)
    {
        pstmt->fStatus |= STMT_OPEN + STMT_CACHE_GET;
        pstmt->pcache = NULL;
    }
    else
    {
        ResetStmt (pstmt, TRUE);     /* clear STMT state and free buffers */
    }
    return rc;
}


/*
**  CachePut
**
**  Put current fetch buffer into cache.
**
**  On entry: pdbc -->Data Base Connection block
**            pstmt-->Statement block
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
*/
RETCODE CachePut(
    LPDBC   pdbc,
    LPSTMT  pstmt)
{
    size_t      size;
    LPCACHE     pcache;

    TRACE_INTL (pdbc, "CachePut");

    if (pstmt->sqlca.SQLCODE == 100)
        pdbc->fStatus |= DBC_CACHE;
    if (pstmt->sqlca.SQLNRP == 0)
        return SQL_SUCCESS;
    /*
    **  Allocate CACHE buffer:
    */
    size = (size_t)pstmt->cbrow * (size_t)pstmt->sqlca.SQLNRP;
    pcache = (LPCACHE)MEreqmem((u_i2)0, sizeof (CACHE) + size, TRUE, NULL);
    if (pcache == NULL)
        return ErrState (SQL_HY001, pstmt, F_OD0002_IDS_CACHE_BUFFER);
    /*
    **  Link into DBC->CACHE chain:
    */
    if (pstmt->pcache)
       pstmt->pcache->pcacheNext = pcache;
    else
       pdbc->pcache = pcache;

    pstmt->pcache = pcache;
    /*
    **  Copy fetch buffer into CACHE buffer:
    */
    pcache->crow = (UWORD)pstmt->sqlca.SQLNRP;
    MEcopy (pstmt->pFetchBuffer, size, pcache + 1);

    return SQL_SUCCESS;
}


/*
**  CacheSqlda
**
**  Save the SQLDA corresponding to a result set to be saved in cache.
**
**  On entry: pdbc -->Data Base Connection block
**            pstmt-->Statement block
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
*/
static RETCODE   CacheSqlda(
    LPDBC   pdbc,
    LPSTMT  pstmt)
{
    size_t  size;

    TRACE_INTL (pdbc, "CacheSqlda");

    pdbc->psqlda = AllocSqlda (pstmt->psqldaFetch->SQLD);
    if (pdbc->psqlda == NULL)
        return ErrState (SQL_HY001, pstmt, F_OD0003_IDS_CACHE_SQLDA);

    size = sizeof (SQLVAR_T) * (size_t)pdbc->psqlda->SQLD;
    MEcopy (pstmt->psqldaFetch->SQLVAR, size, pdbc->psqlda->SQLVAR);

    return SQL_SUCCESS;
}
#endif  /* CATALOG_CACHE_ENABLED */


/*
**  CatAlloc
**
**  Allocate buffer for catalog query SQL syntax string:
**
**  On entry: pstmt   -->statement block.
**
**  Returns:  -->buffer or NULL.
*/
static LPSTR   CatAlloc (LPSTMT  pstmt)
{
    LPSTR sz;

    TRACE_INTL (pstmt, "CatAlloc");

    sz = MEreqmem((u_i2)0, LEN_SQL, TRUE, NULL);
    if (!sz) ErrState (SQL_HY001, pstmt, F_OD0012_IDS_SQL_QUERY);
    return sz;
}


/*
**  CatConnect
**
**  Connect to database as a separate, read only session for catalog queries.
**  This avoids some locking and page group problems when accessing IDMS.
**
**  On entry: pstmt   -->statement block.
**
**  Returns:  TRUE if session connected.
**            FALSE if some error.
*/
static BOOL   CatConnect(
    LPSTMT  pstmt)
{
    RETCODE rc;
    UWORD   cConnect = 0;
    LPDBC   pdbc     = pstmt->pdbcOwner;
    LPSESS  psessCat = &pdbc->sessCat;

    TRACE_INTL (pstmt, "CatConnect");

    if (!(pdbc->fOptions & OPT_CATCONNECT))
        return TRUE;  /* return if not separate Ingres session for catalog */

    if (pdbc->sessCat.fStatus & SESS_CONNECTED)
    {
        pstmt->fStatus |= STMT_CATALOG;
        pstmt->psess = psessCat;
        return TRUE;
    }
    /*
    **  Make sure driver supports multiple connections:
    */
    if (pdbc->fDriver != DBC_IDMS)
        SQLGetInfo_InternalCall (
            pdbc, SQL_ACTIVE_CONNECTIONS,
            &cConnect, sizeof (cConnect), NULL);

    /*
    **  If driver has no definite limit to the number of connections:
    **     connect on a special SID,
    **     set transaction/session to be read only and transient retrieval,
    **     and reset SID for now.
    **  Otherwise
    **     assume that we can't use any more connections
    **     (since the caller may have used them all up)
    **     so just return success.
    */
    if (cConnect == 0)
    {
        pdbc->sqb.pSession = psessCat;
        pdbc->sqb.pSqlca  = &pdbc->sqlca;
        pdbc->sqb.pStmt   = pstmt;
        SqlConnect (pdbc->psqb, pdbc->szDSN);
        rc = SQLCA_ERROR (pdbc, psessCat);
        if (rc == SQL_SUCCESS)
        {
            pdbc->sessCat.fStatus |= SESS_CONNECTED;
            rc = SetTransaction (pdbc, psessCat);
            if (rc != SQL_SUCCESS)
            {
                pdbc->sqb.pSession = psessCat;
                SqlRelease (pdbc->psqb);
                psessCat->fStatus &= ~SESS_CONNECTED;
            }
        }
        pdbc->sqb.pSession = &pdbc->sessMain;

        if (rc != SQL_SUCCESS)
            return FALSE;
        
        pstmt->psess = psessCat;
        pstmt->fStatus |= STMT_CATALOG;
    }
    return TRUE;
}


/*
**  CatConvertDesc
**
**  Adjust catalog IRD descriptor to ODBC catalog format descriptor
**
**  On entry: pIRD-->implementation row descriptor header
**            Name-->column name
**            i   =  column index (starting with 1 past bookmark)
**            TargetType = ird fSqlType type to convert to per ODBC spec
**            Nullable   = SQL_NULLABLE or SQL_NO_NULLS 
**
**  On exit:  
**
**  Returns:  nothing.
*/
#define SQL_CHAR1 SQL_TINYINT

static void  CatConvertDesc(
    LPDESC pIRD, char * Name, i2 i, i2 TargetType, SQLSMALLINT Nullable)
{
    LPDESCFLD pird = pIRD->pcol + i;     /* pird->descriptor field to change */

    STcopy(Name, pird->Name);            /* override that field name */

    pird->Nullable = Nullable;           /* override the Nullable property */

    switch(TargetType)                   /* process the conversion desired */
    {
        case SQL_CHAR1:                  /* need conversion to char(1)? */
            if ((pird->fIngApiType == IIAPI_CHA_TYPE  ||  
                 pird->fIngApiType == IIAPI_CHR_TYPE)   &&  pird->OctetLength != 1)
               { pird->fIngApiType  = IIAPI_CHR_to_CHR1_TYPE;
               }
            else
            if ( pird->fIngApiType == IIAPI_VCH_TYPE)
                 pird->fIngApiType  = IIAPI_VCH_to_CHR1_TYPE;
            break;

        case SQL_INTEGER:                /* need conversion to i4? */
            if (pird->fIngApiType == IIAPI_INT_TYPE  &&  pird->OctetLength == 2)
                pird->OctetLength  =  4;
            else 
            if (pird->fIngApiType == IIAPI_FLT_TYPE)
                pird->fIngApiType  = IIAPI_FLT_to_INT4_TYPE;
            break;

        case SQL_SMALLINT:               /* need conversion to i2? */
            if      (pird->fIngApiType == IIAPI_INT_TYPE  &&  pird->OctetLength  == 4)
                    {pird->fIngApiType  = IIAPI_INT4_to_INT2_TYPE;
                    }

            else if (pird->fIngApiType == IIAPI_CHA_TYPE  ||  
                     pird->fIngApiType == IIAPI_CHR_TYPE)
                    {pird->fIngApiType  = IIAPI_CHR_to_INT2_TYPE;
                     if (pird->OctetLength < 2)  
                        pird->OctetLength = 2;
                    }

            else if (pird->fIngApiType == IIAPI_VCH_TYPE)
                    {pird->fIngApiType  = IIAPI_VCH_to_INT2_TYPE;
                     if (pird->OctetLength < 2)  
                         pird->OctetLength = 2;
                    }

            else if (pird->fIngApiType == IIAPI_FLT_TYPE)
                    {pird->fIngApiType  = IIAPI_FLT_to_INT2_TYPE;
                    }
            break;

        case SQL_VARCHAR:                /* need conversion to varchar? */
            if (pird->fIngApiType == IIAPI_CHA_TYPE  ||
                pird->fIngApiType == IIAPI_CHR_TYPE)
                pird->fIngApiType  = IIAPI_CHA_to_VCH_TYPE;
            break;

        default:                         /* no conversion needed */
            break;

    }   /* end switch(TargetType) */
}


/*
**  CatEscape
**
**  Find escape character for driver, if any.
**
**  On entry: pstmt-->statement block.
**
**  Returns:  ESCAPE character, if any, else 0.
*/
static CHAR    CatEscape (LPSTMT  pstmt)
{
    TRACE_INTL (pstmt, "CatEscape");

    if (pstmt->fMetadataID == TRUE)  /* if SQL_ATTR_METADATA_ID == TRUE */
       return '\0';                  /* indicate there are no wild-card search chars */

    return '\\';
}


/*
**  CatExecDirect
**
**  Execute a catalog query.
**
**  Prepare the statement and open the cursor on catalog connection.
**
**  On entry: pstmt    -->statement control block.
**            szSqlStr -->catalog query SQL syntax  string.
**            fCatalog  = catalog query type flag.
**
**  On exit:  Result set is ready to SQLFetch if no errors.
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
*/
static RETCODE   CatExecDirect(
    LPSTMT      pstmt,
    CHAR   * szSqlStr,
    UWORD       fCatalog)
{
    RETCODE     rc;

    TRACE_INTL (pstmt, "CatExecDirect");

    CatSqlsid (pstmt, TRUE);
    pstmt->fCommand = CMD_SELECT;
    pstmt->fCatalog = fCatalog;
    pstmt->fStatus |= STMT_INTERNAL;

    rc = SQLExecDirect_InternalCall (pstmt, (UCHAR  *)szSqlStr, SQL_NTS);

    pstmt->fStatus &= ~STMT_INTERNAL;
    CatSqlsid (pstmt, FALSE);

    return rc;
}


/*
**  CatFetchBuffer
**
**  Convert data to values specified by ODBC result set.
**
**  On entry: pstmt-->statement.
**
**  On exit:  Fetch buffer values for selected columns are converted
**            to the values specified for the ODBC result set.
**
**  Returns:  nothing.
*/
void CatFetchBuffer(
    LPSTMT  pstmt)
{
    LPDESCFLD    pird;     /* pird -> implementation descriptor field */
    UWORD        crow;
    CHAR    * prow;
    SWORD   * pval;
    SWORD        fType;
    char    * pvalchar;
    LPDBC        pdbc = pstmt->pdbcOwner;

    TRACE_INTL (pstmt, "CatFetchBuffer");

    /*
    **  Loop through the fetch buffer adjusting the values in each row:
    */
    crow = (UWORD)pstmt->sqlca.SQLNRP;
    prow = pstmt->pFetchBuffer;
    if (pstmt->fStatus & STMT_CONST)
        prow += pstmt->cbrow;

    while (crow)
    {
        switch (pstmt->fCatalog)
        {
        case CATLG_COLUMNS:
            /*
            **  SQLColumns:
            **
            **  1. Trim trailing spaces from VARCHAR columns.
            **  2. Convert column description to ODBC terms.
            **  3. Set radix based on ODBC type.
            **  4. Convert NULLABLE indicator in ODBC terms.
            **  5. Set NULL indicator for unsupported columns.
            */

            CatNull (pstmt, prow, QRY_COLS_QUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_COLS_OWNER);
            else CatTrim (pstmt, prow, QRY_COLS_OWNER);
            CatTrim (pstmt, prow, QRY_COLS_TABLE);
            CatTrim (pstmt, prow, QRY_COLS_COLUMN);
            CatInt2 (pstmt, prow, QRY_COLS_DATA_TYPE);
            CatTrim (pstmt, prow, QRY_COLS_TYPE_NAME);
            CatInt2 (pstmt, prow, QRY_COLS_SCALE);
            CatInt2 (pstmt, prow, QRY_COLS_RADIX);
            CatNull (pstmt, prow, QRY_COLS_REMARKS);
            CatInt4 (pstmt, prow, QRY_COLS_PRECISION);
            CatInt4 (pstmt, prow, QRY_COLS_LENGTH);
            CatNull (pstmt, prow, QRY_COLS_DEFAULT);  /* 3.x fields */
            CatInt2 (pstmt, prow, QRY_COLS_VERBOSETYPE);
            CatInt2 (pstmt, prow, QRY_COLS_SUBTYPE);
            CatTrim (pstmt, prow, QRY_COLS_ISNULLABLE);


            fType = CatFetchColumn (
                        pstmt,
                        prow,
                        QRY_COLS_DATA_TYPE,
                        QRY_COLS_TYPE_NAME,
                        QRY_COLS_LENGTH,
                        QRY_COLS_PRECISION,
                        QRY_COLS_SCALE,
                        QRY_COLS_VERBOSETYPE,
                        QRY_COLS_SUBTYPE,
                        QRY_COLS_OCTETLENGTH);

            switch (fType)
            {
            case SQL_DOUBLE:
            case SQL_FLOAT:
            case SQL_REAL:
                pird = pstmt->pIRD->pcol + QRY_COLS_RADIX;
                if (pdbc->penvOwner->ODBCVersion >= SQL_OV_ODBC3)
                     *(SWORD*)(prow + pird->cbDataOffset) = 2;  /* 3.x */
                else *(SWORD*)(prow + pird->cbDataOffset) =10;  /* 2.x */
                break;

            case SQL_BIGINT:
            case SQL_DECIMAL:
            case SQL_INTEGER:
            case SQL_NUMERIC:
            case SQL_SMALLINT:
            case SQL_TINYINT:

/*              set radix as 10 since we borrowed it for CREATE TABLE order */
                pird = pstmt->pIRD->pcol + QRY_COLS_RADIX; 
                *(SWORD*)(prow + pird->cbDataOffset) = 10;
                break;              /* 10 returned by query... */

            default:  /* char, byte, date, and rest have null radix */

                CatNull (pstmt, prow, QRY_COLS_RADIX);
                break;
            }

            pird = pstmt->pIRD->pcol + QRY_COLS_NULLABLE; 
            pval = (SWORD  *)(prow + pird->cbDataOffset);
            pvalchar = (char  *)pval;
            if      (!CMcmpcase(pvalchar,"Y")  ||
                     !CMcmpcase(pvalchar,"y") )  *pval = SQL_NULLABLE;
            else if (!CMcmpcase(pvalchar,"N")  ||
                     !CMcmpcase(pvalchar,"n") )  *pval = SQL_NO_NULLS;
            else                                 *pval = SQL_NULLABLE_UNKNOWN; 

            if (fType == SQL_TYPE_TIMESTAMP  &&
                     (pdbc->fOptions & OPT_BLANKDATEisNULL  ||
                      pdbc->fOptions & OPT_DATE1582isNULL) )
               *pval = SQL_NULLABLE;

            pird = pstmt->pIRD->pcol + QRY_COLS_ISNULLABLE; 
            pvalchar = prow + pird->cbDataOffset + 2;
            if     (*pval == SQL_NO_NULLS)          MEcopy("NO ", 3, pvalchar);
/*          else if(*pval == SQL_NULLABLE)          MEcopy("YES", 3, pvalchar); */
            else if(*pval == SQL_NULLABLE_UNKNOWN)  MEcopy("   ", 3, pvalchar);
            CatTrim (pstmt, prow, QRY_COLS_ISNULLABLE);

            break;

        case CATLG_FKEY:
            /*
            **  SQLForeignKeys:
            **
            **  1. Trim trailing spaces from VARCHAR columns.
            **  2. Set NULL indicator for unsupported columns.
            */

            CatNull (pstmt, prow, QRY_FKEY_PKQUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_FKEY_PKOWNER);
            else CatTrim (pstmt, prow, QRY_FKEY_PKOWNER);
            CatTrim (pstmt, prow, QRY_FKEY_PKTABLE);
            CatTrim (pstmt, prow, QRY_FKEY_PKCOLUMN);
            CatNull (pstmt, prow, QRY_FKEY_FKQUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_FKEY_FKOWNER);
            else CatTrim (pstmt, prow, QRY_FKEY_FKOWNER);
            CatTrim (pstmt, prow, QRY_FKEY_FKTABLE);
            CatTrim (pstmt, prow, QRY_FKEY_FKCOLUMN);
            CatInt2 (pstmt, prow, QRY_FKEY_KEY_SEQ);
            CatNull (pstmt, prow, QRY_FKEY_UPDATE_RULE);
            CatNull (pstmt, prow, QRY_FKEY_DELETE_RULE);
            CatTrim (pstmt, prow, QRY_FKEY_FK_NAME);
            CatTrim (pstmt, prow, QRY_FKEY_PK_NAME);
            break;

        case CATLG_PKEY:
            /*
            **  SQLPrimaryKeys:
            **
            **  1. Trim trailing spaces from VARCHAR columns.
            **  2. Set NULL indicator for unsupported columns.
            */

            CatNull (pstmt, prow, QRY_PKEY_QUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_PKEY_OWNER);
            else CatTrim (pstmt, prow, QRY_PKEY_OWNER);
            CatTrim (pstmt, prow, QRY_PKEY_TABLE);
            CatTrim (pstmt, prow, QRY_PKEY_COLUMN);
            CatInt2 (pstmt, prow, QRY_PKEY_KEY_SEQ);
            CatTrim (pstmt, prow, QRY_PKEY_PK_NAME);
            pird = pstmt->pIRD->pcol + QRY_PKEY_PK_NAME; 
            if (*((II_INT2*)(prow + pird->cbDataOffset)) == 0)
                CatNull (pstmt, prow, QRY_PKEY_PK_NAME);

            break;

        case CATLG_PROC:
            /*
            **  SQLProcedures:
            **
            */
            CatNull (pstmt, prow, QRY_PROC_QUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_PROC_OWNER);
            else CatTrim (pstmt, prow, QRY_PROC_OWNER);
            CatTrim (pstmt, prow, QRY_PROC_NAME);
            CatNull (pstmt, prow, QRY_PROC_NUMIN);
            CatNull (pstmt, prow, QRY_PROC_NUMOUT);
            CatNull (pstmt, prow, QRY_PROC_NUMRES);
            CatNull (pstmt, prow, QRY_PROC_REMARKS);
            CatInt2 (pstmt, prow, QRY_PROC_TYPE);
            break;

        case CATLG_PROCCOL:
            /*
            **  SQLProcedureColumns:
            **
            **  1. Trim trailing spaces from VARCHAR columns.
            **  2. Convert column description to ODBC terms.
            **  3. Set radix based on ODBC type.
            **  4. Convert NULLABLE indicator in ODBC terms.
            **  5. Set NULL indicator for unsupported columns.
            */

            CatNull (pstmt, prow, QRY_PROCCOL_QUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_PROCCOL_OWNER);
            else CatTrim (pstmt, prow, QRY_PROCCOL_OWNER);
            CatTrim (pstmt, prow, QRY_PROCCOL_PROC);
            CatTrim (pstmt, prow, QRY_PROCCOL_COLUMN);
            CatInt2 (pstmt, prow, QRY_PROCCOL_COLUMNTYPE);
            CatInt2 (pstmt, prow, QRY_PROCCOL_DATA_TYPE);
            CatTrim (pstmt, prow, QRY_PROCCOL_TYPE_NAME);
            CatInt2 (pstmt, prow, QRY_PROCCOL_SCALE);
            CatInt2 (pstmt, prow, QRY_PROCCOL_RADIX);
            CatNull (pstmt, prow, QRY_PROCCOL_REMARKS);
            CatInt4 (pstmt, prow, QRY_PROCCOL_PRECISION);
            CatInt4 (pstmt, prow, QRY_PROCCOL_LENGTH);

            fType = CatFetchColumn (
                        pstmt,
                        prow,
                        QRY_PROCCOL_DATA_TYPE,
                        QRY_PROCCOL_TYPE_NAME,
                        QRY_PROCCOL_LENGTH,
                        QRY_PROCCOL_PRECISION,
                        QRY_PROCCOL_SCALE,
                        QRY_PROCCOL_VERBOSETYPE,
                        QRY_PROCCOL_SUBTYPE,
                        QRY_PROCCOL_OCTETLENGTH);

            switch (fType)
            {
            case SQL_DOUBLE:
            case SQL_FLOAT:
            case SQL_REAL:
                pird = pstmt->pIRD->pcol + QRY_PROCCOL_RADIX;
                if (pdbc->penvOwner->ODBCVersion >= SQL_OV_ODBC3)
                     *(SWORD*)(prow + pird->cbDataOffset) = 2;  /* 3.x */
/*              else leave radix as 10 returned by query for       2.x */
                break;

            case SQL_BIGINT:
            case SQL_DECIMAL:
            case SQL_INTEGER:
            case SQL_NUMERIC:
            case SQL_SMALLINT:
            case SQL_TYPE_TIMESTAMP:
            case SQL_TYPE_TIME:
            case SQL_TYPE_DATE:
            case SQL_TINYINT:
            case SQL_INTERVAL_YEAR:
            case SQL_INTERVAL_MONTH:
            case SQL_INTERVAL_YEAR_TO_MONTH:
            case SQL_INTERVAL_DAY:
            case SQL_INTERVAL_HOUR:
            case SQL_INTERVAL_MINUTE:
            case SQL_INTERVAL_SECOND:
            case SQL_INTERVAL_DAY_TO_MINUTE:
            case SQL_INTERVAL_DAY_TO_HOUR:
            case SQL_INTERVAL_DAY_TO_SECOND:
            case SQL_INTERVAL_HOUR_TO_MINUTE:
            case SQL_INTERVAL_HOUR_TO_SECOND:
            case SQL_INTERVAL_MINUTE_TO_SECOND:

                break;              /* 10 returned by query... */

            default:

                CatNull (pstmt, prow, QRY_PROCCOL_RADIX);
                break;
            }

            /* we borrowed COLUMNTYPE for order by param_sequence; now reset it*/
            pird = pstmt->pIRD->pcol + QRY_PROCCOL_COLUMNTYPE; 
            pval = (SWORD  *)(prow + pird->cbDataOffset);
            *pval = SQL_PARAM_TYPE_UNKNOWN;

            pird = pstmt->pIRD->pcol + QRY_PROCCOL_NULLABLE; 
            pval = (SWORD  *)(prow + pird->cbDataOffset);
            pvalchar = (char  *)pval;
            if      (!CMcmpcase(pvalchar,"Y") || !CMcmpcase(pvalchar,"y"))  *pval = SQL_NULLABLE;
            else if (!CMcmpcase(pvalchar,"N") || !CMcmpcase(pvalchar,"n"))  *pval = SQL_NO_NULLS;
            else                                              *pval = SQL_NULLABLE_UNKNOWN; 

            if (fType == SQL_TYPE_TIMESTAMP  &&
                      (pdbc->fOptions & OPT_BLANKDATEisNULL  ||
                       pdbc->fOptions & OPT_DATE1582isNULL) )
               *pval = SQL_NULLABLE;

            break;

        case CATLG_SPECCOL:
            /*
            **  SQLSpecialColumns:
            **
            **  1. Trim trailing spaces from VARCHAR columns.
            **  2. Convert column description to ODBC terms.
            */

            CatInt2 (pstmt, prow, QRY_SPEC_SCOPE);
            CatTrim (pstmt, prow, QRY_SPEC_COLUMN);
            CatInt2 (pstmt, prow, QRY_SPEC_DATA_TYPE);
            CatTrim (pstmt, prow, QRY_SPEC_TYPE_NAME);
            CatInt2 (pstmt, prow, QRY_SPEC_SCALE);
            CatInt2 (pstmt, prow, QRY_SPEC_PSEUDO);
            CatInt4 (pstmt, prow, QRY_SPEC_PRECISION);
            CatInt4 (pstmt, prow, QRY_SPEC_LENGTH);

            pird = pstmt->pIRD->pcol + QRY_SPEC_DATA_TYPE; 
            pval = (SWORD  *)(prow + pird->cbDataOffset);
            if (*pval < -1)   /* if type < 0 then column is nullable */
                *pval = (SWORD)(-(*pval)); /* normalize back to IIAPI_xxx_TYPE */
            
            CatFetchColumn (
                pstmt,
                prow,
                QRY_SPEC_DATA_TYPE,
                QRY_SPEC_TYPE_NAME,
                QRY_SPEC_LENGTH,
                QRY_SPEC_PRECISION,
                QRY_SPEC_SCALE,
                0,
                0,
                0);

            break;

        case CATLG_STATS:
            /*
            **  SQLStatistics:
            **
            **  1. Set NULL indicator for unsupported columns.
            **  2. Trim trailing spaces from VARCHAR columns.
            **  3. Figure out type of row (table or index).
            */
            CatNull (pstmt, prow, QRY_STAT_QUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_STAT_OWNER);
            else CatTrim (pstmt, prow, QRY_STAT_OWNER);
            CatTrim (pstmt, prow, QRY_STAT_TABLE);
            CatInt2 (pstmt, prow, QRY_STAT_TYPE);
            CatNull (pstmt, prow, QRY_STAT_PAGES);
            CatNull (pstmt, prow, QRY_STAT_CARD);
            CatNull (pstmt, prow, QRY_STAT_FILTER);

            pird = pstmt->pIRD->pcol + QRY_STAT_TYPE; 
            fType = *(SWORD  *)(prow + pird->cbDataOffset);

            if (fType ==  SQL_TABLE_STAT)       /* Table statistics */
            {
                CatNull (pstmt, prow, QRY_STAT_UNIQUE);
                CatNull (pstmt, prow, QRY_STAT_IQUALIFIER);
                CatNull (pstmt, prow, QRY_STAT_INDEX);
                CatNull (pstmt, prow, QRY_STAT_SEQUENCE);
                CatNull (pstmt, prow, QRY_STAT_COLUMN);
                CatNull (pstmt, prow, QRY_STAT_COLLATION);
            }
            else                                /* Index statistics */
            {
                CatTrim (pstmt, prow, QRY_STAT_IQUALIFIER);
                CatTrim (pstmt, prow, QRY_STAT_INDEX);
                CatInt2 (pstmt, prow, QRY_STAT_SEQUENCE);
                CatTrim (pstmt, prow, QRY_STAT_COLUMN);

                pird = pstmt->pIRD->pcol + QRY_STAT_IQUALIFIER; 
                if (*((WORD*)(prow + pird->cbDataOffset)) == 0)
                    CatNull (pstmt, prow, QRY_STAT_IQUALIFIER);

                if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                    CatNull (pstmt, prow, QRY_STAT_IQUALIFIER);

                pird = pstmt->pIRD->pcol + QRY_STAT_COLLATION; 
                if (*(prow + pird->cbDataOffset) == ' ')
                    CatNull (pstmt, prow, QRY_STAT_COLLATION);

                pird = pstmt->pIRD->pcol + QRY_STAT_UNIQUE; 
                pval = (SWORD  *)(prow + pird->cbDataOffset);
                pvalchar = (char  *)pval;
                if (!CMcmpcase(pvalchar,"U"))  
                     *pval = FALSE;  /* NON_UNIQUE=False */
                else *pval = TRUE; 
            }

            break;

        case CATLG_CATALOGS:
        case CATLG_OWNERS:
            /*
            **  SQLTables, enumerate owners:
            **
            **  1. Set NULL indicator for unsupported columns.
            **  2. Trim trailing spaces from VARCHAR columns.
            */
            CatNull (pstmt, prow, QRY_TABL_QUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_TABL_OWNER);
            else CatTrim (pstmt, prow, QRY_TABL_OWNER);
            CatNull (pstmt, prow, QRY_TABL_TABLE);
            CatNull (pstmt, prow, QRY_TABL_TYPE);
            CatNull (pstmt, prow, QRY_TABL_REMARKS);
            break;

        case CATLG_TABLES:
            /*
            **  SQLTables, all others:
            **
            **  1. Set NULL indicator for unsupported columns.
            **  2. Trim trailing spaces from VARCHAR columns.
            */
            CatNull (pstmt, prow, QRY_TABL_QUALIFIER);
            if (pdbc->fOptions & OPT_CATSCHEMA_IS_NULL)
                 CatNull (pstmt, prow, QRY_TABL_OWNER);
            else CatTrim (pstmt, prow, QRY_TABL_OWNER);
            CatTrim (pstmt, prow, QRY_TABL_TABLE);
            CatTrim (pstmt, prow, QRY_TABL_TYPE);
            CatNull (pstmt, prow, QRY_TABL_REMARKS);
            break;

       case  CATLG_TABLEPRIV:
            CatTrim (pstmt, prow, QRY_TABLEP_TABLE_CAT);
            CatTrim (pstmt, prow, QRY_TABLEP_TABLE_SCHEM);
            CatTrim (pstmt, prow, QRY_TABLEP_TABLE_NAME);
            CatTrim (pstmt, prow, QRY_TABLEP_GRANTOR);
            CatTrim (pstmt, prow, QRY_TABLEP_GRANTEE);
            CatTrim (pstmt, prow, QRY_TABLEP_PRIVILEGE);
            CatTrim (pstmt, prow, QRY_TABLEP_IS_GRANTABLE);
       
            break;

       case  CATLG_COLUMNPRIV:
            CatTrim (pstmt, prow, QRY_COLUMNP_TABLE_CAT);
            CatTrim (pstmt, prow, QRY_COLUMNP_TABLE_SCHEM);
            CatTrim (pstmt, prow, QRY_COLUMNP_TABLE_NAME);
            CatTrim (pstmt, prow, QRY_COLUMNP_COLUMN_NAME);
            CatTrim (pstmt, prow, QRY_COLUMNP_GRANTOR);
            CatTrim (pstmt, prow, QRY_COLUMNP_GRANTEE);
            CatTrim (pstmt, prow, QRY_COLUMNP_PRIVILEGE);
            CatTrim (pstmt, prow, QRY_COLUMNP_IS_GRANTABLE);
       
            break;
        }
        crow--;
        prow += pstmt->cbrow;
    }
    return;
}


/*
**  CatFetchColumn
**
**  Set common column values for SQLColumns,
**                               SQLProcedureColumns,
**                           and SQLSpecialColumns.
**
**  On entry: pstmt    -->STMT block for query
**            prow     -->row occurrence
**            iType     = index of COL block for type code
**            iTypeName = index of COL block for type name
**            iLength   = index of COL block for length
**            iPrec     = index of COL block for precision
**            iScale    = index of COL block for scale
**
**  On exit:  DATA_TYPE, TYPE_NAME, LENGTH, PRECISION, and SCALE
**            are set in fetch buffer for current row.
**
**  Returns:  nothing
**
**  Note that when doing an SQLColumns for a single table the fetch buffer
**  is actually built from an SQLDA returned by the data base.  The fetch
**  buffer is treated by the rest of ODBC as if it were a "constant".
*/
static SWORD   CatFetchColumn(
    LPSTMT       pstmt,
    CHAR    * prow,
    UWORD        iType,
    UWORD        iTypeName,
    UWORD        iLength,
    UWORD        iPrec,
    UWORD        iScale,
    UWORD        iVerboseType,
    UWORD        iSubtype,
    UWORD        iOctetLength)
{
    LPDBC        pdbc = pstmt->pdbcOwner;
    SWORD   * pfType;
    SWORD   * pfSqlType;
    SWORD   * pcbTypeName;
    LPSTR        szTypeName;
    SQLINTEGER * pcbLength;
    SQLINTEGER * pcbOctetLength;
    SQLINTEGER * pcbPrec;
    SWORD   * pcbScale;
    SDWORD  * pfPrecNull;
    SDWORD  * pfScaleNull;
    SDWORD  * pfOctetLengthNull;
    SWORD      * pfSubType;
    SDWORD  * pfSubTypeNull;
    LPDESC       pIRD = pstmt->pIRD;  /* pird -> implementation descriptor field */
    LPDESCFLD    pird;                /* pird -> implementation descriptor field */
    LPSQLTYPEATTR ptype;
    LPSTR        lpsz;
    DESCFLD      TempDescriptorField = {0};
    SWORD        fType;
    LPDESCFLD    pirdtypename; /* pird -> descriptor field for typename */
    SWORD        lentypename;
    int          i;
    char         tempname[9];

    TRACE_INTL (pstmt, "CatFetchColumn");

    /*
    **  Normalize TYPE and compute TYPE from DATATYPE if needed:
    */
    pird = pIRD->pcol + iType;         /* (QRY_COLS_DATA_TYPE or 
                                           QRY_SPEC_DATA_TYPE) */ 
    pfType = (SWORD  *)(prow + pird->cbDataOffset);
    pirdtypename = pIRD->pcol + iTypeName; 
                         /* (QRY_COLS_TYPE_NAME or QRY_SPEC_TYPE_NAME) */
    pcbTypeName = (SWORD  *)(prow + pirdtypename->cbDataOffset);
                        /* pcbTypeName->length of the varchar TYPE_NAME */
    lentypename = *pcbTypeName;
                        /* length of varchar TYPE_NAME */
    szTypeName  = (char  *)pcbTypeName+2;  /* start of TYPE_NAME */
                        /* szTypeName->string of the varchar TYPE_NAME */

    if (*pfType < -1)   /* if type < 0 then column is nullable */
       {*pfType = (SWORD)(-(*pfType));  /* normalize back to IIAPI_xxx_TYPE */
       }
    else if (*pfType == -1)  /* DB2 (and other views?) always return -1 */
       { int i;            /* for ingdatatype! Map typename to type.  */
         if (lentypename > (sizeof(tempname)-1))  
            lentypename = sizeof(tempname)-1;
         MEcopy(szTypeName,lentypename,tempname);
         CMcpychar("\0",&tempname[lentypename]); 
/*
         for (pvalchar=tempname; *pvalchar; pvalchar++) ** upper case it **
             if (islower(*pvalchar))  toupper(*pvalchar);
*/
         for (i=0; i<SIZEOF_Datatype_To_Ingdatatype_Table; i++)
             {if (STcompare(Datatype_To_Ingdatatype_Table[i].szdatatype,
                  tempname) == 0)
                 {*pfType = Datatype_To_Ingdatatype_Table[i].ingdatatype;
                  break;
                 }
             }  /* end for loop through Datatype_To_Ingdatatype_Table */
         if (i == SIZEOF_Datatype_To_Ingdatatype_Table)
             *pfType = IIAPI_CHA_TYPE;   /* not in conv table; use CHAR */
       }  /* end if (*pfType == -1) */

    if (!lentypename)   /* IIGWPROCPARAMS have ingdatatype, but not typename*/
       { char * s;      /* map type to typename.*/
         for (i=0; i<SIZEOF_Datatype_To_Ingdatatype_Table; i++)
             {if (*pfType == Datatype_To_Ingdatatype_Table[i].ingdatatype)
                 break;
             }  /* end for loop through Datatype_To_Ingdatatype_Table */
         
         if (i == SIZEOF_Datatype_To_Ingdatatype_Table)
              s="UNKNOWN";   /* not in conv table; use "UNKNOWN" */
         else if (*pfType == IIAPI_LVCH_TYPE)  /* if long varchar, */
              s="LONG VARCHAR";                /* use long name */
         else s=Datatype_To_Ingdatatype_Table[i].szdatatype;
         STcopy(s, szTypeName);
         *pcbTypeName=(SWORD)STlength(szTypeName); /* length of varchar TYPE_NAME */
       }  /* endif (!lentypename) */
            
    /*
    **  Find column values in fetch buffer:
    */
    pird        = pIRD->pcol + iLength; 
    pcbLength   = (SQLINTEGER *)(prow + pird->cbDataOffset);

    pird        = pIRD->pcol + iPrec; 
    pcbPrec     = (SQLINTEGER *)(prow + pird->cbDataOffset);
    pfPrecNull  = (SDWORD  *)(prow + pird->cbNullOffset);

    pird        = pIRD->pcol + iScale; 
    pcbScale    = (SWORD  *)(prow + pird->cbDataOffset);
    pfScaleNull = (SDWORD  *)(prow + pird->cbNullOffset);

    if(iVerboseType)  /* if SQL_DATA_TYPE wanted by SQLColumns or SQLProcCols */
      {
       pird = pIRD->pcol + iVerboseType;      /* (QRY_COLS_VERBOSETYPE or 
                                                  QRY_PROCCOL_VERBOSETYPE) */ 
       pfSqlType = (SWORD*)(prow + pird->cbDataOffset);

       pird = pIRD->pcol + iSubtype;          /* (QRY_COLS_SUBTYPE or 
                                                  QRY_PROCCOL_SUBTYPE) */ 
       pfSubType     =  (SWORD*)(prow + pird->cbDataOffset);
       pfSubTypeNull = (SDWORD*)(prow + pird->cbNullOffset);
      }

    if(iOctetLength)  /* if OCTETLENGTH wanted by SQLColumns or SQLProcCols */
      {
       pird = pIRD->pcol + iOctetLength;      /* (QRY_COLS_OCTETLENGTH or 
                                                  QRY_PROCCOL_OCTETLENGTH) */ 
       pcbOctetLength    = (SQLINTEGER*)(prow + pird->cbDataOffset);
       pfOctetLengthNull = (SDWORD*)    (prow + pird->cbNullOffset);
       fType     = *pfType;
       if (fType != IIAPI_CHA_TYPE   &&   /* if not a char nor binary  */
           fType != IIAPI_CHR_TYPE   &&   /*    return NULL for OCTETLENGTH */
           fType != IIAPI_VCH_TYPE   &&
           fType != IIAPI_LVCH_TYPE  &&
           fType != IIAPI_BYTE_TYPE  &&
           fType != IIAPI_VBYTE_TYPE &&
           fType != IIAPI_LBYTE_TYPE &&
           fType != IIAPI_TXT_TYPE   &&
# ifdef IIAPI_NCHA_TYPE
           fType != IIAPI_LTXT_TYPE  &&
           fType != IIAPI_NCHA_TYPE  &&
           fType != IIAPI_NVCH_TYPE  &&
           fType != IIAPI_LNVCH_TYPE)
# else
           fType != IIAPI_LTXT_TYPE)
# endif
               *pfOctetLengthNull = SQL_NULL_DATA /* -1 */;
      }

    /*
    **  Build a dummy IRD descriptor field so we can use DescXXXX functions
    */
    pird                          = &TempDescriptorField;
    pird->fIngApiType = fType     = *pfType;
    pird->OctetLength             = *pcbLength;
    pird->Precision = (SQLSMALLINT)(*pcbPrec);
    pird->Scale                   = *pcbScale;

# ifdef IIAPI_NCHA_TYPE
    if (fType == IIAPI_NCHA_TYPE  ||   /* if NCHAR, convert number of chars */
        fType == IIAPI_NVCH_TYPE  ||   /*                to number of bytes */
        fType == IIAPI_LNVCH_TYPE)
        {
         pird->OctetLength *= sizeof(SQLWCHAR);
        }
# endif

    if (fType == IIAPI_VCH_TYPE  ||   /* if v-length, descriptor */
        fType == IIAPI_TXT_TYPE  ||   /*   includes 2-byte length word */
# ifdef IIAPI_NVCH_TYPE
        fType == IIAPI_VBYTE_TYPE||
        fType == IIAPI_NVCH_TYPE)
# else
		fType == IIAPI_VBYTE_TYPE)
# endif
        {pird->OctetLength  += sizeof(II_INT2);
/*       pird->Precision    += sizeof(II_INT2);   */
        }
    SetDescDefaultsFromType(pstmt->pdbcOwner, pird);

    /*
    ** Convert common attributes:
    **
    **  1. ODBC data type
    **  2. Precison
    **  3. Scale
    */
    *pfType  = pird->ConciseType;

    if(iVerboseType)  /* if SQL_DATA_TYPE wanted by SQLColumns or SQLProcCols */
      {
       *pfSqlType    = pird->VerboseType;
       *pfSubType    = pird->DatetimeIntervalCode;
       if (pird->VerboseType != SQL_DATETIME  &&
           pird->VerboseType != SQL_INTERVAL)
           *pfSubTypeNull = SQL_NULL_DATA /* -1 */;
      }

    *pcbPrec = (SQLINTEGER)(pird->Precision);
    CatScale (pird, pcbScale, pfScaleNull);

    /*
    ** Get type name keying off of fType:
    */
                        /* pcbTypeName->length of the varchar TYPE_NAME */
                        /* szTypeName->string of the varchar TYPE_NAME */
        lpsz = GetDescTypeName (pird);
        if (lpsz)
        {
            *pcbTypeName = (SWORD) STlength (lpsz);
            STcopy (lpsz, szTypeName);
        }
        else
        {
            *pcbTypeName = 0;
            *szTypeName  = 0;
        }

    /*
    **  For SQLColumns length is length returned by default C data type:
    */
    if (iLength == QRY_COLS_LENGTH)
    {
        ptype = CvtGetSqlType (*pfType, pstmt->fAPILevel);
        if (ptype == NULL)
            return ErrState (SQL_HY000, pstmt, F_OD0075_IDS_ERR_TYPE_NONE);

        if ((*pcbLength = ptype->cbSize) == 0)
        {
            *pcbLength = (SQLINTEGER)(pird->Precision);
            if (*pfType == SQL_DECIMAL || *pfType == SQL_NUMERIC)
                *pcbLength += 2;
        }
    }  /* end if (iLength == QRY_COLS_LENGTH) */
    /*
    **  For SQLSpecialColumns length is actual length on server:
    */
    else
    {
        if (pird->fIngApiType == IIAPI_DTE_TYPE)
            *pcbLength = 12;
    }  /* end if (iLength != QRY_COLS_LENGTH) */

    if (*pfType == SQL_WCHAR    ||
        *pfType == SQL_WVARCHAR ||
        *pfType == SQL_WLONGVARCHAR)
       {*pcbLength *= sizeof(SQLWCHAR);    /* number of bytes */
        if (iOctetLength)
           *pcbOctetLength = *pcbLength;    /* number of bytes */
       }

    if (*pcbLength == SQL_NO_TOTAL)  /* if long varchar or long varbyte */
       { *pcbLength = 0;             /*    return 0 for length and precision */
         *pcbPrec   = 0;             /*    since true maximum length unknown */
       }

    fType = *pfType;

    /*
    ** If this is an ODBC 2.x application, return ODBC 2.x versions of
    ** the date/time types.
    */
    if (pdbc->penvOwner->ODBCVersion <= SQL_OV_ODBC2)  
    {
        switch (fType)
        {
        case SQL_TYPE_TIMESTAMP:  
            *pfType = SQL_TIMESTAMP;  
            break;

        case SQL_TYPE_TIME: 
            *pfType = SQL_TIME;  
            break;

        case SQL_TYPE_DATE: 
            *pfType = SQL_DATE; 
            break;
        }
    }
    /*
    **  Return derived type:
    */
    return fType;
}


/*
**  CatInt
**
**  Convert column value to integer from float.
**  (Needed because opingdt and others return 
**   integer literals as float in their SQLVARs.)
**
**  On entry: pstmt-->statement.
**            prow -->row in fetch buffer.
**            icol  = index of column.
**            ftype = index of column.
**
**  On exit:  value is converted to int2 or int4
**
**  Returns:  nothing.
*/
static void CatInt(
    LPSTMT      pstmt,
    CHAR   * prow,
    UWORD       icol,
    UWORD       type)
{
    LPDESCFLD    pird;     /* pird -> implementation descriptor field */
    void  *   p;
    double  * pfloat;
    II_INT2  * pi2;
    II_INT4  * pi4;
    double       floatValue=0.0;
    i4           ltemp=0;
    char         buf[256];
/*  i4           digits, exp;  */
    i2           res_width;

    TRACE_INTL (pstmt, "CatInt");

    MEfill (sizeof(buf), 0, buf);

    pird = pstmt->pIRD->pcol + icol; 
    if (!(pird->fStatus & COL_CVTFLTINT))    /* return if not float->int conv */
       return;

    p      = (void  *)(prow + pird->cbDataOffset); /* ptr to value*/
    pi2    = (II_INT2  *)p;                     /* ptr to int2 ver of value*/
    pi4    = (II_INT4  *)p;                     /* ptr to int4 ver of value*/
    pfloat = (double  *) p;                     /* ptr to flt ver of value*/
    floatValue = *pfloat;                          /* dtemp = (double) value */
/*  s = _fcvt(floatValue, 0, &dec, &sign); */      /* s -> integer string  */
/*  CVfcvt(floatValue, buf, &digits, &exp, 0);    this should work also */
    CVfa(floatValue, 22, 0, 'g', '.', buf, &res_width);
    CVal(buf, &ltemp);

    if (type == SQL_SMALLINT)
          *pi2 = (II_INT2) ltemp;
    else  *pi4 = (II_INT4) ltemp;

    return;
}



/*
**  CatNull
**
**  Set column value as NULL.
**
**  On entry: pstmt-->statement.
**            prow -->row in fetch buffer.
**            icol  = index of column.
**
**  On exit:  NULL indicator for column is set to -1
**
**  Returns:  nothing.
*/
static void   CatNull(
    LPSTMT      pstmt,
    CHAR  *  prow,
    UWORD       icol)
{
    LPDESCFLD   pird;     /* pird -> implementation descriptor field */
    SDWORD    * pfNull;

    TRACE_INTL (pstmt, "CatNull");

    pird = pstmt->pIRD->pcol + icol; 
    pfNull  = (SDWORD  *)(prow + pird->cbNullOffset);
    *pfNull = SQL_NULL_DATA /* -1 */;

    return;
}


/*
**  CatParm
**
**  Identify type and set up parameter for catalog query WHERE clause.
**  Also edit the parm for length, allowing for ESCAPE characters, if any.
**
**  If a parameter contains no search arguments ('%' or '_'), the
**  WHERE clause can test for equality.  If search arguments are
**  present, the WHERE clause must contain a LIKE predicate.
**
**  Unfortunately, SQL identifiers in the catalog are defined as
**  CHAR columns, rather than VARCHAR, and all names are padded with
**  trailing spaces.  The parser pads identifiers with spaces when
**  building it's trees, which are the internal representation
**  of SQL commands, so these spaces are not generally significant,
**  except when using LIKE.  The LIKE predicate is sensitive to
**  the length of its operands, that is, "ABC" is NOT LIKE "ABC ".
**  ESCAPE characters further complicate this process.  This routine
**  attempts to allow for this by adding pad characters to parms
**  shorter than the expected identifier length when a search
**  argument is present in the parm.
**
**  The CatTrim function is applied to identifiers when they are returned
**  from the catalog to trim off trailing blanks.
**
**  On entry: szParm   -->where to return LIKE clause parameter.
**            cbParmMax = maximum length of parameter.
**            rgbParm  -->parameter value.
**            cbParm    = length of parameter calue or SQL_NTS.
**            bEscape   = ESCAPE character 
**                           ( '\\' if SQL_ATTR_METADATA_ID == FALSE).
**                      = '\0' if not to search for wild-card chars
**                           (e.g. SQL_ATTR_METADATA_ID == TRUE)
**
**            Note that sizeof (szParm) is assumed to be 2 * cbParmMax + 1.
**
**  On exit:  szParm contains parameter as a null terminated string,
**            possibly modified as follows:
**
**            rgbParm == NULL, won't need szParm.
**            rgbParm does not contain '%' or '_', strip trailing spaces.
**            rgbParm ends with '%' (and not '\%'), leave as is.
**            rgbParm has '_' but not '%', pad with spaces.
**            rgbParm contains '%' (not '\%'), pad with " %".
**
**  Returns:  PARM_INVALID  if parm too long.
**            PARM_NULL     if rgbParm is a NULL pointer or points to "%".
**            PARM_EMPTY    if rgbParm points to an empty string.
**            PARM_LIKE     if parm contains a search argument.
**            PARM_EQUAL    if parm contains no search argument.
*/
static PARM   CatParm(
    LPDBC       pdbc,
    LPSTR       szParm,
    SWORD       cbParmMax,
    UCHAR  * rgbParmeter,
    OFFSET_TYPE cbParm,
    CHAR        bEscape)
{
    CHAR   * p;
    CHAR   * p1;
    CHAR   * pt;
    CHAR   * rgbParm = (CHAR  *)rgbParmeter;
    LPSTR   sz;
    int     cb, cbnew, i;
    int     cEscape = 0;
    BOOL    fEscape = FALSE;
    BOOL    fWild   = FALSE;
    BOOL    fSearch = FALSE;
    int     cbMax   = 2 * cbParmMax + 1;
    UCHAR   szUnderscoreParm[65] = "";
    i4      bytecnt;

    MEfill (cbMax, 0, szParm);

    if (rgbParm == NULL)
        return PARM_NULL;

    /*
    **  Get actual length of parameter passed:
    */
    if (cbParm == SQL_NTS)
        cbParm = (OFFSET_TYPE)STlength (rgbParm);

    /*
    **  Handle quoted identifier by ignoring quotes:
    */
    cbParm = (OFFSET_TYPE)CatUnquote (&rgbParm, (size_t)cbParm);

    if (cbParm > 32767)
        return PARM_INVALID;

    if (bEscape  &&  pdbc->fOptions & OPT_DISABLEUNDERSCORE)
       {         /* underscores as search characters have been disabled */
        char *s, *s1, *t;
        for (i=0, s = rgbParm, t = (char *)szUnderscoreParm, cbnew=0;
                         i < cbParm;  i++, cbnew++, s++, t++)
            {/* loop through old string and build new string with escapes*/
             s1 = s + 1;
             if (*s == bEscape  &&  (i+1) < cbParm  &&  
                      (*s1 == '_' ||
                       *s1 == '%' ||
                       *s1 == bEscape))
                {  /* escaped char pair found */
                 *t++ = *s++;  /* copy escape char to new string */
                 i++;          /* increment old char position */
                 cbnew++;      /* increment new char length*/
                 /* fall through with s->underscore */
                }  /* end if escaped underscore char pair found */
             else
             if (*s == '_')
                {  /* naked underscore found; escape it to disable 
                      its searching ability */
                 *t++ = bEscape; /* copy escape char to new string */
                 cbnew++;        /* increment new char length*/
                 /* fall through with s->underscore */
                }  /* end if naked underscore found */

             *t = *s;   /* copy char to new string */
            }   /* end for loop through old string */

        rgbParm = 
	    (char *)szUnderscoreParm;  /* rgbParm -> new escaped string */
        cbParm  = cbnew;             /* cbParm = len of escaped string */
       }  /* end if (bEscape  &&  pdbc->fOptions & OPT_DISABLEUNDERSCORE) */

    cb = (int)cbParm;
    if (cb == 0)
    {
        MEfill (cbParmMax, ' ', szParm);
        return PARM_EMPTY;
    }
    else
    if (cb == 1 && !CMcmpcase(rgbParm,"%") && bEscape)
        return PARM_NULL;

    /*
    **  Count escape characters in parameter,
    **  check for search arguments while we're at it:
    */
    if (bEscape)  /* if SQL_ATTR_METADATA_ID == FALSE then look for wild-card chars */
      for (i = 0, p = rgbParm; i < cb; CMnext(p) )
      {
        if (fEscape)
            fEscape = FALSE;        /* eat next character after ESCAPE.*/
        else
        {
			if (!CMcmpcase(p,"%") || !CMcmpcase(p,"_") )
            {
                fWild = TRUE;       /* found a repeating wild card */
                fSearch = TRUE;     /* found a search argument */
            }
			else
            {
                if (bEscape && !CMcmpcase(p,&bEscape) )
                {
                    cEscape++;          /* Count ESCAPE character */
                    fEscape = TRUE;     /* and remember to skip next char */
                }
            }
        }
		bytecnt = CMbytecnt(p);
        i += bytecnt;
      }   /* end for loop searching for wild-card characters */

    if (cb - cEscape > cbParmMax)
        return PARM_INVALID;

    /*
    **  If there are no search arguments, we can use the = predicate
    **  which is not sensitive to trailing spaces (the LIKE predicate
    **  is sensitive to trailing spaces).  We need to remove any
    **  ESCAPE characters for this to work.  We also remove any trailing
    **  spaces to make it easier to test for certain values.
    */
    if (!fSearch)
    {
        if (cEscape)
        {
            i = 0;
			sz = szParm;
			p = (CHAR*)rgbParm;
			while (i < cb) 
            {
                if ( CMcmpcase(p,"\\") )
                {
					CMcpychar(p,sz);
					CMnext(sz);
                }
                i = CMbyteinc(i,p); 
				CMnext(p); 
            }
        }
        else
            MEcopy (rgbParm, cb, szParm);
        /*
        sz = szParm + STlength (szParm);
        while (sz != szParm && *--sz == ' ')
           *sz = 0;
        */
		STtrmwhite(szParm);
        CatToDB_NAME_CASE(pdbc, szParm);

        return PARM_EQUAL;
    }

    /*
    **  Search arguments present, so we will use LIKE predicate.
    **  The LIKE predicate is sesitive to length, so we may need
    **  to pad with spaces.  A repeating wild card at the end
    **  makes length insignificant, anywhere it forces us into a
    **  kluge that will work almost all of the time.
    */
    MEcopy (rgbParm, cb, szParm);
    
    if (pdbc->db_table_name_length==0)  /* if not resolved */
        CatGetTableNameLength(pdbc, NULL);

    if (pdbc->db_table_name_length != -1)  /* pad if not varchar */
    {cbParmMax=(SWORD)pdbc->db_table_name_length;
     if (fWild)
     {
		/* make p -> the last char */
		p = p1 = pt = szParm;
		while (*pt != EOS)
        {
			p1= p;        /* p1 -> 2nd last char */
			p = pt;       /* p  -> last char */
			CMnext(pt);
        }
		
        if ( CMcmpcase(p,"%") || ( !CMcmpcase(p,"%") && (cb>1) && !CMcmpcase(p1,"\\") ))
        {
            if (cb < cbMax)
            {
			    CMnext(p);
                CMcpychar(" ",p);
                if (++cb < cbMax)
                {
                    CMcpychar("%",p);
                }
            }
        }
     }
     else
        MEfill (cbParmMax + cEscape - cb, ' ', szParm + cb);
    } /* endif (pdbc->db_table_name_length!=-1) */

    CatToDB_NAME_CASE(pdbc, szParm);

    return PARM_LIKE;
}


/*
**  CatParmName
**
**  Edit length of schema or table name for catalog query and ensure
**  that it is a null terminated string.
**
**  On entry: szParm -->where to return catalog value.
**            rgbParm-->parameter value.
**            cbParm  = length of parameter calue or SQL_NTS.
**
**  Returns:  PARM_INVALID if too long.
**            PARM_NULL    if NULL.
**            PARM_EQUAL otherwise.
*/
static PARM   CatParmName(
    LPDBC       pdbc,
    LPSTR       szParm,
    UCHAR  * rgbParmeter,
    OFFSET_TYPE cbParm)
{
    CHAR  * rgbParm = (CHAR*)rgbParmeter;

    if (!rgbParmeter) return PARM_NULL;

    if (cbParm == SQL_NTS)
        cbParm = (OFFSET_TYPE)STlength(rgbParm);

    /*
    **  Handle quoted identifier by ignoring quotes:
    */
    cbParm = (OFFSET_TYPE)CatUnquote (&rgbParm, (size_t)cbParm);

    if (cbParm > 32)
        return PARM_INVALID;

    MEfill (33, 0, szParm);
    MEcopy (rgbParm, (size_t)cbParm, szParm);

    CatToDB_NAME_CASE(pdbc, szParm);

    return PARM_EQUAL;
}


/*
**  CatParmForceToUser
**
**  Check the option and force the SchemaName argument 
**  to be {username, dbaname, $ingres}.
**
**  On entry: parm = original to return if option is off
**            pdbc-->connection block
**            cbParm  = length of parameter calue or SQL_NTS.
**
**  Returns:  PARM_USER or
**            original parm value.
*/
static PARM CatParmForceToUser(
    PARM        parm,
    LPDBC       pdbc,
    LPSTR       szOwner)
{
    if (!(pdbc->fOptions & OPT_CATSCHEMA_IS_NULL))
       return parm;       /* return original parm */

    if (!(*pdbc->szDBAname))           /* if DBconstants not resolved, */
       CatGetDBconstants(pdbc, NULL);  /*    do it now */

    STcopy(" in (", szOwner);

    if (!STequal(pdbc->szUsername, pdbc->szDBAname))  /* if not equal */
       {
         STcat(szOwner, "'");
         STcat(szOwner, pdbc->szUsername);
         STcat(szOwner, "',");
       }  /* end if (szUsername != szDBAname) */

    STcat(szOwner, "'");
    STcat(szOwner,      pdbc->szDBAname);
    STcat(szOwner, "'");

    if (*pdbc->szSystemOwnername)
       {
         STcat(szOwner, ",'");
         STcat(szOwner, pdbc->szSystemOwnername);
         STcat(szOwner, "'");
       }

    STcat(szOwner, ")");

    return PARM_USER;
}


/*
**  CatPrepareSqlda
**
**  Adjust SQLDA to conform to ODBC result set specification for
**  catalog functions.  Called by PrepareStmt when pstmt->fCatalog != 0.
**
**  On entry: pstmt-->statement.
**
**  On exit:  All columns defined as NULLABLE in the ODBC result set
**            have a null indicator added to the SQLDA (most of these
**            are defined as NOT NULL on the IDMS catalog).
**
**            A few columns have the data types changed so that
**            SQLDescribeCol and SQLColAttributes will return the
**            data type specified for the ODBC result set.  These
**            are set back to the real types before issuing a FETCH.
**
**  Returns:  nothing.
*/
void CatPrepareSqlda(
    LPSTMT  pstmt)
{
    LPDESC      pIRD = pstmt->pIRD;   /* Implementation Row Descriptor */

    TRACE_INTL (pstmt, "CatPrepareSqlda");

    switch (pstmt->fCatalog)
    {
    case CATLG_COLUMNS:

        CatConvertDesc(pIRD, "TABLE_CAT",              QRY_COLS_QUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_SCHEM",            QRY_COLS_OWNER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_NAME",             QRY_COLS_TABLE,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "COLUMN_NAME",            QRY_COLS_COLUMN,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "DATA_TYPE",              QRY_COLS_DATA_TYPE,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "TYPE_NAME",              QRY_COLS_TYPE_NAME,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "COLUMN_SIZE",            QRY_COLS_PRECISION,
                                          SQL_INTEGER, SQL_NULLABLE);
        CatConvertDesc(pIRD, "BUFFER_LENGTH",          QRY_COLS_LENGTH,
                                          SQL_INTEGER, SQL_NULLABLE);
        CatConvertDesc(pIRD, "DECIMAL_DIGITS",         QRY_COLS_SCALE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "NUM_PREC_RADIX",         QRY_COLS_RADIX,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "NULLABLE",               QRY_COLS_NULLABLE,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "REMARKS",                QRY_COLS_REMARKS,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "COLUMN_DEF",             QRY_COLS_DEFAULT,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "SQL_DATA_TYPE",          QRY_COLS_VERBOSETYPE,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "SQL_DATETIME_SUB",       QRY_COLS_SUBTYPE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "CHAR_OCTET_LENGTH",      QRY_COLS_OCTETLENGTH,
                                          SQL_INTEGER, SQL_NULLABLE);
        CatConvertDesc(pIRD, "ORDINAL_POSITION",       QRY_COLS_POSITION,
                                          SQL_INTEGER, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "IS_NULLABLE",            QRY_COLS_ISNULLABLE,
                                          SQL_VARCHAR, SQL_NULLABLE);

        break;

    case CATLG_FKEY:

        CatConvertDesc(pIRD, "PKTABLE_CAT",            QRY_FKEY_PKQUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PKTABLE_SCHEM",          QRY_FKEY_PKOWNER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PKTABLE_NAME",           QRY_FKEY_PKTABLE,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "PKCOLUMN_NAME",          QRY_FKEY_PKCOLUMN,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "FKTABLE_CAT",            QRY_FKEY_FKQUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "FKTABLE_SCHEM",          QRY_FKEY_FKOWNER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "FKTABLE_NAME",           QRY_FKEY_FKTABLE,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "FKCOLUMN_NAME",          QRY_FKEY_FKCOLUMN,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "KEY_SEQ",                QRY_FKEY_KEY_SEQ,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "UPDATE_RULE",            QRY_FKEY_UPDATE_RULE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "DELETE_RULE",            QRY_FKEY_DELETE_RULE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "FK_NAME",                QRY_FKEY_FK_NAME,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PK_NAME",                QRY_FKEY_PK_NAME,
                                          SQL_VARCHAR, SQL_NULLABLE);

        break;

    case CATLG_PKEY:

        CatConvertDesc(pIRD, "TABLE_CAT",              QRY_PKEY_QUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_SCHEM",            QRY_PKEY_OWNER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_NAME",             QRY_PKEY_TABLE,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "COLUMN_NAME",            QRY_PKEY_COLUMN,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "KEY_SEQ",                QRY_PKEY_KEY_SEQ,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "PK_NAME",                QRY_PKEY_PK_NAME,
                                          SQL_VARCHAR, SQL_NULLABLE);

        break;

    case CATLG_PROC:

        CatConvertDesc(pIRD, "PROCEDURE_CAT",          QRY_PROC_QUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PROCEDURE_SCHEM",        QRY_PROC_OWNER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PROCEDURE_NAME",         QRY_PROC_NAME,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "NUM_INPUT_PARAMS",       QRY_PROC_NUMIN,
                                          SQL_UNKNOWN_TYPE,SQL_NULLABLE);
        CatConvertDesc(pIRD, "NUM_OUTPUT_PARAMS",      QRY_PROC_NUMOUT,
                                          SQL_UNKNOWN_TYPE,SQL_NULLABLE);
        CatConvertDesc(pIRD, "NUM_RESULT_SETS",        QRY_PROC_NUMRES,
                                          SQL_UNKNOWN_TYPE,SQL_NULLABLE);
        CatConvertDesc(pIRD, "REMARKS",                QRY_PROC_REMARKS,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PROCEDURE_TYPE",         QRY_PROC_TYPE,
                                          SQL_SMALLINT,SQL_NULLABLE);

        break;

    case CATLG_PROCCOL:

        CatConvertDesc(pIRD, "PROCEDURE_CAT",          QRY_PROCCOL_QUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PROCEDURE_SCHEM",        QRY_PROCCOL_OWNER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PROCEDURE_NAME",         QRY_PROCCOL_PROC,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "COLUMN_NAME",            QRY_PROCCOL_COLUMN,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "COLUMN_TYPE",            QRY_PROCCOL_COLUMNTYPE,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "DATA_TYPE",              QRY_PROCCOL_DATA_TYPE,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "TYPE_NAME",              QRY_PROCCOL_TYPE_NAME,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "COLUMN_SIZE",            QRY_PROCCOL_PRECISION,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "BUFFER_LENGTH",          QRY_PROCCOL_LENGTH,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "DECIMAL_DIGITS",         QRY_PROCCOL_SCALE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "NUM_PREC_RADIX",         QRY_PROCCOL_RADIX,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "NULLABLE",               QRY_PROCCOL_NULLABLE,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "REMARKS",                QRY_PROCCOL_REMARKS,
                                          SQL_VARCHAR, SQL_NULLABLE);

        break;

    case CATLG_SPECCOL:

        CatConvertDesc(pIRD, "SCOPE",                  QRY_SPEC_SCOPE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "COLUMN_NAME",            QRY_SPEC_COLUMN,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "DATA_TYPE",              QRY_SPEC_DATA_TYPE,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "TYPE_NAME",              QRY_SPEC_TYPE_NAME,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "COLUMN_SIZE",            QRY_SPEC_PRECISION,
                                          SQL_INTEGER, SQL_NULLABLE);
        CatConvertDesc(pIRD, "BUFFER_LENGTH",          QRY_SPEC_LENGTH,
                                          SQL_INTEGER, SQL_NULLABLE);
        CatConvertDesc(pIRD, "DECIMAL_DIGITS",         QRY_SPEC_SCALE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "PSEUDO_COLUMN",          QRY_SPEC_PSEUDO,
                                          SQL_SMALLINT,SQL_NULLABLE);

        break;

    case CATLG_STATS:

        CatConvertDesc(pIRD, "TABLE_CAT",              QRY_STAT_QUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_SCHEM",            QRY_STAT_OWNER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_NAME",             QRY_STAT_TABLE,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "NON_UNIQUE",             QRY_STAT_UNIQUE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "INDEX_QUALIFIER",        QRY_STAT_IQUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "INDEX_NAME",             QRY_STAT_INDEX,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TYPE",                   QRY_STAT_TYPE,
                                          SQL_SMALLINT,SQL_NO_NULLS);
        CatConvertDesc(pIRD, "ORDINAL_POSITION",       QRY_STAT_SEQUENCE,
                                          SQL_SMALLINT,SQL_NULLABLE);
        CatConvertDesc(pIRD, "COLUMN_NAME",            QRY_STAT_COLUMN,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "ASC_OR_DESC",            QRY_STAT_COLLATION,
                                          SQL_CHAR1,   SQL_NULLABLE);
        CatConvertDesc(pIRD, "CARDINALITY",            QRY_STAT_CARD,
                                          SQL_INTEGER, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PAGES",                  QRY_STAT_PAGES,
                                          SQL_INTEGER, SQL_NULLABLE);
        CatConvertDesc(pIRD, "FILTER_CONDITION",       QRY_STAT_FILTER,
                                          SQL_VARCHAR, SQL_NULLABLE);

        break;

    case CATLG_CATALOGS:
    case CATLG_OWNERS:
    case CATLG_TABLES:

        CatConvertDesc(pIRD, "TABLE_CAT",              QRY_TABL_QUALIFIER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_SCHEM",            QRY_TABL_OWNER,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_NAME",             QRY_TABL_TABLE,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_TYPE",             QRY_TABL_TYPE,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "REMARKS",                QRY_TABL_REMARKS,
                                          SQL_VARCHAR, SQL_NULLABLE);

        break;

    case CATLG_TABLEPRIV:
        CatConvertDesc(pIRD, "TABLE_CAT",              QRY_TABLEP_TABLE_CAT,
                                          SQL_VARCHAR, SQL_NULLABLE);

        CatConvertDesc(pIRD, "TABLE_SCHEM",            QRY_TABLEP_TABLE_SCHEM,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_NAME",             QRY_TABLEP_TABLE_NAME,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "GRANTOR",             QRY_TABLEP_GRANTOR,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "GRANTEE",             QRY_TABLEP_GRANTEE,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PRIVILEGE",             QRY_TABLEP_PRIVILEGE,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "IS_GRANTABLE",             QRY_TABLEP_IS_GRANTABLE,
                                          SQL_VARCHAR, SQL_NULLABLE);
               break;

    case CATLG_COLUMNPRIV:
        CatConvertDesc(pIRD, "TABLE_CAT",              QRY_COLUMNP_TABLE_CAT,
                                          SQL_VARCHAR, SQL_NULLABLE);

        CatConvertDesc(pIRD, "TABLE_SCHEM",            QRY_COLUMNP_TABLE_SCHEM,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "TABLE_NAME",             QRY_COLUMNP_TABLE_NAME,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "COLUMN_NAME",             QRY_COLUMNP_COLUMN_NAME,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "GRANTOR",             QRY_COLUMNP_GRANTOR,
                                          SQL_VARCHAR, SQL_NO_NULLS);
        CatConvertDesc(pIRD, "GRANTEE",             QRY_COLUMNP_GRANTEE,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "PRIVILEGE",             QRY_COLUMNP_PRIVILEGE,
                                          SQL_VARCHAR, SQL_NULLABLE);
        CatConvertDesc(pIRD, "IS_GRANTABLE",             QRY_COLUMNP_IS_GRANTABLE,
                                          SQL_VARCHAR, SQL_NULLABLE);
               break;
    }
    return;
}


/*
**  CatRemove
**
**  Strip last token from SQL syntax string:
**
**  On entry: szSqlStr-->SQL syntax string.
**            szToken -->last token (not in string).
**
**  Returns:  nothing.
*/
static void   CatRemove(
    LPSTR   szSqlStr,
    LPCSTR  szToken)
{   OFFSET_TYPE offset = (OFFSET_TYPE)(STlength (szSqlStr) - STlength (szToken));

    if (STcompare(szSqlStr + offset, szToken)==0)
       *(szSqlStr + offset) = 0;
    return;
}


/*
**  CatScale
**
**  Set SCALE according to to data type.
**
**  On entry: pird -->dummy IRD column containing all we need to know.
**            pcbScale-->SCALE value in fetch buffer.
**            pfNull  -->null indicator in fetch buffer.
**
**  On exit:  SCALE is left as is if needed.
**            NULL indicator is set in fetch buffer if no SCALE.
**            SCALE is set from PRECISION if type is TIMESTAMP.
**
**  Returns:  nothing.
*/
static void   CatScale(
    LPDESCFLD    pird,
    SWORD   * pcbScale,
    SDWORD  * pfNull)
{
    switch (pird->fIngApiType)
    {
    case IIAPI_DEC_TYPE:
		break;
		
    case IIAPI_MNY_TYPE:
        *pcbScale = 2;
        break;

    case IIAPI_INT_TYPE:   /* leave scale of integer as 0 */
    case IIAPI_DTE_TYPE:   /* leave number of digits for sub-seconds as 0 */
        break;

    default:

        *pfNull = -1;
        break;
    }
    return;
}


/*
**  CatSqlsid
**
**  Set or reset SQLSID pointer in SQB if executing a catalog function.
**
**  On entry: pstmt-->statement block.
**            fOn   = TRUE  to set catalog SQLSID
**                  = FALSE to set main SQLSID
**
**  Returns:  nothing.
*/
void CatSqlsid(
    LPSTMT  pstmt,
    BOOL    set_catalog_session)
{
    LPDBC   pdbc = pstmt->pdbcOwner;

    if (pdbc->sessCat.fStatus & SESS_CONNECTED)
       { if (set_catalog_session)
            { pdbc->sqb.pSession   = &pdbc->sessCat;
            }
         else /* set main session handles */
            { pdbc->sqb.pSession   = &pdbc->sessMain;
            }
       }
    return;
}


/*
**  CatToDB_NAME_CASE
**
**  Convert catalog object name to proper case
**  depending on DB_NAME_CASE in iidbcapabilities.
**
**  Some applications (like NETCON) expect the case of table name they
**  create to be the same case to search on.  They don't expect that
**  Ingres will force the names to lower case.  Without this code
**  a SQLColumns on "LOG" would not match "log".
**
**  See description of CatParm for more info.
**
**  On entry: pdbc-->Connection block containing server class
**            s   -->string to convert to lower case.
**
**  On exit:  If serverclass= "INGRES" the tablename, columnname
**               owner, procname, etc. is lower cased.
**            Else it's left untouched.
**
**  Returns:  nothing.
*/
VOID CatToDB_NAME_CASE(
    LPDBC       pdbc,
    char  *  s)
{
    if      (pdbc->db_name_case=='L')
              CVlower(s);
    else if (pdbc->db_name_case=='U')
              CVupper(s);
}


/*
**  CatTrim
**
**  Trim trailing spaces from catalog identifiers.
**
**  This makes the identifiers name returned by the catalog functions
**  easier to concatenate into SQL syntax strings, which seems to be
**  the intent of the way the catalog function result sets are defined.
**
**  See description of CatParm for more info.
**
**  On entry: pstmt-->statement.
**            prow -->row in fetch buffer.
**            icol  = index of column.
**
**  On exit:  Varchar length in fetch buffer for column is
**            reduced by the number of trailing spaces in value.
**
**  Returns:  nothing.
*/
static void   CatTrim(
    LPSTMT      pstmt,
    CHAR  *  prow,
    UWORD       icol)
{
    LPDESCFLD   pird;     /* pird -> implementation descriptor field */
    CHAR  *  p;
    UWORD  * pcb;

    TRACE_INTL (pstmt, "CatTrim");

    pird = pstmt->pIRD->pcol + icol;  /* pird -> impl row descriptor field */
    pcb  = (UWORD*)(prow + pird->cbDataOffset); /* pcb = offset in buf*/
    
    if (pird->cbDataOffsetAdjustment)  /* if converting char-->varchar */
       {
        *pcb = (UWORD)(pird->OctetLength-2);           /* set the length fld */
       }
	
	p = (char*) pcb + 2;     /* p-> 1st char */
	*pcb = (UWORD) STtrmwhite(p);
    return;
}


/*
**  CatType
**
**  Parse type parameter for SQLTables.
**
**  On entry: pstmt  -->statement.
**            rgbType-->type parameter.
**            cbType  = length of type parameter or SQL_NTS.
**
**  Returns:  TYPE_ERROR if some error, with message in pstmt.
**            Otherwise type flags OR'd together.
*/
static UWORD   CatType(
    LPSTMT      pstmt,
    CHAR   * rgbType,
    SDWORD      cbType)
{
    int     i;
    UWORD   fType  = 0;
    LPSTR   szType = NULL;
    LPSTR   szNext;
    LPSTR   sz;
    size_t  cb;

    /*
    **  Table Type table:
    */
    static const struct
    {
        LPSTR   lpszType;           /* -->ODBC type name */
        UWORD   fType;              /* internal type flag */
    }
    TABLE_TYPE[] =
    {
     /* { "SYSTEM TABLE", TYPE_SYSTEM  }, not supported for right now */
        { "TABLE",        TYPE_TABLE   },
        { "VIEW",         TYPE_VIEW    },
        { "",             0            }
    };

    if (rgbType == NULL || *rgbType == 0)
        return TYPE_ALL;

    /*
    **  Copy type string to make it easier to parse:
    */
    cb = (cbType == SQL_NTS) ? STlength (rgbType) : (size_t)cbType;
    szType = MEreqmem((u_i2)0, cb + 2, TRUE, NULL);
    if (szType == NULL)
    {
        ErrState (SQL_HY001, pstmt, F_OD0010_IDS_PARSE_BUFFER);
        return TYPE_ERROR;
    }
    STlcopy (rgbType, szType, cb);
    CVupper (szType);

    /*
    **  Search type string and flag types found.
    */
    sz = strtok (szType, ",");
    while (sz)
    {
        cb = STlength (sz);
        szNext = sz + cb + 1;
        cb = CatUnquote (&sz, cb);
        *(sz + cb) = 0;
        for (i = 0; TABLE_TYPE[i].fType; i++)
        {
            if (STcompare (sz, TABLE_TYPE[i].lpszType) == 0)
            {
                fType |= TABLE_TYPE[i].fType;
                break;
            }
        }
        sz = strtok (szNext, ",");
    }
    MEfree (szType);
    return fType;
}


/*
**  CatUnquote
**
**  Find and "remove" leading and trailing spaces and quotes for removal.
**
**  On entry: psz-->address of string.
**            cb  = length of string.
**
**  On exit:  *psz = address of string sans leading spaces and quote.
**
**  Returns:  length of string sans trailing quote and spaces.
**
**  Note:     cannot assume string is null terminated.
*/
static size_t   CatUnquote (
    CHAR   *  * psz,
    size_t cb)
{
    LPSTR  sz = *psz;
    char * pLastQuote;
    char szTemp[256];
    i4   bytecnt;

    MEfill (sizeof(szTemp), 0, szTemp);
    /*
    **  Skip leading junk:
    */
    while ((cb > 0) && CMwhite(sz))
    {
        bytecnt = CMbytecnt(sz);
        CMnext(sz);
        cb = cb - bytecnt;
    }
    if ((cb > 0) && !CMcmpcase(sz,"'"))
    {
        CMnext(sz);
        cb--;
    }
	MEcopy (sz, cb, szTemp);
    *psz = sz;

	cb = STtrmwhite(szTemp);
    pLastQuote = STrindex(szTemp,"'",0);
	if (pLastQuote != NULL)
    {
		CMcpychar("\0",pLastQuote);
		cb--;
    }

	/* Strip off the double quote for bug 118193*/
	if ((cb > 0) && !CMcmpcase(sz,"\""))
    {
        CMnext(sz);
        cb--;
    }
	MEcopy (sz, cb, szTemp);
    *psz = sz;
    pLastQuote = STrindex(szTemp,"\"",0);
	if (pLastQuote != NULL)
    {
		CMcpychar("\0",pLastQuote);
		cb--;
    }
	
    return cb;
}


/*
**  CatWhere
**
**  Add WHERE clause to SQL syntax string.
**
**  On entry: szSqlStr-->SQL syntax string.
**            parm     =  predicate type flag.
**            fColumn  = string id of column name.
**            szValue -->column value string.
**            cEscape  = ESCAPE character value, if any.
**
**  Returns:  nothing.
*/
static void   CatWhere (
    LPSTR   szSqlStr,
    PARM    parm,
    LPCSTR  szColumn,
    LPCSTR  szValue,
    CHAR    bEscape)
{
    CHAR    szEscape[5] = {'\'',       0, '\'', ' ', 0};
            szEscape[1] =        bEscape;

    switch (parm)
    {
    case PARM_EMPTY:
    case PARM_EQUAL:

        STcat (szSqlStr, (char *) szColumn);
        STcat (szSqlStr, (char *) EQUAL);
        STcat (szSqlStr, (char *) SQUOTE);
        STcat (szSqlStr, (char *) szValue);
        STcat (szSqlStr, (char *) SQUOTE);
        break;

    case PARM_LIKE:

        STcat (szSqlStr, (char *) szColumn);
        STcat (szSqlStr, (char *) LIKE);
        STcat (szSqlStr, (char *) SQUOTE);
        STcat (szSqlStr, (char *) szValue);
        STcat (szSqlStr, (char *) SQUOTE);
        if (bEscape)
        {
            STcat (szSqlStr, (char *) ESCAPE);
            STcat (szSqlStr, szEscape);
        }
        break;

    case PARM_USER:

        STcat (szSqlStr, (char *) szColumn);
        STcat (szSqlStr, (char*)  szValue);
        break;

    default:

        return;
    }
    STcat (szSqlStr, (char *) AND);
    return;
}

/*
**  CatWhereStr
**
**  Add WHERE clause to SQL syntax string.
**
**  On entry: szSqlStr-->SQL syntax string.
**            parm     =  predicate type flag.
**            fColumn  = string id of column name.
**            szValue -->column value string.
**            cEscape  = ESCAPE character value, if any.
**
**  Returns:  nothing.
*/
static void   CatWhereStr (
    LPSTR   szSqlStr,
    PARM    parm,
    LPCSTR  szColumn,
    LPCSTR  szValue,
    CHAR    bEscape)
{
    CHAR    szEscape[5] = {'\'',       0, '\'', ' ', 0};
            szEscape[1] =        bEscape;

    switch (parm)
    {
    case PARM_EMPTY:
    case PARM_EQUAL:

        STcat (szSqlStr, (char *) szColumn);
        STcat (szSqlStr, (char *) EQUAL);
        STcat (szSqlStr, (char *) SQUOTE);
        STcat (szSqlStr, (char *) szValue);
        STcat (szSqlStr, (char *) SQUOTE);
        break;

    case PARM_LIKE:

        STcat (szSqlStr, (char *) szColumn);
        STcat (szSqlStr, (char *) LIKE);
        STcat (szSqlStr, (char *) SQUOTE);
        STcat (szSqlStr, (char *) szValue);
        STcat (szSqlStr, (char *) SQUOTE);
        if (bEscape)
        {
            STcat (szSqlStr, (char *) ESCAPE);
            STcat (szSqlStr, (char *) szEscape);
        }
        break;

    default:

        return;
    }
    STcat (szSqlStr, (char *) AND);
    return;
}

/*
**  CatQueryOpen
**
**  Open an API query.
**  
**  Note:  No column may be longer than OBJECT_NAME_SIZE
**         since that is what is allocated for column buffer.
**
**  On entry: pstmt   -->STMT block.
**            pqueryParm-->API queryParm.
**            szSqlStr-->SQL syntax string.
**            
**  Returns:  TRUE (success) or FALSE (failure).
**
*/
BOOL   CatQueryOpen (
    LPSTMT              pstmt,
    IIAPI_QUERYPARM *   pqueryParm,
    LPSTR               szSqlStr)
{
    /*
    ** IIapi_query("SELECT ...").
    */
    pqueryParm->qy_genParm.gp_callback = NULL;
    pqueryParm->qy_genParm.gp_closure  = NULL;
    pqueryParm->qy_connHandle          = pstmt->psess->connHandle;
    pqueryParm->qy_queryType           = IIAPI_QT_QUERY;
	pqueryParm->qy_queryText           =  szSqlStr; 
    pqueryParm->qy_parameters          = FALSE;  
    pqueryParm->qy_tranHandle          = pstmt->psess->tranHandle;
    pqueryParm->qy_stmtHandle          = NULL;
    pqueryParm->qy_flags = 0;
 
    IIapi_query( pqueryParm );
    pstmt->psess->tranHandle = pqueryParm->qy_tranHandle;

    if (!odbc_getResult( &pqueryParm->qy_genParm, &pstmt->sqlca, (II_LONG)pstmt->cQueryTimeout ))
    {   /* wait for completion and get any errors */ 
        return (FALSE);
    }
    return(TRUE);	
}

/*
**  CatQueryDescriptor
**
**  Get the descriptor for an opened API query.
**
**  On entry: pstmt   -->STMT block.
**            pqueryParm-->API queryParm.
**            pgetDescrParm-->API Descriptor parm.
**            
**  Returns:  TRUE (success) or FALSE (failure).
**
*/
static BOOL   CatQueryDescriptor (
    LPSTMT              pstmt,
    IIAPI_QUERYPARM   * pqueryParm,
    IIAPI_GETDESCRPARM* pgetDescrParm)
{
    /*
    ** IIapi_getDescriptor().
    */
    pgetDescrParm->gd_genParm.gp_callback = NULL;
    pgetDescrParm->gd_genParm.gp_closure = NULL;
    pgetDescrParm->gd_stmtHandle = pqueryParm->qy_stmtHandle;
    pgetDescrParm->gd_descriptorCount = 0;
    pgetDescrParm->gd_descriptor = NULL;

    IIapi_getDescriptor( pgetDescrParm );
    
    if (!odbc_getResult( &pgetDescrParm->gd_genParm, &pstmt->sqlca, (II_LONG)pstmt->cQueryTimeout ))
        return (FALSE);   /* wait for completion and get any errors */ 
   
    return(TRUE);	
}

/*
**  CatQueryGetColumns
**
**  Get the descriptor for an opened API query.
**
**  On entry: pstmt   -->STMT block.
**            pqueryParm-->API queryParm.
**            pgetDescrParm-->API Descriptor parm.
**            
**  Returns:  TRUE (success) or FALSE (failure).
**
*/
static BOOL   CatQueryGetColumns (
    LPSTMT              pstmt,
    IIAPI_QUERYPARM   * pqueryParm,
    IIAPI_GETDESCRPARM* pgetDescrParm,
    IIAPI_GETCOLPARM  * pgetColParm)
{
    II_INT2             ColumnCount, i;
    IIAPI_DESCRIPTOR   *pDescriptor;

    if (!pgetColParm->gc_rowCount)  /* if 1st time, initialize */
       {
	    /*
        ** IIapi_getColumns() Initialization.
        */
        pgetColParm->gc_genParm.gp_callback = NULL;
        pgetColParm->gc_genParm.gp_closure = NULL;
        pgetColParm->gc_rowCount = 1;
        pgetColParm->gc_columnCount = 
           ColumnCount = pgetDescrParm->gd_descriptorCount;
        pgetColParm->gc_columnData = ( IIAPI_DATAVALUE * )
               odbc_malloc(sizeof(IIAPI_DATAVALUE) * ColumnCount);

        pDescriptor=pgetDescrParm->gd_descriptor;   /* --> descriptor array */

        for (i=0; i<ColumnCount; i++, pDescriptor++)  /* allocate value buffers */
            {
             pgetColParm->gc_columnData[i].dv_value 
                                 = odbc_malloc(pDescriptor->ds_length + 3);
                             /* add 3 for null term and length conversions */
            }  /* end for loop allocating value buffers */

        pgetColParm->gc_stmtHandle   = pqueryParm->qy_stmtHandle;
        pgetColParm->gc_moreSegments = 0;
        }  /* end if (!pgetColParm->gc_rowCount)  (end if 1st time) */
    
    IIapi_getColumns( pgetColParm );

	if (!odbc_getResult( &pgetColParm->gc_genParm, &pstmt->sqlca, (II_LONG)pstmt->cQueryTimeout))
        return (FALSE);   /* wait for completion and get any errors */ 


    return(TRUE);
}

/*
**  CatQueryClose
**
**  Close an opened API query.
**
**  On entry: pstmt   -->STMT block.
**            pqueryParm-->API queryParm.
**            pgetDescrParm-->API Descriptor parm.
**            
**  Returns:  TRUE (success) or FALSE (failure).
**
*/
static BOOL   CatQueryClose (
    LPSTMT              pstmt,
    IIAPI_QUERYPARM   * pqueryParm,
    IIAPI_GETDESCRPARM* pgetDescrParm,
    IIAPI_GETCOLPARM  * pgetColParm)
{
    II_INT2             ColumnCount, i;
    II_PTR              stmtSave;

    stmtSave = pstmt->stmtHandle;
	pstmt->stmtHandle = pqueryParm->qy_stmtHandle;
	odbc_getQInfo( pstmt );  /* get any additional error or response data */
    odbc_close( pstmt );     /* close the query */
   	pstmt->stmtHandle = stmtSave;
    

    if (pgetColParm && pgetColParm->gc_rowCount)  /* if never initialized, skip it */
       {
        ColumnCount = pgetDescrParm->gd_descriptorCount;
        if (pgetColParm->gc_columnData)  /* if value buffers, free them */
           {
            for (i=0; i<ColumnCount; i++)  /* free value buffers */
                {if (pgetColParm->gc_columnData[i].dv_value)
                     odbc_free(pgetColParm->gc_columnData[i].dv_value); 
                }  /* end for loop freeing value buffers */
            odbc_free(pgetColParm->gc_columnData);  /* free array */
            pgetColParm->gc_columnData=NULL;
           }  /* end if (pgetColParm->gc_columnData) */

        pgetColParm->gc_rowCount=0;
       }  /* end if (!pgetColParm->gc_rowCount) */
    
    return(TRUE);
}

/*
**  CatGetDBcapabilities
**
**  Get DB_NAME_CASE and name lengths from iicapabilities tables.
**
**  On entry: pdbc     -->DBC  block. 
**            pstmtparm-->STMT block (may be NULL).
**            
**  Returns:  TRUE (success) or FALSE (failure).
**
*/
BOOL CatGetDBcapabilities(LPDBC pdbc, LPSTMT pstmtparm)
{   LPSTMT pstmt;
    IIAPI_QUERYPARM     queryParm={0};
    IIAPI_GETDESCRPARM  getDescrParm={0};
    IIAPI_GETCOLPARM    getColParm={0};
    IIAPI_GETCOLPARM  *pgetColParm=NULL;
    BOOL                rc=TRUE;
    UWORD              *pmaxlength=NULL;
    CHAR               *p;        /* work ptr --> cap_capability */
    CHAR               *q;        /* work ptr --> cap_value      */
    CHAR               *szSqlStr=
      "select cap_capability, cap_value from iidbcapabilities "
      "where cap_capability = 'DB_NAME_CASE'  or "
            "cap_capability = 'DB_DELIMITED_CASE' or "
            "cap_capability = 'MAX_COLUMNS' or "
            "cap_capability = 'SQL_MAX_SCHEMA_NAME_LEN' or "
            "cap_capability = 'SQL_MAX_TABLE_NAME_LEN' or "
            "cap_capability = 'SQL_MAX_COLUMN_NAME_LEN' or "
            "cap_capability = 'SQL_MAX_CHAR_COLUMN_LEN' or "
            "cap_capability = 'SQL_MAX_VCHR_COLUMN_LEN' or "
            "cap_capability = 'SQL_MAX_BYTE_COLUMN_LEN' or "
            "cap_capability = 'SQL_MAX_VBYT_COLUMN_LEN' or "
            "cap_capability = 'SQL_MAX_ROW_LEN'";

    if (pdbc->db_name_case)  /* if already resolved, just return */
       return(TRUE);

    if ((pstmt=pstmtparm)==NULL)  /* if no stmt handle then allocate one */
       if (AllocStmt(pdbc, &pstmt)!=SQL_SUCCESS)
          return(FALSE);          /* if error, return FALSE  */

    if (isServerClassINGRES(pdbc))
         pdbc->db_name_case='L';    /* take a "LOWER" or "MIXED" */
    else pdbc->db_name_case='M';    /* guess in case things go horribly wrong */
    pdbc->db_delimited_case='M';

    if (    (rc=CatQueryOpen      (pstmt, &queryParm, szSqlStr))==TRUE)
        if ((rc=CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))==TRUE)
             rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, 
                                                    pgetColParm=&getColParm);
    
    while (rc && getColParm.gc_genParm.gp_status != IIAPI_ST_NO_DATA)
                                   /* if no errors and a row found - success! */
       {p=(CHAR*)getColParm.gc_columnData[0].dv_value;  /* p->cap_capability */
        if (getDescrParm.gd_descriptor[0].ds_dataType == IIAPI_VCH_TYPE)
            p=p+2;                         /* if VARCHAR, skip past 2-byte length */
        
        q=(CHAR*)getColParm.gc_columnData[1].dv_value;  /* q->cap_value */
        if (getDescrParm.gd_descriptor[1].ds_dataType == IIAPI_VCH_TYPE)
            q=q+2;                         /* if VARCHAR, skip past 2-byte length */
        
        if      (MEcmp(p,"DB_NAME_CASE",12)==0)
           {if (*q=='L'  ||  *q=='U'  ||  *q=='M')
               pdbc->db_name_case=*q;
           }
        else if (MEcmp(p,"DB_DELIMITED_CASE",17)==0)
           {if (*q=='L'  ||  *q=='U'  ||  *q=='M')
               pdbc->db_delimited_case=*q;
           }
        else if (MEcmp(p,"MAX_COLUMNS",11)==0)
               pmaxlength=&pdbc->max_columns_in_table;
        else if (MEcmp(p,"SQL_MAX_SCHEMA_NAME_LEN",23)==0)
               pmaxlength=&pdbc->max_schema_name_length;
        else if (MEcmp(p,"SQL_MAX_TABLE_NAME_LEN",22)==0)
               pmaxlength=&pdbc->max_table_name_length;
        else if (MEcmp(p,"SQL_MAX_COLUMN_NAME_LEN",23)==0)
               pmaxlength=&pdbc->max_column_name_length;
        else if (memcmp(p,"SQL_MAX_ROW_LEN",15)==0)
            pmaxlength=&pdbc->max_row_length;
        else if (memcmp(p,"SQL_MAX_CHAR_COLUMN_LEN",23)==0)
            pmaxlength=&pdbc->max_char_column_length;
        else if (memcmp(p,"SQL_MAX_VCHR_COLUMN_LEN",23)==0)
            pmaxlength=&pdbc->max_vchr_column_length;
        else if (memcmp(p,"SQL_MAX_BYTE_COLUMN_LEN",23)==0)
            pmaxlength=&pdbc->max_byte_column_length;
        else if (memcmp(p,"SQL_MAX_VBYT_COLUMN_LEN",23)==0)
            pmaxlength=&pdbc->max_vbyt_column_length;
        if (pmaxlength)
           {*pmaxlength=atoi(q);
            pmaxlength=NULL; 
           }

        rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, pgetColParm);
       } /* endif row found! */

    CatQueryClose(pstmt, &queryParm, &getDescrParm, pgetColParm);  
                                  /* close and free up */

    if (pstmtparm==NULL)          /* if stmt was allocated */
       FreeStmt(pstmt, SQL_DROP);  /* free it */
    return(rc);
}

/*
**  CatGetDBconstants
**
**  Get dba_name, system_owner from iidbconstants table.
**  This info is needed and this function is only invoked 
**  for procedure parameter calls to avoid unnecessary
**  queries.
**
**  On entry: pdbc     -->DBC  block. 
**            pstmtparm-->STMT block (may be NULL).
**            
**  Returns:  TRUE (success) or FALSE (failure).
**
*/
BOOL CatGetDBconstants(LPDBC pdbc, LPSTMT pstmtparm)
{   LPSTMT pstmt;
    IIAPI_QUERYPARM     queryParm={0};
    IIAPI_GETDESCRPARM  getDescrParm={0};
    IIAPI_GETCOLPARM    getColParm={0};
    IIAPI_GETCOLPARM  *pgetColParm=NULL;
    RETCODE             rc=TRUE;
    II_INT2             len;
    BOOL                isGateway;
    int                *pmaxlength=NULL;
    char               *p;        /* work ptr --> cap_capability */
    char               *q;        /* work ptr --> cap_value      */
    char               *szSqlStr;
    char               *szSqlStrIngres=
                            "select user_name, dba_name, system_owner "
                                 "from iidbconstants";
    char               *szSqlStrGateway= /* IMS and VSAM don't have sysowner */
                            "select user_name, dba_name, dba_name "
                                 "from iidbconstants";

    if (*pdbc->szDBAname)  /* if already resolved, just return */
       return(TRUE);

    if (isServerClassIMS(pdbc)  ||  isServerClassVSAM(pdbc))
       {  /* IMS and VSAM don't have iidbconstants(system_owner) */
        isGateway = TRUE;
        szSqlStr = szSqlStrGateway;
       }
    else  /* server supports iidbconstants(system_owner) */
       {
        isGateway = FALSE;
        szSqlStr = szSqlStrIngres;
       }  /* end if IMS or VSAM */

    STcopy("$ingres",        pdbc->szSystemOwnername);  /* defaults in case */
    STcopy(pdbc->szUsername, pdbc->szDBAname);       /* something goes wrong*/


    if ((pstmt=pstmtparm)==NULL)  /* if no stmt handle then allocate one */
       if (AllocStmt(pdbc, &pstmt)!=SQL_SUCCESS)
          return(FALSE);          /* if error, return FALSE  */

    if (    (rc=CatQueryOpen      (pstmt, &queryParm, szSqlStr))==TRUE)
        if ((rc=CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))==TRUE)
             rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, 
                                                    pgetColParm=&getColParm);
    
    while (rc && getColParm.gc_genParm.gp_status != IIAPI_ST_NO_DATA)
                                  /* if no errors and a row found - success! */
       {p   = getColParm.gc_columnData[0].dv_value;  /* p->user_name */
        len = getColParm.gc_columnData[0].dv_length;
        if (getDescrParm.gd_descriptor[0].ds_dataType == IIAPI_VCH_TYPE)
           {len = *((II_INT2*)p);
            p=p+2;                    /* if VARCHAR, skip past 2-byte length */
           }
        for (q=p + len - 1; len && (*q-- == ' '); len--); /* back trim spaces */
        if (len > 32)  len = 32;  /* safety check */
        p[len] = '\0';
        if (len)
           memcpy(pdbc->szUsername, p, len+1);  /* copy name and \0 */

        p   = getColParm.gc_columnData[1].dv_value;  /* p->dba_name */
        len = getColParm.gc_columnData[1].dv_length;
        if (getDescrParm.gd_descriptor[1].ds_dataType == IIAPI_VCH_TYPE)
           {len = *((II_INT2*)p);
            p=p+2;                    /* if VARCHAR, skip past 2-byte length */
           }
        for (q=p + len - 1; len && (*q-- == ' '); len--); /* back trim spaces */
        if (len > 32)  len = 32;  /* safety check */
        p[len] = '\0';
        if (len)
           memcpy(pdbc->szDBAname, p, len+1);  /* copy name and \0 */

        p   = getColParm.gc_columnData[2].dv_value;  /* p->system_owner */
        len = getColParm.gc_columnData[2].dv_length;
        if (getDescrParm.gd_descriptor[2].ds_dataType == IIAPI_VCH_TYPE)
           {len = *((II_INT2*)p);
            p=p+2;                    /* if VARCHAR, skip past 2-byte length */
           }
        for (q=p + len - 1; len && (*q-- == ' '); len--); /* back trim spaces */
        if (len > 32)  len = 32;  /* safety check */
        p[len] = '\0';
        if (!isGateway  && len)  /* skip if no iidbconstants(system_owner) */
           memcpy(pdbc->szSystemOwnername, p, len+1);   /* copy name and \0 */

        rc=CatQueryGetColumns(pstmt, &queryParm, &getDescrParm, pgetColParm);
       } /* endif row found! */

    CatQueryClose(pstmt, &queryParm, &getDescrParm, pgetColParm);  
                                  /* close and free up */

    if (pstmtparm==NULL)          /* if stmt was allocated */
       FreeStmt(pstmt, SQL_DROP);  /* free it */
    return(rc);
}

/*
**  CatGetTableNameLength
**
**  Get length of table name and other names.
**  (The length may be 32 in OI, 24 in Ingres 6.4,
**   18 in DB2, or even a varchar
**
**  On entry: pdbc     -->DBC  block. 
**            pstmtparm-->STMT block (may be NULL and usually is).
**
**            pdbc->max_table_name_length, max_column_name_length,
**              and/or max_schema_name_length may have been resolved
**              already by previous calls or by CatGetDBcapabilities
**            
**  Returns:  -1 if varchar, or length or table_name char(n)
**            Fill in length (in DBC) of:
**                 max_table_name_length
**                 max_schema_name_length
**                 max_column_name_length
**
*/
int CatGetTableNameLength(LPDBC pdbc, LPSTMT pstmtparm)
{   LPSTMT pstmt;
    IIAPI_QUERYPARM     queryParm={0};
    IIAPI_GETDESCRPARM  getDescrParm={0};
    IIAPI_GETCOLPARM    getColParm={0};
    IIAPI_GETCOLPARM  *pgetColParm=NULL;
    BOOL                rc=TRUE;
    int                 table_len=24;
    int                 owner_len=24;
    int                 column_len=0;
    CHAR               *szSqlStr=
      "select table_owner, table_name from iitables where table_name = 'ii+odbc'";
            /* select will return zero rows but we'll get the descriptor
               which is all that we really want or need */

    if (pdbc->db_table_name_length   &&  /* if all lengths already resolved, */
        pdbc->max_table_name_length  &&  /*    just return */
        pdbc->max_schema_name_length &&
        pdbc->max_column_name_length)
           return(pdbc->db_table_name_length);

    do       /* convenient loop to break out of */
    {
    if (isServerClassINGRES(pdbc))               /* take a guess */
       {if (isOld6xRelease(pdbc))                /*  in case things go horribly wrong */
             {table_len=owner_len=24;}  /* 6.4 */
        else {table_len=owner_len=32;   /* OpenIngres, Ingres II, and later */
              break;                    /* skip the query, we know it's 32 */
             }
       }
    else if (isServerClassDB2(pdbc))
             {table_len=18; owner_len=8;                 break;}
    else if (isServerClassDCOM(pdbc))
             {table_len=32; owner_len=32;                break;}
    else if (isServerClassIDMS(pdbc))
             {table_len=18; owner_len=18; column_len=32; break;}
    else if (isServerClassIMS(pdbc)  && isOld6xRelease(pdbc))
             {table_len=24; owner_len=24;                break;}
    else if (isServerClassVSAM(pdbc) && isOld6xRelease(pdbc))
             {table_len=24; owner_len=24;                break;}
    else if (isServerClassORACLE(pdbc))
             {table_len=32; owner_len=32;}
    else if (isServerClassRDB(pdbc))
             {table_len=20; owner_len=20;}
    else if (isServerClassINFORMIX(pdbc))
             {table_len=18; owner_len=18;}
    else     {table_len=30; owner_len=30;} /* guess for other gateways */

    if ((pstmt=pstmtparm)==NULL)  /* if no stmt handle then allocate one */
       if (AllocStmt(pdbc, &pstmt)!=SQL_SUCCESS)
          break;                  /* if error, just return default  */

    if (    (rc=CatQueryOpen      (pstmt, &queryParm, szSqlStr))==TRUE)
        if ((rc=CatQueryDescriptor(pstmt, &queryParm, &getDescrParm))==TRUE  &&
                                           getDescrParm.gd_descriptorCount)
           {owner_len=getDescrParm.gd_descriptor[0].ds_length;
            table_len=getDescrParm.gd_descriptor[1].ds_length;
            if (getDescrParm.gd_descriptor[0].ds_dataType == IIAPI_VCH_TYPE)
              {owner_len -= 2;                 /* strip length indicator */
              }
            if (getDescrParm.gd_descriptor[1].ds_dataType == IIAPI_VCH_TYPE)
              {pdbc->db_table_name_length= -1; /* if VARCHAR, return -1 */
               table_len -= 2;
              }
           }
        
    CatQueryClose(pstmt, &queryParm, &getDescrParm, pgetColParm);  
                                  /* close and free up */

    if (pstmtparm==NULL)          /* if stmt was allocated */
       FreeStmt(pstmt, SQL_DROP);  /* free it */
    } while(0); /* end convenient loop */

    /* resolve the lengths that have not been filled in yet */
    if (!column_len)  /* if no explicit column length, assume table_len */
         column_len                   = table_len;
    if (!pdbc->db_table_name_length)
         pdbc->db_table_name_length   = table_len;
    if (!pdbc->max_table_name_length)
         pdbc->max_table_name_length  = table_len;
    if (!pdbc->max_column_name_length)
         pdbc->max_column_name_length = column_len;
    if (!pdbc->max_schema_name_length)
         pdbc->max_schema_name_length = owner_len;

    return(pdbc->db_table_name_length);

}
