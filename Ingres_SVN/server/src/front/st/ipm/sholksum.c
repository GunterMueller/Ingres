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
#include <st.h>
#include <cs.h>
#include <dbms.h>
#include <fe.h>
#include <pc.h>
#include <lk.h>
#include <lg.h>
#include <er.h>
#include        <adf.h>
#include        <ft.h>
#include        <fmt.h>
#include        <frame.h>
#include "lists.h"
#include "dba.h"
/*
** Forward and external references
*/
VOID get_lklgstats();
VOID printlkstats();
/*
** file wide statics
**
** interval_display - set according to whether stats are displayed since
**	system start or since a user specified time
** since_time - if there is a user specified time that marks the start
**	of the stat collection interval, that time is stored in this var
** startstat - contains the values at interval start.  showlksumm fills
**	this and printlkstat displays the right value
*/
static bool interval_display = FALSE; /* default is since sys startup */
  static char since_time[26];
static LK_STAT	startstat;
/*
**
**  File: showlksumm.qc
**
**  Purpose - this file contains the routines which display locking
**	system summary form.  
**
**  This file contains:
**	showlksumm - displays the locking system information form. 
**	printlkstat - gets the info and puts it on screen or in a file.
**
**  History
**	2/22/89		tomt	created
**      2/24/89		tomt	added vaxcluster support
**	8/21/89		tomt	use INGDBA versions of LK structs
**	8/27/89		tomt	added 6.2 lock summary fields for VAXclusters
**	9/6/89		tomt	added autorefresh capabilities
**	9/24/89		tomt	pulled display code into seperate routine
**	9/24/89		tomt	added interval setting for stats collection
**	6/21/90		tomt	added locking summary info (# of locks used etc)
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	11/1/95		nick	removed flag_file
**      21-apr-1999 (hanch04)
**          Added st.h
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
VOID
showlksumm()
{
    bool	e_xit = FALSE;
  int timeout;
  char form_name[FE_MAXNAME + 1];
  GLOBALREF int *fshowlksumm;
  GLOBALREF int *fshowlksummno;
    static bool	forms_added = FALSE;
    /*
    **  Determine if forms are added, if not addform them.
    */
    if (!forms_added)
    {
	if (flag_cluster)
	{
/* # line 99 "sholksum.qsc" */	/* addform */
  {
    IIaddform(fshowlksumm);
  }
/* # line 100 "sholksum.qsc" */	/* host code */
        }
	else
	{
/* # line 103 "sholksum.qsc" */	/* addform */
  {
    IIaddform(fshowlksummno);
  }
/* # line 104 "sholksum.qsc" */	/* host code */
	}
	forms_added = TRUE;
    }
    /* set FRS timeout */
    timeout = frs_timeout;		/* place into local frs var */
/* # line 110 "sholksum.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
    } /* IIiqset */
  }
/* # line 112 "sholksum.qsc" */	/* host code */
    /* Display the form and get the information */
    if (flag_cluster) {
	STcopy("fshowlksumm", form_name);
    } else {
	STcopy("fshowlksummno", form_name);
    }
    /*
    ** Form used to be a popup
    */
