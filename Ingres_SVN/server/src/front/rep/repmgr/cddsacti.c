# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <si.h>
# include <me.h>
# include <er.h>
# include <te.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include <tblobjs.h>
# include <tbldef.h>
# include "errm.h"
/*
** Name:	cddsacti.sc - CDDS activate
**
** Description:
**	Defines
**		turn_recorder	- turn Change Recorder on or off
**		cdds_activate - creates activation rules for a CDDS
**		tbl_deactivate - drops change recorder rules for a table
**
** History:
**	09-jan-97 (joea)
**		Created based on cddsacti.sc in replicator library.
**	26-mar-97 (joea)
**		Call RMtcb_flush after deactivating a table.
**	19-may-97 (joea) bug 82149
**		Deal with tbl_rules() returning -1 (no need to activate).
**	25-sep-97 (joea)
**		Remove unused second argument to tbl_deactivate.
**	09-oct-97 (joea)
**		Remove unused argument to tbl_rules.
**	04-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr. In cdds_activate, retrieve
**		the number of tables in the CDDS and then allocate memory for
**		the table numbers instead of having an arbitrary limit.
**	10-dec-98 (abbjo03)
**		Change action/tbl_activate parameters to a bool. Eliminate
**		dbname parameter to cdds_activate.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF TBLDEF	*RMtbl;
STATUS db_config_changed(i2 db_no);
FUNC_EXTERN STATUS RMtcb_flush(void);
STATUS tbl_deactivate(i4 table_no);
# ifdef NT_GENERIC
STATUS std_turn_recorder(char *dbname, i2 cdds_no, bool activate);
STATUS
turn_recorder(
char	*dbname,
i2	cdds_no,
bool	activate)
{
	STATUS	status;
	TErestore(TE_NORMAL);
	status = std_turn_recorder(dbname, cdds_no, activate);
	TErestore(TE_FORMS);
	return (status);
}
# define turn_recorder		std_turn_recorder
# endif
/*{
** Name:	cdds_activate
**
** Description:
**	Creates or drops the change recorder rules for a CDDS.
**
** Inputs:
**	cdds_no		- integer dataset number.
**	tbl_activate	- activate or deactivate
**
** Outputs:
**	none
**
** Returns
**	0	- action successful
**	-1	- no tables in CDDS
**	err	- database error number
*/
STATUS
cdds_activate(
i2	cdds_no,
bool	tbl_activate)
# if 0
  i2 cdds_no;
# endif
{
	i4	i;
  i4 num_tables;
  i4 *table_nos;
	DBEC_ERR_INFO	errinfo;
	STATUS	err = 0;
/* # line 117 "cddsacti.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_regist_tables where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num_tables);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 121 "cddsacti.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
/* # line 123 "cddsacti.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 124 "cddsacti.sc" */	/* host code */
		IIUGerr(E_RM0082_Error_rtrv_tables, UG_ERR_ERROR, 0);
		return (errinfo.errorno);
	}
	if (!num_tables)
	{
/* # line 129 "cddsacti.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 130 "cddsacti.sc" */	/* host code */
		return (-1);
	}
	table_nos = (i4 *)MEreqmem(0, (u_i4)(num_tables * sizeof(i4 *)), TRUE,
		NULL);
	if (!table_nos)
	{
/* # line 137 "cddsacti.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 138 "cddsacti.sc" */	/* host code */
		IIUGerr(E_RM0083_Error_alloc_tables, UG_ERR_ERROR, 0);
		return (E_RM0083_Error_alloc_tables);
	}
	i = 0;
/* # line 143 "cddsacti.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_no from dd_regist_tables where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&table_nos[i]);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 148 "cddsacti.sc" */	/* host code */
		if (i >= num_tables)
/* # line 149 "cddsacti.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 150 "cddsacti.sc" */	/* host code */
		++i;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (RPdb_error_check(0, &errinfo) != OK)
	{
/* # line 154 "cddsacti.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 155 "cddsacti.sc" */	/* host code */
		if (errinfo.errorno != 0)
		{
			IIUGerr(E_RM0082_Error_rtrv_tables, UG_ERR_ERROR, 0);
			return (errinfo.errorno);
		}
	}
