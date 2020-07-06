# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** procinfo.qc
**
** This file contains routines which obtain from VMS process information
** about a particular pid.
**
** This file defines
**	showproc() - displays the fshowproc form
**	get_proc_info() - gets the information using SYS$GETJPIW
**	end_refresh() - cancels timer request for autorefresh
*/
/* includes and globals */
#include <compat.h>
#include <dbms.h>
#include <st.h>
#include <fe.h>
#include <adf.h>
#include <ft.h>
#include <fmt.h>
#include <frame.h>
#include "dba.h"
#include "msgs.h"
/*
** Forward/External References
*/
VOID showproc();
static STATUS  get_proc_info();	/* the routine which refreshes the screen */
VOID end_refresh();
/*
** Declares
*/
  static char Fshowproc[]= "fshowproc";
#ifdef VMS
#ifdef SEVMSBUILD
#include <psldef>
#else
#include psldef
#endif
/* Definition of a string descriptor, used in various system calls */
typedef struct dsc$descriptor STR_DESC;
#endif
/* Status Codes from SYS$ calls we want to specifially check for */
#define 	NONEXPROCESS	2280
#define		NOSUCHLOGICAL 	532
/*	Mode table to translate process mode into readable codes */
#define STATE_SUSP 9	/* position of SUSP in states table below */
#ifdef VMS
  static char *modes[]= { "OTHER", "NETWORK", "BATCH", "INTERACTIVE"};
static u_i4	modes_size = sizeof(modes) / sizeof(modes[0]);
  static char *states[]= 
{ 
	" ", "COLPG", "MWAIT", "CEF", "PFW", "LEF", 
	"LEFO", "HIB", "HIBO",
	"SUSP", "SUSPO", "FPG", "COM", "COMO", "CUR"
};
static u_i4	states_size = sizeof(states) / sizeof(states[0]);
static i4	timer_number;
#endif /* VMS */
/*
**	Statics for display autorefresh
*/
#define	DEFAULT_SEC_L "0 00:00:04.00"
#define	DEFAULT_SEC_S "0 00:00:01.00"
static bool	suppress_display;	/* used to avoid drawing the form during initialize block */
static i4	procpid;	/*
				** this is the pid to get info on.  It
				** is declared as a static so that it does
				** not have to be passed into the AST
				*/
#ifdef VMS
/*
** Define struct for getjpi to place information in
*/
  static struct _procinfo {
    int pid;
    int owner;
    char terminal[8];
    char prcnam[16];
    char account[9];
    char username[13];
    int grp;
    int mem;
    int astcnt;
    int astlm;
    int pri;
    int biolm;
    int biocnt;
    int bufio;
    int bytcnt;
    int bytlm;
    int cputim;
    int dfwscnt;
    int diocnt;
    int diolm;
    int dirio;
    int enqcnt;
    int enqlm;
    int filcnt;
    int fillm;
    char imagname[40];
    int mode;
    int pgflquota;
    int pageflts;
    int pagfilcnt;
    int prib;
    int gpgcnt;
    int ppgcnt;
    int state;
    int wsauth;
    int prclm;
    int wsauthext;
    int tqcnt;
    int wsextent;
    int tqlm;
    int wspeak;
    int wsquota;
    int wssize;
  } procinfo;
