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
#include <pc.h>
#include <dbms.h>
#include <fe.h>
#include <cs.h>
#include <lk.h>
#include <lg.h>
#include <er.h>
#include        <adf.h>
#include        <ft.h>
#include        <fmt.h>
#include        <frame.h>
#include "lists.h"
#include "msgs.h" 
#include "dba.h"
#include "block.h"
FUNC_EXTERN VOID set_options();
/*
**  File: dsplklst.qc
**
**  Purpose - this file contains the routines which display the lock
**	list information form.  
**
**  This file contains:
**	displklists() - displays a list of lock list form. 
**	printlklists() - gets, formats and outputs lock list information.
**	getlk_trid() - gets the xaction id given an internal locklist id
**
**  History
**	2/21/89		tomt	created
**	3/15/89		tomt	modularized lock formatting code
**				    -its in a seperate routine.
**	4/13/89		tomt	made form a popup
**	7/7/89		tomt	uses CL routines instead of strcpy etc..
**	8/16/89		tomt	uses INGDBA version of LK structs.
**				These are only used when getting info
**				from l_retrieve!
**	9/7/89		tomt	added getlk_trid - returns xaction id given
**				    an internal lock list id.
**	9/7/89		tomt	now display session name instead of lock list
**				id.  id is now displayed on more info form
**	9/13/89		tomt	added block info functionality
**	9/19/89		tomt	moved ##ADDFORMS here and made form names
**				    readonly c vars
**	9/24/89		tomt	now call showblock() in SHOWBLOCK mode
**	10/1/89		tomt	integrated 6.3 changes.  Don't need to
**				call get_session_id, now call get_sessname.
**				Also, the locklist status type constants
**				are now a PTR defined in <locks.h>
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**	2-Feb-1994 (fredv)
**		Moved <lk.h> after <cs.h> so that CS_SID will be defined.
**	 1-Nov-1995 (nick)
**		Remve unused variable.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
** forward declarations
*/
VOID printlklists();
VOID get_lklgstats();
VOID morelklists();
VOID locklistdet();
#ifdef VMS
    VOID showproc();
#endif 
VOID getlk_trid();
VOID get_sessname();
VOID stat_decode();
FUNC_EXTERN VOID tablefield_find();
VOID showblock();
/*
** Declares
*/
  static char Fdisplklists[]= "fdisplklists";
  static char Fmorelklists[]= "fmorelklists";
  static char Fingdba[]= "fingdba";
