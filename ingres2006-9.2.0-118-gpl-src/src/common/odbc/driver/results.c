/*
** Copyright (c) 1992, 2006 Ingres Corporation
**
*/

#include <compat.h>
#ifndef VMS
#include <systypes.h>
#endif
#include <sql.h>                    /* ODBC Core definitions */
#include <sqlext.h>                 /* ODBC extensions */
#include <sqlstate.h>               /* ODBC driver sqlstate codes */
#include <sqlcmd.h>

#include <gl.h>
#include <iicommon.h>
#include <clfloat.h>
#include <cm.h>
#include <cv.h>
#include <me.h>
#include <nm.h> 
#include <st.h> 
#include <tr.h>
#include <er.h>
#include <erodbc.h>

#include <iicommon.h>
#include <gl.h>
#include <iiapi.h>

#include <sqlca.h>
#include <idmsxsq.h>                /* control block typedefs */
#include <idmsutil.h>               /* utility stuff */
#include "caidoopt.h"              /* connect options */
#include "idmsoinf.h"               /* information definitions */
#include "idmsoids.h"               /* string ids */
#include <idmseini.h>               /* ini file keys */
#include "idmsodbc.h"               /* ODBC driver definitions             */

#include <idmsocvt.h>               /* ODBC conversion routines */

/*
** Name: RESULTS.C
**
** Description:
**	Result routines for ODBC driver.
**
** History:
**	01-dec-1992 (rosda01)
**	    Initial coding
**	01-feb-1993 (rosda01)
**	    Conversion routines
**	03-may-1993 (rosda01)
**	    Removed error routines to OERR.
**	14-mar-1995 (rosda01)
**	    ODBC 2.0 upgrade...
**	16-jan-1997 (tenje01)
**	    put current pstmt in sqb before calling sql
**	28-jan-1997 (tenje01)
**	    GetColumn ...
**	29-jan-1997 (tenje01)
**	    SQLColAttributes ...
**	03-feb-1997 (tenje01)
**	    DescType ... 
**	03-feb-1997 (tenje01)
**	    support SQL_VARBINARY,SQL_TINYINT in GetColumn
**	03-feb-1997 (tenje01)
**	    DescTypeName ...
**	03-feb-1997 (tenje01)
**	    Commented out FakeType. Ingres does not need it.
**	03-feb-1997 (tenje01)
**	    DescScale ... 
**	04-feb-1997 (tenje01)
**	    added CvtIngresDateToChar ... 
**	04-feb-1997 (tenje01)
**	    DescPrecision ...
**	05-feb-1997 (tenje01)
**	    GetColumn ...
**	13-feb-1997 (tenje01)
**	    CVT_TRUNCATION if col has more segs to fetch
**	14-feb-1997 (tenje01)
**	    SQLGetData... skip subsequent segs of prev col if any
**	21-feb-1997 (tenje01)
**	    Removed jdt.h, chgd MAX_COLUMNS_IDMS 
**	24-feb-1997 (tenje01)
**	    Added code for SQLNumParams
**	26-feb-1997 (tenje01)
**	    skip GetColumn if col is bound long 
**	27-feb-1997 (tenje01)
**	    Added code for calling a procedure
**	12-mar-1997 (tenje01)
**	    Added a fourth parm to GetColumn
**	19-mar-1997 (tenje01)
**	    Called api in CvtIngresDatetoChar
**	24-mar-1997 (thoda04)
**	    Bug fix to GetColumn and SQL_22002
**	07-apr-1997 (tenje01)
**	    reset icolFetch only when calling api to fetch
**	09-apr-1997 (tenje01)
**	    bug fix to fetching long data objects 
**	17-apr-1997 (thoda04)
**	    Bug fix to GetColumn and null indicator test
**	28-apr-1997 (thoda04)
**	    Clean up 16-bit warnings
**	04-may-1997 (thoda04)
**	    Bug fix to null indicator test
**	29-jul-1997 (thoda04)
**	    Bug fix to SQLColAttributes(SQL_COLUMN_LABEL)
**	05-sep-1997 (thoda04)
**	    Bug fix to SQLColAttributes(SQL_COLUMN_TYPE_NAME)
**	15-sep-1997 (tenje01)
**	    fixed blank date and added IDS_ERR_DT_INTERVALS 
**	06-oct-1997 (thoda04)
**	    SQLRowCount should return -1 if count unknown
**	12-nov-1997 (thoda04)
**	    Documentation clean-up
**	21-nov-1997 (thoda04)
**	    Ingres does not support piggyback COMMIT
**	05-dec-1997 (tenje01)
**	    conver C run-time functions to CL
**	09-jan-1998 (tenje01)
**	    DescPrecision, set precision for money data type 
**	14-jan-1998 (tenje01)
**	    DescScale, set scale for money data type
**	05-jan-1998 (thoda04)
**	    If GetCharChar has 0 length buffer, raise truncation err
**	14-jan-1998 (thoda04)
**	    Return correct precision and scale for MONEY data type
**	06-feb-1998 (thoda04)
**	    Fixed sizes from 16-bit to 32-bit
**	20-feb-1998 (thoda04)
**	    Convert date format to ODBC format despite II_DATE_FORMAT.
**	04-mar-1998 (thoda04)
**	    If no time in date append 00:00:00
**	09-mar-1998 (thoda04)
**	    Allocate null term byte for buffer in CvtIngresDateToChar
**	09-apr-1998 (thoda04)
**	    Fixed DescName to allow embedded spaces
**	20-may-1998 (thoda04)
**	    Strip trailing '.' for float->char conversions
**	26-may-1998 (thoda04)
**	    don't do SQLGetData fetch if an internal constant query
**	30-jun-1998 (thoda04)
**	    Use 7 digits for SQL_REAL/SQL_FLOAT -> char conversions
**	24-jul-1998 (thoda04)
**	    Check SQLGetData stays in bounds for pstmt->icolPrev
**	30-nov-1998 (thoda04)
**	    Rewrite of SQLPrepare/SQLExecute sequence.
**	14-apr-1999 (padni01)
**	    In GetCharChar(), change strncpy to memcpy 
**	    to allow for embedded nulls
**	22-apr-1999 (thoda04)
**	    Add prepare_or_describe parm to :.
**	15-nov-1999 (thoda04)
**	    Added option to return blank date as NULL.
**	12-jan-2000 (thoda04)
**	    Added option to return date 1582-01-01 as NULL.
**	10-mar-2000 (loera01)
**	    Bug 97372:  Added new internal conversion
**	    function CvtDoubleToChar() to use as the
**	    default instead of _gcvt(). 
**	11-may-2000 (loera01)
**	    Bug 97488:  For SQLNumResultColumns(), add
**	    a check for the current statement context.
**	12-may-2000 (thoda04)
**	    Increased size of szValue work field.
**	27-jun-2000 (thoda04)
**	    Get blobs with embedded null term characters
**	08-sep-2000 (thoda04)
**	    Blobs are SQL_COLUMN_SEARCHABLE=SQL_UNSEARCHABLE
**	16-oct-2000 (loera01)
**	    Added support for II_DECIMAL (Bug 102666).
**	07-nov-2000 (thoda04)
**	    Convert nat's to i4
**	11-jul-2001 (thoda04)
**	    Correct SQLGetData length for char<-->wchar conversions
**	13-jul-2001 (somsa01)
**	    Cleaned up compiler warnings.
**	16-jul-2001 (thoda04)
**	    Add support for SQL_WLONGVARCHAR (Unicode blobs)
**	23-jul-2001 (thoda04)
**	    Return SQLDescribeCol ColumnSize of 15,15, and 7
**	    for SQL_FLOAT, SQL_DOUBLE, and SQL_REAL (Bug 105316).
**	31-jul-2001 (somsa01)
**	    Add support for parameter offset binding
**	31-jul-2001 (thoda04)
**	    Add support for parameter offset binding
**	13-aug-2001 (thoda04)
**	    Add support for LONGDISPLAY option.
**	29-oct-2001 (thoda04)
**	    Corrections to 64-bit compiler warnings change.
**	11-dec-2001 (thoda04)
**	    Additional 64-bit changes for overloaded attributes.
**	11-mar-2002 (thoda04)
**	    Add support for row-producing procedures.
**	01-apr-2002 (thoda04)
**	    If SQLBindCol does ReallocDescriptor(ARD), it also needs to do IRD.
**      18-jul-2002 (loera01) Bug 108301
**          In GetColumn(), utilize LONGBUFFER structure from the ARD instead
**          of the statement control block.
**      19-aug-2002 (loera01) Bug 108536
**          First phase of support for SQL_C_NUMERIC.
**      04-nov-2002 (loera01) Bug 108536
**          Second phase of support for SQL_C_NUMERIC.  Added coercions for
**          all supported datatypes besides SQL_NUMERIC and SQL_DECIMAL.
**     20-dec-2002 (loera01) Bug 108536
**          Adjusted string length if the input value was decimal and the
**          output was a string.
**      14-feb-2003 (loera01) SIR 109643
**          Minimize platform-dependency in code.  Make backward-compatible
**          with earlier Ingres versions.
**      21-mar-2003(weife01) Bug 109890, 109891
**          GetCharWChar now return CVT_TRANCATION to signal more segment of
**          BLOB to be retrieved by SQLGetData; GetCharWChar now properly
**          get the size of ASCII character to fill the unicode buffer for 
**          SQLGetData. 
**      29-may-2003 (loera01) 
**          If the options field in the connection block has OPT_DECIMALPT 
**          set, supply a comma for the decimal character.
**     25-jun-2003 (loera01) Bug 110475
**          In GetColumns(), when coercing SQL_NUMERIC into SQL_C_LONG,
**          reference szValue instead or rgbData.
**     22-sep-2003 (loera01)
**          Added internal ODBC tracing.
**     17-oct-2003 (loera01)
**          Changed "IIsyc_" routines to "odbc_".
**     05-nov-2003 (loera01)
**          Removed calls to PrepareStmt().  This is now handled
**          in odbc_setDescriptor().
**    13-nov-2003 (loera01)
**          Removed obsolete arguments from SqlToIngesAPI().
**    04-dec-2003 (loera01)  SIR 111408
**          Added the capability to fetch "true" segments from SQLGetData().
**          Previously all the blob data had been fetched before SQLGetData()
**          was ever invoked.
**	23-apr-2004 (somsa01)
**	    Include systypes.h to avoid compile errors on HP.
**     27-apr-2004 (loera01)
**          First phase of support for SQL_BIGINT.
**     27-apr-2004 (loera01)
**          Port SQL_BIGINT change to Unix.
**     29-apr-2004 (loera01)
**          Second phase of support for SQL_BIGINT.
**      1-oct-2004 (thoda04) Bug 113170
**          Added support for SQL_BIGINT data returned as SQL_WCHAR.
**     07-oct-2004 (loera01) Bug 113203
**          In SQLGetData(), free and reset plb structure in the 
**          statement handle after all of the blob data has been returned.
**          Otherwise, the blob data from the previous fetch will get
**          concatenated to the current buffer.
**     15-nov-2004 (Ralph.Loen@ca.com) Bug 113462
**          Use IItrace global block to handle tracing.
**     26-may-2005 (loera01) Bug 114577
**          In FetchRowset(), initialize the row status array elemements
**          in the ARD before fetching anything.
**     27-sep-2005 (loera01) Bug 115051
**          In SQLGetData(), reset prevOctetLength field when data has been
**          retrieved successfully.
**     02-feb-2006 (Ralph Loen) SIR 115708
**          Added support for SQLExtendedFetch().  The new routine
**          FetchRowsetExtended() was driveved from FetchRowset(), but
**          modified to use the arguments from SQLExtendedFetch() rather
**          than the ARD and IRD. 
**     10-mar-2006 (Ralph Loen) Bug 115832
**          In GetWCharChar(), return the byte length of the converted
**          multibyte string in the "string length or indicator" argument.
**     10-mar-2006 (Ralph Loen) Bug 115833
**          In GetWCharChar(), don't return an error if the conversion
**          fails and the DBC control block has "szMultibyteFillChar" set
**          to non-blank.  Instead, fill the converted string with the 
**          specified fill character for those characters that failed to
**          convert.
**     10-Aug-2006 (Ralph Loen) SIR 116477
**          Add support for ISO dates, times, and intervals (first phase).
**     16-Aug-2006 (Ralph Loen) SIR 116477 (second phase)
**          Add support for ISO timestamps and DS intervals.
**     28-Sep-2006 (Ralph Loen) SIR 116477
**          Adjusted treatment of ISO date/times according to new API/ADF rules.
**     03-Oct-2006 (Ralph Loen) SIR 116477
**          Dynamic parameters for dates and timestamps are now sent as binary
**          values for backward-compatibility with ingresdate.
**     10-oct-2006 (Fei Ge) Bug 116827
**          In GetColumn(), for date in the format of SQL_CHAR returned from
**          non-Ingres server, first convert it to the yyyy-mm-dd format.
**     23-Oct-2006 (Ralph Loen) SIR 116477
**          Obsoleted CvtCharIsoTime(); replaced with common
**          GetTimeOrIntervalChar().
**     22-Nov-2006 (Ralph Loen) SIR 116477
**          In CvtIsoTimestampToChar(), allowed for ISO time precision to
**          be treated as a separate filed in the IRD.
**     21-Aug-2007 (Ralph Loen) Bug 118993
**          In SQLMoreResults(), return SQL_NO_DATA for insert or 
**          update queries.
**     20-Sep-2007 (weife01) SIR 119140
**          Added support for new DSN/connection string configuration
**          option:"Convert int8 from DBMS to int4".
*/



/*
**  function prototypes:
*/
static UDWORD  GetBit          (CHAR     *, SDWORD, UDWORD);
static UDWORD  GetBinChar      (LPDESCFLD, CHAR     *, SQLINTEGER);
static UDWORD  GetBinWChar     (LPDESCFLD, CHAR     *, SQLINTEGER);
static UDWORD  GetCharBin      (LPDESCFLD, CHAR     *, SQLINTEGER);
static UDWORD  GetCharChar     (LPDESCFLD, CHAR     *, SQLINTEGER);
static UDWORD  GetCharWChar    (LPDESCFLD, CHAR     *, SQLINTEGER);
static UDWORD  GetWCharChar    (LPDESCFLD, CHAR     *, SQLINTEGER, CHAR);
static UDWORD  GetWCharWChar   (LPDESCFLD, CHAR     *, SQLINTEGER);
static UDWORD  GetNumBin       (LPDESCFLD, CHAR     *, SQLINTEGER);
static UDWORD  GetNumChar      (LPDESCFLD, CHAR     *, SQLINTEGER, UDWORD, SWORD);
static UDWORD  GetNumWChar     (LPDESCFLD, CHAR     *, SQLINTEGER);
static VOID    GetNumFrac      (LPSTR, UWORD, UWORD);
static UDWORD CvtIngresDateToChar(LPDBC pdbc, CHAR     *, LPDESCFLD);
static UDWORD CvtIsoTimestampToChar(LPDBC pdbc, CHAR*, LPDESCFLD);
static UDWORD CvtIngresMnyToDec(CHAR     *, LPDESCFLD);
static UDWORD CvtChardateFormat(CHAR *, UWORD);
static BOOL GetTimeOrIntervalChar(CHAR *, CHAR *, VOID *, LPDESCFLD);

RETCODE FetchRow       (LPSTMT, SQLSMALLINT, SQLINTEGER);
RETCODE FetchRowset    (LPSTMT, SQLSMALLINT, SQLINTEGER);
RETCODE FetchRowsetExtended    (LPSTMT, SQLSMALLINT, SQLINTEGER,
    SQLUSMALLINT *, SQLUINTEGER *);

/*
**  SQLBindCol
**
**  Associate a user-supplied buffer with a database column.
**
**  On entry: pstmt     -->statement.
**            icol,      = ordinal column number.
**            fCType     = C data type in which to return value.
**            rgbValue  -->column value buffer.
**            cbValueMax = length of column value buffer.
**            pcbValue  -->where to return column value length.
**
**  On exit:  Nothing special.
**
**  Returns:  SQL_SUCCESS
**            SQL_ERROR
**            SQL_INVALID_HANDLE (by driver manager).
*/

