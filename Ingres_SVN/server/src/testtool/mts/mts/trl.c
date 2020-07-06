# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* History:
** --------
** 02/09/93 (erniec)
**      modify "key" fields to "key1", "key" is now a keyword in 6.5
**
**	create tst68-71 to do DSQL selects from pre-created views
**	these are new test routines for testcase 4 (xaa04s.sep)
**		instead of tst32-35 which did create view, select, drop view
**
**
** 02/03/93 (erniec)
**	removed unused ERR_DEADLOCK, ERR_OUT_OF_LOCKS, ERR_LOGFULL
**	all tst() test routines - add trace message if (sqlca.sqlcode < 0)
**	add variable levels of tracing information, traceflag>1 for debugging
**		traceflag = 1 prints messages on all errors encountered
**	tst26 - modify if statements on cursor opens
**	tst29 - modify error messages
**	tst32-35 - modify error handling: dropping views, freeing memory, and
**		logging errors with "goto", set lockmode timeout
**		generate specific view names for each routine
**	tst36-40 - set lockmode timeout for undetectable distributed deadlock
**	tst37 - remove 40X loop, we can control with testreps
**	tst52 - modify if statements on cursor opens
**
**
** 08/13/92 (erniec)
**	Corrected tst33 and tst34 to check the result column data type
**	If it is money (+-5) or date (+-3) "describe" sets sqllen to 0.
**	If it is either of these 2 types, set the calloc "len" such that
**	len = 8 for money and len = 25 for date.
**
**	Correct tst50 to open cursor c6b instead of c6.
**
**
**	06/10/92	Changed tst18() to call error handling routine
**			merr instead of sqlprint
**
**
**	add code for routine 99, a performance routine used in 
**	mtstest10-based tests using the HISTORY table.
**
**	Added entries for tests beyond 40, up to 99.
**
**	The entries are in the declaration and the GLOBALDEF,
**	and stub routines are in place to receive functional
**	code.
**		js 2/3/92
**
**	Renamed existing functions tst22() to tst28(), tst23() to tst29() and
**	tst24() to tst30(), then added new functions tst22() to tst27() 
**	inclusive.
**			Steve R. on 15-Aug-1990
**
**	26-aug-1993 (dianeh)
**		Added #include of <st.h>.
**
**	29-sep-1995 (somsa01)
**		Added definition of sleep(x) to be Sleep(x*1000) (Win32), since
**		sleep is not defined on NT_GENERIC platforms.
**	30-Sep-1998 (merja01)
**      Change sql long types to i4 to make test protable to axp_osf.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/* # line 73 "trl.sc" */	/* host code */
# include	<compat.h>
# include	<si.h>
# include	<st.h>
# include 	<mh.h>
# include 	<lo.h>
# define	ERR_DROP 	-2753
# define	TBLSIZE		 5120	/* for tst28, i.e. the  old tst22 */
# define	BRANCHS_PER_BANK	10
# define	TELLERS_PER_BRANCH	10
# define	ACCOUNTS_PER_BRANCH	1000
# define	LARGEST_DEPOSIT		1000
# define	MAX_SCAN		100
# define	DATE_TYPE		3
# define	MONEY_TYPE		5
# define	DATE_LEN		25
# define	MONEY_LEN		8
# ifdef NT_GENERIC
# define	sleep(x)		Sleep(x*1000)
# endif
/*
**
**	each test routine must have an entry in the 
**	following list.  js 2/3/92
**
*/
int	tst0();
int 	tst1();
int 	tst2();
int 	tst3();
int 	tst4();
int 	tst5();
int 	tst6();
int 	tst7();
int 	tst8();
int 	tst9();
int	tst10();
int	tst11();
int	tst12();
int	tst13();
int	tst14();
int	tst15();
int	tst16();
int	tst17();
int	tst18();
int	tst19();
int	tst20();
int	tst21();
int	tst22();
int	tst23();
int	tst24();
int	tst25();
int	tst26();
int	tst27();
int	tst28();
int	tst29();
int	tst30();
int	tst31();
int	tst32();
int	tst33();
int	tst34();
int	tst35();
int	tst36();
int	tst37();
int	tst38();
int	tst39();
int	tst40();
int	tst41();
int	tst42();
int	tst43();
int	tst44();
int	tst45();
int	tst46();
int	tst47();
int	tst48();
int	tst49();
int	tst50();
int	tst51();
int	tst52();
int	tst53();
int	tst54();
int	tst55();
int	tst56();
int	tst57();
int	tst58();
int	tst59();
int	tst60();
int	tst61();
int	tst62();
int	tst63();
int	tst64();
int	tst65();
int	tst66();
int	tst67();
int	tst68();
int	tst69();
int	tst70();
int	tst71();
int	tst72();
int	tst73();
int	tst74();
int	tst75();
int	tst76();
int	tst77();
int	tst78();
int	tst79();
int	tst80();
int	tst81();
int	tst82();
int	tst83();
int	tst84();
int	tst85();
int	tst86();
int	tst87();
int	tst88();
int	tst89();
int	tst90();
int	tst91();
int	tst92();
int	tst93();
int	tst94();
int	tst95();
int	tst96();
int	tst97();
int	tst98();
int	tst99();
GLOBALREF     i4 traceflag;              /* flag to print detailed results */
/*
**	each test routine must have an entry in
**	the following GLOBALDEF.  js 2/3/92
**
*/
GLOBALDEF	struct	trl_routine {
	int	(*func)();
} trl[] = {	
	tst0,
	tst1,
	tst2,
	tst3,
	tst4,
	tst5,
	tst6,
	tst7,
	tst8,
	tst9,
	tst10,
	tst11,
	tst12,
	tst13,
	tst14,
	tst15,
	tst16,
	tst17,
	tst18,
	tst19,
	tst20,
	tst21,
	tst22,
	tst23,
	tst24,
	tst25,
	tst26,
	tst27,
	tst28,
	tst29,
	tst30,
	tst31,
	tst32,
	tst33,
	tst34,
	tst35,
	tst36,
	tst37,
	tst38,
	tst39,
	tst40,
	tst41,
	tst42,
	tst43,
	tst44,
	tst45,
	tst46,
	tst47,
	tst48,
	tst49,
	tst50,
	tst51,
	tst52,
	tst53,
	tst54,
	tst55,
	tst56,
	tst57,
	tst58,
	tst59,
	tst60,
	tst61,
	tst62,
	tst63,
	tst64,
	tst65,
	tst66,
	tst67,
	tst68,
	tst69,
	tst70,
	tst71,
	tst72,
	tst73,
	tst74,
	tst75,
	tst76,
	tst77,
	tst78,
	tst79,
	tst80,
	tst81,
	tst82,
	tst83,
	tst84,
	tst85,
	tst86,
	tst87,
	tst88,
	tst89,
	tst90,
	tst91,
	tst92,
	tst93,
	tst94,
	tst95,
	tst96,
	tst97,
	tst98,
	tst99
};
/*
**
**	Be sure that last one does NOT have a 
**	comma at the end.  JS  2/3/92
**
*/
int 
trlsize()
{
	return ( (sizeof(trl)/sizeof(struct trl_routine)) - 1);
}
tst0(s,c)
  int s;
  int c;
{
/* # line 359 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into test(num, slave, routs, s, c)values(100, 0, 0, ");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 362 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("insert into test - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 371 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 372 "trl.sc" */	/* host code */
}
tst1(s,c)
  int s;
  int c;
{
  int num;
/* # line 385 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1 from data1 where cnt=");
    IIputdomio((short *)0,1,30,4,&c);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 393 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("select key1 from data1 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 402 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 403 "trl.sc" */	/* host code */
}
tst2(s,c)
  int s;
  int c;
{
/* # line 412 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(key1, slave, rout)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)", 2)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 415 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("insert into data2 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 424 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 425 "trl.sc" */	/* host code */
}
tst3(s,c)
  int s;
  int c;
{
/* # line 435 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data3(i1, i2, i4, f4, f8, date, char61)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +10, ");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*5, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)
"*10, '31-dec-1999', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 439 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("insert into data3 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 448 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 449 "trl.sc" */	/* host code */
}
tst4(s,c)
  int s;
  int c;
{
/* # line 459 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from data3 where i1=");
    IIputdomio((short *)0,1,30,4,&c);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 461 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("delete from data3 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 470 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 471 "trl.sc" */	/* host code */
}
tst5(s,c)
  int s;
  int c;
{
/* # line 481 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data3 set i1=i1 +1, i\
4=");
    IIputdomio((short *)0,1,30,4,&s);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 485 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update data3 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 494 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 495 "trl.sc" */	/* host code */
}
tst6(s,c)
  int s;
  int c;
{
  int num;
  int r, m, iterations, i, seed, x;
/* # line 508 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select seed_val from seeds");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&seed);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 512 "trl.sc" */	/* host code */
	srand( seed+(s*c) );
	num = rand();
	r = num%(10000-1) + 1;
	m = r%5;
	if ( m == 0 )
	{
/*		SIprintf ("m is %d, appending %d\n",m,r); */
/*		SIflush(stdout); */
/* # line 524 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl1",7554,15668);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl1",7554,15668);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data4(i1, i2, i4, f4, f8, date, money, char61a, char100)v\
alues(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)"*2, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*5, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)
"*10, '31-dec-1999', 12.12, 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'\
, 'yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl1",7554,15668);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 529 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("m=0: repeated insert into data4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
		if ( r%2 == 0 )
		{
/*			SIprintf ("also deleting %d\n",r); */
/*			SIflush(stdout); */
/* # line 544 "trl.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl2",7555,9459);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl2",7555,9459);
        IIwritio(0,(short *)0,1,32,0,(char *)"delete from data4 where \
i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl2",7555,9459);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 545 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
				{
					SIprintf("m=0: repeated delete from data4 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		};
/* # line 555 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 556 "trl.sc" */	/* host code */
	};
	if ( m == 1 )
	{
/*		SIprintf ("m is %d, appending & deleting %d\n",m,r); */
/*		SIflush(stdout); */
/* # line 563 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl3",7556,8889);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl3",7556,8889);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data4(i1, i2, i4, f4, f8, date, money, char61a, char100)v\
alues(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)"*2, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*5, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)
"*10, '31-dec-1999', 17.23, 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'\
, 'yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl3",7556,8889);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 569 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("m=1: repeated insert into data4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 580 "trl.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl4",7557,8699);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl4",7557,8699);
        IIwritio(0,(short *)0,1,32,0,(char *)"delete from data4 where \
i2>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)"and i2<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)"*2 and i4>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl4",7557,8699);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 582 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("m=1: repeated delete from data4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 591 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 592 "trl.sc" */	/* host code */
	};
	if ( m == 2 )
	{
/*		SIprintf ("m is %d, replacing r %d\n",m,r); */
/*		SIflush(stdout); */
/* # line 599 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl5",7558,10224);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl5",7558,10224);
        IIwritio(0,(short *)0,1,32,0,(char *)"update data4 set i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)" +1, i4=i4 +1000 where i\
2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)"and i4>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl5",7558,10224);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 602 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("m=2: repeated update data4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 611 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 612 "trl.sc" */	/* host code */
	};
	if ( m == 3 )
	{
/*		SIprintf ("m is %d, retrieving with readlock no lock\n",m); */
/*		SIflush(stdout); */
/* # line 619 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 620 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on data4 where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 621 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"select i2 from data4");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&r);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 625 "trl.sc" */	/* host code */
			/* SELECT loop, disgard the data */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 627 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 628 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on data4 where readlock=shared");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 629 "trl.sc" */	/* host code */
	};
	if ( m == 4 )
	{
/*		SIprintf ("m is %d, retrieve, delete, append and replace %d\n",m,r); */
/*		SIflush(stdout); */
/* # line 636 "trl.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl6",7559,24630);
      IIputdomio((short *)0,1,30,4,&r);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl6",7559,24630);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select i2 from data4 where i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl6",7559,24630);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 641 "trl.sc" */	/* host code */
			/* SELECT loop, disgard the data */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 645 "trl.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl7",7560,26040);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl7",7560,26040);
        IIwritio(0,(short *)0,1,32,0,(char *)"delete from data4 where \
i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl7",7560,26040);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 647 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("m=4: repeated delete from data4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 657 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl8",7561,27380);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl8",7561,27380);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data4(i1, i2, i4, f4, f8, date, money, char61a, char100)v\
alues(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)"*2, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*5, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)
"*10, '31-dec-1999', 19.95, 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'\
, 'yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl8",7561,27380);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 662 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("m=4: repeated insert into data4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 672 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl9",7562,27125);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl9",7562,27125);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data4 set i2=1, i4=1000 where i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl9",7562,27125);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 676 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("m=4: repeated update data4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 685 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 686 "trl.sc" */	/* host code */
	};
}
tst7(s,c)
  int s;
  int c;
{
  int i;
	for (i=1; i<c; i++)
	{
/* # line 703 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl10",7563,1077);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl10",7563,1077);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data3(i1, i2, i4, f4, f8, date, money, char61)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)" +10, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*5, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)
"*10, '31-dec-1999', 737.93, 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
')");
        IIexDefine(0,(char *)"trl10",7563,1077);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 708 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("repeated insert into data3 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
	};
/* # line 719 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 720 "trl.sc" */	/* host code */
}
tst8(s,c)
  int s;
  int c;
{
/* # line 730 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from data3 where i1=");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i2=");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +10 and f4=");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*5");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 732 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("delete from data3 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 741 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 742 "trl.sc" */	/* host code */
}
tst9(s,c)
  int s;
  int c;
{
/* # line 751 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data3 set i2=i2 -100, i4=i4*2 where date='31-dec-1999'");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 755 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update data3 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 764 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 765 "trl.sc" */	/* host code */
}
tst10(s,c)
  int s;
  int c;
{
	/* this routine appends 128 rows to the table width500 */
	/* rows added will be 1000 + (slave_no * 128) */
	int	i;
	int	base;
	int	first;
	int	last;
  int v_i2;
  int v_f4;
  int v_f8;
  int v_dollar;
	base = 1000;
	first = base + (s + c - 2)*128 + 1;
	last = base + (s + c - 1)*128 + 1;
	for (i=first; i<last; i++)
	{
		v_i2 = i ;
 	  	v_f4 = i ;
	  	v_f8 = i ;
	  	v_dollar = i;
/*		SIprintf ("appending tuple where i2 = %d\n\n",i);	*/
/* # line 803 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl11",7564,108);
      IIputdomio((short *)0,1,30,4,&v_i2);
      IIputdomio((short *)0,1,30,4,&v_f4);
      IIputdomio((short *)0,1,30,4,&v_f8);
      IIputdomio((short *)0,1,30,4,&v_dollar);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl11",7564,108);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into width500(i2, f4, f8, dollar, day, c6, txt58, txt98one, tx\
t98two, txt98three, txt98four)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&v_i2);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&v_f4);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&v_f8);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&v_dollar);
        IIwritio(0,(short *)0,1,32,0,(char *)
", '1/1/1987', 'abcedf', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx')");
        IIexDefine(0,(char *)"trl11",7564,108);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 813 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("repeated insert into width500 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
	} /* end for loop*/
/* # line 824 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 825 "trl.sc" */	/* host code */
}
tst11(s,c)
  int s;
  int c;
{
	/* used with tst12, will lock 50 tuples, 12 pages of data */
/* # line 837 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update width500 set f4=f4*100, txt58='tst11_txt58_replaced_value' whe\
re i2>50 and i2<80");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 841 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update width500  where i2 > 50 and i2 < 80 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 851 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 852 "trl.sc" */	/* host code */
}
tst12(s,c)
  int s;
  int c;
{
/* # line 862 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update width500 set f4=f4 -100, txt58='tst12_txt58_replaced_value' wh\
ere i2>100 and i2<150");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 865 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update width500 where i2 > 100 and i2 < 150 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 875 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 876 "trl.sc" */	/* host code */
}
tst13(s,c)
  int s;
  int c;
{
/* # line 886 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update width500 set f4=f4 +500, txt58='tst13_txt58_replaced_value' wh\
ere i2>50 and i2<80");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 890 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update width500 where i2 > 50 and i2 < 80 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 900 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 901 "trl.sc" */	/* host code */
}
tst14(s,c)
  int s;
  int c;
{
/* # line 911 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update width500 set f4=1, txt58='tst14_txt58_replaced_value' where i2\
>70 and i2<100");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 915 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update width500 where i2 > 70 and i2 < 100 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 925 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 926 "trl.sc" */	/* host code */
}
tst15(s,c)
  int s;
  int c;
{
	/* Used with tst11 will lock the same 128 rows, 32 pages */
	/* retrieve into slave_no * 128 rows into a temp table */
	/* since there are 4 tuples per page, 128 rows will have 32 pages */
	/* which is enough to fill up the AM buffer. */
/* # line 943 "trl.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ret_temp as select * from width500 where i2<");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)"*128");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 947 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("create table ret_temp from width500 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 957 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 958 "trl.sc" */	/* host code */
}
tst16(s,c)
  int s;
  int c;
{
	/* Same as tst15 except set lockmode sessions where readlock=nolock */
	/* retrieve into slave_no * 128 rows into a temp table */
	/* since there are 4 tuples per page, 128 rows will have 32 pages */
	/* which is enough to fill up the AM buffer. */
/* # line 975 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 976 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 978 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("set lockmode readlock = nolock - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 988 "trl.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ret_temp as select * from width500 where i2<");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)"*128");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 992 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("create table ret_temp from width500 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 1002 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1003 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where readlock=shared");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1004 "trl.sc" */	/* host code */
}
tst17(s,c)
  int s;
  int c;
{
/*
**	***************************************
**	*                                     *
**	* This will NOT work with STAR..      *
**	*                                     *
**	* STAR does not permit modifying the  *
**	* structure of tables.                *
**	*                                     *
**      ***************************************
*/
	/* modify widht500 to other structures, should get Xclusive table lock */
	switch (c) {
	case '1':
               if (traceflag)
                {
		SIprintf ("Modifying to btree\n\n");
		SIflush (stdout);
		}
/* # line 1035 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify width500 to btree with fillfactor=95, indexfill=85");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1037 "trl.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index width500ind on width500(f8, dollar, day, c6, txt98four)w\
ith structure=hash");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1040 "trl.sc" */	/* host code */
		break;
	case '2':
               if (traceflag)
                {
		SIprintf ("Modifying to isam\n\n");
		SIflush (stdout);
		}
/* # line 1048 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify width500 to isam with fillfactor=95");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1050 "trl.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index width500ind on width500(txt58, txt98one, txt98two, txt98\
three)with structure=btree");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1053 "trl.sc" */	/* host code */
		break;
	case '3':
               if (traceflag)
                {
		SIprintf ("Modifying to hash\n\n");
		SIflush (stdout);
		}
/* # line 1061 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify width500 to chash on f4, f8 with fillfactor=90, maxpages=100");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1063 "trl.sc" */	/* host code */
		break;
	case '4':
               if (traceflag)
                {
		SIprintf ("Modifying to cbtree\n\n");
		SIflush (stdout);
		}
/* # line 1071 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify width500 to cbtree");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1072 "trl.sc" */	/* host code */
		break;
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 1080 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1081 "trl.sc" */	/* host code */
}
tst18(s,c)
  int s;
  int c;
{
/* # line 1092 "trl.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table ret_temp");
    IIsyncup((char *)0,0);
  }
/* # line 1095 "trl.sc" */	/* host code */
	/* error -5202, failed to destroy due to non-exist table, it's OK */
	/* since the slave is not expecting this error juct change it to 0 */ 
	/* instead of the actual ingres error no. */
	if (sqlca.sqlcode == ERR_DROP)
	{
		sqlca.sqlcode = 0;
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 1107 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1108 "trl.sc" */	/* host code */
}
/*
**	The purpose of tests 19, 20 and 21 is to exercise the system catalogs in
**	a concurrent environment by doing creates, destroys, modifies,
**	and define permits.
*/
tst19(s,c)
  i4 s;
  int c;
{
  char table_name[33];
  char init_table[33];
  char query[100];
	char	string[20];
/*	IN THIS TEST WE USE A DIFFERENT TABLE FOR EACH SLAVE.
**	THE DATABASE SHOULD HAVE TABLES LIKE DATA10_1, DATA10_2
**	DATA10_3, ETC.
*/
	CVla(s, string);
	STcopy("tbl", table_name);
	STcat(table_name, string);
	STcopy("data10_", init_table);
	STcat(init_table, string);
        if (traceflag)
        {
	SIprintf ("\nCreating the table %s\n",table_name);
	SIflush(stdout);
	}
 	STprintf(query,"create table %s as select * from %s",table_name,init_table);
/* # line 1150 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1152 "trl.sc" */	/* host code */
	if (sqlca.sqlcode == 0)
		sqlca.sqlcode = 0;
	if (sqlca.sqlcode < 0)
	{
		return;
	}
	STprintf(query,"grant all on table %s to public",table_name);
/* # line 1162 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1164 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 1169 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1171 "trl.sc" */	/* host code */
}
tst20(s,c)
  i4 s;
  int c;
{
/*
**      ***************************************
**      *                                     *
**      * This will NOT work with STAR..      *
**      *                                     *
**      * STAR does not permit modifying the  *
**      * structure of tables.                *
**      *                                     *
**      ***************************************
*/
  char table_name[33];
  char query[100];
  char index_name[33];
  char index1[33], index2[33], index3[33], index4[33];
	char	string[20];
	STcopy("tbl", table_name);
	CVla(s, string);
	STcat(table_name, string);
	STcopy("ind", index_name);
	STcat(index_name, table_name);
        if (traceflag)
        {
	SIprintf ("\nModifying the table %s\n",table_name);
	SIflush(stdout);
 	}
	switch (c) {
	case 1:
               if (traceflag)
                {
		SIprintf ("Modifying to btree unique, 1 index\n\n");
		SIflush(stdout);
		}
		STprintf(query, "modify %s to btree unique with fillfactor=95, indexfill = 50, maxindexfill = 90", 
			table_name);
/* # line 1224 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1226 "trl.sc" */	/* host code */
		STprintf(query, "create index %s on %s (i4, varchar50, date, moneyn,t1n,f8) with structure=hash", 
			index_name, table_name);
/* # line 1229 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1230 "trl.sc" */	/* host code */
		break;
	case 2:
               if (traceflag)
                {
		SIprintf ("Modifying to isam, 4 indeces\n\n");
		SIflush(stdout);
		}
		STcopy(index_name,index1);
		STcopy(index_name,index2);
		STcopy(index_name,index3);
		STcopy(index_name,index4);
		CVla(1, string);
		STcat(index1, string);
		CVla(2, string);
		STcat(index2, string);
		CVla(3, string);
		STcat(index3, string);
		CVla(4, string);
		STcat(index4, string);
		STprintf (query,"modify %s to isam with fillfactor = 25",table_name);
/* # line 1255 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1257 "trl.sc" */	/* host code */
		STprintf (query,"create index %s on %s (i4)", index1, table_name);
/* # line 1260 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1262 "trl.sc" */	/* host code */
		STprintf (query,"create index %s on %s (i2,f4,date) with structure=btree",
			 index2, table_name);
/* # line 1266 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1268 "trl.sc" */	/* host code */
		STprintf (query,"create index %s on %s (t1n) with structure=hash", 
			index3, table_name);
/* # line 1272 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1274 "trl.sc" */	/* host code */
		STprintf (query,"create index %s on %s (varchar50n,char50n) with structure=chash", 
			index4, table_name);
/* # line 1277 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1278 "trl.sc" */	/* host code */
		break;
	case 3:
                if (traceflag)
                {
		SIprintf ("\nModifying to chash\n");
		SIflush(stdout);
		}
		STprintf (query,"modify %s to chash on f4,f8 with fillfactor = 90, maxpages=100, minpages=30", 
			table_name);
/* # line 1290 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1291 "trl.sc" */	/* host code */
		break;
	case 4:
                if (traceflag)
                {
		SIprintf ("\nModifying to cbtree\n");
		SIflush(stdout);
		}
		STprintf (query,"modify %s to cbtree", table_name);
/* # line 1301 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1302 "trl.sc" */	/* host code */
		break;
	case 5:
                if (traceflag)
                {
		SIprintf ("\nModifying to heap \n");
		SIflush(stdout);
		}
		STprintf (query,"modify %s to heap", table_name);
/* # line 1313 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1314 "trl.sc" */	/* host code */
		break;
 	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 1322 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1324 "trl.sc" */	/* host code */
}
tst21(s,c)
  i4 s;
  int c;
{
  char table_name[33];
  char query[100];
	char	string[20];
	STcopy("tbl", table_name);
	CVla(s, string);
	STcat(table_name, string);
        if (traceflag)
        {
	SIprintf ("\nDestroying the table %s\n",table_name);
	SIflush(stdout);
	}
	STprintf(query,"drop table %s", table_name); 
/* # line 1353 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(query);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1355 "trl.sc" */	/* host code */
	if (sqlca.sqlcode == ERR_DROP)
	{
		sqlca.sqlcode = 0;
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 1364 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1366 "trl.sc" */	/* host code */
}
tst22(s,c)
int	s;
int	c;
{
  int d5key1;
  int d5cnt;
  int d6key1;
  float d6float;
  int d6cnt;
  int d7key1;
  float d7money;
  int d8key1;
  char d8char[66];
  int d9key1;
  char d9date[26];
  int done;
  int sleeptime;
	/*  plain cursor, retrieves all rows of a single table  */
/* # line 1407 "trl.sc" */	/* host code */
	/* with qualification and join*/
/* # line 1418 "trl.sc" */	/* host code */
	/*  a join, and ORDER BY  */
/* # line 1430 "trl.sc" */	/* host code */
	/*  another  join, and ORDER BY  */
/* # line 1443 "trl.sc" */	/* host code */
	/*  another plain retrieve */
/* # line 1453 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c5 cursor - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return; 
	}
/* # line 1464 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1466 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on data5 where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1467 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on data6 where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1468 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on data7 where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1469 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on data8 where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1470 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on data9 where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1472 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("set lockmode readlock=nolock - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
	}
/*
**	inserted and commented out; jds 19-SEP-1991
**	seems to be a real problem with locks here...
**
**	and put back 1-nov
*/
	sleeptime = 50 * s;
/*
**	adjust this from 1000 to 100 - 1000 was too long
**	for slave 30    jds   4-NOV-1991 16:14
**
**	and shrink it further to 50  12-nov-1991  jds
*/
	/*  GENERIC RETRIEVE BEGIN  */
        if (traceflag > 2)
        {
	SIprintf( "\nRetrieve data5 only (all)\n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 1509 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c1",7565,11947);
    IIwritio(0,(short *)0,1,32,0,(char *)"select key1, cnt from data5");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c1",7565,11947);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1511 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c1 for readonly - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 1523 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c1",7565,11947,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1525 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf( "fetch c1 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 1535 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1537 "trl.sc" */	/* host code */
/*
**              if (done && !(sqlca.sqlcode < 0))
**              {
**                      SIprintf( "%d, %d\n\0", d5key1, d5cnt);
**                      SIflush(stdout);
**              }
*/
		PCsleep(sleeptime); 	/* wait about "s" seconds */
	}
	if (traceflag > 2)
	{
		SIprintf( "After fetch c1 loop: sqlcode=%d, done=%d\n\0", sqlca.sqlcode, done);
		SIflush(stdout);
	}
        if (traceflag > 2)
        {
	SIprintf( "Join all tables  where data5.cnt < 50\n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 1562 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c2",7566,11058);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.key1, a.cnt, b.floatv, f.money, d.charv, e.date from data5 a\
, data6 b, data7 f, data8 d, data9 e where a.cnt<50 and a.key1=b.key1 \
and a.key1=f.key1 and a.key1=d.key1 and a.key1=e.key1");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c2",7566,11058);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1564 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c2 for readonly - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 1576 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c2",7566,11058,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1579 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf( "fetch c2 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 1589 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1591 "trl.sc" */	/* host code */
/*
**		if (done && !(sqlca.sqlcode < 0))
**		{
**			SIprintf( "%d, %d, %f, %f, %s, %s\n\0",
**			  	 d5key1, d5cnt,d6float,'.',d7money,'.',d8char,d9date );
**			SIflush(stdout);
**		}
*/
		PCsleep(sleeptime); 	/*	wait about "s" seconds	 */
	}
	if (traceflag > 2)
	{
		SIprintf( "After fetch c2 loop: sqlcode=%d, done=%d\n\0", sqlca.sqlcode, done);
		SIflush(stdout);
	}
        if (traceflag > 2)
        {
	SIprintf( "Join: all tables where data5.cnt > 10\n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 1616 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c3",7567,22769);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.key1, a.cnt, b.floatv, f.money, d.charv, e.date from data5 a\
, data6 b, data7 f, data8 d, data9 e where a.cnt>20 and a.key1=b.key1 \
and a.key1=f.key1 and a.key1=d.key1 and a.key1=e.key1 order by a.key1,\
 a.cnt");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c3",7567,22769);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1618 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c3 for readonly - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 1631 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c3",7567,22769,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1634 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf( "fetch c3 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 1644 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1646 "trl.sc" */	/* host code */
/*
**		if (done && !(sqlca.sqlcode < 0))
**		{
**			SIprintf( "%d, %d, %f, %f, %s, %s\n\0",
**			  	 d5key1, d5cnt,d6float,'.',d7money,'.',d8char,d9date );
**			SIflush(stdout);
**		}
*/
		PCsleep(sleeptime); 	/*	wait about "s" seconds	 */
	}
	if (traceflag > 2)
	{
		SIprintf( "After fetch c3 loop: sqlcode=%d, done=%d\n\0", sqlca.sqlcode, done);
		SIflush(stdout);
	}
	if (traceflag > 2)
        {
	SIprintf( "Join: All tables where data5.cnt>10 and data6.cnt<200nt<20 \n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 1673 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c4",7568,24104);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.key1, a.cnt, b.cnt, b.floatv, f.money, d.charv, e.date from \
data5 a, data6 b, data7 f, data8 d, data9 e where a.cnt>10 and b.cnt<2\
00 and a.key1=b.key1 and a.key1=f.key1 and a.key1=d.key1 and a.key1=e.\
key1 order by a.cnt, b.cnt, f.money");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c4",7568,24104);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1675 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c4 for readonly - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 1688 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c4",7568,24104,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsGetio((short *)0,1,30,4,&d6cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1691 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf( "fetch c4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 1701 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1703 "trl.sc" */	/* host code */
/*
**		if (done && !(sqlca.sqlcode < 0))
**		{
**			SIprintf( "%d, %d, %f, %f, %s, %s\n\0",
**			  	 d5key1, d5cnt,d6float,'.',d7money,'.',d8char,d9date );
**			SIflush(stdout);
**		}
*/
		PCsleep(sleeptime); 	/*	wait about "s" seconds	 */
	}
	if (traceflag > 2)
	{
		SIprintf("After fetch c4 loop: sqlcode=%d, done=%d\n\0", sqlca.sqlcode, done);
		SIflush(stdout);
	}
        if (traceflag > 2)
        {
	SIprintf( "Join all tables \n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 1729 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c5",7569,25581);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.key1, a.cnt, b.floatv, f.money, d.charv, e.date from data5 a\
, data6 b, data7 f, data8 d, data9 e where a.key1=b.key1 and a.key1=f.\
key1 and a.key1=d.key1 and a.key1=e.key1");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c5",7569,25581);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1731 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c5 for readonly - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 1743 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c5",7569,25581,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1746 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf( "fetch c5 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 1756 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1758 "trl.sc" */	/* host code */
/*
**		if (done && !(sqlca.sqlcode < 0))
**		{
**			SIprintf( "%d, %d, %f, %f, %s, %s\n\0",
**			  	 d5key1, d5cnt,d6float,'.',d7money,'.',d8char,d9date );
**			SIflush(stdout);
**		}
*/
		PCsleep(sleeptime); 	/*	wait about "s" seconds	 */
	}
	if (traceflag > 2)
	{
		SIprintf( "After fetch c5 loop: sqlcode=%d, done=%d\n\0", sqlca.sqlcode, done);
		SIflush(stdout);
	}
/* # line 1776 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1778 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("\n COMMIT error %d\n\0", sqlca.sqlcode);
		SIflush(stdout);
		return;
	}
} /* End of tst22() */
tst23(s,c)
int	s;
int	c;
{
  int d5key1;
  int d5cnt;
  int done;
	/*  direct update using join */
/* # line 1809 "trl.sc" */	/* host code */
	if (traceflag > 2)
        {
	SIprintf( "\nDirect update where 30<data5.key1<300 \n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 1817 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c6",7570,31332);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, cnt from data5 where key1>30 and key1<300 for direct upd\
ate of key1, cnt");
    IIcsQuery((char *)"c6",7570,31332);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1819 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c6 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 1831 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c6",7570,31332,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1832 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
       			{
				SIprintf("fetch c6 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 1842 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1844 "trl.sc" */	/* host code */
		if (!done && !(sqlca.sqlcode < 0))
		{
/*			SIprintf("Direct   --> key1= %d, cnt=%d\n", d5key1, d5cnt);
**			SIflush(stdout);
*/
/* # line 1849 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data5 set key1=key1 +\
20");
    IIcsERplace((char *)"c6",7570,31332);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1853 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("update data5 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 1870 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1872 "trl.sc" */	/* host code */
} /* End of tst23() */
tst24(s,c)
int	s;
int	c;
{
  int d6key1;
  float d6float;
  int d6cnt;
  int done;
	/*  direct update on the whole table */
	/* Add WHERE clause that is always true to avoid bug 32481 */
/* # line 1900 "trl.sc" */	/* host code */
        if (traceflag > 2)
        {
		SIprintf( "\nDirect update on data6 \n\0" );
		SIflush(stdout);
	}
	done = 0;
/* # line 1908 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c7",7571,19564);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, floatv, cnt from data6 where 1=1 for direct update of ke\
y1, floatv");
    IIcsQuery((char *)"c7",7571,19564);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1910 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c7 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 1922 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c7",7571,19564,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d6key1);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,30,4,&d6cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1923 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf("fetch c7 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 1933 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1934 "trl.sc" */	/* host code */
		if (!done && !(sqlca.sqlcode < 0))
		{
/* # line 1936 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data6 set key1=key1 -10, floatv=floatv*2");
    IIcsERplace((char *)"c7",7571,19564);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1941 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("update data6 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 1958 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 1960 "trl.sc" */	/* host code */
}	/* End of tst24() */
tst25(s,c)
int	s;
int	c;
{
  int d7key1;
  float d7money;
  int done;
/* # line 1984 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("declare c8 cursor - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
        if (traceflag > 2)
        {
	SIprintf( "\nDeferred update on data7 \n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 2003 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c8",7572,18020);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, money from data7 where key1<200 and money>10 for deferre\
d update of key1, money");
    IIcsQuery((char *)"c8",7572,18020);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2005 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("open c8 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 2017 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c8",7572,18020,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d7key1);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2018 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf("fetch c8 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 2028 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2029 "trl.sc" */	/* host code */
		if (!done && !(sqlca.sqlcode < 0))
		{
/* # line 2031 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data7 set key1=key1*2");
    IIcsERplace((char *)"c8",7572,18020);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2035 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("update data7 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 2052 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2054 "trl.sc" */	/* host code */
}	/* End of tst25() */
tst26(s,c)
int	s;
int	c;
{
  int d5key1;
  int d5cnt;
  int d8key1;
  char d8char[66];
  int done;
/* # line 2086 "trl.sc" */	/* host code */
        if (traceflag > 2)
        {
	SIprintf( "\nJoin data5 and data8 and replace data8 \n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 2094 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c9",7573,16617);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.key1, a.cnt from data5 a, data8 d where a.key1=d.key1");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c9",7573,16617);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2096 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("open c9 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 2106 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c10",7574,8616);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, charv from data8 where key1>10 and key1<100 for direct u\
pdate of key1, charv");
    IIcsQuery((char *)"c10",7574,8616);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2108 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
       		{
			SIprintf("open c10 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 2120 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c9",7573,16617,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2121 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf("fetch c9 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 2131 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2132 "trl.sc" */	/* host code */
		if (!done && !(sqlca.sqlcode < 0))
		{
/*
**			SIprintf("c9: %d, %d \n", d5key1, d5cnt);
**			SIflush (stdout);
*/
/* # line 2138 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c10",7574,8616,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d8key1);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2139 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("fetch c10 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
/* # line 2149 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2150 "trl.sc" */	/* host code */
			if (!done && !(sqlca.sqlcode < 0))
			{
/*				SIprintf("c10: %d \n", d8key1);
**				SIflush (stdout);
*/
/* # line 2155 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data8 set key1=key1*2, charv='Test 26 replace '");
    IIcsERplace((char *)"c10",7574,8616);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2160 "trl.sc" */	/* host code */
				if (sqlca.sqlcode < 0)
				{
					if (traceflag)
        				{
						SIprintf("update data8 - FAILED: %d\n", sqlca.sqlcode);
						SIflush(stdout);
					}
					return;
				}
			}
		}
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 2179 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2181 "trl.sc" */	/* host code */
}	/* End of tst26() */
tst27(s,c)
int	s;
int	c;
{
  int d9key1;
  char d9date[26];
  int done;
	/*  cursor for delete */
/* # line 2207 "trl.sc" */	/* host code */
	/* cursor for update */
/* # line 2216 "trl.sc" */	/* host code */
	/*  GENERIC RERIEVE BEGIN  */
        if (traceflag > 2)
        {
	SIprintf( "\nDelete data9 where key1 <10 or key1 > 300 \n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 2226 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c11",7575,12071);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, date from data9 where key1<10 or key1>300 for direct upd\
ate of key1");
    IIcsQuery((char *)"c11",7575,12071);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2228 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("open c11 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 2240 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c11",7575,12071,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d9key1);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2241 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf("fetch c11 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 2251 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2252 "trl.sc" */	/* host code */
		if (!done && !(sqlca.sqlcode < 0))
		{
/*			SIprintf( "%d, %s\n\0", d9key1, d9date);
**			SIflush(stdout);
*/
/* # line 2258 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"data9",(char *)"c11",7575,12071);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2260 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("delete from data9 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
	}
/* # line 2273 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c11",7575,12071);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2275 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("close c11 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
        if (traceflag > 2)
        {
	SIprintf( "\nReplace data9 where 10<key1<300 \n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 2293 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c12",7576,9183);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, date from data9 where key1>10 or key1<300 for direct upd\
ate of key1");
    IIcsQuery((char *)"c12",7576,9183);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2295 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("open c12 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 2307 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c12",7576,9183,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d9key1);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2309 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
       			{
				SIprintf("fetch c12 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 2319 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2320 "trl.sc" */	/* host code */
		if (!done && !(sqlca.sqlcode < 0))
   		{
/*			SIprintf( "%d, %s\n\0", d9key1, d9date);
**			SIflush(stdout);
*/
/* # line 2325 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data9 set key1=key1 +\
20");
    IIcsERplace((char *)"c12",7576,9183);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2329 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("update data9 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
	}
/* # line 2341 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c12",7576,9183);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2343 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("close c12 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 2353 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2355 "trl.sc" */	/* host code */
}	/* End of tst27() */
tst28(s,c)
  i4 s;
  int c;
{
  i4 emp_no;
  i4 smallest_emp_no;
/* # line 2371 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select max(eno)from btbl_1");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&emp_no);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2372 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2373 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on btbl_1 where readlock=shared");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2375 "trl.sc" */	/* host code */
        emp_no++;
/* # line 2377 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into btbl_1(eno, deptno, ename, deptname, manager, avgsal, sal\
es)values(");
    IIputdomio((short *)0,1,30,4,&emp_no);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&emp_no);
    IIwritio(0,(short *)0,1,32,0,(char *)
", 'xxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'dddddddddddddddddddddddddddddddd',\
 'mmmmmmmmmmmmmmmmmmmmmmmmmmm', 30000, 500000)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2394 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("insert into btbl_1 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 2403 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2405 "trl.sc" */	/* host code */
        smallest_emp_no = emp_no - TBLSIZE;
/*	SIprintf ("\n smallest_emp_no =%d\n",smallest_emp_no);	*/
/*	SIflush(stdout);					*/
/* # line 2410 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from btbl_1 where eno\
<=");
    IIputdomio((short *)0,1,30,4,&smallest_emp_no);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2412 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (sqlca.sqlcode < 0)
		{
			SIprintf("delete into btbl_1 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 2421 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2423 "trl.sc" */	/* host code */
}
tst29(s,c)
  i4 s;
  int c;
{
  int v_amount;
  int account_no;
  int branch_no;
  int teller_no;
  float persec;
  int status;
  int seed;
  int time[2];
  int type;
  int selct;
	int     scale = 1;
/* # line 2446 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2448 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on account where readlock=exclusive, level=page");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2451 "trl.sc" */	/* host code */
	/*	Begin the transaction. */
	/*  Pick a branch, teller, account, amount and type. */
	    branch_no = (i4)(MHrand() * (f8)BRANCHS_PER_BANK);
	    teller_no = (i4)(MHrand() * (f8)TELLERS_PER_BRANCH);
	    selct     = (i4)(MHrand() * (f8)MAX_SCAN);
	    account_no =(i4)(MHrand() * (f8)ACCOUNTS_PER_BRANCH);
	    v_amount  =  (i4)(MHrand() * (f8)(LARGEST_DEPOSIT));
	    /*  Change account record. */
/* # line 2461 "trl.sc" */	/* repeat update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl12",7577,7015);
      IIputdomio((short *)0,1,30,4,&v_amount);
      IIputdomio((short *)0,1,30,4,&account_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl12",7577,7015);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update account set sequence=sequence +1, balance=balance +");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)"where(number=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&account_no);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl12",7577,7015);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2465 "trl.sc" */	/* host code */
          if (sqlca.sqlcode < 0)
	  {
		if (traceflag)
        	{
			SIprintf("repeat update account - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
	      	return;
	  }
	    /*  Change teller record. */
/* # line 2478 "trl.sc" */	/* repeat update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl13",7578,2459);
      IIputdomio((short *)0,1,30,4,&v_amount);
      IIputdomio((short *)0,1,30,4,&teller_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl13",7578,2459);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update teller set balance=balance +");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)"where(number=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&teller_no);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl13",7578,2459);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2482 "trl.sc" */	/* host code */
          if (sqlca.sqlcode < 0)
	  {
		if (traceflag)
        	{
			SIprintf("repeat update  teller - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
	      	return;
	  }
	    /*  Change branch record. */
/* # line 2493 "trl.sc" */	/* repeat update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl14",7579,4060);
      IIputdomio((short *)0,1,30,4,&v_amount);
      IIputdomio((short *)0,1,30,4,&branch_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl14",7579,4060);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update branch set balance=balance +");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)"where(number=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&branch_no);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl14",7579,4060);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2497 "trl.sc" */	/* host code */
          if (sqlca.sqlcode < 0)
	  {
		if (traceflag)
        	{
			SIprintf("repeat update branch - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
	      	return;
	  }
	    /*  Update history. */
/* # line 2509 "trl.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl15",7580,13730);
      IIputdomio((short *)0,1,30,4,&account_no);
      IIputdomio((short *)0,1,30,4,&branch_no);
      IIputdomio((short *)0,1,30,4,&teller_no);
      IIputdomio((short *)0,1,30,4,&v_amount);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl15",7580,13730);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into history(acc_number, br_number, tel_number, amount, pad)va\
lues(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&account_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&branch_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&teller_no);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)
", '0123456789              ')");
        IIexDefine(0,(char *)"trl15",7580,13730);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2514 "trl.sc" */	/* host code */
          if (sqlca.sqlcode < 0)
	  {
		if (traceflag)
        	{
			SIprintf("repeat insert into history - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
	      	return;
	  }
/* # line 2524 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2525 "trl.sc" */	/* host code */
};
tst30(s,c)
  i4 s;
  int c;
{
  int v_amount;
  int account_no;
  int branch_no;
  int teller_no;
  float persec;
  int status;
  int seed;
  int time[2];
  int type;
  int selct;
  int return_value;
	int     scale = 1;
/* # line 2549 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on account where readlock=exclusive, level=page");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2552 "trl.sc" */	/* host code */
	/*	Begin the transaction. */
	/*  Pick a branch, teller, account, amount and type. */
	    branch_no = (i4)(MHrand() * (f8)BRANCHS_PER_BANK);
	    teller_no = (i4)(MHrand() * (f8)TELLERS_PER_BRANCH);
	    selct     = (i4)(MHrand() * (f8)MAX_SCAN);
	    account_no =(i4)(MHrand() * (f8)ACCOUNTS_PER_BRANCH);
	    v_amount  =  (i4)(MHrand() * (f8)(LARGEST_DEPOSIT));
/*  Call the stored procedure that updates account record, teller record,
**  branch record and inserts new history record.
*/
/* # line 2565 "trl.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"tp1");
    IILQprvProcValio((char *)"acct_no",0,(short *)0,1,30,4,&account_no);
    IILQprvProcValio((char *)"tell_no",0,(short *)0,1,30,4,&teller_no);
    IILQprvProcValio((char *)"branch",0,(short *)0,1,30,4,&branch_no);
    IILQprvProcValio((char *)"amount",0,(short *)0,1,30,4,&v_amount);
    while (IILQprsProcStatus(0) != 0) {
      if (sqlca.sqlcode < 0) 
        merr();
    } /* IILQprsProcStatus */
    IIeqiqio((short *)0,1,30,4,&return_value,(char *)"iiret");
  }
/* # line 2571 "trl.sc" */	/* host code */
          if (sqlca.sqlcode < 0)
	  {
		if (traceflag)
        	{
			SIprintf("execute procedure tp1 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
	      	return;
	  }
/* # line 2581 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2582 "trl.sc" */	/* host code */
};
tst31(s,c)
  i4 s;
  int c;
{
	sleep (10);
};
tst32(s,c)
  i4 s;
  int c;
{
  char sql_stmt[300];
/* # line 2607 "trl.sc" */	/* host code */
	int		i;	/* index into sqlvar */
	IISQLVAR	*sqv;	/* pointer to sqlvar */
	IISQLDA		*sqlda = (IISQLDA *)0;
	char		viewname[10];
	char		sql_buff[300];
	i4		tempcode = 0;	/* retain sqlcode for error recording */
/* # line 2614 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2615 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2617 "trl.sc" */	/* host code */
	/*
	** Allocate memory for sqlda, enough for 3 columns
	*/
	sqlda = (IISQLDA *)
		calloc(1, IISQDA_HEAD_SIZE + (3 * IISQDA_VAR_SIZE));
        if (sqlda == (IISQLDA *)0)
        {
                SIprintf("*** MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                return;
        }
	sqlda->sqln = 3;   /* number of columns */
	STcopy ("view32_1_1", viewname);
	/* Make a unique viewname by appending the slave and count numbers */
	viewname[7] = (s % 10) + '0';
	viewname[9] = (c % 10) + '0';
	STcopy ("", sql_buff);   /* build SQL statement in buffer */
	STcat (sql_buff, "create view %s as ");
	STcat (sql_buff, "select a.key1, a.cnt, b.floatv, c.charv ");
	STcat (sql_buff, "from data1 a, data6 b, data8 c ");
	STcat (sql_buff, "where a.key1 = b.key1 and a.key1 = c.key1");
	STprintf (sql_stmt, sql_buff, viewname);   /* create view viewname */
/* # line 2646 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2647 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("execute immediate - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 2658 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2660 "trl.sc" */	/* host code */
	/*
	** Execute a query that uses the view
	*/
	STcopy ("", sql_buff);
	STcopy ("", sql_stmt);
	STcat (sql_buff, "select key1, floatv, charv ");
	STcat (sql_buff, "from %s where cnt = %d");
	STprintf (sql_stmt, sql_buff, viewname, c);
	/* select key1, floatv, charv from viewname where cnt = c */
/* # line 2672 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s32",(char *)0,0,sql_stmt);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2673 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("prepare s32 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 2684 "trl.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s32",sqlda,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2685 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("describe s32 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 2697 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c32 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 2708 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c32",7581,18585);
    IIwritio(0,(short *)0,1,32,0,(char *)"s32");
    IIcsQuery((char *)"c32",7581,18585);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2709 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c32 - %s FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
	for (i=0; i < sqlda->sqld; i++)   /* allocate result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   if (traceflag > 4)
           {
		SIprintf("sqv->sqllen: %d\n", sqv->sqllen);
		SIflush(stdout);
	   }
	   sqv->sqldata = (char *) calloc(1,sqv->sqllen);
           if(!sqv->sqldata)
           {
                SIprintf("*** SQLDATA MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                goto close_cursor;
           }
	}
	while (sqlca.sqlcode == 0)
	{
/* # line 2741 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c32",7581,18585,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2743 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c32 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
				SIflush(stdout);
			}
			tempcode = sqlca.sqlcode;
			goto free_sqldata;
		}
	}
free_sqldata:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqv->sqldata); \n");
                SIflush(stdout);
        }
	for (i=0; i < sqlda->sqld; i++)   /* free result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   free ((char *)sqv->sqldata);
	}
close_cursor:
        if (traceflag > 2)
        {
                SIprintf("Going to close c32); \n");
                SIflush(stdout);
        }
/* # line 2775 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c32",7581,18585);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2776 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("close c32 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
drop_view:
        if (traceflag > 2)
        {
                SIprintf("Going to drop view %s \n", viewname);
                SIflush(stdout);
        }
	STcopy ("", sql_stmt);
	STprintf (sql_stmt, "drop view %s", viewname);
/* # line 2794 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2796 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("execute immediate - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
free_sqlda:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqlda) \n");
                SIflush(stdout);
        }
	free ((char *)sqlda);   /* free sqlda memory */
	/* This is an attempt to simulate the normal error handling	*/
	/* If an error is encountered, sqlcode is set to that error	*/
	/* sqlcode was getting reset to 0 by a successful drop_view	*/
	/* Note: the last error encountered will be returned		*/
        if (tempcode < 0)
        {
		if (traceflag)
		{
			SIprintf("error - sqlcode = tempcode: %d\n", tempcode);
			SIflush(stdout);
		}
                sqlca.sqlcode = tempcode;
                return;
        }
/* # line 2832 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2833 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2834 "trl.sc" */	/* host code */
};
tst33(s,c)
  i4 s;
  int c;
{
  char sql_stmt[300];
/* # line 2849 "trl.sc" */	/* host code */
	int		i;	/* index into sqlvar */
	int		len;	/* calloc length */
	IISQLVAR	*sqv;	/* pointer to sqlvar */
	IISQLDA		*sqlda = (IISQLDA *)0;
	char		viewname[10];
	char		sql_buff[300];
	i4		tempcode = 0;	/* retain sqlcode for error recording */
/* # line 2857 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2858 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2859 "trl.sc" */	/* host code */
	/*
	** Allocate memory for sqlda, enough for 3 columns
	*/
	sqlda = (IISQLDA *)
		calloc(1, IISQDA_HEAD_SIZE + (3 * IISQDA_VAR_SIZE));
        if (sqlda == (IISQLDA *)0)
        {
                SIprintf("*** SQLDA MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                return;
        }
	sqlda->sqln = 3;   /* number of columns */
	STcopy ("view33_1_1", viewname);
	/* Make a unique viewname by appending the slave and count numbers */
	viewname[7] = (s % 10) + '0';
	viewname[9] = (c % 10) + '0';
	STcopy ("", sql_buff);   /* build SQL statement in buffer */
	STcat (sql_buff, "create view %s as select ");
	STcat (sql_buff, "b.key1, a.f8, a.money, a.date, a.char61, b.cnt ");
	STcat (sql_buff, "from data3 a, data5 b, data7 c ");
	STcat (sql_buff, "where a.money < c.money and b.key1 = c.key1");
	STprintf (sql_stmt, sql_buff, viewname);   /* create view viewname */
/* # line 2888 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2889 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("execute immediate - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 2899 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2901 "trl.sc" */	/* host code */
	/*
	** Execute a query that uses the view
	*/
	STcopy ("", sql_buff);
	STcopy ("", sql_stmt);
	STcat (sql_buff, "select key1, money, date ");
	STcat (sql_buff, "from %s where cnt = %d");
	STprintf (sql_stmt, sql_buff, viewname, c);
	/* select key1, money, date from viewname where cnt = c */
/* # line 2913 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s33",(char *)0,0,sql_stmt);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2914 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("prepare s33 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 2925 "trl.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s33",sqlda,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2926 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("describe s33 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 2938 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c33 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 2949 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c33",7582,23127);
    IIwritio(0,(short *)0,1,32,0,(char *)"s33");
    IIcsQuery((char *)"c33",7582,23127);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2950 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c33 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
	for (i=0; i < sqlda->sqld; i++)   /* allocate result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   len = sqv->sqllen;
	   /* account for describe setting sqllen to 0 for money and date */
	   if ((len == 0) && (abs(sqv->sqltype) == DATE_TYPE))
		len = DATE_LEN;
	   else if ((len == 0) && (abs(sqv->sqltype) == MONEY_TYPE))
		len = MONEY_LEN;
	   if (traceflag > 4)
           {
		SIprintf("len: %d\n", len);
		SIflush(stdout);
	   }
	   sqv->sqldata = (char *) calloc(1, len);
	   if(!sqv->sqldata)
	   {
		SIprintf("*** SQLDATA MEMORY ALLOCATION ERROR ***\n");
		SIflush(stdout);
		goto close_cursor; 
	   }
	}
	while (sqlca.sqlcode == 0)
	{
/* # line 2990 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c33",7582,23127,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 2992 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c33 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
				SIflush(stdout);
			}
			tempcode = sqlca.sqlcode;
			goto free_sqldata;
		}
	}
free_sqldata:
	if (traceflag > 2)
	{
		SIprintf("Going to free ((char *)sqv->sqldata); \n");
		SIflush(stdout);
	}
	for (i=0; i < sqlda->sqld; i++)   /* free result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   free ((char *)sqv->sqldata);
	}
close_cursor:
	if (traceflag > 2)
	{
		SIprintf("Going to close c33); \n");
		SIflush(stdout);
	}
/* # line 3024 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c33",7582,23127);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3025 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("close c33 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
drop_view:
	if (traceflag > 2)
	{
		SIprintf("Going to drop view %s \n", viewname);
		SIflush(stdout);
	}
	STcopy ("", sql_stmt);
	STprintf (sql_stmt, "drop view %s", viewname);
/* # line 3044 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3046 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("execute immediate - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
free_sqlda:
	if (traceflag > 2)
	{
		SIprintf("Going to free ((char *)sqlda) \n");
		SIflush(stdout);
	}
	free ((char *)sqlda);   /* free sqlda memory */
	/* This is an attempt to simulate the normal error handling	*/
	/* If an error is encountered, sqlcode is set to that error	*/
	/* sqlcode was getting reset to 0 by a successful drop_view	*/
	/* Note: the last error encountered will be returned		*/
        if (tempcode < 0)
        {
		if (traceflag)
		{
			SIprintf("error - sqlcode = tempcode: %d\n", tempcode);
			SIflush(stdout);
		}
                sqlca.sqlcode = tempcode;
                return;
        }
/* # line 3081 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3082 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3083 "trl.sc" */	/* host code */
};
tst34(s,c)
  i4 s;
  int c;
{
  char sql_stmt[300];
/* # line 3097 "trl.sc" */	/* host code */
	int		i;	/* index into sqlvar */
	int		len;	/* calloc length */
	IISQLVAR	*sqv;	/* pointer to sqlvar */
	IISQLDA		*sqlda = (IISQLDA *)0;
	char		viewname[10];
	char		sql_buff[300];
	i4		tempcode = 0;	/* retain sqlcode for error recording */
/* # line 3105 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3106 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3108 "trl.sc" */	/* host code */
	/*
	** Allocate memory for sqlda, enough for 3 columns
	*/
	sqlda = (IISQLDA *)
		calloc(1, IISQDA_HEAD_SIZE + (3 * IISQDA_VAR_SIZE));
        if (sqlda == (IISQLDA *)0)
        {
                SIprintf("*** MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                return;
        }
	sqlda->sqln = 3;   /* number of columns */
	STcopy ("view34_1_1", viewname);
	/* Make a unique viewname by appending the slave and count numbers */
	viewname[7] = (s % 10) + '0';
	viewname[9] = (c % 10) + '0';
	STcopy ("", sql_buff);   /* build SQL statement in buffer */
	STcat (sql_buff, "create view %s as ");
	STcat (sql_buff, "select i4, f4, money, date from data4 ");
	STcat (sql_buff, "where date in (select date from data9 ");
	STcat (sql_buff, "where key1 in (select key1 from data1)) ");
	STprintf (sql_stmt, sql_buff, viewname);   /* create view viewname */
/* # line 3137 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3138 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("execute immediate - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 3148 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3150 "trl.sc" */	/* host code */
	/*
	** Execute a query that uses the view
	*/
	STcopy ("", sql_buff);
	STcopy ("", sql_stmt);
	STcat (sql_buff, "select f4, money, date ");
	STcat (sql_buff, "from %s where i4 >= %d + 500");
	STprintf (sql_stmt, sql_buff, viewname, c);
	/* select f4, money, date from viewname where i4 >= c + 500 */
/* # line 3162 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s34",(char *)0,0,sql_stmt);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3163 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("prepare s34 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 3174 "trl.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s34",sqlda,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3175 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("describe s34 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 3187 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c34 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
		}
/* # line 3198 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c34",7583,2903);
    IIwritio(0,(short *)0,1,32,0,(char *)"s34");
    IIcsQuery((char *)"c34",7583,2903);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3199 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c34 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
	for (i=0; i < sqlda->sqld; i++)   /* allocate result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   len = sqv->sqllen;
	   /* account for describe setting sqllen to 0 for money and date */
	   if ((len == 0) && (abs(sqv->sqltype) == DATE_TYPE))
		len = DATE_LEN;
	   else if ((len == 0) && (abs(sqv->sqltype) == MONEY_TYPE))
		len = MONEY_LEN;
	   if (traceflag > 4)
           {
		SIprintf("len: %d\n", len);
		SIflush(stdout);
	   }
	   sqv->sqldata = (char *) calloc(1, len);
           if(!sqv->sqldata)
           {
                SIprintf("*** SQLDATA MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                goto close_cursor;
           }
	}
	while (sqlca.sqlcode == 0)
	{
/* # line 3240 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c34",7583,2903,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3242 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c34 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
				SIflush(stdout);
			}
			tempcode = sqlca.sqlcode;
			goto free_sqldata;
		}
	}
free_sqldata:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqv->sqldata); \n");
                SIflush(stdout);
        }
	for (i=0; i < sqlda->sqld; i++)   /* free result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   free ((char *)sqv->sqldata);
	}
close_cursor:
        if (traceflag > 2)
        {
                SIprintf("Going to close c32); \n");
                SIflush(stdout);
        }
/* # line 3274 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c34",7583,2903);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3276 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("close c34 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
drop_view:
        if (traceflag > 2)
        {
                SIprintf("Going to drop view %s \n", viewname);
                SIflush(stdout);
        }
	STcopy ("", sql_stmt);
	STprintf (sql_stmt, "drop view %s", viewname);
/* # line 3295 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3296 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("execute immediate - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
free_sqlda:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqlda) \n");
                SIflush(stdout);
        }
        free ((char *)sqlda);   /* free sqlda memory */
	/* This is an attempt to simulate the normal error handling	*/
	/* If an error is encountered, sqlcode is set to that error	*/
	/* sqlcode was getting reset to 0 by a successful drop_view	*/
	/* Note: the last error encountered will be returned		*/
        if (tempcode < 0)
        {
		if (traceflag)
		{
			SIprintf("error - sqlcode = tempcode: %d\n", tempcode);
			SIflush(stdout);
		}
                sqlca.sqlcode = tempcode;
                return;
        }
/* # line 3330 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3331 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3332 "trl.sc" */	/* host code */
};
tst35(s,c)
  i4 s;
  int c;
{
  char sql_stmt[300];
/* # line 3347 "trl.sc" */	/* host code */
	int		i;	/* index into sqlvar */
	IISQLVAR	*sqv;	/* pointer to sqlvar */
	IISQLDA		*sqlda = (IISQLDA *)0;
	char		viewname[10];
	char		sql_buff[300];
	i4		tempcode = 0;	/* retain sqlcode for error recording */
/* # line 3354 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3355 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3357 "trl.sc" */	/* host code */
	/* Allocate memory for sqlda, enough for 1 column only */
	sqlda = (IISQLDA *)
		calloc(1, IISQDA_HEAD_SIZE + (1 * IISQDA_VAR_SIZE));
        if (sqlda == (IISQLDA *)0)
        {
                SIprintf("*** MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                return;
        }
	sqlda->sqln = 1;   /* number of columns */
	STcopy ("view35_1_1", viewname);
	/* Make a unique viewname by appending the slave and count numbers */
	viewname[7] = (s % 10) + '0';
	viewname[9] = (c % 10) + '0';
	STcopy ("", sql_buff);   /* build SQL statement in buffer */
	STcat (sql_buff, "create view %s as ");
	STcat (sql_buff, "select key1, cnt from data1 union ");
	STcat (sql_buff, "select key1, cnt from data5");
	STprintf (sql_stmt, sql_buff, viewname);   /* create view viewname */
/* # line 3383 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3384 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("execute immediate - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 3394 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3396 "trl.sc" */	/* host code */
	/*
	** Execute a query that uses the view
	*/
	STcopy ("", sql_buff);
	STcopy ("", sql_stmt);
	STprintf (sql_stmt, "select key1 from %s where cnt > %d * 2", 
				viewname, c);
	/* select key1 from viewname where cnt > c x 2 */
/* # line 3407 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s35",(char *)0,0,sql_stmt);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3408 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("prepare s35 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 3419 "trl.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s35",sqlda,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3420 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("describe s35 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 3432 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c35 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
/* # line 3443 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c35",7584,5399);
    IIwritio(0,(short *)0,1,32,0,(char *)"s35");
    IIcsQuery((char *)"c35",7584,5399);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3444 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c35 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto drop_view;
	}
	for (i=0; i < sqlda->sqld; i++)   /* allocate result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   if (traceflag > 4)
           {
		SIprintf("sqv->sqllen: %d\n", sqv->sqllen);
		SIflush(stdout);
	   }
	   sqv->sqldata = (char *) calloc(1,sqv->sqllen);
	}
	while (sqlca.sqlcode == 0)
	{
/* # line 3470 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c35",7584,5399,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3472 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c35 using descriptor - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			tempcode = sqlca.sqlcode;
			goto free_sqldata;
		}
	}
free_sqldata:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqv->sqldata); \n");
                SIflush(stdout);
        }
	for (i=0; i < sqlda->sqld; i++)   /* free result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   free ((char *)sqv->sqldata);
	}
close_cursor:
        if (traceflag > 2)
        {
                SIprintf("Going to close c33); \n");
                SIflush(stdout);
        }
/* # line 3504 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c35",7584,5399);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3505 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("close c35 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
drop_view:
        if (traceflag > 2)
        {
                SIprintf("Going to drop view %s \n", viewname);
                SIflush(stdout);
        }
	STcopy ("", sql_stmt);
	STprintf (sql_stmt, "drop view %s", viewname);
/* # line 3524 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(sql_stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3525 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("execute immediate - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
free_sqlda:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqlda) \n");
                SIflush(stdout);
        }
	free ((char *)sqlda);   /* free sqlda memory */
	/* This is an attempt to simulate the normal error handling	*/
	/* If an error is encountered, sqlcode is set to that error	*/
	/* sqlcode was getting reset to 0 by a successful drop_view	*/
	/* Note: the last error encountered will be returned		*/
        if (tempcode < 0)
        {
		if (traceflag)
		{
			SIprintf("error - sqlcode = tempcode: %d\n", tempcode);
			SIflush(stdout);
		}
                sqlca.sqlcode = tempcode;
                return;
        }
/* # line 3560 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3561 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3562 "trl.sc" */	/* host code */
};
tst36(s,c)
int	s;
int	c;
{
  int i, d7key1;
	/* in case of undetectable distributed deadlock */
/* # line 3573 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3574 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3581 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c36 cursor - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3591 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update width500 set f4=f4*3.142, txt58='tst36_txt58_updated_value' wh\
ere i2>40 and i2<=90");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3595 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update width500 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3605 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data4 set money=money(999), date=date('15-dec-90'), char61a='t\
st36_char61_updated_value' where i4>5 and i4<=20");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3610 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update data4 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3620 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data3 set money=money\
(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +100), f8=f8 +(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*1000)where i4>200 and i4<=2\
50");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3624 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update data3 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3634 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data9 set date=date('4-mar-1991')where key1>350 and key1<=400");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3638 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("update data9 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3648 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c36",7585,30173);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1 from data7 where key1>=300 and key1<400 for update of mon\
ey");
    IIcsQuery((char *)"c36",7585,30173);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3649 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c36 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3661 "trl.sc" */	/* host code */
	for (i = 0;; i++)
	{
/* # line 3663 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c36",7585,30173,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d7key1);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode == 100) 
      goto closec36;
    else if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3665 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c36 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3675 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data7 set money=money\
(");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)"*1000)");
    IIcsERplace((char *)"c36",7585,30173);
    if (sqlca.sqlcode == 100) 
      goto closec36;
    else if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3679 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("update data7 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
	}
closec36:
/* # line 3692 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3693 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3694 "trl.sc" */	/* host code */
}
tst37(s,c)
  int s;
  int c;
{
  int i;
		/* in case of undetectable distributed deadlock */
/* # line 3707 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3708 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3710 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(key1, slave, rout)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3712 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("insert into data2 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3723 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from data5 where cnt=");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"or key1=");
    IIputdomio((short *)0,1,30,4,&i);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3724 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("delete from data5 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3735 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data8 set charv='tst37_charv_updated_value' where key1=");
    IIputdomio((short *)0,1,30,4,&i);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3738 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("update data8 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3750 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from width500 where i\
2=");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)" +500");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3751 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("delete from width500 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3761 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data6 set cnt=(cnt +");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)")*100 where key1=");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)" +40");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3764 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("update data6 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3774 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3775 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3776 "trl.sc" */	/* host code */
}
tst38(s,c)
  int s;
  int c;
{
  char d3money[32], d3date[24], d9date[24];
  int done1, done2, done3, d6cnt, d9key1;
  double d6floatv;
	/* delcare 3 cursors for updates of data3, data6, data9 */
	/* in case of undetectable distributed deadlock */
/* # line 3794 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3795 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3803 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c138 cursor - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3819 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c238 cursor - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3835 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c338 cursor - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3845 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3847 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c138",7586,14996);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select char(money), char(date)from data3 where i1=61 for direct updat\
e of money, date");
    IIcsQuery((char *)"c138",7586,14996);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3849 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c138 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3859 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c238",7587,18459);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select floatv, cnt from data6 where key1>5 and key1<=25 for direct up\
date of floatv, cnt");
    IIcsQuery((char *)"c238",7587,18459);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3861 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c238 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 3871 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c338",7588,19858);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, char(date)from data9 where key1>");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)"*100 and key1<=");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)
"*100 +50 for direct update of date");
    IIcsQuery((char *)"c338",7588,19858);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3873 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c338 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	/* loop through all tables until data exhausted */
	do {
/* # line 3886 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c138",7586,14996,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,31,d3money);
      IIcsGetio((short *)0,1,32,23,d3date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3887 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf("fetch c138 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3897 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done1,61);
  }
/* # line 3898 "trl.sc" */	/* host code */
		if (!done1 && !(sqlca.sqlcode < 0))
		{
/* # line 3900 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data3 set money=money(56789), date=date('15-mar-1991')");
    IIcsERplace((char *)"c138",7586,14996);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3905 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("update data3 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
/* # line 3916 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c238",7587,18459,-1,-1) != 0) {
      IIcsGetio((short *)0,1,31,8,&d6floatv);
      IIcsGetio((short *)0,1,30,4,&d6cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3917 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf("fetch c238 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3927 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done2,61);
  }
/* # line 3928 "trl.sc" */	/* host code */
		if (!done2 && !(sqlca.sqlcode < 0))
		{
/* # line 3930 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data6 set floatv=floa\
tv +");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*5.5, cnt=cnt +");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +100");
    IIcsERplace((char *)"c238",7587,18459);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3935 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("update data6 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
/* # line 3946 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c338",7588,19858,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d9key1);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3947 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
        		{
				SIprintf("fetch c338 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 3957 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done3,61);
  }
/* # line 3958 "trl.sc" */	/* host code */
		if (!done3 && !(sqlca.sqlcode < 0))
		{
/* # line 3960 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data9 set date=date('30-apr-1991')");
    IIcsERplace((char *)"c338",7588,19858);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3964 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
        			{
					SIprintf("update data9 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
	} while (!(done1 && done2 && done3));
/* # line 3977 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c138",7586,14996);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3978 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c238",7587,18459);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3979 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c338",7588,19858);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3981 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3982 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3983 "trl.sc" */	/* host code */
}
tst39(s,c)
  int s;
  int c;
{
		/* in case of undetectable distributed deadlock */
/* # line 3992 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3993 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3995 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update width500 set f8=12.41\
6*");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"where i2>100 and i2<=105");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 3999 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("update width500 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4009 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(key1, slave, rout)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +2000, ");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)" +2000, 39)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4012 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("insert into data2 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4023 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from data4 where money<=money(1000)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4025 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("delete from data4 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4035 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data7(key1, money)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +1000, money(1000*");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4038 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("insert into data7 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4048 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data8 set charv='tst39_charv_updated_value' where key1>100 and\
 key1<=150");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4052 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("update data8 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4062 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4063 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4064 "trl.sc" */	/* host code */
}
tst40(s,c)
  int s;
  int c;
{
		/* in case of undetectable distributed deadlock */
/* # line 4073 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4074 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4076 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into width500(i2, f4, f8, dollar, day, c6, txt58, txt98one, tx\
t98two, txt98three, txt98four)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" -100, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*8.125, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*16.25, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)
" +20000, '1/1/1992', 'tst040', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4087 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("insert into width500 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4097 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(key1, slave, rout)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*100, ");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)" +900, 40)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4100 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("insert into data2 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4110 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data5(key1, cnt)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*500, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4113 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("insert into data5 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4123 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data7(key1, money)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*1200, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*9.5)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4126 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("insert into data7 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4136 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data8(key1, charv)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)
"*700, 'tst40_charv_inserted_value')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4139 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("insert into data8 - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4149 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4150 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4151 "trl.sc" */	/* host code */
}
tst41(s,c)
  int s;
  int c;
{
/* # line 4159 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4161 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into width500(i2, f4, f8, dollar, day, c6, txt58, txt98one, tx\
t98two, txt98three, txt98four)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" -100, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*8.125, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*16.25, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)
" +20000, '1/1/1992', 'tst040', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4172 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(key1, slave, rout)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*100, ");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)" +900, 40)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4175 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data5(key1, cnt)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*500, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)" +");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4178 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data7(key1, money)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*1200, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*9.5)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4181 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data8(key1, charv)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)
"*700, 'tst40_charv_inserted_value')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4184 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4185 "trl.sc" */	/* host code */
}
tst42(s,c)
  int s;
  int c;
{
/* # line 4193 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4195 "trl.sc" */	/* host code */
}
tst43(s,c)
  int s;
  int c;
{
/* # line 4203 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4205 "trl.sc" */	/* host code */
}
tst44(s,c)
  int s;
  int c;
{
/* # line 4213 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4215 "trl.sc" */	/* host code */
}
tst45(s,c)
  int s;
  int c;
{
/* # line 4223 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4225 "trl.sc" */	/* host code */
}
tst46(s,c)
  int s;
  int c;
{
/* # line 4233 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4235 "trl.sc" */	/* host code */
}
tst47(s,c)
  int s;
  int c;
{
/* # line 4243 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4245 "trl.sc" */	/* host code */
}
tst48(s,c)
  int s;
  int c;
{
/* # line 4253 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4255 "trl.sc" */	/* host code */
}
tst49(s,c)
  int s;
  int c;
{
/* # line 4263 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4265 "trl.sc" */	/* host code */
}
tst50(s,c)
int	s;
int	c;
{
  int d5key1;
  int d5cnt;
  int done;
	/*  direct update using join */
/* # line 4288 "trl.sc" */	/* host code */
/*
**	SIprintf( "\nDirect update where 30<data5.key1<300 \n\0" );
**	SIflush(stdout);
*/
	done = 0;
/* # line 4295 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c6b",7589,30811);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, cnt from data5 where key1>50 and key1<320 for direct upd\
ate of key1, cnt");
    IIcsQuery((char *)"c6b",7589,30811);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4297 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("open c6b - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 4309 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c6b",7589,30811,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4311 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c6b into - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4320 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 4322 "trl.sc" */	/* host code */
		if (!done && !(sqlca.sqlcode < 0))
		{
/*			SIprintf("Direct   --> key1= %d, cnt=%d\n", d5key1, d5cnt);
**			SIflush(stdout);
*/
/* # line 4327 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data5 set key1=key1 -\
20");
    IIcsERplace((char *)"c6b",7589,30811);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4331 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
				{
					SIprintf("update data5 - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
		}
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 4348 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4350 "trl.sc" */	/* host code */
} /* End of tst50() */
tst51(s,c)
  int s;
  int c;
{
/* # line 4361 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"data1_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4366 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4373 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4375 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst52(s,c)
int	s;
int	c;
/*
**		reverses the action of tst26  
**
**		jds 11/14/91
*/
{
  int d5key1;
  int d5cnt;
  int d8key1;
  char d8char[66];
  int done;
/* # line 4420 "trl.sc" */	/* host code */
        if (traceflag > 2)
        {
	SIprintf( "\nJoin data5 and data8 and replace data8 \n\0" );
	SIflush(stdout);
	}
	done = 0;
/* # line 4428 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c9b",7590,25624);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.key1, a.cnt from data5 a, data8 d where a.key1=d.key1");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c9b",7590,25624);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4429 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("open c9b for readonly - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
/* # line 4439 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c10b",7591,1238);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1, charv from data8 where key1>20 and key1<200 for direct u\
pdate of key1, charv");
    IIcsQuery((char *)"c10b",7591,1238);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4440 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
        	{
			SIprintf("open c10b - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		return;
	}
	while (!done && !(sqlca.sqlcode < 0))
	{
/* # line 4453 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c9b",7590,25624,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5key1);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4455 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c9b - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			return;
		}
/* # line 4465 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 4466 "trl.sc" */	/* host code */
		if (!done && !(sqlca.sqlcode < 0))
		{
/*
**		SIprintf("c9: %d, %d \n", d5key1, d5cnt);
**		SIflush (stdout);
*/
/* # line 4472 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c10b",7591,1238,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d8key1);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4474 "trl.sc" */	/* host code */
			if (sqlca.sqlcode < 0)
			{
				if (traceflag)
				{
					SIprintf("fetch c10b - FAILED: %d\n", sqlca.sqlcode);
					SIflush(stdout);
				}
				return;
			}
/* # line 4484 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 4485 "trl.sc" */	/* host code */
			if (!done && !(sqlca.sqlcode < 0))
			{
/*				SIprintf("c10: %d \n", d8key1);
**				SIflush (stdout);
*/
/* # line 4490 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data8 set key1=key1/2, charv='Test 52 restore '");
    IIcsERplace((char *)"c10b",7591,1238);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4495 "trl.sc" */	/* host code */
				if (sqlca.sqlcode < 0)
				{
					if (traceflag)
					{
						SIprintf("fetch c10b - FAILED: %d\n", sqlca.sqlcode);
						SIflush(stdout);
					}
					return;
				}
			}
		}
	}
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 4513 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4515 "trl.sc" */	/* host code */
}	/* End of tst52() */
tst53(s,c)
  int s;
  int c;
{
/* # line 4526 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"data3_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4531 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4538 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4540 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst54(s,c)
  int s;
  int c;
{
/* # line 4555 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"data4_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4560 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4567 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4569 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst55(s,c)
  int s;
  int c;
{
/* # line 4584 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"data5_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4589 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4596 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4598 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst56(s,c)
  int s;
  int c;
{
/* # line 4612 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"data6_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4617 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4624 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4626 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst57(s,c)
  int s;
  int c;
{
/* # line 4640 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"data7_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4645 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4652 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4654 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst58(s,c)
  int s;
  int c;
{
/* # line 4668 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"data8_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4673 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4680 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4682 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst59(s,c)
  int s;
  int c;
{
/* # line 4696 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"data9_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4701 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4708 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4710 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst60(s,c)
  int s;
  int c;
{
/* # line 4724 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"d1d2_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4729 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4736 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4738 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst61(s,c)
  int s;
  int c;
{
/* # line 4752 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"d1d3_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4757 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4764 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4766 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst62(s,c)
  int s;
  int c;
{
/* # line 4790 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"d1d3d4_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4795 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4802 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4804 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst63(s,c)
  int s;
  int c;
{
/* # line 4828 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"d1d2d3d4d5_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"param",(char *)"(Min_key=10, Max_key=20)");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4834 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4841 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4843 "trl.sc" */	/* host code */
	PCsleep(1200000);
	/*
	**	wait about 20 minutes
	*/
}
tst64(s,c)
  int s;
  int c;
{
/* # line 4857 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"d5d4d1_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4862 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4869 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4871 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst65(s,c)
/*
**	jds	 4-NOV-1991 16:26:35.30
**
**	add new test routine
**
**	3 table join in each of 5 ldbs, then
**	UNION ALL to unite them.
**
**	uses SELECT begin/end loop
**
**	has a 1/2 second sleep between each retrieve of the loop,
**	to emulate a user scanning data.
**
**	THIS is "star-ish"...
**
**	requires that data_4, data_8, data_9 all be registered from
**	each of MTSLDB1-5, with an under_score, dbnumber suffix, as
**	" data8_2 " - table data8 from MTSLDB2.
*/
  int s;
  int c;
{
  char d4money[32];
  int d8key1;
  char d8char[66];
  int d9key1;
  char d9date[26];
  int sleeptime;
	sleeptime = (s * 1000);
	PCsleep(sleeptime);
/*
**	sleep 1 second per slave_number
*/
/* # line 4944 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select d8a.key1, d8a.charv, d9.date, d4.money from data8_1 d8a, data9\
_1 d9, data4_1 d4 where d4.i2=d8a.key1 and d9.key1=d8a.key1 union all \
select d8.key1, d8.charv, d9.date, d4.money from data8_2 d8, data9_2 d\
9, data4_2 d4 where d4.i2=d8.key1 and d9.key1=");
    IIwritio(0,(short *)0,1,32,0,(char *)
"d8.key1 union all select d8.key1, d8.charv, d9.date, d4.money from da\
ta8_3 d8, data9_3 d9, data4_3 d4 where d4.i2=d8.key1 and d9.key1=d8.ke\
y1 union all select d8.key1, d8.charv, d9.date, d4.money from data8_4 \
d8, data9_4 d9, data4_4 d4 where d4.i2=d8.key1");
    IIwritio(0,(short *)0,1,32,0,(char *)
" and d9.key1=d8.key1 union all select d8.key1, d8.charv, d9.date, d4.\
money from data8_5 d8, data9_5 d9, data4_5 d4 where d4.i2=d8.key1 and \
d9.key1=d8.key1 order by d8a.key1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&d8key1);
      IIgetdomio((short *)0,1,32,65,d8char);
      IIgetdomio((short *)0,1,32,25,d9date);
      IIgetdomio((short *)0,1,32,31,d4money);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 4969 "trl.sc" */	/* host code */
		PCsleep(300);	/* a pause to refresh */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
    if (sqlca.sqlcode < 0) 
      merr();
  }
/*
**	should return 1000 rows
*/
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("select, union all - FAILED: %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 4985 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 4987 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst66(s,c)
  int s;
  int c;
{
/* # line 5002 "trl.sc" */	/* call */
  {
    IIsqInit(&sqlca);
    IIutsys(0,(char *)"report",(char *)0);
    IIutsys(1,(char *)"database",(char *)"mtsddb1/star");
    IIutsys(1,(char *)"report",(char *)"d1d3d4_report");
    IIutsys(1,(char *)"file",(char *)"NLA0:");
    IIutsys(1,(char *)"silent",(char *)"");
    IIutsys(2,(char *)0,(char *)0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5007 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		SIprintf ("Error from report call %d",sqlca.sqlcode); 
		SIflush(stdout); 
		return;
	}
/* # line 5014 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5016 "trl.sc" */	/* host code */
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
	PCsleep(120000);
	/*
	**	wait about 2 minutes
	*/
}
tst67(s,c)
  int s;
  int c;
{
  int num;
/* # line 5043 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key1 from data1 where cnt=");
    IIputdomio((short *)0,1,30,4,&c);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 5048 "trl.sc" */	/* host code */
/*		SIprintf ("Slave %d retrieving %d\n", s, num); */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
    if (sqlca.sqlcode < 0) 
      merr();
  }
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 5056 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5057 "trl.sc" */	/* host code */
}
tst68(s,c)
  i4 s;
  int c;
{
  char sql_stmt[300];
/* # line 5071 "trl.sc" */	/* host code */
	int		i;	/* index into sqlvar */
	IISQLVAR	*sqv;	/* pointer to sqlvar */
	IISQLDA		*sqlda = (IISQLDA *)0;
	char		viewname[10];
	char		sql_buff[300];
	i4		tempcode = 0;	/* retain sqlcode for error recording */
/* # line 5078 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5079 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5081 "trl.sc" */	/* host code */
	/*
	** Allocate memory for sqlda, enough for 3 columns
	*/
	sqlda = (IISQLDA *)
		calloc(1, IISQDA_HEAD_SIZE + (3 * IISQDA_VAR_SIZE));
        if (sqlda == (IISQLDA *)0)
        {
                SIprintf("*** MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                return;
        }
	sqlda->sqln = 3;   /* number of columns */
	STcopy ("view68", viewname);
	/*
	** Execute a query that uses the view
	*/
	STcopy ("", sql_buff);
	STcopy ("", sql_stmt);
	STcat (sql_buff, "select key1, floatv, charv ");
	STcat (sql_buff, "from %s where cnt = %d");
	STprintf (sql_stmt, sql_buff, viewname, c);
	/* select key1, floatv, charv from viewname where cnt = c */
/* # line 5111 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s68",(char *)0,0,sql_stmt);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5112 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("prepare s68 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5123 "trl.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s68",sqlda,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5124 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("describe s68 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5136 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c68 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5147 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c68",7592,3798);
    IIwritio(0,(short *)0,1,32,0,(char *)"s68");
    IIcsQuery((char *)"c68",7592,3798);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5149 "trl.sc" */	/* host code */
	if (traceflag > 4)
	{
		SIprintf("open c68 - %s : %d\n", sql_stmt, sqlca.sqlcode);
		SIflush(stdout);
	}
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c68 - %s FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
	for (i=0; i < sqlda->sqld; i++)   /* allocate result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   if (traceflag > 4)
           {
		SIprintf("sqv->sqllen: %d\n", sqv->sqllen);
		SIflush(stdout);
	   }
	   sqv->sqldata = (char *) calloc(1,sqv->sqllen);
           if(!sqv->sqldata)
           {
                SIprintf("*** SQLDATA MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                goto close_cursor;
           }
	}
	while (sqlca.sqlcode == 0)
	{
/* # line 5187 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c68",7592,3798,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5189 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c68 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
				SIflush(stdout);
			}
			tempcode = sqlca.sqlcode;
			goto free_sqldata;
		}
	}
free_sqldata:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqv->sqldata); \n");
                SIflush(stdout);
        }
	for (i=0; i < sqlda->sqld; i++)   /* free result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   free ((char *)sqv->sqldata);
	}
close_cursor:
        if (traceflag > 2)
        {
                SIprintf("Going to close c68); \n");
                SIflush(stdout);
        }
/* # line 5221 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c68",7592,3798);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5222 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("close c68 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
free_sqlda:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqlda) \n");
                SIflush(stdout);
        }
	free ((char *)sqlda);   /* free sqlda memory */
	/* This is an attempt to simulate the normal error handling	*/
	/* If an error is encountered, tempcode is set to that error	*/
	/* At the end of the test, sqlcode is reset to tempcode		*/
	/* in case sqlcode may have been written over			*/
        if (tempcode < 0)
        {
		if (traceflag)
		{
			SIprintf("error - sqlcode = tempcode: %d\n", tempcode);
			SIflush(stdout);
		}
                sqlca.sqlcode = tempcode;
                return;
        }
/* # line 5258 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5259 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5260 "trl.sc" */	/* host code */
};
tst69(s,c)
  i4 s;
  int c;
{
  char sql_stmt[300];
/* # line 5275 "trl.sc" */	/* host code */
	int		i;	/* index into sqlvar */
	int		len;	/* calloc length */
	IISQLVAR	*sqv;	/* pointer to sqlvar */
	IISQLDA		*sqlda = (IISQLDA *)0;
	char		viewname[10];
	char		sql_buff[300];
	i4		tempcode = 0;	/* retain sqlcode for error recording */
/* # line 5283 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5284 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5285 "trl.sc" */	/* host code */
	/*
	** Allocate memory for sqlda, enough for 3 columns
	*/
	sqlda = (IISQLDA *)
		calloc(1, IISQDA_HEAD_SIZE + (3 * IISQDA_VAR_SIZE));
        if (sqlda == (IISQLDA *)0)
        {
                SIprintf("*** SQLDA MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                return;
        }
	sqlda->sqln = 3;   /* number of columns */
	STcopy ("view69", viewname);
	/*
	** Execute a query that uses the view
	*/
	STcopy ("", sql_buff);
	STcopy ("", sql_stmt);
	STcat (sql_buff, "select key1, money, date ");
	STcat (sql_buff, "from %s where cnt = %d");
	STprintf (sql_stmt, sql_buff, viewname, c);
	/* select key1, money, date from viewname where cnt = c */
/* # line 5315 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s69",(char *)0,0,sql_stmt);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5316 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("prepare s69 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5327 "trl.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s69",sqlda,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5328 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("describe s69 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5340 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c69 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5351 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c69",7593,5014);
    IIwritio(0,(short *)0,1,32,0,(char *)"s69");
    IIcsQuery((char *)"c69",7593,5014);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5353 "trl.sc" */	/* host code */
	if (traceflag > 4)
	{
		SIprintf("open c69 - %s : %d\n", sql_stmt, sqlca.sqlcode);
		SIflush(stdout);
	}
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c69 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
	for (i=0; i < sqlda->sqld; i++)   /* allocate result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   len = sqv->sqllen;
	   /* account for describe setting sqllen to 0 for money and date */
	   if ((len == 0) && (abs(sqv->sqltype) == DATE_TYPE))
		len = DATE_LEN;
	   else if ((len == 0) && (abs(sqv->sqltype) == MONEY_TYPE))
		len = MONEY_LEN;
	   if (traceflag > 4)
           {
		SIprintf("len: %d\n", len);
		SIflush(stdout);
	   }
	   sqv->sqldata = (char *) calloc(1, len);
	   if(!sqv->sqldata)
	   {
		SIprintf("*** SQLDATA MEMORY ALLOCATION ERROR ***\n");
		SIflush(stdout);
		goto close_cursor; 
	   }
	}
	while (sqlca.sqlcode == 0)
	{
/* # line 5399 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c69",7593,5014,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5401 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c69 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
				SIflush(stdout);
			}
			tempcode = sqlca.sqlcode;
			goto free_sqldata;
		}
	}
free_sqldata:
	if (traceflag > 2)
	{
		SIprintf("Going to free ((char *)sqv->sqldata); \n");
		SIflush(stdout);
	}
	for (i=0; i < sqlda->sqld; i++)   /* free result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   free ((char *)sqv->sqldata);
	}
close_cursor:
	if (traceflag > 2)
	{
		SIprintf("Going to close c69); \n");
		SIflush(stdout);
	}
/* # line 5433 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c69",7593,5014);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5434 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("close c69 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
free_sqlda:
	if (traceflag > 2)
	{
		SIprintf("Going to free ((char *)sqlda) \n");
		SIflush(stdout);
	}
	free ((char *)sqlda);   /* free sqlda memory */
	/* This is an attempt to simulate the normal error handling	*/
	/* If an error is encountered, tempcode is set to that error	*/
	/* At the end of the test, sqlcode is reset to tempcode		*/
	/* in case sqlcode may have been written over			*/
        if (tempcode < 0)
        {
		if (traceflag)
		{
			SIprintf("error - sqlcode = tempcode: %d\n", tempcode);
			SIflush(stdout);
		}
                sqlca.sqlcode = tempcode;
                return;
        }
/* # line 5469 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5470 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5471 "trl.sc" */	/* host code */
};
tst70(s,c)
  i4 s;
  int c;
{
  char sql_stmt[300];
/* # line 5485 "trl.sc" */	/* host code */
	int		i;	/* index into sqlvar */
	int		len;	/* calloc length */
	IISQLVAR	*sqv;	/* pointer to sqlvar */
	IISQLDA		*sqlda = (IISQLDA *)0;
	char		viewname[10];
	char		sql_buff[300];
	i4		tempcode = 0;	/* retain sqlcode for error recording */
/* # line 5493 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5494 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5496 "trl.sc" */	/* host code */
	/*
	** Allocate memory for sqlda, enough for 3 columns
	*/
	sqlda = (IISQLDA *)
		calloc(1, IISQDA_HEAD_SIZE + (3 * IISQDA_VAR_SIZE));
        if (sqlda == (IISQLDA *)0)
        {
                SIprintf("*** MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                return;
        }
	sqlda->sqln = 3;   /* number of columns */
	STcopy ("view70", viewname);
	/*
	** Execute a query that uses the view
	*/
	STcopy ("", sql_buff);
	STcopy ("", sql_stmt);
	STcat (sql_buff, "select f4, money, date ");
	STcat (sql_buff, "from %s where i4 >= %d + 500");
	STprintf (sql_stmt, sql_buff, viewname, c);
	/* select f4, money, date from viewname where i4 >= c + 500 */
/* # line 5526 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s70",(char *)0,0,sql_stmt);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5527 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("prepare s70 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5538 "trl.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s70",sqlda,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5539 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("describe s70 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5551 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c70 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
		}
/* # line 5562 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c70",7594,18067);
    IIwritio(0,(short *)0,1,32,0,(char *)"s70");
    IIcsQuery((char *)"c70",7594,18067);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5564 "trl.sc" */	/* host code */
	if (traceflag > 4)
	{
		SIprintf("open c70 - %s : %d\n", sql_stmt, sqlca.sqlcode);
		SIflush(stdout);
	}
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c70 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
	for (i=0; i < sqlda->sqld; i++)   /* allocate result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   len = sqv->sqllen;
	   /* account for describe setting sqllen to 0 for money and date */
	   if ((len == 0) && (abs(sqv->sqltype) == DATE_TYPE))
		len = DATE_LEN;
	   else if ((len == 0) && (abs(sqv->sqltype) == MONEY_TYPE))
		len = MONEY_LEN;
	   if (traceflag > 4)
           {
		SIprintf("len: %d\n", len);
		SIflush(stdout);
	   }
	   sqv->sqldata = (char *) calloc(1, len);
           if(!sqv->sqldata)
           {
                SIprintf("*** SQLDATA MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                goto close_cursor;
           }
	}
	while (sqlca.sqlcode == 0)
	{
/* # line 5611 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c70",7594,18067,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5613 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c70 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
				SIflush(stdout);
			}
			tempcode = sqlca.sqlcode;
			goto free_sqldata;
		}
	}
free_sqldata:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqv->sqldata); \n");
                SIflush(stdout);
        }
	for (i=0; i < sqlda->sqld; i++)   /* free result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   free ((char *)sqv->sqldata);
	}
close_cursor:
        if (traceflag > 2)
        {
                SIprintf("Going to close c32); \n");
                SIflush(stdout);
        }
/* # line 5645 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c70",7594,18067);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5647 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("close c70 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
free_sqlda:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqlda) \n");
                SIflush(stdout);
        }
        free ((char *)sqlda);   /* free sqlda memory */
	/* This is an attempt to simulate the normal error handling	*/
	/* If an error is encountered, tempcode is set to that error	*/
	/* At the end of the test, sqlcode is reset to tempcode		*/
	/* in case sqlcode may have been written over			*/
        if (tempcode < 0)
        {
		if (traceflag)
		{
			SIprintf("error - sqlcode = tempcode: %d\n", tempcode);
			SIflush(stdout);
		}
                sqlca.sqlcode = tempcode;
                return;
        }
/* # line 5681 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5682 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5683 "trl.sc" */	/* host code */
};
tst71(s,c)
  i4 s;
  int c;
{
  char sql_stmt[300];
/* # line 5698 "trl.sc" */	/* host code */
	int		i;	/* index into sqlvar */
	IISQLVAR	*sqv;	/* pointer to sqlvar */
	IISQLDA		*sqlda = (IISQLDA *)0;
	char		viewname[10];
	char		sql_buff[300];
	i4		tempcode = 0;	/* retain sqlcode for error recording */
/* # line 5705 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5706 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=900");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5708 "trl.sc" */	/* host code */
	/* Allocate memory for sqlda, enough for 1 column only */
	sqlda = (IISQLDA *)
		calloc(1, IISQDA_HEAD_SIZE + (1 * IISQDA_VAR_SIZE));
        if (sqlda == (IISQLDA *)0)
        {
                SIprintf("*** MEMORY ALLOCATION ERROR ***\n");
                SIflush(stdout);
                return;
        }
	sqlda->sqln = 1;   /* number of columns */
	STcopy ("view71", viewname);
	/*
	** Execute a query that uses the view
	*/
	STcopy ("", sql_buff);
	STcopy ("", sql_stmt);
	STprintf (sql_stmt, "select key1 from %s where cnt > %d * 2", 
				viewname, c);
	/* select key1 from viewname where cnt > c x 2 */
/* # line 5735 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s71",(char *)0,0,sql_stmt);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5736 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("prepare s71 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5747 "trl.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s71",sqlda,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5748 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("describe s71 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5760 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("declare c71 - FAILED: %d\n", sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
/* # line 5771 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c71",7595,11346);
    IIwritio(0,(short *)0,1,32,0,(char *)"s71");
    IIcsQuery((char *)"c71",7595,11346);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5773 "trl.sc" */	/* host code */
	if (traceflag > 4)
	{
		SIprintf("open c71 - %s : %d\n", sql_stmt, sqlca.sqlcode);
		SIflush(stdout);
	}
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("open c71 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
		goto free_sqlda;
	}
	for (i=0; i < sqlda->sqld; i++)   /* allocate result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   if (traceflag > 4)
           {
		SIprintf("sqv->sqllen: %d\n", sqv->sqllen);
		SIflush(stdout);
	   }
	   sqv->sqldata = (char *) calloc(1,sqv->sqllen);
	}
	while (sqlca.sqlcode == 0)
	{
/* # line 5805 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c71",7595,11346,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5807 "trl.sc" */	/* host code */
		if (sqlca.sqlcode < 0)
		{
			if (traceflag)
			{
				SIprintf("fetch c71 using descriptor - FAILED: %d\n", sqlca.sqlcode);
				SIflush(stdout);
			}
			tempcode = sqlca.sqlcode;
			goto free_sqldata;
		}
	}
free_sqldata:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqv->sqldata); \n");
                SIflush(stdout);
        }
	for (i=0; i < sqlda->sqld; i++)   /* free result areas */
	{
	   sqv = &sqlda->sqlvar[i];
	   free ((char *)sqv->sqldata);
	}
close_cursor:
        if (traceflag > 2)
        {
                SIprintf("Going to close c33); \n");
                SIflush(stdout);
        }
/* # line 5839 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c71",7595,11346);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5840 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		if (traceflag)
		{
			SIprintf("close c71 - %s - FAILED: %d\n", sql_stmt, sqlca.sqlcode);
			SIflush(stdout);
		}
		tempcode = sqlca.sqlcode;
	}
free_sqlda:
        if (traceflag > 2)
        {
                SIprintf("Going to free ((char *)sqlda) \n");
                SIflush(stdout);
        }
	free ((char *)sqlda);   /* free sqlda memory */
	/* This is an attempt to simulate the normal error handling	*/
	/* If an error is encountered, tempcode is set to that error	*/
	/* At the end of the test, sqlcode is reset to tempcode		*/
	/* in case sqlcode may have been written over			*/
        if (tempcode < 0)
        {
		if (traceflag)
		{
			SIprintf("error - sqlcode = tempcode: %d\n", tempcode);
			SIflush(stdout);
		}
                sqlca.sqlcode = tempcode;
                return;
        }
/* # line 5875 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5876 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where timeout=session");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5877 "trl.sc" */	/* host code */
};
tst72(s,c)
  int s;
  int c;
{
/* # line 5885 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5887 "trl.sc" */	/* host code */
}
tst73(s,c)
  int s;
  int c;
{
/* # line 5895 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5897 "trl.sc" */	/* host code */
}
tst74(s,c)
  int s;
  int c;
{
/* # line 5905 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5907 "trl.sc" */	/* host code */
}
tst75(s,c)
  int s;
  int c;
{
/* # line 5915 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5917 "trl.sc" */	/* host code */
}
tst76(s,c)
  int s;
  int c;
{
/* # line 5925 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5927 "trl.sc" */	/* host code */
}
tst77(s,c)
  int s;
  int c;
{
/* # line 5935 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5937 "trl.sc" */	/* host code */
}
tst78(s,c)
  int s;
  int c;
{
/* # line 5945 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5947 "trl.sc" */	/* host code */
}
tst79(s,c)
  int s;
  int c;
{
/* # line 5955 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5957 "trl.sc" */	/* host code */
}
tst80(s,c)
  int s;
  int c;
{
/* # line 5965 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5967 "trl.sc" */	/* host code */
}
tst81(s,c)
  int s;
  int c;
{
/* # line 5975 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5977 "trl.sc" */	/* host code */
}
tst82(s,c)
  int s;
  int c;
{
/* # line 5985 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5987 "trl.sc" */	/* host code */
}
tst83(s,c)
  int s;
  int c;
{
/* # line 5995 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 5997 "trl.sc" */	/* host code */
}
tst84(s,c)
  int s;
  int c;
{
/* # line 6005 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6007 "trl.sc" */	/* host code */
}
tst85(s,c)
  int s;
  int c;
{
/* # line 6015 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6017 "trl.sc" */	/* host code */
}
tst86(s,c)
  int s;
  int c;
{
/* # line 6025 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6027 "trl.sc" */	/* host code */
}
tst87(s,c)
  int s;
  int c;
{
/* # line 6035 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6037 "trl.sc" */	/* host code */
}
tst88(s,c)
  int s;
  int c;
{
/* # line 6045 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6047 "trl.sc" */	/* host code */
}
tst89(s,c)
  int s;
  int c;
{
/* # line 6055 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6057 "trl.sc" */	/* host code */
}
tst90(s,c)
  int s;
  int c;
{
/* # line 6065 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6067 "trl.sc" */	/* host code */
}
tst91(s,c)
  int s;
  int c;
{
/* # line 6075 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6077 "trl.sc" */	/* host code */
}
tst92(s,c)
  int s;
  int c;
{
/* # line 6085 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6087 "trl.sc" */	/* host code */
}
tst93(s,c)
  int s;
  int c;
{
/* # line 6095 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6097 "trl.sc" */	/* host code */
}
tst94(s,c)
  int s;
  int c;
{
/* # line 6105 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6107 "trl.sc" */	/* host code */
}
tst95(s,c)
  int s;
  int c;
{
/* # line 6115 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6117 "trl.sc" */	/* host code */
}
tst96(s,c)
  int s;
  int c;
{
/* # line 6125 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6127 "trl.sc" */	/* host code */
}
tst97(s,c)
  int s;
  int c;
{
/* # line 6135 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6137 "trl.sc" */	/* host code */
}
tst98(s,c)
  int s;
  int c;
{
/* # line 6145 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6147 "trl.sc" */	/* host code */
}
tst99(s,c)
  int s;
  int c;
{
  i4 running_kids;
/* # line 6161 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6164 "trl.sc" */	/* host code */
/*
**
**	Track the performance of these tests every twenty
**	minutes (or as close as the operating system will
**	track that time).
**
**	The tests which use tst29 append a row to the history
**	table with each successful transaction.  This routine
**	uses tables	history_count,
**			test_temp,
**			test_perf
**
**	history_count is a kluge to get the count into a
**	variable so I can use it.
**
**	test_temp retains information about the start of the
**	test and the previous number of transactions and
**	the total test minutes.
**
**	test_perf collects the performance data in units of
**	transactions per minute, both throughout the test so
**	far, and in the latest sampling period.
**
**	All 3 tables are created and initialized in MTSLDB3
**	with the initialization step of the test.
**
**
**	The test checks to see whether slaves_up still has
**	any rows in it (copied from mtscoord.sc) before it
**	tries the update/insert cycle.  If only 1, that's
**	THIS slave, and it will exit.
**
**	The routine uses DIRECT CONNECT, so it is suitable for
**	STAR only;  it also uses mtsnode3, so that must be defined
**	and set up correctly.
**
**
**	Why do it via direct connect?
**
**	(1) the STAR database is already open, and a session is established
**	to MTSLDB for the administrative tables; I need that to find out
**	when this test should exit;
**
**	(2) DIRECT CONNECT minimizes the impact on the STAR server at
**	the cost of an extra STAR session.
**
**		jds	2/3/92
**
*/
/* # line 6215 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode on slaves_up where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6217 "trl.sc" */	/* host code */
/*
**	let me read the slaves_up table without messing
**	up the coordinator or the other slaves.
*/
do
 {
/* # line 6226 "trl.sc" */	/* direct connect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"direct connect with database=mtsldb3, node=mtsnode3");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6228 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 6233 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where readlock=nolock");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6235 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set autocommit on");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6238 "trl.sc" */	/* host code */
/*
**
**	a kluge to get this count into the temp area
**
*/
/* # line 6244 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from history_count");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6246 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 6251 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into history_count(hcount)select count(acc_number)from history");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6254 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/*
**
**	Load the one-tuple table with
**
**		curmin - minutes into the test
**		curtrans - total successful transactions
**
*/
/* # line 6269 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update test_temp set curmin=interval('minutes', (date('now') -startti\
me))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6272 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 6277 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update test_temp from history_count set curtrans=hcount");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6281 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/*
**
**	Calculate and store the changes from the last sample in
**	the one-tuple table.
**
**		elapmin - minutes since last sample
**		elaptran - number of transactions since last sample
*/
/* # line 6295 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update test_temp set elapmin=curmin -premin, elaptrans=curtrans -pret\
rans");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6300 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/*
**
**	Once those numers are there, calculate and store
**	the transaction rate
**
**		currate - number of new transactions divided by
**			number of minutes this sample.
*/
/* # line 6314 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update test_temp set currate=elaptrans/elapmin");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6317 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/*
**
**	Save the results in the test history table
**
*/
/* # line 6328 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into test_perf(sample_time, test_minutes, total_trans, test_ra\
te, sample_min, sample_trans, sample_rate)select 'now', curmin, curtra\
ns, curtrans/curmin, elapmin, elaptrans, currate from test_temp");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6335 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/*
**
**	Move the current minutes and transaction count to the 
**	"pre" fields to use next iteration
**
*/
/* # line 6347 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update test_temp set premin=curmin, pretrans=curtrans");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6351 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 6357 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6359 "trl.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
		return;
	}
/* # line 6364 "trl.sc" */	/* direct disconnect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6367 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from slaves_u\
p");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&running_kids);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      merr();
  }
/* # line 6371 "trl.sc" */	/* host code */
 if (running_kids > 1)
      PCsleep( 1200000 );
/*
**	sleep 20 minutes
*/
 } while (running_kids > 1);
}	/* end of routine 99 */
