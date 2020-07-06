# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include <compat.h>
# include <st.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <generr.h>
# include <rpu.h>
# include <tblobjs.h>
# include <tbldef.h>
# include <targtype.h>
# include "errm.h"
/*
** Name:	dropobj.sc - drop a database object
**
** Description:
**	Defines
**		dropobj		- drop a database object
**		dropsupp	- drop support objects
**
** History:
**	10-jan-97 (joea)
**		Created based on dropobj.sc in replicator library.
**	14-oct-97 (joea) bug 83765
**		Remove unused code for switching sessions if table owner is not
**		the DBA.  In dropsupp(), if the target is URO, return without
**		attempting to drop objects.
**	24-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr.
**/
GLOBALREF TBLDEF	*RMtbl;
/*{
** Name:	dropobj - drop a support object
**
** Description:
**	Drops a single support objects.
**
** Inputs:
**	object_name	- name of the object
**	objtype		- type of the object
**
** Outputs:
**	none
**
** Returns:
**	OK, E_RM00DA_Err_drop_obj, or errinfo.errorno.
*/
STATUS
dropobj(
char	*object_name,
char	*objtype)
# if 0
  char *object_name;
# endif
{
	DBEC_ERR_INFO	errinfo;
  char buff[256];
  char sql_buff[80];
	STprintf(sql_buff, ERx("DROP %s %s"), objtype, object_name);
/* # line 75 "dropobj.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_buff);
    IIsyncup((char *)0,0);
  }
/* # line 76 "dropobj.sc" */	/* host code */
	RPdb_error_check(0, &errinfo);
	/* Rollback on all errors EXCEPT object not found */
	if (errinfo.errorno && errinfo.errorno != GE_NOT_FOUND &&
		errinfo.errorno != GE_TABLE_NOT_FOUND)
	{
/* # line 82 "dropobj.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,32,255,buff,63);
  }
/* # line 83 "dropobj.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 84 "dropobj.sc" */	/* host code */
		IIUGerr(E_RM00DA_Err_drop_obj, UG_ERR_ERROR, 2, object_name,
			buff);
		return (E_RM00DA_Err_drop_obj);
	}
	if (STequal(objtype, ERx("TABLE")))
	{
/* # line 92 "dropobj.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_support_tables where table_name=");
    IIputdomio((short *)0,1,32,0,object_name);
    IIsyncup((char *)0,0);
  }
/* # line 93 "dropobj.sc" */	/* host code */
		/*
		** FIXME:  In theory, there should be a single row in
		** dd_suport tables, but in practice sometimes it has
		** disappeared and upstream don't report an error.
		*/
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
/* # line 100 "dropobj.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 101 "dropobj.sc" */	/* host code */
			return (errinfo.errorno);
		}
	}
	return (OK);
}
/*{
** Name:	dropsupp - drop support objects
**
** Description:
**	Drops all support objects for a given table.
**
** Inputs:
**	table_no	- table number
**
** Outputs:
**	none
**
** Returns:
**	OK, or errors from dropobj.
*/
STATUS
dropsupp(
i4	table_no)
{
	STATUS	err;
	if (err = RMtbl_fetch(table_no, TRUE))
		return (err);
	if (RMtbl == NULL || RMtbl->target_type == TARG_UNPROT_READ)
		return (OK);
	if (err = dropobj(RMtbl->sha_name, ERx("TABLE")))
		return (err);
	if (err = dropobj(RMtbl->arc_name, ERx("TABLE")))
		return (err);
	if (err = dropobj(RMtbl->rem_ins_proc, ERx("PROCEDURE")))
		return (err);
	if (err = dropobj(RMtbl->rem_upd_proc, ERx("PROCEDURE")))
		return (err);
	if (err = dropobj(RMtbl->rem_del_proc, ERx("PROCEDURE")))
		return (err);
/* # line 150 "dropobj.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 152 "dropobj.sc" */	/* host code */
	return (OK);
}
