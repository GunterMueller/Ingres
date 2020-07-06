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
**  File: dsplogdb.qc
**
**  Purpose - this file contains the routines which display the active dbs
**	(in logging system) information form.  
**
**  This file contains:
**	displgdb() - displays a list of dbs in the logging system form. 
**	printlgdb() - gets, formats and outputs logging system db information.
**	get_logdb_name() - given a logging db id, returns the name.
**
**  History
**	8/29/89		tomt	created
**	9/4/89		tomt	added code to display log db info
**	9/5/89		tomt	added get_logdb_name() routine
**	10/1/89		tomt	integrated 6.3 changes
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	 1-Nov-1995 (nick)
**		Remove unused variable.
**      15-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's change (from 6.4) for axp_osf
**              9/10/93 kchin   changed type of lforce, and lwait from long to
**                              i4.  This is to fix problems when running
**                              on 64-bit platform such as Alpha OSF/1, in
**                              this case long is 64-bit, which is no longer
**                              similar in size to 32-bit int, thus mixing
**                              them will result in error, moreover, 64-bit
**                              integer is not supported by SQL/QUEL yet.
**      08-Dec-1997 (hanch04)
**          Use new la_block field when block number is needed instead
**          of calculating it from the offset
**          Keep track of block number and offset into the current block.
**          for support of logs > 2 gig
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
#define		LOC_MAXSIZE		(DB_MAXNAME * 2)
/*
** forward declarations
*/
VOID printlgdb();
VOID get_lklgstats();
VOID morelgdb();
VOID stat_decode();
FUNC_EXTERN VOID tablefield_find();
FUNC_EXTERN VOID string_move();
/*
** Declare file wide statics
*/
  static char Fdisplgdb[]= "fdisplgdb";
  static char Fmorelgdb[]= "fmorelgdb";
