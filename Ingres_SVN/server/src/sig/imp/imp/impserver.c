# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	impserver.sc
**
** This file contains all of the routines needed to support the "SERVER" menu
** tree of the IMA based IMP lookalike tools.
**
**
** The original intention with the IMP tool was to build in-memory lists of
** everything - in a similar way to IPM. This is the only file that actually 
** does this - all the other routines simply use the forms as the method of
** storage.
** 
** Even if the lists were to be used - it would be an ideal to rewrite using
** QU.
**
## History:
##
##	10-feb-94	(johna)
##			created from routines in individual files
##			impdspsvr.sc
##			impdspsess.sc
##			impsesdet.sc
##			impsvrdet.sc
##
##	18-mar-94	(johna)
##			reworked to use better use the table keys to
##			avoid scans and to remove single vnode dependencies.
##	23-Mar-94	(johna)
##			removed references to "active sessions" and the query
##			flattening params
##	04-may-94	(johna)
##			some comments
##	30-Nov-94 	(johna)
##			added client_info popup.
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##      11-Nov-2002 (fanra01)
##          Bug 109104
##          Extend local variable for server_name so it doesn't run into
##          session_id.
##	03-Sep-03 	(flomi02) 
##mdf040903
##			added lastquery information
##	26-Mar-04 	(srisu02) 
##mdf260304
##			added call to newSessEntry()
##      06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
##      05-Oct-04    (srisu02)
##              Make function displayGccInfo() return different values
##              when there is information to display about the Net server
##              and when there is not. Based on this value, the Net server
##              'More_info' screen may or may not be displayed
##      05-Oct-04       (srisu02)
##              Removed unnecessary comments, corrected spelling mistakes 
##		in the code comments
## 17-Aug-2005  (hanje04)
##		BUG:115067
##		Make thread (in displayServerSessions()) 32bytes to handle
##		64bit session ID's
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
  extern int inFormsMode;
  extern int imp_server_list;
  extern int imp_query_info;
  extern int imp_server_popup;
  extern int imp_server_parms;
  extern int imp_session_list;
  extern int imp_session_detail;
  extern int imp_client_info;
  extern int timeVal;
  extern GcnEntry *GcnListHead;
  extern SessEntry *SessListHead;
  extern char *currentVnode;
  SessEntry l_sess;
