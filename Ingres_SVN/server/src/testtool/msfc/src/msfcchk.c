# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  MSFC_CHECK	    - print results of fast commit run.
**
**  Inputs:
**	dbname	    - database
**	base_num    - test driver identifier
**	sess_num    - test session number
**
**      History:
**
**      DD-MMM-YYYY     Unknown
**		Created Multi-Server Fast Commit test.
**      21-Nov-1991	Jeromef
**		Add msfc test to piccolo library
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
    CL_SYS_ERR	syserr;
    int		j;
    int		row_count;
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
    CVal(argv[3], &sess_num);
    if (base_num < 0 || sess_num < 0)
    {
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
/* # line 85 "msfcchk.qc" */	/* ingres */
    {
      IIingopen(0,dbname,(char *)0);
    }
/* # line 87 "msfcchk.qc" */	/* host code */
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
    row_count = 0;
    STprintf(table_name, "rtab_%d_%d", base_num, sess_num);
    TRdisplay("-----------------------------------------------------------------\n");
    TRdisplay("| RTAB\n");
    TRdisplay("-----------------------------------------------------------------\n");
/* # line 103 "msfcchk.qc" */	/* retrieve */
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
/* # line 107 "msfcchk.qc" */	/* host code */
	TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3 );
	row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE1:;
    }
/* # line 110 "msfcchk.qc" */	/* host code */
    TRdisplay("-----------------------------------------------------------------\n");
    TRdisplay("(%d rows)\n", row_count);
    TRdisplay(" \n");
    row_count = 0;
    STprintf(table_name, "rtab2_%d_%d", base_num, sess_num);
    TRdisplay("-----------------------------------------------------------------\n");
    TRdisplay("| RTAB2\n");
    TRdisplay("-----------------------------------------------------------------\n");
/* # line 119 "msfcchk.qc" */	/* retrieve */
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
/* # line 123 "msfcchk.qc" */	/* host code */
	TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
	row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 126 "msfcchk.qc" */	/* host code */
    TRdisplay("-----------------------------------------------------------------\n");
    TRdisplay("(%d rows)\n", row_count);
    TRdisplay(" \n");
    for (i = 1; i <= 14; i++)
    {
	row_count = 0;
	STprintf(table_name, "stab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| STAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 137 "msfcchk.qc" */	/* retrieve */
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
/* # line 141 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE3:;
    }
/* # line 144 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 14; i++)
    {
	row_count = 0;
	STprintf(table_name, "btab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| BTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 156 "msfcchk.qc" */	/* retrieve */
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
".tid))sort by dcol1, col2, col3, col4");
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
/* # line 160 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15d|%15s|%15s|\n", dcol1, col2, col3);
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE4:;
    }
/* # line 163 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 14; i++)
    {
	row_count = 0;
	STprintf(table_name, "itab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| ITAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 175 "msfcchk.qc" */	/* retrieve */
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
/* # line 179 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE5:;
    }
/* # line 182 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 14; i++)
    {
	row_count = 0;
	STprintf(table_name, "htab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| HTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 194 "msfcchk.qc" */	/* retrieve */
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
/* # line 198 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15s|%15s|%15s|\n", col1, col2, col3);
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE6:;
    }
/* # line 201 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 4; i++)
    {
	row_count = 0;
	STprintf(table_name, "xtab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| XTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 213 "msfcchk.qc" */	/* retrieve */
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
".tid))sort by dcol1, col2, col3, col4");
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
/* # line 217 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15d|%15s|%15s|\n", dcol1, col2, col3);
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE7:;
    }
/* # line 220 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (j = 1; j <= 4;  j++)
    {
	STprintf(base_name, "xtab%d_%d_%d", j, base_num, sess_num);
/* # line 228 "msfcchk.qc" */	/* retrieve */
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
/* # line 230 "msfcchk.qc" */	/* host code */
	for (i = 1; i <= 7; i++)
	{
	    row_count = 0;
	    STprintf(table_name, "i%dx%d_%d_%d", j, i, base_num, sess_num);
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("| I%dX%d\n",j,i);
	    TRdisplay("-----------------------------------------------------------------\n");
	    switch (i)
	    {
	      case 1:
	      case 2:
/* # line 241 "msfcchk.qc" */	/* retrieve */
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
/* # line 244 "msfcchk.qc" */	/* host code */
		    TRdisplay("|%15d|\n", dcol1);
		    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE9:;
    }
/* # line 248 "msfcchk.qc" */	/* host code */
		idx_count = 0;
/* # line 249 "msfcchk.qc" */	/* retrieve */
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
/* # line 252 "msfcchk.qc" */	/* host code */
		    if (dcol1 == dcol2)
			idx_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE10:;
    }