RETCODE SQL_API SQLBindCol(
    SQLHSTMT      hstmt,        /* StatementHandle  */
    UWORD         icol,         /* ColumnNumber     */
    SWORD         fCType,       /* TargetType       */
    SQLPOINTER    rgbValue,     /* TargetValuePtr   */
    SQLINTEGER        cbValueMax,   /* BufferLength     */
    SQLINTEGER        *pcbValue)    /* StrLen_or_IndPtr */
{
    LPSTMT        pstmt          = (LPSTMT)hstmt;
    SWORD         VerboseCType   = fCType;
    SWORD         ConciseCType   = fCType;
    SQLSMALLINT   CPrecision     = 0;
    i4            icolMax;
    LPDESCFLD     pard;
    LPSQLTYPEATTR ptype;

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLBINDCOL, pstmt,
            icol, fCType, rgbValue, cbValueMax, pcbValue);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    /* Driver Manager is suppose to map but Merant 3.11 DM fails to map
       old date, time, timestamp values to new values */
    if      (fCType==SQL_C_DATE)
             fCType=VerboseCType=ConciseCType = SQL_C_TYPE_DATE;
    else if (fCType==SQL_C_TIME)
             fCType=VerboseCType=ConciseCType = SQL_C_TYPE_TIME;
    else if (fCType==SQL_C_TIMESTAMP)
             fCType=VerboseCType=ConciseCType = SQL_C_TYPE_TIMESTAMP;

    icolMax = MAX_COLUMNS_INGRES;

    /*
    **  Ensure a valid column number:
    */
    if (icol > (UWORD)icolMax) return ErrUnlockStmt (SQL_07009, pstmt);
                                         /* invalid descriptor index */
    if (icol == 0)      return ErrUnlockStmt (SQL_HYC00, pstmt);
                                         /* optional feature not implemented */

    /*
    **  Unbind a single bound column:
    */
    if (rgbValue == NULL)
    {
        if (icol <= (UWORD)pstmt->pARD->Count)
        {
            pard = pstmt->pARD->pcol + icol;
            if (pard->fStatus & COL_BOUND)
                pard->fStatus = 0;

            while (icol > 0  &&  icol==pstmt->pARD->Count)
                {  /* implicit resetting of COUNT field to highest bound column*/
                 pard = pstmt->pARD->pcol + icol;
                 if (pard->fStatus & COL_BOUND)
                    break;
                 ResetDescriptorFields(pstmt->pARD, icol, icol);
                 icol--;
                 pstmt->pARD->Count--;
                }
        }
        UnlockStmt (pstmt);
        return SQL_SUCCESS;
    }

    /*
    **  Ensure that ARD row descriptor is large enough:
    */
    if (icol > 
	(UWORD)pstmt->pARD->CountAllocated)  /* if need more col, re-allocate */
        if (ReallocDescriptor(pstmt->pARD, (i2)icol) == NULL)
           {
            RETCODE rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
            UnlockStmt (pstmt);
            return rc;
           }

    if (icol > 
	(UWORD)pstmt->pIRD->CountAllocated)  /* if need more col, re-allocate */
        if (ReallocDescriptor(pstmt->pIRD, (i2)icol) == NULL)
           {
            RETCODE rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
            UnlockStmt (pstmt);
            return rc;
           }

    /*
    **  Bind column into the ARD descriptor:
    */
    if ((UWORD)pstmt->pARD->Count < icol)  /* bump count of ARD fields w/data */
       {
        ResetDescriptorFields(pstmt->pARD, pstmt->pARD->Count+1, icol);
        pstmt->pARD->Count = icol;
       }
    if ((UWORD)pstmt->pIRD->Count < icol)  /* bump count of IRD fields w/data */
       {
        ResetDescriptorFields(pstmt->pIRD, pstmt->pIRD->Count+1, icol);
/*      pstmt->pIRD->Count = icol;       do NOT overlay result set descriptor */
       }

    pard = pstmt->pARD->pcol + icol;  /* pard->descriptor field */

    VerboseCType = fCType;
    ConciseCType = fCType;

    if (fCType == SQL_C_NUMERIC)
        CPrecision = DB_MAX_DECPREC;

    pard->ConciseType   = ConciseCType;   /* ARD SQL_DESC_CONCISE_TYPE   */
    ptype = CvtGetCType(ConciseCType, pstmt->fAPILevel); 
    if (ptype)
        pard->VerboseType = ptype->fVerboseType;
    else 
        pard->VerboseType   = VerboseCType;   /* ARD SQL_DESC_TYPE           */
    pard->Precision     = CPrecision;     /* ARD SQL_DESC_PRECISION      */
    pard->Scale         = 0;              /* ARD SQL_DESC_SCALE          */
    pard->DataPtr       = (PTR)rgbValue;  /* ARD SQL_DESC_DATA_PTR       */
    pard->OctetLength   = cbValueMax;     /* ARD SQL_DESC_OCTET_LENGTH   */
    pard->OctetLengthPtr= pcbValue;       /* ARD SQL_DESC_OCTET_LENGTH_PTR*/
    pard->IndicatorPtr  = pcbValue;       /* ARD SQL_DESC_INDICATOR_PTR  */
    if (rgbValue != NULL)                 /* if TargetValuePtr is not NULL */
         pard->fStatus  |=  COL_BOUND;    /* indicate column is bound */
    else pard->fStatus  &= ~COL_BOUND;    /* indicate column is unbound */

    UnlockStmt (pstmt);
    return SQL_SUCCESS;
}

/*
**  SQLDescribeCol
**
**  Return information about a database column.
**
**  On entry: pstmt       -->statement.
**            icol,        = ordinal column number.
**            szColName   -->column name return buffer.
**            cbColNameMax = length of column name buffer.
**            pcbColName  -->where to return actual name length.
**            pfSqlType   -->where to return SQL data type.
**            pcbColDef   -->where to return precision.
**            pibScale    -->where to return scale.
**            pfNullable  -->where to return NULLs allowed indicator.
**
**  On exit:  Column description returned from SQLVAR.
**
**  Returns:  SQL_SUCCESS
**            SQL_SUCCESS_WITH_INFO
**            SQL_ERROR
**            SQL_INVALID_HANDLE (by driver manager).
*/

RETCODE SQL_API SQLDescribeCol(
    SQLHSTMT     hstmt,
    UWORD        icol,
    UCHAR      * szColName,
    SWORD        cbColNameMax,
    SWORD      * pcbColName,
    SWORD      * pfSqlType,
    SQLUINTEGER    * pcbColDef,
    SWORD      * pibScale,
    SWORD      * pfNullable)
{
    return SQLDescribeCol_InternalCall(
                 hstmt,
                 icol,
                 szColName,
                 cbColNameMax,
                 pcbColName,
                 pfSqlType,
                 pcbColDef,
                 pibScale,
                 pfNullable);
}

RETCODE SQL_API SQLDescribeCol_InternalCall(
    SQLHSTMT     hstmt,
    UWORD        icol,
    UCHAR      * szColName,
    SWORD        cbColNameMax,
    SWORD      * pcbColName,
    SWORD      * pfSqlType,
    SQLUINTEGER    * pcbColDef,
    SWORD      * pibScale,
    SWORD      * pfNullable)
{
    LPSTMT     pstmt = (LPSTMT)hstmt;
    RETCODE    rc;
    LPDESCFLD  pird;

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLDESCRIBECOL, pstmt,
            icol, szColName, cbColNameMax, pcbColName,
            pfSqlType, pcbColDef, pibScale, pfNullable);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    
    /*
    **  Find SQLVAR for column requested:
    */
    rc = FindDescIRDField(pstmt, icol, &pird);
    if (rc != SQL_SUCCESS)
    {
        UnlockStmt (pstmt);
        return rc;
    }

    if ( pfSqlType)
        *pfSqlType = pird->ConciseType;

    if ( pfNullable)
        *pfNullable = pird->Nullable;
    if ((pstmt->fOptions & OPT_CONVERTINT8TOINT4) && (pird->ConciseType == SQL_BIGINT))
	   {
		   *pfSqlType = SQL_INTEGER;
		   *pfNullable = 0;
	   }
    if ( pcbColDef)
       {if      (pird->ConciseType == SQL_FLOAT  ||
                 pird->ConciseType == SQL_DOUBLE)
            *pcbColDef = 15;
        else if (pird->ConciseType == SQL_REAL)
            *pcbColDef = 7;
        else
            *pcbColDef = pird->Precision;
       }
    if ( pibScale)
        *pibScale = pird->Scale;

    rc = GetChar (pstmt, pird->Name, (CHAR *)szColName, cbColNameMax, pcbColName);

    UnlockStmt (pstmt);
    return rc;
}


/*  SQLFetch
**
**  1. Fetch the specified rowset of data from the result set,
**  2. Fetch next bulk buffer from server if needed.
**  3. Return values of all bound columns in the current row.
**
**  On entry: pstmt-->statement
**
**  Returns:  SQL_SUCCESS
**            SQL_SUCCESS_WITH_INFO
**            SQL_NO_DATA_FOUND
**            SQL_ERROR
**  Change History
**  --------------
**
**  date        programmer	description
**
*/
RETCODE SQL_API SQLFetch(
    SQLHSTMT hstmt)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    RETCODE rc;

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLFETCH, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    rc = FetchRowset(pstmt, SQL_FETCH_NEXT, 0);

    UnlockStmt (pstmt);
    return rc;
}


/*
**  SQLFetchScroll
**
**  Fetches the specified rowset of data from the result set and 
**  returns data for all bound columns. Rowsets can be specified 
**  at an absolute or relative position or by bookmark.
**  Equivalent to 2.x SQLExtendedFetch.
**
**  On entry:
**    StatementHandle [Input]
**    FetchOrientation [Input]
**       SQL_FETCH_NEXT    (this is the only value we support)
**       SQL_FETCH_PRIOR
**       SQL_FETCH_FIRST
**       SQL_FETCH_LAST
**       SQL_FETCH_ABSOLUTE
**       SQL_FETCH_RELATIVE
**       SQL_FETCH_BOOKMARK
**    FetchOffset [Input]
**       Number of the row to fetch. The interpretation of this 
**       argument depends on the value of the FetchOrientation argument.
**
**  Returns:
**    SQL_SUCCESS
**    SQL_SUCCESS_WITH_INFO
**    SQL_INVALID_HANDLE
**    SQL_ERROR
**
*/
SQLRETURN SQL_API SQLFetchScroll(
    SQLHSTMT     hstmt,
    SQLSMALLINT  FetchOrientation,
    SQLINTEGER FetchOffset)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    RETCODE rc;

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLFETCHSCROLL, hstmt, FetchOrientation, 
        FetchOffset);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    rc = FetchRowset(pstmt, FetchOrientation, FetchOffset);

    UnlockStmt (pstmt);
    return rc;
}


/*  FetchRowset
**
**  1. Fetch the specified rowset of data from the result set,
**  2. Fetch next bulk buffer from server if needed.
**  3. Return values of all bound columns in the current row.
**
**  On entry: pstmt-->statement
**            FetchOrientation = type of fetch:
**                SQL_FETCH_NEXT (only one support (
**                SQL_FETCH_PRIOR
**                SQL_FETCH_FIRST
**                SQL_FETCH_LAST
**                SQL_FETCH_ABSOLUTE
**                SQL_FETCH_RELATIVE
**                SQL_FETCH_BOOKMARK
**            FetchOffset = number of the row to fetch.
**                Used only for ABSOLUTE, RELATIVE, and BOOKMARK.
**
**  Returns:  SQL_SUCCESS
**            SQL_SUCCESS_WITH_INFO
**            SQL_NO_DATA_FOUND
**            SQL_ERROR
**  Change History
**  --------------
**
**  date        programmer  description
**
**  04/09/2001  Dave Thole  initial coding
**  26-may-2005 (loera01) Bug 114577
**      Initialize the row status array elemements in the ARD before 
**      fetching anything.
*/
RETCODE   FetchRowset(
    LPSTMT         pstmt,
    SQLSMALLINT    FetchOrientation,
    SQLINTEGER   FetchOffset)
{
    LPDESC         pARD = pstmt->pARD; /* pARD->application row descriptor */
    LPDESC         pIRD = pstmt->pIRD; /* pIRD->implementation row descriptor */
    SQLUINTEGER        RowArraySize     = pARD->ArraySize;
    SQLUINTEGER        RowsFetched      = 0;
    SQLUINTEGER       *pRowsFetched     = pIRD->RowsProcessedPtr;
    SQLUSMALLINT  *pRowsStatusArray = pIRD->ArrayStatusPtr;
    SQLUSMALLINT   rsarc;
    SQLRETURN      rc = SQL_SUCCESS;
    SQLRETURN      finalrc = SQL_SUCCESS;
    SQLUINTEGER        i;

    TRACE_INTL(pstmt,"FetchRowset");
    if (RowArraySize == 0)
        RowArraySize  = 1;

    if (pRowsFetched)
       *pRowsFetched = 0;  /* init count of rows fetched */

     /*
     **  Initialize row status array.
     */
     if (pARD->ArraySize > 0)
     {
         if (pIRD->ArrayStatusPtr)
         {
             for (i = 0; i < pARD->ArraySize; i++)
                 pIRD->ArrayStatusPtr[i] = SQL_PARAM_UNUSED;
         }
     }

    for (i = 0; i < RowArraySize; i++)
        {
         if (rc != SQL_NO_DATA)
            {
             pstmt->irowCurrent= i+1;  /* set current row in rowset for error handler*/
             rc = FetchRow(pstmt, FetchOrientation, FetchOffset);
             if (rc != SQL_SUCCESS)
                 ErrFlushErrorsToSQLCA(pstmt);
                   /* Flush STMT errors out of STMT block and into SQLCA error list */

             if      (rc == SQL_ERROR)
                      finalrc = SQL_ERROR;
             else if (rc == SQL_SUCCESS_WITH_INFO  &&  finalrc != SQL_ERROR)
                      finalrc = SQL_SUCCESS_WITH_INFO;
             else if (rc == SQL_NO_DATA  &&  RowsFetched == 0)
                      finalrc = SQL_NO_DATA;
            }

         if      (rc   == SQL_NO_DATA)   /* figure out row status */
                  rsarc = SQL_ROW_NOROW;
         else if (rc   == SQL_SUCCESS)
                  rsarc = SQL_ROW_SUCCESS;
         else if (rc   == SQL_SUCCESS_WITH_INFO)
                  rsarc = SQL_ROW_SUCCESS_WITH_INFO;
         else     rsarc = SQL_ROW_ERROR;

         if (pRowsStatusArray)            /* if row status array present */
             pRowsStatusArray[i] = rsarc; /*    set the row status */

         if (rc != SQL_NO_DATA)
             RowsFetched = i+1;  /* count of rows in row status array with data */

         if (rc == SQL_ERROR  &&  pstmt->fCvtError == 0)  /* stop filling in */
             rc = SQL_NO_DATA;   /* row status array if hard non-conversion error*/

        }  /* end for loop */

     if (pRowsFetched)
        *pRowsFetched = RowsFetched;  /* count of rows in row status array with data */

     if (finalrc == SQL_ERROR  &&  RowsFetched>1)
         finalrc = SQL_SUCCESS_WITH_INFO;

     pstmt->irowCurrent= 0;  /* reset current row in rowset for error handler*/
     pstmt->icolCurrent= 0;  /* reset current col in row for error handler*/

     return (finalrc);
}

/*
**  SQLExtendedFetch
**
**  Fetches the specified rowset of data from the result set and 
**  returns data for all bound columns. Rowsets can be specified 
**  at an absolute or relative position or by bookmark.
**  Equivalent to 2.x SQLExtendedFetch.
**
**  On entry:
**    StatementHandle [Input]
**    FetchOrientation [Input]
**       SQL_FETCH_NEXT    (this is the only value we support)
**       SQL_FETCH_PRIOR
**       SQL_FETCH_FIRST
**       SQL_FETCH_LAST
**       SQL_FETCH_ABSOLUTE
**       SQL_FETCH_RELATIVE
**       SQL_FETCH_BOOKMARK
**    FetchOffset [Input]
**       Number of the row to fetch. The interpretation of this 
**       argument depends on the value of the FetchOrientation argument.
**
**  Returns:
**    SQL_SUCCESS
**    SQL_SUCCESS_WITH_INFO
**    SQL_INVALID_HANDLE
**    SQL_ERROR
**
*/
SQLRETURN SQL_API SQLExtendedFetch(
    SQLHSTMT       hstmt,
    SQLUSMALLINT   FetchOrientation,
    SQLINTEGER     FetchOffset,
    SQLUINTEGER *  RowCountPtr,
    SQLUSMALLINT * RowStatusArray)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    RETCODE rc;
    LPDESC         pARD = pstmt->pARD; /* pARD->application row descriptor */
    LPDESC         pIRD = pstmt->pIRD; /* pIRD->implementation row descriptor */

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLEXTENDEDFETCH, hstmt, FetchOrientation,
        FetchOffset, RowCountPtr, RowStatusArray);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    rc = FetchRowsetExtended(pstmt, FetchOrientation, FetchOffset, 
        RowStatusArray, RowCountPtr);

    UnlockStmt (pstmt);
    return rc;
}

/*  FetchRowsetExtended
**
**  1. Fetch the specified rowset of data from the result set,
**  2. Fetch next bulk buffer from server if needed.
**  3. Return values of all bound columns in the current row.
**
**  On entry: pstmt-->statement
**            FetchOrientation = type of fetch:
**                SQL_FETCH_NEXT (only one support (
**                SQL_FETCH_PRIOR
**                SQL_FETCH_FIRST
**                SQL_FETCH_LAST
**                SQL_FETCH_ABSOLUTE
**                SQL_FETCH_RELATIVE
**                SQL_FETCH_BOOKMARK
**            FetchOffset = number of the row to fetch.
**                Used only for ABSOLUTE, RELATIVE, and BOOKMARK.
**
**  Returns:  SQL_SUCCESS
**            SQL_SUCCESS_WITH_INFO
**            SQL_NO_DATA_FOUND
**            SQL_ERROR
**  Change History
**  --------------
**
**  date        programmer  description
**
**  04/09/2001  Dave Thole  initial coding
**  26-may-2005 (loera01) Bug 114577
**      Initialize the row status array elemements in the ARD before 
**      fetching anything.
*/
RETCODE   FetchRowsetExtended(
    LPSTMT         pstmt,
    SQLSMALLINT    FetchOrientation,
    SQLINTEGER     FetchOffset, 
    SQLUSMALLINT * extRowStatusArray,
    SQLUINTEGER *  RowCountPtr)
{
    LPDESC         pARD = pstmt->pARD; /* pARD->application row descriptor */
    LPDESC         pIRD = pstmt->pIRD; /* pIRD->implementation row descriptor */
    SQLUINTEGER        RowArraySize     = pARD->ArraySize;
    SQLUINTEGER        RowsFetched      = 0;
    SQLUINTEGER       *pRowsFetched     = RowCountPtr;
    SQLUSMALLINT  *pRowsStatusArray = pIRD->ArrayStatusPtr;
    SQLUSMALLINT   rsarc;
    SQLRETURN      rc = SQL_SUCCESS;
    SQLRETURN      finalrc = SQL_SUCCESS;
    SQLUINTEGER        i;

    TRACE_INTL(pstmt,"FetchRowset");
    if (RowArraySize == 0)
        RowArraySize  = 1;

    if (pRowsFetched)
       *pRowsFetched = 0;  /* init count of rows fetched */

    if (pstmt->cRowsetSize <= 0)    
        RowArraySize = pstmt->cRowsetSize = 1;
    else
        RowArraySize = pstmt->cRowsetSize;

    if (extRowStatusArray)
        for (i = 0; i < pstmt->cRowsetSize; i++) 
            extRowStatusArray[i] = SQL_PARAM_UNUSED; 

    for (i = 0; i < RowArraySize; i++)
        {
         if (rc != SQL_NO_DATA)
            {
             pstmt->irowCurrent= i+1;  /* set current row in rowset for error handler*/
             rc = FetchRow(pstmt, FetchOrientation, FetchOffset);
             if (rc != SQL_SUCCESS)
                 ErrFlushErrorsToSQLCA(pstmt);
                   /* Flush STMT errors out of STMT block and into SQLCA error list */

             if      (rc == SQL_ERROR)
                      finalrc = SQL_ERROR;
             else if (rc == SQL_SUCCESS_WITH_INFO  &&  finalrc != SQL_ERROR)
                      finalrc = SQL_SUCCESS_WITH_INFO;
             else if (rc == SQL_NO_DATA  &&  RowsFetched == 0)
                      finalrc = SQL_NO_DATA;
            }

         if      (rc   == SQL_NO_DATA)   /* figure out row status */
                  rsarc = SQL_ROW_NOROW;
         else if (rc   == SQL_SUCCESS)
                  rsarc = SQL_ROW_SUCCESS;
         else if (rc   == SQL_SUCCESS_WITH_INFO)
                  rsarc = SQL_ROW_SUCCESS_WITH_INFO;
         else     rsarc = SQL_ROW_ERROR;

         if (extRowStatusArray)
              extRowStatusArray[i] = rsarc;
     
         if (rc != SQL_NO_DATA)
             RowsFetched = i+1;  /* count of rows in row status array with data */

         if (rc == SQL_ERROR  &&  pstmt->fCvtError == 0)  /* stop filling in */
             rc = SQL_NO_DATA;   /* row status array if hard non-conversion error*/

        }  /* end for loop */

     if (pRowsFetched)
        *pRowsFetched = RowsFetched;  /* count of rows in row status array with data */

     if (finalrc == SQL_ERROR  &&  RowsFetched>1)
         finalrc = SQL_SUCCESS_WITH_INFO;

     pstmt->irowCurrent= 0;  /* reset current row in rowset for error handler*/
     pstmt->icolCurrent= 0;  /* reset current col in row for error handler*/

     return (finalrc);
}


