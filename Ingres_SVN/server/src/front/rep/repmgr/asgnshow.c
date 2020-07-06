# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include <rpu.h>
# include <targtype.h>
# include "errm.h"
/**
** Name:	asgnshow.qsc - show server assignments
**
** Description:
**	Defines
**		svr_show_assignments - show server assignments
**
** History:
**	16-dec-96 (joea)
**		Created based on assgnsho.osq in replicator library.
**	14-jan-97 (joea)
**		Use RMtarg_get_descr instead of dd_target_types table.
**	29-may-97 (joea)
**		Ensure assignments are ordered by database, then CDDS.
**/
GLOBALREF
  i2 RMlocal_db_no;
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmshow_assignment"), *tf_name = ERx("iitf"), *
  fserver_no = ERx("server_no"), *fcdds_no = ERx("cdds_no"), *fcdds_name = ERx("cdds_name")
  , *fdb_no = ERx("database_no"), *ffull_db_name = ERx("dbname"), *
  ftarget_type = ERx("target_type"), *ftarget_descr = ERx("short_description")
  ;
/*{
** Name:	svr_show_assignments - show server assignments
**
** Description:
**	Show the DB/cdds's assigned to a particular server.
**
** Inputs:
**	server_no	- server number
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
svr_show_assignments(
i2	server_no)
# if 0
  i2 server_no;
# endif
  {
  i2 db_no;
  char full_db_name[DB_MAXNAME*2+3];
  i2 cdds_no;
  char cdds_name[DB_MAXNAME+1];
  i2 target_type;
    char *target_descr;
	DBEC_ERR_INFO	errinfo;
	if (!form_initd)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name) != OK)
		{
			IIUGerr(E_RM0006_Get_form_failed, UG_ERR_ERROR, 1,
				form_name);
			return;
		}
		form_initd = TRUE;
	}
/* # line 96 "asgnshow.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 97 "asgnshow.qsc" */	/* host code */
/* # line 92 "asgnshow.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select d.database_no, TRIM(db.vnode_name) +'::' +TRIM(db.database_nam\
e), c.cdds_no, c.cdds_name, d.target_type from dd_db_cdds d, dd_cdds c\
, dd_databases db where d.server_no=");
    IIputdomio((short *)0,1,30,2,&server_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and d.database_no=db.database_no and d.cdds_no=c.cdds_no and d.databa\
se_no!=");
    IIputdomio((short *)0,1,30,2,&RMlocal_db_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by 1, 3");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,full_db_name);
      IIgetdomio((short *)0,1,30,2,&cdds_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,cdds_name);
      IIgetdomio((short *)0,1,30,2,&target_type);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 104 "asgnshow.sc" */	/* host code */
		target_descr = RMtarg_get_descr(target_type);
/* # line 110 "asgnshow.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
        IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
        IItcoputio(fcdds_no,(short *)0,1,30,2,&cdds_no);
        IItcoputio(fcdds_name,(short *)0,1,32,0,cdds_name);
        IItcoputio(ftarget_descr,(short *)0,1,32,0,target_descr);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 113 "asgnshow.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 121 "asgnshow.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 122 "asgnshow.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 128 "asgnshow.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 129 "asgnshow.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 132 "asgnshow.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 133 "asgnshow.sc" */	/* host code */
/* # line 130 "asgnshow.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio(fserver_no,(short *)0,1,30,2,&server_no);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 135 "asgnshow.qsc" */	/* host code */
		FEhelp(ERx("rmasgsho.hlp"), ERget(F_RM008E_Show_svr_assign));
          }
        } else if (IIretval() == 2) {
          {
/* # line 141 "asgnshow.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else {
          if (1) goto IIfdE1;
        } /* IIretval */
      } /* IIrunform */
IIfdF1:
      if (IIchkfrm() == 0) goto IIfdB1;
      goto IIfdE1;
IImuI1:
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,1) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,2) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
