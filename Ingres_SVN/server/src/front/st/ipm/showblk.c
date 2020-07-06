# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
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
#include "block.h"
/*
** Forward and External References
*/
STATUS get_db_name();
STATUS get_rel_name();
VOID showblock();
i4  blklkwork();
VOID printresdet();
VOID close_db();
FUNC_EXTERN VOID open_db();
FUNC_EXTERN VOID tablefield_find();
char	*lock_itoa();
/*
** Constants
*/
#define POPUP_R_SIZE 15
/*
** showblock.qc - this file contains the routines necessary
**	to find blocking/blocked locks and display them for the
**	user to see.
**
** This file contains:
**	showblock - displays the blocked/blocking lock form.
**	blklkwork - finds the blocked/blocking locks
**
** History
**	4/18/89		tomt		written
**	9/13/89		tomt		added necessary code to display
**					    only blocking locks
**	9/24/89		tomt		now pass mode in which showblock
**					is called.  Determines which title
**					is displayed
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**      24-Apr-1995 (nick) Bug #67888
**              Added LK_CKP_TXN and LK_EVCONNECT
**	 1-Nov-1995 (nick)
**		Change display method for LK_CONTROL.
**		Added LK_ROW.
**		Added LK_AUDIT.
**      22-nov-1996 (dilma04)
**              Row-level locking project:
**                - add LK_PH_PAGE;
**	          - add LK_VAL_LOCK;
**		  - display row and value locks by default.
**      16-oct-1998 (stial01)
**              Correctly print last element of Lock_type array
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	05-Mar-2002 (jenjo02)
**	    Added LK_SEQUENCE
*/
/*
** showblock - this function will display the blocking resource and the
**	locks contained in that resource.  This routine expects to receive
**	a blocking resource ID.  This resource ID is usually obtained from
**	the lock list structure.
**
** Inputs:
**	irsb_blocking - the ID of the resource which contains the blocking
**		lock.
**
** Outputs:
**	This routine displays the form which contains the blocking resource
**	information.
**
** Returns
**	none
**
** History:
**	9/13/89		tomt	added code to display blocking lock info
**	9/24/89		tomt		now pass mode in which showblock
**					is called.  Determines which title
**					is displayed
**
*/
VOID
showblock(irsb_blocking, mode)
i4 irsb_blocking;
i4  mode;
{
  int begin_row, p_numrows, currow, screen_rows;
  char titlmsg[35], colname[FE_MAXNAME + 1];
    char	form_name[FE_MAXNAME + 1];
    /* set form name to pass to blklkwork */
    STcopy("fshowblock", form_name);
    /* Calculate starting row of popup */
    p_numrows = POPUP_R_SIZE;
/* # line 121 "showblk.qsc" */	/* inquire_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&currow,48,(char *)0,0);
    } /* IIiqset */
  }
/* # line 122 "showblk.qsc" */	/* inquire_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&screen_rows,44,(char *)0,0);
    } /* IIiqset */
  }
/* # line 123 "showblk.qsc" */	/* host code */
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
    /* Display the form and get the information */
/* # line 137 "showblk.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(19),(void *)IILQint(19));
    IIFRgpsetio(1,(short *)0,1,30,4,&begin_row);
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpcontrol(2,0);
    if (IIdispfrm((char *)"fshowblock",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 142 "showblk.qsc" */	/* host code */
	/* Put information on the form */
	/* display the appropriate title: this allows one form for two uses */
	if (mode == SHOWBLOCK)
	{
	    STcopy("Blocking Lock Display", titlmsg);
	}
	else
	{
	    STcopy("Resource Lock Display", titlmsg);
	}
/* # line 153 "showblk.qsc" */	/* putform */
      {
        if (IIfsetio((char *)"fshowblock") != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,titlmsg);
        } /* IIfsetio */
      }
