/*
** Copyright 2004 Ingres Corporation
*/

/*
**
LIBRARY = IMPULFLIBDATA
**
*/

#include <compat.h>
#include <gl.h>
#include <iicommon.h>
#include <dbdbms.h>
#include <ddb.h>
#include <ulf.h>
#include <adf.h>
#include <dmf.h>
#include <cs.h>
#include <scf.h>
#include <qsf.h>
#include <dmtcb.h>
#include <qefrcb.h>
#include <psfparse.h>
#include <qefnode.h>
#include <qefact.h>

/* File: ulddata.roc - Utility lookup tables
**
** Description:
**	This file contains some utility lookup tables for general back-
**	end use with error messages, debug output, and the like.
**
** History:
**	25-Jan-2004 (schka24)
**	    Invent as someplace to put query-mode text lookups.
*/

/*
** Name: Uld_ahd_xlatetab - QEA_XXX action header type lookup table
**
** Description:
**	This table is an INTXLATE code-to-string lookup table for QEF
**	action header types, defined as QEF QEA_XXX symbols (QEA_GET,
**	etc).  Action header text lookups are currently used for error
**	messages and debug messages, so speed is not critical.  Therefore
**	the table is in no special order.  Use uld_qef_actstr() to
**	actually look up an action header type description using this table.
**
**	Action header names are typically 4 characters, although this
**	is not a hard limit.
**
** History:
**	5-Feb-2004 (schka24)
**	    I needed to update the OP150, do it this way instead of
**	    adding to a giant case statement.
**      27-aug-2004 (stial01)
**          Removed SET NOBLOBLOGGING statement
**	29-Sep-2004 (drivi01)
**	    Added LIBRARY jam hint to put this file into IMPULFLIBDATA
**	    in the Jamfile.
*/

GLOBALDEF READONLY INTXLATE Uld_ahd_xlatetab[] =
{
	{QEA_DMU, "DMU"},
	{QEA_APP, "APP"},
	{QEA_SAGG, "SAGG"},
	{QEA_PROJ, "PROJ"},
	{QEA_AGGF, "AGGF"},
	{QEA_UPD, "UPD"},
	{QEA_DEL, "DEL"},
	{QEA_GET, "GET"},
	{QEA_UTL, "UTL"},
	{QEA_RUP, "RUP"},
	{QEA_RAGGF, "RAGF"},
	{QEA_LOAD, "LOAD"},
	{QEA_PUPD, "PUPD"},
	{QEA_PDEL, "PDEL"},
	{QEA_COMMIT, "CMMT"},
	{QEA_ROLLBACK, "RLBK"},
	{QEA_IF, "IF"},
	{QEA_RETURN, "RET"},
	{QEA_MESSAGE, "MSG"},
	{QEA_CALLPROC, "CALL"},
	{QEA_EMESSAGE, "EMSG"},
	{QEA_IPROC, "IPRC"},
	{QEA_RDEL, "RDEL"},
	{QEA_EVREGISTER, "EVRG"},
	{QEA_EVDEREG, "EVRM"},
	{QEA_EVRAISE, "EVRA"},
	{QEA_CREATE_RULE, "CRUL"},
	{QEA_CREATE_PROCEDURE, "CDBP"},
	{QEA_CREATE_INTEGRITY, "CINT"},
	{QEA_CREATE_TABLE, "CTBL",},
	{QEA_DROP_INTEGRITY, "DINT"},
	{QEA_MODIFY_TABLE, "MDFY"},
	{QEA_PROC_INSERT, "DBPI"},
	{QEA_CLOSETEMP, "CLST"},
	{QEA_INVOKE_RULE, "IRUL"},
	{QEA_CREATE_SCHEMA, "CSCH"},
	{QEA_EXEC_IMM, "EXIM"},
	{QEA_CREATE_VIEW, "CVIEW"},
	{QEA_FOR, "FOR"},
	{QEA_RETROW, "RETROW"},
	{QEA_HAGGF, "HAGF"},
	{QEA_D0_NIL, "DNIL"},
	{QEA_D1_QRY, "DQRY"},
	{QEA_D2_GET, "DGET"},
	{QEA_D3_XFR, "DXFR"},
	{QEA_D4_LNK, "DLNK"},
	{QEA_D5_DEF, "DDEF"},
	{QEA_D6_AGG, "DAGG"},
	{QEA_D7_OPN, "DOPN"},
	{QEA_D8_CRE, "DCRE"},
	{QEA_D9_UPD, "DUPD"},
	{QEA_D10_REGPROC, "DRPR"},
	{0, NULL}
};

