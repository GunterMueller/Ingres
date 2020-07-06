# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  MSFC_CREATE	    - run fast commit test
**
**  This program executes the fast commit tests.
**  It will execute only one test at a time, as the server is crashed at the
**  end of each test.
**
**  It will request an installation lock using the DMF trace point DM1420
**  before crashing in order to sync up with the test driver.
**
**  Before requesting the DM1420 trace point, it will create a copy file
**  in the MSFC_DATA directory identifying itself as ready to crash.
**
**  Test 0 indicates to run all the tests without causing crash in order
**  to generate a canon file.
**
**  Inputs:
**	dbname	    - database to use
**	test_num    - fast commit test to run (0-28)
**	base_num    - test driver identifier
**	sess_num    - session this is (multiple sessions may be running)
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
**	22-sep-2000 (mcgem01)
**		More nat and longnat to i4 changes.
**
**
NEEDLIBS = LIBINGRES
**
*/
#include	<compat.h>
#include	<tr.h>
#include	<pc.h>
#include	<si.h>
#include	<st.h>
#define		ARRAY_SIZE	100
typedef	struct msfc_struct
	{
		int	(*func)();
	} MSFC_LIST;
int	msfc_test0();
int	msfc_test1();
int	msfc_test2();
int	msfc_test3();
int	msfc_test4();
int	msfc_test5();
int	msfc_test6();
int	msfc_test7();
int	msfc_test8();
int	msfc_test9();
int	msfc_test10();
int	msfc_test11();
int	msfc_test12();
int	msfc_test13();
int	msfc_test14();
int	msfc_test15();
int	msfc_test16();
int	msfc_test17();
int	msfc_test18();
int	msfc_test19();
int	msfc_test20();
int	msfc_test21();
int	msfc_test22();
int	msfc_test23();
int	msfc_test24();
int	msfc_test25();
int	msfc_test26();
int	msfc_test27();
int	msfc_test28();
static MSFC_LIST msfc_array[ARRAY_SIZE] = {
			msfc_test0,
			msfc_test1,
			msfc_test2,
			msfc_test3,
			msfc_test4,
			msfc_test5,
			msfc_test6,
			msfc_test7,
			msfc_test8,
			msfc_test9,
			msfc_test10,
			msfc_test11,
			msfc_test12,
			msfc_test13,
			msfc_test14,
			msfc_test15,
			msfc_test16,
			msfc_test17,
			msfc_test18,
			msfc_test19,
			msfc_test20,
			msfc_test21,
			msfc_test22,
			msfc_test23,
			msfc_test24,
			msfc_test25,
			msfc_test26,
			msfc_test27,
			msfc_test28
		};
main(argc, argv)
int	    argc;
char	    *argv[];
  {
    char *dbname;
    int base_num = -1;
    int sess_num = -1;
    int test_num = -1;
    int start_num = -1;
    char copy_file[40];
    char table_name[32];
    CL_SYS_ERR	syserr;
    int		i;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &syserr);
#endif
#ifdef UNIX
    TRset_file(TR_T_OPEN, "stdio", 5, &syserr);
