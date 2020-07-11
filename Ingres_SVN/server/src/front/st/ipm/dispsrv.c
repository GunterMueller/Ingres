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
#include <si.h>
#include <cs.h>
#include <dbms.h>
#include <fe.h>
#include <me.h>
#include <cm.h>
#include <pc.h>
#include <lg.h>
#include <er.h>
#include <nm.h>
#include <lk.h>
#include <lo.h>
#include <si.h>
#include <st.h>
#include <cv.h>
#include <adf.h>
#include <ft.h>
#include <fmt.h>
#include <frame.h>
#include "lists.h"
#include "dba.h"
#include "msgs.h"
#include "ingpriv.h"
#include "servinfo.h"
/*
** Constants for keeping track of server state
*/
#define SERVER_TERMINATED	1
#define SERVER_UP		0
#define INFO_OBTAINED		1	/* server info already retrieved */
#define INFO_NEEDED		0	/* need to get server info */
/*
** constants for session status - if user removes a session, we track it
** to prevent her from doing it again.
*/
#define SESSION_OK		0
#define SESSION_TERMINATED	1
#define SESSION_SUSPENDED	2
/* forward/external references */
char	*sub_str();	/* return a PTR to the substr in string */
i4	get_pid_from_name();	/* given a server name - returns a pid */
i4	get_pid_from_dev();	/* given a device name - returns a pid */
i4	chkingpriv();
bool	shutdown_submenu();	/* shutdown submenu for server list */
i4	d_scan(), d_scan_sub();
char	*s_scan(), *s_scan_sub(), *wordify(), *state_sym(), *state_unsym(),
	*fac_sym(), *servtype_sym();
VOID	showproc();
VOID	showsess();
bool	has_clients();
FUNC_EXTERN	VOID	string_move();
FUNC_EXTERN	VOID	geting_error();
/*
** Globals
**
** curr_servinfo - this pointer is used by st_servinfo.  load_servinfo
**	walks thru server linked list using the server names placed there
**	by st_nameinfo.  load_servinfo calls the routines which get the
**	info from the DBMS server.  These routines call st_servinfo which
**	must know the pointer to the server info struct to store the info.
**	This variable allows the pointer obtained in load_servinfo to be
**	used by st_servinfo.  This hack is done to avoid radically changing
**	code derived from IIMONITOR.
** curr_servlistno - this int stores the number of the current server list.
**	In order to store session information (which is a sublist off each
**	server list element), we need to know what position the current
**	server list is.  This number starts at one.
** curr_sessinfo - same as curr_servinfo
*/
static SESSION_INFO *curr_sessinfo;
static SERVER_INFO *curr_servinfo;
static i4  curr_servlistno;
#if defined(LP64)
static char sessdtl[] = "fsessdetail64";
static char shwsess[] = "fshowsess64";
#else
static char sessdtl[] = "fsessdetail";
static char shwsess[] = "fshowsess";
#endif  /* LP64 */
  static char Fdispsrv[]= "fdispsrv";
  static char *Fsessdetail = sessdtl;
  static char *Fshowsess = shwsess;
  static char Fservdetail[]= "fservdetail";
  static char Fqrytext[]= "fqrytext";