/*
** Name: Uld_psq_xlatetab - PSQ_XXX query mode lookup table
**
** Description:
**	This table is an INTXLATE code-to-string lookup table for
**	"query modes", defined as parser PSQ_XXXX symbols (PSQ_CREATE,
**	etc).  Query-mode text lookup is currently used for error
**	messages and debug messages, so speed is not critical.  Therefore
**	the table is in no particular order.  Use uld_psq_modestr()
**	to actually look up a query mode description using this table.
**
**	No description may be longer than PSL_MAX_COMM_STRING-1.
**	This was 39 at the time of writing.
**	At the moment there is no enforcement mechanism.
**
** History:
**	25-Jan-2004 (schka24)
**	    Invent to make generalizing some parser productions and their
**	    error messages a bit easier and less tedious.
**	24-May-2006 (kschendel)
**	    Update with latest additions, deletions.
*/

GLOBALDEF READONLY INTXLATE Uld_psq_xlatetab[] =
{
	/* PSQ_xxx code, the text description */
	{PSQ_AAPLID, "ALTER ROLE"},
	{PSQ_ABORT, "ABORT"},
	{PSQ_ABSAVE, "ABORT TO SAVEPOINT"},
	{PSQ_ADFTRACE, "SET TRACE POINT ADXXXX"},
	{PSQ_AGROUP, "ALTER GROUP...ADD"},
	{PSQ_ALLJOURNAL, "SET JOURNALING"},
	{PSQ_ALOC, "ALTER LOCATION"},
	{PSQ_ALTAUDIT, "ALTER SECURITY_AUDIT"},
	{PSQ_ALTERDB, "ALTER DATABASE"},
	{PSQ_ALTERTABLE, "ALTER TABLE"},
	{PSQ_APPEND, "APPEND"},
	{PSQ_APROFILE, "ALTER PROFILE"},
	{PSQ_ASEQUENCE, "ALTER SEQUENCE"},
	{PSQ_ASSIGN, "ASSIGNMENT"},
	{PSQ_ATBL_ADD_COLUMN, "ALTER TABLE (add column)"},
	{PSQ_ATBL_ALTER_COLUMN, "ALTER TABLE (alter column)"},
	{PSQ_ATBL_DROP_COLUMN, "ALTER TABLE (drop column)"},
	{PSQ_AUSER, "ALTER USER"},
	{PSQ_AUTOCOMMIT, "AUTOCOMMIT"},
	{PSQ_BGNTRANS, "BEGIN TRANSACTION"},
	{PSQ_CALARM, "CREATE SECURITY_ALARM"},
	{PSQ_CALLPROC, "EXECUTE PROCEDURE"},
	{PSQ_CAPLID, "CREATE ROLE"},
	{PSQ_CGROUP, "CREATE GROUP"},
	{PSQ_CLOC, "CREATE LOCATION"},
	{PSQ_CLSCURS, "CLOSE CURSOR"},
	{PSQ_COMMENT, "COMMENT ON"},
	{PSQ_COMMIT, "COMMIT"},
	{PSQ_CONS, "ANSI CONSTRAINT declaration"},
	{PSQ_COPY, "COPY"},
	{PSQ_CPROFILE, "CREATE PROFILE"},
	{PSQ_CREATE, "CREATE TABLE"},
	{PSQ_CREATE_SCHEMA, "CREATE SCHEMA"},
	{PSQ_CREDBP, "CREATE PROCEDURE"},
	{PSQ_CRESETDBP, "CREATE PROCEDURE (set-of)"},
	{PSQ_CSEQUENCE, "CREATE SEQUENCE"},
	{PSQ_CSYNONYM, "CREATE SYNONYM"},
	{PSQ_CUSER, "CREATE USER"},
	{PSQ_DDCOPY, "COPY IN (Star)"},
	{PSQ_DDEXECPROC, "EXECUTE PROCEDURE (Star)"},
	{PSQ_DDLCONCUR, "SET DDL_CONCURRENCY"},
	{PSQ_DEFCURS, "DEFINE CURSOR"},
	{PSQ_DEFLOC, "DEFINE LOCATION"},
	{PSQ_DEFQRY, "DEFINE QUERY"},
	{PSQ_DEFREF, "DEFINE REFERENCE"},
	{PSQ_DELCURS, "DELETE CURSOR"},
	{PSQ_DELETE, "DELETE"},
	{PSQ_DESCINPUT, "DESCRIBE INPUT"},
	{PSQ_DESCRIBE, "DESCRIBE (output)"},
	{PSQ_DESTROY, "DESTROY"},
	{PSQ_DGROUP, "ALTER GROUP...DROP"},
	{PSQ_DGTT, "DECLARE GLOBAL TEMPORARY TABLE"},
	{PSQ_DGTT_AS_SELECT, "DECLARE GLOBAL TEMPORARY TABLE"},
	{PSQ_DIRCON, "DIRECT CONNECT"},
	{PSQ_DIRDISCON, "DIRECT DISCONNECT"},
	{PSQ_DIREXEC, "DIRECT EXECUTE IMMEDIATE"},
	{PSQ_DISAUDIT, "DISABLE SECURITY_AUDIT"},
	{PSQ_DISTCREATE, "DISTRIBUTED CREATE"},
	{PSQ_DMFTRACE, "SET TRACE POINT DMXXXX"},
	{PSQ_DPROFILE, "DROP PROFILE"},
	{PSQ_DRODBP, "DROP DB PROCEDURE"},
	{PSQ_DROP_SCHEMA, "DROP SCHEMA"},
	{PSQ_DSEQUENCE, "DROP SEQUENCE"},
	{PSQ_DSTINTEG, "DROP INTEGRITY"},
	{PSQ_DSTPERM, "DROP PERMIT"},
	{PSQ_DSTREF, "DESTROY REFERENCE"},
	{PSQ_DSTRULE, "DROP RULE"},
	{PSQ_DSYNONYM, "DROP SYNONYM"},
	{PSQ_ENAUDIT, "ENABLE SECURITY_AUDIT"},
	{PSQ_ENDLOOP, "ENDLOOP"},
	{PSQ_ENDTRANS, "END TRANSACTION"},
	{PSQ_EVDEREG, "REMOVE DBEVENT"},
	{PSQ_EVDROP, "DROP DBEVENT"},
	{PSQ_EVENT, "CREATE DBEVENT"},
	{PSQ_EVRAISE, "RAISE DBEVENT"},
	{PSQ_EVREGISTER, "REGISTER DBEVENT"},
	{PSQ_EXCURS, "EXECUTE CURSOR"},
	{PSQ_EXECQRY, "EXECUTE QUERY"},
	{PSQ_EXEDBP, "EXECUTE PROCEDURE"},
	{PSQ_FREELOCATOR, "FREE LOCATOR"},
	{PSQ_FOR, "FOR"},
	{PSQ_GDBPRIV, "GRANT ON DATABASE"},
	{PSQ_GOKPRIV, "GRANT ACCESS,UPDATE_SYSCAT"},
	{PSQ_GRANT, "GRANT"},
	{PSQ_GWFTRACE, "SET TRACE POINT GWXXX"},
	{PSQ_HELP, "HELP"},
	{PSQ_IF, "IF/THEN/ELSE"},
	{PSQ_INDEX, "CREATE INDEX"},
	{PSQ_INPREPARE, "PREPARE INTO"},
	{PSQ_INTEG, "CREATE INTEGRITY"},
	{PSQ_IPROC, "EXECUTE INTERNAL"},
	{PSQ_JTIMEOUT, "SET JOINOP (timeout)"},
	{PSQ_KALARM, "DROP SECURITY_ALARM"},
	{PSQ_KAPLID, "DROP ROLE"},
	{PSQ_KGROUP, "DROP GROUP"},
	{PSQ_KLOC, "DROP LOCATION"},
	{PSQ_KUSER, "DROP USER"},
	{PSQ_MESSAGE, "MESSAGE"},
	{PSQ_MODIFY, "MODIFY"},
	{PSQ_MXQUERY, "SET [NO]MAX..."},
	{PSQ_MXSESSION, "SET [NO]MAX SESSION"},
	{PSQ_OBSOLETE, "OBSOLETE"},
	{PSQ_OPFTRACE, "SET TRACE POINT OPXXXX"},
	{PSQ_PREPARE, "PREPARE"},
	{PSQ_PRINT, "PRINT"},
	{PSQ_PROT, "CREATE PERMIT"},
	{PSQ_PSFTRACE, "SET TRACE POINT PSXXXX"},
	{PSQ_QEFTRACE, "SET TRACE POINT QEXXXX"},
	{PSQ_QRYDEFED, "DEFINE CURSOR"},
	{PSQ_QSFTRACE, "SET TRACE POINT QSXXXX"},
	{PSQ_RANGE, "RANGE"},
	{PSQ_RDBPRIV, "REVOKE ON DATABASE"},
	{PSQ_RDFTRACE, "SET TRACE POINT RDXXXX"},
	{PSQ_REGPROC, "EXECUTE REGISTERED DBP"},
	{PSQ_REG_IMPORT, "REGISTER...AS IMPORT"},
	{PSQ_REG_INDEX, "REGISTER INDEX"},
	{PSQ_REG_LINK, "REGISTER...AS LINK"},
	{PSQ_REG_REMOVE, "REMOVE [TABLE/INDEX]"},
	{PSQ_RELOCATE, "RELOCATE"},
	{PSQ_REMOVE, "REMOVE [TABLE|VIEW]"},
	{PSQ_REPCURS, "REPLACE CURSOR"},
	{PSQ_REPEAT, "REPEAT"},
	{PSQ_REPLACE, "REPLACE"},
	{PSQ_REREGISTER, "REGISTER ... AS REFRESH"},
	{PSQ_RETCURS, "RETRIEVE CURSOR"},
	{PSQ_RETINTO, "CREATE TABLE"},
	{PSQ_RETRIEVE, "RETRIEVE"},
	{PSQ_RETROW, "RETURN ROW"},
	{PSQ_RETURN, "RETURN"},
	{PSQ_REVOKE, "REVOKE"},
	{PSQ_RGRANT, "GRANT ROLE"},
	{PSQ_RLSAVE, "ROLLBACK TO SAVEPOINT"},
	{PSQ_ROKPRIV, "REVOKE ACCESS,UPDATE_SYSCAT"},
	{PSQ_ROLLBACK, "ROLLBACK"},
	{PSQ_RREVOKE, "REVOKE ROLE"},
	{PSQ_RS_ERROR, "RAISE ERROR"},
	{PSQ_RULE, "CREATE RULE"},
	{PSQ_SAGGR, "SET AGGREGATE"},
	{PSQ_SAVE, "SAVE"},
	{PSQ_SCFTRACE, "SET TRACE POINT SCXXXX"},
	{PSQ_SCPUFACT, "SET CPUFACTOR"},
	{PSQ_SDATEFMT, "SET DATE_FORMAT"},
	{PSQ_SDECIMAL, "SET DECIMAL"},
	{PSQ_SECURE, "PREPARE TO COMMIT"},
	{PSQ_SETRANDOMSEED, "SET RANDOM_SEED"},
	{PSQ_SETROLE, "SET ROLE"},
	{PSQ_SETUNICODESUB, "SET [no]UNICODE_SUBSTITUTION"},
	{PSQ_SET_SESSION, "SET SESSION ..."},
	{PSQ_SET_SESS_AUTH_ID, "SET SESSION AUTHORIZATION"},
	{PSQ_SJOURNAL, "SET JOURNALING"},
	{PSQ_SLOCKMODE, "SET LOCKMODE"},
	{PSQ_SLOGGING, "SET [NO]LOGGING"},
	{PSQ_SMNFMT, "SET MONEY FORMAT"},
	{PSQ_SMNPREC, "SET MONEY PRECISION"},
	{PSQ_SON_ERROR, "SET SESSION ON_ERROR"},
	{PSQ_SQEP, "SET [NO]QEP"},
	{PSQ_SRINTO, "SET RETRIEVE_INTO"},
	{PSQ_SSQL, "SET SQL"},
	{PSQ_SSTATS, "SET STATISTICS"},
	{PSQ_STRACE, "SET TRACE"},
	{PSQ_STRANSACTION, "SET TRANSACTION"},
	{PSQ_SVEPOINT, "SAVEPOINT"},
	{PSQ_SWORKLOC, "SET WORK LOCATIONS"},
	{PSQ_SXFTRACE, "SET TRACE POINT SXXXX"},
	{PSQ_UPD_ROWCNT, "SET UPDATE_ROWCOUNT"},
	{PSQ_VAR, "CREATE PROCEDURE (declaration)"},
	{PSQ_VIEW, "CREATE VIEW"},
	{PSQ_WHILE, "WHILE"},
	{PSQ_XA_COMMIT, "COMMIT (distributed)"},
	{PSQ_XA_PREPARE, "PREPARE TO COMMIT (distributed)"},
	{PSQ_XA_ROLLBACK, "ROLLBACK (distributed)"},
	{PSQ_0_CRT_LINK, "CREATE LINK"},
	{PSQ_50DEFQRY, "5.0 REPEAT QUERY"},
	{0, NULL}
};

