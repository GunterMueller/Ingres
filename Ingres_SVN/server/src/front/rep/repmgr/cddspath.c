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
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <erfe.h>
# include <adf.h>
# include <fmt.h>
# include <ft.h>
# include <frame.h>
# include <runtime.h>
# include <uf.h>
# include <ug.h>
# include <stdprmpt.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	cddspath.qsc - CDDS paths
**
** Description:
**	Defines
**		cdds_path	- CDDS paths
**
** History:
**	05-dec-96 (joea)
**		Created based on pathways.osq in replicator library.
**	14-nov-97 (joea)
**		Issue an error message if zero rows selected in ListChoices
**		menu item.
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
  char *form_name = ERx("rmcdds_paths"), *tf_name = ERx("iitf"), *fcdds_no = ERx("cdds_no")
  , *fcdds_name = ERx("cdds_name"), *fsource_db = ERx("sourcedb"), *flocal_db 
  = ERx("localdb"), *ftarget_db = ERx("targetdb"), *fold_source_db = ERx("old_source_db")
  , *fold_local_db = ERx("old_local_db"), *fold_target_db = ERx("old_target_db")
  , *fsource_name = ERx("s_name"), *flocal_name = ERx("l_name"), *ftarget_name 
  = ERx("t_name");
  static i2 h_target_type;
static STATUS get_database(i2 db_no, i2 cdds_no, char *db_name);
/*{
** Name:	cdds_path	- CDDS paths
**
** Description:
**	Insert/Update/Delete Existing Data Propagation Pathways
**
** Inputs:
**	cdds_no		- CDDS number
**	cdds_name	- CDDS name
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
cdds_path(
i2	cdds_no,
char	*cdds_name)
# if 0
  i2 cdds_no;
  char *cdds_name;
# endif
  {
  i2 db_no;
  char db_name[DB_MAXNAME*2+3];
  i2 source_db;
  i2 local_db;
  i2 target_db;
  i2 old_source_db;
  i2 old_local_db;
  i2 old_target_db;
  i4 n;
    i4 rowstate;
    i4 rownumber;
    i4 chng;
    i4 getmsg_stat;
    char objname[FE_MAXNAME+1];
	DBEC_ERR_INFO	errinfo;
	i4	i;
	i4	row;
	char	*p1, *p2, *pend;
	char	list_row[36];
	char	list_rows[2048];
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
/* # line 133 "cddspath.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"f") != 0) {
        IIthidecol(fold_source_db,(char *)"i2 not null");
        IIthidecol(fold_local_db,(char *)"i2 not null");
        IIthidecol(fold_target_db,(char *)"i2 not null");
        IItfill();
      } /* IItbinit */
    }
/* # line 135 "cddspath.qsc" */	/* host code */
	IIUGmsg(ERget(F_RM001C_Selecting_data), FALSE, 0);
/* # line 127 "cddspath.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select sourcedb, localdb, targetdb from dd_paths where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
"order by sourcedb, localdb, targetdb");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&source_db);
      IIgetdomio((short *)0,1,30,2,&local_db);
      IIgetdomio((short *)0,1,30,2,&target_db);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 133 "cddspath.sc" */	/* host code */
/* # line 142 "cddspath.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fsource_db,(short *)0,1,30,2,&source_db);
        IItcoputio(fold_source_db,(short *)0,1,30,2,&source_db);
        IItcoputio(flocal_db,(short *)0,1,30,2,&local_db);
        IItcoputio(fold_local_db,(short *)0,1,30,2,&local_db);
        IItcoputio(ftarget_db,(short *)0,1,30,2,&target_db);
        IItcoputio(fold_target_db,(short *)0,1,30,2,&target_db);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 146 "cddspath.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 150 "cddspath.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 151 "cddspath.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
/* # line 155 "cddspath.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 156 "cddspath.sc" */	/* host code */
	if (errinfo.rowcount == 0)
	{
/* # line 159 "cddspath.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
        IIstfsio(21,tf_name,0,(short *)0,1,32,0,(char *)"f");
      } /* IIiqset */
    }
