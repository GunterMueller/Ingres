# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  MSFC_TEST	    - run fast commit test
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
**      22-sep-2000 (mcgem01)
**              More nat and longnat to i4 changes.
**
**
NEEDLIBS = LIBINGRES
**
*/
#include	<compat.h>
#include	<tr.h>
#include	<pc.h>
#include	<me.h>
#include	<st.h>
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
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    dbname = argv[1];
    CVal(argv[2], &test_num);
    CVal(argv[3], &base_num);
    CVal(argv[4], &sess_num);
    if (base_num < 0 || test_num < 0 || test_num > 28 || sess_num < 0)
    {
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    if (test_num == 0)
    {
	msfc_canon(dbname, base_num, sess_num);
	PCexit(0);
    }
/* # line 92 "msfctest.qc" */	/* ingres */
    {
      IIingopen(0,dbname,(char *)0);
    }
/* # line 94 "msfctest.qc" */	/* host code */
    /*
    ** Can use printqry to track down what each test session did - 
    ** but it does produce a lot of output - particulary on test 28.
    ** ##  	set printqry
    */
    TRdisplay("Session # %d executing test # %d\n", sess_num, test_num);
    switch(test_num)
    {
      case  1:  msfc_test1(base_num, sess_num); break;
      case  2:  msfc_test2(base_num, sess_num); break;
      case  3:  msfc_test3(base_num, sess_num); break;
      case  4:  msfc_test4(base_num, sess_num); break;
      case  5:  msfc_test5(base_num, sess_num); break;
      case  6:  msfc_test6(base_num, sess_num); break;
      case  7:  msfc_test7(base_num, sess_num); break;
      case  8:  msfc_test8(base_num, sess_num); break;
      case  9:  msfc_test9(base_num, sess_num); break;
      case 10:  msfc_test10(base_num, sess_num); break;
      case 11:  msfc_test11(base_num, sess_num); break;
      case 12:  msfc_test12(base_num, sess_num); break;
      case 13:  msfc_test13(base_num, sess_num); break;
      case 14:  msfc_test14(base_num, sess_num); break;
      case 15:  msfc_test15(base_num, sess_num); break;
      case 16:  msfc_test16(base_num, sess_num); break;
      case 17:  msfc_test17(base_num, sess_num); break;
      case 18:  msfc_test18(base_num, sess_num); break;
      case 19:  msfc_test19(base_num, sess_num); break;
      case 20:  msfc_test20(base_num, sess_num); break;
      case 21:  msfc_test21(base_num, sess_num); break;
      case 22:  msfc_test22(base_num, sess_num); break;
      case 23:  msfc_test23(base_num, sess_num); break;
      case 24:  msfc_test24(base_num, sess_num); break;
      case 25:  msfc_test25(base_num, sess_num); break;
      case 26:  msfc_test26(base_num, sess_num); break;
      case 27:  msfc_test27(base_num, sess_num); break;
      case 28:  msfc_test28(base_num, sess_num); break;
      default:
	TRdisplay("Unknown test number - %d\n", test_num);
    }
    TRdisplay("Session # %d now waiting for server crash\n", sess_num);
    /*
    ** Create file so driver knows we are ready for server crash.
    */
    STprintf(table_name, "msfc_temp_tab_%d_%d", base_num, sess_num);
    STprintf(copy_file, "MSFC_DATA:msfc_copy_%d_%d.in", base_num, sess_num);
/* # line 143 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 144 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=c30, b=c20)");
      IIsyncup((char *)0,0);
    }
/* # line 145 "msfctest.qc" */	/* append */
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
/* # line 146 "msfctest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=c30, b=c20, d=d0nl)into\
 ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 147 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 149 "msfctest.qc" */	/* host code */
    /*
    ** Wait for sync lock - driver will release this when all fast commit
    ** tests have reached this point.
    */
/* # line 153 "msfctest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set trace point DM1420");
      IIsyncup((char *)0,0);
    }
/* # line 155 "msfctest.qc" */	/* host code */
    /*
    ** Crash server
    */
/* # line 158 "msfctest.qc" */	/* sleep */
    {
      IIsleep(5);
    }
/* # line 159 "msfctest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set trace point SC908");
      IIsyncup((char *)0,0);
    }
/* # line 161 "msfctest.qc" */	/* exit */
    {
      IIexit();
    }
/* # line 162 "msfctest.qc" */	/* host code */
    PCexit(0);
  }
