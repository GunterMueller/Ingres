# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** (c) 1989 Relational Technology
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** includes and defines
*/
#include <compat.h>
#include <dbms.h>
#include <fe.h>
#include "servinfo.h"
#include "dba.h"
#include "msgs.h"
/*
** Forward and external declarations
*/
VOID servdetail();
/*
**
**  File: srvdtail.qc
**
**  Purpose - this file contains the routines which display the server
**	detail information form.  
**
**  This file contains:
**	servdetail - displays the server detail information form. 
**
**  History
**	1/6/89		tomt	created
**	4/4/89		tomt	added code to display the info
**	8/24/89		tomt	added server_name parm and refresh menuitem
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**	5/18/95		harpa06 Cross Integration change for bug 56349 by nick:
**				disconnect from the server when processing
**				Quit() as the disconnect is normally done
**				in the caller when we return.
*/
VOID
servdetail(server)
PTR server;
{
    bool	e_xit = FALSE;
    /* Display the form and get the information */
/* # line 50 "srvdtail.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"fservdetail",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 53 "srvdtail.qsc" */	/* host code */
	/*  Put information on the form	*/
	dispsrvinfo(DISP_SERV_INFO, server, (char *)0);
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 59 "srvdtail.qsc" */	/* message */
          {
            IImessage((char *)"Loading server information...");
          }
/* # line 60 "srvdtail.qsc" */	/* host code */
	if (load_servinfo(GET_SERVINFO, server) != OK)
	{
	    POP_MSG("Error getting server information.  Server may not exist\
 or may be refusing connects.  To get an updated list of servers, return to\
 the previous screen and select the 'Refresh' menuitem.");
/* # line 65 "srvdtail.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 66 "srvdtail.qsc" */	/* host code */
	}
	dispsrvinfo(DISP_SERV_INFO, server, (char *)0);
        }
      } else if (IIretval() == 2) {
        {
/* # line 71 "srvdtail.qsc" */	/* host code */
	my_help("ipmsvdet.hlp", "Server Detail");
        }
      } else if (IIretval() == 3) {
        {
/* # line 75 "srvdtail.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 79 "srvdtail.qsc" */	/* host code */
	e_xit = TRUE;	/* user wants to leave */
        if (dosrvoper(GC_SERV_DISCONNECT, (char *)NULL) != OK)
        {
            POP_MSG("Error disconnecting from server");
        }
/* # line 84 "srvdtail.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 85 "srvdtail.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
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
    if (IInmuact((char *)"Help",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 88 "srvdtail.qsc" */	/* host code */
    if (e_xit) {	/* user wants to leave */
	terminate(OK);
    }
    return;
}
