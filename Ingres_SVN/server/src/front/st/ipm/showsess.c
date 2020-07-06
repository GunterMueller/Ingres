# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
** All rights reserved.
*/
#include <compat.h>
#include <cs.h>
#include <dbms.h>
#include <fe.h>
#include <pc.h>
#include <lk.h>
#include <lg.h>
#include <er.h>
#include "lists.h"
#include "servinfo.h"
#include "ingpriv.h"
#include "msgs.h"
#include "dba.h"
VOID showsess();
VOID sessdetail();
i4   chkingpriv();
FUNC_EXTERN VOID tablefield_find();
/*
** constants for session status - if user removes a session, we track it
** to prevent them from doing it again.
*/
#define SESSION_OK		0
#define SESSION_TERMINATED	1
#define SESSION_SUSPENDED	2
/*
** Declare file wide statics
*/
#if defined(LP64)
static char shwsess[] = "fshowsess64";
#else
static char shwsess[] = "fshowsess";
#endif  /* LP64 */
  static char *Fshowsess = shwsess;
/*
**
**  File: showsess.qc
**
**  Purpose - this file contains the routines which display the session
**	information form.  
**
**  This file contains:
**	showsess - displays the session list form. 
**
**  History
**	1/6/89		tomt	created
**	4/5/89		tomt	added routines to display the data
**	4/7/89		tomt	added sess suspend,resume and remove
**				functionality
**	8/24/89		tomt	eliminated dispsessinfo routine and replaced
**				with dispsrvinfo.  Added refresh menuitem.
**      3/12/90         tomt    changed appearance of showsess and sessdetail
**                              forms.
**	6/19/90		tomt	no need to connect with server to delete
**				sessions since connection is already 
**				established before this routine is called.
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	18-may-1995 (harpa06)
**		Cross Integration bug fix for 56349 by nick: ensure we
**		disconnect from the server if processing Quit() ; End() 
**		doesn't require this.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-oct-2001 (somsa01)
**	    Added references to the 64-bit forms.
*/
VOID
showsess(server_name)
PTR server_name;	/* need this to physically connect to server */
{
    STATUS	status;
    bool	e_xit = FALSE;
  int session_stat;
  char sess_id[HEXID_SIZE + 1], colname[FE_MAXNAME + 1];
  u_i4 currow;
  int sess_count = 0;
    /* Display the form and get the information */
/* # line 96 "showsess.qsc" */	/* display */
  {
    if (IIdispfrm(Fshowsess,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 99 "showsess.qsc" */	/* host code */
	/* Put information on the form */
/* # line 100 "showsess.qsc" */	/* inittable */
      {
        if (IItbinit(Fshowsess,(char *)"sessdet",(char *)"r") != 0) {
          IIthidecol((char *)"sess_stat",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 101 "showsess.qsc" */	/* host code */
	dispsrvinfo(DISP_SESS_LIST, server_name, (char *)0);
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 106 "showsess.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fshowsess,(char *)"sessdet",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&session_stat,(char *)
"sess_stat");
              IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,sess_id,(char *)"\
session_id");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 108 "showsess.qsc" */	/* host code */
	if (session_stat == SESSION_TERMINATED) {
	    POP_MSG("this session has been terminated by you");
	}
	sessdetail(server_name, sess_id);
        }
      } else if (IIretval() == 2) {
        {
/* # line 115 "showsess.qsc" */	/* display */
          {
            if (IInestmu() == 0) goto IIfdE2;
            goto IImuI2;
IIfdI2:;
            if (IIfsetio((char *)0) == 0) goto IIfdE2;
            {
/* # line 119 "showsess.qsc" */	/* host code */
	    /* Verify the user is a superuser, else refuse access */
	    if (chkingpriv(ING_SUPERUSER) != OK) {
		POP_MSG("You are not a superuser, no privilege for attempted operation");
/* # line 122 "showsess.qsc" */	/* breakdisplay */
              {
                if (1) goto IIfdE2;
              }
/* # line 123 "showsess.qsc" */	/* host code */
            }
            }
IIfdB2:
            while (IIrunnest() != 0) {
              if (IIretval() == 1) {
                {
/* # line 129 "showsess.qsc" */	/* getrow */
                  {
                    if (IItbsetio(2,Fshowsess,(char *)"sessdet",-2) != 0) {
                      IItcogetio((short *)0,1,30,4,&session_stat,(char *)
"sess_stat");
                      IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,sess_id,
                      (char *)"session_id");
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
/* # line 131 "showsess.qsc" */	/* host code */
	    if (session_stat == SESSION_TERMINATED) {
		POP_MSG("This session has already been terminated by you");
/* # line 133 "showsess.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB2;
                  }
/* # line 134 "showsess.qsc" */	/* host code */
	    }
	    status = dosrvoper(REMOVE_SESSION, sess_id);
	    if (status != OK) {
		POP_MSG("Error deleting session");
/* # line 139 "showsess.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB2;
                  }
/* # line 140 "showsess.qsc" */	/* host code */
	    }
	    session_stat = SESSION_TERMINATED;
/* # line 142 "showsess.qsc" */	/* putrow */
                  {
                    if (IItbsetio(3,Fshowsess,(char *)"sessdet",-2) != 0) {
                      IItcoputio((char *)"sess_stat",(short *)0,1,30,4,&
                      session_stat);
                      IITBceColEnd();
                    } /* IItbsetio */
                  }
                }
              } else if (IIretval() == 2) {
                {
/* # line 146 "showsess.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(4,0,Fshowsess,(char *)"sessdet",(char *)0) != 
                    0) {
                      IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,
                      (char *)0,0);
                    } /* IIiqset */
                  }
/* # line 147 "showsess.qsc" */	/* host code */
	    tablefield_find();
/* # line 148 "showsess.qsc" */	/* resume */
                  {
                    IIrescol((char *)"sessdet",colname);
                    if (1) goto IIfdB2;
                  }
                }
              } else if (IIretval() == 3) {
                {
/* # line 152 "showsess.qsc" */	/* host code */
	    my_help("ipmdbaop.hlp", "DBA Operations");
                }
              } else if (IIretval() == 4) {
                {
/* # line 156 "showsess.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE2;
                  }
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
            if (IInmuact((char *)"Delete",(char *)0,2,2,1) == 0) goto IIfdE2;
            if (IInmuact((char *)"Find",(char *)0,2,2,2) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(7,(char *)0,2,2,2) == 0) goto IIfdE2;
            if (IInmuact((char *)"Help",(char *)0,2,2,3) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
            if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
            if (IIendmu() == 0) goto IIfdE2;
            goto IIfdI2;
IIfdE2:;
            IIendnest();
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 161 "showsess.qsc" */	/* message */
          {
            IImessage((char *)"Loading session list...");
          }
/* # line 162 "showsess.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fshowsess,(char *)"sessdet",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&currow,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 163 "showsess.qsc" */	/* host code */
	if ((status = load_servinfo(GET_SERVINFO, server_name) != OK))
	{
	    POP_MSG("Error getting session list.  Server may not exist\
 or server may not be accepting connects.  To get an updated list of servers,\
 return to previous screen and select the 'Refresh' menuitem");
/* # line 168 "showsess.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 169 "showsess.qsc" */	/* host code */
	}
/* # line 170 "showsess.qsc" */	/* clear */
          {
            IIfldclear((char *)"sessdet");
          }
/* # line 171 "showsess.qsc" */	/* host code */
	dispsrvinfo(DISP_SESS_LIST, server_name, (char *)0);
/* # line 172 "showsess.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Fshowsess,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&sess_count,32,(char *)"sessd\
et",0);
            } /* IIiqset */
          }
/* # line 173 "showsess.qsc" */	/* host code */
	if (currow > sess_count)
	{
	    currow = sess_count;
	}
/* # line 177 "showsess.qsc" */	/* scroll */
          {
            if (IItbsetio(1,Fshowsess,(char *)"sessdet",-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,currow) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 181 "showsess.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,Fshowsess,(char *)"sessdet",(char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 182 "showsess.qsc" */	/* host code */
	tablefield_find();
/* # line 183 "showsess.qsc" */	/* resume */
          {
            IIrescol((char *)"sessdet",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 187 "showsess.qsc" */	/* host code */
	my_help("ipmshses.hlp", "Session List");
        }
      } else if (IIretval() == 6) {
        {
/* # line 191 "showsess.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 195 "showsess.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
        if (dosrvoper(GC_SERV_DISCONNECT, (char *)NULL) != OK)
        {
            POP_MSG("Error disconnecting from server");
        }
/* # line 200 "showsess.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 201 "showsess.qsc" */	/* breakdisplay */
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
    if (IInmuact((char *)"More_Info",(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"DBA_Operations",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Find",(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 204 "showsess.qsc" */	/* host code */
    if (e_xit) {		/* user wants to leave */
	terminate(OK);
    }
    return;
}
