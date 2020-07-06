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
** Name:	dbedit.qsc - edit a replicated database
**
** Description:
**	Defines
**		db_edit		- edit a database
**		valid_dbms_type	- validate a DBMS type
**		dbms_type_lookup - list choices for DBMS type
**
** History:
**	16-dec-96 (joea)
**		Created based on dbedit.osq in replicator library.
**	14-nov-97 (joea)
**		Correct call to IIUGerr.
**	17-mar-98 (joea)
**		Support gateways that allow '@' as first character.
**	04-may-98 (joea) bug 90732
**		Allow initial numeric character in vnode name.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF
  char RMdb_display[];
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmdb_detail"), *fdb_no = ERx("database_no"), *
  fvnode_name = ERx("vnode_name"), *fdb_name = ERx("database_name"), *
  fdb_owner = ERx("database_owner"), *fdbms_type = ERx("dbms_type"), *
  fdbms_descr = ERx("short_description"), *fremark = ERx("remark");
STATUS valid_dbms_type(char *dbms_type, char *short_descr);
void dbms_type_lookup(void);
/*{
** Name:	db_edit - edit a database
**
** Description:
**	Allows the user to edit a replicated database information.
**
** Inputs:
**	db_no		- database number
**	local_db	- is this the local database
**	vnode_name	- virtual node name
**	db_name		- database name
**	db_owner	- database owner
**	dbms_type	- DBMS type
**	remark		- remark
**
** Outputs:
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
db_edit(
i2	db_no,
i2	local_db,
char	*vnode_name,
char	*db_name,
char	*db_owner,
char	*dbms_type,
char	*remark)
# if 0
  i2 db_no;
  char *vnode_name;
  char *db_name;
  char *db_owner;
  char *dbms_type;
  char *remark;
# endif
  {
    i4 chng;
    char objname[FE_MAXNAME+1];
    char short_descr[21];
	STATUS	retval;
	STATUS	valid;
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
	if ((valid = valid_dbms_type(dbms_type, short_descr)) == -1)
	{
/* # line 127 "dbedit.qsc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage((char *)"Error validating DBMS type...");
    }
/* # line 128 "dbedit.qsc" */	/* host code */
		*short_descr = EOS;
	}
	else if (valid == 0)
	{
		IIUGerr(E_RM0018_Invalid_dbms_type, UG_ERR_ERROR, 0);
		*short_descr = EOS;
	}
/* # line 136 "dbedit.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
      IIputfldio(fdb_no,(short *)0,1,30,2,&db_no);
      IIputfldio(fvnode_name,(short *)0,1,32,0,vnode_name);
      IIputfldio(fdb_name,(short *)0,1,32,0,db_name);
      IIputfldio(fdb_owner,(short *)0,1,32,0,db_owner);
      IIputfldio(fdbms_type,(short *)0,1,32,0,dbms_type);
      IIputfldio(fdbms_descr,(short *)0,1,32,0,short_descr);
      IIputfldio(fremark,(short *)0,1,32,0,remark);
      {
/* # line 142 "dbedit.qsc" */	/* host code */
		if (local_db)
/* # line 143 "dbedit.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
            IIstfsio(53,fdb_name,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1
            ));
          } /* IIiqset */
        }
/* # line 144 "dbedit.qsc" */	/* host code */
		else
/* # line 145 "dbedit.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
            IIstfsio(53,fdb_name,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0
            ));
          } /* IIiqset */
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 149 "dbedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 150 "dbedit.qsc" */	/* host code */
		if (chng)
		{
/* # line 152 "dbedit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,vnode_name,fvnode_name);
              } /* IIfsetio */
            }
/* # line 153 "dbedit.qsc" */	/* host code */
			STtrmwhite(vnode_name);
			if (!RPvalid_name(vnode_name) && !CMdigit(vnode_name))
			{
				IIUGerr(E_RM0008_Invalid_vnode_name,
					UG_ERR_ERROR, 0);
/* # line 158 "dbedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 159 "dbedit.qsc" */	/* host code */
			}
		}