msfc_usage()
{
    TRdisplay("USAGE: MSFC_TEST dbname test base session\n");
    TRdisplay("    dbname - database.\n");
    TRdisplay("    test   - test number to execute.\n");
    TRdisplay("    base   - base # of driver.\n");
    TRdisplay("    session - which slave this is.\n");
    TRdisplay("\n");
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
/* # line 192 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIsyncup((char *)0,0);
    }
/* # line 193 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 194 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=1)");
      IIsyncup((char *)0,0);
    }
/* # line 195 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=2)");
      IIsyncup((char *)0,0);
    }
/* # line 196 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=3)");
      IIsyncup((char *)0,0);
    }
/* # line 197 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=4)");
      IIsyncup((char *)0,0);
    }
/* # line 198 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=5)");
      IIsyncup((char *)0,0);
    }
/* # line 199 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=6)");
      IIsyncup((char *)0,0);
    }
/* # line 200 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=7)");
      IIsyncup((char *)0,0);
    }
/* # line 201 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=8)");
      IIsyncup((char *)0,0);
    }
/* # line 202 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=9)");
      IIsyncup((char *)0,0);
    }
/* # line 203 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=10)");
      IIsyncup((char *)0,0);
    }
/* # line 204 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=11)");
      IIsyncup((char *)0,0);
    }
/* # line 205 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=12)");
      IIsyncup((char *)0,0);
    }
/* # line 206 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=13)");
      IIsyncup((char *)0,0);
    }
/* # line 207 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=14)");
      IIsyncup((char *)0,0);
    }
/* # line 208 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=15)");
      IIsyncup((char *)0,0);
    }
/* # line 209 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=16)");
      IIsyncup((char *)0,0);
    }
/* # line 210 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=17)");
      IIsyncup((char *)0,0);
    }
/* # line 211 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=18)");
      IIsyncup((char *)0,0);
    }
/* # line 212 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=19)");
      IIsyncup((char *)0,0);
    }
/* # line 213 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=20)");
      IIsyncup((char *)0,0);
    }
/* # line 215 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 216 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 217 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 218 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 219 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 220 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 221 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 222 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 223 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 224 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 225 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 226 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 227 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 228 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 229 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 230 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 231 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 232 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 233 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 234 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 235 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 237 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 238 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 239 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)");
      IIsyncup((char *)0,0);
    }
/* # line 240 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1)");
      IIsyncup((char *)0,0);
    }
/* # line 241 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2)");
      IIsyncup((char *)0,0);
    }
/* # line 242 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+2)");
      IIsyncup((char *)0,0);
    }
/* # line 243 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+4)");
      IIsyncup((char *)0,0);
    }
/* # line 244 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+8)");
      IIsyncup((char *)0,0);
    }
/* # line 245 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16)");
      IIsyncup((char *)0,0);
    }
/* # line 246 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32)");
      IIsyncup((char *)0,0);
    }
/* # line 247 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+64)");
      IIsyncup((char *)0,0);
    }
/* # line 248 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+128)");
      IIsyncup((char *)0,0);
    }
/* # line 249 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+256)");
      IIsyncup((char *)0,0);
    }
/* # line 250 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+512)");
      IIsyncup((char *)0,0);
    }
/* # line 252 "msfctest.qc" */	/* host code */
    STprintf(copy_file, "MSFC_DATA:ctab_%d_%d.in", base_num, sess_num);
/* # line 253 "msfctest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()into ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 255 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 258 "msfctest.qc" */	/* host code */
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
    /*
    ** Append rows that will cause additions to ovfl leaf chains of btree
    */
/* # line 279 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 280 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 281 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 282 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=1)");
      IIsyncup((char *)0,0);
    }
/* # line 283 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=2)");
      IIsyncup((char *)0,0);
    }
/* # line 284 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=3)");
      IIsyncup((char *)0,0);
    }
/* # line 285 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=4)");
      IIsyncup((char *)0,0);
    }
/* # line 286 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=5)");
      IIsyncup((char *)0,0);
    }
/* # line 287 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=6)");
      IIsyncup((char *)0,0);
    }
/* # line 288 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=7)");
      IIsyncup((char *)0,0);
    }
/* # line 289 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=8)");
      IIsyncup((char *)0,0);
    }
/* # line 290 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=9)");
      IIsyncup((char *)0,0);
    }
/* # line 291 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=10)");
      IIsyncup((char *)0,0);
    }
/* # line 292 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=11)");
      IIsyncup((char *)0,0);
    }
/* # line 293 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=12)");
      IIsyncup((char *)0,0);
    }
/* # line 294 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=13)");
      IIsyncup((char *)0,0);
    }
/* # line 295 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=14)");
      IIsyncup((char *)0,0);
    }
/* # line 296 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=15)");
      IIsyncup((char *)0,0);
    }
/* # line 297 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=16)");
      IIsyncup((char *)0,0);
    }
/* # line 298 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=17)");
      IIsyncup((char *)0,0);
    }
/* # line 299 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=18)");
      IIsyncup((char *)0,0);
    }
/* # line 300 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=19)");
      IIsyncup((char *)0,0);
    }
/* # line 301 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=20)");
      IIsyncup((char *)0,0);
    }
/* # line 302 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=1)");
      IIsyncup((char *)0,0);
    }
/* # line 303 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=2)");
      IIsyncup((char *)0,0);
    }
/* # line 304 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=3)");
      IIsyncup((char *)0,0);
    }
/* # line 305 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=4)");
      IIsyncup((char *)0,0);
    }
/* # line 306 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=5)");
      IIsyncup((char *)0,0);
    }
/* # line 307 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=6)");
      IIsyncup((char *)0,0);
    }
/* # line 308 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=7)");
      IIsyncup((char *)0,0);
    }
/* # line 309 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=8)");
      IIsyncup((char *)0,0);
    }
/* # line 310 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=9)");
      IIsyncup((char *)0,0);
    }
/* # line 311 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=10)");
      IIsyncup((char *)0,0);
    }
/* # line 313 "msfctest.qc" */	/* host code */
    /*
    ** Delete rows on ovfl leaf chains
    */
/* # line 316 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 317 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 318 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 319 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 320 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,btab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 322 "msfctest.qc" */	/* host code */
    /*
    ** Replace rows on ovfl leaf chains
    */
/* # line 325 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 326 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 327 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 328 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 329 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,btab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
  }
/* # line 332 "msfctest.qc" */	/* host code */
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
/* # line 347 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 348 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 349 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 350 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 351 "msfctest.qc" */	/* replace */
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
/* # line 353 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 354 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 355 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 356 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 357 "msfctest.qc" */	/* replace */
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
/* # line 360 "msfctest.qc" */	/* host code */
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
    /*
    ** Do same queries with HASH, ISAM, HEAP tables
    */
/* # line 384 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 385 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 386 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 387 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 389 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 390 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 391 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 392 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 393 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,htab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 395 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 396 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 397 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 398 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 399 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 400 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,htab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 401 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 403 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 404 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 405 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 406 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 407 "msfctest.qc" */	/* replace */
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
/* # line 409 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 410 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 411 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 412 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 413 "msfctest.qc" */	/* replace */
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
/* # line 416 "msfctest.qc" */	/* host code */
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
/* # line 440 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 441 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 442 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 443 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 445 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 446 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 447 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 448 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 449 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 450 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,itab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 451 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 453 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 454 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 455 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 456 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,htab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 457 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,itab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 459 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 460 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 461 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 462 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 463 "msfctest.qc" */	/* replace */
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
/* # line 465 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 466 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 467 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam on a");
      IIsyncup((char *)0,0);
    }
/* # line 468 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 469 "msfctest.qc" */	/* replace */
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
/* # line 472 "msfctest.qc" */	/* host code */
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
/* # line 494 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 495 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 496 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 497 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 499 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 500 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 501 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 502 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 503 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 504 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,stab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 505 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 507 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 508 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 509 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 510 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 511 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,stab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=0");
      IIsyncup((char *)0,0);
    }
/* # line 513 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 514 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 515 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 516 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 517 "msfctest.qc" */	/* replace */
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
/* # line 519 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIsyncup((char *)0,0);
    }
/* # line 520 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 521 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 522 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 523 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab10_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 524 "msfctest.qc" */	/* replace */
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
/* # line 525 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 528 "msfctest.qc" */	/* host code */
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
/* # line 552 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 553 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 554 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 555 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 556 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 558 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 559 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 560 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 561 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 563 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 564 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 565 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 566 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,btab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 568 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 569 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 570 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 571 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,btab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 573 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 574 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 575 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 576 "msfctest.qc" */	/* replace */
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
/* # line 578 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 579 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 580 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 581 "msfctest.qc" */	/* replace */
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
/* # line 582 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 585 "msfctest.qc" */	/* host code */
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
/* # line 606 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 607 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 608 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 609 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 611 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 612 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 613 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 614 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 615 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,htab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 617 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 618 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 619 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 620 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 621 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,htab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 623 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 624 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 625 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 626 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 627 "msfctest.qc" */	/* replace */
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
/* # line 629 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 630 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 631 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 632 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 633 "msfctest.qc" */	/* replace */
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
/* # line 636 "msfctest.qc" */	/* host code */
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
/* # line 657 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 658 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 659 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 660 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 662 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 663 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 664 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 665 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 666 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,itab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 668 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 669 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 670 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 671 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 672 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,itab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 674 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 675 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 676 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 677 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 678 "msfctest.qc" */	/* replace */
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
/* # line 680 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 681 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 682 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 683 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 684 "msfctest.qc" */	/* replace */
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
/* # line 687 "msfctest.qc" */	/* host code */
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
/* # line 708 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 709 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 710 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 711 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 713 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 714 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 715 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 716 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 717 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,stab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 719 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 720 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 721 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 722 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 723 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=\"NEW\")where ");
      IIwritio(1,(short *)0,1,32,0,stab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a>0");
      IIsyncup((char *)0,0);
    }
/* # line 725 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 726 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 727 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 728 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 729 "msfctest.qc" */	/* replace */
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
/* # line 731 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 732 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20, c=i4, d=c180\
)");
      IIsyncup((char *)0,0);
    }
/* # line 733 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 734 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c)");
      IIsyncup((char *)0,0);
    }
/* # line 735 "msfctest.qc" */	/* replace */
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
/* # line 738 "msfctest.qc" */	/* host code */
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
/* # line 760 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 761 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 762 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 763 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 764 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 765 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 766 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 767 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 768 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 769 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 770 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 771 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 772 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 773 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 774 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 775 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 776 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 777 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 778 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 779 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 780 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 781 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 783 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 784 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 785 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 786 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 787 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,btab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 789 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 790 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 791 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 792 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 793 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)where ");
      IIwritio(1,(short *)0,1,32,0,btab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 795 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 796 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 797 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 798 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,btab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 799 "msfctest.qc" */	/* replace */
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
/* # line 802 "msfctest.qc" */	/* host code */
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
/* # line 824 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 825 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 826 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 827 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 829 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 830 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 831 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 832 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 833 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,htab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 835 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 836 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 837 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 838 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 839 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)where ");
      IIwritio(1,(short *)0,1,32,0,htab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 841 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 842 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 843 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 844 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,htab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 845 "msfctest.qc" */	/* replace */
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
/* # line 848 "msfctest.qc" */	/* host code */
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
/* # line 867 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 868 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 869 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 870 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 872 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 873 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 874 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 875 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 876 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,itab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 878 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 879 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 880 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 881 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 882 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)where ");
      IIwritio(1,(short *)0,1,32,0,itab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 884 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 885 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 886 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 887 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,itab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 888 "msfctest.qc" */	/* replace */
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
/* # line 891 "msfctest.qc" */	/* host code */
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
/* # line 910 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIsyncup((char *)0,0);
    }
/* # line 911 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 912 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 913 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab11_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 915 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIsyncup((char *)0,0);
    }
/* # line 916 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 917 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 918 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 919 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,stab12_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 921 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIsyncup((char *)0,0);
    }
/* # line 922 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 923 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 924 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 925 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)where ");
      IIwritio(1,(short *)0,1,32,0,stab13_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c=0");
      IIsyncup((char *)0,0);
    }
/* # line 927 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIsyncup((char *)0,0);
    }
/* # line 928 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=c20, c=i4, d=c180)with duplicates");
      IIsyncup((char *)0,0);
    }
/* # line 929 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree on a");
      IIsyncup((char *)0,0);
    }
/* # line 930 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,stab14_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,rtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 931 "msfctest.qc" */	/* replace */
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
/* # line 934 "msfctest.qc" */	/* host code */
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
/* # line 949 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 950 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 951 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 952 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 953 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 954 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 955 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 956 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 957 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 958 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 959 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 960 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 961 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 962 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=0)");
      IIsyncup((char *)0,0);
    }
/* # line 963 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 964 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,dtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash unique");
      IIsyncup((char *)0,0);
    }
  }
/* # line 967 "msfctest.qc" */	/* host code */
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
/* # line 995 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 996 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 997 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 999 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1000 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1001 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 1002 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1003 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1004 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1005 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i1x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1007 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1008 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1009 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a, b");
      IIsyncup((char *)0,0);
    }
/* # line 1010 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1011 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1012 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1013 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i1x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1015 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1016 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1017 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1018 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1019 "msfctest.qc" */	/* append */
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
/* # line 1020 "msfctest.qc" */	/* append */
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
/* # line 1023 "msfctest.qc" */	/* host code */
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
/* # line 1048 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1049 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 1050 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1052 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1053 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1054 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 1055 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1056 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1057 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1058 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i2x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1060 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1061 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1062 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a, b");
      IIsyncup((char *)0,0);
    }
/* # line 1063 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1064 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1065 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1066 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i2x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1068 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1069 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1070 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1071 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1072 "msfctest.qc" */	/* append */
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
/* # line 1073 "msfctest.qc" */	/* append */
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
/* # line 1074 "msfctest.qc" */	/* append */
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
/* # line 1075 "msfctest.qc" */	/* append */
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
/* # line 1076 "msfctest.qc" */	/* append */
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
/* # line 1077 "msfctest.qc" */	/* append */
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
/* # line 1078 "msfctest.qc" */	/* append */
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
/* # line 1079 "msfctest.qc" */	/* append */
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
/* # line 1082 "msfctest.qc" */	/* host code */
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
/* # line 1107 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1108 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 1109 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1111 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1112 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1113 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 1114 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1115 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1116 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1117 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i3x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1119 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1120 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1121 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a, b");
      IIsyncup((char *)0,0);
    }
/* # line 1122 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1123 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1124 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1125 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i3x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1127 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1128 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1129 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1130 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1131 "msfctest.qc" */	/* append */
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
/* # line 1132 "msfctest.qc" */	/* append */
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
/* # line 1133 "msfctest.qc" */	/* append */
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
/* # line 1134 "msfctest.qc" */	/* append */
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
/* # line 1135 "msfctest.qc" */	/* append */
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
/* # line 1136 "msfctest.qc" */	/* append */
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
/* # line 1137 "msfctest.qc" */	/* append */
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
/* # line 1138 "msfctest.qc" */	/* append */
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
/* # line 1140 "msfctest.qc" */	/* delete */
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
/* # line 1143 "msfctest.qc" */	/* host code */
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
/* # line 1168 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1169 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 1170 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1172 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1173 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a)");
      IIsyncup((char *)0,0);
    }
/* # line 1174 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a, b)");
      IIsyncup((char *)0,0);
    }
/* # line 1175 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1176 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c)");
      IIsyncup((char *)0,0);
    }
/* # line 1177 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1178 "msfctest.qc" */	/* index */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"index on ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"=");
      IIwritio(1,(short *)0,1,32,0,i4x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b)");
      IIsyncup((char *)0,0);
    }
