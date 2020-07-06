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
#include <cx.h>
#include <lg.h>
#include <cm.h>
#include <er.h>
#include "lists.h"
#include "dba.h"
#include "block.h"
/*
**
**  File: lklstdet.qc
**
**  Routines:
**	locklistdet() - displays the lock list detail form
**	printlockdet() - gets and formats lock list detail
**
**  Purpose - this file contains the routines to displays locks for all lock
**	lists or for a selected lock list.  This routine displays all
**	locks and is called from the lock list list display.
**
**  Inputs
**	sub_list - the desired sublist for a given main lock list element.  For
**		example, all the locks for the 2nd locklist row in the
**		tablefield of Lock List Information Display.
**
**  History
**	1/6/89		tomt		created
**	3/1/89		tomt		can only be called from displklists()
**					   if called to display a particular sublist
**	3/15/89		tomt		broke out get and format lkdet code
**	8/18/89		tomt		use INGDBA version of LK structs
**	9/24/89		tomt		call showblock with mode so showblock
**					knows how it was called.
**	3/21/91		tomt		changed title on form to trim.  Removed
**                              	variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>. Also, removed extra argument to STtrmwhile()
**			calls.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**      27-Dec-1994 (canor01)
**              Added function declaration for lock_itoa()
**      24-Apr-1995 (nick) Bug 67888
**              Added LK_CKP_TXN
**	21-aug-1995 (albany)  Bug #70787
**		printlockdet() now properly decodes the lkb_flags bitmap
**		rather than attempting to index off of it.
**	 1-nov-1995 (nick)
**		Change display method for LK_CONTROL. #71654
**		Add support for LK_AUDIT and LK_ROW. #72320
**	22-nov-1996 (dilma04)
**		Added LK_PH_PAGE and LK_VAL_LOCK for row level loking 
**		project.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	19-jul-2001 (devjo01)
**	    Modify for new LK_LKB_INFO struct. s103715.
*/
/*
** Forward and external Declares
*/
VOID close_db();
STATUS get_rel_name();
STATUS get_db_name();
VOID morelklock();
VOID open_db();
VOID printlockdet();
VOID showblock();
VOID string_move();
#ifdef VMS
    VOID showproc();
    VOID stat_decode();
#endif
FUNC_EXTERN VOID tablefield_find();
char *lock_itoa();
/*
**  Name: locklistdet()
**
**  Purpose - this routine displays locks for all lock lists
**	or for a selected lock list.  This routine displays all
**	locks and is called from the lock list list display.
**
**
**  Inputs
**	sub_list - the desired sublist for a given main lock list element.  For
**		example, all the locks for the 2nd resource row in the
**		tablefield of Lock List Information Display.
**
**  History
**	1/6/89		tomt		created
**	3/1/89		tomt		can only be called from displklists()
**					   if called to display a particular sublist
**	3/15/89		tomt		broke out get and format lkdet code
**	9/13/89		tomt		added ability to look at other locks
**					   on a specified resource
**	9/24/89		tomt		call showblock with mode so showblock
**					knows how it was called.
*/
VOID
locklistdet(sub_list)
i4  sub_list;
{
  char colname[FE_MAXNAME + 1], rsb_to_check[HEXID_SIZE + 1];
    i4	i_rsb;
    bool	e_xit = FALSE;
    /*
    **  Display the form and get the information
    */
/* # line 134 "lklstdet.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"fshowlklock",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 137 "lklstdet.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"fshowlklock",(char *)"lockdetail",(char *)"\
r") != 0) {
          IIthidecol((char *)"rsb",(char *)"c8");
          IIthidecol((char *)"lock_id",(char *)"c8");
          IIthidecol((char *)"value",(char *)"c16");
          IItfill();
        } /* IItbinit */
      }
