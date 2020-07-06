# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/* includes and defines */
#include <compat.h>
#include <si.h>
#include <cs.h>
#include <dbms.h>
#include <fe.h>
#include <pc.h>
#include <lk.h>
#include <lg.h>
#include <er.h>
#include <st.h>
#include        <adf.h>
#include        <ft.h>
#include        <fmt.h>
#include        <frame.h>
#include "lists.h"
#include "msgs.h"
#include "dba.h"
/*
**  File: dsplkres.qc
**
**  Purpose - this file contains the routines which display the lock
**	resource list form.  
**
**  This file contains:
**	displkres() - displays the lock resource list form
**	printlkres() - gets and formats the resource info for output
**
**  History
**	2/21/89		tomt	created
**	8/21/89		tomt	use INGDBA versions of LK structs.
**				They are used in place of the
**				equivalent LK structs when getting
**				info from l_retrieve()
**	9/19/89		tomt	moved ##ADDFORMS here and made form names
**				    readonly c vars
**	10/2/89		tomt	need to check if there are no db resources
**				    and open up a db if necessary
**	10/5/89		tomt	made various fixes to filtering and formatting
**				code
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**      27-Dec-1994 (canor01)
**              Added function declaration for lock_itoa()
**      24-apr-95 (nick) Bug #67888
**              Added LK_CKP_TXN
**	 1-nov-95 (nick)
**		Change display method for LK_CONTROL - the key is the 
**		entire rsb_key and can't be broken down.  
**		Remove unused variable.
**		Added LK_ROW.
**		Added LK_AUDIT.
**      22-nov-1996 (dilma04) 
**		Added LK_PH_PAGE and LK_VAL_LOCK for row level locking
**		project.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
** Forward declarations
*/
VOID close_db();
VOID open_db();
STATUS get_rel_name();
STATUS get_db_name();
VOID printlkres();
VOID morelkres();
VOID lockresdet();
VOID string_move();
FUNC_EXTERN VOID tablefield_find();
FUNC_EXTERN VOID get_lklgstats();
#ifdef VMS
    VOID showproc();
#endif 
char	*lock_itoa();
  GLOBALREF char *Invalid[];
/*
** Declares
*/
  static char Fdisplkres[]= "fdisplkres";
  static char Fmorelkres[]= "fmorelkres";
/*
** displkres() - this routine displays a form with lock resources
**	
** Input: none
**
** Output: none
**
** Returns: none
**
** History
**	9/19/89		tomt	moved ##ADDFORMS here and made form names
**				    readonly c vars
**	10/1/89		tomt	integrated 6.3 changes
**	11/1/95		nick	remove flag_file
*/
VOID
displkres()
{
  int list_position, resource_count;
    bool	e_xit = FALSE;
  char colname[FE_MAXNAME + 1];
    static bool	forms_added = FALSE;
  GLOBALREF int *fdisplkres;
  GLOBALREF int *fshowlkres;
  GLOBALREF int *fmorelkres;
    /*
    **  Determine if forms are added, if not addform them.  Also
    **  don't add forms if output is being directed to a file
    */
    if (!forms_added)
    {
/* # line 138 "dsplkres.qsc" */	/* addform */
  {
    IIaddform(fdisplkres);
  }
/* # line 139 "dsplkres.qsc" */	/* addform */
  {
    IIaddform(fshowlkres);
  }
/* # line 140 "dsplkres.qsc" */	/* addform */
  {
    IIaddform(fmorelkres);
  }
/* # line 141 "dsplkres.qsc" */	/* host code */
	forms_added = TRUE;
    }
    /* Display the form and get the information */
/* # line 145 "dsplkres.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fdisplkres,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 149 "dsplkres.qsc" */	/* host code */
	/* Put the info in the tablefield */
/* # line 150 "dsplkres.qsc" */	/* inittable */
      {
        if (IItbinit(Fdisplkres,(char *)"reslist",(char *)"r") != 0) {
          IIthidecol((char *)"value",(char *)"c18");
          IIthidecol((char *)"invalid",(char *)"c10");
          IIthidecol((char *)"rsb_count",(char *)"i4");
          IIthidecol((char *)"row_num",(char *)"i2");
          IItfill();
        } /* IItbinit */
      }
/* # line 152 "dsplkres.qsc" */	/* host code */
	printlkres();	/* populate the tablefield */
	/* get number of lock resource */
/* # line 155 "dsplkres.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,Fdisplkres,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&resource_count,32,(char *)"resli\
st",0);
        } /* IIiqset */
      }
