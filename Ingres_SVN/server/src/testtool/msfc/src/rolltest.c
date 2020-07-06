# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  ROLL_TEST	    - run fast commit test
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
**	16-aug-1999 (somsa01)
**		Changed nat and longnat to i4.
**	07-dec-1999 (somsa01)
**		Include me.h .
**      13-Jan-98 (fanra01)
**              Add me header for MEcopy.
**      22-sep-2000 (mcgem01)
**              More nat and longnat to i4 changes.
**
**
NEEDLIBS = LIBINGRES
**
*/
#include        <compat.h>
#include        <me.h>
#include        <tr.h>
#include        <pc.h>
#include        <st.h>
#include        <me.h>
main(argc, argv)
int	    argc;
char	    *argv[];
  {
    char *dbname;
    int base_num = -1;
    int sess_num = -1;
    int test_num = -1;
    char copy_file[40];
    char table_name[32];
    CL_SYS_ERR	syserr;
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
    if (base_num < 0 || test_num < 0 || test_num > 28 || sess_num < 0)
    {
	roll_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    if (test_num == 0)
    {
	roll_canon(dbname, base_num, sess_num);
	PCexit(0);
    }
/* # line 102 "rolltest.qc" */	/* ingres */
    {
      IIingopen(0,dbname,(char *)0);
    }
/* # line 104 "rolltest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set printqry");
      IIsyncup((char *)0,0);
    }
/* # line 106 "rolltest.qc" */	/* host code */
    switch(test_num)
    {
      case  1:  roll_test1(base_num, sess_num); break;
      case  2:  roll_test2(base_num, sess_num); break;
      case  3:  roll_test3(base_num, sess_num); break;
      case  4:  roll_test4(base_num, sess_num); break;
      case  5:  roll_test5(base_num, sess_num); break;
      case  6:  roll_test6(base_num, sess_num); break;
      case  7:  roll_test7(base_num, sess_num); break;
      case  8:  roll_test8(base_num, sess_num); break;
      case  9:  roll_test9(base_num, sess_num); break;
      case 10:  roll_test10(base_num, sess_num); break;
      case 11:  roll_test11(base_num, sess_num); break;
      case 12:  roll_test12(base_num, sess_num); break;
      case 13:  roll_test13(base_num, sess_num); break;
      case 14:  roll_test14(base_num, sess_num); break;
      case 15:  roll_test15(base_num, sess_num); break;
      case 16:  roll_test16(base_num, sess_num); break;
      case 17:  roll_test17(base_num, sess_num); break;
      case 18:  roll_test18(base_num, sess_num); break;
      case 19:  roll_test19(base_num, sess_num); break;
      case 20:  roll_test20(base_num, sess_num); break;
      case 21:  roll_test21(base_num, sess_num); break;
      case 22:  roll_test22(base_num, sess_num); break;
      case 23:  roll_test23(base_num, sess_num); break;
      case 24:  roll_test24(base_num, sess_num); break;
      case 25:  roll_test25(base_num, sess_num); break;
      case 26:  roll_test26(base_num, sess_num); break;
      case 27:  roll_test27(base_num, sess_num); break;
      case 28:  roll_test28(base_num, sess_num); break;
      default:
	TRdisplay("Unknown test number - %d\n", test_num);
    }
    /*
    ** Create file so driver knows we are ready for server crash.
    */
    STprintf(table_name, "roll_temp_tab_%d_%d", base_num, sess_num);
    STprintf(copy_file, "ROLL_DATA:roll_copy_%d_%d.in", base_num, sess_num);
/* # line 145 "rolltest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 146 "rolltest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=c30, b=c20)");
      IIsyncup((char *)0,0);
    }
/* # line 147 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIputdomio((short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)", b=");
      IIputdomio((short *)0,1,32,0,dbname);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 148 "rolltest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=c30, b=c20, d=d0nl)into\
 ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 149 "rolltest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 151 "rolltest.qc" */	/* host code */
    /*
    ** Wait for sync lock - driver will release this when all fast commit
    ** tests have reached this point.
    */
/* # line 155 "rolltest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set trace point DM1420");
      IIsyncup((char *)0,0);
    }
/* # line 157 "rolltest.qc" */	/* host code */
    /*
    ** Crash server
    */
/* # line 160 "rolltest.qc" */	/* sleep */
    {
      IIsleep(5);
    }
/* # line 161 "rolltest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set trace point SC908");
      IIsyncup((char *)0,0);
    }
/* # line 163 "rolltest.qc" */	/* exit */
    {
      IIexit();
    }
/* # line 164 "rolltest.qc" */	/* host code */
    PCexit(0);
  }
roll_usage()
{
    TRdisplay("USAGE: ROLL_TEST dbname test base session\n");
    TRdisplay("    dbname - database.\n");
    TRdisplay("    test   - test number to execute.\n");
    TRdisplay("    base   - base # of driver.\n");
    TRdisplay("    session - which slave this is.\n");
    TRdisplay("\n");
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
/* # line 194 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIsyncup((char *)0,0);
    }
/* # line 195 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=1)");
      IIsyncup((char *)0,0);
    }
/* # line 196 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=2)");
      IIsyncup((char *)0,0);
    }
/* # line 197 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=3)");
      IIsyncup((char *)0,0);
    }
/* # line 198 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=4)");
      IIsyncup((char *)0,0);
    }
/* # line 199 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=5)");
      IIsyncup((char *)0,0);
    }
/* # line 200 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=6)");
      IIsyncup((char *)0,0);
    }
/* # line 201 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=7)");
      IIsyncup((char *)0,0);
    }
/* # line 202 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=8)");
      IIsyncup((char *)0,0);
    }
/* # line 203 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=9)");
      IIsyncup((char *)0,0);
    }
/* # line 204 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=10)");
      IIsyncup((char *)0,0);
    }
/* # line 205 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=11)");
      IIsyncup((char *)0,0);
    }
/* # line 206 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=12)");
      IIsyncup((char *)0,0);
    }
/* # line 207 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=13)");
      IIsyncup((char *)0,0);
    }
/* # line 208 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=14)");
      IIsyncup((char *)0,0);
    }
/* # line 209 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=15)");
      IIsyncup((char *)0,0);
    }
/* # line 210 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=16)");
      IIsyncup((char *)0,0);
    }
/* # line 211 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=17)");
      IIsyncup((char *)0,0);
    }
/* # line 212 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=18)");
      IIsyncup((char *)0,0);
    }
/* # line 213 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=19)");
      IIsyncup((char *)0,0);
    }
/* # line 214 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=20)");
      IIsyncup((char *)0,0);
    }
/* # line 216 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 217 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 218 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 219 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 220 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 221 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 222 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 223 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 224 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 225 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 226 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 227 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 228 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 229 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 230 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 231 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 232 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 233 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 234 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 235 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 237 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 238 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 239 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1)");
      IIsyncup((char *)0,0);
    }
/* # line 240 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2)");
      IIsyncup((char *)0,0);
    }
/* # line 241 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+2)");
      IIsyncup((char *)0,0);
    }
/* # line 242 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+4)");
      IIsyncup((char *)0,0);
    }
/* # line 243 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+8)");
      IIsyncup((char *)0,0);
    }
/* # line 244 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16)");
      IIsyncup((char *)0,0);
    }
/* # line 245 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32)");
      IIsyncup((char *)0,0);
    }
/* # line 246 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+64)");
      IIsyncup((char *)0,0);
    }
/* # line 247 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+128)");
      IIsyncup((char *)0,0);
    }
/* # line 248 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+256)");
      IIsyncup((char *)0,0);
    }
/* # line 249 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+512)");
      IIsyncup((char *)0,0);
    }
/* # line 251 "rolltest.qc" */	/* host code */
    STprintf(copy_file, "ROLL_DATA:ctab_%d_%d.in", base_num, sess_num);
/* # line 252 "rolltest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()into ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 254 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 257 "rolltest.qc" */	/* host code */
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
    /*
    ** Append rows that will cause additions to ovfl leaf chains of btree
    */
/* # line 278 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 279 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 280 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=1)");
      IIsyncup((char *)0,0);
    }
/* # line 281 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=2)");
      IIsyncup((char *)0,0);
    }
/* # line 282 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=3)");
      IIsyncup((char *)0,0);
    }
/* # line 283 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=4)");
      IIsyncup((char *)0,0);
    }
/* # line 284 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=5)");
      IIsyncup((char *)0,0);
    }
/* # line 285 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=6)");
      IIsyncup((char *)0,0);
    }
/* # line 286 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=7)");
      IIsyncup((char *)0,0);
    }
/* # line 287 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=8)");
      IIsyncup((char *)0,0);
    }
/* # line 288 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=9)");
      IIsyncup((char *)0,0);
    }
/* # line 289 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=10)");
      IIsyncup((char *)0,0);
    }
/* # line 290 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=11)");
      IIsyncup((char *)0,0);
    }
/* # line 291 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=12)");
      IIsyncup((char *)0,0);
    }
/* # line 292 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=13)");
      IIsyncup((char *)0,0);
    }
/* # line 293 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=14)");
      IIsyncup((char *)0,0);
    }
/* # line 294 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=15)");
      IIsyncup((char *)0,0);
    }
/* # line 295 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=16)");
      IIsyncup((char *)0,0);
    }
/* # line 296 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=17)");
      IIsyncup((char *)0,0);
    }
/* # line 297 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=18)");
      IIsyncup((char *)0,0);
    }
/* # line 298 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=19)");
      IIsyncup((char *)0,0);
    }
/* # line 299 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=20)");
      IIsyncup((char *)0,0);
    }
/* # line 300 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=1)");
      IIsyncup((char *)0,0);
    }
/* # line 301 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=2)");
      IIsyncup((char *)0,0);
    }
/* # line 302 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=3)");
      IIsyncup((char *)0,0);
    }
/* # line 303 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=4)");
      IIsyncup((char *)0,0);
    }
/* # line 304 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=5)");
      IIsyncup((char *)0,0);
    }
/* # line 305 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=6)");
      IIsyncup((char *)0,0);
    }
/* # line 306 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=7)");
      IIsyncup((char *)0,0);
    }
/* # line 307 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=8)");
      IIsyncup((char *)0,0);
    }
/* # line 308 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=9)");
      IIsyncup((char *)0,0);
    }
/* # line 309 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=10)");
      IIsyncup((char *)0,0);
    }
/* # line 311 "rolltest.qc" */	/* host code */
    /*
    ** Delete rows on ovfl leaf chains
    */
/* # line 314 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 315 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 316 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 317 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 319 "rolltest.qc" */	/* host code */
    /*
    ** Replace rows on ovfl leaf chains
    */
/* # line 322 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 323 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 324 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 325 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 328 "rolltest.qc" */	/* host code */
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
/* # line 343 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 344 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 345 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 346 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+400)where ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c>0");
      IIsyncup((char *)0,0);
    }
/* # line 348 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 349 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 350 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 351 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)where ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c>0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 354 "rolltest.qc" */	/* host code */
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
    /*
    ** Do same queries with HASH, ISAM, HEAP tables
    */
/* # line 378 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 379 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 380 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 382 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 383 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 384 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 385 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 387 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 388 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 389 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 390 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 391 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 392 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 394 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 395 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 396 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 397 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+400)where ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c>0");
      IIsyncup((char *)0,0);
    }
/* # line 399 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 400 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 401 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 402 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)where ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c>0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 405 "rolltest.qc" */	/* host code */
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
/* # line 429 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 430 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 431 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 433 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 434 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 435 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 436 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 437 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 438 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 440 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 441 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 442 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 443 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 445 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 446 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 447 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 448 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+400)where ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c>0");
      IIsyncup((char *)0,0);
    }
/* # line 450 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 451 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 452 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 453 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)where ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c>0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 456 "rolltest.qc" */	/* host code */
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
/* # line 478 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 479 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 480 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 482 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 483 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 484 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 485 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 486 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 487 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 489 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 490 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 491 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 492 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 494 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 495 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 496 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 497 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+400)where ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c>0");
      IIsyncup((char *)0,0);
    }
/* # line 499 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 500 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 501 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 502 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 503 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)where ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c>0");
      IIsyncup((char *)0,0);
    }
/* # line 504 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 507 "rolltest.qc" */	/* host code */
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
/* # line 531 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 532 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 533 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 534 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 535 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 537 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 538 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 539 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 541 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 542 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 543 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 545 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 546 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 547 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 549 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 550 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 551 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+400)where ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 553 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 554 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 555 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=400, c=");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)where ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 556 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 559 "rolltest.qc" */	/* host code */
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
/* # line 580 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 581 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 582 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 584 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 585 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 586 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 587 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 589 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 590 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 591 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 592 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 594 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 595 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 596 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 597 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+400)where ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 599 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 600 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 601 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 602 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=400, c=");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)where ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 605 "rolltest.qc" */	/* host code */
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
/* # line 626 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 627 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 628 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 630 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 631 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 632 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 633 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 635 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 636 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 637 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 638 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 640 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 641 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 642 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 643 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+400)where ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 645 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 646 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 647 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 648 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=400, c=");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)where ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 651 "rolltest.qc" */	/* host code */
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
/* # line 672 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 673 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 674 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 676 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 677 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 678 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 679 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 681 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 682 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 683 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 684 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 686 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 687 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 688 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 689 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+400)where ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 691 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 692 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 693 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 694 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=400, c=");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)where ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 697 "rolltest.qc" */	/* host code */
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
/* # line 719 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 720 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 721 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 722 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 723 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 724 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 725 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 726 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 727 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 728 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 729 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 730 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 731 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 732 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 733 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 734 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 735 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 736 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 737 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 738 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 739 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 741 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 742 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 743 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 744 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 746 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 747 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 748 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 749 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)where ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 751 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 752 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 753 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 754 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid)where ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 757 "rolltest.qc" */	/* host code */
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
/* # line 779 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 780 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 781 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 783 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 784 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 785 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 786 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 788 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 789 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 790 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 791 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)where ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 793 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 794 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 795 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 796 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid)where ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 799 "rolltest.qc" */	/* host code */
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
/* # line 818 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 819 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 820 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 822 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 823 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 824 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 825 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 827 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 828 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 829 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 830 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)where ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 832 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 833 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 834 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 835 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid)where ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 838 "rolltest.qc" */	/* host code */
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
/* # line 857 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 858 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 859 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 861 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 862 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 863 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 864 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 866 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 867 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 868 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 869 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)where ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 871 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 872 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 873 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 874 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".tid)where ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 877 "rolltest.qc" */	/* host code */
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
/* # line 892 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 893 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 894 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 895 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 896 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 897 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 898 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 899 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 900 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 901 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 902 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 903 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 904 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 905 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 906 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash unique");
      IIsyncup((char *)0,0);
    }
  }
/* # line 909 "rolltest.qc" */	/* host code */
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
/* # line 937 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 938 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 940 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 941 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 942 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 943 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 944 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 945 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 946 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 948 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 949 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 950 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a, b");
      IIsyncup((char *)0,0);
    }
/* # line 951 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 952 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 953 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 954 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 956 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 957 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=2)");
      IIsyncup((char *)0,0);
    }
/* # line 958 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=3)");
      IIsyncup((char *)0,0);
    }
/* # line 959 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=4)");
      IIsyncup((char *)0,0);
    }
/* # line 960 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+4, b=");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 961 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+8, c=");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 964 "rolltest.qc" */	/* host code */
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
/* # line 989 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 990 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 992 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 993 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 994 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 995 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 996 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 997 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 998 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1000 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1001 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1002 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a, b");
      IIsyncup((char *)0,0);
    }
/* # line 1003 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1004 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1005 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1006 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1008 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1009 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1010 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1011 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1012 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+4, b=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1013 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+8, c=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 1014 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16, b=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16)");
      IIsyncup((char *)0,0);
    }
/* # line 1015 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32, b=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32)");
      IIsyncup((char *)0,0);
    }
/* # line 1016 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+64, b=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+64)");
      IIsyncup((char *)0,0);
    }
/* # line 1017 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+128, b=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+128)");
      IIsyncup((char *)0,0);
    }
/* # line 1018 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+512, b=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+512)");
      IIsyncup((char *)0,0);
    }
/* # line 1019 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+1024, b=");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+1024)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1022 "rolltest.qc" */	/* host code */
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
/* # line 1047 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1048 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1050 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1051 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1052 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 1053 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1054 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1055 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1056 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1058 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1059 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1060 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a, b");
      IIsyncup((char *)0,0);
    }
/* # line 1061 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1062 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1063 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1064 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1066 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1067 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1068 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1069 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1070 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+4, b=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1071 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+8, c=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 1072 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16, b=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16)");
      IIsyncup((char *)0,0);
    }
/* # line 1073 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32, b=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32)");
      IIsyncup((char *)0,0);
    }
/* # line 1074 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+64, b=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+64)");
      IIsyncup((char *)0,0);
    }
/* # line 1075 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+128, b=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+128)");
      IIsyncup((char *)0,0);
    }
/* # line 1076 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+512, b=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+512)");
      IIsyncup((char *)0,0);
    }
/* # line 1077 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+1024, b=");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+1024)");
      IIsyncup((char *)0,0);
    }
/* # line 1079 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>23 and ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a<400");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1082 "rolltest.qc" */	/* host code */
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
/* # line 1107 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1108 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1110 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1111 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1112 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 1113 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1114 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1115 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1116 "rolltest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1118 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1119 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1120 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a, b");
      IIsyncup((char *)0,0);
    }
/* # line 1121 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1122 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1123 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1124 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1126 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1127 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1128 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1129 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1130 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+4, b=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1131 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+8, c=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 1132 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16, b=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16)");
      IIsyncup((char *)0,0);
    }
/* # line 1133 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32, b=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32)");
      IIsyncup((char *)0,0);
    }
/* # line 1134 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+64, b=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+64)");
      IIsyncup((char *)0,0);
    }
/* # line 1135 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+128, b=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+128)");
      IIsyncup((char *)0,0);
    }
/* # line 1136 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+512, b=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+512)");
      IIsyncup((char *)0,0);
    }
/* # line 1137 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+1024, b=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+1024)");
      IIsyncup((char *)0,0);
    }
/* # line 1139 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=24, b=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b-485, c=");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)where ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>233 and ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a<501");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1143 "rolltest.qc" */	/* host code */
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
/* # line 1172 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1173 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1174 "rolltest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()from ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 1175 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1177 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1178 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1179 "rolltest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()from ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 1180 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1182 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1183 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1184 "rolltest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()from ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 1185 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1187 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 1188 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 1189 "rolltest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()from ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 1190 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1192 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 1193 "rolltest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into hash");
      IIsyncup((char *)0,0);
    }
/* # line 1194 "rolltest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 1195 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1197 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 1198 "rolltest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into isam");
      IIsyncup((char *)0,0);
    }
/* # line 1199 "rolltest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 1200 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1202 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 1203 "rolltest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into btree");
      IIsyncup((char *)0,0);
    }
/* # line 1204 "rolltest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 1205 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1207 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 1208 "rolltest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into heap");
      IIsyncup((char *)0,0);
    }
/* # line 1209 "rolltest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 1210 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1213 "rolltest.qc" */	/* host code */
roll_test21(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char ttab1_name[16];
    /*
    ** Fast21
    */
    STprintf(ttab1_name, "ttab1_%d_%d", base_num, sess_num);
/* # line 1224 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1225 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 1227 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1228 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1229 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1230 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1231 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1232 "rolltest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s1");
      IIsyncup((char *)0,0);
    }
/* # line 1234 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5)");
      IIsyncup((char *)0,0);
    }
/* # line 1235 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6)");
      IIsyncup((char *)0,0);
    }
/* # line 1238 "rolltest.qc" */	/* abort */
    {
      IIxact(2);
    }
/* # line 1238 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1239 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)");
      IIsyncup((char *)0,0);
    }
/* # line 1240 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=11)");
      IIsyncup((char *)0,0);
    }
/* # line 1241 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=12)");
      IIsyncup((char *)0,0);
    }
/* # line 1243 "rolltest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s2");
      IIsyncup((char *)0,0);
    }
/* # line 1244 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=13)");
      IIsyncup((char *)0,0);
    }
/* # line 1245 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=14)");
      IIsyncup((char *)0,0);
    }
/* # line 1246 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=15)");
      IIsyncup((char *)0,0);
    }
/* # line 1247 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=12");
      IIsyncup((char *)0,0);
    }
/* # line 1248 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=40)where ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=11");
      IIsyncup((char *)0,0);
    }
/* # line 1249 "rolltest.qc" */	/* abort */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"abort to s2");
      IIsyncup((char *)0,0);
    }
/* # line 1251 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=16)");
      IIsyncup((char *)0,0);
    }
/* # line 1252 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=17)");
      IIsyncup((char *)0,0);
    }
/* # line 1253 "rolltest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s3");
      IIsyncup((char *)0,0);
    }
/* # line 1255 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=31");
      IIsyncup((char *)0,0);
    }
/* # line 1256 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=90)where ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=32");
      IIsyncup((char *)0,0);
    }
/* # line 1257 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=18)");
      IIsyncup((char *)0,0);
    }
/* # line 1258 "rolltest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s4");
      IIsyncup((char *)0,0);
    }
/* # line 1260 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=19)");
      IIsyncup((char *)0,0);
    }
/* # line 1261 "rolltest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s5");
      IIsyncup((char *)0,0);
    }
/* # line 1263 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=20)");
      IIsyncup((char *)0,0);
    }
/* # line 1264 "rolltest.qc" */	/* abort */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"abort to s4");
      IIsyncup((char *)0,0);
    }
