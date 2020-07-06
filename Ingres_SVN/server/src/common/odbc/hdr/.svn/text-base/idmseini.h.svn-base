/*
**  IDMSEINI
**
**  Module Name:    idmseini.h
**
**  Copyright (c) 2004 Ingres Corporation
**
**  description:    CAIDMS.INI Setup definitions used in ODBC, QCLI, DTS et al
**
**   Change History
**   --------------
**
**   date        programmer          description
**
**   05/28/1993  Dave Ross           Initial coding.
**   08/26/1993  Dave Ross           Added options for ACCESS.
**   10/13/1993  Dave Ross           Added options for DLCI.
**   03/03/1997  Jean Teng           Changed LEN_NODE from 8 to 32
**   03/06/1997  Jean Teng           Added LEN_PROMPT
**   11/07/1997  Jean Teng           Added LEN_ROLENAME and LEN_ROLEPWD
**   12/01/1997  Jean Teng           Added LEN_SELECTLOOPS
**   12/09/1997  Dave Thole          Changed LEN_ROLEPWD from 32 to 64 for string write
**	 05/29/1998	 Dmitriy Bubis		 Added valid keynames for ini file section
**   12/21/1998  Dave Thole          Added KEY_SERVERCLASSES
**   03/29/1999  Dave Thole          Added SYSTEM_LOCATION_VARIABLE
**   03/30/1999  Dave Thole          Added other product support
**   01/19/2001  Dave Thole          Increased LEN_NODE to 128 for nodeless connect
**   08/17/2001  Dave Thole          Added VANT (CA-Vantage) server class
**   05/22/2002  Ralph Loen          Bug 107814: Add "advanced" DSN keys.
**   06/11/2002  Ralph Loen          Fixed C++-style comments.
**   06/27/2002  Fei Wei             Add DB2UDB to the server classes.
**   03/14/2003  Ralph Loen          Added common definition for INI files.
**   06/02/2003  Ralph Loen          Moved ScanPastSpaces(), GetOdbcIniToken(),
**                                   and getDriverVersion() templates 
**                                   to this file.
**   28-jan-2004 (loera01)
**       Added boolean argument to getFileToken() for VMS.
**   11-Jun-2004 (somsa01)
**	 Cleaned up code for Open Source.
**   18-jan-2006 (loera01) SIR 115615
**       Removed outmoded registry references.
**   10-Mar-2006 (Ralph Loen) Bug 115833
**       Added support for new DSN configuration option: "Fill character
**       for failed Unicode/multibyte conversions".
**   03-Jul-2006 (Ralph Loen) SIR 116326
**       Added KEY_DRIVERNAMES to support SQLBrowseConnect().
**   30-Jan-2007 (Ralph Loen) Bug 117569
**       Rename the driver name "Ingres 3.0" to "Ingres 2006".
**   19-Oct-2007 (Ralph Loen) Bug 119329
**       GetDriverVersion() now has a "path" input argument.
*/

#ifndef _INC_IDMSEINI
#define _INC_IDMSEINI

#define ODBC_INI "odbc.ini"
#define ODBCINST_INI "odbcinst.ini"
#define ODBC_DSN "ODBC Data Sources"

/*
**  Ini file lengths:
*/
#define LEN_ATOE            256
#define LEN_DBNAME          32
#define LEN_DESCRIPTION     255
#define LEN_DICTIONARY      16
#define LEN_DRIVER_NAME     32
#define LEN_SERVER          32
#define LEN_DSN             32
#define LEN_SERVERTYPE      32
#define LEN_ROLENAME        32
#define LEN_ROLEPWD         64
#define LEN_GROUP           32
#define LEN_PROMPT          1
#define LEN_SELECTLOOPS     1
#define LEN_LOGFILE         255
#define LEN_NODE            128
#define LEN_PRODUCT_NAME    40
#define LEN_RESOURCE        8
#define LEN_SYSTEMID        8
#define LEN_TASK            8
#define LEN_VIEW            37
#define LEN_VERSION_STRING  16
#define LEN_XREFILE         255
#define LEN_IDMS_NAME       LEN_DRIVER_NAME + 1
#define LEN_DSNSECT         LEN_DSN + 1
#define LEN_NODESECT        LEN_NODE + 5
#define LEN_RESOURCESECT    LEN_DRIVER_NAME + 9
#define LEN_DRIVERSECT       LEN_DRIVER_NAME + 9
#define LEN_DRIVER_TYPE      LEN_DRIVER_NAME
#define LEN_IIDECIMAL       1
#define LEN_CONVERTINT8TOINT4 1

