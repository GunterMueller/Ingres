# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  MSFC_DRIVER	    - Sync up fast commit sessions before crash.
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
**	copy files out to the msfc_data directory.  These copy files must
**	be cleaned out previous to each fast commit test run.
**
**	Inputs:
**	    dbname	- database to use.
**	    base_num	- test driver identifier
**	    num_slaves	- number of fast commit suites running.
**
**	Also, the logical MSFC_DATA must be defined to this process.
**
**      History:
**
**      DD-MMM-YYYY     Unknown
**              Created Multi-Server Fast Commit test.
**      21-Nov-1991     Jeromef
**              Add msfc test to piccolo library
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
#include	<compat.h>
#include	<tr.h>
#include	<pc.h>
#include	<si.h>
#include	<st.h>
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
  char error_buf[150];
    char	table_name[32];
    int		msfc_error();
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
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    dbname = argv[1];
    CVal(argv[2], &base_num);
    CVal(argv[3], &num_slaves);
    if (base_num < 0 || num_slaves <= 0)
    {
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    /* IIseterr(msfc_error); */
/* # line 95 "msfcdrv.sc" */	/* host code */
    /*
    ** Establish two sessions - one for holding the sync lock and
    ** the other for checking for the completion of the tests.
    */
/* # line 99 "msfcdrv.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(1);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 100 "msfcdrv.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,149,error_buf,63);
  }
/* # line 101 "msfcdrv.sc" */	/* host code */
	SIprintf ("\nConnected to session 1 %s \n", error_buf);
        SIflush(stdout);
/* # line 104 "msfcdrv.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(2);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 105 "msfcdrv.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,149,error_buf,63);
  }
/* # line 106 "msfcdrv.sc" */	/* host code */
	SIprintf ("\nConnected to session 2 %s \n", error_buf);
        SIflush(stdout);
    /*
    ** Get sync lock in session #1 - this will prevent any test slaves
    ** from hitting the crash point until the driver says its OK.
    */
/* # line 113 "msfcdrv.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 114 "msfcdrv.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,149,error_buf,63);
  }
/* # line 115 "msfcdrv.sc" */	/* host code */
	SIprintf ("\nSwitched to session 1 %s \n", error_buf);
        SIflush(stdout);
/* # line 117 "msfcdrv.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set trace point DM1420");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 119 "msfcdrv.sc" */	/* host code */
    /*
    ** Switch to session #2 and create the sync table.
    ** This will start the slave processes going.
    */
/* # line 123 "msfcdrv.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
  }
/* # line 124 "msfcdrv.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,149,error_buf,63);
  }
/* # line 125 "msfcdrv.sc" */	/* host code */
	SIprintf ("\nSwitched to session 2 %s \n", error_buf);
        SIflush(stdout);
    STprintf(table_name, "msfc_sync_tab_%d", base_num);
    STprintf(command_line, "create table %s (%s, %s) with duplicates",
	table_name, 
	"base i4 not null with default", 
	"slaves i4 not null with default");
/* # line 132 "msfcdrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 133 "msfcdrv.sc" */	/* host code */
    STprintf(command_line, "insert into %s (base, slaves) values (%d, %d)",
	table_name, base_num, num_slaves);
/* # line 135 "msfcdrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 136 "msfcdrv.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 139 "msfcdrv.sc" */	/* host code */
    /*
    ** Wait for slaves to all get to point where they are ready for
    ** the servers to crash.
    **
    ** The slaves indicate they are ready by writing information to
    ** copy files.
    */
    STprintf(table_name, "msfc_slave_tab_%d", base_num);
    STprintf(command_line, "create table %s (%s) with duplicates",
	table_name, 
	"a c50 not null");
/* # line 151 "msfcdrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 152 "msfcdrv.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 154 "msfcdrv.sc" */	/* host code */
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
/* # line 165 "msfcdrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 167 "msfcdrv.sc" */	/* host code */
	for (i = 1; i <= num_slaves; i++)
	{
	    /* Build filename based on base and slave number */
	    STprintf(copy_file, "MSFC_DATA:msfc_copy_%d_%d.in",
		base_num, i);
    	    STprintf(command_line, "copy table %s (a=c0nl) from '%s'", 
		table_name, copy_file);
/* # line 174 "msfcdrv.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(command_line);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 175 "msfcdrv.sc" */	/* host code */
	}
	STprintf(command_line, "select count(distinct a) from %s",
		table_name);
/* # line 180 "msfcdrv.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c",7806,26346);
    IIwritio(1,(short *)0,1,32,0,command_line);
    IIcsQuery((char *)"c",7806,26346);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 181 "msfcdrv.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c",7806,26346,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&slaves_done);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 182 "msfcdrv.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c",7806,26346);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 183 "msfcdrv.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 184 "msfcdrv.sc" */	/* host code */
    }
    /*
    ** Switch back to session 1 to commit the transaction holding
    ** the sync lock.
    */
/* # line 190 "msfcdrv.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 191 "msfcdrv.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,149,error_buf,63);
  }
/* # line 192 "msfcdrv.sc" */	/* host code */
	SIprintf ("\nSwitched back to session 1 %s \n", error_buf);
        SIflush(stdout);
/* # line 194 "msfcdrv.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 195 "msfcdrv.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(1);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 197 "msfcdrv.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
  }
/* # line 198 "msfcdrv.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,149,error_buf,63);
  }
/* # line 199 "msfcdrv.sc" */	/* host code */
	SIprintf ("\nSwitched back to session 1 %s \n", error_buf);
        SIflush(stdout);
/* # line 201 "msfcdrv.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(2);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      msfc_error();
  }
/* # line 203 "msfcdrv.sc" */	/* host code */
    PCexit(0);
}
int msfc_error()
{
  char error_buf[150];
    /*
    ** Don't error on COPY FILE doesn't exist - this is an expected error.
    */
    if (sqlca.sqlcode == -38100)
	return(0);
    TRdisplay("Error code %d\n", sqlca.sqlcode);
/* # line 219 "msfcdrv.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,149,error_buf,63);
  }
/* # line 220 "msfcdrv.sc" */	/* host code */
    TRdisplay("\nSQL Error:\n    %s\n", error_buf );
    return (0);
}
msfc_usage()
{
    TRdisplay("USAGE: MSFC_DRIVER dbname base num_slaves\n");
    TRdisplay("    dbname - database to create driver tables in -\n");
    TRdisplay("             any of db's used in test can be used.\n");
    TRdisplay("    base   - base # for slaves of this driver.\n");
    TRdisplay("             this is to allow multiple drivers to run - each\n");
    TRdisplay("             must specify a different base number.\n");
    TRdisplay("    num_slaves - number of slave scripts\n");
    TRdisplay("\n");
}