/* # line 1266 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1269 "rolltest.qc" */	/* host code */
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
/* # line 1284 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1285 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1286 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1288 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1289 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1290 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1291 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1292 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1293 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 1294 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1295 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=19)where ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=2");
      IIsyncup((char *)0,0);
    }
/* # line 1296 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b=1");
      IIsyncup((char *)0,0);
    }
/* # line 1297 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1300 "rolltest.qc" */	/* host code */
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
    MEfill(1000, ' ', blah_data);
    MEcopy("LARGE_ROW   ", 9, blah_data);
    large_data1 = blah_data;
/* # line 1316 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1317 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cheap");
      IIsyncup((char *)0,0);
    }
/* # line 1319 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1320 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=1, b=\"one\", c=\"ROW 1\", d=\"row_one\")");
      IIsyncup((char *)0,0);
    }
/* # line 1321 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=\"two\", c=\"ROW 2\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1322 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=3, b=\"three\", c=\"ROW 3\", d=\"row_three\")");
      IIsyncup((char *)0,0);
    }
/* # line 1323 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=\"four\", c=\"ROW \
4\")");
      IIsyncup((char *)0,0);
    }
/* # line 1324 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=5, b=\"five\", c=\"ROW 5\", d=\"row_five\")");
      IIsyncup((char *)0,0);
    }
/* # line 1325 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6, b=\"six\", c=\"ROW 6\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1326 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=7, b=\"seven\", c=\"ROW 7\", d=\"row_seven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1327 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=8, b=\"eight\", c=\"ROW 8\")");
      IIsyncup((char *)0,0);
    }
/* # line 1328 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=9, b=\"nine\", c=\"ROW 9\", d=\"row_nine\")");
      IIsyncup((char *)0,0);
    }
/* # line 1329 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=10, b=\"ten\", c=\"ROW 10\")");
      IIsyncup((char *)0,0);
    }
/* # line 1330 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=11, b=\"eleven\", c=\"ROW 11\", d=\"row_eleven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1331 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=12, b=\"twelve\", c=\"ROW 12\")");
      IIsyncup((char *)0,0);
    }
/* # line 1332 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=13, b=\"thirteen\", c=\"ROW 13\", d=\"row_thirteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1333 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=14, b=\"fourteen\", c=\"ROW 14\")");
      IIsyncup((char *)0,0);
    }
/* # line 1334 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=15, b=\"fifteen\", c=\"ROW 15\", d=\"row_fifteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1335 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=16, b=\"sixteen\", c=\"ROW 16\")");
      IIsyncup((char *)0,0);
    }
/* # line 1336 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=17, b=\"seventeen\", c=\"ROW 17\", d=\"row_seventeen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1337 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=18, b=\"eighteen\", c=\"ROW 18\")");
      IIsyncup((char *)0,0);
    }
/* # line 1338 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=19, b=\"nineteen\", c=\"ROW 19\", d=\"row_nineteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1339 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=20, b=\"twenty\", c=\"ROW 20\")");
      IIsyncup((char *)0,0);
    }
/* # line 1340 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=");
      IIputdomio((short *)0,1,32,0,large_data1);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=1");
      IIsyncup((char *)0,0);
    }
/* # line 1341 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=");
      IIputdomio((short *)0,1,32,0,large_data1);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=2");
      IIsyncup((char *)0,0);
    }
/* # line 1342 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+\"_new\")");
      IIsyncup((char *)0,0);
    }
/* # line 1343 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=\"NOT_NULL\")where ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=18");
      IIsyncup((char *)0,0);
    }
/* # line 1344 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>5 and ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a<10");
      IIsyncup((char *)0,0);
    }
/* # line 1345 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1349 "rolltest.qc" */	/* host code */
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
    MEfill(1000, ' ', blah_data);
    MEcopy("LARGE_ROW   ", 9, blah_data);
    large_data2 = blah_data;
