# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <me.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <generr.h>
# include <rpu.h>
# include "collisn.h"
# include "errm.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
# include <tbldef.h>
/*
# include <tbldef.h>
*/
/*
** Name:	colllist.sc - build list of collisions
**
** Description:
**	Defines
**		get_collisions		- build list of collisions
**		check_insert		- check for insert collisions
**		check_update_delete	- check for update/delete collisions
**		init_sessions		- init db sessions for targets
**		close_sessions		- close sessions
**
** History:
**	09-jan-97 (joea)
**		Created based on rslvmann.sc in replicator library.
**	20-jan-97 (joea)
**		Add msg_func parameter to create_key_clauses.
**	22-jun-97 (joea)
**		Discontinue using the ingrep role.
**	27-May-98 (kinte01)
**		Add include of me.h as the code references MEreqmem. In
**		me.h IIMEreqmem is defined as MEreqmem and it is the
**		function IIMEreqmem that is provided in the VMS compatlib
**		shared library.
**	01-oct-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr. Make cursor select from
**		distribution queue consistent with RepServer's select.
**	30-apr-99 (abbjo03)
**		Change Dist_queue_empty message to error.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
  typedef struct {
    bool connected;
    i2 database_no;
    char vnode_name[DB_MAXNAME+1];
    char database_name[DB_MAXNAME+1];
    char dba[DB_MAXNAME+1];
    char full_name[DB_MAXNAME*2+3];
  } DBSESSION;
typedef void MSG_FUNC(i4 msg_level, i4  msg_num, ...);
GLOBALDEF COLLISION_FOUND collision_list[1000];
GLOBALDEF i4	unresolved_collisions;
GLOBALREF TBLDEF	*RMtbl;
static i4	NumCollisions;
  static DBSESSION *Session;
  static i4 NumSessions;
  static i4 initial_session;
static void check_insert(i4 *collision_count, i4  localdb, i2 targetdb,
	i2 sourcedb, i4 transaction_id, i4 sequence_no, i4 table_no,
	char *local_owner);
static void check_update_delete(i4 trans_type, i4  *collision_count,
	i4 localdb, i2 targetdb, i2 old_sourcedb, i4 old_transaction_id,
	i4 old_sequence_no, i2 sourcedb, i4 transaction_id, i4 sequence_no);
STATUS init_sessions(char *localdbname, char *vnode_name, i4  localdb);
STATUS close_sessions(void);
FUNC_EXTERN void ddba_messageit(i4 msg_level, i4  msg_num, ...);
FUNC_EXTERN STATUS create_key_clauses(char *where_clause, char *name_clause,
	char *insert_value_clause, char *update_clause, char *table_name,
	char *table_owner, i4 table_no, char *sha_name, i2 database_no,
	i4 transaction_id, i4 sequence_no, MSG_FUNC *msg_func);
/*{
** Name:	get_collisions	- build list of collisions
**
** Description:
**	Populate the collision_list[] array with any queue collisions found by
**	analyzing the local dd_distrib_queue.
** Inputs:
**	localdbname	- char string containing local dbname
**	vnode_name	- char string containing local vnode
**	localdb		- local database number
**	local_owner	- local database owner
**
** Outputs:
**	collision_count	- integer count of # of found collisions
**	records		- integer count of # of distribution queue records
**			  inspected.
*/
STATUS
get_collisions(
char	*localdbname,
char	*vnode_name,
i2	localdb,
char	*local_owner,
i4	*collision_count,
i4	*records)
# if 0
  char *localdbname;
  char *vnode_name;
  i2 localdb;
