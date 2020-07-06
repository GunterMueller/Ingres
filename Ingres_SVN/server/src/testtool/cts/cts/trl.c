# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** TRL.SC
**	This is the test routine library (TRL) for CTS/MTS. Each routine
**	represents a transaction that a CTS slave execute. You can find
**	out which routines are executed by a CTS test by looking at the
**	table test_index in the CTS database. This table contains an
**	index of how many slaves a test has and which are the routines
**	that each slave executes. The table test_reps has the information
**	about how many times each routine should be executed. This
**      table has one entry for each test and each slave in that test.
**
**      There are three important considerations when you plan to add
**	routines to this library.
**	1. If you are adding a routine to be executed by CTS you should
**	   put one transaction (SQT or MQT) per routine. This is necessary
**	   because CTS needs to keep track of the commit stamps of each
**	   transaction. If the routine is going to be excuted by MTS you 
**	   don't have this restriction. 
**      2. There are two parameters that are passed to each routine: the
**	   number of the slave that is executing the routine and the
**	   repetition number. If you need more information for the routine
**	   you can put that information in a table which is initialized by
**	   the coordinator or the slave.
**      3. To be able to execute a routine as part of a CTS test you need
**	   to add information to the table test_index and test_reps about
**	   the routine and the test. See the CTS documentation for more
**	   information on how to do that.
**
**	History:
**
**      14-may-1991 (lauraw)
**              Changed sqlprint() to CTS_errors().
**
**      05-jun-1991 (jeromef)
**              Updated trl with new routines 26, 27, 28, 29, and 30.  They are
**		new tests for testing the new Btree index compression routines.
**		There are specific comments in each routine.  Also cleaned up
**		some code and added form feed between each routine.
**      24-oct-1991 (jeromef)
**		Removed the rollback to savepoint statement since it was 
**		unnecessary and cause an extra error message to be reported.
**      18-may-1992 (jeromef)
**		Added test routines 31, 32 and 33 to test the SQL ROLLBACK
**		feature.
**      26-aug-1993 (dianeh)
**		Added #include <st.h>.
**      10-Dec-1993 (Jeromef)
**		Added an "if" clause to deal with a divide by zero problem.  
**		In tst26 through tst29, there is "mod" command whose result
**		is used for a division.  The "if" will change the result from
**		zero to the "mod" divisor value.
**	22-Jan-1994 (fredv)
**		Move the "if" after STcopy() in tst26 to tst29 because
**		there is no array[7]. Try to do STcopy(array[7], array_buffer)
**		will cause SEGV.
**	02-mar-1995 (canor01)
**		Change the "f4 = f4 * 3" to "f4 = f4 * 1.02" in tst6 to
**		prevent floating point overflow, which is now checked for
**      13-aug-1997 (stial01)
**              tst21: After UPDATE, INQUIRE_INGRES again.
**              We may deadlock trying to get the lock on the new page.
**              tst20,tst21,tst24,tst28: Increase MAXLOCKS so slaves don't get 
**              into escalate-deadlock loop.
**      10/26/99 (hweho01)
**              Added IILQint() function prototype. Without the 
**              explicit declaration, default int return value for a function 
**              will truncate a 64-bit address on ris_u64 platform. 
**      02/04/2000 (stial01)
**              Added a message to tst4 case 4, to explain row count
**              differences when running with lock level row, isolation level
**              cursor stability or repeatable read.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      31-jul-2001 (stial01)
**          Fixed deadlock handling in tst4, case 3 (b105392)
**	31-mar-2003 (somsa01)
**	    Changed return type of IILQint() to "void *".
**      03-mar-2003 (legru01)
**          Added two routines (tst34 & tst35) 
**          TST34: will be used to check 
**          a single sequence object operation when multiple slaves 
**          are executing the routine against that object. 
**          TST35: will be used to check
**          the operation of multiple sequence objects when multiple slaves
**          are executing the routine.
**	31-mar-2003 (somsa01)
**	    Changed return type of IILQint() to "void *".
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 96 "trl.sc" */	/* host code */
# include       <compat.h>
# include       <lo.h>
# include       <me.h>
# include       <mh.h>
# include       <si.h>
# include       <st.h>
# include       <tm.h>
# define	ERR_DEADLOCK	4700
# define	ERR_LOGFULL 	4706
# define	ERR_DROP	2753
# define	ERR_CREATE 	5102
# define	ERR_DUPLICATE	2010
# define	BRANCHS_PER_BANK	10
# define	TELLERS_PER_BRANCH	10
# define	ACCOUNTS_PER_BRANCH	1000
# define	LARGEST_DEPOSIT		1000
# define	MAX_SCAN		100
# define	TBLSIZE			5120		/* used for tst22 */
FUNC_EXTERN    VOID * IILQint();
extern		i4		traceflag;
  extern int ing_err;
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
int     tst35();
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
        tst35 
};
int 
trlsize()
{
      return ( (sizeof(trl)/sizeof(struct trl_routine)) - 1);
}
/*
** Function TST0
** Purpose:   This routine is intended to be used as a test to verify that
**	      CTS/MTS are working.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst0(s,c)
  int s;
  int c;
{
/* # line 224 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data0(num, slave, routs, reps)values(100, ");
    IIputdomio((short *)0,1,30,4,&s);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 226 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 227 "trl.sc" */	/* host code */
	if (ing_err != 0)
	{
		return;
	}
}
/*
** Function TST1
** Purpose: Insert one row according to the slave number, the repetition
**	    number and the total number of slaves.  This routine should 
**	    be executed by several slaves such that they insert different 
**	    rows in ascending order.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst1(s,c)
  int s;
  int c;
{
  int value;
  int num = 0;
/* # line 258 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select num_kids from num_sla\
ves");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 261 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 263 "trl.sc" */	/* host code */
  value = s + (num * c);
  if (traceflag)
  {
	SIprintf ("Running SLAVE %d: Rout 1, pass %d, inserting %d \n",s,c, value);
	SIflush (stdout);
  }
/* # line 270 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl1",7550,8697);
      IIputdomio((short *)0,1,30,4,&value);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl1",7550,8697);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data1(tbl_key, rout, slave, cnt)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)", 1, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl1",7550,8697);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 273 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 274 "trl.sc" */	/* host code */
  if (ing_err  != 0)
  {
	return;
  }
}
/*
** Function TST2
** Purpose: This routine inserts values in descending order.  It should be used
**          with a btree to test page splits and sorting.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst2(s,c)
  int s;
  int c;
{
  int value;
  int num;
/* # line 303 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select num_kids from num_sla\
ves");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 306 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 308 "trl.sc" */	/* host code */
  value = s + (500 - (num * c));  
  if (traceflag)
  {
	SIprintf ("Running SLAVE %d: Rout 2, pass %d, inserting %d \n",s,c, value);
	SIflush (stdout);
  }
/* # line 314 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl2",7551,8922);
      IIputdomio((short *)0,1,30,4,&value);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl2",7551,8922);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data1(tbl_key, rout, slave, cnt)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)", 2, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl2",7551,8922);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 317 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 318 "trl.sc" */	/* host code */
  if (ing_err  != 0)
  {
	return;
  }
}
/*
** Function TST3
** Purpose: insert one row using the passed number as the key.  In this test
**          the slaves would be inserting rows with the same key such that
**          the table will have several duplicate keys.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst3(s,c)
  int s;
  int c;
{
  if (traceflag)
  {
	SIprintf ("Running SLAVE %d: Rout 3, pass %d, inserting %d \n",s,c,c);
	SIflush (stdout);
  }
/* # line 347 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl3",7552,8692);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl3",7552,8692);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data1(tbl_key, rout, slave, cnt)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", 3, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl3",7552,8692);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 349 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 351 "trl.sc" */	/* host code */
  if (ing_err  != 0)
  {
	return;
  }
}
/*
** TST4
** Purpose: This routine uses a random number generator to decide which
**	    transaction to execute.  The seed for the random number is
**	    placed in the table seeds by the coordinator, and corresponds
**	    to the time of day.  Note that only one transaction will be
**	    executed in each pass.  Most transactions use repeat queries
**	    and perform operations that require exclusive locks.
**	    Note also the use of PCsleep (mainly for the VAX/VMS systems)
**	    to prevent deadlock thrashing.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst4(s,c)
  int s;
  int c;
{
  int num, r, m, i, seed;
  int qry_end;
  int i2_var;
  int i4_var;
  int trans;
  char prepare_buffer[1000];
/* # line 389 "trl.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl4",7553,8396);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"trl4",7553,8396);
        IIwritio(0,(short *)0,1,32,0,(char *)"select seed_val from see\
ds");
        IIexDefine(0,(char *)"trl4",7553,8396);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&seed);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 393 "trl.sc" */	/* host code */
  MHsrand((i4)((seed + s) * c)); 
  r   = (i4) ((MHrand() * ( MHrand() * (f8) (32767))) +1);
  m   = r%6;
  if (traceflag)
  {	
	SIprintf ("Running SLAVE %d: Rout 4, pass %d, using case %d \n",
        s, c, m );
	SIflush (stdout);
  }
  switch (m)
  {
  case 0:
	if (traceflag)
	{
		SIprintf ("\t\t inserting tbl_key = %d \n", r);
		SIflush (stdout);
	}
/* # line 414 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl5",7554,9938);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl5",7554,9938);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
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
"*10, '31-dec-1999', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'yyyy\
yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl5",7554,9938);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 420 "trl.sc" */	/* host code */
	break;			
  case 1:
	if (traceflag)
	{
		SIprintf("\t\t inserting & deleting tbl_key = %d \n",r);
		SIflush(stdout);
	}
/* # line 429 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl6",7555,7659);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl6",7555,7659);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
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
"*10, '31-dec-1999', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'yyyy\
yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl6",7555,7659);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 435 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 436 "trl.sc" */	/* host code */
	if (ing_err != 0)
	{
		break;
	}
	if (traceflag)
	{
		SIprintf ("\t\t case 1 delete WHERE data2.i2 = %d \n", r);
		SIflush (stdout);
	}
/* # line 448 "trl.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl7",7556,25811);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl7",7556,25811);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from data2 where data2.i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl7",7556,25811);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 450 "trl.sc" */	/* host code */
	break;
  case 2:
	if (traceflag)
	{
		SIprintf("\t\t updating tbl_key = %d \n",r);
		SIflush(stdout);
	}
/* # line 459 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update data2 set i2=");
    IIputdomio((short *)0,1,30,4,&r);
    IIwritio(0,(short *)0,1,32,0,(char *)" +1, i4=i4 +1000 where i2=");
    IIputdomio((short *)0,1,30,4,&r);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i4>");
    IIputdomio((short *)0,1,30,4,&r);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 463 "trl.sc" */	/* host code */
	break;
  case 3:
	if (traceflag)
	{
		SIprintf("\t\t select all the rows with readlock=nolock, tbl_key=%d \n",
		    r);
		SIflush(stdout);
	}
/* # line 473 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 474 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE on data2 where READLOCK=NOLOCK");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 479 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c_tst4_3",7557,14025);
    IIwritio(0,(short *)0,1,32,0,(char *)"select i2 from data2");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c_tst4_3",7557,14025);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 480 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&qry_end,61);
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 481 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK)
	    break;
	do
	{
/* # line 486 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c_tst4_3",7557,14025,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&i2_var);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 488 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&qry_end,61);
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 489 "trl.sc" */	/* host code */
	  if (ing_err == ERR_DEADLOCK)
	      break;
	  if (ing_err != 0)
	  {
/* # line 493 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c_tst4_3",7557,14025);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 494 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 495 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE on data2 where READLOCK=SHARED");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 496 "trl.sc" */	/* host code */
		break;
	  }
	} while (qry_end == 0);
	if (ing_err == ERR_DEADLOCK)
	    break;
/* # line 502 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c_tst4_3",7557,14025);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 503 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 504 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE on data2 where READLOCK=SHARED");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 506 "trl.sc" */	/* host code */
	break;
  case 4:
	if (traceflag)
	{
		SIprintf("\t\t insert, update and delete, tbl_key=%d \n", r);
		SIflush(stdout);
	}
	m = m + 6;
/* # line 516 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&r);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&r);
    IIwritio(0,(short *)0,1,32,0,(char *)"*2, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*5, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)
"*10, '31-dec-1999', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'yyyy\
yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 520 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 521 "trl.sc" */	/* host code */
	if (ing_err != 0)
	{
		break;
	}
	m = m + 7;
