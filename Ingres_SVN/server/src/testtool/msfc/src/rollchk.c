# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  ROLL_CHECK	    - print results of fast commit run.
**
**  Inputs:
**	dbname	    - database
**	base_num    - test driver identifier
**	sess_num    - test session number
**
**      History:
**
**      DD-MMM-YYYY     Unknown
**              Created Multi-Server Fast Commit test.
**      01-Mar-1991     Jeromef
**              Modified Multi-Server Fast Commit test to include ckpdb and
**		rollforwarddb.
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
    int sess_num = -1;
    char table_name[32];
    char base_name[32];
    char col1[80];
    char col2[80];
    char col3[80];
    char col4[80];
    char col5[80];
    char col9[1002];
    int dcol1;
    int dcol2;
    int dcol3;
    int dcol4;
    int dcol5;
    int dcol6;
    int base_count;
    int idx_count;
    short nulli;
    int i;
    int file_test;
    int		row_count;
    int		tbl_cnt;
    CL_SYS_ERR	syserr;
    int		j;
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
    CVal(argv[3], &sess_num);
    if (base_num < 0 || sess_num < 0)
    {
	roll_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
/* # line 91 "rollchk.qc" */	/* ingres */
    {
      IIingopen(0,dbname,(char *)0);
    }
/* # line 93 "rollchk.qc" */	/* host code */
    TRdisplay(" \n");
    TRdisplay("Test results for Multi Server Fast Commit test suite\n");
#ifdef NOTDEFINED	/* leave out this so won't get diffs */
    TRdisplay("----------------------------------------------------\n");
    TRdisplay("  Database    : %s\n", dbname);
    TRdisplay("  Base Number : %d   Session Number %d\n", base_num, sess_num);
    TRdisplay("----------------------------------------------------\n");
#endif
    TRdisplay(" \n");
    TRdisplay(" \n");
    STprintf(table_name, "rtab_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) ) 
    {
	row_count = 0;
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| RTAB\n");
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 111 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=left(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, 10), col3=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
".tid))sort by col1, col2, col3, col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col3);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB1;
        {
/* # line 115 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE1:;
    }
/* # line 118 "rollchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    STprintf(table_name, "rtab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) ) 
    {
	row_count = 0;
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| RTAB2\n");
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 130 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=left(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, 10), col3=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
".tid))sort by col1, col2, col3, col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col3);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB2;
        {
/* # line 134 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 137 "rollchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 14; i++)
    {
	STprintf(table_name, "stab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| STAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 151 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=left(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, 10), col3=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
".tid))sort by col1, col2, col3, col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col3);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB3;
        {
/* # line 155 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE3:;
    }
/* # line 158 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 14; i++)
    {
	STprintf(table_name, "btab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| BTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 173 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, col2=left(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, 10), col3=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
".tid))order by dcol1, col2, col3, col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col3);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB4;
        {
/* # line 177 "rollchk.qc" */	/* host code */
		TRdisplay("|%15d|%15s|%15s|\n", dcol1, col2, col3);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE4:;
    }
/* # line 180 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 14; i++)
    {
	STprintf(table_name, "itab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| ITAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 195 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=left(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, 10), col3=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
".tid))sort by col1, col2, col3, col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col3);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB5;
        {
/* # line 199 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE5:;
    }
/* # line 202 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 14; i++)
    {
	STprintf(table_name, "htab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| HTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 217 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=left(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, 10), col3=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
".tid))sort by col1, col2, col3, col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col3);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB6;
        {
/* # line 221 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE6:;
    }
/* # line 224 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 4; i++)
    {
	STprintf(table_name, "xtab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| XTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 239 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, col2=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b), col3=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
".tid))order by dcol1, col2, col3, col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col3);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB7;
        {
/* # line 243 "rollchk.qc" */	/* host code */
		TRdisplay("|%15d|%15s|%15s|\n", dcol1, col2, col3);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE7:;
    }
/* # line 246 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (j = 1; j <= 4;  j++)
    {
	STprintf(base_name, "xtab%d_%d_%d", j, base_num, sess_num);
	if (file_exist(base_name))
	{
/* # line 257 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(base_count=count(");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&base_count);
        if (IIerrtest() != 0) goto IIrtB8;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE8:;
    }
/* # line 259 "rollchk.qc" */	/* host code */
	  for (i = 1; i <= 7; i++)
	  {
	    row_count = 0;
	    STprintf(table_name, "i%dx%d_%d_%d", j, i, base_num, sess_num);
	    if (file_exist(table_name))
	    {
	      TRdisplay("-----------------------------------------------------------------\n");
	      TRdisplay("| I%dX%d\n",j,i);
	      TRdisplay("-----------------------------------------------------------------\n");
	      switch (i)
	      {
		case 1:
		case 2:
/* # line 272 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)order by dcol1");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        if (IIerrtest() != 0) goto IIrtB9;
        {
/* # line 275 "rollchk.qc" */	/* host code */
		    TRdisplay("|%15d|\n", dcol1);
		    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE9:;
    }
/* # line 279 "rollchk.qc" */	/* host code */
		idx_count = 0;
/* # line 280 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, dcol2=");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tidp=");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        IIgetdomio((short *)0,1,30,4,&dcol2);
        if (IIerrtest() != 0) goto IIrtB10;
        {
/* # line 283 "rollchk.qc" */	/* host code */
		    if (dcol1 == dcol2)
			idx_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE10:;
    }
/* # line 287 "rollchk.qc" */	/* host code */
		break;
		case 3:
/* # line 290 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, col2=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b))order by dcol1, col2");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        IIgetdomio((short *)0,1,32,79,col2);
        if (IIerrtest() != 0) goto IIrtB11;
        {
/* # line 293 "rollchk.qc" */	/* host code */
		    TRdisplay("|%15d|%15s|\n", dcol1, col2);
		    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE11:;
    }
/* # line 297 "rollchk.qc" */	/* host code */
		idx_count = 0;
/* # line 298 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, dcol2=");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, dcol3=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, dcol4=");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tidp=");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE12;
IIrtB12:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        IIgetdomio((short *)0,1,30,4,&dcol2);
        IIgetdomio((short *)0,1,30,4,&dcol3);
        IIgetdomio((short *)0,1,30,4,&dcol4);
        if (IIerrtest() != 0) goto IIrtB12;
        {
/* # line 302 "rollchk.qc" */	/* host code */
		    if (dcol1 == dcol2 && dcol3 == dcol4)
			idx_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE12:;
    }
/* # line 306 "rollchk.qc" */	/* host code */
		break;
		case 4:
		case 5:
/* # line 310 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)order by dcol1");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE13;
IIrtB13:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        if (IIerrtest() != 0) goto IIrtB13;
        {
/* # line 313 "rollchk.qc" */	/* host code */
		    TRdisplay("|%15d|\n", dcol1);
		    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE13:;
    }