/*  FetchRow
**
**  1. Fetch a row of data from the result set,
**  2. Fetch next bulk buffer from server if needed.
**  3. Return values of all bound columns in the current row.
**
**  On entry: pstmt-->statement
**            FetchOrientation = type of fetch:
**                SQL_FETCH_NEXT (only one support (
**                SQL_FETCH_PRIOR
**                SQL_FETCH_FIRST
**                SQL_FETCH_LAST
**                SQL_FETCH_ABSOLUTE
**                SQL_FETCH_RELATIVE
**                SQL_FETCH_BOOKMARK
**            FetchOffset = number of the row to fetch.
**                Used only for ABSOLUTE, RELATIVE, and BOOKMARK.
**
**  Returns:  SQL_SUCCESS
**            SQL_SUCCESS_WITH_INFO
**            SQL_NO_DATA_FOUND
**            SQL_ERROR
**  Change History
**  --------------
**
**  date        programmer  description
**
**  01/16/1997  Jean Teng   put current pstmt in sqb before calling sqlfetch
*/
RETCODE   FetchRow(
    LPSTMT         pstmt,
    SQLSMALLINT    FetchOrientation,
    SQLINTEGER   FetchOffset)
{
    RETCODE rc;
    RETCODE rc2;
    SDWORD  i;
    SWORD   IRDCount;
    LPDBC   pdbc;
    LPSESS  psess;
    BOOL    fCommit = FALSE;
    LPDESC       pARD = pstmt->pARD;  /* pARD->application row descriptor */
    LPDESC       pIRD = pstmt->pIRD;  /* pIRD->implementation row descriptor */
    LPDESCFLD    pard;                /* pard->application row descriptor field */
    LPDESCFLD    pird;                /* pird->implementation row descriptor field*/

    TRACE_INTL(pstmt,"FetchRow");
    pdbc  = pstmt->pdbcOwner;
    psess = pstmt->psess;

    /*
    **  Check that CURSOR has been opened:
    **
    **  Note that real cursor might have been closed by a fetch
    **  that returned SQLCODE = 100, in which case STMT_EOF is set.
    **  The statement is still flagged as open to allow any
    **  remaining rows in the fetch buffer to be retrieved.
    **
    **  First check that the statement has been prepared.
    **  Normally the driver manager would catch this, but when
    **  cursor commit behavior is "close and delete" it does
    **  not seem to notice that the statement state reverts
    **  to "allocated" after an autocommit.
    */
    if (FetchOrientation != SQL_FETCH_NEXT) /* we only support forward-only */
        return ErrState (SQL_HY106, pstmt); /* Fetch type out of range */


    pstmt->icolPrev = 0;
    pstmt->fCvtError= 0;   /* make sure conversion error clear
                              for next fetch row in rowset */

    /*
    **  Check that SQL_MAX_ROWS not exceeded:
    */
    if (pstmt->crowMax && pstmt->crow >= pstmt->crowMax)
        return SQL_NO_DATA_FOUND;

    /*
    **  Get next row (it may be in bulk FETCH buffer):
    */
    if (pstmt->crowBuffer > 0)  /* if still some rows left in buffer */
    {
        /*
        **  Advance to next row in buffer:
        */
        pstmt->crow++;                      /* crow = current row number  */
        pstmt->crowBuffer--;                /* crowBuffer = number rows left in buf */
        pstmt->prowCurrent += pstmt->cbrow; /* prowCurrent -> current row */
        rc = SQL_SUCCESS;
    }
    else  /* refill buffer from server */
    {
        /*
        **  Fetch next bulk buffer from server:
        */
        pstmt->icolFetch = 0; 
        if (pstmt->fStatus & STMT_EOF)
            return SQL_NO_DATA_FOUND;

#ifdef CATALOG_CACHE_ENABLED
        if (pstmt->fStatus & STMT_CACHE_GET)
        {
            /*
            **  If result is in cache, we don't call server:
            */
            rc = SQL_SUCCESS;
            pstmt->prowCurrent = CacheGet (pdbc, pstmt);
        }
        else
#endif  /* CATALOG_CACHE_ENABLED */

        {
            /*
            **  Set SQB info for fetch:
            **  
            **  1. Use SQLCA from STMT block.
            **  2. Request piggy backed commands to be executed at end of scan
            **     (when SQLCODE = 100 is returned by server):  
            **
            **     - CLOSE   Always requested, done when SQLCODE = 100
            **
            **     - COMMIT  Attempt to minimize contention, request only 
            **                   
            **     - SUSPEND Always requested, always done
            ** 
            **  Note: To add support for positioned updates some changes needed,
            **  can only piggyback commands if cursor is read only and turn off
            **  bulk fetch, fetch one row at a time.  Until then, cursors are
            **  always read only (actually IDMS lets a postioned delete happen
            **  even if cursor was not opened for update, if someone were to do
            **  this with a bulk fetch they probably won't delete the correct row).  
            */
            pdbc->sqb.pSqlca = &pstmt->sqlca;
            pdbc->sqb.Options = SQB_CLOSE | SQB_SUSPEND;

            pstmt->fStatus &= ~STMT_OPEN;   /* check other cursors only */
                                            /* we'll turn STMT_OPEN back in a second */
            /*
            **  If fetching results for a catalog function,
            **  we may be using a separate connection:
            */
            if (pstmt->fStatus & STMT_CATALOG)  /* if catalog statement */
                CatSqlsid (pstmt, TRUE);        /* use the catalog session */

            pstmt->fStatus |= STMT_OPEN;    /* result set really is still open */
            pdbc->sqb.pStmt = pstmt;

            SqlFetch (                       /* Bulk fetch */
                &pdbc->sqb,                  /* -->session block et al... */
                pstmt->pFetchBuffer);         /* -->fetch buffer */

            if (pstmt->sqlca.SQLCODE == 100) /* remember piggybacked commands */
            {
                pstmt->fStatus |= STMT_CLOSED;
                if (fCommit)
                    psess->fStatus &= ~SESS_STARTED; /* no transaction active */
            }
            if (pstmt->sqlca.SQLCODE >= 0)   /* if everything OK, we can */
                psess->fStatus |= SESS_SUSPENDED;  /* suspend the session later */

            if (pstmt->fStatus & STMT_CATALOG)
                CatSqlsid (pstmt, FALSE);    /* reset back to main session */

            /*
            **  If we requested truncation, ignore warnings:
            */
            if (pstmt->cbValueMax && pstmt->sqlca.SQLCODE > 0)
            {
                if (pstmt->sqlca.SQLCODE != 100)
                    pstmt->sqlca.SQLCODE = 0;
                pstmt->sqlca.SQLERC  = 0;
                pstmt->sqlca.SQLMCT  = 0;
            }

            rc = SQLCA_ERROR(pstmt, psess);
            if (rc == SQL_ERROR)
            {
#ifdef CATALOG_CACHE_ENABLED
                if (pstmt->fStatus & STMT_CACHE_PUT)
                    CacheFree (pdbc);
#endif  /* CATALOG_CACHE_ENABLED */
                return SQL_ERROR;
            }
            /*
            **  If fetching results for a catalog function,
            **  convert the values for some of the columns
            **  in the buffer to those needed for the result set
            **  specified by ODBC.
            */
            if (pstmt->fCatalog)
                CatFetchBuffer (pstmt);

            /*
            **  Save fetch buffer in cache if needed: pig?
            */
#ifdef CATALOG_CACHE_ENABLED
            if (pstmt->fStatus & STMT_CACHE_PUT)
            {
                rc = CachePut (pdbc, pstmt);
                if (rc != SQL_SUCCESS)
                    return rc;
            }
#endif  /* CATALOG_CACHE_ENABLED */
            pstmt->prowCurrent = pstmt->pFetchBuffer;
        }
        /*
        **  SQLCA now knows results, fetched from server or CACHE:
        */
        if (pstmt->sqlca.SQLCODE == 100)
        {
            pstmt->fStatus |= STMT_EOF;

            if (pstmt->sqlca.SQLNRP == 0)
                return SQL_NO_DATA_FOUND;
            if (pstmt->sqlca.SQLMCT == 0)
                rc = SQL_SUCCESS;
        }
        pstmt->crowBuffer  = (UWORD)(pstmt->sqlca.SQLNRP - 1);
        pstmt->crow++;
    }   /* end refilling buffer */

    /*
    **  Return all bound columns in current row
    **  and reset all column returned flags:
    */
    rc2 = rc;
    pARD = pstmt->pARD;     /* pARD->application row descriptor */
    pIRD = pstmt->pIRD;     /* pIRD->application row descriptor */
    IRDCount = pIRD->Count; /* count of columns in result set */

    if (IRDCount > pARD->CountAllocated)  /* make sure ARD large enough */
        if (ReallocDescriptor(pARD, IRDCount) == NULL)
           {
            rc = ErrState (SQL_HY001, pstmt, F_OD0001_IDS_BIND_ARRAY);
            return rc;
           }

    pard = pARD->pcol + 1;  /* pard-> 1st ARD column after bookmark */
    pird = pIRD->pcol + 1;  /* pard-> 1st IRD column after bookmark */

    for  (i = 1; i <= IRDCount; i++, pard++, pird++)
    {
        pard->cbPartOffset = 0;
        pard->fStatus &= ~COL_RETURNED;

        if (pard->fStatus & COL_BOUND)
        {                      /* if SQLBindCol called or pARD->SQL_DESC_DATA_PTR is set */
                char        * SavedDataPtr        = pard->DataPtr;   /* save */
                SQLINTEGER      * SavedIndicatorPtr   = pard->IndicatorPtr;
                SQLINTEGER      * SavedOctetLengthPtr = pard->OctetLengthPtr;

                GetBoundDataPtrs(pstmt, Type_ARD, pARD, pard,
                                 &pard->DataPtr,
                                 &pard->OctetLengthPtr,
                                 &pard->IndicatorPtr);
                    /*  Get DataPtr, OctetLengthPtr, and IndicatorPtr
                        after adjustment for bind offset, row number,
                        and row or column binding. */

                pstmt->icolCurrent = i;   /* column in error if problem */

                rc2 = GetColumn (pstmt, pard, pird, NULL);

                pard->DataPtr        = SavedDataPtr;    /* restore pointers */
                pard->IndicatorPtr   = SavedIndicatorPtr;
                pard->OctetLengthPtr = SavedOctetLengthPtr;

            /*
            **  Let SQLGetData also get this column:
            */
            pard->cbPartOffset = 0;
            pard->fStatus &= ~COL_RETURNED;


            /*
            **  Code to call translation DLL might go here...
            */

            /*
            **  Update return code for caller:
            */
            if ((rc2 == SQL_ERROR) ||
                (rc2 == SQL_SUCCESS_WITH_INFO && rc != SQL_ERROR))
                rc = rc2;
        }  /* end if (pard->fStatus & COL_BOUND) */
    }   /* end for loop through the bound columns */

    pstmt->icolCurrent = 0;   /* reset column in error back to 0 */

    return rc;
}


/*
**  SQLGetData
**
**  Returns result data for a single column in the current row.
**
**  On entry: pstmt-->statement
**            icol,      = ordinal column number.
**            fCType     = C data type in which to return value.
**            rgbValue  -->column value buffer.
**            cbValueMax = length of column value buffer.
**            pcbValue  -->where to return column value length.
**
**  On exit:  Column is converted and returned to users buffer.
**
**  Returns:  SQL_SUCCESS
**            SQL_SUCCESS_WITH_INFO
**            SQL_NO_DATA_FOUND
**            SQL_ERROR
*/

RETCODE SQL_API SQLGetData(
    SQLHSTMT     hstmt,
    UWORD        icol,
    SWORD        fCType,
    SQLPOINTER   rgbValue,
    SQLINTEGER       cbValueMax,
    SQLINTEGER     * pcbValue)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    RETCODE      rc;
    LPDESC       pARD;   /* pARD->application row descriptor describing target */
    LPDESC       pIRD;   /* pIRD->implementation row descriptor describing source*/
    LPDESCFLD    pard;   /* pard->application row descriptor field*/
    LPDESCFLD    pird;   /* pird->implementation row descriptor field*/
    DESCFLD      ard;    /* work application row descriptor field*/
    BOOL         fNeedFetch = FALSE;

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLGETDATA, pstmt,
            icol, fCType, rgbValue, cbValueMax, pcbValue);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    /* Driver Manager is suppose to map but Merant 3.11 DM fails to map
       old date, time, timestamp values to new values */
    if      (fCType==SQL_C_DATE)
             fCType= SQL_C_TYPE_DATE;
    else if (fCType==SQL_C_TIME)
             fCType= SQL_C_TYPE_TIME;
    else if (fCType==SQL_C_TIMESTAMP)
             fCType= SQL_C_TYPE_TIMESTAMP;

    pARD = pstmt->pARD;  /* pARD->application row descriptor describing target */
    pIRD = pstmt->pIRD;  /* pIRD->implementation row descriptor describing source */

    /*
    **  Ensure a valid column number:
    */
    if (icol > (UWORD)pIRD->Count)   /* error if col no is > actual result set*/
        return ErrUnlockStmt (SQL_07009, pstmt);
                              /* invalid descriptor index */
    if (icol == 0)
        return ErrUnlockStmt (SQL_HYC00, pstmt);
                              /* optional feature not implemented */

    /*
    **  Off length remaining in previous COL, if any,
    **  and set current COL as next times previous COL:
    */
    if (pstmt->icolPrev && icol != pstmt->icolPrev && 
        (UWORD)pstmt->icolPrev <= pIRD->Count)
                /* stay in bounds for pstmt->icolPrev */
    {
        pard= pARD->pcol + pstmt->icolPrev;
        pard->cbPartOffset = 0;
    }
    pstmt->icolPrev = icol;

    /*
    **  Find COL and make sure it hasn't been done already:
    */
    pard= pARD->pcol + pstmt->icolPrev;
    if (pard->fStatus & COL_RETURNED)
    {
        UnlockStmt (pstmt);
        return SQL_NO_DATA_FOUND;
    }

    if (!(pard->fStatus & COL_BOUND))
        pard->OctetLength = cbValueMax;

    /* if get data of a column not fetched yet (columns after long data).
       (don't try the fetch if an internal constant query like SQLGetTypeInfo */
    if (icol > pstmt->icolFetch  &&  !(pstmt->fStatus & STMT_CONST))
        fNeedFetch = TRUE;

    if (fNeedFetch)
    {
            pstmt->pdbcOwner->sqb.pStmt = pstmt;
            pstmt->pdbcOwner->sqb.pSqlca = &pstmt->sqlca;
            SqlFetchSegment (&(pstmt->pdbcOwner->sqb),
                        pstmt->pFetchBuffer);

            rc = SQLCA_ERROR(pstmt, pstmt->psess);
            if (rc == SQL_ERROR)
            {
                UnlockStmt (pstmt);
                return SQL_ERROR;
            }
    }
    /*
    **  Return column value using internal bind column block:
    */

    MEcopy ((PTR)pard, sizeof(ard), (PTR)&ard);
    ard.ConciseType   = fCType;
    switch (fCType)
    {
        case SQL_C_TYPE_DATE:
        case SQL_C_TYPE_TIME:
        case SQL_C_TYPE_TIMESTAMP:
            ard.VerboseType   = SQL_DATETIME;
            break;

        case SQL_C_INTERVAL_YEAR:
        case SQL_C_INTERVAL_MONTH:
        case SQL_C_INTERVAL_DAY:
        case SQL_C_INTERVAL_HOUR:
        case SQL_C_INTERVAL_MINUTE:
        case SQL_C_INTERVAL_SECOND:
        case SQL_C_INTERVAL_YEAR_TO_MONTH:
        case SQL_C_INTERVAL_DAY_TO_HOUR:
        case SQL_C_INTERVAL_DAY_TO_MINUTE:
        case SQL_C_INTERVAL_DAY_TO_SECOND:
        case SQL_C_INTERVAL_HOUR_TO_MINUTE:
        case SQL_C_INTERVAL_HOUR_TO_SECOND:
        case SQL_C_INTERVAL_MINUTE_TO_SECOND:
            ard.VerboseType   = SQL_INTERVAL;
            break;

        default:
            ard.VerboseType   = fCType;
            break;
    }
    ard.DataPtr       = rgbValue;
    ard.OctetLength   = cbValueMax;
    ard.OctetLengthPtr= pcbValue;
    ard.IndicatorPtr  = pcbValue;

    pird = pstmt->pIRD->pcol + icol;
    pstmt->icolCurrent = icol;   /* column in error if problem */

    rc = GetColumn (pstmt, &ard, pird, NULL);
    if (rc == SQL_SUCCESS_WITH_INFO)
        pard->cbPartOffset = ard.cbPartOffset;
    if (rc == SQL_SUCCESS)
    {
        pard->fStatus |= COL_RETURNED;
        if (pard->plb)
        {
            if (pard->plb->pLongData && pard->DataPtr
                != pard->plb->pLongData)
                MEfree ((PTR)pard->plb->pLongData);
            MEfree ((PTR)pard->plb);
            pard->plb = NULL;
        }
        pird->prevOctetLength = 0;
    } 
    UnlockStmt (pstmt);
    return rc;
}


