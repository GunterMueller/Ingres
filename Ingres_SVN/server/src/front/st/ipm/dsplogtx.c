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
/*
**  File: dsplogtx.qc
**
**  Purpose - this file contains the routines which display the active 
**	transactions (in logging system) information form.  
**
**  This file contains:
**	displgtx() - displays a list of transactions in the logging system form.
**	printlgtx() - gets, formats and outputs logging system xaction 
**			information.
**	printlghlgx() - gets,formats and outputs logging system xaction
**	    info for log header form.
**	get_sessname() - given a transaction id, returns a session name
**
**  History
**	8/29/89		tomt	created
**	9/5/89		tomt	added code to display info
**	9/8/89		tomt	fixed get_session_id to automatically refresh
**				the xaction list if its empty
**	9/19/89		tomt	do addforms here
**	10/1/89		tomt	integrated 6.3 changes
**	10/1/89		tomt	wrote get_sessname based on 6.3 changes.
**				get_session_id is OBSOLETE (and gone)
**      3/12/90         tomt    changed appearence of log xaction display
**      3/12/90         tomt    added log filter for inactive transactions
**	3/18/90		tomt	made new routine printlghlgx for log header
**				form: gets xaction info and DISPLAYS it.
**	5/7/90		tomt	fixed bug with xaction wraparound in printlghlgx
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**	10 Feb 1993	jpk	better support display of XA transactions.
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**      26-oct-1993 (tomm)
**              XA transaction ID's have changed.  This will at least allow
**              the file to compile.  Note, however, it is untested.
**	15-Nov-1993 (tad)
**		Bug #56449
**		Replace %x with %p for pointer values where necessary.
**	 1-Nov-1995 (nick)
**		Remove unused variable.
**      08-Dec-1997 (hanch04)
**          Use new la_block field when block number is needed instead
**          of calculating it from the offset
**          Keep track of block number and offset into the current block.
**          for support of logs > 2 gig
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	20-aug-2004 (vansa02)Bug # 111901 INGSRV2738
**		Increase the length of session ID to 16 from 8 and change 
**		the form layout to accommodate the session ID being 16 in length. 
*/
VOID printlgtx();
VOID printlghlgx();
VOID get_lklgstats();
VOID morelgtx();
VOID stat_decode();
VOID tablefield_find();
VOID get_logdb_name();
VOID get_sessname();
VOID get_logpid();
VOID ipm_format_dis_tran_id(DB_DIS_TRAN_ID *dis_tran_id, char *buf);
/*
** Declare file wide statics
*/
  static char Fdisplgtx[]= "fdisplgtx";
  static char Fmorelgtx[]= "fmorelgtx";
static char Lgtxstat[] = "INACTIVE,ACTIVE,SPLIT,PROTECT,JOURNAL,READONLY,\
NOABORT,RECOVER,FORCE_ABORT,SESSION_ABORT,SERVER_ABORT,PASS_ABORT,\
DBINCONSISTENT,,DISTRIBUTED,WILLING_COMMIT,RE-ASSOC,RESUME,MAN_ABORT,\
MAN_COMMIT,CKPDB_PEND";
/*
** Routine: displgtx()  displays a list of transactions on a popup form
**
** Inputs: none
**
** Outputs: displays a form
**
** Side effects: assumes the forms system is active
**
** History
**	8/29/89		tomt	created
**	9/5/89		tomt	added code to display info
**	9/19/89		tomt	do addforms here
**	10/1/89		tomt	integrated 6.3 changes
**	10/1/89		tomt	wrote get_sessname based on 6.3 changes.
**				get_session_id is OBSOLETE (and gone)
**      3/12/90         tomt    changed appearence of log xaction display
**	10 Feb 1993	jpk	display of XA transactions the same way
**				logdump does; in fact, the code is taken
**				from there and lives in new function
**				ipm_format_dis_tran_id(); this simplifies
**				the body of displgtx().
*/
VOID
displgtx()
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
  GLOBALREF int *fdisplgtx;
  GLOBALREF int *fmorelgtx;
    /*
    **  Determine if forms are added, if not addform them. 
    */
    if (!forms_added)
    {
/* # line 155 "dsplogtx.qsc" */	/* addform */
  {
    IIaddform(fdisplgtx);
  }
/* # line 156 "dsplogtx.qsc" */	/* addform */
  {
    IIaddform(fmorelgtx);
  }
/* # line 157 "dsplogtx.qsc" */	/* host code */
	forms_added = TRUE;
    }
    /* Display the form and get the information */