/* # line 1180 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1181 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1182 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a, b");
      IIsyncup((char *)0,0);
    }
/* # line 1183 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1184 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1185 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1186 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,i4x7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1188 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1189 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1190 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1191 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,xtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1192 "msfctest.qc" */	/* append */
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
/* # line 1193 "msfctest.qc" */	/* append */
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
/* # line 1194 "msfctest.qc" */	/* append */
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
/* # line 1195 "msfctest.qc" */	/* append */
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
/* # line 1196 "msfctest.qc" */	/* append */
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
/* # line 1197 "msfctest.qc" */	/* append */
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
/* # line 1198 "msfctest.qc" */	/* append */
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
/* # line 1199 "msfctest.qc" */	/* append */
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
/* # line 1201 "msfctest.qc" */	/* replace */
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
/* # line 1205 "msfctest.qc" */	/* host code */
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
/* # line 1234 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1235 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)");
      IIsyncup((char *)0,0);
    }
/* # line 1236 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1237 "msfctest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()from ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 1238 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1240 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1241 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)");
      IIsyncup((char *)0,0);
    }
/* # line 1242 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1243 "msfctest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()from ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 1244 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1246 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1247 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)");
      IIsyncup((char *)0,0);
    }
/* # line 1248 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1249 "msfctest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()from ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 1250 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1252 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 1253 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)");
      IIsyncup((char *)0,0);
    }
/* # line 1254 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to heap");
      IIsyncup((char *)0,0);
    }
/* # line 1255 "msfctest.qc" */	/* copy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"copy ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"()from ");
      IIvarstrio((short *)0,1,32,0,copy_file);
      IIsyncup((char *)0,0);
    }
/* # line 1256 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1258 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 1259 "msfctest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into hash");
      IIsyncup((char *)0,0);
    }
/* # line 1260 "msfctest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 1261 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1263 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIsyncup((char *)0,0);
    }
/* # line 1264 "msfctest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into isam");
      IIsyncup((char *)0,0);
    }
/* # line 1265 "msfctest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 1266 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab7_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1268 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIsyncup((char *)0,0);
    }
/* # line 1269 "msfctest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into btree");
      IIsyncup((char *)0,0);
    }
/* # line 1270 "msfctest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 1271 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab8_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
/* # line 1273 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIsyncup((char *)0,0);
    }
/* # line 1274 "msfctest.qc" */	/* set */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"set ret_into heap");
      IIsyncup((char *)0,0);
    }
