# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
# include <st.h>
# include <cv.h>
# include <er.h>
# include <lo.h>
# include <nm.h>
# include <si.h>
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
** Name:	editcfg.qsc - edit RepServer configuration file
**
** Description:
**	Defines
**		edit_cfg_file	- edit configuration file
**
** History:
**	03-dec-96 (joea)
**		Created based on ed_file.osq in replicator library.
**	03-jun-98 (abbjo03)
**		Use RMget_server_dir to prepare server directory.
**	21-oct-98 (abbjo03)
**		Remove defaults that are already defaulted by the RepServer. Do
**		not include vnode in IDB since it should usually be a local
**		connection.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF
  i2 RMlocal_db_no;
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmed_file"), *tf_name = ERx("iitf"), *tf2_name = ERx("flags")
  , *fsvr_no = ERx("server"), *fline = ERx("line"), *fflag_name = ERx("flag_name")
  , *fshort_descr = ERx("short_description"), *flong_descr = ERx("long_descr")
  ;
/*{
** Name:	edit_cfg_file	- edit configuration file
**
** Description:
**	Allows the user to edit a RepServer configuration file (runrepl.opt).
**
** Inputs:
**	svr_no	- server number
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
edit_cfg_file(
i2	svr_no)
# if 0
  i2 svr_no;
# endif
  {
  char flag_name[7];
  char short_descr[21];
  char long_descr[501];
  char local_db_name[DB_MAXNAME+1];
  char dba_name[DB_MAXNAME+1];
  char dbms_type[9];
  i4 numrows;
  i4 num_value;
  char alpha_value[21];
    char fld_name[FE_MAXNAME+1];
    char buff[81];
    i4 chng;
    char newbuff[81];
	char	*p;
	char	path[MAX_LOC+1];
	FILE	*fp;
	LOCATION	loc;
	LOINFORMATION	locinfo;
	i4	locflags;
	bool	found;
	i4	i;
	i4	row;
	char	*p1, *pend;
	char	list_row[36];
	char	list_rows[2048];
	if (RMget_server_dir(svr_no, &loc, path) != OK)
	{
		IIUGerr(E_RM005D_Rservers_not_defined, UG_ERR_ERROR, 0);
		return;
	}
	LOfstfile(ERx("runrepl.opt"), &loc);
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
/* # line 131 "editcfg.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"f") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 132 "editcfg.qsc" */	/* host code */
	locflags = 0;
	if (LOinfo(&loc, &locflags, &locinfo) == OK)
	{
		if (SIfopen(&loc, ERx("r"), SI_TXT, SI_MAX_TXT_REC, &fp) != OK)
		{
			LOtos(&loc, &p);
			IIUGerr(E_RM005E_Error_open_file, UG_ERR_ERROR, 1, p);
			return;
		}
		while (SIgetrec(buff, (i4)sizeof(buff), fp) != ENDFILE)
/* # line 143 "editcfg.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fline,(short *)0,1,32,0,buff);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 144 "editcfg.qsc" */	/* host code */
		SIclose(fp);
	}
/* # line 147 "editcfg.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf2_name,(char *)"r") != 0) {
        IIthidecol(flong_descr,(char *)"varchar(500) not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 148 "editcfg.qsc" */	/* host code */
/* # line 154 "editcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select flag_name, short_description, flag_description from dd_server_\
flags order by flag_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,6,flag_name);
      IIgetdomio((short *)0,1,32,20,short_descr);
      IIgetdomio((short *)0,1,32,500,long_descr);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 159 "editcfg.sc" */	/* host code */
/* # line 153 "editcfg.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf2_name,1) != 0) {
        IItcoputio(fflag_name,(short *)0,1,32,0,flag_name);
        IItcoputio(fshort_descr,(short *)0,1,32,0,short_descr);
        IItcoputio(flong_descr,(short *)0,1,32,0,long_descr);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 155 "editcfg.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (RPdb_error_check(0, NULL) != OK)
	{
/* # line 172 "editcfg.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 173 "editcfg.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
/* # line 177 "editcfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 178 "editcfg.sc" */	/* host code */
/* # line 165 "editcfg.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio(fsvr_no,(short *)0,1,30,2,&svr_no);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 170 "editcfg.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 171 "editcfg.qsc" */	/* host code */
		if (!chng)
		{
			IIUGerr(E_RM0029_No_changes_to_save, 0, 0);
/* # line 174 "editcfg.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 175 "editcfg.qsc" */	/* host code */
		}
		if (SIfopen(&loc, ERx("w"), SI_TXT, SI_MAX_TXT_REC, &fp) != OK)
		{
			LOtos(&loc, &p);
			STcopy(p, path);
			IIUGerr(E_RM005F_Error_open_file_write, UG_ERR_ERROR,
				1, path);
/* # line 183 "editcfg.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 184 "editcfg.qsc" */	/* host code */
		}
