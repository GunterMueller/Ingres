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
**	17-aug-2004 (lazro01)
**	    Written.
**	    To upgrade from 1.4 to 1.5.
**	    Changed repacount in ii_reports and rcosequence in ii_rcommands to
**	    i4 to handle values greater than 32K. 
**	3-Dec-2004 (schka24)
**	    Fix typo (ii_commands->ii_rcommands) that prevented Star databases
**	    from upgrading properly.
**      06-Jan-2008 (hanal04) Bug 119874
**          Make changes to allow spoof catalogs in gateway DBs to be
**          upgraded. If we have a gateway do not try to connect to iidbdb
**          to confirm whether this is a star connection (there won't be a DBMS
**          server), or try to update ii_reports (which will not exist), or
**          issue MODIFY syntax that is not supported in the gateways.
**/
/*{
** Name:	main() -	Main Entry Point for apdev1, v4-->5 upgrade.
**
** Description:
**	Performs upgrade of APDEV1 from version 4 to version 5.
**
** Side Effects:
**	Updates system catalogs.
**
** History:
**	17-aug-2004 (lazro01) written.
*/
/*
**	MKMFIN Hints
PROGRAM =	ad105uin
NEEDLIBS =	DICTUTILLIB UILIB LIBQSYSLIB LIBQLIB UGLIB LIBQGCALIB FMTLIB \
		SQLCALIB AFELIB GCFLIB ADFLIB CUFLIB COMPATLIB
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
bool    maybe_star = TRUE;
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
            {
		chp = NULL;
                maybe_star = FALSE;
            }
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
	if (chp == NULL && maybe_star && (STcompare (argv[1], ERx("iidbdb")) ))
	{
/* # line 134 "apdev15u.sc" */	/* host code */
	  /* If there is a vnode we are trying to upgrade a remote db,
	  ** So connect to the remote DBs iidbdb to see if this is a 
	  ** star database
	  */
	  if (vnode[0] != '\0')
	     STprintf(iidbdbname, "%s::iidbdb", vnode);
	  else
	     STcopy("iidbdb", iidbdbname);
/* # line 142 "apdev15u.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,iidbdbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 143 "apdev15u.sc" */	/* host code */
	  err_check();
/* # line 144 "apdev15u.sc" */	/* select */
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
/* # line 146 "apdev15u.sc" */	/* host code */
          err_check();
/* # line 147 "apdev15u.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 148 "apdev15u.sc" */	/* host code */
	  if (dbservice & 1)  /* 1 = ddb  2 = cdb */
	     chp = argv[1]; 
	}
	if (IIDDdbcDataBaseConnect(argc,&largv) != OK)
	{
		PCexit(FAIL);
	}
/* # line 157 "apdev15u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 158 "apdev15u.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set autocommit off");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 159 "apdev15u.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=table, readlock=exclusive");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 161 "apdev15u.sc" */	/* host code */
	iiuicw1_CatWriteOn();
        if (chp != NULL)
	{ 
/* # line 165 "apdev15u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 166 "apdev15u.sc" */	/* host code */
        if (IIDDccCdbConnect() != OK)
	{
	   err_check();
        }
	}
	/* Alter table ii_reports to modify the datatype of repacount to i4.*/
        /* ii_reports won't exist if this is a non-dbms gateway */
        if(IIDDigIsGateway() == FALSE)
        {
/* # line 177 "apdev15u.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iitmp_ii_reports as select * from ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 178 "apdev15u.sc" */	/* host code */
	    err_check();
/* # line 179 "apdev15u.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 180 "apdev15u.sc" */	/* host code */
	    err_check();
/* # line 181 "apdev15u.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_reports(object_id integer not null, reptype char(1)no\
t null, replevel integer not null with default, repacount integer not \
null, repscount smallint not null, repqcount smallint not null, repbco\
unt smallint not null)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 191 "apdev15u.sc" */	/* host code */
	    err_check();
/* # line 192 "apdev15u.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_reports(object_id, reptype, replevel, repacount, repsc\
ount, repqcount, repbcount)select object_id, reptype, replevel, repaco\
unt, repscount, repqcount, repbcount from iitmp_ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 199 "apdev15u.sc" */	/* host code */
	    err_check();
/* # line 200 "apdev15u.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ii_reports to BTREE unique on object_id");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 202 "apdev15u.sc" */	/* host code */
	    err_check();
/* # line 203 "apdev15u.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iitmp_ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 204 "apdev15u.sc" */	/* host code */
	    err_check();
/* # line 205 "apdev15u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 206 "apdev15u.sc" */	/* host code */
        }
	/*
	** Alter table ii_rcommands to modify the datatype of rcosequence to
	** i4.
	*/
