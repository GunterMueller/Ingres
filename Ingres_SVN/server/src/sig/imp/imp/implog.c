# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	implog.sc
**
** This file contains all of the routines needed to support the "LOG" menu
** tree of the IMA based IMP lookalike tools.
**
** The structures in this file could easily be moved to the impcommon.sc file.
**
** In common with the implocks.sc file - these routines generalyy are of the
** form
**	processBlah() - called from menu
**		getBlah() - get the values from the IMA query
**		displayBlah() - do the putform stuff.
**
** Also in common with the implocks stuff - the situation may have changed by
** the time we take a closer look - this needs to be examined.
**
** The 'hard' parts of the log header screen/ dual log stuff all needs to be
** written.
**
** decodeStatusVal is present as the object in IMA that provides this
** information is wrong (exp.dmf.lg.lpb_status) - see bug 62546 
**
## History:
##
##	14-apr-94	(johna)
##			Started.
##	04-may-94	(johna)
##			some comments.
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##
##      04-Sep-03        Added extra functionality (mdf040903)
##                         Session_Info 
##                         Tx_Info
##                         Lock_Info
##      06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
##    08-Oct-2007 (hanje04)
##        SIR 114907
##        Remove declaration of timebuf as it's not referenced and causes
##        errors on Mac OSX
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
SessEntry *newSessEntry();
void resetLogSummary();
void getLogSummary();
void displayLogSummary();
void displayHeaderInfo();
void displayLogProcesses();
void TransactionDetail();
void displayLogDatabases();
void displayLogTransactions();
void getServerName();
  extern int imp_transaction_detail;
  struct lgd_stat {
    long db_adds;
    long db_removes;
    long tx_begins;
    float tx_ends;
    long log_writes;
    long log_write_ios;
    long log_read_ios;
    long log_waits;
    long log_split_waits;
    long log_free_waits;
    long log_stall_waits;
    long log_forces;
    long log_group_commit;
    long log_group_count;
    long check_commit_timer;
    long timer_write;
    float kbytes_written;
    long block_size;
    float buffer_utilisation;
    double avg_txn_size;
    long inconsistent_db;
  };
  struct lg_header {
    char status[50];
    long block_count;
    long block_size;
    long buf_count;
    long logfull_interval;
    long abort_interval;
    long cp_interval;
    char begin_addr[40];
    long begin_block;
    char end_addr[40];
    long end_block;
    char cp_addr[40];
    long cp_block;
    long next_cp_block;
    long reserved_blocks;
    long inuse_blocks;
    long avail_blocks;
  };
  extern int imp_lg_menu;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern int userSession;
  extern char *currentVnode;
  MenuEntry LgMenuList[]= {
	1,	"summary",
		"Display a Logging System Summary",
	2,	"header", 
		"Display the Log File Header",
	3,	"processes",
		"Display active processes in the logging system",
	4,	"databases",
		"Display logging information on each open database",
	5,	"transactions",
		"Display logging information on each transaction",
	0,	"no_entry",
		"No Entry"
};
/*
** Name:	implgmain.sc
**
** Logging System Menu for the IMA based IMP-like tool
**
## History:
##
##      04-Sep-03       (mdf040903)                     
##              Commit after calling imp_complete_vnode 
##              Commit after calling imp_reset_domain
*/
int
loggingMenu()
{
  int i;
  char buf[60];
/* # line 170 "implog.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lg_menu",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 173 "implog.sc" */	/* host code */
                makeIidbdbConnection(); 
/* # line 174 "implog.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lg_menu") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 176 "implog.sc" */	/* repeated select */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexExec(1,(char *)"implog1",7543,1277);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIexDefine(1,(char *)"implog1",7543,1277);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('ima_vnode')");
          IIexDefine(0,(char *)"implog1",7543,1277);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,59,buf);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 177 "implog.sc" */	/* host code */
		sqlerr_check();
		if (strcmp(buf,currentVnode) != 0) {
/* # line 179 "implog.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_reset_domain");
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 180 "implog.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_set_domain");
      IILQprvProcValio((char *)"entry",0,(short *)0,1,32,0,currentVnode);
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 182 "implog.sc" */	/* host code */
		} else {
/* # line 183 "implog.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_reset_domain");
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 184 "implog.sc" */	/* host code */
		}
/* # line 185 "implog.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 189 "implog.sc" */	/* inittable */
    {
      if (IItbinit((char *)"imp_lg_menu",(char *)"imp_menu",(char *)"r") != 0
      ) {
        IItfill();
      } /* IItbinit */
    }