/*
** Routine: displklists()  displays a list of locklists on a popup form
**
** Inputs: none
**
** Outputs: displays a form
**
** Side effects: assumes the forms system is active
**
** History
**	1/89		tomt	written
**	9/13/89		tomt	added block info functionality
**	9/19/89		tomt	moved ##ADDFORMS here and made form names
**				    readonly c vars
**	9/24/89		tomt	now call showblock() in SHOWBLOCK mode
**	11/1/95		nick	remove flag_file
**	12/05/1996	nanpr01 bug 77771 : takes out of lock screen.
*/
VOID
displklists()
{
    bool	e_xit = FALSE;
  int list_position, list_count, lock_count;
    i4	iwaiting_rsb;
  char colname[FE_MAXNAME + 1], dbnam[FE_MAXNAME + 1];
  char waiting_rsb[HEXID_SIZE + 1];
    static bool	forms_added = FALSE;
  GLOBALREF int *fdisplklists;
  GLOBALREF int *fshowlklock;
  GLOBALREF int *fmorelklists;
  GLOBALREF int *fmorelklock;
  GLOBALREF int *fshowblock;
    /*
    **  Determine if forms are added, if not addform them.  Also
    **  don't add forms if output is being directed to a file
    */
    if (!forms_added)
    {
/* # line 143 "dsplklst.qsc" */	/* addform */
  {
    IIaddform(fshowblock);
  }
/* # line 144 "dsplklst.qsc" */	/* addform */
  {
    IIaddform(fdisplklists);
  }
/* # line 145 "dsplklst.qsc" */	/* addform */
  {
    IIaddform(fshowlklock);
  }
/* # line 146 "dsplklst.qsc" */	/* addform */
  {
    IIaddform(fmorelklists);
  }
/* # line 147 "dsplklst.qsc" */	/* addform */
  {
    IIaddform(fmorelklock);
  }
/* # line 148 "dsplklst.qsc" */	/* host code */
	forms_added = TRUE;
    }
    /* Display the form and get the information */
/* # line 152 "dsplklst.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fdisplklists,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 156 "dsplklst.qsc" */	/* host code */
        /* Put the info in the tablefield */
/* # line 157 "dsplklst.qsc" */	/* inittable */
      {
        if (IItbinit(Fdisplklists,(char *)"locklists",(char *)"r") != 0) {
          IIthidecol((char *)"row_num",(char *)"i2");
          IIthidecol((char *)"rllb",(char *)"c8");
          IIthidecol((char *)"rcnt",(char *)"i4");
          IIthidecol((char *)"wait",(char *)"c8");
          IIthidecol((char *)"key",(char *)"c16");
          IItfill();
        } /* IItbinit */
      }
/* # line 159 "dsplklst.qsc" */	/* host code */
	printlklists();	/* put info in tablefield */
        /* get number of lock lists */
/* # line 162 "dsplklst.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,Fdisplklists,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)"locklists",0);
        } /* IIiqset */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 166 "dsplklst.qsc" */	/* host code */
        /* Get lock resource info from INGRES */
/* # line 167 "dsplklst.qsc" */	/* message */
          {
            IImessage((char *)"Loading lock information...");
          }
/* # line 168 "dsplklst.qsc" */	/* host code */
        get_lklgstats(GET_LOCKRES, (PTR) 0);
        get_lklgstats(GET_LOCKLIST, (PTR) 0);
        /* Put the info in the tablefield */
/* # line 172 "dsplklst.qsc" */	/* clear */
          {
            IIfldclear((char *)"locklists");
          }
/* # line 173 "dsplklst.qsc" */	/* host code */
	printlklists();	/* put info in tablefield */
        /* get number of lock lists */
/* # line 176 "dsplklst.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Fdisplklists,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)"lockl\
ists",0);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 180 "dsplklst.qsc" */	/* host code */
	if (list_count == 0) {
	    POP_MSG("There are no lock lists to get information on");
/* # line 182 "dsplklst.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 183 "dsplklst.qsc" */	/* host code */
	}
	morelklists();
        }
      } else if (IIretval() == 3) {
        {
/* # line 188 "dsplklst.qsc" */	/* host code */
	if (list_count == 0)
	{
	    POP_MSG("There are no lock lists to examine");
/* # line 191 "dsplklst.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 192 "dsplklst.qsc" */	/* host code */
	}
/* # line 193 "dsplklst.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdisplklists,(char *)"locklists",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&list_position,(char *)"row\
_num");
              IItcogetio((short *)0,1,30,4,&lock_count,(char *)
"llb_lkb_count");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 195 "dsplklst.qsc" */	/* message */
          {
            IImessage((char *)"Loading lock information...");
          }
/* # line 196 "dsplklst.qsc" */	/* host code */
	locklistdet(list_position);
        }
      } else if (IIretval() == 4) {
        {
/* # line 200 "dsplklst.qsc" */	/* host code */
	if (list_count == 0)
	{
	    POP_MSG("There are no lock lists to get block information on");
/* # line 203 "dsplklst.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 204 "dsplklst.qsc" */	/* host code */
	}
/* # line 205 "dsplklst.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdisplklists,(char *)"locklists",-2) != 0) {
              IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,waiting_rsb,(char *)"\
wait");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 206 "dsplklst.qsc" */	/* host code */
	CVahxl(waiting_rsb, &iwaiting_rsb);	/* to int */
	/* If this lock list has no blocked locks, don't bother looking. */
	if (iwaiting_rsb == 0)
	{
	    if (flag_cluster)	/* if on a VAXcluster print a different msg */
	    {
		POP_MSG("Either this locklist either has no locks\
 that are currently blocked, or the blocking lock\
 may be on another node of the VAXcluster");
	    }
	    else	/* print regular message */
	    {
		POP_MSG("This locklist has no currently blocked locks");
	    }
/* # line 221 "dsplklst.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 222 "dsplklst.qsc" */	/* host code */
	}
/* # line 223 "dsplklst.qsc" */	/* message */
          {
            IImessage((char *)"Finding blocking locks...");
          }
/* # line 224 "dsplklst.qsc" */	/* host code */
	showblock(iwaiting_rsb, SHOWBLOCK); /* go check for blocking lock */
        }
      } else if (IIretval() == 5) {
        {
/* # line 228 "dsplklst.qsc" */	/* host code */
	set_options();
        }
      } else if (IIretval() == 6) {
        {
/* # line 232 "dsplklst.qsc" */	/* host code */
	if (list_count == 0)
	{
	    POP_MSG("There is nothing to find");
/* # line 235 "dsplklst.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 236 "dsplklst.qsc" */	/* host code */
	}
/* # line 237 "dsplklst.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,Fdisplklists,(char *)"locklists",(char *)0) != 0) 
            {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 238 "dsplklst.qsc" */	/* host code */
	tablefield_find();
/* # line 239 "dsplklst.qsc" */	/* resume */
          {
            IIrescol((char *)"locklists",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 243 "dsplklst.qsc" */	/* host code */
	my_help("ipmdplkl.hlp", "Locks by Lock List");
        }
      } else if (IIretval() == 8) {
        {
/* # line 247 "dsplklst.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 251 "dsplklst.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
/* # line 252 "dsplklst.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 253 "dsplklst.qsc" */	/* breakdisplay */
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
    if (IInmuact((char *)"More_Info",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Examine",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Block_Info",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Options",(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"Find",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,8) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,8) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,9) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 256 "dsplklst.qsc" */	/* host code */
    if (e_xit)			/* user wants to leave */
    {
	terminate(OK);
    }
    return;
}
/*
** morelklists() - This routine displays the popup form which show the
**	less needed resource info.
**
** History
**	3/2/89		tomt	written
**
*/
VOID
morelklists()
{
/*
** Declare Variables
*/
  char c_rllb[HEXID_SIZE + 1], c_wait[HEXID_SIZE + 1], c_rllb_key[17];
  int ircnt;
/* # line 280 "dsplklst.qsc" */	/* display */
  {
    if (IIdispfrm(Fmorelklists,(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 283 "dsplklst.qsc" */	/* host code */
        /*
        **   Get the values off of the tablefield on the base form and
        **   place them on the current form
        */
/* # line 287 "dsplklst.qsc" */	/* getrow */
      {
        if (IItbsetio(2,Fdisplklists,(char *)"locklists",-2) != 0) {
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,c_rllb,(char *)"\
rllb");
          IItcogetio((short *)0,1,30,4,&ircnt,(char *)"rcnt");
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,c_wait,(char *)"\
wait");
          IItcogetio((short *)0,1,32,16,c_rllb_key,(char *)"key");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 289 "dsplklst.qsc" */	/* putform */
      {
        if (IIfsetio(Fmorelklists) != 0) {
          IIputfldio((char *)"rllb",(short *)0,1,32,0,c_rllb);
          IIputfldio((char *)"rcnt",(short *)0,1,30,4,&ircnt);
          IIputfldio((char *)"wait",(short *)0,1,32,0,c_wait);
          IIputfldio((char *)"key",(short *)0,1,32,0,c_rllb_key);
        } /* IIfsetio */
      }
/* # line 291 "dsplklst.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 292 "dsplklst.qsc" */	/* submenu */
      {
        goto IImuI3;
IIfdB3:
        IIrunmu(0);
        if (IIretval() == 1) {
          {
/* # line 295 "dsplklst.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else {
          if (1) goto IIfdE3;
        } /* IIretval */
        goto IIfdE3;
IImuI3:
        IImuonly();
        if (IIinitmu() == 0) goto IIfdE3;
        if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE3;
        IIFRInternal(0);
        if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE3;
        if (IIendmu() == 0) goto IIfdE3;
        goto IIfdB3;
IIfdE3:;
      }
    }
IIfdB2:
    while (IIrunform() != 0) {
    } /* IIrunform */
    if (IIFRitIsTimeout() != 0) goto IIfdE2;
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 299 "dsplklst.qsc" */	/* host code */
    return;
}
/*
** printlklists() - this routine gets, formats and outputs the lock list
**	information.
**
** Inputs:
**	none
**
** Outputs:
**	formatted lock list information to screen or file
**
** Returns:
**	none
**
** History:
**	3/15/89		tomt	written
**	3/17/89		tomt	added filter to suppress display of lock lists
**				with no locks.
**	9/7/89		tomt	added display of session name
**	10/1/89		tomt	integrated 6.3 changes.  Don't need to
**				call get_session_id, now call get_sessname.
**				Also, the locklist status type constants
**				are now a PTR defined in <locks.h>
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
*/
VOID
printlklists()
{
    bool	refresh = TRUE; /* tell get_sessname to
				** refresh before giving up search.
				** It is toggled off after first
				** refresh to avoid unneccesary overhead
				*/
    bool	reset = TRUE;	/* start at beginning of lk list */
  int counter;
  char cllb_id[HEXID_SIZE + 1], cllb_key[17], session_name[FE_MAXNAME + 1], 
  crllb[HEXID_SIZE + 1], cwait[HEXID_SIZE + 1], cstat[101];
  i4 l_r_cnt;
  i4 l_lkb_count;
  i4 l_llkb_count;
  i4 l_max_lkb;
    LK_LLB_INFO *llb;
    counter = 0;
    while ((llb = (LK_LLB_INFO *)l_retrieve(LOCKINFO, 0, &reset)) != NULL)
    {
        /*
        **    do formatting for display
        */
	counter++;	/* keep track of element number */
        /*
        **    Filter for suppression of nonprotected (i.e. system) locklists.
        **    If the flag is set, system lock lists will be displayed
        */
	if (!flag_nonprot_lklists && (llb->llb_status & LLB_NONPROTECT))
	    continue;
	STprintf(cllb_id, "%x", llb->llb_id);
	STprintf(cllb_key, "%x%08x", llb->llb_key[0], llb->llb_key[1]);
	STprintf(crllb, "%x", llb->llb_r_llb_id);
	STprintf(cwait, "%x", llb->llb_wait_rsb_id);
	if (flag_cluster) 		/* on a VAXcluster */
	{
	    stat_decode(cstat, Lklststcl, (u_i4) llb->llb_status);
	}
	else
	{
	    stat_decode(cstat, Lklstst, (u_i4) llb->llb_status); 
	}
	get_sessname((u_i4)llb->llb_key[0],
	    (u_i4)llb->llb_key[1], session_name, &refresh);
	if (llb->llb_status == 0)	/* just in case, make it clear on scr */
	    STcopy("<none>", cstat);
	l_r_cnt = llb->llb_r_cnt;
	l_lkb_count = llb->llb_lkb_count;
	l_llkb_count = llb->llb_llkb_count;
	l_max_lkb = llb->llb_max_lkb;
/* # line 386 "dsplklst.qsc" */	/* loadtable */
  {
    if (IItbact(Fdisplklists,(char *)"locklists",1) != 0) {
      IItcoputio((char *)"id",(short *)0,1,32,0,cllb_id);
      IItcoputio((char *)"key",(short *)0,1,32,0,cllb_key);
      IItcoputio((char *)"rllb",(short *)0,1,32,0,crllb);
      IItcoputio((char *)"rcnt",(short *)0,1,30,4,&l_r_cnt);
      IItcoputio((char *)"wait",(short *)0,1,32,0,cwait);
      IItcoputio((char *)"session_name",(short *)0,1,32,0,session_name);
      IItcoputio((char *)"liststat",(short *)0,1,32,0,cstat);
      IItcoputio((char *)"row_num",(short *)0,1,30,4,&counter);
      IItcoputio((char *)"llb_lkb_count",(short *)0,1,30,4,&l_lkb_count);
      IItcoputio((char *)"llb_llkb_count",(short *)0,1,30,4,&l_llkb_count);
      IItcoputio((char *)"llb_max_lkb",(short *)0,1,30,4,&l_max_lkb);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 393 "dsplklst.qsc" */	/* host code */
    }
    return;
}
/*
** get_lktrid() - this routine gets the transaction id given a locklist id.
**
** Inputs:
**	high_trid - a pointer to where the high order xaction id is placed
**	low_trid - a pointer to where the low order xaction id is placed
**	lklist_id - the lock list ID to get the transaction ID for.
**
** Outputs:
**	the high and low order parts of the xaction id will be placed
**	in the xaction id buffer.
**
** Returns:
**	none
**
** History:
**	9/7/89		tomt	written
**	9/8/89		tomt	fixed routine to automatically refresh the
**				lock list if it is empty
*/
VOID
getlk_trid(high_trid, low_trid, lklist_id)
u_i4 *high_trid;
u_i4 *low_trid;
i4  lklist_id;
{
    bool	looped_once = FALSE;	/* indicates if first time thru for */
    bool	reset = TRUE;	/* start at beginning of linked list */
    LK_LLB_INFO *llb;
    /*
    ** Check that a pointer was passed
    */
    if ((high_trid == NULL) || (low_trid == NULL))	/* bad ptrs */
	return;
    *high_trid = 0;	/* zero tells caller none found */
    *low_trid = 0;
    for (llb = (LK_LLB_INFO *)l_retrieve(LOCKINFO, 0, &reset);
	;
	llb = (LK_LLB_INFO *)l_retrieve(LOCKINFO, 0, &reset))
    {
        if (!looped_once && (llb == NULL)) /* GET LOCKLIST */
	{
	    get_lklgstats(GET_LOCKLIST, (PTR) 0);
	    reset = TRUE;
	    looped_once = TRUE;
	    continue;		/* start over again */
	}
	else if (llb == NULL)	/* no more info, get out */
	{
	    break;
	}
	else
	{
	    looped_once = TRUE;	/* been through once - set */
	}
	/*
	**    find the requested xaction ids
	*/
	if (lklist_id == llb->llb_id)	/* are the internal lklist ids =? */
	{
	    *high_trid = llb->llb_key[0];
	    *low_trid = llb->llb_key[1];
	    break;
	}
    }
    return;
}