/* # line 212 "apdev15u.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table iitmp_ii_rcommands as select * from ii_rcommands");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 214 "apdev15u.sc" */	/* host code */
	err_check();
/* # line 215 "apdev15u.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table ii_rcommands");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 216 "apdev15u.sc" */	/* host code */
	err_check();
/* # line 217 "apdev15u.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_rcommands(object_id integer not null, rcotype char(2)\
not null, rcosequence integer not null, rcosection varchar(12)not null\
, rcoattid varchar(32)not null, rcocommand varchar(12)not null, rcotex\
t varchar(100)not null)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 227 "apdev15u.sc" */	/* host code */
	err_check();
/* # line 228 "apdev15u.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_rcommands(object_id, rcotype, rcosequence, rcosection,\
 rcoattid, rcocommand, rcotext)select object_id, rcotype, rcosequence,\
 rcosection, rcoattid, rcocommand, rcotext from iitmp_ii_rcommands");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 235 "apdev15u.sc" */	/* host code */
	err_check();
        /* Gateway won't support the modify syntax */
        if(IIDDigIsGateway() == FALSE)
        {
/* # line 240 "apdev15u.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ii_rcommands to BTREE unique on object_id, rcotype, rcosequenc\
e with COMPRESSION=(DATA)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 243 "apdev15u.sc" */	/* host code */
	    err_check();
        }
/* # line 245 "apdev15u.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table iitmp_ii_rcommand\
s");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 246 "apdev15u.sc" */	/* host code */
	err_check();
/* # line 247 "apdev15u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 249 "apdev15u.sc" */	/* host code */
        if (chp != NULL)
	{
	/*  commit changes to the CDB */
/* # line 252 "apdev15u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 254 "apdev15u.sc" */	/* host code */
	/*  disconnect from the CDB  */
	IIDDcd2CdbDisconnect2();
        /* we are now re-connected to the DDB session  */
/* # line 258 "apdev15u.sc" */	/* remove table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove table ii_reports");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 259 "apdev15u.sc" */	/* host code */
	err_check();
/* # line 260 "apdev15u.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table ii_reports as link");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 261 "apdev15u.sc" */	/* host code */
	err_check();
/* # line 262 "apdev15u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 263 "apdev15u.sc" */	/* remove table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"remove table ii_rcommands");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 264 "apdev15u.sc" */	/* host code */
	err_check();
/* # line 265 "apdev15u.sc" */	/* register table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"register table ii_rcommands as link");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 266 "apdev15u.sc" */	/* host code */
	err_check();
/* # line 267 "apdev15u.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 268 "apdev15u.sc" */	/* host code */
	}
	so_clean_up(OK);
}
static VOID
err_check()
{
	STATUS stat = FEinqerr();
	if (stat != OK)
	{
/* # line 280 "apdev15u.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 281 "apdev15u.sc" */	/* host code */
		so_clean_up(FAIL);
	}
}
static VOID
so_clean_up(stat)
STATUS	stat;
{
/* # line 289 "apdev15u.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where level=page, readlock=shared");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 291 "apdev15u.sc" */	/* host code */
	iiuicw0_CatWriteOff();
	PCexit(stat);
}
