# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <si.h>
# include <lo.h>
# include <er.h>
# include <pc.h>
# include <te.h>
# include <gl.h>
# include <iicommon.h>
# include <generr.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include <targtype.h>
# include "errm.h"
# include <tbldef.h>
/*
** Name:	tblprocs.sc - table database procedures
**
** Description:
**	Defines
**		tbl_dbprocs	- create database procedures
**		drop_proclist	- drop procedures
**		grant_proc_to_tblOwner - grant execute to owner
**
** History:
**	09-jan-97 (joea)
**		Created based on tblprocs.sc in replicator library.
**	14-jan-97 (joea)
**		Use defined constants for target types.
**	02-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define NUMPROCS	3
GLOBALREF
  TBLDEF *RMtbl;
  typedef struct {
    char procedure_name[DB_MAXNAME+1];
    char tlp_name[13];
    char ofile[6];
    bool created;
  } PLIST;
static STATUS grant_proc_to_tblOwner(PLIST proclist[]);
static STATUS drop_proclist(PLIST proclist[]);
# ifdef NT_GENERIC
STATUS
tbl_dbprocs(
i4	table_no)
{
	STATUS	status;
	TErestore(TE_NORMAL);
	status = std_tbl_dbprocs(table_no);
	TErestore(TE_FORMS);
	return (status);
}
# define tbl_dbprocs	std_tbl_dbprocs
# endif
/*{
** Name:	tbl_dbprocs - create database procedures
**
** Description:
**	Creates the database procedures for the highlighted table on the
**	registration screen.
**
** Inputs:
**	table_no	- table number
**
** Outputs:
**	none
**
** Returns:
**	E_RM0085_No_cols_registered
*/
STATUS
tbl_dbprocs(
i4	table_no)
{
	DBEC_ERR_INFO	errinfo;
  char dba[DB_MAXNAME+1];
  i4 cnt;
  i4 is_stale;
  i4 err, err1;
  PLIST proclist[3];
	i4	i;
	err = RMtbl_fetch(table_no, FALSE);
	if (err)
		return (err);
	/* For UPRO cdds, no dbprocs are required */
	if (RMtbl->target_type == TARG_UNPROT_READ)
	{
		IIUGerr(E_RM00F4_No_URO_procs, UG_ERR_ERROR, 0);
		return (E_RM00F4_No_URO_procs);
	}
	/*
	** Check to see if columns have been registered and support tables
	** built
	*/
/* # line 132 "tblprocs.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select DBMSINFO('dba')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dba);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 133 "tblprocs.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		IIUGerr(E_RM00F5_Err_rtrv_dba, UG_ERR_ERROR, 0);
		return (E_RM00F5_Err_rtrv_dba);
	}
/* # line 138 "tblprocs.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 140 "tblprocs.sc" */	/* host code */
	if (*RMtbl->columns_registered == EOS)	/* no columns */
	{
		IIUGerr(E_RM0085_No_cols_registered, UG_ERR_ERROR, 2,
			RMtbl->table_owner, RMtbl->table_name);
		return (E_RM0085_No_cols_registered);
	}
	/* Build list of procedures and templates */
	STcopy(RMtbl->rem_ins_proc, proclist[0].procedure_name);
	STprintf(proclist[0].tlp_name, ERx("insproc%d.tlp"),
		RMtbl->target_type);
	STcopy(ERx("pi"), proclist[0].ofile);
	STcopy(RMtbl->rem_upd_proc, proclist[1].procedure_name);
	STprintf(proclist[1].tlp_name, ERx("updproc%d.tlp"),
		RMtbl->target_type);
	STcopy(ERx("pu"), proclist[1].ofile);
	STcopy(RMtbl->rem_del_proc, proclist[2].procedure_name);
	STprintf(proclist[2].tlp_name, ERx("delproc%d.tlp"),
		RMtbl->target_type);
	STcopy(ERx("pd"), proclist[2].ofile);
	/*
	** Check for the timestamp on the first procedure and compute
	** is_stale
	*/
/* # line 168 "tblprocs.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct INT4(interval('seconds', DATE(");
    IIputdomio((short *)0,1,32,0,RMtbl->columns_registered);
    IIwritio(0,(short *)0,1,32,0,(char *)
") -DATE(create_date)))from iiprocedures where procedure_name=");
    IIputdomio((short *)0,1,32,0,RMtbl->rem_ins_proc);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&is_stale);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 173 "tblprocs.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
		if (errinfo.rowcount == 0)
		{
			is_stale = TRUE;	/* proc does not exist */
		}
		else
		{
			IIUGerr(E_RM00F6_Err_rtrv_proc_time, UG_ERR_ERROR, 2,
				RMtbl->table_owner, RMtbl->table_name);
			return (E_RM00F6_Err_rtrv_proc_time);
		}
	}
	if (is_stale < 0)
		return (-1);	/* Denotes no need to proceed */
	/*
	** Go through proclist, creating each in turn.  As a precaution, use
	** dropobj() to be sure the object doesn't exist.
	*/
	for (i = 0 ; i < NUMPROCS; i++)
	{
		proclist[i].created = FALSE;
		err = dropobj(proclist[i].procedure_name, ERx("PROCEDURE"));
		if (err)
		{
/* # line 200 "tblprocs.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 201 "tblprocs.sc" */	/* host code */
			err1 = drop_proclist(proclist);
			if (err1)
				return (err1);
/* # line 204 "tblprocs.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 205 "tblprocs.sc" */	/* host code */
			return (err);
		}
/* # line 207 "tblprocs.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 209 "tblprocs.sc" */	/* host code */
		SIprintf("Creating procedure '%s' . . .\n\r",
			proclist[i].procedure_name);
		SIflush(stdout);
		/*
		** sql_gen() now executes the procedure immediately, and checks
		** for errors.  If GE_NO_RESOURCE is returned, the query is
		** written to a file and executed via TM in which case we need
		** to perform the select to determine success of sql_gen()
		*/
		err = sql_gen(RMtbl->table_name, RMtbl->table_owner,
			RMtbl->table_no, proclist[i].tlp_name,
			proclist[i].ofile);
		if (err && err != GE_NO_RESOURCE)
		{
/* # line 224 "tblprocs.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 225 "tblprocs.sc" */	/* host code */
			err1 = drop_proclist(proclist);
/* # line 226 "tblprocs.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 227 "tblprocs.sc" */	/* host code */
			if (err1)
				return (err1);
			if (err == GE_NO_PRIVILEGE)
				IIUGerr(E_RM00F7_No_grant_permit, UG_ERR_ERROR,
					0);
			else
				IIUGerr(E_RM00F8_Err_creat_proc, UG_ERR_ERROR,
					1, proclist[i].procedure_name);
			return (err);
		}
		if (err == GE_NO_RESOURCE)
		{
/* # line 240 "tblprocs.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiprocedures where procedure_name=");
    IIputdomio((short *)0,1,32,0,proclist[i].procedure_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and procedure_owner=");
    IIputdomio((short *)0,1,32,0,dba);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 245 "tblprocs.sc" */	/* host code */
			if (cnt == 0)
			{
				IIUGerr(E_RM00F8_Err_creat_proc, UG_ERR_ERROR,
					1, proclist[i].procedure_name);
				err = drop_proclist(proclist);
/* # line 250 "tblprocs.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 251 "tblprocs.sc" */	/* host code */
				save_rpgen_log();
				return (E_RM00F8_Err_creat_proc);
			}
/* # line 254 "tblprocs.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 255 "tblprocs.sc" */	/* host code */
		}
		proclist[i].created = TRUE;
	}
	err = grant_proc_to_tblOwner(proclist);
	if (err)
	{
/* # line 262 "tblprocs.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 263 "tblprocs.sc" */	/* host code */
		err1 = drop_proclist(proclist);
/* # line 264 "tblprocs.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 265 "tblprocs.sc" */	/* host code */
		if (err1)
			return (err1);
		if (err == GE_NO_PRIVILEGE)
			IIUGerr(E_RM00F7_No_grant_permit, UG_ERR_ERROR, 0);
		else
			IIUGerr(E_RM00F8_Err_creat_proc, UG_ERR_ERROR, 1,
				proclist[i].procedure_name);
		return (err);
	}