/* # line 527 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data2 set i2=1, i4=1000 where i2=");
    IIputdomio((short *)0,1,30,4,&r);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 530 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 531 "trl.sc" */	/* host code */
	if (ing_err != 0)
	{
		break;
	}
	if (traceflag)
	{
		SIprintf ("\t\t case 4 delete WHERE data2.i2 = %d \n", r);
		SIflush (stdout);
	}
	m = m + 8;
/* # line 544 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from data2 where i2=");
    IIputdomio((short *)0,1,30,4,&r);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 546 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 547 "trl.sc" */	/* host code */
	if (ing_err == 0 && sqlca.sqlerrd[2] != 0)
	{
	    SIprintf ("\t\t TST4 case 4 delete WHERE data2.i2 = %d deleted %d rows\n",
	    r, sqlca.sqlerrd[2]);
	    SIprintf ("\t\t A phantom has been inserted between the UPDATE&DELETE\n");
	    SIprintf ("\t\t This is OK only if isolation level is not SERIALIZABLE\n");
	    SIprintf ("\t\t The coordinator table will have %d more rows\n",
	    sqlca.sqlerrd[2]);
	    SIflush (stdout);
	}
	break;
  case 5:
  default:
	if (traceflag)
	{
		SIprintf("\t\t inserting %d rows, tbl_key=%d \n", c*2, r);
		SIflush(stdout);
	}
	for (i=1; i<c*2; i++)
	{
/* # line 569 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&i);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&r);
    IIwritio(0,(short *)0,1,32,0,(char *)"*2, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*5, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*10, '11-may-1958')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 571 "trl.sc" */	/* host code */
 	  if (traceflag)
	  {
		SIprintf ("\t\t case 5 inserting i=%d and r*2=%d \n", i, r*2);
		SIflush (stdout);
	  }
/* # line 576 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 577 "trl.sc" */	/* host code */
	  if (ing_err != 0)
	  {
		break;
	  }
	}
	break;
  }	/* end of select	*/
/* # line 587 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 588 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
	PCsleep( (u_i4)30000 );
	return;
  }
  return;
}
/*
** Function TST5
** Purpose:	This routine inserts 128 rows in a MQT to the table data3
**		the row added will be 200 + (slave_no * 128)
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst5(s,c)
  int s;
  int c;
{
  int i;
  int base;
  int first;
  int last;
  int v_i2;
  int v_f4;
  int v_f8;
  int v_dollar;
  base		= 200;
  first		= base + (s + c - 2) * 128 + 1;
  last		= base + (s + c - 1) * 128 + 1;
/*	Set lockmode to 35 because we are going to insert 128 rows which 
**	takes 32 pages.  In this way we can avoid a deadlock when escalating
**	to table level lock.  If we don't change the value of maxlocks, they
**	stay in a deadlock loop when trying to escalate to table level when
**	10 pages have been locked (10 is the default for maxlocks).
*/
/* # line 636 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where MAXLOCKS=35");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 638 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 5, pass %d, MAXLOCKS = 35 \n",
        s, c);
    SIflush (stdout);
  }
  for (i=first; i>last; i++)
  {
    if (traceflag)
    {
      SIprintf ("\t\t inserting tuple where tbl_key = %d:\n", i); 
      SIflush (stdout);
    }
    v_i2 = i;
    v_f4 = s + i;
    v_f8 = c * i;
    v_dollar = i;
/* # line 657 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl8",7558,25801);
      IIputdomio((short *)0,1,30,4,&v_i2);
      IIputdomio((short *)0,1,30,4,&v_f4);
      IIputdomio((short *)0,1,30,4,&v_f8);
      IIputdomio((short *)0,1,30,4,&v_dollar);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl8",7558,25801);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data3(i2, f4, f8, dollar, day, c6, txt58, txt98one, txt98\
two, txt98three, txt98four)values(");
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
", '1/1/1987', 'abcedf', 'tst5_txt58_insert_value_xxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx')");
        IIexDefine(0,(char *)"trl8",7558,25801);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 666 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 667 "trl.sc" */	/* host code */
    if (ing_err != 0)
    {
      return;
    }
  }	/* end of FOR loop	*/
}
/*
** Function TST6
** Purpose:	This routine updates rows in data3 where the key is
**		50 < i2 < 80.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst6(s,c)
  int s;
  int c;
{
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 6, pass %d, update on data3 \n",
        s, c);
    SIflush (stdout);
  }
/*	Increase maxlocks so it doesn't try to escalate to table level lock */
/* # line 702 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where MAXLOCKS=25");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 704 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl9",7559,20428);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl9",7559,20428);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data3 set f4=f4*1.02, txt58='tst6_txt58_update_value', dollar=\
dollar +1.0 where i2>50 and i2<80");
        IIexDefine(0,(char *)"trl9",7559,20428);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 710 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 711 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST7
** Purpose:	This routine updates rows that match the following requirement
**		200 < i2 < 225 in the table data3.  This function should be
**		used together with tst6.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst7(s,c)
  int s;
  int c;
{
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 7, pass %d, update on data3 where 200<i2<225 \n",
        s, c);
    SIflush (stdout);
  }
/*	Increase maxlocks so it doesn't try to escalate to table level lock */
/* # line 744 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where MAXLOCKS=25");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 746 "trl.sc" */	/* host code */
/*	  EXEC SQL REPEATED UPDATE data3	JNF test	*/
/* # line 747 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data3 set f4=f4 -100, txt58='tst7_txt58_update_value' where i2\
>200 and i2<225");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 752 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 753 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST8
** Purpose:	This routine delete rows in data3 using a exact match.  It 
**		chooses the key for the exact match by multiplying the slave
**		number by 2 and the repetition pass number by 3 then adding
**		the results together.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst8(s,c)
  int s;
  int c;
{
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 8, pass %d, delete on data3 \n",
        s, c);
    SIflush (stdout);
  }
/*	Increase maxlocks so it doesn't try to escalate to table level lock */
/* # line 787 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where MAXLOCKS=20");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 790 "trl.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl10",7560,9684);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl10",7560,9684);
        IIwritio(0,(short *)0,1,32,0,(char *)"delete from data3 where \
i2=(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)"*2) +(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*3)");
        IIexDefine(0,(char *)"trl10",7560,9684);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 792 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 793 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST9
** Purpose:	This routine inserts 1 row in each pass to the table data3. 
**		Each slave will insert the same key in each pass such that
**		we get a lot of concurrency and some duplicate keys.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst9(s,c)
  int s;
  int c;
{
  int tbl_key;
  int base;
  int v_i2;
  int v_f4;
  int v_f8;
  int v_dollar;
/*	Set lockmode to 35 because we are going to insert 128 rows which 
**	takes 32 pages.  In this way we can avoid a deadlock when escalating
**	to table level lock.  If we don't change the value of maxlocks, they
**	stay in a deadlock loop when trying to escalate to table level when
**	10 pages have been locked (10 is the default for maxlocks).
*/
/*	  EXEC SQL SET LOCKMODE SESSION WHERE MAXLOCKS = 35;	JNF test */
  base	= 200;
  tbl_key	= base + (c * 2);
  v_i2	= tbl_key;
  v_f4	= s * 2;
  v_f8	= tbl_key * 20;
  v_dollar = c;
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 9, pass %d, insert 128 rows \n",
        s, c);
    SIflush (stdout);
  }
  if (traceflag)
  {
      SIprintf ("\t\t inserting tuple where tbl_key = %d:\n", tbl_key); 
      SIflush (stdout);
  }
/* # line 855 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl11",7561,15818);
      IIputdomio((short *)0,1,30,4,&v_i2);
      IIputdomio((short *)0,1,30,4,&v_f4);
      IIputdomio((short *)0,1,30,4,&v_f8);
      IIputdomio((short *)0,1,30,4,&v_dollar);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl11",7561,15818);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data3(i2, f4, f8, dollar, day, c6, txt58, txt98one, txt98\
two, txt98three, txt98four)values(");
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
", '1/1/1987', 'abcedf', 'tst9_txt58_insert_value_xxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx')");
        IIexDefine(0,(char *)"trl11",7561,15818);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 866 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 867 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST10
** Purpose:	This routine selects 128 rows into a temporary table.  Since
**		there are 4 tuples per page, 128 rows will have 32 pages
**		which is enough to fill up the AM buffer.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst10(s,c)
  int s;
  int c;
{
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 10, pass %d, create with select (128 rows)  \n",
        s, c);
    SIflush (stdout);
  }
/*EXEC SQL SET LOCKMODE SESSION WHERE LEVEL = TABLE;	JNF test	*/
/* # line 900 "trl.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop data3_tmp");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 901 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 902 "trl.sc" */	/* host code */
  if (ing_err != 0 && ing_err != ERR_DROP)
  {
/*    EXEC SQL COMMIT;	JNF test	*/
/*    EXEC SQL SET LOCKMODE SESSION WHERE LEVEL = PAGE;	JNF test	*/
    return;
  }
/* # line 909 "trl.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table data3_tmp as select * from data3 where i2<128");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 911 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 912 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
/*    EXEC SQL COMMIT;	JNF test	*/
/*    EXEC SQL SET LOCKMODE SESSION WHERE LEVEL = PAGE;	JNF test	*/
    return;
  }
/*	  EXEC SQL COMMIT;	*/
/*	  EXEC SQL SET LOCKMODE SESSION WHERE LEVEL = PAGE;	*/
/*	  EXEC SQL COMMIT;	*/
}
/*
** Function TST11
** Purpose:	This routine is the same as tst10 but with readlock = nolock.  
**	        This routine selects 128 rows into a temporary table.  Since
**		there are 4 tuples per page, 128 rows will have 32 pages
**		which is enough to fill up the AM buffer.  Do not execute 
**		this test in 5.0 with btree because there is a bug that 
**		causes a syserr when using nolock on btrees.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst11(s,c)
  int s;
  int c;
{
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 11, pass %d, tst10 with readlock=nolock \n",
        s, c);
    SIflush (stdout);
  }
/* # line 953 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where READLOCK=NOLOCK");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 955 "trl.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop data3_tmp");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 956 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 957 "trl.sc" */	/* host code */
  if (ing_err != 0 && ing_err != ERR_DROP)
  {
    return;
  }
/* # line 962 "trl.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table data3_tmp as select * from data3 where i2<128");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 964 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 965 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
/* # line 970 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where READLOCK=SHARED");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 971 "trl.sc" */	/* host code */
}
/*
** Function TST12
** Purpose:	Modify data3 to other structures, should get an Xclusive table 
**		lock.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst12(s,c)
  int s;
  int c;
{
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 12, pass %d, modify data3 structures \n",
        s, c);
    SIflush (stdout);
  }
/*  EXEC SQL SET LOCKMODE SESSION WHERE LEVEL = PAGE;	JNF test	*/
  switch (c)
  {
  case '1':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying datat3 to btree \n" );
	  SIflush (stdout);
	}
/* # line 1007 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify data3 to BTREE on i2 with FILLFACTOR=60, INDEXFILL=60");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1008 "trl.sc" */	/* host code */
	break;
  case '2':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying datat3 to isam \n" );
	  SIflush (stdout);
	}
/* # line 1015 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify data3 to ISAM on i2 with FILLFACTOR=95");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1016 "trl.sc" */	/* host code */
	break;
  case '3':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying datat3 to chash \n" );
	  SIflush (stdout);
	}
/* # line 1023 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify data3 to CHASH on i2, f4, f8 with FILLFACTOR=90");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1024 "trl.sc" */	/* host code */
	break;
  case '4':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying datat3 to cheap\n" );
	  SIflush (stdout);
	}
/* # line 1031 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify data3 to CHEAP");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1032 "trl.sc" */	/* host code */
	break;
  default:
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying datat3 to btree \n" );
	  SIflush (stdout);
	}
/* # line 1039 "trl.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify data3 to BTREE on i2 with FILLFACTOR=80, INDEXFILL=85");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1040 "trl.sc" */	/* host code */
	break;
  }
/* # line 1043 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1044 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST13
** Purpose:	Create secondary indices for data3.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst13(s,c)
  int s;
  int c;
{
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 13, pass %d, create secondary indices for data3 \n",
        s, c);
    SIflush (stdout);
  }
  switch (c)
  {
  case '1':
	if (traceflag)
	{
	  SIprintf ( "\t\tcreating index with f8, dollar, day, c6 and txt98four \n" );
	  SIflush (stdout);
	}
/* # line 1082 "trl.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index data3ind on data3(f8, dollar, day, c6, txt98four)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1083 "trl.sc" */	/* host code */
	break;
  case '2':
	if (traceflag)
	{
	  SIprintf ( "\t\tcreating index with txt58, txt98one, txt98two and txt98three  \n" );
	  SIflush (stdout);
	}
