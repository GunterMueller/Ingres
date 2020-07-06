# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include	<compat.h>
# include	<pc.h>		/* 6-x_PC_80x86 */
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ui.h>
# include	<uigdata.h>
# include	<dictutil.h>
static VOID	err_check();
static VOID	so_clean_up();
/**
** Name:	main.c -	Main Routine
**
** Description:
**	main		The main program.
**
** History:
**	18-jan-1993 (jpk)
**		Written.
**		To upgrade from 1.3 to 1.4 is simple: three queries.
**		They were written by davel.
**	8-Sept-1993 (jpk)
**		add support for new ii_reports column.
**	17-Mar-1997 (kitch01)
**		Bug 80734. Upgradefe performed on a remote database fails when
**		run from a client install. This is because we try to connect to
**		the local iidbdb, rather than the remote one, to find out if 
**		the database we are upgrading is a star database. I amended the
**		code to connect to the remote idbdb using the vnode passed as 
**		part of the remote database name.
**	9/24/93 (dkh) - Added "with default" for column replevel in ii_reports.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB to replace
**	    its static libraries.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB to replace
**	    its static libraries.
**/
/*{
** Name:	main() -	Main Entry Point for apdev1, v3-->4 upgrade.
**
** Description:
**	Performs upgrade of APDEV1 from version 3 to version 4.
**
** Side Effects:
**	Updates system catalogs.
**
** History:
**	18-jan-1993 (jpk) written.
**	 8-Sept-1993 (jpk) added new ii_reports column rptlevel
**	15-Sept-1993 (jpk) make that replevel, not rptlevel
**	9/24/93 (dkh) - Added "with default" for column replevel in ii_reports.
**      19-jun-95 (lawst01)
**        if upgrading DDB connect to CDB to do table modifications
**	17-Mar-1997 (kitch01)
**		Bug 80734. Upgradefe performed on a remote database fails when
**		run from a client install. This is because we try to connect to
**		the local iidbdb, rather than the remote one, to find out if
**		the database we are upgrading is a star database. I amended the
**		code to connect to the remote iidbdb using the vnode passed as
**		part of the remote database name.
**	23-dec-99 (hayke02)
**	    Check for a non-null chp before calling STbcompare() in order
**	    to prevent a SIGSEGV. The preceding call to STindex() will return
**	    null if the database name does not contain a '/' eg. ad104uin
**	    b99825. This change fixes bug 99825.
**      25-aug-2000 (chash01) 08-jun-99 (chash01)
**        Do not confuse gateway (i.e dbname/RMS) database with STAR database
**        by determine if extension is /STAR.
*/
/*
**	MKMFIN Hints
PROGRAM =	ad104uin
NEEDLIBS =	DICTUTILLIB SHQLIB COMPATLIB SHEMBEDLIB
UNDEFS =	II_copyright
*/
i4
main(argc, argv)
i4	argc;
char	**argv;
{
char	*largv[10], *chp = NULL;
int i;
char	*colon_ptr;
char	*vnode_ptr;
char	vnode[DB_MAXNAME+1];
  char dbname[DB_MAXNAME+1];
  i4 dbservice;
  char iidbdbname[DB_MAXNAME+1];
        for (i=0; i< argc; i++)
        {
           largv[i] = argv[i];
        }
        chp = STindex(argv[1], ERx("/"), 0);
        largv[3] = ERx("-u'$ingres'");
        /*
        ** chash01 (08-jum-99) if "/" exists, then we need to know
        **   what follows it, if not STAR, we set chp back to NULL
        */
	if (chp)
	{
	    if ( STbcompare("STAR", 4, chp+1, 4, TRUE) )
		chp = NULL;
	}
	STcopy ((char *)argv[1], dbname );
	if (((colon_ptr = STindex(dbname, ":", 0)) != NULL) &&
	STindex(colon_ptr + 1, ":", 0) == (colon_ptr + 1))
	{
		/* Having found there is a vnode copy it */
		STlcopy(dbname, vnode, colon_ptr - dbname);
		vnode[colon_ptr-dbname] = '\0';
		/* The dbname as passed is a quoted string. We need
		** to remove the leading quote from the vnode, if there
		** is one.
		*/
		if ((vnode_ptr = STindex(vnode, "'", 1)) != NULL)
		{
			STcopy(vnode_ptr+1, vnode);
		}
	}
	else
	   vnode[0] = '\0';
	/* IF we got control from upgradedb, the '/star' is not passed
	   therefore, we must connect to the iidbdb and determine if
	   this database is a DDB.                                     */
	if (chp == NULL && (STcompare (argv[1], ERx("iidbdb")) ))
	{
/* # line 160 "apdev14u.sc" */	/* host code */
	  /* If there is a vnode we are trying to upgrade a remote db,
	  ** So connect to the remote DBs iidbdb to see if this is a 
	  ** star database
	  */
	  if (vnode[0] != '\0')
	     STprintf(iidbdbname, "%s::iidbdb", vnode);
	  else
	     STcopy("iidbdb", iidbdbname);
/* # line 168 "apdev14u.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,iidbdbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 169 "apdev14u.sc" */	/* host code */
	  err_check();
/* # line 170 "apdev14u.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbservice from iidatabase where name=");
    IIputdomio((short *)0,1,32,0,dbname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dbservice);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 172 "apdev14u.sc" */	/* host code */
          err_check();
/* # line 173 "apdev14u.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 174 "apdev14u.sc" */	/* host code */
	  if (dbservice & 1)  /* 1 = ddb  2 = cdb */
	     chp = argv[1]; 
	}
	if (IIDDdbcDataBaseConnect(argc,&largv) != OK)
	{
		PCexit(FAIL);
	}
/* # line 183 "apdev14u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 184 "apdev14u.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set autocommit off");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 185 "apdev14u.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=table, readlock=exclusive");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 187 "apdev14u.sc" */	/* host code */
	iiuicw1_CatWriteOn();
/* # line 189 "apdev14u.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects a from ii_objects o set retlength=length(a.abf_s\
ource), rettype='char(' +ascii(length(a.abf_source)) +')' where o.obje\
ct_id=a.object_id and o.object_class=2120 and a.abf_version<3 and a.re\
tadf_type=20");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 204 "apdev14u.sc" */	/* host code */
	err_check();
/* # line 206 "apdev14u.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects a from ii_objects o set retlength=2 +length(a.ab\
f_source), rettype='varchar(' +ascii(2 +length(a.abf_source)) +')' whe\
re o.object_id=a.object_id and o.object_class=2120 and a.abf_version<3\
 and a.retadf_type=21");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 221 "apdev14u.sc" */	/* host code */
	err_check();
/* # line 223 "apdev14u.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects a from ii_objects o set abf_version=3 where o.ob\
ject_id=a.object_id and o.object_class=2120 and a.abf_version<3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 235 "apdev14u.sc" */	/* host code */
	err_check();
        if (chp != NULL)
	{ 
/* # line 239 "apdev14u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 240 "apdev14u.sc" */	/* host code */
        if (IIDDccCdbConnect() != OK)
	{
	   err_check();
        }
	}
	/* usual ersatz ALTER TABLE to add a column */
/* # line 246 "apdev14u.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iitmp_ii_reports as select * from ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 247 "apdev14u.sc" */	/* host code */
	err_check();
/* # line 248 "apdev14u.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 249 "apdev14u.sc" */	/* host code */
	err_check();
/* # line 250 "apdev14u.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_reports(object_id integer not null, reptype char(1)no\
t null, replevel integer not null with default, repacount smallint not\
 null, repscount smallint not null, repqcount smallint not null, repbc\
ount smallint not null)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 260 "apdev14u.sc" */	/* host code */
	err_check();
/* # line 261 "apdev14u.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_reports(object_id, reptype, replevel, repacount, repsc\
ount, repqcount, repbcount)select object_id, reptype, 0, repacount, re\
pscount, repqcount, repbcount from iitmp_ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 268 "apdev14u.sc" */	/* host code */
	err_check();
/* # line 269 "apdev14u.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iitmp_ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 270 "apdev14u.sc" */	/* host code */
	err_check();
        if (chp != NULL)
	{
	/*  commit changes to the CDB */
/* # line 275 "apdev14u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 277 "apdev14u.sc" */	/* host code */
	/*  disconnect from the CDB  */
	IIDDcd2CdbDisconnect2();
        /* we are now re-connected to the DDB session  */
/* # line 281 "apdev14u.sc" */	/* remove table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove table ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 282 "apdev14u.sc" */	/* host code */
	err_check();
/* # line 283 "apdev14u.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table ii_reports as link");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 284 "apdev14u.sc" */	/* host code */
	err_check();
	}
	so_clean_up(OK);
}
static VOID
err_check()
{
	STATUS stat = FEinqerr();
	if (stat == OK)
	{
/* # line 297 "apdev14u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 298 "apdev14u.sc" */	/* host code */
	}
	else
	{
/* # line 301 "apdev14u.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 302 "apdev14u.sc" */	/* host code */
		so_clean_up(FAIL);
	}
}
static VOID
so_clean_up(stat)
STATUS	stat;
{
/* # line 310 "apdev14u.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=page, readlock=shared");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 312 "apdev14u.sc" */	/* host code */
	iiuicw0_CatWriteOff();
	PCexit(stat);
}
