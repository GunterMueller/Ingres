# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	implocks.sc
**
** This file contains all of the routines needed to support the "LOCKS" menu
** tree of the IMA based IMP lookalike tools.
**
** In an attempt to avoid globals and to shut up the preprocessor - this
** file uses many similar names for the same object types. The plan was to
** have a declared strucure type in impcommon.sc and declare local instances
** of the structure in each routine. I never got around to this.
**
** I also started to put some error checking in here - but this needs
** reworking - most stuff here breaks if a remote vnode does not respond
** because of bug 62631.
**
** The general format of things in this file is:
**
**	processBlah()	- called from the Menu
**		getBlah() - get the values from the IMA table
**		displayBlah() - do the putform stuff
**
**
** As we pull a value from the form to examine, there is a good chance that
** when we do the select to look up the details - the situation will have
** changed - this is a difference from IPM - which takes a snapshot of the 
** world and then examines the snapshot
**
** Looking up the resource names is a bit of a nightmare - the way in which we
** get connected to the correct databases needs to be rethought - the way it 
** is currently done 'just happened'.
**		
## History:
##
##	10-feb-94	(johna)
##			created from routines in individual files
##			impdsplkd.sc
##			impdsplklst.sc
##			implkmain.sc
##
##	18-mar-94	(johna)
##			reworked queries to use the keys!
##
## 	04-may-94	(johna)
##			some comments.
*/
/* 
##      10-apr-02       (mdf100402)
##			Altered the registration of the imp_lkmo_llb table .
##			Removed fields from this routine accordingly.
##
##			Altered the registration of the imp_lkmo_rsb table .
##			Removed fields from this routine accordingly.
##
##			Altered the registration of the imp_lkmo_lkb table .
##			Removed fields from this routine accordingly.
##
##			When examining lock list, always get the database
##                      name because there is no guarantee that the last
##                      locklist which was 'Examine'd was for
##                      the same dbname as the current LockList.
##
##      17-apr-02       (mdf170402)
##			Wrapped a varchar() function around column name.
##			Otherwise, 2.5/6 gives fatal exception error.
##
##      24-apr-02       (mdf240402)
##			Removed a condition from where clause.
##			Otherwise, 2.6 gives no locklist information
##
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##      04-Sep-03       (mdf040903)                     
##              Commit after calling imp_complete_vnode 
##              Commit more often especially after selecting from iidatabase !
##              Add Row Number info for Row Level locking
##      06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
##	08-Oct-2007 (hanje04)
##	    SIR 114907
##	    Remove declaration of timebuf as it's not referenced and causes
##	    errors on Max OSX
##
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
  extern int imp_lk_menu;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern int userSession;
  extern char *currentVnode;
  char reason[100];
  MenuEntry LkMenuList[]= {
	1,	"summary","Display a Locking System Summary",
	2,	"locks", "Display locklists and related information",
	3,	"resources","Display resource lists and related information",
	0,	"no_entry","No Entry"
};
SessEntry *newSessEntry();
void resetLockSummary();
void getLockSummary();
void displayLockSummary();
void displayLockLists();
void clearLockLists();
void examineLockList();
void getTableName();
/*
** Name:	implkmain.sc
**
** Locking System Menu for the IMA based IMP-like tool
**
## History:
##
## 16-feb-94	(johna)
##		Written.
## 10-feb-94	(johna)
##		moved into the implocks file
*/
int
lockingMenu()
{
  int i;
  char buf[60];
/* # line 148 "implocks.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lk_menu",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 151 "implocks.sc" */	/* host code */
		makeIidbdbConnection();
/* # line 152 "implocks.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lk_menu") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 154 "implocks.sc" */	/* host code */
		/* 
##
##mdf100402. 
                ** We need to look at all sessions in this installation. 
                ** Therefore, reset and then look at the 
                ** complete vnode details .
                */
/* # line 161 "implocks.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_reset_domain");
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 162 "implocks.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 163 "implocks.sc" */	/* host code */
/* 
##
##mdf240402 */
/* # line 166 "implocks.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_set_domain");
      IILQprvProcValio((char *)"entry",0,(short *)0,1,32,0,currentVnode);
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 168 "implocks.sc" */	/* host code */
/*
##
##mdf040903
*/
/* # line 172 "implocks.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 173 "implocks.sc" */	/* host code */
/* 
##
##mdf040903 
*/
/* # line 177 "implocks.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_complete_vnode");
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 178 "implocks.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 179 "implocks.sc" */	/* host code */
/*
##
##mdf240402 
*/
/* # line 184 "implocks.sc" */	/* inittable */
    {
      if (IItbinit((char *)"imp_lk_menu",(char *)"imp_menu",(char *)"r") != 0
      ) {
        IItfill();
      } /* IItbinit */
    }
/* # line 185 "implocks.sc" */	/* host code */
		i = 0;
		while (LkMenuList[i].item_number != 0) {
/* # line 187 "implocks.sc" */	/* loadtable */
    {
      if (IItbact((char *)"imp_lk_menu",(char *)"imp_menu",1) != 0) {
        IItcoputio((char *)"item_number",(short *)0,1,30,4,&
        LkMenuList[i].item_number);
        IItcoputio((char *)"short_name",(short *)0,1,32,0,
        LkMenuList[i].short_name);
        IItcoputio((char *)"long_name",(short *)0,1,32,0,
        LkMenuList[i].long_name);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 191 "implocks.sc" */	/* host code */
			i++;
		}
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 197 "implocks.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lk_menu",(char *)"imp_menu",-2) != 0) 
          {
            IItcogetio((short *)0,1,30,4,&i,(char *)"item_number");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 198 "implocks.sc" */	/* host code */
		switch (i) {
		case 1:	processLockSummary();
			break;
		case 2: processLockLists();
			break;
		case 3: displayResourceList();
			break;
		default:
			sprintf(buf,"Unknown option %d",i);
/* # line 207 "implocks.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(buf);
        }
/* # line 208 "implocks.sc" */	/* host code */
		}
      } else if (IIretval() == 2) {
/* # line 213 "implocks.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"End",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 216 "implocks.sc" */	/* host code */
	if (strcmp(buf,currentVnode) == 0) {
/* # line 217 "implocks.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_complete_vnode");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 218 "implocks.sc" */	/* host code */
		if (sqlerr_check() == NO_ROWS) {
			fatal("lockingMenu: cannot execute imp_complete_vnode");
		}
/* # line 221 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 222 "implocks.sc" */	/* host code */
/* 
##
##mdf040903 
*/
	}
	return(TRUE);
}
/*
** Name:	impdsplkd.sc
**
** Display the locking system summary
**
## History:
##
## 	16-feb-94	(johna)
##			written
##	10-mar-94	(johna)
##			Moved into the implocks.sc file
##
*/
  extern int imp_lock_summary;
  extern int inFormsMode;
  extern int timeVal;
  struct lkd_ {
    char vnode[65];
    long lkd_rsh_size;
    long lkd_lkh_size;
    long lkd_max_lkb;
    long lkd_stat_create_list;
    long lkd_stat_release_all;
    long lkd_llb_inuse;
    long lock_lists_remaining;
    long lock_lists_available;
    long lkd_lkb_inuse;
    long locks_remaining;
    long locks_available;
    long lkd_rsb_inuse;
    long lkd_stat_request_new;
    long lkd_stat_request_cvt;
    long lkd_stat_convert;
    long lkd_stat_release;
    long lkd_stat_cancel;
    long lkd_stat_rlse_partial;
    long lkd_stat_dlock_srch;
    long lkd_stat_cvt_deadlock;
    long lkd_stat_deadlock;
    long lkd_stat_convert_wait;
    long lkd_stat_wait;
  };
  struct lkd_ lkd;
  struct lkd_ new_lkd;
  int newStats = 0;
int
processLockSummary()
{
  char vnode_name[SERVER_NAME_LENGTH];
/* # line 291 "implocks.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lock_summary",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
/* # line 294 "implocks.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lock_summary") != 0) {
        IIputfldio((char *)"vnode_name",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 295 "implocks.sc" */	/* host code */
		getLockSummary();
		displayLockSummary();
/* # line 297 "implocks.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 302 "implocks.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 303 "implocks.sc" */	/* display */
        {
          if (IInestmu() == 0) goto IIfdE3;
          goto IImuI3;
IIfdI3:;
IIfdB3:
          while (IIrunnest() != 0) {
            if (IIretval() == 1) {
/* # line 307 "implocks.sc" */	/* host code */
			newStats = 0;
			resetLockSummary();
			getLockSummary();
			displayLockSummary();
/* # line 311 "implocks.sc" */	/* set_frs */
              {
                if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                  IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
                } /* IIiqset */
              }
/* # line 312 "implocks.sc" */	/* enddisplay */
              {
                if (1) goto IIfdF3;
              }
            } else if (IIretval() == 2) {
/* # line 317 "implocks.sc" */	/* host code */
			newStats = 1;
			resetLockSummary();
			getLockSummary();
			copyLockSummary();
			getLockSummary();
			displayLockSummary();
/* # line 323 "implocks.sc" */	/* set_frs */
              {
                if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                  IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
                } /* IIiqset */
              }
/* # line 324 "implocks.sc" */	/* enddisplay */
              {
                if (1) goto IIfdF3;
              }
            } else {
              if (1) goto IIfdE3;
            } /* IIretval */
          } /* IIrunnest */
IIfdF3:
          if (IIchkfrm() == 0) goto IIfdB3;
          goto IIfdE3;
IImuI3:
          if (IIinitmu() == 0) goto IIfdE3;
          if (IInmuact((char *)"Since_Startup",(char *)0,2,2,1) == 0) goto 
          IIfdE3;
          if (IInmuact((char *)"Begin_Now",(char *)0,2,2,2) == 0) goto IIfdE3;
          if (IIendmu() == 0) goto IIfdE3;
          goto IIfdI3;
IIfdE3:;
          IIendnest();
        }
      } else if (IIretval() == 2) {
/* # line 331 "implocks.sc" */	/* host code */
		getLockSummary();
		displayLockSummary();
      } else if (IIretval() == 3) {
		getLockSummary();
		displayLockSummary();
      } else if (IIretval() == 4) {
/* # line 343 "implocks.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 344 "implocks.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
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
    if (IInmuact((char *)"Interval",(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IIFRtoact(0,2) == 0) goto IIfdE2;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 347 "implocks.sc" */	/* host code */
	return(TRUE);
}
void resetLockSummary()
{
	lkd.lkd_stat_create_list = 0;
	lkd.lkd_stat_release_all = 0;
	lkd.lkd_rsb_inuse = 0;
	lkd.lkd_stat_request_new = 0;
	lkd.lkd_stat_request_cvt = 0;
	lkd.lkd_stat_convert = 0;
	lkd.lkd_stat_release = 0;
	lkd.lkd_stat_cancel = 0;
	lkd.lkd_stat_rlse_partial = 0;
	lkd.lkd_stat_dlock_srch = 0;
	lkd.lkd_stat_cvt_deadlock = 0;
	lkd.lkd_stat_deadlock = 0;
	lkd.lkd_stat_convert_wait = 0;
	lkd.lkd_stat_wait = 0;
}
copyLockSummary()
{
	lkd.lkd_stat_create_list = new_lkd.lkd_stat_create_list;
	lkd.lkd_stat_release_all = new_lkd.lkd_stat_release_all;
	lkd.lkd_stat_request_new = new_lkd.lkd_stat_request_new;
	lkd.lkd_stat_request_cvt = new_lkd.lkd_stat_request_cvt;
	lkd.lkd_stat_convert = new_lkd.lkd_stat_convert;
	lkd.lkd_stat_release = new_lkd.lkd_stat_release;
	lkd.lkd_stat_cancel = new_lkd.lkd_stat_cancel;
	lkd.lkd_stat_rlse_partial = new_lkd.lkd_stat_rlse_partial;
	lkd.lkd_stat_dlock_srch = new_lkd.lkd_stat_dlock_srch;
	lkd.lkd_stat_cvt_deadlock = new_lkd.lkd_stat_cvt_deadlock;
	lkd.lkd_stat_deadlock = new_lkd.lkd_stat_deadlock;
	lkd.lkd_stat_convert_wait = new_lkd.lkd_stat_convert_wait;
	lkd.lkd_stat_wait = new_lkd.lkd_stat_wait;
}
void getLockSummary()
{
/* # line 387 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks1",7541,4515);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implocks1",7541,4515);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select vnode, lkd_rsh_size, lkd_lkh_size, lkd_max_lkb, lkd_stat_creat\
e_list, lkd_stat_release_all, lkd_llb_inuse, lock_lists_remaining, loc\
k_lists_available, lkd_lkb_inuse, locks_remaining, locks_available, lk\
d_rsb_inuse, lkd_stat_request_new, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"lkd_stat_request_cvt, lkd_stat_convert, lkd_stat_release, lkd_stat_ca\
ncel, lkd_stat_rlse_partial, lkd_stat_dlock_srch, lkd_stat_cvt_deadloc\
k, lkd_stat_deadlock, lkd_stat_convert_wait, lkd_stat_wait from imp_lk\
_summary_view where vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implocks1",7541,4515);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,64,new_lkd.vnode);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_rsh_size);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_lkh_size);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_max_lkb);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_create_list);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_release_all);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_llb_inuse);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lock_lists_remaining);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lock_lists_available);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_lkb_inuse);
      IIgetdomio((short *)0,1,30,4,&new_lkd.locks_remaining);
      IIgetdomio((short *)0,1,30,4,&new_lkd.locks_available);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_rsb_inuse);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_request_new);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_request_cvt);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_convert);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_release);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_cancel);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_rlse_partial);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_dlock_srch);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_cvt_deadlock);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_deadlock);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_convert_wait);
      IIgetdomio((short *)0,1,30,4,&new_lkd.lkd_stat_wait);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 440 "implocks.sc" */	/* host code */
	if (sqlerr_check() == NO_ROWS) {
		fatal("getLockSummary: no rows from lk_summary_view");
	}
	new_lkd.lkd_stat_create_list -= lkd.lkd_stat_create_list;
	new_lkd.lkd_stat_release_all -= lkd.lkd_stat_release_all;
	new_lkd.lkd_stat_request_new -= lkd.lkd_stat_request_new;
	new_lkd.lkd_stat_request_cvt -= lkd.lkd_stat_request_cvt;
	new_lkd.lkd_stat_convert -= lkd.lkd_stat_convert;
	new_lkd.lkd_stat_release -= lkd.lkd_stat_release;
	new_lkd.lkd_stat_cancel -= lkd.lkd_stat_cancel;
	new_lkd.lkd_stat_rlse_partial -= lkd.lkd_stat_rlse_partial;
	new_lkd.lkd_stat_dlock_srch -= lkd.lkd_stat_dlock_srch;
	new_lkd.lkd_stat_cvt_deadlock -= lkd.lkd_stat_cvt_deadlock;
	new_lkd.lkd_stat_deadlock -= lkd.lkd_stat_deadlock;
	new_lkd.lkd_stat_convert_wait -= lkd.lkd_stat_convert_wait;
	new_lkd.lkd_stat_wait -= lkd.lkd_stat_wait;
/* # line 457 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 458 "implocks.sc" */	/* host code */
/* 
##
##mdf040903 
*/
}
void displayLockSummary()
{
/* # line 466 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"vnode_name",(short *)0,1,32,0,new_lkd.vnode);
    } /* IIfsetio */
  }