/* # line 190 "implog.sc" */	/* host code */
		i = 0;
		while (LgMenuList[i].item_number != 0) {
/* # line 192 "implog.sc" */	/* loadtable */
    {
      if (IItbact((char *)"imp_lg_menu",(char *)"imp_menu",1) != 0) {
        IItcoputio((char *)"item_number",(short *)0,1,30,4,&
        LgMenuList[i].item_number);
        IItcoputio((char *)"short_name",(short *)0,1,32,0,
        LgMenuList[i].short_name);
        IItcoputio((char *)"long_name",(short *)0,1,32,0,
        LgMenuList[i].long_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 196 "implog.sc" */	/* host code */
			i++;
		}
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 202 "implog.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lg_menu",(char *)"imp_menu",-2) != 0) 
          {
            IItcogetio((short *)0,1,30,4,&i,(char *)"item_number");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 203 "implog.sc" */	/* host code */
		switch (i) {
		case 1:	processLogSummary();
			break;
		case 2:	processLogHeader();
			break;
		case 3:	processLogProcesses();
			break;
		case 4:	processLogDatabases();
			break;
		case 5:	processLogTransactions();
			break;
		default:
			sprintf(buf,"Unknown option %d",i);
/* # line 216 "implog.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(buf);
        }
/* # line 217 "implog.sc" */	/* host code */
		}
      } else if (IIretval() == 2) {
/* # line 222 "implog.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 225 "implog.sc" */	/* host code */
	if (strcmp(buf,currentVnode) == 0) {
/* # line 226 "implog.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_complete_vnode");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 227 "implog.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 230 "implog.sc" */	/* host code */
	}
	return(TRUE);
}
/*
** Name:	impdsplgd.sc
**
** Display the Loging system summary
**
## History:
##
*/
  extern int imp_lg_summary;
  extern int timeVal;
  struct lgd_stat lgd;
  struct lgd_stat new_lgd;
  int newLgStats = 0;
int
processLogSummary()
{
  char vnode_name[SERVER_NAME_LENGTH];
/* # line 262 "implog.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lg_summary",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
/* # line 265 "implog.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lg_summary") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 266 "implog.sc" */	/* host code */
		getLogSummary();
		displayLogSummary();
/* # line 268 "implog.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 273 "implog.sc" */	/* host code */
		getLogSummary();
		displayLogSummary();
      } else if (IIretval() == 2) {
/* # line 279 "implog.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 280 "implog.sc" */	/* display */
        {
          if (IInestmu() == 0) goto IIfdE3;
          goto IImuI3;
IIfdI3:;
IIfdB3:
          while (IIrunnest() != 0) {
            if (IIretval() == 1) {
/* # line 284 "implog.sc" */	/* host code */
			newLgStats = 0;
			resetLogSummary();
			getLogSummary();
			displayLogSummary();
/* # line 288 "implog.sc" */	/* set_frs */
              {
                if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                  IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
                } /* IIiqset */
              }
/* # line 289 "implog.sc" */	/* enddisplay */
              {
                if (1) goto IIfdF3;
              }
            } else if (IIretval() == 2) {
/* # line 294 "implog.sc" */	/* host code */
			newLgStats = 1;
			resetLogSummary();
			getLogSummary();
			copyLogSummary();
			getLogSummary();
			displayLogSummary();
/* # line 300 "implog.sc" */	/* set_frs */
              {
                if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                  IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
                } /* IIiqset */
              }
/* # line 301 "implog.sc" */	/* enddisplay */
              {
                if (1) goto IIfdF3;
              }
            } else {
              if (1) goto IIfdE3;
            } /* IIretval */
          } /* IIrunnest */
IIfdF3:
          if (IIchkfrm() == 0) goto IIfdB3;
          goto IIfdE3;
IImuI3:
          if (IIinitmu() == 0) goto IIfdE3;
          if (IInmuact((char *)"Since_Startup",(char *)0,2,2,1) == 0) goto 
          IIfdE3;
          if (IInmuact((char *)"Begin_Now",(char *)0,2,2,2) == 0) goto IIfdE3;
          if (IIendmu() == 0) goto IIfdE3;
          goto IIfdI3;
IIfdE3:;
          IIendnest();
        }
      } else if (IIretval() == 3) {
/* # line 308 "implog.sc" */	/* host code */
		getLogSummary();
		displayLogSummary();
      } else if (IIretval() == 4) {
/* # line 314 "implog.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 315 "implog.sc" */	/* breakdisplay */
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
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact((char *)"Interval",(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IIFRtoact(0,3) == 0) goto IIfdE2;
    if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 318 "implog.sc" */	/* host code */
	return(TRUE);
}
void resetLogSummary()
{
	lgd.db_adds = 0;
	lgd.db_removes = 0;
	lgd.tx_begins = 0;
	lgd.tx_ends = 0;
	lgd.log_writes = 0;
	lgd.log_write_ios = 0;
	lgd.log_read_ios = 0;
	lgd.log_waits = 0;
	lgd.log_split_waits = 0;
	lgd.log_free_waits = 0;
	lgd.log_stall_waits = 0;
	lgd.log_forces = 0;
	lgd.log_group_commit = 0;
	lgd.log_group_count = 0;
	lgd.check_commit_timer = 0;
	lgd.timer_write = 0;
	lgd.kbytes_written = 0;
	lgd.buffer_utilisation = 0;
	lgd.avg_txn_size = 0;
	lgd.block_size = 0;
	lgd.inconsistent_db = 0;
}
copyLogSummary()
{
	lgd.db_adds = new_lgd.db_adds;
	lgd.db_removes = new_lgd.db_removes;
	lgd.tx_begins = new_lgd.tx_begins;
	lgd.tx_ends = new_lgd.tx_ends;
	lgd.log_writes = new_lgd.log_writes;
	lgd.log_write_ios = new_lgd.log_write_ios;
	lgd.log_read_ios = new_lgd.log_read_ios;
	lgd.log_waits = new_lgd.log_waits;
	lgd.log_split_waits = new_lgd.log_split_waits;
	lgd.log_free_waits = new_lgd.log_free_waits;
	lgd.log_stall_waits = new_lgd.log_stall_waits;
	lgd.log_forces = new_lgd.log_forces;
	lgd.log_group_commit = new_lgd.log_group_commit;
	lgd.log_group_count = new_lgd.log_group_count;
	lgd.check_commit_timer = new_lgd.check_commit_timer;
	lgd.timer_write = new_lgd.timer_write;
	lgd.kbytes_written = new_lgd.kbytes_written;
	lgd.buffer_utilisation = new_lgd.buffer_utilisation;
	lgd.avg_txn_size = new_lgd.avg_txn_size;
	lgd.block_size = new_lgd.block_size;
	lgd.inconsistent_db = new_lgd.inconsistent_db;
}
void getLogSummary()
{
/* # line 374 "implog.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implog2",7544,7933);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implog2",7544,7933);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select lgd_stat_add, lgd_stat_remove, lgd_stat_begin, lgd_stat_end, l\
gd_stat_write, lgd_stat_writeio, lgd_stat_readio, lgd_stat_wait, lgd_s\
tat_split, lgd_stat_free_wait, lgd_stat_stall_wait, lgd_stat_force, lg\
d_stat_group_force, lgd_stat_group_count, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"lgd_stat_pgyback_check, lgd_stat_pgyback_write, lgd_stat_kbytes, lfb_\
hdr_lgh_size, lgd_stat_inconsist_db from imp_lgmo_lgd, imp_lgmo_lfb wh\
ere imp_lgmo_lgd.vnode=imp_lgmo_lfb.vnode and imp_lgmo_lfb.vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implog2",7544,7933);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&new_lgd.db_adds);
      IIgetdomio((short *)0,1,30,4,&new_lgd.db_removes);
      IIgetdomio((short *)0,1,30,4,&new_lgd.tx_begins);
      IIgetdomio((short *)0,1,31,4,&new_lgd.tx_ends);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_writes);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_write_ios);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_read_ios);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_waits);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_split_waits);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_free_waits);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_stall_waits);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_forces);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_group_commit);
      IIgetdomio((short *)0,1,30,4,&new_lgd.log_group_count);
      IIgetdomio((short *)0,1,30,4,&new_lgd.check_commit_timer);
      IIgetdomio((short *)0,1,30,4,&new_lgd.timer_write);
      IIgetdomio((short *)0,1,31,4,&new_lgd.kbytes_written);
      IIgetdomio((short *)0,1,30,4,&new_lgd.block_size);
      IIgetdomio((short *)0,1,30,4,&new_lgd.inconsistent_db);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 424 "implog.sc" */	/* host code */
	sqlerr_check();
	/* 
	** kbytes written is actually the number of VMS blocks
	** so....
	*/
	new_lgd.kbytes_written = new_lgd.kbytes_written / 2;
/*
new_lgd.buffer_utilisation = 12.5;
*/
new_lgd.buffer_utilisation =  (new_lgd.kbytes_written*100.00)
/*
 /
                        (new_lgd.log_write_ios*(new_lgd.block_size/1024.00))  
*/
;
new_lgd.avg_txn_size =  (new_lgd.kbytes_written * 1024.00/ new_lgd.tx_ends) ;
	new_lgd.db_adds -= lgd.db_adds;
	new_lgd.db_removes -= lgd.db_removes;
	new_lgd.tx_begins -= lgd.tx_begins;
	new_lgd.tx_ends -= lgd.tx_ends;
	new_lgd.log_writes -= lgd.log_writes;
	new_lgd.log_write_ios -= lgd.log_write_ios;
	new_lgd.log_read_ios -= lgd.log_read_ios;
	new_lgd.log_waits -= lgd.log_waits;
	new_lgd.log_split_waits -= lgd.log_split_waits;
	new_lgd.log_free_waits -= lgd.log_free_waits;
	new_lgd.log_stall_waits -= lgd.log_stall_waits;
	new_lgd.log_forces -= lgd.log_forces;
	new_lgd.log_group_commit -= lgd.log_group_commit;
	new_lgd.log_group_count -= lgd.log_group_count;
	new_lgd.check_commit_timer -= lgd.check_commit_timer;
	new_lgd.timer_write -= lgd.timer_write;
	new_lgd.kbytes_written -= lgd.kbytes_written;
        new_lgd.buffer_utilisation -=  lgd.buffer_utilisation;
        new_lgd.avg_txn_size -=  lgd.avg_txn_size;
	new_lgd.inconsistent_db -= lgd.inconsistent_db;
}
void displayLogSummary()
{
/* # line 469 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
    } /* IIfsetio */
  }
/* # line 471 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"db_adds",(short *)0,1,30,4,&new_lgd.db_adds);
    } /* IIfsetio */
  }
