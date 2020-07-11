-- {
-- File Name:	SQL_STANDARD.ADA
-- Purpose:	SQL_STANDARD package for ADA data types.
--
-- Packages:	SQL_STANDARD
-- Usage:	with SQL_STANDARD; use SQL_STANDARD;
--
-- Copyright (c) 2004 Ingres Corporation 
--
-- }
with SYSTEM; use SYSTEM;
package SQL_STANDARD is 
    package CHARACTER_SET renames STANDARD;
    subtype CHARACTER_TYPE is CHARACTER_SET.CHARACTER;
    type CHAR is array (POSITIVE range <>) of CHARACTER_TYPE;
    type BIT is array (NATURAL range <>) of BOOLEAN;
    type SMALLINT is range short_integer'first .. short_integer'last;
    type INT is range integer'first .. integer'last;
    type REAL is digits 6;
    type DOUBLE_PRECISION is digits 9;
    subtype INDICATOR_TYPE is SMALLINT;
    type SQLCODE_TYPE is range integer'first .. integer'last;
    subtype SQL_ERROR is SQLCODE_TYPE range SQLCODE_TYPE'FIRST .. -1;
    subtype NOT_FOUND is SQLCODE_TYPE range 100 .. 100;
    type SQLSTATE_TYPE is new CHAR (1 .. 5);
    package SQLSTATE_CODES is
       AMBIGUOUS_CURSOR_NAME_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="3C000";
       CARDINALITY_VIOLATION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="21000";
       CONNECTION_EXCEPTION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="08000";
       CONNECTION_EXCEPTION_CONNECTION_DOES_NOT_EXIST:
            constant SQLSTATE_TYPE :="08003";
       CONNECTION_EXCEPTION_CONNECTION_FAILURE:
            constant SQLSTATE_TYPE :="08006";
       CONNECTION_EXCEPTION_CONNECTION_NAME_IN_USE:
            constant SQLSTATE_TYPE :="08002";
       CONNECTION_EXCEPTION_SQLCLIENT_UNABLE_TO_ESTABLISH_SQLCONNECTION:
            constant SQLSTATE_TYPE :="08001";
       CONNECTION_EXCEPTION_SQLSERVER_REJECTED_ESTABLISHMENT_OF_SQLCONNECTION:
            constant SQLSTATE_TYPE :="08004";
       CONNECTION_EXCEPTION_TRANSACTION_RESOLUTION_UNKNOWN:
            constant SQLSTATE_TYPE :="08007";
       DATA_EXCEPTION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="22000";
       DATA_EXCEPTION_CHARACTER_NOT_IN_REPERTOIRE:
            constant SQLSTATE_TYPE :="22021";
       DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW:
            constant SQLSTATE_TYPE :="22008";
       DATA_EXCEPTION_DIVISION_BY_ZERO:
            constant SQLSTATE_TYPE :="22012";
       DATA_EXCEPTION_ERROR_IN_ASSIGNMENT:
            constant SQLSTATE_TYPE :="22005";
       DATA_EXCEPTION_INDICATOR_OVERFLOW:
            constant SQLSTATE_TYPE :="22022";
       DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW:
            constant SQLSTATE_TYPE :="22015";
       DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST:
            constant SQLSTATE_TYPE :="22018";
       DATA_EXCEPTION_INVALID_DATETIME_FORMAT:
            constant SQLSTATE_TYPE :="22007";
       DATA_EXCEPTION_INVALID_ESCAPE_CHARACTER:
            constant SQLSTATE_TYPE :="22019";
       DATA_EXCEPTION_INVALID_ESCAPE_SEQUENCE:
            constant SQLSTATE_TYPE :="22025";
       DATA_EXCEPTION_INVALID_PARAMETER_VALUE:
            constant SQLSTATE_TYPE :="22023";
       DATA_EXCEPTION_INVALID_TIME_ZONE_DISPLACEMENT_VALUE:
            constant SQLSTATE_TYPE :="22009";    
       DATA_EXCEPTION_NULL_VALUE_NO_INDICATOR_PARAMETER:
            constant SQLSTATE_TYPE :="22002";
       DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE:
            constant SQLSTATE_TYPE :="22003";
       DATA_EXCEPTION_STRING_DATA_LENGTH_MISMATCH:
            constant SQLSTATE_TYPE :="22026";
       DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION:
            constant SQLSTATE_TYPE :="22001";
       DATA_EXCEPTION_SUBSTRING_ERROR:
            constant SQLSTATE_TYPE :="22011";
       DATA_EXCEPTION_TRIM_ERROR:
            constant SQLSTATE_TYPE :="22027";
       DATA_EXCEPTION_UNTERMINATED_C_STRING:
            constant SQLSTATE_TYPE :="22024";
       DEPENDENT_PRIVILEGE_DESCRIPTORS_STILL_EXIST_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="2B000";
       DYNAMIC_SQL_ERROR_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="07000";
       DYNAMIC_SQL_ERROR_CURSOR_SPECIFICATION_CANNOT_BE_EXECUTED:
            constant SQLSTATE_TYPE :="07003";
       DYNAMIC_SQL_ERROR_INVALID_DESCRIPTOR_COUNT:
            constant SQLSTATE_TYPE :="07008";
       DYNAMIC_SQL_ERROR_INVALID_DESCRIPTOR_INDEX:
            constant SQLSTATE_TYPE :="07009";
       DYNAMIC_SQL_ERROR_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION:
            constant SQLSTATE_TYPE :="07005";
       DYNAMIC_SQL_ERROR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION:
            constant SQLSTATE_TYPE :="07006";
       DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_DYNAMIC_PARAMETER_SPEC:
            constant SQLSTATE_TYPE :="07001";
       DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_TARGET_SPEC:
            constant SQLSTATE_TYPE :="07002";
       DYNAMIC_SQL_ERROR_USING_CLAUSE_REQUIRED_FOR_DYNAMIC_PARAMETERS:
            constant SQLSTATE_TYPE :="07004";
       DYNAMIC_SQL_ERROR_USING_CLAUSE_REQUIRED_FOR_RESULT_FIELDS:
            constant SQLSTATE_TYPE :="07007";
       FEATURE_NOT_SUPPORTED_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="0A000";
       FEATURE_NOT_SUPPORTED_MULTIPLE_ENVIRONMENT_TRANSACTIONS:
            constant SQLSTATE_TYPE :="0A001";
       INTEGRITY_CONSTRAINT_VIOLATION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="23000";
       INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="28000";
       INVALID_CATALOG_NAME_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="3D000";
       INVALID_CHARACTER_SET_NAME_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="2C000";
       INVALID_CONDITION_NUMBER_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="35000";
       INVALID_CONNECTION_NAME_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="2E000";
       INVALID_CURSOR_NAME_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="34000";
       INVALID_CURSOR_STATE_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="24000";
       INVALID_SCHEMA_NAME_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="3F000";
       INVALID_SQL_DESCRIPTOR_NAME_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="33000";
       INVALID_SQL_STATEMENT_NAME_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="26000";
       INVALID_TRANSACTION_STATE_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="25000";
       INVALID_TRANSACTION_TERMINATION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="2D000";
       NO_DATA_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="02000";
       REMOTE_DATABASE_ACCESS_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="HZ000";
       SUCCESSFUL_COMPLETION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="00000";
       SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="42000";
       SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_IN_DIRECT_STATEMENT_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="2A000";
       SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_IN_DYNAMIC_STATEMENT_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="37000";
       TRANSACTION_ROLLBACK_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="40000";
       TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION:
            constant SQLSTATE_TYPE :="40002";
       TRANSACTION_ROLLBACK_SERIALIZATION_FAILURE:
            constant SQLSTATE_TYPE :="40001";
       TRANSACTION_ROLLBACK_STATEMENT_COMPLETION_UNKNOWN:
            constant SQLSTATE_TYPE :="40003";
       TRIGGERED_DATA_CHANGE_VIOLATION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="27000";
       WARNING_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="01000";
       WARNING_CURSOR_OPERATION_CONFLICT:
            constant SQLSTATE_TYPE :="01001";
       WARNING_DISCONNECT_ERROR:
            constant SQLSTATE_TYPE :="01002";
       WARNING_IMPLICIT_ZERO_BIT_PADDING:
            constant SQLSTATE_TYPE :="01008";
       WARNING_INSUFFICIENT_ITEM_DESCRIPTOR_AREAS:
            constant SQLSTATE_TYPE :="01005";
       WARNING_NULL_VALUE_ELIMINATED_IN_SET_FUNCTION:
            constant SQLSTATE_TYPE :="01003";
       WARNING_PRIVILEGE_NOT_GRANTED:
            constant SQLSTATE_TYPE :="01007";
       WARNING_PRIVILEGE_NOT_REVOKED:
            constant SQLSTATE_TYPE :="01006";
       WARNING_QUERY_EXPRESSION_TOO_LONG_FOR_INFORMATION_SCHEMA:
            constant SQLSTATE_TYPE :="0100A";
       WARNING_SEARCH_CONDITION_TOO_LONG_FOR_INFORMATION_SCHEMA:
            constant SQLSTATE_TYPE :="01009";
       WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING:
            constant SQLSTATE_TYPE :="01004";
       WITH_CHECK_OPTION_VIOLATION_NO_SUBCLASS:
            constant SQLSTATE_TYPE :="44000";
    end SQLSTATE_CODES;
end SQL_STANDARD;