/* Name: Uld_struct_xlatetab - Storage structure name lookup table
**
** Description:
**	This table is an INTXLATE code-to-string lookup table for
**	storage structure names:  heap, hash, btree, isam, rtree.
**	The corresponding integer value is the DB_xxx_STORE code.
**
**	Note that we don't parse the compressed versions (cbtree,
**	etc).  It's better if the caller takes care of that, because
**	some sort of separate flag will have to be set, and there is
**	no server-wide standard for how to indicate compression.
**	Fortunately, none of the standard structure names start with
**	the letter C, so the caller can parse it off easily enough.
**
** History:
**	23-Nov-2005 (kschendel)
**	    Add for the result_structure option.
*/

GLOBALDEF READONLY INTXLATE Uld_struct_xlatetab[] =
{
	{DB_HEAP_STORE, "heap"},
	{DB_HASH_STORE, "hash"},
	{DB_ISAM_STORE, "isam"},
	{DB_BTRE_STORE, "btree"},
	{DB_RTRE_STORE, "rtree"},
	{DB_SORT_STORE, "heapsort"},
	{0, NULL}
};

/*
** Name: Uld_syscat_xlatetab - System catalog table name lookup table
**
** Description:
**	This table is an INTXLATE code-to-string lookup table for
**	system catalog names, as identified by the table number (either
**	baseid, ie reltid, or indexid, reltidx, as appropriate).
**	System catalog table name lookup is currently used for error
**	messages and debug messages, so speed is not critical.  Therefore
**	the table is in no particular order.  Use uld_syscat_namestr()
**	to actually look up a catalog name using this table.
**
** History:
**	30-Dec-2004 (schka24)
**	    Something harmless to do until I'm feeling better.
*/

