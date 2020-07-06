# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  ROLL_CREATE	    - run fast commit test
**
**  This program executes the fast commit tests.
**  It will execute only one test at a time, as the server is crashed at the
**  end of each test.
**
**  It will request an installation lock using the DMF trace point DM1420
**  before crashing in order to sync up with the test driver.
**
**  Before requesting the DM1420 trace point, it will create a copy file
**  in the ROLL_DATA directory identifying itself as ready to crash.
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
**      22-sep-2000 (mcgem01)
**              More nat and longnat to i4 changes.
**
**
NEEDLIBS = LIBINGRES
**
*/
#include        <compat.h>
#include        <tr.h>
#include        <pc.h>
#include        <si.h>
#include        <st.h>
#define		ARRAY_SIZE	100
typedef struct roll_struct
	{
		int	(*func)();
	} ROLL_LIST;
int	roll_test0();
int	roll_test1();
int	roll_test2();
int	roll_test3();
int	roll_test4();
int	roll_test5();
int	roll_test6();
int	roll_test7();
int	roll_test8();
int	roll_test9();
int	roll_test10();
int	roll_test11();
int	roll_test12();
int	roll_test13();
int	roll_test14();
int	roll_test15();
int	roll_test16();
int	roll_test17();
int	roll_test18();
int	roll_test19();
int	roll_test20();
int	roll_test21();
int	roll_test22();
int	roll_test23();
int	roll_test24();
int	roll_test25();
int	roll_test26();
int	roll_test27();
int	roll_test28();
static ROLL_LIST roll_array[ARRAY_SIZE] =
		{
			roll_test0,
			roll_test1,
			roll_test2,
			roll_test3,
			roll_test4,
			roll_test5,
			roll_test6,
			roll_test7,
			roll_test8,
			roll_test9,
			roll_test10,
			roll_test11,
			roll_test12,
			roll_test13,
			roll_test14,
			roll_test15,
			roll_test16,
			roll_test17,
			roll_test18,
			roll_test19,
			roll_test20,
			roll_test21,
			roll_test22,
			roll_test23,
			roll_test24,
			roll_test25,
			roll_test26,
			roll_test27,
			roll_test28
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
int	i;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &syserr);
#endif
#ifdef UNIX
    TRset_file(TR_T_OPEN, "stdio", 5, &syserr);
#endif
    if (argc < 5)
    {
	roll_usage();
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
	roll_usage();
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
/* # line 178 "rollcrea.qc" */	/* ingres */
    {
      IIingopen(0,dbname,(char *)0);
    }
/* # line 180 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set printqry");
      IIsyncup((char *)0,0);
    }
/* # line 182 "rollcrea.qc" */	/* host code */
    SIprintf ("database name : %s, %d, %d, %d\n", dbname, test_num, base_num,
	sess_num);
    SIflush (stdout);
    for (i = start_num; i <= test_num; i++)
    {
	SIprintf ("I'm in loop %d\n ", i);
	SIflush (stdout);
	(*roll_array[i].func)(base_num, sess_num);
    }
/* # line 195 "rollcrea.qc" */	/* exit */
    {
      IIexit();
    }
/* # line 196 "rollcrea.qc" */	/* host code */
    PCexit(0);
  }
roll_usage()
{
    TRdisplay("USAGE: ROLL_CREATE dbname test base session [start]\n");
    TRdisplay("    dbname - database.\n");
    TRdisplay("    test   - maximum test number to executed.\n");
    TRdisplay("    base   - base # of driver.\n");
    TRdisplay("    session - which slave this is.\n");
    TRdisplay("    start - optional: test number to start with (default is 1)\n");
    TRdisplay("\n");
}
roll_test0(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    SIprintf ("dummy function should never be used\n");
    SIflush (stdout);
  }
roll_test1(base_num, sess_num)
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
    SIprintf ("roll_test1: %s, %s, %s\n", rtab_name, rtab2_name,
	ctab1_name);
    SIflush (stdout);
/* # line 241 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIsyncup((char *)0,0);
    }
/* # line 242 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 244 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 245 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 247 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 248 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
  }
/* # line 251 "rollcrea.qc" */	/* host code */
roll_test2(base_num, sess_num)
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
    SIprintf ("roll_test2: %s, %s, %s, %s\n", btab1_name, btab2_name,
	btab3_name, rtab_name);
    SIflush (stdout);
/* # line 273 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 274 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 276 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 277 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 279 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 280 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 283 "rollcrea.qc" */	/* host code */
roll_test3(base_num, sess_num)
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
    SIprintf ("roll_test3: %s, %s, %s\n", btab9_name, btab10_name,
	rtab_name);
    SIflush (stdout);
/* # line 302 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 303 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 305 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 306 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 309 "rollcrea.qc" */	/* host code */
roll_test4(base_num, sess_num)
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
    SIprintf ("roll_test4: %s, %s, %s, %s, %s, %s\n", htab1_name, htab2_name,
	htab3_name, htab9_name, htab10_name, rtab_name);
    SIflush (stdout);
    /*
    ** Do same queries with HASH, ISAM, HEAP tables
    */
