# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  ROLL_DRIVER	    - Sync up fast commit sessions before crash.
**
**	This program is used to sync up the fast commit slaves before they
**	crash servers so that they all get to ready point before any of them
**	crash.
**
**	The fast commit servers will request an exclusive lock using the
**	DMF trace point DM1420 before crashing.  This program will hold
**	this lock until it sees that all the sessions have reached the crash
**	point.
**
**	Sessions indicate they have reached the crash point by writing copy
**	copy files out to the roll_data directory.  These copy files must
**	be cleaned out previous to each fast commit test run.
**
**	Inputs:
**	    dbname	- database to use.
**	    base_num	- test driver identifier
**	    num_slaves	- number of fast commit suites running.
**
**	Also, the logical ROLL_DATA must be defined to this process.
**
**      History:
**
**      DD-MMM-YYY     Unknown
**              Created Multi-Server Fast Commit test.
**      01-Mar-1991     Jeromef
**              Modified Multi-Server Fast Commit test to include ckpdb and
**              rollforwarddb.
**      21-Nov-1991     Jeromef
**              Add rollforward/msfc test to piccolo library
**      20-Jan-1994 (huffman)
**              Correct include files (xxcl.h) should look at the
**              common version (xx.h) for compiling.
**      17-feb-1994 (donj)
**              Get rid of "trigraph replaced warning"
**
**
NEEDLIBS = LIBINGRES
**
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
#include        <compat.h>
#include        <tr.h>
#include        <pc.h>
#include        <st.h>
main(argc, argv)
int	    argc;
char	    *argv[];
{
  char *dbname;
  int base_num = -1;
  int num_slaves = -1;
  int slaves_done;
  char copy_file[40];
  char command_line[200];
    char	table_name[32];
    int		roll_error();
    int		i;
    CL_SYS_ERR	syserr;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &syserr);
#endif
#ifdef UNIX
    TRset_file(TR_T_OPEN, "stdio", 5, &syserr);
#endif
    if (argc < 4)
    {
	roll_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    dbname = argv[1];
    CVal(argv[2], &base_num);
    CVal(argv[3], &num_slaves);
    if (base_num < 0 || num_slaves <= 0)
    {
	roll_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    /* IIseterr(roll_error); */
/* # line 96 "rolldrv.sc" */	/* host code */
    /*
    ** Establish two sessions - one for holding the sync lock and
    ** the other for checking for the completion of the tests.
    */
/* # line 100 "rolldrv.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(1);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 101 "rolldrv.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(2);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 103 "rolldrv.sc" */	/* host code */
    /*
    ** Get sync lock in session #1 - this will prevent any test slaves
    ** from hitting the crash point until the driver says its OK.
    */
/* # line 107 "rolldrv.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 108 "rolldrv.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set trace point DM1420");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 110 "rolldrv.sc" */	/* host code */
    /*
    ** Switch to session #2 and create the sync table.
    ** This will start the slave processes going.
    */
/* # line 114 "rolldrv.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
  }
/* # line 115 "rolldrv.sc" */	/* host code */
    STprintf(table_name, "roll_sync_tab_%d", base_num);
    STprintf(command_line, "create table %s (%s, %s) with duplicates",
	table_name, 
	"base i4 not null with default", 
	"slaves i4 not null with default");
/* # line 120 "rolldrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 121 "rolldrv.sc" */	/* host code */
    STprintf(command_line, "insert into %s (base, slaves) values (%d, %d)",
	table_name, base_num, num_slaves);
/* # line 123 "rolldrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 124 "rolldrv.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 127 "rolldrv.sc" */	/* host code */
    /*
    ** Wait for slaves to all get to point where they are ready for
    ** the servers to crash.
    **
    ** The slaves indicate they are ready by writing information to
    ** copy files.
    */
    STprintf(table_name, "roll_slave_tab_%d", base_num);
    STprintf(command_line, "create table %s (%s) with duplicates",
	table_name, 
	"a c50 not null");
/* # line 139 "rolldrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 140 "rolldrv.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 142 "rolldrv.sc" */	/* host code */
    /*
    ** Get information from SI files created by slave processes when
    ** they are ready for server crash.  When all slaves have written
    ** to file, then release sync lock to allow slaves to execute server
    ** crashes.
    */
    slaves_done = 0;
    while (slaves_done < num_slaves)
    {
	PCsleep(10000);
    	STprintf(command_line, "modify %s to truncated", table_name);
/* # line 153 "rolldrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 155 "rolldrv.sc" */	/* host code */
	for (i = 1; i <= num_slaves; i++)
	{
	    /* Build filename based on base and slave number */
	    STprintf(copy_file, "ROLL_DATA:roll_copy_%d_%d.in",
		base_num, i);
    	    STprintf(command_line, "copy table %s (a=c0nl) from '%s'", 
		table_name, copy_file);
/* # line 162 "rolldrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 163 "rolldrv.sc" */	/* host code */
	}
	STprintf(command_line, "select count(distinct a) from %s",
		table_name);
/* # line 168 "rolldrv.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c",7798,31748);
    IIwritio(1,(short *)0,1,32,0,command_line);
    IIcsQuery((char *)"c",7798,31748);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 169 "rolldrv.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c",7798,31748,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&slaves_done);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 170 "rolldrv.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c",7798,31748);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 171 "rolldrv.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 172 "rolldrv.sc" */	/* host code */
    }
    /*
    ** Switch back to session 1 to commit the transaction holding
    ** the sync lock.
    */
/* # line 178 "rolldrv.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 179 "rolldrv.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 180 "rolldrv.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(1);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 182 "rolldrv.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
  }
/* # line 183 "rolldrv.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(2);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      roll_error();
  }
/* # line 185 "rolldrv.sc" */	/* host code */
    PCexit(0);
}
int roll_error()
{
  char error_buf[150];
    /*
    ** Don't error on COPY FILE doesn't exist - this is an expected error.
    */
    if (sqlca.sqlcode == -38100)
	return(0);
    TRdisplay("Error code %d\n", sqlca.sqlcode);
/* # line 201 "rolldrv.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,149,error_buf,63);
  }
/* # line 202 "rolldrv.sc" */	/* host code */
    TRdisplay("\nSQL Error:\n    %s\n", error_buf );
    return (0);
}
roll_usage()
{
    TRdisplay("USAGE: ROLL_DRIVER dbname base num_slaves\n");
    TRdisplay("    dbname - database to create driver tables in -\n");
    TRdisplay("             any of db's used in test can be used.\n");
    TRdisplay("    base   - base # for slaves of this driver.\n");
    TRdisplay("             this is to allow multiple drivers to run - each\n");
    TRdisplay("             must specify a different base number.\n");
    TRdisplay("    num_slaves - number of slave scripts\n");
    TRdisplay("\n");
}