/* # line 1090 "trl.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index data3ind on data3(txt58, txt98one, txt98two, txt98three)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1092 "trl.sc" */	/* host code */
	break;
  default:
	if (traceflag)
	{
	  SIprintf ( "\t\tcreating index with i2, f4 and f8 \n" );
	  SIflush (stdout);
	}
/* # line 1099 "trl.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index data3ind on data3(i2, f4, f8)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1100 "trl.sc" */	/* host code */
	break;
  }
/* # line 1103 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1104 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
**
**	The purpose of tests 14 thru 18 is to exercise the system catalogs in
**	a concurrent environment by doing creates, drops, modifies, and by
**	defining permits.
**
*/
/*
** Function TST14
** Purpose:	This test creates a table called data4_x where x is the slave 
**		number
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst14(s,c)
  int s;
  int c;
{
  char table_name[13];
  char string[20];
  char prepare_buffer[1000];
  STcopy ("data4_", table_name);
  CVla (s, string);
  STcat (table_name, string);
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 14, pass %d, creating table %s\n",
	s, c, table_name);
    SIflush (stdout);
  }
  STcopy ("CREATE TABLE ", prepare_buffer);
  STcat (prepare_buffer, table_name);
  STcat (prepare_buffer, " ( id i2, c20 c20, ii i1, i2 i2, i4 i4, ");
  STcat (prepare_buffer, "f4 f4, f8 f8, money money, date date,  ");
  STcat (prepare_buffer, "c1 c1, char1 text(1), c50 c50, ");
  STcat (prepare_buffer, "char50 text(50) )" );
/* # line 1159 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1161 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1162 "trl.sc" */	/* host code */
  if (ing_err == ERR_CREATE || ing_err == ERR_DUPLICATE)
  {
    ing_err = 0; 
  }
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST15
** Purpose:	This test modifies table data4_x to other structures (btree,
**		isam, chash, cbtree, cheap).  The table is dependent on the 
**		slave that is executing the function (data4_x where x = s) and 
**		the structure of the table is dependent on the pass (c).
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst15(s,c)
  int s;
  int c;
{
  char table_name[13];
  char string[20];
  char prepare_buffer[1000];
  STcopy ("data4_", table_name);
  CVla (s, string);
  STcat (table_name, string);
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 15, pass %d, modifying table %s\n",
	s, c, table_name);
    SIflush (stdout);
  }
  switch (c)
  {
  case '1':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying %s to btree \n", table_name);
	  SIflush (stdout);
	}
	STcopy ("MODIFY ", prepare_buffer);
	STcat (prepare_buffer, table_name);
	STcat (prepare_buffer, " TO BTREE UNIQUE WITH FILLFACTOR=95,");  
	STcat (prepare_buffer, " INDEXFILL=50, maxindexfill=90");
/* # line 1222 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1223 "trl.sc" */	/* host code */
	break;
  case '2':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying %s to isam \n", table_name);
	  SIflush (stdout);
	}
	STcopy ("MODIFY ", prepare_buffer);
	STcat (prepare_buffer, table_name);
	STcat (prepare_buffer, " TO ISAM WITH FILLFACTOR=25 ");  
/* # line 1234 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1235 "trl.sc" */	/* host code */
	break;
  case '3':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying %s to chash \n", table_name);
	  SIflush (stdout);
	}
	STcopy ("MODIFY ", prepare_buffer);
	STcat (prepare_buffer, table_name);
	STcat (prepare_buffer, " TO CHASH ON f4, f8 WITH FILLFACTOR=90,  ");  
	STcat (prepare_buffer, " MAXPAGES=100, MINPAGES=30");
/* # line 1247 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1248 "trl.sc" */	/* host code */
	break;
  case '4':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying %s to cbtree \n", table_name);
	  SIflush (stdout);
	}
	STcopy ("MODIFY ", prepare_buffer);
	STcat (prepare_buffer, table_name);
	STcat (prepare_buffer, " TO CBTREE ");  
/* # line 1259 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1260 "trl.sc" */	/* host code */
	break;
  case '5':
	if (traceflag)
	{
	  SIprintf ( "\t\tmodifying %s to cheap \n", table_name);
	  SIflush (stdout);
	}
	STcopy ("MODIFY ", prepare_buffer);
	STcat (prepare_buffer, table_name);
	STcat (prepare_buffer, " TO CHEAP ");  
/* # line 1271 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1272 "trl.sc" */	/* host code */
	break;
  default:
	if (traceflag)
	{
	  SIprintf ( "\t\tdefault modifying datat3 to btree \n", table_name);
	  SIflush (stdout);
	}
	STcopy ("MODIFY ", prepare_buffer);
	STcat (prepare_buffer, table_name);
	STcat (prepare_buffer, " TO BTREE WITH FILLFACTOR=30,");  
	STcat (prepare_buffer, " INDEXFILL=50, maxindexfill=80");
/* # line 1284 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1285 "trl.sc" */	/* host code */
	break;
  }
/* # line 1288 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1289 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST16
** Purpose:	This routine creates indices for the table data4_x where x is 
**		the slave_number.  It should be executed after routine tst14
**		is executed because tst14 creates the tables.  
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst16(s,c)
  int s;
  int c;
{
  char table_name[13];
  char string[20];
  char prepare_buffer[1000];
  STcopy ("data4_", table_name);
  CVla (s, string);
  STcat (table_name, string);
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 16, pass %d, creating indices for %s\n",
	s, c, table_name);
    SIflush (stdout);
  }
  switch ( c % 3 )
  {
  case '1':
	if (traceflag)
	{
	  SIprintf ( "\t\tcreating indices for %s \n", table_name);
	  SIflush (stdout);
	}
	STprintf(prepare_buffer, "CREATE INDEX ind%s ON %s  (i4, date, money, f8) ",
		table_name, table_name);
/* # line 1342 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1343 "trl.sc" */	/* host code */
	break;
  case '2':
	if (traceflag)
	{
	  SIprintf ( "\t\tcreating multiple indices for %s \n", table_name);
	  SIflush (stdout);
	}
        STprintf(prepare_buffer, "CREATE INDEX ind%s1 ON %s (i4)",
                table_name, table_name);
/* # line 1353 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1354 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1355 "trl.sc" */	/* host code */
	if (ing_err != 0)
	{
	  return;
	}
        STprintf(prepare_buffer, "CREATE INDEX ind%s2 ON %s (i2, f4, date) WITH  STRUCTURE = BTREE ",
                table_name, table_name);
/* # line 1362 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1363 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1364 "trl.sc" */	/* host code */
	if (ing_err != 0)
	{
	  return;
	}
        STprintf(prepare_buffer, "CREATE INDEX ind%s3 ON %s (tin) WITH STRUCTURE = HASH ",
                table_name, table_name);
/* # line 1371 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1372 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1373 "trl.sc" */	/* host code */
	if (ing_err != 0)
	{
	  return;
	}
	STprintf(prepare_buffer, "CREATE INDEX ind%s4 ON %s (char50) WITH STRUCTURE = ISAM ", 
		table_name, table_name);
/* # line 1380 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1381 "trl.sc" */	/* host code */
	break;
  default:
	if (traceflag)
	{
	  SIprintf ( "\t\tdefault creating indices for %s \n", table_name);
	  SIflush (stdout);
	}
	STprintf(prepare_buffer, "CREATE INDEX ind%s ON %s (i4, date, money) ",
		table_name, table_name);
/* # line 1391 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1392 "trl.sc" */	/* host code */
	break;
  }
/* # line 1395 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1396 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST17
** Purpose:	This routine defines permits on the table data4_x where x is
**		the slave_number.  It should be executed after tst14 is
**		executed because tst14 creates the table.  Also remember
**		that this routine should be executed by the DBA who is the
**		only one that can define permits on a table.
**
** Parameters:	s = slave number.
**		c = number of repetitions
**
** Returns:
*/
tst17(s,c)
  int s;
  int c;
{
  char table_name[13];
  char string[20];
  char prepare_buffer[1000];
  STcopy ("data4_", table_name);
  CVla (s, string);
  STcat (table_name, string);
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 17, pass %d, granting permits %s\n",
	s, c, table_name);
    SIflush (stdout);
  }
  switch (c)
  {
  case '1':
	if (traceflag)
	{
	  SIprintf ( "\t\tgranting all to all for %s \n", table_name);
	  SIflush (stdout);
	}
	STprintf(prepare_buffer, "GRANT ALL ON %s TO PUBLIC ", table_name);  
/* # line 1449 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1450 "trl.sc" */	/* host code */
	break;
  case '2':
	if (traceflag)
	{
	  SIprintf ( "\t\tgranting update to myrtle for %s \n", table_name);
	  SIflush (stdout);
	}
	STprintf(prepare_buffer, "GRANT UPDATE ON %s TO myrtle", table_name);  
/* # line 1458 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1459 "trl.sc" */	/* host code */
	break;
  case '3':
	if (traceflag)
	{
	  SIprintf ( "\t\tgranting insert to jupqatest for %s \n", table_name);
	  SIflush (stdout);
	}
	STprintf(prepare_buffer, "GRANT INSERT ON %s TO jupqatest", table_name);  
/* # line 1467 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1468 "trl.sc" */	/* host code */
	break;
  case '4':
	if (traceflag)
	{
	  SIprintf ( "\t\tgranting update, insert and delete for %s \n", table_name);
	  SIflush (stdout);
	}
	STprintf(prepare_buffer, "GRANT UPDATE ON %s TO myrtle", table_name);  
/* # line 1476 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1477 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1478 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
	else if (sqlca.sqlcode < 0)
	{
	  break;
	}
	STprintf(prepare_buffer, "GRANT DELETE ON %s TO gladys", table_name);  
/* # line 1488 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1489 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1490 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
	else if (sqlca.sqlcode < 0)
	{
	  break;
	}
	STprintf(prepare_buffer, "GRANT INSERT ON %s TO qatests", table_name);  
/* # line 1500 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1501 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1502 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
	STprintf(prepare_buffer, "CREATE INTEGRITY ON %s IS i2 < 100", 
		table_name);  
/* # line 1509 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1510 "trl.sc" */	/* host code */
	break;
  default:
	if (traceflag)
	{
	  SIprintf ( "\t\tdefault granting qatest insert and delete for %s \n", table_name);
	  SIflush (stdout);
	}
	STprintf(prepare_buffer, "GRANT DELETE, INSERT ON %s TO qatest\n", table_name);  
/* # line 1518 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1520 "trl.sc" */	/* host code */
	STprintf(prepare_buffer, "CREATE INTEGRITY ON %s IS i2 > 100", 
		table_name);  
/* # line 1522 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1524 "trl.sc" */	/* host code */
	break;
  }
/* # line 1526 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1527 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST18
** Purpose:	This routine drops a table according to the slave number.  It 
**		should be executed after tst14 is executed because tst14 
**		creates the temporary tables.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst18(s,c)
  int s;
  int c;
{
  char table_name[13];
  char string[20];
  char prepare_buffer[1000];
  STcopy ("data4_", table_name);
  CVla (s, string);
  STcat (table_name, string);
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 18, pass %d, dropping table %s\n",
	s, c, table_name);
    SIflush (stdout);
  }
  STprintf(prepare_buffer, "DROP %s ", table_name);  
/* # line 1571 "trl.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(prepare_buffer);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1573 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1574 "trl.sc" */	/* host code */
  if (ing_err != 0 && ing_err != ERR_DROP)
  {
    return;
  }
}
/*
** Function TST19
** Purpose:	This routine performs different transactions on data2 and then
**		aborts them.  It uses a random number generator to decide which
**		transcation to execute.  The seed for the random number should 
**		be placed in the table seeds and should be done before the 
**		CTS test that executes this routine is executed.  
**		Note that only one transaction will be excuted (aborted) in each
**		pass.  Most transactions use repeat queries and perform 
**		operations that require exclusive locks.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst19(s,c)
  int s;
  int c;
{
  int num;
  int r;
  int m;
  int i;
  int seed;
  char date[26];
  char xxx61[62];
  char yyy100[101];
/* # line 1615 "trl.sc" */	/* select */
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
      CTS_errors();
  }
