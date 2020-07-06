# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	movecfg.qsc - move replication configuration
**
** Description:
**	Defines
**		move_config	- move replication configuration
**
** History:
**	16-dec-96 (joea)
**		Created based on movecfg.osq in replicator library.
**/
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmmovecfg"), *tf_name = ERx("iitf"), *fdb_no = ERx("database_no")
  , *fvnode_name = ERx("vnode_name"), *fdb_name = ERx("database_name"), *
  ffull_db_name = ERx("vnode_dbname"), *fdbms_type = ERx("dbms_type"), *
  fremark = ERx("remark");
/*{
** Name:	move_config - move replication configuration
**
** Description:
**	Move configuration data to the selected target databases.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
move_config()
  {
  i2 db_no;
  char vnode_name[DB_MAXNAME+1];
  char db_name[DB_MAXNAME+1];
  char dbms_type[9];
  char remark[81];
    char full_db_name[DB_MAXNAME*2+3];
	STATUS	retval;
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
/* # line 84 "movecfg.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IIthidecol(fvnode_name,(char *)"char(32) not null");
        IIthidecol(fdb_name,(char *)"char(32) not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 86 "movecfg.qsc" */	/* host code */
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
/* # line 83 "movecfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct d.database_no, database_name, vnode_name, dbms_type, \
remark from dd_databases d, dd_db_cdds c where d.local_db=0 and d.data\
base_no=c.database_no and c.target_type in(1, 2)and d.vnode_name!='mob\
ile'");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,db_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name);
      IIgetdomio((short *)0,1,32,8,dbms_type);
      IIgetdomio((short *)0,1,32,80,remark);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 93 "movecfg.sc" */	/* host code */
		STtrmwhite(db_name);
		STtrmwhite(vnode_name);
		STprintf(full_db_name, ERx("%s::%s"), vnode_name, db_name);
/* # line 100 "movecfg.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fdb_no,(short *)0,1,30,2,&db_no);
        IItcoputio(fvnode_name,(short *)0,1,32,0,vnode_name);
        IItcoputio(fdb_name,(short *)0,1,32,0,db_name);
        IItcoputio(ffull_db_name,(short *)0,1,32,0,full_db_name);
        IItcoputio(fdbms_type,(short *)0,1,32,0,dbms_type);
        IItcoputio(fremark,(short *)0,1,32,0,remark);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 104 "movecfg.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 113 "movecfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 114 "movecfg.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 120 "movecfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 121 "movecfg.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 124 "movecfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 125 "movecfg.sc" */	/* host code */
/* # line 121 "movecfg.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 126 "movecfg.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&db_no,fdb_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name,
                fvnode_name);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,db_name,fdb_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 128 "movecfg.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 129 "movecfg.qsc" */	/* host code */
		retval = move_1_config(db_no, vnode_name, db_name);
		PCsleep(2000);
/* # line 131 "movecfg.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 132 "movecfg.qsc" */	/* redisplay */
            {
              IIredisp();
            }
/* # line 133 "movecfg.qsc" */	/* host code */
		if (retval)
			IIUGerr(E_RM0053_Error_moving_config, UG_ERR_ERROR,
				2, vnode_name, db_name);
          }
        } else if (IIretval() == 2) {
          {
/* # line 141 "movecfg.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 142 "movecfg.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,2,&db_no,fdb_no);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name,
                fvnode_name);
                IItcogetio((short *)0,1,32,DB_MAXNAME+1-1,db_name,fdb_name);
                IITBceColEnd();
                {
/* # line 145 "movecfg.qsc" */	/* host code */
			retval = move_1_config(db_no, vnode_name, db_name);
			if (retval)
				IIUGerr(E_RM0053_Error_moving_config,
					UG_ERR_ERROR, 2, vnode_name, db_name);
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 150 "movecfg.qsc" */	/* host code */
		PCsleep(2000);
/* # line 151 "movecfg.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 152 "movecfg.qsc" */	/* redisplay */
            {
              IIredisp();
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 157 "movecfg.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 4) {
          {
/* # line 162 "movecfg.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 167 "movecfg.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 173 "movecfg.qsc" */	/* host code */
		FEhelp(ERx("rmmovcfg.hlp"), ERget(F_RM005E_Move_config_data));
          }
        } else if (IIretval() == 7) {
          {
/* # line 179 "movecfg.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_RM005A_ThisDatabase),
      ERget(F_RM005B_ThisDatabase_expl),2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM005C_AllDatabases),
      ERget(F_RM005D_AllDatabases_expl),2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,6) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