/*
** DISPLAY form_name READ WITH STYLE = POPUP (BORDER = NONE,
**	STARTCOLUMN = 3, STARTROW = 3)
*/
/* # line 126 "sholksum.qsc" */	/* display */
  {
    if (IIdispfrm(form_name,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 129 "sholksum.qsc" */	/* host code */
	/* Put information on the form */
	printlkstats(form_name);
        if (interval_display) /* need to place starting time on form */
        {
/* # line 134 "sholksum.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"start_time",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"current_time",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 136 "sholksum.qsc" */	/* putform */
      {
        if (IIfsetio(form_name) != 0) {
          IIputfldio((char *)"start_time",(short *)0,1,32,0,since_time);
          IIputfldio((char *)"current_time",(short *)0,1,32,0,(char *)"\
now");
        } /* IIfsetio */
      }
/* # line 137 "sholksum.qsc" */	/* host code */
        }
	refresh_status(VISIBLE, form_name);
/* # line 139 "sholksum.qsc" */	/* redisplay */
      {
        IIredisp();
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 143 "sholksum.qsc" */	/* host code */
	printlkstats(form_name);
        }
      } else if (IIretval() == 2) {
        {
/* # line 147 "sholksum.qsc" */	/* host code */
	printlkstats(form_name);
        }
      } else if (IIretval() == 3) {
        {
/* # line 151 "sholksum.qsc" */	/* host code */
	/*
	**  Allow user to select the interval that is used to determine what
	**  stats are displayed.  Selecting Since_Startup will display stats
	**  since the INGRES System was started.  Selecting Begin_Now will
	**  (if not already) start displaying statistics accumulated since
	**  Begin_Now is selected.  Also an invisible field will appear
	**  containing the time that collection started. Same for an invisible
	**  field showing current time.
	*/
/* # line 160 "sholksum.qsc" */	/* display */
          {
            if (IInestmu() == 0) goto IIfdE2;
            goto IImuI2;
IIfdI2:;
            if (IIfsetio((char *)0) == 0) goto IIfdE2;
            {
/* # line 163 "sholksum.qsc" */	/* set_frs */
              {
                if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                  IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                } /* IIiqset */
              }
            }
IIfdB2:
            while (IIrunnest() != 0) {
              if (IIretval() == 1) {
                {
/* # line 167 "sholksum.qsc" */	/* set_frs */
                  {
                    if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                      IIstfsio(54,(char *)"start_time",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"current_time",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                    } /* IIiqset */
                  }
/* # line 169 "sholksum.qsc" */	/* host code */
	    interval_display = FALSE;	/* display since sys startup */
/* # line 170 "sholksum.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE2;
                  }
                }
              } else if (IIretval() == 2) {
                {
/* # line 174 "sholksum.qsc" */	/* set_frs */
                  {
                    if (IIiqset(2,0,form_name,(char *)0, (char *)0) != 0) {
                      IIstfsio(54,(char *)"start_time",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"current_time",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                    } /* IIiqset */
                  }
/* # line 176 "sholksum.qsc" */	/* putform */
                  {
                    if (IIfsetio(form_name) != 0) {
                      IIputfldio((char *)"start_time",(short *)0,1,32,0,
                      (char *)"now");
                      IIputfldio((char *)"current_time",(short *)0,1,32,0,
                      (char *)"now");
                    } /* IIfsetio */
                  }
/* # line 177 "sholksum.qsc" */	/* getform */
                  {
                    if (IIfsetio(form_name) != 0) {
                      IIgetfldio((short *)0,1,32,25,since_time,(char *)
"start_time");
                    } /* IIfsetio */
                  }
/* # line 178 "sholksum.qsc" */	/* host code */
	    interval_display = TRUE;	/* display stats since now */
	    /* get reference stats */
	    get_lklgstats(GET_LOCKSTAT, (PTR) &startstat);
/* # line 182 "sholksum.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE2;
                  }
                }
              } else if (IIretval() == 3) {
                {
/* # line 186 "sholksum.qsc" */	/* breakdisplay */
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
            if (IIfsetio((char *)0) == 0) goto IIfdE2;
            goto IIfdE2;
IImuI2:
            if (IIinitmu() == 0) goto IIfdE2;
            if (IInmuact((char *)"Since_Startup",(char *)0,2,2,1) == 0) goto 
            IIfdE2;
            if (IInmuact((char *)"Begin_Now",(char *)0,2,2,2) == 0) goto IIfdE2;
            if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE2;
            if (IIendmu() == 0) goto IIfdE2;
            goto IIfdI2;
IIfdE2:;
            IIendnest();
          }
          {
/* # line 190 "sholksum.qsc" */	/* host code */
	    printlkstats(form_name);
/* # line 191 "sholksum.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
              } /* IIiqset */
            }
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 196 "sholksum.qsc" */	/* host code */
	/*
	**  set FRS timeout
	*/
/* # line 199 "sholksum.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 200 "sholksum.qsc" */	/* host code */
	my_help("ipmshlks.hlp", "Locking System Summary");
	timeout = frs_timeout;		/* place into local frs var */
/* # line 202 "sholksum.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 206 "sholksum.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 210 "sholksum.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
/* # line 211 "sholksum.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 212 "sholksum.qsc" */	/* breakdisplay */
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
    if (IIFRtoact(0,1) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Interval",(char *)0,2,2,3) == 0) goto IIfdE1;
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
/* # line 215 "sholksum.qsc" */	/* host code */
    if (e_xit) 
    {				/* user wants to leave */
	terminate(OK);
    }
/* # line 219 "sholksum.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 220 "sholksum.qsc" */	/* host code */
    return;
}
/*
** printlkstats() - displays the locking stat to a form or to a file
**
** Inputs:
**	none
**
** Outputs:
**	locking stats to a file or to a form
**
** Returns:
**	none
**
** History:
**	9/24/89		tomt	written
**	6/21/90		tomt	added locking summary info
**
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	18-Jun-2004 (schka24)
**	    Revert calculation for locks-in-use (was changed by someone
**	    unknown, to subtract off lkb's in rsb's, but lkshow already
**	    does that subtraction.)
*/
VOID
printlkstats(formname)
  char *formname;
{
    LK_STAT	*dispstat;
    LK_STAT	curstat;
    LK_STAT	diffstat;
    LK_SUMM	lksumm;
  i4 lkbs_remain, llbs_remain;
  i4 ccreate;
  i4 request;
  i4 re_request;
  i4 convert;
  i4 release;
  i4 escalate;
  i4 ddestroy;
  i4 wait;
  i4 con_wait;
  i4 con_dead;
  i4 dead_search;
  i4 deadlock;
  i4 cancel;
  i4 lkb_size;
  i4 lkbs_per_xact;
  i4 lkb_hash_size;
  i4 lkbs_inuse;
  i4 rsb_hash_size;
  i4 rsbs_inuse;
  i4 llbs_inuse;
  i4 llb_size;
    get_lklgstats(GET_LOCKSTAT, (PTR) &curstat);	/* get current stats */
    get_lklgstats(GET_LOCKSUMM, (PTR) &lksumm);	/* get summ stats */
    if (interval_display)	/* calculate the difference */
    {
	dispstat = &diffstat;	/* display the difference */
	diffstat.create = curstat.create - startstat.create;
	diffstat.request = curstat.request - startstat.request;
	diffstat.re_request = curstat.re_request - startstat.re_request;
	diffstat.convert = curstat.convert - startstat.convert;
	diffstat.release = curstat.release - startstat.release;
	diffstat.escalate = curstat.escalate - startstat.escalate;
	diffstat.destroy = curstat.destroy - startstat.destroy;
	diffstat.wait = curstat.wait - startstat.wait;
	diffstat.con_wait = curstat.con_wait - startstat.con_wait;
	diffstat.con_dead = curstat.con_dead - startstat.con_dead;
	diffstat.dead_search = curstat.dead_search - startstat.dead_search;
	diffstat.deadlock = curstat.deadlock - startstat.deadlock;
	diffstat.cancel = curstat.cancel - startstat.cancel;
	if (flag_cluster) 
	{
	    diffstat.enq = curstat.enq - startstat.enq;
	    diffstat.deq = curstat.deq - startstat.deq;
	    diffstat.gdlck_search = curstat.gdlck_search - 
		startstat.gdlck_search;
	    diffstat.gdeadlock = curstat.gdeadlock - startstat.gdeadlock;
	    diffstat.gdlck_grant = curstat.gdlck_grant - startstat.gdlck_grant;
	    diffstat.totl_gdlck_search = curstat.totl_gdlck_search -
		startstat.totl_gdlck_search;
	    diffstat.gdlck_call = curstat.gdlck_call - startstat.gdlck_call;
	    diffstat.gdlck_sent = curstat.gdlck_sent - startstat.gdlck_sent;
	    diffstat.cnt_gdlck_call = curstat.cnt_gdlck_call -
		startstat.cnt_gdlck_call;
	    diffstat.cnt_gdlck_sent = curstat.cnt_gdlck_sent -
		startstat.cnt_gdlck_sent;
	    diffstat.unsent_gdlck_search = curstat.unsent_gdlck_search -
		startstat.unsent_gdlck_search;
	    diffstat.sent_all = curstat.sent_all - startstat.sent_all;
	}
    }
    else	/* display the current stats */
    {
	dispstat = &curstat;	/* display current stats */
    }
    /* calculate locks and locklists remaining */
    /* Note: LK_SHOW has already included locks embedded in rsb's */
    lkbs_remain = lksumm.lkb_size - lksumm.lkbs_inuse;
    llbs_remain = lksumm.llb_size - lksumm.llbs_inuse;
    ccreate = dispstat->create;
    request = dispstat->request;
    re_request = dispstat->re_request;
    convert = dispstat->convert;
    release = dispstat->release;
    escalate = dispstat->escalate;
    ddestroy = dispstat->destroy;
    wait = dispstat->wait;
    con_wait = dispstat->con_wait;
    con_dead = dispstat->con_dead;
    dead_search = dispstat->dead_search;
    deadlock = dispstat->deadlock;
    cancel = dispstat->cancel;
    lkb_size = lksumm.lkb_size;
    lkbs_per_xact = lksumm.lkbs_per_xact;
    lkb_hash_size = lksumm.lkb_hash_size;
    lkbs_inuse = lksumm.lkbs_inuse;
    rsb_hash_size = lksumm.rsb_hash_size;
    rsbs_inuse = lksumm.rsbs_inuse;
    llbs_inuse = lksumm.llbs_inuse;
    llb_size = lksumm.llb_size;
/* # line 356 "sholksum.qsc" */	/* putform */
  {
    if (IIfsetio(formname) != 0) {
      IIputfldio((char *)"createlst",(short *)0,1,30,4,&ccreate);
      IIputfldio((char *)"current_time",(short *)0,1,32,0,(char *)"now");
      IIputfldio((char *)"request",(short *)0,1,30,4,&request);
      IIputfldio((char *)"re_request",(short *)0,1,30,4,&re_request);
      IIputfldio((char *)"convert",(short *)0,1,30,4,&convert);
      IIputfldio((char *)"release",(short *)0,1,30,4,&release);
      IIputfldio((char *)"escalate",(short *)0,1,30,4,&escalate);
      IIputfldio((char *)"destroy",(short *)0,1,30,4,&ddestroy);
      IIputfldio((char *)"wait",(short *)0,1,30,4,&wait);
      IIputfldio((char *)"con_wait",(short *)0,1,30,4,&con_wait);
      IIputfldio((char *)"con_dead",(short *)0,1,30,4,&con_dead);
      IIputfldio((char *)"dead_search",(short *)0,1,30,4,&dead_search);
      IIputfldio((char *)"deadlock",(short *)0,1,30,4,&deadlock);
      IIputfldio((char *)"cancel",(short *)0,1,30,4,&cancel);
      IIputfldio((char *)"lkb_size",(short *)0,1,30,4,&lkb_size);
      IIputfldio((char *)"lkbs_per_xact",(short *)0,1,30,4,&lkbs_per_xact);
      IIputfldio((char *)"lkb_hash_size",(short *)0,1,30,4,&lkb_hash_size);
      IIputfldio((char *)"lkbs_inuse",(short *)0,1,30,4,&lkbs_inuse);
      IIputfldio((char *)"rsb_hash_size",(short *)0,1,30,4,&rsb_hash_size);
      IIputfldio((char *)"rsbs_inuse",(short *)0,1,30,4,&rsbs_inuse);
      IIputfldio((char *)"llbs_inuse",(short *)0,1,30,4,&llbs_inuse);
      IIputfldio((char *)"llbs_remain",(short *)0,1,30,4,&llbs_remain);
      IIputfldio((char *)"lkbs_remain",(short *)0,1,30,4,&lkbs_remain);
      IIputfldio((char *)"llb_size",(short *)0,1,30,4,&llb_size);
    } /* IIfsetio */
  }
/* # line 370 "sholksum.qsc" */	/* host code */
    if (flag_cluster) 
    {
  i4 enq;
  i4 deq;
  i4 gdlck_search;
  i4 gdeadlock;
  i4 gdlck_grant;
  i4 totl_gdlck_search;
  i4 gdlck_call;
  i4 gdlck_sent;
  i4 cnt_gdlck_call;
  i4 cnt_gdlck_sent;
  i4 unsent_gdlck_search;
  i4 sent_all;
	enq = dispstat->enq;
	deq = dispstat->deq; 
	gdlck_search = dispstat->gdlck_search;
	gdeadlock = dispstat->gdeadlock;
	gdlck_grant = dispstat->gdlck_grant;
	totl_gdlck_search = dispstat->totl_gdlck_search;
	gdlck_call = dispstat->gdlck_call;
	gdlck_sent = dispstat->gdlck_sent;
	cnt_gdlck_call = dispstat->cnt_gdlck_call;
	cnt_gdlck_sent = dispstat->cnt_gdlck_sent;
	unsent_gdlck_search = dispstat->unsent_gdlck_search;
	sent_all = dispstat->sent_all;
/* # line 398 "sholksum.qsc" */	/* putform */
  {
    if (IIfsetio(formname) != 0) {
      IIputfldio((char *)"enq",(short *)0,1,30,4,&enq);
      IIputfldio((char *)"deq",(short *)0,1,30,4,&deq);
      IIputfldio((char *)"gdlck_search",(short *)0,1,30,4,&gdlck_search);
      IIputfldio((char *)"gdeadlock",(short *)0,1,30,4,&gdeadlock);
      IIputfldio((char *)"gdlck_grant",(short *)0,1,30,4,&gdlck_grant);
      IIputfldio((char *)"totl_gdlck_search",(short *)0,1,30,4,&
      totl_gdlck_search);
      IIputfldio((char *)"gdlck_call",(short *)0,1,30,4,&gdlck_call);
      IIputfldio((char *)"gdlck_sent",(short *)0,1,30,4,&gdlck_sent);
      IIputfldio((char *)"cnt_gdlck_call",(short *)0,1,30,4,&cnt_gdlck_call);
      IIputfldio((char *)"cnt_gdlck_sent",(short *)0,1,30,4,&cnt_gdlck_sent);
      IIputfldio((char *)"unsent_gdlck_search",(short *)0,1,30,4,&
      unsent_gdlck_search);
      IIputfldio((char *)"sent_all",(short *)0,1,30,4,&sent_all);
    } /* IIfsetio */
  }
/* # line 409 "sholksum.qsc" */	/* host code */
    }
    return;
}