/* # line 473 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"db_removes",(short *)0,1,30,4,&new_lgd.db_removes);
    } /* IIfsetio */
  }
/* # line 475 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"tx_begins",(short *)0,1,30,4,&new_lgd.tx_begins);
    } /* IIfsetio */
  }
/* # line 477 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"tx_ends",(short *)0,1,31,4,&new_lgd.tx_ends);
    } /* IIfsetio */
  }
/* # line 479 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_writes",(short *)0,1,30,4,&new_lgd.log_writes);
    } /* IIfsetio */
  }
/* # line 481 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_write_ios",(short *)0,1,30,4,&
      new_lgd.log_write_ios);
    } /* IIfsetio */
  }
/* # line 483 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_read_ios",(short *)0,1,30,4,&
      new_lgd.log_read_ios);
    } /* IIfsetio */
  }
/* # line 485 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_waits",(short *)0,1,30,4,&new_lgd.log_waits);
    } /* IIfsetio */
  }
/* # line 487 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_split_waits",(short *)0,1,30,4,&
      new_lgd.log_split_waits);
    } /* IIfsetio */
  }
/* # line 489 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_free_waits",(short *)0,1,30,4,&
      new_lgd.log_free_waits);
    } /* IIfsetio */
  }
/* # line 491 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_stall_waits",(short *)0,1,30,4,&
      new_lgd.log_stall_waits);
    } /* IIfsetio */
  }
/* # line 493 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_forces",(short *)0,1,30,4,&new_lgd.log_forces);
    } /* IIfsetio */
  }
/* # line 495 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_group_commit",(short *)0,1,30,4,&
      new_lgd.log_group_commit);
    } /* IIfsetio */
  }
/* # line 497 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"log_group_count",(short *)0,1,30,4,&
      new_lgd.log_group_count);
    } /* IIfsetio */
  }
/* # line 499 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"check_commit_timer",(short *)0,1,30,4,&
      new_lgd.check_commit_timer);
    } /* IIfsetio */
  }
/* # line 501 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"timer_write",(short *)0,1,30,4,&new_lgd.timer_write);
    } /* IIfsetio */
  }
/* # line 503 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"kbytes_written",(short *)0,1,31,4,&
      new_lgd.kbytes_written);
    } /* IIfsetio */
  }
/* # line 505 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"block_size",(short *)0,1,30,4,&new_lgd.block_size);
    } /* IIfsetio */
  }
/* # line 507 "implog.sc" */	/* host code */
/*
	exec frs putform imp_lg_summary 
                (avg_txn_size =  :new_lgd.avg_txn_size);
	exec frs putform imp_lg_summary 
                (buffer_utilisation =  :new_lgd.buffer_utilisation);
*/
/* # line 513 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_summary") != 0) {
      IIputfldio((char *)"inconsistent_db",(short *)0,1,30,4,&
      new_lgd.inconsistent_db);
    } /* IIfsetio */
  }
/* # line 516 "implog.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 517 "implog.sc" */	/* host code */
}
  struct lg_header hdr;
  char barchart[BARCHART_LENGTH + 2];
int
processLogHeader()
{
/* # line 528 "implog.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lg_header",(char *)"r") == 0) goto IIfdE4;
    goto IImuI4;
IIfdI4:;
    if (IIfsetio((char *)0) == 0) goto IIfdE4;
/* # line 531 "implog.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lg_header") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 532 "implog.sc" */	/* host code */
		getHeaderInfo();
		displayHeaderInfo();
/* # line 534 "implog.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB4:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 539 "implog.sc" */	/* host code */
		getHeaderInfo();
		displayHeaderInfo();
      } else if (IIretval() == 2) {
		getHeaderInfo();
		displayHeaderInfo();
      } else if (IIretval() == 3) {
/* # line 551 "implog.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 552 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE4;
        }
      } else {
        if (1) goto IIfdE4;
      } /* IIretval */
    } /* IIrunform */
IIfdF4:
    if (IIchkfrm() == 0) goto IIfdB4;
    goto IIfdE4;
IImuI4:
    if (IIFRtoact(0,1) == 0) goto IIfdE4;
    if (IIinitmu() == 0) goto IIfdE4;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE4;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE4;
    if (IIendmu() == 0) goto IIfdE4;
    goto IIfdI4;
IIfdE4:;
    IIendfrm();
  }
/* # line 555 "implog.sc" */	/* host code */
	return(TRUE);
}
int
deriveBlockNo(str)
char	*str;
{
	long	discard;
	int	block;
	if ((sscanf(str,"<%ld,%d,%d>",&discard,&block,&discard)) != 3) {
		return(-1);
	} 
	return(block);
}
getHeaderInfo()
{
	int	last_cp;
	int	next_cp;
	int	start;
	int	end;
	int	reserve;
	int	i;
	int	j;
/* # line 581 "implog.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implog3",7545,26329);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implog3",7545,26329);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select lgd_status, lfb_hdr_lgh_count, lfb_hdr_lgh_size, lfb_buf_cnt, \
lfb_hdr_lgh_l_logfull, lfb_hdr_lgh_l_abort, lfb_hdr_lgh_l_cp, lfb_hdr_\
lgh_begin, lfb_hdr_lgh_end, lfb_hdr_lgh_cp, lfb_reserved_space from im\
p_lgmo_lgd, imp_lgmo_lfb where imp_lgmo_lgd.");
        IIwritio(0,(short *)0,1,32,0,(char *)
"vnode=imp_lgmo_lfb.vnode and imp_lgmo_lfb.vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implog3",7545,26329);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,49,hdr.status);
      IIgetdomio((short *)0,1,30,4,&hdr.block_count);
      IIgetdomio((short *)0,1,30,4,&hdr.block_size);
      IIgetdomio((short *)0,1,30,4,&hdr.buf_count);
      IIgetdomio((short *)0,1,30,4,&hdr.logfull_interval);
      IIgetdomio((short *)0,1,30,4,&hdr.abort_interval);
      IIgetdomio((short *)0,1,30,4,&hdr.cp_interval);
      IIgetdomio((short *)0,1,32,39,hdr.begin_addr);
      IIgetdomio((short *)0,1,32,39,hdr.end_addr);
      IIgetdomio((short *)0,1,32,39,hdr.cp_addr);
      IIgetdomio((short *)0,1,30,4,&hdr.reserved_blocks);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 611 "implog.sc" */	/* host code */
	sqlerr_check();
	/*
	** derive the rest of the required information
	*/
	hdr.begin_block = deriveBlockNo(hdr.begin_addr);
	hdr.end_block = deriveBlockNo(hdr.end_addr);
	hdr.cp_block = deriveBlockNo(hdr.cp_addr);
	hdr.reserved_blocks = hdr.reserved_blocks / hdr.block_size;
	hdr.inuse_blocks = (hdr.end_block > hdr.begin_block) ?
				hdr.end_block - hdr.begin_block :
				(hdr.block_count - hdr.begin_block) +
				hdr.end_block;
	hdr.avail_blocks = hdr.block_count - 
				(hdr.reserved_blocks + hdr.inuse_blocks);
	if ((hdr.cp_block + hdr.cp_interval) > hdr.block_count) {
		hdr.next_cp_block = (hdr.cp_block + hdr.cp_interval) 
					- hdr.block_count;
	} else {
		hdr.next_cp_block = (hdr.cp_block + hdr.cp_interval);
	}
	/*
	** calculate the barchart
	*/
	last_cp = (hdr.cp_block * BARCHART_LENGTH) / hdr.block_count;
	next_cp = (hdr.next_cp_block * BARCHART_LENGTH) / hdr.block_count;
	start = (hdr.begin_block * BARCHART_LENGTH) / hdr.block_count;
	end = (hdr.end_block * BARCHART_LENGTH) / hdr.block_count;
	/*
	** reserved blocks may be shown at the end of the normal blocks?
	*/
	reserve = (hdr.reserved_blocks * BARCHART_LENGTH) /
			hdr.block_count;
	for (i = 0; i < BARCHART_LENGTH; i++) {
		barchart[i] = ' ';
	}
	barchart[BARCHART_LENGTH + 1] = '\0';
	barchart[last_cp] = 'C';
	barchart[next_cp] = 'C';
	if (start == end) {
		barchart[start] = '*';
	} else if (start < end) {
		for (i = start; i <= end; i++) {
			barchart[i] = '-';
		}
		barchart[start] = '>';
		barchart[end] = '>';
	} else {
		for (i = start; i <= BARCHART_LENGTH; i++) {
			barchart[i] = '-';
		}
		for (i = 0; i < end; i++) {
			barchart[i] = '-';
		}
		barchart[start] = '>';
		barchart[end] = '>';
	}
	j = reserve;
	for (i = (start - 1); (i >= 0) && (j > 0); i--) {
		barchart[i] = 'R';
		j--;
	}
	if (j != 0) {
		for (i = BARCHART_LENGTH; j > 0; i--) {
			barchart[i] = 'R';
			j--;
		}
	}
	barchart[last_cp] = 'C';
	barchart[next_cp] = 'C';
}
void displayHeaderInfo()
{
  int pctInUse;
  char buf[30];
/* # line 695 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"status",(short *)0,1,32,0,hdr.status);
    } /* IIfsetio */
  }