# endif
{
	DBEC_ERR_INFO errinfo;
  STATUS err;
  i2 targetdb;
  i2 sourcedb;
  i4 transaction_id;
  i4 sequence_no;
  i2 trans_type;
  char trans_time[26];
  i2 cdds_no;
  i4 table_no = 0;
  i2 old_sourcedb;
  i4 old_transaction_id = 0;
  i4 old_sequence_no = 0;
  i4 done;
	IISQLDA	_sqlda;
	IISQLDA	*sqlda = &_sqlda;
/* # line 158 "colllist.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&initial_session,11);
  }
/* # line 159 "colllist.sc" */	/* host code */
	if (err = init_sessions(localdbname, vnode_name, localdb))
	{
/* # line 161 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 162 "colllist.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 163 "colllist.sc" */	/* host code */
		if (err == E_RM0123_Dist_queue_empty)
			return (OK);	/* no entries to process */
		else
			return (err);
	}
/* # line 169 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 171 "colllist.sc" */	/* host code */
	*collision_count = 0;
	*records = 0;
/* # line 188 "colllist.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c3",7208,3838);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select sourcedb, transaction_id, sequence_no, table_no, old_sourcedb,\
 old_transaction_id, old_sequence_no, trans_time, trans_type, q.cdds_n\
o, targetdb from dd_distrib_queue q, dd_db_cdds c, dd_databases d wher\
e q.cdds_no=c.cdds_no and targetdb=c.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"database_no and c.database_no=d.database_no and c.target_type in(1, 2\
)and LOWERCASE(d.vnode_name)!='mobile' order by trans_time, targetdb, \
sourcedb, transaction_id, cdds_no, sequence_no");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c3",7208,3838);
  }
/* # line 189 "colllist.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
	{
/* # line 191 "colllist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 192 "colllist.sc" */	/* host code */
		IIUGerr(E_RM0116_Err_open_queue_curs, UG_ERR_ERROR, 0);
/* # line 193 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 194 "colllist.sc" */	/* host code */
		return (E_RM0116_Err_open_queue_curs);
	}
	done = FALSE;
	while (!done)
	{
/* # line 200 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 201 "colllist.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c3",7208,3838,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,2,&sourcedb);
      IIcsGetio((short *)0,1,30,4,&transaction_id);
      IIcsGetio((short *)0,1,30,4,&sequence_no);
      IIcsGetio((short *)0,1,30,4,&table_no);
      IIcsGetio((short *)0,1,30,2,&old_sourcedb);
      IIcsGetio((short *)0,1,30,4,&old_transaction_id);
      IIcsGetio((short *)0,1,30,4,&old_sequence_no);
      IIcsGetio((short *)0,1,32,25,trans_time);
      IIcsGetio((short *)0,1,30,2,&trans_type);
      IIcsGetio((short *)0,1,30,2,&cdds_no);
      IIcsGetio((short *)0,1,30,2,&targetdb);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 206 "colllist.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 207 "colllist.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo))
		{
/* # line 209 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 210 "colllist.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c3",7208,3838);
  }
/* # line 211 "colllist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 212 "colllist.sc" */	/* host code */
			IIUGerr(E_RM0117_Err_fetch_queue_curs, UG_ERR_ERROR, 0);
/* # line 213 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 214 "colllist.sc" */	/* host code */
			return (E_RM0117_Err_fetch_queue_curs);
		}
		if (!done)
		{
			*records += 1;
			err = RMtbl_fetch(table_no, TRUE);
			if (err)
			{
/* # line 224 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 225 "colllist.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c3",7208,3838);
  }
/* # line 226 "colllist.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 227 "colllist.sc" */	/* host code */
				return (err);
			}
			switch(trans_type)
			{
			case TX_INSERT:
				check_insert(collision_count, localdb, targetdb,
					sourcedb, transaction_id,
					sequence_no, table_no, local_owner);
				break;
			case TX_UPDATE:
			case TX_DELETE:
				check_update_delete((i4)trans_type,
					collision_count, localdb, targetdb,
					old_sourcedb, old_transaction_id,
					old_sequence_no, sourcedb,
					transaction_id, sequence_no);
				break;
			}
		}
	}
/* # line 250 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 251 "colllist.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c3",7208,3838);
  }
/* # line 252 "colllist.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 254 "colllist.sc" */	/* host code */
	/* Initialize count of unresolved collisions */
	unresolved_collisions = *collision_count;
	NumCollisions = *collision_count;
	if (NumCollisions == 0)
	{
		if (close_sessions() != 0)
			return (-1);
	}
	return (OK);
}
/*{
** Name:	check_insert - check for insert collisions
**
** Description:
**	check for an insert collision & report it if found.
**
** Inputs:
**	localdb		-
**	target		-
**	database_no	-
**	transaction_id	-
**	sequence_no	-
**	table_no	-
**
** Outputs:
**	collision_count	-
**
** Returns:
**	none
*/
static void
check_insert(
i4	*collision_count,
i4	localdb,
i2	targetdb,
i2	sourcedb,
i4	transaction_id,
i4	sequence_no,
i4	table_no,
char	*local_owner)
# if 0
  i2 targetdb;
  i2 sourcedb;
  i4 transaction_id;
  i4 sequence_no;