/* # line 468 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"res_tbl_size",(short *)0,1,30,4,&
      new_lkd.lkd_rsh_size);
    } /* IIfsetio */
  }
/* # line 470 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"lock_tbl_size",(short *)0,1,30,4,&
      new_lkd.lkd_lkh_size);
    } /* IIfsetio */
  }
/* # line 472 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_per_tx",(short *)0,1,30,4,&new_lkd.lkd_max_lkb);
    } /* IIfsetio */
  }
/* # line 474 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"ll_created",(short *)0,1,30,4,&
      new_lkd.lkd_stat_create_list);
    } /* IIfsetio */
  }
/* # line 476 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"ll_released",(short *)0,1,30,4,&
      new_lkd.lkd_stat_release_all);
    } /* IIfsetio */
  }
/* # line 478 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"ll_inuse",(short *)0,1,30,4,&new_lkd.lkd_llb_inuse);
    } /* IIfsetio */
  }
/* # line 480 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"ll_remaining",(short *)0,1,30,4,&
      new_lkd.lock_lists_remaining);
    } /* IIfsetio */
  }
/* # line 482 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"ll_total",(short *)0,1,30,4,&
      new_lkd.lock_lists_available);
    } /* IIfsetio */
  }
/* # line 484 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_in_use",(short *)0,1,30,4,&
      new_lkd.lkd_lkb_inuse);
    } /* IIfsetio */
  }
