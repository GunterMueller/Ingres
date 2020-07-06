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
# include <uigdata.h>
# include <stdprmpt.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	dbadd.qsc - add a replicated database
**
** Description:
**	Defines
**		db_add		- add a database
**
** History:
**	16-dec-96 (joea)
**		Created based on dbadd.osq in replicator library.
**	14-nov-97 (joea)
**		Correct call to IIUGerr.
**	17-mar-98 (joea)
**		Support gateways that allow '@' as first character.
**	04-may-98 (joea) bug 90732
**		Allow initial numeric character in vnode name.
**	24-nov-98 (abbjo03)
**		Initialize the database owner field with the local DBA.
**	07-dec-98 (abbjo03)
**		Correct 24-nov-98 change: STcopy the local DBA info.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF
  char RMdb_display[];
static READONLY
  char *form_name = ERx("rmdb_detail");
static bool	form_initd;
static READONLY
  char *fdb_no = ERx("database_no"), *fvnode_name = ERx("vnode_name"), *
  fdb_name = ERx("database_name"), *fdb_owner = ERx("database_owner"), *
  fdbms_type = ERx("dbms_type"), *fdbms_descr = ERx("short_description"), *
  fremark = ERx("remark");
FUNC_EXTERN STATUS valid_dbms_type(char *dbms_type, char *dbms_descr);
FUNC_EXTERN void dbms_type_lookup(void);
/*{
** Name:	db_add - edit a database
**
** Description:
**	Allows the user to add a replicated database.
**
** Inputs:
**	none
**
** Outputs:
**	database_no	- database number
**	vnode_name	- virtual node name
**	db_name		- database name
**	db_owner	- database owner
**	dbms_type	- DBMS type
**	remark		- remark
**
** Returns:
**	0	- no changes were made
**	1	- changes were made and saved
*/
STATUS
db_add(
i2	*database_no,
char	*vnode_name,
char	*db_name,
char	*db_owner,
char	*dbms_type,
char	*remark)
# if 0
  char *vnode_name;
  char *db_name;
  char *db_owner;
  char *dbms_type;
  char *remark;
# endif
  {
  i4 db_no_exist;
  i2 db_no;
    i4 chng;
    char objname[FE_MAXNAME+1];
    char dbms_descr[21];
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
	STcopy(IIUIdbdata()->suser, db_owner);
/* # line 127 "dbadd.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
      IIputfldio(fdb_owner,(short *)0,1,32,0,db_owner);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 131 "dbadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 132 "dbadd.qsc" */	/* host code */
		if (chng)
		{
/* # line 134 "dbadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&db_no,fdb_no);
              } /* IIfsetio */
            }
/* # line 135 "dbadd.qsc" */	/* host code */
			if (db_no < 1 || db_no > 32767)
			{
				IIUGerr(E_RM0007_Invalid_db_no, UG_ERR_ERROR,
					0);
/* # line 139 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 140 "dbadd.qsc" */	/* host code */
			}
/* # line 154 "dbadd.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_databases where database_no=");
    IIputdomio((short *)0,1,30,2,&db_no);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&db_no_exist);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 158 "dbadd.sc" */	/* host code */
			if (db_no_exist)
			{
				i4	pdb_no = db_no;
				IIUGerr(E_RM0016_Db_no_in_use, UG_ERR_ERROR, 1,
					&pdb_no);
/* # line 152 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 153 "dbadd.qsc" */	/* host code */
			}
		}
/* # line 155 "dbadd.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 160 "dbadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 161 "dbadd.qsc" */	/* host code */
		if (chng)
		{
/* # line 163 "dbadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,vnode_name,fvnode_name);
              } /* IIfsetio */
            }
/* # line 164 "dbadd.qsc" */	/* host code */
			STtrmwhite(vnode_name);
			if (!RPvalid_name(vnode_name) && !CMdigit(vnode_name))
			{
				IIUGerr(E_RM0008_Invalid_vnode_name,
					UG_ERR_ERROR, 0);
/* # line 169 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 170 "dbadd.qsc" */	/* host code */
			}
		}
/* # line 172 "dbadd.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 177 "dbadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 178 "dbadd.qsc" */	/* host code */
		if (chng)
		{
/* # line 180 "dbadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,db_name,fdb_name);
              } /* IIfsetio */
            }
/* # line 181 "dbadd.qsc" */	/* host code */
			STtrmwhite(db_name);
			if (!RPvalid_name(db_name) &&
				CMcmpcase(db_name, ERx("@")) != 0)
			{
				IIUGerr(E_RM0017_Invalid_db_name, UG_ERR_ERROR,
					0);
/* # line 187 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 188 "dbadd.qsc" */	/* host code */
			}
		}
/* # line 190 "dbadd.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 195 "dbadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 196 "dbadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,20,dbms_descr,fdbms_descr);
              } /* IIfsetio */
            }
/* # line 197 "dbadd.qsc" */	/* host code */
		if (chng || *dbms_descr == EOS)
		{
			STATUS	valid;
/* # line 201 "dbadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,dbms_type,fdbms_type);
              } /* IIfsetio */
            }
