# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** Includes and defines
*/
#include <compat.h>
#include <cs.h>
#include <st.h>
#include <dbms.h>
#include <fe.h>
#include <pc.h>
#include <lk.h>
#include <lg.h>
#include <er.h>
#include "lists.h"
#include "msgs.h"
#include "dba.h"
/*
**
**  File: lockresdet.qc
**
**  This file contains the routines to display locks for 
**	a selected resource list. 
**
**
**  Inputs
**	sub_list - the desired sublist for a given main list element.  For
**		example, all the locks for the 2nd resource row in the
**		tablefield of Resource Information Display.
**
**  History
**	1/6/89		tomt		created
**	4/11/89		tomt		positioned form at col 32
**	8/18/89		tomt		use INGDBA version of lk structs
**	9/8/89		tomt		added session_name field to form
**	9/13/89		tomt		now printlkres takes a formname
**					as a parameter as it is called
**					from various places
**	3/21/91		tomt		changed title on form to trim.  Removed
**                              	variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**      27-dec-1994 (canor01)
**              Added declaration for lock_itoa()
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
** Constants
*/
#define POPUP_R_SIZE 14
VOID lockresdet();
VOID printresdet();
VOID tablefield_find();
VOID displkres();
VOID getlk_trid();
VOID get_sessname();
char *lock_itoa();
/*
**
**  Routine: lockresdet()
**
**  Purpose - this routine displays locks for all lock lists
**	or for a selected lock list.  Called from displkres()
**
**
**  Inputs
**	sub_list - the desired sublist for a given main list element.  For
**		example, all the locks for the 2nd resource row in the
**		tablefield of Resource Information Display.
**
**  History
**	1/6/89		tomt	created
**	4/11/89		tomt	positioned form at col 32
**	9/13/89		tomt	now pass form name to printresdet
*/
VOID
lockresdet(sub_list)
i4  sub_list;
{
    bool 	e_xit = FALSE;
  int begin_row, p_numrows, currow, screen_rows;
  char colname[FE_MAXNAME + 1];
    char	form_name[FE_MAXNAME + 1];
    /*
    **  Init local variables
    */
    STcopy("fshowlkres", form_name);
    /*
    **  Calculate starting row of popup
    */
    p_numrows = POPUP_R_SIZE;
/* # line 110 "lkresdet.qsc" */	/* inquire_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&currow,48,(char *)0,0);
    } /* IIiqset */
  }
/* # line 111 "lkresdet.qsc" */	/* inquire_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&screen_rows,44,(char *)0,0);
    } /* IIiqset */
  }
/* # line 112 "lkresdet.qsc" */	/* host code */
    if ((p_numrows + currow) > screen_rows)
    {
	begin_row = currow - p_numrows;
    }
    else if ((currow - p_numrows) < 1)
    {
	begin_row = currow + 1;
    }
    else
    {
	begin_row = 0;	/* let the FRS decide */
    }
    /*
    **  Display the form and get the information
    */
/* # line 129 "lkresdet.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(19),(void *)IILQint(19));
    IIFRgpsetio(1,(short *)0,1,30,4,&begin_row);
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpcontrol(2,0);
    if (IIdispfrm((char *)"fshowlkres",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 133 "lkresdet.qsc" */	/* host code */
   	/* Put information on the form */
