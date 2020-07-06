# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	impdbcon.sc
**
** DB connection routines for the new IMA based IPM lookalike tool.
**
**
** This file contains the routines used to connect and disconnect from the 
** various databases needed by the IMP. This section has undergone quite
** substantial changes as the connection to the iidbdb and user databases
** was deferred.
**
** The error check routine needs considerable rework to deal with no rows 
** situations. I was about to do this when the decision to handover was made.
** 
## History:
##
## 26-jan-94	(johna)
##		started on RIF day from pervious ad-hoc programs
## 04-may-94	(johna)
##		some comments
## 02-Jul-2002 (fanra01)
##      Sir 108159
##      Added to distribution in the sig directory.
## 04-Sep-03    (flomi02)  mdf040903
##              Commit transactions to release locks
## 06-Apr-04    (srisu02) SIR 108159
##      Integrating changes made by flomi02  
##
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef UNIX
#include <sys/signal.h>
#define SIGHANDLERS \
signal(SIGINT,sighandler); \
signal(SIGQUIT,sighandler); \
signal(SIGBUS,sighandler); \
signal(SIGSEGV,sighandler)
#else
#include <signal.h>
#define SIGHANDLERS \
signal(SIGINT,sighandler); \
signal(SIGSEGV,sighandler)
#endif /* NT_GENERIC */
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
void	sighandler( int );
  extern int iidbdbConnection;
  extern int dbConnection;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern char *currentVnode;
int
sqlerr_check()
{
  char msgBuf[1024];
  int currentConnection;
	if (sqlca.sqlcode < 0) {
/* # line 77 "impdbcon.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,32,1023,msgBuf,63);
  }
/* # line 78 "impdbcon.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&currentConnection,11);
  }
/* # line 80 "impdbcon.sc" */	/* host code */
		if (inFormsMode) {
/* # line 81 "impdbcon.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 82 "impdbcon.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(msgBuf);
  }
/* # line 83 "impdbcon.sc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 84 "impdbcon.sc" */	/* endforms */
  {
    IIendforms();
  }
/* # line 85 "impdbcon.sc" */	/* host code */
		} else {
			printf("%s\n",msgBuf);
		}
		if (currentConnection) {
/* # line 89 "impdbcon.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 90 "impdbcon.sc" */	/* host code */
		}
		exit(0);
	} else if (sqlca.sqlcode == 100) {
		return(NO_ROWS);
	}
	return(TRUE);
}
/*
** makeDbConnections - originally made connections to the IMA database and 
** the local IIDBDB. The IIDBDB  connect was postponed as we may want to switch
** vnodes.
*/
int
makeDbConnections(dbname)
  char *dbname;
{
  char buf[60];
  char iidbdbName[60];
/* # line 115 "impdbcon.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(imaSession);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 116 "impdbcon.sc" */	/* host code */
	sqlerr_check();
	dbConnection = TRUE;
/* # line 118 "impdbcon.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where readlock=nolock");
    IIsyncup((char *)0,0);
  }
/* # line 119 "impdbcon.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_complete_vnode");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 120 "impdbcon.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 121 "impdbcon.sc" */	/* host code */
/*
## mdf040903 
*/
/* # line 125 "impdbcon.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 127 "impdbcon.sc" */	/* host code */
        SIGHANDLERS;
	return(TRUE);
}
/*
** Set up the IIDBDB connection for the relevant vnode.
*/
int
makeIidbdbConnection()
{
  char buf[60];
  char iidbdbName[60];
	if (iidbdbConnection) {
		return(FALSE);
	}
	sprintf(iidbdbName,"%s::iidbdb",currentVnode);
/* # line 148 "impdbcon.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(iidbdbSession);
    IIsqConnect(0,iidbdbName,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 149 "impdbcon.sc" */	/* host code */
	sqlerr_check();
/* # line 150 "impdbcon.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 151 "impdbcon.sc" */	/* host code */
	iidbdbConnection = TRUE;
        SIGHANDLERS;
	signal(SIGFPE,sighandler);
	return(TRUE);
}
int
closeIidbdbConnection()
{
  int currentConnection;
	if (iidbdbConnection) {
		iidbdbConnection = FALSE;
/* # line 170 "impdbcon.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&currentConnection,11);
  }
/* # line 171 "impdbcon.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&iidbdbSession);
  }
/* # line 172 "impdbcon.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 173 "impdbcon.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&currentConnection,11);
  }
/* # line 174 "impdbcon.sc" */	/* host code */
		if (currentConnection) {
/* # line 175 "impdbcon.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&currentConnection);
  }
/* # line 176 "impdbcon.sc" */	/* host code */
		}
	}
	return(TRUE);
}
int
closeDbConnections()
{
  int currentConnection;
/* # line 188 "impdbcon.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 189 "impdbcon.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&currentConnection,11);
  }
/* # line 190 "impdbcon.sc" */	/* host code */
	if (currentConnection) {
/* # line 191 "impdbcon.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 192 "impdbcon.sc" */	/* host code */
	}
	closeIidbdbConnection();
	return(TRUE);
}
void
sighandler(sig)
int	sig;
{
  char msgBuf[100];
	sprintf(msgBuf, "Received Signal %d - Exiting...",sig);
	if (inFormsMode) {
/* # line 207 "impdbcon.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 208 "impdbcon.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(msgBuf);
  }
/* # line 209 "impdbcon.sc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 210 "impdbcon.sc" */	/* endforms */
  {
    IIendforms();
  }
/* # line 211 "impdbcon.sc" */	/* host code */
	} else {
		printf("%s\n",msgBuf);
	}
	exit(0);
}
int
fatal(str)
  char *str;
{
  int currentConnection;
/* # line 227 "impdbcon.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&currentConnection,11);
  }
/* # line 229 "impdbcon.sc" */	/* host code */
	if (inFormsMode) {
/* # line 230 "impdbcon.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 231 "impdbcon.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(str);
  }
/* # line 232 "impdbcon.sc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 233 "impdbcon.sc" */	/* endforms */
  {
    IIendforms();
  }
/* # line 234 "impdbcon.sc" */	/* host code */
	} else {
		printf("%s\n",str);
	}
	if (currentConnection) {
/* # line 238 "impdbcon.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 239 "impdbcon.sc" */	/* host code */
	}
	exit(0);
}