/* # line 697 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"block_count",(short *)0,1,30,4,&hdr.block_count);
    } /* IIfsetio */
  }
/* # line 699 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"block_size",(short *)0,1,30,4,&hdr.block_size);
    } /* IIfsetio */
  }
/* # line 701 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"buffer_count",(short *)0,1,30,4,&hdr.buf_count);
    } /* IIfsetio */
  }
/* # line 703 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"logfull_interval",(short *)0,1,30,4,&
      hdr.logfull_interval);
    } /* IIfsetio */
  }
/* # line 705 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"abort_interval",(short *)0,1,30,4,&
      hdr.abort_interval);
    } /* IIfsetio */
  }
/* # line 707 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"cp_interval",(short *)0,1,30,4,&hdr.cp_interval);
    } /* IIfsetio */
  }
/* # line 709 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"bof",(short *)0,1,30,4,&hdr.begin_block);
    } /* IIfsetio */
  }
/* # line 711 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"eof",(short *)0,1,30,4,&hdr.end_block);
    } /* IIfsetio */
  }
/* # line 713 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"previous_cp",(short *)0,1,30,4,&hdr.cp_block);
    } /* IIfsetio */
  }
/* # line 715 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"next_cp",(short *)0,1,30,4,&hdr.next_cp_block);
    } /* IIfsetio */
  }
/* # line 717 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"reserved",(short *)0,1,30,4,&hdr.reserved_blocks);
    } /* IIfsetio */
  }
/* # line 719 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"in_use",(short *)0,1,30,4,&hdr.inuse_blocks);
    } /* IIfsetio */
  }
/* # line 721 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"available",(short *)0,1,30,4,&hdr.avail_blocks);
    } /* IIfsetio */
  }
/* # line 723 "implog.sc" */	/* host code */
	pctInUse = (100 * (hdr.inuse_blocks + hdr.reserved_blocks)) /
			hdr.block_count;
/* # line 725 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"log_pct",(short *)0,1,30,4,&pctInUse);
    } /* IIfsetio */
  }
/* # line 727 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"barchart",(short *)0,1,32,0,barchart);
    } /* IIfsetio */
  }
/* # line 730 "implog.sc" */	/* host code */
	/*
	** instead of calculating foce abort and logfull, simply 
	** look for the status
	*/
	if (strstr(hdr.status,"LOGFULL") != (char *) NULL) {
		strcpy(buf,"LOGFULL");
/* # line 736 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"emergency_status",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 738 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"imp_lg_header",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"emergency_status",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(16,(char *)"emergency_status",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 741 "implog.sc" */	/* host code */
	} else if (strstr(hdr.status,"FORCE") != (char *) NULL) {
		strcpy(buf,"FORCE ABORT");
/* # line 743 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lg_header") != 0) {
      IIputfldio((char *)"emergency_status",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 745 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"imp_lg_header",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"emergency_status",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(16,(char *)"emergency_status",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 748 "implog.sc" */	/* host code */
	} else {
/* # line 749 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"imp_lg_header",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"emergency_status",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(16,(char *)"emergency_status",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 752 "implog.sc" */	/* host code */
	}
/* # line 753 "implog.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 754 "implog.sc" */	/* host code */
}
int
processLogProcesses()
{
/* # line 759 "implog.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lg_processes",(char *)"r") == 0) goto IIfdE5;
    goto IImuI5;
IIfdI5:;
    if (IIfsetio((char *)0) == 0) goto IIfdE5;
/* # line 762 "implog.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lg_processes") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 763 "implog.sc" */	/* host code */
		displayLogProcesses();
/* # line 764 "implog.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB5:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 769 "implog.sc" */	/* host code */
		displayLogProcesses();
      } else if (IIretval() == 2) {
		displayLogProcesses();
      } else if (IIretval() == 3) {
/* # line 779 "implog.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 780 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE5;
        }
      } else {
        if (1) goto IIfdE5;
      } /* IIretval */
    } /* IIrunform */
IIfdF5:
    if (IIchkfrm() == 0) goto IIfdB5;
    goto IIfdE5;
IImuI5:
    if (IIinitmu() == 0) goto IIfdE5;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,1) == 0) goto IIfdE5;
    if (IIFRtoact(0,2) == 0) goto IIfdE5;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE5;
    if (IIendmu() == 0) goto IIfdE5;
    goto IIfdI5;
IIfdE5:;
    IIendfrm();
  }
