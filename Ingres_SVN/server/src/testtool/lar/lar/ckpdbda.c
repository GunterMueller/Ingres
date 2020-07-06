# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
#include    <compat.h>
#include    <tr.h>
/*
**  CKPDB_DELAPP - used for online checkpoint tests - performs delete/appends
**	to btree table.
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
**	13-aug-1991 (lauraw)
**		Filename changed to conform to standard
**		(changed from ckpdb_delap.qc to ckpdbda.qc)
**
**	19-sep-1995 (somsa01)
**		Added defined(DESKTOP) for NT, OS/2, and Netware platforms.
**  08-Sep=1997 (merja01)
**      Changes longs to i4 to make compatable with the 64 bit
**      platforms such as axp_osf (from schte01).
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
PROGRAM = ckpdb_delapp
**
*/
main(argc, argv)
int	argc;
char	*argv[];
{
    CL_SYS_ERR	err_code;
  char *dbname;
  i4 min_value;
  i4 max_value;
  i4 count_values;
    i4  	loops;
    i4  	i;
    i4     start_time, end_time;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &err_code);
#endif
#if defined(UNIX) || defined(DESKTOP)
    TRset_file(TR_T_OPEN, "stdio", 5, &err_code);
#endif
    if (argc < 3)
    {
	TRdisplay("USAGE: UPDATE dbname loops\n");
	TRset_file(TR_T_CLOSE, 0, 0, &err_code);
	exit(1);
    }
    loops = 1000;
    dbname = argv[1];
    loops = atoi(argv[2]);
/* # line 77 "ckpdbda.qc" */	/* ingres */
  {
    IIingopen(0,dbname,(char *)0);
  }
/* # line 79 "ckpdbda.qc" */	/* host code */
	TRdisplay("Starting Online-Checkpoint/Delapp Btree Test\n");
	start_time = TMsecs();
#ifdef VMS
	lib$init_timer();
#endif
	/*
	** Prepare table - get min and max values.
	*/
/* # line 90 "ckpdbda.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(min_value=min(ckpdb_btree.value), max_value=max(ckpdb_btree.\
value), count_values=count(ckpdb_btree.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&min_value);
      IIgetdomio((short *)0,1,30,4,&max_value);
      IIgetdomio((short *)0,1,30,4,&count_values);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 95 "ckpdbda.qc" */	/* host code */
	/*
	** Start Delete/Append loop.
	*/
	for (i = 0; i < loops; i++)
	{
/* # line 100 "ckpdbda.qc" */	/* begin transaction */
  {
    IIxact(0);
  }
/* # line 102 "ckpdbda.qc" */	/* delete */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete ckpdb_btree where ckpdb_btree.value=");
    IIputdomio((short *)0,1,30,4,&min_value);
    IIsyncup((char *)0,0);
  }
/* # line 103 "ckpdbda.qc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_btree(value=");
    IIputdomio((short *)0,1,30,4,&max_value);
    IIwritio(0,(short *)0,1,32,0,(char *)"+1)");
    IIsyncup((char *)0,0);
  }
/* # line 105 "ckpdbda.qc" */	/* delete */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete ckpdb_hash where ckpdb_hash.value=");
    IIputdomio((short *)0,1,30,4,&min_value);
    IIsyncup((char *)0,0);
  }
/* # line 106 "ckpdbda.qc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_hash(value=");
    IIputdomio((short *)0,1,30,4,&max_value);
    IIwritio(0,(short *)0,1,32,0,(char *)"+1)");
    IIsyncup((char *)0,0);
  }
/* # line 107 "ckpdbda.qc" */	/* delete */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete ckpdb_btree_nojnl where ckpdb_btree_nojnl.value=");
    IIputdomio((short *)0,1,30,4,&min_value);
    IIsyncup((char *)0,0);
  }
/* # line 108 "ckpdbda.qc" */	/* append */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_btree_nojnl(val\
ue=");
    IIputdomio((short *)0,1,30,4,&max_value);
    IIwritio(0,(short *)0,1,32,0,(char *)"+1)");
    IIsyncup((char *)0,0);
  }
/* # line 109 "ckpdbda.qc" */	/* host code */
		min_value++, max_value++;
/* # line 111 "ckpdbda.qc" */	/* end transaction */
  {
    IIxact(1);
  }
/* # line 113 "ckpdbda.qc" */	/* host code */
	}
	end_time = TMsecs();
#ifdef VMS
	lib$show_timer();
#endif
	TRdisplay("Online-Checkpoint/Delapp Btree Test completed in %d seconds\n",
	    end_time - start_time);
/* # line 124 "ckpdbda.qc" */	/* exit */
  {
    IIexit();
  }
/* # line 126 "ckpdbda.qc" */	/* host code */
    TRset_file(TR_T_CLOSE, 0, 0, &err_code);
}