/* # line 161 "cddsacti.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 163 "cddsacti.sc" */	/* host code */
	if (tbl_activate)
	{
		/* Create the rules */
		for (i = 0; i < num_tables; i++)
		{
			err = tbl_rules(table_nos[i]);
			if (err != OK && err != -1)
				return (err);
		}
/* # line 172 "cddsacti.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 173 "cddsacti.sc" */	/* host code */
	}
	else	/* Drop the rules */
	{
		for (i = 0; i < num_tables; i++)
		{
			err = tbl_deactivate(table_nos[i]);
			if (err)
				return (err);
		}
/* # line 182 "cddsacti.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 183 "cddsacti.sc" */	/* host code */
	}
	return (err);
}
/*{
** Name:	tbl_deactivate
**
** Description:
**	Drops change recorder rules and updates dd_regist_tables
**	to clear timestamps.
*/
STATUS
tbl_deactivate(
i4	table_no)
{
	DBEC_ERR_INFO	errinfo;
	STATUS	err;
  i4 t_no;
	err = RMtbl_fetch(table_no, TRUE);
	if (err)
		return (err);
	t_no = RMtbl->table_no;
/* # line 212 "cddsacti.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set rules_created='' where table_no=");
    IIputdomio((short *)0,1,30,4,&t_no);
    IIsyncup((char *)0,0);
  }
/* # line 215 "cddsacti.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
		return(errinfo.errorno);
	if (db_config_changed(0) != OK)
	{
/* # line 220 "cddsacti.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 221 "cddsacti.sc" */	/* host code */
		return (FAIL);
	}
	return (RMtcb_flush());
}
/*{
** Name:	turn_recorder - turn change recorder on or off
**
** Description:
**	Turn change recorders on or off, i.e. create or drop
**	rules for tables in the CDDS.
**
** Inputs:
**	dbname	- target db name
**	cdds_no	- cdds no
**	activate	- activate or deactivate
**
** Outputs:
**	none
**
** Returns:
**	OK	- successful
**	-1	- no tables
**	others	- error
*/
STATUS
turn_recorder(
char	*dbname,
i2	cdds_no,
bool	activate)
# if 0
  char *dbname;
# endif
{
	DBEC_ERR_INFO	errinfo;
  char dba[DB_MAXNAME+1];
  char username[DB_MAXNAME+1];
  i4 other_session = 200;
  i4 old_session;
	STATUS	retval;
/* # line 268 "cddsacti.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&old_session,11);
  }
/* # line 270 "cddsacti.sc" */	/* host code */
	if (!activate)
		SIprintf("Deactivate CDDS '%d' in database '%s'. . .\n\r",
			cdds_no, dbname);
	else
		SIprintf("Activate CDDS '%d' in database '%s'. . .\n\r",
			cdds_no, dbname);
	SIflush(stdout);
/* # line 278 "cddsacti.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 279 "cddsacti.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
	{
/* # line 281 "cddsacti.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 282 "cddsacti.sc" */	/* host code */
		IIUGerr(E_RM0084_Error_connecting, UG_ERR_ERROR, 1, dbname);
		return (E_RM0084_Error_connecting);
	}
	/* See if user is the DBA; if not, reconnect as DBA. */
/* # line 287 "cddsacti.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select DBMSINFO('dba'), DBMSINFO('username')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dba);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,username);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 289 "cddsacti.sc" */	/* host code */
	if (STcompare(dba, username))
	{
/* # line 291 "cddsacti.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 292 "cddsacti.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqUser(dba);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 294 "cddsacti.sc" */	/* host code */
		if (RPdb_error_check(0, &errinfo))
		{
/* # line 296 "cddsacti.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 297 "cddsacti.sc" */	/* host code */
			IIUGerr(E_RM0084_Error_connecting, UG_ERR_ERROR, 1,
				dbname);
			return (E_RM0084_Error_connecting);
		}
	}
	retval = cdds_activate(cdds_no, activate);
	if (retval == 0 || retval == -1)
/* # line 306 "cddsacti.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 307 "cddsacti.sc" */	/* host code */
	else
/* # line 308 "cddsacti.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 310 "cddsacti.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(other_session);
    IIsqDisconnect();
  }
/* # line 311 "cddsacti.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&old_session);
  }
/* # line 313 "cddsacti.sc" */	/* host code */
	return (retval);
}
