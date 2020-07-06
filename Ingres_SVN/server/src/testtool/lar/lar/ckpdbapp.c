# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
#include    <compat.h>
#include    <tr.h>
/*
**  CKPDB_APPEND - used for online checkpoint tests - performs appends.
**
**  CKPDB_APPEND dbname loops [options]
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
**              (changed from ckpdb_append.qc to ckpdbapp.qc)
**
**	19-sep-1995 (somsa01)
**		Added defined(DESKTOP) for NT, OS/2, and Netware platforms.
**  08-Sep-1997 (merja01)
**     Change longs to i4 to make compatable with 64 bit platforms
**     such as axp_osf (from schte01). 
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
** Ming hints
NEEDLIBS = LIBINGRES
PROGRAM = ckpdb_append
**
*/
main(argc, argv)
int	argc;
char	*argv[];
{
  char *dbname;
  i4 i;
  i4 j;
  i4 run_no;
    CL_SYS_ERR	err_code;
    i4        loops;
    i4     start_time, end_time;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &err_code);
#endif
#if defined(UNIX) || defined(DESKTOP)
    TRset_file(TR_T_OPEN, "stdio", 5, &err_code);
#endif
    if (argc < 2)
    {
	TRdisplay("USAGE: CKPDB_APPEND dbname loops\n");
	TRset_file(TR_T_CLOSE, 0, 0, &err_code);
	exit(1);
    }
    loops = 1000;
    dbname = argv[1];
    if (argc > 2)
	loops = atoi(argv[2]);
/* # line 75 "ckpdbapp.qc" */	/* ingres */
  {
    IIingopen(0,dbname,(char *)0);
  }
/* # line 77 "ckpdbapp.qc" */	/* host code */
	TRdisplay("Starting Online-Checkpoint/Append Test\n");
	start_time = TMsecs();
#ifdef VMS
	lib$init_timer();
#endif
	/*
	** Find next run number to use.
	*/
/* # line 87 "ckpdbapp.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(run_no=max(ckpdb_ap4.run))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&run_no);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 88 "ckpdbapp.qc" */	/* host code */
	run_no++;
	/*
	** Loop through transactions.
	*/
	for (i = 1; i <= loops; i++)
	{
/* # line 95 "ckpdbapp.qc" */	/* begin transaction */
  {
    IIxact(0);
  }
/* # line 96 "ckpdbapp.qc" */	/* host code */
	    for (j = 1; j <= i % 10; j++)
	    {
/* # line 98 "ckpdbapp.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbapp1",7794,10069);
      IIputdomio((short *)0,1,30,4,&run_no);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&j);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbapp1",7794,10069);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_ap1(run=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&run_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", key=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", sequence=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbapp1",7794,10069);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 99 "ckpdbapp.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbapp2",7795,23669);
      IIputdomio((short *)0,1,30,4,&run_no);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&j);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbapp2",7795,23669);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_ap2(run=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&run_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", key=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", sequence=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbapp2",7795,23669);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 100 "ckpdbapp.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbapp3",7796,25022);
      IIputdomio((short *)0,1,30,4,&run_no);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&j);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbapp3",7796,25022);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_ap3(run=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&run_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", key=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", sequence=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbapp3",7796,25022);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 101 "ckpdbapp.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbapp4",7797,26316);
      IIputdomio((short *)0,1,30,4,&run_no);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&j);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbapp4",7797,26316);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_ap4(run=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&run_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", key=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", sequence=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbapp4",7797,26316);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 102 "ckpdbapp.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbapp5",7798,27600);
      IIputdomio((short *)0,1,30,4,&run_no);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&j);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbapp5",7798,27600);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_ap5_nojnl(r\
un=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&run_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", key=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", sequence=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbapp5",7798,27600);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 103 "ckpdbapp.qc" */	/* host code */
	    }
/* # line 104 "ckpdbapp.qc" */	/* end transaction */
  {
    IIxact(1);
  }
/* # line 105 "ckpdbapp.qc" */	/* host code */
	}
	end_time = TMsecs();
#ifdef VMS
	lib$show_timer();
#endif
	TRdisplay("Online-Checkpoint/Append Test completed in %d seconds\n",
	    end_time - start_time);
/* # line 116 "ckpdbapp.qc" */	/* exit */
  {
    IIexit();
  }
/* # line 118 "ckpdbapp.qc" */	/* host code */
    TRset_file(TR_T_CLOSE, 0, 0, &err_code);
}