/* prototypes */
void getGcnList();
void displayGcnList();
void clearGcnEntryTable();
void display_details();
void getSessList();
void displaySessList();
void clearSessnEntryTable();
void displaySession();
void toggle_server_cols();
int displayGccInfo();
/*
** Name:	processServerList
**
** Display the list of known servers for the new IMA based IPM lookalike tool.
**
## History:
##
## 26-jan-94	(johna)
##		started from pervious ad-hoc programs
## 03-Feb-94	(johna)
##		added Control menuitem 
## 10-feb-94	(johna)
##		Moved into the impserver.sc file
*/
int
processServerList()
{
  GcnEntry *current;
  char tserverAddress[SERVER_NAME_LENGTH];
  char addr[SERVER_NAME_LENGTH];
  char vnode[SERVER_NAME_LENGTH];
/* # line 138 "impserver.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_server_list",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 141 "impserver.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_server_list") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 142 "impserver.sc" */	/* host code */
		getGcnList();
/* # line 143 "impserver.sc" */	/* inittable */
    {
      if (IItbinit((char *)"imp_server_list",(char *)"gcn_entry_tbl",(char *)"\
r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 144 "impserver.sc" */	/* host code */
		displayGcnList();
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 149 "impserver.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_server_list",(char *)"gcn_entry\
_tbl",-2) != 0) {
            IItcogetio((short *)0,1,32,SERVER_NAME_LENGTH-1,addr,(char *)
"serveraddress");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 151 "impserver.sc" */	/* host code */
		current = GcnListHead;
		while (current != (GcnEntry *) NULL) {
			strcpy(tserverAddress,current->serverAddress);
			if (strncmp(current->serverAddress,addr,strlen(addr)) == 0) {
				break;
			} 
			current = current->n;
		}
		if (current) {
                        if (strcmp(current->serverClass,"IINMSVR") == 0) 
                        {
/* # line 162 "impserver.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"Name Servers do not have more info");
        }
/* # line 164 "impserver.sc" */	/* host code */
			} 
			else
			if ((strcmp(current->serverClass,"COMSVR") == 0) ) {
				impGccInfo(current);
			} 
                        else {
				displayServerPopup(current);
			}
		}
      } else if (IIretval() == 2) {
/* # line 177 "impserver.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_server_list",(char *)"gcn_entry\
_tbl",-2) != 0) {
            IItcogetio((short *)0,1,32,SERVER_NAME_LENGTH-1,addr,(char *)
"serveraddress");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 179 "impserver.sc" */	/* host code */
		current = GcnListHead;
		while (current != (GcnEntry *) NULL) {
			if (strncmp(current->serverAddress,addr,strlen(addr)) == 0) {
				break;
			} 
			current = current->n;
		}
		if (current) {
			if ((strcmp(current->serverClass,"IINMSVR") == 0) ||
				(strcmp(current->serverClass,"COMSVR") == 0)) {
/* # line 189 "impserver.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)
"Name Servers and Net Servers do not have parameters");
        }
/* # line 192 "impserver.sc" */	/* host code */
			} else {
				displayServerParms(current);
			}
		}
      } else if (IIretval() == 3) {
/* # line 200 "impserver.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_server_list",(char *)"gcn_entry\
_tbl",-2) != 0) {
            IItcogetio((short *)0,1,32,SERVER_NAME_LENGTH-1,addr,(char *)
"serveraddress");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 202 "impserver.sc" */	/* host code */
		current = GcnListHead;
		while (current != (GcnEntry *) NULL) {
			if (strncmp(current->serverAddress,addr,strlen(addr)) == 0) {
				break;
			} 
			current = current->n;
		}
		if (current) {
			if ((strcmp(current->serverClass,"IINMSVR") == 0) 
                         || (strcmp(current->serverClass,"COMSVR") == 0) 
                        ) {
/* # line 213 "impserver.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)
"Sorry - cannot do internals for Name Servers or Net Servers");
        }
/* # line 216 "impserver.sc" */	/* host code */
			} 
                        else {
				impInternalMenu(current);
			}
		}
      } else if (IIretval() == 4) {
/* # line 225 "impserver.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_server_list",(char *)"gcn_entry\
_tbl",-2) != 0) {
            IItcogetio((short *)0,1,32,SERVER_NAME_LENGTH-1,addr,(char *)
"serveraddress");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 227 "impserver.sc" */	/* host code */
		current = GcnListHead;
		while (current != (GcnEntry *) NULL) {
			if (strncmp(current->serverAddress,addr,strlen(addr)) == 0) {
				break;
			} 
			current = current->n;
		}
		if (current) {
			if ((strcmp(current->serverClass,"IINMSVR") == 0) ||
				(strcmp(current->serverClass,"COMSVR") == 0)) {
/* # line 237 "impserver.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)
"Name Servers and Net Servers do not have sessions");
        }
/* # line 240 "impserver.sc" */	/* host code */
			} else {
				displayServerSessions(current);
			}
		}
      } else if (IIretval() == 5) {
/* # line 248 "impserver.sc" */	/* display */
        {
          if (IInestmu() == 0) goto IIfdE2;
          goto IImuI2;
IIfdI2:;
IIfdB2:
          while (IIrunnest() != 0) {
            if (IIretval() == 1) {
/* # line 251 "impserver.sc" */	/* host code */
			shutdownProcess(SHUTDOWN_SOFT);
            } else if (IIretval() == 2) {
			shutdownProcess(SHUTDOWN_HARD);
            } else if (IIretval() == 3) {
			shutdownProcess(SHUTDOWN_CLOSE);
            } else if (IIretval() == 4) {
			shutdownProcess(SHUTDOWN_OPEN);
            } else if (IIretval() == 5) {
/* # line 271 "impserver.sc" */	/* enddisplay */
              {
                if (1) goto IIfdF2;
              }
            } else {
              if (1) goto IIfdE2;
            } /* IIretval */
          } /* IIrunnest */
IIfdF2:
          if (IIchkfrm() == 0) goto IIfdB2;
          goto IIfdE2;
IImuI2:
          if (IIinitmu() == 0) goto IIfdE2;
          if (IInmuact((char *)"Soft_Shut",(char *)0,2,2,1) == 0) goto IIfdE2;
          if (IInmuact((char *)"Hard_Shut",(char *)0,2,2,2) == 0) goto IIfdE2;
          if (IInmuact((char *)"Close_Listen",(char *)0,2,2,3) == 0) goto 
          IIfdE2;
          if (IInmuact((char *)"Open_Listen",(char *)0,2,2,4) == 0) goto IIfdE2;
          if (IInmuact((char *)"End",(char *)0,2,2,5) == 0) goto IIfdE2;
          IIFRInternal(0);
          if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE2;
          if (IIendmu() == 0) goto IIfdE2;
          goto IIfdI2;
IIfdE2:;
          IIendnest();
        }
      } else if (IIretval() == 6) {
/* # line 278 "impserver.sc" */	/* host code */
		clearGcnEntryTable();
		getGcnList();
		displayGcnList();
      } else if (IIretval() == 7) {
/* # line 285 "impserver.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"More_info",(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"Parameters",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Internals",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Sessions",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Control",(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 288 "impserver.sc" */	/* host code */
	return(TRUE);
}
/*
** Routines to handle the GCN linked list
**
*/
void freeGcnList() 
{
	GcnEntry	*local;
	while (GcnListHead != (GcnEntry *) NULL) {
		local = GcnListHead;
		GcnListHead = GcnListHead->n;
		(void) free((char *) local);
	}
}
GcnEntry        *
newGcnEntry()
{	
	GcnEntry        *local;
	GcnEntry        *here;
	local = (GcnEntry *) malloc((unsigned) sizeof(GcnEntry));
	if (local == (GcnEntry *) NULL) {
		return((GcnEntry *) NULL);
	}
	(void) memset((char *) local, 0, sizeof(GcnEntry));
	if (GcnListHead == (GcnEntry *) NULL) {
		GcnListHead = local;
		local->p = (GcnEntry *) NULL;
		local->n = (GcnEntry *) NULL;
		return(local);
	}
	here = GcnListHead;
	while (here->n != ((GcnEntry *) NULL)) {
		here = here->n;
	}
	here->n = local;
	local->p = here;
	local->n = (GcnEntry *) NULL;
	return(local);
}
/*
** getGcnList()
**
** Given the global pointer to the start of the list of known servers, 
** call IMA and populate the list
*/
void getGcnList()
{
  GcnEntry l;
  GcnEntry *n;
	if (GcnListHead != (GcnEntry *) NULL) {
		freeGcnList();
	}
/* # line 355 "impserver.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impserver1",7539,31852);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"impserver1",7539,31852);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select gcn, address, class, object, max_sessions, num_sessions, serve\
r_pid from imp_gcn_view");
        IIexDefine(0,(char *)"impserver1",7539,31852);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,l.nameServer);
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,l.serverAddress);
      IIgetdomio((short *)0,1,32,SHORT_STRING-1,l.serverClass);
      IIgetdomio((short *)0,1,32,SHORT_STRING-1,l.serverObjects);
      IIgetdomio((short *)0,1,30,4,&l.maxSessions);
      IIgetdomio((short *)0,1,30,4,&l.numSessions);
      IIgetdomio((short *)0,1,30,4,&l.serverPid);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 374 "impserver.sc" */	/* host code */
		sqlerr_check();
		if (n = newGcnEntry()) {
			strcpy(n->nameServer,l.nameServer);
			strcpy(n->serverAddress,l.serverAddress);
			strcpy(n->serverClass,l.serverClass);
			strcpy(n->serverObjects,l.serverObjects);
			n->maxSessions = l.maxSessions;
			n->numSessions = l.numSessions;
			n->serverPid = l.serverPid;
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	sqlerr_check();
/* # line 386 "impserver.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 388 "impserver.sc" */	/* host code */
}
void displayGcnList()
{
	/*
	** loop through the list of know server objects 
	*/
  GcnEntry *local;
	local = GcnListHead;
	while (local != (GcnEntry *) NULL) {
/* # line 403 "impserver.sc" */	/* loadtable */
  {
    if (IItbact((char *)"imp_server_list",(char *)"gcn_entry_tbl",1) != 0) {
      IItcoputio((char *)"serveraddress",(short *)0,1,32,0,
      local->serverAddress);
      IItcoputio((char *)"serverclass",(short *)0,1,32,0,local->serverClass);
      IItcoputio((char *)"numsessions",(short *)0,1,30,4,&local->numSessions);
      IItcoputio((char *)"serverdblist",(short *)0,1,32,0,local->serverObjects);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 408 "impserver.sc" */	/* host code */
		local = local->n;
	}
/* # line 410 "impserver.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 411 "impserver.sc" */	/* host code */
}
void clearGcnEntryTable()
{
/* # line 415 "impserver.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_server_list",(char *)"gcn_entry_tbl",(char *)"\
f") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 416 "impserver.sc" */	/* host code */
}
/*
** ShutdownProcess()
**
**	Shut down the specified server by calling the control menuitem
*/
int
shutdownProcess(how)
  int how;
{
  char buf[100];
  char answer[100];
  char addr[100];
  GcnEntry *who;
/* # line 437 "impserver.sc" */	/* getrow */
  {
    if (IItbsetio(2,(char *)"imp_server_list",(char *)"gcn_entry_tbl",-2) != 0
    ) {
      IItcogetio((short *)0,1,32,SERVER_NAME_LENGTH-1,addr,(char *)
"serveraddress");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 439 "impserver.sc" */	/* host code */
	who = GcnListHead;
	while (who != (GcnEntry *) NULL) {
		if (strcmp(who->serverAddress,addr) == 0) {
			break;
		} 
		who = who->n;
	}
	if (	(strcmp(who->serverClass,"IINMSVR") == 0) ||
		(strcmp(who->serverClass,"IUSVR") == 0) ||
		(strcmp(who->serverClass,"COMSVR") == 0) )
		{
		sprintf(buf,
			"Don't know how to control %s (%s) with IMA yet",
			who->serverClass,
			who->serverAddress);
/* # line 456 "impserver.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(buf);
  }
/* # line 457 "impserver.sc" */	/* host code */
		return(TRUE);
	}
	/*
	** for now - cheat by building the name string directly
	*/
	strcpy(addr,currentVnode);
	strcat(addr,"::/@");
	strcat(addr,who->serverAddress);
	/*
	** we must have an INGRES, STAR, or user specified server
	*/
	/*
	** let the user bottle out..
	*/
	switch(how) {
	case SHUTDOWN_HARD: 
		sprintf(buf,"Confirm %s of server %s (%s) y/n: ",
			"Hard Shutdown",who->serverClass,addr);
		break;
	case SHUTDOWN_SOFT:
		sprintf(buf,"Confirm %s of server %s (%s) y/n: ",
			"Soft Shutdown",who->serverClass,addr);
		break;
	case SHUTDOWN_CLOSE:
		sprintf(buf,"Confirm %s of server %s (%s) y/n: ",
			"Close Listen ",who->serverClass,addr);
		break;
	case SHUTDOWN_OPEN:
		sprintf(buf,"Confirm %s of server %s (%s) y/n: ",
			"Open Listen",who->serverClass,addr);
		break;
	}
/* # line 493 "impserver.sc" */	/* prompt */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IIprmptio(0,buf,(short *)0,1,32,99,answer);
  }
/* # line 494 "impserver.sc" */	/* host code */
	if ((answer[0] != 'Y') && (answer[0] != 'y')) {
/* # line 495 "impserver.sc" */	/* message */
  {
    IImessage((char *)"Cancelled . . .");
  }
/* # line 496 "impserver.sc" */	/* host code */
		PCsleep(2*1000);
		return(TRUE);
	}
	sprintf(buf,"%s (%s) ",who->serverClass,addr);
	switch(how) {
	case SHUTDOWN_HARD: 
/* # line 503 "impserver.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_stop_server");
    IILQprvProcValio((char *)"server_id",0,(short *)0,1,32,0,addr);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 505 "impserver.sc" */	/* host code */
		strcat(buf,"Stopped");
		break;
	case SHUTDOWN_SOFT:
/* # line 508 "impserver.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_shut_server");
    IILQprvProcValio((char *)"server_id",0,(short *)0,1,32,0,addr);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 510 "impserver.sc" */	/* host code */
		strcat(buf,"will shutdown");
		break;
	case SHUTDOWN_CLOSE:
/* # line 513 "impserver.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_close_server");
    IILQprvProcValio((char *)"server_id",0,(short *)0,1,32,0,addr);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 515 "impserver.sc" */	/* host code */
		strcat(buf,"is closed");
		break;
	case SHUTDOWN_OPEN:
/* # line 518 "impserver.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_open_server");
    IILQprvProcValio((char *)"server_id",0,(short *)0,1,32,0,addr);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 520 "impserver.sc" */	/* host code */
		strcat(buf,"is open");
		break;
	}
/* # line 524 "impserver.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(buf);
  }
/* # line 525 "impserver.sc" */	/* host code */
	return(TRUE);
}
/*
** Name:	displayServerPopup
**
** Display the detail of a server for the new IMA based IPM lookalike tool.
**
## History:
##
## 26-jan-94	(johna)
##		started on RIF day from pervious ad-hoc programs
## 04-Feb-94	(johna)
##		added the server startup param screen
## 10-mar-94	(johna)
##		moved into the impserver.sc file
*/
int
displayServerPopup(entry)
GcnEntry	*entry;
{
/* # line 547 "impserver.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_server_popup",(char *)"r") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
/* # line 550 "impserver.sc" */	/* host code */
		display_details(entry);
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 555 "impserver.sc" */	/* breakdisplay */
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
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 558 "impserver.sc" */	/* host code */
	return(TRUE);
}
void display_details(entry)
  GcnEntry *entry;
{
  char buf[31];
/* # line 571 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_popup") != 0) {
      IIputfldio((char *)"current_sessions",(short *)0,1,30,4,&
      entry->numSessions);
    } /* IIfsetio */
  }
/* # line 573 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_popup") != 0) {
      IIputfldio((char *)"max_sessions",(short *)0,1,30,4,&entry->maxSessions);
    } /* IIfsetio */
  }
/* # line 575 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_popup") != 0) {
      IIputfldio((char *)"server_name",(short *)0,1,32,0,entry->serverAddress);
    } /* IIfsetio */
  }
/* # line 577 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_popup") != 0) {
      IIputfldio((char *)"server_type",(short *)0,1,32,0,entry->serverClass);
    } /* IIfsetio */
  }
/* # line 579 "impserver.sc" */	/* host code */
	if (strcmp(entry->serverObjects,"*") == 0) {
		sprintf(buf,"All databases");
	} else {
		strcpy(buf,entry->serverObjects);
	}
/* # line 584 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_popup") != 0) {
      IIputfldio((char *)"server_dblist",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 585 "impserver.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 586 "impserver.sc" */	/* host code */
}
int
displayServerParms(entry)
GcnEntry	*entry;
{
/* # line 592 "impserver.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_server_parms",(char *)"r") == 0) goto IIfdE4;
    goto IImuI4;
IIfdI4:;
    if (IIfsetio((char *)0) == 0) goto IIfdE4;
/* # line 595 "impserver.sc" */	/* host code */
		if (display_parameters(entry) == FALSE) {
/* # line 596 "impserver.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE4;
    }
/* # line 597 "impserver.sc" */	/* host code */
		}
IIfdB4:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 602 "impserver.sc" */	/* display */
        {
          if (IInestmu() == 0) goto IIfdE5;
          goto IImuI5;
IIfdI5:;
IIfdB5:
          while (IIrunnest() != 0) {
            if (IIretval() == 1) {
/* # line 605 "impserver.sc" */	/* host code */
			shutdownProcess(SHUTDOWN_SOFT);
			if (display_parameters(entry) == FALSE) {
/* # line 607 "impserver.sc" */	/* breakdisplay */
              {
                if (1) goto IIfdE5;
              }
/* # line 608 "impserver.sc" */	/* host code */
			}
            } else if (IIretval() == 2) {
			shutdownProcess(SHUTDOWN_HARD);
			if (display_parameters(entry) == FALSE) {
/* # line 615 "impserver.sc" */	/* breakdisplay */
              {
                if (1) goto IIfdE5;
              }
/* # line 616 "impserver.sc" */	/* host code */
			}
            } else if (IIretval() == 3) {
			shutdownProcess(SHUTDOWN_CLOSE);
			if (display_parameters(entry) == FALSE) {
/* # line 623 "impserver.sc" */	/* breakdisplay */
              {
                if (1) goto IIfdE5;
              }
/* # line 624 "impserver.sc" */	/* host code */
			}
            } else if (IIretval() == 4) {
			shutdownProcess(SHUTDOWN_OPEN);
			if (display_parameters(entry) == FALSE) {
/* # line 631 "impserver.sc" */	/* breakdisplay */
              {
                if (1) goto IIfdE5;
              }
/* # line 632 "impserver.sc" */	/* host code */
			}
            } else if (IIretval() == 5) {
/* # line 637 "impserver.sc" */	/* enddisplay */
              {
                if (1) goto IIfdF5;
              }
            } else {
              if (1) goto IIfdE5;
            } /* IIretval */
          } /* IIrunnest */
IIfdF5:
          if (IIchkfrm() == 0) goto IIfdB5;
          goto IIfdE5;
IImuI5:
          if (IIinitmu() == 0) goto IIfdE5;
          if (IInmuact((char *)"Soft_Shut",(char *)0,2,2,1) == 0) goto IIfdE5;
          if (IInmuact((char *)"Hard_Shut",(char *)0,2,2,2) == 0) goto IIfdE5;
          if (IInmuact((char *)"Close_Listen",(char *)0,2,2,3) == 0) goto 
          IIfdE5;
          if (IInmuact((char *)"Open_Listen",(char *)0,2,2,4) == 0) goto IIfdE5;
          if (IInmuact((char *)"End",(char *)0,2,2,5) == 0) goto IIfdE5;
          IIFRInternal(0);
          if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE5;
          if (IIendmu() == 0) goto IIfdE5;
          goto IIfdI5;
IIfdE5:;
          IIendnest();
        }
      } else if (IIretval() == 2) {
/* # line 643 "impserver.sc" */	/* host code */
		if (display_parameters(entry) == FALSE) {
/* # line 644 "impserver.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE4;
        }
/* # line 645 "impserver.sc" */	/* host code */
		}
      } else if (IIretval() == 3) {
/* # line 650 "impserver.sc" */	/* breakdisplay */
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
    if (IIinitmu() == 0) goto IIfdE4;
    if (IInmuact((char *)"Control",(char *)0,2,2,1) == 0) goto IIfdE4;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE4;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE4;
    if (IIendmu() == 0) goto IIfdE4;
    goto IIfdI4;
IIfdE4:;
    IIendfrm();
  }
/* # line 653 "impserver.sc" */	/* host code */
	return(TRUE);
}
int
display_parameters(entry)
  GcnEntry *entry;
{
  char buf[31];
  int cnt;
  char msgbuf[31];
  char local_vnode[SERVER_NAME_LENGTH];
  char local_server[SERVER_NAME_LENGTH];
  int local_cursors;
  char local_capabilities[46];
  int local_cursor_flags;
  int local_fastcommit;
  int local_flatten;
  int local_listen_fails;
  int local_name_service;
  int local_no_star_cluster;
  int local_noflatten;
  int local_no_star_recov;
  int local_nousers;
  int local_incons_risk;
  int local_rule_depth;
  int local_connects;
  char local_listen_state[10];
  int local_max_connects;
  int local_res_connects;
  char local_shut_state[8];
  char local_server_name[20];
  int local_soleserver;
  char local_svr_state[20];
  int local_wbehind;
	/*
	** cheat for now - assume the current vnode
	*/
	strcpy(buf,currentVnode);
	strcat(buf,"::/@");
	strcat(buf,entry->serverAddress);
/* # line 700 "impserver.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impserver2",7540,22972);
      IIputdomio((short *)0,1,32,0,buf);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"impserver2",7540,22972);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select server, cursors, capabilities, cursor_flags, fastcommit, liste\
n_fails, name_service, no_star_cluster, nostar_recov, nousers, incons_\
risk, rule_depth, connects, listen_state, max_connects, res_connects, \
shut_state, server_name, soleserver, svr_state");
        IIwritio(0,(short *)0,1,32,0,(char *)
", wbehind from imp_server_parms where server=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,buf);
        IIexDefine(0,(char *)"impserver2",7540,22972);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,local_server);
      IIgetdomio((short *)0,1,30,4,&local_cursors);
      IIgetdomio((short *)0,1,32,45,local_capabilities);
      IIgetdomio((short *)0,1,30,4,&local_cursor_flags);
      IIgetdomio((short *)0,1,30,4,&local_fastcommit);
      IIgetdomio((short *)0,1,30,4,&local_listen_fails);
      IIgetdomio((short *)0,1,30,4,&local_name_service);
      IIgetdomio((short *)0,1,30,4,&local_no_star_cluster);
      IIgetdomio((short *)0,1,30,4,&local_no_star_recov);
      IIgetdomio((short *)0,1,30,4,&local_nousers);
      IIgetdomio((short *)0,1,30,4,&local_incons_risk);
      IIgetdomio((short *)0,1,30,4,&local_rule_depth);
      IIgetdomio((short *)0,1,30,4,&local_connects);
      IIgetdomio((short *)0,1,32,9,local_listen_state);
      IIgetdomio((short *)0,1,30,4,&local_max_connects);
      IIgetdomio((short *)0,1,30,4,&local_res_connects);
      IIgetdomio((short *)0,1,32,7,local_shut_state);
      IIgetdomio((short *)0,1,32,19,local_server_name);
      IIgetdomio((short *)0,1,30,4,&local_soleserver);
      IIgetdomio((short *)0,1,32,19,local_svr_state);
      IIgetdomio((short *)0,1,30,4,&local_wbehind);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 747 "impserver.sc" */	/* host code */
	sqlerr_check();
/* # line 749 "impserver.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 750 "impserver.sc" */	/* host code */
	if (cnt == 0) {
		sprintf(msgbuf,"No information for server %s",buf);
/* # line 752 "impserver.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(msgbuf);
  }
/* # line 753 "impserver.sc" */	/* host code */
		return(FALSE);
	}
/* # line 756 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"server",(short *)0,1,32,0,local_server);
    } /* IIfsetio */
  }
/* # line 758 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"svr_state",(short *)0,1,32,0,local_svr_state);
    } /* IIfsetio */
  }
