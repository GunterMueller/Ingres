# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	impio.sc
**
** This file contains all of the routines needed to support the "DI" 
** Subcomponent of the IMA based IMP lookalike tools.
**
## History:
##
##	07-Jul-95	(johna)
##		Created.
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##	04-Sep-03   (flomi02) mdf040903
##		Added link to Session Info
##      06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
##	08-Oct-2007 (hanje04)
##	    SIR 114907
##	    Remove timebuf as it's not referenced and causes compilation
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
SessEntry *newSessEntry();
  extern int ima_dio;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern int userSession;
  extern char *currentVnode;
  extern char bar_chart[51];
  extern float pct_used;
/*
** Name:	impiomon.sc
**
** Display the relative I/O rates for this server
**
*/
  extern int timeVal;
  static int highVal = 0;
int
processIoMonitor(server)
  GcnEntry *server;
{
  char server_name[SERVER_NAME_LENGTH];
  char thread[20];
  int scb_pid;
  char msgbuf[40];
  SessEntry *current_sess;
  SessEntry *SessListHead;
/* # line 90 "impio.sc" */	/* set_sql */
  {
    IILQssSetSqlio(46,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 91 "impio.sc" */	/* host code */
        strcpy(server_name,currentVnode);   
	strcat(server_name,"::/@");
	strcat(server_name,server->serverAddress);
/* # line 95 "impio.sc" */	/* declare */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"declare global temporary table session.dio_table(l_server varchar(25)\
, l_session varchar(25), l_terminal varchar(50), l_user varchar(50), l\
_inuse integer, l_dio integer, l_old_dio integer)on commit preserve ro\
ws with norecovery");
    IIsyncup((char *)0,0);
  }
/* # line 106 "impio.sc" */	/* display */
  {
    if (IIdispfrm((char *)"ima_dio",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 109 "impio.sc" */	/* putform */
    {
      if (IIfsetio((char *)"ima_dio") != 0) {
        IIputfldio((char *)"server",(short *)0,1,32,0,server_name);
      } /* IIfsetio */
    }
/* # line 110 "impio.sc" */	/* clear */
    {
      IIfldclear((char *)"highval");
    }
/* # line 111 "impio.sc" */	/* host code */
	highVal = 0;                                
 /*
## mdf040903
*/
		displayIoSummary(server_name);
/* # line 116 "impio.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 122 "impio.sc" */	/* host code */
		displayIoSummary(server_name);
      } else if (IIretval() == 2) {
		displayIoSummary(server_name);
      } else if (IIretval() == 3) {
/* # line 132 "impio.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"ima_dio",(char *)"ima_dio",-2) != 0) {
            IItcogetio((short *)0,1,32,19,thread,(char *)"session_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 135 "impio.sc" */	/* host code */
            /* current_sess = SessListHead; */
            if (current_sess = newSessEntry()) {                    
                strcpy(current_sess->sessionId,thread );            
                /*current_sess->scb_pid = scb_pid;     
/*
##mdf240402
*/  
                }                                                   
            if (current_sess) {                                   
                    displaySessionDetail(current_sess,server_name);  
            }                                                     
      } else if (IIretval() == 4) {
	    highVal = 0;
/* # line 154 "impio.sc" */	/* clear */
        {
          IIfldclear((char *)"highval");
        }
      } else if (IIretval() == 5) {
/* # line 159 "impio.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 160 "impio.sc" */	/* drop */
        {
          IIsqInit(&sqlca);
          IIwritio(0,(short *)0,1,32,0,(char *)
"drop table session.dio_table");
          IIsyncup((char *)0,0);
        }
/* # line 161 "impio.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Session_Info",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Reset",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 164 "impio.sc" */	/* host code */
	return(TRUE);
}
displayIoSummary(server_name)
  char *server_name;
{
  int i;
  int cnt;
  int blks;
  int j;
  float scale;
  char msgBuf[100];
  char local_server[50];
  char local_terminal[50];
  char local_user[50];
  char local_session[25];
  char local_euser[50];
  int local_cpu;
  int local_new_dio;
  int local_old_dio;
  int local_dio;
  int local_bio;
/* # line 191 "impio.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update session.dio_table set l_inuse=0");
    IIsyncup((char *)0,0);
  }
/* # line 192 "impio.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 193 "impio.sc" */	/* set_frs */
  {
    if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
      IIstfsio(67,(char *)"Session_Info",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 207 "impio.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"dio_csr",7546,3550);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select s.server, s.session_id, terminal, username, dio from imp_scs_s\
essions s, imp_cs_sessions c where c.server=s.server and s.session_id=\
c.session_id and s.server=");
    IIputdomio((short *)0,1,32,0,server_name);
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"dio_csr",7546,3550);
  }
/* # line 208 "impio.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"dio_csr",7546,3550,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,49,local_server);
      IIcsGetio((short *)0,1,32,24,local_session);
      IIcsGetio((short *)0,1,32,49,local_terminal);
      IIcsGetio((short *)0,1,32,49,local_user);
      IIcsGetio((short *)0,1,30,4,&local_dio);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 215 "impio.sc" */	/* host code */
    while (sqlca.sqlcode == 0) 
    {
        /*
        ** look for the row in the session.dio_table
        ** - if it exists, calculate the new cpu
        ** otherwise - insert a new row
        */
/* # line 222 "impio.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update session.dio_table set l_old_dio=l_dio, l_dio=");
    IIputdomio((short *)0,1,30,4,&local_dio);
    IIwritio(0,(short *)0,1,32,0,(char *)", l_inuse=1 where l_server=");
    IIputdomio((short *)0,1,32,0,local_server);
    IIwritio(0,(short *)0,1,32,0,(char *)"and l_session=");
    IIputdomio((short *)0,1,32,0,local_session);
    IIwritio(0,(short *)0,1,32,0,(char *)"and l_terminal=");
    IIputdomio((short *)0,1,32,0,local_terminal);
    IIsyncup((char *)0,0);
  }
/* # line 230 "impio.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 231 "impio.sc" */	/* host code */
        if (cnt == 0) {
/* # line 232 "impio.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into session.dio_table values(");
    IIputdomio((short *)0,1,32,0,local_server);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,local_session);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,local_terminal);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,local_user);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1, ");
    IIputdomio((short *)0,1,30,4,&local_dio);
    IIwritio(0,(short *)0,1,32,0,(char *)", 0)");
    IIsyncup((char *)0,0);
  }
/* # line 240 "impio.sc" */	/* host code */
        }
/* # line 242 "impio.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"dio_csr",7546,3550,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,49,local_server);
      IIcsGetio((short *)0,1,32,24,local_session);
      IIcsGetio((short *)0,1,32,49,local_terminal);
      IIcsGetio((short *)0,1,32,49,local_user);
      IIcsGetio((short *)0,1,30,4,&local_dio);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 248 "impio.sc" */	/* host code */
    }
/* # line 249 "impio.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"dio_csr",7546,3550);
  }
/* # line 251 "impio.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from session.dio_table where l_inuse=0");
    IIsyncup((char *)0,0);
  }
/* # line 252 "impio.sc" */	/* host code */
    sqlerr_check();
/* # line 253 "impio.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 255 "impio.sc" */	/* inittable */
  {
    if (IItbinit((char *)"ima_dio",(char *)"ima_dio",(char *)"f") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 256 "impio.sc" */	/* host code */
    i = 1;
/* # line 257 "impio.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select l_server, l_session, l_terminal, l_user, l_dio, l_old_dio, (l_\
dio -l_old_dio)as l_new_dio from session.dio_table order by l_new_dio \
desc");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,49,local_server);
      IIgetdomio((short *)0,1,32,24,local_session);
      IIgetdomio((short *)0,1,32,49,local_terminal);
      IIgetdomio((short *)0,1,32,49,local_user);
      IIgetdomio((short *)0,1,30,4,&local_dio);
      IIgetdomio((short *)0,1,30,4,&local_old_dio);
      IIgetdomio((short *)0,1,30,4,&local_new_dio);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 279 "impio.sc" */	/* host code */
        if ((local_new_dio != 0) && (local_old_dio != 0)) 
        {
            /*
            ** calc the barchart
            */
            if (i == 1) {
                cnt = local_new_dio;
                if (cnt > highVal) {
                    highVal = cnt;
                } else {
                    cnt = highVal;
                }
            }
            /*
            ** fill the barchart - top row is always full
            */
            scale = ((float) local_new_dio / cnt);
            blks = (scale * 40) ;
            sprintf(msgBuf,"Scale %f, blks %d",scale,blks); 
            for (j = 0; j < blks; j++) {
                bar_chart[j] = '%';
            }
            bar_chart[j] = '\0';
/* # line 302 "impio.sc" */	/* insertrow */
      {
        if (IItbsetio(5,(char *)"ima_dio",(char *)"ima_dio",-2) != 0) {
          if (IItinsert() != 0) {
            IItcoputio((char *)"session_id",(short *)0,1,32,0,local_session);
            IItcoputio((char *)"terminal",(short *)0,1,32,0,local_terminal);
            IItcoputio((char *)"username",(short *)0,1,32,0,local_user);
            IItcoputio((char *)"barchart",(short *)0,1,32,0,bar_chart);
            IItcoputio((char *)"io_rate",(short *)0,1,30,4,&local_new_dio);
            IITBceColEnd();
          } /* IItinsert */
        } /* IItbsetio */
      }
/* # line 308 "impio.sc" */	/* host code */
            i++;
/* # line 309 "impio.sc" */	/* set_frs */
      {
        if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
          IIstfsio(67,(char *)"Session_Info",0,(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 310 "impio.sc" */	/* host code */
        }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    sqlerr_check();
/* # line 313 "impio.sc" */	/* putform */
  {
    if (IIfsetio((char *)"ima_dio") != 0) {
      IIputfldio((char *)"highval",(short *)0,1,30,4,&highVal);
    } /* IIfsetio */
  }
/* # line 314 "impio.sc" */	/* scroll */
  {
    if (IItbsetio(1,(char *)"ima_dio",(char *)"ima_dio",-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,1) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 315 "impio.sc" */	/* host code */
/*
##mdf040903
*/ 
}