/*  SQLMoreResults
**
**  Determines if there are more results available for
**  SELECT, INSERT, UPDATE, or DELETE and initializes processing.
**
**  On entry: pstmt-->statement
**            pccol-->where to return number of columns.
**
**  On exit:  Number of columns in statement SQLDA, if any.
**
**  Returns:  SQL_SUCCESS
**            SQL_SUCCESS_WITH_INFO
**            SQL_NO_DATA_FOUND
**            SQL_ERROR
**
**  This driver supports parameter arrays, but not batched SQL.
**  The following describes the SQL statements that can have
**  multiple results (according to ODBC) and how they are processed:
**
**  SELECT The current open cursor is closed, and reopened with
**         the next set of parameters, if any.  Note that COMMIT
**         is only done when the last result set is closed.
**
**  INSERT SQL_NO_DATA is returned.  The value of pstmt->sqlca.SQLNRP 
**  UPDATE contains the number of rows processed.
**  DELETE 
**
*/
RETCODE SQL_API SQLMoreResults (
    SQLHSTMT hstmt)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    RETCODE rc = SQL_NO_DATA_FOUND;
    LPSQB   psqb;
              
    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLMORERESULTS, pstmt);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    switch (pstmt->fCommand)
    {
    case CMD_SELECT:
    case CMD_EXECPROC:

        if (pstmt->crowParm > 1 &&
            pstmt->icolParmLast &&    /* count of parameter markers != 0 */
            ++pstmt->irowParm < pstmt->crowParm)
        {
            if ((pstmt->fStatus & (STMT_OPEN | STMT_CLOSED)) == STMT_OPEN)
            {      /* if result set is still open, close it */
                psqb = &pstmt->pdbcOwner->sqb;
                psqb->pSqlca = &pstmt->sqlca;
                psqb->pStmt = pstmt;
                SqlClose (psqb);
                rc = SQLCA_ERROR (pstmt, pstmt->psess);
            }
            pstmt->fStatus &= ~(STMT_OPEN | STMT_EOF | STMT_CLOSED |
                                STMT_API_CURSOR_OPENED);
            if (rc != SQL_ERROR)
            if (pstmt->fCommand == CMD_SELECT)
                rc = ExecuteSelectStmt (pstmt);
            else
                rc = ExecuteStmt (pstmt);
        } 
        else
        {
            rc = FreeStmt (pstmt, SQL_CLOSE);
            if (rc != SQL_ERROR)
                rc = SQL_NO_DATA_FOUND;
        }
        break;

    case CMD_INSERT:
    case CMD_UPDATE:
    case CMD_DELETE:
    case CMD_BULK:            /* Not currently supported */
    default:

        break;
    }
    UnlockStmt (pstmt);
    return rc; 
}



/*  SQLNumResultCols
**
**  Returns the number of columns associated with a statement:
**
**  On entry: pstmt-->statement
**            pccol-->where to return number of columns.
**
**  On exit:  Number of columns in statement SQLDA, if any.
**
**  Returns:  SQL_SUCCESS
*/

RETCODE SQL_API SQLNumResultCols(
    SQLHSTMT    hstmt,
    SWORD     * pccol)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLNUMRESULTCOLS, pstmt, pccol);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    if (pccol)
        *pccol = pstmt->pIRD->Count;

    UnlockStmt (pstmt);
    return SQL_SUCCESS;
}


/*  SQLNumParams
**
**  Returns the number of columns associated with a statement:
**
**  On entry: pstmt-->statement
**            pcpar-->where to return number of parameters in the statement.
**
**  On exit:  Number of parameters in the statement.
**
**  Returns:  SQL_SUCCESS
*/

RETCODE SQL_API SQLNumParams(
    SQLHSTMT    hstmt,
    SWORD     * pcpar)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;

    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLNUMPARAMS, pstmt, pcpar);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */

    if (pcpar)
       *pcpar = 0;

    if (!( pstmt->szSqlStr && (pstmt->cbSqlStr > 0)) )
    {
          return ErrUnlockStmt (SQL_HY010, pstmt); /* function sequence error */
    }
    if (pcpar)
       *pcpar = (SWORD)pstmt->icolParmLast;

    UnlockStmt (pstmt);
    return SQL_SUCCESS;
}


/*  SQLRowCount
**
**  Returns the number of rows affected by an UPDATE, INSERT,
**  or DELETE statement associated with the specified hstmt.
**
**  On entry: pstmt-->statement
**            pcrow-->where to return number of rows.
**
**  On exit:  Number of rows from statement SQLCA.
**
**  Returns:  SQL_SUCCESS
*/

RETCODE SQL_API SQLRowCount(
    SQLHSTMT     hstmt,
    SQLINTEGER     * pcrow)
{
    LPSTMT  pstmt = (LPSTMT)hstmt;
    SDWORD  crow;
    
    if (!LockStmt (pstmt)) return SQL_INVALID_HANDLE;

    if (IItrace.fTrace >= ODBC_EX_TRACE)
        TraceOdbc (SQL_API_SQLROWCOUNT, pstmt, pcrow);

    ErrResetStmt (pstmt);        /* clear ODBC error status on STMT */
    
    crow = pstmt->sqlca.SQLNRP;  /* crow = number of rows processed */
    
                                 /* if crow==0  and a SELECT statement */
    if (!crow  &&  pstmt->fCommand == CMD_SELECT)
         crow = -1;              /*    then return -1 as "not available */
    
    if (pcrow) *pcrow = crow;

    UnlockStmt (pstmt);
    return SQL_SUCCESS;
}


/*
**  Internal functions:
*/