/* # line 760 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"shut_state",(short *)0,1,32,0,local_shut_state);
    } /* IIfsetio */
  }
/* # line 762 "impserver.sc" */	/* host code */
	sprintf(buf,"%s",local_name_service ? "ON" : "OFF");
/* # line 763 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"name_service",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 765 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"listen_state",(short *)0,1,32,0,local_listen_state);
    } /* IIfsetio */
  }
/* # line 767 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"server_name",(short *)0,1,32,0,local_server_name);
    } /* IIfsetio */
  }
/* # line 769 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"listen_fails",(short *)0,1,30,4,&local_listen_fails);
    } /* IIfsetio */
  }
/* # line 771 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"connects",(short *)0,1,30,4,&local_connects);
    } /* IIfsetio */
  }
/* # line 773 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"max_connects",(short *)0,1,30,4,&local_max_connects);
    } /* IIfsetio */
  }
/* # line 775 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"nousers",(short *)0,1,30,4,&local_nousers);
    } /* IIfsetio */
  }
/* # line 777 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"res_connects",(short *)0,1,30,4,&local_res_connects);
    } /* IIfsetio */
  }
/* # line 779 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"capabilities",(short *)0,1,32,0,local_capabilities);
    } /* IIfsetio */
  }
/* # line 781 "impserver.sc" */	/* host code */
	sprintf(buf,"%s",local_fastcommit ? "ON" : "OFF");