# endif
{
	DBEC_ERR_INFO errinfo;
  STATUS err;
  i4 cnt;
  char select_statement[1024];
  char where_clause[1024];
	i4	i;
	char	*target_owner;
/* # line 317 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 319 "colllist.sc" */	/* host code */
	err = create_key_clauses(where_clause, NULL, NULL, NULL,
		RMtbl->table_name, RMtbl->table_owner, RMtbl->table_no,
		RMtbl->sha_name, sourcedb, transaction_id, sequence_no,
		ddba_messageit);
	if (err)
	{
/* # line 325 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 326 "colllist.sc" */	/* host code */
		return;
	}
	if (STequal(RMtbl->table_owner, local_owner))
	{
		for (i = 1; i < NumSessions; ++i)
			if (Session[i].database_no == targetdb)
				break;
		target_owner = Session[i].dba;
	}
	else
	{
		target_owner = RMtbl->dlm_table_owner;
	}
	/* is the row to be inserted already in the user table on the target */
	STprintf(select_statement,
		ERx("SELECT COUNT(*) FROM %s.%s t WHERE %s"),
		target_owner, RMtbl->dlm_table_name, where_clause);
/* # line 345 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,2,&targetdb);
  }
/* # line 347 "colllist.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(select_statement);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 348 "colllist.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo))
	{
/* # line 350 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 351 "colllist.sc" */	/* host code */
		IIUGerr(E_RM0118_Err_ck_targ_row, UG_ERR_ERROR, 0);
		return;
	}
	if (cnt)	/* collision */
	{
		collision_list[*collision_count].local_db = (i2)localdb;
		collision_list[*collision_count].remote_db = targetdb;
		collision_list[*collision_count].type = TX_INSERT;
		collision_list[*collision_count].resolved = FALSE;
		collision_list[*collision_count].db1.sourcedb = sourcedb;
		collision_list[*collision_count].db1.transaction_id =
			transaction_id;
		collision_list[*collision_count].db1.sequence_no = sequence_no;
		collision_list[*collision_count].db1.table_no =
			RMtbl->table_no;
		sourcedb = (i2)0;
		transaction_id = sequence_no = 0;
		STprintf(select_statement,
			ERx("SELECT sourcedb, transaction_id, sequence_no \
FROM %s t WHERE in_archive = 0 AND %s"),
			RMtbl->sha_name, where_clause);
/* # line 374 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,2,&targetdb);
  }
/* # line 376 "colllist.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(select_statement);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&sourcedb);
      IIgetdomio((short *)0,1,30,4,&transaction_id);
      IIgetdomio((short *)0,1,30,4,&sequence_no);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 378 "colllist.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo))
		{
/* # line 380 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 381 "colllist.sc" */	/* host code */
			IIUGerr(E_RM0119_Err_rtrv_shad_row, UG_ERR_ERROR, 0);
			return;
		}
		collision_list[*collision_count].db2.sourcedb = sourcedb;
		collision_list[*collision_count].db2.transaction_id =
			transaction_id;
		collision_list[*collision_count].db2.sequence_no = sequence_no;
		collision_list[*collision_count].db2.table_no = RMtbl->table_no;
		*collision_count += 1;
	}
/* # line 393 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 394 "colllist.sc" */	/* host code */
}
/*{
** Name:	check_update_delete - check for update/delete collisions
**
** Description:
**	check for an update or delete collision & report it if found.
**
** Inputs:
**	trans_type	-
**	localdb		-
**	targetdb	-
**	old_sourcedb	-
**	old_transaction_id -
**	old_sequence_no	-
**	database_no	-
**	transaction_id	-
**	sequence_no	-
**
** Outputs:
**	collision_count	-
**
** Returns:
**	none
*/
static void
check_update_delete(
i4	trans_type,
i4	*collision_count,
i4	localdb,
i2	targetdb,
i2	old_sourcedb,
i4	old_transaction_id,
i4	old_sequence_no,
i2	sourcedb,
i4	transaction_id,
i4	sequence_no)
# if 0
  i2 targetdb;
  i2 sourcedb;
  i4 transaction_id;
  i4 sequence_no;