/* # line 337 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 338 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 340 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 341 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 343 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 344 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 346 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 347 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 349 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 350 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 353 "rollcrea.qc" */	/* host code */
roll_test5(base_num, sess_num)
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
/* # line 377 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 378 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 380 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 381 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 383 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 384 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 386 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 387 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 389 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 390 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 393 "rollcrea.qc" */	/* host code */
roll_test6(base_num, sess_num)
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
/* # line 415 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 416 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 418 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 419 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 421 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 422 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 424 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 425 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 427 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 428 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 431 "rollcrea.qc" */	/* host code */
roll_test7(base_num, sess_num)
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
/* # line 455 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 456 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 457 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 458 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 459 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 461 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 462 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 463 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 464 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 465 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 468 "rollcrea.qc" */	/* host code */
roll_test8(base_num, sess_num)
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
/* # line 489 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 490 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 492 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 493 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 495 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 496 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 498 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 499 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 501 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 502 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 505 "rollcrea.qc" */	/* host code */
roll_test9(base_num, sess_num)
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
/* # line 526 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 527 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 529 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 530 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 532 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 533 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 535 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 536 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 538 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 539 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 542 "rollcrea.qc" */	/* host code */
roll_test10(base_num, sess_num)
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
/* # line 563 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 564 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 566 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 567 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 569 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 570 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 572 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 573 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 575 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 576 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 579 "rollcrea.qc" */	/* host code */
roll_test11(base_num, sess_num)
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
/* # line 601 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 602 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 604 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 605 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 607 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 608 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 610 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 611 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 614 "rollcrea.qc" */	/* host code */
roll_test12(base_num, sess_num)
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
/* # line 636 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 637 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 639 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 640 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 642 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 643 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 645 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 646 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 649 "rollcrea.qc" */	/* host code */
roll_test13(base_num, sess_num)
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
/* # line 668 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 669 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 671 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 672 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 674 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 675 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 677 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 678 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 681 "rollcrea.qc" */	/* host code */
roll_test14(base_num, sess_num)
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
/* # line 700 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 701 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 703 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 704 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 706 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 707 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
/* # line 709 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 710 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates, journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 713 "rollcrea.qc" */	/* host code */
roll_test15(base_num, sess_num)
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
/* # line 728 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 729 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 732 "rollcrea.qc" */	/* host code */
roll_test16(base_num, sess_num)
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
/* # line 760 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 761 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=i4)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 763 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 764 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 765 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 766 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 767 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 768 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 769 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 770 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 773 "rollcrea.qc" */	/* host code */
roll_test17(base_num, sess_num)
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
/* # line 798 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 799 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=i4)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 801 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 802 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 803 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 804 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 805 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 806 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 807 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 808 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 812 "rollcrea.qc" */	/* host code */
roll_test18(base_num, sess_num)
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
/* # line 837 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 838 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=i4)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 840 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 841 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 842 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 843 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 844 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 845 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 846 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 847 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 851 "rollcrea.qc" */	/* host code */
roll_test19(base_num, sess_num)
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
/* # line 876 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 877 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=i4)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 879 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 880 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 881 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 882 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 883 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 884 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 885 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 886 "rollcrea.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 890 "rollcrea.qc" */	/* host code */
roll_test20(base_num, sess_num)
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
    STprintf(copy_file, "ROLL_DATA:ctab_%d_%d.in", base_num, sess_num);
/* # line 919 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 920 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
/* # line 922 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 923 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
/* # line 925 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 926 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
/* # line 928 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 929 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
/* # line 931 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 932 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set journaling");
      IIsyncup((char *)0,0);
    }
/* # line 933 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into hash");
      IIsyncup((char *)0,0);
    }
/* # line 934 "rollcrea.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 936 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 937 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into isam");
      IIsyncup((char *)0,0);
    }
/* # line 938 "rollcrea.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 940 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 941 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into btree");
      IIsyncup((char *)0,0);
    }
/* # line 942 "rollcrea.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 944 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 945 "rollcrea.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into heap");
      IIsyncup((char *)0,0);
    }
/* # line 946 "rollcrea.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 949 "rollcrea.qc" */	/* host code */
roll_test21(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char ttab1_name[16];
    /*
    ** Fast21
    */
    STprintf(ttab1_name, "ttab1_%d_%d", base_num, sess_num);
/* # line 960 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 961 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)with journali\
ng");
      IIsyncup((char *)0,0);
    }
  }
/* # line 964 "rollcrea.qc" */	/* host code */
roll_test22(base_num, sess_num)
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
/* # line 979 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 980 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 981 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 982 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)with journalin\
g");
      IIsyncup((char *)0,0);
    }
/* # line 983 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)with journalin\
g");
      IIsyncup((char *)0,0);
    }
/* # line 984 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)with journalin\
g");
      IIsyncup((char *)0,0);
    }
  }
/* # line 988 "rollcrea.qc" */	/* host code */
roll_test23(base_num, sess_num)
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
/* # line 1001 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1002 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)w\
ith journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1007 "rollcrea.qc" */	/* host code */
roll_test24(base_num, sess_num)
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
/* # line 1020 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1021 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)w\
ith journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1026 "rollcrea.qc" */	/* host code */
roll_test25(base_num, sess_num)
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
/* # line 1039 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1040 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)w\
ith journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1045 "rollcrea.qc" */	/* host code */
roll_test26(base_num, sess_num)
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
/* # line 1058 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1059 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)w\
ith journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1064 "rollcrea.qc" */	/* host code */
roll_test27(base_num, sess_num)
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
/* # line 1086 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1087 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1088 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1089 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1090 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 1091 "rollcrea.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 1093 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1094 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1095 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1096 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1097 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
/* # line 1098 "rollcrea.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=i4, c=c100)with journaling");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1102 "rollcrea.qc" */	/* host code */
roll_test28(base_num, sess_num)
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
