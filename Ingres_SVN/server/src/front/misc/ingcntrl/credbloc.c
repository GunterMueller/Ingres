# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<lo.h>
#include	<st.h>
#include	<er.h>
#include	<gl.h>
#include	<sl.h>
#include	<iicommon.h>
#include	<fe.h>
# include <ui.h>
/*
#include	<ui.h>
*/
#include	<ug.h>
#include	"eric.h"
#include	<erscf.h>
GLOBALREF char	*Uover;	/* -u flag */
  GLOBALREF char *Pswd;
typedef	i4 (*FUNCP)();
FUNC_EXTERN	FUNCP IIseterr();
static	i4	usererr();
/**
** CREDBLOC -- create a data base directory location
**	       Must override user's umask (UNIX terms) to insure
**			ingres can access database.
**
** History:
**
**    01/20/86 (ac)   Bug # 6436 fix. LOexist(db directory)
**			    on the new location before calling
**			    LOcreate().
**
**    4/22/87 (peterk) - separated into separate file.
**			    added code to nofity database of new
**			    location.
**      27-sep-1989 (jennifer)
**             OBSOLETED THIS FILE BY REPLACING THIS WITH an internal database 
**             procedure which creates the location and adds it to iilocation.
**	5/16/90 (pete)   - Change FEingres to FEningres. Also Pass Pswd (-P
**				flag for passwords)
**	5/22/90 (pete)	- Change to use multiple sessions, rather than
**			connecting and disconnecting.
**	6/19/90 (pete)  Change to check E_SC0301_LOC_EXISTS after the DEFINE
**			LOCATION statement, rather than checking the
**			"hard-coded" number 0x090301.
**			Also, check the "dbmserror" (ingres), rather than
**			the "errorno" (generic error).
**      19-oct-1990 (pete)
**              Merge in changes done by Jennifer Lyerla in 9/89 for B1 proj.
**	22-Oct-90 (pete)
**		Moved check for non-existant location above point where
**		we issue the DEFINE LOCATION statement. If area
**		for this location does not exist then don't want to
**		extend the database to it. (Bug 34023)
**	19-dec-91 (pete)
**		Check ERRORNO in rather than DBMSERROR in inquire_ingres
**		statement for E_SC0301_LOC_EXISTS. Bug 41367. Also added
**		support for IIseterr so can suppress the E_SC0301_LOC_EXISTS
**		error message.
**	04-dec-92 (billc) this is no longer called.  Dead file.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      23-feb-2005 (stial01)
**          Explicitly set isolation level after connect
*/
STATUS
credbloc(db, dbaname, loc_name, area)
  char *db;
  char *dbaname;
  char *loc_name;
  char *area;
{
    LOCATION	loc;
    STATUS	ret_val;
    char	*pathname;
  char definecmd[DB_AREA_MAX+FE_MAXNAME+60];
  i4 ingerr;
    i4	(*errfunc)();
    /* Here in 6.0 we will have to:
    **		exit from dbdb
    **		enter selected db
    **		DEFINE LOCATION loc_name WITH TYPE = DATA, AREA = area_pathname
    **		exit selected db
    **		re-enter dbdb
    **		if any error return error code
    */
    LOingpath(area, db, LO_DB, &loc);
    /* test length of area pathname */
    LOtos(&loc, &pathname);
    if (STlength(pathname) > DB_AREA_MAX)
    {
	i4 area_max = DB_AREA_MAX ;
	IIUGerr(E_IC000E_Full_pathname_too_big, UG_ERR_ERROR, (i4) 3,
		(PTR) pathname, (PTR) loc_name, (PTR) &area_max);
	return FAIL;
    }
    if (LOexist(&loc) != OK)
    {
	/* location does not exist; try to create it. */
	PEsave();
	PEumask(ERx("rwxrwx"));
	ret_val = LOcreate(&loc);
	PEreset();
	if (ret_val != OK)
	{
	    IIUGerr(E_IC006A_BadArea, UG_ERR_ERROR, 3, (PTR) loc_name,
		(PTR) pathname, (PTR) db);
	    return ret_val;
	}
    }
/* # line 132 "credbloc.qsc" */	/* message */
  {
    IImessage(ERget(S_IC000F_Starting_Target_DBMS));
  }
/* # line 133 "credbloc.qsc" */	/* host code */
    /* connect to user's database as DBA */
/* # line 128 "credbloc.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IILQsidSessID(2);
    IIsqUser(dbaname);
    IIsqConnect(0,db,(char *)"-$",(char *)"+U",Pswd,(char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0);
  }
/* # line 130 "credbloc.sc" */	/* host code */
    if (FEinqerr() != 0)
    {
/* # line 132 "credbloc.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 133 "credbloc.sc" */	/* host code */
        IIUGerr(E_IC0010_Error_opening_target, UG_ERR_ERROR, 1, db);
        return FAIL;
    }
/* # line 136 "credbloc.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
    IIsyncup((char *)0,0);
  }
/* # line 137 "credbloc.sc" */	/* host code */
    /* update db's config file and notify servers of new location */
    _VOID_ STprintf(definecmd,
		ERx("DEFINE LOCATION %s WITH TYPE = DATA, AREA = \"%s\" "),
		loc_name, pathname
    );
    errfunc = IIseterr(usererr);  /* Set error handler to one that will
				  ** suppress message: E_SC0301_LOC_EXISTS.
				  */
/* # line 158 "credbloc.qsc" */	/* iistatement */
  {
    IIwritio(1,(short *)0,1,32,0,definecmd);
    IIsyncup((char *)0,0);
  }
/* # line 160 "credbloc.qsc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&ingerr,(char *)"ERRORNO");
  }
/* # line 162 "credbloc.qsc" */	/* host code */
    _VOID_ IIseterr(errfunc);
    if ((ingerr != 0) && (ingerr != E_SC0301_LOC_EXISTS))
    {
/* # line 158 "credbloc.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 159 "credbloc.sc" */	/* host code */
    }
    else
    {
/* # line 162 "credbloc.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 163 "credbloc.sc" */	/* host code */
    }
    /* exit the user's database and re-enter iidbdb */
/* # line 165 "credbloc.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 166 "credbloc.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(UI_SESSION),(void *)IILQint(
    UI_SESSION));
  }
/* # line 167 "credbloc.sc" */	/* host code */
    /*
    ** Don't check the error status of the DEFINE LOCATION statement until
    ** AFTER we've restarted the iidbdb. Then, report the problem
    ** and exit, once the iidbdb is up, so our program can continue.
    */
    if ((ingerr != 0) && (ingerr != E_SC0301_LOC_EXISTS))
    {
	IIUGerr(E_IC0014_define_loc_error, UG_ERR_ERROR, 0);
	return FAIL;
    }
    return OK;
}
/* Suppress selected error message */
static i4
usererr(errnum)
i4 *errnum;
{
	if (*errnum == E_SC0301_LOC_EXISTS)
	{
		return (0);	/* don't print error message */
	}
	else
	{
		return (*errnum);	/* don't print error message */
	}
}
