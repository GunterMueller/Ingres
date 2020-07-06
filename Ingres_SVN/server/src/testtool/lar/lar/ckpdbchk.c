# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
#include    <compat.h>
#include    <gl.h>
#include    <iicommon.h>
#include    <pc.h>
#include    <mh.h>
#include    <tr.h>
#include    <tm.h>
/*
**  CKPDB1_CHECK - used for online checkpoint tests - checks state of database
**	for consistency.
**
**	This program checks the specified database for consistency following
**	execution of the online checkpoint tests.
**
**	This program can be run either previous to the test, after completion
**	of the tests - or on any consistent version of the database mid-test.
**
**	The purpose of the program is not to verify the results of the test, but
**	to verify the consistency of the database.  This program is run after
**	restoring the database to a checkpoint created during execution of the
**	checkpoint tests.  Since we cannot predict the state of the database
**	at the time the checkpoint started (it is somewhat random) - we cannot
**	compare the restored database to any CANON version - all we can do is
**	verify that it is consistent.  The checkpoint tests use a different
**	method to verify the data is correct after using the journal files to
**	restore the database from the checkpoint.
**
**	Consistency checks which can be made in this program:
**
**	    If table's should always have a constant number of rows, then
**		the rowcount of those tables can be checked.  For example,
**		if a table starts with 1000 rows and each transaction which
**		acts on that table either replaces or does a delete/append in
**		the same transaction, then any consistent view of the table
**		should have 1000 rows.
**
**	    If a test writes duplicate data in the each transaction, then we
**		can check to see that the data is correctly duplicated.  For
**		example, if a test has two identical tables, and all operations
**		are performed to each table in the same transaction, then in any
**		consistent view the tables should remain identical.
**
**	    If a test keeps history information of changes made (i.e. TP1), then
**		the data can be checked against the history for consistency.
**
**	When building new tests, consider adding duplicate tables or history
**	information so checks can be added here for consistency.
**
**  CKPDB_CHECK dbname [options]
**
**  Required arguments
**	dbname	        - database name
**
**  Optional arguments
**	none
**
**  History
**	13-aug-1991 (lauraw)
**		Filename changed to conform to standard
**		(changed from ckpdb1_check.qc to ckpdbchk.qc)
**	9-Aug-1993 (fredv)
**		Include <gl.h> and <iicommon.h> so that it will be compiled.
**	29-sep-1993 (dianeh)
**		Remove #include of <lg.h>.
**
**	19-sep-1995 (somsa01)
**		Added defined(DESKTOP) for NT, OS/2, and Netware platforms.
**  08-Sep-1997 (merja01)
**      Changes longs to i4 to make compatable with 64 bit platforms
**      such as axp_osf (from schte01).
**
*/
/*
** Ming hints
NEEDLIBS = LIBINGRES
PROGRAM = ckpdb1_check
*/
main(argc, argv)
int	argc;
char	*argv[];
{
  char *dbname;
    CL_SYS_ERR	err_code;
    i4	start_time, end_time;
    i4	status = 0;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &err_code);
#endif
#if defined(UNIX) || defined(DESKTOP)
    TRset_file(TR_T_OPEN, "stdio", 5, &err_code);
#endif
    if (argc < 2)
    {
	TRdisplay("USAGE: CKPDB_RELOAD dbname\n");
	TRset_file(TR_T_CLOSE, 0, 0, &err_code);
	exit(1);
    }
    dbname = argv[1];
/* # line 109 "ckpdbchk.qc" */	/* ingres */
  {
    IIingopen(0,dbname,(char *)0);
  }
/* # line 111 "ckpdbchk.qc" */	/* host code */
	start_time = TMsecs();
	status += check_abort();
	status += check_append();
	status += check_delapp();
	if (status == 0)
	    TRdisplay("Online Checkpoint Consistency Check Successfull\n");
	else
	    TRdisplay("Online Checkpoint Consistency Check Failure\n");
	end_time = TMsecs();
/* # line 124 "ckpdbchk.qc" */	/* exit */
  {
    IIexit();
  }
/* # line 126 "ckpdbchk.qc" */	/* host code */
    TRset_file(TR_T_CLOSE, 0, 0, &err_code);
    PCexit(status);
}
/*
** CHECK_ABORT - Check data from abort test.
**
*/
check_abort()
{
  i4 i;
  i4 j;
	i4  	status = 0;
	/*
	** Check CKPDB_ABORT data.
	**
	**     ckpdb_abort1 should have   0 rows.
	**     ckpdb_abort2 should have 100 rows with values 1-100.
	**     ckpdb_ab_control should have rows with sequential values.
	*/
/* # line 148 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(i=count(ckpdb_abort1.a))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 149 "ckpdbchk.qc" */	/* host code */
	if (i != 0)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_ABORT1 has %d rows - should have 0\n", i);
	}
/* # line 154 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(i=count(ckpdb_ab1_nojnl.a))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB2;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 155 "ckpdbchk.qc" */	/* host code */
	if (i != 0)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AB1_NOJNL has %d rows - should have 0\n", i);
	}
