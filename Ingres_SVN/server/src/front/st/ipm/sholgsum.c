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
VOID printlgsumm();
/*
** Declare file wide statics
**
** interval_display - set according to whether stats are displayed since
**	system start or since a user specified time
** Since_time - if there is a user specified time that marks the start
**	of the stat collection interval, that time is stored in this var
** Startstat - the statistics obtained when the Interval option was selected.
**	Its needed to calculate the delta change between the start and current
**	statistics.
** Min_tps - the minimum instantaneous commits per second value.  Its a static
**	as it is persistent as long as the user does not exit IPM and the
**	interval option is enabled.
** Max_tps - the maximum instantaneous commits per second value.
** Min_writeio - the minimum instantaneous log i/o's per second value.  Its a static
**	as it is persistent as long as the user does not exit IPM and the
**	interval option is enabled.
** Max_writeio - the maximum instantaneous log i/o's per second value.
** Start_time - the starting time in seconds since Jan 1, 1970.
**	Used to calculate average TPS.  It is not displayed.
** Previous_time - used to calculate length of time between calculations
** 	(i.e. between screen refreshes)
** Previous_lgh_end - stores the previous # of end transactions (commits)
**	from the last refresh.  Used to calculate current TPS.
** Previous_lgh_writeio - stores the previous # of log write io's 
**	from the last refresh.  Used to calculate current phys io/sec.
*/
static bool interval_display = FALSE; /* default is since sys startup */
  static char Since_time[26];
  static char Fshowlgsumm[]= "fshowlgsumm";
static LG_STAT	Startstat;
  static char *Tps_title = 	"----Log-commits-per-second----";
  static i4 Max_tps = 0;
  static i4 Min_tps = -1;
  static char *Writeio_title =	"---Log-write-I/O-per-second---";
  static i4 Max_writeio = 0;
  static i4 Min_writeio = -1;
static i4	Start_time = 0;
static i4	Previous_time = 0;
static i4	Previous_lgs_end = 0;
static i4	Previous_lgs_writeio = 0;
/*
**
**  File: sholgsum.qc
**
**  Purpose - this file contains the routines which display logging
**	system summary form.  
**
**  This file contains:
**	showlgsumm - displays the locking system information form. 
**	printlgsumm - print the info on form or screen
**
**  History
**	8/27/89		tomt	created
**	9/8/89		tomt	added autorefresh capabilities
**	9/24/89		tomt	added interval code
**	10/1/89		tomt	integrated 6.3 changes
**	9/11/90		tomt	modified appearence of displayed form
**	9/14/90		tomt	added commits per second logic.
**				The average
**				can be maintained even though the user leaves
**				the screen and returns.  However, the max and
**				min will be somewhat meaningless upon
**				reentry to the screen.
**				This is because while the user was browsing
**				elsewhere, the monitoring was not continued
**				(hence the granularity of the sample was very
**				coarse which means more likelyhood in missing
**				peaks).  Of course, as soon as the screen starts
**				updating again, the sampling interval becomes
**				acceptable.  The alternative is to clear those
**				values, but the user may want to keep them
**				around while they look at other screens.
**	9/16/90		tomt	added log phys io's/sec
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	11/1/95		nick	removed flag_file
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
VOID
showlgsumm()
{
    bool	e_xit = FALSE;
  i4 timeout;
    static i4	forms_added = FALSE;
  GLOBALREF int *fshowlgsumm;
    /*
    **  Determine if forms are added, if not addform them.
    */
    if (!forms_added)
    {
/* # line 135 "sholgsum.qsc" */	/* addform */
  {
    IIaddform(fshowlgsumm);
  }
/* # line 136 "sholgsum.qsc" */	/* host code */
	forms_added = TRUE;
    }
    /* Set refresh time */
    timeout = frs_timeout;		/* place into local frs var */
/* # line 141 "sholgsum.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
    } /* IIiqset */
  }
/* # line 143 "sholgsum.qsc" */	/* host code */
    /* Display the form */