GLOBALDEF READONLY INTXLATE Uld_syscat_xlatetab[] =
{
	/* Table ID, the catalog name */
	{DM_B_RELATION_TAB_ID, "iirelation"},
	{DM_I_RIDX_TAB_ID, "iirel_idx"},
	{DM_B_ATTRIBUTE_TAB_ID, "iiattribute"},
	{DM_B_INDEX_TAB_ID, "iiindex"},
	{DM_B_DEVICE_TAB_ID, "iidevice"},
	{DM_B_QRYTEXT_TAB_ID, "iiqrytext"},
	{DM_B_TREE_TAB_ID, "iitree"},
	{DM_B_PROTECT_TAB_ID, "iiprotect"},
	{DM_B_INTEGRITY_TAB_ID, "iiintegrity"},
	{DM_I_INTEGRITYIDX_TAB_ID, "iiintegrityidx"},
	{DM_B_RULE_TAB_ID, "iirule"},
	{DM_I_RULEIDX_TAB_ID, "iiruleidx"},
	{DM_I_RULEIDX1_TAB_ID, "iiruleidx1"},
	{DM_B_DBLOC_TAB_ID, "iidbloc"},
	{DM_B_DEPENDS_TAB_ID, "iidbdepends"},
	{DM_I_XDEPENDS_TAB_ID, "iixdbdepends"},
	{DM_B_STATISTICS_TAB_ID, "iistatistics"},
	{DM_B_HISTOGRAM_TAB_ID, "iihistogram"},
	{DM_B_DBP_TAB_ID, "iiprocedure"},
	{DM_I_XDBP_TAB_ID, "iixprocedure"},
	{DM_B_LOCATIONS_TAB_ID, "iilocations"},
	{DM_B_DATABASE_TAB_ID, "iidatabase"},
	{DM_B_DBACCESS_TAB_ID, "iidbaccess"},
	{DM_B_EXTEND_TAB_ID, "iiextend"},
	{DM_B_USER_TAB_ID, "iiuser"},
	{DM_B_GRPID_TAB_ID, "iiusergroup"},
	{DM_B_APLID_TAB_ID, "iirole"},
	{DM_B_DBPRIV_TAB_ID, "iidbpriv"},
	{DM_B_SECSTATE_TAB_ID, "iisecuritystate"},
	{DM_B_EVENT_TAB_ID, "iievent"},
	{DM_I_XEVENT_TAB_ID, "iixevent"},
	{DM_B_COMMENT_TAB_ID, "iidbms_comment"},
	{DM_B_SYNONYM_TAB_ID, "iisynonym"},
	{DM_I_XSYN_TAB_ID, "iixsynonym"},
	{DM_B_PRIV_TAB_ID, "iipriv"},
	{DM_I_XPRIV_TAB_ID, "iixprov"},
	{DM_B_ETAB_TAB_ID, "iiextended_relation"},
	{DM_B_IIKEY_TAB_ID, "iikey"},
	{DM_B_IIDEFAULT_TAB_ID, "iidefault"},
	{DM_I_IIDEFAULTIDX_TAB_ID, "iidefaultidx"},
	{DM_B_IISCHEMA_TAB_ID, "iischema"},
	{DM_I_IISCHEMAIDX_TAB_ID, "iischemaidx"},
	{DM_B_IIPROC_PARAM_TAB_ID, "iiprocedure_parameter"},
	{DM_B_IIPROFILE_TAB_ID, "iiprofile"},
	{DM_B_IISECALARM_TAB_ID, "iisecalarm"},
	{DM_B_IIROLEGRANT_TAB_ID, "iirolegrant"},
	{DM_B_RANGE_TAB_ID, "iirange"},
	{DM_B_SEQ_TAB_ID, "iisequence"},
	{DM_B_DSCHEME_TAB_ID, "iidistscheme"},
	{DM_B_DISTCOL_TAB_ID, "iidistcol"},
	{DM_B_DISTVAL_TAB_ID, "iidistval"},
	{DM_B_PNAME_TAB_ID, "iipartname"},
	{0, NULL}
};