/* # line 486 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_remaining",(short *)0,1,30,4,&
      new_lkd.locks_remaining);
    } /* IIfsetio */
  }
/* # line 488 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_total",(short *)0,1,30,4,&
      new_lkd.locks_available);
    } /* IIfsetio */
  }
/* # line 490 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"res_in_use",(short *)0,1,30,4,&new_lkd.lkd_rsb_inuse);
    } /* IIfsetio */
  }
/* # line 492 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_requested",(short *)0,1,30,4,&
      new_lkd.lkd_stat_request_new);
    } /* IIfsetio */
  }
/* # line 494 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_rerequested",(short *)0,1,30,4,&
      new_lkd.lkd_stat_request_cvt);
    } /* IIfsetio */
  }
/* # line 496 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_converted",(short *)0,1,30,4,&
      new_lkd.lkd_stat_convert);
    } /* IIfsetio */
  }
/* # line 498 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_released",(short *)0,1,30,4,&
      new_lkd.lkd_stat_release);
    } /* IIfsetio */
  }
/* # line 500 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_cancelled",(short *)0,1,30,4,&
      new_lkd.lkd_stat_cancel);
    } /* IIfsetio */
  }
/* # line 502 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"locks_escalated",(short *)0,1,30,4,&
      new_lkd.lkd_stat_rlse_partial);
    } /* IIfsetio */
  }
/* # line 504 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"deadlock_search",(short *)0,1,30,4,&
      new_lkd.lkd_stat_dlock_srch);
    } /* IIfsetio */
  }
/* # line 506 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"convert_deadlock",(short *)0,1,30,4,&
      new_lkd.lkd_stat_cvt_deadlock);
    } /* IIfsetio */
  }
/* # line 508 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"deadlock",(short *)0,1,30,4,&
      new_lkd.lkd_stat_deadlock);
    } /* IIfsetio */
  }
/* # line 510 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"convert_wait",(short *)0,1,30,4,&
      new_lkd.lkd_stat_convert_wait);
    } /* IIfsetio */
  }
/* # line 512 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_lock_summary") != 0) {
      IIputfldio((char *)"lock_wait",(short *)0,1,30,4,&new_lkd.lkd_stat_wait);
    } /* IIfsetio */
  }
/* # line 514 "implocks.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 515 "implocks.sc" */	/* host code */
}
/*
** Name:	impdsplklst.sc
**
** Display the list of lock lists for the new IMA based IPM lookalike tool.
**
## History:
##
## 24-feb-94	(johna)
##	Written.
##
##	10-mar-94	(johna)
##			Moved into the implocks.sc file
*/
  extern int imp_lk_list;
  extern int inFormsMode;
  extern int timeVal;
int
processLockLists()
{
  SessEntry *current;
  int pid;
  int scb_pid;
  char thread[20];
  char session_id[20];
  char vnode_name[30];
  char session_name[30];
  char e_server[SERVER_NAME_LENGTH];
  SessEntry *SessListHead;
  SessEntry *current_sess;
  int lkb_id_id;
  int llb_id_id;
  char buf[60];
/* # line 566 "implocks.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lk_list",(char *)"r") == 0) goto IIfdE4;
    goto IImuI4;
IIfdI4:;
    if (IIfsetio((char *)0) == 0) goto IIfdE4;
/* # line 569 "implocks.sc" */	/* inittable */
    {
      if (IItbinit((char *)"imp_lk_list",(char *)"locktbl",(char *)"r") != 0) 
      {
        IItfill();
      } /* IItbinit */
    }
/* # line 570 "implocks.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lk_list") != 0) {
        IIputfldio((char *)"vnode",(short *)0,1,32,0,currentVnode);
      } /* IIfsetio */
    }
/* # line 571 "implocks.sc" */	/* host code */
		displayLockLists();
IIfdB4:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		clearLockLists();
		displayLockLists();
      } else if (IIretval() == 2) {
/* # line 582 "implocks.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lk_list",(char *)"locktbl",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&pid,(char *)"pid");
            IItcogetio((short *)0,1,32,19,thread,(char *)"sid");
            IItcogetio((short *)0,1,32,29,session_name,(char *)"userna\
me");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 586 "implocks.sc" */	/* host code */
		displayLockDetail(pid,thread,session_name);
      } else if (IIretval() == 3) {
/* # line 591 "implocks.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lk_list",(char *)"locktbl",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&pid,(char *)"pid");
            IItcogetio((short *)0,1,32,19,thread,(char *)"sid");
            IItcogetio((short *)0,1,30,4,&llb_id_id,(char *)"id_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 595 "implocks.sc" */	/* host code */
		examineLockList(pid,thread,llb_id_id);
      } else if (IIretval() == 4) {
/* # line 600 "implocks.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lk_list",(char *)"locktbl",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&pid,(char *)"pid");
            IItcogetio((short *)0,1,32,19,thread,(char *)"sid");
            IItcogetio((short *)0,1,30,4,&llb_id_id,(char *)"id_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 604 "implocks.sc" */	/* host code */
		findBlockingLocks(pid,thread,llb_id_id);
      } else if (IIretval() == 5) {
/* # line 609 "implocks.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lk_list",(char *)"locktbl",-2) != 0) {
            IItcogetio((short *)0,1,32,19,thread,(char *)"sid");
            IItcogetio((short *)0,1,30,4,&scb_pid,(char *)"scb_pid");
            IItcogetio((short *)0,1,32,SERVER_NAME_LENGTH-1,e_server,(char *)"\
server");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 619 "implocks.sc" */	/* host code */
                /* current_sess = SessListHead; */
                if (current_sess = newSessEntry()) {                    
                    strcpy(current_sess->sessionId,thread );            
                    current_sess->scb_pid = scb_pid;     
/*
##
##mdf240402
*/
                    }                                                   
                if (current_sess) {                                     
                        displaySessionDetail(current_sess,e_server);    
                }                                                       
      } else if (IIretval() == 6) {
/* # line 637 "implocks.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Refresh",(char *)0,2,2,1) == 0) goto IIfdE4;
    if (IInmuact((char *)"More_info",(char *)0,2,2,2) == 0) goto IIfdE4;
    if (IInmuact((char *)"Examine",(char *)0,2,2,3) == 0) goto IIfdE4;
    if (IInmuact((char *)"Block_Info",(char *)0,2,2,4) == 0) goto IIfdE4;
    if (IInmuact((char *)"Session_Info",(char *)0,2,2,5) == 0) goto IIfdE4;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE4;
    if (IIendmu() == 0) goto IIfdE4;
    goto IIfdI4;
IIfdE4:;
    IIendfrm();
  }
/* # line 640 "implocks.sc" */	/* host code */
	return(TRUE);
}
void displayLockLists()
{
  char computed_id[SHORT_STRING];
  char computed_status[50];
  char l_vnode[SERVER_NAME_LENGTH];
  int l_llb_pid;
  char l_database[30];
  int l_scb_pid;
  char l_llb_sid[SESSION_ID_LENGTH];
  int l_llb_id_id;
  char l_username[SESSION_ID_LENGTH];
  int l_llb_lkb_count;
  int l_llb_llkb_count;
  int l_llb_max_lkb;
  char l_llb_status[50];
  char l_state[50];
  char l_server[65];
	sprintf(computed_id,"%s%%",currentVnode);
/* # line 683 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks2",7542,10229);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIputdomio((short *)0,1,32,0,computed_id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"implocks2",7542,10229);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct llb.vnode, sess.server, llb_pid, sess.scb_pid, sess.d\
atabase, llb_sid, llb_id_id, hex(llb_id_id), username, llb_lkb_count, \
llb_llkb_count, llb_max_lkb, llb_status, state from imp_lkmo_llb llb, \
imp_session_view sess where vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIwritio(0,(short *)0,1,32,0,(char *)"and server like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,computed_id);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and varchar(llb_sid)=session_id and llb_related_llb<>0 and sess.datab\
ase!='imadb' order by llb_id_id");
        IIexDefine(0,(char *)"implocks2",7542,10229);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,l_vnode);
      IIgetdomio((short *)0,1,32,64,l_server);
      IIgetdomio((short *)0,1,30,4,&l_llb_pid);
      IIgetdomio((short *)0,1,30,4,&l_scb_pid);
      IIgetdomio((short *)0,1,32,29,l_database);
      IIgetdomio((short *)0,1,32,SESSION_ID_LENGTH-1,l_llb_sid);
      IIgetdomio((short *)0,1,30,4,&l_llb_id_id);
      IIgetdomio((short *)0,1,32,SHORT_STRING-1,computed_id);
      IIgetdomio((short *)0,1,32,SESSION_ID_LENGTH-1,l_username);
      IIgetdomio((short *)0,1,30,4,&l_llb_lkb_count);
      IIgetdomio((short *)0,1,30,4,&l_llb_llkb_count);
      IIgetdomio((short *)0,1,30,4,&l_llb_max_lkb);
      IIgetdomio((short *)0,1,32,49,l_llb_status);
      IIgetdomio((short *)0,1,32,49,l_state);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 763 "implocks.sc" */	/* host code */
		/*sprintf(computed_id,"%lx",(65536 * l_llb_id_id)  */
                              /*  + l_llb_id_instance */
                               /*); */
		if (l_llb_status[0] == ' ') {
			strcpy(computed_status,"<None>");
		} else {
			strcpy(computed_status,l_llb_status);
		}
/* # line 771 "implocks.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_lk_list",(char *)"locktbl",1) != 0) {
          IItcoputio((char *)"pid",(short *)0,1,30,4,&l_llb_pid);
          IItcoputio((char *)"scb_pid",(short *)0,1,30,4,&l_scb_pid);
          IItcoputio((char *)"server",(short *)0,1,32,0,l_server);
          IItcoputio((char *)"database",(short *)0,1,32,0,l_database);
          IItcoputio((char *)"sid",(short *)0,1,32,0,l_llb_sid);
          IItcoputio((char *)"computed_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"id_id",(short *)0,1,30,4,&l_llb_id_id);
          IItcoputio((char *)"username",(short *)0,1,32,0,l_username);
          IItcoputio((char *)"lkb_count",(short *)0,1,30,4,&l_llb_lkb_count);
          IItcoputio((char *)"llkb_count",(short *)0,1,30,4,&l_llb_llkb_count);
          IItcoputio((char *)"max_lkb",(short *)0,1,30,4,&l_llb_max_lkb);
          IItcoputio((char *)"llb_status",(short *)0,1,32,0,computed_status);
          IItcoputio((char *)"state",(short *)0,1,32,0,l_state);
          IITBceColEnd();
        } /* IItbact */
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
/* # line 801 "implocks.sc" */	/* host code */
	(void) sqlerr_check();
/* # line 802 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 803 "implocks.sc" */	/* host code */
/* 
##
##mdf040903 
*/
/* # line 807 "implocks.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 808 "implocks.sc" */	/* host code */
}
void clearLockLists()
{
/* # line 812 "implocks.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_lk_list",(char *)"locktbl",(char *)"f") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 813 "implocks.sc" */	/* host code */
}
  extern int inp_lk_tx;