/*
**
**  File: dispsrv.qc
**
**  Purpose - this file contains the routines which display the server
**	information form.  
**
**  This file contains:
**	dispsrv - displays the server information form. 
**	load_servinfo - gets list of DBMS servers from Name server and
**		gets info from a specified server
**	st_nameinfo - called by gcn_format to place name info in linked list
**	st_servinfo - called by getsrvdata to place serv info in linked list
**	dispsrvinfo - places server info on form.
**
**  History
**	1/6/89		tomt	created
**	3/21/89		tomt	get data and place on screen
**	3/29/89		tomt	added routine to place data in linked
**				list. Also added routines to display
**				info from DBMS servers.
**	4/5/89		tomt	fixing session format session
**	4/7/89		tomt	now perform GCA_INIT in this routine
**				only need to do this once coming in
**				Also perform GCA_TERMINATE on the way
**				out.  Also set state of server from
**				the perspective of this program.
**				If the user terminates the server,
**				a hidden column is set.
**	8/22/89		tomt	combined get_servinfo and get_nameinfo
**				into load_servinfo
**	8/23/89		tomt	added refresh menuitem
**	9/19/89		tomt	do addforms here now
**	6/19/90		tomt	changed server connection behaviour so
**				that the connection is made when the
**				session or server detail menuitem is
**				selected and not disconnected until
**				returning to server list screen.  This
**				yields better performance because the
**				connection is not made every time the
**				user selects a refresh menuitem.
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**	12-sep-91 (kirke)
**	    Removed unused cindex() reference.
**      5/26/93 (vijay)
**              include pc.h
**	14-sep-93 (robf)
**          Add support for enhanced security info in session details
**	15-sep-93 (robf)
**	    Change hard-coded 18 for server name to FE_MAXNAME, fixes
**	    problem where long server names got truncated unexpectedly.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**      02-Dec-94  (liibi01)
**              Bug 46411, cross integration from 6.4.
**              Fixed nested UNLOADTABLE bug. 
**      08-Dec-94  (liibi01)
**              Bug 46455 & 49084, cross integration from 6.4.
**              Harmonized various string buffer lengths to GCN matching
**              values. This solves several problems with string truncation
**              causing failed connections, incorrect database list displays
**              etc.
**	25-sep-95 (nick)
**	    Call l_srvsort() after getting the server list to remove
**	    duplicate server entries - #48542.
**      12-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's change for axp_osf
**	31-mar-94 (kchin)
**	    Changed type of i_sessid from i4 to long, & call 
**	    CVahxl8() and sprintf() rather than CVahxl() and 
**	    STprintf() in routine dispsrvinfo().  This is to fix 
**	    64-bit address being truncated.
**	11-Sep-96 (gordy)
**	    GCA Initialization must be paired with Termination.
**      24-mar-1999 (hweho01)
**          Extended the changes date 31-mar-94 by kchin for
**          axp_osf to ris_u64 (AIX 64-bit platform).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	10-Dec-1999 (jenjo02)
**	    Added display of OS thread id to session detail, when
**	    available.
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      21-Sep-2000 (hanje04)
**          Extended the changes dated 31-mar-94, by kchin, for
**          axp_osf, to axp_lnx (Alpha Linux)
**	13-oct-2001 (somsa01)
**	    Porting changes for LP64.
**	21-oct-2001 (somsa01)
**	    Only initialize the 64-bit forms if LP64 is defined.
**      24-Oct-2001 (horda03)
**         Changes to the layout of "SHOW SERVER" and "FORMAT USER" in IIMONITOR
**         results in bad information being displayed by IPM for Idle Quantum
**         count and Process Info for Batch processes (processes where the
**         terminal type is unknown).
**         SHOW SERVER now returns the "Active High Water" value, also for user
**         sessions the Client PID is returned. Bugs 105200 and 104438.
**	18-sep-2003 (abbjo03)
**	    Include pc.h ahead of lg.h.
*/
/*
** Routine:	dispsrv()
**
** Purpose: displays the server list form
**
** Inputs:
**	none
**
** Outputs:
**	displays form
**
** Returns:
**	status - FAIL if server list cant be displayed on startup
**
** History
**	1/6/89		tomt	Written
**	9/19/89		tomt	do addforms here now
**	6/19/90		tomt	when selecting shutdown of a server, the
**				server indicated when the menuitem is selected
**				is connected to and the connection is maintained
**				until the submenu is exited.  This means its
**				harder to issue a soft shutdown and not be
**				able to issue a hard shutdown.  This occured
**				because IPM would connect, issue the command
**				and disconnect.  Once a 'set server shut' is
**				issued and you disconnect, it is impossible
**				to reconnect.  This change helps eliminate
**				that problem.
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      12/8/94       liibi01   Change string lengths to GCN values
**	25/9/95		nick	call l_srvsort()
**	11/1/95		nick	remove flag_file
**	11-Sep-96 (gordy)
**	    GCA Initialization must be paired with Termination.
*/
static bool	fd_forms_added = FALSE;
STATUS
dispsrv()
{
    STATUS	status;
    bool	reset_flag = TRUE;
    bool	e_xit = FALSE;
  i4 server_stat, server_count, server_info_stat, rownum;
  char server_name[GCN_VAL_MAX_LEN + 1];
  char i_service[GCN_TYP_MAX_LEN + 1];
#ifdef VMS
    i4		pid;
    char	cpid[11];
#endif
  char *Procinfo =
#ifdef VMS 
			"Process_Info";
#else
			"";
#endif
    SERVER_INFO *servinfo;
  GLOBALREF int *fdispsrv;
  GLOBALREF int *fsessdetail;
  GLOBALREF int *fsessdetail64;
  GLOBALREF int *fservdetail;
  GLOBALREF int *fshowsess;
  GLOBALREF int *fshowsess64;
  GLOBALREF int *fqrytext;
    /*
    **  Determine if forms are added, if not addform them.  Also
    **  don't add forms if output is being directed to a file
    */
    if (!fd_forms_added)
    {
/* # line 282 "dispsrv.qsc" */	/* addform */
  {
    IIaddform(fdispsrv);
  }
/* # line 283 "dispsrv.qsc" */	/* host code */
#if defined(LP64)
/* # line 284 "dispsrv.qsc" */	/* addform */
  {
    IIaddform(fsessdetail64);
  }
/* # line 285 "dispsrv.qsc" */	/* addform */
  {
    IIaddform(fshowsess64);
  }
/* # line 286 "dispsrv.qsc" */	/* host code */
#else
/* # line 287 "dispsrv.qsc" */	/* addform */
  {
    IIaddform(fsessdetail);
  }
/* # line 288 "dispsrv.qsc" */	/* addform */
  {
    IIaddform(fshowsess);
  }
/* # line 289 "dispsrv.qsc" */	/* host code */
#endif  /* LP64 */
/* # line 290 "dispsrv.qsc" */	/* addform */
  {
    IIaddform(fservdetail);
  }
/* # line 291 "dispsrv.qsc" */	/* addform */
  {
    IIaddform(fqrytext);
  }
/* # line 292 "dispsrv.qsc" */	/* host code */
	fd_forms_added = TRUE;
    }
    status = OK;		/* XXX why need to initialize here? -- jpk */
/* # line 297 "dispsrv.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fdispsrv,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 301 "dispsrv.qsc" */	/* message */
      {
        IImessage((char *)"Loading server list...");
      }
/* # line 302 "dispsrv.qsc" */	/* inittable */
      {
        if (IItbinit(Fdispsrv,(char *)"serverlist",(char *)"f") != 0) {
          IIthidecol((char *)"row_num",(char *)"i2");
          IIthidecol((char *)"server_stat",(char *)"i2");
          IIthidecol((char *)"server_info_stat",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 305 "dispsrv.qsc" */	/* host code */
        if ((status = load_servinfo(GET_SERVLIST, (char *)NULL)) != OK)
	{
	    POP_MSG("Could not obtain server list");
/* # line 308 "dispsrv.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE1;
      }
/* # line 309 "dispsrv.qsc" */	/* host code */
	}
	/* sort the list of servers ( by connection id ) */
	l_srvsort();
	/* get number of servers */
/* # line 315 "dispsrv.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,Fdispsrv,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&server_count,32,(char *)"serverl\
ist",0);
        } /* IIiqset */
      }
/* # line 316 "dispsrv.qsc" */	/* host code */
	/* place info on form */
	dispsrvinfo(DISP_SERV_LIST, (char *)NULL, (char *)NULL); 
	srvsess();
/* # line 319 "dispsrv.qsc" */	/* clear */
      {
        IIfldclear((char *)"serverlist");
      }
/* # line 320 "dispsrv.qsc" */	/* host code */
	dispsrvinfo(DISP_SERV_LIST, (char *)NULL, (char *)NULL); 
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 324 "dispsrv.qsc" */	/* host code */
	if (server_count == 0)
	{
	    POP_MSG("There are no servers to get information on");
/* # line 327 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 328 "dispsrv.qsc" */	/* host code */
	}
/* # line 329 "dispsrv.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdispsrv,(char *)"serverlist",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&server_stat,(char *)
"server_stat");
              IItcogetio((short *)0,1,30,4,&server_info_stat,(char *)
"server_info_stat");
              IItcogetio((short *)0,1,32,GCN_VAL_MAX_LEN + 1-1,server_name,
              (char *)"server");
              IItcogetio((short *)0,1,32,GCN_TYP_MAX_LEN + 1-1,i_service,
              (char *)"service");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 332 "dispsrv.qsc" */	/* host code */
	if (server_stat == SERVER_TERMINATED)
	{
	    POP_MSG("This server has been terminated by you");
/* # line 335 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 336 "dispsrv.qsc" */	/* host code */
	}
	if (!has_clients(i_service))
	{
	    POP_MSG("only INGRES or STAR servers have more information");
/* # line 340 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 341 "dispsrv.qsc" */	/* host code */
	}
/* # line 343 "dispsrv.qsc" */	/* message */
          {
            IImessage((char *)"Loading server information...");
          }
/* # line 345 "dispsrv.qsc" */	/* host code */
	/*
	**  Initialize the GCA, connect to the desired server and get the
	**  information.  Don't disconnect until after return from the 
	**  detail form.  This means the connection is good until the user
	**  returns to the server list form.
	*/
	if ((status = dosrvoper(GC_SERV_INIT, (char *)NULL)) != OK)
	{
	    POP_MSG("Error initializing connect to server.");
/* # line 354 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 355 "dispsrv.qsc" */	/* host code */
	}
	if ((status = dosrvoper(GC_SERV_CONNECT, server_name)) != OK)
	{
	    status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL);
	    POP_MSG("Error getting server information.  Server may not exist\
 or may be refusing connects.  To get an updated list of servers, select the\
 \"Refresh\" menuitem");
/* # line 363 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 364 "dispsrv.qsc" */	/* host code */
	}
	if ((status = load_servinfo(GET_SERVINFO, server_name)) != OK)
	{
	    status = dosrvoper(GC_SERV_DISCONNECT, (char *)NULL); 
	    status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL);
	    POP_MSG("Error getting server information.  To get an updated list\
 of servers, select the \"Refresh\" menuitem");
/* # line 372 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 373 "dispsrv.qsc" */	/* host code */
	}
	servdetail(server_name);	/* call the detail form */
	if ((status = dosrvoper(GC_SERV_DISCONNECT, (char *)NULL)) != OK)
	{
	    POP_MSG("Error disconnecting from server");
	}
        if ((status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL)) != OK)
	{
	    POP_MSG("Error shuting down communications");
	}
        }
      } else if (IIretval() == 2) {
        {
/* # line 386 "dispsrv.qsc" */	/* host code */
	if (server_count == 0) 
	{
	    POP_MSG("There are no servers to get session information on");
/* # line 389 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 390 "dispsrv.qsc" */	/* host code */
	}
/* # line 391 "dispsrv.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdispsrv,(char *)"serverlist",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&server_stat,(char *)
"server_stat");
              IItcogetio((short *)0,1,30,4,&server_info_stat,(char *)
"server_info_stat");
              IItcogetio((short *)0,1,32,GCN_VAL_MAX_LEN + 1-1,server_name,
              (char *)"server");
              IItcogetio((short *)0,1,32,GCN_TYP_MAX_LEN + 1-1,i_service,
              (char *)"service");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 394 "dispsrv.qsc" */	/* host code */
	if (server_stat == SERVER_TERMINATED)
	{
	    POP_MSG("This server has been terminated by you");
/* # line 397 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 398 "dispsrv.qsc" */	/* host code */
	}
	if (!has_clients(i_service))
	{
	    POP_MSG("only INGRES or STAR servers have sessions");
/* # line 402 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 403 "dispsrv.qsc" */	/* host code */
	}
	/*
	**  Initialize the GCA, connect to the desired server and get the
	**  information.  Don't disconnect until after return from the 
	**  detail form.  This means the connection is good until the user
	**  returns to the server list form.
	*/
/* # line 411 "dispsrv.qsc" */	/* message */
          {
            IImessage((char *)"Loading session information...");
          }
/* # line 412 "dispsrv.qsc" */	/* host code */
	if ((status = dosrvoper(GC_SERV_INIT, (char *)NULL)) != OK)
	{
	    POP_MSG("Error initializing GCA.");
/* # line 415 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 416 "dispsrv.qsc" */	/* host code */
	}
	if ((status = dosrvoper(GC_SERV_CONNECT, server_name)) != OK)
	{
	    status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL);
	    POP_MSG("Error getting server information.  Server may not exist\
 or may be refusing connects.  To get an updated list of servers, select the\
 \"Refresh\" menuitem");
/* # line 424 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 425 "dispsrv.qsc" */	/* host code */
	}
	if ((status = load_servinfo(GET_SERVINFO, server_name)) != OK)
	{
	    status = dosrvoper(GC_SERV_DISCONNECT, (char *)NULL); 
	    status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL);
	    POP_MSG("Error getting server information.  To get an updated list\
 of servers, select the \"Refresh\" menuitem");
/* # line 433 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 434 "dispsrv.qsc" */	/* host code */
	}
	showsess(server_name);		/* call the detail form */
	if ((status = dosrvoper(GC_SERV_DISCONNECT, (char *)NULL)) != OK)
	{
	    POP_MSG("Error disconnecting from server.");
	}
        if ((status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL)) != OK)
	{
	    POP_MSG("Error shuting down communications");
	}
        }
      } else if (IIretval() == 3) {
        {
/* # line 448 "dispsrv.qsc" */	/* host code */
#ifdef VMS 
	if (server_count == 0) 
	{
	    POP_MSG("There are no servers to get information on");
/* # line 452 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 453 "dispsrv.qsc" */	/* host code */
	}
/* # line 454 "dispsrv.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdispsrv,(char *)"serverlist",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&server_stat,(char *)
"server_stat");
              IItcogetio((short *)0,1,32,GCN_VAL_MAX_LEN + 1-1,server_name,
              (char *)"server");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 456 "dispsrv.qsc" */	/* host code */
	if (server_stat == SERVER_TERMINATED) 
	{
	    POP_MSG("This server has been terminated by you");
	}
	/*
	**  Get pid based on server_name - convert it to appropriate format
	**  based on whether the user wants pids displayed in hex or decimal
	*/
	if ((pid = get_pid_from_name(server_name)) > 0)  /* got a pid */
	{
	    STprintf(cpid, "%x", pid);
        } 
	else 
	{
	    POP_MSG("Could not obtain the process ID (PID) for this server");
/* # line 472 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 473 "dispsrv.qsc" */	/* host code */
	}
	showproc(cpid);
#else
	POP_MSG("Process_Info (server) is available only on VMS");
/* # line 478 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 479 "dispsrv.qsc" */	/* host code */
#endif /* VMS */
        }
      } else if (IIretval() == 4) {
        {
/* # line 484 "dispsrv.qsc" */	/* host code */
	/* Refuse access unless the user is a superuser */
	if (chkingpriv(ING_SUPERUSER) != OK) 
	{
	    POP_MSG("You are not a superuser, no privilege for attempted operation");
/* # line 488 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 489 "dispsrv.qsc" */	/* host code */
        }
	if (server_count == 0)
	{
	    POP_MSG("There are no servers to shutdown");
/* # line 494 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 495 "dispsrv.qsc" */	/* host code */
	}
	/* Connect to server */
/* # line 498 "dispsrv.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdispsrv,(char *)"serverlist",-2) != 0) {
              IItcogetio((short *)0,1,32,GCN_VAL_MAX_LEN + 1-1,server_name,
              (char *)"server");
              IItcogetio((short *)0,1,30,4,&server_stat,(char *)
"server_stat");
              IItcogetio((short *)0,1,30,4,&rownum,(char *)"_RECORD");
              IItcogetio((short *)0,1,32,GCN_TYP_MAX_LEN + 1-1,i_service,
              (char *)"service");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 501 "dispsrv.qsc" */	/* host code */
	if (server_stat == SERVER_TERMINATED) 
	{
	    POP_MSG("This server has already been terminated by you");
/* # line 504 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 505 "dispsrv.qsc" */	/* host code */
	}
	if (!has_clients(i_service))
	{
	    POP_MSG("only INGRES or STAR sessions may be shutdown");
/* # line 509 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 510 "dispsrv.qsc" */	/* host code */
	}
	if ((status = dosrvoper(GC_SERV_INIT, (char *)NULL)) != OK)
	{
	    POP_MSG("Error initializing connect to server.");
/* # line 515 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 516 "dispsrv.qsc" */	/* host code */
	}
	if (status = dosrvoper(GC_SERV_CONNECT, server_name) != OK)
	{
	    status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL);
	    POP_MSG("Error connecting to DBMS server");
/* # line 522 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 523 "dispsrv.qsc" */	/* host code */
	}
	/*
	**  Because readonly tablefields don't allow activates to keep user
	**  from scrolling off the server they want to shutdown, only option
	**  is a submenu.  But submenus exit when a submenu item is picked.  
	**  So the hack is to put the submenu in a loop. - If there is 
	**  a better way, do it.
	*/
	for (;;)
	{
	    if (shutdown_submenu())	/* user wants to exit submenu */
	    {
		break;	/* leave the submenu */
	    }
	}
        if ((status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL)) != OK)
	{
	    POP_MSG("Error shuting down communications");
	}
        }
      } else if (IIretval() == 5) {
        {
/* # line 547 "dispsrv.qsc" */	/* message */
          {
            IImessage((char *)"Loading server list...");
          }
/* # line 548 "dispsrv.qsc" */	/* host code */
	if ((status = load_servinfo(GET_SERVLIST, server_name)) != OK)
	{
	    POP_MSG("Error getting server list.  The Nameserver may not exist\
 or may be refusing connects.  That problem must be corrected before any\
 server information can be obtained.");
/* # line 553 "dispsrv.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 554 "dispsrv.qsc" */	/* host code */
	}
	/* sort the list of servers ( by connection id ) */
	l_srvsort();
/* # line 559 "dispsrv.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Fdispsrv,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&server_count,32,(char *)
"serverlist",0);
            } /* IIiqset */
          }
/* # line 560 "dispsrv.qsc" */	/* host code */
	dispsrvinfo(DISP_SERV_LIST, (char *)NULL, (char *)NULL); 
	srvsess();
/* # line 562 "dispsrv.qsc" */	/* clear */
          {
            IIfldclear((char *)"serverlist");
          }
/* # line 563 "dispsrv.qsc" */	/* host code */
	dispsrvinfo(DISP_SERV_LIST, (char *)NULL, (char *)NULL); 
        }
      } else if (IIretval() == 6) {
        {
/* # line 567 "dispsrv.qsc" */	/* host code */
	my_help("ipmdspsv.hlp", "Server List");
        }
      } else if (IIretval() == 7) {
        {
/* # line 571 "dispsrv.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 575 "dispsrv.qsc" */	/* host code */
	e_xit = TRUE;	/* user wants to leave */
/* # line 576 "dispsrv.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 577 "dispsrv.qsc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Sessions",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(Procinfo,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Shutdown",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,8) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,8) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 580 "dispsrv.qsc" */	/* host code */
    if (e_xit) 	/* user wants to leave */
	terminate(OK);
    return(status);
}
/*
** shutdown_submenu() - this routine contains the shutdown submenu.
**	The caller  calls this routine in a loop.  Using a  ##SUBMENU
**	keeps the cursor on the command line so the user can't scroll
**	from the server that was highlighted when the submenu was selected.
**	Calling this in a loop forces the SUBMENU not to exit (as far ast the
**	user is concerned).
**
** Inputs
**	none.  This routine assumes that a server connection (iimonitor
**	type connection) has been made by the caller.
**
** Returns
**	status - TRUE if the submenu should be exited, FALSE if the
**		submenu should stay displayed.
**
** History
**	6/25/90		tomt	Written
**     12/8/94       liibi01    Fix string length to GCN value
*/
bool
shutdown_submenu()
{
    STATUS 	status;
    bool 	breakloop = FALSE;	/* default is keep submenu displayed */
  i4 serv_stat;
  char y_or_n[10];
  char serv_name[GCN_VAL_MAX_LEN + 1];
  char warntext[100];
/* # line 616 "dispsrv.qsc" */	/* submenu */
  {
    goto IImuI2;
IIfdB2:
    IIrunmu(0);
    if (IIretval() == 1) {
      {
/* # line 619 "dispsrv.qsc" */	/* getrow */
        {
          if (IItbsetio(2,Fdispsrv,(char *)"serverlist",-2) != 0) {
            IItcogetio((short *)0,1,32,GCN_VAL_MAX_LEN + 1-1,serv_name,
            (char *)"server");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 620 "dispsrv.qsc" */	/* host code */
	STprintf(warntext, "Soft shutdown of %s: are you sure? ", serv_name);
/* # line 621 "dispsrv.qsc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,warntext,(short *)0,1,32,9,y_or_n);
        }
/* # line 622 "dispsrv.qsc" */	/* host code */
	if ((y_or_n[0] != 'Y') && (y_or_n[0] != 'y'))
	{
	    return breakloop;
	}
	status = dosrvoper(SET_SERVER_SHUT, (char *)0);   /* soft shut */
	if (status != OK) 
	{
	    POP_MSG("Error shutting down DBMS server");
	    return TRUE;
	}
	POP_MSG("Server will terminate gracefully");
	serv_stat = SERVER_TERMINATED;
/* # line 636 "dispsrv.qsc" */	/* putrow */
        {
          if (IItbsetio(3,Fdispsrv,(char *)"serverlist",-2) != 0) {
            IItcoputio((char *)"server_stat",(short *)0,1,30,4,&serv_stat);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 637 "dispsrv.qsc" */	/* host code */
	breakloop = FALSE;	/* don't exit submenu */
      }
    } else if (IIretval() == 2) {
      {
/* # line 641 "dispsrv.qsc" */	/* getrow */
        {
          if (IItbsetio(2,Fdispsrv,(char *)"serverlist",-2) != 0) {
            IItcogetio((short *)0,1,32,GCN_VAL_MAX_LEN + 1-1,serv_name,
            (char *)"server");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 642 "dispsrv.qsc" */	/* host code */
	STprintf(warntext, "Hard shutdown of %s: are you sure? ", serv_name);
/* # line 643 "dispsrv.qsc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,warntext,(short *)0,1,32,9,y_or_n);
        }
/* # line 644 "dispsrv.qsc" */	/* host code */
	if ((y_or_n[0] != 'Y') && (y_or_n[0] != 'y'))
	{
	    return breakloop;
	}
	breakloop = TRUE;
	status = dosrvoper(STOP_SERVER, (char *)0);   /* hard shutdown */
	if (status != OK) 
	{
	    POP_MSG("Error shutting down DBMS server");
	    return breakloop;
	}
	POP_MSG("Server terminated, connection broken");
	serv_stat = SERVER_TERMINATED;
/* # line 659 "dispsrv.qsc" */	/* putrow */
        {
          if (IItbsetio(3,Fdispsrv,(char *)"serverlist",-2) != 0) {
            IItcoputio((char *)"server_stat",(short *)0,1,30,4,&serv_stat);
            IITBceColEnd();
          } /* IItbsetio */
        }
      }
    } else if (IIretval() == 3) {
      {
/* # line 663 "dispsrv.qsc" */	/* host code */
	my_help("ipmshut.hlp", "Server Shutdown");
	breakloop = FALSE;	/* don't exit submenu */
/* # line 665 "dispsrv.qsc" */	/* clear */
        {
          IIclrscr();
        }
/* # line 666 "dispsrv.qsc" */	/* redisplay */
        {
          IIredisp();
        }
      }
    } else if (IIretval() == 4) {
      {
/* # line 670 "dispsrv.qsc" */	/* host code */
        breakloop = TRUE;
	/* disconnect from server */
	status = dosrvoper(GC_SERV_DISCONNECT, (char *)0);
	if (status != OK)
	{
	    POP_MSG("Error disconnecting from DBMS server");
	}
      }
    } else {
      if (1) goto IIfdE2;
    } /* IIretval */
    goto IIfdE2;
IImuI2:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact((char *)"Soft_Shutdown",(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact((char *)"Hard_Shutdown",(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact((char *)"Help",(char *)0,2,2,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdB2;
IIfdE2:;
  }
/* # line 678 "dispsrv.qsc" */	/* host code */
    return breakloop;
}
/*
** load_servinfo() - this routine calls the routines which get the information
**	from the nameserver and specified DBMS servers.  
**
** Inputs
**	operation - the operation the caller wants, one of
**				GET_SERVLIST - get a list of servers
**				GET_SERVINFO - get all info for a server or all
**
**	serverptr - the name of the server to get info on.  
**
** Returns
**	status - OK if no errors, else error code
**
** History
**	3/29/89		tomt	Written
**	4/7/89		tomt	calling routines need to perform
**				GC_TERMINATE
**	8/22/89		tomt	combined get_nameinfo and get_servinfo into
**				load_servinfo
**	8/24/89		tomt	added options to this routine:
**				GET_SERVLIST - get a list of servers
**				GET_SERVINFO - get all info for a server or all
**	6/19/90		tomt	removed init of gca, server connect and
**				disconnect from this routine.  This means
**				that once a server is selected, the connection
**				lasts until the user (or caller) decides
**				to shut it down.  The gca init and server
**				connection MUST be done before this routine
**				is called (from dispsrv()).
**      12/8/94     liibi01     Fix string lengths to GCN
**      18/4/96     rajus01     Added BRIDGE to server list.
**	20/Jun/05	wanfr01	Bug 114712, INGCBT571
**				Added diag lines for increased -L output
*/
STATUS
load_servinfo(operation, serverptr)
i4  operation;
char *serverptr;
{
    STATUS	status;
    char 	lserver_name[GCN_VAL_MAX_LEN + 1]; 
    bool	reset_flag = TRUE;
    bool	reset_flag1 = TRUE;
    char 	server_name[GCN_VAL_MAX_LEN + 1];
    bool	server_list = FALSE, server_info = FALSE;
    SERVER_INFO *i_serv;	/* i_nternal to this function */
    PTR		s_ptr = NULL;
    /* determine if caller wants ALL servers info */
    switch (operation)
    {
      case GET_SERVLIST:		/* get a list of servers only */
	server_list = TRUE;	
	break;
      case GET_SERVINFO:		/* get info on one server */
	if (serverptr == NULL)
	{
	    return(FAIL);		/* must specify a server name */
	}
	STlcopy(serverptr, lserver_name, sizeof(lserver_name) - 1);
	server_info = TRUE;	
	break;
      default:
	break;
    }
    /*
    **  Get info from name server if caller wants a refresh of server list.
    **  Otherwise don't do anything with name server.  Init GCA, then connect
    **  and obtain info.  Finally, clean up GCA association
    */
    if (server_list)
    {
        /* init GCA for name */
        if ((status = get_name_info(GC_NAME_INIT, (char *)NULL)) != OK)
	    return(status);
	l_free(SERVERINFO, 0);	/* free up memory in server linked list */
	if ((status = get_name_info(GC_NAME_CONNECT, (char *)NULL)) != OK)
	    return status;
	/* DBMS servers */
	if ((status = get_name_info(NAME_SHOW, (char *)0)) != OK)
	    return status;
	/* is II_DBMS_SERVER set? */
	NMgtAt("II_DBMS_SERVER", &s_ptr);
	if ((s_ptr == 0) || (*s_ptr == 0))
	{	/* no, perform lookups for other servers */
	    d_print("II_DBMS_SERVER is not set\n");
	    /* star servers */
	    if ((status = get_name_info(NAME_SHOW, "STAR")) != OK)
		return status;
	    /* comm servers */
	    if ((status = get_name_info(NAME_SHOW, "COMSVR")) != OK)
		return status;
	    if ((status = get_name_info(NAME_SHOW, "BRIDGE")) != OK)
		return status;
	    /* name server herself */
	    if ((status = get_name_info(NAME_SHOW, "IINMSVR")) != OK)
		return status;
	}
        else
	    d_print("II_DBMS_SERVER is set\n");
	if ((status = get_name_info(GC_NAME_DISCONNECT, (char *)NULL)) != OK)
	    return status;
    }
    if (server_info)
    {
	/* Walk thru server linked list */
	STzapblank(lserver_name, lserver_name);
	for (curr_servlistno = 1,
	    curr_servinfo = (SERVER_INFO *)l_retrieve(SERVERINFO, 0,
		&reset_flag);
	    curr_servinfo != NULL;
	    curr_servlistno++,
	    curr_servinfo = (SERVER_INFO *)l_retrieve(SERVERINFO, 0,
		(bool *)NULL))
	{
	    STlcopy(curr_servinfo->server, server_name,
		sizeof(server_name) - 1);
            STzapblank(server_name, server_name);
	    /* compare the server names and only get info when they match */
	    if (STcompare(server_name, lserver_name) == 0)
	    {
		/* freemem for this server*/
		l_free(SERVERINFO, curr_servlistno);	
		/* get serverinfo */
		status = dosrvoper(GET_SERVER_INFO, (char *)NULL);    
		if (status != OK) 
		    break; 
		/* get session info */
		status = dosrvoper(GET_SESSION_INFO, (char *)NULL);
		if (status != OK) 
		    break; 
		/*
		** Step thru session list which was JUST built and place the 
		** format info there
		*/
		for (curr_sessinfo = (SESSION_INFO *)l_retrieve(SERVERINFO, 
				curr_servlistno, &reset_flag1)
			; curr_sessinfo != NULL
			; curr_sessinfo = (SESSION_INFO *)l_retrieve(SERVERINFO,
				curr_servlistno, (bool *)NULL))
		{
		    d_print("session loop driver, session <%s>\n",
			curr_sessinfo->session_id);
		    /* get session info */
	            status = dosrvoper(GET_FORMAT_SESSION, 
				curr_sessinfo->session_id);
	            if (status != OK) 
			break; 
		}
		if (status != OK) 
		    break;
	    }	
	}  
    }
    return(status);
}
/*
** st_nameinfo - this routine places the passed information into
**	the linked list which holds the server info.  It is called
**	from gcn_format()
**	XXX no, called from gcn_respond or directly from get_name_info -- jpk
**
** History
**	3/29/89		tomt	written
**	24 July 1991	jpk	commented
*/
st_nameinfo(ctype, cobject, cdata)
char *ctype, *cobject, *cdata;
{
    STATUS	status;
    SERVER_INFO *servinfo;
    /*
    **  Store the server list info - place data into struct and pass
    **  struct addr to l_add to place in linked list.
    */
    if ((servinfo = (SERVER_INFO *)MEreqmem((u_i4)0, 
	(u_i4)sizeof(SERVER_INFO), FALSE, &status)) == NULL) 
    { 
	terminate(FAIL, "st_nameinfo: error allocating memory for server list information");
    }
    STlcopy(ctype, servinfo->service, sizeof(servinfo->service) - 1);
    STlcopy(cobject, servinfo->database, sizeof(servinfo->database) - 1);
    STlcopy(cdata, servinfo->server, sizeof(servinfo->server) - 1);
    servinfo->ssn_cur = 0;
    servinfo->ssn_max = 0;
    servinfo->act_cur = 0;
    servinfo->act_max = 0;
    servinfo->q_cur = 0;
    servinfo->q_max = 0;
    l_add(SERVERINFO, 0, 0, (PTR) servinfo); /* put it away */
    return;
}
/*
** st_servinfo() - this routine stores the information passed in by
**	getsrvdata into the linked list.  Note that this routine is
**	called by the routines called by load_servinfo.  Because of
**	the disjoint relationship of st_servinfo and load_servinfo,
**	we need curr_servlistno and curr_servinfo file wide statics
**	so that st_servinfo knows what linked list element is being
**	referred to
**
** History
**	3/29/89		tomt	Written
**	5/3/90		tomt	added monitor enhancements for 6.3 maint rel
**	12/13/92	jpk	support longer strings and new ones from SCF
**				(session user)
**	15-Feb-1993	jpk	Modify to support new SCF strings: no more
**				group id at startup or role id at startup;
**				terminal now after both username and
**				username at session startup, not between
**				them; new format for a monitor session.
**	15-nov-93 (robf)
**         Add support session description
**      31-Aug-98 (wanya01)
**         Bug 89176: IPM "Session Detail" has corrupted information during
**         DBMS recovery of a session.  
**      25-Oct-2001 (horda03)
**         When scanning the GET_SERVER_INFO buffer, a Active Highwater count is
**         supplied, this value is now stored in the SERVER_INFO block.
**         When scanning the GET_FORMAT_SESSION buffer, the PID of the user
**         connected to the session may be provided as the "Client PID" data. This
**         value (if specified) is now added to the SESSION_INFO block.
**         Bugs 104438 and 105200.
*/
/* FIXLEN_GET fixed length get of size S into object O to parse out thing W.
** This macro hardwires in bufptr variable!  Otherwise function call
** would have to pass &bufptr as arg every time.
*/
#define FIXLEN_GET(O,S,W) if (STlength(bufptr) < S) \
	{terminate(FAIL, \
	    "fixlen_get: length of \"%s\" is %d, expected %d for %s", \
	    bufptr, STlength(bufptr), S, W);} \
	else {STlcopy(bufptr, O, S); wordify(O); bufptr += S;}
/* FIXLEN_ADV fixed length advance to thing Q, step over thing W.
** This macro also hardwires in bufptr variable for same reason.
*/
#define FIXLEN_ADV(W,Q) if (STlength(bufptr) < STlength(W)) \
	{terminate(FAIL, \
	    "fixlen_adv: advance to %s expected \"%s\" (%d) got \"%s\" (%d)", \
	    Q, W, STlength(W), bufptr, STlength(bufptr));} \
	else {bufptr += STlength(W);}
st_servinfo(opcode, servdata)
int opcode;
char *servdata;
{
    char	database[FE_MAXNAME + 1];
    char	roleid[FE_MAXNAME + 1];
    char	groupid[FE_MAXNAME + 1];
    char	cur_facname[7];
    char	activity[MMAXSTR];
    char	act_records[11];
    char	act_logaddr[25];
    char	terminal[10];
    char	apparent_user[FE_MAXNAME + 1];
    char	real_user[FE_MAXNAME + 1];
    char	dba[FE_MAXNAME + 1];
    char	appcode[9];
    char	cur_fac[9];
    char	querytext[ER_MAX_LEN];
    char	bitbucket[150];		/* used to throw things out */
    char	*bufptr = servdata;
    char	*holder = NULL, was_holder;
    STATUS	status;
    char	*tmp;
    /* Take the passed info, parse it and put into appropriate struct */
    switch(opcode) 
    {
    case GET_SERVER_INFO:
	/*
	** 		Server: 10000 ^M
	** sessions 19.(55.) active: 0.(55.)^M
	** rdy mask 00008080 state: 00000020^M
	** idle quantums 0./0. (100.) 
	*/
	(VOID) s_scan(&bufptr, 2, "server: ID");/* move past "Server: 10000" */
	curr_servinfo->ssn_cur = (u_i4) d_scan(&bufptr, 1, "ssn_cur");
	curr_servinfo->ssn_max = (u_i4) d_scan(&bufptr, 1, "ssn_max");
	curr_servinfo->act_cur = (u_i4) d_scan(&bufptr, 1, "act_cur");
	curr_servinfo->act_max = (u_i4) d_scan(&bufptr, 1, "act_max");
	curr_servinfo->act_hw  = (u_i4) d_scan(&bufptr, 1, "act_hw");
	STlcopy(s_scan(&bufptr, 4, "rdy mask"), curr_servinfo->ready_mask,
	    HEXID_SIZE);
	STlcopy(s_scan(&bufptr, 2, "srv state"), curr_servinfo->state,
	    HEXID_SIZE);
	curr_servinfo->q_cur = (u_i4) d_scan(&bufptr, 1, "q_cur");
	curr_servinfo->q_max = (u_i4) d_scan(&bufptr, 1, "q_max");
        break;
    case GET_SESSION_INFO:
	/* session 00222D90 ( <idle job> ) cs_state: CS_COMPUTABLE cs_mask: */
	d_print("GET_SESSION_INFO:\n");
	/* 
	** XXX Gross hack to handle GCA concatenating strings. 
	** Warning: will fail if "session" occurs as a session name! 
	*/
	/* for each session */
	while (sub_str(bufptr, "Session ") != NULL)
	{
	    /* if another session, null terminate (otherwise already is) */
	    if ((holder = sub_str(bufptr + 3, "Session ")) != NULL)
		*holder = EOS;
	    if ((curr_sessinfo = (SESSION_INFO *)MEreqmem((u_i4)0,
		    (u_i4)sizeof(SESSION_INFO), TRUE, &status)) == NULL)
	    { 
		terminate(FAIL, "st_servinfo: error allocating session list");
	    }
	    STcopy(" ", curr_sessinfo->cs_mask);/* opt. so initialize */
	    /* 6 possible formats; all 6 have session id "Session 002BC5D8" */
	    STlcopy(wordify(s_scan(&bufptr, 2, "session id")),
		curr_sessinfo->session_id, HEXID_SIZE);
	    d_print("\tsession_id=\"%s\"\n", curr_sessinfo->session_id);
	    /*
	    ** If thread id appended to session id ("Session 002BC5D8:122")
	    ** extract thread_id.
	    */
	    if ( *bufptr++ == ':' )
		STcopy(s_scan(&bufptr, 1, "thread id"), curr_sessinfo->thread_id);
	    else
		STcopy("", curr_sessinfo->thread_id);
	    FIXLEN_ADV("(", "session name");
	    /* and all 6 formats have session names */
	    FIXLEN_GET(curr_sessinfo->session_name, OLDINGSIZE, "sess name");
	    d_print("\tsession_name=\"%s\"\n", curr_sessinfo->session_name);
	    s_scan(&bufptr, 2, "past cs_state");/* past "%) cs_state: " */
	    /* but the 5 formats vary starting here */
	    /* first one special case */
	    if (sub_str(bufptr, "cs_state: <invalid>") != NULL)
	    {
		/* cs_state: <invalid> %x */
		STlcopy(s_scan(&bufptr, 2, "null state"),
			curr_sessinfo->cs_state, STlength("<invalid>"));
		break;
	    }
	    /* 4 remaining cases all have a cs_state */
	    STcopy(s_scan(&bufptr, 1, "cs_state"), curr_sessinfo->cs_state);
	    /* then they vary */
	    if (sub_str(curr_sessinfo->cs_state, "CS_FREE") != NULL ||
		sub_str(curr_sessinfo->cs_state, "CS_COMPUTABLE") != NULL ||
		sub_str(curr_sessinfo->cs_state, "CS_STACK_WAIT") != NULL ||
		sub_str(curr_sessinfo->cs_state, "CS_UWAIT") != NULL)
	    {
		/* cs_state: %w cs_mask: %v */
		s_scan(&bufptr, 1, "past cs_mask");/* move past "cs_mask:" */
		STcopy(bufptr, curr_sessinfo->cs_mask);
		wordify(curr_sessinfo->cs_mask);
	    }
	    else if (sub_str(curr_sessinfo->cs_state, "CS_EVENT_WAIT") != NULL)
	    {
		/* cs_state: %w (%s) cs_mask: %v */
		STcopy(wordify(s_scan(&bufptr, 1, "waitfor")),
		    curr_sessinfo->waitfor);
		d_print("\twaitfor=\"%s\"\n", curr_sessinfo->waitfor);
		s_scan(&bufptr, 1, "(wait) past cs_mask");/* past "cs_mask:" */
		STcopy(bufptr, curr_sessinfo->cs_mask);
		wordify(curr_sessinfo->cs_mask);
	    }
	    else if (sub_str(curr_sessinfo->cs_state, "CS_MUTEX") != NULL)
	    {
		/* cs_state: %w ((%s) %x) cs_mask: %v */
		FIXLEN_ADV(" ((", "mutexed s_or_x");
		FIXLEN_GET(curr_sessinfo->mutex_sx, 1, "mutexed s or x");
		FIXLEN_ADV(")", "mutexed session");
		STlcopy(s_scan(&bufptr, 1, "mutex"), curr_sessinfo->mutex_sess,
		    HEXID_SIZE);	/* get mutex_sess */
		s_scan(&bufptr, 1, "(mutex) past cs_mask");
		STcopy(bufptr, curr_sessinfo->cs_mask);
		wordify(curr_sessinfo->cs_mask);
	    }
	    else if (sub_str(curr_sessinfo->cs_state, "CS_CNDWAIT") != NULL)
	    {
		/* cs_state: %w (%x) cs_mask: %v */
		FIXLEN_ADV(" (", "cndwait sess");
		STlcopy(s_scan(&bufptr, 1, "cndwait"), curr_sessinfo->cndwait,
		    HEXID_SIZE);
		s_scan(&bufptr, 1, "(cndwait) past cs_mask");
		STcopy(bufptr, curr_sessinfo->cs_mask);
		wordify(curr_sessinfo->cs_mask);
	    }
	    else
	    {
		terminate(FAIL, "unrecognized session list format: %s",
		    curr_sessinfo->cs_state);
	    }
	    d_print("\tcs_state=\"%s\"\n", curr_sessinfo->cs_state);
	    d_print("\tcs_mask=\"%s\"\n", curr_sessinfo->cs_mask);
	    /* finished.  add to list. */
	    l_add(SERVERINFO, curr_servlistno, 0, (PTR) curr_sessinfo); 
	    /* XXX undo dreadful hack - restore next "session" header. */
	    if (holder != NULL)
	    {
		*holder = 'S';
		bufptr = holder;		/* advance to next session */
	    }
	}
        break;
    case GET_FORMAT_SESSION:
	/*
	** ">>>>>Session_002F4900<<<<<____DB_Name:__________________________\
	** (Owned_by___________________________)____\
	** User:__<Write_Behind_Thread>___(_<Write_Behind_Thread>__)_\
        ** User Name at Session Startup:  <Dead Process Detector>\
	** Terminal:_NONE________\
        ** Group_Id:________________________________\
	** Role_Id:_____________________________\
	** Application_Code:_00000000____________\
	** Current_Facility:_<None>_(00000000)____\
	**************** each of the next two lines is optional
	** Act._Detail:_11._log_records_processed,_log_address_27e6a616:750a94\
	**_Activity:_Waiting_on_event_lock_signal_(LKevent)____\
	** [Description: some_description]
	**_Query:_"
	*/
	curr_sessinfo->act_records = 0;		/* all opt. so initialize */
        curr_sessinfo->client_pid = 0;
	STcopy("", curr_sessinfo->act_logaddr);
	STcopy("", curr_sessinfo->activity);
	STcopy("", curr_sessinfo->act_det);
	/* two special cases, no further information available */
	if (sub_str(bufptr, "is an internal task") != NULL ||
	    sub_str(bufptr, "Invalid session id") != NULL)
	{
	    break;
	}
	/*
	** INGRES/Star is a special case too, but some info available:
	** >>>>>Session 002A6040<<<<<
	** DB_Name:_distdb_____________Terminal:_ttypc
	** DB_Owner:_______________________User:_jpk___(jpk________)
	** Application_Code:_00000000____________Current_Facility:_00000000
	**/
	if (sub_str(bufptr, "DB Owner: ") != NULL)
	{
	    /* strip newlines -- they occur in wrong places */
	    tr_dash_d(bufptr, "\n");
	    /* past ">>>Session 002A6040<<< DB Name:" */
	    s_scan(&bufptr, 4, "(star) past sessid, dbname");
	    bufptr++;					/* past " " */
	    FIXLEN_GET(curr_sessinfo->database, OLDINGSIZE, "(star) dbname");
	    d_print("\tdatabase=\"%s\"\n", curr_sessinfo->database);
	    if (*bufptr == '(')				/* dbname(exclusive) */
	    {
		curr_sessinfo->db_exclusive = TRUE;
		/* move past "(exclusive)" */
		s_scan(&bufptr, 1, "(star) past exclusive");
	    }
	    bufptr++;					/* past " " */
	    s_scan(&bufptr, 1, "(star) past terminal");	/* past "Terminal:" */
	    bufptr++;					/* past " " */
	    FIXLEN_GET(curr_sessinfo->terminal, 8, "tty");
	    d_print("\tterminal=\"%s\"\n", curr_sessinfo->terminal);
	    s_scan(&bufptr, 2, "(star) past db owner");	/* past "DB Owner:" */
	    bufptr++;					/* past " " */
	    FIXLEN_GET(curr_sessinfo->dba, OLDINGSIZE, "dba");
	    s_scan(&bufptr, 1, "(star) past user");	/* past "User:" */
	    bufptr++;					/* move past " " */
	    FIXLEN_GET(curr_sessinfo->apparent_user, OLDINGSIZE,
		"(star) apparent user");
	    d_print("\tapparent_user=\"%s\"\n", curr_sessinfo->apparent_user);
	    bufptr += 2;				/* past " (" */
	    FIXLEN_GET(curr_sessinfo->real_user, OLDINGSIZE,
		"(star) real user");
	    d_print("\treal_user=\"%s\"\n", curr_sessinfo->real_user);
	    bufptr++;					/* past ")" */
	    s_scan(&bufptr, 2, "(star) past app code");	/* past "App. Code:" */
	    bufptr++;					/* past " " */
	    FIXLEN_GET(curr_sessinfo->appcode, HEXID_SIZE, "(star) app. cd.");
	    d_print("\tappcode=\"%s\"\n", curr_sessinfo->appcode);
	    s_scan(&bufptr, 2, "(star) past curfac");	/* past "Cur. Fac.:" */
	    bufptr++;					/* past " " */
	    FIXLEN_GET(curr_sessinfo->cur_fac, 8, "(star) fac.cd.");
	    d_print("\tcur_fac=\"%s\"\n", curr_sessinfo->cur_fac);
	    break;
	}
	/* monitor sessions have sessids but not dbnames or dbas */
	if (sub_str(bufptr, "Activity: (Monitor Session)") != NULL)
	{
	    s_scan(&bufptr, 2, "past sessid (monitor session)");
	}
	else
	{
	    /* past ">>>Session 002F4900<<< DB Name:" */
	    s_scan(&bufptr, 4, "past sessid, dbname");
	    bufptr++;						/* past " " */
	    FIXLEN_GET(curr_sessinfo->database, DB_MAXNAME, "dbname");
	    d_print("\tdatabase=\"%s\"\n", curr_sessinfo->database);
	    if (*bufptr == '(')		/* dbname(exclusive) possiblility */
	    {
		curr_sessinfo->db_exclusive = TRUE;
		s_scan(&bufptr, 1, "past exclusive");/* past "(exclusive)" */
	    }
	    s_scan(&bufptr, 2, "past owned by");	/* past "(Owned by:" */
	    bufptr++;					/* past " " */
	    FIXLEN_GET(curr_sessinfo->dba, DB_MAXNAME, "dba");/* get dba */
	    d_print("\tdba=\"%s\"\n", curr_sessinfo->dba);
	    bufptr += 2;					/* past " )" */
	}
	s_scan(&bufptr, 1, "past user");		/* past "User:" */
	bufptr++;					/* past " " */
 	FIXLEN_GET(curr_sessinfo->apparent_user, DB_MAXNAME, "app user");
	d_print("\tapparent_user=\"%s\"\n", curr_sessinfo->apparent_user);
	bufptr += 2;					/* past " (" */
	/* get real user */
 	FIXLEN_GET(curr_sessinfo->real_user, DB_MAXNAME, "realuser");
	d_print("\treal_user=\"%s\"\n", curr_sessinfo->real_user);
	bufptr++;					/* past ")" */
 	s_scan(&bufptr, 5, "past user name at session startup");/* past that */
 	bufptr++;					/* past " " */
 	FIXLEN_GET(curr_sessinfo->startup_user, DB_MAXNAME, "startup_user");
 	d_print("\tstartup_user=\"%s\"\n", curr_sessinfo->startup_user);
	s_scan(&bufptr, 1, "past terminal");		/* past "Terminal:" */
	bufptr++;					/* past " " */
	FIXLEN_GET(curr_sessinfo->terminal, 16, "tty");	/* get terminal */
	d_print("\tterminal=\"%s\"\n", curr_sessinfo->terminal);
	/* monitor sessions have only an activity */
	if (sub_str(bufptr, "Activity: (Monitor Session)") != NULL)
	{
	    s_scan(&bufptr, 1, "past activity");	/* past "Activity:" */
	    bufptr++;					/* past " " */
	    STlcopy(bufptr, curr_sessinfo->activity, MMAXSTR - 1);
	    wordify(curr_sessinfo->activity);
	    break;
	}
	s_scan(&bufptr, 2, "past groupid");		/* past "Group Id:" */
	bufptr++;					/* past " " */
	/* get group id */
	FIXLEN_GET(curr_sessinfo->groupid, DB_MAXNAME, "groupid");
	d_print("\tgroupid=\"%s\"\n", curr_sessinfo->groupid);
	s_scan(&bufptr, 2, "past roleid");		/* past "Role Id:" */
	bufptr++;					/* past " " */
	/* get role id */
	FIXLEN_GET(curr_sessinfo->roleid, DB_MAXNAME, "roleid");
	d_print("\troleid=\"%s\"\n", curr_sessinfo->roleid);
	bufptr++;					/* past " " */
	s_scan(&bufptr, 2, "past App Code");	/* past "App. Code:" */
	bufptr++;					/* past " " */
	/* get app code */
	FIXLEN_GET(curr_sessinfo->appcode, HEXID_SIZE, "app code");
	d_print("\tappcode=\"%s\"\n", curr_sessinfo->appcode);
	s_scan(&bufptr, 2, "past cur fac");		/* past "Cur. Fac.:" */
	STcopy(wordify(s_scan(&bufptr, 1, "cur_facname")),
	    curr_sessinfo->cur_facname);		/* get fac name */
	d_print("\tcur_facname=\"%s\"\n", curr_sessinfo->cur_facname);
	bufptr += 2;					/* past " (" */
	FIXLEN_GET(curr_sessinfo->cur_fac, 8, "fac. cd");/* facility code */
	d_print("\tcur_fac=\"%s\"\n", curr_sessinfo->cur_fac);
	bufptr++;					/* past " " */
        if ((tmp = sub_str(bufptr, "Activity:")) != NULL)
        {
         bufptr = tmp;
         s_scan(&bufptr, 1, "past activity");        /* past "Activity:" */
         bufptr++;                                   /* past " " */
         tmp=sub_str(bufptr, "Act. Detail:");
         if(tmp!=NULL)
             STlcopy(bufptr, curr_sessinfo->activity, tmp-bufptr-1);
         else
             STlcopy(bufptr, curr_sessinfo->activity, MMAXSTR);
         d_print("\tactivity=\"%s\"\n", curr_sessinfo->activity);
         STtrmwhite(curr_sessinfo->activity);
        }
	/*
	** Either, neither, or both activity and act. detail can occur.
	** If act. detail. occurs, it will occur somewhere before activity.
	** If act. detail. occurs, it will occur directly after fac. cd.
        ** Act. Detail: 1. log record processed, log address 2883bcc9:9cde0
	*/
	/* if activity detail AND log record(s) AND log address, trust it */
	if (sub_str(bufptr, "Act. Detail:") != NULL)
	{
	    s_scan(&bufptr, 1, "past act. det.");	/* past "Act. Det:" */
	    if (sub_str(bufptr, "log record") != NULL &&
	    	sub_str(bufptr, "log addr") != NULL)
	    {
		curr_sessinfo->act_records = d_scan(&bufptr, 1, "act_records");
		d_print("\tact_records=\"%d\"\n", curr_sessinfo->act_records);
		if (*bufptr == '.')
		    bufptr++;				/* past "." */
		/* past "log records processed, log address", get logaddr */
		STcopy(s_scan(&bufptr, 9, "logaddr"),
		    curr_sessinfo->act_logaddr);
		d_print("\tact_logaddr=\"%s\"\n", curr_sessinfo->act_logaddr);
	    }
	    else	/* DON'T trust it */
	    {
		/* but save activity detail if we can */
		if ((tmp = sub_str(bufptr, "Activity:")) != NULL ||
		    (tmp = sub_str(bufptr, "Description:")) != NULL ||
		    (tmp = sub_str(bufptr, "Query:")) != NULL)
		{
		    STlcopy(bufptr, curr_sessinfo->act_det, tmp - bufptr - 1);
		    d_print("\tact_det=\"%s\"\n", curr_sessinfo->act_det);
		    wordify(curr_sessinfo->act_det);
		}
	    }
	}
        if ((tmp = sub_str(bufptr, "Client pid:")) != NULL)
        {
            /* Client PID information contained. However, we only set
            ** the value if the process id is specified.
            */
            bufptr = tmp;
            s_scan(&bufptr, 2, "past Client pid:"); /* past "Client pid:" */
            bufptr++;
            curr_sessinfo->client_pid = (CMdigit(bufptr)) ? 
                                   d_scan_sub(&bufptr, "Get Client Pid") : 0;
        }
	if ((tmp = sub_str(bufptr, "Description:")) != NULL)
	{
	    bufptr = tmp;
	    s_scan(&bufptr, 1, "past Description:");	/* past "Description:" */
	    bufptr++;
	    /* Note: Depends on Query: always following Description: */
	    tmp=sub_str(bufptr, "Query:");
	    if(tmp!=NULL)
		    STlcopy(bufptr, curr_sessinfo->sess_desc, tmp-bufptr-4);
	    else
		    STlcopy(bufptr, curr_sessinfo->sess_desc, MAX_DESC_LEN);
	    STtrmwhite(curr_sessinfo->sess_desc);
	    d_print("\tdescription=\"%s\"\n", curr_sessinfo->sess_desc);
	    /* Scan forward if known */
	    if(tmp)
		    bufptr=tmp;
	}
	if ((tmp = sub_str(bufptr, "Query:")) != NULL)
	{
	    bufptr = tmp;
	    s_scan(&bufptr, 1, "past Query");		/* past "Query:" */
	    STlcopy(bufptr, curr_sessinfo->querytext, ER_MAX_LEN);/* get qry */
	    d_print("\tquerytext=\"%s\"\n", curr_sessinfo->querytext);
	    tr_dash_d(curr_sessinfo->querytext, "\n");
	    wordify(curr_sessinfo->querytext);
	}
    break;
    case SET_SERVER_SHUT:
    case STOP_SERVER:
    case SUSPEND_SESSION:
    case RESUME_SESSION:
    case REMOVE_SESSION:
	dispmsg(servdata, MSG_LINE, (u_i4)2);
        break;
    }
    return;
}
/*
** dispsrvinfo - this routine gets the values out of the server info linked
**	list and places them on the screen.
**
**
** Inputs
**	getwhat - what server information to display.  One of
**		DISP_SERV_LIST	- A list of servers
**		DISP_SERV_INFO  - Info on a specified server i.e. quantum etc.
**		DISP_SESS_LIST  - Session list for a specified server
**		DISP_SESS_INFO	- Info on a particular session
**
**		This parameter is ignored if outputting to a file
**
**	serverptr - what server to display data for
**	sessionptr - the session to display data for
**
** History
**	3/23/89		tomt	Written
**	3/29/89		tomt	gets info from linked list instead
**	8/23/89		tomt	added operations so caller can specify
**				what info to display
**	5/3/90		tomt	added new fields on session detail form
**				and changed strings to string pointers.
**      08-Dec-94(liibi01)
**          Fix string lengths to GCN
**      18-Apr-96 (rajus01)
**          Added BRIDGE to server list.
**	14-dec-1998 (nanpr01)
**	    Session Id's are unsigned i4's rather than signed
**	    causing to display 0 in TPC-C Session ID's.
**      25-Oct-2001 (horda03)
**          The PID of the session is now stored in the SESSION_INFO block
**          if provided. If no PID suplied, try to get the PID from the
**          terminal ID. If no PID is determined, then we must set the
**          first character of the client_pid variable to '<' to ensure
**          that the wrong process information isn't displayed.
**          Bug 104438.
*/
/* puts string "<none>" into otherwise empty strings */
#define NONE_IZE(P) ((*P == EOS) ? "<none>" : P)
VOID
dispsrvinfo(getwhat, serverptr, sessionptr)
i4  getwhat;
char *serverptr;
PTR sessionptr;
{
    SERVER_INFO *servinfo;
    SESSION_INFO *sessinfo;
#if defined(axp_osf) || defined(ris_u64) || defined(axp_lnx)
    unsigned long	i_sessid;
#else /* axp_osf */
    SIZE_TYPE	i_sessid;
#endif /* axp_osf */
  u_i4 act_records, ssn_cur, ssn_max, act_cur, act_max, q_cur, q_max;
  i4 sess_stat, pid, counter, srvr_stat;
  char *session_name, *cs_state, *cs_mask, *terminal, *real_user, *
  apparent_user, *dba, *appcode, *cur_fac, *state, *roleid, *groupid, *
  activity, *act_det, *act_logaddr, *curfac_name, *querytext, *sess_desc, *
  service, *database, *thread_id;
  char client_pid[HEXID_SIZE + 1];
  char lsession[HEXID_SIZE + 1];
  char session_id[HEXID_SIZE + 1];
  char q_cur_max[21];
  char server[GCN_VAL_MAX_LEN + 1];
  char lserver[GCN_VAL_MAX_LEN + 1];
  bool reset_flag = TRUE;
    bool	session_gone = TRUE;
    STATUS	status;
    /* Check passed options; could do better error handling, it just returns */
    switch (getwhat)
    {
    case DISP_SERV_LIST:
	d_print("dispsrvinfo: DISP_SERV_LIST\n");
	break;
    case DISP_SERV_INFO:
    case DISP_SESS_LIST:
	if (serverptr == NULL)
	    return;
	d_print("dispsrvinfo: serv=\"%s\"\n",serverptr);
	STcopy(serverptr, lserver);
	STzapblank(lserver, lserver);
	break;
    case DISP_SESS_INFO:
	if ((serverptr == NULL) || (sessionptr == NULL))
	    return;
	d_print("dispsrvinfo: serv=\"%s\",ses=\"%s\"\n",serverptr, sessionptr);
	STcopy(serverptr, lserver);
	STcopy(sessionptr, lsession);
	STzapblank(lserver, lserver);
	STzapblank(lsession, lsession);
	break;
    default:
	return;
    }
    /*
    ** Now, Get the requested info.  We loop thru a list of servers.
    ** In the case of DISP_SERVLIST, they are simply placed on the form
    ** or to a file.  For session lists/info and server info, when the
    ** passed server_name matches a server in the list, then that info
    ** is obtained.
    */
    sess_stat = SESSION_OK;	/* init session status */
    srvr_stat = SERVER_UP;	/* set state for each server */
    counter = 1;
    for (servinfo = (SERVER_INFO *)l_retrieve(SERVERINFO, 0, &reset_flag);
	servinfo != NULL;
	servinfo = (SERVER_INFO *)l_retrieve(SERVERINFO, 0, (bool *)NULL))
    {
	d_print("Detected:   %s   Class: %s  \n",servinfo->server, servinfo->service);
        STcopy(servinfo->server, server);
	STzapblank(server, server);
	switch (getwhat)
	{
	case DISP_SERV_LIST:
	    service = servinfo->service;
	    if (has_clients(service))
	    {
		database = (STequal(wordify(servinfo->database), "*")) ?
		    "ALL" : servinfo->database;
		ssn_cur = servinfo->ssn_cur;
		act_cur = servinfo->act_cur;
	    }
	    else
	    {
		database = "";
		ssn_cur = act_cur = 0;
	    }
	    if (STequal(service, "IINMSVR"))
		service = "NAME";
	    else if (STequal(service, "COMSVR"))
		service = "NET";
	    else if (STequal(service, "BRIDGE"))
		service = "BRIDGE";
/* # line 1524 "dispsrv.qsc" */	/* loadtable */
  {
    if (IItbact(Fdispsrv,(char *)"serverlist",1) != 0) {
      IItcoputio((char *)"server",(short *)0,1,32,0,server);
      IItcoputio((char *)"service",(short *)0,1,32,0,service);
      IItcoputio((char *)"ssn_cur",(short *)0,1,30,4,&ssn_cur);
      IItcoputio((char *)"act_cur",(short *)0,1,30,4,&act_cur);
      IItcoputio((char *)"database",(short *)0,1,32,0,database);
      IItcoputio((char *)"server_stat",(short *)0,1,30,4,&srvr_stat);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1527 "dispsrv.qsc" */	/* host code */
	    break;
	case DISP_SERV_INFO:
	    if (STcompare(server, lserver) == 0) /* this is desired server */
	    {
		ssn_cur = servinfo->ssn_cur;
		ssn_max = servinfo->ssn_max;
		act_cur = servinfo->act_cur;
		act_max = servinfo->act_max;
		service = servtype_sym(servinfo);
		database = (STequal(wordify(servinfo->database), "*")) ?
		    "all databases" : servinfo->database;
#ifdef VMS
		STprintf(q_cur_max, "%d/%d", servinfo->q_cur, servinfo->q_max);
/* # line 1541 "dispsrv.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fservdetail,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"q_cur_max",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1542 "dispsrv.qsc" */	/* host code */
#else
/* # line 1543 "dispsrv.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fservdetail,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"q_cur_max",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1544 "dispsrv.qsc" */	/* host code */
#endif
/* # line 1545 "dispsrv.qsc" */	/* putform */
  {
    if (IIfsetio(Fservdetail) != 0) {
      IIputfldio((char *)"ssn_cur",(short *)0,1,30,4,&ssn_cur);
      IIputfldio((char *)"ssn_max",(short *)0,1,30,4,&ssn_max);
      IIputfldio((char *)"act_cur",(short *)0,1,30,4,&act_cur);
      IIputfldio((char *)"act_max",(short *)0,1,30,4,&act_max);
      IIputfldio((char *)"q_cur_max",(short *)0,1,32,0,q_cur_max);
      IIputfldio((char *)"name",(short *)0,1,32,0,server);
      IIputfldio((char *)"type",(short *)0,1,32,0,service);
      IIputfldio((char *)"connecting",(short *)0,1,32,0,database);
    } /* IIfsetio */
  }
/* # line 1549 "dispsrv.qsc" */	/* host code */
		return;	/* no need to loop anymore - just return */
	    };
	case DISP_SESS_LIST:	/* get a list of sessions from server */
	    if (STcompare(server, lserver) == 0) /* this is desired server */
	    {
		/* walk from start when calling l_retrieve */
		reset_flag = TRUE;
		for (sessinfo = (SESSION_INFO *)l_retrieve(SERVERINFO,
			counter, &reset_flag);
		    sessinfo != NULL;
		    sessinfo = (SESSION_INFO *)l_retrieve(SERVERINFO,
			counter, (bool *)NULL))
		{
		    STcopy(sessinfo->session_id, session_id);
#if defined(axp_osf) || defined(ris_u64) || defined(axp_lnx)
                    CVuahxl8(session_id, &i_sessid);     /* strip leading 0's */
                    sprintf(session_id, "%lx", i_sessid);
#else /* axp_osf */
#if defined(LP64)
		    CVuahxi64(session_id, &i_sessid);	/* strip leading 0's */
		    STprintf(session_id, "%p", i_sessid);
#else
		    CVuahxl(session_id, &i_sessid);	/* strip leading 0's */
		    STprintf(session_id, "%x", i_sessid);
#endif  /* LP64 */
#endif /* axp_osf */
		    session_name = sessinfo->session_name;
		    cs_state = state_sym(sessinfo);
		    database = sessinfo->database;
		    curfac_name = sessinfo->cur_facname;
		    querytext = sessinfo->querytext;
		    terminal = sessinfo->terminal;
		    /* If not a superuser, don't display query field */
		    if (chkingpriv(ING_SUPERUSER) != OK)
		    {
			querytext = "";
		    }
/* # line 1590 "dispsrv.qsc" */	/* loadtable */
  {
    if (IItbact(Fshowsess,(char *)"sessdet",1) != 0) {
      IItcoputio((char *)"session_id",(short *)0,1,32,0,session_id);
      IItcoputio((char *)"session_name",(short *)0,1,32,0,session_name);
      IItcoputio((char *)"tty",(short *)0,1,32,0,terminal);
      IItcoputio((char *)"facility",(short *)0,1,32,0,curfac_name);
      IItcoputio((char *)"cs_state",(short *)0,1,32,0,cs_state);
      IItcoputio((char *)"sess_stat",(short *)0,1,30,4,&sess_stat);
      IItcoputio((char *)"database",(short *)0,1,32,0,database);
      IItcoputio((char *)"query",(short *)0,1,32,0,querytext);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1596 "dispsrv.qsc" */	/* host code */
		}
		return;	/* no need to continue w/outer loop */
	    }
	    break;
	/* session detail */
	case DISP_SESS_INFO:
	    if (STcompare(server, lserver) == 0)/* this is desired server */
	    {
		/* walk from start when calling l_retrieve */
		reset_flag = TRUE;
		for (sessinfo = (SESSION_INFO *)l_retrieve(SERVERINFO,
			counter, &reset_flag);
		    sessinfo != NULL;
		    sessinfo = (SESSION_INFO *)l_retrieve(SERVERINFO,
			counter, (bool *)NULL))
		{
		    STcopy(sessinfo->session_id, session_id);
#if defined(axp_osf) || defined(ris_u64) || defined(axp_lnx)
                    CVuahxl8(session_id, &i_sessid);     /* strip leading 0's */
                    sprintf(session_id, "%lx", i_sessid);
#else /* axp_osf */
#if defined(LP64)
		    CVuahxi64(session_id, &i_sessid);	/* strip leading 0's */
		    STprintf(session_id, "%p", i_sessid);
#else
		    CVuahxl(session_id, &i_sessid);	/* strip leading 0's */
		    STprintf(session_id, "%x", i_sessid);
#endif  /* LP64 */
#endif /* axp_osf */
		    if (STcompare(lsession, session_id) == 0)
		    {				/* this is desired session */
			session_gone = FALSE;
			session_name = sessinfo->session_name;
	    		terminal = NONE_IZE(sessinfo->terminal);
			database = NONE_IZE(sessinfo->database);
			dba = NONE_IZE(sessinfo->dba);
			real_user = NONE_IZE(sessinfo->real_user);
			cs_state = state_unsym(sessinfo);
			cs_mask = sessinfo->cs_mask;
			apparent_user = sessinfo->apparent_user;
			appcode = NONE_IZE(sessinfo->appcode);
			roleid = NONE_IZE(sessinfo->roleid);
			groupid = NONE_IZE(sessinfo->groupid);
			activity = NONE_IZE(sessinfo->activity);
			act_records = sessinfo->act_records;
			act_det = sessinfo->act_det;
			act_logaddr = NONE_IZE(sessinfo->act_logaddr);
			curfac_name = NONE_IZE(fac_sym(sessinfo));
		        querytext = NONE_IZE(sessinfo->querytext);
		        sess_desc = NONE_IZE(sessinfo->sess_desc);
			thread_id = sessinfo->thread_id;
			if ( *thread_id == EOS )
			{
/* # line 1651 "dispsrv.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fsessdetail,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tid",0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1653 "dispsrv.qsc" */	/* host code */
			}
			else
			{
/* # line 1656 "dispsrv.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fsessdetail,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tid",0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1658 "dispsrv.qsc" */	/* host code */
			}
                        /* Determine the PID of the session, the PID may have
                        ** been passed with the information obtained for the
                        ** session, or we must determine the pid from the
                        ** terminal type (if known).
                        */
			if ( pid = ( (sessinfo->client_pid) ?
                                     sessinfo->client_pid :
                                     get_pid_from_dev(sessinfo->terminal)
                                   ))
			{
/* # line 1670 "dispsrv.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fsessdetail,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"client_pid",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1672 "dispsrv.qsc" */	/* host code */
			    STprintf(client_pid, "%x", pid);
		        }
			else
			{				/* nothing to show */
/* # line 1676 "dispsrv.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fsessdetail,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"client_pid",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1678 "dispsrv.qsc" */	/* host code */
                            /* As we don't know the PID, set <unknown>. This is
                            ** required as the ProcInfo menu is only displayed
                            ** if the client_pid [0] is not '<'.
                            */
                            STprintf(client_pid, "<unknown>");
			}
			if (sessinfo->db_exclusive)
			{
/* # line 1686 "dispsrv.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fsessdetail,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"exclusive",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1688 "dispsrv.qsc" */	/* host code */
			}
			else
			{
/* # line 1691 "dispsrv.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fsessdetail,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"exclusive",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1693 "dispsrv.qsc" */	/* host code */
			}
/* # line 1696 "dispsrv.qsc" */	/* putform */
  {
    if (IIfsetio(Fsessdetail) != 0) {
      IIputfldio((char *)"session_id",(short *)0,1,32,0,session_id);
      IIputfldio((char *)"terminal",(short *)0,1,32,0,terminal);
      IIputfldio((char *)"sess_name",(short *)0,1,32,0,session_name);
      IIputfldio((char *)"tid",(short *)0,1,32,0,thread_id);
      IIputfldio((char *)"real_user",(short *)0,1,32,0,real_user);
      IIputfldio((char *)"app_user",(short *)0,1,32,0,apparent_user);
      IIputfldio((char *)"db_owner",(short *)0,1,32,0,dba);
      IIputfldio((char *)"database",(short *)0,1,32,0,database);
      IIputfldio((char *)"appcode",(short *)0,1,32,0,appcode);
      IIputfldio((char *)"act_logaddr",(short *)0,1,32,0,act_logaddr);
      IIputfldio((char *)"act_logrec",(short *)0,1,30,4,&act_records);
      IIputfldio((char *)"activity",(short *)0,1,32,0,activity);
      IIputfldio((char *)"act_det",(short *)0,1,32,0,act_det);
      IIputfldio((char *)"cs_state",(short *)0,1,32,0,cs_state);
      IIputfldio((char *)"cs_mask",(short *)0,1,32,0,cs_mask);
      IIputfldio((char *)"groupid",(short *)0,1,32,0,groupid);
      IIputfldio((char *)"roleid",(short *)0,1,32,0,roleid);
      IIputfldio((char *)"querytext",(short *)0,1,32,0,querytext);
      IIputfldio((char *)"client_pid",(short *)0,1,32,0,client_pid);
      IIputfldio((char *)"sess_desc",(short *)0,1,32,0,sess_desc);
      IIputfldio((char *)"curfac_name",(short *)0,1,32,0,curfac_name);
    } /* IIfsetio */
  }
/* # line 1710 "dispsrv.qsc" */	/* host code */
			/* in case MoreQuery menu item is later selected */
/* # line 1711 "dispsrv.qsc" */	/* putform */
  {
    if (IIfsetio(Fqrytext) != 0) {
      IIputfldio((char *)"querytext",(short *)0,1,32,0,querytext);
    } /* IIfsetio */
  }
/* # line 1712 "dispsrv.qsc" */	/* host code */
			return;	/* we have everything we need */
		    }
		} /* for each session */
	    } /* if (STcompare(server, lserver) == 0) */
	    break; /* case DISP_SESS_INFO */
	} /* switch */
	counter++;
    } /* for each server */
    if (getwhat == DISP_SESS_INFO && session_gone)
    {
	POP_MSG("This session has exited");
    }
    return;
}
/* VARARGS1 */
terminate(status, tmplt, a1, a2, a3, a4, a5, a6, a7)
STATUS status;
char *tmplt;
PTR a1, a2, a3, a4, a5, a6, a7;
{
    clean_up();
    if (status != OK)
    {
	if (tmplt == NULL)
	    SIprintf("IPM: error message not available\n");
	else
	{
	    SIprintf("IPM: ");
	    SIprintf(tmplt, a1, a2, a3, a4, a5, a6, a7);
	    if (tmplt[strlen(tmplt) - 1] != '\n')
		SIprintf("\n");
	}
    }
    PCexit(status);
}
/* VARARGS1 */
d_print(tmplt, a1, a2, a3, a4, a5, a6, a7, a8, a9)
char *tmplt;
PTR a1, a2, a3, a4, a5, a6, a7, a8, a9;
{
    static FILE *fd = (FILE*)NULL;
    LOCATION loc;
    char locbuf[MAX_LOC + 1];
    if (flag_debug_log)
    {
	if (fd == NULL)
	{
	    STcopy(ERx("ipm.log"), locbuf);
	    LOfroms(FILENAME, locbuf, &loc);
	    SIfopen(&loc, "w", SI_TXT, (i4)0, &fd);
	}
	SIfprintf(fd, tmplt, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	SIflush(fd);
    }
}
/*
** s_scan and d_scan -- parse string or number from character buffer
**
**
** Inputs
**	buf		character buffer (ptr to ptr)
**	cnt		count of strings or numbers to get
**			ALL BUT LAST ONE ARE THROWN OUT
**			last one is returned
** Outputs
**	return val	string (null terminated, allocs own storage)
**			or number
1v**	buf		moves buffer ptr past scanned
** History
**	31 Oct 90	jpk	written
*/
char	*
s_scan(buf, cnt, whatfor)
char	**buf;
i4	cnt;
char	*whatfor;
{
	while (--cnt >= 0) 
	{
		if (cnt == 0)
			return s_scan_sub(buf, whatfor);
		else
			(VOID) s_scan_sub(buf, whatfor);
	}
	/* if fall through, do nothing gracefully */
	return "";
}
i4
d_scan(buf, cnt, whatfor)
char	**buf;
i4	cnt;
char	*whatfor;
{
	while (--cnt >= 0) 
	{
		if (cnt == 0)
			return d_scan_sub(buf, whatfor);
		else
			(VOID) d_scan_sub(buf, whatfor);
	}
	/* if fall through, do nothing gracefully */
	return 0;
}
#define	SUBMAXLEN	200
/* s_scan_sub: return a string from a character buffer */
char	*
s_scan_sub(buf, whatfor)
char	**buf;			/* ptr to buff ptr, so we can advance it */
char	*whatfor;		/* what we're looking for, debugging suport */
{
	static char	returnval[SUBMAXLEN];
	char		*strstart = NULL;
	char		*pbuf = *buf;
	i4		session_id = STequal(whatfor, "session id");
	while (*pbuf != EOS)
	{
		if (CMwhite(pbuf)) 	/* string -> whitespace sep. thing */
		{
			if (strstart != NULL)
				break;
		}
		else
		{
			if (strstart == NULL)
				strstart = pbuf;
			/*
			** If parsing session id and thread_id is appended
			** (session_id:thread_id), terminate session id
			** session id string when we hit ":".
			*/
			else if ( session_id && *pbuf == ':' )
			    break;
		}
		++pbuf;
	}
	/* if fall through, EOS before start of string seen, error */
	if (*pbuf == EOS && strstart == NULL)
	{
		terminate(FAIL, "s_scan: premature EOS; wanted %s, got \"%s\"",
			whatfor, *buf);
	}
	/* else we have a string */
	if ((pbuf - strstart) > (SUBMAXLEN - 1))
	{
		terminate(FAIL, "s_scan: substring > %d while looking for %s",
			SUBMAXLEN, whatfor);
	}
	STlcopy(strstart, returnval, (i4) (pbuf - strstart));
	*buf = pbuf;
	return returnval;
}
/* simple state machine: read until digit, continue until non-digit */
i4
d_scan_sub(buf, whatfor)
char	**buf;			/* ptr to buff ptr, so we can advance it */
char	*whatfor;		/* what we're looking for, debugging suport */
{
	int	num = 0;
	char	*pbuf = *buf;
	char	*numstart = NULL;
	STATUS	status;
	char	tmpspace[100];
	while (*pbuf != EOS)
	{
		if (CMdigit(pbuf))
		{
			if (numstart == NULL)
				numstart = pbuf;
		}
		else
		{
			if (numstart != NULL)
				break;
		}
		++pbuf;
	}
	/* if fall through, EOS before digit seen, error */
	if (*pbuf == EOS && numstart == NULL)
	{
		terminate(FAIL, "d_scan: premature EOS; wanted %s, got \"%s\"",
			whatfor, *buf);
	}
	/* else we have a number */
	STlcopy(numstart, tmpspace, (i4) (pbuf - numstart));
	if ((status = CVan(tmpspace, &num)) != OK)
	{
		terminate(FAIL, "d_scan: %s num error, wanted %s, got \"%s\"",
			(status == CV_SYNTAX) ? "syntax" :
			((status == CV_OVERFLOW) ? "overflow" : "unknown"),
			whatfor, *buf);
	}
	/* else it's a good number */
	*buf = pbuf;
	return num;
}
/* wordify: remove enclosing parens, leading and trailing spaces */
char	*
wordify(str)
char	*str;
{
	char	*first = str - 1, *last = str + STlength(str), *p = str;
	/* set first to point to first "real" char in word */
	while (++first < last)
		if (!CMwhite(first) && *first != '(')
			break;
	/* set last to point to last "real" char in word */
	while (--last >= str)
		if (!CMwhite(last) && *last != ')')
			break;
	/* short circuit if no leading spaces or parens */
	if (first == str)
		*++last = EOS;
	/* short circuit: if zippo, just return */
	else if (last <= first)
	{
		*str = EOS;
	}
	/* otherwise need to shift left */
	else
	{
		while (first <= last)
			*p++ = *first++;
		*p = EOS;
	}
	if (STequal(str, "NONE") || STequal(str, "<None>"))
		*str = EOS;
	return str;
}
/* state_sym: transform state into something user can understand */
char	*
state_sym(sessinfo)
SESSION_INFO	*sessinfo;
{
	static char	returnval[100];
	if (STequal(sessinfo->cs_state, "CS_FREE"))
		STcopy("FREE", returnval);
	else if (STequal(sessinfo->cs_state, "CS_COMPUTABLE"))
		STcopy("COM", returnval);
	else if (STequal(sessinfo->cs_state, "CS_EVENT_WAIT"))
		STcopy(sessinfo->waitfor, returnval);
	else if (STequal(sessinfo->cs_state, "CS_MUTEX"))
		STcopy("MUTEX", returnval);
	else if (STequal(sessinfo->cs_state, "CS_STACK_WAIT"))
		STcopy("STACK", returnval);
	else if (STequal(sessinfo->cs_state, "CS_UWAIT"))
		STcopy("UWAIT", returnval);
	else if (STequal(sessinfo->cs_state, "CS_CNDWAIT"))
		STcopy("CNDWT", returnval);
	else	/* not recognized, use truncated state */
		STlcopy(sessinfo->cs_state, returnval, 5);
	return returnval;
}
/* state_unsym: transform parsed state symbol back into iimonitor-like fmt */
char	*
state_unsym(s)
SESSION_INFO *s;
{
    static char	returnval[50];
    STcopy(wordify(s->cs_state), returnval);
    if (STequal(returnval, "CS_EVENT_WAIT"))
    {
	STcat(returnval, " (");
	STcat(returnval, s->waitfor);
	STcat(returnval, ")");
    }
    else if (STequal(returnval, "CS_MUTEX"))
    {
	if (STequal(s->mutex_sx, "x"))
	    STcat(returnval, "(exclusive) ");
	else if (STequal(s->mutex_sx, "s"))
	    STcat(returnval, "(shared) ");
	else
	    STcat(returnval, " ");	/* just to separate from mutex_sess */
	STcat(returnval, s->mutex_sess);
    }
    else if (STequal(returnval, "CS_CNDWAIT"))
    {
	STcat(returnval, " (");
	STcat(returnval, s->cndwait);
	STcat(returnval, ")");
    }
    return returnval;
}
/* 
** Name: state_sym
**
** Description:
**	Transform server type into something user can understand 
**
** History:
**	14-sep-93 (robf)
**	   Add IUSVR, for recovery server
**	18-apr-96 (rajus01)
**	   Added BRIDGE, for Protocol Bridge.
*/
char	*
servtype_sym(servinfo)
SERVER_INFO *servinfo;
{
	static char	returnval[100];
	if (STequal(servinfo->service, "INGRES"))
		STcopy("INGRES (DBMS Server)", returnval);
	else if (STequal(servinfo->service, "STAR"))
		STcopy("STAR (Star Server)", returnval);
	else if (STequal(servinfo->service, "COMSVR"))
		STcopy("COMSVR (Communications Server)", returnval);
	else if (STequal(servinfo->service, "BRIDGE"))
		STcopy("BRIDGE (Protocol Bridge)", returnval);
	else if (STequal(servinfo->service, "IINMSVR"))
		STcopy("IINMSVR (Name Server)", returnval);
	else if (STequal(servinfo->service, "IUSVR"))
		STcopy("IUSVR (Recovery Server)", returnval);
	return returnval;
}
/* 
** Name: fac_sym
**
** Description:
**	transform server facility into something user can understand
**
** History:
**   1 June 1992        jpk     nope, not query evaluation facility,
**                              query execution facility, got it
**
**   14-sep-93 (robf)
**	Correct typo, GWF, not GSF. 
**	Add SXF as new facility
**	Structure comments
*/
char	*
fac_sym(sessinfo)
SESSION_INFO	*sessinfo;
{
	static char	returnval[100];
	if (STlength(sessinfo->cur_facname) == 0)
		*returnval = EOS;
	else if (STequal(sessinfo->cur_facname, "CLF"))
		STcopy("CLF (Compatibility Library Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "ADF"))
		STcopy("ADF (Abstract Datatype Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "DMF"))
		STcopy("DMF (Data Manipulation Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "OPF"))
		STcopy("OPF (Optimizer Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "PSF"))
		STcopy("PSF (Parser Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "QEF"))
		STcopy("QEF (Query Execution Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "QSF"))
		STcopy("QSF (Query Storage Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "RDF"))
		STcopy("RDF (Relation Description Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "SCF"))
		STcopy("SCF (System Control Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "ULF"))
		STcopy("ULF (Utility Library Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "DUF"))
		STcopy("DUF (Database Utility Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "GCF"))
		STcopy("GCF (General Communications Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "RQF"))
		STcopy("RQF (Relation Query Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "TPF"))
		STcopy("TPF (Transaction Processing Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "GWF"))
		STcopy("GWF (Gateway Facility)", returnval);
	else if (STequal(sessinfo->cur_facname, "SXF"))
		STcopy("SXF (Security Extensions Facility)", returnval);
	return returnval;
}
/* return whether service has clients (name and comm servers don't
*/
bool
has_clients(service)
char	*service;
{
	return (STequal(service, "INGRES") || STequal(service, "STAR"));
}
/* remove all occurrences of char from string (name from UNIX tr util) */
tr_dash_d(from, what)
char	*from, *what;
{
	char	*to = from;
	while (*from != EOS)
	{
		if (*from == *what)
			++from;
		else
			*to++ = *from++;
	} 
	*to = EOS;
}
/* Name: srvsess
**
** Description:
**	Get server information from tablefield, look up
** 	current and active sessions for each server.
** 	This just modularizes existing code so refresh doesn't
** 	need replicate code.
**
** History:
**	15-sep-93 (robf)
**	    Make server_name FE_MAXNAME instead of hardcoded 18, this
**	    caused truncation on long names (like in UNIX-domain sockets)
**      02-Dec-94 (liibi01)
**          Fix bug 46411, change return to ENDLOOP to avoid producing
**          error 'cannot nest UNLOADTABLE statement'.
**	25-sep-95 (nick)
**	    Remove assumptions about list ordering.
**	11-Sep-96 (gordy)
**	    GCA Initialization must be paired with Termination.
**/
  srvsess()
  {
	STATUS	status;
    char server_name[GCN_VAL_MAX_LEN + 1];
    char i_service[GCN_TYP_MAX_LEN + 1];
/* # line 2157 "dispsrv.qsc" */	/* unloadtable */
    {
      if (IItbact(Fdispsrv,(char *)"serverlist",0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,GCN_VAL_MAX_LEN + 1-1,server_name,(char *)"\
server");
        IItcogetio((short *)0,1,32,GCN_TYP_MAX_LEN + 1-1,i_service,(char *)"\
service");
        IITBceColEnd();
        {
/* # line 2160 "dispsrv.qsc" */	/* host code */
	    /* don't assume anything about the ordering - just do it */
	    if (has_clients(i_service))
	    {
	    	if ((status = dosrvoper(GC_SERV_INIT, (char *)NULL)) != OK)
	    	{
		    POP_MSG("Error initializing connect to server.");
/* # line 2166 "dispsrv.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 2167 "dispsrv.qsc" */	/* host code */
	    	}
	    	if ((status = dosrvoper(GC_SERV_CONNECT, server_name)) != OK)
	    	{
		    status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL);
		    POP_MSG("Error getting server information.");
/* # line 2173 "dispsrv.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 2174 "dispsrv.qsc" */	/* host code */
	    	}
	    	if ((status = load_servinfo(GET_SERVINFO, server_name)) != OK)
	    	{
		    status = dosrvoper(GC_SERV_DISCONNECT, (char *)NULL); 
		    status = dosrvoper(GC_SERV_TERMINATE, (char *)NULL);
		    POP_MSG("Error getting extended server information");
/* # line 2181 "dispsrv.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 2182 "dispsrv.qsc" */	/* host code */
	    	}
	    	if ((status = dosrvoper(GC_SERV_DISCONNECT, 
		    (char *)NULL)) != OK)
	    	{
		    POP_MSG("Error disconnecting from server");
/* # line 2187 "dispsrv.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 2188 "dispsrv.qsc" */	/* host code */
	    	}
		if ((status = dosrvoper(GC_SERV_TERMINATE, 
		    (char *)NULL)) != OK)
		{
		    POP_MSG("Error shuting down communications");
/* # line 2193 "dispsrv.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 2194 "dispsrv.qsc" */	/* host code */
		}
	    }
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
  }