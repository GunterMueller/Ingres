/*
** Copyright (c) 2004 Ingres Corporation 
*/ 

#include <compat.h>
#include <mu.h>
#include <qu.h>
#include <sql.h>                    /* ODBC Core definitions */
#include <sqlext.h>                 /* ODBC extensions */
#include <iiodbc.h>
#include <iiodbcfn.h>
#include "odbclocal.h"
#include "tracefn.h"

/* 
** Name: info.c - Get ODBC driver metadata for ODBC CLI.
** 
** Description: 
** 		This file defines: 
** 
**              SQLGetInfo - Get driver information.
**              SQLGetTypeInfo - Get information for individual data types.
** 
** History: 
**   14-jun-04 (loera01)
**     Created.
**   04-Oct-04 (hweho01)
**     Avoid compiler error on AIX platform, put include
**     files of sql.h and sqlext.h after qu.h.
**   15-Jul-2005 (hanje04)
**	Include iiodbcfn.h and tracefn.h which are the new home for the 
**	ODBC CLI function pointer definitions.
*/ 

/*
** Name: 	SQLGetInfo - Get general information
** 
** Description: 
**              SQLGetInfo returns general information about the driver and 
**              data source associated with a connection.
**
** Inputs:
**              hdbc - Connection handle.
**              fInfoType - Type of data to retrieve.
**              cbInfoValueMax - Maximum length of the info buffer.
**              pcbInfoValue - Actual length of the info buffer.
**              
**
** Outputs: 
**              rgbInfoValueParameter - Information buffer.
**
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
**              None.
**
** History: 
**    14-jun-04 (loera01)
**      Created.
*/ 
RETCODE SQL_API SQLGetInfo(
    SQLHDBC hdbc,
    UWORD   fInfoType,
    SQLPOINTER  rgbInfoValueParameter,
    SWORD   cbInfoValueMax,
    SWORD     *pcbInfoValue)
{
    pDBC pdbc = (pDBC)hdbc;
    RETCODE rc, traceRet = 1;

    ODBC_TRACE_ENTRY(ODBC_TR_TRACE, IITraceSQLGetInfo(hdbc, fInfoType,
        rgbInfoValueParameter, cbInfoValueMax, 
        pcbInfoValue), traceRet);   

    if (validHandle(hdbc, SQL_HANDLE_DBC) != SQL_SUCCESS)
    {
        ODBC_TRACE(ODBC_TR_TRACE, IITraceReturn( traceRet, SQL_INVALID_HANDLE)); 
        return SQL_INVALID_HANDLE;
    }

    resetErrorBuff(hdbc, SQL_HANDLE_DBC);

    rc = IIGetInfo(pdbc->hdr.driverHandle,
        fInfoType,
        rgbInfoValueParameter,
        cbInfoValueMax,
        pcbInfoValue);
   
    applyLock(SQL_HANDLE_DBC, pdbc);
    if (rc != SQL_SUCCESS)
    {
        pdbc->hdr.driverError = TRUE;
        pdbc->errHdr.rc = rc;
    }
    releaseLock(SQL_HANDLE_DBC, pdbc);

    ODBC_TRACE(ODBC_TR_TRACE, IITraceReturn(traceRet, rc));
    return rc;
}

/*
** Name: 	SQLGetTypeInfo - Get information about a data type.
** 
** Description: 
**              SQLGetTypeInfo returns information about data types 
**              supported by the data source. The driver returns the 
**              information in the form of an SQL result set.
**
** Inputs:
**              hstmt - Statement handle.
**              fSqlType - Data type.
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
**    14-jun-04 (loera01)
**      Created.
*/ 

RETCODE SQL_API SQLGetTypeInfo(
    SQLHSTMT  hstmt,
    SWORD     fSqlType)
{
    pSTMT pstmt = (pSTMT)hstmt;
    RETCODE rc, traceRet = 1;

    ODBC_TRACE_ENTRY(ODBC_TR_TRACE, IITraceSQLGetTypeInfo(hstmt, fSqlType),
        traceRet);

    if (validHandle(hstmt, SQL_HANDLE_STMT) != SQL_SUCCESS)
    {
        ODBC_TRACE(ODBC_TR_TRACE, IITraceReturn(traceRet, SQL_INVALID_HANDLE));
        return SQL_INVALID_HANDLE;
    }

    resetErrorBuff(hstmt, SQL_HANDLE_STMT);

    rc = IIGetTypeInfo(pstmt->hdr.driverHandle,
        fSqlType);

    applyLock(SQL_HANDLE_STMT, pstmt);
    if (rc != SQL_SUCCESS)
    {
        pstmt->hdr.driverError = TRUE;
        pstmt->errHdr.rc = rc;
        pstmt->hdr.state = S1;
    }
    else if (SQL_SUCCEEDED(rc))
        pstmt->hdr.state = S5;

    releaseLock(SQL_HANDLE_STMT, pstmt);
    ODBC_TRACE(ODBC_TR_TRACE, IITraceReturn(traceRet, rc));
    return rc;
}
