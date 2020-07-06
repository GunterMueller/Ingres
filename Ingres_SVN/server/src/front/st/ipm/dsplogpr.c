# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** includes and defines
*/
#include <compat.h>
#include <si.h>
#include <cs.h>
#include <st.h>
#include <dbms.h>
#include <fe.h>
#include <pc.h>
#include <lg.h>
#include <lk.h>
#include <er.h>
#include        <adf.h>
#include        <ft.h>
#include        <fmt.h>
#include        <frame.h>
#include "lists.h"
#include "msgs.h" 
#include "dba.h"
/*
**  File: dsplogpr.qc
**
**  Purpose - this file contains the routines which display the active processes
**	(in logging system) information form.  
**
**  This file contains:
**	displgpr() - displays a list of processes in the logging system form. 
**	printlgpr() - gets, formats and outputs logging system process information.
**	get_logpid() - given an internal id, returns the PID for a given process
**
**  History
**	8/29/89		tomt	created
**	9/2/89		tomt	added code to display data
**	9/6/89		tomt	added get_logpid()
**	9/19/89		tomt	do addforms here
**	10/1/89		tomt	integrated 6.3 changes
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	 1-Nov-1995 (nick)
**		Remove unused variable.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	18-sep-2003 (abbjo03)
**	    Include pc.h ahead of lg.h.
*/
/*
** forward and external declarations
*/
VOID printlgpr();
VOID get_lklgstats();
VOID stat_decode();
VOID tablefield_find();
VOID get_logpid();
/*
** Declare file wide vars
*/
  static char Fdisplgproc[]= "fdisplgproc";