# endif
{
	DBEC_ERR_INFO errinfo;
  i4 cnt;
  char select_statement[1024];
/* # line 447 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,2,&targetdb);
  }
/* # line 449 "colllist.sc" */	/* host code */
	STprintf(select_statement,
		ERx("SELECT COUNT(*) FROM %s WHERE sourcedb= %d AND \
transaction_id = %d AND sequence_no = %d AND in_archive = 0"),
		RMtbl->sha_name, old_sourcedb, old_transaction_id,
		old_sequence_no);
/* # line 455 "colllist.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(select_statement);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 456 "colllist.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo))
	{
/* # line 458 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 459 "colllist.sc" */	/* host code */
		IIUGerr(E_RM011A_Err_ck_targ_shad, UG_ERR_ERROR, 0);
		return;
	}
	if (cnt == 0)	/* possible collision */
	{
		/*
		** check new_key flag in the source shadow table.  if
		** it's set, then we don't have a collision.
		*/
/* # line 468 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 470 "colllist.sc" */	/* host code */
		STprintf(select_statement,
			ERx("SELECT COUNT(*) FROM %s WHERE sourcedb = %d AND \
transaction_id = %d AND sequence_no = %d AND new_key = 1"),
			RMtbl->sha_name, old_sourcedb, old_transaction_id,
			old_sequence_no);
/* # line 476 "colllist.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(select_statement);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 477 "colllist.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo))
		{
/* # line 479 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 480 "colllist.sc" */	/* host code */
			IIUGerr(E_RM011B_Err_ck_src_shad, UG_ERR_ERROR, 0);
			return;
		}
		if (cnt == 0) /* collision */
		{
			collision_list[*collision_count].local_db =
					(i2)localdb;
			collision_list[*collision_count].remote_db = targetdb;
			collision_list[*collision_count].type = trans_type;
			collision_list[*collision_count].resolved = FALSE;
			collision_list[*collision_count].db1.sourcedb =
					sourcedb;
			collision_list[*collision_count].db1.transaction_id =
					transaction_id;
			collision_list[*collision_count].db1.sequence_no =
					sequence_no;
			collision_list[*collision_count].db1.table_no =
					RMtbl->table_no;
			*collision_count += 1;
		}
	}
/* # line 502 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 503 "colllist.sc" */	/* host code */
}
STATUS
init_sessions(
char	*localdbname,
char	*vnode_name,
i4	localdb)
# if 0
  char *localdbname;
  char *vnode_name;
  i4 localdb;
# endif
{
  char localiidbdb[100];
  char tmp[128];
  i4 initial_session;
	DBEC_ERR_INFO errinfo;
	STATUS	mem_stat;
	i4	i;
/* # line 528 "colllist.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&initial_session,11);
  }
/* # line 530 "colllist.sc" */	/* host code */
	if (*vnode_name == EOS)
		STcopy(localdbname, tmp);
	else
		STprintf(tmp, ERx("%s::%s"), vnode_name, localdbname);
/* # line 535 "colllist.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(distinct targetdb)from dd_distrib_queue");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&NumSessions);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 538 "colllist.sc" */	/* host code */
	if (RPdb_error_check(0, NULL))
	{
		IIUGerr(E_RM011C_Err_cnt_conns, UG_ERR_ERROR, 0);
		return (E_RM011C_Err_cnt_conns);
	} 
	if (NumSessions == 0)
	{
		IIUGerr(E_RM0123_Dist_queue_empty, UG_ERR_ERROR, 0);
		return (E_RM0123_Dist_queue_empty);
	}
	Session = (DBSESSION *)MEreqmem(0, (u_i4)(NumSessions + 1) *
		sizeof(DBSESSION), TRUE, &mem_stat);
	if (Session == NULL)
	{
		IIUGerr(E_RM011D_Err_alloc_sess, UG_ERR_ERROR, 0);
		return (E_RM011D_Err_alloc_sess);
	}
	/* Establish a connection to the local db */
/* # line 558 "colllist.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select DBMSINFO('dba')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,Session[0].dba);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 560 "colllist.sc" */	/* host code */
	STcopy(vnode_name, Session[0].vnode_name);
	STcopy(localdbname, Session[0].database_name);
	Session[0].database_no = (i2)localdb;
	if (*vnode_name == EOS)
		STcopy(Session[0].database_name, Session[0].full_name);
	else
		STprintf(Session[0].full_name, ERx("%s::%s"),
			Session[0].vnode_name, Session[0].database_name);
/* # line 569 "colllist.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(Session[0].database_no);
    IIsqUser(Session[0].dba);
    IIsqConnect(0,Session[0].full_name,(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 572 "colllist.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
	{
/* # line 574 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 575 "colllist.sc" */	/* host code */
		return (-1);
	}
	Session[0].connected = TRUE;