/* # line 1275 "msfctest.qc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(");
      IIwritio(1,(short *)0,1,32,0,ctab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".all)");
      IIsyncup((char *)0,0);
    }
/* # line 1276 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ctab9_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5000)");
      IIsyncup((char *)0,0);
    }
  }
/* # line 1279 "msfctest.qc" */	/* host code */
msfc_test21(base_num, sess_num)
int	base_num;
int	sess_num;
  {
    char ttab1_name[16];
    /*
    ** Fast21
    */
    STprintf(ttab1_name, "ttab1_%d_%d", base_num, sess_num);
/* # line 1290 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1291 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=c20)");
      IIsyncup((char *)0,0);
    }
/* # line 1292 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash on a");
      IIsyncup((char *)0,0);
    }
/* # line 1294 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1295 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1296 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2)");
      IIsyncup((char *)0,0);
    }
/* # line 1297 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3)");
      IIsyncup((char *)0,0);
    }
/* # line 1298 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4)");
      IIsyncup((char *)0,0);
    }
/* # line 1299 "msfctest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s1");
      IIsyncup((char *)0,0);
    }
/* # line 1301 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=5)");
      IIsyncup((char *)0,0);
    }
/* # line 1302 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6)");
      IIsyncup((char *)0,0);
    }
/* # line 1305 "msfctest.qc" */	/* abort */
    {
      IIxact(2);
    }
/* # line 1305 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1306 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=10)");
      IIsyncup((char *)0,0);
    }
/* # line 1307 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=11)");
      IIsyncup((char *)0,0);
    }
/* # line 1308 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=12)");
      IIsyncup((char *)0,0);
    }
/* # line 1310 "msfctest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s2");
      IIsyncup((char *)0,0);
    }
/* # line 1311 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=13)");
      IIsyncup((char *)0,0);
    }
/* # line 1312 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=14)");
      IIsyncup((char *)0,0);
    }
/* # line 1313 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=15)");
      IIsyncup((char *)0,0);
    }
/* # line 1314 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=12");
      IIsyncup((char *)0,0);
    }
/* # line 1315 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=40)where ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=11");
      IIsyncup((char *)0,0);
    }
/* # line 1316 "msfctest.qc" */	/* abort */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"abort to s2");
      IIsyncup((char *)0,0);
    }