int
displayLockDetail(pid,session_id,username)
  int pid;
  char *session_id;
  char *username;
{
  int i;
  char tx_id[40];
  char computed_id[SHORT_STRING];
  int count;
  int id_id;
  int instance =0;
  int related_llb;
/* # line 836 "implocks.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_lk_tx",(char *)"r") == 0) goto IIfdE5;
    goto IImuI5;
IIfdI5:;
    if (IIfsetio((char *)0) == 0) goto IIfdE5;
/* # line 840 "implocks.sc" */	/* message */
    {
      IImessage((char *)"Working . . .");
    }
/* # line 841 "implocks.sc" */	/* repeated select */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexExec(1,(char *)"implocks3",7543,22194);
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIputdomio((short *)0,1,30,4,&pid);
        IIputdomio((short *)0,1,32,0,session_id);
        IIputdomio((short *)0,1,32,0,username);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIexDefine(1,(char *)"implocks3",7543,22194);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select lxb_tran_id from imp_lgmo_lxb where vnode=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,currentVnode);
          IIwritio(0,(short *)0,1,32,0,(char *)"and lxb_pid=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,30,4,&pid);
          IIwritio(0,(short *)0,1,32,0,(char *)"and lxb_sid=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,32,0,session_id);
          IIwritio(0,(short *)0,1,32,0,(char *)"and lxb_user_name=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,32,0,username);
          IIexDefine(0,(char *)"implocks3",7543,22194);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,39,tx_id);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 853 "implocks.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lk_tx") != 0) {
        IIputfldio((char *)"tx_id",(short *)0,1,32,0,tx_id);
      } /* IIfsetio */
    }
/* # line 854 "implocks.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 855 "implocks.sc" */	/* host code */
/* 
##
##mdf040903 
*/
/* # line 860 "implocks.sc" */	/* repeated select */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexExec(1,(char *)"implocks4",7544,23777);
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIputdomio((short *)0,1,30,4,&pid);
        IIputdomio((short *)0,1,32,0,session_id);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIexDefine(1,(char *)"implocks4",7544,23777);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select llb_lkb_count, llb_id_id, llb_related_llb from imp_lkmo_llb wh\
ere vnode=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,currentVnode);
          IIwritio(0,(short *)0,1,32,0,(char *)
"and llb_id_id in(select llb_related_llb_id_id from imp_lkmo_llb where\
 vnode=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,32,0,currentVnode);
          IIwritio(0,(short *)0,1,32,0,(char *)"and llb_pid=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,30,4,&pid);
          IIwritio(0,(short *)0,1,32,0,(char *)"and llb_sid=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,32,0,session_id);
          IIwritio(0,(short *)0,1,32,0,(char *)")");
          IIexDefine(0,(char *)"implocks4",7544,23777);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&count);
        IIgetdomio((short *)0,1,30,4,&id_id);
        IIgetdomio((short *)0,1,30,4,&related_llb);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 885 "implocks.sc" */	/* host code */
		if (sqlerr_check() == NO_ROWS) {
/* # line 886 "implocks.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 889 "implocks.sc" */	/* message */
    {
      IImessage((char *)"No Information Found...");
    }
/* # line 890 "implocks.sc" */	/* host code */
			PCsleep(2*1000);
		} else {
/* # line 892 "implocks.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 895 "implocks.sc" */	/* host code */
			sprintf(computed_id,"%lx",(65536 * id_id) + instance);
/* # line 896 "implocks.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lk_tx") != 0) {
        IIputfldio((char *)"related_lock_list_id",(short *)0,1,32,0,
        computed_id);
      } /* IIfsetio */
    }
/* # line 898 "implocks.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lk_tx") != 0) {
        IIputfldio((char *)"related_count",(short *)0,1,30,4,&count);
      } /* IIfsetio */
    }
/* # line 900 "implocks.sc" */	/* host code */
			computed_id[0] = '\0';
			id_id = 0;
			instance = 0;
			/*
			** Get The Wait Resource ID
			*/
/* # line 907 "implocks.sc" */	/* repeated select */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexExec(1,(char *)"implocks5",7545,21737);
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIputdomio((short *)0,1,30,4,&pid);
        IIputdomio((short *)0,1,32,0,session_id);
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIexDefine(1,(char *)"implocks5",7545,21737);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select rsb_id_id from imp_lkmo_rsb where vnode=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
          IIputdomio((short *)0,1,32,0,currentVnode);
          IIwritio(0,(short *)0,1,32,0,(char *)
"and rsb_id_id in(select lkb_rsb_id_id from imp_lkmo_lkb lk, imp_lkmo_\
llb lb where lk.lkb_id_id=lb.llb_wait_id_id and lb.llb_pid=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
          IIputdomio((short *)0,1,30,4,&pid);
          IIwritio(0,(short *)0,1,32,0,(char *)"and lb.llb_sid=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
          IIputdomio((short *)0,1,32,0,session_id);
          IIwritio(0,(short *)0,1,32,0,(char *)
"and lk.vnode=lb.vnode and lk.vnode=");
          IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
          IIputdomio((short *)0,1,32,0,currentVnode);
          IIwritio(0,(short *)0,1,32,0,(char *)")");
          IIexDefine(0,(char *)"implocks5",7545,21737);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&id_id);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 931 "implocks.sc" */	/* host code */
			if (sqlerr_check() != NO_ROWS) {
/* # line 932 "implocks.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 935 "implocks.sc" */	/* host code */
				sprintf(computed_id,"%lx",
					(65536 * id_id) + instance);
/* # line 937 "implocks.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_lk_tx") != 0) {
        IIputfldio((char *)"waiting_resource_list_id",(short *)0,1,32,0,
        computed_id);
      } /* IIfsetio */
    }
/* # line 940 "implocks.sc" */	/* host code */
			}
/* # line 941 "implocks.sc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 942 "implocks.sc" */	/* host code */
		}