/* # line 274 "tblprocs.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 276 "tblprocs.sc" */	/* host code */
	SIprintf("Support procedures for '%s.%s' have been created . . .\n\r",
		RMtbl->table_owner, RMtbl->table_name);
	SIprintf(ERx("\n\r"));
	SIflush(stdout);
	PCsleep(2);
	return (OK);
}
static STATUS
drop_proclist(
PLIST	*proclist)
{
	STATUS	err;
	i4	i;
	err = dropobj(RMtbl->arc_name, ERx("TABLE"));
	if (err)
		return (err);
	err = dropobj(RMtbl->sha_name, ERx("TABLE"));
	if (err)
		return (err);
	for (i = 0; i < NUMPROCS; i++)
	{
		if (proclist[i].created)
		{
			if (err = dropobj(proclist[i].procedure_name,
				ERx("PROCEDURE")))
			return (err);
			IIUGmsg(ERget(F_RM00D1_Drop_proc), FALSE, 1,
				proclist[i].procedure_name);
		}
	}
	return (OK);
}
static STATUS
grant_proc_to_tblOwner(
PLIST	*proclist)
{
  char stmt_buf[128];
  char dba[DB_MAXNAME+1];
	i4	i;
/* # line 326 "tblprocs.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select DBMSINFO('dba')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dba);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 328 "tblprocs.sc" */	/* host code */
	if (STequal(dba, RMtbl->table_owner))
		return (OK);
	for (i = 0; i < NUMPROCS; i++)
	{
		if (proclist[i].created)
		{
			STprintf(stmt_buf,
				ERx("GRANT EXECUTE ON PROCEDURE %s TO %s"),
				proclist[i].procedure_name,
				RMtbl->dlm_table_owner);
/* # line 339 "tblprocs.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(stmt_buf);
    IIsyncup((char *)0,0);
  }
/* # line 340 "tblprocs.sc" */	/* host code */
			if (RPdb_error_check(0, NULL) != OK)
			{
				IIUGerr(E_RM00F9_Err_grant_proc, UG_ERR_ERROR,
					1, proclist[i].procedure_name);
				return (E_RM00F9_Err_grant_proc);
			}
		}
	}
	return (OK);
}
