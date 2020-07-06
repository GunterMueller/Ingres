# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<er.h>
#include	<gl.h>
#include	<sl.h>
#include	<iicommon.h>
#include	<fe.h>
#include	<erfe.h>
#include	<ug.h>
#include	<uf.h>
#include	"eric.h"
#include	"global.h"
# include "ictables.h"
/*
fool ming:
#include	"ictables.h"
*/
/**
** Name:	selects.qsc	- Select queries. Return counts or single rows.
**
** Description:
**	Routines which do repeated SELECTs of single rows in tables. Information
**	is selected into structures whose names match up with the corresponding
**	table. Arguments to each routine are the primary key of the table.
**	The "Exists" routines return a count and generally the arguments only
**	give part of a composite primary key.
**
**	These routines maximize performance and query sharing and help
**	minimize the number of repeated queries in the server.
**
** Defines:
**	iiicsuSelectUser	- select info about a user from iiuser.
**	iiicsgSelectUsergroup	- select info about a group from iiusergroup.
**	iiicsrSelectRole	- select info about a role from iirole.
**	iiicsdSelectDatabase	- select database info from iidatabase.
**	iiicsp2SelectDbpriv2	- select info from iidbpriv where gtype=~V.
**
**	iiicueUsergroupExists	- Count number of rows in iiusergroup with
**				  a particular group name.
**	iiicaeAreaExists	- Check if a location's area exists.
** History:
**	6-nov-1990 (pete)
**		Initial Version.
**	20-sep-93 (robf)
**              Update queries to use standard catalogs (e.g iiusers) instead
**		of base catalogs (iiuser) this allows users without
**	        QUERY_SYSCAT privilege to get appricable info.
**	7-dec-1994 (andyw)
**	  	Update query to handle valid work location for purposes
**		of partial backup & recovery
**      25-sep-96 (mcgem01)
**              Global data moved to ingdata.c
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	23-May-2001 (jenjo02)
**	    Added iiicaeAreaExists().
**	15-Oct-2001 (jenjo02)
**	    Removed short-lived iiicaeAreaExists(). Area directories
**	    are now made by CREATE/ALTER LOCATION, obviating the
**	    need to check existence.
**/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
  GLOBALREF IIUSER Iiuser;
  GLOBALREF IIUSERGROUP Iiusergroup;
  GLOBALREF IIROLE Iirole;
  GLOBALREF IIDBPRIV Iidbpriv;
  GLOBALREF IIDATABASE Iidatabase;