/* # line 1318 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=16)");
      IIsyncup((char *)0,0);
    }
/* # line 1319 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=17)");
      IIsyncup((char *)0,0);
    }
/* # line 1320 "msfctest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s3");
      IIsyncup((char *)0,0);
    }
/* # line 1322 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=31");
      IIsyncup((char *)0,0);
    }
/* # line 1323 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=90)where ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=32");
      IIsyncup((char *)0,0);
    }
/* # line 1324 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=18)");
      IIsyncup((char *)0,0);
    }
/* # line 1325 "msfctest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s4");
      IIsyncup((char *)0,0);
    }
/* # line 1327 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=19)");
      IIsyncup((char *)0,0);
    }
/* # line 1328 "msfctest.qc" */	/* savepoint */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"savepoint s5");
      IIsyncup((char *)0,0);
    }
/* # line 1330 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,ttab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=20)");
      IIsyncup((char *)0,0);
    }
/* # line 1331 "msfctest.qc" */	/* abort */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"abort to s4");
      IIsyncup((char *)0,0);
    }
/* # line 1333 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1336 "msfctest.qc" */	/* host code */
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
/* # line 1351 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1352 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1353 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1354 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 1355 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 1356 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 1358 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1359 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1360 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1361 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=3, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1362 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1363 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a)");
      IIsyncup((char *)0,0);
    }
/* # line 1364 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(b=");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b)");
      IIsyncup((char *)0,0);
    }
/* # line 1365 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=19)where ");
      IIwritio(1,(short *)0,1,32,0,mtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=2");
      IIsyncup((char *)0,0);
    }
/* # line 1366 "msfctest.qc" */	/* delete */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"delete ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" where ");
      IIwritio(1,(short *)0,1,32,0,mtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".b=1");
      IIsyncup((char *)0,0);
    }
/* # line 1367 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1370 "msfctest.qc" */	/* host code */
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
    MEfill(1000, ' ', blah_data);
    MEcopy("LARGE_ROW   ", 9, blah_data);
    large_data1 = blah_data;
/* # line 1386 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1387 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)");
      IIsyncup((char *)0,0);
    }
/* # line 1389 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cheap");
      IIsyncup((char *)0,0);
    }
/* # line 1391 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1392 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=1, b=\"one\", c=\"ROW 1\", d=\"row_one\")");
      IIsyncup((char *)0,0);
    }
/* # line 1393 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=\"two\", c=\"ROW 2\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1394 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=3, b=\"three\", c=\"ROW 3\", d=\"row_three\")");
      IIsyncup((char *)0,0);
    }
/* # line 1395 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=\"four\", c=\"ROW \
4\")");
      IIsyncup((char *)0,0);
    }
/* # line 1396 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=5, b=\"five\", c=\"ROW 5\", d=\"row_five\")");
      IIsyncup((char *)0,0);
    }
/* # line 1397 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6, b=\"six\", c=\"ROW 6\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1398 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=7, b=\"seven\", c=\"ROW 7\", d=\"row_seven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1399 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=8, b=\"eight\", c=\"ROW 8\")");
      IIsyncup((char *)0,0);
    }
/* # line 1400 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=9, b=\"nine\", c=\"ROW 9\", d=\"row_nine\")");
      IIsyncup((char *)0,0);
    }
/* # line 1401 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=10, b=\"ten\", c=\"ROW 10\")");
      IIsyncup((char *)0,0);
    }
/* # line 1402 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=11, b=\"eleven\", c=\"ROW 11\", d=\"row_eleven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1403 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=12, b=\"twelve\", c=\"ROW 12\")");
      IIsyncup((char *)0,0);
    }
/* # line 1404 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=13, b=\"thirteen\", c=\"ROW 13\", d=\"row_thirteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1405 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=14, b=\"fourteen\", c=\"ROW 14\")");
      IIsyncup((char *)0,0);
    }
/* # line 1406 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=15, b=\"fifteen\", c=\"ROW 15\", d=\"row_fifteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1407 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=16, b=\"sixteen\", c=\"ROW 16\")");
      IIsyncup((char *)0,0);
    }
/* # line 1408 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=17, b=\"seventeen\", c=\"ROW 17\", d=\"row_seventeen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1409 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=18, b=\"eighteen\", c=\"ROW 18\")");
      IIsyncup((char *)0,0);
    }
/* # line 1410 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=19, b=\"nineteen\", c=\"ROW 19\", d=\"row_nineteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1411 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=20, b=\"twenty\", c=\"ROW 20\")");
      IIsyncup((char *)0,0);
    }
/* # line 1412 "msfctest.qc" */	/* replace */
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
/* # line 1413 "msfctest.qc" */	/* replace */
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
/* # line 1414 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+\"_new\")");
      IIsyncup((char *)0,0);
    }
/* # line 1415 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=\"NOT_NULL\")where ");
      IIwritio(1,(short *)0,1,32,0,comptab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=18");
      IIsyncup((char *)0,0);
    }
/* # line 1416 "msfctest.qc" */	/* delete */
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
/* # line 1417 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1421 "msfctest.qc" */	/* host code */
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
    MEfill(1000, ' ', blah_data);
    MEcopy("LARGE_ROW   ", 9, blah_data);
    large_data2 = blah_data;