/* # line 202 "dbadd.qsc" */	/* host code */
			STtrmwhite(dbms_type);
			valid = valid_dbms_type(dbms_type, dbms_descr);
			if (valid == -1)
			{
/* # line 206 "dbadd.qsc" */	/* message */
            {
              IIFRgpcontrol(1,0);
              IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
              IIFRgpcontrol(2,0);
              IImessage((char *)"Error validating DBMS type...");
            }
/* # line 208 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 209 "dbadd.qsc" */	/* host code */
			}
			else if (!valid)
			{
				IIUGerr(E_RM0018_Invalid_dbms_type,
					UG_ERR_ERROR, 0);
/* # line 214 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 215 "dbadd.qsc" */	/* host code */
			}
		}
/* # line 217 "dbadd.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fdbms_descr,(short *)0,1,32,0,dbms_descr);
              } /* IIfsetio */
            }
/* # line 218 "dbadd.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 224 "dbadd.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 225 "dbadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,db_name,fdb_name);
                IIgetfldio((short *)0,1,32,0,vnode_name,fvnode_name);
                IIgetfldio((short *)0,1,32,0,db_owner,fdb_owner);
                IIgetfldio((short *)0,1,32,0,dbms_type,fdbms_type);
                IIgetfldio((short *)0,1,32,0,remark,fremark);
              } /* IIfsetio */
            }
/* # line 229 "dbadd.qsc" */	/* host code */
		/* Make sure that vnode::dbname is unique */
/* # line 334 "dbadd.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no from dd_databases where database_name=");
    IIputdomio((short *)0,1,32,0,db_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and vnode_name=");
    IIputdomio((short *)0,1,32,0,vnode_name);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 341 "dbadd.sc" */	/* host code */
		if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
		{
			IIUGerr(E_RM0019_Error_check_vnode_db, UG_ERR_ERROR,
				0);
/* # line 241 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 242 "dbadd.qsc" */	/* host code */
		}
		if (errinfo.rowcount > 0)
		{
			char	full_db_name[DB_MAXNAME*2+3];
			i4	pdb_no = db_no;
			STprintf(full_db_name, ERx("%s::%s"), vnode_name,
				db_name);
			IIUGerr(E_RM001A_Db_already_defined, UG_ERR_ERROR, 2,
				full_db_name, &pdb_no);
/* # line 252 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 253 "dbadd.qsc" */	/* host code */
		}
/* # line 255 "dbadd.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,30,2,&db_no,fdb_no);
              } /* IIfsetio */
            }
/* # line 256 "dbadd.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0009_Saving), FALSE, 0);
		retval = 0;
/* # line 374 "dbadd.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dbadd1",7221,22134);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,32,0,vnode_name);
      IIputdomio((short *)0,1,32,0,db_name);
      IIputdomio((short *)0,1,32,0,db_owner);
      IIputdomio((short *)0,1,32,0,dbms_type);
      IIputdomio((short *)0,1,32,0,remark);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dbadd1",7221,22134);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_databases(database_no, vnode_name, database_name, data\
base_owner, local_db, dbms_type, remark)values(");
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
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,dbms_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,remark);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"dbadd1",7221,22134);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 379 "dbadd.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 381 "dbadd.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 382 "dbadd.sc" */	/* host code */
			retval = -1;
		}
		else if (db_config_changed(0) != OK)
		{
/* # line 386 "dbadd.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 387 "dbadd.sc" */	/* host code */
			retval = -1;
		}
		if (retval)
		{
			FTbell();
			IIUGerr(E_RM0009_Error_saving, UG_ERR_ERROR, 0);
/* # line 278 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 279 "dbadd.qsc" */	/* host code */
		}
/* # line 399 "dbadd.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 400 "dbadd.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
		{
/* # line 402 "dbadd.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 403 "dbadd.sc" */	/* host code */
			IIUGerr(E_RM000A_Error_committing, UG_ERR_ERROR, 0);
/* # line 285 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 286 "dbadd.qsc" */	/* host code */
		}
		*database_no = db_no;
		retval = 1;
/* # line 290 "dbadd.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 297 "dbadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 298 "dbadd.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000B_Clear_form, S_RM000C_Yes_clear,
			S_RM000D_No_clear, ERx(""), TRUE) != CONFCH_YES)
/* # line 301 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 303 "dbadd.qsc" */	/* clear */
            {
              IIclrflds();
            }
/* # line 304 "dbadd.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 305 "dbadd.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
              } /* IIfsetio */
            }
/* # line 306 "dbadd.qsc" */	/* resume */
            {
              IIresfld(fdb_no);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 313 "dbadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 314 "dbadd.qsc" */	/* host code */
		if (STequal(objname, fdbms_type))
			dbms_type_lookup();
		else
			IIUFfieldHelp();
          }
        } else if (IIretval() == 8) {
          {
/* # line 324 "dbadd.qsc" */	/* host code */
		FEhelp(ERx("rmdbadd.hlp"), ERget(F_RM0026_Add_replic_db));
          }
        } else if (IIretval() == 9) {
          {
/* # line 331 "dbadd.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 332 "dbadd.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 335 "dbadd.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 337 "dbadd.qsc" */	/* host code */
		retval = 0;
/* # line 338 "dbadd.qsc" */	/* breakdisplay */
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
      if (IIFRafActFld(fdb_name,0,3) == 0) goto IIfdE1;
      if (IIFRafActFld(fdbms_type,0,4) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,5) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM000A_Clear),ERget(F_RM000B_Clear_expl),0,0,6) == 
      0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(16,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,0,7) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,8) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,8) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,9) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,9) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 341 "dbadd.qsc" */	/* host code */
	return (retval);
  }