/* # line 157 "dsplkres.qsc" */	/* host code */
	/* get number of lock resource */
/* # line 158 "dsplkres.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,Fdisplkres,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&resource_count,32,(char *)"resli\
st",0);
        } /* IIiqset */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 162 "dsplkres.qsc" */	/* host code */
	/* Get lock resource info from INGRES */
/* # line 163 "dsplkres.qsc" */	/* message */
          {
            IImessage((char *)"Loading lock information...");
          }
/* # line 164 "dsplkres.qsc" */	/* host code */
        get_lklgstats(GET_LOCKRES, (PTR) 0);
        get_lklgstats(GET_LOCKLIST, (PTR) 0);
/* # line 166 "dsplkres.qsc" */	/* clear */
          {
            IIfldclear((char *)"reslist");
          }
/* # line 167 "dsplkres.qsc" */	/* host code */
	printlkres();	/* populate the tablefield */
	/* get number of lock resource */
/* # line 170 "dsplkres.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Fdisplkres,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&resource_count,32,(char *)
"reslist",0);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 175 "dsplkres.qsc" */	/* host code */
	if (resource_count == 0)
	{
	    POP_MSG("There are no resources to get information on");
/* # line 178 "dsplkres.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 179 "dsplkres.qsc" */	/* host code */
	}
	morelkres();
        }
      } else if (IIretval() == 3) {
        {
/* # line 184 "dsplkres.qsc" */	/* host code */
	if (resource_count == 0)
	{
	    POP_MSG("There are no resources to examine");
/* # line 187 "dsplkres.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 188 "dsplkres.qsc" */	/* host code */
	}
/* # line 189 "dsplkres.qsc" */	/* message */
          {
            IImessage((char *)"Loading lock information...");
          }
/* # line 190 "dsplkres.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdisplkres,(char *)"reslist",-2) != 0) {
              IItcogetio((short *)0,1,30,4,&list_position,(char *)"row\
_num");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 191 "dsplkres.qsc" */	/* host code */
	lockresdet(list_position);
        }
      } else if (IIretval() == 4) {
        {
/* # line 196 "dsplkres.qsc" */	/* host code */
	if (resource_count == 0)
	{
	    POP_MSG("There is nothing to find");
/* # line 199 "dsplkres.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 200 "dsplkres.qsc" */	/* host code */
	}
/* # line 201 "dsplkres.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,Fdisplkres,(char *)"reslist",(char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 202 "dsplkres.qsc" */	/* host code */
	tablefield_find();
/* # line 203 "dsplkres.qsc" */	/* resume */
          {
            IIrescol((char *)"reslist",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 207 "dsplkres.qsc" */	/* host code */
	my_help("ipmdplkr.hlp", "Locks by Resource List");
        }
      } else if (IIretval() == 6) {
        {
/* # line 211 "dsplkres.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 215 "dsplkres.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
/* # line 216 "dsplkres.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 217 "dsplkres.qsc" */	/* breakdisplay */
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
/* # line 220 "dsplkres.qsc" */	/* host code */
    if (e_xit)			/* user wants to leave */
    {
	terminate(OK);
    }
    return;
}
/*
** morelkres() - This routine displays the popup form which show the
**	less needed resource info.
**
** History
**	3/2/89		tmt	written
**	10/1/89		tomt	integrated 6.3 changes
**
*/
VOID
morelkres()
{
/*
** Declare Variables
*/
  char cinvalid[10], cvalue[19];
  int rsbcount;
/* # line 245 "dsplkres.qsc" */	/* display */
  {
    if (IIdispfrm(Fmorelkres,(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 249 "dsplkres.qsc" */	/* host code */
	/*
	**   Get the values off of the tablefield on the base form and
	**   place them on the current form
	*/
/* # line 253 "dsplkres.qsc" */	/* getrow */
      {
        if (IItbsetio(2,Fdisplkres,(char *)"reslist",-2) != 0) {
          IItcogetio((short *)0,1,32,9,cinvalid,(char *)"invalid");
          IItcogetio((short *)0,1,32,18,cvalue,(char *)"value");
          IItcogetio((short *)0,1,30,4,&rsbcount,(char *)"rsb_count");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 255 "dsplkres.qsc" */	/* putform */
      {
        if (IIfsetio(Fmorelkres) != 0) {
          IIputfldio((char *)"invalid",(short *)0,1,32,0,cinvalid);
          IIputfldio((char *)"value",(short *)0,1,32,0,cvalue);
          IIputfldio((char *)"rsb_count",(short *)0,1,30,4,&rsbcount);
        } /* IIfsetio */
      }
/* # line 257 "dsplkres.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 258 "dsplkres.qsc" */	/* submenu */
      {
        goto IImuI3;
IIfdB3:
        IIrunmu(0);
        if (IIretval() == 1) {
          {
/* # line 261 "dsplkres.qsc" */	/* breakdisplay */
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
/* # line 265 "dsplkres.qsc" */	/* host code */
    return;
}
/*
** Routine: printlkres() - this routine gets the resource information,
**	formats it and puts it onto the screen.  If output is to a file,
**	this routine writes it there instead.
**
** Inputs: none
**
** Output: data written to screen or file
**
** Returns: none
**
** History
**	3/15/89		tmt	written
**	10/1/89		tomt	integrated 6.3 changes
**	10/2/89		tomt	need to check if there are no db resources
**				    and open up a db if necessary
**	10/5/89		tomt	made various fixes to filtering and formatting
**				code
**	10 Aug 1991	jpk	modified to handle display of new lock types;
**				makes it more robust as well.
**      24/4/95         nick    Added LK_CKP_TXN
**	11/1/95		nick	Change display of LK_CONTROL.
**				Added LK_ROW
**				Added LK_AUDIT
**      4-oct-1995      canor01 close open databases on exit (B71791)
**	22-nov-1996	dilma04 Added LK_PH_PAGE and LK_VAL_LOCK for row 
**	                        level loking project.
**      16-oct-1998     stial01 Correctly print last element of Lock_type array
**      14-dec-1998     nanpr01 Correctly print Value locks.
**	05-Mar-2002 (jenjo02)
**	    Added LK_SEQUENCE
**	17-Aug-2007 (kibro01) b118930
**	    If the owner or tablename is a hashed value (i.e. the string
**	    was >8 characters for username or >12 characters for tablename)
**	    then simply print hashes so it doesn't look like the locklist is
**	    corrupted - the values in key[n] are integers rather than a string.
*/
VOID
printlkres()
{
  int counter;
  char id[HEXID_SIZE + 1], lock_tp[15];
  char value[17], ckey[200], tempbuf[25];
  char tab_name[FE_MAXNAME + 1], oldtab_name[FE_MAXNAME + 1];
  char crtblname[13], crownname[9];
    char	dbbuf[FE_MAXNAME + 1];		/* for printing dbname */
  char *dbname = dbbuf;
    char	olddbname[FE_MAXNAME + 1];
    bool	db_name_changed = FALSE;	/* flags unexpected change */
    bool	reset = TRUE;		/* always start at list begin */
    i4		lock_catagory;
    LK_RSB_INFO *rsb;
  char gr_str[5];
  i4 rsb_count;
  char cv_str[5];
  i4 rsb_invalid;
    /* Get the Info */
    dbbuf[0] = EOS;
    olddbname[0] = EOS;
    counter = 0;
    while ((rsb = (LK_RSB_INFO *)l_retrieve(RESINFO, 0, &reset)) != NULL) 
    {
	/*
	** Filter out locks that the user does NOT want printed.  If they asked
	** for certain lock types (table, database, etc...) we check now.
	*/
	counter++;	/* keep track of element number */
	if (flag_locktype && (ing_locktype != rsb->rsb_key[0]))
	{
	    continue;
	}
	/* do formatting for display */
	STprintf(id, "%x", rsb->rsb_id);
	STprintf(value, "%x%08x", rsb->rsb_value[0], rsb->rsb_value[1]);
	if (rsb->rsb_key[0] <= Lock_type_size)
	    STcopy(Lock_type[rsb->rsb_key[0]], lock_tp);
	else						/* new lock type! */
	    STprintf(lock_tp, "<%d>", rsb->rsb_key[0]);
        lock_catagory = getlockcat(rsb->rsb_key[0]); /* get general catagory */
	switch(rsb->rsb_key[0]) {
	    case LK_DATABASE:
	    case LK_SV_DATABASE:
	    case LK_OWNER_ID:
	    case LK_JOURNAL:
	    case LK_DB_TEMP_ID:
	    case LK_CONFIG:
	    case LK_OPEN_DB:
	    case LK_CKP_CLUSTER:
	    case LK_BM_DATABASE:
		/* 
		** Open the next database, if necessary, because you are going
		** to have to print out relation name.  If the user wants only
		** DB locks, do NOT open up databases
		*/
		if (rsb->rsb_key[0] == LK_BM_DATABASE)
		    (VOID)get_db_name(dbname, rsb->rsb_key[1]);
		else
		    STcopy((char *) &rsb->rsb_key[1], dbname);
		STprintf(ckey, "%-32s", dbname);
		if (!(flag_locktype &&
		    (getlockcat(ing_locktype) == DATABASE_LOCK)))
		{
		    if (STcompare(olddbname, dbname) != 0)
		    {
			close_db();
			open_db(dbname);	/* open a database */
		        STcopy(dbname, olddbname);
		    }
		}
		break;
	    case LK_CONTROL:
		string_move(tempbuf, (PTR) &rsb->rsb_key[1], LK_KEY_LENGTH);
		tempbuf[LK_KEY_LENGTH] = EOS;	/* null terminate */
		STprintf(ckey, "SYS CONTROL: %s", tempbuf);
		break;
	    case LK_TABLE:
	    case LK_BM_TABLE:
	    case LK_SV_TABLE:
	    case LK_EXTEND_FILE:
	    case LK_TBL_CONTROL:
		/* 
		** Open the next database, if necessary, because you are going
		** to have to print out relation name.  Ignore if user wants
		** only database locks.
		*/
		(VOID)get_db_name(dbname, rsb->rsb_key[1]);
		if (STcompare(olddbname, dbname) != 0) /* different */
		{
		    close_db();
		    open_db(dbname);	/* open a database */
		    STcopy(dbname, olddbname);
		    db_name_changed = TRUE;
		}
		else
		    db_name_changed = FALSE;
		(VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
		STcopy(tab_name, oldtab_name);
		if (db_name_changed)
		{
		    STprintf(ckey, "%s TABLE: %s", dbname, tab_name);
		}
		else
		{
		    STprintf(ckey, "     %-32s", tab_name);
		}
		break;
	    case LK_PAGE:
	    case LK_BM_PAGE:
            case LK_PH_PAGE:	
		/* 
		** Open the next database, if necessary, because you are going
		** to have to print out relation name.  Ignore if user wants
		**  only database locks.
		*/
		(VOID)get_db_name(dbname, rsb->rsb_key[1]);
		if (STcompare(olddbname, dbname) != 0) /* different */
		{
		    close_db();
		    open_db(dbname);	/* open a database */
		    STcopy(dbname, olddbname);
		    db_name_changed = TRUE;
	        }
		else
		    db_name_changed = FALSE;
		(VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
		if (!STcompare(tab_name, oldtab_name))
		{ /* only print page #*/
		    STprintf(ckey, "%44d", rsb->rsb_key[4]);
		}
		else if (db_name_changed)
		{
		    STprintf(ckey, "%s TABLE: %s PAGE: %d", dbname,
			tab_name, rsb->rsb_key[4]);
		}
		else
		{
		    STprintf(ckey, "     %-32s %6d", tab_name, rsb->rsb_key[4]);
		}
		STcopy(tab_name, oldtab_name);	/* for next time */
		break;
	    case LK_AUDIT:
		if (rsb->rsb_key[1] == SXAS_LOCK)
		{
		    if (rsb->rsb_key[2] == SXAS_STATE_LOCK)
			STprintf(ckey, "AUDIT: State(primary)");
		    else if (rsb->rsb_key[2] == SXAS_SAVE_LOCK)
			STprintf(ckey, "AUDIT: State(save)");
		    else
			STprintf(ckey, "AUDIT: State(oper=%d)", 
				rsb->rsb_key[2]);
		}
		else if (rsb->rsb_key[1] == SXAP_LOCK)
		{
		    if (rsb->rsb_key[2] == SXAP_SHMCTL)
			STprintf(ckey, 
			    "AUDIT: Physical_layer(shmctl, node=%x)",
			    rsb->rsb_key[3]);
		    else if (rsb->rsb_key[2] == SXAP_ACCESS)
			STprintf(ckey, "AUDIT: Physical_layer(access)");
		    else if (rsb->rsb_key[2] == SXAP_QUEUE)
			STprintf(ckey,
			    "AUDIT: Physical_layer(queue, node=%x)",
			    rsb->rsb_key[3]);
		    else
			STprintf(ckey, "AUDIT: Physical_layer(oper=%d)",
			    rsb->rsb_key[2]);
		}
		else
		{
		    STprintf(ckey, "AUDIT: TYPE=%d, OPER=%d", rsb->rsb_key[1],
			rsb->rsb_key[2]);
		}
		break;
	    case LK_ROW:
		(VOID)get_db_name(dbname, rsb->rsb_key[1]);
		if (STcompare(olddbname, dbname) != 0) /* different */
		{
		    close_db();
		    open_db(dbname);	/* open a database */
		    STcopy(dbname, olddbname);
		    db_name_changed = TRUE;
	        }
		else
		{
		    db_name_changed = FALSE;
		}
		(VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
		if (!STcompare(tab_name, oldtab_name))
		{ /* only print page # and row # */
		    STprintf(tempbuf, "%d[%d]", rsb->rsb_key[4],
			rsb->rsb_key[5]);
		    STprintf(ckey, "%44s", tempbuf);
		}
		else if (db_name_changed)
		{
		    STprintf(ckey, "%s TABLE: %s PAGE: %d ROW: %d", dbname,
			tab_name, rsb->rsb_key[4], rsb->rsb_key[5]);
		}
		else
		{
		    STprintf(tempbuf, "%d[%d]", rsb->rsb_key[4],
			rsb->rsb_key[5]);
		    STprintf(ckey, "     %-26s %12s", tab_name, tempbuf);
		}
		STcopy(tab_name, oldtab_name);	/* for next time */
		break;
	    case LK_SS_EVENT:
		STprintf(ckey, "SERVER = %x,[%x,%x])",
		    rsb->rsb_key[1], rsb->rsb_key[2], rsb->rsb_key[3]);
		break;
	    case LK_EVCONNECT:
		STprintf(ckey, "Server PID = %d", rsb->rsb_key[1]);
		break;
	    case LK_CKP_TXN:
		if ((rsb->rsb_key[3] != NULL) && (rsb->rsb_key[1] != NULL))
		{
		    string_move(crtblname, (PTR) &rsb->rsb_key[3], 16);
		    string_move(crownname, (PTR) &rsb->rsb_key[1], 8);
		    crtblname[12] = crownname[8] = NULL;
		}
		else
		{
		    crtblname[0] = crownname[0] = NULL;
		}
		STprintf(tempbuf, "%s-%s", crtblname, crownname);
		STzapblank(tempbuf, tempbuf);
		STprintf(ckey, "     NAME = ('%s')", tempbuf);
		break;
	    case LK_CKP_DB:
	    case LK_CREATE_TABLE:
	    case LK_SEQUENCE:
		/* 
		** Open the next database, if necessary, because you are going
		** to have to print out relation name.  Ignore if user wants
		** only database locks.
		*/
		(VOID)get_db_name(dbname, rsb->rsb_key[1]);
		if (STcompare(olddbname, dbname) != 0) /* different */
		{
		    close_db();
		    open_db(dbname);	/* open a database */
		    STcopy(dbname, olddbname);
		    db_name_changed = TRUE;
	        }
		else
		{
		    db_name_changed = FALSE;
		}
		/*
		** The values needed for display are stored as non null
		** terminated strings.  That's handled here.  Also, sometimes
		** the pointers are null, so we check that also
		*/
		if ((rsb->rsb_key[4] != NULL) && (rsb->rsb_key[2] != NULL))
		{
		    /* If the top byte of the key is 0, that means this is
		    ** a hashed value - see HSH_char for the algorithm.
		    ** (HSH_char always leaves the top byte set to 0)
		    ** There is nothing useful we can print here since we
		    ** cannot get back to the name (kibro01) b118930
		    */
		    if ((rsb->rsb_key[4] & 0xff000000) == 0)
			string_move(crtblname, "############", 12);
		    else
			string_move(crtblname, (PTR) &rsb->rsb_key[4], 12);
		    if ((rsb->rsb_key[2] & 0xff000000) == 0)
			string_move(crtblname, "########", 8);
		    else
			string_move(crownname, (PTR) &rsb->rsb_key[2], 8);
		    crtblname[12] = crownname[8] = NULL;
		}
		else
		{
		    crtblname[0] = crownname[0] = NULL;
		}
		STprintf(tempbuf, "%s-%s", crtblname, crownname);
		STzapblank(tempbuf, tempbuf);
		if (db_name_changed)
		{
		    STprintf(ckey, "%s, NAME = ('%s')", dbname, tempbuf);
		}
		else
		{
		    STprintf(ckey, "     NAME = ('%s')", tempbuf);
		}
		break;
	    case LK_BM_LOCK:
		string_move(tempbuf, (PTR) &rsb->rsb_key[1], HEXID_SIZE);
		tempbuf[HEXID_SIZE] = EOS;	/* null terminate */
		STprintf(ckey, "Buffer Manager = '%s'", tempbuf);
		d_print(ckey);
		break;
            case LK_VAL_LOCK:
                (VOID)get_db_name(dbname, rsb->rsb_key[1]);
                (VOID)get_rel_name(tab_name, rsb->rsb_key[2], rsb->rsb_key[3]);
                STprintf(ckey, "%s, %s, VALUE: <%d,%d,%d>",
                    dbname, tab_name, rsb->rsb_key[4], rsb->rsb_key[5],
                    rsb->rsb_key[6]);
                break;
	    default:
		STprintf(ckey, "%d, (%x,%x,%x,%x,%x,%x)",
		    rsb->rsb_key[0], rsb->rsb_key[1],
                    rsb->rsb_key[2], rsb->rsb_key[3],
		    rsb->rsb_key[4], rsb->rsb_key[5], rsb->rsb_key[6]); 
	}
	/*
	** If user wants to view a specific table, filter out all other locks
	** here. If the current table is not the one we want, don't put it
	** into the tblfld. Note this is a special case of specifying a
	** specific locktype (table) except we let all locks under it pass too
	*/
	if (flag_table &&
	    (((lock_catagory != TABLE_LOCK) && (lock_catagory != PAGE_LOCK) &&
              (lock_catagory != ROW_LOCK) && (lock_catagory != VALUE_LOCK)) || 
	    (STcompare(ptr_tablename, tab_name) != 0)))
	    {
		continue;
	    }
	/*
	** This checks whether or not the user wants null locks printed or not.
	** flag_nl is set when null locks are to be printed.
	*/
	if (!flag_nl && (rsb->rsb_convert == 0) && (rsb->rsb_grant == 0))
	{
	    continue;
	}
	STcopy(lock_itoa(rsb->rsb_grant), gr_str);
	STcopy(lock_itoa(rsb->rsb_convert), cv_str);
	rsb_invalid = rsb->rsb_invalid;
	rsb_count = rsb->rsb_count;
/* # line 668 "dsplkres.qsc" */	/* loadtable */
  {
    if (IItbact(Fdisplkres,(char *)"reslist",1) != 0) {
      IItcoputio((char *)"id",(short *)0,1,32,0,id);
      IItcoputio((char *)"grant",(short *)0,1,32,0,gr_str);
      IItcoputio((char *)"convert",(short *)0,1,32,0,cv_str);
      IItcoputio((char *)"value",(short *)0,1,32,0,value);
      IItcoputio((char *)"invalid",(short *)0,1,32,0,Invalid[rsb_invalid]);
      IItcoputio((char *)"key",(short *)0,1,32,0,ckey);
      IItcoputio((char *)"row_num",(short *)0,1,30,4,&counter);
      IItcoputio((char *)"lock_type",(short *)0,1,32,0,lock_tp);
      IItcoputio((char *)"rsb_count",(short *)0,1,30,4,&rsb_count);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 674 "dsplkres.qsc" */	/* host code */
    }
    close_db();
    return;
}