/* # line 139 "lklstdet.qsc" */	/* host code */
	printlockdet(sub_list);	/* put lock list locks on form */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 144 "lklstdet.qsc" */	/* host code */
	morelklock();
        }
      } else if (IIretval() == 2) {
        {
/* # line 148 "lklstdet.qsc" */	/* message */
          {
            IImessage((char *)"Loading resource information...");
          }
/* # line 149 "lklstdet.qsc" */	/* getrow */
          {
            if (IItbsetio(2,(char *)"fshowlklock",(char *)"lockdetail",-2) != 
            0) {
              IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,rsb_to_check,
              (char *)"rsb");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 150 "lklstdet.qsc" */	/* host code */
	CVahxl(rsb_to_check, &i_rsb);	/* to int */
	showblock(i_rsb, SHOWRES);
        }
      } else if (IIretval() == 3) {
        {
/* # line 155 "lklstdet.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,(char *)"fshowlklock",(char *)"lockdetail",
            (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 156 "lklstdet.qsc" */	/* host code */
	tablefield_find();
/* # line 157 "lklstdet.qsc" */	/* resume */
          {
            IIrescol((char *)"lockdetail",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 161 "lklstdet.qsc" */	/* host code */
	my_help("ipmlkltd.hlp", "Lock Detail");
        }
      } else if (IIretval() == 5) {
        {
/* # line 165 "lklstdet.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 169 "lklstdet.qsc" */	/* host code */
	e_xit = TRUE;	/* user wants to leave */
/* # line 170 "lklstdet.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 171 "lklstdet.qsc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Resource_Info",(char *)0,2,2,2) == 0) goto IIfdE1;
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
/* # line 174 "lklstdet.qsc" */	/* host code */
    if (e_xit)  /* user wants to leave */
    {
	terminate(OK);
    }
    return;
}
/*
** morelklock() - This routine displays the popup form which show the
**	less needed lock detail info.
**
** History
**	3/2/89		tomt	written
**
*/
VOID
morelklock()
{
  char crsb[HEXID_SIZE + 1], clock_id[HEXID_SIZE + 1], cvalue[17];
/* # line 194 "lklstdet.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"fmorelklock",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 197 "lklstdet.qsc" */	/* host code */
	/*
	**   Get the values off of the tablefield on the base form and
	**   place them on the current form
	*/
/* # line 201 "lklstdet.qsc" */	/* getrow */
      {
        if (IItbsetio(2,(char *)"fshowlklock",(char *)"lockdetail",-2) != 0) 
        {
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,crsb,(char *)"rs\
b");
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,clock_id,(char *)
"lock_id");
          IItcogetio((short *)0,1,32,16,cvalue,(char *)"value");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 203 "lklstdet.qsc" */	/* putform */
      {
        if (IIfsetio((char *)"fmorelklock") != 0) {
          IIputfldio((char *)"rsb",(short *)0,1,32,0,crsb);
          IIputfldio((char *)"lock_id",(short *)0,1,32,0,clock_id);
          IIputfldio((char *)"value",(short *)0,1,32,0,cvalue);
        } /* IIfsetio */
      }
/* # line 205 "lklstdet.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 206 "lklstdet.qsc" */	/* submenu */
      {
        goto IImuI3;
IIfdB3:
        IIrunmu(0);
        if (IIretval() == 1) {
          {
/* # line 209 "lklstdet.qsc" */	/* breakdisplay */
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
/* # line 213 "lklstdet.qsc" */	/* host code */
    return;
}
/*
** Routine:
**	printlockdet() - gets and formats lock list detail
**
**	This routine gets and formats lock list locks
**
** Inputs:
**	sublist - which sublist to get.  0 means all sublists
**
** Outputs:
**	none
** Returns:
**	none
**
** History
**	3/15/89		tomt	written
**      4/24/95         nick    Added LK_CKP_TXN
**	21-aug-1995 (albany)  Bug #70787
**		printlockdet() now properly decodes the lkb_flags bitmap
**		rather than attempting to index off of it.
**	11/1/95		nick	Add support for LK_AUDIT and LK_ROW. #72320
**				Change handling of LK_CONTROL. #71654
**      22-nov-1996     dilma04 Added LK_PH_PAGE and LK_VAL_LOCK for row 
**                              level loking project.
**      16-oct-1998     stial01 Correctly print last element of Lock_type array
**      08-dec-1998     stial01 Increase size of key buffer
**      08-Aug-2000 (hanal04)
**              Made crtblname 17 elements long to resolve out of bounds
**              reference.
**	19-jul-2001 (devjo01)
**	    Modify for new LK_LKB_INFO struct. s103715.
**	05-Mar-2002 (jenjo02)
**	    Added LK_SEQUENCE
*/
VOID
printlockdet(sublist) 
i4	sublist;
{
  char id[HEXID_SIZE + 1], rsb[HEXID_SIZE + 1], lock_id[2 * (HEXID_SIZE + 2)];
  char state[51], tmpstate[51], flagstate[51];
  char lock_value[ 4 *(HEXID_SIZE + 2)], llb[HEXID_SIZE + 1], lock_tp[15];
  char key[200];
  char tempbuf[50];
  char tab_name[FE_MAXNAME + 1];
    char	dbbuf[FE_MAXNAME + 1]; /* for printing dbname */
  char *dbname = dbbuf;
    char	*dbn = dbbuf;
    char	olddbname[FE_MAXNAME + 1];
  char crtblname[17], crownname[9];
    bool	is_db_same = FALSE;	/* used to help formatting */
    bool	is_table_same = FALSE;  /* used to help formatting */
    i4		table_id = 0, table_idx = 0;
    i4		select_sub;
    bool	reset = TRUE;	/* start at beginning of list */
    i4		num_in_list;
    STATUS	status;
    LK_LKB_INFO *lkb;
  char str_rq[5];
  char str_gr[5];
    /*  Determine if caller wants a particular sublist  */
    if (sublist > 0)		/* yes, caller does */
    {
	select_sub = TRUE;
    }
    else 			/* no, caller wants all sublists */
    {
	sublist = 1;		/* start with first sublist */
	select_sub = FALSE;
	num_in_list = l_num_elements(LOCKINFO); /* how many elements to walk */
    }
    for (; ;) 	/* something to break out of */
    {
	while ((lkb = (LK_LKB_INFO *)l_retrieve(LOCKINFO,
	    sublist, &reset)) != NULL) 
	{
	    /*
	    **        do formatting for display
	    */
	    STprintf(id, "%x", lkb->lkb_id);
	    STprintf(rsb, "%x", lkb->lkb_rsb_id);
	    STprintf(llb, "%x", lkb->lkb_llb_id);
	    if ((i4)lkb->lkb_state < Lkstate_size)
		STcopy(Lkstate[(i4)lkb->lkb_state], state);
	    else
		STprintf(state, "(%d)", (i4)lkb->lkb_state);
	    if (lkb->lkb_flags & 1) {
		STprintf(tmpstate, "%s PHYS(%d) ", state, lkb->lkb_phys_cnt);
		STcopy(tmpstate, state);
	    }
	    if ( CXcluster_configured() )
	    {
		STprintf(lock_id, "%x.%x", 
		 lkb->lkb_dlm_lkid.lk_uhigh,  lkb->lkb_dlm_lkid.lk_ulow);
		STprintf(lock_value, "%08x %08x %08x %08x", 
			 lkb->lkb_dlm_lkvalue[0], lkb->lkb_dlm_lkvalue[1],
			 lkb->lkb_dlm_lkvalue[2], lkb->lkb_dlm_lkvalue[3]);
# ifdef VMS
		stat_decode( flagstate, Vmslkstat, (u_i4)lkb->lkb_flags );
		STprintf(tmpstate, "%s %s", state, flagstate );
		STcopy(tmpstate, state);
# endif /* VMS */
	    }
	    else
	    {
		STcopy("<n/a>", lock_value);
		STcopy("<n/a>", lock_id);
	    }
	    if ((i4)lkb->lkb_key[0] <= Lock_type_size)
		STcopy(Lock_type[(i4)lkb->lkb_key[0]], lock_tp);
	    else	/* new lock type! */ 
		STprintf(lock_tp, "<%d>", (i4)lkb->lkb_key[0]);
	    switch(lkb->lkb_key[0]) {
	    case LK_DATABASE:
	    case LK_SV_DATABASE:
	    case LK_OWNER_ID:
	    case LK_JOURNAL:
	    case LK_DB_TEMP_ID:
	    case LK_CONFIG:
	    case LK_OPEN_DB:
	    case LK_CKP_CLUSTER:
	    case LK_BM_DATABASE:
		if (lkb->lkb_key[0] == LK_BM_DATABASE)	/* dbname handling */
		    (VOID)get_db_name(dbname, lkb->lkb_key[1]);
		else
		    STcopy((char*) &lkb->lkb_key[1], dbname);
		if (!CMnmstart(dbn))	/* truncate any invalid non INGNAMEchar */
		{
		     *dbn = EOS;
		}
		else	/* step thru, insert NULL at first nonvalid char */
		{
		    for (CMnext(dbn); *dbn != EOS; CMnext(dbn))
		    {
			if (!CMnmchar(dbn))
			{
			    *dbn = EOS;
			    break;
			}
		    }
		}
		STprintf(key, "%-32s", dbname);
		break;
	    case LK_CONTROL:
		string_move(tempbuf, (PTR) &lkb->lkb_key[1], LK_KEY_LENGTH);
		tempbuf[LK_KEY_LENGTH] = EOS;   /* null terminate */
		STprintf(key, "SYS CONTROL: %s", tempbuf);
		break;
	    case LK_TABLE:
	    case LK_BM_TABLE:
	    case LK_SV_TABLE:
	    case LK_EXTEND_FILE:
	    case LK_TBL_CONTROL:
		/* 
		**  Open the next database, if necessary, because
		**  you are going to have to print out relation name.  
		*/
	        (VOID)get_db_name(dbname, lkb->lkb_key[1]);
	        if (STcompare(olddbname, dbname) != 0)	/* different */
		{
		    close_db();
		    open_db(dbname);			/* open a database */
		    STcopy(dbname, olddbname);
		    is_db_same = FALSE;
	        }
		else
		    is_db_same = TRUE;
	        if (!is_db_same || (table_id != lkb->lkb_key[2]) || 
		    (table_idx != lkb->lkb_key[3]))	/* diff. tbl name */
		{
		    status = get_rel_name(tab_name, lkb->lkb_key[2],
			lkb->lkb_key[3]);
		    STtrmwhite(dbname);
		    if (status == OK)			/* if real tbl name */
			STzapblank(tab_name, tab_name);	/* trim string */
		    else
			STtrmwhite(tab_name);
		    table_id = lkb->lkb_key[2];		/* for next time */
		    table_idx = lkb->lkb_key[3];
	        }
		if (is_db_same)
		{
		    STprintf(key, "   %-32s", tab_name);
		}
		else
		{
		    STprintf(key, "%s, TABLE: %s", dbname, tab_name);
		}
	        break;
	    case LK_PAGE:
	    case LK_BM_PAGE:
	    case LK_PH_PAGE:
		/* 
		** Open the next database, if necessary, because you are going to have
		** to print out relation name.  Try to be clever about formatting:
		** if dbname is same as previous lock then don't print it etc...
		*/
		(VOID)get_db_name(dbname, lkb->lkb_key[1]);
		if (STcompare(olddbname, dbname) != 0)	/* different */
		{
		    close_db();
		    open_db(dbname);			/* open a database */
		    STcopy(dbname, olddbname);
		    is_db_same = FALSE;
	        }
		else
		    is_db_same = TRUE;
	        if ((table_id != lkb->lkb_key[2]) || !is_db_same ||
		    (table_idx != lkb->lkb_key[3]))	/* diff tbl name */
		{
		    (VOID)get_rel_name(tab_name, lkb->lkb_key[2],
			lkb->lkb_key[3]);
		    table_id = lkb->lkb_key[2];		/* for next time */
		    table_idx = lkb->lkb_key[3];
		    is_table_same = FALSE;
	        }
		else
		{
		    is_table_same = TRUE;
		}
		if ((is_db_same) && (is_table_same))
		{
		    STprintf(key, "%40d", lkb->lkb_key[4]);
		}
		else if ((is_db_same) && !is_table_same)
		{
		    STprintf(key, "   %-30s %6d", tab_name, lkb->lkb_key[4]);
		}
		else
		{
		    STprintf(key, "%s, TABLE: %s, PAGE: %d",
		    dbname, tab_name, lkb->lkb_key[4]);
		}
	        break;
	    case LK_SS_EVENT:
	        STprintf(key, "SERVER: %x,[%x,%x]", lkb->lkb_key[1],
		    lkb->lkb_key[2], lkb->lkb_key[3]);
	        break;
	    case LK_EVCONNECT:
		STprintf(key, "Server PID = %d", lkb->lkb_key[1]);
		break;
	    case LK_CKP_TXN:
		if ((lkb->lkb_key[3] != NULL) && (lkb->lkb_key[1] != NULL))
		{
		    string_move(crtblname, (PTR) &lkb->lkb_key[3], 16);
		    string_move(crownname, (PTR) &lkb->lkb_key[1], 8);
		    crtblname[16] = crownname[8] = NULL;
		}
		else
		{
		    crtblname[0] = crownname[0] = NULL;
		}
		STprintf(tempbuf, "%s-%s", crtblname, crownname);
		STzapblank(tempbuf, tempbuf);
		STprintf(key, "   ('%s')", tempbuf);
	        break;
	    case LK_AUDIT:
		if (lkb->lkb_key[1] == SXAS_LOCK)
		{
		    if (lkb->lkb_key[2] == SXAS_STATE_LOCK)
			STprintf(key, "AUDIT: State(primary)");
		    else if (lkb->lkb_key[2] == SXAS_SAVE_LOCK)
			STprintf(key, "AUDIT: State(save)");
		    else
			STprintf(key, "AUDIT: State(oper=%d)", 
				lkb->lkb_key[2]);
		}
		else if (lkb->lkb_key[1] == SXAP_LOCK)
		{
		    if (lkb->lkb_key[2] == SXAP_SHMCTL)
			STprintf(key, 
			    "AUDIT: Physical_layer(shmctl, node=%x)",
			    lkb->lkb_key[3]);
		    else if (lkb->lkb_key[2] == SXAP_ACCESS)
			STprintf(key, "AUDIT: Physical_layer(access)");
		    else if (lkb->lkb_key[2] == SXAP_QUEUE)
			STprintf(key,
			    "AUDIT: Physical_layer(queue, node=%x)",
			    lkb->lkb_key[3]);
		    else
			STprintf(key, "AUDIT: Physical_layer(oper=%d)",
			    lkb->lkb_key[2]);
		}
		else
		{
		    STprintf(key, "AUDIT: TYPE=%d, OPER=%d", lkb->lkb_key[1],
			lkb->lkb_key[2]);
		}
		break;
	    case LK_ROW:
		(VOID)get_db_name(dbname, lkb->lkb_key[1]);
		if (STcompare(olddbname, dbname) != 0)
		{
		    close_db();
		    open_db(dbname);			/* open a database */
		    STcopy(dbname, olddbname);
		    is_db_same = FALSE;
	        }
		else
		{
		    is_db_same = TRUE;
		}
	        if ((table_id != lkb->lkb_key[2]) || 
			!is_db_same ||
		    	(table_idx != lkb->lkb_key[3]))	/* diff tbl name */
		{
		    (VOID)get_rel_name(tab_name, lkb->lkb_key[2],
			lkb->lkb_key[3]);
		    table_id = lkb->lkb_key[2];		/* for next time */
		    table_idx = lkb->lkb_key[3];
		    is_table_same = FALSE;
	        }
		else
		{
		    is_table_same = TRUE;
		}
		if ((is_db_same) && (is_table_same))
		{
		    STprintf(tempbuf, "%d[%d]", lkb->lkb_key[4], 
			lkb->lkb_key[5]);
		    STprintf(key, "%40s", tempbuf);
		}
		else if ((is_db_same) && !is_table_same)
		{
		    STprintf(tempbuf, "%d[%d]", lkb->lkb_key[4], 
			lkb->lkb_key[5]);
		    STprintf(key, "   %-24s %12s", tab_name, tempbuf);
		}
		else
		{
		    STprintf(key, "%s, TABLE: %s, PAGE: %d, ROW: %d",
		    dbname, tab_name, lkb->lkb_key[4], lkb->lkb_key[5]);
		}
		break;
	    case LK_CREATE_TABLE:
	    case LK_CKP_DB:
	    case LK_SEQUENCE:
		/* 
		** Open the next database, if necessary,
		** because you are going to have to print out relation name.  
		*/
	        (VOID)get_db_name(dbname, lkb->lkb_key[1]);
	        if (STcompare(olddbname, dbname) != 0) 
		{
		    close_db();
		    open_db(dbname);			/* open a database */
		    STcopy(dbname, olddbname);
		    is_db_same = FALSE;
	        }
		else
		    is_db_same = TRUE;
		/*
		** Extract the table and owner name from the lock struct
		** They are stored without null terminators, so we take care
		** of that. In some cases, the pointers are null, we check
		*/
		if ((lkb->lkb_key[4] != NULL) && (lkb->lkb_key[2] != NULL))
		{
		    string_move(crtblname, (PTR) &lkb->lkb_key[4], 12);
		    string_move(crownname, (PTR) &lkb->lkb_key[2], 8);
		    crtblname[12] = crownname[8] = NULL;
		}
		else
		{
		    crtblname[0] = crownname[0] = NULL;
		}
		STprintf(tempbuf, "%s-%s", crtblname, crownname);
		STzapblank(tempbuf, tempbuf);
		if (is_db_same)
		{
		    STprintf(key, "   ('%s')", tempbuf);
		}
		else
		{
		    STprintf(key, "%s, NAME: ('%s')", dbname, tempbuf);
		}
	        break;
	    case LK_BM_LOCK:
		string_move(tempbuf, (PTR) &lkb->lkb_key[1], 8);
		tempbuf[8] = NULL;
		STprintf(key, "BUFMGR = '%s'", tempbuf);
		break;
            case LK_VAL_LOCK:
                (VOID)get_db_name(dbname, lkb->lkb_key[1]);
                (VOID)get_rel_name(tab_name, lkb->lkb_key[2], lkb->lkb_key[3]);
                STprintf(key, "%s, %s, VALUE: <%d,%d,%d>", 
                    dbname, tab_name, lkb->lkb_key[4], lkb->lkb_key[5], 
                    lkb->lkb_key[6]);
                break;
	    default:
	        STprintf(key, "%d, <(%x,%x,%x,%x,%x,%x)>",
		    lkb->lkb_key[0],lkb->lkb_key[1],
                    lkb->lkb_key[2],lkb->lkb_key[3],
		    lkb->lkb_key[4],lkb->lkb_key[5],lkb->lkb_key[6]); 
	    }
	    STcopy(lock_itoa(lkb->lkb_request_mode), str_rq);
/*	    STcopy(lock_itoa(lkb->lkb_grant_mode), str_gr);*/
/* # line 648 "lklstdet.qsc" */	/* loadtable */
  {
    if (IItbact((char *)"fshowlklock",(char *)"lockdetail",1) != 0) {
      IItcoputio((char *)"id",(short *)0,1,32,0,id);
      IItcoputio((char *)"rsb",(short *)0,1,32,0,rsb);
      IItcoputio((char *)"request",(short *)0,1,32,0,str_rq);
      IItcoputio((char *)"state",(short *)0,1,32,0,state);
      IItcoputio((char *)"lock_id",(short *)0,1,32,0,lock_id);
      IItcoputio((char *)"value",(short *)0,1,32,0,lock_value);
      IItcoputio((char *)"key",(short *)0,1,32,0,key);
      IItcoputio((char *)"lock_type",(short *)0,1,32,0,lock_tp);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 653 "lklstdet.qsc" */	/* host code */
	}
	if (select_sub)		/* if user wants this particular sublist */
	{
	    break;		/* then we are done getting the data */
	} 
	else			/* else user wants all sublists */
	{
	    if (sublist >= num_in_list)		/* no more to get */
		break;
	    sublist++;				/* move to next sublist */
	}
    }
    return;
}