/* # line 161 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(i=count(ckpdb_abort2.a))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB3;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/* # line 162 "ckpdbchk.qc" */	/* host code */
	if (i != 100)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_ABORT2 has %d rows - should have 100\n", i);
	}
/* # line 167 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(i=countu(ckpdb_abort2.a))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB4;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
/* # line 168 "ckpdbchk.qc" */	/* host code */
	if (i != 100)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_ABORT2 has %d unique rows - should have 100\n", i);
	}
/* # line 173 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(i=min(ckpdb_abort2.\
a))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB5;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
/* # line 174 "ckpdbchk.qc" */	/* host code */
	if (i != 1)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_ABORT2 min row is %d - should be 1\n", i);
	}
/* # line 179 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(i=max(ckpdb_abort2.\
a))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB6;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
/* # line 180 "ckpdbchk.qc" */	/* host code */
	if (i != 100)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_ABORT2 max row is %d - should be 100\n", i);
	}
/* # line 186 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(i=count(ckpdb_ab_control.cnt))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB7;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
/* # line 187 "ckpdbchk.qc" */	/* host code */
	if (i != 0)
	{
/* # line 189 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(j=countu(ckpdb_ab_control.cnt))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&j);
      if (IIerrtest() != 0) goto IIrtB8;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE8:;
  }
/* # line 190 "ckpdbchk.qc" */	/* host code */
	    if (i != j)
	    {
		status = 1;
		TRdisplay("Table CKPDB_AB_CONTROL has %d duplicate rows\n",i-j);
	    }
/* # line 195 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(i=min(ckpdb_ab_control.cnt))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB9;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE9:;
  }
/* # line 196 "ckpdbchk.qc" */	/* host code */
	    if (i != 1)
	    {
		status = 1;
		TRdisplay("Table CKPDB_AB_CONTROL min row is %d - should be 1\n", i);
	    }
/* # line 201 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(i=max(ckpdb_ab_control.cnt))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&i);
      if (IIerrtest() != 0) goto IIrtB10;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE10:;
  }
/* # line 202 "ckpdbchk.qc" */	/* host code */
	    if (i != j)
	    {
		status = 1;
		TRdisplay("Table CKPDB_AB_CONTROL max row is %d - should be %d\n", i, j);
	    }
	}
	return (status);
}
/*
** CHECK_APPEND - Check data from append test.
**
*/
check_append()
{
  i4 k;
  i4 l;
	i4  	status = 0;
	/*
	** Check CKPDB_APPEND data.
	**
	**     ckpdb_ap1, ckpdb_ap2, ckpdb_ap3, ckpdb_ap4 should all be same.
	**     Each should have sequential run #'s.
	*/
/* # line 228 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(k=count(ckpdb_ap1.t\
id))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&k);
      if (IIerrtest() != 0) goto IIrtB11;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE11:;
  }
/* # line 229 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(l=countu(ascii(ckpdb_ap1.run)+ascii(ckpdb_ap1.key)+ascii(ckp\
db_ap1.sequence)))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE12;
IIrtB12:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB12;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE12:;
  }
/* # line 230 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP1 has %d duplicate rows\n", k - l);
	}
/* # line 236 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(l=count(ckpdb_ap2.a\
ll))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE13;
IIrtB13:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB13;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE13:;
  }
/* # line 237 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP2 has %d rows, should have %d\n", l, k);
	}
/* # line 242 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(l=count(ckpdb_ap3.a\
ll))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE14;
IIrtB14:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB14;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE14:;
  }
/* # line 243 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP3 has %d rows, should have %d\n", l, k);
	}
/* # line 248 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(l=count(ckpdb_ap4.t\
id))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE15;
IIrtB15:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB15;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE15:;
  }
/* # line 249 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP4 has %d rows, should have %d\n", l, k);
	}
/* # line 254 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(l=count(ckpdb_ap5_nojnl.tid))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE16;
IIrtB16:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB16;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE16:;
  }
/* # line 255 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP5_NOJNL has %d rows, should have %d\n", l, k);
	}
/* # line 260 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(l=count(ap4_ind.tid\
p))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE17;
IIrtB17:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB17;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE17:;
  }
/* # line 261 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table AP4_IND has %d rows, should have %d\n", l, k);
	}
/* # line 266 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(l=count(ap5_ind.tid\
p))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE18;
IIrtB18:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB18;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE18:;
  }
/* # line 267 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table AP5_IND has %d rows, should have %d\n", l, k);
	}
/* # line 273 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(k=max(ckpdb_ap1.run\
))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE19;
IIrtB19:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&k);
      if (IIerrtest() != 0) goto IIrtB19;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE19:;
  }
/* # line 274 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(l=max(ckpdb_ap2.run\
))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE20;
IIrtB20:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB20;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE20:;
  }
/* # line 275 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP2 max is %d, should be %d\n", l, k);
	}
/* # line 280 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(l=max(ckpdb_ap3.run\
))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE21;
IIrtB21:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB21;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE21:;
  }
/* # line 281 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP3 max is %d, should be %d\n", l, k);
	}
/* # line 286 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(l=max(ckpdb_ap4.run\
))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE22;
IIrtB22:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB22;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE22:;
  }
/* # line 287 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP4 max is %d, should be %d\n", l, k);
	}
/* # line 292 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(l=max(ckpdb_ap5_nojnl.run))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE23;
IIrtB23:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l);
      if (IIerrtest() != 0) goto IIrtB23;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE23:;
  }
/* # line 293 "ckpdbchk.qc" */	/* host code */
	if (k != l)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_AP5_NOJNL max is %d, should be %d\n", l, k);
	}
	return (status);
}
/*
** CHECK_DELAPP - Check data from delete/append test.
**
*/
check_delapp()
{
  i4 m;
  i4 n;
  i4 o;
	i4  	status = 0;
	/*
	** Check CKPDB_DELAPP data.
	**
	**     ckpdb_btree and ckpdb_hash should be identical tables.
	**     Both should have 1000 sequential tuples.
	*/
/* # line 319 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(m=count(ckpdb_btree.all))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE24;
IIrtB24:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&m);
      if (IIerrtest() != 0) goto IIrtB24;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE24:;
  }
/* # line 320 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(n=count(ckpdb_hash.all))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE25;
IIrtB25:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n);
      if (IIerrtest() != 0) goto IIrtB25;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE25:;
  }
/* # line 321 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(o=count(ckpdb_btree_nojnl.all))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE26;
IIrtB26:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&o);
      if (IIerrtest() != 0) goto IIrtB26;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE26:;
  }
/* # line 322 "ckpdbchk.qc" */	/* host code */
	if (m != 1000)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_BTREE has %d rows, should have 1000\n", m);
	}
	if (n != 1000)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_HASH has %d rows, should have 1000\n", n);
	}
	if (o != 1000)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_BTREE_NOJNL has %d rows, should have 1000\n", o);
	}