/* # line 161 "dsplogtx.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fdisplgtx,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 166 "dsplogtx.qsc" */	/* inittable */
      {
        if (IItbinit(Fdisplgtx,(char *)"txlist",(char *)"r") != 0) {
          IIthidecol((char *)"tr_db_id",(char *)"c8");
          IIthidecol((char *)"tr_pr_id",(char *)"c8");
          IIthidecol((char *)"tr_sess_id",(char *)"c16");
          IIthidecol((char *)"tr_first",(char *)"c30");
          IIthidecol((char *)"tr_last",(char *)"c30");
          IIthidecol((char *)"tr_cp",(char *)"c30");
          IIthidecol((char *)"tr_id",(char *)"c8");
          IIthidecol((char *)"tr_pid",(char *)"c8");
          IIthidecol((char *)"tr_dis_id",(char *)"c150");
          IIthidecol((char *)"force",(char *)"i4");
          IIthidecol((char *)"wait",(char *)"i4");
          IItfill();
        } /* IItbinit */
      }
/* # line 170 "dsplogtx.qsc" */	/* host code */
/*	c150 ~= DB_XA_XIDDATASIZE + 15 */
	printlgtx();	/* put info in tablefield */
	/* get number of tranactions */
/* # line 174 "dsplogtx.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,Fdisplgtx,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)"txlist",0);
        } /* IIiqset */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 178 "dsplogtx.qsc" */	/* host code */
	/* Get lock resource info from INGRES */
/* # line 179 "dsplogtx.qsc" */	/* message */
          {
            IImessage((char *)"Loading logging information...");
          }
/* # line 180 "dsplogtx.qsc" */	/* host code */
	get_lklgstats(GET_LOGXACTLIST, (PTR) 0);
/* # line 182 "dsplogtx.qsc" */	/* clear */
          {
            IIfldclear((char *)"txlist");
          }
/* # line 183 "dsplogtx.qsc" */	/* host code */
	printlgtx();	/* put info in tablefield */
	/* get number of xactions */
