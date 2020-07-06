# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
# include <st.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include <stdprmpt.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	dbaddloc.qsc - define local database
**
** Description:
**	Defines
**		db_add_local	- define the local database
**
** History:
**	16-dec-96 (joea)
**		Created based on crtinstl.osq in replicator library.
**	14-nov-97 (joea)
**		Correct call to IIUGerr.
**	10-jul-98 (padni01) #90732
**		Allow initial numeric character in vnode name.
**	15-jul-98 (padni01) #90732
**		Include cm.h
**/
static READONLY
  char *form_name = ERx("rmlocal_db_detail");
static bool	form_initd;
static READONLY
  char *fdb_no = ERx("database_no"), *fvnode_name = ERx("vnode_name"), *
  fdb_name = ERx("database_name"), *fdb_owner = ERx("database_owner"), *
  fdbms_type = ERx("dbms_type"), *fremark = ERx("remark");
/*{
** Name:	db_add_local - define the local database
**
** Description:
**	Creates the local database.
**
** Inputs:
**	db_name		- database name
**	db_owner	- database owner
**
** Outputs:
**	db_no		- database number
**
** Returns:
**	OK	- changes were made and saved
**	FAIL	- no changes were made
*/
STATUS
db_add_local(
char	*db_name,
char	*db_owner,
i2	*database_no)
# if 0
  char *db_name;
  char *db_owner;
# endif
  {
  i2 db_no = 0;
  char vnode_name[DB_MAXNAME+1];
  char dbms_type[9];
  char remark[81];
    i4 chng;
	bool	must_save = TRUE;
	STATUS	retval = FAIL;
	DBEC_ERR_INFO	errinfo;
/* # line 81 "dbaddloc.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select LOWERCASE(cap_value)from iidbcapabilities where cap_capability\
='DBMS_TYPE'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,8,dbms_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 85 "dbaddloc.sc" */	/* host code */
	if (!form_initd)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name) != OK)
		{
			IIUGerr(E_RM0006_Get_form_failed, UG_ERR_ERROR, 1,
				form_name);
			return (FAIL);
		}
		form_initd = TRUE;
	}
/* # line 104 "dbaddloc.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio(fdb_name,(short *)0,1,32,0,db_name);
      IIputfldio(fdb_owner,(short *)0,1,32,0,db_owner);
      IIputfldio(fdbms_type,(short *)0,1,32,0,dbms_type);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 109 "dbaddloc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 110 "dbaddloc.qsc" */	/* host code */
		if (chng)
		{
/* # line 112 "dbaddloc.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&db_no,fdb_no);
              } /* IIfsetio */
            }
/* # line 113 "dbaddloc.qsc" */	/* host code */
			if (db_no < 1 || db_no > 32767)
			{
				IIUGerr(E_RM0007_Invalid_db_no, UG_ERR_ERROR,
					0);
/* # line 117 "dbaddloc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 118 "dbaddloc.qsc" */	/* host code */
			}
		}
/* # line 120 "dbaddloc.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 125 "dbaddloc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 126 "dbaddloc.qsc" */	/* host code */
		if (chng)
		{
/* # line 128 "dbaddloc.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name,
                fvnode_name);
              } /* IIfsetio */
            }
/* # line 129 "dbaddloc.qsc" */	/* host code */
			STtrmwhite(vnode_name);
			if (!RPvalid_name(vnode_name) && !CMdigit(vnode_name))
			{
				IIUGerr(E_RM0008_Invalid_vnode_name,
					UG_ERR_ERROR, 0);
/* # line 134 "dbaddloc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 135 "dbaddloc.qsc" */	/* host code */
			}
		}
/* # line 137 "dbaddloc.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 143 "dbaddloc.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 145 "dbaddloc.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&db_no,fdb_no);
                IIgetfldio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name,
                fvnode_name);
                IIgetfldio((short *)0,1,32,80,remark,fremark);
              } /* IIfsetio */
            }
/* # line 147 "dbaddloc.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0009_Saving), FALSE, 0);
/* # line 197 "dbaddloc.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dbaddloc1",7220,28909);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,32,0,vnode_name);
      IIputdomio((short *)0,1,32,0,db_name);
      IIputdomio((short *)0,1,32,0,db_owner);
      IIputdomio((short *)0,1,32,0,dbms_type);
      IIputdomio((short *)0,1,32,0,remark);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dbaddloc1",7220,28909);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_databases(database_no, vnode_name, database_name, data\
base_owner, local_db, dbms_type, config_changed, remark)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,vnode_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,db_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,db_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)", 1, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,dbms_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", DATE('now'), ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,remark);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"dbaddloc1",7220,28909);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 204 "dbaddloc.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
			FTbell();
/* # line 207 "dbaddloc.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 208 "dbaddloc.sc" */	/* host code */
			IIUGerr(E_RM0009_Error_saving, UG_ERR_ERROR, 0);
/* # line 160 "dbaddloc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 161 "dbaddloc.qsc" */	/* host code */
		}
/* # line 215 "dbaddloc.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 216 "dbaddloc.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
		{
/* # line 218 "dbaddloc.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 219 "dbaddloc.sc" */	/* host code */
			IIUGerr(ERget(E_RM000A_Error_committing), UG_ERR_ERROR,
				0);
/* # line 169 "dbaddloc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 170 "dbaddloc.qsc" */	/* host code */
		}
		*database_no = db_no;
		retval = OK;
/* # line 174 "dbaddloc.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 181 "dbaddloc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 182 "dbaddloc.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000B_Clear_form, S_RM000C_Yes_clear,
			S_RM000D_No_clear, ERx(""), TRUE) != CONFCH_YES)
/* # line 185 "dbaddloc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 187 "dbaddloc.qsc" */	/* clear */
            {
              IIfldclear(fdb_no);
              IIfldclear(fvnode_name);
              IIfldclear(fremark);
            }
/* # line 188 "dbaddloc.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 189 "dbaddloc.qsc" */	/* resume */
            {
              IIresfld(fdb_no);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 196 "dbaddloc.qsc" */	/* host code */
		FEhelp(ERx("rmcrtins.hlp"), ERget(F_RM000C_Define_local_db));
          }
        } else if (IIretval() == 6) {
          {
/* # line 203 "dbaddloc.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 204 "dbaddloc.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 207 "dbaddloc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 209 "dbaddloc.qsc" */	/* host code */
		if (must_save)
		{
			IIUGerr(S_RM0011_Save_local_info, 0, 0);
			must_save = FALSE;
/* # line 213 "dbaddloc.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 214 "dbaddloc.qsc" */	/* host code */
		}
/* # line 215 "dbaddloc.qsc" */	/* breakdisplay */
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
      if (IIFRafActFld(fdb_no,0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(fvnode_name,0,2) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,3) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM000A_Clear),ERget(F_RM000B_Clear_expl),0,0,4) == 
      0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(16,(char *)0,0,0,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,5) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,6) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 218 "dbaddloc.qsc" */	/* host code */
	return (retval);
  }