/* # line 1365 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1366 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to chash");
      IIsyncup((char *)0,0);
    }
/* # line 1368 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1369 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=1, b=\"one\", c=\"ROW 1\", d=\"row_one\")");
      IIsyncup((char *)0,0);
    }
/* # line 1370 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=\"two\", c=\"ROW 2\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1371 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=3, b=\"three\", c=\"ROW 3\", d=\"row_three\")");
      IIsyncup((char *)0,0);
    }
/* # line 1372 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=\"four\", c=\"ROW \
4\")");
      IIsyncup((char *)0,0);
    }
/* # line 1373 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=5, b=\"five\", c=\"ROW 5\", d=\"row_five\")");
      IIsyncup((char *)0,0);
    }
/* # line 1374 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6, b=\"six\", c=\"ROW 6\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1375 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=7, b=\"seven\", c=\"ROW 7\", d=\"row_seven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1376 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=8, b=\"eight\", c=\"ROW 8\")");
      IIsyncup((char *)0,0);
    }
/* # line 1377 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=9, b=\"nine\", c=\"ROW 9\", d=\"row_nine\")");
      IIsyncup((char *)0,0);
    }
/* # line 1378 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=10, b=\"ten\", c=\"ROW 10\")");
      IIsyncup((char *)0,0);
    }
/* # line 1379 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=11, b=\"eleven\", c=\"ROW 11\", d=\"row_eleven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1380 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=12, b=\"twelve\", c=\"ROW 12\")");
      IIsyncup((char *)0,0);
    }
/* # line 1381 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=13, b=\"thirteen\", c=\"ROW 13\", d=\"row_thirteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1382 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=14, b=\"fourteen\", c=\"ROW 14\")");
      IIsyncup((char *)0,0);
    }
/* # line 1383 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=15, b=\"fifteen\", c=\"ROW 15\", d=\"row_fifteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1384 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=16, b=\"sixteen\", c=\"ROW 16\")");
      IIsyncup((char *)0,0);
    }
/* # line 1385 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=17, b=\"seventeen\", c=\"ROW 17\", d=\"row_seventeen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1386 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=18, b=\"eighteen\", c=\"ROW 18\")");
      IIsyncup((char *)0,0);
    }
/* # line 1387 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=19, b=\"nineteen\", c=\"ROW 19\", d=\"row_nineteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1388 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=20, b=\"twenty\", c=\"ROW 20\")");
      IIsyncup((char *)0,0);
    }
/* # line 1389 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=");
      IIputdomio((short *)0,1,32,0,large_data2);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=1");
      IIsyncup((char *)0,0);
    }
/* # line 1390 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=");
      IIputdomio((short *)0,1,32,0,large_data2);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=2");
      IIsyncup((char *)0,0);
    }
/* # line 1391 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+\"_new\")");
      IIsyncup((char *)0,0);
    }
/* # line 1392 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=\"NOT_NULL\")where ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=18");
      IIsyncup((char *)0,0);
    }
/* # line 1393 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>5 and ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a<10");
      IIsyncup((char *)0,0);
    }
/* # line 1394 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1398 "rolltest.qc" */	/* host code */
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
    MEfill(1000, ' ', blah_data);
    MEcopy("LARGE_ROW   ", 9, blah_data);
    large_data3 = blah_data;
