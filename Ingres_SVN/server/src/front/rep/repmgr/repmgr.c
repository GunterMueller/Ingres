# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <st.h>
# include <er.h>
# include <ex.h>
# include <me.h>
# include <pc.h>
# include <nm.h>
# include <lo.h>
# include <ci.h>
# include <gl.h>
# include <iicommon.h>
# include <adf.h>
# include <fe.h>
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include <uigdata.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	repmgr.qsc - Replicator Manager main
**
** Description:
**	Defines
**		main	- Replicator Manager main
**
** History:
**	30-dec-96 (joea)
**		Created based on repmgr.osq in replicator library.
**	23-jan-97 (joea)
**		Call FEendforms after an error in RMget_server_dir.
**	07-oct-97 (joea)
**		Commit after selecting the local database, to prevent a
**		MoveConfig to this database from being blocked by the unreleased
**		dd_databases lock.
**	27-mar-98 (joea)
**		Discontinue use of RPing_ver_ok.
**	03-jun-98 (abbjo03)
**		Don't call RMget_server_dir here.
**	23-nov-98 (abbjo03)
**		Use IIUIdbdata()->suser instead of IIuiDBA because under SQL92
**		the former provides the uppercase user name.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB and SHFRAMELIB
**	    to replace its static libraries.  Added NEEDLIBSW hint which is 
**	    used for compiling on windows and compliments NEEDLIBS.
**/
/*
PROGRAM =	repmgr
NEEDLIBS =	REPMGRLIB REPCOMNLIB SHFRAMELIB SHQLIB SHCOMPATLIB
NEEDLIBSW = 	SHEMBEDLIB SHADFLIB
UNDEFS =	II_copyright
*/
GLOBALDEF
  char RMdb_display[37];
GLOBALDEF
  i2 RMlocal_db_no;
GLOBALDEF ADF_CB	*RMadf_cb;
static	bool	form_initd;
static READONLY
  char *form_name = ERx("rmmain_menu"), *tf_name = ERx("iitf"), *fcmd = ERx("cmd")
  , *fexpl = ERx("explanation");