/* # line 317 "rollchk.qc" */	/* host code */
		idx_count = 0;
/* # line 318 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c, dcol2=");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)where ");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tidp");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE14;
IIrtB14:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        IIgetdomio((short *)0,1,30,4,&dcol2);
        if (IIerrtest() != 0) goto IIrtB14;
        {
/* # line 321 "rollchk.qc" */	/* host code */
		    if (dcol1 == dcol2)
			idx_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE14:;
    }
/* # line 325 "rollchk.qc" */	/* host code */
		break;
		case 6:
		case 7:
/* # line 329 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)order by dcol1");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE15;
IIrtB15:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        if (IIerrtest() != 0) goto IIrtB15;
        {
/* # line 332 "rollchk.qc" */	/* host code */
		    TRdisplay("|%15d|\n", dcol1);
		    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE15:;
    }
/* # line 336 "rollchk.qc" */	/* host code */
		idx_count = 0;
/* # line 337 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, dcol2=");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)where ");
      IIwritio(1,(short *)0,1,32,0,base_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tidp");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE16;
IIrtB16:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        IIgetdomio((short *)0,1,30,4,&dcol2);
        if (IIerrtest() != 0) goto IIrtB16;
        {
/* # line 340 "rollchk.qc" */	/* host code */
		    if (dcol1 == dcol2)
			idx_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE16:;
    }