/* # line 782 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"fastcommit",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 784 "impserver.sc" */	/* host code */
	sprintf(buf,"%s",local_soleserver ? "ON" : "OFF");
/* # line 785 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"soleserver",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 787 "impserver.sc" */	/* host code */
	sprintf(buf,"%s",local_incons_risk ? "ON" : "OFF");
/* # line 788 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"incons_risk",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 790 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"wbehind",(short *)0,1,30,4,&local_wbehind);
    } /* IIfsetio */
  }
/* # line 792 "impserver.sc" */	/* host code */
	sprintf(buf,"%s",local_no_star_recov ? "ON" : "OFF");
/* # line 793 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"nostar_recov",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 795 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"rule_depth",(short *)0,1,30,4,&local_rule_depth);
    } /* IIfsetio */
  }
/* # line 797 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"cursors",(short *)0,1,30,4,&local_cursors);
    } /* IIfsetio */
  }
/* # line 799 "impserver.sc" */	/* host code */
	switch(local_cursor_flags) {
	case 1:	sprintf(buf,"DIRECT_UPDATE(1)"); break;
	case 2:	sprintf(buf,"DEFERRED_UPDATE(2)"); break;
	default:sprintf(buf,"Unknown flag %d",local_cursor_flags);
	}
/* # line 804 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_server_parms") != 0) {
      IIputfldio((char *)"cursor_flags",(short *)0,1,32,0,buf);
    } /* IIfsetio */
  }