IIfdB5:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 947 "implocks.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE5;
        }
      } else {
        if (1) goto IIfdE5;
      } /* IIretval */
    } /* IIrunform */
IIfdF5:
    if (IIchkfrm() == 0) goto IIfdB5;
    goto IIfdE5;
IImuI5:
    if (IIinitmu() == 0) goto IIfdE5;
    if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE5;
    if (IIendmu() == 0) goto IIfdE5;
    goto IIfdI5;
IIfdE5:;
    IIendfrm();
  }
/* # line 950 "implocks.sc" */	/* host code */
	return(TRUE);
}
/*
** have to make the following global to fool the preprocessor into allowing
** selects inside a select loop (in different sessions)
*/
  int gotDbName = FALSE;
  int connectedToDb = FALSE;
  int i;
  char buf[60];
  char userDbName[60];
  int e_llb_id_id;
  int e_lkb_id_id;
  int e_rsb_id_id;
  int e_llb_wait_id_id;
  char e_rsb_grant[5];
  char e_rsb_convert[5];
  char e_lkb_request_mode[5];
  char e_lkb_state[30];
  int e_lkb_rsb_id_id;
  int e_lkb_llb_id_id;
  char e_dbname[30];
  char e_tablename[30];
  char e_s_name[30];
  int e_rsb_name0;
  int e_rsb_name1;
  int e_rsb_name2;
  int e_rsb_name3;
  int e_rsb_name4;
  int e_rsb_name5;
  int e_rsb_name6;
void examineLockList(pid,session_id,llb_id_id)
  int pid;
  char *session_id;
  int llb_id_id;
{
  char computed_id[SHORT_STRING];
  char computed_type[SHORT_STRING];
  char buf[60];
/* # line 1005 "implocks.sc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm((char *)"imp_lk_locks",(char *)"r") == 0) goto IIfdE6;
    goto IImuI6;
IIfdI6:;
    if (IIfsetio((char *)0) == 0) goto IIfdE6;
/* # line 1008 "implocks.sc" */	/* host code */
		ShowLockList(pid,llb_id_id);
IIfdB6:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1013 "implocks.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_lk_locks",(char *)"lock_details",-2) !=
           0) {
            IItcogetio((short *)0,1,32,SHORT_STRING-1,computed_id,(char *)"\
computed_id");
            IItcogetio((short *)0,1,30,4,&e_lkb_rsb_id_id,(char *)
"lkb_rsb_id_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1016 "implocks.sc" */	/* host code */
		displayResourceDetail(e_lkb_rsb_id_id);
      } else if (IIretval() == 2) {
	    ShowLockList(pid,llb_id_id);
      } else if (IIretval() == 3) {
/* # line 1026 "implocks.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Resource_Info",(char *)0,2,2,1) == 0) goto IIfdE6;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE6;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE6;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE6;
    if (IIendmu() == 0) goto IIfdE6;
    goto IIfdI6;
IIfdE6:;
    IIendfrm();
  }
/* # line 1029 "implocks.sc" */	/* host code */
	if (connectedToDb) {
/* # line 1030 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&userSession);
  }
/* # line 1031 "implocks.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 1032 "implocks.sc" */	/* host code */
		connectedToDb = FALSE;
	}
/* # line 1034 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 1035 "implocks.sc" */	/* host code */
}
ShowLockList(pid,llb_id_id)
  int pid;
  int llb_id_id;
{
  char computed_id[SHORT_STRING];
  char computed_type[SHORT_STRING];
/* # line 1048 "implocks.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_lk_locks",(char *)"lock_details",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1050 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks6",7546,29936);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIputdomio((short *)0,1,30,4,&llb_id_id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"implocks6",7546,29936);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select lkb_id_id, lkb_request_mode, lkb_state, lkb_rsb_id_id, lkb_llb\
_id_id, rsb_name0, rsb_name1, rsb_name2, rsb_name3, rsb_name4, rsb_nam\
e5 from imp_lkmo_lkb lk, imp_lkmo_rsb rb where lk.vnode=rb.vnode and l\
k.vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and rsb_id_id=lkb_rsb_id_id and lkb_llb_id_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&llb_id_id);
        IIwritio(0,(short *)0,1,32,0,(char *)
"order by rsb_name1, rsb_name0, rsb_name2, rsb_name3, rsb_name4, rsb_n\
ame5");
        IIexDefine(0,(char *)"implocks6",7546,29936);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&e_lkb_id_id);
      IIgetdomio((short *)0,1,32,4,e_lkb_request_mode);
      IIgetdomio((short *)0,1,32,29,e_lkb_state);
      IIgetdomio((short *)0,1,30,4,&e_lkb_rsb_id_id);
      IIgetdomio((short *)0,1,30,4,&e_lkb_llb_id_id);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name0);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name1);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name2);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name3);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name4);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name5);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 1092 "implocks.sc" */	/* host code */
            getDbName();
            getTableName();
            sprintf(computed_id,"%lx",(65536 * e_lkb_id_id) 
                    /*+ e_lkb_id_instance */
                                        );
            decode_rsb0(e_rsb_name0,computed_type);
            if (e_rsb_name0 != LK_TABLE) {
                if (e_rsb_name0 != LK_ROW) { 
/* # line 1101 "implocks.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_lk_locks",(char *)"lock_details",1) != 0) {
          IItcoputio((char *)"computed_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"lkb_rq_state",(short *)0,1,32,0,
          e_lkb_request_mode);
          IItcoputio((char *)"lkb_gr_state",(short *)0,1,32,0,e_lkb_state);
          IItcoputio((char *)"lkb_type",(short *)0,1,32,0,computed_type);
          IItcoputio((char *)"lkb_db",(short *)0,1,32,0,e_dbname);
          IItcoputio((char *)"lkb_table",(short *)0,1,32,0,e_tablename);
          IItcoputio((char *)"lkb_rsb_id_id",(short *)0,1,30,4,&
          e_lkb_rsb_id_id);
          IItcoputio((char *)"lkb_page",(short *)0,1,30,4,&e_rsb_name4);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1110 "implocks.sc" */	/* host code */
                } else {
/* # line 1111 "implocks.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_lk_locks",(char *)"lock_details",1) != 0) {
          IItcoputio((char *)"computed_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"lkb_rq_state",(short *)0,1,32,0,
          e_lkb_request_mode);
          IItcoputio((char *)"lkb_gr_state",(short *)0,1,32,0,e_lkb_state);
          IItcoputio((char *)"lkb_type",(short *)0,1,32,0,computed_type);
          IItcoputio((char *)"lkb_db",(short *)0,1,32,0,e_dbname);
          IItcoputio((char *)"lkb_table",(short *)0,1,32,0,e_tablename);
          IItcoputio((char *)"lkb_rsb_id_id",(short *)0,1,30,4,&
          e_lkb_rsb_id_id);
          IItcoputio((char *)"lkb_page",(short *)0,1,30,4,&e_rsb_name4);
          IItcoputio((char *)"lkb_row",(short *)0,1,30,4,&e_rsb_name5);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1122 "implocks.sc" */	/* host code */
                } 
            } else {
/* # line 1124 "implocks.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_lk_locks",(char *)"lock_details",1) != 0) {
          IItcoputio((char *)"computed_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"lkb_rq_state",(short *)0,1,32,0,
          e_lkb_request_mode);
          IItcoputio((char *)"lkb_gr_state",(short *)0,1,32,0,e_lkb_state);
          IItcoputio((char *)"lkb_type",(short *)0,1,32,0,computed_type);
          IItcoputio((char *)"lkb_db",(short *)0,1,32,0,e_dbname);
          IItcoputio((char *)"lkb_table",(short *)0,1,32,0,e_tablename);
          IItcoputio((char *)"lkb_rsb_id_id",(short *)0,1,30,4,&
          e_lkb_rsb_id_id);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1132 "implocks.sc" */	/* host code */
            }
/* # line 1133 "implocks.sc" */	/* set_sql */
      {
        IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
/* # line 1135 "implocks.sc" */	/* host code */
        if (sqlerr_check() == NO_ROWS) {
/* # line 1136 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1139 "implocks.sc" */	/* host code */
        sprintf(buf,"No Resources Found for Lock Id %d",llb_id_id);
/* # line 1140 "implocks.sc" */	/* message */
  {
    IImessage(buf);
  }
/* # line 1141 "implocks.sc" */	/* host code */
        PCsleep(2*1000);
        }
/* # line 1143 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1146 "implocks.sc" */	/* host code */
	gotDbName = FALSE;   /* 
##mdf100402 
Always get dbname next time*/
}
int
getDbName()
{
  char buf[100];
	if (gotDbName == FALSE) {
/* # line 1162 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&iidbdbSession);
  }
/* # line 1163 "implocks.sc" */	/* host code */
		sqlerr_check();              /* 
##mdf040903
*/
/* # line 1167 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks7",7547,17582);
      IIputdomio((short *)0,1,30,4,&e_rsb_name1);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implocks7",7547,17582);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select name from iidatabase where db_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&e_rsb_name1);
        IIexDefine(0,(char *)"implocks7",7547,17582);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,29,e_dbname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1175 "implocks.sc" */	/* host code */
		if (sqlerr_check() == NO_ROWS) {
/* # line 1177 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1180 "implocks.sc" */	/* host code */
			gotDbName = FALSE;
			connectedToDb = FALSE;
			strcpy(reason,"Unavailable");
/* # line 1183 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 1184 "implocks.sc" */	/* host code */
			return(FALSE);
		} else {
			gotDbName = TRUE; 
			/* gotDbName = FALSE;   
##mdf100402
*/
		}
/* # line 1193 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1194 "implocks.sc" */	/* host code */
	}
	sprintf(userDbName,"%s::%s",currentVnode,e_dbname);
	if (connectedToDb == FALSE) {
/* # line 1198 "implocks.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(userSession);
    IIsqConnect(0,userDbName,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 1199 "implocks.sc" */	/* host code */
		if (sqlca.sqlcode != 0) {
			connectedToDb = FALSE;
			strcpy(reason,"Unavailable");
		} else {
			connectedToDb = TRUE; 
		}
	}
/* # line 1206 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 1207 "implocks.sc" */	/* host code */
	return(TRUE);
}
void getTableName()
{
		if (connectedToDb) {
/* # line 1213 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&userSession);
  }
/* # line 1214 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks8",7548,19676);
      IIputdomio((short *)0,1,30,4,&e_rsb_name2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implocks8",7548,19676);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select relid from iirelation where reltid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&e_rsb_name2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=0");
        IIexDefine(0,(char *)"implocks8",7548,19676);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,29,e_tablename);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1223 "implocks.sc" */	/* host code */
			sqlerr_check();
/* # line 1224 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1225 "implocks.sc" */	/* host code */
		} else {
			strcpy(e_tablename,reason);
		}
/* # line 1228 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 1229 "implocks.sc" */	/* host code */
}
displayResourceDetail(id_id)
  int id_id;
{
	/*
	** we should still be connected to the user db at this point
	*/
  char computed_type[SHORT_STRING];
  char computed_id[SHORT_STRING];
  char e_server[65];
  int e_scb_pid;
  char e_session_id[SESSION_ID_LENGTH];
  SessEntry *SessListHead;
  SessEntry *current_sess;
/* # line 1256 "implocks.sc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm((char *)"imp_res_locks",(char *)"r") == 0) goto IIfdE7;
    goto IImuI7;
IIfdI7:;
    if (IIfsetio((char *)0) == 0) goto IIfdE7;
/* # line 1259 "implocks.sc" */	/* host code */
		ShowResourceLocks(id_id);
IIfdB7:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1265 "implocks.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_res_locks",(char *)"res_lock_tb\
l",-2) != 0) {
            IItcogetio((short *)0,1,32,SESSION_ID_LENGTH-1,e_session_id,
            (char *)"session_id");
            IItcogetio((short *)0,1,30,4,&e_scb_pid,(char *)"scb_pid");
            IItcogetio((short *)0,1,32,SERVER_NAME_LENGTH-1,e_server,(char *)"\
server");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1271 "implocks.sc" */	/* host code */
                /* current_sess = SessListHead; */
                if (current_sess = newSessEntry()) {
                    strcpy(current_sess->sessionId,e_session_id );
                    current_sess->scb_pid = e_scb_pid;
                    }
                if (current_sess) {                                       
                        displaySessionDetail(current_sess,e_server);   
                }                                                         
      } else if (IIretval() == 2) {
	    ShowResourceLocks(id_id);
      } else if (IIretval() == 3) {
/* # line 1290 "implocks.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE7;
        }
      } else {
        if (1) goto IIfdE7;
      } /* IIretval */
    } /* IIrunform */
IIfdF7:
    if (IIchkfrm() == 0) goto IIfdB7;
    goto IIfdE7;
IImuI7:
    if (IIinitmu() == 0) goto IIfdE7;
    if (IInmuact((char *)"Session_Info",(char *)0,2,2,1) == 0) goto IIfdE7;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE7;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE7;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE7;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE7;
    if (IIendmu() == 0) goto IIfdE7;
    goto IIfdI7;
IIfdE7:;
    IIendfrm();
  }