/*
**  GetColumn
**
**  Return column value to caller's data buffer:
**
**  On entry: pstmt  -->Statement block.
**            pard   -->application row descriptor field.
**            pird   -->implementation row descriptor field.
**            prgbData->source data to move from or NULL
**                       (used by odbc_retrieveByRefParms)
**
**  On exit:  Column value is converted to C type and returned
**            if success or success with info.
**            SQLSTATE is set if not success.
**            Bind column block status updated.
**
**  Returns:  SQL_SUCCESS
**            SQL_SUCCESS_WITH_INFO
**            SQL_NO_DATA_FOUND
**            SQL_ERROR
**
**  Called by:
**            FetchScroll after fetch and get bound columns
**            SQLGetData to get one specific column
**            odbc_retrieveByRefParms to update in/out proc parms
**
**  Change History
**  --------------
**
**  date         programmer	 description
**  02/04/1997   Jean Teng   added CvtIngresDateToChar ...
**  02/05/1997   Jean Teng   added CvtIngresMnyToDec to support money data 
**                           type. Added code to support long varchar/byte
**  05/12/2000  Dave Thole   Increased size of szValue work field.
**  06/25/2003  Ralph Loen   When coercing SQL_NUMERIC into SQL_C_LONG,
**                           reference szValue instead or rgbData.
**  10/10/2006  Fei Ge       for SQL_CHAR to SQL_C_TYPE_DATE conversion for
**                           non-Ingres, convert date to yyyy-mm-dd.
*/
RETCODE GetColumn(
    LPSTMT       pstmt,
    LPDESCFLD    pard,
    LPDESCFLD    pird,
    CHAR     *   prgbData)
{
    UDWORD      rc = CVT_SUCCESS;
    CHAR     *  rgbData;            /* -->data in fetch buffer */
    CHAR     *  rgbNull;            /* -->null indicator in fetch buffer */
    SQLINTEGER      cbData;             /* length of data in buffer */
    UCHAR       cbLData=0;          /* length of long vchar data in buffer */
    SWORD       fSqlType;           /* SQL data type of column */
    SWORD       fCType;             /* C data type for column */
    LPSQLTYPEATTR  ptypeSQL;        /* -->TYPE_TABLE entry for SQL type. */
    LPSQLTYPEATTR  ptypeC;          /* -->TYPE_TABLE entry for C type. */
    SFLOAT      realValue = 0.0;
    SDOUBLE     floatValue = 0.0;   /* conversion work buffers: */
    SDOUBLE     floatRound = 0.5;
    DATE_STRUCT dateValue;
    ISO_TIME_STRUCT timeValue;
    TIMESTAMP_STRUCT stampValue;
    SQL_INTERVAL_STRUCT intervalValue;
    SDWORD      intValue = 0;
    SWORD       shortValue = 0;
    SCHAR       tinyValue = 0;
    ODBCINT64 int64Value = 0;
    SDWORD      intNum = 0;
    SQLINTEGER      cbValueSavedInCaseOfError = 0;
    SQLINTEGER      IndicatorSavedInCaseOfError = 0;
    CHAR        szValue[340];  /* intermediate result for SQL_DOUBLE ->
                                  SQL_C_CHAR conversion can be as long 
                                  as 325 (sign+308+decpt+15) digits*/
    CHAR        szValueWChar[340]; /* intermediate result for SQL_WCHAR ->
                                      SQL_C_xxx conversions*/
    CHAR        szNum[DB_MAX_DECPREC+3];
    CHAR        szSqlNum[MAX_NUM_DIGITS+3];
    ODBC_DATENTRNL * d;
    ODBC_TIME * t;
    UCHAR  decimal_char;
    SQL_NUMERIC_STRUCT *ns;
    TRACE_INTL (pstmt, "GetColumn");

    /*
    **  Find column in fetch buffer:   rgbData->source data to move from
    **                                 rgbNull->source data's null indicator
    */
    if (!prgbData)
        rgbData = pstmt->prowCurrent + pird->cbDataOffset;
    else
        rgbData = prgbData;

    rgbNull = pstmt->prowCurrent + pird->cbNullOffset;
    cbData  = pird->OctetLength;

    /*
    **  Return NULL value, if that's what we got:
    */
    if (!prgbData)
    {
        if (pird->Nullable==SQL_NULLABLE  &&
            (*(SCHAR *)rgbNull == SQL_NULL_DATA /* -1 */))
        {
            if (!pard->IndicatorPtr)   /* if no ind from SQLBindCol(pcbValue) */
                return ErrState (SQL_22002, pstmt);

            *pard->IndicatorPtr = SQL_NULL_DATA;  /* SQLBindCol(pcbValue) */
            return (SQL_SUCCESS);
        }
    }
    /*
    **  Find entry for SQL and C types in attribute table:
    */
    fSqlType = pird->ConciseType;
    ptypeSQL = CvtGetSqlType (fSqlType, pstmt->fAPILevel);
    if (ptypeSQL == NULL)
    {
        pstmt->fCvtError |= CVT_INVALID;
        return (SQL_ERROR);
    }

    fCType = pard->ConciseType;
    if (fCType == SQL_C_DEFAULT)
        fCType = ptypeSQL->fCType;
    ptypeC = CvtGetCType (fCType, pstmt->fAPILevel);
    if (ptypeC == NULL)
    {
        pstmt->fCvtError |= CVT_INVALID;
        return (SQL_ERROR);
    }

    /*
    **  Get default length:
    */
    switch (fSqlType)
    {
    case SQL_BIT:
        /*
        **  Driver won't handle what it shouldn't get:
        */
        pstmt->fCvtError |= CVT_NOT_CAPABLE;
        return (SQL_ERROR);

    case SQL_VARBINARY:
    case SQL_VARCHAR:
    case SQL_WVARCHAR:
        cbData   = *(UWORD *)rgbData;
        rgbData += 2;
        break;

    case SQL_LONGVARCHAR:
    case SQL_LONGVARBINARY:
    case SQL_WLONGVARCHAR:
        if (pird->fStatus & COL_MULTISEGS)
        {
            if (fCType == SQL_C_CHAR  ||
                fCType == SQL_C_WCHAR ||
                fCType == SQL_C_BINARY)
            {
                cbData = pard->plb->cbData;
                rgbData = pard->plb->pLongData;
            }
            else
            {
                pstmt->fCvtError |= CVT_INVALID;
                return(SQL_ERROR);
            }
        }
        else
        {
            cbData   = *(UWORD *)rgbData;
            rgbData += 2;
        }
        /*
        ** Save length for SQL_C_NUMERIC.
        */
        cbLData = (UCHAR)cbData;
        
        break;

    case SQL_TYPE_DATE:
        rc = CvtIngresDateToChar(pstmt->pdbcOwner, rgbData, pird);
        if (rc != CVT_SUCCESS)
            return(SQL_ERROR);
        cbData = STlength(rgbData);
        if (cbData==0  &&  pard->IndicatorPtr) /* SQLBindCol(pcbValue) */
        {
            *pard->IndicatorPtr = SQL_NULL_DATA;  /* SQLBindCol(pcbValue) */
            return(SQL_SUCCESS);
        }
        break;

    case SQL_TYPE_TIMESTAMP:
        d = (ODBC_DATENTRNL *)rgbData;
        /*
        ** Currently, the driver does not support intervals in a pre-ISO 
        ** context.
        */
        if (d->date_spec & ODBC_TIME_INTERVAL && 
            pstmt->fAPILevel < IIAPI_LEVEL_4)
            return ErrState (SQL_HY000, pstmt, F_OD0079_IDS_ERR_DT_INTERVALS);

        if (pstmt->fAPILevel < IIAPI_LEVEL_4 || pird->fIngApiType ==
            IIAPI_DTE_TYPE)
            rc = CvtIngresDateToChar(pstmt->pdbcOwner, rgbData, pird);
        else
        {
            rc = CvtIsoTimestampToChar(pstmt->pdbcOwner, rgbData, pird);
        }
        if (rc != CVT_SUCCESS)
            return(SQL_ERROR);

        cbData = STlength(rgbData);
        if (cbData==0  &&  pard->IndicatorPtr) /* SQLBindCol(pcbValue) */
        {
            *pard->IndicatorPtr = SQL_NULL_DATA;  /* SQLBindCol(pcbValue) */
            return(SQL_SUCCESS);
        }
        break;

    case SQL_INTERVAL_YEAR_TO_MONTH:
    case SQL_INTERVAL_YEAR:
    case SQL_INTERVAL_MONTH:
        if (pstmt->fAPILevel > IIAPI_LEVEL_3)
        {
            if (!GetTimeOrIntervalChar (szValue, rgbData, pstmt->pdbcOwner->envHndl,
                pird))
                return (SQL_ERROR);
        }
        else
            return (SQL_ERROR);

        rgbData = &szValue[0];
        cbData = STlength(rgbData);
        if (cbData==0  &&  pard->IndicatorPtr) /* SQLBindCol(pcbValue) */
        {
            *pard->IndicatorPtr = SQL_NULL_DATA;  /* SQLBindCol(pcbValue) */
            return(SQL_SUCCESS);
        }
        break;

     case SQL_INTERVAL_DAY:
     case SQL_INTERVAL_HOUR:
     case SQL_INTERVAL_MINUTE:
     case SQL_INTERVAL_SECOND:
     case SQL_INTERVAL_DAY_TO_HOUR:
     case SQL_INTERVAL_DAY_TO_MINUTE:
     case SQL_INTERVAL_DAY_TO_SECOND:
     case SQL_INTERVAL_HOUR_TO_MINUTE:
     case SQL_INTERVAL_HOUR_TO_SECOND:
     case SQL_INTERVAL_MINUTE_TO_SECOND:
     case SQL_TYPE_TIME:
        if (pstmt->fAPILevel > IIAPI_LEVEL_3)
        {
            t = (ODBC_TIME *)rgbData;
            if (!GetTimeOrIntervalChar (szValue, rgbData, pstmt->pdbcOwner->envHndl,
                pird))
                return (SQL_ERROR);
        }
        szValue[pird->OctetLength] = '\0';
        rgbData = &szValue[0];
        cbData = STlength(rgbData);
        if (cbData==0  &&  pard->IndicatorPtr) /* SQLBindCol(pcbValue) */
        {
            *pard->IndicatorPtr = SQL_NULL_DATA;  /* SQLBindCol(pcbValue) */
            return(SQL_SUCCESS);
        }
        break;

    case SQL_DECIMAL:
        if (pird->fIngApiType == IIAPI_MNY_TYPE)
        {
            rc = CvtIngresMnyToDec(rgbData, pird); 
            if (rc != CVT_SUCCESS)
                return(SQL_ERROR);
        }
    }

    /*
    **  if source type is a wide (Unicode) string and
    **  target type is not binary, char, nor wchar (they handle
    **  the wide string themselves) then convert the wide string
    **  to a normal character string first and point rgbData to it.
    */
    if ((fSqlType == SQL_WCHAR         ||   /* source types */
         fSqlType == SQL_WVARCHAR      ||
         fSqlType == SQL_WLONGVARCHAR)
       &&
       !(fCType   == SQL_C_BINARY      ||   /* target types */
         fCType   == SQL_C_CHAR        ||
         fCType   == SQL_C_WCHAR))
        {
            RETCODE  rc;
        
            /* 
            ** On entry: rgbData -> wide string
            ** cbData  =  wide string length (bytes)
            */
            rc = ConvertWCharToChar(pstmt,
                (SQLWCHAR*)rgbData, cbData/sizeof(SQLWCHAR), /* src */
                szValueWChar, sizeof(szValueWChar),          /* target */
                NULL, NULL, &cbData);
            if (rc != SQL_SUCCESS  &&  rc != SQL_SUCCESS_WITH_INFO)
                return(rc);
            rgbData = szValueWChar;
            /* 
            ** On exit:  rgbData -> character string
            ** cbData  =  character string length 
            */
       }

    /*
    **  Set default length if caller specified OctetLengthPtr(pcbValue).
    **  We save the current value so we can restore it if
    **  an error.  Seems silly, but that's the ODBC 2.x spec.
    */
    if (pard->OctetLengthPtr)  /* if SQLBindCol(pcbValue) */
    {
        IndicatorSavedInCaseOfError = *pard->IndicatorPtr;
        cbValueSavedInCaseOfError   = *pard->OctetLengthPtr;
        *pard->IndicatorPtr   = 0;
        *pard->OctetLengthPtr = ptypeC->cbSize;
    }

    /*
    **  Get value in default format:
    */
    if (fCType != SQL_C_BINARY)
    {
        switch (fSqlType)
        {
        case SQL_DECIMAL:
        case SQL_NUMERIC:
        {
            char *p;
            if (!CvtApiTypes( szValue, rgbData, pstmt->pdbcOwner->envHndl, 
                pird, IIAPI_CHA_TYPE, IIAPI_DEC_TYPE))
                return SQL_ERROR;
            cbData  = pird->Precision + 3;
            if (pstmt->pdbcOwner->fOptions & OPT_DECIMALPT)
            {
                p = STindex(szValue, ",", cbData);
                if (p != NULL)
                    *p = '.'; 
            }
            break;
        }
        case SQL_TINYINT:
        
            MEcopy(rgbData, sizeof(SCHAR), (PTR)&tinyValue);
            shortValue = (SWORD)tinyValue;
            intValue = (SDWORD)tinyValue;
                int64Value = (ODBCINT64)tinyValue;
            break;
        
        case SQL_SMALLINT:

            MEcopy(rgbData, sizeof(SWORD), (PTR)&shortValue);
            tinyValue = (SCHAR)shortValue;
            intValue = (SDWORD)shortValue;
            int64Value = (ODBCINT64)shortValue;
            break;

        case SQL_INTEGER:
 
            MEcopy(rgbData, sizeof(SDWORD), (PTR)&intValue); 
            tinyValue = (SCHAR)intValue;
            shortValue = (SWORD)intValue;
            int64Value = (ODBCINT64)intValue;
            break;

        case SQL_BIGINT:    
                           
            MEcopy(rgbData, sizeof(ODBCINT64), (PTR)&int64Value);
            tinyValue = (SCHAR)int64Value;
            shortValue = (SWORD)int64Value;
            intValue = (SDWORD)int64Value;
			if((pstmt->fOptions & OPT_CONVERTINT8TOINT4)&&(intValue!=int64Value))
				rc = CVT_TRUNCATION;
            break;

        case SQL_REAL:
            MEcopy(rgbData, sizeof(SFLOAT), (PTR)&realValue);
            floatValue = (SDOUBLE)realValue;
            break;

        case SQL_FLOAT:
        case SQL_DOUBLE:

            MEcopy(rgbData, sizeof(SDOUBLE), (PTR)&floatValue);
            break;
        
        default:

            break;
        }
    }

    /*
    **  Convert data to C type requested:
    */
    switch (fCType)
    {
    case SQL_C_BINARY:

        switch (fSqlType)
        {
        case SQL_BINARY:
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_VARBINARY:
        case SQL_LONGVARCHAR:
        case SQL_LONGVARBINARY:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
            rc = GetCharBin (pard, rgbData, cbData);
            break;

        default:

            rc = GetNumBin (pard, rgbData, cbData);
            break;
        }
        break;

    case SQL_C_BIT:

        switch (fSqlType)
        {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_DECIMAL:
        case SQL_NUMERIC:

            rc = CvtCharInt (&intValue, rgbData, (UWORD)cbData);
            break;

        case SQL_BIGINT:            /* Never in Win32 */

            if (intNum != 0) rc = CVT_OUT_OF_RANGE;

        case SQL_INTEGER:
        case SQL_SMALLINT:

            break;

        case SQL_REAL:
        case SQL_FLOAT:
        case SQL_DOUBLE:
            intValue = (SDWORD)(floatValue + 0.5);
            if ((SDOUBLE)intValue != floatValue) rc = CVT_TRUNCATION;
            break;

        default:

            rc = CVT_INVALID;
            break;

        }
        if (rc == CVT_SUCCESS || rc  == CVT_TRUNCATION)
            rc = GetBit (pard->DataPtr, intValue, rc);  /* SQLBindCol(rgbValue) */
        break;

    case SQL_C_CHAR:

        switch (fSqlType)
        {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
            rc = GetCharChar (pard, rgbData, cbData);
            break;

        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
            rc = GetWCharChar (pard, rgbData, cbData, 
                pstmt->pdbcOwner->multibyteFillChar);
            break;

        case SQL_BINARY:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
            rc = GetBinChar (pard, rgbData, cbData); 
            break;

        case SQL_SMALLINT:
        case SQL_INTEGER:
        case SQL_TINYINT:

            CVla ((SDWORD)intValue, szValue);
            cbData = STlength (szValue);
            rc = GetNumChar (pard, szValue, cbData, pstmt->fAPILevel, fSqlType);
            break;

        case SQL_REAL:

            decimal_char = 
                pstmt->pdbcOwner->fOptions & OPT_DECIMALPT ? ',' : '.';
            cbData = CvtDoubleToChar(floatValue, 7, szValue,decimal_char);
            rc = GetNumChar (pard, szValue, cbData, pstmt->fAPILevel, fSqlType);
            break;

        case SQL_FLOAT:
        case SQL_DOUBLE:

            decimal_char = 
                pstmt->pdbcOwner->fOptions & OPT_DECIMALPT ? ',' : '.';
            cbData = CvtDoubleToChar(floatValue, 15, szValue,decimal_char);
            rc = GetNumChar (pard, szValue, cbData, pstmt->fAPILevel, fSqlType);
            break;

        case SQL_DECIMAL:
        case SQL_NUMERIC:
        {
           char *p;

            CvtCharNumStr (szNum, sizeof (szNum), szValue, (UWORD)cbData,
                           NULL, NULL);
            GetNumFrac (szNum, sizeof (szNum), (UWORD)pird->Scale);
            if (pstmt->pdbcOwner->fOptions & OPT_DECIMALPT)
            {
                p = STindex(szNum, ".", cbData);
                if (p != NULL)
                    *p = ','; 
            }
            rc = GetNumChar (pard, szNum, STlength (szNum), pstmt->fAPILevel,
                fSqlType);
            break;
        }
       
        case SQL_BIGINT:

            CVla8 (int64Value, szValue);
            cbData = STlength (szValue);
            rc = GetNumChar (pard, szValue, cbData, pstmt->fAPILevel, fSqlType);
            break;

        case SQL_TYPE_DATE:
            rc = GetCharChar (pard, rgbData, cbData);
            break;
            
        case SQL_TYPE_TIME:
        case SQL_TYPE_TIMESTAMP:
        case SQL_INTERVAL_YEAR_TO_MONTH:
        case SQL_INTERVAL_YEAR:
        case SQL_INTERVAL_MONTH:
        case SQL_INTERVAL_DAY:
        case SQL_INTERVAL_HOUR:
        case SQL_INTERVAL_MINUTE:
        case SQL_INTERVAL_SECOND:
        case SQL_INTERVAL_DAY_TO_HOUR:
        case SQL_INTERVAL_DAY_TO_MINUTE:
        case SQL_INTERVAL_DAY_TO_SECOND:
        case SQL_INTERVAL_HOUR_TO_MINUTE:
        case SQL_INTERVAL_HOUR_TO_SECOND:
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            cbData = STlength(rgbData);
            rc = GetNumChar (pard, rgbData, cbData, pstmt->fAPILevel, fSqlType); 
            break;

        default:

            rc = CVT_INVALID;
            break;
        }
        break;


    case SQL_C_WCHAR:

        switch (fSqlType)
        {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
            rc = GetCharWChar (pard, rgbData, cbData);
            break;

        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
            rc = GetWCharWChar (pard, rgbData, cbData);
            break;

        case SQL_BINARY:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
            rc = GetBinWChar (pard, rgbData, cbData);
            break;

        case SQL_SMALLINT:
        case SQL_INTEGER:
        case SQL_TINYINT:

            CVla ((SDWORD)intValue, szValue);
            cbData = STlength (szValue);
            rc = GetNumWChar (pard, szValue, cbData);
            break;

        case SQL_REAL:

            decimal_char = 
                pstmt->pdbcOwner->fOptions & OPT_DECIMALPT ? ',' : '.';
            cbData = CvtDoubleToChar(floatValue, 7, szValue,decimal_char);
            rc = GetNumWChar (pard, szValue, cbData);
            break;

        case SQL_FLOAT:
        case SQL_DOUBLE:

            decimal_char = 
                pstmt->pdbcOwner->fOptions & OPT_DECIMALPT ? ',' : '.';
            cbData = CvtDoubleToChar(floatValue, 15, szValue,decimal_char);
            rc = GetNumWChar (pard, szValue, cbData);
            break;

        case SQL_DECIMAL:
        case SQL_NUMERIC:

            CvtCharNumStr (szNum, sizeof (szNum), szValue, (UWORD)cbData, NULL, NULL);
            GetNumFrac (szNum, sizeof (szNum), (UWORD)pird->Scale);
            rc = GetNumWChar (pard, szNum, (UWORD)strlen (szNum));
            break;

        case SQL_BIGINT:
            CVla8 (int64Value, szValue);
            cbData = STlength (szValue);
            rc = GetNumWChar (pard, szValue, cbData);
            break;

        case SQL_TYPE_DATE:
        case SQL_TYPE_TIME:
        case SQL_TYPE_TIMESTAMP:
        case SQL_INTERVAL_YEAR_TO_MONTH:
        case SQL_INTERVAL_YEAR:
        case SQL_INTERVAL_MONTH:
        case SQL_INTERVAL_DAY:
        case SQL_INTERVAL_HOUR:
        case SQL_INTERVAL_MINUTE:
        case SQL_INTERVAL_SECOND:
        case SQL_INTERVAL_DAY_TO_HOUR:
        case SQL_INTERVAL_DAY_TO_MINUTE:
        case SQL_INTERVAL_DAY_TO_SECOND:
        case SQL_INTERVAL_HOUR_TO_MINUTE:
        case SQL_INTERVAL_HOUR_TO_SECOND:
        case SQL_INTERVAL_MINUTE_TO_SECOND:
            rc = GetNumWChar (pard, rgbData, cbData); 
            break;

        default:
            rc = CVT_INVALID;
            break;
        }
        break;


    case SQL_C_TINYINT:
    case SQL_C_STINYINT:
    case SQL_C_UTINYINT:

        switch (fSqlType)
        {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_DECIMAL:
        case SQL_NUMERIC:

            rc = CvtCharInt ((SDWORD *)&tinyValue, rgbData, (UWORD)cbData);
            break;

        case SQL_BIGINT:            /* Never in Win32... */

            if (intNum != 0) rc = CVT_OUT_OF_RANGE;
            break;

        case SQL_INTEGER:
        case SQL_SMALLINT:
        case SQL_TINYINT:

            break;

        case SQL_REAL:
        case SQL_FLOAT:
        case SQL_DOUBLE:

            if (fCType == SQL_C_UTINYINT)
            {
                if (floatValue < 0 || floatValue > 255.0)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
            }
            else
            {
                if (floatValue < -128.0 || floatValue > 127.0)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
                if (floatValue < 0) floatRound = -0.5;
           }

            tinyValue = (SCHAR)(floatValue + floatRound);
            if ((SDOUBLE)tinyValue != floatValue) rc = CVT_TRUNCATION;
            break;

        default:

            rc = CVT_INVALID;
            break;

        }
        if (rc == CVT_SUCCESS || rc == CVT_TRUNCATION)
        {
            if (fCType == SQL_C_UTINYINT)
            {
                if (tinyValue < 0 || tinyValue > 255)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
	        MEcopy((PTR)&tinyValue, sizeof(UCHAR), pard->DataPtr);
            }
            else
            {
                if (tinyValue < -128 || tinyValue > 127)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
	        MEcopy((PTR)&tinyValue, sizeof(SCHAR), pard->DataPtr);
            }
        }
        break;

    case SQL_C_SHORT:
    case SQL_C_SSHORT:
    case SQL_C_USHORT:

        switch (fSqlType)
        {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_DECIMAL:
        case SQL_NUMERIC:

            rc = CvtCharInt ((SDWORD *)&shortValue, rgbData, (UWORD)cbData);
            break;

        case SQL_BIGINT:            /* Never in Win32 */

            if (intNum != 0) rc = CVT_OUT_OF_RANGE;
            break;

        case SQL_INTEGER:
        case SQL_SMALLINT:
        case SQL_TINYINT:
            break;

        case SQL_REAL:
        case SQL_FLOAT:
        case SQL_DOUBLE:

            if (fCType == SQL_C_USHORT)
            {
                if (floatValue < 0 || floatValue > 65535.0)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
            }
            else
            {
                if (floatValue < -32768.0 || floatValue > 32767.0)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
                if (floatValue < 0) floatRound = -0.5;
            }
            shortValue = (SWORD)(floatValue + floatRound);
            if ((SDOUBLE)shortValue != floatValue) rc = CVT_TRUNCATION;
            break;

        default:

            rc = CVT_INVALID;
            break;

        }
        if (rc == CVT_SUCCESS || rc == CVT_TRUNCATION)
        {
            if (fCType == SQL_C_USHORT)
            {
                if (shortValue < 0 || shortValue > 65535)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
	        MEcopy((PTR)&shortValue, sizeof(UWORD), pard->DataPtr);
            }
            else
            {
                if (shortValue < -32768 || shortValue > 32767)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
                MEcopy((PTR)&shortValue, sizeof(SWORD), pard->DataPtr);
            }
        }
        break;

    case SQL_C_SBIGINT:
    case SQL_C_UBIGINT:

        switch (fSqlType)
        {
        case SQL_DECIMAL:
        case SQL_NUMERIC:

            rc = CvtCharInt64 (&int64Value, szValue, (UWORD)cbData);
            break;

        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:

            rc = CVal8 (rgbData, &int64Value);
            break;

        case SQL_BIGINT:
        case SQL_INTEGER:
        case SQL_SMALLINT:
        case SQL_TINYINT:
            break;

        case SQL_REAL:
        case SQL_FLOAT:
        case SQL_DOUBLE:

            if (fCType == SQL_C_UBIGINT)
            {
                if (floatValue < 0 || floatValue > 18446744073709551615.0)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
            }
            else
            {
                if (floatValue < -9223372036854775807.0 || floatValue > 9223372036854775807.0)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
                if (floatValue < 0) floatRound = -0.5;
            }
            int64Value = (ODBCINT64)(floatValue + floatRound);
            if ((SDOUBLE)int64Value != floatValue) rc = CVT_TRUNCATION;
            break;

        default:

            rc = CVT_INVALID;
            break;

        }
        if (rc == CVT_SUCCESS || rc == CVT_TRUNCATION)
        {
            if (fCType == SQL_C_UBIGINT)
		MEcopy((PTR)&int64Value, sizeof(unsigned ODBCINT64), 
                    pard->DataPtr);
            else
		MEcopy((PTR)&int64Value, sizeof(ODBCINT64), pard->DataPtr);
        }

        break;


    case SQL_C_LONG:
    case SQL_C_SLONG:
    case SQL_C_ULONG:

        switch (fSqlType)
        {
        case SQL_DECIMAL:
        case SQL_NUMERIC:

            rc = CvtCharInt (&intValue, szValue, (UWORD)cbData);
            break;

        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:

            rc = CvtCharInt (&intValue, rgbData, (UWORD)cbData);
            break;

        case SQL_BIGINT:           
            
			intValue = (SDWORD)int64Value;
            break;

        case SQL_INTEGER:
        case SQL_SMALLINT:
        case SQL_TINYINT:
            break;

        case SQL_REAL:
        case SQL_FLOAT:
        case SQL_DOUBLE:

            if (fCType == SQL_C_ULONG)
            {
                if (floatValue < 0 || floatValue > 4294967295.0)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
            }
            else
            {
                if (floatValue < -2147483648.0 || floatValue > 2147483647.0)
                {
                    rc = CVT_OUT_OF_RANGE;
                    break;
                }
                if (floatValue < 0) floatRound = -0.5;
            }
            intValue = (SDWORD)(floatValue + floatRound);
            if ((SDOUBLE)intValue != floatValue) rc = CVT_TRUNCATION;
            break;

        default:

            rc = CVT_INVALID;
            break;

        }
        if (rc == CVT_SUCCESS || rc == CVT_TRUNCATION)
            if (fCType == SQL_C_ULONG)
                *(UDWORD*)pard->DataPtr = (UDWORD)intValue;
            else
                *(SDWORD*)pard->DataPtr = intValue;
        break;

    case SQL_C_FLOAT:

        switch (fSqlType)
        {
        case SQL_SMALLINT:
        case SQL_INTEGER:
        case SQL_TINYINT:
            realValue = (SFLOAT)intValue;
            MEcopy((PTR)&realValue, sizeof(SFLOAT), pard->DataPtr);
            break;

        case SQL_BIGINT:
            realValue = (SFLOAT)int64Value;
            MEcopy((PTR)&realValue, sizeof(SFLOAT), pard->DataPtr);
            *(SFLOAT*)pard->DataPtr = (SFLOAT)int64Value;
            break;

        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        
            rc = CvtCharFloat (&floatValue, rgbData, (UWORD)cbData);
            if (rc != CVT_SUCCESS) break;
            MEcopy((PTR)&floatValue, sizeof(SFLOAT), pard->DataPtr);
            *(SFLOAT*)pard->DataPtr = (SFLOAT)floatValue;
            break;

        case SQL_DECIMAL:
        case SQL_NUMERIC:
            rc = CvtCharFloat (&floatValue, szValue, (UWORD)cbData);
            if (rc != CVT_SUCCESS) break;
            /* falls through ... */
        case SQL_FLOAT:
        case SQL_DOUBLE:

            if (floatValue < -(FLT_MAX) || floatValue > FLT_MAX)
            {
                rc = CVT_OUT_OF_RANGE;
                break;
            }
            /* falls through ... */
        case SQL_REAL:

            realValue = (SFLOAT)floatValue;
            MEcopy((PTR)&realValue, sizeof(SFLOAT), pard->DataPtr);
            break;

        default:

            rc = CVT_INVALID;
            break;
        }
        break;

    case SQL_C_DOUBLE:

        switch (fSqlType)
        {
        case SQL_SMALLINT:
        case SQL_INTEGER:
        case SQL_TINYINT:

            *(SDOUBLE*)pard->DataPtr = (SDOUBLE)intValue;
            break;

        case SQL_BIGINT:

            *(SDOUBLE*)pard->DataPtr = (SDOUBLE)CvtBigFloat (rgbData);
            break;

        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
            rc = CvtCharFloat (&floatValue, rgbData, (UWORD)cbData);
            if (rc != CVT_SUCCESS) break;
            MEcopy((PTR)&floatValue, sizeof(SDOUBLE), pard->DataPtr);
            *(SDOUBLE*)pard->DataPtr = (SDOUBLE)floatValue;
            break;
        
        case SQL_DECIMAL:
        case SQL_NUMERIC:

            rc = CvtCharFloat (&floatValue, szValue, (UWORD)cbData);
            if (rc != CVT_SUCCESS) break;
            /* falls through ... */
        case SQL_FLOAT:
        case SQL_DOUBLE:
        case SQL_REAL:

            MEcopy((PTR)&floatValue, sizeof(SDOUBLE), pard->DataPtr);
            break;

        default:

            rc = CVT_INVALID;
            break;
        }
        break;

    case SQL_C_TYPE_DATE:

        switch (fSqlType)
        {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_TYPE_TIMESTAMP:

        if (fSqlType == SQL_CHAR && !isServerClassINGRES(pstmt->pdbcOwner))
        {
            /* convert date to yyyy-mm-dd format */
            rc = CvtChardateFormat(rgbData, (UWORD)cbData); 
              
            if (rc == CVT_SUCCESS)
                rc = CvtCharDate(&dateValue, rgbData, (UWORD)cbData);
    
            if (rc == CVT_SUCCESS)
            {
                ((DATE_STRUCT*)pard->DataPtr)->year  = dateValue.year;
                ((DATE_STRUCT*)pard->DataPtr)->month = dateValue.month;
                ((DATE_STRUCT*)pard->DataPtr)->day   = dateValue.day;
            }    
           break;
        }
        else
            rc = CvtCharTimestamp (&stampValue, rgbData, (UWORD)cbData, pstmt->fAPILevel);
            if (rc == CVT_SUCCESS)
            {
                ((DATE_STRUCT*)pard->DataPtr)->year  = stampValue.year;
                ((DATE_STRUCT*)pard->DataPtr)->month = stampValue.month;
                ((DATE_STRUCT*)pard->DataPtr)->day   = stampValue.day;

                if (stampValue.hour   || stampValue.minute ||
                    stampValue.second || stampValue.fraction)
                    rc = CVT_TRUNCATION;
                break;
            }
            
        case SQL_TYPE_DATE:
            rc = CvtCharDate ((DATE_STRUCT*)pard->DataPtr, rgbData, 10);
            
            break;

        default:
            rc = CVT_INVALID;
            break;
        }
        break;

    case SQL_C_TYPE_TIME:

        switch (fSqlType)
        {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_TYPE_TIMESTAMP:

            rc = CvtCharTimestamp (&stampValue, rgbData, (UWORD)cbData, 
                pstmt->fAPILevel);
            if (rc == CVT_SUCCESS)
            {
                ((ISO_TIME_STRUCT*)pard->DataPtr)->hour   = stampValue.hour;
                ((ISO_TIME_STRUCT*)pard->DataPtr)->minute = stampValue.minute;
                ((ISO_TIME_STRUCT*)pard->DataPtr)->second = stampValue.second;

                if (pstmt->fAPILevel < IIAPI_LEVEL_4)
                {
                    if (stampValue.fraction)
                        rc = CVT_TRUNCATION;
                }
                else
                    ((ISO_TIME_STRUCT*)pard->DataPtr)->fraction = 
                        stampValue.fraction;

                break;
            }

        case SQL_TYPE_TIME:

            rc = CvtCharTime ((ISO_TIME_STRUCT*)pard->DataPtr, rgbData, 
                (UWORD)cbData, pstmt->fAPILevel);
            *pard->IndicatorPtr = sizeof(SQL_TIME_STRUCT);
            break;

        default:

            rc = CVT_INVALID;
            break;
        }
        break;

    case SQL_C_TYPE_TIMESTAMP:

        switch (fSqlType)
        {
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_TYPE_TIMESTAMP:

            rc = CvtCharTimestamp (
                (TIMESTAMP_STRUCT*)pard->DataPtr, rgbData, (UWORD)cbData, 
                     pstmt->fAPILevel);
            if (rc == CVT_SUCCESS) break;

        case SQL_TYPE_DATE:

            rc = CvtCharDate (&dateValue, rgbData, (UWORD)cbData);
            if (rc == CVT_SUCCESS)
            {
                TIMESTAMP_STRUCT* tm = (TIMESTAMP_STRUCT*)pard->DataPtr;
                tm->year     = dateValue.year;
                tm->month    = dateValue.month;
                tm->day      = dateValue.day;
                tm->hour     = 0;
                tm->minute   = 0;
                tm->second   = 0;
                tm->fraction = 0;

                break;
            }

        case SQL_TYPE_TIME:

            rc = CvtCharTime (&timeValue, rgbData, (UWORD)cbData, pstmt->fAPILevel);
            if (rc == CVT_SUCCESS)
            {
                TIMESTAMP_STRUCT* tm = (TIMESTAMP_STRUCT*)pard->DataPtr;
                CvtCurrStampDate (tm, pstmt->fAPILevel);
                tm->hour     = timeValue.hour;
                tm->minute   = timeValue.minute;
                tm->second   = timeValue.second;
                tm->fraction = 0;
            }
            break;

        default:

            rc = CVT_INVALID;
            break;
        }
        break;

    case SQL_C_NUMERIC:

        ns = (SQL_NUMERIC_STRUCT *)pard->DataPtr; 
        MEfill(sizeof(SQL_NUMERIC_STRUCT), 0, ns);
        switch (fSqlType)
        {
        case SQL_NUMERIC:
        case SQL_DECIMAL:

            CvtCharNumStr (szSqlNum, DB_MAX_DECPREC+2, szValue, (UWORD)cbData, 
                NULL, NULL);
            CvtStrSqlNumeric( ns, pird, cbData, szSqlNum );
            break;

        case SQL_WCHAR:
        case SQL_WVARCHAR:
        case SQL_WLONGVARCHAR:
        case SQL_CHAR:
        case SQL_VARCHAR:
        case SQL_LONGVARCHAR:
        {
            char *p,*termSpace;
            if (pstmt->pdbcOwner->fOptions & OPT_DECIMALPT)
            {
                p = STindex(rgbData, ",", cbData);
                if (p != NULL)
                    *p = '.'; 
            }
            CvtCharNumStr (szSqlNum, min(pird->Precision, MAX_NUM_DIGITS+2), 
                rgbData, (UWORD)cbData, NULL, NULL);
            CvtStrSqlNumeric( ns, pird, cbData, szSqlNum );
            /*
            **  Long varchar types have no implicit precision.  Fudge this
            **  from the size of the data.
            */
            if (fSqlType == SQL_LONGVARCHAR)
                ns->precision = cbLData;
            else if (fSqlType == SQL_WLONGVARCHAR)
                ns->precision = cbLData/sizeof(SQLWCHAR);
            /*
            **  Determine the scale from the input string.
            */
            if (fSqlType == SQL_CHAR)
                termSpace = STindex(szSqlNum, " ", 0);
            p = STindex (szSqlNum, ".", 0);
            if (p == NULL)
                ns->scale = 0;
            else
            {
                CMnext(p);
                if (fSqlType == SQL_CHAR && termSpace != NULL)
                    ns->scale = termSpace - p;
                else
                    ns->scale = STlength(p);
            }
            break;
        }
        case SQL_INTEGER:
        case SQL_SMALLINT:
        case SQL_TINYINT:

            ns->precision = 0;
            ns->sign = intValue >= 0 ? 1 : 0;
            if (fSqlType == SQL_INTEGER)
                *((SDWORD *)ns->val) = intValue;
            else if (fSqlType == SQL_SMALLINT)
                *((SWORD *)ns->val) = (SWORD)intValue;
            else if (fSqlType == SQL_TINYINT)
                *((SCHAR *)ns->val) = (SCHAR)intValue;
            break;

        case SQL_BIGINT:
        
            ns->precision = 0;
            ns->sign = int64Value >= 0 ? 1 : 0;
            *((ODBCINT64 *)ns->val) = int64Value;
            break;

        case SQL_REAL:
        case SQL_FLOAT:
        case SQL_DOUBLE:

            cbData = CvtDoubleToChar(floatValue, (UCHAR)(fSqlType == 
                SQL_REAL ? 7 : 15), szValue,'.');
            CvtStrSqlNumeric( ns, pird, cbData, szValue );
            break;

        default:

            rc = CVT_INVALID;
            break;
        }
        break;

    case SQL_C_INTERVAL_YEAR_TO_MONTH:
    case SQL_C_INTERVAL_YEAR:
    case SQL_C_INTERVAL_MONTH:
    case SQL_C_INTERVAL_DAY:
    case SQL_C_INTERVAL_HOUR:
    case SQL_C_INTERVAL_MINUTE:
    case SQL_C_INTERVAL_SECOND:
    case SQL_C_INTERVAL_DAY_TO_HOUR:
    case SQL_C_INTERVAL_DAY_TO_MINUTE:
    case SQL_C_INTERVAL_DAY_TO_SECOND:
    case SQL_C_INTERVAL_HOUR_TO_MINUTE:
    case SQL_C_INTERVAL_HOUR_TO_SECOND:
    case SQL_C_INTERVAL_MINUTE_TO_SECOND:
    {
        SQL_INTERVAL_STRUCT *intv = (SQL_INTERVAL_STRUCT *)pard->DataPtr; 
        rc = CvtCharInterval(rgbData, &intervalValue, ptypeC->fSqlType);
        intv->interval_sign = intervalValue.interval_sign;
        intv->interval_type = intervalValue.interval_type;
        if (fCType == SQL_C_INTERVAL_YEAR || fCType == SQL_C_INTERVAL_MONTH ||
            fCType == SQL_C_INTERVAL_YEAR_TO_MONTH)
        {
            intv->intval.year_month.year = intervalValue.intval.year_month.year;
            intv->intval.year_month.month = intervalValue.intval.year_month.month;
        }
        else
        {
            intv->intval.day_second.day = intervalValue.intval.day_second.day;
            intv->intval.day_second.hour = intervalValue.intval.day_second.hour;
            intv->intval.day_second.minute = 
                intervalValue.intval.day_second.minute;
            intv->intval.day_second.second = 
                intervalValue.intval.day_second.second;
            intv->intval.day_second.fraction = 
                 intervalValue.intval.day_second.fraction;
        }
        break;
    }

    default:

        rc = CVT_INVALID;
        break;
    }

    /*
    **  Return results:
    */
    pstmt->fCvtError |= rc;
    switch (rc)
    {
    case CVT_SUCCESS:

        return SQL_SUCCESS;

    case CVT_TRUNCATION:

        return SQL_SUCCESS_WITH_INFO;

    default:

        if (pard->IndicatorPtr)     /* SQLBindCol(pcbValue) */
            *pard->IndicatorPtr = IndicatorSavedInCaseOfError;
        if (pard->OctetLengthPtr)     /* SQLBindCol(pcbValue) */
            *pard->OctetLengthPtr = cbValueSavedInCaseOfError;
        return SQL_ERROR;
    }
}