/* # line 161 "dbedit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 166 "dbedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 167 "dbedit.qsc" */	/* host code */
		if (chng)
		{
/* # line 169 "dbedit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,db_name,fdb_name);
              } /* IIfsetio */
            }
/* # line 170 "dbedit.qsc" */	/* host code */
			STtrmwhite(db_name);
			if (!RPvalid_name(db_name) &&
				CMcmpcase(db_name, ERx("@")) != 0)
			{
				IIUGerr(E_RM0017_Invalid_db_name, UG_ERR_ERROR,
					0);
/* # line 176 "dbedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 177 "dbedit.qsc" */	/* host code */
			}
		}
/* # line 179 "dbedit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 184 "dbedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 185 "dbedit.qsc" */	/* host code */
		if (chng)
		{
/* # line 187 "dbedit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,dbms_type,fdbms_type);
              } /* IIfsetio */
            }
/* # line 188 "dbedit.qsc" */	/* host code */
			STtrmwhite(dbms_type);
			valid = valid_dbms_type(dbms_type, short_descr);
			if (valid == -1)
			{
/* # line 192 "dbedit.qsc" */	/* message */
            {
              IIFRgpcontrol(1,0);
              IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
              IIFRgpcontrol(2,0);
              IImessage((char *)"Error validating DBMS type...");
            }
/* # line 194 "dbedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 195 "dbedit.qsc" */	/* host code */
			}
			else if (!valid)
			{
				IIUGerr(E_RM0018_Invalid_dbms_type, UG_ERR_ERROR, 0);
/* # line 199 "dbedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 200 "dbedit.qsc" */	/* host code */
			}
		}
/* # line 202 "dbedit.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(fdbms_descr,(short *)0,1,32,0,short_descr);
              } /* IIfsetio */
            }
/* # line 203 "dbedit.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 209 "dbedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 210 "dbedit.qsc" */	/* host code */
		if (!chng)
		{
			IIUGerr(E_RM0029_No_changes_to_save, 0, 0);
/* # line 213 "dbedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 214 "dbedit.qsc" */	/* host code */
		}
/* # line 216 "dbedit.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 217 "dbedit.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,db_name,fdb_name);
                IIgetfldio((short *)0,1,32,0,vnode_name,fvnode_name);
                IIgetfldio((short *)0,1,32,0,db_owner,fdb_owner);
                IIgetfldio((short *)0,1,32,0,dbms_type,fdbms_type);
                IIgetfldio((short *)0,1,32,0,remark,fremark);
              } /* IIfsetio */
            }
/* # line 221 "dbedit.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0009_Saving), FALSE, 0);
		retval = 0;
/* # line 333 "dbedit.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dbedit1",7219,32616);
      IIputdomio((short *)0,1,32,0,db_name);
      IIputdomio((short *)0,1,32,0,vnode_name);
      IIputdomio((short *)0,1,32,0,db_owner);
      IIputdomio((short *)0,1,32,0,dbms_type);
      IIputdomio((short *)0,1,32,0,remark);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dbedit1",7219,32616);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update dd_databases set database_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,db_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", vnode_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,vnode_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", database_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,db_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)", dbms_type=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,dbms_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", remark=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,remark);
        IIwritio(0,(short *)0,1,32,0,(char *)"where database_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIexDefine(0,(char *)"dbedit1",7219,32616);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 339 "dbedit.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
/* # line 341 "dbedit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 342 "dbedit.sc" */	/* host code */
			retval = -1;
		}
		else if (db_config_changed(0) != OK)
		{
/* # line 346 "dbedit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 347 "dbedit.sc" */	/* host code */
			retval = -1;
		}
		if (retval)
		{
			FTbell();
			IIUGerr(E_RM0009_Error_saving, UG_ERR_ERROR, 0);
/* # line 244 "dbedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 245 "dbedit.qsc" */	/* host code */
		}
/* # line 359 "dbedit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 360 "dbedit.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
		{
/* # line 362 "dbedit.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 363 "dbedit.sc" */	/* host code */
			IIUGerr(E_RM000A_Error_committing, UG_ERR_ERROR, 0);
