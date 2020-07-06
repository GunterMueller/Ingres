# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	impqsf.sc
**
** This file contains all of the routines needed to support the "QSF" 
** Subcomponent of the IMA based IMP lookalike tools.
**
## History:
##
##	21-Jun-95	(johna)
##		Incorporated from earlier QSF tools
##
##      10-Apr-02       (mdf)
##              Expand domain to handle looking at a different QSF .
##
##      17-Apr-02       (mdf170402)
##              Sort the dbprocs on usage, in descending order.
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##	08-Oct-2007 (hanje04)
##	    SIR 114907
##	    Remove declaration of timebuf as it's not referenced and causes
##	    errors on Mac OSX
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
  struct qsr_ {
    char server[65];
    long qsr_bkts_used;
    long qsr_bmaxobjs;
    long qsr_decay_factor;
    long qsr_memleft;
    long qsr_memtot;
    long qsr_mx_index;
    long qsr_mx_named;
    long qsr_mx_rsize;
    long qsr_mx_size;
    long qsr_mx_unnamed;
    long qsr_mxbkts_used;
    long qsr_mxobjs;
    long qsr_mxsess;
    long qsr_named_requests;
    long qsr_nbuckets;
    long qsr_no_destroyed;
    long qsr_no_index;
    long qsr_no_named;
    long qsr_no_unnamed;
    long qsr_nobjs;
    long qsr_nsess;
  } qsr;
  char bar_chart[51];
  float pct_used;
  extern int ima_qsf_cache_stats;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern int userSession;
  extern char *currentVnode;
/*
** Name:	impqsfsum.sc
**
** Display the QSF cache statistics summary
**
*/
  extern int timeVal;