/* # line 144 "sholgsum.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(3),(void *)IILQint(3));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(3),(void *)IILQint(3));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fshowlgsumm,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 148 "sholgsum.qsc" */	/* host code */
	/* Put information on the form. */
	if (interval_display) /* need to place starting time on form */
	{	/* make everything visible */
/* # line 151 "sholgsum.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,Fshowlgsumm,(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"start_time",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"current_time",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"tps_title",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"min_writeio",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"max_writeio",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"cur_writeio",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"avg_writeio",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"min_tps",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"max_tps",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"cur_tps",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"avg_tps",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
          IIstfsio(54,(char *)"writeio_title",0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 158 "sholgsum.qsc" */	/* putform */
      {
        if (IIfsetio(Fshowlgsumm) != 0) {
          IIputfldio((char *)"start_time",(short *)0,1,32,0,Since_time);
          IIputfldio((char *)"current_time",(short *)0,1,32,0,(char *)"\
now");
        } /* IIfsetio */
      }
/* # line 160 "sholgsum.qsc" */	/* host code */
	}
/* # line 161 "sholgsum.qsc" */	/* putform */
      {
        if (IIfsetio(Fshowlgsumm) != 0) {
          IIputfldio((char *)"tps_title",(short *)0,1,32,0,Tps_title);
          IIputfldio((char *)"writeio_title",(short *)0,1,32,0,Writeio_title);
        } /* IIfsetio */
      }
/* # line 163 "sholgsum.qsc" */	/* host code */
	printlgsumm();	/* put values on form */
	refresh_status(VISIBLE, "flogmenu");
