# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 2004 Ingres Corporation
**  All rights reserved.
*/
/*
** includes and defines
*/
#include <compat.h>
#include <dbms.h>
#include <fe.h>
#include "servinfo.h"
#include "msgs.h"
#include "ingpriv.h"
#include "dba.h"
/*
** Forward and External References
*/
i4	chkingpriv();
#if defined(LP64)
static char sessdtl[] = "fsessdetail64";
#else
static char sessdtl[] = "fsessdetail";
#endif  /* LP64 */
  static char *Fsessdetail = sessdtl;
/*
**
**  File: sessdetail.qc
**
**  Purpose - this file contains the routines which display the session
**	detail information form.  
**
**  This file contains:
**	sessdetail - displays the session detail information form. 
**
**
** Inputs
**	server_name - need to know this to get and refresh data.
**	session_id - same as above
**
**  History
**	1/6/89		tomt	created
**	4/5/89		tomt	added code to display info
**	4/6/89		tomt	changed fields on form - got rid of some
**	8/24/89		tomt	changed calls to get info on screen and
**				added refresh menuitem.
**	6/1/90		tomt	added new 6.3/02 info: query, activity
**				rollout info.
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**	29 May 1992	jpk	added support for long queries
**	18-may-1995     harpa06 Cross Integration bug fix 56349 by nick:
**				disconnect before Quit() - server is connected
**				when calling sessdetail() so we need to
**				disconnect if we are quiting rather than
**				returning.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-oct-2001 (somsa01)
**	    Added references to the 64-bit forms.
*/
VOID
sessdetail(server_name, session_id)
PTR server_name;
PTR session_id;
{
    bool	e_xit = FALSE;
  char *Procinfo =
#ifdef VMS 
			"Process_Info";
#else
			"";
#endif
    /* Display the form and get the information */
/* # line 83 "sesdtail.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(3),(void *)IILQint(3));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fsessdetail,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 88 "sesdtail.qsc" */	/* host code */
	/* If the user is not a superuser, don't display query field */
	if (chkingpriv(ING_SUPERUSER) != OK)
	{
/* # line 91 "sesdtail.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,Fsessdetail,(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"querytext",0,(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 92 "sesdtail.qsc" */	/* host code */
        }
	/* Put information on the form	*/
	dispsrvinfo(DISP_SESS_INFO, server_name, session_id);
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 99 "sesdtail.qsc" */	/* display */
          {
            IIFRgpcontrol(1,0);
            IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
            IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
            IIFRgpsetio(1,(short *)0,1,30,sizeof(6),(void *)IILQint(6));
            IIFRgpcontrol(2,0);
            if (IIdispfrm((char *)"fqrytext",(char *)"r") == 0) goto IIfdE2;
            goto IImuI2;
IIfdI2:;
IIfdB2:
            while (IIrunform() != 0) {
              if (IIretval() == 1) {
                {
/* # line 103 "sesdtail.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE2;
                  }
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
            if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE2;
            if (IIendmu() == 0) goto IIfdE2;
            goto IIfdI2;
IIfdE2:;
            IIendfrm();
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 108 "sesdtail.qsc" */	/* message */
          {
            IImessage((char *)"Loading session information...");
          }
/* # line 109 "sesdtail.qsc" */	/* host code */
	if (load_servinfo(GET_SERVINFO, server_name) != OK)
	{
	    POP_MSG("Error getting session information.  Session may not exist\
 or server may not be accepting connects.  To get an updated list of sessions\
 for this server, return to previous screen and select the 'Refresh'\
 menuitem");
/* # line 115 "sesdtail.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 116 "sesdtail.qsc" */	/* host code */
	}
	dispsrvinfo(DISP_SESS_INFO, server_name, session_id);
        }
      } else if (IIretval() == 3) {
        {
/* # line 121 "sesdtail.qsc" */	/* host code */
#ifdef VMS
          char client_pid[HEXID_SIZE + 1];
/* # line 123 "sesdtail.qsc" */	/* getform */
          {
            if (IIfsetio(Fsessdetail) != 0) {
              IIgetfldio((short *)0,1,32,HEXID_SIZE + 1-1,client_pid,(char *)"\
client_pid");
            } /* IIfsetio */
          }
/* # line 124 "sesdtail.qsc" */	/* host code */
	if (client_pid[0] == '<')   /* no pid to use */
	{
	    POP_MSG("Could not obtain client process information for this\
 session.  This is because a PID could not be obtained because this session\
 is being run from a batch job, detached process or is internal.");
/* # line 129 "sesdtail.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 130 "sesdtail.qsc" */	/* host code */
	}
	showproc(client_pid);
#else
	POP_MSG("Process_Info (client) is available only on VMS");
/* # line 134 "sesdtail.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 135 "sesdtail.qsc" */	/* host code */
#endif 
        }
      } else if (IIretval() == 4) {
        {
/* # line 140 "sesdtail.qsc" */	/* host code */
	my_help("ipmsesdt.hlp", "Session List");
        }
      } else if (IIretval() == 5) {
        {
/* # line 144 "sesdtail.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 148 "sesdtail.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
        if (dosrvoper(GC_SERV_DISCONNECT, (char *)NULL) != OK)
        {
            POP_MSG("Error disconnecting from server");
        }
/* # line 153 "sesdtail.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 154 "sesdtail.qsc" */	/* breakdisplay */
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
    if (IInmuact((char *)"MoreQuery",(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(Procinfo,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 157 "sesdtail.qsc" */	/* host code */
    if (e_xit) {		/* user wants to leave */
	terminate(OK);
    }
    return;
}