/* # line 338 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(m=countu(ckpdb_btree.all))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE27;
IIrtB27:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&m);
      if (IIerrtest() != 0) goto IIrtB27;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE27:;
  }
/* # line 339 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(n=countu(ckpdb_hash.all))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE28;
IIrtB28:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n);
      if (IIerrtest() != 0) goto IIrtB28;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE28:;
  }
/* # line 340 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(o=countu(ckpdb_btree_nojnl.all))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE29;
IIrtB29:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&o);
      if (IIerrtest() != 0) goto IIrtB29;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE29:;
  }
/* # line 341 "ckpdbchk.qc" */	/* host code */
	if (m != 1000)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_BTREE has %d unique rows, should have 1000\n", m);
	}
	if (n != 1000)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_HASH has %d unique rows, should have 1000\n", n);
	}
	if (o != 1000)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_BTREE_JNL has %d unique rows, should have 1000\n", o);
	}
/* # line 357 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(m=min(ckpdb_btree.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE30;
IIrtB30:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&m);
      if (IIerrtest() != 0) goto IIrtB30;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE30:;
  }
/* # line 358 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(n=max(ckpdb_btree.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE31;
IIrtB31:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n);
      if (IIerrtest() != 0) goto IIrtB31;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE31:;
  }
/* # line 359 "ckpdbchk.qc" */	/* host code */
	if (n-m != 999)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_BTREE max-min is %d, should be 999\n", n-m);
	}
/* # line 365 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(m=min(ckpdb_btree.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE32;
IIrtB32:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&m);
      if (IIerrtest() != 0) goto IIrtB32;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE32:;
  }
/* # line 366 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(n=min(ckpdb_hash.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE33;
IIrtB33:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n);
      if (IIerrtest() != 0) goto IIrtB33;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE33:;
  }
/* # line 367 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(o=min(ckpdb_btree_nojnl.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE34;
IIrtB34:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&o);
      if (IIerrtest() != 0) goto IIrtB34;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE34:;
  }
/* # line 368 "ckpdbchk.qc" */	/* host code */
	if (n != m)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_HASH min is %d, should be %d\n", n, m);
	}
	if (o != m)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_BTREE_NOJNL min is %d, should be %d\n", o, m);
	}
/* # line 379 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(m=max(ckpdb_btree.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE35;
IIrtB35:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&m);
      if (IIerrtest() != 0) goto IIrtB35;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE35:;
  }
/* # line 380 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(n=max(ckpdb_hash.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE36;
IIrtB36:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n);
      if (IIerrtest() != 0) goto IIrtB36;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE36:;
  }
/* # line 381 "ckpdbchk.qc" */	/* retrieve */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(o=max(ckpdb_btree_nojnl.value))");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE37;
IIrtB37:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&o);
      if (IIerrtest() != 0) goto IIrtB37;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE37:;
  }
/* # line 382 "ckpdbchk.qc" */	/* host code */
	if (n != m)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_HASH max is %d, should be %d\n", n, m);
	}
	if (o != m)
	{
	    status = 1;
	    TRdisplay("Table CKPDB_BTREE_NOJNL max is %d, should be %d\n", o, m);
	}
	return (status);
}