/* # line 165 "sholgsum.qsc" */	/* redisplay */
      {
        IIredisp();
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 169 "sholgsum.qsc" */	/* host code */
	printlgsumm();	/* display the values */
        }
      } else if (IIretval() == 2) {
        {
/* # line 173 "sholgsum.qsc" */	/* host code */
	printlgsumm();	/* display the values */
        }
      } else if (IIretval() == 3) {
        {
/* # line 177 "sholgsum.qsc" */	/* host code */
	/*
	** Allow user to select the interval that is used to determine what
	** stats are displayed.  Selecting Since_Startup will display stats
	** since the INGRES System was started.  Selecting Begin_Now will (if
	** not already) start displaying statistics accumulated since
	** Begin_Now is selected.  Also an invisible field will appear
	** containing the time collection started.  Same for an invisible
	** field showing current time.
	*/
/* # line 186 "sholgsum.qsc" */	/* display */
          {
            if (IInestmu() == 0) goto IIfdE2;
            goto IImuI2;
IIfdI2:;
            if (IIfsetio((char *)0) == 0) goto IIfdE2;
            {
/* # line 189 "sholgsum.qsc" */	/* set_frs */
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
/* # line 193 "sholgsum.qsc" */	/* set_frs */
                  {
                    if (IIiqset(2,0,Fshowlgsumm,(char *)0, (char *)0) != 0) {
                      IIstfsio(54,(char *)"start_time",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"current_time",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"tps_title",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"min_writeio",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"max_writeio",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"cur_writeio",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"avg_writeio",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"min_tps",0,(short *)0,1,30,sizeof(1
                      ),(void *)IILQint(1));
                      IIstfsio(54,(char *)"max_tps",0,(short *)0,1,30,sizeof(1
                      ),(void *)IILQint(1));
                      IIstfsio(54,(char *)"cur_tps",0,(short *)0,1,30,sizeof(1
                      ),(void *)IILQint(1));
                      IIstfsio(54,(char *)"avg_tps",0,(short *)0,1,30,sizeof(1
                      ),(void *)IILQint(1));
                      IIstfsio(54,(char *)"writeio_title",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                    } /* IIiqset */
                  }
/* # line 200 "sholgsum.qsc" */	/* host code */
	    interval_display = FALSE;	/* display since sys startup */
	    Previous_time = 0;
	    Previous_lgs_end = 0;
/* # line 204 "sholgsum.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE2;
                  }
                }
              } else if (IIretval() == 2) {
                {
/* # line 208 "sholgsum.qsc" */	/* set_frs */
                  {
                    if (IIiqset(2,0,Fshowlgsumm,(char *)0, (char *)0) != 0) {
                      IIstfsio(54,(char *)"start_time",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"current_time",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"tps_title",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"min_writeio",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"max_writeio",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"cur_writeio",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"avg_writeio",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"min_tps",0,(short *)0,1,30,sizeof(0
                      ),(void *)IILQint(0));
                      IIstfsio(54,(char *)"max_tps",0,(short *)0,1,30,sizeof(0
                      ),(void *)IILQint(0));
                      IIstfsio(54,(char *)"cur_tps",0,(short *)0,1,30,sizeof(0
                      ),(void *)IILQint(0));
                      IIstfsio(54,(char *)"avg_tps",0,(short *)0,1,30,sizeof(0
                      ),(void *)IILQint(0));
                      IIstfsio(54,(char *)"writeio_title",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                    } /* IIiqset */
                  }
/* # line 215 "sholgsum.qsc" */	/* putform */
                  {
                    if (IIfsetio(Fshowlgsumm) != 0) {
                      IIputfldio((char *)"start_time",(short *)0,1,32,0,
                      (char *)"now");
                    } /* IIfsetio */
                  }
/* # line 216 "sholgsum.qsc" */	/* getform */
                  {
                    if (IIfsetio(Fshowlgsumm) != 0) {
                      IIgetfldio((short *)0,1,32,25,Since_time,(char *)
"start_time");
                    } /* IIfsetio */
                  }
/* # line 217 "sholgsum.qsc" */	/* host code */
	    interval_display = TRUE;		/* display stats since now */
	    /* get reference stats */
	    get_lklgstats( GET_LOGSTAT, (PTR) &Startstat);
	    Max_tps = 0;
	    Min_tps = -1;	/* flags 1st sample, used in printlgsumm()*/
	    Min_writeio = -1;		
	    Max_writeio = 0;
	    Start_time = TMsecs();	/* get the starting time */
	    /*
	    ** wait for delta time after obtaining the reference (starting)
	    ** stats.  This ensures the calculations are right.
	    ** also put up current time
	    */
/*##	    MESSAGE "Starting interval display..."*/
/*##	    SLEEP 1*/	/* remove this sleep and IPM crashes */
/* # line 235 "sholgsum.qsc" */	/* putform */
                  {
                    if (IIfsetio(Fshowlgsumm) != 0) {
                      IIputfldio((char *)"current_time",(short *)0,1,32,0,
                      (char *)"now");
                    } /* IIfsetio */
                  }
/* # line 236 "sholgsum.qsc" */	/* breakdisplay */
                  {
                    if (1) goto IIfdE2;
                  }
                }
              } else if (IIretval() == 3) {
                {
/* # line 240 "sholgsum.qsc" */	/* breakdisplay */
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
/* # line 244 "sholgsum.qsc" */	/* host code */
	    printlgsumm();
/* # line 245 "sholgsum.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
              } /* IIiqset */
            }
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 250 "sholgsum.qsc" */	/* host code */
	/* Set refresh time */
/* # line 251 "sholgsum.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 252 "sholgsum.qsc" */	/* host code */
	my_help("ipmshlgs.hlp", "Logging System Summary");
	timeout = frs_timeout;		/* place into local frs var */
/* # line 254 "sholgsum.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 259 "sholgsum.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 263 "sholgsum.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
/* # line 264 "sholgsum.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 265 "sholgsum.qsc" */	/* breakdisplay */
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
/* # line 268 "sholgsum.qsc" */	/* host code */
    if (e_xit) {		/* user wants to leave */
	terminate(OK);
    }
/* # line 271 "sholgsum.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 272 "sholgsum.qsc" */	/* host code */
    refresh_status(INVISIBLE, "flogmenu");
    return;
}
/*
** printlgsumm - displays the information on the screen
**
** Inputs - none
**
** Outputs - to screen or file
**
** Returns - none
**
** History:
**	9/19/89		tomt	created from showlgsumm routine
**	9/24/89		tomt	added interval code
**	10/1/89		tomt	integrated 6.3 changes
**	9/13/90		tomt	added tps logic.  When generating
**				difference stats, the second set should
**				obtained a set amount of time after the first.
**	9/16/90		tomt	added phys io logic.
**				
**
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	15-Mar-2006 (jenjo02)
**	    Fixed up log stall wait counts, total waits, now embedded
**	    in lgs_wait[] array.
*/
VOID
printlgsumm()
{
    /*
    ** declare
    */
  i4 lgkb;
  i4 cur_writeio, avg_writeio;
  i4 cur_tps, avg_tps;
    LG_STAT	*dispstat;
    LG_STAT	diffstat;
    LG_STAT	curstat;
    i4			current_time;	/* time is seconds since 1/1/70 */
    i4			total_time;	/* same */
    i4			delta_time;	/* same */
    i4		delta_lgs_end;
    i4		delta_lgs_writeio;
  i4 lgs_add;
  i4 lgs_remove;
  i4 lgs_begin;
  i4 lgs_end;
  i4 lgs_readio;
  i4 lgs_writeio;
  i4 lgs_write;
  i4 lgs_force;
  i4 lgs_wait;
  i4 lgs_split;
  i4 lgs_group;
  i4 lgs_group_count;
  i4 lgs_inconsist_db;
  i4 lgs_check_timer;
  i4 lgs_timer_write;
  i4 lgs_free_wait;
  i4 lgs_stall_wait;
    get_lklgstats( GET_LOGSTAT, (PTR) &curstat);
    if (interval_display == TRUE)	/* calculate the difference */
    {
	dispstat = &diffstat;
	diffstat.lgs_add = curstat.lgs_add - Startstat.lgs_add;
	diffstat.lgs_remove = curstat.lgs_remove - Startstat.lgs_remove;
	diffstat.lgs_begin = curstat.lgs_begin - Startstat.lgs_begin;
	diffstat.lgs_end = curstat.lgs_end - Startstat.lgs_end;
	diffstat.lgs_readio = curstat.lgs_readio - Startstat.lgs_readio;
	diffstat.lgs_writeio = curstat.lgs_writeio - Startstat.lgs_writeio;
	diffstat.lgs_write = curstat.lgs_write - Startstat.lgs_write;
	diffstat.lgs_force = curstat.lgs_force - Startstat.lgs_force;
	diffstat.lgs_wait[LG_WAIT_ALL] = 
	    curstat.lgs_wait[LG_WAIT_ALL] - 
		Startstat.lgs_wait[LG_WAIT_ALL];
	diffstat.lgs_split = curstat.lgs_split - Startstat.lgs_split;
	diffstat.lgs_group = curstat.lgs_group - Startstat.lgs_group;
	diffstat.lgs_group_count = curstat.lgs_group_count -
	    Startstat.lgs_group_count;
	diffstat.lgs_inconsist_db = curstat.lgs_inconsist_db -
	    Startstat.lgs_inconsist_db;
	diffstat.lgs_check_timer = curstat.lgs_check_timer -
	    Startstat.lgs_check_timer;
	diffstat.lgs_timer_write = curstat.lgs_timer_write -
	    Startstat.lgs_timer_write;
	diffstat.lgs_wait[LG_WAIT_FREEBUF] = 
	    curstat.lgs_wait[LG_WAIT_FREEBUF] -
	        Startstat.lgs_wait[LG_WAIT_FREEBUF];
	/* 3 possible stalls */
	diffstat.lgs_wait[LG_WAIT_CKPDB] = 
	    curstat.lgs_wait[LG_WAIT_CKPDB] -
		Startstat.lgs_wait[LG_WAIT_CKPDB];
	diffstat.lgs_wait[LG_WAIT_BCP] = 
	    curstat.lgs_wait[LG_WAIT_BCP] -
		Startstat.lgs_wait[LG_WAIT_BCP];
	diffstat.lgs_wait[LG_WAIT_LOGFULL] = 
	    curstat.lgs_wait[LG_WAIT_LOGFULL] -
		Startstat.lgs_wait[LG_WAIT_LOGFULL];
	diffstat.lgs_kbytes = curstat.lgs_kbytes - Startstat.lgs_kbytes;
	/*
	** Calculate the delta and total time for tps and phys io.  Also 
	** calculate delta for count of commits and phys io
	*/
	current_time = TMsecs();	/* # of seconds since 1/1/70 */	
	total_time = current_time - Start_time;	/* # of seconds since start */
	if (total_time <= 0)
	{
	    total_time = 1;
	}
	delta_time = current_time - Previous_time; /* interval since last refresh */
	Previous_time = current_time;	/* store for next calculation */
        delta_lgs_end = curstat.lgs_end - Previous_lgs_end;
	Previous_lgs_end = curstat.lgs_end;
        delta_lgs_writeio = curstat.lgs_writeio - Previous_lgs_writeio;
	Previous_lgs_writeio = curstat.lgs_writeio;
	/*
	** Calculate the average, current TPS.  Also check min and max
	** TPS
	*/
	avg_tps = diffstat.lgs_end / total_time;
	if (delta_time <= 0)	/* prevent divide by 0 */
	{
	    delta_time = 1;	/* granularity is one second anyway */
	}
	cur_tps = delta_lgs_end / delta_time;
	if (cur_tps > Max_tps)
	{
	    Max_tps = cur_tps;
	}
	/*
	** Calculate the average, current Physical log write io's.  Also
	** check min and max i/o's
	*/
	avg_writeio = diffstat.lgs_writeio / total_time;
	if (delta_time <= 0)	/* prevent divide by 0 */
	{
	    delta_time = 1;	/* granularity is one second anyway */
	}
	cur_writeio = delta_lgs_writeio / delta_time;
	if (cur_writeio > Max_writeio)
	{
	    Max_writeio = cur_writeio;
	}
	/*
	** In remembering the minimum tps, care needs to be taken the
	** first time an interval occurs after the user selects the Interval
	** option.  The sholghdr routine sets this to -1 the first time thru.
	** This is to prevent a 0 (zero) from being compared to the 
	** current tps the first time thru, as the 0 would always remain.
	** So we set Min_tps to the current tps on the first pass.
	*/
	if (Min_tps == -1)	/* the first time thru */
	{
	    Min_tps = cur_tps;
	}
	else
	{
	    if (cur_tps < Min_tps)
	    {
		Min_tps = cur_tps;
	    }
	}
	/*
	** In remembering the minimum writeio, care needs to be taken the
	** first time an interval occurs after the user selects the Interval
	** option.  The sholghdr routine sets this to -1 the first time thru.
	** This is to prevent a 0 (zero) from being compared to the 
	** current log writeio the first time thru, as the 0 would always remain.
	** So we set Min_writeio to the current writeio on the first pass.
	*/
	if (Min_writeio == -1)	/* the first time thru */
	{
	    Min_writeio = cur_writeio;
	}
	else
	{
	    if (cur_writeio < Min_writeio)
	    {
		Min_writeio = cur_writeio;
	    }
	}
    }
    else	/* display the current stats */
    {
	dispstat = &curstat;
	Min_tps = Max_tps = 0;	/* zero out to prevent garbage in putform */
	cur_tps = avg_tps = 0;	/* zero out to prevent garbage in putform */
	Min_writeio = Max_writeio = 0;	/* zero out to prevent garbage in putform */
	cur_writeio = avg_writeio = 0;	/* zero out to prevent garbage in putform */
    }
    lgs_add = dispstat->lgs_add;
    lgs_remove = dispstat->lgs_remove;
    lgs_begin = dispstat->lgs_begin;
    lgs_end = dispstat->lgs_end;
    lgs_readio = dispstat->lgs_readio;
    lgs_writeio = dispstat->lgs_writeio;
    lgs_write = dispstat->lgs_write;
    lgs_force = dispstat->lgs_force;
    lgs_wait = dispstat->lgs_wait[LG_WAIT_ALL];
    lgs_split = dispstat->lgs_split;
    lgs_group = dispstat->lgs_group;
    lgs_group_count = dispstat->lgs_group_count;
    lgs_inconsist_db = dispstat->lgs_inconsist_db;
    lgs_check_timer = dispstat->lgs_check_timer;
    lgs_timer_write = dispstat->lgs_timer_write;
    lgs_free_wait = dispstat->lgs_wait[LG_WAIT_FREEBUF];
    lgs_stall_wait = dispstat->lgs_wait[LG_WAIT_CKPDB] +
		     dispstat->lgs_wait[LG_WAIT_BCP] + 
		     dispstat->lgs_wait[LG_WAIT_LOGFULL];
    lgkb = dispstat->lgs_kbytes / 2;	/* element is 2 * real number of kb */
/* # line 499 "sholgsum.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowlgsumm) != 0) {
      IIputfldio((char *)"lgs_add",(short *)0,1,30,4,&lgs_add);
      IIputfldio((char *)"current_time",(short *)0,1,32,0,(char *)"now");
      IIputfldio((char *)"lgs_remove",(short *)0,1,30,4,&lgs_remove);
      IIputfldio((char *)"lgs_begin",(short *)0,1,30,4,&lgs_begin);
      IIputfldio((char *)"lgs_end",(short *)0,1,30,4,&lgs_end);
      IIputfldio((char *)"lgs_readio",(short *)0,1,30,4,&lgs_readio);
      IIputfldio((char *)"lgs_writeio",(short *)0,1,30,4,&lgs_writeio);
      IIputfldio((char *)"lgs_write",(short *)0,1,30,4,&lgs_write);
      IIputfldio((char *)"lgs_force",(short *)0,1,30,4,&lgs_force);
      IIputfldio((char *)"lgs_wait",(short *)0,1,30,4,&lgs_wait);
      IIputfldio((char *)"lgs_split",(short *)0,1,30,4,&lgs_split);
      IIputfldio((char *)"lgs_group",(short *)0,1,30,4,&lgs_group);
      IIputfldio((char *)"lgs_group_count",(short *)0,1,30,4,&lgs_group_count);
      IIputfldio((char *)"lgs_inconsist_db",(short *)0,1,30,4,&
      lgs_inconsist_db);
      IIputfldio((char *)"lgs_check_timer",(short *)0,1,30,4,&lgs_check_timer);
      IIputfldio((char *)"lgs_timer_write",(short *)0,1,30,4,&lgs_timer_write);
      IIputfldio((char *)"lgs_kbytes",(short *)0,1,30,4,&lgkb);
      IIputfldio((char *)"lgs_free_wait",(short *)0,1,30,4,&lgs_free_wait);
      IIputfldio((char *)"lgs_stall_wait",(short *)0,1,30,4,&lgs_stall_wait);
      IIputfldio((char *)"min_tps",(short *)0,1,30,4,&Min_tps);
      IIputfldio((char *)"max_tps",(short *)0,1,30,4,&Max_tps);
      IIputfldio((char *)"cur_tps",(short *)0,1,30,4,&cur_tps);
      IIputfldio((char *)"avg_tps",(short *)0,1,30,4,&avg_tps);
      IIputfldio((char *)"min_writeio",(short *)0,1,30,4,&Min_writeio);
      IIputfldio((char *)"max_writeio",(short *)0,1,30,4,&Max_writeio);
      IIputfldio((char *)"cur_writeio",(short *)0,1,30,4,&cur_writeio);
      IIputfldio((char *)"avg_writeio",(short *)0,1,30,4,&avg_writeio);
    } /* IIfsetio */
  }
/* # line 515 "sholgsum.qsc" */	/* host code */
}
