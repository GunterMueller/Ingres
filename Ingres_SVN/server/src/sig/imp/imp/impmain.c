# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	imp.sc
**
** Driving routine for the new IMA based IPM lookalike tool.
**
** This routine really needs to parse command line args properly (in addition
** to the as-yet-unwritten options screen.
**
** General comments for the whole utility - error checking does not exist or
** does not work (what do you expect for a prototype) - also there seems to be
** some problem with the session switching logic.
**
** Each form needs to display the auto-timeout value (if set) or some other
** suiutable piece of information.
**
** The whole code should be reworked with the CL.
**
## History:
##
## 26-jan-94	(johna)
##		started on RIF day from pervious ad-hoc programs
## 04-may-94	(johna)
##		some comments
##
## 02-Jul-2002 (fanra01)
##      Sir 108159
##      Added to distribution in the sig directory.
## 06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
  extern int VnodeNotDefined = TRUE;
  extern int timeVal;
  extern char *currentVnode;
# ifndef DATABASE
# define DATABASE "imadb"
# endif
void getVnode();
void getTimeVal();
void printerr();
int
main(argc,argv)
int	argc;
char	*argv[];
{
  char dbname[30];
  char argbuf[30];
  int cnt = 1;
  int spareInt = 0;
	VnodeNotDefined = TRUE;             /*
##mdf040903
*/
	/*
	** process the command line args
	*/
    	strcpy(dbname, DATABASE);
	while (cnt < argc) {
		if (*argv[cnt] != '-') {
			/*
			** assume a database name
			*/
			strcpy(dbname,argv[cnt]);
		} else {
			/*
			** flag - process as appropriate
			*/
			strncpy(argbuf,argv[cnt],30);
			switch(argbuf[1]) {
				case 'r':
					getTimeVal(argbuf);
					break;
				/* 
##mdf040903 { 
*/
				case 'v':
					VnodeNotDefined = FALSE;
					VnodeNotDefined = TRUE;
					getVnode(argbuf);
					break;
				/* 
##mdf040903 } 
*/
				default:
					printerr(argbuf);
			}
		}
		cnt++;
	}
/* # line 110 "impmain.sc" */	/* set_sql */
  {
    IILQssSetSqlio(49,(short *)0,1,32,0,(char *)"imp.trace");
  }
/* # line 111 "impmain.sc" */	/* host code */
	makeDbConnections(dbname);
/* # line 112 "impmain.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('ima_vnode')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,29,dbname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 113 "impmain.sc" */	/* host code */
	sqlerr_check();
        if (VnodeNotDefined == TRUE) {
	    currentVnode = dbname;
        }
	/* 
	** don't make the iidbdb connection until we need to - in 
	** case the user switches Vnodes on us before we goo looking
	*/
	/* makeIidbdbConnection(); */
	setupFrs();
	startMenu();
	closeFrs();
	closeDbConnections();
}
void getVnode(buf)
char	*buf;
{
	char	nodeval[30];
	if (sscanf(buf,"-v%s",nodeval) != 1) {
		fprintf(stderr,
			"Ignoring \"%s\" - not a valid node value\n",
			buf);
	} else {
		currentVnode = nodeval;
/*exec frs message :currentVnode; exec frs sleep 3; */
	}
}
void getTimeVal(buf)
char	*buf;
{
	int	spareInt = 0;
	if (sscanf(buf,"-r%d",&spareInt) != 1) {
		fprintf(stderr,
			"Ignoring \"%s\" - not a valid timeout value\n",
			buf);
	} else {
		timeVal = spareInt;
	}
}
void printerr(buf)
char	*buf;
{
	fprintf(stderr,
	"Ignoring \"%s\" - not a valid flag\n",
	buf);
}
