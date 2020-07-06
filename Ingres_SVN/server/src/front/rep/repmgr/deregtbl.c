# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <tblobjs.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	deregtbl.sc - De-register replicated table
**
** Description:
**	Defines
**		deregister	- de-register a replicated table
**
** History:
**	16-dec-96 (joea)
**		Created based on deregtbl.sc in replicator library.
**	10-oct-97 (joea)
**		Remove unused argument to dropsupp and deregister.
**      16-Aug-98 (islsa01)
**              Call RMtcb_flush() to toss the tcb after deregisterring a table
**	25-sep-98 (abbjo03)
**		Replace ddba_messageit and string literals with IIUGerr calls.
**/
FUNC_EXTERN STATUS db_config_changed(i2 db_no);
FUNC_EXTERN STATUS RMtcb_flush(void);
/*{
** Name:	deregister - de-register a replicated table
**
** Description:
**	Removes registration information, rules, procedures, and
**	support tables for the specified table.
**
** Inputs:
**	table_no	- numeric identifier for the table
**
** Outputs:
**	none
**
** Returns:
**	0 for success
**	E_RM00ED_Cant_dereg_tbl, E_RM00EC_Err_del_reg_tbl
**	Also returns statuses from the "drop" routines
*/
STATUS
deregister(
i4	*table_no)
# if 0
  i4 *table_no;
# endif
{
	DBEC_ERR_INFO errinfo;
	STATUS	err;
  i4 NumQueue;
/* # line 68 "deregtbl.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_distrib_queue where table_no=");
    IIputdomio((short *)0,1,30,4,table_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&NumQueue);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 72 "deregtbl.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
		return (FAIL);
	if (NumQueue > 0)
	{
		IIUGerr(E_RM00ED_Cant_dereg_tbl, UG_ERR_ERROR, 0);
		return (E_RM00ED_Cant_dereg_tbl);
	}
/* # line 81 "deregtbl.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_input_queue where table_no=");
    IIputdomio((short *)0,1,30,4,table_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&NumQueue);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 85 "deregtbl.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
		return (FAIL);
	if (NumQueue > 0)
	{
		IIUGerr(E_RM00ED_Cant_dereg_tbl, UG_ERR_ERROR, 0);
		return (E_RM00ED_Cant_dereg_tbl);
	}
	/* Drop all support objects */
	err = dropsupp(*table_no);
	if (err)
		return (err);
	/* Now delete from registration tables */
/* # line 101 "deregtbl.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_regist_columns where table_no=");
    IIputdomio((short *)0,1,30,4,table_no);
    IIsyncup((char *)0,0);
  }
/* # line 102 "deregtbl.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
	{
/* # line 104 "deregtbl.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 105 "deregtbl.sc" */	/* host code */
		IIUGerr(E_RM00EC_Err_del_reg_tbl, UG_ERR_ERROR, table_no);
		return (E_RM00EC_Err_del_reg_tbl);
	}
/* # line 110 "deregtbl.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_regist_tables where table_no=");
    IIputdomio((short *)0,1,30,4,table_no);
    IIsyncup((char *)0,0);
  }
/* # line 111 "deregtbl.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo))
	{
/* # line 113 "deregtbl.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 114 "deregtbl.sc" */	/* host code */
		IIUGerr(E_RM00EC_Err_del_reg_tbl, UG_ERR_ERROR, table_no);
		return (E_RM00EC_Err_del_reg_tbl);
	}
	if (db_config_changed(0) != OK)
	{
/* # line 120 "deregtbl.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 121 "deregtbl.sc" */	/* host code */
		return (FAIL);
	}
/* # line 124 "deregtbl.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 125 "deregtbl.sc" */	/* host code */
        *table_no = 0;
        return (RMtcb_flush());
}