/* # line 160 "cddspath.qsc" */	/* host code */
	}
	else
	{
/* # line 163 "cddspath.qsc" */	/* unloadtable */
    {
      if (IItbact(form_name,tf_name,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,30,2,&source_db,fsource_db);
        IItcogetio((short *)0,1,30,2,&local_db,flocal_db);
        IItcogetio((short *)0,1,30,2,&target_db,ftarget_db);
        IITBceColEnd();
        {
/* # line 166 "cddspath.qsc" */	/* host code */
			if (get_database(source_db, cdds_no, db_name) != OK)
				return;
/* # line 168 "cddspath.qsc" */	/* putrow */
          {
            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
              IItcoputio(fsource_name,(short *)0,1,32,0,db_name);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 170 "cddspath.qsc" */	/* host code */
			if (get_database(local_db, cdds_no, db_name) != OK)
				return;
/* # line 172 "cddspath.qsc" */	/* putrow */
          {
            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
              IItcoputio(flocal_name,(short *)0,1,32,0,db_name);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 174 "cddspath.qsc" */	/* host code */
			if (get_database(target_db, cdds_no, db_name) != OK)
				return;
/* # line 176 "cddspath.qsc" */	/* putrow */
          {
            if (IItbsetio(3,form_name,tf_name,-2) != 0) {
              IItcoputio(ftarget_name,(short *)0,1,32,0,db_name);
              IItcoputio((char *)"_STATE",(short *)0,1,30,sizeof(2),
              (void *)IILQint(2));
              IITBceColEnd();
            } /* IItbsetio */
          }
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 179 "cddspath.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
        IIstfsio(21,tf_name,0,(short *)0,1,32,0,(char *)"u");
      } /* IIiqset */
    }
/* # line 180 "cddspath.qsc" */	/* host code */
	}
/* # line 182 "cddspath.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&getmsg_stat,63,(char *)0,0);
      } /* IIiqset */
    }
/* # line 183 "cddspath.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(63,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 184 "cddspath.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"db",(short *)0,1,32,0,RMdb_display);
      IIputfldio(fcdds_no,(short *)0,1,30,2,&cdds_no);
      IIputfldio(fcdds_name,(short *)0,1,32,0,cdds_name);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 189 "cddspath.qsc" */	/* host code */
		target_db = 0;
/* # line 190 "cddspath.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&source_db,fsource_db);
                IItcogetio((short *)0,1,30,2,&target_db,ftarget_db);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 192 "cddspath.qsc" */	/* host code */
		if (source_db == target_db)
		{
			IIUGerr(E_RM002B_Orig_db_eq_target_db, UG_ERR_ERROR,
				0);
/* # line 196 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 197 "cddspath.qsc" */	/* host code */
		}
/* # line 198 "cddspath.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,form_name,tf_name,(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 199 "cddspath.qsc" */	/* host code */
		if (chng)
		{
			if (get_database(source_db, cdds_no, db_name) != OK)
/* # line 202 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 203 "cddspath.qsc" */	/* host code */
			if (STequal(db_name, ERget(F_RM0035_None)))
			{
				i4 pdb_no = source_db;
				FTbell();
				IIUGerr(E_RM002C_Not_valid_value, UG_ERR_ERROR,
					1, &pdb_no);
/* # line 210 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 211 "cddspath.qsc" */	/* host code */
			}
			if (h_target_type != 1)
			{
				FTbell();
				IIUGerr(E_RM002D_Orig_local_full_peer,
					UG_ERR_ERROR, 0);
/* # line 217 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 218 "cddspath.qsc" */	/* host code */
			}
/* # line 219 "cddspath.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(fsource_name,(short *)0,1,32,0,db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 220 "cddspath.qsc" */	/* host code */
		}
/* # line 221 "cddspath.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 226 "cddspath.qsc" */	/* host code */
		target_db = 0;
/* # line 227 "cddspath.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&local_db,flocal_db);
                IItcogetio((short *)0,1,30,2,&target_db,ftarget_db);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 229 "cddspath.qsc" */	/* host code */
		if (local_db == target_db)
		{
			IIUGerr(E_RM002E_Local_db_eq_target_db, UG_ERR_ERROR,
				0);
/* # line 233 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 234 "cddspath.qsc" */	/* host code */
		}
/* # line 235 "cddspath.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,form_name,tf_name,(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 236 "cddspath.qsc" */	/* host code */
		if (chng)
		{
			if (get_database(local_db, cdds_no, db_name) != OK)
/* # line 239 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 240 "cddspath.qsc" */	/* host code */
			if (STequal(db_name, ERget(F_RM0035_None)))
			{
				i4 pdb_no = local_db;
				FTbell();
				IIUGerr(E_RM002C_Not_valid_value, UG_ERR_ERROR,
					1, &pdb_no);
/* # line 247 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 248 "cddspath.qsc" */	/* host code */
			}
			if (h_target_type != 1)
			{
				FTbell();
				IIUGerr(E_RM002D_Orig_local_full_peer,
					UG_ERR_ERROR, 0);
/* # line 254 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 255 "cddspath.qsc" */	/* host code */
			}
/* # line 256 "cddspath.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(flocal_name,(short *)0,1,32,0,db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 257 "cddspath.qsc" */	/* host code */
		}
/* # line 258 "cddspath.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 263 "cddspath.qsc" */	/* host code */
		source_db = local_db = 0;
/* # line 264 "cddspath.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&source_db,fsource_db);
                IItcogetio((short *)0,1,30,2,&local_db,flocal_db);
                IItcogetio((short *)0,1,30,2,&target_db,ftarget_db);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 266 "cddspath.qsc" */	/* host code */
		if (source_db == target_db || local_db == target_db)
		{
			IIUGerr(E_RM002F_Target_db_eq_others, UG_ERR_ERROR,
				0);
/* # line 270 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 271 "cddspath.qsc" */	/* host code */
		}
/* # line 272 "cddspath.qsc" */	/* inquire_frs */
            {
              if (IIiqset(5,0,form_name,tf_name,(char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 273 "cddspath.qsc" */	/* host code */
		if (chng)
		{
			if (get_database(target_db, cdds_no, db_name) != OK)
/* # line 276 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 277 "cddspath.qsc" */	/* host code */
			if (STequal(db_name, ERget(F_RM0035_None)))
			{
				i4 pdb_no = target_db;
				FTbell();
				IIUGerr(E_RM002C_Not_valid_value, UG_ERR_ERROR,
					1, &pdb_no);
/* # line 284 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 285 "cddspath.qsc" */	/* host code */
			}
/* # line 286 "cddspath.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(ftarget_name,(short *)0,1,32,0,db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 287 "cddspath.qsc" */	/* host code */
		}
/* # line 288 "cddspath.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 294 "cddspath.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 295 "cddspath.qsc" */	/* host code */
		if (!chng)
		{
			IIUGerr(E_RM0029_No_changes_to_save, 0, 0);
/* # line 298 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 299 "cddspath.qsc" */	/* host code */
		}
/* # line 301 "cddspath.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 302 "cddspath.qsc" */	/* host code */
		IIUGmsg(ERget(F_RM0009_Saving), FALSE, 0);
/* # line 303 "cddspath.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_RECO\
RD");
                IItcogetio((short *)0,1,30,2,&old_source_db,fold_source_db);
                IItcogetio((short *)0,1,30,2,&old_local_db,fold_local_db);
                IItcogetio((short *)0,1,30,2,&old_target_db,fold_target_db);
                IITBceColEnd();
                {
/* # line 308 "cddspath.qsc" */	/* host code */
			if (rowstate == stDELETE)
			{
/* # line 505 "cddspath.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddspath1",7222,11515);
      IIputdomio((short *)0,1,30,2,&old_source_db);
      IIputdomio((short *)0,1,30,2,&old_local_db);
      IIputdomio((short *)0,1,30,2,&old_target_db);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddspath1",7222,11515);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from dd_paths where sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&old_source_db);
        IIwritio(0,(short *)0,1,32,0,(char *)"and localdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&old_local_db);
        IIwritio(0,(short *)0,1,32,0,(char *)"and targetdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&old_target_db);
        IIwritio(0,(short *)0,1,32,0,(char *)"and cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIexDefine(0,(char *)"cddspath1",7222,11515);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 509 "cddspath.sc" */	/* host code */
				if (RPdb_error_check(DBEC_SINGLE_ROW, NULL)
					!= OK)
				{
/* # line 512 "cddspath.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 513 "cddspath.sc" */	/* host code */
					IIUGerr(E_RM002A_Error_saving_tf,
						UG_ERR_ERROR, 0);
/* # line 321 "cddspath.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 322 "cddspath.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 323 "cddspath.qsc" */	/* host code */
				}
			}
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 327 "cddspath.qsc" */	/* host code */
		/* process all but Deleted rows */
/* # line 328 "cddspath.qsc" */	/* unloadtable */
            {
              if (IItbact(form_name,tf_name,0) == 0) goto IItbE3;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_STATE");
                IItcogetio((short *)0,1,30,4,&rownumber,(char *)"_RECO\
RD");
                IItcogetio((short *)0,1,30,2,&source_db,fsource_db);
                IItcogetio((short *)0,1,30,2,&old_source_db,fold_source_db);
                IItcogetio((short *)0,1,30,2,&local_db,flocal_db);
                IItcogetio((short *)0,1,30,2,&old_local_db,fold_local_db);
                IItcogetio((short *)0,1,30,2,&target_db,ftarget_db);
                IItcogetio((short *)0,1,30,2,&old_target_db,fold_target_db);
                IITBceColEnd();
                {
/* # line 334 "cddspath.qsc" */	/* host code */
			if (rowstate == stNEW)
/* # line 556 "cddspath.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddspath2",7223,4539);
      IIputdomio((short *)0,1,30,2,&local_db);
      IIputdomio((short *)0,1,30,2,&source_db);
      IIputdomio((short *)0,1,30,2,&target_db);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddspath2",7223,4539);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into dd_paths(localdb, sourcedb, targetdb, cdds_no)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&local_db);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&source_db);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&target_db);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"cddspath2",7223,4539);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 561 "cddspath.sc" */	/* host code */
			else if (rowstate == stCHANGE)
/* # line 562 "cddspath.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddspath3",7224,8123);
      IIputdomio((short *)0,1,30,2,&local_db);
      IIputdomio((short *)0,1,30,2,&source_db);
      IIputdomio((short *)0,1,30,2,&target_db);
      IIputdomio((short *)0,1,30,2,&old_source_db);
      IIputdomio((short *)0,1,30,2,&old_local_db);
      IIputdomio((short *)0,1,30,2,&old_target_db);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cddspath3",7224,8123);
        IIwritio(0,(short *)0,1,32,0,(char *)"update dd_paths set loca\
ldb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&local_db);
        IIwritio(0,(short *)0,1,32,0,(char *)", sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&source_db);
        IIwritio(0,(short *)0,1,32,0,(char *)", targetdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&target_db);
        IIwritio(0,(short *)0,1,32,0,(char *)"where sourcedb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&old_source_db);
        IIwritio(0,(short *)0,1,32,0,(char *)"and localdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,2,&old_local_db);
        IIwritio(0,(short *)0,1,32,0,(char *)"and targetdb=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,2,&old_target_db);
        IIwritio(0,(short *)0,1,32,0,(char *)"and cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIexDefine(0,(char *)"cddspath3",7224,8123);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 570 "cddspath.sc" */	/* host code */
			if (rowstate == stNEW || rowstate == stCHANGE)
			{
				if (RPdb_error_check(DBEC_SINGLE_ROW, NULL)
					!= OK)
				{
/* # line 575 "cddspath.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 576 "cddspath.sc" */	/* host code */
					IIUGerr(E_RM002A_Error_saving_tf,
						UG_ERR_ERROR, 0);
/* # line 357 "cddspath.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,rownumber) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 358 "cddspath.qsc" */	/* resume */
                  {
                    IIresfld(tf_name);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 359 "cddspath.qsc" */	/* host code */
				}
			}
                }
              } /* IItunload */
IItbE3:
              IItunend();
            }
/* # line 363 "cddspath.qsc" */	/* host code */
		if (db_config_changed(0) != OK)
		{
/* # line 603 "cddspath.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 604 "cddspath.sc" */	/* host code */
/* # line 366 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 367 "cddspath.qsc" */	/* host code */
		}
/* # line 610 "cddspath.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 611 "cddspath.sc" */	/* host code */
		if (RPdb_error_check(0, NULL) != OK)
		{
/* # line 613 "cddspath.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 614 "cddspath.sc" */	/* host code */
			IIUGerr(E_RM000A_Error_committing, UG_ERR_ERROR, 0);
/* # line 374 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 375 "cddspath.qsc" */	/* host code */
		}
/* # line 377 "cddspath.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 383 "cddspath.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,form_name,tf_name,-2) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio((char *)"_STATE",(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 390 "cddspath.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,form_name,tf_name,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 391 "cddspath.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 398 "cddspath.qsc" */	/* inquire_frs */
            {
              if (IIiqset(4,0,form_name,tf_name,(char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,objname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 399 "cddspath.qsc" */	/* host code */
		*list_rows = EOS;
		if (STequal(objname, ftarget_db))
		{
/* # line 402 "cddspath.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,30,2,&source_db,fsource_db);
                IItcogetio((short *)0,1,30,2,&local_db,flocal_db);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 404 "cddspath.qsc" */	/* host code */
/* # line 677 "cddspath.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no, TRIM(vnode_name) +'::' +database_name from dd_dat\
abases where database_no!=");
    IIputdomio((short *)0,1,30,2,&source_db);
    IIwritio(0,(short *)0,1,32,0,(char *)"and database_no!=");
    IIputdomio((short *)0,1,30,2,&local_db);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and database_no in(select database_no from dd_db_cdds where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)")order by database_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,db_name);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 689 "cddspath.sc" */	/* host code */
				STtrmwhite(db_name);
				STprintf(list_row, ERx("%d;%s\n"), db_no,
					db_name);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
		}
		else	/* localdb or sourcedb */
		{
/* # line 697 "cddspath.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no, TRIM(vnode_name) +'::' +database_name from dd_dat\
abases where database_no in(select database_no from dd_db_cdds where c\
dds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and target_type=1)order by database_no");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&db_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,db_name);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 708 "cddspath.sc" */	/* host code */
				STtrmwhite(db_name);
				STprintf(list_row, ERx("%d;%s\n"), db_no,
					db_name);
				STcat(list_rows, list_row);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
		}
/* # line 714 "cddspath.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&n,8);
  }
/* # line 715 "cddspath.sc" */	/* host code */
		if (!n)
		{
			IIUGerr(E_RM0080_No_valid_choices, UG_ERR_ERROR, 0);
/* # line 445 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 446 "cddspath.qsc" */	/* host code */
		}
/* # line 724 "cddspath.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 725 "cddspath.sc" */	/* host code */
		row = IIFDlpListPick(NULL, list_rows, 5, -1, -1, NULL, NULL,
			NULL, NULL);
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
			STcopy(pend, db_name);
			*p2 = EOS;
			CVan(p1, &i);
			db_no = (i2)i;
			if (STequal(objname, fsource_db))
/* # line 468 "cddspath.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(fsource_db,(short *)0,1,30,2,&db_no);
                IItcoputio(fsource_name,(short *)0,1,32,0,db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 470 "cddspath.qsc" */	/* host code */
			else if (STequal(objname, flocal_db))
/* # line 471 "cddspath.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(flocal_db,(short *)0,1,30,2,&db_no);
                IItcoputio(flocal_name,(short *)0,1,32,0,db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 473 "cddspath.qsc" */	/* host code */
			else
/* # line 474 "cddspath.qsc" */	/* putrow */
            {
              if (IItbsetio(3,form_name,tf_name,-2) != 0) {
                IItcoputio(ftarget_db,(short *)0,1,30,2,&db_no);
                IItcoputio(ftarget_name,(short *)0,1,32,0,db_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 476 "cddspath.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 477 "cddspath.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 8) {
          {
/* # line 482 "cddspath.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 9) {
          {
/* # line 487 "cddspath.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 492 "cddspath.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 499 "cddspath.qsc" */	/* host code */
		FEhelp(ERx("rmpaths.hlp"), ERget(F_RM0037_Update_paths));
          }
        } else if (IIretval() == 12) {
          {
/* # line 506 "cddspath.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chng,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 507 "cddspath.qsc" */	/* host code */
		if (chng && IIUFccConfirmChoice(CONF_GENERIC, NULL, NULL,
			NULL, NULL, S_RM000E_End_no_save, S_RM000F_Yes_end,
			S_RM0010_No_end, ERx(""), TRUE) != CONFCH_YES)
/* # line 510 "cddspath.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 512 "cddspath.qsc" */	/* breakdisplay */
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
      if (IITBcaClmAct(tf_name,fsource_db,0,1) == 0) goto IIfdE1;
      if (IITBcaClmAct(tf_name,flocal_db,0,2) == 0) goto IIfdE1;
      if (IITBcaClmAct(tf_name,ftarget_db,0,3) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Save),ERget(F_FE0117_SaveExpl),2,2,4) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Insert),ERget(F_RM0036_Insert_expl),2,2,5) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(12,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_RM0032_RowDelete),ERget(F_RM0033_RowDelete_expl),0,
      0,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(14,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,0,7) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,10) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,11) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,11) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,12) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,0,12) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 515 "cddspath.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(63,(char *)0,0,(short *)0,1,30,4,&getmsg_stat);
      } /* IIiqset */
    }
  }
/* # line 519 "cddspath.qsc" */	/* host code */
/*{
** Name:	get_database	- get database
**
** Description:
**	get info for a database in a given CDDS
**
** Inputs:
**	db_no		- database number
**	cdds_no		- CDDS number
**
** Outputs:
**	db_name		- database name
**
** Returns:
**	OK if database found, FAIL otherwise
*/
static STATUS
get_database(
i2	db_no,
i2	cdds_no,
char	*db_name)
# if 0
  i2 db_no;
  i2 cdds_no;
  char *db_name;
# endif
{
	DBEC_ERR_INFO	errinfo;
/* # line 920 "cddspath.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cddspath4",7225,27899);
      IIputdomio((short *)0,1,30,2,&db_no);
      IIputdomio((short *)0,1,30,2,&cdds_no);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"cddspath4",7225,27899);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(d.vnode_name) +'::' +TRIM(d.database_name), c.target_type\
 from dd_databases d, dd_db_cdds c where d.database_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,2,&db_no);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and c.database_no=d.database_no and c.cdds_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&cdds_no);
        IIexDefine(0,(char *)"cddspath4",7225,27899);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,db_name);
      IIgetdomio((short *)0,1,30,2,&h_target_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 928 "cddspath.sc" */	/* host code */
	if (RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo) != OK)
	{
/* # line 930 "cddspath.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 931 "cddspath.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return (FAIL);
	}
	else if (errinfo.rowcount == 0)
	{
		STcopy(ERget(F_RM0035_None), db_name);
	}
/* # line 939 "cddspath.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 940 "cddspath.sc" */	/* host code */
	return (OK);
}