/* # line 806 "impserver.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 807 "impserver.sc" */	/* host code */
	return(TRUE);
}
/*
** Name:	displayServerSessions
**
** Display the list of known sessions for the new IMA based IPM lookalike tool.
**
## History:
##
## 26-jan-94	(johna)
##		started on RIF day from pervious ad-hoc programs
## 10-mar-94	(johna)
##		moved to the impserver.sc file
## 17-Aug-2005  (hanje04)
##		BUG:115067
##		Make thread 32bytes to handle 64bit session ID's
##
*/
int
displayServerSessions(server)
GcnEntry	*server;
{
  GcnEntry *t;
  SessEntry *current_sess;
  char thread[32];
  char server_name[SERVER_NAME_LENGTH];
  char buf[100];
  char answer[100];
  int resource_toggle;
/* # line 843 "impserver.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_session_list",(char *)"r") == 0) goto IIfdE6;
    goto IImuI6;
IIfdI6:;
    if (IIfsetio((char *)0) == 0) goto IIfdE6;
/* # line 846 "impserver.sc" */	/* host code */
        resource_toggle = FALSE;    /*
##mdf040903
*/
            toggle_server_cols(&resource_toggle); 
		t = GcnListHead;
		while (t != (GcnEntry *) NULL) {
			if (t->serverPid == server->serverPid) {
				sprintf(server_name,	
					"%s::/@%s",
					currentVnode,t->serverAddress);
				break;
			}
			t = t->n;
		}
		/*
		** reset the domain at this point to be just the server 
		** we are interested in - that way the IMA gateway does not
		** have to collect things it is not interested in
		*/
/* # line 865 "impserver.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_reset_domain");
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 867 "impserver.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_set_domain");
      IILQprvProcValio((char *)"entry",0,(short *)0,1,32,0,server_name);
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 869 "impserver.sc" */	/* host code */
		/*
		** FIXME - set the correct domain at this point
		*/
/* # line 872 "impserver.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 874 "impserver.sc" */	/* host code */
		getSessList(server_name);
/* # line 876 "impserver.sc" */	/* inittable */
    {
      if (IItbinit((char *)"imp_session_list",(char *)"svr_sess_tbl",(char *)"\
r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 877 "impserver.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_session_list") != 0) {
        IIputfldio((char *)"server_name",(short *)0,1,32,0,server_name);
      } /* IIfsetio */
    }
/* # line 879 "impserver.sc" */	/* host code */
		displaySessList();
IIfdB6:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 884 "impserver.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_session_list",(char *)"svr_sess\
_tbl",-2) != 0) {
            IItcogetio((short *)0,1,32,31,thread,(char *)"session_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 886 "impserver.sc" */	/* host code */
		current_sess = SessListHead;
		while (current_sess != (SessEntry *) NULL) {
			if (strcmp(current_sess->sessionId,thread) == 0) {
				break;
			} 
			current_sess = current_sess->n;
		}
		if (current_sess) {
			displaySessionDetail(current_sess,server_name);
		}
      } else if (IIretval() == 2) {
/* # line 900 "impserver.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_session_list",(char *)"svr_sess\
_tbl",-2) != 0) {
            IItcogetio((short *)0,1,32,31,thread,(char *)"session_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 902 "impserver.sc" */	/* host code */
		current_sess = SessListHead;
		while (current_sess != (SessEntry *) NULL) {
			if (strcmp(current_sess->sessionId,thread) == 0) {
				break;
			} 
			current_sess = current_sess->n;
		}
/* # line 909 "impserver.sc" */	/* display */
        {
          if (IInestmu() == 0) goto IIfdE7;
          goto IImuI7;
IIfdI7:;
IIfdB7:
          while (IIrunnest() != 0) {
            if (IIretval() == 1) {
/* # line 912 "impserver.sc" */	/* host code */
				sprintf(buf,
				"Confirm Removal of session %s (%s) y/n: ",
				current_sess->sessionId,
				current_sess->username);
/* # line 917 "impserver.sc" */	/* prompt */
              {
                IIFRgpcontrol(1,0);
                IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
                IIFRgpcontrol(2,0);
                IIprmptio(0,buf,(short *)0,1,32,99,answer);
              }
/* # line 919 "impserver.sc" */	/* host code */
				if ((answer[0] != 'Y') && (answer[0] != 'y')) {
/* # line 920 "impserver.sc" */	/* message */
              {
                IImessage((char *)"Cancelled . . .");
              }
/* # line 921 "impserver.sc" */	/* host code */
					PCsleep(2*1000);
				} else {
/* # line 923 "impserver.sc" */	/* execute procedure */
              {
                IIsqInit(&sqlca);
                IILQpriProcInit(2,(char *)"imp_remove_session");
                IILQprvProcValio((char *)"session_id",0,(short *)0,1,32,0,
                current_sess->sessionId);
                IILQprvProcValio((char *)"server_id",0,(short *)0,1,32,0,
                current_sess->serverAddress);
                while (IILQprsProcStatus(0) != 0) {
                } /* IILQprsProcStatus */
              }
/* # line 929 "impserver.sc" */	/* commit */
              {
                IIsqInit(&sqlca);
                IIxact(3);
              }
/* # line 930 "impserver.sc" */	/* host code */
				}
            } else if (IIretval() == 2) {
/* # line 935 "impserver.sc" */	/* enddisplay */
              {
                if (1) goto IIfdF7;
              }
            } else {
              if (1) goto IIfdE7;
            } /* IIretval */
          } /* IIrunnest */
IIfdF7:
          if (IIchkfrm() == 0) goto IIfdB7;
          goto IIfdE7;
IImuI7:
          if (IIinitmu() == 0) goto IIfdE7;
          if (IInmuact((char *)"Remove",(char *)0,2,2,1) == 0) goto IIfdE7;
          if (IInmuact((char *)"End",(char *)0,2,2,2) == 0) goto IIfdE7;
          IIFRInternal(0);
          if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE7;
          if (IIendmu() == 0) goto IIfdE7;
          goto IIfdI7;
IIfdE7:;
          IIendnest();
        }
      } else if (IIretval() == 3) {
/* # line 941 "impserver.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_session_list",(char *)"svr_sess\
_tbl",-2) != 0) {
            IItcogetio((short *)0,1,32,31,thread,(char *)"session_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 943 "impserver.sc" */	/* host code */
		current_sess = SessListHead;
		while (current_sess != (SessEntry *) NULL) {
			if (strcmp(current_sess->sessionId,thread) == 0) {
				break;
			} 
			current_sess = current_sess->n;
		}
		if (current_sess) {
			displayClientInfo(server_name,thread);
		}
      } else if (IIretval() == 4) {
            toggle_server_cols(&resource_toggle); 
      } else if (IIretval() == 5) {
	    clearSessnEntryTable();
	    getSessList(server_name);
	    displaySessList();
      } else if (IIretval() == 6) {
/* # line 969 "impserver.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"More_info",(char *)0,2,2,1) == 0) goto IIfdE6;
    if (IInmuact((char *)"DBA_Operations",(char *)0,2,2,2) == 0) goto IIfdE6;
    if (IInmuact((char *)"Client_Info",(char *)0,2,2,3) == 0) goto IIfdE6;
    if (IInmuact((char *)"Resource_Toggle",(char *)0,2,2,4) == 0) goto IIfdE6;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,5) == 0) goto IIfdE6;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE6;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE6;
    if (IIendmu() == 0) goto IIfdE6;
    goto IIfdI6;
IIfdE6:;
    IIendfrm();
  }
/* # line 972 "impserver.sc" */	/* host code */
	/*
	** reset the domain to include the whole installation
	*/
/* # line 975 "impserver.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impserver3",7541,17460);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"impserver3",7541,17460);
        IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('ima_vno\
de')");
        IIexDefine(0,(char *)"impserver3",7541,17460);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,99,buf);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 976 "impserver.sc" */	/* host code */
	sqlerr_check();
	if (strcmp(buf,currentVnode) == 0) {
/* # line 978 "impserver.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_complete_vnode");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 979 "impserver.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 980 "impserver.sc" */	/* host code */
	}
	return(TRUE);
}
/*
** Routines to handle the session linked list
*/
void freeSessList() 
{
	SessEntry	*local;
	while (SessListHead != (SessEntry *) NULL) {
		local = SessListHead;
		SessListHead = SessListHead->n;
		(void) free((char *) local);
	}
}
SessEntry        * newSessEntry()
{	
	SessEntry        *local;
	SessEntry        *here;
	local = (SessEntry *) malloc((unsigned) sizeof(SessEntry));
	if (local == (SessEntry *) NULL) {
		return((SessEntry *) NULL);
	}
	(void) memset((char *) local, 0, sizeof(SessEntry));
	if (SessListHead == (SessEntry *) NULL) {
		SessListHead = local;
		local->p = (SessEntry *) NULL;
		local->n = (SessEntry *) NULL;
		return(local);
	}
	here = SessListHead;
	while (here->n != ((SessEntry *) NULL)) {
		here = here->n;
	}
	here->n = local;
	local->p = here;
	local->n = (SessEntry *) NULL;
	return(local);
}
/*
** getSessList()
**
** Given the global pointer to the start of the list of known servers, 
** call IMA and populate the list
**
** MDF200302 Sort on database (descending )
*/
void getSessList(serverName)
  char *serverName;
{
  SessEntry *n_sess;
  char hack_buf[100];
	if (SessListHead != (SessEntry *) NULL) {
		freeSessList();
	}
/* # line 1051 "impserver.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impserver4",7542,20408);
      IIputdomio((short *)0,1,32,0,serverName);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"impserver4",7542,20408);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select server, session_id, state, wait_reason, mask, priority, thread\
_type, timeout, smode, uic, cputime, dio, bio, locks, username, assoc_\
id, euser, ruser, database, dblock, facidx, facility, activity, act_de\
tail, query, dbowner, terminal, s_name, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"groupid, role, appcode, scb_pid from imp_session_view where server=tr\
im(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,serverName);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by database desc");
        IIexDefine(0,(char *)"impserver4",7542,20408);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,l_sess.serverAddress);
      IIgetdomio((short *)0,1,32,SESSION_ID_LENGTH-1,l_sess.sessionId);
      IIgetdomio((short *)0,1,32,19,l_sess.state);
      IIgetdomio((short *)0,1,32,19,l_sess.wait_reason);
      IIgetdomio((short *)0,1,32,19,l_sess.mask);
      IIgetdomio((short *)0,1,30,4,&l_sess.priority);
      IIgetdomio((short *)0,1,32,4,l_sess.thread_type);
      IIgetdomio((short *)0,1,30,4,&l_sess.timeout);
      IIgetdomio((short *)0,1,30,4,&l_sess.mode);
      IIgetdomio((short *)0,1,30,4,&l_sess.uic);
      IIgetdomio((short *)0,1,30,4,&l_sess.cputime);
      IIgetdomio((short *)0,1,30,4,&l_sess.dio);
      IIgetdomio((short *)0,1,30,4,&l_sess.bio);
      IIgetdomio((short *)0,1,30,4,&l_sess.locks);
      IIgetdomio((short *)0,1,32,29,l_sess.username);
      IIgetdomio((short *)0,1,30,4,&l_sess.assoc_id);
      IIgetdomio((short *)0,1,32,19,l_sess.euser);
      IIgetdomio((short *)0,1,32,19,l_sess.ruser);
      IIgetdomio((short *)0,1,32,29,l_sess.database);
      IIgetdomio((short *)0,1,32,9,l_sess.dblock);
      IIgetdomio((short *)0,1,30,4,&l_sess.facility_id);
      IIgetdomio((short *)0,1,32,3,l_sess.facility);
      IIgetdomio((short *)0,1,32,79,l_sess.activity);
      IIgetdomio((short *)0,1,32,79,l_sess.act_detail);
      IIgetdomio((short *)0,1,32,1023,l_sess.query);
      IIgetdomio((short *)0,1,32,9,l_sess.dbowner);
      IIgetdomio((short *)0,1,32,9,l_sess.terminal);
      IIgetdomio((short *)0,1,32,29,l_sess.s_name);
      IIgetdomio((short *)0,1,32,9,l_sess.groupid);
      IIgetdomio((short *)0,1,32,9,l_sess.role);
      IIgetdomio((short *)0,1,30,4,&l_sess.appcode);
      IIgetdomio((short *)0,1,30,4,&l_sess.scb_pid);
      if (IIerrtest() != 0) goto IIrtB10;
/* # line 1123 "impserver.sc" */	/* host code */
		sqlerr_check();
		if (n_sess = newSessEntry()) {
			strcpy(n_sess->serverAddress,l_sess.serverAddress);
			strcpy(n_sess->sessionId,l_sess.sessionId);
			strcpy(n_sess->state,l_sess.state);
			if (strncmp(l_sess.wait_reason,"00",2) == 0) {
				sprintf(n_sess->wait_reason,"COM");
			} else {
				strcpy(n_sess->wait_reason,l_sess.wait_reason);
			}
			strcpy(n_sess->mask,l_sess.mask);
			n_sess->priority - l_sess.priority;
			strcpy(n_sess->thread_type,l_sess.thread_type);
			n_sess->timeout - l_sess.timeout;
			n_sess->mode - l_sess.mode;
			n_sess->uic - l_sess.uic;
			n_sess->cputime - l_sess.cputime;
			n_sess->bio - l_sess.bio;
			n_sess->dio - l_sess.dio;
			n_sess->locks - l_sess.locks;
			strcpy(n_sess->username,l_sess.username);
			n_sess->assoc_id - l_sess.assoc_id;
			strcpy(n_sess->euser,l_sess.euser);
			strcpy(n_sess->ruser,l_sess.ruser);
			if (strncmp(l_sess.database,"<no_",4) == 0) {
				sprintf(n_sess->database,"NONE");
			} else {
				strcpy(n_sess->database,l_sess.database);
			}
			strcpy(n_sess->dblock,l_sess.dblock);
			n_sess->facility_id - l_sess.facility_id;
			if (l_sess.facility_id == 0) {
				sprintf(n_sess->facility,"   ");
			} else {
				strcpy(n_sess->facility,l_sess.facility);
			}
			strcpy(n_sess->activity,l_sess.activity);
			strcpy(n_sess->act_detail,l_sess.act_detail);
			strcpy(n_sess->query,l_sess.query);
			strcpy(n_sess->dbowner,l_sess.dbowner);
			strcpy(n_sess->terminal,l_sess.terminal);
			if (l_sess.s_name[0] == ' ') {
				strcpy(n_sess->s_name, (char *) (l_sess.s_name + 1));
			} else {
				strcpy(n_sess->s_name,l_sess.s_name);
			}
			strcpy(n_sess->groupid,l_sess.groupid);
			strcpy(n_sess->role,l_sess.role);
			n_sess->appcode = l_sess.appcode;
			n_sess->scb_pid = l_sess.scb_pid;
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE10:;
  }
	sqlerr_check();
/* # line 1176 "impserver.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1178 "impserver.sc" */	/* host code */
}
void displaySessList()
{
	/*
	** loop through the list of know server objects 
	*/
  SessEntry *local_sess;
	local_sess = SessListHead;
	while (local_sess != (SessEntry *) NULL) {
/* # line 1193 "impserver.sc" */	/* loadtable */
  {
    if (IItbact((char *)"imp_session_list",(char *)"svr_sess_tbl",1) != 0) {
      IItcoputio((char *)"session_id",(short *)0,1,32,0,local_sess->sessionId);
      IItcoputio((char *)"session_name",(short *)0,1,32,0,local_sess->s_name);
      IItcoputio((char *)"terminal",(short *)0,1,32,0,local_sess->terminal);
      IItcoputio((char *)"database",(short *)0,1,32,0,local_sess->database);
      IItcoputio((char *)"cputime",(short *)0,1,30,4,&local_sess->cputime);
      IItcoputio((char *)"dio",(short *)0,1,30,4,&local_sess->dio);
      IItcoputio((char *)"state",(short *)0,1,32,0,local_sess->wait_reason);
      IItcoputio((char *)"facility",(short *)0,1,32,0,local_sess->facility);
      IItcoputio((char *)"query",(short *)0,1,32,0,local_sess->query);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1203 "impserver.sc" */	/* host code */
		local_sess = local_sess->n;
	}
/* # line 1205 "impserver.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1206 "impserver.sc" */	/* host code */
}
void clearSessnEntryTable()
{
/* # line 1210 "impserver.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_session_list",(char *)"svr_sess_tbl",(char *)"\
f") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1211 "impserver.sc" */	/* host code */
}
/*
** Name:	impdspses.sc
**
** Display session detail information for the new IMA based IPM lookalike tool.
**
## History:
##
## 27-jan-94	(johna)
##		started from pervious ad-hoc programs
## 10-feb-94	(johna)
##		moved into the impserver.sc file
##
## 24-apr-02    (
##mdf240402)
##		Don't use scb_pid in the WHERE clause
*/
int displaySessionDetail(sess,server)
SessEntry	*sess;
char		*server;
{
/*
	exec sql begin declare section;
		char		buf[100];
	exec sql end declare section;
*/
/* # line 1239 "impserver.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_session_detail",(char *)"r") == 0) goto IIfdE8;
    goto IImuI8;
IIfdI8:;
    if (IIfsetio((char *)0) == 0) goto IIfdE8;
/* # line 1242 "impserver.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
/* # line 1244 "impserver.sc" */	/* host code */
		if (getSessionInfo(sess,server)) {
			displaySession();
		} 
                else {
/* # line 1248 "impserver.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 1249 "impserver.sc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage((char *)"That session has exited.");
    }
/* # line 1251 "impserver.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE8;
    }
/* # line 1252 "impserver.sc" */	/* host code */
		}
IIfdB8:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		displayClientInfo(server,sess->sessionId);
      } else if (IIretval() == 2) {
		displayQueryInfo(server,sess->sessionId);
			displaySession();
      } else if (IIretval() == 3) {
		if (getSessionInfo(sess, server)) {
			displaySession();
		} else {
/* # line 1271 "impserver.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 1272 "impserver.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"That session has exited.");
        }
/* # line 1274 "impserver.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE8;
        }
/* # line 1275 "impserver.sc" */	/* host code */
		}
      } else if (IIretval() == 4) {
		if (getSessionInfo(sess,server)) {
			displaySession();
		} else {
/* # line 1283 "impserver.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 1284 "impserver.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"That session has exited.");
        }
/* # line 1286 "impserver.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE8;
        }
/* # line 1287 "impserver.sc" */	/* host code */
		}
		displaySession();
      } else if (IIretval() == 5) {
/* # line 1293 "impserver.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 1294 "impserver.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Client_Info",(char *)0,2,2,1) == 0) goto IIfdE8;
    if (IInmuact((char *)"More Query",(char *)0,2,2,2) == 0) goto IIfdE8;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,3) == 0) goto IIfdE8;
    if (IIFRtoact(0,4) == 0) goto IIfdE8;
    if (IInmuact((char *)"End",(char *)0,2,2,5) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE8;
    if (IIendmu() == 0) goto IIfdE8;
    goto IIfdI8;
IIfdE8:;
    IIendfrm();
  }
/* # line 1297 "impserver.sc" */	/* host code */
	return(TRUE);
}
int
getSessionInfo(p_sess,p_server)
  SessEntry *p_sess;
  char *p_server;
{
	/* reselect the information in case it has changed */
  int i;
  int j;
  char buf[100];
	i = 0;
/* # line 1316 "impserver.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impserver5",7543,28287);
      IIputdomio((short *)0,1,32,0,p_sess->sessionId);
      IIputdomio((short *)0,1,32,0,p_server);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"impserver5",7543,28287);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select server, session_id, state, wait_reason, mask, priority, thread\
_type, timeout, smode, uic, cputime, dio, bio, locks, username, assoc_\
id, euser, ruser, database, dblock, facidx, facility, activity, act_de\
tail, query, dbowner, terminal, s_name, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"groupid, role, appcode, scb_pid from imp_session_view where session_i\
d=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,p_sess->sessionId);
        IIwritio(0,(short *)0,1,32,0,(char *)"and server=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,p_server);
        IIexDefine(0,(char *)"impserver5",7543,28287);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,l_sess.serverAddress);
      IIgetdomio((short *)0,1,32,SESSION_ID_LENGTH-1,l_sess.sessionId);
      IIgetdomio((short *)0,1,32,19,l_sess.state);
      IIgetdomio((short *)0,1,32,19,l_sess.wait_reason);
      IIgetdomio((short *)0,1,32,19,l_sess.mask);
      IIgetdomio((short *)0,1,30,4,&l_sess.priority);
      IIgetdomio((short *)0,1,32,4,l_sess.thread_type);
      IIgetdomio((short *)0,1,30,4,&l_sess.timeout);
      IIgetdomio((short *)0,1,30,4,&l_sess.mode);
      IIgetdomio((short *)0,1,30,4,&l_sess.uic);
      IIgetdomio((short *)0,1,30,4,&l_sess.cputime);
      IIgetdomio((short *)0,1,30,4,&l_sess.dio);
      IIgetdomio((short *)0,1,30,4,&l_sess.bio);
      IIgetdomio((short *)0,1,30,4,&l_sess.locks);
      IIgetdomio((short *)0,1,32,29,l_sess.username);
      IIgetdomio((short *)0,1,30,4,&l_sess.assoc_id);
      IIgetdomio((short *)0,1,32,19,l_sess.euser);
      IIgetdomio((short *)0,1,32,19,l_sess.ruser);
      IIgetdomio((short *)0,1,32,29,l_sess.database);
      IIgetdomio((short *)0,1,32,9,l_sess.dblock);
      IIgetdomio((short *)0,1,30,4,&l_sess.facility_id);
      IIgetdomio((short *)0,1,32,3,l_sess.facility);
      IIgetdomio((short *)0,1,32,79,l_sess.activity);
      IIgetdomio((short *)0,1,32,79,l_sess.act_detail);
      IIgetdomio((short *)0,1,32,1023,l_sess.query);
      IIgetdomio((short *)0,1,32,9,l_sess.dbowner);
      IIgetdomio((short *)0,1,32,9,l_sess.terminal);
      IIgetdomio((short *)0,1,32,29,l_sess.s_name);
      IIgetdomio((short *)0,1,32,9,l_sess.groupid);
      IIgetdomio((short *)0,1,32,9,l_sess.role);
      IIgetdomio((short *)0,1,30,4,&l_sess.appcode);
      IIgetdomio((short *)0,1,30,4,&l_sess.scb_pid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1387 "impserver.sc" */	/* host code */
	if ((sqlca.sqlcode != 0) && 
		(sqlca.sqlcode != 100)) 
	{
		sprintf(buf,"Sqlca.sqlcode is %d",sqlca.sqlcode);
/* # line 1391 "impserver.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1392 "impserver.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(buf);
  }
/* # line 1393 "impserver.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
    } /* IIiqset */
  }
/* # line 1394 "impserver.sc" */	/* host code */
	}
/* # line 1395 "impserver.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&i,8);
  }
/* # line 1396 "impserver.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1397 "impserver.sc" */	/* host code */
	if (i == 0) {
		return(FALSE);
	}
/* 
##mdf040902  
Get lastquery info */
/* # line 1403 "impserver.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impserver6",7544,25472);
      IIputdomio((short *)0,1,32,0,p_sess->sessionId);
      IIputdomio((short *)0,1,32,0,p_server);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"impserver6",7544,25472);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select session_lastquery from imp_lastquery where session_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,p_sess->sessionId);
        IIwritio(0,(short *)0,1,32,0,(char *)"and server=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,p_server);
        IIexDefine(0,(char *)"impserver6",7544,25472);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1023,l_sess.lastquery);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1410 "impserver.sc" */	/* host code */
	if ((sqlca.sqlcode != 0) && 
		(sqlca.sqlcode != 100)) 
	{
		sprintf(buf,"Sqlca.sqlcode is %d",sqlca.sqlcode);
/* # line 1414 "impserver.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1415 "impserver.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(buf);
  }
/* # line 1416 "impserver.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
    } /* IIiqset */
  }
/* # line 1417 "impserver.sc" */	/* host code */
	}
/* # line 1418 "impserver.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1419 "impserver.sc" */	/* host code */
        /* We are not interested in a zero rowcount */
	return(TRUE);
}
void displaySession()
{
  char lbuf[300];
/* # line 1432 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"session_id",(short *)0,1,32,0,l_sess.sessionId);
    } /* IIfsetio */
  }
/* # line 1433 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"session_id",(short *)0,1,32,0,l_sess.sessionId);
    } /* IIfsetio */
  }