/* # line 783 "implog.sc" */	/* host code */
	return(TRUE);
}
void displayLogProcesses()
{
  char computed_id[20];
  long id_id;
  long id_instance;
  long pid;
  int status_val;
  char status[30];
  long open_db;
  long writes;
  long forces;
  long waits;
  long begins;
  long ends;
  char typeBuf[100];
/* # line 805 "implog.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_lg_processes",(char *)"process_tbl",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 806 "implog.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implog4",7546,20730);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"implog4",7546,20730);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select lpb_id_id, lpb_id_instance, lpb_pid, lpb_status_num, lpb_lpd_c\
ount, lpb_stat_write, lpb_stat_force, lpb_stat_wait, lpb_stat_begin, l\
pb_stat_end from imp_lgmo_lpb where vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implog4",7546,20730);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&id_id);
      IIgetdomio((short *)0,1,30,4,&id_instance);
      IIgetdomio((short *)0,1,30,4,&pid);
      IIgetdomio((short *)0,1,30,4,&status_val);
      IIgetdomio((short *)0,1,30,4,&open_db);
      IIgetdomio((short *)0,1,30,4,&writes);
      IIgetdomio((short *)0,1,30,4,&forces);
      IIgetdomio((short *)0,1,30,4,&waits);
      IIgetdomio((short *)0,1,30,4,&begins);
      IIgetdomio((short *)0,1,30,4,&ends);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 833 "implog.sc" */	/* host code */
		sqlerr_check();
		sprintf(computed_id,"%lx",(65536 * id_id) + id_instance);
		decodeStatusVal(status_val,typeBuf);
/* # line 836 "implog.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_lg_processes",(char *)"process_tbl",1) != 0) 
        {
          IItcoputio((char *)"computed_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"pid",(short *)0,1,30,4,&pid);
          IItcoputio((char *)"process_type",(short *)0,1,32,0,typeBuf);
          IItcoputio((char *)"open_db",(short *)0,1,30,4,&open_db);
          IItcoputio((char *)"writes",(short *)0,1,30,4,&writes);
          IItcoputio((char *)"forces",(short *)0,1,30,4,&forces);
          IItcoputio((char *)"waits",(short *)0,1,30,4,&waits);
          IItcoputio((char *)"begins",(short *)0,1,30,4,&begins);
          IItcoputio((char *)"ends",(short *)0,1,30,4,&ends);
          IITBceColEnd();
        } /* IItbact */
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
/* # line 847 "implog.sc" */	/* host code */
	sqlerr_check();
/* # line 848 "implog.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 849 "implog.sc" */	/* host code */
}
void TransactionDetail(tx_id)
char	*tx_id;
{
  char l_session_id[40];
  int l_server_pid;
  char buf[100];
  char l_server_name[SERVER_NAME_LENGTH];
  SessEntry *current_sess;
  SessEntry *SessListHead;
/* # line 870 "implog.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_transaction_detail",(char *)"r") == 0) goto 
    IIfdE6;
    goto IImuI6;
IIfdI6:;
    if (IIfsetio((char *)0) == 0) goto IIfdE6;
/* # line 873 "implog.sc" */	/* host code */
        if ( DisplayTransactionDetail(tx_id) == FALSE) {    
/* # line 874 "implog.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE6;
    }
/* # line 875 "implog.sc" */	/* host code */
        }                                            
/* # line 876 "implog.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB6:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 881 "implog.sc" */	/* host code */
        if ( DisplayTransactionDetail(tx_id) == FALSE) {    
/* # line 882 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE6;
        }
/* # line 883 "implog.sc" */	/* host code */
        }                                            
      } else if (IIretval() == 2) {
/* # line 889 "implog.sc" */	/* getform */
        {
          if (IIfsetio((char *)"imp_transaction_detail") != 0) {
            IIgetfldio((short *)0,1,32,39,l_session_id,(char *)"lxb_si\
d");
          } /* IIfsetio */
        }
/* # line 890 "implog.sc" */	/* getform */
        {
          if (IIfsetio((char *)"imp_transaction_detail") != 0) {
            IIgetfldio((short *)0,1,30,4,&l_server_pid,(char *)"lxb_pi\
d");
          } /* IIfsetio */
        }
/* # line 892 "implog.sc" */	/* host code */
            getServerName(l_server_pid, l_server_name);
            /* current_sess = SessListHead; */
            if (current_sess = newSessEntry()) {                      
                strcpy(current_sess->sessionId,l_session_id );              
                }                                                     
            if (current_sess) {                                       
                    displaySessionDetail(current_sess,l_server_name);   
            }                                                         
            /* On return, refresh current screen */
            if ( DisplayTransactionDetail(tx_id) == FALSE) {    
/* # line 905 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE6;
        }
/* # line 906 "implog.sc" */	/* host code */
            }                                            
      } else if (IIretval() == 3) {
        if ( DisplayHexDump(tx_id) == FALSE) {    
/* # line 912 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE6;
        }
/* # line 913 "implog.sc" */	/* host code */
        }                                            
      } else if (IIretval() == 4) {
/* # line 918 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE6;
        }
      } else {
        if (1) goto IIfdE6;
      } /* IIretval */
    } /* IIrunform */
IIfdF6:
    if (IIchkfrm() == 0) goto IIfdB6;
    goto IIfdE6;
IImuI6:
    if (IIinitmu() == 0) goto IIfdE6;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,1) == 0) goto IIfdE6;
    if (IInmuact((char *)"Session_Info",(char *)0,2,2,2) == 0) goto IIfdE6;
    if (IInmuact((char *)"Dist_Txn_Hex_Dump",(char *)0,2,2,3) == 0) goto IIfdE6;
    if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE6;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE6;
    if (IIendmu() == 0) goto IIfdE6;
    goto IIfdI6;
IIfdE6:;
    IIendfrm();
  }
/* # line 920 "implog.sc" */	/* host code */
}
int
DisplayHexDump(p_tx_id)
  char *p_tx_id;
{
  int cnt;
  char msgbuf[100];
  char l_lxb_dis_tran_id_hexdump[350];
/* # line 935 "implog.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_transaction_hexdump",(char *)"r") == 0) goto 
    IIfdE7;
    goto IImuI7;
IIfdI7:;
    if (IIfsetio((char *)0) == 0) goto IIfdE7;
/* # line 938 "implog.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
/* # line 940 "implog.sc" */	/* repeated select */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexExec(1,(char *)"implog5",7547,23258);
        IIputdomio((short *)0,1,32,0,p_tx_id);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIexDefine(1,(char *)"implog5",7547,23258);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select lxb_dis_tran_id_hexdump from imp_lgmo_lxb where lxb_tran_id=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,p_tx_id);
          IIexDefine(0,(char *)"implog5",7547,23258);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,349,l_lxb_dis_tran_id_hexdump);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 946 "implog.sc" */	/* host code */
        sqlerr_check();
/* # line 947 "implog.sc" */	/* inquire_sql */
    {
      IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
    }
/* # line 948 "implog.sc" */	/* host code */
        if (cnt == 0) {                                              
            sprintf(msgbuf,"No information for Transaction id %s",p_tx_id); 
/* # line 950 "implog.sc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage(msgbuf);
    }
/* # line 951 "implog.sc" */	/* host code */
            return(FALSE);                                       
        }                                                            
/* # line 954 "implog.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_transaction_hexdump") != 0) {
        IIputfldio((char *)"lxb_dis_tran_id_hexdump",(short *)0,1,32,0,
        l_lxb_dis_tran_id_hexdump);
      } /* IIfsetio */
    }
IIfdB7:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 959 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE7;
        }
      } else {
        if (1) goto IIfdE7;
      } /* IIretval */
    } /* IIrunform */
IIfdF7:
    if (IIchkfrm() == 0) goto IIfdB7;
    goto IIfdE7;
IImuI7:
    if (IIinitmu() == 0) goto IIfdE7;
    if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE7;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE7;
    if (IIendmu() == 0) goto IIfdE7;
    goto IIfdI7;