int
processQsfSummary(server)
  GcnEntry *server;
{
  char server_name[SERVER_NAME_LENGTH];
/* # line 98 "impqsf.sc" */	/* set_sql */
  {
    IILQssSetSqlio(46,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 99 "impqsf.sc" */	/* select */
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
/* # line 100 "impqsf.sc" */	/* host code */
	strcat(server_name,"::/@");
	strcat(server_name,server->serverAddress);
/* # line 102 "impqsf.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_set_domain");
    IILQprvProcValio((char *)"entry",0,(short *)0,1,32,0,server_name);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 107 "impqsf.sc" */	/* display */
  {
    if (IIdispfrm((char *)"ima_qsf_cache_stats",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 110 "impqsf.sc" */	/* putform */
    {
      if (IIfsetio((char *)"ima_qsf_cache_stats") != 0) {
        IIputfldio((char *)"server",(short *)0,1,32,0,server_name);
      } /* IIfsetio */
    }
/* # line 111 "impqsf.sc" */	/* host code */
		getQsfSummary(server_name);
		displayQsfSummary();
/* # line 113 "impqsf.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 119 "impqsf.sc" */	/* host code */
		getQsfSummary(server_name);
		displayQsfSummary();
      } else if (IIretval() == 2) {
		clear_qsf();
		getQsfSummary(server_name);
		displayQsfSummary();
      } else if (IIretval() == 3) {
/* # line 132 "impqsf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 133 "impqsf.sc" */	/* host code */
		processQsfProcedures(server_name,"ALL");
/* # line 134 "impqsf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
          } /* IIiqset */
        }
      } else if (IIretval() == 4) {
/* # line 139 "impqsf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 140 "impqsf.sc" */	/* host code */
		processQsfQueries(server_name);
/* # line 141 "impqsf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
          } /* IIiqset */
        }
      } else if (IIretval() == 5) {
/* # line 146 "impqsf.sc" */	/* host code */
		getQsfSummary(server_name);
		displayQsfSummary();
      } else if (IIretval() == 6) {
/* # line 152 "impqsf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 153 "impqsf.sc" */	/* set_sql */
        {
          IILQssSetSqlio(46,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        }
/* # line 154 "impqsf.sc" */	/* breakdisplay */
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
    if (IIFRtoact(0,1) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact((char *)"Flush",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Procedures",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"RepeatQueries",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 157 "impqsf.sc" */	/* host code */
	return(TRUE);
}
getQsfSummary(server_name)
  char *server_name;
{
	int i;
	int blks;
  char msgBuf[100];
/* # line 171 "impqsf.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impqsf1",7545,3323);
      IIputdomio((short *)0,1,32,0,server_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"impqsf1",7545,3323);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select server, qsf_bkts_used, qsf_bmaxobjs, qsf_decay_factor, qsf_mem\
left, qsf_memtot, qsf_mx_index, qsf_mx_named, qsf_mx_rsize, qsf_mx_siz\
e, qsf_mx_unnamed, qsf_mxbkts_used, qsf_mxobjs, qsf_mxsess, qsf_named_\
requests, qsf_nbuckets, qsf_no_destroyed, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"qsf_no_index, qsf_no_named, qsf_no_unnamed, qsf_nobjs, qsf_nsess from\
 ima_qsf_cache_stats where server=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,server_name);
        IIexDefine(0,(char *)"impqsf1",7545,3323);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,64,qsr.server);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_bkts_used);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_bmaxobjs);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_decay_factor);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_memleft);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_memtot);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_mx_index);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_mx_named);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_mx_rsize);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_mx_size);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_mx_unnamed);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_mxbkts_used);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_mxobjs);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_mxsess);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_named_requests);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_nbuckets);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_no_destroyed);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_no_index);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_no_named);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_no_unnamed);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_nobjs);
      IIgetdomio((short *)0,1,30,4,&qsr.qsr_nsess);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 222 "impqsf.sc" */	/* host code */
	sqlerr_check();
	/*
	** calculate the percentage used and the bar_chart
	*/
	pct_used = (100 * (qsr.qsr_memtot - qsr.qsr_memleft)) ;
	pct_used = pct_used / qsr.qsr_memtot;
	blks = (pct_used * 50) / 100;
	for (i = 0; i < blks; i++) {
		bar_chart[i] = '%';
	}
	bar_chart[i] = '\0';
}
clear_qsf()
{
/* # line 238 "impqsf.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set trace point qs506");
    IIsyncup((char *)0,0);
  }
/* # line 239 "impqsf.sc" */	/* host code */
}
displayQsfSummary()
{
/* # line 243 "impqsf.sc" */	/* putform */
  {
    if (IIfsetio((char *)"ima_qsf_cache_stats") != 0) {
      IIputfldio((char *)"server",(short *)0,1,32,0,qsr.server);
      IIputfldio((char *)"qsr_bkts_used",(short *)0,1,30,4,&qsr.qsr_bkts_used);
      IIputfldio((char *)"qsr_bmaxobjs",(short *)0,1,30,4,&qsr.qsr_bmaxobjs);
      IIputfldio((char *)"qsr_decay_factor",(short *)0,1,30,4,&
      qsr.qsr_decay_factor);
      IIputfldio((char *)"qsr_memleft",(short *)0,1,30,4,&qsr.qsr_memleft);
      IIputfldio((char *)"qsr_memtot",(short *)0,1,30,4,&qsr.qsr_memtot);
      IIputfldio((char *)"qsr_mx_index",(short *)0,1,30,4,&qsr.qsr_mx_index);
      IIputfldio((char *)"qsr_mx_named",(short *)0,1,30,4,&qsr.qsr_mx_named);
      IIputfldio((char *)"qsr_mx_rsize",(short *)0,1,30,4,&qsr.qsr_mx_rsize);
      IIputfldio((char *)"qsr_mx_size",(short *)0,1,30,4,&qsr.qsr_mx_size);
      IIputfldio((char *)"qsr_mx_unnamed",(short *)0,1,30,4,&
      qsr.qsr_mx_unnamed);
      IIputfldio((char *)"qsr_mxbkts_used",(short *)0,1,30,4,&
      qsr.qsr_mxbkts_used);
      IIputfldio((char *)"qsr_mxobjs",(short *)0,1,30,4,&qsr.qsr_mxobjs);
      IIputfldio((char *)"qsr_mxsess",(short *)0,1,30,4,&qsr.qsr_mxsess);
      IIputfldio((char *)"qsr_named_requests",(short *)0,1,30,4,&
      qsr.qsr_named_requests);
      IIputfldio((char *)"qsr_nbuckets",(short *)0,1,30,4,&qsr.qsr_nbuckets);
      IIputfldio((char *)"qsr_no_destroyed",(short *)0,1,30,4,&
      qsr.qsr_no_destroyed);
      IIputfldio((char *)"qsr_no_index",(short *)0,1,30,4,&qsr.qsr_no_index);
      IIputfldio((char *)"qsr_no_named",(short *)0,1,30,4,&qsr.qsr_no_named);
      IIputfldio((char *)"qsr_no_unnamed",(short *)0,1,30,4,&
      qsr.qsr_no_unnamed);
      IIputfldio((char *)"qsr_nobjs",(short *)0,1,30,4,&qsr.qsr_nobjs);
      IIputfldio((char *)"qsr_nsess",(short *)0,1,30,4,&qsr.qsr_nsess);
      IIputfldio((char *)"pct_used",(short *)0,1,31,4,&pct_used);
      IIputfldio((char *)"bar_chart",(short *)0,1,32,0,bar_chart);
    } /* IIfsetio */
  }
/* # line 268 "impqsf.sc" */	/* host code */
}
processQsfProcedures(server_name,user_name)
  char *server_name;
  char *user_name;
{
/* # line 278 "impqsf.sc" */	/* display */
  {
    if (IIdispfrm((char *)"ima_qsf_dbp",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
/* # line 281 "impqsf.sc" */	/* putform */
    {
      if (IIfsetio((char *)"ima_qsf_dbp") != 0) {
        IIputfldio((char *)"server",(short *)0,1,32,0,server_name);
      } /* IIfsetio */
    }
/* # line 282 "impqsf.sc" */	/* host code */
		displayDbProcs(user_name);
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		displayDbProcs(user_name);
      } else if (IIretval() == 2) {
		clear_qsf();
		displayDbProcs(user_name);
      } else if (IIretval() == 3) {
		displayDbProcs(user_name);
      } else if (IIretval() == 4) {
/* # line 303 "impqsf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
          } /* IIiqset */
        }
/* # line 304 "impqsf.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIFRtoact(0,1) == 0) goto IIfdE2;
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact((char *)"Flush",(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 307 "impqsf.sc" */	/* host code */
	return(TRUE);
}
processQsfQueries(server_name)
  char *server_name;
{
/* # line 316 "impqsf.sc" */	/* display */
  {
    if (IIdispfrm((char *)"ima_qsf_rqp",(char *)"r") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
/* # line 319 "impqsf.sc" */	/* putform */
    {
      if (IIfsetio((char *)"ima_qsf_rqp") != 0) {
        IIputfldio((char *)"server",(short *)0,1,32,0,server_name);
      } /* IIfsetio */
    }
/* # line 320 "impqsf.sc" */	/* host code */
		displayRqp();
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		displayRqp();
      } else if (IIretval() == 2) {
		clear_qsf();
		displayRqp();
      } else if (IIretval() == 3) {
		displayRqp();
      } else if (IIretval() == 4) {
/* # line 342 "impqsf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
          } /* IIiqset */
        }
/* # line 343 "impqsf.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
      } else {
        if (1) goto IIfdE3;
      } /* IIretval */
    } /* IIrunform */
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    goto IIfdE3;
IImuI3:
    if (IIFRtoact(0,1) == 0) goto IIfdE3;
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact((char *)"Flush",(char *)0,2,2,2) == 0) goto IIfdE3;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,3) == 0) goto IIfdE3;
    if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 346 "impqsf.sc" */	/* host code */
	return(TRUE);
}
displayDbProcs(user_name)
  char *user_name;
{
  char db_name[80];
  char db_user_name[80];
  char dbp_name[80];
  int db_id;
  int size;
  int usage;
  int allUsers = FALSE;
  int anything = FALSE;
	makeIidbdbConnection();
	if (strcmp(user_name,"ALL") == 0) allUsers = TRUE;
/* # line 368 "impqsf.sc" */	/* inittable */
  {
    if (IItbinit((char *)"ima_qsf_dbp",(char *)"ima_qsf_dbp",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 369 "impqsf.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impqsf2",7546,4475);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"impqsf2",7546,4475);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select trim(shift(dbp_name,  -20)), trim(dbp_owner), dbp_size, dbp_db\
id, dbp_usage_count from ima_qsf_dbp order by dbp_usage_count desc");
        IIexDefine(0,(char *)"impqsf2",7546,4475);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,79,dbp_name);
      IIgetdomio((short *)0,1,32,79,db_user_name);
      IIgetdomio((short *)0,1,30,4,&size);
      IIgetdomio((short *)0,1,30,4,&db_id);
      IIgetdomio((short *)0,1,30,4,&usage);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 387 "impqsf.sc" */	/* host code */
		if ((allUsers) || (strcmp(db_user_name,user_name) == 0))
		{
			decode_db_id(db_name,db_id);
/* # line 390 "impqsf.sc" */	/* loadtable */
      {
        if (IItbact((char *)"ima_qsf_dbp",(char *)"ima_qsf_dbp",1) != 0) {
          IItcoputio((char *)"dbp_name",(short *)0,1,32,0,dbp_name);
          IItcoputio((char *)"dbp_owner",(short *)0,1,32,0,db_user_name);
          IItcoputio((char *)"dbp_size",(short *)0,1,30,4,&size);
          IItcoputio((char *)"dbp_dbname",(short *)0,1,32,0,db_name);
          IItcoputio((char *)"dbp_usage_count",(short *)0,1,30,4,&usage);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 396 "impqsf.sc" */	/* host code */
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	sqlerr_check();
/* # line 399 "impqsf.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 402 "impqsf.sc" */	/* host code */
}
displayRqp()
{
  char db_name[80];
  char db_user_name[80];
  char rqp_name[80];
  int db_id;
  int size;
  int usage;
  int allUsers = FALSE;
  int anything = FALSE;
	makeIidbdbConnection();
/* # line 419 "impqsf.sc" */	/* inittable */
  {
    if (IItbinit((char *)"ima_qsf_rqp",(char *)"ima_qsf_rqp",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 420 "impqsf.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impqsf3",7547,4635);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"impqsf3",7547,4635);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select trim(shift(rqp_name,  -20)), rqp_size, rqp_dbid, rqp_usage_cou\
nt from ima_qsf_rqp order by rqp_usage_count desc");
        IIexDefine(0,(char *)"impqsf3",7547,4635);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,79,rqp_name);
      IIgetdomio((short *)0,1,30,4,&size);
      IIgetdomio((short *)0,1,30,4,&db_id);
      IIgetdomio((short *)0,1,30,4,&usage);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 434 "impqsf.sc" */	/* host code */
		decode_db_id(db_name,db_id);
/* # line 435 "impqsf.sc" */	/* loadtable */
      {
        if (IItbact((char *)"ima_qsf_rqp",(char *)"ima_qsf_rqp",1) != 0) {
          IItcoputio((char *)"rqp_name",(short *)0,1,32,0,rqp_name);
          IItcoputio((char *)"rqp_size",(short *)0,1,30,4,&size);
          IItcoputio((char *)"rqp_dbname",(short *)0,1,32,0,db_name);
          IItcoputio((char *)"rqp_usage_count",(short *)0,1,30,4,&usage);
          IITBceColEnd();
        } /* IItbact */
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/* # line 441 "impqsf.sc" */	/* host code */
	sqlerr_check();
/* # line 442 "impqsf.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 445 "impqsf.sc" */	/* host code */
}
decode_db_id(name,id)
  char *name;
  int id;
{
/* # line 454 "impqsf.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&iidbdbSession);
  }
/* # line 455 "impqsf.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impqsf4",7548,12763);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"impqsf4",7548,12763);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select name from iidatabase where db_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"impqsf4",7548,12763);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 463 "impqsf.sc" */	/* host code */
		if (sqlerr_check() == NO_ROWS) {
			sprintf(name,"(%ld)",id);
		}
/* # line 466 "impqsf.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 467 "impqsf.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 468 "impqsf.sc" */	/* host code */
}