/* # line 1414 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1415 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cisam");
      IIsyncup((char *)0,0);
    }
/* # line 1417 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1418 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=1, b=\"one\", c=\"ROW 1\", d=\"row_one\")");
      IIsyncup((char *)0,0);
    }
/* # line 1419 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=\"two\", c=\"ROW 2\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1420 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=3, b=\"three\", c=\"ROW 3\", d=\"row_three\")");
      IIsyncup((char *)0,0);
    }
/* # line 1421 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=\"four\", c=\"ROW \
4\")");
      IIsyncup((char *)0,0);
    }
/* # line 1422 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=5, b=\"five\", c=\"ROW 5\", d=\"row_five\")");
      IIsyncup((char *)0,0);
    }
/* # line 1423 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6, b=\"six\", c=\"ROW 6\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1424 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=7, b=\"seven\", c=\"ROW 7\", d=\"row_seven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1425 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=8, b=\"eight\", c=\"ROW 8\")");
      IIsyncup((char *)0,0);
    }
/* # line 1426 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=9, b=\"nine\", c=\"ROW 9\", d=\"row_nine\")");
      IIsyncup((char *)0,0);
    }
/* # line 1427 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=10, b=\"ten\", c=\"ROW 10\")");
      IIsyncup((char *)0,0);
    }
/* # line 1428 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=11, b=\"eleven\", c=\"ROW 11\", d=\"row_eleven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1429 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=12, b=\"twelve\", c=\"ROW 12\")");
      IIsyncup((char *)0,0);
    }
/* # line 1430 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=13, b=\"thirteen\", c=\"ROW 13\", d=\"row_thirteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1431 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=14, b=\"fourteen\", c=\"ROW 14\")");
      IIsyncup((char *)0,0);
    }
/* # line 1432 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=15, b=\"fifteen\", c=\"ROW 15\", d=\"row_fifteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1433 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=16, b=\"sixteen\", c=\"ROW 16\")");
      IIsyncup((char *)0,0);
    }
/* # line 1434 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=17, b=\"seventeen\", c=\"ROW 17\", d=\"row_seventeen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1435 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=18, b=\"eighteen\", c=\"ROW 18\")");
      IIsyncup((char *)0,0);
    }
/* # line 1436 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=19, b=\"nineteen\", c=\"ROW 19\", d=\"row_nineteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1437 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=20, b=\"twenty\", c=\"ROW 20\")");
      IIsyncup((char *)0,0);
    }
/* # line 1438 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=");
      IIputdomio((short *)0,1,32,0,large_data3);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=1");
      IIsyncup((char *)0,0);
    }
/* # line 1439 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=");
      IIputdomio((short *)0,1,32,0,large_data3);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=2");
      IIsyncup((char *)0,0);
    }
/* # line 1440 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+\"_new\")");
      IIsyncup((char *)0,0);
    }
/* # line 1441 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=\"NOT_NULL\")where ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=18");
      IIsyncup((char *)0,0);
    }
/* # line 1442 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>5 and ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a<10");
      IIsyncup((char *)0,0);
    }
/* # line 1443 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1447 "rolltest.qc" */	/* host code */
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
    MEfill(1000, ' ', blah_data);
    MEcopy("LARGE_ROW   ", 9, blah_data);
    large_data4 = blah_data;