/* # line 1437 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1438 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)");
      IIsyncup((char *)0,0);
    }
/* # line 1440 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to chash");
      IIsyncup((char *)0,0);
    }
/* # line 1442 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1443 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=1, b=\"one\", c=\"ROW 1\", d=\"row_one\")");
      IIsyncup((char *)0,0);
    }
/* # line 1444 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=\"two\", c=\"ROW 2\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1445 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=3, b=\"three\", c=\"ROW 3\", d=\"row_three\")");
      IIsyncup((char *)0,0);
    }
/* # line 1446 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=\"four\", c=\"ROW \
4\")");
      IIsyncup((char *)0,0);
    }
/* # line 1447 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=5, b=\"five\", c=\"ROW 5\", d=\"row_five\")");
      IIsyncup((char *)0,0);
    }
/* # line 1448 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6, b=\"six\", c=\"ROW 6\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1449 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=7, b=\"seven\", c=\"ROW 7\", d=\"row_seven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1450 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=8, b=\"eight\", c=\"ROW 8\")");
      IIsyncup((char *)0,0);
    }
/* # line 1451 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=9, b=\"nine\", c=\"ROW 9\", d=\"row_nine\")");
      IIsyncup((char *)0,0);
    }
/* # line 1452 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=10, b=\"ten\", c=\"ROW 10\")");
      IIsyncup((char *)0,0);
    }
/* # line 1453 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=11, b=\"eleven\", c=\"ROW 11\", d=\"row_eleven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1454 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=12, b=\"twelve\", c=\"ROW 12\")");
      IIsyncup((char *)0,0);
    }
/* # line 1455 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=13, b=\"thirteen\", c=\"ROW 13\", d=\"row_thirteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1456 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=14, b=\"fourteen\", c=\"ROW 14\")");
      IIsyncup((char *)0,0);
    }
/* # line 1457 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=15, b=\"fifteen\", c=\"ROW 15\", d=\"row_fifteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1458 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=16, b=\"sixteen\", c=\"ROW 16\")");
      IIsyncup((char *)0,0);
    }
/* # line 1459 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=17, b=\"seventeen\", c=\"ROW 17\", d=\"row_seventeen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1460 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=18, b=\"eighteen\", c=\"ROW 18\")");
      IIsyncup((char *)0,0);
    }
/* # line 1461 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=19, b=\"nineteen\", c=\"ROW 19\", d=\"row_nineteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1462 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=20, b=\"twenty\", c=\"ROW 20\")");
      IIsyncup((char *)0,0);
    }
/* # line 1463 "msfctest.qc" */	/* replace */
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
/* # line 1464 "msfctest.qc" */	/* replace */
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
/* # line 1465 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+\"_new\")");
      IIsyncup((char *)0,0);
    }
/* # line 1466 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=\"NOT_NULL\")where ");
      IIwritio(1,(short *)0,1,32,0,comptab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=18");
      IIsyncup((char *)0,0);
    }
/* # line 1467 "msfctest.qc" */	/* delete */
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
/* # line 1468 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1472 "msfctest.qc" */	/* host code */
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
    MEfill(1000, ' ', blah_data);
    MEcopy("LARGE_ROW   ", 9, blah_data);
    large_data3 = blah_data;