static	char	*db_name = NULL;
static	char	*user_name = ERx("");
static	ARG_DESC args[] =
{
	/* required */
	{ERx("database"), DB_CHR_TYPE, FARG_PROMPT, (PTR)&db_name},
	/* optional */
	{ERx("user"), DB_CHR_TYPE, FARG_FAIL, (PTR)&user_name},
	NULL
};
FUNC_EXTERN STATUS db_add_local(char *db_name, char *db_owner, i2 *db_no);
FUNC_EXTERN STATUS cfg_menu(void);
FUNC_EXTERN STATUS FEhandler(EX_ARGS *ex);
FUNC_EXTERN ADF_CB *FEadfcb(void);
/*{
** Name:	main	- Replicator Manager main
**
** Description:
**	Replicator Manager (RepMgr) main menu.  It can be invoked as:
**
**		repmgr dbname [-uuser]
**
**	where:
**		dbname	- database name, which may include a vnode.  If not
**			  specified, the user will be prompted.
**		user	- alternate username.
**
** Inputs:
**	argc	- number of arguments on the command line
**	argv	- argument vector, from command line
**
** Returns:
**	OK	- everything went OK
**	FAIL	- something went wrong
*/
i4
main(
i4	argc,
char	*argv[])
  {
    char menu_choice[81];
	EX_CONTEXT	context;
	DBEC_ERR_INFO	errinfo;
	/* Tell EX this is an INGRES tool */
	(void)EXsetclient(EX_INGRES_TOOL);
	/* Use the INGRES memory allocator */
	(void)MEadvise(ME_INGRES_ALLOC);
	/* Initialize character set attribute table */
	if (IIUGinit() != OK)
		PCexit(FAIL);
	/* Parse the command line arguments */
	if (IIUGuapParse(argc, argv, ERx("repmgr"), args) != OK)
		PCexit(FAIL);
	/* Set up the generic exception handler */
	if (EXdeclare(FEhandler, &context) != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
	/* Start the forms system and display the copyright message */
	if (FEforms() != OK)
	{
		EXdelete();
		PCexit(FAIL);
	}
	FEcopyright(ERx("REPMGR"), ERx("1993"));
	RMadf_cb = FEadfcb();
	/* Open the database */
	if (FEningres(NULL, (i4)0, db_name, user_name, NULL) != OK)
	{
		FEendforms();
		EXdelete();
		PCexit(FAIL);
	}
/* # line 150 "repmgr.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
  }
/* # line 151 "repmgr.sc" */	/* set_sql */
  {
    IILQssSetSqlio(3,(short *)0,1,32,0,(char *)"genericerror");
  }
/* # line 152 "repmgr.sc" */	/* host code */
	if (!IIuiIsDBA)
	{
		EXdelete();
		IIUGerr(E_RM0002_Must_be_DBA, UG_ERR_FATAL, 0);
	}
	if (!RMcheck_cat_level())
	{
		EXdelete();
		IIUGerr(E_RM0003_No_rep_catalogs, UG_ERR_FATAL, 0);
	}
/* # line 162 "repmgr.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no from dd_databases where local_db=1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&RMlocal_db_no);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 168 "repmgr.sc" */	/* host code */
	(void)RPdb_error_check(DBEC_ZERO_ROWS_OK, &errinfo);
	if (errinfo.errorno)
	{
		EXdelete();
		IIUGerr(E_RM0004_Error_get_local_db, UG_ERR_FATAL, 0);
	}
	else if (errinfo.rowcount == 0)
	{
		if (db_add_local(IIuiDatabase, IIUIdbdata()->suser,
			&RMlocal_db_no) != OK)
		{
			FEendforms();
			FEing_exit();
			EXdelete();
			PCexit(FAIL);
		}
	}
	else if (errinfo.rowcount > 1)
	{
		EXdelete();
		IIUGerr(E_RM0005_Too_many_local_db, UG_ERR_FATAL, 0);
	}
	STprintf(RMdb_display, ERx("%s %d"), IIuiDatabase, RMlocal_db_no);
/* # line 191 "repmgr.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 192 "repmgr.sc" */	/* host code */
	if (!form_initd)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name) != OK)
			IIUGerr(E_RM0006_Get_form_failed, UG_ERR_FATAL, 1,
				form_name);
		form_initd = TRUE;
	}
/* # line 227 "repmgr.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 228 "repmgr.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0001_Configuration));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM0002_Configuration_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 230 "repmgr.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0003_Monitor));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM0004_Monitor_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 232 "repmgr.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fcmd,(short *)0,1,32,0,ERget(F_RM0005_Reports));
        IItcoputio(fexpl,(short *)0,1,32,0,ERget(F_RM0006_Reports_expl));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 234 "repmgr.qsc" */	/* scroll */
    {
      if (IItbsetio(1,form_name,tf_name,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,1) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 236 "repmgr.qsc" */	/* display */
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
/* # line 241 "repmgr.qsc" */	/* getrow */
            {
              if (IItbsetio(2,form_name,tf_name,-2) != 0) {
                IItcogetio((short *)0,1,32,80,menu_choice,fcmd);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 242 "repmgr.qsc" */	/* host code */
		if (STequal(menu_choice, ERget(F_RM0001_Configuration)))
		{
			if (cfg_menu() == 1)
/* # line 245 "repmgr.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 246 "repmgr.qsc" */	/* host code */
		}
		else if (STequal(menu_choice, ERget(F_RM0003_Monitor)))
		{
			rep_monitor();
		}
		else if (STequal(menu_choice, ERget(F_RM0005_Reports)))
		{
			if (reports() == 1)
/* # line 254 "repmgr.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 255 "repmgr.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 260 "repmgr.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 3) {
          {
/* # line 265 "repmgr.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 270 "repmgr.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 276 "repmgr.qsc" */	/* host code */
		FEhelp(ERx("rmrepmgr.hlp"), ERget(F_RM0007_Repmgr_main_menu));
          }
        } else if (IIretval() == 6) {
          {
/* # line 282 "repmgr.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Select),ERget(F_FE0108_ExplSelect),2,2,1) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,2,5) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),ERget(F_RM0008_Quit_expl),2,2,6) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 285 "repmgr.qsc" */	/* host code */
	FEendforms();
	FEing_exit();
	EXdelete();
	PCexit(OK);
  }