/* # line 1463 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1464 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cbtree");
      IIsyncup((char *)0,0);
    }
/* # line 1466 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1467 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=1, b=\"one\", c=\"ROW 1\", d=\"row_one\")");
      IIsyncup((char *)0,0);
    }
/* # line 1468 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=\"two\", c=\"ROW 2\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1469 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=3, b=\"three\", c=\"ROW 3\", d=\"row_three\")");
      IIsyncup((char *)0,0);
    }
/* # line 1470 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=\"four\", c=\"ROW \
4\")");
      IIsyncup((char *)0,0);
    }
/* # line 1471 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=5, b=\"five\", c=\"ROW 5\", d=\"row_five\")");
      IIsyncup((char *)0,0);
    }
/* # line 1472 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6, b=\"six\", c=\"ROW 6\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1473 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=7, b=\"seven\", c=\"ROW 7\", d=\"row_seven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1474 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=8, b=\"eight\", c=\"ROW 8\")");
      IIsyncup((char *)0,0);
    }
/* # line 1475 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=9, b=\"nine\", c=\"ROW 9\", d=\"row_nine\")");
      IIsyncup((char *)0,0);
    }
/* # line 1476 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=10, b=\"ten\", c=\"ROW 10\")");
      IIsyncup((char *)0,0);
    }
/* # line 1477 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=11, b=\"eleven\", c=\"ROW 11\", d=\"row_eleven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1478 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=12, b=\"twelve\", c=\"ROW 12\")");
      IIsyncup((char *)0,0);
    }
/* # line 1479 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=13, b=\"thirteen\", c=\"ROW 13\", d=\"row_thirteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1480 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=14, b=\"fourteen\", c=\"ROW 14\")");
      IIsyncup((char *)0,0);
    }
/* # line 1481 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=15, b=\"fifteen\", c=\"ROW 15\", d=\"row_fifteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1482 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=16, b=\"sixteen\", c=\"ROW 16\")");
      IIsyncup((char *)0,0);
    }
/* # line 1483 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=17, b=\"seventeen\", c=\"ROW 17\", d=\"row_seventeen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1484 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=18, b=\"eighteen\", c=\"ROW 18\")");
      IIsyncup((char *)0,0);
    }
/* # line 1485 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=19, b=\"nineteen\", c=\"ROW 19\", d=\"row_nineteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1486 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=20, b=\"twenty\", c=\"ROW 20\")");
      IIsyncup((char *)0,0);
    }
/* # line 1487 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=");
      IIputdomio((short *)0,1,32,0,large_data4);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=1");
      IIsyncup((char *)0,0);
    }
/* # line 1488 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=");
      IIputdomio((short *)0,1,32,0,large_data4);
      IIwritio(0,(short *)0,1,32,0,(char *)")where ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=2");
      IIsyncup((char *)0,0);
    }
/* # line 1489 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+\"_new\")");
      IIsyncup((char *)0,0);
    }
/* # line 1490 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=\"NOT_NULL\")where ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=18");
      IIsyncup((char *)0,0);
    }
/* # line 1491 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>5 and ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a<10");
      IIsyncup((char *)0,0);
    }
/* # line 1492 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1496 "rolltest.qc" */	/* host code */
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
/* # line 1517 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1519 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1520 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1521 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1522 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1523 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 1524 "rolltest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 1527 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1528 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1529 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1530 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cisam");
      IIsyncup((char *)0,0);
    }
/* # line 1531 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to chash");
      IIsyncup((char *)0,0);
    }
/* # line 1532 "rolltest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cbtree");
      IIsyncup((char *)0,0);
    }
/* # line 1534 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1535 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1536 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+2, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1537 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+4, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1538 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+8, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1539 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1540 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1542 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, b=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1543 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, b=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1544 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, b=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1545 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, b=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1546 "rolltest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a, b=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1548 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1552 "rolltest.qc" */	/* host code */
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
/***    for (i = 0 ; i < 10000; i ++)	full test, but not enough disk space ***/
    for (i = 0 ; i < 1000; i ++)
    {
	/*
	** Use dbms_cpu as a sort of random number generator.
	** Use the divide-by-two because on some systems the
	** number always seems to be an even-number.
	*/
/* # line 1583 "rolltest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(num=int4(dbmsinfo(\"dbms_cpu\")))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&num);
        if (IIerrtest() != 0) goto IIrtB1;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE1:;
    }
/* # line 1584 "rolltest.qc" */	/* host code */
	num = ((num / 2) % 64) + 1;
/* # line 1586 "rolltest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1588 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,concrtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=");
      IIwritio(1,(short *)0,1,32,0,concrtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b+1)where ");
      IIwritio(1,(short *)0,1,32,0,concrtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&num);
      IIsyncup((char *)0,0);
    }
/* # line 1590 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,concrtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=");
      IIwritio(1,(short *)0,1,32,0,concrtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b+1)where ");
      IIwritio(1,(short *)0,1,32,0,concrtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&num);
      IIsyncup((char *)0,0);
    }
/* # line 1592 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,concrtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=");
      IIwritio(1,(short *)0,1,32,0,concrtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b+1)where ");
      IIwritio(1,(short *)0,1,32,0,concrtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&num);
      IIsyncup((char *)0,0);
    }
/* # line 1594 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,concrtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=");
      IIwritio(1,(short *)0,1,32,0,concrtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b+1)where ");
      IIwritio(1,(short *)0,1,32,0,concrtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&num);
      IIsyncup((char *)0,0);
    }
/* # line 1596 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,concrtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=");
      IIwritio(1,(short *)0,1,32,0,concrtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b+1)where ");
      IIwritio(1,(short *)0,1,32,0,concrtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&num);
      IIsyncup((char *)0,0);
    }
/* # line 1598 "rolltest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,concrtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=");
      IIwritio(1,(short *)0,1,32,0,concrtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b+1)where ");
      IIwritio(1,(short *)0,1,32,0,concrtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=");
      IIputdomio((short *)0,1,30,4,&num);
      IIsyncup((char *)0,0);
    }
/* # line 1601 "rolltest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 1602 "rolltest.qc" */	/* host code */
    }
  }
