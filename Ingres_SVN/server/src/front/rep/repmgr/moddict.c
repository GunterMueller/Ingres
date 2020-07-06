# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include <compat.h>
/**
** Name:	moddict.sc - Modify Replicator DD tables
**
** Description:
**	Defines
**		modify_tables	- Remodify DD tables
**
** History:
**	16-dec-96 (joea)
**		Created based on moddict.sc in replicator library.
**	14-jan-97 (joea)
**		Remove dd_target_types.
**	24-jan-97 (joea)
**		Add MAXPAGES to dd_last_number to prevent lock escalation.
**	22-apr-97 (joea)
**		Modify dd_input_queue only on dd_transaction_id.
**	16-jun-98 (abbjo03)
**		Add indexes argument to create persistent indexes if called
**		from repcat and convrep, but not from repmod.
**	03-sep-99 (abbjo03) sir 98645
**		Modify input queue to hash with suitable minpages. Modify
**		distribution queue to get better performance on deletes and on
**		read joins.
**	28-oct-99 (abbjo03)
**		Split out input queue modification so it can be called from
**		create_replication_keys.
**/
STATUS RMmodify_input_queue(void);
/*{
** Name:	modify_tables - Remodify Replicator DD tables
**
** Description:
**	Remodifies the Replicator data dictionary tables to their
**	optimal storage structures.
**
** FIXME:
**	This could take a parameter to avoid remodifying read-only
**	tables like dd_events.
**
** Inputs:
**	indexes	- create indexes?
**
** Outputs:
**	none
**
** Returns:
**	OK on success, FAIL if there is a database error.
*/
STATUS
modify_tables(
bool	indexes)
{
/* # line 66 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_nodes to BTREE unique on vnode_name");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 68 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_databases to BTREE unique on database_no");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 70 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_paths to BTREE unique on cdds_no, sourcedb, localdb, target\
db");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 73 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_cdds to BTREE unique on cdds_no");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 75 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_db_cdds to BTREE unique on cdds_no, database_no");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 77 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_regist_tables to BTREE unique on table_no");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 79 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_regist_columns to BTREE unique on table_no, column_name");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 82 "moddict.sc" */	/* host code */
	if (RMmodify_input_queue() != OK)
		return (FAIL);
/* # line 85 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_distrib_queue to BTREE on targetdb, cdds_no, sourcedb, tran\
saction_id");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 88 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_mail_alert to BTREE unique on mail_username");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 90 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_servers to BTREE unique on server_no");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 92 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_support_tables to BTREE unique on table_name");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 94 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_last_number to HASH unique on column_name with MAXPAGES=7");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 97 "moddict.sc" */	/* host code */
	/* read-only tables */
/* # line 99 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_events to ISAM unique on dbevent with FILLFACTOR=100");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 102 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_server_flags to ISAM unique on flag_name with FILLFACTOR=10\
0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 105 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_option_values to ISAM unique on option_type, option_name, n\
umeric_value with FILLFACTOR=100");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 109 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_flag_values to ISAM on startup_flag");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 111 "moddict.sc" */	/* host code */
	if (indexes)
	{
/* # line 113 "moddict.sc" */	/* create unique index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index dd_db_name_idx on dd_databases(vnode_name, databa\
se_name)with PERSISTENCE");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 117 "moddict.sc" */	/* create unique index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index dd_cdds_name_idx on dd_cdds(cdds_name)with PERSIS\
TENCE");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 121 "moddict.sc" */	/* create unique index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index dd_reg_tbl_idx on dd_regist_tables(table_name, ta\
ble_owner)with PERSISTENCE");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto mod_err;
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 124 "moddict.sc" */	/* host code */
	}
	return (OK);
mod_err:
	return (FAIL);
}
STATUS
RMmodify_input_queue()
{
  i4 err;
/* # line 140 "moddict.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify dd_input_queue to HASH on transaction_id with MINPAGES=1024");
    IIsyncup((char *)0,0);
    if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 142 "moddict.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 143 "moddict.sc" */	/* host code */
	return ((STATUS)err);
}
