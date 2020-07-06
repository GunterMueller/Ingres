# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1989, 2005 Ingres Corporation
**
**
*/
#include	<compat.h>
#include	<gl.h>
#include	<st.h>
#include	<me.h>
#include	<sl.h>
#include	<iicommon.h>
#include	<dbdbms.h>
#include	<er.h>
#include	<lo.h>
#include	<si.h>
#include	<cui.h>
#include	<duerr.h>
#include	<duvfiles.h>
#include	<dudbms.h>
#include	<duve.h>
#include	<duustrings.h>
#include	<cs.h>
#include	<dm.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/**
**
**  Name: DUVEDBDB.SC - Verifydb DATABASE DATABASE Catalog checks / repairs
**
**  Description:
**      This module contains the ESQL to execute the tests/fixed on the system
**	catalogs specified by the Verifydb Product Specification for:
**	    iicdbid_idx
**	    iidatabase
**	    iidbpriv
**	    iidbid_idx
**	    iiextend
**	    iilocations
**	    iistar_cdbs
**	    iiuser
**	    iiusergroup
**
**	Special ESQL include files are used to describe each database database
**	catalog.  These include files are generated via the DCLGEN utility,
**	and require a dbms server to be running.  The following commands create
**	the include files in the format required by verifydb:
	    dclgen c iidbdb iicdbid_idx duvecdbx.sh cdb_indx
	    dclgen c iidbdb iidatabase duvedb.sh db_tbl
	    dclgen c iidbdb iidbpriv duvepriv.sh priv_tbl 
	    dclgen c iidbdb iidbid_idx duvedbx.sh dbx_tbl 
	    dclgen c iidbdb iiextend duveext.sh ext_tbl 
	    dclgen c iidbdb iilocations duveloc.sh loc_tbl 
	    dclgen c iidbdb iistar_cdbs duvecdbs.sh cdbs_tbl
	    dclgen c iidbdb iiuser duveuser.sh user_tbl 
	    dclgen c iidbdb iiusergroup duveugrp.sh ugrp_tbl 
	    dclgen c iidbdb iiprofile duveuprof.sh uprof_tbl 
**
**      duvedbdb.sc uses the verifydb control block to obtain information
**	about the verifydb system catalog check environment and to access
**	cached information.
**
**	    ckdbpriv	- check / fix iidbpriv database database catalog
**	    ckcdbidx	- check / fix iicdbid_idx secondary index
**          ckdatabase	- check / fix iidatabase database database catalog
**	    duvedbdb	- get information from iidbdb before working on
**			  specified database.
**          ckdbidx	- check / fix iidbidx database database catalog
**	    ckextend	- check / fix iiextend database database catalog
**	    ckloc	- check / fix iilocations database database catalog
**	    ckstarcdbs  - check / fix iistar_cdbs database database catalog
**          ckuser	- check / fix iiuser database database catalog
**          ckusrgrp	- check / fix iiusergroup database database catalog
**	    ckusrprofile- check / fix iiprofile database database catalog
**
**
**  History:
**      31-JAN-89 (teg)
**          initial creation
**	21-dec-90 (teresa)
**	    DROPPED IN:  03-Dec-90 (jlw)  [ingres6202p change 131898]
**	    Removed duplicate header file inclusions from begining of file:
**	    duvedb.sh, duvedbx.sh, duveacc.sh, duveext.sh, duveloc.sh
**	    Headers are already #included in the appropriate functions
**	    causing duplicate definition errors.
**	16-apr-92 (teresa)
**	    compare against 0 instead of NULL (ingres63p change 265521).
**      09-nov-92 (jrb)
**	    Changed ING_SORTDIR to ING_WORKDIR and DU_SORTS_LOC to DU_WORK_LOC
**          for ML Sorts Project.
**	07-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**      11-aug-93 (ed)
**          added missing includes
**	8-sep-93 (robf)
**	    Updated iiuser checks for new privileges
**	20-dec-93 (robf)
**          Added ckusrprofile() to check iiprofile(), and add new checks
**	    for iiuser.
**      02-nov-94 (sarjo01) Bug 60179
**          ckdbpriv(): fixed inappropriate error DU168A for blank dbname
**          when GRANT was for CURRENT INSTALLATION (no dbname given)
**      03-feb-95 (chech02)
**          After 'select count(*) into :cnt from table' stmt, We need to
**          check cnt also.
**      05-jul-95 (chech02) 69675
**          fixed SEGV if -sinstallation option is used.
**	27-dec-1995 (nick)
**	    Query FIPS state before querying iiuser and set our username
**	    case appropriately. #72866
**	22-aug-1996 (kch)
**	    Added new variable, sagg, used to hold return from query to iiuser
**	    which verifies that user is known to the installation. The previous
**	    variable used, agg, was a i4 - this was not being assigned
**	    correctly on some platforms (eg. rmx), causing E_DU3001 when
**	    running verifydb with the -u flag (sagg is an i2).
**      21-apr-1999 (hanch04)
**          replace STindex with STchr
**      28-apr-1999 (hanch04)
**          Replaced STlcopy with STncpy
**	19-jan-2005 (abbjo03)
**	    Change duvecb to GLOBALREF.
**      12-mar-2007 (horda03) Bug 41389
**          When verifying an RMS Gateway DB, the user must specify /RMS with
**          the DB name. This server class must be removed when checking for
**          the existence of the DB.
**          If a DB in the INSTALLATION or for a DBA is an RMS Gateway DB then
**          need to append "/RMS" as the server class to the DB name.
[@history_template@]...
**/
/*
**  Forward and/or External typedef/struct references.
*/
	GLOBALREF DUVE_CB duvecb;
