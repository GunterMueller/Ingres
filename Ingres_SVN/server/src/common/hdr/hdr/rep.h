/*
** Copyright (c) 2004 Ingres Corporation Inc
*/

/*
** Name: rep.h - replicator definitions common to back and front
**
** Description:
**	This file contains definintions for replicator that are common to
**	both the backend and the frontend tools.
**
** History:
**	23-apr-97 (stephenb)
**	    Initial creation, add replicator catalog name definitions (moved
**	    from back!dmf!hdr!dm2rep.h).
**	14-aug-97 (stephenb)
**	    Add upper case catalog names for SQL92 case semantics.
**
*/

/*
** Replicator catalogs
*/
#define     DD_CDDS             "dd_cdds                         "
#define     DD_CDDS_NAME_IDX    "dd_cdds_name_idx                "
#define     DD_DATABASES        "dd_databases                    "
#define     DD_DB_CDDS          "dd_db_cdds                      "
#define     DD_DB_NAME_IDX      "dd_db_name_idx                  "
#define     DD_DBMS_TYPES       "dd_dbms_types                   "
#define     DD_DISTRIB_QUEUE    "dd_distrib_queue                "
#define     DD_EVENTS           "dd_events                       "
#define     DD_FLAG_VALUES      "dd_flag_values                  "
#define     DD_INPUT_QUEUE      "dd_input_queue                  "
#define     DD_LAST_NUMBER      "dd_last_number                  "
#define     DD_MAIL_ALERT       "dd_mail_alert                   "
#define     DD_NODES            "dd_nodes                        "
#define     DD_OPTION_VALUES    "dd_option_values                "
#define     DD_PATHS            "dd_paths                        "
#define     DD_REG_TBL_IDX      "dd_reg_tbl_idx                  "
#define     DD_REGIST_COLUMNS   "dd_regist_columns               "
#define     DD_REGIST_TABLES    "dd_regist_tables                "
#define     DD_SERVER_FLAGS     "dd_server_flags                 "
#define     DD_SERVERS          "dd_servers                      "
#define     DD_SUPPORT_TABLES   "dd_support_tables               "

#define     DD_CDDS_U             "DD_CDDS                         "
#define     DD_CDDS_NAME_IDX_U    "DD_CDDS_NAME_IDX                "
#define     DD_DATABASES_U        "DD_DATABASES                    "
#define     DD_DB_CDDS_U          "DD_DB_CDDS                      "
#define     DD_DB_NAME_IDX_U      "DD_DB_NAME_IDX                  "
#define     DD_DBMS_TYPES_U       "DD_DBMS_TYPES                   "
#define     DD_DISTRIB_QUEUE_U    "DD_DISTRIB_QUEUE                "
#define     DD_EVENTS_U           "DD_EVENTS                       "
#define     DD_FLAG_VALUES_U      "DD_FLAG_VALUES                  "
#define     DD_INPUT_QUEUE_U      "DD_INPUT_QUEUE                  "
#define     DD_LAST_NUMBER_U      "DD_LAST_NUMBER                  "
#define     DD_MAIL_ALERT_U       "DD_MAIL_ALERT                   "
#define     DD_NODES_U            "DD_NODES                        "
#define     DD_OPTION_VALUES_U    "DD_OPTION_VALUES                "
#define     DD_PATHS_U            "DD_PATHS                        "
#define     DD_REG_TBL_IDX_U      "DD_REG_TBL_IDX                  "
#define     DD_REGIST_COLUMNS_U   "DD_REGIST_COLUMNS               "
#define     DD_REGIST_TABLES_U    "DD_REGIST_TABLES                "
#define     DD_SERVER_FLAGS_U     "DD_SERVER_FLAGS                 "
#define     DD_SERVERS_U          "DD_SERVERS                      "
#define     DD_SUPPORT_TABLES_U   "DD_SUPPORT_TABLES               "