#endif
    if (argc < 5)
    {
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    dbname = argv[1];
    CVal(argv[2], &test_num);
    CVal(argv[3], &base_num);
    CVal(argv[4], &sess_num);
    if (argc == 6)
    {
	CVal(argv[5], &start_num);
    }
    if (base_num <= 0 || test_num <= 0 || test_num > 28 || sess_num <= 0
	 || start_num > test_num)	
    {
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    SIprintf ("start num %d, test_num %d, argc %d \n ", 
	start_num, test_num, argc);
    SIflush (stdout);
    if (start_num <= 0 || start_num > 29)
    {
	SIprintf ("changing start_num \n ");
	SIflush (stdout);
	start_num = 1;
    }
/* # line 176 "msfccrea.qc" */	/* ingres */
    {
      IIingopen(0,dbname,(char *)0);
    }
/* # line 178 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set printqry");
      IIsyncup((char *)0,0);
    }
/* # line 180 "msfccrea.qc" */	/* host code */
    SIprintf ("database name : %s, %d, %d, %d\n", dbname, test_num, base_num,
	sess_num);
    SIflush (stdout);
    for (i = start_num; i <= test_num; i++)
    {
	SIprintf ("I'm in loop %d\n ", i);
	SIflush (stdout);
	(*msfc_array[i].func)(base_num, sess_num);
    }
/* # line 193 "msfccrea.qc" */	/* exit */
    {
      IIexit();
    }
/* # line 194 "msfccrea.qc" */	/* host code */
    PCexit(0);
  }
msfc_usage()
{
    TRdisplay("USAGE: MSFC_CREATE dbname test base session [start]\n");
    TRdisplay("    dbname - database.\n");
    TRdisplay("    test   - maximum test number to executed.\n");
    TRdisplay("    base   - base # of driver.\n");
    TRdisplay("    session - which slave this is.\n");
    TRdisplay("    start - optional: test number to start with (default is 1)\n");
    TRdisplay("\n");
}
msfc_test0(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    SIprintf ("dummy function should never be used\n");
    SIflush (stdout);
  }
msfc_test1(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char rtab_name[16];
    char rtab2_name[16];
    char rtab3_name[16];
    char ctab1_name[16];
    char copy_file[40];
    /*
    ** Fast1
    */
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
    STprintf(rtab2_name, "rtab2_%d_%d", base_num, sess_num);
    STprintf(ctab1_name, "ctab1_%d_%d", base_num, sess_num);
    SIprintf ("msfc_test1: %s, %s, %s\n", rtab_name, rtab2_name,
	ctab1_name);
    SIflush (stdout);
/* # line 239 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIsyncup((char *)0,0);
    }
/* # line 240 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 242 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 243 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 245 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 246 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
  }
/* # line 249 "msfccrea.qc" */	/* host code */
msfc_test2(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char btab1_name[16];
    char btab2_name[16];
    char btab3_name[16];
    char rtab_name[16];
    /*
    ** Fast2
    */
    STprintf(btab1_name, "btab1_%d_%d", base_num, sess_num);
    STprintf(btab2_name, "btab2_%d_%d", base_num, sess_num);
    STprintf(btab3_name, "btab3_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
    SIprintf ("msfc_test2: %s, %s, %s, %s\n", btab1_name, btab2_name,
	btab3_name, rtab_name);
    SIflush (stdout);
/* # line 271 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 272 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 274 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 275 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 277 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 278 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 281 "msfccrea.qc" */	/* host code */
msfc_test3(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char btab9_name[16];
    char btab10_name[16];
    char rtab_name[16];
    /*
    ** Fast3
    */
    STprintf(btab9_name, "btab9_%d_%d", base_num, sess_num);
    STprintf(btab10_name, "btab10_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
    SIprintf ("msfc_test3: %s, %s, %s\n", btab9_name, btab10_name,
	rtab_name);
    SIflush (stdout);
/* # line 300 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 301 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 303 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 304 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 307 "msfccrea.qc" */	/* host code */
msfc_test4(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char htab1_name[16];
    char htab2_name[16];
    char htab3_name[16];
    char htab9_name[16];
    char htab10_name[16];
    char rtab_name[16];
    /*
    ** Fast4
    */
    STprintf(htab1_name, "htab1_%d_%d", base_num, sess_num);
    STprintf(htab2_name, "htab2_%d_%d", base_num, sess_num);
    STprintf(htab3_name, "htab3_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
    STprintf(htab9_name, "htab9_%d_%d", base_num, sess_num);
    STprintf(htab10_name, "htab10_%d_%d", base_num, sess_num);
    SIprintf ("msfc_test4: %s, %s, %s, %s, %s, %s\n", htab1_name, htab2_name,
	htab3_name, htab9_name, htab10_name, rtab_name);
    SIflush (stdout);
    /*
    ** Do same queries with HASH, ISAM, HEAP tables
    */
/* # line 335 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 336 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 338 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 339 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 341 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 342 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 344 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 345 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 347 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 348 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 351 "msfccrea.qc" */	/* host code */
msfc_test5(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char itab1_name[16];
    char itab2_name[16];
    char itab3_name[16];
    char itab9_name[16];
    char itab10_name[16];
    char htab1_name[16];
    char rtab_name[16];
    /*
    ** Fast5
    */
    STprintf(itab1_name, "itab1_%d_%d", base_num, sess_num);
    STprintf(itab2_name, "itab2_%d_%d", base_num, sess_num);
    STprintf(itab3_name, "itab3_%d_%d", base_num, sess_num);
    STprintf(itab9_name, "itab9_%d_%d", base_num, sess_num);
    STprintf(itab10_name, "itab10_%d_%d", base_num, sess_num);
    STprintf(htab1_name, "htab1_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
/* # line 375 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 376 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 378 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 379 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 381 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 382 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 384 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 385 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 387 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 388 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 391 "msfccrea.qc" */	/* host code */
msfc_test6(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char stab1_name[16];
    char stab2_name[16];
    char stab3_name[16];
    char stab9_name[16];
    char stab10_name[16];
    char rtab_name[16];
    /*
    ** Fast6
    */
    STprintf(stab1_name, "stab1_%d_%d", base_num, sess_num);
    STprintf(stab2_name, "stab2_%d_%d", base_num, sess_num);
    STprintf(stab3_name, "stab3_%d_%d", base_num, sess_num);
    STprintf(stab9_name, "stab9_%d_%d", base_num, sess_num);
    STprintf(stab10_name, "stab10_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
/* # line 413 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 414 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 416 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 417 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 419 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 420 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 422 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 423 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 425 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 426 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 429 "msfccrea.qc" */	/* host code */
msfc_test7(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char btab4_name[16];
    char btab5_name[16];
    char btab6_name[16];
    char btab7_name[16];
    char btab8_name[16];
    char rtab_name[16];
    /*
    ** Fast7
    */
    STprintf(btab4_name, "btab4_%d_%d", base_num, sess_num);
    STprintf(btab5_name, "btab5_%d_%d", base_num, sess_num);
    STprintf(btab6_name, "btab6_%d_%d", base_num, sess_num);
    STprintf(btab7_name, "btab7_%d_%d", base_num, sess_num);
    STprintf(btab8_name, "btab8_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
    /*
    ** Try non - duplicate key
    */
/* # line 453 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 454 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 455 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 456 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 457 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 459 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 460 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 461 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 462 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 463 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 466 "msfccrea.qc" */	/* host code */
msfc_test8(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char htab4_name[16];
    char htab5_name[16];
    char htab6_name[16];
    char htab7_name[16];
    char htab8_name[16];
    char rtab_name[16];
    /*
    ** Fast8
    */
    STprintf(htab4_name, "htab4_%d_%d", base_num, sess_num);
    STprintf(htab5_name, "htab5_%d_%d", base_num, sess_num);
    STprintf(htab6_name, "htab6_%d_%d", base_num, sess_num);
    STprintf(htab7_name, "htab7_%d_%d", base_num, sess_num);
    STprintf(htab8_name, "htab8_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
/* # line 487 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 488 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 490 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 491 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 493 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 494 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 496 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 497 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 499 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 500 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 503 "msfccrea.qc" */	/* host code */
msfc_test9(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char itab4_name[16];
    char itab5_name[16];
    char itab6_name[16];
    char itab7_name[16];
    char itab8_name[16];
    char rtab_name[16];
    /*
    ** Fast9
    */
    STprintf(itab4_name, "itab4_%d_%d", base_num, sess_num);
    STprintf(itab5_name, "itab5_%d_%d", base_num, sess_num);
    STprintf(itab6_name, "itab6_%d_%d", base_num, sess_num);
    STprintf(itab7_name, "itab7_%d_%d", base_num, sess_num);
    STprintf(itab8_name, "itab8_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
/* # line 524 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 525 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 527 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 528 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 530 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 531 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 533 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 534 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 536 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 537 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 540 "msfccrea.qc" */	/* host code */
msfc_test10(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char stab4_name[16];
    char stab5_name[16];
    char stab6_name[16];
    char stab7_name[16];
    char stab8_name[16];
    char rtab_name[16];
    /*
    ** Fast10
    */
    STprintf(stab4_name, "stab4_%d_%d", base_num, sess_num);
    STprintf(stab5_name, "stab5_%d_%d", base_num, sess_num);
    STprintf(stab6_name, "stab6_%d_%d", base_num, sess_num);
    STprintf(stab7_name, "stab7_%d_%d", base_num, sess_num);
    STprintf(stab8_name, "stab8_%d_%d", base_num, sess_num);
    STprintf(rtab_name, "rtab_%d_%d", base_num, sess_num);
/* # line 561 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 562 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 564 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 565 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 567 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 568 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 570 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 571 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 573 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 574 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 577 "msfccrea.qc" */	/* host code */
msfc_test11(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char btab11_name[16];
    char btab12_name[16];
    char btab13_name[16];
    char btab14_name[16];
    char rtab2_name[16];
    /*
    ** Fast11
    */
    STprintf(btab11_name, "btab11_%d_%d", base_num, sess_num);
    STprintf(btab12_name, "btab12_%d_%d", base_num, sess_num);
    STprintf(btab13_name, "btab13_%d_%d", base_num, sess_num);
    STprintf(btab14_name, "btab14_%d_%d", base_num, sess_num);
    STprintf(rtab2_name, "rtab2_%d_%d", base_num, sess_num);
    /*
    ** Try duplicate rows
    */
/* # line 599 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 600 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 602 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 603 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 605 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 606 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 608 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 609 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 612 "msfccrea.qc" */	/* host code */
msfc_test12(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char htab11_name[16];
    char htab12_name[16];
    char htab13_name[16];
    char htab14_name[16];
    char rtab2_name[16];
    /*
    ** Fast12
    */
    STprintf(htab11_name, "htab11_%d_%d", base_num, sess_num);
    STprintf(htab12_name, "htab12_%d_%d", base_num, sess_num);
    STprintf(htab13_name, "htab13_%d_%d", base_num, sess_num);
    STprintf(htab14_name, "htab14_%d_%d", base_num, sess_num);
    STprintf(rtab2_name, "rtab2_%d_%d", base_num, sess_num);
    /*
    ** Try duplicate rows - HASH, ISAM, HEAP
    */
/* # line 634 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 635 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 637 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 638 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 640 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 641 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 643 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 644 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 647 "msfccrea.qc" */	/* host code */
msfc_test13(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char itab11_name[16];
    char itab12_name[16];
    char itab13_name[16];
    char itab14_name[16];
    char rtab2_name[16];
    /*
    ** Fast13
    */
    STprintf(itab11_name, "itab11_%d_%d", base_num, sess_num);
    STprintf(itab12_name, "itab12_%d_%d", base_num, sess_num);
    STprintf(itab13_name, "itab13_%d_%d", base_num, sess_num);
    STprintf(itab14_name, "itab14_%d_%d", base_num, sess_num);
    STprintf(rtab2_name, "rtab2_%d_%d", base_num, sess_num);
/* # line 666 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 667 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 669 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 670 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 672 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 673 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 675 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 676 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 679 "msfccrea.qc" */	/* host code */
msfc_test14(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char stab11_name[16];
    char stab12_name[16];
    char stab13_name[16];
    char stab14_name[16];
    char rtab2_name[16];
    /*
    ** Fast14
    */
    STprintf(stab11_name, "stab11_%d_%d", base_num, sess_num);
    STprintf(stab12_name, "stab12_%d_%d", base_num, sess_num);
    STprintf(stab13_name, "stab13_%d_%d", base_num, sess_num);
    STprintf(stab14_name, "stab14_%d_%d", base_num, sess_num);
    STprintf(rtab2_name, "rtab2_%d_%d", base_num, sess_num);
/* # line 698 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 699 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 701 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 702 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 704 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 705 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 707 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 708 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 711 "msfccrea.qc" */	/* host code */
msfc_test15(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char dtab1_name[16];
    /*
    ** Fast15
    */
    STprintf(dtab1_name, "dtab1_%d_%d", base_num, sess_num);
    /*
    ** append dups, then modify to unique and make sure that
    ** transactions don't attempt to redo.
    */
/* # line 726 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 727 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 730 "msfccrea.qc" */	/* host code */
msfc_test16(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char xtab1_name[16];
    char i1x1_name[16];
    char i1x2_name[16];
    char i1x3_name[16];
    char i1x4_name[16];
    char i1x5_name[16];
    char i1x6_name[16];
    char i1x7_name[16];
    /*
    ** Fast16
    */
    STprintf(xtab1_name, "xtab1_%d_%d", base_num, sess_num);
    STprintf(i1x1_name, "i1x1_%d_%d", base_num, sess_num);
    STprintf(i1x2_name, "i1x2_%d_%d", base_num, sess_num);
    STprintf(i1x3_name, "i1x3_%d_%d", base_num, sess_num);
    STprintf(i1x4_name, "i1x4_%d_%d", base_num, sess_num);
    STprintf(i1x5_name, "i1x5_%d_%d", base_num, sess_num);
    STprintf(i1x6_name, "i1x6_%d_%d", base_num, sess_num);
    STprintf(i1x7_name, "i1x7_%d_%d", base_num, sess_num);
    /*
    ** Secondary index tests.
    */
/* # line 758 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 759 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=i4)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 761 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 762 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 763 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 764 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 765 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 766 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 767 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 768 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 771 "msfccrea.qc" */	/* host code */
msfc_test17(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char xtab2_name[16];
    char i2x1_name[16];
    char i2x2_name[16];
    char i2x3_name[16];
    char i2x4_name[16];
    char i2x5_name[16];
    char i2x6_name[16];
    char i2x7_name[16];
    /*
    ** Fast17
    */
    STprintf(xtab2_name, "xtab2_%d_%d", base_num, sess_num);
    STprintf(i2x1_name, "i2x1_%d_%d", base_num, sess_num);
    STprintf(i2x2_name, "i2x2_%d_%d", base_num, sess_num);
    STprintf(i2x3_name, "i2x3_%d_%d", base_num, sess_num);
    STprintf(i2x4_name, "i2x4_%d_%d", base_num, sess_num);
    STprintf(i2x5_name, "i2x5_%d_%d", base_num, sess_num);
    STprintf(i2x6_name, "i2x6_%d_%d", base_num, sess_num);
    STprintf(i2x7_name, "i2x7_%d_%d", base_num, sess_num);
/* # line 796 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 797 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=i4)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 799 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 800 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 801 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 802 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 803 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 804 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 805 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 806 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 810 "msfccrea.qc" */	/* host code */
msfc_test18(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char xtab3_name[16];
    char i3x1_name[16];
    char i3x2_name[16];
    char i3x3_name[16];
    char i3x4_name[16];
    char i3x5_name[16];
    char i3x6_name[16];
    char i3x7_name[16];
    /*
    ** Fast18
    */
    STprintf(xtab3_name, "xtab3_%d_%d", base_num, sess_num);
    STprintf(i3x1_name, "i3x1_%d_%d", base_num, sess_num);
    STprintf(i3x2_name, "i3x2_%d_%d", base_num, sess_num);
    STprintf(i3x3_name, "i3x3_%d_%d", base_num, sess_num);
    STprintf(i3x4_name, "i3x4_%d_%d", base_num, sess_num);
    STprintf(i3x5_name, "i3x5_%d_%d", base_num, sess_num);
    STprintf(i3x6_name, "i3x6_%d_%d", base_num, sess_num);
    STprintf(i3x7_name, "i3x7_%d_%d", base_num, sess_num);
/* # line 835 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 836 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=i4)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 838 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 839 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 840 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 841 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 842 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 843 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 844 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 845 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 849 "msfccrea.qc" */	/* host code */
msfc_test19(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char xtab4_name[16];
    char i4x1_name[16];
    char i4x2_name[16];
    char i4x3_name[16];
    char i4x4_name[16];
    char i4x5_name[16];
    char i4x6_name[16];
    char i4x7_name[16];
    /*
    ** Fast19
    */
    STprintf(xtab4_name, "xtab4_%d_%d", base_num, sess_num);
    STprintf(i4x1_name, "i4x1_%d_%d", base_num, sess_num);
    STprintf(i4x2_name, "i4x2_%d_%d", base_num, sess_num);
    STprintf(i4x3_name, "i4x3_%d_%d", base_num, sess_num);
    STprintf(i4x4_name, "i4x4_%d_%d", base_num, sess_num);
    STprintf(i4x5_name, "i4x5_%d_%d", base_num, sess_num);
    STprintf(i4x6_name, "i4x6_%d_%d", base_num, sess_num);
    STprintf(i4x7_name, "i4x7_%d_%d", base_num, sess_num);
/* # line 874 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 875 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=i4)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 877 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 878 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 879 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 880 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 881 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 882 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 883 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 884 "msfccrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 888 "msfccrea.qc" */	/* host code */
msfc_test20(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char ctab1_name[16];
    char ctab2_name[16];
    char ctab3_name[16];
    char ctab4_name[16];
    char ctab5_name[16];
    char ctab6_name[16];
    char ctab7_name[16];
    char ctab8_name[16];
    char ctab9_name[16];
    char copy_file[40];
    /*
    ** Fast20
    */
    STprintf(ctab1_name, "ctab1_%d_%d", base_num, sess_num);
    STprintf(ctab2_name, "ctab2_%d_%d", base_num, sess_num);
    STprintf(ctab3_name, "ctab3_%d_%d", base_num, sess_num);
    STprintf(ctab4_name, "ctab4_%d_%d", base_num, sess_num);
    STprintf(ctab5_name, "ctab5_%d_%d", base_num, sess_num);
    STprintf(ctab6_name, "ctab6_%d_%d", base_num, sess_num);
    STprintf(ctab7_name, "ctab7_%d_%d", base_num, sess_num);
    STprintf(ctab8_name, "ctab8_%d_%d", base_num, sess_num);
    STprintf(ctab9_name, "ctab9_%d_%d", base_num, sess_num);
    STprintf(copy_file, "MSFC_DATA:ctab_%d_%d.in", base_num, sess_num);
/* # line 917 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 918 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
/* # line 920 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 921 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
/* # line 923 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 924 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
/* # line 926 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 927 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
/* # line 929 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 930 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 931 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into hash");
      IIsyncup((char *)0,0);
    }
/* # line 932 "msfccrea.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 934 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 935 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into isam");
      IIsyncup((char *)0,0);
    }
/* # line 936 "msfccrea.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 938 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 939 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into btree");
      IIsyncup((char *)0,0);
    }
/* # line 940 "msfccrea.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 942 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 943 "msfccrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into heap");
      IIsyncup((char *)0,0);
    }
/* # line 944 "msfccrea.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 947 "msfccrea.qc" */	/* host code */
msfc_test21(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char ttab1_name[16];
    /*
    ** Fast21
    */
    STprintf(ttab1_name, "ttab1_%d_%d", base_num, sess_num);
/* # line 958 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 959 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
  }
/* # line 962 "msfccrea.qc" */	/* host code */
msfc_test22(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char mtab1_name[16];
    char mtab2_name[16];
    char mtab3_name[16];
    /*
    ** Fast22
    */
    STprintf(mtab1_name, "mtab1_%d_%d", base_num, sess_num);
    STprintf(mtab2_name, "mtab2_%d_%d", base_num, sess_num);
    STprintf(mtab3_name, "mtab3_%d_%d", base_num, sess_num);
/* # line 977 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 978 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 979 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 980 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)with journalin\
g");
      IIsyncup((char *)0,0);
    }
/* # line 981 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)with journalin\
g");
      IIsyncup((char *)0,0);
    }
/* # line 982 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)with journalin\
g");
      IIsyncup((char *)0,0);
    }
  }
/* # line 986 "msfccrea.qc" */	/* host code */
msfc_test23(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char comptab1_name[16];
    char *large_data1;
    char	blah_data[1001];
    /*
    ** Fast23
    */
    STprintf(comptab1_name, "comptab1_%d_%d", base_num, sess_num);
/* # line 999 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1000 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)w\
ith journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1005 "msfccrea.qc" */	/* host code */
msfc_test24(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char comptab2_name[16];
    char *large_data2;
    char	blah_data[1001];
    /*
    ** Fast24
    */
    STprintf(comptab2_name, "comptab2_%d_%d", base_num, sess_num);
/* # line 1018 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1019 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)w\
ith journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1024 "msfccrea.qc" */	/* host code */
msfc_test25(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char comptab3_name[16];
    char *large_data3;
    char	blah_data[1001];
    /*
    ** Fast25
    */
    STprintf(comptab3_name, "comptab3_%d_%d", base_num, sess_num);
/* # line 1037 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1038 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)w\
ith journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1043 "msfccrea.qc" */	/* host code */
msfc_test26(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char comptab4_name[16];
    char *large_data4;
    char	blah_data[1001];
    /*
    ** Fast26
    */
    STprintf(comptab4_name, "comptab4_%d_%d", base_num, sess_num);
/* # line 1056 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1057 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)w\
ith journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1062 "msfccrea.qc" */	/* host code */
msfc_test27(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char concurtab1_name[16];
    char concurtab2_name[16];
    char concurtab3_name[16];
    char concurtab4_name[16];
    char concurtab5_name[16];
    char concurtab6_name[16];
    /*
    ** Fast27
    */
    STprintf(concurtab1_name, "concurtab1_%d", base_num);
    STprintf(concurtab2_name, "concurtab2_%d", base_num);
    STprintf(concurtab3_name, "concurtab3_%d", base_num);
    STprintf(concurtab4_name, "concurtab4_%d", base_num);
    STprintf(concurtab5_name, "concurtab5_%d", base_num);
    STprintf(concurtab6_name, "concurtab6_%d", base_num);
/* # line 1084 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1085 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1086 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1087 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1088 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 1089 "msfccrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 1091 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1092 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1093 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1094 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1095 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1096 "msfccrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1100 "msfccrea.qc" */	/* host code */
msfc_test28(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char concrtab1_name[16];
    char concrtab2_name[16];
    char concrtab3_name[16];
    char concrtab4_name[16];
    char concrtab5_name[16];
    char concrtab6_name[16];
    long num;
    i4     	i;
    /*
    ** Fast28
    */
    STprintf(concrtab1_name, "concurtab1_%d", base_num);
    STprintf(concrtab2_name, "concurtab2_%d", base_num);
    STprintf(concrtab3_name, "concurtab3_%d", base_num);
    STprintf(concrtab4_name, "concurtab4_%d", base_num);
    STprintf(concrtab5_name, "concurtab5_%d", base_num);
    STprintf(concrtab6_name, "concurtab6_%d", base_num);
  }