/* # line 1293 "implocks.sc" */	/* host code */
}
ShowResourceLocks(id_id)
  int id_id;
{
  char computed_id[SHORT_STRING];
  char computed_id1[SHORT_STRING];
  char e_server[65];
  char e_session_id[SESSION_ID_LENGTH];
  int e_scb_pid;
/* # line 1308 "implocks.sc" */	/* clear */
  {
    IIclrflds();
  }
/* # line 1309 "implocks.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_res_locks",(char *)"res_lock_tbl",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1310 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks9",7549,16860);
      IIputdomio((short *)0,1,30,4,&id_id);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implocks9",7549,16860);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select rsb_id_id, rsb_grant_mode, rsb_convert_mode, rsb_name0, rsb_na\
me1, rsb_name2, rsb_name3, rsb_name4, rsb_name5, rsb_name6 from imp_lk\
mo_rsb where rsb_id_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implocks9",7549,16860);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&e_rsb_id_id);
      IIgetdomio((short *)0,1,32,4,e_rsb_grant);
      IIgetdomio((short *)0,1,32,4,e_rsb_convert);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name0);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name1);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name2);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name3);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name4);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name5);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name6);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1340 "implocks.sc" */	/* host code */
    if (sqlerr_check() == NO_ROWS) {
/* # line 1341 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1344 "implocks.sc" */	/* message */
  {
    IImessage((char *)"No information available..");
  }
/* # line 1345 "implocks.sc" */	/* host code */
        PCsleep(2*1000);
        } 
    else 
    {
/* # line 1349 "implocks.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1352 "implocks.sc" */	/* host code */
        sprintf(computed_id,"%lx",(65536 * e_rsb_id_id) 
                                   /* + e_rsb_id_instance */
                                         );
/* # line 1355 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_res_locks") != 0) {
      IIputfldio((char *)"resource_id",(short *)0,1,32,0,computed_id);
    } /* IIfsetio */
  }
/* # line 1356 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_res_locks") != 0) {
      IIputfldio((char *)"convert",(short *)0,1,32,0,e_rsb_convert);
    } /* IIfsetio */
  }
/* # line 1357 "implocks.sc" */	/* host code */
        decode_rsb0(e_rsb_name0,computed_id);
/* # line 1358 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_res_locks") != 0) {
      IIputfldio((char *)"resource_type",(short *)0,1,32,0,computed_id);
    } /* IIfsetio */
  }
/* # line 1359 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_res_locks") != 0) {
      IIputfldio((char *)"granted",(short *)0,1,32,0,e_rsb_grant);
    } /* IIfsetio */
  }
/* # line 1361 "implocks.sc" */	/* host code */
        /* 
        ** get the database, table and  page information
        */
        getDbName();
/* # line 1365 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_res_locks") != 0) {
      IIputfldio((char *)"dbname",(short *)0,1,32,0,e_dbname);
    } /* IIfsetio */
  }