/* # line 344 "rollchk.qc" */	/* host code */
		break;
	      }		/* end of switch			*/
	    }		/* end of table_name IF statement	*/
	    if (row_count == base_count)
		TRdisplay("| Row Count matches Base Count\n");
	    if (idx_count == base_count)
		TRdisplay("| Index Count matches Base Count\n");
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	  }			/* end of inner FOR loop	*/
	}		/* end of the IF statement	*/
    }		/* end of outer FOR loop	*/
    for (i = 1; i <= 9; i++)
    {
	STprintf(table_name, "ctab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| CTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 370 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid))sort by col1, col2, \
col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE17;
IIrtB17:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB17;
        {
/* # line 374 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|%15s|\n", col1, col2);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE17:;
    }
/* # line 377 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 3; i++)
    {
	STprintf(table_name, "mtab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| MTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 392 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid))sort by col1, col2, \
col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE18;
IIrtB18:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB18;
        {
/* # line 396 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|%15s|\n", col1, col2);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE18:;
    }
/* # line 399 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 1; i++)
    {
	STprintf(table_name, "dtab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| DTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 414 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid))sort by col1, col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE19;
IIrtB19:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB19;
        {
/* # line 418 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|\n", col1);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE19:;
    }
/* # line 421 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 1; i++)
    {
	STprintf(table_name, "ttab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| TTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 436 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b), col4=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid))sort by col1, col2, \
col4");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE20;
IIrtB20:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col4);
        if (IIerrtest() != 0) goto IIrtB20;
        {
/* # line 440 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|%15s|\n", col1, col2);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE20:;
    }
/* # line 443 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    /*
    ** Collect results for compressed table tests.
    ** (Fast Commit tests 23-26)
    */
    for (i = 1; i <= 4; i++)
    {
	STprintf(table_name, "comptab%d_%d_%d", i, base_num, sess_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| COMPTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 462 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col2=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b, col3=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c, col9=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".d, col5=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
".tid))sort by col1, col2, col3, col9, col5");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE21;
IIrtB21:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col2);
        IIgetdomio((short *)0,1,32,79,col3);
        IIgetdomio(&nulli,1,32,1001,col9);
        IIgetdomio((short *)0,1,32,79,col5);
        if (IIerrtest() != 0) goto IIrtB21;
        {
/* # line 467 "rollchk.qc" */	/* host code */
		if (nulli == -1)
		STcopy("NULL_VALUE", col9);
		TRdisplay("|%15s|%15s|%15s|%15s|\n", col1, col2, col3, col9);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE21:;
    }
/* # line 472 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    /*
    ** Check results of concurrency tables (Fast Commit tests 27-28).  
    ** These tables are shared by all sessions running fast commit tests 
    ** and the results in them will depend on the number of sessions which 
    ** executed the fast commit test suite.
    **
    ** Rather than print out the table, we read through the tables to make
    ** sure that the information is consistent.  The following should be true:
    **
    **     Each table should have 64 rows numbered 1-64
    **     Each table should be identical
    **     The sum of the column b's in each table should be a multiple of
    **         10,000 (+64 for the orignal values).
    */
    for (i = 1; i <= 6; i++)
    {
	STprintf(table_name, "concurtab%d_%d", i, base_num);
	if (file_exist(table_name))
	{
	    row_count = 0;
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| CONCURTAB%d\n",i);
	    TRdisplay("-----------------------------------------------------------------\n");
/* # line 501 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(col1=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a), col5=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid))sort by col1, col5");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE22;
IIrtB22:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,79,col1);
        IIgetdomio((short *)0,1,32,79,col5);
        if (IIerrtest() != 0) goto IIrtB22;
        {
/* # line 504 "rollchk.qc" */	/* host code */
		TRdisplay("|%15s|\n", col1);
		row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE22:;
    }
/* # line 507 "rollchk.qc" */	/* host code */
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
/* # line 511 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=sum(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE23;
IIrtB23:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        if (IIerrtest() != 0) goto IIrtB23;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE23:;
    }
/* # line 512 "rollchk.qc" */	/* host code */
	    j = dcol1 - 64;
	    if ((j % 10000) != 0)
		TRdisplay("Sum of CONCURTAB%d.B : %d is wrong.\n", i, dcol1);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 64; i++)
    {
	tbl_cnt = 0;
	STprintf(table_name, "concurtab1_%d", base_num);
	if (file_exist(table_name))
	{
/* # line 525 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&i);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE24;
IIrtB24:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        if (IIerrtest() != 0) goto IIrtB24;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE24:;
    }
/* # line 526 "rollchk.qc" */	/* host code */
	    tbl_cnt = tbl_cnt + 1;
	}
	STprintf(table_name, "concurtab2_%d", base_num);
	if (file_exist(table_name))
	{
/* # line 531 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol2=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&i);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE25;
IIrtB25:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol2);
        if (IIerrtest() != 0) goto IIrtB25;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE25:;
    }
/* # line 532 "rollchk.qc" */	/* host code */
	    tbl_cnt = tbl_cnt + 1;
	}
	STprintf(table_name, "concurtab3_%d", base_num);
	if (file_exist(table_name))
	{
/* # line 537 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol3=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&i);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE26;
IIrtB26:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol3);
        if (IIerrtest() != 0) goto IIrtB26;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE26:;
    }
/* # line 538 "rollchk.qc" */	/* host code */
	    tbl_cnt = tbl_cnt + 1;
	}
	STprintf(table_name, "concurtab4_%d", base_num);
	if (file_exist(table_name))
	{
/* # line 543 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol4=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&i);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE27;
IIrtB27:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol4);
        if (IIerrtest() != 0) goto IIrtB27;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE27:;
    }
/* # line 544 "rollchk.qc" */	/* host code */
	    tbl_cnt = tbl_cnt + 1;
	}
	STprintf(table_name, "concurtab5_%d", base_num);
	if (file_exist(table_name))
	{
/* # line 549 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol5=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&i);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE28;
IIrtB28:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol5);
        if (IIerrtest() != 0) goto IIrtB28;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE28:;
    }
/* # line 550 "rollchk.qc" */	/* host code */
	    tbl_cnt = tbl_cnt + 1;
	}
	STprintf(table_name, "concurtab6_%d", base_num);
	if (file_exist(table_name))
	{
/* # line 555 "rollchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol6=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)where ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&i);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE29;
IIrtB29:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol6);
        if (IIerrtest() != 0) goto IIrtB29;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE29:;
    }
/* # line 556 "rollchk.qc" */	/* host code */
	    tbl_cnt = tbl_cnt + 1;
	}
	if ( (tbl_cnt > 0) && ((dcol1 != dcol2) || (dcol1 != dcol3) || 
		(dcol1 != dcol4) || (dcol1 != dcol5) || (dcol1 != dcol6)) )
	{
	    TRdisplay("CONCURTAB table mismatch: row %d: |%d|%d|%d|%d|%d|%d|\n",
		i, dcol1, dcol2, dcol3, dcol4, dcol5, dcol6);
	}
    }
/* # line 569 "rollchk.qc" */	/* exit */
    {
      IIexit();
    }
/* # line 570 "rollchk.qc" */	/* host code */
    PCexit(0);
  }
roll_usage()
{
    TRdisplay("USAGE: ROLL_CHECK dbname base_number session_number\n");
}
int file_exist(table_name)
  char table_name[32];
{
  int file_test;
/* # line 584 "rollchk.qc" */	/* retrieve */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"rollchk1",7802,3061);
      IIputdomio((short *)0,1,32,0,table_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"rollchk1",7802,3061);
        IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(file_test=count(iitables.table_name where iitables.table_nam\
e=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"))");
        IIexDefine(0,(char *)"rollchk1",7802,3061);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE30;
IIrtB30:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&file_test);
      if (IIerrtest() != 0) goto IIrtB30;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE30:;
  }
/* # line 586 "rollchk.qc" */	/* host code */
    if (file_test != 0)
	return(1);
    else
	return(0);
}