/* # line 186 "dsplogtx.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Fdisplgtx,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)"txlis\
t",0);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 190 "dsplogtx.qsc" */	/* host code */
	if (list_count == 0) {
	    POP_MSG("There are no transactions to get information on");
/* # line 192 "dsplogtx.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 193 "dsplogtx.qsc" */	/* host code */
	}
	morelgtx();
        }
      } else if (IIretval() == 3) {
        {
/* # line 198 "dsplogtx.qsc" */	/* host code */
#ifdef VMS 
          char cpid[11];
	if (list_count == 0) 
	{
	    POP_MSG("There are no processes to get information on");
/* # line 204 "dsplogtx.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 205 "dsplogtx.qsc" */	/* host code */
	}
/* # line 206 "dsplogtx.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdisplgtx,(char *)"txlist",-2) != 0) {
              IItcogetio((short *)0,1,32,10,cpid,(char *)"tr_pid");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 207 "dsplogtx.qsc" */	/* host code */
	showproc(cpid);
#else
	POP_MSG("Process_Info (transactions) is available only on VMS");
/* # line 210 "dsplogtx.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 211 "dsplogtx.qsc" */	/* host code */
#endif 
        }
      } else if (IIretval() == 4) {
        {
/* # line 215 "dsplogtx.qsc" */	/* host code */
	if (list_count == 0) 
	{
	    POP_MSG("There is nothing to find");
/* # line 218 "dsplogtx.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 219 "dsplogtx.qsc" */	/* host code */
	}
/* # line 220 "dsplogtx.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,Fdisplgtx,(char *)"txlist",(char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 221 "dsplogtx.qsc" */	/* host code */
	tablefield_find();
/* # line 222 "dsplogtx.qsc" */	/* resume */
          {
            IIrescol((char *)"txlist",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 226 "dsplogtx.qsc" */	/* host code */
	my_help("ipmdslgt.hlp", "Log Transaction");
        }
      } else if (IIretval() == 6) {
        {
/* # line 230 "dsplogtx.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 234 "dsplogtx.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
/* # line 235 "dsplogtx.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 236 "dsplogtx.qsc" */	/* breakdisplay */
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
    if (IInmuact(Procinfo,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Find",(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 239 "dsplogtx.qsc" */	/* host code */
    if (e_xit) 
    {	
	terminate(OK);		/* user wants to leave */
    }
    return;
}
/*
** morelgtx() - This routine displays the popup form which show the
**	less needed database info.
**
** History
**	8/29/89		tomt	written
**	9/5/89		tomt	added code to display info
**	10/1/89		tomt	integrated 6.3 changes
**	3/12/90         tomt    changed form added wait and force ondetail.
**
*/
VOID
morelgtx()
{
    /*
    ** Declare Variables
    */
  char db_id[HEXID_SIZE + 1], prid[HEXID_SIZE + 1], sessid[HEXID_SIZE + 1];
  char trfirst[31], trlast[31], trcp[31];
  char trid[HEXID_SIZE + 1], pid[HEXID_SIZE + 1], tr_disid
  [DB_XA_XIDDATASIZE + 15];
  int trwait, trforce;
/* # line 270 "dsplogtx.qsc" */	/* display */
  {
    if (IIdispfrm(Fmorelgtx,(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 273 "dsplogtx.qsc" */	/* host code */
	/*
	**   Get the values off of the tablefield on the base form and
	**   place them on the current form
	*/
/* # line 277 "dsplogtx.qsc" */	/* getrow */
      {
        if (IItbsetio(2,Fdisplgtx,(char *)"txlist",-2) != 0) {
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,db_id,(char *)
"tr_db_id");
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,prid,(char *)
"tr_pr_id");
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,sessid,(char *)
"tr_sess_id");
          IItcogetio((short *)0,1,32,30,trfirst,(char *)"tr_first");
          IItcogetio((short *)0,1,32,30,trlast,(char *)"tr_last");
          IItcogetio((short *)0,1,30,4,&trwait,(char *)"wait");
          IItcogetio((short *)0,1,30,4,&trforce,(char *)"force");
          IItcogetio((short *)0,1,32,30,trcp,(char *)"tr_cp");
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,pid,(char *)"tr_\
pid");
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,trid,(char *)"tr\
_id");
          IItcogetio((short *)0,1,32,DB_XA_XIDDATASIZE + 15-1,tr_disid,
          (char *)"tr_dis_id");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 281 "dsplogtx.qsc" */	/* putform */
      {
        if (IIfsetio(Fmorelgtx) != 0) {
          IIputfldio((char *)"tr_db_id",(short *)0,1,32,0,db_id);
          IIputfldio((char *)"tr_pr_id",(short *)0,1,32,0,prid);
          IIputfldio((char *)"force",(short *)0,1,30,4,&trforce);
          IIputfldio((char *)"wait",(short *)0,1,30,4,&trwait);
          IIputfldio((char *)"tr_sess_id",(short *)0,1,32,0,sessid);
          IIputfldio((char *)"tr_first",(short *)0,1,32,0,trfirst);
          IIputfldio((char *)"tr_last",(short *)0,1,32,0,trlast);
          IIputfldio((char *)"tr_cp",(short *)0,1,32,0,trcp);
          IIputfldio((char *)"tr_id",(short *)0,1,32,0,trid);
          IIputfldio((char *)"tr_pid",(short *)0,1,32,0,pid);
          IIputfldio((char *)"tr_dis_id",(short *)0,1,32,0,tr_disid);
        } /* IIfsetio */
      }
/* # line 285 "dsplogtx.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 286 "dsplogtx.qsc" */	/* submenu */
      {
        goto IImuI3;
IIfdB3:
        IIrunmu(0);
        if (IIretval() == 1) {
          {
/* # line 289 "dsplogtx.qsc" */	/* breakdisplay */
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
/* # line 293 "dsplogtx.qsc" */	/* host code */
    return;
}
/*
** printlgtx() - this routine gets, formats and outputs the logging system
**	database information.
**
** Inputs:
**	none
**
** Outputs:
**	formatted logging db information to screen or file
**
** Returns:
**	none
**
** History:
**	8/27/89		tomt	written
**	9/8/89		tomt	now starts at beginning of tx list
**	10/1/89		tomt	integrated 6.3 changes
**      3/13/90		tomt	added inactive transaction display feature
**	10 Feb 1993	jpk	added XA support
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
*/
VOID
printlgtx()
  {
    u_i4	bgn, mid, end;
    bool	reset = TRUE;	/* start at beginning of list */
    char tr_eid[17], db_name[FE_MAXNAME + 1];
    char tr_db_id[HEXID_SIZE + 1], tr_pr_id[HEXID_SIZE + 1], tr_sess_id
    [HEXID_SIZE + 1], tr_stat[81];
    char tr_first[31], tr_last[31], tr_cp[31], tr_id[HEXID_SIZE + 1];
    char tr_pid[HEXID_SIZE + 1], tr_dis_id[DB_XA_XIDDATASIZE + 15];
    LG_TRANSACTION	*lxb;
    LG_HEADER	header;
    char *tr_user_name;
    i4 tr_stat_write;
    i4 tr_stat_split;
    i4 tr_stat_force;
    i4 tr_stat_wait;
    /*
    **    Need header info for calculating journal window size
    */
    get_lklgstats(GET_LOGHEADER, (PTR) &header);
    while ((lxb =
	(LG_TRANSACTION *)l_retrieve(LOGXACTINFO, 0, &reset)) != NULL)
    {
	/*
	**      Filter for inactive transactions
	*/
        if ((lxb->tr_status & TR_INACTIVE) && (!flag_inactive_tx))
            continue;
	/*
	**    do formatting for display
	*/
 	STprintf(tr_eid, "%x%0x",
 	    lxb->tr_eid.db_high_tran, lxb->tr_eid.db_low_tran);
	ipm_format_dis_tran_id(&lxb->tr_dis_id, tr_dis_id);
	STprintf(tr_id, "%x", lxb->tr_id);
	STprintf(tr_db_id, "%x", lxb->tr_db_id);
	get_logdb_name(db_name, lxb->tr_db_id);
	/*
	**   get session name 
	*/
	lxb->tr_user_name[lxb->tr_l_user_name - 1] = EOS; /* null EOS */
	STprintf(tr_sess_id, "%p", lxb->tr_sess_id);	/* strip leading 0s */
	STprintf(tr_pr_id, "%x", lxb->tr_pr_id);
	get_logpid(tr_pid, lxb->tr_pr_id);	/* get the REAL PID */
	bgn = lxb->tr_first.la_sequence;
	mid = lxb->tr_first.la_block;
	end = lxb->tr_first.la_offset;
	STprintf(tr_first, "<%d,%d,%d>", bgn, mid, end);
	bgn = lxb->tr_last.la_sequence;
	mid = lxb->tr_last.la_block;
	end = lxb->tr_last.la_offset;
	STprintf(tr_last, "<%d,%d,%d>", bgn, mid, end);
	bgn = lxb->tr_cp.la_sequence;
	mid = lxb->tr_cp.la_block;
	end = lxb->tr_cp.la_offset;
	STprintf(tr_cp, "<%d,%d,%d>", bgn, mid, end);
	stat_decode(tr_stat, Lgtxstat, (u_i4) lxb->tr_status);
	if (lxb->tr_status == 0)  /* just in case, make it clear on scr */
	    STcopy("<none>", tr_stat);
	tr_user_name = lxb->tr_user_name;
	tr_stat_write = lxb->tr_stat.write;
	tr_stat_split = lxb->tr_stat.split;
	tr_stat_force = lxb->tr_stat.force;
	tr_stat_wait = lxb->tr_stat.wait;
/* # line 400 "dsplogtx.qsc" */	/* loadtable */
    {
      if (IItbact(Fdisplgtx,(char *)"txlist",1) != 0) {
        IItcoputio((char *)"tr_eid",(short *)0,1,32,0,tr_eid);
        IItcoputio((char *)"db_name",(short *)0,1,32,0,db_name);
        IItcoputio((char *)"tr_sessname",(short *)0,1,32,0,tr_user_name);
        IItcoputio((char *)"write",(short *)0,1,30,4,&tr_stat_write);
        IItcoputio((char *)"split",(short *)0,1,30,4,&tr_stat_split);
        IItcoputio((char *)"force",(short *)0,1,30,4,&tr_stat_force);
        IItcoputio((char *)"wait",(short *)0,1,30,4,&tr_stat_wait);
        IItcoputio((char *)"tr_db_id",(short *)0,1,32,0,tr_db_id);
        IItcoputio((char *)"tr_pr_id",(short *)0,1,32,0,tr_pr_id);
        IItcoputio((char *)"tr_sess_id",(short *)0,1,32,0,tr_sess_id);
        IItcoputio((char *)"tr_first",(short *)0,1,32,0,tr_first);
        IItcoputio((char *)"tr_last",(short *)0,1,32,0,tr_last);
        IItcoputio((char *)"tr_cp",(short *)0,1,32,0,tr_cp);
        IItcoputio((char *)"tr_id",(short *)0,1,32,0,tr_id);
        IItcoputio((char *)"tr_pid",(short *)0,1,32,0,tr_pid);
        IItcoputio((char *)"tr_status",(short *)0,1,32,0,tr_stat);
        IItcoputio((char *)"tr_dis_id",(short *)0,1,32,0,tr_dis_id);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 408 "dsplogtx.qsc" */	/* host code */
    }
    return;
  }
/*
** get_sessname() - this routine returns the session name corresponding
**	to the passed session_id.
**
** Inputs
**	xact_high - high order transaction id
**
**	xact_low - low order transaction id
**
**	session_name - pointer to the buffer where the session name is
**		placed.
**
** Outputs
**	the session name is placed into the buffer pointed to by session_name
**
** Returns
**	none
**
** Side Effects
**	this routine may refresh all log xaction information.  This could cause
**	a delay in getting session names.
**
** History
**	10/1/89		tomt	written
**	10/3/89		tomt	added optional parameter to indicate whether
**				or not to refresh the server information.  This
**				is useful when the first call could cause a
**				refresh and we don't want the overhead in
**				succeeding calls.  If refresh is TRUE it
**				will be set to FALSE when a refresh occurs.
**				It must be a pointer.
*/
VOID
get_sessname(xact_high, xact_low, session_name, refresh)
u_i4 xact_high;
u_i4 xact_low;
PTR session_name;
bool *refresh;
{
    LG_TRANSACTION	*lxb;
    bool	need_refresh = FALSE;	/* optomistic assumption */
    bool	search_failed = FALSE;	/* used to break out on failure */
    bool	search_done = FALSE;	/* used to break out on success */
    bool	assert_refresh = TRUE;	/* default is to assert refresh */
    bool	reset_flag = TRUE;
    /*
    **  Determine if transaction list is empty.
    */
    for (;;)	/* something to break out of */
    {
	if (search_done)	/* success - leave */
	    break;
	if (search_failed)	/* copy default into buffer; leave */
	{
	    STcopy("<none>", session_name);
	    break;
	}
	if (need_refresh)	/* get all log xaction info */
	{
	    /*
	    ** Determine if refresh is a pointer.  If not, use default
	    ** which is to refresh.  If refresh is TRUE, turn it off after
	    ** refreshing xaction info.  On subsequent calls from the same caller
	    ** the refresh will NOT be done because the passed flag will be false.
	    */
	    if (refresh != NULL)
	    {
		if (*refresh)
		{
		    *refresh = FALSE;
		    get_lklgstats(GET_LOGXACTLIST, (PTR) 0);
		}
		else
		{
		    assert_refresh = FALSE;
		}
	    }
	    else	/* default is to refresh */
	    {
		get_lklgstats(GET_LOGXACTLIST, (PTR) 0);
	    }
	    search_failed = TRUE;	/* if no match before loop starts
					** again - the search failed
					*/
	    need_refresh = FALSE;	/* don't need to refresh again */
	    if (!assert_refresh) /* done - get out */
		continue;	
	}
	/*
	** if list is empty and no search has occured - refresh.  Also,
	** toggle passed refresh flag to TRUE.  This guarantees that the
	** refresh occurs.
	*/
	if ((l_num_elements(LOGXACTINFO) == 0) && !search_failed)
	{
	    if (refresh != NULL)
		*refresh = TRUE;
	    need_refresh = TRUE;
	    continue;	/* top of for loop */
	}
	/*
	**    Search for the session name
	*/
        for ((lxb = (LG_TRANSACTION *)l_retrieve(LOGXACTINFO, 0, &reset_flag));
	    lxb != NULL;
	    (lxb = (LG_TRANSACTION *)l_retrieve(LOGXACTINFO, 0, &reset_flag)))
	{
 	    if ((lxb->tr_eid.db_high_tran == xact_high) &&
 		(lxb->tr_eid.db_low_tran == xact_low))
	    {
		lxb->tr_user_name[lxb->tr_l_user_name - 1] = EOS; /* null term */
		STcopy(&lxb->tr_user_name[0], session_name);
		search_done = TRUE;
		search_failed = FALSE;
		break;
	    }
	    if (search_done)
		break;			/* break out of servinfo for loop */
	}
	/*
	**    If the search yielded nothing and the log xaction info was NOT
	**    yet refreshed by this routine, refresh the information by setting
	**    the appropriate flag
	*/
	if (!search_done && !search_failed)
	    need_refresh = TRUE;
    }		/* end of for (;;) loop */
    return;
}
/*
** printlghlgx() - this routine displays on the log header form those
**	transactions that fall between the specified range of log
**	file pages.
**
** Inputs
**	low_page - begin of range of pages in log file which is used
**	    to find qualifying transactions.
**
**	high_page - end of range of pages in log file which is used
**	    to find qualifying transactions.
**
**	form_name - pointer to the buffer which contains the name of
**	    the form to display the info.
**
**	refresh - If refresh is TRUE it will be set to FALSE when a refresh
**	    occurs.  It must be a pointer.  This prevents multiple refreshs
**	    during subsequent calls to this routine.  The caller can reset
**	    it when it is necessary to refresh.
**
** Outputs
**	displays the qualifying transactions on the log header form
**
** Returns
**	none
**
** Side Effects
**	this routine may refresh all log xaction information.  This could cause
**	a small delay.
**
** History
**	3/18/90		tomt	written
**	12/13/92	jpk	support XA transactions
*/
VOID
printlghlgx(low_page, high_page, form_name, refresh)
int low_page;
int high_page;
  char *form_name;
bool *refresh;
  {
    bool	reset_flag = TRUE;	/* for log xaction loop */
    char t_treid[17], dbname[FE_MAXNAME + 1];
    char t_trstat[81];
    i4 t_trfirst, t_trlast, t_trcp;
    char t_trpid[HEXID_SIZE + 1];
    LG_TRANSACTION	*tlxb;
    LG_HEADER	theader;
    i4 tr_stat_write;
    i4 tr_stat_split;
    i4 tr_stat_force;
    i4 tr_stat_wait;
    char *tr_user_name;
    get_lklgstats(GET_LOGHEADER, (PTR) &theader);   /* calculating eot, bot */
    /*
    **  Determine if transaction list is empty.
    */
    if (refresh != NULL)
    {
	if (*refresh)
	{
	    *refresh = FALSE;
	    get_lklgstats(GET_LOGXACTLIST, (PTR) 0);
	    get_lklgstats(GET_LOGDBLIST, (PTR) 0);
	    get_lklgstats(GET_LOGPROCESS, (PTR) 0);
	}
    }
    /*
    **  if any needed list is empty - refresh.  Also,
    **  toggle passed refresh flag to FALSE.  
    */
    if (((l_num_elements(LOGXACTINFO) == 0) ||
	(l_num_elements(LOGPROCINFO) == 0) || 
	(l_num_elements(LOGDBINFO) == 0)))
    {
	get_lklgstats(GET_LOGXACTLIST, (PTR) 0);
	get_lklgstats(GET_LOGDBLIST, (PTR) 0);
	get_lklgstats(GET_LOGPROCESS, (PTR) 0);
	if (refresh != NULL)
	{
	    *refresh = FALSE;
	}
    }
    /*
    **  Search for qualifying transactions
    */
    for ((tlxb = (LG_TRANSACTION *)l_retrieve(LOGXACTINFO, 0, &reset_flag));
	tlxb != NULL;
	(tlxb = (LG_TRANSACTION *)l_retrieve(LOGXACTINFO, 0, &reset_flag)))
    {
	/*
	**    Filter for inactive transactions
	**    Commented this out.  Better to show all transactions that have
	**    written to the log file regardless of whether or not they
	**    are INACTIVE.  tomt  3-28-90
	*/
	/*        if ((tlxb->tr_status & TR_INACTIVE) && (!flag_inactive_tx))
	**            continue;
	*/
	/*
	**    Get the page numbers begin and end so we cn filter out
	*/
	t_trfirst = tlxb->tr_first.la_block;	/* pg in log file */
	t_trlast = tlxb->tr_last.la_block;
	t_trcp = tlxb->tr_cp.la_block;
	if (t_trlast < t_trfirst)	/* wraparound in the log file */
	{
	    if ((low_page > t_trlast) && (high_page < t_trfirst))
	    {
		continue;	/* skip it - outside xaction window */
	    }
	}
	else	/* no wraparound */
	{
	    if ((low_page > t_trlast) || (high_page < t_trfirst))
	    {
	        continue;	/* skip it - outside xaction window */
	    }
	}
	/*
	**    do formatting for display
	*/
 	STprintf(t_treid, "%x%0x", tlxb->tr_eid.db_high_tran,
	    tlxb->tr_eid.db_low_tran);
	get_logdb_name(dbname, tlxb->tr_db_id);
	/*
	**    get session name 
	*/
	tlxb->tr_user_name[tlxb->tr_l_user_name - 1] = EOS; /* null EOS */
	get_logpid(t_trpid, tlxb->tr_pr_id);	/* get the REAL PID */
	stat_decode(t_trstat, Lgtxstat, (u_i4) tlxb->tr_status);
	if (tlxb->tr_status == 0)  /* just in case, make it clear on scr */
	    STcopy("<none>", t_trstat);
	tr_user_name = tlxb->tr_user_name;
	tr_stat_write = tlxb->tr_stat.write;
	tr_stat_split = tlxb->tr_stat.split;
	tr_stat_force = tlxb->tr_stat.force;
	tr_stat_wait = tlxb->tr_stat.wait;
/* # line 702 "dsplogtx.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,(char *)"log_xlist",1) != 0) {
        IItcoputio((char *)"transaction_id",(short *)0,1,32,0,t_treid);
        IItcoputio((char *)"dbname",(short *)0,1,32,0,dbname);
        IItcoputio((char *)"session_name",(short *)0,1,32,0,tr_user_name);
        IItcoputio((char *)"write",(short *)0,1,30,4,&tr_stat_write);
        IItcoputio((char *)"split",(short *)0,1,30,4,&tr_stat_split);
        IItcoputio((char *)"force",(short *)0,1,30,4,&tr_stat_force);
        IItcoputio((char *)"wait",(short *)0,1,30,4,&tr_stat_wait);
        IItcoputio((char *)"tr_first",(short *)0,1,30,4,&t_trfirst);
        IItcoputio((char *)"tr_last",(short *)0,1,30,4,&t_trlast);
        IItcoputio((char *)"tr_cp",(short *)0,1,30,4,&t_trcp);
        IItcoputio((char *)"tr_pid",(short *)0,1,32,0,t_trpid);
        IItcoputio((char *)"tr_status",(short *)0,1,32,0,t_trstat);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 709 "dsplogtx.qsc" */	/* host code */
    }
    return;
  }
/*
** Name: ipm_format_dis_tran_id     - display distributed transaction id.
**
** Description:
**	This routine formats DB_DIS_TRAN_ID information using STprintf
**
** Inputs:
**	    dis_tran_id			Distributed transaction id.
**
** Outputs:
**	    buff			string to hold result
**	Returns:
**	    VOID
**
** History:
**	1-oct-1992 (nandak)
**	    Created as part of sharing the transaction type.
**	5 Feb 1992 (jpk)
**	    Copied from logdump and modified for IPM; use STprintf
**	    instead of TRdisplay, so that it formats the printable
**	    string into the buffer pointed to, instead of emitting
**	    it directly to the terminal.
*/
VOID
ipm_format_dis_tran_id(DB_DIS_TRAN_ID *dis_tran_id, char *buff)
{
    i4         gtrid_length;
    i4         bqual_length;
    i4         rbytes;
    i4         i = 0;
    if (dis_tran_id->db_dis_tran_id_type == DB_INGRES_DIS_TRAN_ID_TYPE)
    {
	STprintf(buff, "Dis_tran_id: <Ingres,%d,%d> ",
  dis_tran_id->db_dis_tran_id.db_ingres_dis_tran_id.db_tran_id.db_high_tran,
  dis_tran_id->db_dis_tran_id.db_ingres_dis_tran_id.db_tran_id.db_low_tran);
    }
    else if (dis_tran_id->db_dis_tran_id_type == DB_XA_DIS_TRAN_ID_TYPE)
    {
	gtrid_length =
	    dis_tran_id->db_dis_tran_id.db_xa_extd_dis_tran_id.db_xa_dis_tran_id.gtrid_length;
	bqual_length =
	    dis_tran_id->db_dis_tran_id.db_xa_extd_dis_tran_id.db_xa_dis_tran_id.bqual_length;
	STprintf(buff, "Dis_tran_id: <XA,%d,%d,%d,",
	 	   dis_tran_id->db_dis_tran_id.db_xa_extd_dis_tran_id.db_xa_dis_tran_id.formatID,
		   gtrid_length, bqual_length);
	for (i = 0; i < (gtrid_length + bqual_length)/sizeof(i4);
		i += sizeof(i4))
	    STprintf(buff + STlength(buff), "%p",
		(i4 *)
		    &dis_tran_id->db_dis_tran_id.db_xa_extd_dis_tran_id.db_xa_dis_tran_id.data
						    [i*sizeof(i4)]);
        rbytes = gtrid_length+bqual_length;
        if (rbytes > sizeof(i4))
            rbytes = (gtrid_length+bqual_length)/sizeof(i4) %
                     sizeof(i4);
        if (rbytes)
	{
	    STprintf(buff + STlength(buff), "%p",
		  (i4 *)
		    &dis_tran_id->db_dis_tran_id.db_xa_extd_dis_tran_id.db_xa_dis_tran_id.data
						    [i*sizeof(i4)]);
	}
	STcat(buff, "> ");
    }
    else if (dis_tran_id->db_dis_tran_id_type == 0)
	STcopy("Dis_tran_id: <0,0> ", buff);
    else
	STcopy("Dis_tran_id: <UNKNOWN> ", buff);
    return;
}