/* # line 256 "msfcchk.qc" */	/* host code */
		break;
	      case 3:
/* # line 259 "msfcchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, col2=ascii(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b))sort by dcol1, col2");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        IIgetdomio((short *)0,1,32,79,col2);
        if (IIerrtest() != 0) goto IIrtB11;
        {
/* # line 262 "msfcchk.qc" */	/* host code */
		    TRdisplay("|%15d|%15s|\n", dcol1, col2);
		    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE11:;
    }
/* # line 266 "msfcchk.qc" */	/* host code */
		idx_count = 0;
/* # line 267 "msfcchk.qc" */	/* retrieve */
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
/* # line 271 "msfcchk.qc" */	/* host code */
		    if (dcol1 == dcol2 && dcol3 == dcol4)
			idx_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE12:;
    }
/* # line 275 "msfcchk.qc" */	/* host code */
		break;
	      case 4:
	      case 5:
/* # line 279 "msfcchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)sort by dcol1");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE13;
IIrtB13:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        if (IIerrtest() != 0) goto IIrtB13;
        {
/* # line 282 "msfcchk.qc" */	/* host code */
		    TRdisplay("|%15d|\n", dcol1);
		    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE13:;
    }
/* # line 286 "msfcchk.qc" */	/* host code */
		idx_count = 0;
/* # line 287 "msfcchk.qc" */	/* retrieve */
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
/* # line 290 "msfcchk.qc" */	/* host code */
		    if (dcol1 == dcol2)
			idx_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE14:;
    }
/* # line 294 "msfcchk.qc" */	/* host code */
		break;
	      case 6:
	      case 7:
/* # line 298 "msfcchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)sort by dcol1");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE15;
IIrtB15:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dcol1);
        if (IIerrtest() != 0) goto IIrtB15;
        {
/* # line 301 "msfcchk.qc" */	/* host code */
		    TRdisplay("|%15d|\n", dcol1);
		    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE15:;
    }
/* # line 305 "msfcchk.qc" */	/* host code */
		idx_count = 0;
/* # line 306 "msfcchk.qc" */	/* retrieve */
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
/* # line 309 "msfcchk.qc" */	/* host code */
		    if (dcol1 == dcol2)
			idx_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE16:;
    }