/* # line 154 "showblk.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"fshowblock",(char *)"lockdetail",(char *)"\
r") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 155 "showblk.qsc" */	/* host code */
	 /* get lock info and put on screen */
	if (blklkwork((u_i4) irsb_blocking, (PTR) form_name) == FAIL)
	{
	    POP_MSG("The resource that this lock belongs to is not\
 currently in the resource list.  Only resource lists that match the\
 settings shown in the Options Display are available.  To change the\
 resources that are available for display, select the 'Options' menuitem\
 on the previous menu (Lock List Display).  After the Options you specified\
 are saved, select the 'Refresh' menuitem which is also available on the\
 previous menu");
/* # line 165 "showblk.qsc" */	/* breakdisplay */
      {
        if (1) goto IIfdE1;
      }
/* # line 166 "showblk.qsc" */	/* host code */
	}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 171 "showblk.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,(char *)"fshowblock",(char *)"lockdetail",
            (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 172 "showblk.qsc" */	/* host code */
	tablefield_find();
/* # line 173 "showblk.qsc" */	/* resume */
          {
            IIrescol((char *)"lockdetail",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 177 "showblk.qsc" */	/* host code */
	my_help("ipmshwbk.hlp", "Blocking Lock");
        }
      } else if (IIretval() == 3) {
        {
/* # line 181 "showblk.qsc" */	/* breakdisplay */
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
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 183 "showblk.qsc" */	/* host code */
    return;
}
i4
blklkwork(blocking_rsb, formname)
u_i4 blocking_rsb;
PTR formname;
{
  int counter, page_num, row_num;
  char id[HEXID_SIZE + 1], lock_tp[15];
  char tab_name[FE_MAXNAME + 1];
    char	dbbuf[FE_MAXNAME + 1];	/* for printing dbname */
  char *dbname = dbbuf;
    bool	resource_found = FALSE;	/* to determine if a match occured */
    bool	reset = TRUE;		/* always start at list begin */
    LK_RSB_INFO *rsb;
  char str_gr[5];
  char str_cv[5];
  char res_name[FE_MAXNAME + 1];
    dbbuf[0] = EOS;
    counter = 0;
    /* Get the Info */
    close_db();				/* ensure no open databases */
    while ((rsb = (LK_RSB_INFO *)l_retrieve(RESINFO, 0, &reset)) != NULL) 
    {
	/*
	** find proper resource list, format the resource info then get lock 
	** info.  Certain fields on the form will be made invisible depending 
	** on lock type
	*/
	counter++;			/* keep track of element number */
	if (blocking_rsb != rsb->rsb_id)/* NOT the resource id - skip */
	{
	    continue;
	}
	resource_found = TRUE;		/* found a match */
	STprintf(id, "%x", rsb->rsb_id);
	if (rsb->rsb_key[0] <= Lock_type_size)
	    STcopy(Lock_type[rsb->rsb_key[0]], lock_tp);
	else						/* new lock type! */
	    STprintf(lock_tp, "<%d>", rsb->rsb_key[0]);
	switch(rsb->rsb_key[0]) 
	{
	    case LK_DATABASE:
	    case LK_SV_DATABASE:
	    case LK_OWNER_ID:
	    case LK_JOURNAL:
	    case LK_DB_TEMP_ID:
	    case LK_CONFIG:
	    case LK_OPEN_DB:
	    case LK_CKP_CLUSTER:
	    case LK_BM_DATABASE:
	    case LK_CKP_DB:
	    case LK_BM_LOCK:
/* # line 241 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 244 "showblk.qsc" */	/* host code */
		if ((rsb->rsb_key[0] == LK_BM_DATABASE) ||
		    (rsb->rsb_key[0] == LK_CKP_DB)) /* dbname handling */
		    (VOID)get_db_name(dbname, rsb->rsb_key[1]);
		else
		    STcopy((char*)&rsb->rsb_key[1], dbname);
		break;
	    case LK_TABLE:
	    case LK_SV_TABLE:
	    case LK_BM_TABLE:
	    case LK_TBL_CONTROL:
		/* get table name.  Ignore if user is looking for db locks. */
	        (VOID)get_db_name(dbname, rsb->rsb_key[1]);
		open_db(dbname);	/* open a database */
		(VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
/* # line 260 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 263 "showblk.qsc" */	/* host code */
		break;
	    case LK_PAGE:
	    case LK_BM_PAGE:
            case LK_PH_PAGE:
	        (VOID)get_db_name(dbname, rsb->rsb_key[1]);
		open_db(dbname);	/* open a database */
		(VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
		page_num = rsb->rsb_key[4];
                row_num = rsb->rsb_key[5];
/* # line 273 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 276 "showblk.qsc" */	/* host code */
		break;
	    case LK_AUDIT:
		if (rsb->rsb_key[1] == SXAS_LOCK)
		{
		    if (rsb->rsb_key[2] == SXAS_STATE_LOCK)
			STprintf(res_name, "State(primary)");
		    else if (rsb->rsb_key[2] == SXAS_SAVE_LOCK)
			STprintf(res_name, "State(save)");
		    else
			STprintf(res_name, "State(oper=%d)", 
				rsb->rsb_key[2]);
		}
		else if (rsb->rsb_key[1] == SXAP_LOCK)
		{
		    if (rsb->rsb_key[2] == SXAP_SHMCTL)
			STprintf(res_name, "Physical_layer(shmctl, node=%x)",
			    rsb->rsb_key[3]);
		    else if (rsb->rsb_key[2] == SXAP_ACCESS)
			STprintf(res_name, "Physical_layer(access)");
		    else if (rsb->rsb_key[2] == SXAP_QUEUE)
			STprintf(res_name,
			    "Physical_layer(queue, node=%x)",
			    rsb->rsb_key[3]);
		    else
			STprintf(res_name, "Physical_layer(oper=%d)",
			    rsb->rsb_key[2]);
		}
		else
		{
		    STprintf(res_name, "TYPE=%d, OPER=%d", 
			rsb->rsb_key[1], rsb->rsb_key[2]);
		}
/* # line 309 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 312 "showblk.qsc" */	/* host code */
		break;
	    case LK_ROW:
	        (VOID)get_db_name(dbname, rsb->rsb_key[1]);
		open_db(dbname);	/* open a database */
		(VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
		page_num = rsb->rsb_key[4];
		row_num = rsb->rsb_key[5];
/* # line 320 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 323 "showblk.qsc" */	/* host code */
		break;
	    case LK_EXTEND_FILE:
	        (VOID)get_db_name(dbname, rsb->rsb_key[1]);
		open_db(dbname);	/* open a database */
		(VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
/* # line 329 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 332 "showblk.qsc" */	/* host code */
		break;
	    case LK_SS_EVENT:
		STprintf(res_name, "SERVER = %x,[%x,%x])",
		    rsb->rsb_key[1], rsb->rsb_key[2], rsb->rsb_key[3]);
/* # line 337 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 340 "showblk.qsc" */	/* host code */
		break;
	    case LK_EVCONNECT:
		STprintf(res_name, "Server PID = %d", rsb->rsb_key[1]);
/* # line 344 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 347 "showblk.qsc" */	/* host code */
		break;
	    case LK_CONTROL:
		string_move(res_name, (PTR) &rsb->rsb_key[1], LK_KEY_LENGTH);
		res_name[LK_KEY_LENGTH] = EOS;
/* # line 352 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 355 "showblk.qsc" */	/* host code */
		break;
	    case LK_CKP_TXN:
		if (rsb->rsb_key[3] != NULL)
		{
		    string_move(dbname, (PTR) &rsb->rsb_key[3], 16);
		    dbname[16] = NULL;
		}
		else
		{
		    dbname[0] = NULL;
		}
/* # line 367 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 370 "showblk.qsc" */	/* host code */
		break;
	    case LK_CREATE_TABLE:
	    case LK_SEQUENCE:
	        (VOID)get_db_name(dbname, rsb->rsb_key[1]);
		open_db(dbname);	/* open a database */
		(VOID)STprintf(tab_name, "'%s-%s'",
		    &rsb->rsb_key[4], &rsb->rsb_key[2]);
/* # line 378 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 381 "showblk.qsc" */	/* host code */
		break;
            case LK_VAL_LOCK:
                (VOID)get_db_name(dbname, rsb->rsb_key[1]);
                open_db(dbname);        /* open a database */
                (VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
                STprintf(tab_name, "VALUE: <%d,%d,%d>",
                    rsb->rsb_key[4], rsb->rsb_key[5], rsb->rsb_key[6]);
/* # line 389 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 392 "showblk.qsc" */	/* host code */
                break;
	    default:
		STprintf(res_name, "%d, (%x,%x,%x,%x,%x,%x)",
		    rsb->rsb_key[0], rsb->rsb_key[1],
                    rsb->rsb_key[2], rsb->rsb_key[3],
		    rsb->rsb_key[4], rsb->rsb_key[5], rsb->rsb_key[6]); 
/* # line 400 "showblk.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,(char *)"fshowblock",(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"table_name",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"page_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"row_num",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"resource",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 403 "showblk.qsc" */	/* host code */
	}
	STcopy(lock_itoa(rsb->rsb_grant), str_gr);
	STcopy(lock_itoa(rsb->rsb_convert), str_cv);
/* # line 408 "showblk.qsc" */	/* putform */
  {
    if (IIfsetio((char *)"fshowblock") != 0) {
      IIputfldio((char *)"id",(short *)0,1,32,0,id);
      IIputfldio((char *)"grant",(short *)0,1,32,0,str_gr);
      IIputfldio((char *)"convert",(short *)0,1,32,0,str_cv);
      IIputfldio((char *)"table_name",(short *)0,1,32,0,tab_name);
      IIputfldio((char *)"page_num",(short *)0,1,30,4,&page_num);
      IIputfldio((char *)"dbname",(short *)0,1,32,0,dbname);
      IIputfldio((char *)"resource",(short *)0,1,32,0,res_name);
      IIputfldio((char *)"row_num",(short *)0,1,30,4,&row_num);
      IIputfldio((char *)"lock_type",(short *)0,1,32,0,lock_tp);
    } /* IIfsetio */
  }
/* # line 415 "showblk.qsc" */	/* host code */
	/* Now display the actual locks in the tablefield */
	printresdet(counter, formname);
	break;	/* no need to look at rest of resource list */
    }
    close_db();	/* ensure that there are no open databases */
    if (resource_found)
	return(OK);
    else
	return(FAIL);
}