/* jpilst
** is used to store information about process name as requested
** in call to SYS$GETJPI
*/
static struct {
    short 	buffer_len;
    short 	item_code;
    i4  	*buffer_addr;
    i4		*ret_len_addr;
} jpilst[] = {
    {sizeof(procinfo.pid), JPI$_PID, &procinfo.pid, 0},
    {sizeof(procinfo.owner), JPI$_OWNER, &procinfo.owner, 0},
    {sizeof(procinfo.terminal), JPI$_TERMINAL, &procinfo.terminal, 0},
    {sizeof(procinfo.prcnam), JPI$_PRCNAM, &procinfo.prcnam, 0},
    {sizeof(procinfo.account), JPI$_ACCOUNT, &procinfo.account, 0},
    {sizeof(procinfo.username), JPI$_USERNAME, &procinfo.username, 0},
    {sizeof(procinfo.grp), JPI$_GRP, &procinfo.grp, 0},
    {sizeof(procinfo.mem), JPI$_MEM, &procinfo.mem, 0},
    {sizeof(procinfo.astcnt), JPI$_ASTCNT, &procinfo.astcnt, 0},
    {sizeof(procinfo.astlm), JPI$_ASTLM, &procinfo.astlm, 0},
    {sizeof(procinfo.pri), JPI$_PRI, &procinfo.pri, 0},
    {sizeof(procinfo.biolm), JPI$_BIOLM, &procinfo.biolm, 0},
    {sizeof(procinfo.biocnt), JPI$_BIOCNT, &procinfo.biocnt, 0},
    {sizeof(procinfo.bufio), JPI$_BUFIO, &procinfo.bufio, 0},
    {sizeof(procinfo.bytcnt), JPI$_BYTCNT, &procinfo.bytcnt, 0},
    {sizeof(procinfo.bytlm), JPI$_BYTLM, &procinfo.bytlm, 0},
    {sizeof(procinfo.cputim), JPI$_CPUTIM, &procinfo.cputim, 0},
    {sizeof(procinfo.dfwscnt), JPI$_DFWSCNT, &procinfo.dfwscnt, 0},
    {sizeof(procinfo.diocnt), JPI$_DIOCNT, &procinfo.diocnt, 0},
    {sizeof(procinfo.diolm), JPI$_DIOLM, &procinfo.diolm, 0},
    {sizeof(procinfo.dirio), JPI$_DIRIO, &procinfo.dirio, 0},
    {sizeof(procinfo.enqcnt), JPI$_ENQCNT, &procinfo.enqcnt, 0},
    {sizeof(procinfo.enqlm), JPI$_ENQLM, &procinfo.enqlm, 0},
    {sizeof(procinfo.filcnt), JPI$_FILCNT, &procinfo.filcnt, 0},
    {sizeof(procinfo.fillm), JPI$_FILLM, &procinfo.fillm, 0},
    {sizeof(procinfo.gpgcnt), JPI$_GPGCNT, &procinfo.gpgcnt, 0},
    {sizeof(procinfo.imagname), JPI$_IMAGNAME, &procinfo.imagname, 0},
    {sizeof(procinfo.mode), JPI$_MODE, &procinfo.mode, 0},
    {sizeof(procinfo.pgflquota), JPI$_PGFLQUOTA, &procinfo.pgflquota, 0},
    {sizeof(procinfo.pageflts), JPI$_PAGEFLTS, &procinfo.pageflts, 0},
    {sizeof(procinfo.pagfilcnt), JPI$_PAGFILCNT, &procinfo.pagfilcnt, 0},
    {sizeof(procinfo.prib), JPI$_PRIB, &procinfo.prib, 0},
    {sizeof(procinfo.ppgcnt), JPI$_PPGCNT, &procinfo.ppgcnt, 0},
    {sizeof(procinfo.state), JPI$_STATE, &procinfo.state, 0},
    {sizeof(procinfo.wsauth), JPI$_WSAUTH, &procinfo.wsauth, 0},
    {sizeof(procinfo.prclm), JPI$_PRCLM, &procinfo.prclm, 0},
    {sizeof(procinfo.wsauthext), JPI$_WSAUTHEXT, &procinfo.wsauthext, 0},
    {sizeof(procinfo.tqcnt), JPI$_TQCNT, &procinfo.tqcnt, 0},
    {sizeof(procinfo.wsextent), JPI$_WSEXTENT, &procinfo.wsextent, 0},
    {sizeof(procinfo.tqlm), JPI$_TQLM, &procinfo.tqlm, 0},
    {sizeof(procinfo.wspeak), JPI$_WSPEAK, &procinfo.wspeak, 0},
    {sizeof(procinfo.wsquota), JPI$_WSQUOTA, &procinfo.wsquota, 0},
    {sizeof(procinfo.wssize), JPI$_WSSIZE, &procinfo.wssize, 0},
    {0, 0}
};
#endif
/*
**	showproc(cpid)
**
**	This routine displays the fshowproc form
**
**	Inputs: pid - a hex representation of the PID
**
**	Outputs: displays the fshowproc form
**
**	History:
**	11/25/88	tomt	Added logic to avoid excessive screen redraw.
**	11/25/88	tomt	cleaned up logic
**       1/25/89        tomt    converted to release 6
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**	31-aug-93 (ricka)
**		added st.h
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	 1-nov-95 (nick)
**		Remove flag_file.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
VOID
showproc(cpid)
char 	*cpid;
{
    bool	e_xit = FALSE;
    static bool	forms_added = FALSE;
  GLOBALREF int *fshowproc;
    /*
    **  Determine if forms are added, if not addform them.  Also
    **  don't add forms if output is being directed to a file
    */
    if (!forms_added)
    {
/* # line 236 "procinfo.qsc" */	/* addform */
  {
    IIaddform(fshowproc);
  }
/* # line 237 "procinfo.qsc" */	/* host code */
	forms_added = TRUE;
    }
    /*
    **  Display the form and get the information
    */
