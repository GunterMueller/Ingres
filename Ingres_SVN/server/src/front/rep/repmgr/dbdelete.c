# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <rpu.h>
/**
** Name:	dbdelete.sc - delete a database
**
** Description:
**	Defines
**		db_delete	- delete a database
**
** History:
**	16-dec-96 (joea)
**		Created based on dbdelete.sc in replicator library.
**/
STATUS db_config_changed(i2 db_no);
/*{
** Name:	db_delete - delete a database
**
** Description:
**	Delete a replicated database object from the dd_databases and related
**	catalogs.
**
** Inputs:
**	database_no	- database number
**
** Outputs:
**	none
**
** Returns:
**	OK, FAIL or DBMS error
*/
STATUS
database_delete(
i2	database_no)
# if 0
  i2 database_no;
# endif
{
	DBEC_ERR_INFO	errinfo;
/* # line 50 "dbdelete.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dbdelete1",7205,10268);
      IIputdomio((short *)0,1,30,2,&database_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dbdelete1",7205,10268);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_databases where database_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&database_no);
        IIexDefine(0,(char *)"dbdelete1",7205,10268);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 51 "dbdelete.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
/* # line 53 "dbdelete.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 54 "dbdelete.sc" */	/* host code */
		return (errinfo.errorno);
	}
/* # line 58 "dbdelete.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_db_cdds where database_no=");
    IIputdomio((short *)0,1,30,2,&database_no);
    IIsyncup((char *)0,0);
  }
/* # line 59 "dbdelete.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
/* # line 61 "dbdelete.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 62 "dbdelete.sc" */	/* host code */
		return (errinfo.errorno);
	}
/* # line 66 "dbdelete.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dbdelete2",7206,11965);
      IIputdomio((short *)0,1,30,2,&database_no);
      IIputdomio((short *)0,1,30,2,&database_no);
      IIputdomio((short *)0,1,30,2,&database_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dbdelete2",7206,11965);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_paths where localdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&database_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"or sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&database_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"or targetdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&database_no);
        IIexDefine(0,(char *)"dbdelete2",7206,11965);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 69 "dbdelete.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
/* # line 71 "dbdelete.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 72 "dbdelete.sc" */	/* host code */
		return (errinfo.errorno);
	}
	if (db_config_changed(0) != OK)
	{
/* # line 77 "dbdelete.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 78 "dbdelete.sc" */	/* host code */
		return (FAIL);
	}
/* # line 81 "dbdelete.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 82 "dbdelete.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
/* # line 84 "dbdelete.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 85 "dbdelete.sc" */	/* host code */
		return (errinfo.errorno);
	}
	return (OK);
}
