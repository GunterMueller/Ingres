# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	impslave.sc
**
** This file contains all of the routines needed to support the "SLAVE" 
** Subcomponent of the IMA based IMP lookalike tools.
**
## History:
##
##	08-Jul-95 (johna)
##		written
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##    08-Oct-2007 (hanje04)
##	SIR 114907
##	Remove declaration of timebuf as it's not referenced and causes
##	errors on Mac OSX
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
  struct slave_ {
    char server[65];
    long di_slaveno;
    long di_dio;
    long cpu_tm_msecs;
    long cpu_tm_secs;
    long di_idrss;
    long di_maxrss;
    long di_majflt;
    long di_minflt;
    long di_msgrcv;
    long di_msgsnd;
    long di_msgtotal;
    long di_nivcsw;
    long di_nsignals;
    long di_nvcsw;
    long di_nswap;
    long di_reads;
    long di_writes;
    long di_stime_tm_msecs;
    long di_stime_tm_secs;
    long di_utime_tm_msecs;
    long di_utime_tm_secs;
  } slave;
  extern int ima_di_slave_info;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern int userSession;
  extern char *currentVnode;
  extern int timeVal;
int
processSlaveInfo(server)
  GcnEntry *server;
{
  char server_name[SERVER_NAME_LENGTH];
/* # line 107 "impslave.sc" */	/* set_sql */
  {
    IILQssSetSqlio(46,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 108 "impslave.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('IMA_VNODE')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,server_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 109 "impslave.sc" */	/* host code */
	strcat(server_name,"::/@");
	strcat(server_name,server->serverAddress);
/* # line 112 "impslave.sc" */	/* display */
  {
    if (IIdispfrm((char *)"ima_di_slave_info",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 115 "impslave.sc" */	/* putform */
    {
      if (IIfsetio((char *)"ima_di_slave_info") != 0) {
        IIputfldio((char *)"server",(short *)0,1,32,0,server_name);
      } /* IIfsetio */
    }
/* # line 116 "impslave.sc" */	/* host code */
		refreshSlaves(server_name);
		displaySlaveSummary(server_name);
/* # line 118 "impslave.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 123 "impslave.sc" */	/* host code */
		refreshSlaves(server_name);
		displaySlaveSummary(server_name);
      } else if (IIretval() == 2) {
/* # line 129 "impslave.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 130 "impslave.sc" */	/* set_sql */
        {
          IILQssSetSqlio(46,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        }
/* # line 131 "impslave.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 134 "impslave.sc" */	/* host code */
	return(TRUE);
}
displaySlaveSummary(server_name)
  char *server_name;
{
	int i;
	int blks;
  char msgBuf[100];
/* # line 148 "impslave.sc" */	/* inittable */
  {
    if (IItbinit((char *)"ima_di_slave_info",(char *)"ima_di_slave_inf\
o",(char *)"f") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 149 "impslave.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impslave1",7546,10748);
      IIputdomio((short *)0,1,32,0,server_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"impslave1",7546,10748);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select * from ima_di_slave_info where server=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,server_name);
        IIexDefine(0,(char *)"impslave1",7546,10748);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,64,(slave).server);
      IIgetdomio((short *)0,1,30,4,&(slave).di_slaveno);
      IIgetdomio((short *)0,1,30,4,&(slave).di_dio);
      IIgetdomio((short *)0,1,30,4,&(slave).cpu_tm_msecs);
      IIgetdomio((short *)0,1,30,4,&(slave).cpu_tm_secs);
      IIgetdomio((short *)0,1,30,4,&(slave).di_idrss);
      IIgetdomio((short *)0,1,30,4,&(slave).di_maxrss);
      IIgetdomio((short *)0,1,30,4,&(slave).di_majflt);
      IIgetdomio((short *)0,1,30,4,&(slave).di_minflt);
      IIgetdomio((short *)0,1,30,4,&(slave).di_msgrcv);
      IIgetdomio((short *)0,1,30,4,&(slave).di_msgsnd);
      IIgetdomio((short *)0,1,30,4,&(slave).di_msgtotal);
      IIgetdomio((short *)0,1,30,4,&(slave).di_nivcsw);
      IIgetdomio((short *)0,1,30,4,&(slave).di_nsignals);
      IIgetdomio((short *)0,1,30,4,&(slave).di_nvcsw);
      IIgetdomio((short *)0,1,30,4,&(slave).di_nswap);
      IIgetdomio((short *)0,1,30,4,&(slave).di_reads);
      IIgetdomio((short *)0,1,30,4,&(slave).di_writes);
      IIgetdomio((short *)0,1,30,4,&(slave).di_stime_tm_msecs);
      IIgetdomio((short *)0,1,30,4,&(slave).di_stime_tm_secs);
      IIgetdomio((short *)0,1,30,4,&(slave).di_utime_tm_msecs);
      IIgetdomio((short *)0,1,30,4,&(slave).di_utime_tm_secs);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 159 "impslave.sc" */	/* insertrow */
      {
        if (IItbsetio(5,(char *)"ima_di_slave_info",(char *)
"ima_di_slave_info",-2) != 0) {
          if (IItinsert() != 0) {
            IItcoputio((char *)"di_slaveno",(short *)0,1,30,4,&
            slave.di_slaveno);
            IItcoputio((char *)"di_dio",(short *)0,1,30,4,&slave.di_dio);
            IItcoputio((char *)"di_reads",(short *)0,1,30,4,&slave.di_reads);
            IItcoputio((char *)"di_writes",(short *)0,1,30,4,&slave.di_writes);
            IItcoputio((char *)"di_stime_tm_secs",(short *)0,1,30,4,&
            slave.di_stime_tm_secs);
            IItcoputio((char *)"di_stime_tm_msecs",(short *)0,1,30,4,&
            slave.di_stime_tm_msecs);
            IItcoputio((char *)"di_utime_tm_secs",(short *)0,1,30,4,&
            slave.di_utime_tm_secs);
            IItcoputio((char *)"di_utime_tm_msecs",(short *)0,1,30,4,&
            slave.di_utime_tm_msecs);
            IItcoputio((char *)"cpu_tm_secs",(short *)0,1,30,4,&
            slave.cpu_tm_secs);
            IItcoputio((char *)"cpu_tm_msecs",(short *)0,1,30,4,&
            slave.cpu_tm_msecs);
            IITBceColEnd();
          } /* IItinsert */
        } /* IItbsetio */
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 171 "impslave.sc" */	/* host code */
	sqlerr_check();
}
refreshSlaves(server_name)
  char *server_name;
{
/* # line 180 "impslave.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"ima_collect_slave_stats");
    IILQprvProcValio((char *)"server",0,(short *)0,1,32,0,server_name);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 182 "impslave.sc" */	/* host code */
}