/* # line 1616 "trl.sc" */	/* host code */
  MHsrand ( (f8)((seed + s) * c) );
  r   = (i4) ((MHrand() * ( MHrand() * (f8) (32767))) +1);
  m   = r % 6;
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 19, pass %d, Testing Rollback with seed = %d\n",
	s, c, seed );
    SIflush (stdout);
  }
  STcopy ("31-dec-1999", date);
  STcopy ("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", xxx61);
  STcopy ("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy", yyy100);
  switch (m)
  {
  case '0':
	if (traceflag)
	{
	  SIprintf ( "\t\tinserting tbl_key = %d ", r );
	  SIflush (stdout);
	}
/* # line 1640 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl12",7562,26094);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,32,0,date);
      IIputdomio((short *)0,1,32,0,xxx61);
      IIputdomio((short *)0,1,32,0,yyy100);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl12",7562,26094);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
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
        IIwritio(0,(short *)0,1,32,0,(char *)"*10, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,date);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,xxx61);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,yyy100);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl12",7562,26094);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1642 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1643 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
/* # line 1648 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1649 "trl.sc" */	/* host code */
	break;
  case '1':
	if (traceflag)
	{
	  SIprintf ( "\t\tinserting and deleting tbl_key = %d ", r );
	  SIflush (stdout);
	}
/* # line 1657 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl13",7563,28132);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,32,0,date);
      IIputdomio((short *)0,1,32,0,xxx61);
      IIputdomio((short *)0,1,32,0,yyy100);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl13",7563,28132);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
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
        IIwritio(0,(short *)0,1,32,0,(char *)"*10, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,date);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,xxx61);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,yyy100);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl13",7563,28132);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1659 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1660 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
/* # line 1665 "trl.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl14",7564,25836);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl14",7564,25836);
        IIwritio(0,(short *)0,1,32,0,(char *)"delete from data2 where \
i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl14",7564,25836);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1666 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1667 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
/* # line 1672 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1673 "trl.sc" */	/* host code */
	break;
  case '2':
	if (traceflag)
	{
	  SIprintf ( "\t\tupdating tbl_key = %d ", r );
	  SIflush (stdout);
	}
/* # line 1681 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl15",7565,23242);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl15",7565,23242);
        IIwritio(0,(short *)0,1,32,0,(char *)"update data2 set i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)" +1, i4=i4 +1000 where i\
2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&r);
        IIwritio(0,(short *)0,1,32,0,(char *)"and i4>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl15",7565,23242);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1684 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1685 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
/* # line 1690 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1691 "trl.sc" */	/* host code */
	break;
  case '3':
	if (traceflag)
	{
	  SIprintf ( "\t\tselecting all the rows without readlock=nolock \n" );
	  SIflush (stdout);
	}
/* # line 1699 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE on data2 where READLOCK=NOLOCK");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1700 "trl.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl16",7566,23019);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"trl16",7566,23019);
        IIwritio(0,(short *)0,1,32,0,(char *)"select i2 from data2");
        IIexDefine(0,(char *)"trl16",7566,23019);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&r);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1701 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1702 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
/* # line 1707 "trl.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1708 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE on data2 where READLOCK=SHARED");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1709 "trl.sc" */	/* host code */
	break;
  case '4':
	if (traceflag)
	{
	  SIprintf ( "\t\tupdating, deleting, and inserting tbl_key = %d ", r );
	  SIflush (stdout);
	}
/* # line 1717 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl17",7567,10433);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl17",7567,10433);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data2 set i2=1, i4=1000 where i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl17",7567,10433);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1720 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1721 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
/* # line 1726 "trl.sc" */	/* savepoint */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"savepoint saveone");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1728 "trl.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl18",7568,11969);
      IIputdomio((short *)0,1,30,4,&r);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl18",7568,11969);
        IIwritio(0,(short *)0,1,32,0,(char *)"delete from data2 where \
i2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&r);
        IIexDefine(0,(char *)"trl18",7568,11969);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1729 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1730 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
/* # line 1735 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"rollback to saveone");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1737 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl19",7569,13287);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&r);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,32,0,date);
      IIputdomio((short *)0,1,32,0,xxx61);
      IIputdomio((short *)0,1,32,0,yyy100);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl19",7569,13287);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
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
        IIwritio(0,(short *)0,1,32,0,(char *)"*10, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,date);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,xxx61);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,yyy100);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl19",7569,13287);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1739 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1740 "trl.sc" */	/* host code */
	if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	{
	  return;
	}
/* # line 1745 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1747 "trl.sc" */	/* host code */
	break;
  case '5':
	if (traceflag)
	{
	  SIprintf ( "\t\tinserting %d rows in a MST statement \n", c*2 );
	  SIflush (stdout);
	}
	for (i=1; i<c*2; i++)
	{
/* # line 1757 "trl.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&r);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&r);
    IIwritio(0,(short *)0,1,32,0,(char *)"*2, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*5, ");
    IIputdomio((short *)0,1,30,4,&c);
    IIwritio(0,(short *)0,1,32,0,(char *)"*10, ");
    IIputdomio((short *)0,1,32,0,date);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,xxx61);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,yyy100);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1759 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1760 "trl.sc" */	/* host code */
 	  if (ing_err == ERR_DEADLOCK || ing_err == ERR_LOGFULL)
	  {
	    return;
	  }
	}
/* # line 1766 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1768 "trl.sc" */	/* host code */
	break;
  }
/* # line 1771 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1772 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST20
** Purpose:	This routine will select data from 5 different tables.  The 
**		data will be a result simple table selects, table joins, and
**		the order by function.  This routine will also declare 5
**		cursors to handle the select transactions.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst20(s,c)
  int s;
  int c;
{
  int d5tbl_key;
  int d5cnt;
  int d6tbl_key;
  int d6cnt;
  float d6float;
  int d7tbl_key;
  float d7money;
  int d8tbl_key;
  char d8char[66];
  int d9tbl_key;
  char d9date[26];
  int done;
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 20, pass %d Select with cursors \n",
        s, c);
    SIflush (stdout);
  }
  /*
  ** Increase maxlocks to avoid deadlock when escalating to table level lock.
  */
/* # line 1828 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where MAXLOCKS=35");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1830 "trl.sc" */	/* host code */
/* Plain and boring old select, get all rows from a single table */
/* # line 1834 "trl.sc" */	/* host code */
/* Qualify the select, and include join */
/* # line 1842 "trl.sc" */	/* host code */
/* Getting more complicated, qualify the select, and also throw in a	*/
/*	join and a order by.						*/
/* # line 1852 "trl.sc" */	/* host code */
/* Same as cursor c3, qualify the select, and also throw in a join and	*/
/*	a order by.  The order by also uses columns from 2 different	*/
/*	tables.								*/
/* # line 1863 "trl.sc" */	/* host code */
/* Another plain old select, but getting columns from different tables.	*/
/* # line 1870 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("\t\tSelect from data5 only (all)\n");
    SIflush (stdout);
  }
  done = 0;
/* # line 1877 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c1",7570,7356);
    IIwritio(0,(short *)0,1,32,0,(char *)"select tbl_key, cnt from dat\
a5");
    IIcsQuery((char *)"c1",7570,7356);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1878 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1879 "trl.sc" */	/* host code */
  while (!done && !ing_err)
    {
/* # line 1881 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c1",7570,7356,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5tbl_key);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1882 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1883 "trl.sc" */	/* host code */
    if ( !done && !ing_err && traceflag)
    {
      SIprintf ("\t\tc1 got %d, %d \n\0", d5tbl_key, d5cnt);
      SIflush (stdout);
    }
  }
  if (ing_err != 0)
  {
    return;
  }
/* # line 1895 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c1",7570,7356);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1897 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("\t\tJoin all tables where data5.cnt < 50 \n");
    SIflush (stdout);
  }
  done = 0;
/* # line 1904 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c2",7571,6629);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.tbl_key, a.cnt, b.floatv, f.money, d.charv, e.date from data\
5 a, data6 b, data7 f, data8 d, data9 e where a.cnt<50 and a.tbl_key=b\
.tbl_key and a.tbl_key=f.tbl_key and a.tbl_key=d.tbl_key and a.tbl_key\
=e.tbl_key");
    IIcsQuery((char *)"c2",7571,6629);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1905 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1906 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 1908 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c2",7571,6629,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5tbl_key);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1909 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1910 "trl.sc" */	/* host code */
    if ( !done && !ing_err && traceflag)
    {
      SIprintf ("\t\tc2 got %d, %d \n\0", d5tbl_key, d5cnt );
      SIflush (stdout);
    }
  } 
  if (ing_err != 0)
  {
    return;
  }
/* # line 1922 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c2",7571,6629);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1924 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("\t\tJoin all tables where data5.cnt > 20 \n");
    SIflush (stdout);
  }
  done = 0;
/* # line 1931 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c3",7572,5859);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.tbl_key, a.cnt, b.floatv, f.money, d.charv, e.date from data\
5 a, data6 b, data7 f, data8 d, data9 e where a.cnt>20 and a.tbl_key=b\
.tbl_key and a.tbl_key=f.tbl_key and a.tbl_key=d.tbl_key and a.tbl_key\
=e.tbl_key order by a.tbl_key, a.cnt");
    IIcsQuery((char *)"c3",7572,5859);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1932 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1933 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 1935 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c3",7572,5859,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5tbl_key);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1936 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1937 "trl.sc" */	/* host code */
    if ( !done && !ing_err && traceflag)
    {
      SIprintf ("\t\tc3 got %d, %d \n\0", d5tbl_key, d5cnt );
      SIflush (stdout);
    }
  }  
  if (ing_err != 0)
  {
    return;
  }
/* # line 1948 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c3",7572,5859);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1950 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("\t\tJoin all tables where data5.cnt >10 and data6.cnt < 200 \n");
    SIflush (stdout);
  }
  done = 0;
/* # line 1957 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c4",7573,3302);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.tbl_key, a.cnt, b.cnt, b.floatv, f.money, d.charv, e.date fr\
om data5 a, data6 b, data7 f, data8 d, data9 e where a.cnt>10 and b.cn\
t<200 and a.tbl_key=b.tbl_key and a.tbl_key=f.tbl_key and a.tbl_key=d.\
tbl_key and a.tbl_key=e.tbl_key order by a.");
    IIwritio(0,(short *)0,1,32,0,(char *)"tbl_key, b.cnt, f.money");
    IIcsQuery((char *)"c4",7573,3302);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1958 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1959 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 1961 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c4",7573,3302,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5tbl_key);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsGetio((short *)0,1,30,4,&d6cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1963 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1964 "trl.sc" */	/* host code */
    if ( !done && !ing_err && traceflag)
    {
      SIprintf ("\t\tc4 got %d, %d \n\0", d5tbl_key, d5cnt );
      SIflush (stdout);
    }
  }
  if (ing_err != 0)
  {
    return;
  }
/* # line 1975 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c4",7573,3302);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1977 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("\t\tJoin all tables \n");
    SIflush (stdout);
  }
  done = 0;
/* # line 1984 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c5",7574,12989);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.tbl_key, a.cnt, b.floatv, f.money, d.charv, e.date from data\
5 a, data6 b, data7 f, data8 d, data9 e where a.tbl_key=b.tbl_key and \
a.tbl_key=f.tbl_key and a.tbl_key=d.tbl_key and a.tbl_key=e.tbl_key");
    IIcsQuery((char *)"c5",7574,12989);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1985 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 1986 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 1988 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c5",7574,12989,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5tbl_key);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 1989 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 1990 "trl.sc" */	/* host code */
    if ( !done && !ing_err && traceflag)
    {
      SIprintf ("\t\tc5 got %d, %d \n\0", d5tbl_key, d5cnt );
      SIflush (stdout);
    }
  }
  if (ing_err != 0)
  {
    return;
  }
/* # line 2001 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c5",7574,12989);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2003 "trl.sc" */	/* host code */
}
/*
** Function TST21
** Purpose:	This routine uses a cursor to select rows from data5 where 
**		30 < tbl_key < 300.  These rows are then updated in deferred mode.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst21(s,c)
  int s;
  int c;
{
  int d5tbl_key;
  int d5cnt;
  int done;
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 21, pass %d, deferred update using cursors where 30<tbl_key<300 \n",
        s, c);
    SIflush (stdout);
  }
  /*
  ** Increase maxlocks to avoid deadlock when escalating to table level lock.
  */
/* # line 2040 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where MAXLOCKS=35");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2047 "trl.sc" */	/* host code */
  done = 0;