/* # line 79 "selects.sc" */	/* host code */
/**
** Name:        iiicsuSelectUser - select info about a user from iiuser.
**
** Description:
**	Select all info about a particular user from catalog "iiuser".
**
** Inputs:
**      char    *u_user           name of user to select info for.
**      IIUSER  *iiuser         structure to select user info into.
**
** Returns:     OK if SELECT query ran without error.
**		FAIL if error occurred during SELECT.
**
** History:
**      6-nov-1990 (pete)
**              Initial Version.
**	20-jul-94 (robf)
**              Save internal flags and default privileges for iiusers.
*/
STATUS
iiicsuSelectUser(u_name, iiuser)
  char *u_name;
  IIUSER *iiuser;
{
    /* Look up user.
    ** Following 2 queries changed from QUEL to SQL so pattern match
    ** characters in "U_name" will be treated as literals (jupbug 20469).
    */
/* # line 109 "selects.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"selects1",7135,8437);
      IIputdomio((short *)0,1,32,0,u_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"selects1",7135,8437);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select user_name, 0, internal_status, default_group, profile_name, ex\
pire_date, internal_flags, internal_def_priv from iiusers where user_n\
ame=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,u_name);
        IIexDefine(0,(char *)"selects1",7135,8437);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iiuser->name);
      IIgetdomio((short *)0,1,30,4,&iiuser->gid);
      IIgetdomio((short *)0,1,30,4,&iiuser->status);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iiuser->default_group);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iiuser->profile);
      IIgetdomio((short *)0,1,32,25,iiuser->expire_date);
      IIgetdomio((short *)0,1,30,4,&iiuser->internal_flags);
      IIgetdomio((short *)0,1,30,4,&iiuser->default_status);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 124 "selects.sc" */	/* host code */
    STtrmwhite(iiuser->name);
    STtrmwhite(iiuser->default_group);
    return (FEinqerr());
}
/**
** Name:        iiicsgSelectUsergroup - select group info from iiusergroup.
**
** Description:
**	Select all info about a particular group from catalog "iiusergroup".
**
** Inputs:
**      char    *u_group        name of group to select info for.
**	char	*groupmem	user who belongs to this group.
**      IIUSERGROUP  *iiusergroup  structure to select user info into.
**
** Returns:     OK if SELECT query ran without error.
**		FAIL if error occurred during SELECT.
**
** History:
**      6-nov-1990 (pete)
**              Initial Version.
*/
STATUS
iiicsgSelectUsergroup(u_group, groupmem, iiusergroup)
  char *u_group;
  char *groupmem;
  IIUSERGROUP *iiusergroup;
{
/* # line 154 "selects.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"selects2",7136,8556);
      IIputdomio((short *)0,1,32,0,u_group);
      IIputdomio((short *)0,1,32,0,groupmem);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"selects2",7136,8556);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select groupid, groupmem from iiusergroup where groupid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,u_group);
        IIwritio(0,(short *)0,1,32,0,(char *)"and groupmem=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,groupmem);
        IIexDefine(0,(char *)"selects2",7136,8556);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iiusergroup->groupid);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iiusergroup->groupmem);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 159 "selects.sc" */	/* host code */
    STtrmwhite(iiusergroup->groupid);
    STtrmwhite(iiusergroup->groupmem);
    if (FEinqerr() != OK)
	    return FAIL;
    else
	    return OK;
}
/**
** Name:        iiicsrSelectRole - select role info from iirole.
**
** Description:
**	Select all info about a particular role from catalog "iirole".
**
** Inputs:
**      char    *u_role         name of role to select info for.
**      IIROLE  *iirole  	structure to select user info into.
**
** Returns:     OK if SELECT query ran without error.
**		FAIL if error occurred during SELECT.
**
** History:
**      6-nov-1990 (pete)
**              Initial Version.
*/
STATUS
iiicsrSelectRole(u_role, iirole)
  char *u_role;
  IIROLE *iirole;
{
/* # line 190 "selects.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"selects3",7137,8305);
      IIputdomio((short *)0,1,32,0,u_role);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"selects3",7137,8305);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select roleid, rolepass from iirole where roleid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,u_role);
        IIexDefine(0,(char *)"selects3",7137,8305);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iirole->roleid);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iirole->rolepass);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 194 "selects.sc" */	/* host code */
    STtrmwhite(iirole->roleid);
    return FEinqerr();
}
/**
** Name:        iiicsdSelectDatabase - select database info from iidatabase.
**
** Description:
**	Select all info about a particular database from catalog "iidatabase".
**
** Inputs:
**	char	    *dbname	name of database to select info for.
**	IIDATABASE  *iirole	structure to select user info into.
**
** Returns:     OK if SELECT query ran without error.
**		FAIL if error occurred during SELECT.
**
** History:
**      8-nov-1990 (pete)
**              Initial Version.
**	17-dec-93 (robf)
**              Updated attributes names in iidatabase_info after LRC
**		review.
**	7-dec-1994 (andyw)
**		Added valid work_location display when using the partial
**	  	backup and recovery
*/
STATUS
iiicsdSelectDatabase(dbname, iidatabase)
  char *dbname;
  IIDATABASE *iidatabase;
{
/* # line 227 "selects.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"selects4",7138,10081);
      IIputdomio((short *)0,1,32,0,dbname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"selects4",7138,10081);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name, database_owner, data_location, ckp_location, jn\
l_location, work_location, access, database_service, compat_level, com\
pat_level_minor, database_id, dump_location from iidatabase_info where\
 database_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,dbname);
        IIexDefine(0,(char *)"selects4",7138,10081);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidatabase->name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidatabase->own);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidatabase->dbdev);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidatabase->ckpdev);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidatabase->jnldev);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidatabase->sortdev);
      IIgetdomio((short *)0,1,30,4,&iidatabase->access);
      IIgetdomio((short *)0,1,30,4,&iidatabase->dbservice);
      IIgetdomio((short *)0,1,32,4+1-1,iidatabase->dbcmptlvl);
      IIgetdomio((short *)0,1,30,4,&iidatabase->dbcmptminor);
      IIgetdomio((short *)0,1,30,4,&iidatabase->db_id);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidatabase->dmpdev);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 254 "selects.sc" */	/* host code */
    if (FEinqerr() != OK)
    {
	    IIUGerr(E_IC0068_Retrieve_Error, UG_ERR_ERROR,(i4) 1, dbname);
	    return FAIL;
    }
    else if (FEinqrows() <= 0 )
    {
	    IIUGerr( E_IC0020_Database_doesnt_exist, UG_ERR_ERROR,
		    (i4) 1, dbname);
	    return FAIL;
    }
    STtrmwhite(iidatabase->name);
    STtrmwhite(iidatabase->own);
    STtrmwhite(iidatabase->dbdev);
    STtrmwhite(iidatabase->ckpdev);
    STtrmwhite(iidatabase->jnldev);
    STtrmwhite(iidatabase->sortdev);
    STtrmwhite(iidatabase->dmpdev);
    return (OK);
}
/**
** Name:      iiicspSelectDbpriv - select database privilege info from iidbpriv.
**
** Description:
**	Select all info on database privileges from the iidbpriv catalog.
**	Selects multiple rows -- caller must provide function to handle
**	the selected rows.
**
** Inputs:
**	char	    *dbname	name of database to select info for.
**	IIDBPRIV    *iidbpriv	structure to select each row of dbpriv info to.
**	STATUS      (*func)()	function to call inside loop. This function
**				should return a STATUS. If returns FAIL, then
**				iiicspSelectDbpriv will terminate the SELECT
**				loop. If set to NULL, then function is not
**				called.
**
** Returns:     OK if SELECT query ran without error.
**		FAIL if error occurred during SELECT.
**
** History:
**      9-nov-1990 (pete)
**              Initial Version.
**	2-dec-1991 (pete)
**		Change "fill1" to "dbflags" to match 6.5 table definition.
**      19-Oct-1998 (wanya01)
**              Bug 93383. Add new parameter dflag.
*/
STATUS
iiicspSelectDbpriv(dbname, iidbpriv, func, dflag)
  char *dbname;
  IIDBPRIV *iidbpriv;