/* # line 251 "dbedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 252 "dbedit.qsc" */	/* host code */
		}
/* # line 254 "dbedit.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIstfsio(53,fdb_name,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 255 "dbedit.qsc" */	/* host code */
		retval = 1;
/* # line 256 "dbedit.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 263 "dbedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 264 "dbedit.qsc" */	/* host code */
		if (STequal(objname, fdbms_type))
			dbms_type_lookup();
		else
			IIUFfieldHelp();
          }
        } else if (IIretval() == 6) {
          {
/* # line 274 "dbedit.qsc" */	/* host code */
		FEhelp(ERx("rmdbedit.hlp"), ERget(F_RM0027_Edit_replic_db));
          }
        } else if (IIretval() == 7) {
          {
/* # line 281 "dbedit.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 282 "dbedit.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 285 "dbedit.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 287 "dbedit.qsc" */	/* set_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIstfsio(53,fdb_name,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 288 "dbedit.qsc" */	/* host code */
		retval = 0;
/* # line 289 "dbedit.qsc" */	/* breakdisplay */
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
      if (IIFRafActFld(fvnode_name,0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(fdb_name,0,2) == 0) goto IIfdE1;
      if (IIFRafActFld(fdbms_type,0,3) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,4) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,0,5) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,6) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,7) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 292 "dbedit.qsc" */	/* host code */
	return (retval);
  }
STATUS
valid_dbms_type(
char	*dbms_type,
char	*short_descr)
# if 0
  char *dbms_type;
  char *short_descr;
# endif
{
	DBEC_ERR_INFO	errinfo;
/* # line 481 "dbedit.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dbedit2",7220,25964);
      IIputdomio((short *)0,1,32,0,dbms_type);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"dbedit2",7220,25964);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select short_description from dd_dbms_types where dbms_type=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,dbms_type);
        IIexDefine(0,(char *)"dbedit2",7220,25964);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,short_descr);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 485 "dbedit.sc" */	/* host code */
	RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo);
/* # line 486 "dbedit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 487 "dbedit.sc" */	/* host code */
	if (errinfo.errorno)
		return (-1);
	else if (errinfo.rowcount == 0)
		return (0);
	return (1);
}
void
dbms_type_lookup()
  {
  char dbms_type[9];
  char short_descr[21];
	i4	i;
	i4	row;
	char	*p1, *p2, *pend;
	char	list_row[36];
	char	list_rows[2048];
	*list_rows = EOS;
/* # line 506 "dbedit.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbms_type, short_description from dd_dbms_types order by dbms_\
type");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,dbms_type);
      IIgetdomio((short *)0,1,32,0,short_descr);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 511 "dbedit.sc" */	/* host code */
		STtrmwhite(dbms_type);
		STprintf(list_row, ERx("%s;%s\n"), dbms_type, short_descr);
		STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 515 "dbedit.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 516 "dbedit.sc" */	/* host code */
	row = IIFDlpListPick(NULL, list_rows, 5, -1, -1, NULL, NULL, NULL,
		NULL);
	if (row >= 0)
	{
		p1 = list_rows;
		pend = STindex(p1, ERx("\n"), 0);
		for (i = 0; i < row; ++i)
		{
			p1 = CMnext(pend);
			pend = STindex(p1, ERx("\n"), 0);
		}
		*pend = EOS;
		p2 = pend = STindex(p1, ERx(";"), 0);
		CMnext(pend);
		STcopy(pend, short_descr);
		*p2 = EOS;
		STcopy(p1, dbms_type);
/* # line 364 "dbedit.qsc" */	/* putform */
    {
      if (IIfsetio(form_name) != 0) {
        IIputfldio(fdbms_type,(short *)0,1,32,0,dbms_type);
        IIputfldio(fdbms_descr,(short *)0,1,32,0,short_descr);
      } /* IIfsetio */
    }
/* # line 366 "dbedit.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 367 "dbedit.qsc" */	/* host code */
	}
  }