/* # line 2048 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c6",7575,451);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tbl_key, cnt from data5 where tbl_key>30 and tbl_key<300 for u\
pdate of tbl_key, cnt");
    IIcsQuery((char *)"c6",7575,451);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2049 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2050 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 2052 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c6",7575,451,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5tbl_key);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2053 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2054 "trl.sc" */	/* host code */
    if ( !done && !ing_err )
    {
/* # line 2056 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data5 set tbl_key=tbl_key +20");
    IIcsERplace((char *)"c6",7575,451);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2060 "trl.sc" */	/* host code */
      /*
      ** After UPDATE, INQUIRE_INGRES again.
      ** We may deadlock trying to get the lock on the new page.
      */
/* # line 2064 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2066 "trl.sc" */	/* host code */
      if (traceflag && !done && !ing_err)
      {
	SIprintf ("\t\tc6 Direct --> tbl_key = %d, cnt = %d \n", 
		d5tbl_key, d5cnt );
	SIflush (stdout);
      }
    }
  } 
/* # line 2075 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2076 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
/* # line 2080 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c6",7575,451);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2081 "trl.sc" */	/* host code */
}
/*
** Function TST22
** Purpose:	This routine uses a cursor to select rows from data6.  These 
**		rows are then updated in deferred mode.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst22(s,c)
  int s;
  int c;
{
  int d6tbl_key;
  int d6cnt;
  float d6float;
  int done;
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 22, pass %d, deferred update using cursors on data6 \n",
        s, c);
    SIflush (stdout);
  }
/* # line 2119 "trl.sc" */	/* host code */
  done = 0;
/* # line 2120 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c7",7576,1216);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tbl_key, cnt, floatv from data6 for update of tbl_key, cnt, fl\
oatv");
    IIcsQuery((char *)"c7",7576,1216);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2121 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2123 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 2125 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c7",7576,1216,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d6tbl_key);
      IIcsGetio((short *)0,1,30,4,&d6cnt);
      IIcsGetio((short *)0,1,31,4,&d6float);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2126 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2127 "trl.sc" */	/* host code */
    if ( !done && !ing_err )
    {
/* # line 2129 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data6 set tbl_key=tbl_key -10, floatv=floatv*2");
    IIcsERplace((char *)"c7",7576,1216);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2132 "trl.sc" */	/* host code */
      if (traceflag)
      {
	SIprintf ("\t\tc7 Direct --> tbl_key = %d, cnt = %d, float = %f \n", 
		d6tbl_key, d6cnt, d6float );
	SIflush (stdout);
      }
    }
  } 
/* # line 2141 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2142 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
/* # line 2146 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c7",7576,1216);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2147 "trl.sc" */	/* host code */
}
/*
** Function TST23
** Purpose:	This routine uses a cursor to select rows from data7.  It will
**		only select row where tbl_key < 200 and money > 10.  These 
**		rows are then updated in deferred mode.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst23(s,c)
  int s;
  int c;
{
  int d7tbl_key;
  float d7money;
  int done;
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 23, pass %d, deferred update using cursors on data7 \n",
        s, c);
    SIflush (stdout);
  }
/* # line 2185 "trl.sc" */	/* host code */
  done = 0;
/* # line 2186 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c8",7577,7615);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tbl_key, money from data7 where tbl_key<200 and money>10 for u\
pdate of tbl_key, money");
    IIcsQuery((char *)"c8",7577,7615);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2187 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2188 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 2190 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c8",7577,7615,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d7tbl_key);
      IIcsGetio((short *)0,1,31,4,&d7money);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2191 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2192 "trl.sc" */	/* host code */
    if ( !done && !ing_err )
    {
/* # line 2194 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data7 set tbl_key=tbl_key*2");
    IIcsERplace((char *)"c8",7577,7615);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2198 "trl.sc" */	/* host code */
      if (traceflag)
      {
	SIprintf ("\t\tc8 Direct --> tbl_key = %d, %f \n", 
		d7tbl_key, d7money );
	SIflush (stdout);
      }
    }
  } 
  if (ing_err != 0)
  {
    return;
  }
/* # line 2212 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c8",7577,7615);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2214 "trl.sc" */	/* host code */
}
/*
** Function TST24
** Purpose:	This routine uses two cursors to select rows from table data5 
**		and data8.  The selects are used nested inside a while loop, 
**		with the inner select updating the data8 table.
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst24(s,c)
  int s;
  int c;
{
  int d5tbl_key;
  int d5cnt;
  int d8tbl_key;
  char d8char[66];
  int done;
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 24, pass %d, nested selects with cursors on data5 and data8 \n",
        s, c);
    SIflush (stdout);
  }
  /*
  ** Increase maxlocks to avoid deadlock when escalating to table level lock.
  */
/* # line 2254 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where MAXLOCKS=35");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2265 "trl.sc" */	/* host code */
  done = 0;
/* # line 2266 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c10",7579,26839);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tbl_key, charv from data8 where tbl_key>10 and tbl_key<100 for\
 update of tbl_key, charv");
    IIcsQuery((char *)"c10",7579,26839);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2267 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2269 "trl.sc" */	/* host code */
  if ( ing_err != 0 )
  {
    return;
  }
/* # line 2274 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c9",7578,2231);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.tbl_key, a.cnt from data5 a, data8 d where a.tbl_key=d.tbl_k\
ey");
    IIcsQuery((char *)"c9",7578,2231);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2275 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2277 "trl.sc" */	/* host code */
  if ( ing_err != 0 )
  {
    return;
  }
  while (!done && !ing_err)
  {
/* # line 2284 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c9",7578,2231,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d5tbl_key);
      IIcsGetio((short *)0,1,30,4,&d5cnt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2285 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2286 "trl.sc" */	/* host code */
    if ( !done && !ing_err )
    {  
      if ( traceflag )
      {
	SIprintf ("\t\tc9: %d %d \n", d5tbl_key, d5cnt);
	SIflush (stdout);
      }
/* # line 2294 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c10",7579,26839,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d8tbl_key);
      IIcsGetio((short *)0,1,32,65,d8char);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2295 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2296 "trl.sc" */	/* host code */
      if ( !done && !ing_err )
      {
	if ( traceflag )
	{
	  SIprintf ("\t\tc10: %d %s \n", d8tbl_key, d8char );
	  SIflush (stdout);
	}
/* # line 2304 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data8 set tbl_key=tbl_key*2, charv='Test 24 update value '");
    IIcsERplace((char *)"c10",7579,26839);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2307 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2308 "trl.sc" */	/* host code */
      }
    } 
  }
  if ( ing_err != 0 )
  {
    return;
  }
/* # line 2317 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c9",7578,2231);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2318 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c10",7579,26839);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2319 "trl.sc" */	/* host code */
}
/* 
** Function TST25 
** Purpose:	This routine uses two cursors to select rows from the same 
**		data9 table with options for direct update.  The cursors will
**		be opened and closed one at a time, not concurrently. 
** Parameters: 
**          s = number of slaves. 
**	    c = number of repetitions.
** Returns: 
*/ 
tst25(s,c)
  int s;
  int c;
{
  int d9tbl_key;
  char d9date[26];
  int done;
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 25, pass %d, selects with 2 cursors on data9 \n",
        s, c);
    SIflush (stdout);
  }
/* # line 2362 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("\t\t delete data9 where tbl_key<10 or tbl_key>300 using cursors \n");
    SIflush (stdout);
  }
  done = 0;
/* # line 2369 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c11",7580,5308);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tbl_key, date from data9 where tbl_key<10 or tbl_key>300 for u\
pdate of tbl_key");
    IIcsQuery((char *)"c11",7580,5308);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2370 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2371 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 2373 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c11",7580,5308,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d9tbl_key);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2374 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2375 "trl.sc" */	/* host code */
    if ( !done && !ing_err )
    {
/* # line 2378 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"data9",(char *)"c11",7580,5308);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2380 "trl.sc" */	/* host code */
      if ( traceflag )
      {
	SIprintf ("\t\tc11 delete : %d %s \n", d9tbl_key, d9date );
	SIflush (stdout);
      }
    }
  }
  if (ing_err != 0)
  {
/* # line 2390 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c11",7580,5308);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2391 "trl.sc" */	/* host code */
    return;
  }
/* # line 2394 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c11",7580,5308);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2396 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2397 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
  if (traceflag)
  {
    SIprintf ("\t\t update data9 where 10<tbl_key<300 using cursors \n");
    SIflush (stdout);
  }
  done = 0;
/* # line 2409 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c12",7581,6618);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select tbl_key, date from data9 where tbl_key>10 and tbl_key<300 for \
update of tbl_key");
    IIcsQuery((char *)"c12",7581,6618);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2410 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2411 "trl.sc" */	/* host code */
  while (!done && !ing_err)
  {
/* # line 2413 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"c12",7581,6618,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&d9tbl_key);
      IIcsGetio((short *)0,1,32,25,d9date);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2414 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 2415 "trl.sc" */	/* host code */
    if ( !done && !ing_err )
    {
/* # line 2417 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data9 set tbl_key=(tbl_key*3)/2");
    IIcsERplace((char *)"c12",7581,6618);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2421 "trl.sc" */	/* host code */
      if ( traceflag )
      {
	SIprintf ("\t\tc12 update : %d %s \n", d9tbl_key, d9date );
	SIflush (stdout);
      }
    }
  } 
  if (ing_err != 0)
  {
/* # line 2431 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c12",7581,6618);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2432 "trl.sc" */	/* host code */
    return;
  }
/* # line 2435 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"c12",7581,6618);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2437 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2438 "trl.sc" */	/* host code */
  if (ing_err != 0)
  {
    return;
  }
}
/* 
** Function tst26 
** Purpose:	Test the index compressions with concurrency where the key 
**		ranges from very compressable, to keys where the compression 
**		would consist of no compression at all.  There would be four 
**		runs of this test with the first run on a old format 
**		un-compressed btree table, the second will be on a new 
**		compressed btree table with data and index compressions, 
**		third on a btree with index compression only, and the last 
**		depending on time, will be btree with data compression only.
**
**		The function which would be run 60 (?) times by each slave 
**		would follow the following pseudo code:
**
**		for (i=0; i <= repetition number; i++)
**		{
**		    insert into table row where numeric are (slave number ** i + i)
**			and character field are copies of predetermined strings
**			in array[ ((slave number *i) mod array size) ]
**		}
**
**		The array elements would be along the following lines of thought:
**		array[0] = 'a'
**		array[1] - 'abcde'
**		array[2] - 'abcdefghij'
**		array[3] - 'abcdefghijklmn'
**		array[4] - 'abcdefghijklmnopqr'
**		array[5] - 'abcdefghijklmnopqrstuvw'
**		array[6] - 'abcdefg... 90 characters ...stuvwxyz'
**
**		The table would be along the following lines of thought::
**	Column Name                      Type		     Length
**						     external	    internal
**	i2                               integer         2		 5
**	i2_1                             integer         2       	 5
**	i2_2                             integer         2       	 5
**	i2_3                             integer         2       	 5
**	f4                               float           4	 	 5
**	f8                               float           8	 	 9
**	dollar                           money           8            	 9
**	day                              date           12	 	13
**	day_1                            date           12	 	13
**	day_2                            date           12	 	13
**	c90                              c              90	 	91
**	txt1                             char           90	 	91
**	txt2                             vchar          90	 	93
**	txt3                             text           90		93
**       -----------------------------------------------------------------
**	text                             vchar         100 
**
**
** Parameters: 
**          s = slave number. 
**	    c = number of repetitions.
** Returns: 
*/ 
tst26(s,c)
  int s;
  int c;
{
  int i;
  int element;
  char array_buffer[100];
  static char *array[]= { "a", "123456789_", "abcdefghijklmnopqrstuvwxyz",
		"_9876543210__9876543210_9876543210", 
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"99999999999999999999999999999999999999999999999999999999999999999999999",
    "abcdefgh_1_ijkomno_2_pqrstuv_3_xyzABCD_4_EFGHIJK_5_LMNOPQR_6_STUVWXY_7_Zabcdef_8_ghijkom_9"
			   };
/* # line 2522 "trl.sc" */	/* savepoint */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"savepoint tst26save");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2524 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 26, pass %d, Insert into table data10 \n",
        s, c);
    SIflush (stdout);
  }
/*	  for (i=1; i <= 5; i++)		*/
/*	  {					*/
    i = s * c * 23;
    element = ((i*s) % 7);
    STcopy (array[element], array_buffer);
    if ( element == 0 ) { element = 7; }