/* # line 579 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 581 "colllist.sc" */	/* host code */
	++NumSessions;
/* # line 587 "colllist.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"opendb_csr",7209,27382);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct q.targetdb, d.database_name, d.vnode_name from dd_dis\
trib_queue q, dd_databases d where q.targetdb=d.database_no");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"opendb_csr",7209,27382);
  }
/* # line 588 "colllist.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
	{
		IIUGerr(E_RM011E_Err_open_sess_curs, UG_ERR_ERROR, 0);
		return (E_RM011E_Err_open_sess_curs);
	} 
	for (i = 1; i < NumSessions; ++i)
	{
/* # line 596 "colllist.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"opendb_csr",7209,27382,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,2,&Session[i].database_no);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,Session[i].database_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,Session[i].vnode_name);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 600 "colllist.sc" */	/* host code */
		RPdb_error_check(0, &errinfo);
		STtrmwhite(Session[i].vnode_name);
		STtrmwhite(Session[i].database_name);
		switch (errinfo.errorno)
		{
		case GE_OK:
			if (*Session[i].vnode_name != EOS)
			{
				STprintf(tmp, ERx("%s::%s"),
					Session[i].vnode_name,
					Session[i].database_name);
				STprintf(localiidbdb, ERx("%s::%s"),
					Session[i].vnode_name, ERx("iidbdb"));
			} 
			else
			{
				STcopy(Session[i].database_name, tmp);
				STcopy(ERx("iidbdb"), localiidbdb);
			}
			STcopy(tmp, Session[i].full_name);
			/* Get owner of database */
/* # line 622 "colllist.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(Session[i].database_no);
    IIsqConnect(0,localiidbdb,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 624 "colllist.sc" */	/* host code */
			if (RPdb_error_check(0, &errinfo))
			{
				IIUGerr(E_RM0084_Error_connecting, UG_ERR_ERROR,
					1, localiidbdb);
				return (E_RM0084_Error_connecting);
			}
/* # line 630 "colllist.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select own from iidatabase where name=");
    IIputdomio((short *)0,1,32,0,Session[i].database_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,Session[i].dba);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 634 "colllist.sc" */	/* host code */
			if (RPdb_error_check(0, &errinfo))
			{
/* # line 636 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 637 "colllist.sc" */	/* host code */
				return (-1);
			}
			STtrmwhite(Session[i].dba);
/* # line 640 "colllist.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(Session[i].database_no);
    IIsqDisconnect();
  }
/* # line 642 "colllist.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(Session[i].database_no);
    IIsqUser(Session[i].dba);
    IIsqConnect(0,tmp,(char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0);
  }
/* # line 644 "colllist.sc" */	/* host code */
			if (RPdb_error_check(0, &errinfo))
			{
/* # line 646 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 647 "colllist.sc" */	/* host code */
				return (-1);
			}
			Session[i].connected = TRUE;
/* # line 650 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 651 "colllist.sc" */	/* host code */
			break;
		case GE_NO_MORE_DATA:
			goto closecsr;
		default:
			IIUGerr(E_RM011F_Err_fetch_sess_curs, UG_ERR_ERROR, 0);
			return (E_RM011F_Err_fetch_sess_curs);
		}
	}
closecsr:
/* # line 663 "colllist.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"opendb_csr",7209,27382);
  }
/* # line 664 "colllist.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 666 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&initial_session);
  }
/* # line 668 "colllist.sc" */	/* host code */
	return (OK);
}
STATUS
close_sessions()
{
	i4	i;
  i4 curr_session;
	if (Session == NULL)
		return (OK);
/* # line 683 "colllist.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&curr_session,11);
  }
/* # line 685 "colllist.sc" */	/* host code */
	for (i = 0; i < NumSessions; ++i)
	{
		if (Session[i].connected)
		{
/* # line 689 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,2,&Session[i].database_no);
  }
/* # line 690 "colllist.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(Session[i].database_no);
    IIsqDisconnect();
  }
/* # line 691 "colllist.sc" */	/* host code */
		}
	}
	MEfree((PTR)Session);
	Session = NULL;
/* # line 696 "colllist.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&curr_session);
  }
/* # line 697 "colllist.sc" */	/* host code */
	return (OK);
}