/* # line 313 "msfcchk.qc" */	/* host code */
		break;
	    }
	    if (row_count == base_count)
		TRdisplay("| Row Count matches Base Count\n");
	    if (idx_count == base_count)
		TRdisplay("| Index Count matches Base Count\n");
	    TRdisplay("-----------------------------------------------------------------\n");
	    TRdisplay("(%d rows)\n", row_count);
	    TRdisplay(" \n");
	}
    }
    for (i = 1; i <= 9; i++)
    {
	row_count = 0;
	STprintf(table_name, "ctab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| CTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 335 "msfcchk.qc" */	/* retrieve */
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
/* # line 339 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15s|%15s|\n", col1, col2);
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE17:;
    }
/* # line 342 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 3; i++)
    {
	row_count = 0;
	STprintf(table_name, "mtab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| MTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 354 "msfcchk.qc" */	/* retrieve */
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
/* # line 358 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15s|%15s|\n", col1, col2);
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE18:;
    }
/* # line 361 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 1; i++)
    {
	row_count = 0;
	STprintf(table_name, "dtab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| DTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 373 "msfcchk.qc" */	/* retrieve */
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
/* # line 377 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15s|\n", col1 );
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE19:;
    }
/* # line 380 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 1; i++)
    {
	row_count = 0;
	STprintf(table_name, "ttab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| TTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 392 "msfcchk.qc" */	/* retrieve */
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
/* # line 396 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15s|%15s|\n", col1, col2 );
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE20:;
    }
/* # line 399 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
    }
    /*
    ** Collect results for compressed table tests.
    ** (Fast Commit tests 23-26)
    */
    for (i = 1; i <= 4; i++)
    {
	row_count = 0;
	STprintf(table_name, "comptab%d_%d_%d", i, base_num, sess_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| COMPTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 415 "msfcchk.qc" */	/* retrieve */
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
".tid))sort by col1, col2, col9, col5");
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
/* # line 420 "msfcchk.qc" */	/* host code */
	    if (nulli == -1)
		STcopy("NULL_VALUE", col9);
	    TRdisplay("|%15s|%15s|%15s|%15s|\n", col1, col2, col3, col9 );
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE21:;
    }
/* # line 425 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
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
	row_count = 0;
	STprintf(table_name, "concurtab%d_%d", i, base_num);
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("| CONCURTAB%d\n",i);
	TRdisplay("-----------------------------------------------------------------\n");
/* # line 451 "msfcchk.qc" */	/* retrieve */
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
/* # line 454 "msfcchk.qc" */	/* host code */
	    TRdisplay("|%15s|\n", col1 );
	    row_count++;
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE22:;
    }
/* # line 457 "msfcchk.qc" */	/* host code */
	TRdisplay("-----------------------------------------------------------------\n");
	TRdisplay("(%d rows)\n", row_count);
	TRdisplay(" \n");
/* # line 461 "msfcchk.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(dcol1=sum(");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b))sort by dcol1");
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
/* # line 463 "msfcchk.qc" */	/* host code */
	j = dcol1 - 64;
	if ((j % 10000) != 0)
		TRdisplay("Sum of CONCURTAB%d.B : %d is wrong.\n", i, dcol1);
	TRdisplay(" \n");
    }
    for (i = 1; i <= 64; i++)
    {
	STprintf(table_name, "concurtab1_%d", base_num);
/* # line 472 "msfcchk.qc" */	/* retrieve */
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
/* # line 473 "msfcchk.qc" */	/* host code */
	STprintf(table_name, "concurtab2_%d", base_num);
/* # line 474 "msfcchk.qc" */	/* retrieve */
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
/* # line 475 "msfcchk.qc" */	/* host code */
	STprintf(table_name, "concurtab3_%d", base_num);
/* # line 476 "msfcchk.qc" */	/* retrieve */
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
/* # line 477 "msfcchk.qc" */	/* host code */
	STprintf(table_name, "concurtab4_%d", base_num);
/* # line 478 "msfcchk.qc" */	/* retrieve */
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
/* # line 479 "msfcchk.qc" */	/* host code */
	STprintf(table_name, "concurtab5_%d", base_num);
/* # line 480 "msfcchk.qc" */	/* retrieve */
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
/* # line 481 "msfcchk.qc" */	/* host code */
	STprintf(table_name, "concurtab6_%d", base_num);
/* # line 482 "msfcchk.qc" */	/* retrieve */
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
/* # line 484 "msfcchk.qc" */	/* host code */
	if ((dcol1 != dcol2) || (dcol1 != dcol3) || (dcol1 != dcol4) ||
	    (dcol1 != dcol5) || (dcol1 != dcol6))
	{
	    TRdisplay("CONCURTAB table mismatch: row %d: |%d|%d|%d|%d|%d|%d|\n",
		i, dcol1, dcol2, dcol3, dcol4, dcol5, dcol6);
	}
    }
/* # line 494 "msfcchk.qc" */	/* exit */
    {
      IIexit();
    }
/* # line 495 "msfcchk.qc" */	/* host code */
    PCexit(0);
  }
msfc_usage()
{
    TRdisplay("USAGE: MSFC_CHECK dbname base_number session_number\n");
}