int            *dflag;
STATUS		(*func)();
{
    STATUS stat = OK;
/* # line 312 "selects.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"selects5",7139,25200);
      IIputdomio((short *)0,1,32,0,dbname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"selects5",7139,25200);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select dbname, grantee, gtype, dbflags, control, flags, qrow_limit, q\
dio_limit, qcpu_limit, qpage_limit, qcost_limit from iidbpriv where db\
name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,dbname);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by dbname, gtype");
        IIexDefine(0,(char *)"selects5",7139,25200);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidbpriv->dbname);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidbpriv->grantee);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->gtype);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->dbflags);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->control);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->flags);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qrow_limit);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qdio_limit);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qcpu_limit);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qpage_limit);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qcost_limit);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 339 "selects.sc" */	/* host code */
    {
    	STtrmwhite(iidbpriv->dbname);
    	STtrmwhite(iidbpriv->grantee);
        if (!STbcompare(iidbpriv->grantee,6,ERget(F_IC000C_public),6,1))
           *dflag =  ((iidbpriv->flags & DB_ACCESS) != 0) ? TRUE : FALSE ;
	if ( (*func) != NULL)
	{
	    stat = (*func)(iidbpriv);
	}
	if (stat == FAIL)
/* # line 349 "selects.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
        if (sqlca.sqlcode < 0) 
          IIsqPrint(&sqlca);
      }
/* # line 350 "selects.sc" */	/* host code */
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
    if (FEinqerr() != OK)	/* error in above SELECT */
	stat = FAIL;
    return stat;
}
/**
** Name:      iiicsp2SelectDbpriv2 - select from iidbpriv where gtype=~V.
**
** Description:
**	Select all info on database privileges from the iidbpriv catalog
**	for a particular Grantee type. Order by database name.
**	Selects multiple rows -- caller must provide function to handle
**	the selected rows.
**
** Inputs:
**	i4	    gtype	Grantee type to select.
**	IIDBPRIV    *iidbpriv	structure to select each row of dbpriv info to.
**	STATUS      (*func)()	function to call inside loop. This function
**				should return a STATUS. If returns FAIL, then
**				iiicsp2SelectDbpriv2 will terminate the SELECT
**				loop. If set to NULL, then function is not
**				called.
**
** Returns:     OK if SELECT query ran without error.
**		FAIL if error occurred during SELECT.
**
** History:
**      15-nov-1990 (pete)
**              Initial Version.
*/
STATUS
iiicsp2SelectDbpriv2(gtype, iidbpriv, func)
  i4 gtype;
  IIDBPRIV *iidbpriv;
STATUS		(*func)();
{
    STATUS stat = OK;
/* # line 390 "selects.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"selects6",7140,28014);
      IIputdomio((short *)0,1,30,4,&gtype);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"selects6",7140,28014);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select dbname, grantee, gtype, dbflags, control, flags, qrow_limit, q\
dio_limit, qcpu_limit, qpage_limit, qcost_limit from iidbpriv where gt\
ype=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&gtype);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by dbname, gtype");
        IIexDefine(0,(char *)"selects6",7140,28014);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidbpriv->dbname);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,iidbpriv->grantee);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->gtype);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->dbflags);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->control);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->flags);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qrow_limit);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qdio_limit);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qcpu_limit);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qpage_limit);
      IIgetdomio((short *)0,1,30,4,&iidbpriv->qcost_limit);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 417 "selects.sc" */	/* host code */
    {
    	STtrmwhite(iidbpriv->dbname);
    	STtrmwhite(iidbpriv->grantee);
	if ( (*func) != NULL)
	{
	    stat = (*func)(iidbpriv);
	}
	if (stat == FAIL)
/* # line 425 "selects.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
        if (sqlca.sqlcode < 0) 
          IIsqPrint(&sqlca);
      }
/* # line 426 "selects.sc" */	/* host code */
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
    if (FEinqerr() != OK)	/* error in above SELECT */
	stat = FAIL;
    return stat;
}
/**
** Name:        iiicueUsergroupExists - Check if usergroup exists.
**
** Description:
**	Select count of how many usergroups with a particular group name
**	exist.
**
** Inputs:
**      char    *u_group           name of group to check.
**
** Returns:
**	Number of rows in iiusergroup with this group.
**
** History:
**      6-nov-1990 (pete)
**              Initial Version.
*/
i4
iiicueUsergroupExists(u_group)
  char *u_group;
{
  i4 cnt =0;
/* # line 458 "selects.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"selects7",7141,26077);
      IIputdomio((short *)0,1,32,0,u_group);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"selects7",7141,26077);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiusergroup where groupid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,u_group);
        IIexDefine(0,(char *)"selects7",7141,26077);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 462 "selects.sc" */	/* host code */
    return cnt;
}