/* # line 1434 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"server_addr",(short *)0,1,32,0,l_sess.serverAddress);
    } /* IIfsetio */
  }
/* # line 1435 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"session_name",(short *)0,1,32,0,l_sess.username);
    } /* IIfsetio */
  }
/* # line 1436 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"terminal",(short *)0,1,32,0,l_sess.terminal);
    } /* IIfsetio */
  }
/* # line 1437 "impserver.sc" */	/* host code */
	if (strncmp(l_sess.wait_reason,"00",2) == 0) {
		sprintf(lbuf,"CS_COMPUTABLE");
	} else {
		sprintf(lbuf,"%s (%s)",l_sess.state,l_sess.wait_reason);
	}
/* # line 1442 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"state_description",(short *)0,1,32,0,lbuf);
    } /* IIfsetio */
  }
/* # line 1443 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"mask",(short *)0,1,32,0,l_sess.mask);
    } /* IIfsetio */
  }
/* # line 1444 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"ruser",(short *)0,1,32,0,l_sess.ruser);
    } /* IIfsetio */
  }
/* # line 1445 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"euser",(short *)0,1,32,0,l_sess.euser);
    } /* IIfsetio */
  }
/* # line 1446 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"database",(short *)0,1,32,0,l_sess.database);
    } /* IIfsetio */
  }
/* # line 1447 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"dba",(short *)0,1,32,0,l_sess.dbowner);
    } /* IIfsetio */
  }
/* # line 1448 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"group_id",(short *)0,1,32,0,l_sess.groupid);
    } /* IIfsetio */
  }