roll_canon(name, base_num, sess_num)
char	*name;
int	base_num;
int	sess_num;
{
  char *dbname;
    dbname = name;
/* # line 1615 "rolltest.qc" */	/* ingres */
  {
    IIingopen(0,dbname,(char *)0);
  }
/* # line 1617 "rolltest.qc" */	/* host code */
    roll_test1(base_num, sess_num);
    roll_test2(base_num, sess_num);
    roll_test3(base_num, sess_num);
    roll_test4(base_num, sess_num);
    roll_test5(base_num, sess_num);
    roll_test6(base_num, sess_num);
    roll_test7(base_num, sess_num);
    roll_test8(base_num, sess_num);
    roll_test9(base_num, sess_num);
    roll_test10(base_num, sess_num);
    roll_test11(base_num, sess_num);
    roll_test12(base_num, sess_num);
    roll_test13(base_num, sess_num);
    roll_test14(base_num, sess_num);
    roll_test15(base_num, sess_num);
    roll_test16(base_num, sess_num);
    roll_test17(base_num, sess_num);
    roll_test18(base_num, sess_num);
    roll_test19(base_num, sess_num);
    roll_test20(base_num, sess_num);
    roll_test21(base_num, sess_num);
    roll_test22(base_num, sess_num);
    roll_test23(base_num, sess_num);
    roll_test24(base_num, sess_num);
    roll_test25(base_num, sess_num);
    roll_test26(base_num, sess_num);
    roll_test27(base_num, sess_num);
    roll_test28(base_num, sess_num);
/* # line 1646 "rolltest.qc" */	/* exit */
  {
    IIexit();
  }
/* # line 1647 "rolltest.qc" */	/* host code */
}