/*
**  Ini file section and key names:
*/
#define KEY_DSN				"DSN"
#define KEY_DESCRIPTION_UP	"DESCRIPTION"
#define KEY_SERVER_UP       "SERVER"
#define KEY_SERVERTYPE_UP   "SERVERTYPE"
#define KEY_DATABASE_UP     "DATABASE"
#define KEY_ROLENAME        "ROLENAME"
#define KEY_ROLEPWD	        "ROLEPWD"
#define KEY_GROUP           "GROUP"
#define KEY_PROMPT	        "PROMPT"
#define KEY_WITHOPTION      "WITHOPTION"
#define KEY_PROMPTUIDPWD    "PROMPTUIDPWD"
#define KEY_DATASOURCENAME	"DATASOURCENAME"
#define KEY_SERVERNAME      "SERVERNAME"
#define KEY_SRVR	        "SRVR"
#define KEY_DB		        "DB"
#define KEY_DEFAULTS        "Defaults"

#define KEY_OPTIONS         "Options"

#define KEY_SERVER          "Server"
#define KEY_SERVERTYPE      "ServerType"
#define KEY_DESCRIPTION     "Description"
#define KEY_DATABASE        "Database"

#define KEY_ACCESSIBLE      "AccessibleTables"
#define KEY_ACCESSINFO      "AccessInfo"
#define KEY_ACCESSTYPE      "AccessType"
#define KEY_ACCT_PROMPT     "AccountPrompt"
#define KEY_ATOE            "AsciiEbcdicTables"
#define KEY_AUTOCOMMIT      "AutoCommit"
#define KEY_CACHE_PROC      "CacheProcAddresses"
#define KEY_CACHE_TABLES    "CacheSQLTables"
#define KEY_CLASS           "ClassOfService"
#define KEY_COLUMNS         "CatalogColumns"
#define KEY_COMMIT_BEHAVIOR "CommitBehavior"
#define KEY_CUSTOM          "Custom"
#define KEY_DATACOM         "DATACOM"
#define KEY_DATASOURCES     "Data Sources"
#define KEY_DBCS            "Dbcs"
#define KEY_DBCS_PATH       "DbcsPath"
#define KEY_DBCS_TYPE       "DbcsType"
#define KEY_DBCS_TYPES      "DBCS Types"
#define KEY_DBNAME          "Dbname"
#define KEY_DBNAMES         "Dbnames"
#define KEY_DEFAULT         "Default"

#define KEY_DICTIONARY      "Dictionary"
#define KEY_DICTNAME        "Dictname"
#define KEY_DICTNODE        "Dictnode"
#define KEY_DISABLE_ENSURE  "DisableEnsure"
#define KEY_ENABLE_ENSURE   "EnableEnsure"
#define KEY_DOUBLE          "FetchDouble"
#define KEY_FDXREF          "FDXref"
#define KEY_ICON            "Icon"
#define KEY_IDMS            "IDMS"
#define KEY_IDMS_DEFAULT    "IDMS Default"
#define KEY_KATAKANA        "Katakana"
#define KEY_INPUT_ENABLE    "InputEnable"
#define KEY_MAXTIMEOUT      "MaxTimeOut"
#define KEY_NAME            "Name"
#define KEY_NODE            "Node"
#define KEY_NODES           "Nodes"
#define KEY_PATH            "Path"
#define KEY_QUEUE           "Queue"
#define KEY_READONLY        "ReadOnly"
#define KEY_REPEATABLE      "Repeatable"
#define KEY_RESOURCE        "Resource"
#define KEY_ROWS            "FetchRows"
#define KEY_SERVERS         "Servers"
#define KEY_SUBSCHEMA       "Subschema"
#define KEY_TASK            "AlternateTask"
#define KEY_TIMEOUT         "TimeOut"
#define KEY_TEMP_DSN        "DriverConnectTemp"
#define KEY_TEMP_SERVER     "Server DriverConnectTemp"
#define KEY_TXNISOLATION    "TxnIsolation"
#define KEY_UDEF            "User-Defined Record"
#define KEY_UNCOMMITTED     "Uncommitted"
#define KEY_VIEW            "CatalogTable"
#define KEY_WEIGHT          "ConversionWeight"