/* # line 1488 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1489 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)");
      IIsyncup((char *)0,0);
    }
/* # line 1491 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cisam");
      IIsyncup((char *)0,0);
    }
/* # line 1493 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1494 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=1, b=\"one\", c=\"ROW 1\", d=\"row_one\")");
      IIsyncup((char *)0,0);
    }
/* # line 1495 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=\"two\", c=\"ROW 2\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1496 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=3, b=\"three\", c=\"ROW 3\", d=\"row_three\")");
      IIsyncup((char *)0,0);
    }
/* # line 1497 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=\"four\", c=\"ROW \
4\")");
      IIsyncup((char *)0,0);
    }
/* # line 1498 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=5, b=\"five\", c=\"ROW 5\", d=\"row_five\")");
      IIsyncup((char *)0,0);
    }
/* # line 1499 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6, b=\"six\", c=\"ROW 6\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1500 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=7, b=\"seven\", c=\"ROW 7\", d=\"row_seven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1501 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=8, b=\"eight\", c=\"ROW 8\")");
      IIsyncup((char *)0,0);
    }
/* # line 1502 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=9, b=\"nine\", c=\"ROW 9\", d=\"row_nine\")");
      IIsyncup((char *)0,0);
    }
/* # line 1503 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=10, b=\"ten\", c=\"ROW 10\")");
      IIsyncup((char *)0,0);
    }
/* # line 1504 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=11, b=\"eleven\", c=\"ROW 11\", d=\"row_eleven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1505 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=12, b=\"twelve\", c=\"ROW 12\")");
      IIsyncup((char *)0,0);
    }
/* # line 1506 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=13, b=\"thirteen\", c=\"ROW 13\", d=\"row_thirteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1507 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=14, b=\"fourteen\", c=\"ROW 14\")");
      IIsyncup((char *)0,0);
    }
/* # line 1508 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=15, b=\"fifteen\", c=\"ROW 15\", d=\"row_fifteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1509 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=16, b=\"sixteen\", c=\"ROW 16\")");
      IIsyncup((char *)0,0);
    }
/* # line 1510 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=17, b=\"seventeen\", c=\"ROW 17\", d=\"row_seventeen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1511 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=18, b=\"eighteen\", c=\"ROW 18\")");
      IIsyncup((char *)0,0);
    }
/* # line 1512 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=19, b=\"nineteen\", c=\"ROW 19\", d=\"row_nineteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1513 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=20, b=\"twenty\", c=\"ROW 20\")");
      IIsyncup((char *)0,0);
    }
/* # line 1514 "msfctest.qc" */	/* replace */
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
/* # line 1515 "msfctest.qc" */	/* replace */
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
/* # line 1516 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+\"_new\")");
      IIsyncup((char *)0,0);
    }
/* # line 1517 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=\"NOT_NULL\")where ");
      IIwritio(1,(short *)0,1,32,0,comptab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=18");
      IIsyncup((char *)0,0);
    }
/* # line 1518 "msfctest.qc" */	/* delete */
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
/* # line 1519 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1523 "msfctest.qc" */	/* host code */
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
    MEfill(1000, ' ', blah_data);
    MEcopy("LARGE_ROW   ", 9, blah_data);
    large_data4 = blah_data;
/* # line 1539 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1540 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=i4, b=char(50)with null, c=varchar(25), d=varchar(1000)with null)");
      IIsyncup((char *)0,0);
    }
/* # line 1542 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cbtree");
      IIsyncup((char *)0,0);
    }
/* # line 1544 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1545 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=1, b=\"one\", c=\"ROW 1\", d=\"row_one\")");
      IIsyncup((char *)0,0);
    }
/* # line 1546 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=\"two\", c=\"ROW 2\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1547 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=3, b=\"three\", c=\"ROW 3\", d=\"row_three\")");
      IIsyncup((char *)0,0);
    }
/* # line 1548 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=4, b=\"four\", c=\"ROW \
4\")");
      IIsyncup((char *)0,0);
    }
/* # line 1549 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=5, b=\"five\", c=\"ROW 5\", d=\"row_five\")");
      IIsyncup((char *)0,0);
    }
/* # line 1550 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=6, b=\"six\", c=\"ROW 6\
\")");
      IIsyncup((char *)0,0);
    }
/* # line 1551 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=7, b=\"seven\", c=\"ROW 7\", d=\"row_seven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1552 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=8, b=\"eight\", c=\"ROW 8\")");
      IIsyncup((char *)0,0);
    }
/* # line 1553 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=9, b=\"nine\", c=\"ROW 9\", d=\"row_nine\")");
      IIsyncup((char *)0,0);
    }
/* # line 1554 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=10, b=\"ten\", c=\"ROW 10\")");
      IIsyncup((char *)0,0);
    }
/* # line 1555 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=11, b=\"eleven\", c=\"ROW 11\", d=\"row_eleven\")");
      IIsyncup((char *)0,0);
    }
/* # line 1556 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=12, b=\"twelve\", c=\"ROW 12\")");
      IIsyncup((char *)0,0);
    }
/* # line 1557 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=13, b=\"thirteen\", c=\"ROW 13\", d=\"row_thirteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1558 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=14, b=\"fourteen\", c=\"ROW 14\")");
      IIsyncup((char *)0,0);
    }
/* # line 1559 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=15, b=\"fifteen\", c=\"ROW 15\", d=\"row_fifteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1560 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=16, b=\"sixteen\", c=\"ROW 16\")");
      IIsyncup((char *)0,0);
    }
/* # line 1561 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=17, b=\"seventeen\", c=\"ROW 17\", d=\"row_seventeen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1562 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=18, b=\"eighteen\", c=\"ROW 18\")");
      IIsyncup((char *)0,0);
    }
/* # line 1563 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=19, b=\"nineteen\", c=\"ROW 19\", d=\"row_nineteen\")");
      IIsyncup((char *)0,0);
    }
/* # line 1564 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)
"(a=20, b=\"twenty\", c=\"ROW 20\")");
      IIsyncup((char *)0,0);
    }
/* # line 1565 "msfctest.qc" */	/* replace */
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
/* # line 1566 "msfctest.qc" */	/* replace */
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
/* # line 1567 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(c=");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".c+\"_new\")");
      IIsyncup((char *)0,0);
    }
/* # line 1568 "msfctest.qc" */	/* replace */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"replace ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(d=\"NOT_NULL\")where ");
      IIwritio(1,(short *)0,1,32,0,comptab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a=18");
      IIsyncup((char *)0,0);
    }
/* # line 1569 "msfctest.qc" */	/* delete */
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
/* # line 1570 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1574 "msfctest.qc" */	/* host code */
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
/* # line 1595 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1597 "msfctest.qc" */	/* host code */
    /*
    ** Get a sync table lock to make sure only one session executes
    ** this create sequence at a time.
    */
    TRdisplay("Create CONCUR_SYNC_TABLE\n");
    TRdisplay("This may cause a duplicate table error message\n");
