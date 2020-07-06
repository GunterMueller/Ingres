# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <rpu.h>
/*
** Name:	cddsdel.sc
**
** Description:
**	Delete a CDDS from the dd_cdds table and deassign tables in the
**	deleted CDDS.
**
** History:
**	16-dec-96 (joea)
**		Created based on cddsdel.sc in replicator library.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FUNC_EXTERN STATUS db_config_changed(i2 db_no);
STATUS
cdds_delete(
i2	cdds_no,
i4	RegTableCount)
# if 0
  i2 cdds_no;
# endif
{
	DBEC_ERR_INFO	errinfo;
/* # line 41 "cddsdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddsdel1",7210,30715);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddsdel1",7210,30715);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_cdds where cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIexDefine(0,(char *)"cddsdel1",7210,30715);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 42 "cddsdel.sc" */	/* host code */
	if (RPdb_error_check(DBEC_SINGLE_ROW, &errinfo) != OK)
	{
/* # line 44 "cddsdel.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 45 "cddsdel.sc" */	/* host code */
		return (errinfo.errorno);
	}
/* # line 49 "cddsdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddsdel2",7211,32253);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddsdel2",7211,32253);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_paths where cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIexDefine(0,(char *)"cddsdel2",7211,32253);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 50 "cddsdel.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
/* # line 52 "cddsdel.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 53 "cddsdel.sc" */	/* host code */
		return (errinfo.errorno);
	}
/* # line 57 "cddsdel.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_db_cdds where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIsyncup((char *)0,0);
  }
/* # line 58 "cddsdel.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
/* # line 60 "cddsdel.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 61 "cddsdel.sc" */	/* host code */
		return (errinfo.errorno);
	}
	if (RegTableCount > 0)
	{
/* # line 66 "cddsdel.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_regist_tables set cdds_no=0 where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIsyncup((char *)0,0);
  }
/* # line 69 "cddsdel.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
/* # line 71 "cddsdel.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 72 "cddsdel.sc" */	/* host code */
			return (errinfo.errorno);
		}
	}
	if (db_config_changed(0) != OK)
	{
/* # line 78 "cddsdel.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 79 "cddsdel.sc" */	/* host code */
		return (FAIL);
	}
/* # line 82 "cddsdel.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 83 "cddsdel.sc" */	/* host code */
	if (RPdb_error_check(0, &errinfo) != OK)
	{
/* # line 85 "cddsdel.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 86 "cddsdel.sc" */	/* host code */
		return (errinfo.errorno);
	}
	return (OK);
}
