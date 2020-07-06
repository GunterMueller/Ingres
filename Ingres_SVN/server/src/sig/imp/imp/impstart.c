# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	impstart.sc
**
** Start Menu for the IMA based IMP-like tool
**
## History:
##
## 26-jan-94	(johna)
##		started on RIF day from pervious ad-hoc programs
## 12-apr-94	(johna)
##		completed the change_vnode functionality
## 04-may-94	(johna)
##		some comments
## 02-Jul-2002 (fanra01)
##      Sir 108159
##      Added to distribution in the sig directory.
## 04-Sep-03     (mdf040903)                     
##              Commit after calling imp_reset_domain
##      06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
  extern int imp_main_menu;
  extern int iidbdbConnection;
  extern int inFormsMode;
  extern char *currentVnode;
  extern int VnodeNotDefined;
  MenuEntry MenuList[]= {
	1,	"server_list", 	"Display a list of servers to examine",
	2,	"lock_info", 	"Display locks and related information",
	3,	"log_info",	"Display information on the logging system",
	0,	"no_entry",	"No Entry"
};
int
startMenu()
{
  int i;
  char buf[60];
  char nodebuf[60];
/* # line 67 "impstart.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_main_menu",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 70 "impstart.sc" */	/* host code */
		/*exec sql select dbmsinfo('ima_server') into :buf;*/
                if (VnodeNotDefined == TRUE) {   
/* # line 72 "impstart.sc" */	/* select */
    {
      IIsqInit(&sqlca);
      IIsqMods(1);
      IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('ima_vnode\
')");
      IIretinit((char *)0,0);
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,59,buf);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 73 "impstart.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_main_menu") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 74 "impstart.sc" */	/* host code */
		    currentVnode = buf;
                }                                
                else
                {                                
/* # line 78 "impstart.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_main_menu") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 79 "impstart.sc" */	/* host code */
                }                                
/* # line 80 "impstart.sc" */	/* inittable */
    {
      if (IItbinit((char *)"imp_main_menu",(char *)"imp_menu",(char *)"\
r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 81 "impstart.sc" */	/* host code */
		i = 0;
		while (MenuList[i].item_number != 0) {
/* # line 83 "impstart.sc" */	/* loadtable */
    {
      if (IItbact((char *)"imp_main_menu",(char *)"imp_menu",1) != 0) {
        IItcoputio((char *)"item_number",(short *)0,1,30,4,&
        MenuList[i].item_number);
        IItcoputio((char *)"short_name",(short *)0,1,32,0,
        MenuList[i].short_name);
        IItcoputio((char *)"long_name",(short *)0,1,32,0,MenuList[i].long_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 87 "impstart.sc" */	/* host code */
			i++;
		}
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 93 "impstart.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_main_menu",(char *)"imp_menu",-2) != 0
          ) {
            IItcogetio((short *)0,1,30,4,&i,(char *)"item_number");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 94 "impstart.sc" */	/* host code */
		switch (i) {
		case 1:	processServerList();
			break;
		case 2: lockingMenu();
			break;
		case 3: loggingMenu();
			break;
		default:
			sprintf(buf," Unknown Option (%d)",i);
/* # line 103 "impstart.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(buf);
        }
/* # line 104 "impstart.sc" */	/* host code */
		}
      } else if (IIretval() == 2) {
/* # line 109 "impstart.sc" */	/* prompt */
        {
          IIprmptio(0,(char *)"Enter Vnode: ",(short *)0,1,32,59,nodebuf);
        }
/* # line 110 "impstart.sc" */	/* execute procedure */
        {
          IIsqInit(&sqlca);
          IILQpriProcInit(2,(char *)"imp_reset_domain");
          while (IILQprsProcStatus(0) != 0) {
          } /* IILQprsProcStatus */
        }
/* # line 111 "impstart.sc" */	/* execute procedure */
        {
          IIsqInit(&sqlca);
          IILQpriProcInit(2,(char *)"imp_set_domain");
          IILQprvProcValio((char *)"entry",0,(short *)0,1,32,0,nodebuf);
          while (IILQprsProcStatus(0) != 0) {
          } /* IILQprsProcStatus */
        }
/* # line 112 "impstart.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
        }
/* # line 113 "impstart.sc" */	/* host code */
/* 
##mdf040903 
*/
		currentVnode = nodebuf;
/* # line 117 "impstart.sc" */	/* putform */
        {
          if (IIfsetio((char *)"imp_main_menu") != 0) {
            IIputfldio((char *)"vnode",(short *)0,1,32,0,nodebuf);
          } /* IIfsetio */
        }
/* # line 118 "impstart.sc" */	/* host code */
		if (iidbdbConnection) {
			closeIidbdbConnection();
			/*
			** defer making the iidbdb connection until
			** we need it
			*/
			/* makeIidbdbConnection(); */
		}
      } else if (IIretval() == 3) {
/* # line 130 "impstart.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Change_Vnode",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 133 "impstart.sc" */	/* host code */
	return(TRUE);
}