/* # line 1603 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create concur_sync_table(a\
=i4)");
      IIsyncup((char *)0,0);
    }
/* # line 1604 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"modify concur_sync_table to heap");
      IIsyncup((char *)0,0);
    }
/* # line 1606 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIsyncup((char *)0,0);
    }
/* # line 1607 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIsyncup((char *)0,0);
    }
/* # line 1608 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIsyncup((char *)0,0);
    }
/* # line 1609 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIsyncup((char *)0,0);
    }
/* # line 1610 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIsyncup((char *)0,0);
    }
/* # line 1611 "msfctest.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIsyncup((char *)0,0);
    }
/* # line 1613 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=c100)");
      IIsyncup((char *)0,0);
    }
/* # line 1614 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=c100)");
      IIsyncup((char *)0,0);
    }
/* # line 1615 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=c100)");
      IIsyncup((char *)0,0);
    }
/* # line 1616 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=c100)");
      IIsyncup((char *)0,0);
    }
/* # line 1617 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=c100)");
      IIsyncup((char *)0,0);
    }
/* # line 1618 "msfctest.qc" */	/* create */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"create ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=i4, b=i4, c=c100)");
      IIsyncup((char *)0,0);
    }
/* # line 1620 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to isam");
      IIsyncup((char *)0,0);
    }
/* # line 1621 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab2_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to hash");
      IIsyncup((char *)0,0);
    }
/* # line 1622 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab3_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to btree");
      IIsyncup((char *)0,0);
    }
/* # line 1623 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab4_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cisam");
      IIsyncup((char *)0,0);
    }
/* # line 1624 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab5_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to chash");
      IIsyncup((char *)0,0);
    }
/* # line 1625 "msfctest.qc" */	/* modify */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"modify ");
      IIwritio(1,(short *)0,1,32,0,concurtab6_name);
      IIwritio(0,(short *)0,1,32,0,(char *)" to cbtree");
      IIsyncup((char *)0,0);
    }
/* # line 1627 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=1, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1628 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=2, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1629 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+2, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1630 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+4, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1631 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+8, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1632 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+16, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1633 "msfctest.qc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append ");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)"(a=");
      IIwritio(1,(short *)0,1,32,0,concurtab1_name);
      IIwritio(0,(short *)0,1,32,0,(char *)".a+32, b=1)");
      IIsyncup((char *)0,0);
    }
/* # line 1635 "msfctest.qc" */	/* append */
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
/* # line 1636 "msfctest.qc" */	/* append */
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
/* # line 1637 "msfctest.qc" */	/* append */
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
/* # line 1638 "msfctest.qc" */	/* append */
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
/* # line 1639 "msfctest.qc" */	/* append */
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
/* # line 1641 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
  }
/* # line 1645 "msfctest.qc" */	/* host code */
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
    for (i = 0 ; i < 10000; i ++)
    {
	/*
	** Use dbms_cpu as a sort of random number generator.
	** Use the divide-by-two because on some systems the
	** number always seems to be an even-number.
	*/
/* # line 1675 "msfctest.qc" */	/* retrieve */
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
/* # line 1676 "msfctest.qc" */	/* host code */
	num = ((num / 2) % 64) + 1;
/* # line 1678 "msfctest.qc" */	/* begin transaction */
    {
      IIxact(0);
    }
/* # line 1680 "msfctest.qc" */	/* replace */
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
/* # line 1682 "msfctest.qc" */	/* replace */
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
/* # line 1684 "msfctest.qc" */	/* replace */
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
/* # line 1686 "msfctest.qc" */	/* replace */
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
/* # line 1688 "msfctest.qc" */	/* replace */
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
/* # line 1690 "msfctest.qc" */	/* replace */
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
/* # line 1693 "msfctest.qc" */	/* end transaction */
    {
      IIxact(1);
    }
/* # line 1694 "msfctest.qc" */	/* host code */
    }
  }
msfc_canon(name, base_num, sess_num)
char	*name;
int	base_num;
int	sess_num;
{
  char *dbname;
    dbname = name;
/* # line 1707 "msfctest.qc" */	/* ingres */
  {
    IIingopen(0,dbname,(char *)0);
  }
/* # line 1709 "msfctest.qc" */	/* host code */
    msfc_test1(base_num, sess_num);
    msfc_test2(base_num, sess_num);
    msfc_test3(base_num, sess_num);
    msfc_test4(base_num, sess_num);
    msfc_test5(base_num, sess_num);
    msfc_test6(base_num, sess_num);
    msfc_test7(base_num, sess_num);
    msfc_test8(base_num, sess_num);
    msfc_test9(base_num, sess_num);
    msfc_test10(base_num, sess_num);
    msfc_test11(base_num, sess_num);
    msfc_test12(base_num, sess_num);
    msfc_test13(base_num, sess_num);
    msfc_test14(base_num, sess_num);
    msfc_test15(base_num, sess_num);
    msfc_test16(base_num, sess_num);
    msfc_test17(base_num, sess_num);
    msfc_test18(base_num, sess_num);
    msfc_test19(base_num, sess_num);
    msfc_test20(base_num, sess_num);
    msfc_test21(base_num, sess_num);
    msfc_test22(base_num, sess_num);
    msfc_test23(base_num, sess_num);
    msfc_test24(base_num, sess_num);
    msfc_test25(base_num, sess_num);
    msfc_test26(base_num, sess_num);
    msfc_test27(base_num, sess_num);
    msfc_test28(base_num, sess_num);
/* # line 1738 "msfctest.qc" */	/* exit */
  {
    IIexit();
  }
/* # line 1739 "msfctest.qc" */	/* host code */
}