/* # line 1367 "implocks.sc" */	/* host code */
        if (    (e_rsb_name0 != LK_DATABASE) &&
            (e_rsb_name0 != LK_SV_DATABASE) &&
            (e_rsb_name0 != LK_OPEN_DB) &&
            (e_rsb_name0 != LK_BM_DATABASE) ) {
            getTableName();
/* # line 1373 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_res_locks") != 0) {
      IIputfldio((char *)"table_name",(short *)0,1,32,0,e_tablename);
    } /* IIfsetio */
  }
/* # line 1375 "implocks.sc" */	/* host code */
        }
/* # line 1377 "implocks.sc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"imp_res_locks",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"page_number",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1378 "implocks.sc" */	/* host code */
        if (       (e_rsb_name0 == LK_PAGE) 
            || (e_rsb_name0 == LK_BM_PAGE)
            || (e_rsb_name0 == LK_ROW)
                ) {
/* # line 1382 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_res_locks") != 0) {
      IIputfldio((char *)"page_number",(short *)0,1,30,4,&e_rsb_name4);
    } /* IIfsetio */
  }
/* # line 1383 "implocks.sc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"imp_res_locks",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"page_number",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1384 "implocks.sc" */	/* host code */
        }
        /* 
##mdf040903 Show row number information 
*/
/* # line 1389 "implocks.sc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"imp_res_locks",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"row_number",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1390 "implocks.sc" */	/* host code */
        if    (e_rsb_name0 == LK_ROW) {
/* # line 1391 "implocks.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_res_locks") != 0) {
      IIputfldio((char *)"row_number",(short *)0,1,30,4,&e_rsb_name5);
    } /* IIfsetio */
  }
/* # line 1392 "implocks.sc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"imp_res_locks",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"row_number",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1393 "implocks.sc" */	/* host code */
        }
        /* 
        ** traverse the list of locks including this resource
        */
        sprintf(computed_id,"%s%%",currentVnode);
/* # line 1399 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks10",7550,13533);
      IIputdomio((short *)0,1,32,0,computed_id);
      IIputdomio((short *)0,1,30,4,&e_rsb_id_id);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"implocks10",7550,13533);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select server, session_id, scb_pid, lkb_id_id, llb_id_id, lkb_request\
_mode, lkb_state, s_name from imp_lkmo_lkb, imp_lkmo_llb, imp_scs_sess\
ions where lkb_llb_id_id=llb_id_id and varchar(llb_sid)=session_id and\
 server like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,computed_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and lkb_rsb_id_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&e_rsb_id_id);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and imp_lkmo_lkb.vnode=imp_lkmo_llb.vnode and imp_lkmo_lkb.vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implocks10",7550,13533);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,e_server);
      IIgetdomio((short *)0,1,32,SESSION_ID_LENGTH-1,e_session_id);
      IIgetdomio((short *)0,1,30,4,&e_scb_pid);
      IIgetdomio((short *)0,1,30,4,&e_lkb_id_id);
      IIgetdomio((short *)0,1,30,4,&e_llb_id_id);
      IIgetdomio((short *)0,1,32,4,e_lkb_request_mode);
      IIgetdomio((short *)0,1,32,29,e_lkb_state);
      IIgetdomio((short *)0,1,32,29,e_s_name);
      if (IIerrtest() != 0) goto IIrtB7;
/* # line 1450 "implocks.sc" */	/* host code */
            sqlerr_check();
            sprintf(computed_id,"%lx",
                    (65536 *e_lkb_id_id) 
                                              /* + e_lkb_id_instance */
                                              );
            sprintf(computed_id1,"%lx",
                    (65536 *e_llb_id_id) 
                                               /*+ e_llb_id_instance */
                                                );
/* # line 1459 "implocks.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_res_locks",(char *)"res_lock_tbl",1) != 0) {
          IItcoputio((char *)"resource_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"list_id",(short *)0,1,32,0,computed_id1);
          IItcoputio((char *)"session_name",(short *)0,1,32,0,e_s_name);
          IItcoputio((char *)"server",(short *)0,1,32,0,e_server);
          IItcoputio((char *)"scb_pid",(short *)0,1,30,4,&e_scb_pid);
          IItcoputio((char *)"session_id",(short *)0,1,32,0,e_session_id);
          IItcoputio((char *)"request_mode",(short *)0,1,32,0,
          e_lkb_request_mode);
          IItcoputio((char *)"grant_state",(short *)0,1,32,0,e_lkb_state);
          IITBceColEnd();
        } /* IItbact */
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
/* # line 1475 "implocks.sc" */	/* host code */
    }
}
/*
** findBlockingLocks()
** look for the lock that is blocking this list.
*/
findBlockingLocks(pid,session_id,llb_id_id)
  int pid;
  char *session_id;
  int llb_id_id;
{
  char buf[100];
/* # line 1494 "implocks.sc" */	/* message */
  {
    IImessage((char *)"Working...");
  }
/* # line 1496 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks11",7551,20707);
      IIputdomio((short *)0,1,30,4,&llb_id_id);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implocks11",7551,20707);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select llb_wait_id_id from imp_lkmo_llb where llb_id_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&llb_id_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implocks11",7551,20707);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&e_llb_wait_id_id);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1506 "implocks.sc" */	/* host code */
	sqlerr_check();
	if (e_llb_wait_id_id == 0) {
/* # line 1509 "implocks.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage((char *)"This locklist has no currently blocked locks");
  }
/* # line 1511 "implocks.sc" */	/* host code */
		return;
	}
	/* get the resource ID */
/* # line 1514 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks12",7553,5360);
      IIputdomio((short *)0,1,30,4,&e_llb_wait_id_id);
      IIputdomio((short *)0,1,32,0,currentVnode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"implocks12",7553,5360);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select lkb_rsb_id_id from imp_lkmo_lkb where lkb_id_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&e_llb_wait_id_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and vnode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,currentVnode);
        IIexDefine(0,(char *)"implocks12",7553,5360);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&e_lkb_rsb_id_id);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1524 "implocks.sc" */	/* host code */
	sqlerr_check();
	displayResourceDetail(e_lkb_rsb_id_id);
}
/*
** display resource list
*/
displayResourceList()
{
  char computed_type[SHORT_STRING];
  char computed_id[SHORT_STRING];
  char computed_id1[SHORT_STRING];
  int old_db_id = 0;
/* # line 1544 "implocks.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_res_list",(char *)"r") == 0) goto IIfdE8;
    goto IImuI8;
IIfdI8:;
    if (IIfsetio((char *)0) == 0) goto IIfdE8;
/* # line 1547 "implocks.sc" */	/* inittable */
    {
      if (IItbinit((char *)"imp_res_list",(char *)"lock_details",(char *)
"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 1548 "implocks.sc" */	/* host code */
		refreshResourceList();
IIfdB8:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1554 "implocks.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_res_list",(char *)"lock_details",-2) !=
           0) {
            IItcogetio((short *)0,1,30,4,&e_lkb_rsb_id_id,(char *)
"lkb_rsb_id_id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1556 "implocks.sc" */	/* host code */
		displayResourceDetail(e_lkb_rsb_id_id);
      } else if (IIretval() == 2) {
		refreshResourceList();
      } else if (IIretval() == 3) {
/* # line 1567 "implocks.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Resource_Info",(char *)0,2,2,1) == 0) goto IIfdE8;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,2) == 0) goto IIfdE8;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE8;
    if (IIendmu() == 0) goto IIfdE8;
    goto IIfdI8;
IIfdE8:;
    IIendfrm();
  }
/* # line 1570 "implocks.sc" */	/* host code */
	if (connectedToDb) {
/* # line 1571 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&userSession);
  }
/* # line 1572 "implocks.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 1573 "implocks.sc" */	/* host code */
		connectedToDb = FALSE;
	}
/* # line 1575 "implocks.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
  }
/* # line 1576 "implocks.sc" */	/* host code */
}
decode_rsb0(id,str)
int	id;
char	*str;
{
	switch(id) {
	case LK_DATABASE:	strcpy(str, LK_DATABASE_NAME ); break;
	case LK_TABLE:		strcpy(str, LK_TABLE_NAME ); break;
	case LK_PAGE:		strcpy(str, LK_PAGE_NAME ); break;
	case LK_EXTEND_FILE:	strcpy(str, LK_EXTEND_FILE_NAME ); break;
	case LK_BM_PAGE:	strcpy(str, LK_BM_PAGE_NAME ); break;
	case LK_CREATE_TABLE:	strcpy(str, LK_CREATE_TABLE_NAME ); break;
	case LK_OWNER_ID:	strcpy(str, LK_OWNER_ID_NAME ); break;
	case LK_CONFIG:		strcpy(str, LK_CONFIG_NAME ); break;
	case LK_DB_TEMP_ID:	strcpy(str, LK_DB_TEMP_ID_NAME ); break;
	case LK_SV_DATABASE:	strcpy(str, LK_SV_DATABASE_NAME ); break;
	case LK_SV_TABLE:	strcpy(str, LK_SV_TABLE_NAME ); break;
	case LK_SS_EVENT:	strcpy(str, LK_SS_EVENT_NAME ); break;
	case LK_TBL_CONTROL:	strcpy(str, LK_TBL_CONTROL_NAME ); break;
	case LK_JOURNAL:	strcpy(str, LK_JOURNAL_NAME ); break;
	case LK_OPEN_DB:	strcpy(str, LK_OPEN_DB_NAME ); break;
	case LK_CKP_DB:		strcpy(str, LK_CKP_DB_NAME ); break;
	case LK_CKP_CLUSTER:	strcpy(str, LK_CKP_CLUSTER_NAME ); break;
	case LK_BM_LOCK:	strcpy(str, LK_BM_LOCK_NAME ); break;
	case LK_BM_DATABASE:	strcpy(str, LK_BM_DATABASE_NAME ); break;
	case LK_BM_TABLE:	strcpy(str, LK_BM_TABLE_NAME ); break;
	case LK_CONTROL:	strcpy(str, LK_CONTROL_NAME ); break;
	case LK_EVCONNECT:	strcpy(str, LK_EVCONNECT_NAME ); break;
	case LK_AUDIT:		strcpy(str, LK_AUDIT_NAME ); break;
	case LK_ROW:		strcpy(str, LK_ROW_NAME ); break;
	default:		strcpy(str, "Unknown");
	}
}
refreshResourceList()
{
  char computed_type[SHORT_STRING];
  char computed_id[SHORT_STRING];
  char computed_id1[SHORT_STRING];
  int old_db_id = 0;
  int lk_row = LK_ROW;
  int lk_database = LK_DATABASE;
  int lk_table = LK_TABLE;
  int lk_page = LK_PAGE;
/* # line 1625 "implocks.sc" */	/* inittable */
  {
    if (IItbinit((char *)"imp_res_list",(char *)"lock_details",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1626 "implocks.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"implocks13",7554,5610);
      IIputdomio((short *)0,1,30,4,&lk_database);
      IIputdomio((short *)0,1,30,4,&lk_table);
      IIputdomio((short *)0,1,30,4,&lk_page);
      IIputdomio((short *)0,1,30,4,&lk_row);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"implocks13",7554,5610);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select rsb_id_id, rsb_grant_mode, rsb_convert_mode, rsb_name0, rsb_na\
me1, rsb_name2, rsb_name3, rsb_name4, rsb_name5, rsb_name6 from imp_lk\
mo_rsb where rsb_name0 in(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&lk_database);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&lk_table);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&lk_page);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&lk_row);
        IIwritio(0,(short *)0,1,32,0,(char *)
")order by rsb_name1, rsb_name0 asc, rsb_name2, rsb_name4, rsb_name5");
        IIexDefine(0,(char *)"implocks13",7554,5610);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&e_rsb_id_id);
      IIgetdomio((short *)0,1,32,4,e_rsb_grant);
      IIgetdomio((short *)0,1,32,4,e_rsb_convert);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name0);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name1);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name2);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name3);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name4);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name5);
      IIgetdomio((short *)0,1,30,4,&e_rsb_name6);
      if (IIerrtest() != 0) goto IIrtB8;