/* # line 2537 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl20",7582,24244);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&element);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl20",7582,24244);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data10(i2, i2_1, i2_2, i2_3, f4, f8, dollar, day, day_1, \
day_2, c90, txt1, txt2, txt3, text)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)"/");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,30,4,&element);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $11=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $12=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)
", '15-Apr-1991', '16-Apr-1991', '17-Apr-1991', ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $13=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $14=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $15=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $16=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $17=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl20",7582,24244);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2544 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2546 "trl.sc" */	/* host code */
    if (ing_err != 0)
    {
/* # line 2548 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2549 "trl.sc" */	/* host code */
      return;
    }
    if (traceflag)
    {
      SIprintf ("\t\t inserting into data10 numeric %d and array element %d \n",
	i, element);
      SIflush (stdout);
    }
/*	  }					*/
}
/* 
** Function tst27 
** Purpose:	Test the index compressions with concurrency where the key 
**		ranges from very compressable, to keys where the compression 
**		would consist of no compression at all.  There would be four 
**		runs of this test with the first run on a old format 
**		un-compressed btree table, the second will be on a new 
**		compressed btree table with data and index compressions, 
**		third on a btree with index compression only, and the last 
**		depending on time, will be btree with data compression only.
**
**		The function which would be run 60 (?) times by each slave 
**		would follow the following pseudo code:
**
**		for (i=0; i <= repetition number; i++)
**		{
**		    insert into table row where numeric are (slave number ** i + i)
**			and character field are copies of predetermined strings
**			in array[ ((slave number *i) mod array size) ]
**		}
**
**		The table would be along the following lines of thought::
**	Column Name                      Type		     Length
**						     external	    internal
**	lt_name				vchar		50		53
**	ft_name				char		50		51
**	mid_init			c1		 1		 1
**	emp_num				integer		 2		 5
**	emp_titl			text		50		53
**	st_date 			date		12		13
**       -----------------------------------------------------------------
**	text                             vchar         100	       103
**
**
** Parameters: 
**          s = slave number. 
**	    c = number of repetitions.
** Returns: 
*/ 
tst27(s,c)
  int s;
  int c;
{
  int i;
  int j;
  int k;
  int element;
  char First_Name[51];
  char Last_Name[51];
  char Middle_Init[2];
  int Emp_Num;
  char Emp_Title[51];
  static char *f_name[]= { " ", "a", "123456789_", 
		"abcdefghijklmnopqrstuvwxyz",
		"_9876543210__9876543210_9876543210", 
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
		"Steven", "John", "Donald", "Krzysztof", "Hamilton", 
		"Jerry", "Saundra", "Steve", "Kelly", "Richard", "Roy",
		"Jennifer", "Claire"
			   };
  static char *l_name[]= { " ", "b", "b23456789_", 
		"bcdefghijklmnopqrstuvwxyza",
		"b9876543210__9876543210_9876543210", 
		"cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"Pennebaker", "Simutis", "Johnson", "Stec", "Whitener",
		"Dufour", "Flaggs", "Doan", "Park", "Lamberty", "Tessler",
		"Johnson", "Leachman", "Brussels"
			   };
  static char *titles[]= { " ", "c", "123456789_", 
		"cdefghijklmnopqrstuvwxyzab",
		"c9876543210__9876543210_9876543210", 
		"cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"student", "Member of Technical Staff", "Manager", 
		"Director", "Vice President", "Exective Vice President",
		"CEO"
			   };
  static char *middle[]= { "A", "B", "C", "D", "E", "F", "G", "H", "I",
                            "J", "K", "L", "M", "N", "O", "P", "Q", "R",
                            "S", "T", "U", "V", "W", "X", "Y", "Z", " "
                           };
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 27, pass %d, Insert into table data21 \n",
        s, c);
    SIflush (stdout);
  }
/* # line 2659 "trl.sc" */	/* savepoint */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"savepoint tst27save");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2661 "trl.sc" */	/* host code */
  for (i=1, j=c, k=s; i <= 5; i++, j++, k++)
  {
    element = ((i*s) % 19);
    STcopy (f_name[element], First_Name);
    if ( element == 0 ) { element = 19; }
    element = ((j*s) % 20);
    STcopy (l_name[element], Last_Name);
    if ( element == 0 ) { element = 20; }
    element = (( i*j*k ) % 27);
    STcopy (middle[element], Middle_Init);
    if ( element == 0 ) { element = 27; }
    Emp_Num = (i*j*k % 32761);
    element = ((k*s) % 13);
    STcopy (titles[element], Emp_Title);
    if ( element == 0 ) { element = 13; }
    if (traceflag)
    {
      SIprintf ("\t about to inserting into DATA11 numeric %d and element %d \n",
	i, element);
      SIflush (stdout);
    }
/* # line 2684 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl21",7583,18896);
      IIputdomio((short *)0,1,32,0,Last_Name);
      IIputdomio((short *)0,1,32,0,First_Name);
      IIputdomio((short *)0,1,32,0,Middle_Init);
      IIputdomio((short *)0,1,30,4,&Emp_Num);
      IIputdomio((short *)0,1,32,0,Emp_Title);
      IIputdomio((short *)0,1,32,0,Last_Name);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl21",7583,18896);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into DATA11(lt_name, ft_name, mid_init, emp_num, emp_titl, st_\
date, text)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,Last_Name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,First_Name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,Middle_Init);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&Emp_Num);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,Emp_Title);
        IIwritio(0,(short *)0,1,32,0,(char *)", '15-Apr-1991', ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,Last_Name);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl21",7583,18896);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2689 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2690 "trl.sc" */	/* host code */
    if (ing_err != 0)
    {
/* # line 2692 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2693 "trl.sc" */	/* host code */
      return;
    }
    if (traceflag)
    {
      SIprintf ("\t done inserting into DATA11 numeric %d and array element %d \n",
	i, element);
      SIprintf ("\t\t row = %s, %s %s - %d - %s \n",
	Last_Name, First_Name, Middle_Init, Emp_Num, Emp_Title);
      SIflush (stdout);
    }
  }
}
/*
** Function tst28
** Purpose:	Test the new btree compressed index's ability to deal with 
**		numerous overflow pages.  The test would create row with 
**		similiar keys.  Again, there would be four runs of this test 
**		like the test above.  The number of tid's in an overflow page 
**		is 250+, so the number of times each slave need to run to 
**		create an overflow needs to be larger than 250.
**
**		The function which would be run 600 (?) times by each slave 
**		would follow the following pseudo code:
**
**	for (i=0; i <= repetition number; i++)
**	{
**		insert into table row where numeric are (i)
**			and character field are copies of predetermined strings
**			in array[ i mod array size ]
**	}
**
**		The array elements and the table used would be similiar to 
**		those defined in test number 1.  At the end of the test with 
**		60 repetition, keys with numeric value equal to 1 would have 
**		600 duplicate rows, the key with numeric value equal to 2 
**		would have 599 duplicate rows, etc.
**
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst28(s,c)
  int s;
  int c;
{
  int i;
  int element;
  char Last_Name[51];
  char First_Name[51];
  char Middle_Init[2];
  int Emp_Num;
  char Emp_Title[51];
  char array_buffer[100];
  static char key_buffer[30]= "tst28 string field data";
static	char	*l_name[] = { "Pennebaker", "Simutis", "Johnson", "Stec", 
		"Whitener", "Dufour", "Flaggs", "Doan", "Park", "Lamberty", 
		"Tessler", "Johnson", "Leachman"
			   };
static	char	*f_name[] = { "Steven", "John", "Donald", "Krzysztof", 
		"Hamilton", "Jerry", "Saundra", "Steve", "Kelly", "Richard", 
		"Roy", "Jennifer", "Claire"
			   };
static	char	*titles[] = { "Supervisor", "Member of Technical Staff", "Manager", 
		"Director", "Vice President", "Exective Vice President",
		"CEO"
			   };
static	char    *middle[]= { "A", "B", "C", "D", "E", "F", "G", "H", "I",
                            "J", "K", "L", "M"
                           };
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 28, pass %d, Insert into table data10 \n",
        s, c);
    SIflush (stdout);
  }
  STprintf(array_buffer, "tst28 non_key data for each row, slave %d, reps %d, loop_counter %d",
                s, c, i);
  /*
  ** Increase maxlocks to avoid deadlock when escalating to table level lock.
  */
