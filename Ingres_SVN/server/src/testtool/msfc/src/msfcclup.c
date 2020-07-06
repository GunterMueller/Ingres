# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  MSFC_CLEANUP    - clean up tables to prepare for fast commit test run.
**
**	Destroys tables left around in sync database by the fast commit
**	test driver:
**
**	    msfc_sync_tab_NN	    (tables have test driver number appened
**	    msfc_slave_tab_NN	     to them so multiple drivers can run)
**
**	This must be run previous to starting up slave sessions sessions or
**	they may get out of sync.
**
**	Inputs:
**	    dbname	- database in which sync tables are used.
**	    base_num	- test driver identifier.
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
#include	<compat.h>
#include	<tr.h>
#include	<pc.h>
#include	<st.h>
main(argc, argv)
int	    argc;
char	    *argv[];
{
  int count_tups = 0;
  char *dbname;
  char table_name[32];
    int		base_num = 0;
    CL_SYS_ERR	syserr;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &syserr);
#endif
#ifdef UNIX
    TRset_file(TR_T_OPEN, "stdio", 5, &syserr);
#endif
    if (argc < 3)
    {
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    dbname = argv[1];
    CVal(argv[2], &base_num);
/* # line 66 "msfcclup.qc" */	/* ingres */
  {
    IIingopen(0,dbname,(char *)0);
  }
/* # line 68 "msfcclup.qc" */	/* host code */
    STprintf(table_name, "msfc_sync_tab_%d", base_num);
/* # line 69 "msfcclup.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
    IIwritio(1,(short *)0,1,32,0,table_name);
    IIsyncup((char *)0,0);
  }
/* # line 70 "msfcclup.qc" */	/* host code */
    STprintf(table_name, "msfc_slave_tab_%d", base_num);
/* # line 71 "msfcclup.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
    IIwritio(1,(short *)0,1,32,0,table_name);
    IIsyncup((char *)0,0);
  }
/* # line 73 "msfcclup.qc" */	/* exit */
  {
    IIexit();
  }
/* # line 75 "msfcclup.qc" */	/* host code */
    PCexit(0);
}
msfc_usage()
{
    TRdisplay("USAGE: MSFC_CLEANUP  dbname base_num\n");
}