/* "Advanced" options */

#define KEY_SELECTLOOPS "SELECTLOOPS"
#define KEY_DISABLECATUNDERSCORE "DISABLECATUNDERSCORE"
#define KEY_ALLOWPROCEDUREUPDATE "ALLOWPROCEDUREUPDATE"
#define KEY_MULTIBYTEFILLCHAR "MULTIBYTEFILLCHAR"
#define KEY_CONVERTTHREEPARTNAMES "CONVERTTHREEPARTNAMES"
#define KEY_USESYSTABLES "USESYSTABLES"
#define KEY_BLANKDATE "BLANKDATE"
#define KEY_DATE1582 "DATE1582"
#define KEY_CATCONNECT "CATCONNECT"
#define KEY_NUMERIC_OVERFLOW "NUMERIC_OVERFLOW"
#define KEY_SUPPORTIIDECIMAL "SUPPORTIIDECIMAL"
#define KEY_CATSCHEMANULL "CATSCHEMANULL"
#define KEY_CONVERTINT8TOINT4 "CONVERTINT8TOINT4"

#define KEY_LOG_FILE        "LogFile"
#define KEY_LOG_OPTIONS     "LogOptions"

#define KEY_TRACE_CM        "CmTrace"
#define KEY_TRACE_CM_BE     "CmBeTrace"
#define KEY_TRACE_DNS       "DnsTrace"
#define KEY_TRACE_DNS_BE    "DnsBeTrace"
#define KEY_TRACE_DTS       "DtsTrace"
#define KEY_TRACE_DTS_BE    "DtsBeTrace"
#define KEY_TRACE_FDE       "FdeTrace"
#define KEY_TRACE_IDMS      "IdmsTrace"
#define KEY_TRACE_NDL       "NdlTrace"
#define KEY_TRACE_NDL_BE    "NdlBeTrace"
#define KEY_TRACE_ODBC      "OdbcTrace"
#define KEY_TRACE_SQL       "SqlTrace"
#define KEY_TRACE_SQL_BE    "SqlBeTrace"
#define KEY_TRACE_UTIL      "UtilTrace"

#define KEY_SERVERCLASSES   { \
    "Ingres", "OPINGDT (OpenIngres Desktop 1.2)", "DCOM (Datacom)", "IDMS",\
    "DB2", "IMS", "VSAM", "RDB (Rdb/VMS)", "STAR", "RMS",\
    "Oracle", "Informix", "Sybase", "ALB (ALLBASE/SQL)",\
    "INGDSK  (Ingres II Desktop)", "MSSQL (MS SQL Server)", "ODBC", \
    "VANT (CA-Vantage)", "DB2UDB", NULL}

#define KEY_DRIVERNAMES { \
    "Ingres", "Ingres 2006", NULL}
#ifndef SYSTEM_LOCATION_VARIABLE   /* normally defined in gl.h */
# define SYSTEM_LOCATION_VARIABLE      "II_SYSTEM"
# define SYSTEM_LOCATION_SUBDIRECTORY  "ingres"
# define SYSTEM_LOCATION_HOSTNAME      "II_HOSTNAME"
#endif /* SYSTEM_LOCATION_VARIABLE */

# define SYSTEM_API_DLL      "oiapi.dll"
# define SYSTEM_CLF_DLL      "oiclfnt.dll"
# define SYSTEM_CLFDATA_DLL  "oiclfdata.dll"
# define SERVER_TYPE_DEFAULT "INGRES"

/*
** External routines for reading INI files.
*/
char * ScanPastSpaces(char *p);
char * GetOdbcIniToken(char *p, char * szToken);
void getDriverVersion( char *p);

/*
** The next two routines are the equivalent of ScanPastSpaces() and
** GetOdbcIniToken(). They are being maintained indepenently until
** a common odbc utility library is created for the driver,
** configuration, and administrator modules.
*/
char * skipSpaces (char *);
char * getFileToken (char *, char *, bool);

char **getDefaultInfo();
char *getMgrInfo(bool *sysIniDefined);

#endif   /* _INC_IDMSEINI */