/* # line 186 "editcfg.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,80,buff,fline);
                IITBceColEnd();
                {
/* # line 188 "editcfg.qsc" */	/* host code */
			STtrmwhite(buff);
			if (*buff != EOS)
				SIfprintf(fp, ERx("%s\n"), buff);
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 192 "editcfg.qsc" */	/* host code */
		SIclose(fp);
/* # line 194 "editcfg.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 200 "editcfg.qsc" */	/* inittable */
            {
              if (IItbinit(form_name,tf_name,(char *)"f") != 0) {
                IItfill();
              } /* IItbinit */
            }
/* # line 201 "editcfg.qsc" */	/* host code */
		STprintf(buff, ERx("-SVR%-d"), svr_no);
/* # line 202 "editcfg.qsc" */	/* loadtable */
            {
              if (IItbact(form_name,tf_name,1) != 0) {
                IItcoputio(fline,(short *)0,1,32,0,buff);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 204 "editcfg.qsc" */	/* host code */
/* # line 258 "editcfg.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"editcfg1",7218,30662);
      IIputdomio((short *)0,1,30,2,&RMlocal_db_no);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"editcfg1",7218,30662);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name, dbms_type, database_owner from dd_databases whe\
re database_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&RMlocal_db_no);
        IIexDefine(0,(char *)"editcfg1",7218,30662);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,local_db_name);
      IIgetdomio((short *)0,1,32,8,dbms_type);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dba_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 263 "editcfg.sc" */	/* host code */
		if (RPdb_error_check(DBEC_SINGLE_ROW, NULL) != OK)
		{
			STcopy(ERget(F_RM0088_unknown), local_db_name);
			STcopy(ERget(F_RM0088_unknown), dba_name);
			IIUGerr(E_RM0060_Error_get_local_db, UG_ERR_ERROR, 0);
		}
/* # line 269 "editcfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 270 "editcfg.sc" */	/* host code */
		STtrmwhite(local_db_name);
		STprintf(buff, ERx("-IDB%s"), local_db_name);
		STtrmwhite(dbms_type);
		if (!STequal(dbms_type, ERx("ingres")))
		{
			STcat(buff, ERx("/"));
			STcat(buff, dbms_type);
		}
/* # line 225 "editcfg.qsc" */	/* loadtable */
            {
              if (IItbact(form_name,tf_name,1) != 0) {
                IItcoputio(fline,(short *)0,1,32,0,buff);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 227 "editcfg.qsc" */	/* host code */
		STprintf(buff, ERx("-OWN%s"), dba_name);
/* # line 228 "editcfg.qsc" */	/* loadtable */
            {
              if (IItbact(form_name,tf_name,1) != 0) {
                IItcoputio(fline,(short *)0,1,32,0,buff);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 230 "editcfg.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 236 "editcfg.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld_name,19,(char *)0,
                0);
              } /* IIiqset */
            }
/* # line 237 "editcfg.qsc" */	/* host code */
		if (!STequal(fld_name, tf2_name))
		{
			IIUGerr(E_RM0061_Not_on_svr_flags_tf, UG_ERR_ERROR, 0);
/* # line 240 "editcfg.qsc" */	/* resume */
            {
              IIresfld(tf2_name);
              if (1) goto IIfdB1;
            }
/* # line 241 "editcfg.qsc" */	/* host code */
		}
/* # line 243 "editcfg.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf2_name,-2) != 0) {
                IItcogetio((short *)0,1,32,6,flag_name,fflag_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 244 "editcfg.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,80,buff,fline);
                IITBceColEnd();
                {
/* # line 246 "editcfg.qsc" */	/* host code */
			if (*buff != EOS &&
				STbcompare(buff, 4, flag_name, 4, FALSE) == 0)
			{
				IIUGerr(E_RM0062_Flag_already_used,
					UG_ERR_ERROR, 1, flag_name);
/* # line 252 "editcfg.qsc" */	/* resume */
                  {
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 253 "editcfg.qsc" */	/* host code */
			}
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 256 "editcfg.qsc" */	/* loadtable */
            {
              if (IItbact(form_name,tf_name,1) != 0) {
                IItcoputio(fline,(short *)0,1,32,0,flag_name);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 257 "editcfg.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 258 "editcfg.qsc" */	/* resume */
            {
              IIresfld(tf_name);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 264 "editcfg.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,80,buff,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 265 "editcfg.qsc" */	/* host code */
		if (!STequal(buff, tf2_name))
		{
			IIUGerr(E_RM0061_Not_on_svr_flags_tf, UG_ERR_ERROR, 0);
/* # line 268 "editcfg.qsc" */	/* resume */
            {
              IIresfld(tf2_name);
              if (1) goto IIfdB1;
            }
/* # line 269 "editcfg.qsc" */	/* host code */
		}
/* # line 271 "editcfg.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf2_name,-2) != 0) {
                IItcogetio((short *)0,1,32,500,long_descr,flong_descr);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 272 "editcfg.qsc" */	/* message */
            {
              IIFRgpcontrol(1,0);
              IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
              IIFRgpcontrol(2,0);
              IImessage(long_descr);
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 279 "editcfg.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,80,buff,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 280 "editcfg.qsc" */	/* host code */
		if (!STequal(buff, tf2_name))
		{
			IIUFfieldHelp();
/* # line 283 "editcfg.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 284 "editcfg.qsc" */	/* host code */
		}
/* # line 286 "editcfg.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf2_name,-2) != 0) {
                IItcogetio((short *)0,1,32,6,flag_name,fflag_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 287 "editcfg.qsc" */	/* host code */
/* # line 435 "editcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from dd_option_values where flag_name=");
    IIputdomio((short *)0,1,32,0,flag_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&numrows);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 439 "editcfg.sc" */	/* host code */
		if (numrows == 0)
		{
			IIUGerr(S_RM0063_Flag_has_no_int_val, 0, 0);
/* # line 294 "editcfg.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 295 "editcfg.qsc" */	/* host code */
		}
		else if (numrows == 1)
		{
			IIUGerr(S_RM0064_Flag_uses_pos_int, 0, 0);
/* # line 299 "editcfg.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 300 "editcfg.qsc" */	/* host code */
		}
		else
		{
			*list_rows = EOS;
/* # line 460 "editcfg.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select numeric_value, alpha_value from dd_option_values where flag_na\
me=");
    IIputdomio((short *)0,1,32,0,flag_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by numeric_value");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num_value);
      IIgetdomio((short *)0,1,32,20,alpha_value);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 466 "editcfg.sc" */	/* host code */
				STtrmwhite(alpha_value);
				STprintf(list_row, ERx("%d;%s\n"), num_value,
					alpha_value);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 471 "editcfg.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 472 "editcfg.sc" */	/* host code */
			row = IIFDlpListPick(NULL, list_rows, numrows, -1, -1,
				NULL, NULL, NULL, NULL);
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
				pend = STindex(p1, ERx(";"), 0);
				*pend = EOS;
				CVan(p1, &num_value);
				found = FALSE;
				STprintf(newbuff, ERx("%s%d"), flag_name,
					num_value);
/* # line 335 "editcfg.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE3;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,80,buff,fline);
                IITBceColEnd();
                {
/* # line 337 "editcfg.qsc" */	/* host code */
					if (STbcompare(buff, 4, flag_name, 4,
						0) == 0)
					{
/* # line 340 "editcfg.qsc" */	/* putrow */
                  {
                    if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                      IItcoputio(fline,(short *)0,1,32,0,newbuff);
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
/* # line 342 "editcfg.qsc" */	/* host code */
						found = TRUE;
/* # line 343 "editcfg.qsc" */	/* endloop */
                  {
                    if (1) goto IItbE3;
                  }
/* # line 344 "editcfg.qsc" */	/* host code */
					}
                }
              } /* IItunload */
IItbE3:
              IItunend();
            }
/* # line 347 "editcfg.qsc" */	/* host code */
				if (!found)
				{
/* # line 349 "editcfg.qsc" */	/* loadtable */
            {
              if (IItbact(form_name,tf_name,1) != 0) {
                IItcoputio(fline,(short *)0,1,32,0,newbuff);
                IITBceColEnd();
              } /* IItbact */
            }
/* # line 351 "editcfg.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 352 "editcfg.qsc" */	/* resume */
            {
              IIresfld(tf_name);
              if (1) goto IIfdB1;
            }
/* # line 353 "editcfg.qsc" */	/* host code */
				}
			}
		}
          }
        } else if (IIretval() == 6) {
          {
/* # line 362 "editcfg.qsc" */	/* host code */
		FEhelp(ERx("rmedfile.hlp"), ERget(F_RM008D_Edit_config_file));
          }
        } else if (IIretval() == 7) {
          {
/* # line 369 "editcfg.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 370 "editcfg.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 373 "editcfg.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 375 "editcfg.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,1) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0086_Default),ERget(F_RM0087_Default_expl),2,2,2)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0089_SelectFlag),ERget(F_RM008A_SelectFlag_expl),
      2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM008B_DescribeFlag),
      ERget(F_RM008C_DescribeFlag_expl),2,2,4) == 0) goto IIfdE1;
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
  }