/* # line 1449 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"role",(short *)0,1,32,0,l_sess.role);
    } /* IIfsetio */
  }
/* # line 1450 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"dblock",(short *)0,1,32,0,l_sess.dblock);
    } /* IIfsetio */
  }
/* # line 1451 "impserver.sc" */	/* host code */
	switch (l_sess.facility_id) {
	    case 0: sprintf(lbuf,"(none)"); break;
	    case 1: sprintf(lbuf,"CLF (Compatibility Lib)"); break;
	    case 2: sprintf(lbuf,"ADF (Abstract Datatype Facility)"); break;
	    case 3: sprintf(lbuf,"DMF (Data Manipulation Facility)"); break;
	    case 4: sprintf(lbuf,"OPF (Optimizer Facility)"); break;
	    case 5: sprintf(lbuf,"PSF (Parser Facility)"); break;
	    case 6: sprintf(lbuf,"QEF (Query Execution Facility)"); break;
	    case 7: sprintf(lbuf,"QSF (Query Storage Facility)"); break;
	    case 8: sprintf(lbuf,"RDF (Relation Destriptor Facility)"); break;
	    case 9: sprintf(lbuf,"SCF (System Control Facility)"); break;
	    case 10:sprintf(lbuf,"ULF (Utility Facility)"); break;
	    case 11:sprintf(lbuf,"DUF (Database Utility Facility)"); break;
	    case 12:sprintf(lbuf,"GCF (General Communication facility)"); break;
	    case 13:sprintf(lbuf,"RQF (Remote Query facility)"); break;
	    case 14:sprintf(lbuf,"TPF (Transaction Processing facility)"); break;
	    case 15:sprintf(lbuf,"GWF (Gateway facility)"); break;
	    case 16:sprintf(lbuf,"SXF (Security Extensions facility)"); break;
	    default:sprintf(lbuf,"Unknown facility code %d",l_sess.facility_id);
	}
/* # line 1472 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"facility_desc",(short *)0,1,32,0,lbuf);
    } /* IIfsetio */
  }
/* # line 1473 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"app_code",(short *)0,1,30,4,&l_sess.appcode);
    } /* IIfsetio */
  }
/* # line 1474 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"activity",(short *)0,1,32,0,l_sess.activity);
    } /* IIfsetio */
  }
/* # line 1475 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"activity_detail",(short *)0,1,32,0,l_sess.act_detail);
    } /* IIfsetio */
  }
/* # line 1476 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"query",(short *)0,1,32,0,l_sess.query);
    } /* IIfsetio */
  }
/* # line 1477 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_session_detail") != 0) {
      IIputfldio((char *)"lastquery",(short *)0,1,32,0,l_sess.lastquery);
    } /* IIfsetio */
  }
/* # line 1478 "impserver.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1479 "impserver.sc" */	/* host code */
}
displayDomain(str)
char	*str;
{
  char buf1[300];
  char buf[30];
	if ((str) && (*str)) {
		strcpy(buf1, str);
		strcat(buf1, " - Domain is ");
	} else {
		strcpy(buf1,"Domain is ");
	}
/* # line 1495 "impserver.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impserver7",7545,4439);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"impserver7",7545,4439);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select domplace from imp_domain");
        IIexDefine(0,(char *)"impserver7",7545,4439);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,99,buf);
      if (IIerrtest() != 0) goto IIrtB11;
/* # line 1497 "impserver.sc" */	/* host code */
		strcat(buf1,buf);
		strcat(buf1,", ");
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE11:;
  }
/* # line 1501 "impserver.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(buf1);
  }
/* # line 1502 "impserver.sc" */	/* host code */
}
/*
** display the client information for this session
*/
displayClientInfo(server,session)
  char *server;
  char *session;
{
  char client_server[21];
  char client_session_id[21];
  char client_pid[21];
  char client_host[21];
  char client_user[21];
  char client_tty[21];
  char client_connection[31];
  char client_info[100];
  int rowsFound = 0;
/* # line 1525 "impserver.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_client_info",(char *)"r") == 0) goto IIfdE9;
    goto IImuI9;
IIfdI9:;
    if (IIfsetio((char *)0) == 0) goto IIfdE9;
/* # line 1528 "impserver.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 1529 "impserver.sc" */	/* repeated select */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexExec(1,(char *)"impserver8",7546,4452);
        IIputdomio((short *)0,1,32,0,server);
        IIputdomio((short *)0,1,32,0,session);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIexDefine(1,(char *)"impserver8",7546,4452);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select server, session_id, client_host, client_pid, client_tty, clien\
t_user, client_info, client_connect from imp_client_info where server=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,server);
          IIwritio(0,(short *)0,1,32,0,(char *)"and session_id=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,session);
          IIexDefine(0,(char *)"impserver8",7546,4452);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,20,client_server);
        IIgetdomio((short *)0,1,32,20,client_session_id);
        IIgetdomio((short *)0,1,32,20,client_host);
        IIgetdomio((short *)0,1,32,20,client_pid);
        IIgetdomio((short *)0,1,32,20,client_tty);
        IIgetdomio((short *)0,1,32,20,client_user);
        IIgetdomio((short *)0,1,32,99,client_info);
        IIgetdomio((short *)0,1,32,30,client_connection);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 1552 "impserver.sc" */	/* host code */
            if (sqlerr_check() == NO_ROWS) {                              
/* # line 1553 "impserver.sc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage((char *)"That session has exited");
    }
/* # line 1555 "impserver.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE9;
    }
/* # line 1556 "impserver.sc" */	/* host code */
	    } else {
/* # line 1558 "impserver.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"client_user",(short *)0,1,32,0,client_user);
        IIputfldio((char *)"client_host",(short *)0,1,32,0,client_host);
        IIputfldio((char *)"client_tty",(short *)0,1,32,0,client_tty);
        IIputfldio((char *)"client_pid",(short *)0,1,32,0,client_pid);
        IIputfldio((char *)"client_connection",(short *)0,1,32,0,
        client_connection);
        IIputfldio((char *)"client_info",(short *)0,1,32,0,client_info);
      } /* IIfsetio */
    }
/* # line 1565 "impserver.sc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 1566 "impserver.sc" */	/* host code */
	    }
IIfdB9:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1571 "impserver.sc" */	/* breakdisplay */
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
    if (IIinitmu() == 0) goto IIfdE9;
    if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE9;
    if (IIendmu() == 0) goto IIfdE9;
    goto IIfdI9;
IIfdE9:;
    IIendfrm();
  }
/* # line 1574 "impserver.sc" */	/* host code */
	return(TRUE);
}
/*
** 
##mdf040903
** display the full query information for this session
*/
int
displayQueryInfo(server,p_session)
  char *server;
  char *p_session;
{
	/* reselect the information in case it has changed */
  int rowsFound;
  int j;
  char buf[100];
  SessEntry *local_sess = (SessEntry*)NULL;
	rowsFound = 0;
/* # line 1603 "impserver.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_query_info",(char *)"r") == 0) goto IIfdE10;
    goto IImuI10;
IIfdI10:;
    if (IIfsetio((char *)0) == 0) goto IIfdE10;
/* # line 1606 "impserver.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 1607 "impserver.sc" */	/* host code */
		local_sess = newSessEntry() ;/* 
##mdf260304 
*/
                strcpy(local_sess->sessionId,p_session );
                if (getSessionInfo(local_sess, server)) 
                {
/* # line 1613 "impserver.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"lastquery",(short *)0,1,32,0,l_sess.lastquery);
        IIputfldio((char *)"query",(short *)0,1,32,0,l_sess.query);
      } /* IIfsetio */
    }
/* # line 1617 "impserver.sc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 1618 "impserver.sc" */	/* host code */
                } else {                                                   
/* # line 1619 "impserver.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 1620 "impserver.sc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage((char *)"That l_session has exited.");
    }
/* # line 1622 "impserver.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE10;
    }
/* # line 1623 "impserver.sc" */	/* host code */
                }                                                          
IIfdB10:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
                strcpy(local_sess->sessionId,p_session );
                if (getSessionInfo(local_sess, server)) 
                {
/* # line 1631 "impserver.sc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio((char *)"lastquery",(short *)0,1,32,0,l_sess.lastquery);
            IIputfldio((char *)"query",(short *)0,1,32,0,l_sess.query);
          } /* IIfsetio */
        }
/* # line 1635 "impserver.sc" */	/* redisplay */
        {
          IIredisp();
        }
/* # line 1636 "impserver.sc" */	/* host code */
                } else {                                                   
/* # line 1637 "impserver.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 1638 "impserver.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"That p_session hhas exited.");
        }
/* # line 1640 "impserver.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE10;
        }
/* # line 1641 "impserver.sc" */	/* host code */
                }                                                          
      } else if (IIretval() == 2) {
/* # line 1646 "impserver.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE10;
        }
      } else {
        if (1) goto IIfdE10;
      } /* IIretval */
    } /* IIrunform */
IIfdF10:
    if (IIchkfrm() == 0) goto IIfdB10;
    goto IIfdE10;
IImuI10:
    if (IIinitmu() == 0) goto IIfdE10;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,1) == 0) goto IIfdE10;
    if (IInmuact((char *)"End",(char *)0,2,2,2) == 0) goto IIfdE10;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE10;
    if (IIendmu() == 0) goto IIfdE10;
    goto IIfdI10;