static char Lgprstat[] = "RECOVERY,ARCHIV,FCTDBMS,RUNAWAY,DBMS,CKPDB,VOID,\
SHR_BM,IDLE,DEAD,DYING";
/*
** Routine: displgpr()  displays a list of processes in the logging
**		system on a popup form
**
** Inputs: none
**
** Outputs: displays a form
**
** Side effects: assumes the forms system is active
*/
VOID
displgpr()
{
    bool	e_xit = FALSE;
  int list_count;
  char colname[FE_MAXNAME + 1];
    static bool	forms_added = FALSE;
  char *Procinfo =
#ifdef VMS 
			"Process_Info";
#else
			"";
#endif
  GLOBALREF int *fdisplgproc;
    /*
    **  Determine if forms are added, if not addform them.  Also
    **  don't add forms if output is being directed to a file
    */
    if (!forms_added)
    {
/* # line 115 "dsplogpr.qsc" */	/* addform */
  {
    IIaddform(fdisplgproc);
  }
/* # line 116 "dsplogpr.qsc" */	/* host code */
	forms_added = TRUE;
    }
    /* Display the form and get the information */
/* # line 120 "dsplogpr.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fdisplgproc,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 124 "dsplogpr.qsc" */	/* host code */
	/* Put the info in the tablefield */
/* # line 125 "dsplogpr.qsc" */	/* inittable */
      {
        if (IItbinit(Fdisplgproc,(char *)"logprocess",(char *)"r") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 126 "dsplogpr.qsc" */	/* host code */
	printlgpr();
	/* get number of processes */
/* # line 129 "dsplogpr.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,Fdisplgproc,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)"logproces\
s",0);
        } /* IIiqset */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 133 "dsplogpr.qsc" */	/* host code */
	/* Put the info in the tablefield */
/* # line 134 "dsplogpr.qsc" */	/* message */
          {
            IImessage((char *)"Loading logging information...");
          }
/* # line 135 "dsplogpr.qsc" */	/* clear */
          {
            IIfldclear((char *)"logprocess");
          }
/* # line 136 "dsplogpr.qsc" */	/* host code */
        get_lklgstats(GET_LOGPROCESS, (PTR) 0);
	printlgpr();	/* put info in tablefield */
	/* get number of processes */
/* # line 140 "dsplogpr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Fdisplgproc,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)
"logprocess",0);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 145 "dsplogpr.qsc" */	/* host code */
#ifdef VMS 
          char cpid[11];
	if (list_count == 0) 
	{
	    POP_MSG("There are no processes to get information on");
/* # line 151 "dsplogpr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 152 "dsplogpr.qsc" */	/* host code */
	}
/* # line 153 "dsplogpr.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdisplgproc,(char *)"logprocess",-2) != 0) {
              IItcogetio((short *)0,1,32,10,cpid,(char *)"pr_pid");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 154 "dsplogpr.qsc" */	/* host code */
	showproc(cpid);
#else
	POP_MSG("Process_Info (logging system) is available only on VMS");
/* # line 157 "dsplogpr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 158 "dsplogpr.qsc" */	/* host code */
#endif 
        }
      } else if (IIretval() == 3) {
        {
/* # line 162 "dsplogpr.qsc" */	/* host code */
	if (list_count == 0) {
	    POP_MSG("There is nothing to find");
/* # line 164 "dsplogpr.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 165 "dsplogpr.qsc" */	/* host code */
	}
/* # line 166 "dsplogpr.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,Fdisplgproc,(char *)"logprocess",(char *)0) != 0) 
            {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 167 "dsplogpr.qsc" */	/* host code */
	tablefield_find();
/* # line 168 "dsplogpr.qsc" */	/* resume */
          {
            IIrescol((char *)"logprocess",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 172 "dsplogpr.qsc" */	/* host code */
	my_help("ipmdplgp.hlp", "Log Process");
        }
      } else if (IIretval() == 5) {
        {
/* # line 176 "dsplogpr.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 180 "dsplogpr.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
/* # line 181 "dsplogpr.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 182 "dsplogpr.qsc" */	/* breakdisplay */
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
    if (IInmuact(Procinfo,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Find",(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,3) == 0) goto IIfdE1;
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
/* # line 185 "dsplogpr.qsc" */	/* host code */
    if (e_xit) {		/* user wants to leave */
	terminate(OK);
    }
    return;
}
/*
** printlgpr() - this routine gets, formats and outputs the logging system
**	database information.
**
** Inputs:
**	none
**
** Outputs:
**	formatted logging process information to screen or file
**
** Returns:
**	none
**
** History:
**	8/27/89		tomt	written
**	9/2/89		tomt	added code to display logging process info
**	10/1/89		tomt	integrated 6.3 changes
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
*/
VOID
printlgpr()
{
    bool	reset = TRUE;	/* always start at beginning of list */
  char pr_id[HEXID_SIZE + 1], pr_pid[HEXID_SIZE + 1], pr_type[20];
    LG_PROCESS	*lpb;
  i4 pr_stat_database;
  i4 pr_stat_write;
  i4 pr_stat_force;
  i4 pr_stat_wait;
  i4 pr_stat_begin;
  i4 pr_stat_end;
    while ((lpb = (LG_PROCESS *)l_retrieve(LOGPROCINFO, 0, &reset)) != NULL) 
    {
	/*
	**    do formatting for display
	*/
	STprintf(pr_id, "%x", lpb->pr_id);
	STprintf(pr_pid, "%x", lpb->pr_pid);
	stat_decode(pr_type, Lgprstat, (u_i4) lpb->pr_type);
	if (lpb->pr_type == 0)	/* just in case, make it clear on scr */
	    STcopy("<none>", pr_type);
	pr_stat_database = lpb->pr_stat.database;
	pr_stat_write = lpb->pr_stat.write;
	pr_stat_force = lpb->pr_stat.force;
	pr_stat_wait = lpb->pr_stat.wait;
	pr_stat_begin = lpb->pr_stat.begin;
	pr_stat_end = lpb->pr_stat.end;
/* # line 244 "dsplogpr.qsc" */	/* loadtable */
  {
    if (IItbact(Fdisplgproc,(char *)"logprocess",1) != 0) {
      IItcoputio((char *)"pr_id",(short *)0,1,32,0,pr_id);
      IItcoputio((char *)"pr_pid",(short *)0,1,32,0,pr_pid);
      IItcoputio((char *)"pr_type",(short *)0,1,32,0,pr_type);
      IItcoputio((char *)"pr_open",(short *)0,1,30,4,&pr_stat_database);
      IItcoputio((char *)"pr_write",(short *)0,1,30,4,&pr_stat_write);
      IItcoputio((char *)"pr_force",(short *)0,1,30,4,&pr_stat_force);
      IItcoputio((char *)"pr_wait",(short *)0,1,30,4,&pr_stat_wait);
      IItcoputio((char *)"pr_begin",(short *)0,1,30,4,&pr_stat_begin);
      IItcoputio((char *)"pr_end",(short *)0,1,30,4,&pr_stat_end);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 249 "dsplogpr.qsc" */	/* host code */
    }
    return;
}
/*
** get_logpid - this routine gets the PID for a specified process given
**	an external PID.
**
** Inputs:
**	external_pid - pointer to a buffer to place the PID
**	internal_pid - the internal process id
**
** Outputs:
**	value placed into buffer pointed to by external_id
**
** Returns:
**	none
**
** History:
**	9/6/89		tomt	written
*/
VOID
get_logpid(external_pid, internal_pid)
PTR external_pid;
i4 internal_pid;
{
    bool	reset = TRUE;	/* start at beginning of linked list */
    bool	found = FALSE;
    LG_PROCESS	*lpb;
    while ((lpb = (LG_PROCESS *)l_retrieve(LOGPROCINFO, 0, &reset)) != NULL)
    {
	/*
	**    determine if internal pids match
	*/
	if (lpb->pr_id == internal_pid)
	{
	    STprintf(external_pid, "%x", lpb->pr_pid);
	    found = TRUE;
	    break;	/* done */
	}
    }
    if (!found)
	STcopy("<none>", external_pid);
    return;
}