IIfdE7:;
    IIendfrm();
  }
/* # line 961 "implog.sc" */	/* host code */
    return(TRUE);                                       
}
int
DisplayTransactionDetail(p_tx_id)
  char *p_tx_id;
{
  int cnt;
  char msgbuf[100];
  char l_lxb_db_name[32];
  char l_lxb_db_owner[32];
  char l_lxb_first_lga[32];
  int l_lxb_id_id;
  int l_lxb_id_instance;
  char l_lxb_is_prepared[2];
  char l_lxb_is_xa_dis_tran_id[2];
  char l_lxb_last_lga[32];
  char l_lxb_last_lsn[32];
  int l_lxb_pid;
  int l_lxb_reserved_space;
  char l_lxb_sid[40];
  int l_lxb_stat_force;
  int l_lxb_stat_split;
  int l_lxb_stat_wait;
  int l_lxb_stat_write;
  char l_lxb_status[101];
  char l_lxb_user_name[101];
  char l_lxb_wait_reason[21];
  char l_vnode[70];
  char l_lxb_tran_id[20];
/* # line 997 "implog.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implog6",7548,19958);
      IIputdomio((short *)0,1,32,0,p_tx_id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implog6",7548,19958);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select lxb_db_name, lxb_db_owner, lxb_first_lga, lxb_id_id, lxb_id_in\
stance, lxb_is_prepared, lxb_is_xa_dis_tran_id, lxb_last_lga, lxb_last\
_lsn, lxb_pid, lxb_reserved_space, lxb_sid, lxb_stat_force, lxb_stat_s\
plit, lxb_stat_wait, lxb_stat_write, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"lxb_status, lxb_user_name, lxb_wait_reason, vnode, lxb_tran_id from i\
mp_lgmo_lxb where lxb_tran_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,p_tx_id);
        IIexDefine(0,(char *)"implog6",7548,19958);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,31,l_lxb_db_name);
      IIgetdomio((short *)0,1,32,31,l_lxb_db_owner);
      IIgetdomio((short *)0,1,32,31,l_lxb_first_lga);
      IIgetdomio((short *)0,1,30,4,&l_lxb_id_id);
      IIgetdomio((short *)0,1,30,4,&l_lxb_id_instance);
      IIgetdomio((short *)0,1,32,1,l_lxb_is_prepared);
      IIgetdomio((short *)0,1,32,1,l_lxb_is_xa_dis_tran_id);
      IIgetdomio((short *)0,1,32,31,l_lxb_last_lga);
      IIgetdomio((short *)0,1,32,31,l_lxb_last_lsn);
      IIgetdomio((short *)0,1,30,4,&l_lxb_pid);
      IIgetdomio((short *)0,1,30,4,&l_lxb_reserved_space);
      IIgetdomio((short *)0,1,32,39,l_lxb_sid);
      IIgetdomio((short *)0,1,30,4,&l_lxb_stat_force);
      IIgetdomio((short *)0,1,30,4,&l_lxb_stat_split);
      IIgetdomio((short *)0,1,30,4,&l_lxb_stat_wait);
      IIgetdomio((short *)0,1,30,4,&l_lxb_stat_write);
      IIgetdomio((short *)0,1,32,100,l_lxb_status);
      IIgetdomio((short *)0,1,32,100,l_lxb_user_name);
      IIgetdomio((short *)0,1,32,20,l_lxb_wait_reason);
      IIgetdomio((short *)0,1,32,69,l_vnode);
      IIgetdomio((short *)0,1,32,19,l_lxb_tran_id);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1043 "implog.sc" */	/* host code */
    sqlerr_check();
/* # line 1044 "implog.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 1045 "implog.sc" */	/* host code */
    if (cnt == 0) {                                              
                sprintf(msgbuf,"No information for Transaction id %s",p_tx_id); 
/* # line 1047 "implog.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(msgbuf);
  }
/* # line 1048 "implog.sc" */	/* host code */
                return(FALSE);                                       
    }                                                            
/* # line 1051 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_db_name",(short *)0,1,32,0,l_lxb_db_name);
    } /* IIfsetio */
  }
/* # line 1052 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_db_owner",(short *)0,1,32,0,l_lxb_db_owner);
    } /* IIfsetio */
  }
/* # line 1053 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_first_lga",(short *)0,1,32,0,l_lxb_first_lga);
    } /* IIfsetio */
  }
/* # line 1054 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_tran_id",(short *)0,1,32,0,l_lxb_tran_id);
    } /* IIfsetio */
  }
/* # line 1055 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_id_id",(short *)0,1,30,4,&l_lxb_id_id);
    } /* IIfsetio */
  }
/* # line 1056 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_id_instance",(short *)0,1,30,4,&
      l_lxb_id_instance);
    } /* IIfsetio */
  }
/* # line 1057 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_is_prepared",(short *)0,1,32,0,l_lxb_is_prepared);
    } /* IIfsetio */
  }
/* # line 1058 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_is_xa_dis_tran_id",(short *)0,1,32,0,
      l_lxb_is_xa_dis_tran_id);
    } /* IIfsetio */
  }
/* # line 1059 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_last_lga",(short *)0,1,32,0,l_lxb_last_lga);
    } /* IIfsetio */
  }
/* # line 1060 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_last_lsn",(short *)0,1,32,0,l_lxb_last_lsn);
    } /* IIfsetio */
  }
/* # line 1061 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_pid",(short *)0,1,30,4,&l_lxb_pid);
    } /* IIfsetio */
  }
/* # line 1062 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_reserved_space",(short *)0,1,30,4,&
      l_lxb_reserved_space);
    } /* IIfsetio */
  }
/* # line 1063 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_sid",(short *)0,1,32,0,l_lxb_sid);
    } /* IIfsetio */
  }
/* # line 1064 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_stat_force",(short *)0,1,30,4,&l_lxb_stat_force);
    } /* IIfsetio */
  }
/* # line 1065 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_stat_split",(short *)0,1,30,4,&l_lxb_stat_split);
    } /* IIfsetio */
  }
/* # line 1066 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_stat_wait",(short *)0,1,30,4,&l_lxb_stat_wait);
    } /* IIfsetio */
  }
/* # line 1067 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_stat_write",(short *)0,1,30,4,&l_lxb_stat_write);
    } /* IIfsetio */
  }
/* # line 1068 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_status",(short *)0,1,32,0,l_lxb_status);
    } /* IIfsetio */
  }
/* # line 1069 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_user_name",(short *)0,1,32,0,l_lxb_user_name);
    } /* IIfsetio */
  }
/* # line 1070 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"lxb_wait_reason",(short *)0,1,32,0,l_lxb_wait_reason);
    } /* IIfsetio */
  }
/* # line 1071 "implog.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_transaction_detail") != 0) {
      IIputfldio((char *)"vnode",(short *)0,1,32,0,l_vnode);
    } /* IIfsetio */
  }
/* # line 1073 "implog.sc" */	/* host code */
    if   (strcmp(l_lxb_is_xa_dis_tran_id,"N") == 0) {
/* # line 1074 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Dist_Txn_Hex_Dump",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1075 "implog.sc" */	/* host code */
    }
    else
    {
/* # line 1078 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Dist_Txn_Hex_Dump",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1079 "implog.sc" */	/* host code */
    }