IIfdE10:;
    IIendfrm();
  }
/* # line 1649 "impserver.sc" */	/* host code */
	return(TRUE);
}
void toggle_server_cols(p_resource_toggle)
  int *p_resource_toggle;
{
    if (*p_resource_toggle) {
/* # line 1658 "impserver.sc" */	/* set_frs */
  {
    if (IIiqset(4,0,(char *)"imp_session_list",(char *)"svr_sess_tbl",
    (char *)0) != 0) {
      IIstfsio(54,(char *)"cputime",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"dio",0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(54,(char *)"state",0,(short *)0,1,30,sizeof(1),(void *)IILQint(
      1));
      IIstfsio(54,(char *)"facility",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1664 "impserver.sc" */	/* host code */
        *p_resource_toggle = FALSE;
    }
    else {
/* # line 1667 "impserver.sc" */	/* set_frs */
  {
    if (IIiqset(4,0,(char *)"imp_session_list",(char *)"svr_sess_tbl",
    (char *)0) != 0) {
      IIstfsio(54,(char *)"cputime",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dio",0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(54,(char *)"state",0,(short *)0,1,30,sizeof(0),(void *)IILQint(
      0));
      IIstfsio(54,(char *)"facility",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1673 "impserver.sc" */	/* host code */
        *p_resource_toggle = TRUE;
    }
}
/*
** Monitor Traffic for Specific GCC Server
**
**
## History:
##      04-Sep-03       (
##mdf040903)                     
*/
int
impGccInfo(p_server)
GcnEntry	*p_server;
{
  int i;
  char buf[60];
/* # line 1694 "impserver.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_gcc_info",(char *)"r") == 0) goto IIfdE11;
    goto IImuI11;
IIfdI11:;
    if (IIfsetio((char *)0) == 0) goto IIfdE11;
/* # line 1697 "impserver.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
/* # line 1699 "impserver.sc" */	/* repeated select */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexExec(1,(char *)"impserver9",7547,182);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIexDefine(1,(char *)"impserver9",7547,182);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('ima_vnode')");
          IIexDefine(0,(char *)"impserver9",7547,182);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,99,buf);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 1700 "impserver.sc" */	/* host code */
		sqlerr_check();
/* # line 1701 "impserver.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_reset_domain");
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 1702 "impserver.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_set_domain");
      IILQprvProcValio((char *)"entry",0,(short *)0,1,32,0,currentVnode);
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 1704 "impserver.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 1708 "impserver.sc" */	/* host code */
		if(displayGccInfo(p_server) == FALSE)
                {
/* # line 1710 "impserver.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE11;
    }
/* # line 1711 "impserver.sc" */	/* host code */
                }
IIfdB11:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		displayGccInfo(p_server);
      } else if (IIretval() == 2) {
		displayGccInfo(p_server);
      } else if (IIretval() == 3) {
/* # line 1729 "impserver.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 1730 "impserver.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE11;
        }
      } else {
        if (1) goto IIfdE11;
      } /* IIretval */
    } /* IIrunform */
IIfdF11:
    if (IIchkfrm() == 0) goto IIfdB11;
    goto IIfdE11;
IImuI11:
    if (IIinitmu() == 0) goto IIfdE11;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,1) == 0) goto IIfdE11;
    if (IIFRtoact(0,2) == 0) goto IIfdE11;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE11;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE11;
    if (IIendmu() == 0) goto IIfdE11;
    goto IIfdI11;
IIfdE11:;
    IIendfrm();
  }
/* # line 1733 "impserver.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_complete_vnode");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 1734 "impserver.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1737 "impserver.sc" */	/* host code */
	return(TRUE);
}
int displayGccInfo(p_server)
GcnEntry	*p_server;
{
  int l_conn_count;
  int l_data_in;
  int l_data_out;
  char l_ib_encrypt_mech[13];
  char l_ib_encrypt_mode[13];
  int l_inbound_current;
  int l_inbound_max;
  int l_msgs_in;
  int l_msgs_out;
  char l_net_server[65];
  char l_ob_encrypt_mech[13];
  char l_ob_encrypt_mode[13];
  char l_outbound_current;
  int l_outbound_max;
  int l_pl_protocol;
  char l_registry_mode[13];
  int l_trace_level;
  char l_msgbuf[100];
    strcpy(l_net_server,currentVnode);
    strcat(l_net_server,"::/@");                                         
    strcat(l_net_server,p_server->serverAddress);                          
/* # line 1767 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"net_server",(short *)0,1,32,0,l_net_server);
    } /* IIfsetio */
  }
/* # line 1769 "impserver.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impserver10",7548,6291);
      IIputdomio((short *)0,1,32,0,l_net_server);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"impserver10",7548,6291);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select int4(conn_count), int4(data_in), int4(data_out), ib_encrypt_me\
ch, ib_encrypt_mode, int4(inbound_current), int4(inbound_max), int4(ms\
gs_in), int4(msgs_out), ob_encrypt_mech, ob_encrypt_mode, int4(outboun\
d_current), int4(outbound_max), int4(");
        IIwritio(0,(short *)0,1,32,0,(char *)
"pl_protocol), registry_mode, int4(trace_level)from imp_gcc_info where\
 net_server=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,l_net_server);
        IIexDefine(0,(char *)"impserver10",7548,6291);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&l_conn_count);
      IIgetdomio((short *)0,1,30,4,&l_data_in);
      IIgetdomio((short *)0,1,30,4,&l_data_out);
      IIgetdomio((short *)0,1,32,12,l_ib_encrypt_mech);
      IIgetdomio((short *)0,1,32,12,l_ib_encrypt_mode);
      IIgetdomio((short *)0,1,30,4,&l_inbound_current);
      IIgetdomio((short *)0,1,30,4,&l_inbound_max);
      IIgetdomio((short *)0,1,30,4,&l_msgs_in);
      IIgetdomio((short *)0,1,30,4,&l_msgs_out);
      IIgetdomio((short *)0,1,32,12,l_ob_encrypt_mech);
      IIgetdomio((short *)0,1,32,12,l_ob_encrypt_mode);
      IIgetdomio((short *)0,1,30,1,&l_outbound_current);
      IIgetdomio((short *)0,1,30,4,&l_outbound_max);
      IIgetdomio((short *)0,1,30,4,&l_pl_protocol);
      IIgetdomio((short *)0,1,32,12,l_registry_mode);
      IIgetdomio((short *)0,1,30,4,&l_trace_level);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1806 "impserver.sc" */	/* host code */
    if (sqlerr_check() == NO_ROWS) {                              
/* # line 1807 "impserver.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1808 "impserver.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1809 "impserver.sc" */	/* host code */
        sprintf(l_msgbuf,"No information for Net Server %s",l_net_server);
/* # line 1810 "impserver.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(l_msgbuf);
  }
/* # line 1811 "impserver.sc" */	/* host code */
        return(FALSE);
    }                                                             
    else {                                                        
/* # line 1814 "impserver.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1816 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"conn_count",(short *)0,1,30,4,&l_conn_count);
    } /* IIfsetio */
  }
/* # line 1817 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"data_in",(short *)0,1,30,4,&l_data_in);
    } /* IIfsetio */
  }
/* # line 1818 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"data_out",(short *)0,1,30,4,&l_data_out);
    } /* IIfsetio */
  }
/* # line 1819 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"ib_encrypt_mech",(short *)0,1,32,0,l_ib_encrypt_mech);
    } /* IIfsetio */
  }
/* # line 1820 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"ib_encrypt_mode",(short *)0,1,32,0,l_ib_encrypt_mode);
    } /* IIfsetio */
  }
/* # line 1821 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"inbound_current",(short *)0,1,30,4,&
      l_inbound_current);
    } /* IIfsetio */
  }
/* # line 1822 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"inbound_max",(short *)0,1,30,4,&l_inbound_max);
    } /* IIfsetio */
  }
/* # line 1823 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"msgs_in",(short *)0,1,30,4,&l_msgs_in);
    } /* IIfsetio */
  }
/* # line 1824 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"msgs_out",(short *)0,1,30,4,&l_msgs_out);
    } /* IIfsetio */
  }
/* # line 1825 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"net_server",(short *)0,1,32,0,l_net_server);
    } /* IIfsetio */
  }
/* # line 1826 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"ob_encrypt_mech",(short *)0,1,32,0,l_ob_encrypt_mech);
    } /* IIfsetio */
  }
/* # line 1827 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"ob_encrypt_mode",(short *)0,1,32,0,l_ob_encrypt_mode);
    } /* IIfsetio */
  }
/* # line 1828 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"outbound_current",(short *)0,1,30,1,&
      l_outbound_current);
    } /* IIfsetio */
  }
/* # line 1829 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"outbound_max",(short *)0,1,30,4,&l_outbound_max);
    } /* IIfsetio */
  }
/* # line 1830 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"pl_protocol",(short *)0,1,30,4,&l_pl_protocol);
    } /* IIfsetio */
  }
/* # line 1831 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"registry_mode",(short *)0,1,32,0,l_registry_mode);
    } /* IIfsetio */
  }
/* # line 1832 "impserver.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_gcc_info") != 0) {
      IIputfldio((char *)"trace_level",(short *)0,1,30,4,&l_trace_level);
    } /* IIfsetio */
  }
/* # line 1833 "impserver.sc" */	/* host code */
        return(TRUE);
    }
}
