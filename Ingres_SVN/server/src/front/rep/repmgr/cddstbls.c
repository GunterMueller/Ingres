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
**  Name:	cddstbls.qsc - tables in a CDDS
**
**  Description:
**	Defines
**		cdds_regist_tables - CDDS registered tables
**
**  History:
**	16-dec-96 (joea)
**		Created based on cddstbls.osq in replicator library.
**	18-sep-97 (joea) bug 85643
**		Trim trailing white space from table status variables to
**		display correct status.
**/
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmcdds_tables"), *tf_name = ERx("iitf"), *ftable_name 
  = ERx("table_name"), *ftable_owner = ERx("table_owner"), *fcolumns = ERx("columns")
  , *fsupport = ERx("support"), *frules = ERx("rules");
/*{
** Name:	cdds_regist_tables - CDDS registered tables
**
** Description:
**	Display list of tables registered in a given cdds
**
** Inputs:
**	cdds_no		- CDDS number
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
cdds_regist_tables(
i2	cdds_no)
# if 0
  i2 cdds_no;
# endif
  {
  char table_name[DB_MAXNAME+1];
  char table_owner[DB_MAXNAME+1];
  char columns[26];
  char support[26];
  char rules[26];
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
/* # line 85 "cddstbls.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 87 "cddstbls.qsc" */	/* host code */
/* # line 84 "cddstbls.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, columns_registered, supp_objs_created\
, rules_created from dd_regist_tables where cdds_no=");
    IIputdomio((short *)0,1,30,2,&cdds_no);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by table_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,table_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,table_owner);
      IIgetdomio((short *)0,1,32,25,columns);
      IIgetdomio((short *)0,1,32,25,support);
      IIgetdomio((short *)0,1,32,25,rules);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 92 "cddstbls.sc" */	/* host code */
		STtrmwhite(columns);
		STtrmwhite(support);
		STtrmwhite(rules);
		if (*columns != EOS)
			STcopy(ERget(F_RM0038_yes), columns);
		if (*support != EOS)
			STcopy(ERget(F_RM0038_yes), support);
		if (*rules != EOS)
			STcopy(ERget(F_RM0038_yes), rules);
/* # line 104 "cddstbls.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(ftable_name,(short *)0,1,32,0,table_name);
        IItcoputio(ftable_owner,(short *)0,1,32,0,table_owner);
        IItcoputio(fcolumns,(short *)0,1,32,0,columns);
        IItcoputio(fsupport,(short *)0,1,32,0,support);
        IItcoputio(frules,(short *)0,1,32,0,rules);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 107 "cddstbls.qsc" */	/* host code */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	RPdb_error_check(0, &errinfo);
	if (errinfo.errorno)
	{
/* # line 117 "cddstbls.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 118 "cddstbls.sc" */	/* host code */
		FTbell();
		IIUGerr(E_RM0012_Error_selecting, UG_ERR_ERROR, 0);
		return;
	}
	else if (errinfo.rowcount == 0)
	{
/* # line 124 "cddstbls.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 125 "cddstbls.sc" */	/* host code */
		IIUGerr(E_RM0013_No_data_found, UG_ERR_ERROR, 0);
		return;
	}
/* # line 123 "cddstbls.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 126 "cddstbls.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 2) {
          {
/* # line 131 "cddstbls.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 136 "cddstbls.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 142 "cddstbls.qsc" */	/* host code */
		FEhelp(ERx("rmcddstb.hlp"), ERget(F_RM0039_Cdds_tables));
          }
        } else if (IIretval() == 5) {
          {
/* # line 148 "cddstbls.qsc" */	/* breakdisplay */
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
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,3) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,4) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,5) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