/* # line 1080 "implog.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1082 "implog.sc" */	/* host code */
    return(TRUE);                                       
}
int
decodeStatusVal(val,buf)
int	val;
char	*buf;
{
	/* note that we do not check the validity of buf */
	*buf = '\0';
	if (val & LPB_MASTER) {
		strcat(buf,"RCP ");
	}
	if (val & LPB_ARCHIVER) {
		strcat(buf,"ACP ");
	}
	if (val & LPB_FCT) {
		strcat(buf,"FCTDBMS ");
	}
	if (val & LPB_RUNAWAY) {
		strcat(buf,"RUNAWAY ");
	}
	if (val & LPB_SLAVE) {
		strcat(buf,"DBMS ");
	}
	if (val & LPB_CKPDB) {
		strcat(buf,"CKP ");
	}
	if (val & LPB_VOID) {
		strcat(buf,"VOID ");
	}
	if (val & LPB_SHARED_BUFMGR) {
		strcat(buf,"SHARED ");
	}
	if (val & LPB_IDLE) {
		strcat(buf,"IDLE ");
	}
	if (val & LPB_DEAD) {
		strcat(buf,"DEAD ");
	}
	if (val & LPB_DYING) {
		strcat(buf,"DYING ");
	}
	if (val & LPB_FOREIGN_RUNDOWN) {
		strcat(buf,"RUNDOWN ");
	}
	if (val & LPB_CPAGENT) {
		strcat(buf,"CPAGENT ");
	}
	return(TRUE);
}
int
processLogDatabases()
{
/* # line 1141 "implog.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lg_databases",(char *)"r") == 0) goto IIfdE8;
    goto IImuI8;
IIfdI8:;
    if (IIfsetio((char *)0) == 0) goto IIfdE8;
/* # line 1144 "implog.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lg_processes") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 1145 "implog.sc" */	/* host code */
		displayLogDatabases();
/* # line 1146 "implog.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB8:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1151 "implog.sc" */	/* host code */
		displayLogDatabases();
      } else if (IIretval() == 2) {
		displayLogDatabases();
      } else if (IIretval() == 3) {
/* # line 1161 "implog.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 1162 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE8;
        }
      } else {
        if (1) goto IIfdE8;
      } /* IIretval */
    } /* IIrunform */
IIfdF8:
    if (IIchkfrm() == 0) goto IIfdB8;
    goto IIfdE8;
IImuI8:
    if (IIinitmu() == 0) goto IIfdE8;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,1) == 0) goto IIfdE8;
    if (IIFRtoact(0,2) == 0) goto IIfdE8;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE8;
    if (IIendmu() == 0) goto IIfdE8;
    goto IIfdI8;
IIfdE8:;
    IIendfrm();
  }
/* # line 1165 "implog.sc" */	/* host code */
	return(TRUE);
}
void displayLogDatabases()
{
  char dbname[40];
  char dbstatus[40];
  long tx_count;
  long db_begins;
  long db_ends;
  long db_reads;
  long db_writes;
/* # line 1180 "implog.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_lg_databases",(char *)"logdb_tbl",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1181 "implog.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implog7",7549,14037);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"implog7",7549,14037);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select ldb_db_name, ldb_status, ldb_lxb_count, ldb_stat_begin, ldb_st\
at_end, ldb_stat_read, ldb_stat_write from imp_lgmo_ldb where vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implog7",7549,14037);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,39,dbname);
      IIgetdomio((short *)0,1,32,39,dbstatus);
      IIgetdomio((short *)0,1,30,4,&tx_count);
      IIgetdomio((short *)0,1,30,4,&db_begins);
      IIgetdomio((short *)0,1,30,4,&db_ends);
      IIgetdomio((short *)0,1,30,4,&db_reads);
      IIgetdomio((short *)0,1,30,4,&db_writes);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 1202 "implog.sc" */	/* host code */
		sqlerr_check();
/* # line 1203 "implog.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_lg_databases",(char *)"logdb_tbl",1) != 0) {
          IItcoputio((char *)"dbname",(short *)0,1,32,0,dbname);
          IItcoputio((char *)"dbstatus",(short *)0,1,32,0,dbstatus);
          IItcoputio((char *)"tx_count",(short *)0,1,30,4,&tx_count);
          IItcoputio((char *)"begins",(short *)0,1,30,4,&db_begins);
          IItcoputio((char *)"ends",(short *)0,1,30,4,&db_ends);
          IItcoputio((char *)"reads",(short *)0,1,30,4,&db_reads);
          IItcoputio((char *)"writes",(short *)0,1,30,4,&db_writes);
          IITBceColEnd();
        } /* IItbact */
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
/* # line 1212 "implog.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1213 "implog.sc" */	/* host code */
}
int
processLogTransactions()
{
  char txn_id[40];
  char session_id[40];
  int server_pid;
  int llb_id_id;
  char server_name[SERVER_NAME_LENGTH];
  char buf[100];
  SessEntry *current_sess;
  SessEntry *SessListHead;
/* # line 1241 "implog.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lg_transactions",(char *)"r") == 0) goto IIfdE9;
    goto IImuI9;
IIfdI9:;
    if (IIfsetio((char *)0) == 0) goto IIfdE9;
/* # line 1244 "implog.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lg_transactions") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 1245 "implog.sc" */	/* host code */
		displayLogTransactions();
/* # line 1246 "implog.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 1247 "implog.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB9:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1257 "implog.sc" */	/* host code */
		displayLogTransactions();
      } else if (IIretval() == 2) {
		displayLogTransactions();
      } else if (IIretval() == 3) {
/* # line 1267 "implog.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lg_transactions",(char *)"log_t\
xtbl",-2) != 0) {
            IItcogetio((short *)0,1,32,39,txn_id,(char *)"ext_tx_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1269 "implog.sc" */	/* host code */
	    TransactionDetail(txn_id);
	    displayLogTransactions();
      } else if (IIretval() == 4) {
/* # line 1275 "implog.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lg_transactions",(char *)"log_t\
xtbl",-2) != 0) {
            IItcogetio((short *)0,1,32,39,txn_id,(char *)"ext_tx_id");
            IItcogetio((short *)0,1,32,39,session_id,(char *)"session_\
id");
            IItcogetio((short *)0,1,30,4,&server_pid,(char *)"server_p\
id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1281 "implog.sc" */	/* select */
        {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select llb_id_id from imp_lkmo_llb llb1 where vnode=");
          IIputdomio((short *)0,1,32,0,currentVnode);
          IIwritio(0,(short *)0,1,32,0,(char *)"and llb_pid=");
          IIputdomio((short *)0,1,30,4,&server_pid);
          IIwritio(0,(short *)0,1,32,0,(char *)"and llb_sid=");
          IIputdomio((short *)0,1,32,0,session_id);
          IIwritio(0,(short *)0,1,32,0,(char *)
"and llb_related_llb_id_id in(select llb_id_id from imp_lkmo_llb llb2 \
where vnode=llb1.vnode and llb_pid=llb1.llb_pid and llb_sid=llb1.llb_s\
id)");
          IIretinit((char *)0,0);
          if (IInextget() != 0) {
            IIgetdomio((short *)0,1,30,4,&llb_id_id);
          } /* IInextget */
          IIsqFlush((char *)0,0);
        }
/* # line 1294 "implog.sc" */	/* host code */
             if (sqlerr_check() == NO_ROWS) {                                
/* # line 1295 "implog.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
        }
/* # line 1296 "implog.sc" */	/* host code */
                sprintf(buf,"No Session information for Transaction id %s",txn_id); 
/* # line 1297 "implog.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(buf);
        }
/* # line 1298 "implog.sc" */	/* host code */
             }                                                               
             else {
/* # line 1300 "implog.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
        }
/* # line 1301 "implog.sc" */	/* host code */
                examineLockList(server_pid,session_id,llb_id_id) ;
             }                                                               
      } else if (IIretval() == 5) {
/* # line 1307 "implog.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lg_transactions",(char *)"log_t\
xtbl",-2) != 0) {
            IItcogetio((short *)0,1,32,39,txn_id,(char *)"ext_tx_id");
            IItcogetio((short *)0,1,32,39,session_id,(char *)"session_\
id");
            IItcogetio((short *)0,1,30,4,&server_pid,(char *)"server_p\
id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1313 "implog.sc" */	/* host code */
            getServerName(server_pid, server_name);
            /* current_sess = SessListHead; */
            if (current_sess = newSessEntry()) {                      
                strcpy(current_sess->sessionId,session_id );              
                }                                                     
            if (current_sess) {                                       
                    displaySessionDetail(current_sess,server_name);   
            }                                                         
            displayLogTransactions();
      } else if (IIretval() == 6) {
/* # line 1330 "implog.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 1331 "implog.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE9;
        }
      } else {
        if (1) goto IIfdE9;
      } /* IIretval */
    } /* IIrunform */