/* # line 2790 "trl.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set LOCKMODE session where MAXLOCKS=35");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2792 "trl.sc" */	/* savepoint */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"savepoint tst28save");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2794 "trl.sc" */	/* host code */
  for (i=1; i <= c+1; i++)
  {
    element = (s % 13);
    STcopy (l_name[element], Last_Name);
    STcopy (f_name[element], First_Name);
    STcopy (middle[element], Middle_Init);
    if ( element == 0 ) { element = 13; }
    Emp_Num = (s % 13);
    if (element > 7) element = element % 7;
    STcopy (titles[element], Emp_Title);
/* # line 2805 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl22",7584,29110);
      IIputdomio((short *)0,1,32,0,Last_Name);
      IIputdomio((short *)0,1,32,0,First_Name);
      IIputdomio((short *)0,1,32,0,Middle_Init);
      IIputdomio((short *)0,1,30,4,&Emp_Num);
      IIputdomio((short *)0,1,32,0,Emp_Title);
      IIputdomio((short *)0,1,32,0,key_buffer);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl22",7584,29110);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into DATA11(lt_name, ft_name, mid_init, emp_num, emp_titl, st_\
date, text)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,Last_Name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,First_Name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,Middle_Init);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&Emp_Num);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,Emp_Title);
        IIwritio(0,(short *)0,1,32,0,(char *)", '15-Apr-1991', ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,key_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl22",7584,29110);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2810 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2811 "trl.sc" */	/* host code */
    if (ing_err != 0)
    {
/* # line 2813 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2814 "trl.sc" */	/* host code */
      return;
    }
    if (traceflag)
    {
      SIprintf ("\t\t inserting into data11 similiar row with key value %d \n",
	i );
      SIflush (stdout);
    }
  }
}
/*
** Function tst29
** Purpose:	Test the new btree compressed index's ability to deal with 
**	page splits.  The test will create rows with key values that will 
**	increment with values that leaves gaps for future insertations.  An 
**	example would be slave number 1 will insert rows 1, 6, 11, and 16; 
**	slave number 2 would insert rows 2, 7, 12, and 17; slave number 3 
**	would insert rows 3, 8, 13, and 18.  There would be 5 slave and in the 
**	end, the random insertions should fill in all the rows, and the thought 
**	of limiting ? number of keys on a page would cause the pages to split.  
**	Another factor that can be used is the fill factor values. 
**
**	The function which would be run 60 times by each slave would follow 
**	the following pseudo code:
**
**	for (i=0; i <= repetition number; i++)
**	{
**		insert into table row where numeric are (i) except for the 
**			first key value which would be (slave number * i * 5)
**			and character field is a copy of a predetermined string
**	}
**	
**	The table used would be very similiar to the table defined in test 
**	number 25.
**
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst29(s,c)
  int s;
  int c;
{
  int i;
  int j;
  int k;
  int insert_value;
  int element;
  char First_Name[51];
  char Last_Name[51];
  char Middle_Init[2];
  int Emp_Num;
  char Emp_Title[51];
  char array_buffer[100];
  static char key_buffer[30]= "tst29 string field data";
static	char	*f_name[] = { " ", "a", "123456789_", 
		"abcdefghijklmnopqrstuvwxyz",
		"_9876543210__9876543210_9876543210", 
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
		"Steven", "John", "Donald", "Krzysztof", "Hamilton", 
		"Jerry", "Saundra", "Steve", "Kelly", "Richard", "Roy",
		"Jennifer", "Claire"
			   };
static	char	*l_name[] = { " ", "b", "b23456789_", 
		"bcdefghijklmnopqrstuvwxyza",
		"b9876543210__9876543210_9876543210", 
		"cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"Pennebaker", "Simutis", "Johnson", "Stec", "Whitener",
		"Dufour", "Flaggs", "Doan", "Park", "Lamberty", "Tessler",
		"Johnson", "Leachman", "Brussels"
			   };
static	char	*titles[] = { " ", "c", "123456789_", 
		"cdefghijklmnopqrstuvwxyzab",
		"c9876543210__9876543210_9876543210", 
		"cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"student", "Member of Technical Staff", "Manager", 
		"Director", "Vice President", "Exective Vice President",
		"CEO"
			   };
static	char    *middle[]= { "A", "B", "C", "D", "E", "F", "G", "H", "I",
                            "J", "K", "L", "M", "N", "O", "P", "Q", "R",
                            "S", "T", "U", "V", "W", "X", "Y", "Z", " "
                           };
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 29, pass %d, Insert into table data11 \n",
        s, c);
    SIflush (stdout);
  }
  STprintf(array_buffer, "tst29 non_key data for each row, slave %d, reps %d, loop_counter %d",
                s, c, i);
/* # line 2921 "trl.sc" */	/* savepoint */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"savepoint tst29save");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2923 "trl.sc" */	/* host code */
  for (i=1, j=c, k=s; i <= c+1; i++, j++, k++)
  {
    Emp_Num = (i*s*5);
    element = ((i*s) % 19);
    STcopy (f_name[element], First_Name);
    if ( element == 0 ) { element = 19; }
    element = ((j*s) % 20);
    STcopy (l_name[element], Last_Name);
    if ( element == 0 ) { element = 20; }
    element = (( i*j*k ) % 27);
    STcopy (middle[element], Middle_Init);
    if ( element == 0 ) { element = 27; }
    element = ((k*s) % 13);
    STcopy (titles[element], Emp_Title);
    if ( element == 0 ) { element = 13; }
/* # line 2939 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl23",7585,25818);
      IIputdomio((short *)0,1,32,0,Last_Name);
      IIputdomio((short *)0,1,32,0,First_Name);
      IIputdomio((short *)0,1,32,0,Middle_Init);
      IIputdomio((short *)0,1,30,4,&Emp_Num);
      IIputdomio((short *)0,1,32,0,Emp_Title);
      IIputdomio((short *)0,1,32,0,key_buffer);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl23",7585,25818);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into DATA11(lt_name, ft_name, mid_init, emp_num, emp_titl, st_\
date, text)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,Last_Name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,First_Name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,Middle_Init);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&Emp_Num);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,Emp_Title);
        IIwritio(0,(short *)0,1,32,0,(char *)", '15-Apr-1991', ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,key_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl23",7585,25818);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2944 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 2945 "trl.sc" */	/* host code */
    if (ing_err != 0)
    {
/* # line 2947 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 2948 "trl.sc" */	/* host code */
      return;
    }
    if (traceflag)
    {
      SIprintf ("\t\t inserting into data11 similiar row with key value %d \n",
	i );
      SIflush (stdout);
    }
  }
}
/*
** Function tst30
** Purpose:	Test the Update function with the new btree.  The reason for 
**	the special test is that Update internally, is two functions.  
**	Internally, to update a row, the system must first Delete the row, then 
**	it will re-insert the row with the new values.  
**
**	The function which would be run 60 times by each slave would follow 
**	the following pseudo code: 
**
**	for (i=0; i <= repetition number; i++)
**	{
**		switch( ((i * slave number) MOD 3) )
**		case 0:
**		{
**			update rows where key < lower range 
**				key = key * 3
**		}
**		case 1:
**		{
**			update rows where lower range < key < upper range
**				key = key * 4 - 2047
**		}
**		case 2:
**		{
**			update rows where key > upper range 
**				key = key / 2
**		}
**		default:
**		{
**			update rows everything 
**				key = key + 4095
**		}
**	}
**
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst30(s,c)
  int s;
  int c;
{
# define range_1 392
# define range_2 1536
# define range_3 3700
# define range_4 7260
# define range_5 12717
# define range_6 20748
# define range_7 32758
  int i;
  int j;
  int key;
  int row_count;
  int sleep_value;
  char First_Name[51];
  char Last_Name[51];
  char Middle_Init[2];
  int Emp_Num;
  char Emp_Title[51];
  char array_buffer[100];
  static char key_buffer[30]= "tst30 string field data";
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 30, pass %d, Insert into table data11 \n",
        s, c);
    SIflush (stdout);
  }
/* # line 3041 "trl.sc" */	/* savepoint */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"savepoint tst30save");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3043 "trl.sc" */	/* host code */
    sleep_value = (s * s) * 100; 
    PCsleep( (u_i4)sleep_value);
    STcopy (array_buffer, " ");
    j = (((c * 11) * (s * 13)) % 7);
    switch( j )
    {
      case 0:
      {
	STprintf(array_buffer, "tst30 text data for case 0, slave %d",
                s);
/* # line 3054 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl24",7586,8397);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,30,sizeof(range_1),(void *)IILQint(range_1));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl24",7586,8397);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data11 set Emp_Num=Emp_Num*20 +400, text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)"where Emp_Num<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(range_1),(void *)IILQint(range_1));
        IIexDefine(0,(char *)"trl24",7586,8397);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3058 "trl.sc" */	/* host code */
	break;
      }
      case 1:
      {
	STprintf(array_buffer, "tst30 text data for case 1, slave %d",
                s);
/* # line 3064 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl25",7587,9898);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,30,sizeof(range_1),(void *)IILQint(range_1));
      IIputdomio((short *)0,1,30,sizeof(range_2),(void *)IILQint(range_2));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl25",7587,9898);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data11 set Emp_Num=Emp_Num*4 -50, text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)"where Emp_Num>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(range_1),(void *)IILQint(range_1));
        IIwritio(0,(short *)0,1,32,0,(char *)"and Emp_Num<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(range_2),(void *)IILQint(range_2));
        IIexDefine(0,(char *)"trl25",7587,9898);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3068 "trl.sc" */	/* host code */
	break;
      }
      case 2:
      {
	STprintf(array_buffer, "tst30 text data for case 2, slave %d",
                s);
/* # line 3074 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl26",7588,11475);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,30,sizeof(range_2),(void *)IILQint(range_2));
      IIputdomio((short *)0,1,30,sizeof(range_3),(void *)IILQint(range_3));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl26",7588,11475);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data11 set Emp_Num=Emp_Num*9 +1024, text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)"where Emp_Num>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(range_2),(void *)IILQint(range_2));
        IIwritio(0,(short *)0,1,32,0,(char *)"and Emp_Num<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(range_3),(void *)IILQint(range_3));
        IIexDefine(0,(char *)"trl26",7588,11475);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3078 "trl.sc" */	/* host code */
	break;
      }
      case 3:
      {
	STprintf(array_buffer, "tst30 text data for case 3, slave %d",
                s);
/* # line 3084 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl27",7589,10958);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,30,sizeof(range_3),(void *)IILQint(range_3));
      IIputdomio((short *)0,1,30,sizeof(range_4),(void *)IILQint(range_4));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl27",7589,10958);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data11 set Emp_Num=Emp_Num/10 -100, text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)"where Emp_Num>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(range_3),(void *)IILQint(range_3));
        IIwritio(0,(short *)0,1,32,0,(char *)"and Emp_Num<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(range_4),(void *)IILQint(range_4));
        IIexDefine(0,(char *)"trl27",7589,10958);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3088 "trl.sc" */	/* host code */
	break;
      }
      case 4:
      {
	STprintf(array_buffer, "tst30 text data for case 4, slave %d",
                s);
/* # line 3094 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl28",7590,14510);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,30,sizeof(range_4),(void *)IILQint(range_4));
      IIputdomio((short *)0,1,30,sizeof(range_5),(void *)IILQint(range_5));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl28",7590,14510);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data11 set Emp_Num=Emp_Num +8507, text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)"where Emp_Num>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(range_4),(void *)IILQint(range_4));
        IIwritio(0,(short *)0,1,32,0,(char *)"and Emp_Num<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(range_5),(void *)IILQint(range_5));
        IIexDefine(0,(char *)"trl28",7590,14510);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3098 "trl.sc" */	/* host code */
	break;
      }
      case 5:
      {
	STprintf(array_buffer, "tst30 text data for case 2, slave %d",
                s);
/* # line 3104 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl29",7591,16300);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,30,sizeof(range_5),(void *)IILQint(range_5));
      IIputdomio((short *)0,1,30,sizeof(range_6),(void *)IILQint(range_6));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl29",7591,16300);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data11 set Emp_Num=Emp_Num/9 -1024, text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)"where Emp_Num>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(range_5),(void *)IILQint(range_5));
        IIwritio(0,(short *)0,1,32,0,(char *)"and Emp_Num<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(range_6),(void *)IILQint(range_6));
        IIexDefine(0,(char *)"trl29",7591,16300);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3108 "trl.sc" */	/* host code */
	break;
      }
      case 6:
      {
	STprintf(array_buffer, "tst30 text data for case 2, slave %d",
                s);
/* # line 3114 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl30",7592,6574);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,30,sizeof(range_6),(void *)IILQint(range_6));
      IIputdomio((short *)0,1,30,sizeof(range_7),(void *)IILQint(range_7));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl30",7592,6574);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data11 set Emp_Num=Emp_Num +5000, text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)"where Emp_Num>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(range_6),(void *)IILQint(range_6));
        IIwritio(0,(short *)0,1,32,0,(char *)"and Emp_Num<");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(range_7),(void *)IILQint(range_7));
        IIexDefine(0,(char *)"trl30",7592,6574);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3118 "trl.sc" */	/* host code */
	break;
      }
      default:
      {
	STprintf(array_buffer, "tst30 text data for case default, slave %d",
                s);
/* # line 3124 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl31",7593,6096);
      IIputdomio((short *)0,1,32,0,array_buffer);
      IIputdomio((short *)0,1,30,sizeof(range_7),(void *)IILQint(range_7));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl31",7593,6096);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update data11 set Emp_Num=Emp_Num/62*5 -255, text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,array_buffer);
        IIwritio(0,(short *)0,1,32,0,(char *)"where Emp_Num>");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(range_7),(void *)IILQint(range_7));
        IIexDefine(0,(char *)"trl31",7593,6096);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3128 "trl.sc" */	/* host code */
	break;
      }
    }
/* # line 3132 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&row_count,8);
  }
/* # line 3133 "trl.sc" */	/* host code */
    if (ing_err != 0)
    {
/* # line 3135 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3136 "trl.sc" */	/* host code */
      return;
    }
    if (traceflag)
    {
      SIprintf ("\t\t inserting into data11 similiar row with key value %d \n",
	key);
      SIflush (stdout);
    }
}
/*
** Function TST31
** Purpose: Rollback test with btree inserts.  The test will insert various
**		rows into the data2 table.  The rows that will be inserted are 
**		determined by a calculated value.  The calculated value is 
**		the number of slaves times the iteration number, plus the 
**		slave number.  After the insertions are done, the same 
**		calculated value is used again to determine if a rollback will 
**		be done.  If the calculated value is even, a rollback will be 
**		performed to take out the inserted rows.  The insertions are 
**		committed if the value is odd.  
**		
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst31(s,c)
  int s;
  int c;
{
  int value;
  int num = 0;
/* # line 3179 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select num_kids from num_sla\
ves");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3182 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3184 "trl.sc" */	/* host code */
  value = s + (num * c);
  if (traceflag)
  {
	SIprintf ("Running SLAVE %d: Rout 31, pass %d, inserting %d \n",s,c, value);
	SIflush (stdout);
  }
/* # line 3191 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl32",7594,2247);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl32",7594,2247);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)
", '10-dec-1989', 'x1-1xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'yyyyyyy\
yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl32",7594,2247);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3197 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3198 "trl.sc" */	/* host code */
  if (ing_err  != 0)
  {
	return;
  }
/* # line 3203 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl33",7595,14276);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl33",7595,14276);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)
" +1, '11-dec-1989', 'x2-2xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'yyyy\
yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl33",7595,14276);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3209 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3210 "trl.sc" */	/* host code */
  if (ing_err  != 0)
  {
	return;
  }
/* # line 3215 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl34",7596,15822);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl34",7596,15822);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)
" +2, '12-dec-1989', 'x3-3xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'yyyy\
yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl34",7596,15822);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3221 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3222 "trl.sc" */	/* host code */
  if (ing_err  != 0)
  {
	return;
  }
/* # line 3227 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl35",7597,13480);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl35",7597,13480);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)
" +3, '13-dec-1989', 'x4-4xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'yyyy\
yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl35",7597,13480);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3233 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3234 "trl.sc" */	/* host code */
  if (ing_err  != 0)
  {
	return;
  }
