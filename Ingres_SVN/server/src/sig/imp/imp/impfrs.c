# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	impfrs.sc
**
** FRS routines for the new IMA based IPM lookalike tool.
**
** Basically - whenever a new form is added to the utility - it's name should 
** be added in here and an 'addform' done.
**
## History:
##
## 26-jan-94	(johna)
##		started on RIF day from pervious ad-hoc programs
## 04-may-94	(johna)
##		commented.
## 22-May-95 	(johna)
##		Added imp_server_diag - server internals diagram
##		Added ima_dmf_cache_stats -
## 21-Jun-95 	(johna)
##		Added ima_qsf_cache_stats
## 22-Jun-95 	(johna)
##		Added ima_qsf_dbp
## 07-Jul-95 	(johna)
##		Added ima_qsf_rqp and ima_rdf_cache_info ima_dio
## 08-Jul-95 	(johna)
##		Added ima_di_slave_info
##
## 02-Jul-2002 (fanra01)
##              Sir 108159
##              Added to distribution in the sig directory.
## 03-Sep-03 	(flomi02) mdf030903
##		Added imp_query_info
## 04-Sep-03 	(flomi02) mdf040903
##		Added imp_transaction_detail
##		Added imp_transaction_hexdump
##		Added imp_gcc_info
## 06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
  extern int imp_client_info;
  extern int imp_query_info;
  extern int imp_lg_transactions;
  extern int imp_lg_databases;
  extern int imp_lg_processes;
  extern int imp_lg_header;
  extern int imp_main_menu;
  extern int imp_server_list;
  extern int imp_session_detail;
  extern int imp_session_list;
  extern int imp_server_popup;
  extern int imp_server_parms;
  extern int imp_lock_summary;
  extern int imp_lg_menu;
  extern int imp_lg_summary;
  extern int imp_lk_menu;
  extern int imp_lk_list;
  extern int imp_lk_tx;
  extern int imp_lk_locks;
  extern int imp_res_locks;
  extern int imp_res_list;
  extern int imp_server_diag;
  extern int imp_dmf_cache_stats;
  extern int ima_qsf_cache_stats;
  extern int ima_qsf_dbp;
  extern int ima_qsf_rqp;
  extern int ima_rdf_cache_info;
  extern int ima_dio;
  extern int ima_di_slave_info;
  extern int imp_dmfcache;
  extern int imp_transaction_detail;
  extern int imp_transaction_hexdump;
  extern int imp_gcc_info;
  extern int inFormsMode;
int
setupFrs()
{
/* # line 100 "impfrs.sc" */	/* forms */
  {
    IIforms(0);
  }
/* # line 101 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_main_menu);
  }
/* # line 102 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_server_list);
  }
/* # line 103 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_session_list);
  }
/* # line 104 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_server_popup);
  }
/* # line 105 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_session_detail);
  }
/* # line 106 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_server_parms);
  }
/* # line 107 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lock_summary);
  }
/* # line 108 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lk_menu);
  }
/* # line 109 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lk_list);
  }
/* # line 110 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lk_tx);
  }
/* # line 111 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lk_locks);
  }
/* # line 112 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_res_locks);
  }
/* # line 113 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_res_list);
  }
/* # line 114 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lg_menu);
  }
/* # line 115 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lg_summary);
  }
/* # line 116 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lg_header);
  }
/* # line 117 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lg_processes);
  }
/* # line 118 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lg_databases);
  }
/* # line 119 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_lg_transactions);
  }
/* # line 120 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_client_info);
  }
/* # line 121 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_query_info);
  }
/* # line 122 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_server_diag);
  }
/* # line 123 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_dmf_cache_stats);
  }
/* # line 124 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_dmfcache);
  }
/* # line 125 "impfrs.sc" */	/* addform */
  {
    IIaddform(ima_qsf_cache_stats);
  }
/* # line 126 "impfrs.sc" */	/* addform */
  {
    IIaddform(ima_qsf_dbp);
  }
/* # line 127 "impfrs.sc" */	/* addform */
  {
    IIaddform(ima_qsf_rqp);
  }
/* # line 128 "impfrs.sc" */	/* addform */
  {
    IIaddform(ima_rdf_cache_info);
  }
/* # line 129 "impfrs.sc" */	/* addform */
  {
    IIaddform(ima_dio);
  }
/* # line 130 "impfrs.sc" */	/* addform */
  {
    IIaddform(ima_di_slave_info);
  }
/* # line 131 "impfrs.sc" */	/* host code */
	/*exec frs addform :imp_gcn_info;   */
/* # line 132 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_transaction_detail);
  }
/* # line 133 "impfrs.sc" */	/* host code */
/*
## mdf040903 
*/
/* # line 136 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_transaction_hexdump);
  }
/* # line 137 "impfrs.sc" */	/* addform */
  {
    IIaddform(imp_gcc_info);
  }
/* # line 138 "impfrs.sc" */	/* host code */
	inFormsMode = TRUE;
	return(TRUE);
}
int
closeFrs()
{
	if (inFormsMode) {
/* # line 146 "impfrs.sc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 147 "impfrs.sc" */	/* endforms */
  {
    IIendforms();
  }
/* # line 148 "impfrs.sc" */	/* host code */
		inFormsMode = FALSE;
	}
	return(TRUE);
}