IIfdF9:
    if (IIchkfrm() == 0) goto IIfdB9;
    goto IIfdE9;
IImuI9:
    if (IIFRtoact(0,1) == 0) goto IIfdE9;
    if (IIinitmu() == 0) goto IIfdE9;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE9;
    if (IInmuact((char *)"Tx_Detail",(char *)0,2,2,3) == 0) goto IIfdE9;
    if (IInmuact((char *)"Lock_Info",(char *)0,2,2,4) == 0) goto IIfdE9;
    if (IInmuact((char *)"Session_Info",(char *)0,2,2,5) == 0) goto IIfdE9;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE9;
    if (IIendmu() == 0) goto IIfdE9;
    goto IIfdI9;
IIfdE9:;
    IIendfrm();
  }
/* # line 1335 "implog.sc" */	/* host code */
	return(TRUE);
}
void displayLogTransactions()
{
  int server_pid;
  char session_id[40];
  int cnt;
  char tbl_tx_id[40];
  int h_row;
  char tx_id[40];
  char dbname[40];
  char txstatus[40];
  char txname[40];
  long db_splits;
  long db_writes;
    /* 
##mdf040903. 
Remember the current tx_id so we can scroll back to it */
/* # line 1367 "implog.sc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,(char *)"imp_lg_transactions",(char *)0, (char *)0) != 0) 
    {
      IIiqfsio((short *)0,1,30,4,&h_row,32,(char *)"log_txtbl",0);
    } /* IIiqset */
  }
/* # line 1369 "implog.sc" */	/* host code */
        if (h_row > 0)
    {
/* # line 1371 "implog.sc" */	/* getrow */
  {
    if (IItbsetio(2,(char *)"imp_lg_transactions",(char *)"log_txtbl",-2) != 0
    ) {
      IItcogetio((short *)0,1,32,39,tbl_tx_id,(char *)"ext_tx_id");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 1373 "implog.sc" */	/* host code */
    }
    h_row = 0;
/* # line 1375 "implog.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_lg_transactions",(char *)"log_txtbl",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1376 "implog.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implog8",7550,24052);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"implog8",7550,24052);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select lxb_pid, lxb_sid, lxb_tran_id, lxb_db_name, lxb_user_name, lxb\
_status, lxb_stat_write, lxb_stat_split from imp_lgmo_lxb where vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and lxb_status like 'ACTIVE%' order by lxb_stat_write desc");
        IIexDefine(0,(char *)"implog8",7550,24052);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&server_pid);
      IIgetdomio((short *)0,1,32,39,session_id);
      IIgetdomio((short *)0,1,32,39,tx_id);
      IIgetdomio((short *)0,1,32,39,dbname);
      IIgetdomio((short *)0,1,32,39,txname);
      IIgetdomio((short *)0,1,32,39,txstatus);
      IIgetdomio((short *)0,1,30,4,&db_writes);
      IIgetdomio((short *)0,1,30,4,&db_splits);
      if (IIerrtest() != 0) goto IIrtB7;
/* # line 1409 "implog.sc" */	/* host code */
		sqlerr_check();
/* # line 1410 "implog.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_lg_transactions",(char *)"log_txtbl",1) != 0
        ) {
          IItcoputio((char *)"server_pid",(short *)0,1,30,4,&server_pid);
          IItcoputio((char *)"session_id",(short *)0,1,32,0,session_id);
          IItcoputio((char *)"ext_tx_id",(short *)0,1,32,0,tx_id);
          IItcoputio((char *)"database",(short *)0,1,32,0,dbname);
          IItcoputio((char *)"username",(short *)0,1,32,0,txname);
          IItcoputio((char *)"status",(short *)0,1,32,0,txstatus);
          IItcoputio((char *)"writes",(short *)0,1,30,4,&db_writes);
          IItcoputio((char *)"splits",(short *)0,1,30,4,&db_splits);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1423 "implog.sc" */	/* host code */
                if (strcmp(tx_id, tbl_tx_id)==0 )
                {
/* # line 1425 "implog.sc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,(char *)"imp_lg_transactions",(char *)0, (char *)0) !=
         0) {
          IIiqfsio((short *)0,1,30,4,&h_row,32,(char *)"log_txtbl",0);
        } /* IIiqset */
      }
/* # line 1427 "implog.sc" */	/* host code */
                }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
	sqlerr_check();
/* # line 1430 "implog.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 1433 "implog.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1436 "implog.sc" */	/* host code */
        if (cnt == 0) {                    
/* # line 1437 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Tx_Detail",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1438 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Lock_Info",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1439 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Session_Info",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1440 "implog.sc" */	/* host code */
                }                          
        else {                             
/* # line 1442 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Tx_Detail",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1443 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Lock_Info",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1444 "implog.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Session_Info",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1445 "implog.sc" */	/* host code */
            if (h_row > 0) {                    
/* # line 1446 "implog.sc" */	/* scroll */
  {
    if (IItbsetio(1,(char *)"imp_lg_transactions",(char *)"log_txtbl",-3) != 0
    ) {
      IItbsmode((char *)"to");
      if (IItscroll(0,h_row) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 1447 "implog.sc" */	/* host code */
                } 
            }                          
/* # line 1449 "implog.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1450 "implog.sc" */	/* host code */
}
void getServerName(p_server_pid, p_server_name)
  int p_server_pid;
  char *p_server_name;
{                                   
  char msgbuf[100];
  char l_server_name[20];
    /*
    ** Based on the tx_id, and the session_info
    ** get the server listen address
    */
/* # line 1467 "implog.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implog9",7551,25302);
      IIputdomio((short *)0,1,30,4,&p_server_pid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implog9",7551,25302);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select server from imp_srv_block where server_pid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&p_server_pid);
        IIexDefine(0,(char *)"implog9",7551,25302);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,l_server_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1472 "implog.sc" */	/* host code */
    sqlerr_check();
    strcpy(p_server_name,l_server_name );              
}