/* # line 3239 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl36",7598,12967);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIputdomio((short *)0,1,30,4,&c);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&value);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl36",7598,12967);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data2(i1, i2, i4, f4, f8, date, char61a, char100)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)"*");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&value);
        IIwritio(0,(short *)0,1,32,0,(char *)
" +4, '14-dec-1989', 'x5-5xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 'yyyy\
yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy')");
        IIexDefine(0,(char *)"trl36",7598,12967);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3246 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3247 "trl.sc" */	/* host code */
  if (ing_err  != 0)
  {
	return;
  }
  if ((value % 2) == 0)
  {
/* # line 3254 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3255 "trl.sc" */	/* host code */
  }
}
/*
** Function TST32
** Purpose: Rollback test with btree deletes.  The test will delete various
**		rows from the data2 table.  The rows that will be deleted is 
**		determined by a calculated value.  The calculated value is 
**		the number of slaves times the iteration number, plus the 
**		slave number.  After the delete is done, the same calculated 
**		value is used again to determine if a rollback will be done.  
**		If the calculated value is even, a rollback will be performed 
**		to restore the deleted rows.  The deletions are committed 
**		if the value is odd.  
**		
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst32(s,c)
  int s;
  int c;
{
  int value;
  int num = 0;
  int temp;
/* # line 3289 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select num_kids from num_sla\
ves");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3292 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3294 "trl.sc" */	/* host code */
  value = s * (num * c);
  temp = value % 3;
  if (traceflag)
  {
	SIprintf ("Running SLAVE %d: Rout 32, pass %d, inserting %d \n",s,c, value);
	SIflush (stdout);
  }
  switch (temp)
  {
    case 0: 
    {
	temp = value % 127;
/* # line 3308 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from data2 where i1=");
    IIputdomio((short *)0,1,30,4,&temp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i2<0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3310 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3311 "trl.sc" */	/* host code */
	if (ing_err  != 0)
	{
		return;
	}
	break;
    }
    case 1: 
    {
	temp = value % 127;
/* # line 3321 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from data2 where i1=");
    IIputdomio((short *)0,1,30,4,&temp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i2>0 and i2<10000");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3323 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3324 "trl.sc" */	/* host code */
	if (ing_err  != 0)
	{
		return;
	}
	break;
    }
    default: 
    {
	temp = value % 127;
/* # line 3334 "trl.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from data2 where i1=");
    IIputdomio((short *)0,1,30,4,&temp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i2>10000");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3336 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3337 "trl.sc" */	/* host code */
	if (ing_err  != 0)
	{
		return;
	}
	break;
    }
  }
  if ((value % 2) == 0)
  {
/* # line 3348 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3349 "trl.sc" */	/* host code */
  }
}
/*
** Function TST33
** Purpose: Rollback test with btree updates.  The test will update various
**		rows from the data2 table.  The rows that will be updated is 
**		determined by a calculated value.  The calculated value is 
**		the number of slaves times the iteration number, plus the 
**		slave number.  After the updates are done, the same calculated 
**		value is used again to determine if a rollback will be done.  
**		If the calculated value is even, a rollback will be performed 
**		to restore the updated rows.  The updates are committed 
**		if the value is odd.  
**		
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst33(s,c)
  int s;
  int c;
{
  int value;
  int num = 0;
  int temp;
/* # line 3384 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select num_kids from num_sla\
ves");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&num);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3387 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3389 "trl.sc" */	/* host code */
  value = s * (num * c);
  temp = value % 3;
  if (traceflag)
  {
	SIprintf ("Running SLAVE %d: Rout 33, pass %d, inserting %d \n",s,c, value);
	SIflush (stdout);
  }
  switch (temp)
  {
    case 0: 
    {
	temp = value % 127;
/* # line 3403 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data2 set i2=i2 +1, date='03-jan-1992' where i1=");
    IIputdomio((short *)0,1,30,4,&temp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i2<0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3406 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3407 "trl.sc" */	/* host code */
	if (ing_err  != 0)
	{
		return;
	}
	break;
    }
    case 1: 
    {
	temp = value % 127;
/* # line 3417 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data2 set i2=i2 +2, date='04-jan-1992' where i1=");
    IIputdomio((short *)0,1,30,4,&temp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i2<0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3420 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3421 "trl.sc" */	/* host code */
	if (ing_err  != 0)
	{
		return;
	}
	break;
    }
    default: 
    {
	temp = value % 127;
/* # line 3431 "trl.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update data2 set i2=i2 +3, date='05-jan-1992' where i1=");
    IIputdomio((short *)0,1,30,4,&temp);
    IIwritio(0,(short *)0,1,32,0,(char *)"and i2<0");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3434 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3435 "trl.sc" */	/* host code */
	if (ing_err  != 0)
	{
		return;
	}
	break;
    }
  }
  if ((value % 2) == 0)
  {
/* # line 3446 "trl.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3447 "trl.sc" */	/* host code */
  }
}
/*
** Function TST126
** Purpose:	This routine performs a TP1 transaction in which the relations 
**		account, teller branch and history get updated in a MQT. It
**		uses a seed which should be put in the seeds table before 
**		running the test. 
** Parameters:
**          s = slave number.
**	    c = number of repetitions.
** Returns:
*/
tst126(s,c)
  int s;
  int c;
{
  int seed;
  int scale;
  int account_no;
  int teller_no;
  int branch_no;
  double v_amount;
  int row_cnt;
  int rnum;
	i4	type;
/* # line 3483 "trl.sc" */	/* select */
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
      CTS_errors();
  }
/* # line 3484 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select scale_val from tp1_sc\
ale");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&scale);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3486 "trl.sc" */	/* host code */
  if (traceflag)
  {
    SIprintf ("Running SLAVE %d: Rout 126 (TP1), pass %d, seed %d \n",
        s, c, seed);
    SIflush (stdout);
  }
  MHsrand (seed + (s * c));
  rnum = MHrand () / 10000;
  teller_no = rnum * TELLERS_PER_BRANCH + scale + 1;
  if (rnum * 100 > 85)
    branch_no = (i4)((rnum * scale * BRANCHS_PER_BANK + 1));
  else 
    branch_no = (teller_no - 1) / BRANCHS_PER_BANK + 1;
  account_no = (branch_no + rnum * ACCOUNTS_PER_BRANCH) / 100 + (s * c);
  if (account_no > 1000000)
    account_no = account_no / 10 + (s * c);
  else if (account_no < 100000)
    account_no = account_no * 10 + (s * c);
  type = rnum * 2 + 1;
  if (type == 2)
    v_amount = -.01;
  else
    v_amount = 1.0;
			/*  Change account record  */
/* # line 3516 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl37",7599,1481);
      IIputdomio((short *)0,1,31,8,&v_amount);
      IIputdomio((short *)0,1,30,4,&account_no);
      IIputdomio((short *)0,1,31,8,&v_amount);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl37",7599,1481);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update account set sequence=sequence +1, balance=balance -");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,31,8,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)"where number=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&account_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(balance -");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,31,8,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)")>=0");
        IIexDefine(0,(char *)"trl37",7599,1481);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3519 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&row_cnt,8);
  }
/* # line 3520 "trl.sc" */	/* host code */
  if ( traceflag )
  {
    SIprintf ("\t\tchange account record, account_no = %d rowcount = %d \n",
	account_no, row_cnt );
    SIflush (stdout);
  }
  if (ing_err != 0)
  {
    return;
  }
			/*  Change teller record  */
/* # line 3533 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl38",7600,965);
      IIputdomio((short *)0,1,31,8,&v_amount);
      IIputdomio((short *)0,1,30,4,&teller_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl38",7600,965);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update teller set balance=balance +");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,31,8,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)"where number=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&teller_no);
        IIexDefine(0,(char *)"trl38",7600,965);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3536 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&row_cnt,8);
  }
/* # line 3537 "trl.sc" */	/* host code */
  if ( traceflag )
  {
    SIprintf ("\t\tchange teller record, teller_no = %d rowcount = %d \n",
	teller_no, row_cnt );
    SIflush (stdout);
  }
  if (ing_err != 0)
  {
    return;
  }
			/*  Change branch record  */
/* # line 3550 "trl.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl39",7601,714);
      IIputdomio((short *)0,1,31,8,&v_amount);
      IIputdomio((short *)0,1,30,4,&branch_no);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl39",7601,714);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update branch set balance=balance +");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,31,8,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)"where number=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&branch_no);
        IIexDefine(0,(char *)"trl39",7601,714);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3553 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&row_cnt,8);
  }
/* # line 3554 "trl.sc" */	/* host code */
  if ( traceflag )
  {
    SIprintf ("\t\tchange BRANCH record, branch_no = %d rowcount = %d \n",
	branch_no, row_cnt );
    SIflush (stdout);
  }
  if (ing_err != 0)
  {
    return;
  }
			/*  Insert an history record  */
/* # line 3567 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl40",7602,9411);
      IIputdomio((short *)0,1,30,4,&account_no);
      IIputdomio((short *)0,1,30,4,&branch_no);
      IIputdomio((short *)0,1,30,4,&teller_no);
      IIputdomio((short *)0,1,31,8,&v_amount);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl40",7602,9411);
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
        IIputdomio((short *)0,1,31,8,&v_amount);
        IIwritio(0,(short *)0,1,32,0,(char *)", '0123456789')");
        IIexDefine(0,(char *)"trl40",7602,9411);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3570 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
    IILQisInqSqlio((short *)0,1,30,4,&row_cnt,8);
  }
/* # line 3571 "trl.sc" */	/* host code */
  if ( traceflag )
  {
    SIprintf ("\t\tinserted history record, amount = %d rowcount = %d \n",
	v_amount, row_cnt );
    SIflush (stdout);
  }
  if (ing_err != 0)
  {
    return;
  }
}
/*
** Function TST34
** Purpose: Inserts one row in ascending order. The inserted value will be
**          generated from the sequence object. Multiple slaves will be 
**          responsible for increasing the value of a single sequence object 
**          and inserting that value into the table.
** Parameters:
**          s = slave number.
**          c = number of repetitions.
** Returns:
**
*/
tst34(s,c)
  int s;
  int c;
{
  int inserting = 0;
	int slaveSleep = (s * c) * 100;
        PCsleep((u_i4)slaveSleep);
/* # line 3610 "trl.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select tst34seq.nextval");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&inserting);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3611 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3612 "trl.sc" */	/* host code */
        if(ing_err != 0) { return; }
        if (traceflag)
        {
        SIprintf ("Running SLAVE %d: Rout 34, pass %d,inserting %d \n",s,c,inserting);
        SIflush (stdout);
        }
/* # line 3620 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl41",7603,24232);
      IIputdomio((short *)0,1,30,4,&inserting);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl41",7603,24232);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data1(tbl_key, rout, slave, cnt)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&inserting);
        IIwritio(0,(short *)0,1,32,0,(char *)", 34, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl41",7603,24232);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3622 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3623 "trl.sc" */	/* host code */
        if (ing_err != 0) { return; }
}
/*
** Function TST35
** Purpose: Multiple slaves will increase or decrease the value of 
**          multiple sequence objects, then each slave will insert its
**          value into the table data1. This routine will check a subset 
**          of options used with sequence objects. 
** Parameters:
**          s = slave number.
**          c = number of repetitions.
** Returns:
**
*/
tst35(s,c)
  int s;
  int c;
{
  int inserting;
  char prepare_buff[250];
	        int slaveSleep = (s * c )* 100;
                PCsleep((u_i4)slaveSleep);
                STprintf(prepare_buff,"select tst35seq%d.nextval",s);
/* # line 3654 "trl.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"stmt",(char *)0,0,prepare_buff);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3656 "trl.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"seq_cur",7604,19873);
    IIwritio(0,(short *)0,1,32,0,(char *)"stmt");
    IIcsQuery((char *)"seq_cur",7604,19873);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3657 "trl.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"seq_cur",7604,19873,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&inserting);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3658 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3659 "trl.sc" */	/* host code */
                if (ing_err != 0) { return; }
/* # line 3660 "trl.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"seq_cur",7604,19873);
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3662 "trl.sc" */	/* host code */
if (traceflag){
SIprintf ("Running SLAVE %d: Rout 35,pass %d, inserting %d \n", s,c,inserting);} 
/* # line 3664 "trl.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"trl42",7605,26781);
      IIputdomio((short *)0,1,30,4,&inserting);
      IIputdomio((short *)0,1,30,4,&s);
      IIputdomio((short *)0,1,30,4,&c);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"trl42",7605,26781);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into data1(tbl_key, rout, slave, cnt)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&inserting);
        IIwritio(0,(short *)0,1,32,0,(char *)", 35, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&s);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&c);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"trl42",7605,26781);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (sqlca.sqlcode < 0) 
      CTS_errors();
  }
/* # line 3667 "trl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ing_err,0);
  }
/* # line 3668 "trl.sc" */	/* host code */
           if (ing_err != 0) { return; }
}