/*
**  GetBinChar
**
**  Return binary as character data.
**
**  On entry: pard   -->bind column block.
**            rgbData-->data to copy from.
**            cbData  = length of data to copy.
**
**  On exit:  Data is converted to hex display and
**            copied to buffer, possibly truncated.
**            Bind column block is updated with length returned.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATION        Data truncated.
*/

static UDWORD GetBinChar(
    LPDESCFLD   pard,
    CHAR     *  rgbData,
    SQLINTEGER      cbData)
{
    CHAR     *  rgbCopy;
    SQLINTEGER      cbCopy, cbLeft;

    rgbCopy = rgbData + pard->cbPartOffset;
    cbLeft  = cbData  - pard->cbPartOffset;
    cbCopy  = UMIN (cbLeft, (pard->OctetLength - 1) / 2);

    CvtBinChar (pard->DataPtr, rgbCopy, (UWORD)cbCopy);  /* SQLBindCol(rgbValue)*/

    pard->cbPartOffset += cbCopy;
    if (pard->OctetLengthPtr)   /* id SQLBindCol(pcbValue)*/
       *pard->OctetLengthPtr = (SDWORD)(cbLeft * 2);

    if (cbLeft - cbCopy > 0) 
        return (CVT_TRUNCATION);

    return (CVT_SUCCESS);
}


/*
**  GetBinWChar
**
**  Return binary as wide (Unicode) character data.
**
**  On entry: pard   -->bind column block.
**            rgbData-->data to copy.
**            cbData  = length of data to copy.
**
**  On exit:  Data is converted to hex display, then to wchar display and
**            copied to buffer, possibly truncated.
**            Bind column block is updated with length returned.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATION        Data truncated.
*/

UDWORD GetBinWChar(
    LPDESCFLD   pard,
    CHAR      * rgbData,
    SQLINTEGER      cbData)
{
    CHAR      * rgbCopy;
    SQLINTEGER      cbCopy, cbLeft;
    char      * pwork = NULL;
    char        workbuffer[1004];
    char      * szValue;
    SQLINTEGER      cbValueMax;     /* Length of column value buffer.
                                   from SQLBindCol(cbValueMax) */

    cbValueMax= (pard->OctetLength - sizeof(SQLWCHAR)) /
                                 (2*sizeof(SQLWCHAR));
    if (cbValueMax < 0)   /* safety check */
        cbValueMax = 0;

    rgbCopy = rgbData + pard->cbPartOffset;
    cbLeft  = cbData  - pard->cbPartOffset;
    cbCopy  = UMIN (cbLeft, cbValueMax);
                             /* number of binary bytes to convert */

    if (cbCopy < sizeof(workbuffer)-4)
       { szValue = workbuffer;   /* use the stack work buffer */
                                 /* for the char string */
       }
    else
        {
         szValue =
           pwork = MEreqmem(0, cbCopy+8, TRUE, NULL);
                     /* allocate work area for string plus another safety */
         if (pwork == NULL)  /* no memory!? */
            { szValue = workbuffer;   /* use the stack work buffer */
                                      /* for the char string */
              cbCopy = UMIN(cbCopy, sizeof(workbuffer)-4);
            }
        }

    CvtBinChar (szValue, rgbCopy, (UWORD)cbCopy);  /* bin  -> char  */
    GetCharWChar(pard, szValue, cbCopy*2);         /* char -> wchar */

    MEfree((PTR)pwork);                    /* free work string area */

    pard->cbPartOffset += cbCopy;  /* bump up offset past src data just moved */

    if (pard->OctetLengthPtr)                 /* return length in bytes */
       *pard->OctetLengthPtr = (SDWORD)(cbLeft * 2 * sizeof(SQLWCHAR));

    if (cbLeft - cbCopy > 0) 
        return (CVT_TRUNCATION);

    return (CVT_SUCCESS);
}


/*
**  GetBit
**
**  Return integer as bit data.
**
**  On entry: intValue = integer value.
**            rgbValue-->where to return bit value.
**
**  On exit:  Value returned if 0 or 1.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_OUT_OF_RANGE      Not 0 or 1.
*/
static UDWORD GetBit(
    CHAR     *  rgbValue,
    SDWORD      intValue,
    UDWORD      rc)
{
    if (intValue != 0 && intValue != 1)
        return (CVT_OUT_OF_RANGE);

    *(UCHAR     *)rgbValue = (UCHAR)intValue;

    return (rc);
}


/*
**  GetCharBin
**
**  Return character as binary data.
**
**  On entry: pard   -->bind column block.  (pard->DataPtr-> where to copy to)
**            rgbData-->data to copy.
**            cbData  = length of data to copy.
**
**  On exit:  Data is copied to buffer, possibly truncated.
**            Bind column block is updated with length moved.
**            Bind column block status is updated.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATION        Data truncated.
*/
static UDWORD GetCharBin(
    LPDESCFLD   pard,
    CHAR     *  rgbData,
    SQLINTEGER      cbData)
{
    CHAR     *  rgbCopy;
    SQLINTEGER      cbCopy, cbLeft;

    rgbCopy = rgbData + pard->cbPartOffset;
    cbLeft  = cbData  - pard->cbPartOffset;
    cbCopy  = UMIN (cbLeft, pard->OctetLength);  /* SQLBindCol(cbValueMax) */

    pard->cbPartOffset += cbCopy;

    if (pard->OctetLengthPtr)  /* SQLBindCol(cbValueMax) */
       *pard->OctetLengthPtr = (SDWORD)(cbLeft);
    if (pard->DataPtr != rgbData) /* SQLBindCol(rgbValue) */
        MEcopy (rgbCopy, (size_t)cbCopy, pard->DataPtr);

    if (cbLeft - cbCopy > 0) 
        return (CVT_TRUNCATION);

    return (CVT_SUCCESS);
}


/*
**  GetCharCharCommon
**
**  GetCharChar  and GetCharWCharCommon common code
**
**  On entry: pard   -->bind column block.  (pard->DataPtr-> where to copy to)
**            rgbData-->data to copy from.  (pard->cbPartOffset has displacement)
**            cbData  = length (in bytes) of data to copy (less displacement)
**            sizeofNullTerm = size of null-terminator
**
**  On exit:  Data is copied to buffer, possibly truncated, null terminated.
**            Bind column block is updated with length moved.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATION        Data truncated.
*/
UDWORD GetCharCharCommon(
    LPDESCFLD   pard,
    CHAR      * rgbData,
    SQLINTEGER      cbData,
    SIZE_TYPE   sizeofNullTerm)
{
    SQLINTEGER      OctetLengthMinusSizeofNullTerm;
    CHAR      * rgbCopy;
    SQLINTEGER      cbCopy, cbLeft;

    OctetLengthMinusSizeofNullTerm = pard->OctetLength - sizeofNullTerm;
    if (OctetLengthMinusSizeofNullTerm < 0)
        OctetLengthMinusSizeofNullTerm = 0;

    rgbCopy = rgbData + pard->cbPartOffset;  /* rgbCopy->where to copy from */
    cbLeft  = cbData  - pard->cbPartOffset;  /* cbLeft = length remaining   */
    cbCopy  = UMIN (cbLeft, OctetLengthMinusSizeofNullTerm);
 /* cbCopy  = length to move so it will fit in buffer with a null terminator  */

    if (pard->OctetLengthPtr)                 /* tell appl size (in bytes) of data */
       *pard->OctetLengthPtr = (SDWORD)(cbLeft);  /* moved (not counting null-term)*/

    if (pard->OctetLength <= 0)  /* if don't even have room for null */
        return (CVT_TRUNCATION); /*   terminator then raise truncation error */

      
    if (pard->DataPtr != rgbCopy)
        MEcopy(rgbCopy, (size_t)cbCopy, pard->DataPtr);

    if (sizeofNullTerm == sizeof(SQLWCHAR))
         *(((SQLWCHAR *)pard->DataPtr) + cbCopy/sizeof(SQLWCHAR)) = 0;
    else
         *(             pard->DataPtr  + cbCopy) = '\0';

    pard->cbPartOffset += cbCopy;  /* bump up offset past data just moved */
  
    if (cbLeft - cbCopy > 0)      /* if length remaining > length moved */
        return (CVT_TRUNCATION);  /* signal truncation */

    return (CVT_SUCCESS);

}

/*
**  GetCharChar
**
**  Return character as character data.
**
**  On entry: pard   -->bind column block.  (pard->DataPtr-> where to copy to)
**            rgbData-->data to copy.
**            cbData  = length (in bytes) of data to copy.
**
**  On exit:  Data is copied to buffer, possibly truncated, null terminated.
**            Bind column block is updated with length moved.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATION        Data truncated.
*/
UDWORD GetCharChar(
    LPDESCFLD   pard,
    CHAR      * rgbData,
    SQLINTEGER      cbData)
{
    return GetCharCharCommon(pard, rgbData, cbData, sizeof(SQLCHAR));
}

/*
**  GetWCharWChar
**
**  Return wide character as wide character data.
**
**  On entry: pard   -->bind column block.  (pard->DataPtr-> where to copy to)
**            rgbData-->data to copy from.  (pcol->cbPartOffset has displacement)
**            cbData  = length (in bytes) of data to copy (less displacement)
**
**  On exit:  Data is copied to buffer, possibly truncated, null terminated.
**            Bind column block is updated with length moved.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATION        Data truncated.
*/
UDWORD GetWCharWChar(
    LPDESCFLD   pard,
    CHAR      * rgbData,
    SQLINTEGER      cbData)
{
    return GetCharCharCommon(pard, rgbData, cbData, sizeof(SQLWCHAR));
}