/* # line 134 "lkresdet.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"fshowlkres",(char *)"lockdetail",(char *)"\
r") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 135 "lkresdet.qsc" */	/* host code */
	/* get lock info and put on screen */
	printresdet(sub_list, form_name);
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 140 "lkresdet.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,(char *)"fshowlkres",(char *)"lockdetail",
            (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 141 "lkresdet.qsc" */	/* host code */
	tablefield_find();
/* # line 142 "lkresdet.qsc" */	/* resume */
          {
            IIrescol((char *)"lockdetail",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 146 "lkresdet.qsc" */	/* host code */
	my_help("ipmlkrdt.hlp", "Lock Detail");
        }
      } else if (IIretval() == 3) {
        {
/* # line 150 "lkresdet.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 154 "lkresdet.qsc" */	/* host code */
	e_xit = TRUE;	/* user wants to leave */
/* # line 155 "lkresdet.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 156 "lkresdet.qsc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Find",(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,1) == 0) goto IIfdE1;
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
/* # line 159 "lkresdet.qsc" */	/* host code */
    if (e_xit) {	/* user wants to leave */
	terminate(OK);
    }
    return;
}
/*
** printresdet() - this routine gets, formats and outputs the locks in a
**	resource list.
**
** Inputs:
**	listnum - which resource element to get locks from
**
** Outputs:
**	writes formatted lock info to screen or file
**
** Returns:
**	none
**
** History
**	3/15/89		tomt	written
**	9/8/89		tomt	added session_name to form
**	9/13/89		tomt	now called from various places, so now we pass
**				in form name...
*/
VOID
printresdet(sublist, form_name)
i4  sublist;
PTR form_name;
{
    u_i4	txid_low, txid_high;
  char id[HEXID_SIZE + 1], state[51], formname[FE_MAXNAME + 1];
    char	tmpstate[51];
  char llb[HEXID_SIZE + 1], session_name[FE_MAXNAME + 1];
    i4		lk_state_i;
    bool	select_sub;
    i4		num_in_list;
    bool	reset = TRUE;	/* start at beginning of list */
    bool	refresh = TRUE;	/* tell get_sessname to refresh if
				** search was unsuccessful.  get_sessname
				** will toggle this off after the first
				** search to eliminate unneccesary refreshes
				*/
    LK_LKB_INFO *lkb;
  char str_rq[5];
  char str_gr[5];
    /* copy form name to local buffer because PTR type not known by eqc */
    STcopy(form_name, formname);
    if (sublist > 0) 
    {		
	/* caller wants a particular sublist */
	select_sub = TRUE;
    } 
    else 
    {			
	/* caller wants all sublists */
	sublist = 1;
	select_sub = FALSE;	/* start with first sublist */
	num_in_list = l_num_elements(RESINFO); /* how many elements to walk */
    }
    for ( ; ; )			/* perhaps sublist++ to control loop?? */
    {
	while ((lkb = (LK_LKB_INFO *)l_retrieve(RESINFO,
	    sublist, &reset)) != NULL) 
	{
	    /* format for display on form */
	    STprintf(id, "%x", lkb->lkb_id);
	    STprintf(llb, "%x", lkb->lkb_llb_id);
	    /*
	    ** to get session name, first get the xaction id that this lock
	    ** belongs to.  That is used to find the session name.
	    ** The refresh flag will be toggled off after first refresh to 
	    ** eliminate
	    ** overhead of useless succeeding refreshes because no session name
	    ** could be found
	    */
	    getlk_trid(&txid_high, &txid_low, lkb->lkb_llb_id); 
	    get_sessname(txid_high, txid_low, session_name, &refresh);
	    lk_state_i = (i4)lkb->lkb_state;	/* cast for following hack */
	    /*
	    **  Old check for subscript out of bounds.  A fix was made to 
	    **  eliminate the problem.  However, the code was left in as 
	    **  an error checking measure.
	    **
	    **  Problem found (9/24/89) but leaving in for a while until we 
	    **  are sure.
	    */
	    /* 4 and 6 are array size bounds -- jpk */
/*	    if (lk_state_i >= 4)*/
/*	       || lkb->lkb_request_mode >= 6 || lkb->lkb_grant_mode >= 6)*/
/*	    {*/
/*		POP_MSG("Attempt to display a lock with bad values.");*/
/*		break;*/
/*	    }*/
	    if (lk_state_i < Lkstate_size)
		STcopy(Lkstate[lk_state_i], state);
	    else
		STprintf(state, "(%d)", lk_state_i);
	    if (lkb->lkb_flags & 1)
	    {
		STprintf(tmpstate, "%s PHYS(%d) ", state, lkb->lkb_phys_cnt);
		STcopy(tmpstate, state);
	    }
	    STcopy(lock_itoa(lkb->lkb_request_mode), str_rq);
/*	    STcopy(lock_itoa(lkb->lkb_grant_mode), str_gr);*/
/* # line 273 "lkresdet.qsc" */	/* loadtable */
  {
    if (IItbact(formname,(char *)"lockdetail",1) != 0) {
      IItcoputio((char *)"id",(short *)0,1,32,0,id);
      IItcoputio((char *)"llb",(short *)0,1,32,0,llb);
      IItcoputio((char *)"request",(short *)0,1,32,0,str_rq);
      IItcoputio((char *)"state",(short *)0,1,32,0,state);
      IItcoputio((char *)"session_name",(short *)0,1,32,0,session_name);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 276 "lkresdet.qsc" */	/* host code */
	}
	if (select_sub) 
	{ 
	    /* user wants this particular sub_list */
	    break;	    /* we are done getting the data */
	} 
	else 
	{ 
	    /* user wants all sublists */
	    if (sublist >= num_in_list) /* no more to get */
		break;
	    sublist++;	/* move to next sublist */
	}
    }
    return;
}
