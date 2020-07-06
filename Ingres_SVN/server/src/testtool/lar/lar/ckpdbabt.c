# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
#include    <compat.h>
#include    <tr.h>
/*
**  CKPDB_ABORT - used for online checkpoint tests - performs transaction
**	aborts.
**
**  CKPDB_ABORT dbname loops [options]
**
**  Required arguments
**	dbname	        - database name
**	loops	        - number of iterations
**
**  Optional arguments
**	none
**
**  History
**      13-aug-1991 (lauraw)
**              Filename changed to conform to standard
**		(ckpdb_abort.qc changed to ckpdbabt.qc)
**
**	19-sep-1995 (somsa01)
**		Added defined(DESKTOP) for NT, OS/2, and Netware platforms.
**  08-Sep-1997 (merja01)
**      Change long to i4 to make compatable with 64 bit platforms     
**      such as axp_osf (from schte01). 
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
** Ming hints
NEEDLIBS = LIBINGRES
PROGRAM = ckpdb_abort
**
*/
main(argc, argv)
int	argc;
char	*argv[];
{
  char *dbname;
  i4 i;
    CL_SYS_ERR	err_code;
    i4		loops;
    i4     start_time, end_time;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &err_code);
#endif
#if defined(UNIX) || defined(DESKTOP)
    TRset_file(TR_T_OPEN, "stdio", 5, &err_code);
#endif
    if (argc < 2)
    {
	TRdisplay("USAGE: CKPDB_ABORT dbname loops\n");
	TRset_file(TR_T_CLOSE, 0, 0, &err_code);
	exit(1);
    }
    loops = 1000;
    dbname = argv[1];
    if (argc > 2)
	loops = atoi(argv[2]);
/* # line 73 "ckpdbabt.qc" */	/* ingres */
  {
    IIingopen(0,dbname,(char *)0);
  }
/* # line 75 "ckpdbabt.qc" */	/* host code */
	TRdisplay("Starting Online-Checkpoint/Abort Test\n");
	start_time = TMsecs();
#ifdef VMS
	lib$init_timer();
#endif
	/*
	** Start with empty control table.
	*/
/* # line 85 "ckpdbabt.qc" */	/* delete */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"delete ckpdb_ab_control");
    IIsyncup((char *)0,0);
  }
/* # line 87 "ckpdbabt.qc" */	/* host code */
	/*
	** Loop through transactions.
	** Each transaction append rows to table1 and deletes rows from table2
	*/
	for (i = 1; i <= loops; i++)
	{
/* # line 94 "ckpdbabt.qc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_ab_control(cnt=");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 96 "ckpdbabt.qc" */	/* begin transaction */
  {
    IIxact(0);
  }
/* # line 97 "ckpdbabt.qc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append ckpdb_abort1(a=ckpdb_ab_control.cnt)");
    IIsyncup((char *)0,0);
  }
/* # line 98 "ckpdbabt.qc" */	/* delete */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete ckpdb_abort2 where ckpdb_abort2.a<");
    IIputdomio((short *)0,1,30,4,&i);
    IIsyncup((char *)0,0);
  }
/* # line 99 "ckpdbabt.qc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"append ckpdb_ab1_nojnl(a=ckpdb_ab_control.cnt)");
    IIsyncup((char *)0,0);
  }
/* # line 100 "ckpdbabt.qc" */	/* delete */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete ckpdb_ab2_nojnl where ckpdb_ab2_nojnl.a<");
    IIputdomio((short *)0,1,30,4,&i);
    IIsyncup((char *)0,0);
  }
/* # line 103 "ckpdbabt.qc" */	/* abort */
  {
    IIxact(2);
  }
/* # line 103 "ckpdbabt.qc" */	/* host code */
	}
	end_time = TMsecs();
#ifdef VMS
	lib$show_timer();
#endif
	TRdisplay("Online-Checkpoint/Abort Test completed in %d seconds\n",
	    end_time - start_time);
/* # line 114 "ckpdbabt.qc" */	/* exit */
  {
    IIexit();
  }
/* # line 116 "ckpdbabt.qc" */	/* host code */
    TRset_file(TR_T_CLOSE, 0, 0, &err_code);
}