/*
**  GetCharWChar
**
**  Return character as wide character data.
**
**  On entry: pard   -->bind descrptr block. (pard->DataPtr-> where to copy to)
**            rgbData-->data to copy from.   (pard->cbPartOffset has displacement)
**            cbData  = length (in bytes) of data to copy (less displacement)
**
**  On exit:  Data is copied to buffer, possibly truncated, null terminated.
**            Bind column block is updated with length moved.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATION        Data truncated.
*/
UDWORD GetCharWChar(
    LPDESCFLD   pard,
    CHAR      * rgbData,
    SQLINTEGER      cbData)
{
    SQLINTEGER      OctetLengthMinusSizeofNullTerm;
    CHAR      * rgbCopy;
    SQLINTEGER      cbCopy, cbLeft;
    RETCODE     rc;

    OctetLengthMinusSizeofNullTerm = (pard->OctetLength / sizeof(SQLWCHAR)) - sizeof(SQLCHAR);
    if (OctetLengthMinusSizeofNullTerm < 0)
        OctetLengthMinusSizeofNullTerm = 0;

    rgbCopy = rgbData + pard->cbPartOffset;  /* rgbCopy->where to copy from */
    cbLeft  = (SQLUINTEGER)cbData  - pard->cbPartOffset;  /* cbLeft = length (bytes) remaining */
    cbCopy  = UMIN (cbLeft, OctetLengthMinusSizeofNullTerm);
 /* cbCopy  = length to move so it will fit in buffer with a null terminator  */

    if (pard->OctetLengthPtr)                 /* tell appl size (in bytes) of data */
       *pard->OctetLengthPtr =                /* moved (not counting null-term)*/
                       (SDWORD)(cbLeft * sizeof(SQLWCHAR));

    rc = ConvertCharToWChar(NULL,
              rgbCopy,          /* source */
              cbCopy,           /* source length*/
  (SQLWCHAR *)pard->DataPtr,    /* target */
              pard->OctetLength,/* max target length (in SQLWCHAR chars)*/
              NULL, NULL, NULL); /* return length in chars */

    if (rc != SQL_SUCCESS  &&  rc != SQL_SUCCESS_WITH_INFO)
        return CVT_ERROR;   /* some kind of char to wchar conversion error */

    if (rc == SQL_SUCCESS_WITH_INFO)  /* if warning then must be */
        return (CVT_TRUNCATION);      /* truncation warning */

    pard->cbPartOffset += cbCopy;  /* bump up offset past src data just moved */
    if ( pard->cbPartOffset < (SQLUINTEGER)cbData)      /* if length remaining > length moved */
        return (CVT_TRUNCATION);  /* signal truncation */

    return (CVT_SUCCESS);

}

/*
**  GetWCharChar
**
**  Return wide character as character data.
**
**  On entry: pard   -->bind descptr block. (pard->DataPtr-> where to copy to)
**            rgbData-->data to copy from.  (pard->cbPartOffset has displacement)
**            cbData  = length (in bytes) of data to copy (less displacement)
**            multibyteFillChar-->if non-null, specifies fill character for
**                                failed conversions.
**
**  On exit:  Data is copied to buffer, possibly truncated, null terminated.
**            Bind column block is updated with length moved.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATION        Data truncated.
*/
UDWORD GetWCharChar(
    LPDESCFLD   pard,
    CHAR      * rgbData,
    SQLINTEGER      cbData,
    CHAR        multibyteFillChar)
{
    SQLINTEGER      OctetLengthMinusSizeofNullTerm;
    SQLWCHAR  * rgbCopy;
    SQLINTEGER  cbCopy, cbLeft, cbCopyWChars;
    RETCODE     rc, ret;

    OctetLengthMinusSizeofNullTerm = pard->OctetLength - sizeof(SQLCHAR);
    if (OctetLengthMinusSizeofNullTerm < 0)
        OctetLengthMinusSizeofNullTerm = 0;

    rgbCopy = (SQLWCHAR*)
             (rgbData + pard->cbPartOffset); /* rgbCopy->where to copy from */
    cbLeft  = cbData  - pard->cbPartOffset;  /* length in bytes remaining */

    /* 
    ** cbCopy is the length in bytes to move so it will fit in buffer with 
    ** a null terminator.  cbCopyWChars is the corresponding number
    ** expressed as the number of wide characters.
    */
    cbCopy  = UMIN (cbLeft, OctetLengthMinusSizeofNullTerm*sizeof(SQLWCHAR));
    cbCopyWChars = cbCopy / sizeof(SQLWCHAR);

    if (pard->OctetLengthPtr) /* tell appl size (in bytes) of data */
       *pard->OctetLengthPtr =   /* moved (not counting null-term)*/
                       (SDWORD)(cbLeft / sizeof(SQLWCHAR));

    rc = ConvertWCharToChar(NULL,   /* wchar -> char */
              rgbCopy,              /* source */
              cbCopyWChars,         /* source length in Wchar */
              pard->DataPtr,        /* target */
              pard->OctetLength,    /* max target buffer length */
              NULL,
              NULL,
              pard->IndicatorPtr);  /* return length in bytes */

    if (!SQL_SUCCEEDED(rc))
    {
        if ( multibyteFillChar && multibyteFillChar != ' ' )
        {
            int i;
            SQLCHAR *s = pard->DataPtr;
            SQLWCHAR *t = rgbCopy; 
            for (i = 0; i < cbCopyWChars; i++)
            {
                ret = ConvertWCharToChar(NULL,   /* wchar -> char */
                           t,                    /* source */
                           1,                    /* source length in char*/
                           s,                    /* target */
                           pard->OctetLength,    /* max target buffer length */
                           NULL,
                           NULL,
                           NULL);  

                if (!SQL_SUCCEEDED(ret))
                    *s = multibyteFillChar;
                CMnext(s);
                CMnext(t);
            }
            *s = '\0';
            return SQL_SUCCESS;  
        }
        else
            return CVT_ERROR;   /* wchar to char conversion error */
    }

    pard->cbPartOffset += cbCopy;  /* bump up offset past src data just moved */

    if (rc == SQL_SUCCESS_WITH_INFO)  /* if warning then must be */
        return (CVT_TRUNCATION);      /* truncation warning */

    return (CVT_SUCCESS);

}


/*
**  GetNumBin
**
**  Return numeric as binary data.
**
**  On entry: pard   -->bind column block.
**            rgbData-->data to copy.
**            cbData  = length of data to copy.
**
**  On exit:  Data is copied to buffer if not trucated.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_OUT_OF_RANGE.     Data would be truncated.
*/
static UDWORD GetNumBin(
    LPDESCFLD   pard,
    CHAR     *  rgbData,
    SQLINTEGER      cbData)
{
    if (pard->OctetLengthPtr)  /* if SQLBindCol(pcbValue)*/
       *pard->OctetLengthPtr = cbData;

    if ((SDWORD)cbData > pard->OctetLength)  /* SQLBindCol(cbValueMax)*/
        return (CVT_OUT_OF_RANGE);

    MEcopy (rgbData, cbData, pard->DataPtr);  /* SQLBindCol(rgbValue)*/

    return (CVT_SUCCESS);
}


/*
**  GetNumChar
**
**  Return numeric as character data.
**
**  On entry: pard   -->bind column block.
**            rgbData-->data to copy.
**            cbData  = length of data to copy.
**
**  On exit:  Data is copied to buffer if not out of range.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATED.        Data truncated.
**            CVT_OUT_OF_RANGE.     Data out of range.
*/
static UDWORD GetNumChar(
    LPDESCFLD   pard,
    CHAR      * rgbData,
    SQLINTEGER      cbData,
    UDWORD      apiLevel,
    SWORD       fSqlType)
{
    CHAR      * rgb;
    SQLINTEGER      cb    = cbData;
    SQLINTEGER      cbMax = UMIN(pard->OctetLength,  /* SQLBindCol(cbValueMax)*/
                              65535) - 1;
    UDWORD      rc    = CVT_SUCCESS;

    if (pard->OctetLengthPtr)  /* if SQLBindCol(pcbValue)*/
       *pard->OctetLengthPtr = cb;

    if (fSqlType == SQL_TYPE_TIMESTAMP)
    {
        STtrmwhite(rgbData);
        cb = STlength(rgbData);
    }

    switch (fSqlType)
    {
    case SQL_INTERVAL_YEAR:
    case SQL_INTERVAL_MONTH:
    case SQL_INTERVAL_YEAR_TO_MONTH:
    case SQL_INTERVAL_DAY:
    case SQL_INTERVAL_HOUR:
    case SQL_INTERVAL_MINUTE:
    case SQL_INTERVAL_SECOND:
    case SQL_INTERVAL_DAY_TO_HOUR:
    case SQL_INTERVAL_DAY_TO_MINUTE:
    case SQL_INTERVAL_DAY_TO_SECOND:
    case SQL_INTERVAL_HOUR_TO_MINUTE:
    case SQL_INTERVAL_HOUR_TO_SECOND:
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        break;

    default:
        if (cb > cbMax)
        {
            rgb = rgbData + cbData;
    
            CMprev(rgb, rgbData);
            while (cb && CMdigit(rgb))
            {
                CMprev(rgb, rgbData);
                cb--;
            }
    
            if (cb == 0 || CMcmpcase(rgb,".") || --cb > cbMax)
                return (CVT_OUT_OF_RANGE);
            cb = cbMax;
            rc = CVT_TRUNCATION;
        }
        break;
    }
    STlcopy (rgbData, pard->DataPtr, cb);  /* SQLBindCol(rgbValue)*/
    return (rc);
}


/*
**  GetNumWChar
**
**  Return numeric as wide (Unicode) character data.
**
**  On entry: pcol   -->bind column block.
**            rgbData-->data to copy.
**            cbData  = length of data to copy.
**
**  On exit:  Data is copied to buffer if not out of range.
**
**  Returns:  CVT_SUCCESS           Converted successfully.
**            CVT_TRUNCATED.        Data truncated.
**            CVT_OUT_OF_RANGE.     Data out of range.
*/
static UDWORD GetNumWChar(
    LPDESCFLD   pard,
    CHAR      * rgbData,
    SQLINTEGER      cbData)
{
    CHAR      * rgb;
    SQLINTEGER      cb    = cbData;
    SQLINTEGER      cbMax = UMIN(pard->OctetLength,  /* SQLBindCol(cbValueMax)*/
                              65535) - 1;
    UDWORD      rc    = CVT_SUCCESS;

    if (cb > cbMax)
    {
        rgb = rgbData + cbData;

        /*while (cb && isdigit (*(--rgb))) */
        CMprev(rgb, rgbData);
        while (cb && CMdigit(rgb))
        {
                CMprev(rgb, rgbData);
                cb--;
        }
        if (cb == 0 || CMcmpcase(rgb,".") || --cb > cbMax)
            return (CVT_OUT_OF_RANGE);
    }

    rc = GetCharWChar(pard, rgbData, cbData);

    return (rc);
}


/*
**  GetNumFrac
**
**  Ensure that decimal and numeric type has complete fraction.
**  CvtCharNumStr strips all trailing 0's, this undoes that if necessary.
**
**  On entry: szNum   -->numeric value.
**            cbNumMax = length buffer containing value.
**            cbScale  = scale desired.
**
**  On exit:  Decimal and 0's added as needed.
**
**  Returns:  Nothing (assumes buffer large enough, it should be...)
*/
static VOID GetNumFrac(
    LPSTR       szNum,
    UWORD       cbNumMax,
    UWORD       cbScale)
{
    CHAR     *  p;
    UWORD       cb;

    if (cbScale == 0)
        return;

    cb = (UWORD)STlength (szNum);
    MEfill (cbNumMax - cb, 0, szNum + cb );

    p = STindex (szNum, ".", 0);
    if (p == NULL)
    {
        p = szNum + cb;
        CMcpychar(".",p); 
    }
    CMnext(p); 

    while (cbScale-- > 0 && cb < cbNumMax)
    {
        if (!*p)
        {
            CMcpychar("\0",p); 
            cb++;
        }
        CMnext(p); 
    }

    return;
}

static void CvtMonthName(CHAR     * t, CHAR FAR * p)
{
    /*if (!strnicmp(p,"jan",3)) */
    if (!STbcompare(p,3,"jan",3,TRUE))  
        MEcopy("01",2,t);
    else if (!STbcompare(p,3,"feb",3,TRUE)) 
        MEcopy("02",2,t);
    else if (!STbcompare(p,3,"mar",3,TRUE)) 
        MEcopy("03",2,t);
    else if (!STbcompare(p,3,"apr",3,TRUE)) 
        MEcopy("04",2,t);
    else if (!STbcompare(p,3,"may",3,TRUE)) 
        MEcopy("05",2,t);
    else if (!STbcompare(p,3,"jun",3,TRUE)) 
        MEcopy("06",2,t);
    else if (!STbcompare(p,3,"jul",3,TRUE)) 
        MEcopy("07",2,t);
    else if (!STbcompare(p,3,"aug",3,TRUE)) 
        MEcopy("08",2,t);
    else if (!STbcompare(p,3,"sep",3,TRUE)) 
        MEcopy("09",2,t);
    else if (!STbcompare(p,3,"oct",3,TRUE)) 
        MEcopy("10",2,t);
    else if (!STbcompare(p,3,"nov",3,TRUE)) 
        MEcopy("11",2,t);
    else if (!STbcompare(p,3,"dec",3,TRUE)) 
        MEcopy("12",2,t);
    else
        MEcopy("??",2,t);
}
 
static UDWORD CvtIngresDateToChar(LPDBC pdbc, CHAR     * rgbData, LPDESCFLD pird)
{
    UDWORD      rc = CVT_SUCCESS;
    IIAPI_CONVERTPARM   cv;
    CHAR      * p;
    CHAR      * t;
    ODBC_DATENTRNL * d = (ODBC_DATENTRNL *)rgbData;
    ODBC_ADATE     * dt = (ODBC_ADATE *)rgbData;
    ODBC_TIMESTAMP *ts = (ODBC_TIMESTAMP *)rgbData;
    II_INT2     dn_year;
    II_INT2     datelen;
    LPSQLTYPEATTR ptype;

    ptype = CvtGetSqlType(pird->ConciseType, pdbc->fAPILevel);

    if (pird->fIngApiType == IIAPI_DTE_TYPE && pdbc->fAPILevel > IIAPI_LEVEL_3)
    {
        cv.cv_srcDesc.ds_dataType = pird->fIngApiType;
        ptype->fIngApiType = IIAPI_DTE_TYPE;
    }
    else 
        cv.cv_srcDesc.ds_dataType   = ptype->fIngApiType;
    
    if (ptype->fIngApiType == IIAPI_DTE_TYPE)
    {
        if (d->date_spec == ODBC_DATE_NULL) /* blank empty date */
        {
            if (pdbc->fOptions & OPT_BLANKDATEisNULL)
                STcopy("", rgbData);  /* return blank date as NULL */
            else
                STcopy("9999-12-31 23:59:59", rgbData); /* Use default*/
            return(rc);
        }
    
        if (pdbc->fOptions & OPT_DATE1582isNULL  &&
            d->date_spec & (ODBC_YEARSPEC | ODBC_MONTHSPEC | ODBC_DAYSPEC)  &&
            d->year  == 1582  &&  /* date == 1582-01-01 00:00:00 for MK?*/
            d->month == 01    &&
            d->lowday== 01)
        {
           STcopy("", rgbData);  /* return date 1582-01-01 as NULL */
           return(rc);
        }
        dn_year=d->year;
    }
    else if (pird->fIngApiType == IIAPI_DATE_TYPE)
        dn_year = dt->year;
    else
        dn_year = ts->year;
    
    cv.cv_srcDesc.ds_nullable   = pird->Nullable==SQL_NULLABLE?TRUE:FALSE; 
    cv.cv_srcDesc.ds_length     = (II_UINT2) pird->OctetLength; 
    cv.cv_srcDesc.ds_precision  = 0;
    cv.cv_srcDesc.ds_scale      = 0;
    cv.cv_srcDesc.ds_columnType = IIAPI_COL_TUPLE;
    cv.cv_srcDesc.ds_columnName = pird->Name; 
    
    cv.cv_srcValue.dv_null      = FALSE; 
    cv.cv_srcValue.dv_length    = (II_UINT2) pird->OctetLength;
    cv.cv_srcValue.dv_value     = rgbData; 
      
    cv.cv_dstDesc.ds_dataType   = IIAPI_CHA_TYPE;
    cv.cv_dstDesc.ds_nullable   = FALSE;
    cv.cv_dstDesc.ds_length     = (II_UINT2) pird->OctetLength; 
    cv.cv_dstDesc.ds_precision  = 0;
    cv.cv_dstDesc.ds_scale      = 0;
    cv.cv_dstDesc.ds_columnType = IIAPI_COL_TUPLE;
    cv.cv_dstDesc.ds_columnName = NULL;
    
    cv.cv_dstValue.dv_null      = FALSE;
    cv.cv_dstValue.dv_length    = (II_UINT2) pird->OctetLength;
    cv.cv_dstValue.dv_value     = MEreqmem(0, (u_i4)(cv.cv_dstValue.dv_length+1), TRUE, NULL);
    IIapi_convertData(&cv);  /* convert GMT struct to local char */
    
    if (cv.cv_status != IIAPI_ST_SUCCESS)
    {
        MEfree((PTR)cv.cv_dstValue.dv_value);
        return(CVT_ERROR);
    }
    MEfill(cv.cv_dstValue.dv_length,' ',rgbData);
    
    /* 
    ** Convert OpenAPI date format, 19-mar-1997 09:16:21 
    ** to ODBC date format, 1997-03-19 09:16:21.
    */
    *(rgbData+4) = '-';
    *(rgbData+7) = '-';

    if (ptype->fIngApiType == IIAPI_DTE_TYPE)
        *(rgbData+19) = '\0';
    else if (ptype->fIngApiType == IIAPI_DATE_TYPE)
        *(rgbData+10) = '\0';

    t = p = (CHAR     *) cv.cv_dstValue.dv_value;
    
    /* 
    ** The date format returned is dependent on II_DATE_FORMAT,
    ** even though we ask API for SWEDEN/FINISH format always.
    **
    ** Convert to ODBC date format, yyyy-mm-dd hh:mm:ss.  
    */
    
    while (*t  &&  *t != ' ')  t++;  /* peek at the time */
        datelen=(II_INT2)(t-p);
    if (datelen > 10)        /* use this later if unknown format */
        datelen = 10;
    while (*t  &&  *t == ' ')  t++;
    if (*t)                    /* if DATE field has time portion */
        MEcopy(t,8,rgbData+11);          /* fill in time portion */
    else if (ptype->fIngApiType == IIAPI_DTE_TYPE)
        MEcopy("00:00:00",8,rgbData+11); /* no time, use 00:00:00 */
    
    if (p[2]=='-')   /* convert US or DMY date format, dd-mmm-yyyy */
    { 
        MEcopy(p+7,4,rgbData+0);     /* yyyy */
        CvtMonthName(rgbData+5,p+3); /* mm   */
        MEcopy(p+0,2,rgbData+8);     /* dd   */
    }
    else if (p[2]=='/'  ||  p[2]=='.')  /* MULTINATIONALx or GERMAN */ 
    {   /* convert MULTINATIONAL date format, dd/mm/yy */
        if (CMspace(p+8))
        { 
            MEcopy(p+6,2,rgbData+2);         /* ??yy */
            /* 
            ** if dn_year==1999 GMT, but ADF returned "00" then
            ** assume local time already crossed into 2000 and
            ** wish that they have used MULTINATIONAL4 instead 
            */
             if (dn_year == 1999  &&  MEcmp(p+6,"00",2)==0)
                  MEcopy("20",2,rgbData+0);
             else
             if (dn_year < 2000)  /* use orig yyyy to avoid Y2K problem */
                  MEcopy("19",2,rgbData+0);
             else MEcopy("20",2,rgbData+0);
        }
        else /* convert MULTINATIONAL4 date format, dd/mm/yyyy */
             /* convert GERMAN         date format, dd.mm.yyyy */
            MEcopy(p+6,4,rgbData+0);         /* yyyy */
        
        MEcopy(p+3,2,rgbData+5);              /* mm   */
        MEcopy(p+0,2,rgbData+8);              /* dd   */
    }
    else if (CMalpha(p+0))   /* convert MDY date format, mmm-dd-yyyy */
    {
        MEcopy(p+7,4,rgbData+0);     /* yyyy */
        CvtMonthName(rgbData+5,p+0); /* mm   */
        MEcopy(p+4,2,rgbData+8);     /* dd   */
    }
    else if (CMalpha(p+5))   /* convert YMD date format, yyyy-mmm-dd */
    {
        MEcopy(p+0,4,rgbData+0);     /* yyyy */
        CvtMonthName(rgbData+5,p+5); /* mm   */
        MEcopy(p+9,2,rgbData+8);     /* dd   */
    }
    else
    if (p[4]=='-')      /* convert SWEDEN/FINLAND date format, yyyy-mm-dd */
    {
        MEcopy(p+0,10,rgbData+0);    /* yyyy-mm-dd */
    }
    else
    if (p[6]==' ')      /* convert ISO date format, yymmdd */
    {
        MEcopy(p+0,2,rgbData+2);         /* ??yy */
        /* 
        ** If dn_year==1999 GMT, but ADF returned "00", assume
        ** the local time has already crossed into 2000 and
        ** wish that they have used MULTINATIONAL4 instead.
        */
        if (dn_year == 1999  &&  MEcmp(p+0,"00",2)==0)
            MEcopy("20",2,rgbData+0);
        else
        if (dn_year < 2000)  /* use orig yyyy to avoid Y2K problem */
             MEcopy("19",2,rgbData+0);
        else MEcopy("20",2,rgbData+0);
        MEcopy(p+2,2,rgbData+5);         /* mm   */
        MEcopy(p+4,2,rgbData+8);         /* dd   */
    }
    else if (datelen)
        MEcopy(p+0,datelen,rgbData+0); /* unknown format, just copy it */
    
    MEfree(cv.cv_dstValue.dv_value);
    return(rc); 
}