/* # line 1664 "implocks.sc" */	/* host code */
		sprintf(computed_id,"%lx",(65536 * e_rsb_id_id) 
                                   /* + e_rsb_id_instance */
                                         );
		if (e_rsb_name0 != old_db_id) {
			if (connectedToDb == TRUE) {
/* # line 1670 "implocks.sc" */	/* set_sql */
      {
        IILQssSetSqlio(11,(short *)0,1,30,4,&userSession);
      }
/* # line 1671 "implocks.sc" */	/* disconnect */
      {
        IIsqInit(&sqlca);
        IIsqDisconnect();
      }
/* # line 1672 "implocks.sc" */	/* host code */
			}
			connectedToDb = FALSE;
			gotDbName = FALSE;
/* # line 1675 "implocks.sc" */	/* set_sql */
      {
        IILQssSetSqlio(11,(short *)0,1,30,4,&imaSession);
      }
/* # line 1676 "implocks.sc" */	/* host code */
			old_db_id = e_rsb_name0;
		}
                getDbName();
		if (    (e_rsb_name0 != LK_DATABASE) &&
			(e_rsb_name0 != LK_SV_DATABASE) &&
			(e_rsb_name0 != LK_OPEN_DB) &&
			(e_rsb_name0 != LK_BM_DATABASE) ) {
			getTableName();
		}
		decode_rsb0(e_rsb_name0,computed_id1);
		if (e_rsb_name0 == LK_ROW)   /* 
##mdf040903
*/
                { 
/* # line 1692 "implocks.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_res_list",(char *)"lock_details",1) != 0) {
          IItcoputio((char *)"computed_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"lkb_rsb_id_id",(short *)0,1,30,4,&e_rsb_id_id);
          IItcoputio((char *)"rsb_cv_state",(short *)0,1,32,0,e_rsb_convert);
          IItcoputio((char *)"rsb_gr_state",(short *)0,1,32,0,e_rsb_grant);
          IItcoputio((char *)"rsb_type",(short *)0,1,32,0,computed_id1);
          IItcoputio((char *)"rsb_db",(short *)0,1,32,0,e_dbname);
          IItcoputio((char *)"rsb_table",(short *)0,1,32,0,e_tablename);
          IItcoputio((char *)"rsb_page",(short *)0,1,30,4,&e_rsb_name4);
          IItcoputio((char *)"rsb_row",(short *)0,1,30,4,&e_rsb_name5);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1706 "implocks.sc" */	/* host code */
		} 
                else {
		if (	(e_rsb_name0 == LK_PAGE) 
			|| (e_rsb_name0 == LK_BM_PAGE)
                    ) {
/* # line 1711 "implocks.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_res_list",(char *)"lock_details",1) != 0) {
          IItcoputio((char *)"computed_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"lkb_rsb_id_id",(short *)0,1,30,4,&e_rsb_id_id);
          IItcoputio((char *)"rsb_cv_state",(short *)0,1,32,0,e_rsb_convert);
          IItcoputio((char *)"rsb_gr_state",(short *)0,1,32,0,e_rsb_grant);
          IItcoputio((char *)"rsb_type",(short *)0,1,32,0,computed_id1);
          IItcoputio((char *)"rsb_db",(short *)0,1,32,0,e_dbname);
          IItcoputio((char *)"rsb_table",(short *)0,1,32,0,e_tablename);
          IItcoputio((char *)"rsb_page",(short *)0,1,30,4,&e_rsb_name4);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1723 "implocks.sc" */	/* host code */
		} else {
/* # line 1724 "implocks.sc" */	/* loadtable */
      {
        if (IItbact((char *)"imp_res_list",(char *)"lock_details",1) != 0) {
          IItcoputio((char *)"computed_id",(short *)0,1,32,0,computed_id);
          IItcoputio((char *)"lkb_rsb_id_id",(short *)0,1,30,4,&e_rsb_id_id);
          IItcoputio((char *)"rsb_gr_state",(short *)0,1,32,0,e_rsb_grant);
          IItcoputio((char *)"rsb_cv_state",(short *)0,1,32,0,e_rsb_convert);
          IItcoputio((char *)"rsb_type",(short *)0,1,32,0,computed_id1);
          IItcoputio((char *)"rsb_db",(short *)0,1,32,0,e_dbname);
          IItcoputio((char *)"rsb_table",(short *)0,1,32,0,e_tablename);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1735 "implocks.sc" */	/* host code */
		} }
		e_tablename[0] = '\0';
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE8:;
  }
		sqlerr_check();
}