static char Lgdbstat[] = "INVALID,JOURNAL,NOTDB,PURGE,OPENDB_PEND,\
CLOSEDB_PEND,RECOVER,FAST_COMMIT,PRETEND_CONSISTENT,CLOSEDB_DONE,BACKUP,\
STALL,EBACKUP,FBACKUP,CKPDB_PEND";
/*
** Routine: displgdb()  displays a list of databases in the logging system
**	    on a popup form
**
** Inputs: none
**
** Outputs: displays a form
**
** Side effects: assumes the forms system is active
*/
VOID
displgdb()
{
    bool	e_xit = FALSE;
  int list_count;
  char colname[FE_MAXNAME + 1];
    static bool	forms_added = FALSE;
  GLOBALREF int *fdisplgdb;
  GLOBALREF int *fmorelgdb;
    /*
    **  Determine if forms are added, if not addform them.  Also
    **  don't add forms if output is being directed to a file
    */
    if (!forms_added)
    {
/* # line 124 "dsplogdb.qsc" */	/* addform */
  {
    IIaddform(fmorelgdb);
  }
/* # line 125 "dsplogdb.qsc" */	/* addform */
  {
    IIaddform(fdisplgdb);
  }
/* # line 126 "dsplogdb.qsc" */	/* host code */
	forms_added = TRUE;
    }
    /*
    **  Display the form and get the information
    */
/* # line 132 "dsplogdb.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fdisplgdb,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 137 "dsplogdb.qsc" */	/* inittable */
      {
        if (IItbinit(Fdisplgdb,(char *)"dblist",(char *)"r") != 0) {
          IIthidecol((char *)"db_id",(char *)"c8");
          IIthidecol((char *)"db_owner",(char *)"varchar(32)");
          IIthidecol((char *)"force",(char *)"i4");
          IIthidecol((char *)"wait",(char *)"i4");
          IIthidecol((char *)"location",(char *)"varchar(64)");
          IIthidecol((char *)"lga",(char *)"varchar(50)");
          IIthidecol((char *)"backup",(char *)"varchar(50)");
          IItfill();
        } /* IItbinit */
      }
/* # line 141 "dsplogdb.qsc" */	/* host code */
	printlgdb();	/* Put the info in the tablefield */
	/* get number of dbs in logging system */
/* # line 144 "dsplogdb.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,Fdisplgdb,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)"dblist",0);
        } /* IIiqset */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 149 "dsplogdb.qsc" */	/* host code */
	/* Get lock resource info from INGRES */
/* # line 150 "dsplogdb.qsc" */	/* clear */
          {
            IIfldclear((char *)"dblist");
          }
/* # line 151 "dsplogdb.qsc" */	/* message */
          {
            IImessage((char *)"Loading logging information...");
          }
/* # line 152 "dsplogdb.qsc" */	/* host code */
	get_lklgstats(GET_LOGDBLIST, (PTR) 0);
	printlgdb();	/* put the info in the tablefield */
	/* get number of dbs */
/* # line 157 "dsplogdb.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,Fdisplgdb,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)"dblis\
t",0);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 161 "dsplogdb.qsc" */	/* host code */
	if (list_count == 0) {
	    POP_MSG("There are no databases to get information on");
/* # line 163 "dsplogdb.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 164 "dsplogdb.qsc" */	/* host code */
	}
	morelgdb();
        }
      } else if (IIretval() == 3) {
        {
/* # line 169 "dsplogdb.qsc" */	/* host code */
	if (list_count == 0) {
	    POP_MSG("There is nothing to find");
/* # line 171 "dsplogdb.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 172 "dsplogdb.qsc" */	/* host code */
	}
/* # line 173 "dsplogdb.qsc" */	/* inquire_frs */
          {
            if (IIiqset(4,0,Fdisplgdb,(char *)"dblist",(char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,colname,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 174 "dsplogdb.qsc" */	/* host code */
	tablefield_find();
/* # line 175 "dsplogdb.qsc" */	/* resume */
          {
            IIrescol((char *)"dblist",colname);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 179 "dsplogdb.qsc" */	/* host code */
	my_help("ipmdplgd.hlp", "Log Database");
        }
      } else if (IIretval() == 5) {
        {
/* # line 183 "dsplogdb.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 187 "dsplogdb.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
/* # line 188 "dsplogdb.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 189 "dsplogdb.qsc" */	/* breakdisplay */
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
/* # line 192 "dsplogdb.qsc" */	/* host code */
    if (e_xit) {		/* user wants to leave */
	terminate(OK);
    }
    return;
}
/*
** morelgdb() - This routine displays the popup form which show the
**	less needed database info.
**
** History
**	8/29/89		tomt	written
**	9/4/89		tomt	added code to display info
**	10/1/89		tomt	integrated 6.3 changes
**
*/
VOID
morelgdb()
{
/*
** Declare Variables
*/
  char dbowner[FE_MAXNAME + 1], loc[LOC_MAXSIZE + 1], llga[51], dbid
  [HEXID_SIZE + 1];
  char back_up[51];
  i4 lforce, lwait;
/* # line 219 "dsplogdb.qsc" */	/* display */
  {
    if (IIdispfrm(Fmorelgdb,(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 223 "dsplogdb.qsc" */	/* host code */
	/*
	**   Get the values off of the tablefield on the base form and
	**   place them on the current form
	*/
/* # line 227 "dsplogdb.qsc" */	/* getrow */
      {
        if (IItbsetio(2,Fdisplgdb,(char *)"dblist",-2) != 0) {
          IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,dbowner,(char *)
"db_owner");
          IItcogetio((short *)0,1,32,LOC_MAXSIZE + 1-1,loc,(char *)
"location");
          IItcogetio((short *)0,1,32,50,llga,(char *)"lga");
          IItcogetio((short *)0,1,30,4,&lforce,(char *)"force");
          IItcogetio((short *)0,1,30,4,&lwait,(char *)"wait");
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,dbid,(char *)"db\
_id");
          IItcogetio((short *)0,1,32,50,back_up,(char *)"backup");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 230 "dsplogdb.qsc" */	/* putform */
      {
        if (IIfsetio(Fmorelgdb) != 0) {
          IIputfldio((char *)"db_id",(short *)0,1,32,0,dbid);
          IIputfldio((char *)"owner",(short *)0,1,32,0,dbowner);
          IIputfldio((char *)"force",(short *)0,1,30,4,&lforce);
          IIputfldio((char *)"wait",(short *)0,1,30,4,&lwait);
          IIputfldio((char *)"location",(short *)0,1,32,0,loc);
          IIputfldio((char *)"lga",(short *)0,1,32,0,llga);
          IIputfldio((char *)"backup",(short *)0,1,32,0,back_up);
        } /* IIfsetio */
      }
/* # line 232 "dsplogdb.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 233 "dsplogdb.qsc" */	/* submenu */
      {
        goto IImuI3;
IIfdB3:
        IIrunmu(0);
        if (IIretval() == 1) {
          {
/* # line 236 "dsplogdb.qsc" */	/* breakdisplay */
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
/* # line 239 "dsplogdb.qsc" */	/* host code */
    return;
}
/*
** printlgdb() - this routine gets, formats and outputs the logging system
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
**	9/4/89		tomt	added code to display info
**	9/8/89		tomt	fixed so db list traversal begins at beginning
**				of list
**	10/1/89		tomt	integrated 6.3 changes
**	30-July-1993	jpk	update LG_DATABASE structure member names
**				to reflect LGshow interface change.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
*/
VOID
printlgdb()
{
  char db_name[FE_MAXNAME + 1], db_stat[41];
  char db_own[FE_MAXNAME + 1], lga[51];
  char location[LOC_MAXSIZE + 1], db_id[HEXID_SIZE + 1], backup[51];
    u_i4	bgn1, mid1, end1, bgn2, mid2, end2;
    PTR		bufptr;		/* used to step thru ldb->buffer string */
    u_i4	length;
    bool	reset = TRUE;	/* start at beginning of list */
    LG_DATABASE	*ldb;
    LG_HEADER	header;
  i4 db_stat_trans;
  i4 db_stat_read;
  i4 db_stat_write;
  i4 db_stat_begin;
  i4 db_stat_end;
  i4 db_stat_force;
  i4 db_stat_wait;
    /*
    **    Need header info for calculating journal window size
    */
    get_lklgstats(GET_LOGHEADER, (PTR) &header);
    while ((ldb =
	(LG_DATABASE *)l_retrieve(LOGDBINFO, 0, &reset)) != NULL)
    {
	/* do formatting for display */
	STprintf(db_id, "%x", ldb->db_id);
	/* info is a substr of str */
 	string_move(db_name, (PTR) ldb->db_buffer, (i4) INGSIZE);
 	db_name[INGSIZE] = EOS;
 	bufptr = (PTR)&ldb->db_buffer[INGSIZE];	/* move pointer to next item */
 	string_move(db_own, (PTR) bufptr, (i4) INGSIZE);
 	db_own[INGSIZE] = EOS;
	stat_decode(db_stat, Lgdbstat, (u_i4) ldb->db_status);
 	/* len of location (counted string) */
 	length = *(i4 *)&ldb->db_buffer[DB_MAXNAME + DB_MAXNAME + 4];
 	if (length > LOC_MAXSIZE)
 	{
 	    length = LOC_MAXSIZE;
 	}
 	STlcopy(&ldb->db_buffer[DB_MAXNAME + DB_MAXNAME + 4 + sizeof(i4)],
 	    location, length);
	bgn1 = ldb->db_f_jnl_la.la_sequence;
	mid1 = ldb->db_f_jnl_la.la_block,
	end1 = ldb->db_f_jnl_la.la_offset;
	bgn2 = ldb->db_l_jnl_la.la_sequence;
	mid2 = ldb->db_l_jnl_la.la_block;
	end2 = ldb->db_l_jnl_la.la_offset;
	STprintf(lga, "<%d,%d,%d>..<%d,%d,%d>",
	    bgn1, mid1, end1, bgn2, mid2, end2);
	bgn1 = ldb->db_sbackup.la_sequence;
	mid1 = ldb->db_sbackup.la_block;
	end1 = ldb->db_sbackup.la_offset;
	STprintf(backup, "<%d,%d,%d>", bgn1, mid1, end1);
	if (ldb->db_status == 0)  /* just in case, make it clear on scr */
	    STcopy("<none>", db_stat);
	db_stat_trans = ldb->db_stat.trans;
	db_stat_read = ldb->db_stat.read;
	db_stat_write = ldb->db_stat.write;
	db_stat_begin = ldb->db_stat.begin;
	db_stat_end = ldb->db_stat.end;
	db_stat_force = ldb->db_stat.force;
	db_stat_wait = ldb->db_stat.wait;
/* # line 342 "dsplogdb.qsc" */	/* loadtable */
  {
    if (IItbact(Fdisplgdb,(char *)"dblist",1) != 0) {
      IItcoputio((char *)"db_id",(short *)0,1,32,0,db_id);
      IItcoputio((char *)"db_name",(short *)0,1,32,0,db_name);
      IItcoputio((char *)"db_s",(short *)0,1,32,0,db_stat);
      IItcoputio((char *)"trans",(short *)0,1,30,4,&db_stat_trans);
      IItcoputio((char *)"read",(short *)0,1,30,4,&db_stat_read);
      IItcoputio((char *)"write",(short *)0,1,30,4,&db_stat_write);
      IItcoputio((char *)"begin",(short *)0,1,30,4,&db_stat_begin);
      IItcoputio((char *)"end",(short *)0,1,30,4,&db_stat_end);
      IItcoputio((char *)"db_owner",(short *)0,1,32,0,db_own);
      IItcoputio((char *)"force",(short *)0,1,30,4,&db_stat_force);
      IItcoputio((char *)"wait",(short *)0,1,30,4,&db_stat_wait);
      IItcoputio((char *)"location",(short *)0,1,32,0,location);
      IItcoputio((char *)"lga",(short *)0,1,32,0,lga);
      IItcoputio((char *)"backup",(short *)0,1,32,0,backup);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 349 "dsplogdb.qsc" */	/* host code */
    }
    return;
}
/*
** get_logdb_name - this routine finds the name of a database given
**	a database ID.  If it can't find it, a default string is returned
**
** Inputs:
**	db_name - a ptr to a buffer where the name is placed
**	db_id - the db id for which to get the db name
**
** Outputs:
**	the db name placed where db_name points
**
** Returns:
**	none
**
** History:
**	9/5/89		tomt	written
**	12/13/92	jpk	support XA transactions, DB_MAXNAME
*/
VOID
get_logdb_name(db_name, db_id)
PTR db_name;
LG_DBID db_id;
{
    bool		found = FALSE;	/* set if db found */
    bool		reset = TRUE;	/* start at beginning of list */
    LG_DATABASE	*ldb;
    /*
    ** find the matching dbname
    */
    while ((ldb =
	(LG_DATABASE *)l_retrieve(LOGDBINFO, 0, &reset)) != NULL)
    {
	/*
	**    Check if db_ids match
	*/
	if (ldb->db_id == db_id)
	{
 	    string_move(db_name, (PTR) ldb->db_buffer, (i4) INGSIZE); /* info is a substr of str */
 	    db_name[INGSIZE] = EOS;
	    found = TRUE;
	    break;
	}
    }
    if (!found)
	STcopy("<none found>", db_name);
    return;
}