static UDWORD CvtIsoTimestampToChar(LPDBC pdbc, CHAR * rgbData, LPDESCFLD pird)
{
    UDWORD      rc = CVT_SUCCESS;
    II_UINT2 srcLen;
    II_UINT2 dstLen;
    IIAPI_FORMATPARM fp;
    CHAR *p = NULL;

    srcLen = (II_UINT2)pird->OctetLength;

    switch (pird->fIngApiType)
    {
        case IIAPI_TS_TYPE:
        case IIAPI_TSWO_TYPE:
            dstLen = ODBC_TSWO_OUTLENGTH+1;
            break;

        case IIAPI_TSTZ_TYPE:
            dstLen = ODBC_TSTZ_OUTLENGTH+1;
            break;

        case IIAPI_DTE_TYPE:
            srcLen = ODBC_DTE_LEN;
            dstLen = ODBC_DTE_OUTLENGTH+1;
            break;
    }

    fp.fd_envHandle = pdbc->penvOwner->envHndl;
    fp.fd_srcDesc.ds_dataType   = pird->fIngApiType; 
    fp.fd_srcDesc.ds_nullable   = FALSE;  
    fp.fd_srcDesc.ds_length     = (II_UINT2) srcLen;
    fp.fd_srcDesc.ds_precision  = (II_UINT2) pird->IsoTimePrecision;
    fp.fd_srcDesc.ds_scale      = 0;
    fp.fd_srcDesc.ds_columnType = IIAPI_COL_TUPLE;
    fp.fd_srcDesc.ds_columnName = NULL;  
    fp.fd_srcValue.dv_null      = FALSE; 
    fp.fd_srcValue.dv_length    = (II_UINT2) srcLen;
    fp.fd_srcValue.dv_value     = rgbData; 
          
    fp.fd_dstDesc.ds_dataType   = IIAPI_CHA_TYPE; 
    fp.fd_dstDesc.ds_nullable   = FALSE;
    fp.fd_dstDesc.ds_length     = (II_UINT2) dstLen;
    fp.fd_dstDesc.ds_precision  = (II_UINT2) pird->IsoTimePrecision;
    fp.fd_dstDesc.ds_scale      = (II_UINT2) 0;
    fp.fd_dstDesc.ds_columnType = IIAPI_COL_TUPLE;
    fp.fd_dstDesc.ds_columnName = NULL;
    fp.fd_dstValue.dv_null      = FALSE;
    fp.fd_dstValue.dv_length    = (II_UINT2) dstLen;
    fp.fd_dstValue.dv_value     = MEreqmem(0, (u_i4)
        (fp.fd_dstValue.dv_length+1), TRUE, NULL);
    IIapi_formatData( &fp );
    
    if (fp.fd_status != IIAPI_ST_SUCCESS)
    {
        MEfree((PTR)fp.fd_dstValue.dv_value);
        return(CVT_ERROR);
    }
    p = (CHAR *)fp.fd_dstValue.dv_value;
    STtrmwhite(p);
    
    STlcopy(p, rgbData, STlength(p));  
    
    MEfree(fp.fd_dstValue.dv_value);

    return(rc); 
}

/*
**  CvtIngresMnyToDec
**
**  Converts Ingres internal money format to decimal representation.
**
**  On entry: 
**
**  On exit:  
**
**  Returns:  
*/
static UDWORD CvtIngresMnyToDec(CHAR     * rgbData, LPDESCFLD pird)
{
    UDWORD      rc = CVT_SUCCESS;
    IIAPI_CONVERTPARM   cv;
    
    cv.cv_srcDesc.ds_dataType   = IIAPI_MNY_TYPE; 
    cv.cv_srcDesc.ds_nullable   = pird->Nullable==SQL_NULLABLE?TRUE:FALSE; 
    cv.cv_srcDesc.ds_length     = (II_UINT2) pird->OctetLength; 
    cv.cv_srcDesc.ds_precision  = 0;
    cv.cv_srcDesc.ds_scale      = 0;
    cv.cv_srcDesc.ds_columnType = IIAPI_COL_TUPLE;
    cv.cv_srcDesc.ds_columnName = pird->Name; 
    
    cv.cv_srcValue.dv_null      = FALSE; 
    cv.cv_srcValue.dv_length    = (II_UINT2) pird->OctetLength;
    cv.cv_srcValue.dv_value     = rgbData; 
      
    cv.cv_dstDesc.ds_dataType   = IIAPI_DEC_TYPE;
    cv.cv_dstDesc.ds_nullable   = FALSE;
    cv.cv_dstDesc.ds_length     = (II_UINT2) pird->OctetLength; 
    cv.cv_dstDesc.ds_precision  = (II_UINT2) pird->Precision;
    cv.cv_dstDesc.ds_scale      = 2;
    cv.cv_dstDesc.ds_columnType = IIAPI_COL_TUPLE;
    cv.cv_dstDesc.ds_columnName = NULL;
    
    cv.cv_dstValue.dv_null      = FALSE;
    cv.cv_dstValue.dv_length    = (II_UINT2) pird->OctetLength;
    cv.cv_dstValue.dv_value     = MEreqmem(0, cv.cv_dstValue.dv_length, TRUE, NULL);
    IIapi_convertData(&cv);
    
    if (cv.cv_status != IIAPI_ST_SUCCESS)
    {
        MEfree((PTR)cv.cv_dstValue.dv_value);
        return(CVT_ERROR);
    }
    MEcopy(cv.cv_dstValue.dv_value,cv.cv_dstValue.dv_length,rgbData);
    MEfree((PTR)cv.cv_dstValue.dv_value);
    return(rc); 
}
/*
**  GetProcedureReturn
**
**  move the procedure return code from sqlca to bound storage area
**
*/
RETCODE GetProcedureReturn(LPSTMT pstmt) 
{
    SWORD i;
    LPDESC     pAPD=pstmt->pAPD;   /* -> application parameter descriptor */
    LPDESC     pIPD=pstmt->pIPD;   /* -> implementation parameter descriptor */
    LPDESCFLD  papd;
    LPDESCFLD  pipd;
    i2         Count = pIPD->Count; 
    RETCODE    rc    = CVT_SUCCESS;
    SDWORD     intValue = pstmt->sqlca.SQLRS2;

    TRACE_INTL(pstmt,"GetProcedureReturn");
    if (Count == 0)    /* just return OK if no parameters */
        return(SQL_SUCCESS);

    for  (i = 1; i <= Count; i++)
    {
        papd = pAPD->pcol + i;
        pipd = pIPD->pcol + i;

        if (pipd->ParameterType == SQL_PARAM_OUTPUT)
        {
           char        * DataPtr;            /* Ptr to data           */
           SQLINTEGER      * IndicatorPtr;    /* Ptr to NULL indicator */
           SQLINTEGER      * OctetLengthPtr;  /* Ptr to OctetLength    */
    
           GetBoundDataPtrs(pstmt, Type_APD, pstmt->pAPD, papd,
                             &DataPtr,
                             &OctetLengthPtr,
                             &IndicatorPtr);
            /*  
            ** Get DataPtr, OctetLengthPtr, and IndicatorPtr
            ** after adjustment for bind offset, row number,
            ** and row or column binding. 
            */
           if (pstmt->sqlca.SQLRS3 == 0)
           {
             if (IndicatorPtr)
                *IndicatorPtr = SQL_NULL_DATA;
                break;
           }
    
            switch (papd->ConciseType)
            {
            case SQL_C_LONG:
            case SQL_C_SLONG:
                MEcopy((PTR)&intValue, sizeof(SDWORD), DataPtr);
                *OctetLengthPtr = sizeof(SDWORD);
                break;
    
            case SQL_C_ULONG:
                MEcopy((PTR)&intValue, sizeof(UDWORD), DataPtr);
                *OctetLengthPtr = sizeof(UDWORD);
                break;
    
            case SQL_C_SHORT:
            case SQL_C_SSHORT:
                *OctetLengthPtr = sizeof(SWORD);
                if (intValue < -32768 || intValue > 32767)
                    rc = CVT_OUT_OF_RANGE;
                else
                    MEcopy((PTR)&intValue, sizeof(SWORD), DataPtr);
                break;
    
            case SQL_C_USHORT:
                *OctetLengthPtr = sizeof(UWORD);
                if (intValue < 0 || intValue > 65535)
                    rc = CVT_OUT_OF_RANGE;
                else
                    MEcopy((PTR)&intValue, sizeof(UWORD), DataPtr);
                break;
    
            case SQL_C_TINYINT:
            case SQL_C_STINYINT:
                *OctetLengthPtr = sizeof(SCHAR);
                if (intValue < -128 || intValue > 127)
                    rc = CVT_OUT_OF_RANGE;
                else
                    MEcopy((PTR)&intValue, sizeof(SCHAR), DataPtr);
                break;
    
            case SQL_C_UTINYINT:
            case SQL_C_BIT:
                *OctetLengthPtr = sizeof(UCHAR);
                if (intValue < 0 || intValue > 255)
                    rc = CVT_OUT_OF_RANGE;
                else
                    MEcopy((PTR)&intValue, sizeof(UCHAR), DataPtr);
                break;
           
            case SQL_C_FLOAT:
                MEcopy((PTR)&intValue, sizeof(SFLOAT), DataPtr);
                *OctetLengthPtr = sizeof(SFLOAT);
                break;
    
            case SQL_C_DOUBLE:
                MEcopy((PTR)&intValue, sizeof(SDOUBLE), DataPtr);
                *OctetLengthPtr = sizeof(SDOUBLE);
                break;
    
            case SQL_C_CHAR:
            case SQL_C_BINARY:
                CVla(intValue, DataPtr);
                *OctetLengthPtr = STlength(papd->DataPtr);
                break;
    
            default:
                rc = CVT_INVALID;
            } /* switch (papd->ConciseType) */
        } /* if (pipd->ParameterType == SQL_PARAM_OUTPUT) */
    } /* for  (i = 1; i <= Count; i++) */

    pstmt->fCvtError |= rc;
    switch (rc)
    {
    case CVT_SUCCESS:
        return SQL_SUCCESS;

    case CVT_TRUNCATION:
        return SQL_SUCCESS_WITH_INFO;

    default:
        return SQL_ERROR;
    }
}

/*
** CvtChardateFormat
**
** Convert a date in SQL_CHAR to the yyyy-mm-dd format.
** On entry: datestr --> date string to be converted from
**           cbData  --> length of the date string
** On exit:  datestr --> date string to be converted to
** Returns:  CVT_SUCCESS  Converted successfully.
**           CVT_ERROR    Conversion has failed.
*/
static UDWORD CvtChardateFormat(
   CHAR   *datestr,
   UWORD   cbData)
{
   CHAR buf[10];

   if (cbData != 10)
     return CVT_ERROR;

   /* nothing to convert if already yyyy-mm-dd */
   if (datestr[4] == '-' && datestr[7] == '-')
     return CVT_SUCCESS;

   MEcopy(datestr, 10, buf);

   /* if the format is dd.mm.yyyy, convert it to yyyy-mm-dd */
   if (buf[2] == '.' && buf[5] == '.')
   {
      /* convert the year */
      MEcopy(&buf[6], 4, datestr);
      datestr[4] = '-';
      /* convert the month */
      MEcopy(&buf[3], 2, &datestr[5]);
      datestr[7] = '-';
      /* convert the day */
      MEcopy(buf, 2, &datestr[8]);

      return CVT_SUCCESS;
   }

   /* if the format is mm/dd/yyyy, convert it to yyyy-mm-dd */
   if (buf[2] == '/' && buf[5] == '/')
   {
     /* convert the year */
     MEcopy(&buf[6], 4, datestr);
     datestr[4] = '-';
     /* convert the month */
     MEcopy(buf, 2, &datestr[5]);
     datestr[7] = '-';
     /* convert the day */
     MEcopy(&buf[3], 2, &datestr[8]);

     return CVT_SUCCESS;
   }

   /* error */
   return CVT_ERROR;
}
/*
**  GetTimeOrIntervalChar 
**  
**  Convert an ISO interval or time value to a character string.
**
**  On entry: pTarget     -->Buffer pointing to the target data.
**            pSource     -->Buffer pointing to the source data.
**            envHndl     -->API environment handle.
**            pipd        -->IPD (Implementation Parameter Descriptor)
**
**  On exit:  pTarget contains the converted data.
**
**  Returns:  TRUE  - Successful conversion.
**            FALSE - Unsuccessful conversion.
**
*/
static
BOOL GetTimeOrIntervalChar(CHAR *pTarget, CHAR *pSource,  
    VOID *envHndl, LPDESCFLD pird)
{
    IIAPI_FORMATPARM   fp;
    II_UINT2 len;

    switch (pird->fIngApiType)
    {   
        case IIAPI_INTYM_TYPE:
            len = ODBC_INTYM_LEN;
            break;
        
        case IIAPI_INTDS_TYPE:
            len = ODBC_INTDS_LEN;
            break;
 
        case IIAPI_TIME_TYPE:
        case IIAPI_TMWO_TYPE:
        case IIAPI_TMTZ_TYPE:
            len = ODBC_TIME_LEN;
            break;

        default:
            return FALSE;
            break;
    }

    fp.fd_envHandle = envHndl;
    fp.fd_srcDesc.ds_dataType   = pird->fIngApiType; 
    fp.fd_srcDesc.ds_nullable   = FALSE;  
    fp.fd_srcDesc.ds_length     = len;
    fp.fd_srcDesc.ds_precision  = (II_UINT2) pird->IsoTimePrecision;
    fp.fd_srcDesc.ds_scale      = 0;
    fp.fd_srcDesc.ds_columnType = IIAPI_COL_TUPLE;
    fp.fd_srcDesc.ds_columnName = NULL;  
    fp.fd_srcValue.dv_null      = FALSE; 
    fp.fd_srcValue.dv_length    = (II_UINT2) len;
    fp.fd_srcValue.dv_value     = pSource; 
          
    fp.fd_dstDesc.ds_dataType   = IIAPI_CHA_TYPE; 
    fp.fd_dstDesc.ds_nullable   = FALSE;
    fp.fd_dstDesc.ds_length     = (II_UINT2) pird->OctetLength;
    fp.fd_dstDesc.ds_precision  = (II_UINT2) pird->IsoTimePrecision;
    fp.fd_dstDesc.ds_scale      = (II_UINT2) 0;
    fp.fd_dstDesc.ds_columnType = IIAPI_COL_TUPLE;
    fp.fd_dstDesc.ds_columnName = NULL;
    fp.fd_dstValue.dv_null      = FALSE;
    fp.fd_dstValue.dv_length    = (II_UINT2) pird->OctetLength;
    fp.fd_dstValue.dv_value     = pTarget; 

    IIapi_formatData( &fp );
    
    if (fp.fd_status != IIAPI_ST_SUCCESS)
        return(FALSE);

    CMcpychar("\0",&pTarget[pird->OctetLength]);
    STtrmwhite(pTarget);
    return(TRUE); 
}
