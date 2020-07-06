# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Program to add a "seed" value to the table ``seeds''.
**	This "seed" is used by MTS to determine which operations
**	to perform.
**
** 	bh	06/25/92	added libraries for ming - NEEDLIBS
*/
/* ming hints 
**
NEEDLIBS = MTSLIB LIBINGRES
**
*/
main (argc,argv)
int  	argc;
char    *argv[];
{
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
  int seed;
  char *dbname = argv[1];
/* # line 29 "getseed.sc" */	/* host code */
	if (argc != 3)
	{
		printf ("\n\tUsage: getseed <dbname> <seed>\n");
		PCexit();
	}
/* # line 37 "getseed.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      IIsqStop(&sqlca);
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 41 "getseed.sc" */	/* host code */
	seed = atoi(argv[2]);
	if (seed == 0)
	{
		seed = (int) time(0);
	}
	printf ("\n\tSeed used: %d\n", seed);
/* # line 48 "getseed.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify seeds to truncated");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 50 "getseed.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"insert into seeds values(");
    IIputdomio((short *)0,1,30,4,&seed);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 52 "getseed.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 54 "getseed.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
    else if (sqlca.sqlwarn.sqlwarn0 == 'W') 
      IIsqPrint(&sqlca);
  }
/* # line 55 "getseed.sc" */	/* host code */
}