/*
**  Forward and/or External function references.
*/
FUNC_EXTERN	DU_STATUS	ckcdbidx();	/* check/repair iicdbid_idx */
FUNC_EXTERN	DU_STATUS	ckdatabase();	/* check/repair iidatabase */
FUNC_EXTERN	DU_STATUS	duve_dbdb();	/* get name,loc info from iidbdb */
FUNC_EXTERN	DU_STATUS	ckdbidx	();	/* check/repair iidbid_idx */
FUNC_EXTERN	DU_STATUS	ckextend();	/* check/repair iiextend */
FUNC_EXTERN	DU_STATUS	ckloc();	/* check/repair iilocations */
FUNC_EXTERN	DU_STATUS	ckstarcdbs();	/* check/repair iistar_cdbs */
FUNC_EXTERN	DU_STATUS	ckuser();	/* check/repair iiuser */
FUNC_EXTERN	VOID init_stats(DUVE_CB *duve_cb, i4  test_level);
FUNC_EXTERN	VOID printstats(DUVE_CB *duve_cb, i4  test_level, char *id);
/*{
** Name: duve_dbdb	- get info from iidbdb
**
** Description:
**      this routine gets all of the location names known to this installation
**	and places them on a cache.  If -sdba or -sinstallation are specified,
**	it gets all of the database names that meet the criteria of the scope.
**	If -sdbn was specified, it copies the user specified database names
**	to the database name cache. 
**
** Inputs:
**      DUVE_CB                         verifydb control block
**	    duve_scope			    scope
**	    duve_user			    user name specified with -u
**	    duveowner			    verifydb task owner
**
** Outputs:
**      DUVE_CB                         verifydb control block
**	    duve_locs			    location cache
**	    duve_lcnt			    number of locations in cache
**	    duve_list			    cache of list of database names
**	    duve_ncnt			    number of database names in cache
[@PARAM_DESCR@]...
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    INGRES errors will cause the ingres exception handler to be
**	      evoked.  If this occurs, then the program will stop execution.
**
** Side Effects:
**	    Memory is allocated for two memory caches.
**	    IIDBDB is locked for a brief period of time, which keeps other
**		terminal monitors from accessing it.
**
** History:
**      29-sep-1988 (teg)
**          initial creation
**	8-Feb-1988  (teg)
**	    added database version VS verifydb version check.  Also moved
**	    this routine from duvechk.sc to this module.
**	28-Feb-1988  (teg)
**	    Added call to duve_0mk_purgelist if operation is any of the purges
**	24-aug-1989  (teg)
**	    Fixed improper logic where dba was put into
**		duve_cb->duve_list->du_dblist[agg].du_dba) but should go into
**		duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_dba)
**	23-dec-1989 (teg)
**	    Fix ii_database typo.
**      27-dec-1989 (teg)
**          added logic to get verifydb user's INGRES user status from iidatabase
**	29-mar-90 (teg)
**	    add logic to put star CDB into into du_dblist for bug number 20971
**	23-dec-93 (teresa)
**	    change some selects to repeat queries -- part of project to
**	    improve verifydb performance time.
**	27-dec-1995 (nick)
**	    Change case of our effective username appropriately and use that
**	    when querying iiuser. #72866
**	22-aug-1996 (kch)
**	    Added new variable, sagg, used to hold return from query to iiuser
**	    which verifies that user is known to the installation. The previous
**	    variable used, agg, was a i4 - this was not being assigned
**	    correctly on some platforms (eg. rmx), causing E_DU3001 when
**	    running verifydb with the -u flag (sagg is an i2).
**      12-mar-2007 (horda03) Bug 41389
**          Don't include server class when checking if user specified DB exists.
**          If automatically verifying RMS Gateway DBs, append "/RMS" to the DB
**          name.
**      31-jan-2008 (smeke01) b118877
**          As part of enabling the removal of setuid from verifydb, removed 
**          the 'identified by $ingres' clause from the connect statement for  
**          iidbdb. This is because without setuid this clause would in effect 
**          restrict usage only to users who have the Security Admin privilege. 
**          Also amended the error handling in the case where the connect to 
**          iidbdb fails. This is because Clean_Up() downgrades an error if  
**          the operation is ACCESSCHECK. We want a fatal error if we've 
**          failed to connect to iidbdb.
[@history_template@]...
*/
DU_STATUS
duve_dbdb( duve_cb )
DUVE_CB		*duve_cb;
{
  char owner[DB_MAXNAME+1];
  char name[DB_MAXNAME+1];
  char loc[DB_MAXNAME+1];
  char cmptlvl[DU_MAXDBCMPTLVL];
  i4 compatminor;
  i4 dbservice;
  char duve_appl_flag[20];
  i4 agg;
  i2 sagg;
  i4 lstat = 0;
  char dbmstype[DU_STARMAXNAME+1];
  char cdb[DU_STARMAXNAME+1];
  char iidbdbname[DB_MAXNAME+4];
  char fipsbuf[6];
    DU_DBNAME	 tempname;
    char	 tmp_buf[15];
    u_i4	size;
    i4	i;
    STATUS	mem_stat;
    char        *colon_ptr, node[DB_MAXNAME+4];
    char        nodename[DB_MAXNAME+4];
    char        *p= nodename;
    char        *server_class;
    /*
    ** get namelist (unless -sdbn specified)
    */
    duve_cb->duve_ncnt = 0;
    duve_cb->duve_lcnt = 0;
   STcopy("iidbdb", iidbdbname);
   if (duve_cb->duve_scope == DUVE_SPECIFY)
   {
    STcopy( duve_cb->du_scopelst[0], nodename);
    if (((colon_ptr = STchr(p, ':')) != NULL) &&
	STchr(colon_ptr + 1, ':') == (colon_ptr + 1))
    {
       /* copy nodename, everything to left of "::" */
       STncpy(node, nodename, colon_ptr - p);
       node[colon_ptr - p] = '\0';
       /* copy database name, everything to right of "::" */
       STcopy(colon_ptr + 2, name);
       STprintf(iidbdbname, "%s::iidbdb", node);
    }
    else
       STcopy(nodename, name);
   }
/* # line 294 "duvedbdb.sc" */	/* host code */
    /* first try a regular connect. */
/* # line 295 "duvedbdb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,iidbdbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 296 "duvedbdb.sc" */	/* host code */
    if (sqlca.sqlcode != DU_SQL_OK)
    {
	/* open iidbdb with ACCESS EVEN IF INCONSISTENT and lock */
	sqlca.sqlcode=DU_SQL_OK;
        CVla((i4)DBA_PRETEND_VFDB, tmp_buf);
        (VOID)STpolycat(2, "-A",tmp_buf, duve_appl_flag);
/* # line 302 "duvedbdb.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser((char *)"$ingres");
    IIsqConnect(0,iidbdbname,duve_appl_flag,(char *)"-l",(char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 304 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode < DU_SQL_OK)
	{
	    duve_talk(DUVE_ALWAYS, duve_cb, E_DU501A_CANT_CONNECT, 2, 0, iidbdbname);
	    return ( (DU_STATUS) DUVE_BAD);
	}
    }
/* # line 313 "duvedbdb.sc" */	/* host code */
    /* save open db name incase we exit via Clean_Up */
    STcopy("iidbdb",duve_cb->du_opendb);
/* # line 316 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('db_real_user_case')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,5,fipsbuf);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 317 "duvedbdb.sc" */	/* host code */
    if (STcompare(ERx("UPPER"), fipsbuf) == DU_IDENTICAL)
	CVupper(duve_cb->duveowner);
    else if (STcompare(ERx("LOWER"), fipsbuf) == DU_IDENTICAL)
	CVlower(duve_cb->duveowner);
    /* else MIXED or blank - leave as is */
    /* get ingres user status of the verifydb task user */
    STcopy (duve_cb->duveowner,owner);
/* # line 325 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select status from iiuser where name=");
    IIputdomio((short *)0,1,32,0,owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&lstat);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 326 "duvedbdb.sc" */	/* host code */
    duve_cb->du_ownstat = lstat;
    /* get database list */
    if (duve_cb->duve_scope == DUVE_SPECIFY)
    {
	agg = duve_cb->duve_nscopes;
	size = agg * sizeof(tempname);
	duve_cb->duve_list = (DU_DBLIST *) MEreqmem( (u_i4)0, size, TRUE,
			      &mem_stat);
	if ( (mem_stat != OK) || (duve_cb->duve_list == NULL) )
	{
	    duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	    return ( (DU_STATUS) DUVE_BAD);
	}
	for (agg=0,duve_cb->duve_ncnt=0; agg<duve_cb->duve_nscopes; agg++) 
	{
            STcopy( duve_cb->du_scopelst[duve_cb->duve_ncnt], nodename);
            if (((colon_ptr = STchr(p, ':')) != NULL) &&
	         STchr(colon_ptr + 1, ':') == (colon_ptr + 1))
            {
             /* copy nodename, everything to left of "::" */
             STncpy(node, nodename, colon_ptr - p);
	     node[colon_ptr - p] = '\0';
             /* copy database name, everything to right of "::" */
             STcopy(colon_ptr + 2, name);
            }
            else
             STcopy(nodename, name);
            if ( (server_class = STchr(name, '/')) != NULL)
            {
               /* DB name included a server class */
               *server_class = '\0';
            }
	    STcopy( duve_cb->du_scopelst[duve_cb->duve_ncnt ],
		    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt ].du_db);
/* # line 366 "duvedbdb.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvedbdb1",6230,22892);
      IIputdomio((short *)0,1,32,0,name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvedbdb1",6230,22892);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select own, dbcmptlvl, dbcmptminor, dbservice from iidatabase where n\
ame=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIexDefine(0,(char *)"duvedbdb1",6230,22892);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,owner);
      IIgetdomio((short *)0,1,32,DU_MAXDBCMPTLVL-1,cmptlvl);
      IIgetdomio((short *)0,1,30,4,&compatminor);
      IIgetdomio((short *)0,1,30,4,&dbservice);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 369 "duvedbdb.sc" */	/* host code */
	    if (sqlca.sqlcode == DU_SQL_NONE)
	    {
	        /* user specified dbname does not exist */
		duve_talk (DUVE_ALWAYS, duve_cb, E_DU3020_DB_NOT_FOUND, 2, 0,
			   name);
	    }
	    else
	    {
		STzapblank( owner, duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_dba);
		STcopy( cmptlvl, duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_lvl);
		duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_minlvl = compatminor;
		duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_dbservice =dbservice;
		duve_cb->duve_ncnt++;
	    }
	} /* end loop */
    }
    else if (duve_cb->duve_scope == DUVE_DBA)
    {
	if (duve_cb->duve_user[0] == '\0')
	    STcopy (duve_cb->duveowner,owner);
	else
	    STcopy (duve_cb->duve_user,owner);
	/* get number of elements and allocate memory */
/* # line 393 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(name)from iidatabase where own=");
    IIputdomio((short *)0,1,32,0,owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&agg);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 394 "duvedbdb.sc" */	/* host code */
	if (agg == 0)
	{
	    duve_talk (DUVE_ALWAYS, duve_cb, E_DU5036_NODBS_IN_SCOPE, 0);
	    return ( (DU_STATUS) DUVE_BAD);
	}
	size = agg * sizeof(tempname);
	duve_cb->duve_list = (DU_DBLIST *) MEreqmem( (u_i4)0, size, TRUE,
				&mem_stat);
	if ( (mem_stat != OK) || (duve_cb->duve_list == NULL) )
	{
	    duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	    return ( (DU_STATUS) DUVE_BAD);
	}	
	/* now enter loop to get names */
/* # line 410 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select name, dbcmptlvl, dbcmptminor, dbservice from iidatabase where \
own=");
    IIputdomio((short *)0,1,32,0,owner);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,name);
      IIgetdomio((short *)0,1,32,DU_MAXDBCMPTLVL-1,cmptlvl);
      IIgetdomio((short *)0,1,30,4,&compatminor);
      IIgetdomio((short *)0,1,30,4,&dbservice);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 414 "duvedbdb.sc" */	/* host code */
            /* If this is a RMS Gateway append server class */
            if (dbservice & DU_3SER_RMS)
            {
               STcat(name, "/rms");
            }
	    STzapblank (name,
		    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_db);
	    STzapblank( owner, duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_dba);
	    STcopy( cmptlvl,
		    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_lvl);
	    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_minlvl =
		compatminor;
	    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_dbservice =
	        dbservice;
	    duve_cb->duve_ncnt++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
    }
    else /* must be whole installation */
    {
	/* get number of elements and allocate memory */
/* # line 435 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(name)from iidatabase");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&agg);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 436 "duvedbdb.sc" */	/* host code */
	size = agg * sizeof(tempname);
	duve_cb->duve_list = (DU_DBLIST *) MEreqmem( (u_i4)0, size, TRUE,
						    &mem_stat);
	if ( (mem_stat != OK) || (duve_cb->duve_list == NULL) )
	{
	    duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	    return ( (DU_STATUS) DUVE_BAD);
	}
	/* now enter loop to get names */
/* # line 446 "duvedbdb.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvedbdb2",6231,25178);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"duvedbdb2",6231,25178);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select name, own, dbcmptlvl, dbcmptminor, dbservice from iidatabase");
        IIexDefine(0,(char *)"duvedbdb2",6231,25178);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,owner);
      IIgetdomio((short *)0,1,32,DU_MAXDBCMPTLVL-1,cmptlvl);
      IIgetdomio((short *)0,1,30,4,&compatminor);
      IIgetdomio((short *)0,1,30,4,&dbservice);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 450 "duvedbdb.sc" */	/* host code */
            /* If this is a RMS Gateway append server class */
            if (dbservice & DU_3SER_RMS)
            {
               STcat(name, "/rms");
            }
	    STzapblank (name,
		    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_db);
	    STzapblank( owner, 
		   duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_dba);
	    STcopy( cmptlvl,
		    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_lvl);
	    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_minlvl = 
		    compatminor;
	    duve_cb->duve_list->du_dblist[duve_cb->duve_ncnt].du_dbservice =
		    dbservice;
	    duve_cb->duve_ncnt++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
    }
    /*
    ** now walk list we just created and see if we have and DDBs in list.
    ** if so, get CDB information for that DDB.  If its not a DDB, then the
    ** star infomation in dvue_cb->duvelist->du_dblist is uninitialized.
    */
    for (i=0; i<duve_cb->duve_ncnt; i++) 
    {
	if (duve_cb->duve_list->du_dblist[i].du_dbservice & DU_1SER_DDB)
	{
	    STcopy (duve_cb->duve_list->du_dblist[i].du_db,name);
	    STcopy (duve_cb->duve_list->du_dblist[i].du_dba,owner);
/* # line 481 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdb_name, cdb_dbms from iistar_cdbs where ddb_name=");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_owner=");
    IIputdomio((short *)0,1,32,0,owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DU_STARMAXNAME+1-1,cdb);
      IIgetdomio((short *)0,1,32,DU_STARMAXNAME+1-1,dbmstype);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 484 "duvedbdb.sc" */	/* host code */
	    STzapblank(cdb, duve_cb->duve_list->du_dblist[i].du_cdb);
	    STzapblank(dbmstype, duve_cb->duve_list->du_dblist[i].du_dbmstype);
	}    
    }
    /*
    **	now get list of all locations known to the installation
    */
/* # line 493 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(lname)from iilocations");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&agg);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 494 "duvedbdb.sc" */	/* host code */
    size = agg * sizeof(DU_LOCINFO);
    duve_cb->duve_locs = (DUVE_LOCINFO *) MEreqmem( (u_i4)0, size, TRUE,
						 &mem_stat);
    if ( (mem_stat != OK) || (duve_cb->duve_locs == NULL) )
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
    /* now enter loop to get names */
/* # line 505 "duvedbdb.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvedbdb3",6232,30934);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"duvedbdb3",6232,30934);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select lname, status from iilocations");
        IIexDefine(0,(char *)"duvedbdb3",6232,30934);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,name);
      IIgetdomio((short *)0,1,30,4,&lstat);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 507 "duvedbdb.sc" */	/* host code */
	STzapblank (name, 
	  duve_cb->duve_locs->du_locname[duve_cb->duve_lcnt].duloc.db_loc_name);
	duve_cb->duve_locs->du_locname[duve_cb->duve_lcnt].du_lstat = lstat;
	duve_cb->duve_lcnt++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
    if ( (duve_cb->duve_operation == DUVE_PURGE) ||
	 (duve_cb->duve_operation == DUVE_TEMP) ||
	 (duve_cb->duve_operation == DUVE_EXPIRED) )
    {
	if ( duve_0mk_purgelist( duve_cb) == DUVE_BAD)
	    return ( (DU_STATUS) DUVE_BAD);
    }
    /* if a user was specified via the -u flag, verify that user is known to
    ** the installation. */
    if (duve_cb->duve_user[0] != '\0')
    {
	STcopy (duve_cb->duve_user, owner);
/* # line 526 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select any(name)from iiuser where name=");
    IIputdomio((short *)0,1,32,0,owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&sagg);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 528 "duvedbdb.sc" */	/* host code */
	if (sagg == 0)
	{
	    duve_talk (DUVE_ALWAYS, duve_cb, E_DU3001_USER_NOT_KNOWN, 2,
			0, owner);
	    return ( (DU_STATUS) DUVE_BAD);
	}
    }
    /* verify access user if operation is ACCESSCHECK */
    if ( duve_cb->duve_operation == DUVE_ACCESS)
    {
	if (duve_cb->duve_auser[0] != '\0')
	{
	    STcopy (duve_cb->duve_auser,owner);
/* # line 542 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select any(name)from iiuser where name=");
    IIputdomio((short *)0,1,32,0,owner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&agg);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 544 "duvedbdb.sc" */	/* host code */
	    if (agg == 0)
	    {
		duve_talk (DUVE_ALWAYS, duve_cb, E_DU5037_INVALID_ACCESS_USR, 2,
			    0, owner);
		return ( (DU_STATUS) DUVE_BAD);
	    }
	}
    }
/* # line 553 "duvedbdb.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 556 "duvedbdb.sc" */	/* host code */
    /* indicate we no longer have iidbdb open */
    STcopy("",duve_cb->du_opendb);
    return ( (DU_STATUS) DUVE_YES);
}    
/*{
** Name: ckiidbdb	- run verifydb tests to check iidbdb catalogs
**
** Description:
**      This routine is the executive for VERIFYDB PHASE II (iidbdb catalog
**	checks/repairs).  This routine deallocates database specific caches
**	(from dbms catalogs) to avoid being a memory hog.  It calls each of 
**	the iidbdb catalog check/fix routines.  If any of the routines 
**	terminate with an error status (DUVE_BAD), processing stops and
**	ckiidbdb returns an error status.
**
**	Once all of the routines have executed successfully, it walks the
**	iidatabase cache and deletes any iidatabase tuples that are marked
**	for deletion.  (At some future time, verifydb may be modified to
**	drop the database, rather than just delete tuples from the iidatabase
**	catalog, which is why we wait until now to delete iidatabase tuples.)
**
**	Since the subordinate routines may encounter an error that will
**	result in duve_close being called, duve_close has logic to deallocate
**	the caches built by the subordinate routines (duvedbinfo and duveusr).
**	Since duve_close always deallocates these caches, ckiidbdb does not
**	bother to do so.
**
**	This routine assumes that it is connected to the iidbdb upon entry.
**	(It would be feasible to have duve_catalogs() disconnect from the
**	iidbdb then call this routine.  Then this routine would have to
**	connect to the iidbdb.  When we connect to a server or disconnect
**	from it, it is time consuming (up to 2 minutes on a loaded machine).
**	That is why i have implemented it this way.  If things change in the
**	future, it may be desirable to have this routine do its own connection
**	and remove the lines that deallocate memory caches.
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	iidbdb catalogs may be modified
**	databases, users and/or location definitions may be dropped from
**	  the installation.
**	Characteristics assocaited with a database may change (ex: INGRES
**	  users may lose their access to a private database.)
**
** History:
**      08-feb-89   (teg)
**          initial creation
**      07-dec-93 (andre)
**          duve_cb->duvedpnds is no longer a pointer to an array of
**          structures - instead, it is a structure from which two lists may
**          be hanging
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
**	18-jan-94 (andre)
**	    added code to release memory used for the "fix it" cache
**	06-jun-94 (teresa)
**	    add support to populate role and group cache for bug 60761.
[@history_template@]...
*/
DU_STATUS
ckiidbdb(duve_cb)
DUVE_CB            *duve_cb;
{
  char dbname[DB_MAXNAME+1];
    /* deallocate DBMS catalog caches -- we do not want to be a resource hog */
    if ( duve_cb->duverel != NULL)
    {
	MEfree ( (PTR) duve_cb->duverel );
	duve_cb->duverel = NULL;
    }
    if (duve_cb->duvedpnds.duve_indep != NULL)
    {
        MEfree((PTR) duve_cb->duvedpnds.duve_indep);
        duve_cb->duvedpnds.duve_indep = NULL;
        duve_cb->duvedpnds.duve_indep_cnt = 0;
    }
    if (duve_cb->duvedpnds.duve_dep != NULL)
    {
        MEfree((PTR) duve_cb->duvedpnds.duve_dep);
        duve_cb->duvedpnds.duve_dep = NULL;
        duve_cb->duvedpnds.duve_dep_cnt = 0;
    }
    if (duve_cb->duvefixit.duve_schemas_to_add)
    {
        DUVE_ADD_SCHEMA	*p, *nxt;
        for (p = duve_cb->duvefixit.duve_schemas_to_add; p; p = nxt)
        {
	    nxt = p->duve_next;
	    MEfree((PTR) p);
        }
        duve_cb->duvefixit.duve_schemas_to_add = NULL;
    }
    if (duve_cb->duvefixit.duve_objs_to_drop)
    {
        DUVE_DROP_OBJ	*p, *nxt;
        for (p = duve_cb->duvefixit.duve_objs_to_drop; p; p = nxt)
        {
	    nxt = p->duve_next;
	    MEfree((PTR) p);
        }
        duve_cb->duvefixit.duve_objs_to_drop = NULL;
    }
    if ( duve_cb->duvestat != NULL)
    {
	MEfree ( (PTR) duve_cb->duvestat);
	duve_cb->duvestat = NULL;
    }
    if ( duve_cb->duvetree != NULL)
    {
	MEfree ( (PTR) duve_cb->duvetree);
	duve_cb->duvestat = NULL;
    }
    if (duve_cb->duveint != NULL)
    {
	MEfree ( (PTR) duve_cb->duveint);
	duve_cb->duveint = NULL;
    }
    if (duve_cb->duveeves != NULL)
    {
	MEfree ( (PTR) duve_cb->duveeves);
	duve_cb->duveeves = NULL;
    }
    if (duve_cb->duveprocs != NULL)
    {
	MEfree ( (PTR) duve_cb->duveprocs);
	duve_cb->duveprocs = NULL;
    }
    if (duve_cb->duvesyns != NULL)
    {
	MEfree ( (PTR) duve_cb->duvesyns);
	duve_cb->duvesyns = NULL;
    }
    /* populate role and group cache */
    if ( rolegrp (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    /* now perform iidbdb catalog checks/repairs */
    if ( ckusrprofile (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckuser (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckloc (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckdatabase (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckdbidx (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckextend (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckdbpriv (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckstarcdbs (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckcdbidx (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    if ( ckusrgrp (duve_cb) == DUVE_BAD)
	return ( (DU_STATUS) DUVE_BAD);
    while (--duve_cb->duve_dbcnt >= 0)
    {
	if (duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple)
	{
	    STcopy(duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		   dbname);
/* # line 753 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidatabase where name=");
    IIputdomio((short *)0,1,32,0,dbname);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 754 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiextend where dname=");
    IIputdomio((short *)0,1,32,0,dbname);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 755 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbpriv where dbname=");
    IIputdomio((short *)0,1,32,0,dbname);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 756 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where cdb_name=");
    IIputdomio((short *)0,1,32,0,dbname);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 757 "duvedbdb.sc" */	/* host code */
	}
    }
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckcdbidx - run verifydb tests to check iidbdb catalog iicdbid_idx
**
** Description:
**      This routine opens a cursor to walk iicdbid_idx one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency between the secondary index iicdbid_idx
**	and the table base table iistar_cdbs.
**
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Tuples read from iicdbid_idx.
**	    Tuples may be removed from iicdbid_idx.
**
** History:
**	26-May-93 (teresa)
**	    Initial Creation
**	23-dec-93 (teresa)
**	    verifydb performance enhancement -- replace count(xxx) with any(xxx)
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
[@history_template@]...
*/
DU_STATUS
ckcdbidx (duve_cb)
DUVE_CB            *duve_cb;
{
# include <duvecdbx.h>
  i4 cdb_id, cnt;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);
/* # line 815 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"cdbx_cursor",6233,10836);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iicdbid_idx");
    IIcsQuery((char *)"cdbx_cursor",6233,10836);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 817 "duvedbdb.sc" */	/* host code */
    /* loop for each tuple in cdbx */
    for (;;)  
    {
	/* get tuple from iicdbid_idx */
/* # line 821 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"cdbx_cursor",6233,10836,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(cdb_indx).cdb_id);
      IIcsGetio((short *)0,1,30,4,&(cdb_indx).tidp);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 822 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) 
	{
	    /* no more tuples in iicdbid_idx */
/* # line 825 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"cdbx_cursor",6233,10836);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 826 "duvedbdb.sc" */	/* host code */
	    break;
	}
#if 0
-- this test is bad because the optimizer will use the secondary index in the
-- query of what we are trying to test on the base table.  However, test 2
-- adequately tests this functionality, so use just it.
	/* test 1 -- verify that there is a tuple in iistar_cdbs for this
	**	     secondary index */
/* # line 835 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select any(ddb_name)from iistar_cdbs where cdb_id=");
    IIputdomio((short *)0,1,30,4,&cdb_indx.cdb_id);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 837 "duvedbdb.sc" */	/* host code */
	if (cnt == 0)
	{
	    if (duve_banner( DUVE_IIDBID_IDX, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU167B_NO_IISTARCDBS_ENTRY, 2,
		    sizeof(cdb_indx.cdb_id), &cdb_indx.cdb_id);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU0341_WARN_IICDBID_IDX, 0);
	    /* we take no corrective action, we merely print a warning */
	    continue;
	}  /* endif test 1 failed */
#endif	
	/* test 2 -- assure that TIDP is correct */
/* # line 851 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select cdb_id from iistar_cdbs where tid=");
    IIputdomio((short *)0,1,30,4,&cdb_indx.tidp);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cdb_id);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 853 "duvedbdb.sc" */	/* host code */
	if (cdb_id != cdb_indx.cdb_id)
	{
	    if (duve_banner( DUVE_IIDBID_IDX, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU167B_NO_IISTARCDBS_ENTRY, 2,
		    sizeof(cdb_indx.cdb_id), &cdb_indx.cdb_id);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU0341_WARN_IICDBID_IDX, 0);
	    /* we take no corrective action, we merely print a warning */
	    continue;
	}   /* endif test 2 failed */
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckcdbidx");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckdatabase	- run verifydb tests to check iidbdb catalog iidatabase
**
** Description:
**      This routine opens a cursor to walk iidatabase one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iidatabase system catalog.
**
**	It allocates memory to keep track of all databases known to this
**	installation.
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples read from iiuser
**	    users may be removed from the installation
**
** History:
**      31-jan-89   (teg)
**          initial creation
**      18-aug-89   (teg)
**	    fix bug 7638 - verifydb incorrectly handlee ddb entries in the 
**			   iidatabase(reporting errors when none were present.)
**			   Now it ignores them.
**	    ALSO fix bug 7640 - verifydb did not pass test78 on 7.0 dbs cuz
**			    it did not zap blanks before doing a compare.
**      05-feb-1990  (teg)
**          cache CDB/DDB info before skipping rest of dbdb checks for CDB/DDB.
**      09-nov-92 (jrb)
**	    Changed ING_SORTDIR to ING_WORKDIR and DU_SORTS_LOC to DU_WORK_LOC
**          for ML Sorts Project.
**	07-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	01-jun-93 (teresa)
**	    Added test 15, obsoleted test 8.
**	28-jun-93 (teresa)
**	    explicitely fetch dmpdev into db_tbl, since this particular cursor
**	    requires each column be specified explicitely.
**	18-sep-93 (teresa)
**	    Added checks/repairs for new delimited identifiers semanitics
**	    (tests 16 to 19).
**      17-oct-93 (jrb)
**	    Added support for auxiliary work locations; MLSorts project.
**	18-oct-93 (teresa)
**	    Set DU_EXT_DATA bit.
**	23-dec-93 (teresa)
**	    verifydb performance enhancement - replace count(xxx) with any(xxx)
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
**      06-Jun-94 (teresa)
**          change parameters to duve_usrfind for bug 60761.
[@history_template@]...
*/
DU_STATUS
ckdatabase (duve_cb)
DUVE_CB            *duve_cb;
{
    u_i4		size;
    DB_LOC_NAME		default_loc;
    STATUS		mem_stat;
    DU_STATUS		locidx,usridx ;
# include <duvedb.h>
  u_i4 tid;
  i4 cnt;
  i4 extend_stat;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 958 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"db_cursor",6234,28391);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select name, own, dbdev, ckpdev, jnldev, sortdev, access, dbservice, \
dbcmptlvl, dbcmptminor, db_id, dmpdev, tid from iidatabase order by na\
me");
    IIcsQuery((char *)"db_cursor",6234,28391);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 960 "duvedbdb.sc" */	/* host code */
    /* allocate memory to hold drop commands */
/* # line 961 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(name)from iidatabase");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 962 "duvedbdb.sc" */	/* host code */
    if (sqlca.sqlcode == DU_SQL_NONE|| cnt==0)
    {	
	/* its unreasonable to have an empty iidatabase table */
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU5029_EMPTY_IIDATABASE, 0);
	duve_close (duve_cb, TRUE);
	return ( (DU_STATUS) DUVE_BAD);
    }
    size = cnt * sizeof(DU_DBINFO);
    duve_cb->duvedbinfo = (DUVE_DBINFO *) MEreqmem((u_i4)0, size, TRUE,
						    &mem_stat);
    duve_cb->duve_dbcnt = 0;
    if ( (mem_stat != OK) || (duve_cb->duvedbinfo == NULL) )
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
    /* loop for each tuple in iidatabase */
    for (duve_cb->duve_dbcnt=0;;duve_cb->duve_dbcnt++)  
    {
	/* get tuple from iidatabase */
/* # line 982 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"db_cursor",6234,28391,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,db_tbl.name);
      IIcsGetio((short *)0,1,32,32,db_tbl.own);
      IIcsGetio((short *)0,1,32,32,db_tbl.dbdev);
      IIcsGetio((short *)0,1,32,32,db_tbl.ckpdev);
      IIcsGetio((short *)0,1,32,32,db_tbl.jnldev);
      IIcsGetio((short *)0,1,32,32,db_tbl.sortdev);
      IIcsGetio((short *)0,1,30,4,&db_tbl.access);
      IIcsGetio((short *)0,1,30,4,&db_tbl.dbservice);
      IIcsGetio((short *)0,1,32,4,db_tbl.dbcmptlvl);
      IIcsGetio((short *)0,1,30,4,&db_tbl.dbcmptminor);
      IIcsGetio((short *)0,1,30,4,&db_tbl.db_id);
      IIcsGetio((short *)0,1,32,32,db_tbl.dmpdev);
      IIcsGetio((short *)0,1,30,4,&tid);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 987 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in iiuser */
	{
/* # line 989 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"db_cursor",6234,28391);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 990 "duvedbdb.sc" */	/* host code */
	    break;
	}
	/* cache information about the iidatabase tuple */
	STzapblank( db_tbl.name,
		   duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database);
	STzapblank( db_tbl.own,
		   duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbid = db_tbl.db_id;
	duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_access=db_tbl.access;
	duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_tid = tid;
	/* if this is a DDB entry, just put skip to the next entry */
	if (db_tbl.dbservice & DU_1SER_DDB)
	{
	    continue;
	}
	/* test 1 -- verify database name is valid */
	if (du_chk1_dbname(duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database)
	    != OK)
	{
	    if (duve_banner( DUVE_IIDATABASE, 1, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1656_INVALID_DBNAME, 4, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    if (duve_talk(DUVE_IO,duve_cb,S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	}  /* endif test 1 failed */
	/*
	** Test 2 - verify that the default data location is valid.
	*/
	STzapblank( db_tbl.dbdev, default_loc.db_loc_name);
	locidx = duve_locfind( &default_loc, duve_cb);
	if ( locidx == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIDATABASE, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1657_INVALID_DBDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE, 0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 2a failed. */
	else if ((duve_cb->duve_locs->du_locname[locidx].du_lstat & DU_DBS_LOC) 
		== 0)
	{
	    /* mark the location as an invalid default data location so that
	    ** test 4 does not attempt to put a tuple in iiextend for this
	    ** non-data location
	    */
	    locidx = DU_INVALID;
	    /* location is known to the installation, but is not permitted to
	    ** be a default data location */
	    if (duve_banner( DUVE_IIDATABASE, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1657_INVALID_DBDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE, 0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */	    
	}  /* endif test 2b failed */
	/*
	** Test 3 -- Verify that the database owner is a valid INGRES user
	*/
	usridx = duve_usrfind(
			duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
			duve_cb, DUVE_USER_NAME);
	if ( usridx == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIDATABASE, 3, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1658_INVALID_DBOWN, 4, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 3 failed. */	
	/*
	**  test 4 -- verify that there is a location for this database 
	**	      ( in iiextend table)
	*/
/* # line 1108 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select any(dname)from iiextend where dname=");
    IIputdomio((short *)0,1,32,0,db_tbl.name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1110 "duvedbdb.sc" */	/* host code */
	if ( (cnt == 0) && (locidx != DU_INVALID) )
	{
	    if (duve_banner( DUVE_IIDATABASE, 4, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1659_MISSING_IIEXTEND, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, db_tbl.dbdev);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0331_CREATE_IIEXTEND_ENTRY, 4,
		    0,duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, db_tbl.dbdev)
	    == DUVE_YES)
	    {
		extend_stat = DU_EXT_OPERATIVE | DU_EXT_DATA;
/* # line 1124 "duvedbdb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiextend(lname, dname, status)values(");
    IIputdomio((short *)0,1,32,0,db_tbl.dbdev);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,db_tbl.name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&extend_stat);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1126 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, 
			  S_DU0381_CREATE_IIEXTEND_ENTRY, 4, 0,
			  duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
			  0, db_tbl.dbdev);
	    }  /* endif corrective action taken */
	}  /* endif test 4 failed */
	/*
	** Test 5 -- verify that the checkpoint location is valid
	*/
	STzapblank( db_tbl.ckpdev, default_loc.db_loc_name);
	locidx = duve_locfind( &default_loc, duve_cb);
	if ( locidx == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIDATABASE, 5, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU165A_INVALID_CKPDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 5a failed. */	
	else if ((duve_cb->duve_locs->du_locname[locidx].du_lstat & DU_CKPS_LOC)
		== 0)
	{
	    if (duve_banner( DUVE_IIDATABASE, 5, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU165A_INVALID_CKPDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 5 b failed. */	
	/*
	** Test 6 -- verify that the journal location is valid
	*/
	STzapblank( db_tbl.jnldev, default_loc.db_loc_name);
	locidx = duve_locfind( &default_loc, duve_cb);
	if ( locidx == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIDATABASE, 6, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU165B_INVALID_JNLDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 6a failed. */	
	else if ((duve_cb->duve_locs->du_locname[locidx].du_lstat & DU_JNLS_LOC)
		== 0)
	{
	    if (duve_banner( DUVE_IIDATABASE, 6, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU165B_INVALID_JNLDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 6b failed. */	
	/*
	** Test 7 -- verify that the work location is valid
	*/
	STzapblank( db_tbl.sortdev, default_loc.db_loc_name);
	locidx = duve_locfind( &default_loc, duve_cb);
	if ( locidx == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIDATABASE, 7, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU165C_INVALID_SORTDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 7a failed. */	
	else if ((duve_cb->duve_locs->du_locname[locidx].du_lstat
		  & (DU_WORK_LOC | DU_AWORK_LOC) == 0))
	{
	    if (duve_banner( DUVE_IIDATABASE, 7, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU165C_INVALID_SORTDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple = TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 7b failed. */	
	/* test 8 is currently obsolete. */
	/* 
	** test 9 - verify database is operational.
	*/
	if ((db_tbl.access & DU_OPERATIVE) == 0)
	{
	    if (duve_banner( DUVE_IIDATABASE, 9, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);		
	    /* database is not operative.  See if there is a valid reason*/
	    if (db_tbl.access & (DU_DESTROYDB | DU_CREATEDB) )
	    {
		/* we have a partially created or partically destroyed db.
		** user should run destroydb to remove it from the installation
		** PRINT WARNING.
		*/
		duve_talk(DUVE_MODESENS, duve_cb, S_DU165E_BAD_DB, 4, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    }
	    else if (db_tbl.access & DU_CONVERTING)
	    {
		/* we have an unconverted 5.0 db.  User may want it to stay a
		** 5.0 db, or user may wish to convert it.  We do not know,
		** so we print a warning and let the user decide.
		*/
		duve_talk(DUVE_MODESENS, duve_cb, S_DU165F_UNCONVERTED_DB, 4, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    }
	    else if ( STcompare(db_tbl.dbcmptlvl,DU_0DBV60) < DU_IDENTICAL)
	    {
		/* DB version is not compatible with INGRES 6.x */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU1660_INCOMPATIBLE_DB, 4,0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    }
	    else if (STcompare(db_tbl.dbcmptlvl,DU_CUR_DBCMPTLVL)>DU_IDENTICAL)
	    {
		/* DB version is not compatible with INGRES 6.x */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU1660_INCOMPATIBLE_DB, 4,0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    }	    
	    else
	    {
		/* there is no apparent reason that this DB should not be
		** marked operative.  Ask the user for permission to do so.
		*/
		duve_talk(DUVE_MODESENS, duve_cb, S_DU1661_INOPERATIVE_DB, 4, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	        if (duve_talk(DUVE_IO, duve_cb, S_DU0333_MARK_DB_OPERATIVE,0)
		== DUVE_YES)
		{
		    db_tbl.access |= DU_OPERATIVE;
/* # line 1315 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set access\
=");
    IIputdomio((short *)0,1,30,4,&db_tbl.access);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,db_tbl.name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and own=");
    IIputdomio((short *)0,1,32,0,db_tbl.own);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1317 "duvedbdb.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb,
			      S_DU0383_MARK_DB_OPERATIVE, 0);
		}  /* endif corrective action taken */
	    } /* endif */
	} /* endif test 9 */
	/*
	** Test 10 -- Verify that the major compatibility level is valid 
	*/
	(void) STzapblank(db_tbl.dbcmptlvl, db_tbl.dbcmptlvl);
	if ( (STcompare (db_tbl.dbcmptlvl, DU_0DBV60) < DU_IDENTICAL) ||
	     (STcompare (db_tbl.dbcmptlvl, DU_CUR_DBCMPTLVL) > DU_IDENTICAL) )
	{
	    /* major compatibility level is invalid. */
	    if (duve_banner( DUVE_IIDATABASE, 10, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU1660_INCOMPATIBLE_DB, 4,0,
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	}  /* endif compat level too low */
	/*
	**  Test 11 -- verify that minor compatibility level is valid
	*/
	if (db_tbl.dbcmptminor > DU_1CUR_DBCMPTMINOR)
	{
	    /* minor compatibility level is invalid. */
	    if (duve_banner( DUVE_IIDATABASE, 11, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU1662_INCOMPAT_MINOR, 6,0,
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		sizeof(db_tbl.dbcmptminor), &db_tbl.dbcmptminor);
	}  /* endif compat level too low */
	/*
	**  Test 12 -- verify that database id is valid
	*/
	if (db_tbl.db_id == 0)
	{
	    /* database id is invalid */
	    if (duve_banner( DUVE_IIDATABASE, 12, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU1663_ZERO_DBID, 4,0,
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	}  /* endif db_id invalid */
	/*
	**  Test 13 -- verify that database id is unique
	*/
/* # line 1370 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(db_id)from iidatabase where db_id=");
    IIputdomio((short *)0,1,30,4,&db_tbl.db_id);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1372 "duvedbdb.sc" */	/* host code */
	if (cnt > 1)
	{
	    /* database id is not unique */
	    if (duve_banner( DUVE_IIDATABASE, 13, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU1664_DUPLICATE_DBID, 6,0,
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		sizeof(db_tbl.db_id), &db_tbl.db_id);
	}  /* endif db_id is not unique. */
	/*
	** test 14 -- verify that secondary index (iidbid_idx) entry exists
	**	      for this tuple.
	*/
/* # line 1388 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select any(db_id)from iidbid_idx where db_id=");
    IIputdomio((short *)0,1,30,4,&db_tbl.db_id);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1390 "duvedbdb.sc" */	/* host code */
	if (cnt == 0)
	{
	    /* secondary index is missing */
	    if (duve_banner( DUVE_IIDATABASE, 14, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1665_NO_2NDARY_INDEX, 4, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0334_UPDATE_IIDBID_IDX, 4,
		    sizeof(db_tbl.db_id), &db_tbl.db_id, sizeof(tid), &tid)
	    == DUVE_YES)
	    {
/* # line 1402 "duvedbdb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iidbid_idx(db_id, tidp)values(");
    IIputdomio((short *)0,1,30,4,&db_tbl.db_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1404 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0384_UPDATE_IIDBID_IDX, 4,
		    sizeof(db_tbl.db_id), &db_tbl.db_id, sizeof(tid), &tid);
	    }  /* endif corrective action taken */
	} /* endif secondary index entry is missing */
	/*
	** Test 15 -- verify that the dump location is valid
	*/
	STzapblank( db_tbl.dmpdev, default_loc.db_loc_name);
	locidx = duve_locfind( &default_loc, duve_cb);
	if ( locidx == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIDATABASE, 15, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1687_INVALID_DMPDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple =
									   TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 15a failed. */	
	else if ((duve_cb->duve_locs->du_locname[locidx].du_lstat & DU_DMPS_LOC)
		== 0)
	{
	    if (duve_banner( DUVE_IIDATABASE, 15, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1687_INVALID_DMPDEV, 6, 0,
		    duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown,
		    0, default_loc.db_loc_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0330_DROP_IIDATABASE_TUPLE,0)
	    == DUVE_YES)
	    {
		duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_badtuple =
									   TRUE;
		duve_talk(DUVE_MODESENS, duve_cb,
			  S_DU0380_DROP_IIDATABASE_TUPLE, 0);
		continue;
	    }  /* endif corrective action taken */
	} /* endif test 15 b failed. */	
	/*
	** Test 16 -- verify DU_NAME_UPPER Set Semantics 
	*/
	if (
	     (db_tbl.dbservice & DU_NAME_UPPER)
	     &&
	     ( (db_tbl.dbservice & (DU_DELIM_UPPER | DU_DELIM_MIXED)) ==0)
	   )
	{
	    /* If non-delimited is uppercase, then delimited must be either
	    ** mixed or upper case, but it is not
	    */
	    if (duve_banner( DUVE_IIDATABASE, 16, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16A4_NONDELIM_UPPER_ERR, 4,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0242_SET_DELIM_MIXED, 0)
	    == DUVE_YES)
	    {
		db_tbl.dbservice |= DU_DELIM_MIXED;
/* # line 1475 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set dbserv\
ice=");
    IIputdomio((short *)0,1,30,4,&db_tbl.dbservice);
    IIwritio(0,(short *)0,1,32,0,(char *)"where db_id=");
    IIputdomio((short *)0,1,30,4,&db_tbl.db_id);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1477 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb, S_DU0442_SET_DELIM_MIXED, 0);
	    }  /* endif corrective action taken */
	    continue;
	} /* endif illegal semantics when non-delimited case is upper */
	/*
	** Test 17 -- verify DU_NAME_UPPER NOT Set Semantics 
	*/
	if (
	     (~db_tbl.dbservice & DU_NAME_UPPER)
	     &&
	     (db_tbl.dbservice & DU_DELIM_UPPER)
	   )
	{
	    /* If non-delimited is lowercase, then delimited must not be
	    ** uppercase, but it is.
	    */
	    if (duve_banner( DUVE_IIDATABASE, 17, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16A5_NONDELIM_LOWER_ERR, 4,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0243_CLEAR_DELIM_UPPER, 0)
	    == DUVE_YES)
	    {
		db_tbl.dbservice &= ~DU_DELIM_UPPER;
/* # line 1503 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set dbserv\
ice=");
    IIputdomio((short *)0,1,30,4,&db_tbl.dbservice);
    IIwritio(0,(short *)0,1,32,0,(char *)"where db_id=");
    IIputdomio((short *)0,1,30,4,&db_tbl.db_id);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1505 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb, S_DU0443_CLEAR_DELIM_UPPER, 0);
	    }  /* endif corrective action taken */
	    continue;
	} /* endif illegal semantics when non-delimited case is upper */
	/*
	** Test 18 -- verify DU_NAME_UPPER Set Semantics 
	*/
	if (
	     (db_tbl.dbservice & DU_DELIM_UPPER)
	     &&
	     (db_tbl.dbservice & DU_DELIM_MIXED)
	   )
	{
	    /* Delimited Identifiers may be upper case, mixed case or lower
	    ** case, but they must be only one of the three. Here the DB
	    ** claims that delimited identifiers are both upper and mixed
	    ** case.
	    */
	    if (duve_banner( DUVE_IIDATABASE, 18, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16A4_NONDELIM_UPPER_ERR, 4,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
		0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0243_CLEAR_DELIM_UPPER, 0)
	    == DUVE_YES)
	    {
		db_tbl.dbservice &= ~DU_DELIM_UPPER;
/* # line 1533 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set dbserv\
ice=");
    IIputdomio((short *)0,1,30,4,&db_tbl.dbservice);
    IIwritio(0,(short *)0,1,32,0,(char *)"where db_id=");
    IIputdomio((short *)0,1,30,4,&db_tbl.db_id);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1535 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb, S_DU0443_CLEAR_DELIM_UPPER, 0);
	    }  /* endif corrective action taken */
	    continue;
	} /* endif illegal semantics when non-delimited case is upper */
	/*
	** Test 19 -- verify REAL_USER case Semantics 
	*/
	if ( (
	        (db_tbl.dbservice & DU_REAL_USER_MIXED)
		&&
		(db_tbl.dbservice & DU_DELIM_MIXED)
	     )
	     ||
	     ((db_tbl.dbservice & DU_REAL_USER_MIXED)==0)
	   )
	    /* these are healthy cases, so dont worry about them */
	    continue;
	/* if we get here, then DU_REAL_USER_MIXED is set, but DU_DELIM_MIXED
	** is not set.  There are two possible corrective actions, depending
	** on other semantics defined by dbservice semantics:
	**  If DU_DELIM_USER and DU_NAME_USER are both set (or both not set)
	**  then
	**	clear DU_REAL_USER_MIXED
	**  else
	**	set DU_DELIM_MIXED
	*/
	if (duve_banner( DUVE_IIDATABASE, 19, duve_cb) == DUVE_BAD) 
	    return ( (DU_STATUS) DUVE_BAD);
	duve_talk ( DUVE_MODESENS, duve_cb, S_DU16A7_REALUSER_CASE_ERR, 4,
	    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_database,
	    0, duve_cb->duvedbinfo->du_db[duve_cb->duve_dbcnt].du_dbown);
	if ( ( (db_tbl.dbservice & DU_DELIM_UPPER)
	       &&
	       (db_tbl.dbservice & DU_NAME_UPPER)	    
	     )
	     ||
	     ( ((db_tbl.dbservice & DU_DELIM_UPPER)==0)
	       &&
	       ((db_tbl.dbservice & DU_NAME_UPPER)==0)
	     )
	   )
	{
	    /* Either upper or lower case is specified, so, clear
	    ** DU_REAL_USER_MIXED and let it default to the same as everything
	    ** else.
	    */
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0244_CLEAR_REALUSER_MIXED, 0)
	    == DUVE_YES)
	    {
		db_tbl.dbservice &= ~DU_DELIM_UPPER;
/* # line 1588 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set dbserv\
ice=");
    IIputdomio((short *)0,1,30,4,&db_tbl.dbservice);
    IIwritio(0,(short *)0,1,32,0,(char *)"where db_id=");
    IIputdomio((short *)0,1,30,4,&db_tbl.db_id);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1590 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb,
			  S_DU0444_CLEAR_REALUSER_MIXED, 0);
	    }  /* endif corrective action taken */
	}
	else
	{
	    /* Delimited and non-delimited IDs are not the same case so correct
	    ** to the most permissive name case, which is mixed.
	    */
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0242_SET_DELIM_MIXED, 0)
	    == DUVE_YES)
	    {
		db_tbl.dbservice &= ~DU_DELIM_UPPER;
/* # line 1603 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidatabase set dbserv\
ice=");
    IIputdomio((short *)0,1,30,4,&db_tbl.dbservice);
    IIwritio(0,(short *)0,1,32,0,(char *)"where db_id=");
    IIputdomio((short *)0,1,30,4,&db_tbl.db_id);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1605 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb, S_DU0442_SET_DELIM_MIXED, 0);
	    }  /* endif corrective action taken */
	}
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckdatabase");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckdbidx - run verifydb tests to check iidbdb catalog iidbid_idx
**
** Description:
**      This routine opens a cursor to walk iidbid_idx one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency between the secondary index iidbid_idx
**	and the table base table iidatabase.
**
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Tuples read from iidbid_idx.
**	    Tuples may be removed from iidbid_idx.
**
** History:
**      06-Feb-89   (teg)
**          initial creation
**	07-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
[@history_template@]...
*/
DU_STATUS
ckdbidx (duve_cb)
DUVE_CB            *duve_cb;
{
    DU_STATUS		base,next;
# include <duvedbx.h>
  i4 tid, tidp;
  i4 cnt;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 1670 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"dbx_cursor",6235,10306);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iidbid_idx");
    IIcsQuery((char *)"dbx_cursor",6235,10306);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1672 "duvedbdb.sc" */	/* host code */
    /* loop for each tuple in dbx */
    for (;;)  
    {
	/* get tuple from iidbidx */
/* # line 1676 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"dbx_cursor",6235,10306,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(dbx_tbl).db_id);
      IIcsGetio((short *)0,1,30,4,&(dbx_tbl).tidp);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1677 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in iidbid_idx */
	{
/* # line 1679 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"dbx_cursor",6235,10306);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1680 "duvedbdb.sc" */	/* host code */
	    break;
	}
	/* test 1 -- verify database with this id is known to installation */
	base = 0;
	base = duve_idfind(dbx_tbl.db_id, base, duve_cb);
	if ( base == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIDBID_IDX, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1666_NO_IIDATABASE_ENTRY, 2,
		    sizeof(dbx_tbl.db_id), &dbx_tbl.db_id);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0335_DROP_IIDBIDIDX, 0)
	    == DUVE_YES)
	    {
/* # line 1695 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbid_idx where db_id=");
    IIputdomio((short *)0,1,30,4,&dbx_tbl.db_id);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1696 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0385_DROP_IIDBIDIDX,0);
	    }  /* endif corrective action taken */
	    continue;
	}  /* endif test 1 failed */
	/* verify that the base table tuple is not already marked for deletion.
	** if it is marked for deletion, there is no point in continuing tests
	** on the secondary index tuple, as it will be deleted when the base
	** table is deleted.
	*/
	if (duve_cb->duvedbinfo->du_db[base].du_badtuple != 0)
	    continue;
	/* test 2 -- assure that TIDP is correct */
	if (duve_cb->duvedbinfo->du_db[base].du_tid != dbx_tbl.tidp)
	{
	    /* just incase there are duplicate dbid's in the cache, search
	    ** the rest of the list. */
	    next = base;
	    for (;;)
	    {
	    	next = duve_idfind(dbx_tbl.db_id, ++next, duve_cb);
		if ( (next == DU_INVALID) ||
		     (duve_cb->duvedbinfo->du_db[base].du_tid == dbx_tbl.tidp) )
		    break;
	    }
	    if (next == DU_INVALID)
	    {
		/* we did not find an entry in iidatabase cache that matched
		** db_id and tidp.  So, update this iidatabase entry to have
		** the correct tid.
		*/
		tid = duve_cb->duvedbinfo->du_db[base].du_tid;
		if (duve_banner( DUVE_IIDBID_IDX, 2, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk (DUVE_MODESENS, duve_cb, S_DU1667_WRONG_IIDBIDIDX_TID,
		        8, 0, duve_cb->duvedbinfo->du_db[base].du_database,
			0, duve_cb->duvedbinfo->du_db[base].du_dbown, 
			sizeof(tid), &tid, sizeof(dbx_tbl.tidp), &dbx_tbl.tidp);
		if ( duve_talk(DUVE_ASK, duve_cb, S_DU0336_UPDATE_IIDBIDIDX,
			    2, sizeof(tid), &tid)
		== DUVE_YES)
		{
/* # line 1741 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidbid_idx set tidp=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"where db_id=");
    IIputdomio((short *)0,1,30,4,&dbx_tbl.db_id);
    IIwritio(0,(short *)0,1,32,0,(char *)"and tidp=");
    IIputdomio((short *)0,1,30,4,&dbx_tbl.tidp);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1743 "duvedbdb.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU0386_UPDATE_IIDBIDIDX,
			    2, sizeof(tid), &tid);
		}  /* endif corrective action taken */
	    }   /* endif there was not another valid iidatabase tuple for this
		** db_id */
	}   /* endif test 2 failed */
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckdbidx");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckdbpriv - run verifydb tests to check iidbpriv catalog
**
** Description:
**      This routine opens a cursor to walk iidbpriv one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iidbpriv table wtih other
**	database database catalogs and with itself.
**
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Tuples read from iidatabase.
**	    Tuples read from iiuser.
**	    Tuples may be removed from iidbpriv.
**
** History:
**      02-may-93 (teresa)
**	    Initial creation.
**	28-jun-93 (teresa)
**	    iidbpriv has column dbname, not name.  Fixed test 3 accordingly.
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
**      06-Jun-94 (teresa)
**          fix bug 60761.
**      02-nov-94 (sarjo01) Bug 60179
**          Check for blank dbname before possibly generating 168A error
**          (no such database). Blank dbname indicates GRANT ON CURRENT
**          INSTALLATION (no dbname given)
[@history_template@]...
*/
DU_STATUS
ckdbpriv(duve_cb)
DUVE_CB            *duve_cb;
{
    DU_STATUS		name_idx,db_idx;
# include <duvepriv.h>
  i4 flags;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 1819 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"dbpriv_cursor",6236,18136);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iidbpriv");
    IIcsQuery((char *)"dbpriv_cursor",6236,18136);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1821 "duvedbdb.sc" */	/* host code */
    /* loop for each tuple in iidbpriv */
    for (;;)  
    {
	/* get tuple from iidbidx */
/* # line 1825 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"dbpriv_cursor",6236,18136,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(priv_tbl).dbname);
      IIcsGetio((short *)0,1,32,32,(priv_tbl).grantee);
      IIcsGetio((short *)0,1,30,2,&(priv_tbl).gtype);
      IIcsGetio((short *)0,1,30,2,&(priv_tbl).dbflags);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).control);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).flags);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).qrow_limit);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).qdio_limit);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).qcpu_limit);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).qpage_limit);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).qcost_limit);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).idle_time_limit);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).connect_time_limit);
      IIcsGetio((short *)0,1,30,4,&(priv_tbl).priority_limit);
      IIcsGetio((short *)0,1,32,32,(priv_tbl).reserve);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1826 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in iidbpriv */
	{
/* # line 1828 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"dbpriv_cursor",6236,18136);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1829 "duvedbdb.sc" */	/* host code */
	    break;
	}
        /* if dbname is not there, the GRANT was ON CURRENT INSTALLATION */
        if (priv_tbl.dbname[0] != ' ')
        {
	/* Test 1 - verify that the database is known to the installation. */
	   db_idx = duve_dbfind( priv_tbl.dbname, duve_cb);
	   if (db_idx == DU_INVALID)
	   {
	    /* database does not exist (or is being dropped by verifydb) */
	        if (duve_banner( DUVE_IIDBPRIV, 1, duve_cb) == DUVE_BAD) 
	       return ( (DU_STATUS) DUVE_BAD);
	     duve_talk ( DUVE_MODESENS, duve_cb, S_DU168A_NOSUCH_DB, 4,
		        0, priv_tbl.grantee, 0, priv_tbl.dbname);
	     if ( duve_talk(DUVE_ASK, duve_cb, S_DU0349_DROP_FROM_IIDBPRIV, 0)
	          == DUVE_YES)
	     {
/* # line 1846 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iidbpriv",(char *)"dbpriv_cursor",6236,18136);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1847 "duvedbdb.sc" */	/* host code */
	        duve_talk(DUVE_MODESENS, duve_cb,S_DU0399_DROP_FROM_IIDBPRIV,0);
	     }  /* endif corrective action taken */
	     continue;
	 }     /* endif test 1 failed */
      }
	/* test 2 - verify the user is a valid INGRES user. */
	name_idx = duve_usrfind (priv_tbl.grantee, duve_cb,
			DUVE_USER_NAME | DUVE_GROUP_NAME | DUVE_ROLE_NAME );
	if (name_idx == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIDBPRIV, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU168B_NOSUCH_USER, 4,
		    0, priv_tbl.grantee, 0, priv_tbl.dbname);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0349_DROP_FROM_IIDBPRIV, 0)
	    == DUVE_YES)
	    {
/* # line 1866 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iidbpriv",(char *)"dbpriv_cursor",6236,18136);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1867 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb,S_DU0399_DROP_FROM_IIDBPRIV,0);
	    }  /* endif corrective action taken */
	    continue;
	}  /* endif test 2 failed */
	/* TEST 3 -- verify that only valid privileges are turned on */
	if ( ~priv_tbl.control & priv_tbl.flags )
	{
	    /* Some bits were set in flags that were not defined in control.
	    ** clear them
	    */
	    if (duve_banner( DUVE_IIDBPRIV, 3, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU168C_INVALID_PRIVS, 4,
		    0, priv_tbl.grantee, 0, priv_tbl.dbname);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU034A_CLEAR_INVALID_PRIV, 4,
		    0, priv_tbl.grantee, 0, priv_tbl.dbname)
	    == DUVE_YES)
	    {
		flags = priv_tbl.flags & priv_tbl.control;
/* # line 1888 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iidbpriv set flags=");
    IIputdomio((short *)0,1,30,4,&flags);
    IIwritio(0,(short *)0,1,32,0,(char *)"where dbname=");
    IIputdomio((short *)0,1,32,0,priv_tbl.dbname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and grantee=");
    IIputdomio((short *)0,1,32,0,priv_tbl.grantee);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1891 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU039A_CLEAR_INVALID_PRIV,4,
			  0, priv_tbl.grantee, 0, priv_tbl.dbname);
	    }  /* endif corrective action taken */
	}   /* endif test 3 failed */
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckdbpriv");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckextend - run verifydb tests to check iidbdb catalog iiextend
**
** Description:
**      This routine opens a cursor to walk iiextend one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iiextend table wtih other
**	database database catalogs.
**
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Tuples read from iiextend.
**	    Tuples may be removed from iiextend.
**
** History:
**      08-Feb-89   (teg)
**          initial creation
**	07-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
[@history_template@]...
*/
DU_STATUS
ckextend (duve_cb)
DUVE_CB            *duve_cb;
{
    u_i4		len;
    DB_LOC_NAME		loc_name;
# include <duveext.h>
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 1958 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"ext_cursor",6237,9328);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iiextend");
    IIcsQuery((char *)"ext_cursor",6237,9328);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1960 "duvedbdb.sc" */	/* host code */
    /* loop for each tuple in iiextend */
    for (;;)  
    {
	/* get tuple from iiextend */
/* # line 1964 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"ext_cursor",6237,9328,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(ext_tbl).lname);
      IIcsGetio((short *)0,1,32,32,(ext_tbl).dname);
      IIcsGetio((short *)0,1,30,4,&(ext_tbl).status);
      IIcsGetio((short *)0,1,30,4,&(ext_tbl).rawstart);
      IIcsGetio((short *)0,1,30,4,&(ext_tbl).rawblocks);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1965 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in iiextend */
	{
/* # line 1967 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"ext_cursor",6237,9328);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1968 "duvedbdb.sc" */	/* host code */
	    break;
	}
	/* test 1 - verify the database exists. */
	if ( duve_dbfind(ext_tbl.dname, duve_cb) == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIEXTEND, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU166D_NOSUCH_DB, 4,
		    0, ext_tbl.dname, 0, ext_tbl.lname);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU033B_DROP_IIEXTEND, 0)
	    == DUVE_YES)
	    {
/* # line 1981 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiextend",(char *)"ext_cursor",6237,9328);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1982 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU038B_DROP_IIEXTEND, 0);
		continue;
	    }  /* endif corrective action taken */
	}  /* endif test 1 failed */
	/* test 2 - verify the location exists. */
	len = STzapblank(ext_tbl.lname, loc_name.db_loc_name);
	if ( duve_locfind(&loc_name, duve_cb) == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIEXTEND, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU166E_NOSUCH_LOC, 4,
		    0, ext_tbl.dname, 0, ext_tbl.lname);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU033B_DROP_IIEXTEND, 0)
	    == DUVE_YES)
	    {
/* # line 1999 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiextend",(char *)"ext_cursor",6237,9328);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2000 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU038B_DROP_IIEXTEND, 0);
		continue;
	    }  /* endif corrective action taken */
	}  /* endif test 2 failed */
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckextend");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckloc - run verifydb tests to check iidbdb catalog iilocations
**
** Description:
**      This routine opens a cursor to walk iilocations one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iilocations table wtih other
**	database database catalogs and with itself.
**
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Tuples read from iilocations.
**	    Tuples may be removed from iilocations.
**
** History:
**      07-Feb-89   (teg)
**          initial creation
**      18-aug-89 (teg)
**	    fix bug 7548 (add check for DU_DMPS_LOC to test 89)
**      09-nov-92 (jrb)
**	    Changed ING_SORTDIR to ING_WORKDIR and DU_SORTS_LOC to DU_WORK_LOC
**          for ML Sorts Project.
**	07-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**      17-oct-93 (jrb)
**	    Added support for auxiliary work locations; MLSorts project.
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
[@history_template@]...
*/
DU_STATUS
ckloc (duve_cb)
DUVE_CB            *duve_cb;
{
    DU_STATUS	locidx;
    DB_LOC_NAME loc;
    u_i4	len;
    i4		mask;
# include <duveloc.h>
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2076 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"loc_cursor",6238,18237);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iilocations");
    IIcsQuery((char *)"loc_cursor",6238,18237);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2078 "duvedbdb.sc" */	/* host code */
    /* loop for each tuple in iilocations */
    for (;;)  
    {
	/* get tuple from iilocations */
/* # line 2082 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"loc_cursor",6238,18237,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(loc_tbl).status);
      IIcsGetio((short *)0,1,32,32,(loc_tbl).lname);
      IIcsGetio((short *)0,1,32,128,(loc_tbl).area);
      IIcsGetio((short *)0,1,32,8,(loc_tbl).free);
      IIcsGetio((short *)0,1,30,4,&(loc_tbl).rawpct);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2083 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in iilocations*/
	{
/* # line 2085 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"loc_cursor",6238,18237);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2086 "duvedbdb.sc" */	/* host code */
	    break;
	}
	len = STtrmwhite(loc_tbl.area);
	/* test 1 - verify the location name is valid. */
	if ( cui_chk3_locname(loc_tbl.lname) != OK)
	{
	    if (duve_banner( DUVE_IILOCATIONS, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU166B_INVALID_LOCNAME, 4,
		    0, loc_tbl.lname, 0, loc_tbl.area);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU0339_DROP_LOCATION, 2,
			   0, loc_tbl.area)
	    == DUVE_YES)
	    {
/* # line 2103 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iilocations where lname=");
    IIputdomio((short *)0,1,32,0,loc_tbl.lname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and area=");
    IIputdomio((short *)0,1,32,0,loc_tbl.area);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2104 "duvedbdb.sc" */	/* host code */
		STcopy (loc_tbl.lname,loc.db_loc_name);
		locidx = duve_locfind( &loc, duve_cb);
		if (locidx != DU_INVALID)
		   duve_cb->duve_locs->du_locname[locidx].du_lockill=DUVE_BAD;
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0389_DROP_LOCATION, 2,
			   0, loc_tbl.area);
		continue;
	    }  /* endif corrective action taken */
	}  /* endif test 1 failed */
	/* test 2 - verify there are not any trash bits in the status field
	*/
	mask = DU_DBS_LOC | DU_WORK_LOC | DU_AWORK_LOC | DU_JNLS_LOC |
	       DU_CKPS_LOC | DU_DMPS_LOC | DU_GLOB_LOCS;
	if ( loc_tbl.status & ~mask)
	{
	    if (duve_banner( DUVE_IILOCATIONS, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU166C_BAD_LOC_STAT, 4,
		    0, loc_tbl.lname, sizeof(loc_tbl.status), &loc_tbl.status);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU033A_FIX_LOC_STAT, 0)
	    == DUVE_YES)
	    {
		loc_tbl.status = mask;
/* # line 2129 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iilocations set statu\
s=");
    IIputdomio((short *)0,1,30,4,&loc_tbl.status);
    IIwritio(0,(short *)0,1,32,0,(char *)"where lname=");
    IIputdomio((short *)0,1,32,0,loc_tbl.lname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and area=");
    IIputdomio((short *)0,1,32,0,loc_tbl.area);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2131 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU038A_FIX_LOC_STAT, 0);
	    }  /* endif corrective action taken */
	}   /* endif test 2 failed */
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckloc");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckstarcdbs - run verifydb tests to check iidbdb catalog iistar_cdbs
**
** Description:
**      This routine opens a cursor to walk iistar_cdbs one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iistar_cdbs table wtih other
**	database database catalogs and with itself.
**
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Tuples read from iistar_cdbs.
**	    Tuples may be removed from iistar_cdbs.
**
** History:
**	26-May-93 (teresa)
**	    Initial Creation
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
[@history_template@]...
*/
ckstarcdbs (duve_cb)
DUVE_CB            *duve_cb;
{
    DU_STATUS		cdb_idx;
    DU_STATUS		ddb_idx;
# include <duvecdbs.h>
  char newname[DB_MAXNAME+1];
  i4 newval;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2196 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"cdb_cursor",6239,9445);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iistar_cdbs");
    IIcsQuery((char *)"cdb_cursor",6239,9445);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2198 "duvedbdb.sc" */	/* host code */
    /* loop for each tuple in iistar_cdbs */
    for (;;)  
    {
	/* get tuple from catalog */
/* # line 2203 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"cdb_cursor",6239,9445,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(cdbs_tbl).ddb_name);
      IIcsGetio((short *)0,1,32,32,(cdbs_tbl).ddb_owner);
      IIcsGetio((short *)0,1,32,32,(cdbs_tbl).cdb_name);
      IIcsGetio((short *)0,1,32,32,(cdbs_tbl).cdb_owner);
      IIcsGetio((short *)0,1,32,32,(cdbs_tbl).cdb_node);
      IIcsGetio((short *)0,1,32,32,(cdbs_tbl).cdb_dbms);
      IIcsGetio((short *)0,1,32,32,(cdbs_tbl).scheme_desc);
      IIcsGetio((short *)0,1,32,25,(cdbs_tbl).create_date);
      IIcsGetio((short *)0,1,32,8,(cdbs_tbl).original);
      IIcsGetio((short *)0,1,30,4,&(cdbs_tbl).cdb_id);
      IIcsGetio((short *)0,1,30,4,&(cdbs_tbl).cdb_capability);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2204 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in catalog*/
	{
/* # line 2206 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"cdb_cursor",6239,9445);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2207 "duvedbdb.sc" */	/* host code */
	    break;
	}
	/* test 1 - verify the ddb is known to the installation */
	ddb_idx = duve_dbfind( cdbs_tbl.ddb_name, duve_cb);
	if (ddb_idx == DU_INVALID)
	{
	    /* database does not exist (or is being dropped by verifydb) */
	    if (duve_banner( DUVE_IISTAR_CDBS, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1676_NOSUCH_DDB, 2,
		    0, cdbs_tbl.ddb_name);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU033F_REMOVE_STARCDBS, 4,
			   0, cdbs_tbl.ddb_name, 0, cdbs_tbl.cdb_name)
	    == DUVE_YES)
	    {
/* # line 2225 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where cdb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.cdb_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.ddb_name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2227 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU038F_REMOVE_STARCDBS, 4,
			   0, cdbs_tbl.ddb_name, 0, cdbs_tbl.cdb_name);
	    }  /* endif corrective action taken */
	    continue;
	}   /* endif test 1 failed */
	/* test 2 - verify the cdb is known to the installation */
	cdb_idx = duve_dbfind( cdbs_tbl.cdb_name, duve_cb);
	if (cdb_idx == DU_INVALID)
	{
	    /* database does not exist (or is being dropped by verifydb) */
	    if (duve_banner( DUVE_IISTAR_CDBS, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1677_NOSUCH_CDB, 2,
		    0, cdbs_tbl.cdb_name);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU033F_REMOVE_STARCDBS, 4,
			   0, cdbs_tbl.ddb_name, 0, cdbs_tbl.cdb_name)
	    == DUVE_YES)
	    {
/* # line 2248 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistar_cdbs where cdb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.cdb_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.ddb_name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2250 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU038F_REMOVE_STARCDBS, 4,
			   0, cdbs_tbl.ddb_name, 0, cdbs_tbl.cdb_name);
	    }  /* endif corrective action taken */
	    continue;
	}   /* endif test 2 failed */	
	STtrmwhite(cdbs_tbl.ddb_owner);
	STtrmwhite(cdbs_tbl.cdb_owner);
	/* test 3 - verify the owner name for the DDB is correct. */
	if (STcompare(duve_cb->duvedbinfo->du_db[ddb_idx].du_dbown,
			cdbs_tbl.ddb_owner) != DU_IDENTICAL)
	{
	    if (duve_banner( DUVE_IISTAR_CDBS, 3, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1678_WRONG_DDB_OWNER, 6,
		    0, cdbs_tbl.ddb_name,
		    0, duve_cb->duvedbinfo->du_db[ddb_idx].du_dbown,
		    0, cdbs_tbl.ddb_owner);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0340_UPDATE_STARCDBS, 0)
	    == DUVE_YES)
	    {
		STcopy (duve_cb->duvedbinfo->du_db[ddb_idx].du_dbown, newname);
/* # line 2274 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iistar_cdbs set ddb_owner=");
    IIputdomio((short *)0,1,32,0,newname);
    IIwritio(0,(short *)0,1,32,0,(char *)"where cdb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.cdb_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.ddb_name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2277 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0390_UPDATE_STARCDBS, 0);
	    }  /* endif corrective action taken */
	}  /* endif test 3 failed */
	/* test 4 - verify the owner name for the CDB is correct. */
	if (STcompare(duve_cb->duvedbinfo->du_db[cdb_idx].du_dbown,
			cdbs_tbl.cdb_owner) != DU_IDENTICAL)
	{
	    if (duve_banner( DUVE_IISTAR_CDBS, 4, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1679_WRONG_CDB_OWNER, 6,
		    0, cdbs_tbl.cdb_name,
		    0, duve_cb->duvedbinfo->du_db[cdb_idx].du_dbown,
		    0, cdbs_tbl.cdb_owner);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0340_UPDATE_STARCDBS, 0)
	    == DUVE_YES)
	    {
		STcopy (duve_cb->duvedbinfo->du_db[cdb_idx].du_dbown, newname);
/* # line 2297 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iistar_cdbs set cdb_owner=");
    IIputdomio((short *)0,1,32,0,newname);
    IIwritio(0,(short *)0,1,32,0,(char *)"where cdb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.cdb_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.ddb_name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2300 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0390_UPDATE_STARCDBS, 0);
	    }  /* endif corrective action taken */
	}  /* endif test 4 failed */
	/* test 5 - verify that the CDB_ID is correct */
	if (duve_cb->duvedbinfo->du_db[cdb_idx].du_dbid != cdbs_tbl.cdb_id)
	{
	    if (duve_banner( DUVE_IISTAR_CDBS, 5, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb,  S_DU167A_WRONG_CDB_ID, 6,
		    0, cdbs_tbl.cdb_name,
		    sizeof(duve_cb->duvedbinfo->du_db[cdb_idx].du_dbid),
		    &duve_cb->duvedbinfo->du_db[cdb_idx].du_dbid,
		    sizeof(cdbs_tbl.cdb_id), &cdbs_tbl.cdb_id);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0340_UPDATE_STARCDBS, 0)
	    == DUVE_YES)
	    {
		newval = duve_cb->duvedbinfo->du_db[cdb_idx].du_dbid;
/* # line 2320 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iistar_cdbs set cdb_i\
d=");
    IIputdomio((short *)0,1,30,4,&newval);
    IIwritio(0,(short *)0,1,32,0,(char *)"where cdb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.cdb_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ddb_name=");
    IIputdomio((short *)0,1,32,0,cdbs_tbl.ddb_name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2323 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0390_UPDATE_STARCDBS, 0);
	    }  /* endif corrective action taken */
	}  /* endif test 5 failed */
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckstarcdbs");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckuser	- run verifydb tests to check iidbdb catalog iiuser
**
** Description:
**      This routine opens a cursor to walk iiuser one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iiuser system catalog.
**
**	It allocates memory to keep track of all users known to this
**	installation.
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples read from iiuser
**	    users may be removed from the installation
**
** History:
**      31-jan-89   (teg)
**          initial creation
**	07-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	01-jun-93 (teresa)
**	    Add test 3
**	28-jun-93 (teresa)
**	    add the following user flags to test 2: DU_UOPERATOR, DU_UAUDIT,
**	    DU_USYSADMIN, DU_UDOWNGRADE, DU_UPRIV.
**	15-oct-93 (teresa)
**	    Remove test 1 since it blows away user names specified with a
**	    delimited identifier.
**      23-dec-93 (teresa)
**          verifydb performance enhancement -- replace count(xxx) with any(xxx)
**	20-dec-93 (robf)
**          Check for valid profile.
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
**	1-mar-94 (robf)
**          Use DU_UALL_PRIVS/DU_UAUDIT_PRIVS masks rather than 
**	    listing individual privileges. (one less thing to maintain
**	    when we add a new privilege)(
**	14-jul-94 (robf)
**          Allow for DU_UUPSYSCAT. This isn't used in 6.5 but may be still
**          present in 6.4 and lower converted databases.
[@history_template@]...
*/
DU_STATUS
ckuser (duve_cb)
DUVE_CB            *duve_cb;
{
    u_i4		size;
    STATUS		mem_stat;
    i4			valid_bits;
# include <duveuser.h>
  i4 cnt;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2411 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"user_cursor",6240,11587);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iiuser order by name");
    IIcsQuery((char *)"user_cursor",6240,11587);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2413 "duvedbdb.sc" */	/* host code */
    /* allocate memory to hold drop commands */
/* # line 2414 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(name)from iiuse\
r");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2415 "duvedbdb.sc" */	/* host code */
    if (sqlca.sqlcode == DU_SQL_NONE || cnt == 0)
    {	
	/* its unreasonable to have an empty iiuser table */
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU5026_EMPTY_IIUSER, 0);
	duve_close (duve_cb, TRUE);
	return ( (DU_STATUS) DUVE_BAD);
    }
    size = cnt * sizeof(DU_USR);
    duve_cb->duveusr = (DU_USRLST*) MEreqmem((u_i4)0, size, TRUE, &mem_stat);
    duve_cb->duve_ucnt = 0;
    if ( (mem_stat != OK) || (duve_cb->duveusr == NULL) )
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
    /* loop for each tuple in user */
    for (;;)  
    {
	/* get tuple from iiuser */
/* # line 2434 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"user_cursor",6240,11587,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(user_tbl).name);
      IIcsGetio((short *)0,1,30,2,&(user_tbl).gid);
      IIcsGetio((short *)0,1,30,2,&(user_tbl).mid);
      IIcsGetio((short *)0,1,30,4,&(user_tbl).status);
      IIcsGetio((short *)0,1,32,32,(user_tbl).default_group);
      IIcsGetio((short *)0,1,32,24,(user_tbl).password);
      IIcsGetio((short *)0,1,32,8,(user_tbl).reserve);
      IIcsGetio((short *)0,1,32,32,(user_tbl).profile_name);
      IIcsGetio((short *)0,1,30,4,&(user_tbl).default_priv);
      IIcsGetio((short *)0,1,32,25,(user_tbl).expire_date);
      IIcsGetio((short *)0,1,30,4,&(user_tbl).flags_mask);
      IIcsGetio((short *)0,1,30,4,&(user_tbl).user_priv);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2435 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in iiuser */
	{
/* # line 2437 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"user_cursor",6240,11587);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2438 "duvedbdb.sc" */	/* host code */
	    break;
	}
	/* test 1 - verify user name syntax is valid */
	size = STtrmwhite( user_tbl.name );
	if ( du_chk2_usrname(user_tbl.name) != OK)
	{
#if 0
	    if (duve_banner( DUVE_IIUSER, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1654_INVALID_USERNAME, 2,
		    0, user_tbl.name);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU032E_DROP_USER, 2,
		    0, user_tbl.name)
	    == DUVE_YES)
	    {
/* # line 2454 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from iiuser where nam\
e=");
    IIputdomio((short *)0,1,32,0,user_tbl.name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2455 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU037E_DROP_USER, 2,
		    0, user_tbl.name);
		continue;
	    }  /* endif corrective action taken */
#endif
	}  /* endif test 1 failed */
	/* test 2 - verify user status is valid */
	valid_bits = (DU_UALL_PRIVS | DU_UAUDIT_PRIVS | DU_UUPSYSCAT);
	if (user_tbl.status & ~valid_bits)
	{
	    if (duve_banner( DUVE_IIUSER, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1655_INVALID_USERSTAT, 2,
		    0, user_tbl.name);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU032F_CLEAR_BADBITS, 2,
		    0, user_tbl.name)
	    == DUVE_YES)
	    {
		user_tbl.status &= valid_bits;
/* # line 2477 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiuser set status=");
    IIputdomio((short *)0,1,30,4,&user_tbl.status);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,user_tbl.name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2479 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU037F_CLEAR_BADBITS, 2,
		    0, user_tbl.name);
	    }  /* end if corrective action is taken */
	}  /* endif test 2 failed */
	/* test 3 - verify that the default group (if specified) is valid */
	if (STcompare(user_tbl.default_group, DB_ABSENT_NAME))
	{
/* # line 2487 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select any(groupid)from iiusergroup where groupid=");
    IIputdomio((short *)0,1,32,0,user_tbl.default_group);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2489 "duvedbdb.sc" */	/* host code */
	    if (cnt == 0)		
	    {
		if (duve_banner( DUVE_IIUSER, 3, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk ( DUVE_MODESENS, duve_cb,
			    S_DU1688_INVL_DEFAULT_GRP, 4,
			    0, user_tbl.name, 0, user_tbl.default_group);
		if ( duve_talk(DUVE_ASK, duve_cb, S_DU0347_CLEAR_DEFLT_GRP, 2,
			       0, user_tbl.name)
		== DUVE_YES)
		{
		    STcopy(DB_ABSENT_NAME, user_tbl.default_group);
/* # line 2501 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiuser set default_gr\
oup=");
    IIputdomio((short *)0,1,32,0,user_tbl.default_group);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,user_tbl.name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2504 "duvedbdb.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU0397_CLEAR_DEFLT_GRP,
			      2, 0, user_tbl.name);
		}  /* end if corrective action is taken */
	    }  /* endif test 3 failed */
	} /* endif a user group was specified */
	/*
	** test 5: Check for valid profile, if not empty
	*/
	if (STcompare(user_tbl.profile_name, DB_ABSENT_NAME))
	{
/* # line 2514 "duvedbdb.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvedbdb4",6241,15576);
      IIputdomio((short *)0,1,32,0,user_tbl.profile_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvedbdb4",6241,15576);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiprofile where name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,user_tbl.profile_name);
        IIexDefine(0,(char *)"duvedbdb4",6241,15576);
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
      Clean_Up();
  }
/* # line 2518 "duvedbdb.sc" */	/* host code */
	    if (cnt == 0)		
	    {
		if (duve_banner( DUVE_IIUSER, 3, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk ( DUVE_MODESENS, duve_cb,
			    S_DU16B4_INVLD_USER_PROFILE, 4,
			    0, user_tbl.name, 0, user_tbl.profile_name);
		if ( duve_talk(DUVE_ASK, duve_cb, S_DU16B5_USER_PROF_RESET, 2,
			       0, user_tbl.name)
			== DUVE_YES)
		{
		    STcopy(DB_ABSENT_NAME, user_tbl.profile_name);
/* # line 2530 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiuser set profile_na\
me=");
    IIputdomio((short *)0,1,32,0,user_tbl.profile_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,user_tbl.name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2533 "duvedbdb.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU16AA_RESET_DEFLT_PROF,
			      2, 0, user_tbl.name);
		}  /* end if corrective action is taken */
	    }  /* endif test 5 failed */
	}
	/* put user information on cache so other tests may use it. */
	STcopy (user_tbl.name,
		duve_cb->duveusr->du_usrlist[duve_cb->duve_ucnt].du_user);
	duve_cb->duveusr->du_usrlist[duve_cb->duve_ucnt++].du_usrstat =
								user_tbl.status;
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckuser");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckusrgrp - run verifydb tests to check iiusergroup catalog
**
** Description:
**      This routine opens a cursor to walk iiusergrp one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iiusergrp table wtih other
**	database database catalogs and with itself.
**
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Tuples read from iiuser.
**	    Tuples may be removed from iiusergroup.
**
** History:
**      01-may-93 (teresa)
**	    Initial creation.
**	23-dec-93 (teresa)
**	    performance enchancement for verifydb - performance statistics
**	    reporting.
**      06-Jun-94 (teresa)
**          change parameters to duve_usrfind for bug 60761.
[@history_template@]...
*/
DU_STATUS
ckusrgrp(duve_cb)
DUVE_CB            *duve_cb;
{
    DU_STATUS		name_idx,db_idx;
# include <duveugrp.h>
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2603 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"ugrp_cursor",6242,7091);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iiusergroup");
    IIcsQuery((char *)"ugrp_cursor",6242,7091);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2605 "duvedbdb.sc" */	/* host code */
    /* loop for each tuple in iiusergroup */
    for (;;)  
    {
	/* get tuple from iiusergroup */
/* # line 2609 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"ugrp_cursor",6242,7091,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(ugrp_tbl).groupid);
      IIcsGetio((short *)0,1,32,32,(ugrp_tbl).groupmem);
      IIcsGetio((short *)0,1,32,32,(ugrp_tbl).reserve);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2610 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in iiusergroup */
	{
/* # line 2612 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"ugrp_cursor",6242,7091);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2613 "duvedbdb.sc" */	/* host code */
	    break;
	}
	/* test 1 - verify the user is a valid INGRES user. */
	if (STcompare(ugrp_tbl.groupmem, DB_ABSENT_NAME))
	{
	    name_idx = duve_usrfind(ugrp_tbl.groupmem, duve_cb, DUVE_USER_NAME);
	    if (name_idx == DU_INVALID)
	    {
		if (duve_banner( DUVE_IIUSERGROUP, 1, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk ( DUVE_MODESENS, duve_cb,
			    S_DU1689_NOSUCH_USER, 4,
			    0, ugrp_tbl.groupmem, 0, ugrp_tbl.groupid);
		if ( duve_talk(DUVE_ASK, duve_cb, S_DU0348_DROP_USR_FROM_GRP,0)
		== DUVE_YES)
		{
/* # line 2631 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiusergroup",(char *)"ugrp_cursor",6242,7091);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2632 "duvedbdb.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb,
			      S_DU0398_DROP_USR_FROM_GRP, 0);
		    continue;
		}  /* endif corrective action taken */
	    }  /* endif test 1 failed */
	} /* endif this tuple specifies a user name */
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckusrgrp");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: rolegrp - cache  role and group names known to this installation
**
** Description:
**
**      This routine builds cache entries for all role and usergroups known
**	to the installation in alphabetical order.
**
**	This is necessary because there are some operations which can be
**	performed on behalf of roles or groups as well as for individual users,
**	such as granting query_io_limit on a database to a role.
**
**	At present, this only effects test 2 of routine ckdbpriv() and could
**	be handled by simplying having that test scan iirole and iiusergroup
**	catalogs for the desired role/group name.  But there is a good
**	chance that this problem (needing to know about usergroup and roles as
**	well as about individual users) will expand with subsequent development.
**	Therefore, I have chosen to take a general and 	expandable solution
**	(cache role and usergroup names).  This routine and an expansion to
**	utility routine duve_usrfind() [module duveutil.c] provide the
**	infra-structure for subsequent  growth.  This routine caches role and
**	usergroup in the DUVE_CB as follows:
**	    DUVE_CB.duvegrp[] -- array of user group names known to installation
**	    DUVE_CB.duve_gcnt -- count of number of group names in duvegrp array
**	    DUVE_CB.duverole[]-- array of role names known to installation
**	    DUVE_CB.duve_rcnt -- count of number of role names in duverole array
**
**	Another routine [ckuser()] caches individual ingres user names in:
**	    DUVE_CB.duveuser[]-- array of structures describing each ingres user
**				 name known to the installation
**	    DUVE_CB.duve_ucnt -- count of number of user names in duveuser array
**
**	Routine duve_usrfind() will search the user, role or group caches based
**	on the value of parameter "searchspace", which can have any of the
**	following bitmasks set:
**		DUVE_USER_NAME		- search user name cache
**		DUVE_GROUP_NAME		- search group name cache
**		DUVE_ROLE_NAME		- search role name cache
**
**	This may seem like overkill for bug 60761, but provides a good
**	foundation for similar situations which are expected to arise.
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Tuples read from iirole and iiusergroup.
**
** History:
**      07-jun-94 (teresa)
**	    Initial creation for bug 60761.
[@history_template@]...
*/
DU_STATUS
rolegrp(duve_cb)
DUVE_CB            *duve_cb;
{
    u_i4		size;
    STATUS		mem_stat;
    i4			valid_bits;
  char grp_role_name[DB_MAXNAME+1];
  i4 cnt;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2726 "duvedbdb.sc" */	/* host code */
    /* allocate memory to hold cache info on installation roles */
    duve_cb->duve_rcnt = 0;
/* # line 2728 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(distinct(roleid))from iirole");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2729 "duvedbdb.sc" */	/* host code */
    if (sqlca.sqlcode != DU_SQL_NONE && cnt != 0)
    {
	/* there were roles defined in this installation, so cache them */
	size = cnt * sizeof(DU_ROLE);
	duve_cb->duverole = (DU_ROLELST*)MEreqmem((u_i4)0,size,TRUE,&mem_stat);
	if ( (mem_stat != OK) || (duve_cb->duverole == NULL) )
	{
	    duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	    return ( (DU_STATUS) DUVE_BAD);
	}
	/* loop for each tuple in iirole */
/* # line 2740 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct(roleid)from iirole order by roleid");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,grp_role_name);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 2743 "duvedbdb.sc" */	/* host code */
	    size = STtrmwhite( grp_role_name );
	    /* put user information on cache so other tests may use it. */
	    STcopy (grp_role_name,
		duve_cb->duverole->du_rolelist[duve_cb->duve_rcnt++].du_role);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
    } /* endif there were roles defined for this installation */
    /* allocate memory to hold cache info on installation usergroups */
    duve_cb->duve_gcnt = 0;
/* # line 2752 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(distinct(groupid))from iiusergroup");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2753 "duvedbdb.sc" */	/* host code */
    if (sqlca.sqlcode != DU_SQL_NONE && cnt != 0)
    {
	/* there were user groups defined in this installation, so cache them */
	size = cnt * sizeof(DU_GRP);
	duve_cb->duvegrp = (DU_GRPLST*)MEreqmem((u_i4)0,size,TRUE,&mem_stat);
	if ( (mem_stat != OK) || (duve_cb->duvegrp == NULL) )
	{
	    duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	    return ( (DU_STATUS) DUVE_BAD);
	}
	/* loop for each tuple in iiusergroup */
/* # line 2764 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct(groupid)from iiusergroup order by groupid");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,grp_role_name);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 2767 "duvedbdb.sc" */	/* host code */
	    size = STtrmwhite( grp_role_name );
	    /* put user information on cache so other tests may use it. */
	    STcopy (grp_role_name,
	       duve_cb->duvegrp->du_grplist[duve_cb->duve_gcnt++].du_usergroup);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
    } /* endif there were user groups defined for this installation */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "cache group/role");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckusrprofile - run verifydb tests to check iidbdb catalog iiprofile
**
** Description:
**      This routine opens a cursor to walk iiprofile one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iiprofile system catalog.
**
**	It allocates memory to keep track of all profiles known to this
**	installation.
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples read from iiprofiles
**	    profiles may be removed from the installation
**
** History:
**	20-dec-93 (robf)
**          Created
[@history_template@]...
*/
DU_STATUS
ckusrprofile (duve_cb)
DUVE_CB            *duve_cb;
{
    u_i4		size;
    STATUS		mem_stat;
    i4			valid_bits;
    bool		got_default_profile=FALSE;
# include <duveprof.h>
  i4 cnt;
/* # line 2829 "duvedbdb.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"profile_cursor",6243,9946);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iiprofile order by name");
    IIcsQuery((char *)"profile_cursor",6243,9946);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2831 "duvedbdb.sc" */	/* host code */
    /* allocate memory to hold drop commands */
/* # line 2832 "duvedbdb.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(name)from iipro\
file");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2833 "duvedbdb.sc" */	/* host code */
    if (sqlca.sqlcode == DU_SQL_NONE || cnt<1)
    {	
	/*
	** Should always have at least one profile, the default profile
	*/
	cnt=1;
    }  
    size = cnt * sizeof(DU_PROF);
    duve_cb->duveprof = (DU_PROFLST*) MEreqmem((u_i4)0, size, TRUE, &mem_stat);
    duve_cb->duve_profcnt = 0;
    if ( (mem_stat != OK) || (duve_cb->duveprof == NULL) )
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
    /* loop for each tuple in iiprofile */
    for (;;)  
    {
	/* get tuple from iiprofile */
/* # line 2852 "duvedbdb.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"profile_cursor",6243,9946,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(uprof_tbl).name);
      IIcsGetio((short *)0,1,30,4,&(uprof_tbl).status);
      IIcsGetio((short *)0,1,32,32,(uprof_tbl).default_group);
      IIcsGetio((short *)0,1,32,8,(uprof_tbl).reserve);
      IIcsGetio((short *)0,1,30,4,&(uprof_tbl).default_priv);
      IIcsGetio((short *)0,1,32,25,(uprof_tbl).expire_date);
      IIcsGetio((short *)0,1,30,4,&(uprof_tbl).flags_mask);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2853 "duvedbdb.sc" */	/* host code */
	if (sqlca.sqlcode == DU_SQL_NONE) /* then no more tuples in iiuser */
	{
/* # line 2855 "duvedbdb.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"profile_cursor",6243,9946);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2856 "duvedbdb.sc" */	/* host code */
	    break;
	}
	/* test 1 - verify profile name syntax is valid */
	size = STtrmwhite( uprof_tbl.name );
	if(size==0)
	{
		/*
		** Got default profile
		*/
		got_default_profile=TRUE;
	}
	else if ( du_chk2_usrname(uprof_tbl.name) != OK)
	{
	    if (duve_banner( DUVE_IIPROFILE, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16AB_INVALID_PROFNAME, 2,
		    0, uprof_tbl.name);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU16AC_DROP_PROFILE, 2,
		    0, uprof_tbl.name)
		    == DUVE_YES)
	    {
/* # line 2878 "duvedbdb.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprofile where name=");
    IIputdomio((short *)0,1,32,0,uprof_tbl.name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2879 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU16AD_PROFILE_DROPPED, 2,
		    0, uprof_tbl.name);
		continue;
	    }  /* endif corrective action taken */
	}  /* endif test 1 failed */
	/* test 2 - verify profile status is valid */
        valid_bits = DU_UCREATEDB | DU_UDRCTUPDT | DU_UUPSYSCAT | DU_UTRACE |
		     DU_UQRYMODOFF | DU_USYSADMIN | DU_UOPERATOR | DU_USECURITY
		     | DU_UINSERTUP | DU_UINSERTDOWN | DU_UWRITEDOWN |
		     DU_UWRITEFIXED | DU_UWRITEUP | DU_UAUDITOR |
		     DU_UALTER_AUDIT | DU_UDOWNGRADE |
		     DU_UMONITOR | DU_UMAINTAIN_USER |
		     DU_UAUDIT
		     ;
	if (uprof_tbl.status & ~valid_bits)
	{
	    if (duve_banner( DUVE_IIPROFILE, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16AE_INVALID_PROFSTAT, 2,
		    0, uprof_tbl.name);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU16AF_CLEAR_PROFILE_BITS, 2,
		    0, uprof_tbl.name)
	    == DUVE_YES)
	    {
		uprof_tbl.status &= valid_bits;
/* # line 2906 "duvedbdb.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprofile set status=");
    IIputdomio((short *)0,1,30,4,&uprof_tbl.status);
    IIwritio(0,(short *)0,1,32,0,(char *)"where name=");
    IIputdomio((short *)0,1,32,0,uprof_tbl.name);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2908 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU16B0_PROFILE_BITS_CLEARED, 2,
		    0, uprof_tbl.name);
	    }  /* end if corrective action is taken */
	}  /* endif test 2 failed */
	/* put profile information on cache so other tests may use it. */
	STcopy (uprof_tbl.name,
		duve_cb->duveprof->du_proflist[duve_cb->duve_profcnt].du_profile);
	duve_cb->duveprof->du_proflist[duve_cb->duve_profcnt++].du_profstat =
								uprof_tbl.status;
    } /* end of loop for each tuple */
    if(!got_default_profile)
    {
        if (duve_banner( DUVE_IIPROFILE, 3, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
    	duve_talk ( DUVE_MODESENS, duve_cb, S_DU16B1_NO_DEFAULT_PROFILE, 0);
        if ( duve_talk(DUVE_ASK, duve_cb, S_DU16B2_ADD_DEFAULT_PROF, 0)
         	== DUVE_YES)
        {
/* # line 2927 "duvedbdb.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iiprofile(name, status, flags_mask)values('', 0, 0)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2930 "duvedbdb.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU16B3_DEFAULT_PROF_ADDED, 0);
	}
    }
    return ( (DU_STATUS) DUVE_YES);
}