/* # line 243 "procinfo.qsc" */	/* display */
  {
    if (IIdispfrm(Fshowproc,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 247 "procinfo.qsc" */	/* host code */
	CVahxl(cpid, &procpid);
	/*    Get the process information */
	suppress_display = TRUE;/* suppress ##redisplay in get_proc_work
				** because form will be displayed at end
				** of initialize block anyway.
				*/
	if (get_proc_info() == FAIL) 
	{
	    POP_MSG("Process doesn't exist or privilege violation");
/* # line 257 "procinfo.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE1;
      }
/* # line 258 "procinfo.qsc" */	/* host code */
	}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 262 "procinfo.qsc" */	/* host code */
	end_refresh();	/* turn off autorefresh */
/* # line 263 "procinfo.qsc" */	/* printscreen */
          {
            IIprnscr((char *)0);
          }
/* # line 264 "procinfo.qsc" */	/* host code */
	if (get_proc_info() == FAIL) 
	{
	    POP_MSG("That process no longer exists");
/* # line 267 "procinfo.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 268 "procinfo.qsc" */	/* host code */
	}
        }
      } else if (IIretval() == 2) {
        {
/* # line 272 "procinfo.qsc" */	/* host code */
	end_refresh();	/* turn off autorefresh */
	my_help("ipmshopr.hlp", "Process Information");
	if (get_proc_info() == FAIL) 
	{
	    POP_MSG("That process no longer exists");
/* # line 277 "procinfo.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 278 "procinfo.qsc" */	/* host code */
	}
        }
      } else if (IIretval() == 3) {
        {
/* # line 282 "procinfo.qsc" */	/* host code */
	end_refresh();	/* turn off autorefresh */
/* # line 283 "procinfo.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 287 "procinfo.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
	end_refresh();		/* turn off autorefresh */
/* # line 289 "procinfo.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 290 "procinfo.qsc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Print_Screen",(char *)0,2,2,1) == 0) goto IIfdE1;
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
/* # line 293 "procinfo.qsc" */	/* host code */
    if (e_xit) {		/* user wants to leave */
	terminate(OK);
    }
    return;
}
/*	get_proc_info()
**
**	Uses call to SYS$GETJPI to get process information for form display.
**	If process does not exist this routine will return FAIL.
**
**	Input: none - the pid is stored in a file wide static
**
**	Output: Puts values onto form
**
**	Return: OK - if success
**		FAIL - if process doesn't exist.
**
**	History
**	11/26/88	tmt	had to account for jpi returning status showing
**				that process is suspended.
**       1/25/89        tmt     converted for use with R6 INGRES
*/
static STATUS
get_proc_info()
{
#ifdef VMS
    STATUS	status;
    i4		ret_len;
  char cpid[10], cowner[10];
  char cgrp[5], cmem[5];
  char ccputime[13], yorn[20], crealtime[20];
    struct _ctime 
    {
	i4	ctime1;
	i4	ctime2;
    } convtime;					/* quadword for VAX time */
    STR_DESC	time_buff, fao_ctl;
    char	timestring[20], faostr[5];
    i4		hours, minutes, seconds, hundreths;
    static i4	iosb;
    /* Get jpi information of user */
    status = sys$getjpiw(0, &procpid, 0, &jpilst, iosb, 0, 0);
    if (status == SS$_SUSPENDED) 
    {	
	/* not an error for a proc to be suspended */
	/* procinfo.state = STATE_SUSP;	/* set process state to suspended */
    } 
    else if (status != SS$_NORMAL) 
    {
	if (status == NONEXPROCESS) 
	{
	    if (!suppress_display) 
	    { 
		/* when NOT called from init block, prnt message */
/* # line 349 "procinfo.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowproc) != 0) {
      IIputfldio((char *)"jpi_pid",(short *)0,1,32,0,(char *)"<no pid>");
      IIputfldio((char *)"jpi_prcnam",(short *)0,1,32,0,(char *)
"<non-ex-proc>");
    } /* IIfsetio */
  }
/* # line 351 "procinfo.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 352 "procinfo.qsc" */	/* host code */
	    }
	    return(FAIL);
	} 
	else if (status == SS$_NOPRIV) 
	{
	    POP_MSG("privilege error: process info requires WORLD privilege");
	    return FAIL;
	}
	else
	{
	    terminate(status, "get_proc_info: sys$getjpi returned %d", status);
	}
    }	
    /* Get the system time */
    STcopy("!%T", faostr);
    time_buff.dsc$w_length = 12;
    time_buff.dsc$a_pointer = crealtime;
    fao_ctl.dsc$w_length = STlength(faostr);
    fao_ctl.dsc$a_pointer = faostr;
    convtime.ctime1 = 0;
    convtime.ctime2 = 0;
    status = sys$fao(&fao_ctl, 0, &time_buff, convtime);
    crealtime[11] = EOS;	/* chop off garbage */
    /* Process information and place on form */
    STprintf(cgrp, "%o", procinfo.grp);
    STprintf(cmem, "%o", procinfo.mem);
    /*  Translate pid appropriately */
    STprintf(cpid, "%x", procinfo.pid);
    STprintf(cowner, "%x", procinfo.owner);
    /*  convert time - time is in 10ms ticks */
    hours = procinfo.cputim/360000;
    minutes = ((procinfo.cputim/6000) - (hours * 60));
    seconds = ((procinfo.cputim/100) - (minutes * 60));
    hundreths = (procinfo.cputim - ((procinfo.cputim/100)*100));
    STprintf(ccputime, "%d:%02d:%02d.%02d", hours, minutes, seconds, hundreths);
/* # line 393 "procinfo.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowproc) != 0) {
      IIputfldio((char *)"jpi_pid",(short *)0,1,32,0,cpid);
      IIputfldio((char *)"jpi_owner",(short *)0,1,32,0,cowner);
      IIputfldio((char *)"jpi_prcnam",(short *)0,1,32,0,procinfo.prcnam);
      IIputfldio((char *)"jpi_account",(short *)0,1,32,0,procinfo.account);
      IIputfldio((char *)"jpi_terminal",(short *)0,1,32,0,procinfo.terminal);
      IIputfldio((char *)"jpi_username",(short *)0,1,32,0,procinfo.username);
      IIputfldio((char *)"jpi_grp",(short *)0,1,32,0,cgrp);
      IIputfldio((char *)"jpi_mem",(short *)0,1,32,0,cmem);
      IIputfldio((char *)"jpi_astcnt",(short *)0,1,30,4,&procinfo.astcnt);
      IIputfldio((char *)"jpi_astlm",(short *)0,1,30,4,&procinfo.astlm);
      IIputfldio((char *)"jpi_pri",(short *)0,1,30,4,&procinfo.pri);
      IIputfldio((char *)"jpi_biolm",(short *)0,1,30,4,&procinfo.biolm);
      IIputfldio((char *)"jpi_biocnt",(short *)0,1,30,4,&procinfo.biocnt);
      IIputfldio((char *)"jpi_bufio",(short *)0,1,30,4,&procinfo.bufio);
      IIputfldio((char *)"jpi_bytcnt",(short *)0,1,30,4,&procinfo.bytcnt);
      IIputfldio((char *)"jpi_bytlm",(short *)0,1,30,4,&procinfo.bytlm);
      IIputfldio((char *)"curtime",(short *)0,1,32,0,crealtime);
      IIputfldio((char *)"jpi_cputim",(short *)0,1,32,0,ccputime);
      IIputfldio((char *)"jpi_dfwscnt",(short *)0,1,30,4,&procinfo.dfwscnt);
      IIputfldio((char *)"jpi_diocnt",(short *)0,1,30,4,&procinfo.diocnt);
      IIputfldio((char *)"jpi_diolm",(short *)0,1,30,4,&procinfo.diolm);
      IIputfldio((char *)"jpi_dirio",(short *)0,1,30,4,&procinfo.dirio);
      IIputfldio((char *)"jpi_enqcnt",(short *)0,1,30,4,&procinfo.enqcnt);
      IIputfldio((char *)"jpi_enqlm",(short *)0,1,30,4,&procinfo.enqlm);
      IIputfldio((char *)"jpi_filcnt",(short *)0,1,30,4,&procinfo.filcnt);
      IIputfldio((char *)"jpi_fillm",(short *)0,1,30,4,&procinfo.fillm);
      IIputfldio((char *)"jpi_imagname",(short *)0,1,32,0,procinfo.imagname);
      IIputfldio((char *)"jpi_mode",(short *)0,1,32,0,modes[procinfo.mode]);
      IIputfldio((char *)"jpi_pgflquot",(short *)0,1,30,4,&procinfo.pgflquota);
      IIputfldio((char *)"jpi_pageflts",(short *)0,1,30,4,&procinfo.pageflts);
      IIputfldio((char *)"jpi_pagfilcn",(short *)0,1,30,4,&procinfo.pagfilcnt);
      IIputfldio((char *)"jpi_prib",(short *)0,1,30,4,&procinfo.prib);
      IIputfldio((char *)"Jpi_ppgcnt",(short *)0,1,30,4,&procinfo.ppgcnt);
      IIputfldio((char *)"jpi_state",(short *)0,1,32,0,states[procinfo.state]);
      IIputfldio((char *)"jpi_wsauth",(short *)0,1,30,4,&procinfo.wsauth);
      IIputfldio((char *)"jpi_prclm",(short *)0,1,30,4,&procinfo.prclm);
      IIputfldio((char *)"jpi_wsauthex",(short *)0,1,30,4,&procinfo.wsauthext);
      IIputfldio((char *)"jpi_tqcnt",(short *)0,1,30,4,&procinfo.tqcnt);
      IIputfldio((char *)"jpi_wsextent",(short *)0,1,30,4,&procinfo.wsextent);
      IIputfldio((char *)"jpi_tqlm",(short *)0,1,30,4,&procinfo.tqlm);
      IIputfldio((char *)"jpi_wspeak",(short *)0,1,30,4,&procinfo.wspeak);
      IIputfldio((char *)"jpi_wsquota",(short *)0,1,30,4,&procinfo.wsquota);
      IIputfldio((char *)"jpi_wssize",(short *)0,1,30,4,&procinfo.wssize);
      IIputfldio((char *)"jpi_gpgcnt",(short *)0,1,30,4,&procinfo.gpgcnt);
    } /* IIfsetio */
  }
/* # line 416 "procinfo.qsc" */	/* host code */
    if (suppress_display) 
    {	
	/* first time called: don't force display */
	suppress_display = FALSE;	
	/* on second and subsequent times, force redisplay */
        STcopy(DEFAULT_SEC_L, timestring);  /* on second time called, wait longer */
    } 
    else 
    {	
	/* second and subsequent times: force display of data */
/* # line 427 "procinfo.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 428 "procinfo.qsc" */	/* host code */
        STcopy(DEFAULT_SEC_S, timestring); /* >= 3 call, refresh at a smaller delta */
    }
    /*
    **  Reschedule this routine for execution.
    **  Get the time and convert it to internal format.
    **  Then submit a timer request to rerun this routine.
    */
    time_buff.dsc$w_length = STlength(timestring);
    time_buff.dsc$a_pointer = timestring;
    status = sys$bintim(&time_buff, &convtime);
    if (status != SS$_NORMAL) 
    {
	terminate(status, "get_proc_info: sys$bintim returned %d", status);
    }
    status = sys$setimr(0, &convtime, &get_proc_info, &timer_number);
    if (status != SS$_NORMAL) 
    {
	terminate(status, "get_proc_info: sys$setimr returned %d", status);
    }
#endif /* VMS */
    return(OK);
}
/*
**  end_refresh - this routine will cancel the timer request which
** 	refreshes the jpi information.
**
**	11/23/88 tmt - written.
*/
VOID
end_refresh()
{
#ifdef VMS
    STATUS status;
    status = sys$cantim(timer_number, PSL$C_USER);
    if (status != SS$_NORMAL) 
    {
	terminate(status, "end_refresh: sys$cantim returned %d", status);
    }
    /*
    **  Reset suppress_display flag - user is leaving process_info screen
    */
    suppress_display = TRUE;
#endif /* VMS */
    return;
}
