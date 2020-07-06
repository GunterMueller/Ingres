# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
#include <compat.h>
#include <si.h>
#include <cs.h>
#include <st.h>
#include <dbms.h>
#include <pc.h>
#include <lg.h>
#include <lk.h>
#include <fe.h>
#include <er.h>
#include        <adf.h>
#include        <ft.h>
#include        <fmt.h>
#include        <frame.h>
#include "lists.h"
#include "msgs.h" 
#include "dba.h"
/*
** Forward and external references
*/
VOID get_lklgstats();
VOID printlghdr();
VOID printlghlgx();
VOID morelghdr();
VOID morelghdtx();
FUNC_EXTERN VOID stat_decode();
FUNC_EXTERN VOID tablefield_find();
#define DIAGRAM_SIZE 70  /* must change WHEN the field width on form changes */
#define BOFEOF_MARKER '>'	/* delineates eof/bof in log_diagram */
#define NORANGE_MARKER '*'	/* when bof = eof therefor no range */
#define RANGE_MARKER '-'	/* delineates space inbetween bof&eof */
#define EXAMINE_MARKER '^'	/* delineates what block is being examined */
#define RESERVED_MARKER '+'	/* delineates reserved space */
#define	SHOW_RESERVED	TRUE
/*
** Declare file wide vars
**
** Statics of interest:
**	log_diagram - this contains the diagram of the log file displayed
**	    on the form.  It is static because it needs to be manipulated
**	    in the Examine submenu - i.e. highlighting the block that is
**	    being examined for transactions.  It is alos built up in the
**	    printlghdr routine.
**	start_position - this indicates the logical beginning of file
**	    in the log_diagram variable.  This is the array position of
**	    the logical begin of the log file.  This variable is set in
**	    the printlghdr routine and used in the Examine menuitem in
**	    the showlghdr routine.
**	end_position - this indicates the logical end of file
**	    in the log_diagram variable.  This is the array position of
**	    the logical end of the log file.  This variable is set in
**	    the printlghdr routine and used in the Examine menuitem in
**	    the showlghdr routine.
**	reserved_position - this indicates the imaginary position
**	    in the log_diagram variable where reserved space starts.
**	    Between reserved_position and start_position, space will
**	    be displayed as that reserved for use by the recovery system.
**	pages_per_block - how many log file pages are represented by
**	    one block in the log_diagram (scaling factor)
**	eof/bof - end of file and beginning of file are needed in the
**	    examine submenu in showlghdr() but are calculated in printlghdr()
*/
  static char Fshowlghdr[]= "fshowlghdr";
  static char Fmorelghdtx[]= "fmorelghdtx";
  static char Fmorelghdr[]= "fmorelghdr";
  static char *Title1 = "Transactions occuring in";
  static char *Title2 = "the log file between pages";
  static char *Title3 = "are";
  static char *Title4 = "displayed in the scrolling";
  static char *Title5 = "area to the right ------->";
  static char *Title6 = "Transaction ID   Session      Database";
  static char log_diagram[DIAGRAM_SIZE + 1];
  static int start_position, end_position, reserved_position;
  static double pages_per_block;
  static i4 ii_eof, bof;
/*
** List Constants used by log display routines
*/
static char Lghdrstat[] = "ONLINE,CPNEEDED,LOGFULL,FORCE_ABORT,RECOVER,\
ARCHIVE,ACP_SHUTDOWN,IMM_SHUTDOWN,START_ARCHIVER,PURGEDB,\
OPEN_DB,CLOSE_DB,START_SHUTDOWN,BCPDONE,CPFLUSH,ECP,ECPDONE,\
CPWAKEUP,BCPSTALL,CKP_SBACKUP,,,MAN_ABORT,MAN_COMMIT,RCP_RECOVER,,COPY_STALL,DUAL_LOGGING,DISABLE_DUAL_LOGGING,EMER_SHUTDOWN";
/* this long line needed to work around esql/c bug */
/*
**
**  File: sholghdr.qc
**
**  Purpose - this file contains the routines which display logging
**	system header form.  
**
**  This file contains:
**	showlghdr - displays the logging system header information form. 
**	printlghdr - puts the values on the form
**	morelghdtx - displays more info for examining log file transactions.
**      morelghdr - displays more info for the log header screen
**
**  History
**	8/27/89		tomt	created
**	9/2/89		tomt	added display code
**	9/6/89		tomt	added bargraph for %logfile used.
**				added autorefresh capabilities.
**	9/29/89		tomt	added blocks in use field
**	10/1/89		tomt	integrated 6.3 changes
**	3/15/90		tomt	added blocks_free field and changed form
**	3/17/90		tomt	changed percent display to diagram of log file
**	4/2/90		tomt	added more info form for examining transactions
**	9/12/90		tomt	added more info form for log header info
**				and also changed appearance of log header
**				screen
**	3/21/91		tomt	changed title on form to trim.  Removed
**                              variable used to hold title.
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	4-Jan-1994 (jpk)
**		Updated Lghdrstat to add logging system statuses
**		new in 6.5.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	 1-nov-95 (nick)
**		Removed flag_file.
**	24-nov-95 (nick)
**		Fix #58838 ; ensure the calculated current_position
**		remains within the log diagram array's bounds.
**	18-mar-96 (mckba01) 
**		#70619, modified 'printlghdr' so that a calculation
**		is not performed if the logging system is not online. 
**	20-jan-97 (mcgem01)
**		eof is reserved when using Micorsoft Visual/C++
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
**	18-sep-2003 (abbjo03)
**	    Include pc.h ahead of lg.h.
*/
VOID
showlghdr()
{
    bool	e_xit = FALSE, refresh = TRUE;
  int timeout, diagram_column;
  int curcolumn, current_position, old_position;
  int low_page, high_page, list_count;
  int on_valid_area = FALSE;
  char field_name[FE_MAXNAME + 1];
    static i4	forms_added = FALSE;
  GLOBALREF int *fshowlghdr;
  GLOBALREF int *fmorelghdtx;
  GLOBALREF int *fmorelghdr;
    /*
    **  Determine if forms are added, if not addform them.  Also
    **  don't add forms if output is being directed to a file
    */
    if (forms_added == FALSE)
    {
/* # line 181 "sholghdr.qsc" */	/* addform */
  {
    IIaddform(fshowlghdr);
  }
/* # line 182 "sholghdr.qsc" */	/* addform */
  {
    IIaddform(fmorelghdtx);
  }
/* # line 183 "sholghdr.qsc" */	/* addform */
  {
    IIaddform(fmorelghdr);
  }
/* # line 184 "sholghdr.qsc" */	/* host code */
	forms_added = TRUE;
    }
    /* Set the refresh interval */
    timeout = frs_timeout;		/* place into local frs var */
/* # line 189 "sholghdr.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
    } /* IIiqset */
  }
/* # line 191 "sholghdr.qsc" */	/* host code */
/* ##  DISPLAY Fshowlghdr READ		-- no longer this */
    /* Display the form -- once again a popup */
/* # line 193 "sholghdr.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(2),(void *)IILQint(2));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fshowlghdr,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 197 "sholghdr.qsc" */	/* host code */
	/* Put information on the form.  some of it must be formatted first */
/* # line 198 "sholghdr.qsc" */	/* putform */
      {
        if (IIfsetio(Fshowlghdr) != 0) {
          IIputfldio((char *)"title1",(short *)0,1,32,0,Title1);
          IIputfldio((char *)"title2",(short *)0,1,32,0,Title2);
          IIputfldio((char *)"title3",(short *)0,1,32,0,Title3);
          IIputfldio((char *)"title4",(short *)0,1,32,0,Title4);
          IIputfldio((char *)"title5",(short *)0,1,32,0,Title5);
          IIputfldio((char *)"title6",(short *)0,1,32,0,Title6);
        } /* IIfsetio */
      }
/* # line 201 "sholghdr.qsc" */	/* host code */
	current_position = 0;
	old_position = 0;
	printlghdr();	/* put the values on the form */
	refresh_status(VISIBLE, "flogmenu");
/* # line 205 "sholghdr.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 206 "sholghdr.qsc" */	/* resume */
      {
        IIresmu();
        if (1) goto IIfdB1;
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 210 "sholghdr.qsc" */	/* host code */
	printlghdr();	/* put the values on the form */
/* # line 211 "sholghdr.qsc" */	/* resume */
          {
            IIresmu();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 215 "sholghdr.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 216 "sholghdr.qsc" */	/* host code */
	morelghdr();
	/*  Set the refresh interval */
	timeout = frs_timeout;		/* place into local frs var */
/* # line 219 "sholghdr.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 223 "sholghdr.qsc" */	/* host code */
	printlghdr();	/* put the values on the form */
        }
      } else if (IIretval() == 4) {
        {
/* # line 227 "sholghdr.qsc" */	/* display */
          {
            if (IInestmu() == 0) goto IIfdE2;
            goto IImuI2;
IIfdI2:;
            if (IIfsetio((char *)0) == 0) goto IIfdE2;
            {
/* # line 230 "sholghdr.qsc" */	/* host code */
	    /* Turn on Examine fields and messages */
/* # line 231 "sholghdr.qsc" */	/* set_frs */
              {
                if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                  IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                } /* IIiqset */
              }
/* # line 232 "sholghdr.qsc" */	/* set_frs */
              {
                if (IIiqset(2,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
                  IIstfsio(54,(char *)"low_page",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IIstfsio(54,(char *)"high_page",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IIstfsio(54,(char *)"title1",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IIstfsio(54,(char *)"title2",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IIstfsio(54,(char *)"title3",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IIstfsio(54,(char *)"title4",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IIstfsio(54,(char *)"title5",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IIstfsio(54,(char *)"title6",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                  IIstfsio(15,(char *)"log_diagram",0,(short *)0,1,30,sizeof(1
                  ),(void *)IILQint(1));
                  IIstfsio(54,(char *)"log_xlist",0,(short *)0,1,30,sizeof(0),
                  (void *)IILQint(0));
                } /* IIiqset */
              }
/* # line 238 "sholghdr.qsc" */	/* host code */
	    refresh = TRUE;	/* ensure fresh log xaction info */
/* # line 240 "sholghdr.qsc" */	/* inittable */
              {
                if (IItbinit(Fshowlghdr,(char *)"log_xlist",(char *)"r") != 0
                ) {
                  IIthidecol((char *)"write",(char *)"i4");
                  IIthidecol((char *)"split",(char *)"i4");
                  IIthidecol((char *)"force",(char *)"i4");
                  IIthidecol((char *)"wait",(char *)"i4");
                  IIthidecol((char *)"tr_first",(char *)"i4");
                  IIthidecol((char *)"tr_last",(char *)"i4");
                  IIthidecol((char *)"tr_cp",(char *)"i4");
                  IIthidecol((char *)"tr_pid",(char *)"c8");
                  IIthidecol((char *)"tr_status",(char *)"c80");
                  IItfill();
                } /* IItbinit */
              }
/* # line 245 "sholghdr.qsc" */	/* host code */
	    /*
	    ** determine if user is on the log_diagram field.  If the user is,
	    ** then determine if the cursor rests on an occupied part of the
	    ** diagram.
	    */
/* # line 250 "sholghdr.qsc" */	/* inquire_frs */
              {
                if (IIiqset(2,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
                  IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,field_name,19,
                  (char *)0,0);
                } /* IIiqset */
              }
/* # line 251 "sholghdr.qsc" */	/* host code */
	    if (STcompare(field_name, "log_diagram") == 0)  
	    {
		/*
		** Cursor is on log_diagram field, mark position if valid in
		** the log_diagram and get the log xaction info.  
		*/
		/* where on field ? */
/* # line 259 "sholghdr.qsc" */	/* inquire_frs */
              {
                if (IIiqset(2,0,(char *)"",(char *)0, (char *)0) != 0) {
                  IIiqfsio((short *)0,1,30,4,&diagram_column,47,(char *)0,0);
                } /* IIiqset */
              }
/* # line 260 "sholghdr.qsc" */	/* inquire_frs */
              {
                if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                  IIiqfsio((short *)0,1,30,4,&curcolumn,49,(char *)0,0);
                } /* IIiqset */
              }
/* # line 261 "sholghdr.qsc" */	/* host code */
		diagram_column--;	/* field title fools FRS */
		/*
		** just because we have the log diagram as the current
		** field doesn't mean the cursor is on it ; it may be
		** on the menuline
		*/
		if (curcolumn < diagram_column)
		{
		    curcolumn = diagram_column;
		}
		current_position = curcolumn - diagram_column;
								/* with respect
								** to 'C' array
								*/
		if (current_position >= DIAGRAM_SIZE)
		{
		    current_position = DIAGRAM_SIZE - 1;
		}
		if (end_position < start_position)	/* case 1: eof < bof */
		{
		    if ((current_position <= end_position) /* valid area */
			|| (current_position >= start_position))
		    {    
			on_valid_area = TRUE;	/* used below to set range */
		    }
		    else	
		    {
			on_valid_area = FALSE;	/* used below */
		    }
		}
		else if (start_position < end_position) /* case 2: bof < eof */
		{
		    if ((current_position <= end_position)
			&& (current_position >= start_position))
		    {				/* valid area */
			on_valid_area = TRUE;	/* used below to set range */
		    }
		    else	
		    {
			on_valid_area = FALSE;	/* used below */
		    }
		}
		else	/* case 3: eof is in same block as bof */
		{
		    if (current_position == end_position) /* valid area */
		    {    
			on_valid_area = TRUE;	/* used below to set range */
		    }
		    else	
		    {
			on_valid_area = FALSE;	/* used below */
		    }
		}
	    }
	    else	/* on some other field */
	    {
		/* Not on log_diagram field, set logical accordingly */
		on_valid_area = FALSE;
	    } /* end if (STcompare(field_name, "log_diagram") == 0)  */
	    /*
	    ** In any event, display the range of log file pages represented
	    ** by the current position if cursor was on valid area, else
	    ** default to logical bof.  Set the proper position in the
	    ** log_diagram to indicate what part we are examining.
	    */
	    if (on_valid_area)	/* cursor on an occupied area of log file */
	    {
		low_page = (int)((double)current_position *
		    pages_per_block);
		high_page = (int)(((double)current_position *
		    pages_per_block) + pages_per_block);
	    }
	    else	/* not on valid area, default to start_position */
	    {
		low_page = (int)((double)start_position *
		    pages_per_block);
		high_page = (int)(((double)start_position *
		    pages_per_block) + pages_per_block);
		current_position = start_position;
	    }
	    /*
	    **	  adjust low and high page so they don't indicate values
	    **	  outside of valid range
	    */
	    if (ii_eof > bof)
	    {
		if (low_page < bof)
	        {
		    low_page = bof;
	        }
	        if (high_page > ii_eof)
	        {
		    high_page = ii_eof;
	        }
	    }
	    else   /* tail is past the head */
	    {
		if ((low_page < bof) && (low_page > ii_eof))
	        {
		    low_page = bof;
	        }
	        if ((high_page > ii_eof) && (high_page < bof))
	        {
		    high_page = ii_eof;
	        }
	    }
	    if (low_page == 0)	/* page 0 reserved for log header */
	    {
		low_page = 1;	/* useless to look for xactions on page 0 */
	    }
	    /* shows where we are */
	    log_diagram[current_position] = EXAMINE_MARKER;
	    /* Put the matching transactions on the form */
	    printlghlgx(low_page, high_page, Fshowlghdr, &refresh);
/* # line 382 "sholghdr.qsc" */	/* putform */
              {
                if (IIfsetio(Fshowlghdr) != 0) {
                  IIputfldio((char *)"log_diagram",(short *)0,1,32,0,
                  log_diagram);
                  IIputfldio((char *)"low_page",(short *)0,1,30,4,&low_page);
                  IIputfldio((char *)"high_page",(short *)0,1,30,4,&high_page);
                } /* IIfsetio */
              }
/* # line 384 "sholghdr.qsc" */	/* host code */
	    refresh_status(INVISIBLE, "flogmenu");
/* # line 385 "sholghdr.qsc" */	/* redisplay */
              {
                IIredisp();
              }
            }
IIfdB2:
            while (IIrunnest() != 0) {
              if (IIretval() == 1) {
                {
/* # line 389 "sholghdr.qsc" */	/* host code */
	    /* Move the examine_marker to next block  */
	    old_position = current_position;	/* used to reset prev loc */
	    if (current_position == end_position) /* wrap around logical eof */
	    {
		current_position = start_position;
	    }
	    /* wrap around phys eof */
	    else if ((current_position + 1) == DIAGRAM_SIZE)
	    {
		current_position = 0;	/* wrap around */
	    }
	    else	/* move one to the right */
	    {
		current_position++;	
	    }	/* end of wrap around handling */
	    log_diagram[current_position] = EXAMINE_MARKER;
	    /* Set the old_position marker in log_diagram to what it was */
	    if ((old_position == start_position) &&
		(start_position == end_position)) /* end = beginning */
	    {
		log_diagram[old_position] = NORANGE_MARKER;
	    }
	    else if ((old_position == start_position) ||
		(old_position == end_position)) /* set to eof/bof char */
	    {
		log_diagram[old_position] = BOFEOF_MARKER;
	    }
	    else
	    {
		log_diagram[old_position] = RANGE_MARKER;
	    }
	    /* Calculate the new ranges for current page */
	    low_page = (int)((double)current_position *
		pages_per_block);
	    high_page = (int)(((double)current_position *
		pages_per_block) + pages_per_block);
	    /*
	    ** adjust low and high page so they don't indicate values
	    **	  outside of valid range
	    */
	    if (ii_eof > bof)
	    {
		if (low_page < bof)
	        {
		    low_page = bof;
	        }
	        if (high_page > ii_eof)
	        {
		    high_page = ii_eof;
	        }
	    }
	    else   /* tail is past the head */
	    {
		if ((low_page < bof) && (low_page > ii_eof))
	        {
		    low_page = bof;
	        }
	        if ((high_page > ii_eof) && (high_page < bof))
	        {
		    high_page = ii_eof;
	        }
	    }
	    if (low_page == 0)	/* page 0 reserved for log header */
	    {
		low_page = 1;	/* useless to look for xactions on page 0 */
	    }
	    /*
	    **	  Put the matching transactions on the form
	    */
/* # line 462 "sholghdr.qsc" */	/* clear */
                  {
                    IIfldclear((char *)"log_xlist");
                  }
/* # line 463 "sholghdr.qsc" */	/* host code */
	    printlghlgx(low_page, high_page, Fshowlghdr, &refresh);
/* # line 464 "sholghdr.qsc" */	/* putform */
                  {
                    if (IIfsetio(Fshowlghdr) != 0) {
                      IIputfldio((char *)"log_diagram",(short *)0,1,32,0,
                      log_diagram);
                      IIputfldio((char *)"low_page",(short *)0,1,30,4,&
                      low_page);
                      IIputfldio((char *)"high_page",(short *)0,1,30,4,&
                      high_page);
                    } /* IIfsetio */
                  }
                }
              } else if (IIretval() == 2) {
                {
/* # line 469 "sholghdr.qsc" */	/* host code */
	    /*
	    **	  Move the examine_marker to previous block
	    */
	    old_position = current_position;	/* used to reset previous loc */
	    if (current_position == start_position) /* wrap around logical eof */
	    {
		current_position = end_position;
	    }
	    else if (current_position == 0)	/* wrap around phys eof */
	    {
		current_position = DIAGRAM_SIZE - 1;	/* wrap around */
	    }
	    else	/* move one to the left */
	    {
		current_position--;
	    }	/* end of wrap around handling */
	    log_diagram[current_position] = EXAMINE_MARKER;
	    /* Set the old_position marker in log_diagram to what it was */
	    if ((old_position == start_position) &&
		(start_position == end_position)) /* end = beginning */
	    {
		log_diagram[old_position] = NORANGE_MARKER;
	    }
	    else if ((old_position == start_position) ||
		(old_position == end_position)) /* set to eof/bof char */
	    {
		log_diagram[old_position] = BOFEOF_MARKER;
	    }
	    else
	    {
		log_diagram[old_position] = RANGE_MARKER;
	    }
	    /* Calculate the new ranges for current page */
	    low_page = (int)((double)current_position *
		pages_per_block);
	    high_page = (int)(((double)current_position *
		pages_per_block) + pages_per_block);
	    /*
	    **	  adjust low and high page so they don't indicate values
	    **	  outside of valid range
	    */
	    if (ii_eof > bof)
	    {
		if (low_page < bof)
	        {
		    low_page = bof;
	        }
	        if (high_page > ii_eof)
	        {
		    high_page = ii_eof;
	        }
	    }
	    else   /* tail is past the head */
	    {
		if ((low_page < bof) && (low_page > ii_eof))
	        {
		    low_page = bof;
	        }
	        if ((high_page > ii_eof) && (high_page < bof))
	        {
		    high_page = ii_eof;
	        }
	    }
	    if (low_page == 0)	/* page 0 reserved for log header */
	    {
		low_page = 1;	/* useless to look for xactions on page 0 */
	    }
	    /*
	    **	  Put the matching transactions on the form
	    */
/* # line 543 "sholghdr.qsc" */	/* clear */
                  {
                    IIfldclear((char *)"log_xlist");
                  }
/* # line 544 "sholghdr.qsc" */	/* host code */
	    printlghlgx(low_page, high_page, Fshowlghdr, &refresh);
/* # line 545 "sholghdr.qsc" */	/* putform */
                  {
                    if (IIfsetio(Fshowlghdr) != 0) {
                      IIputfldio((char *)"log_diagram",(short *)0,1,32,0,
                      log_diagram);
                      IIputfldio((char *)"low_page",(short *)0,1,30,4,&
                      low_page);
                      IIputfldio((char *)"high_page",(short *)0,1,30,4,&
                      high_page);
                    } /* IIfsetio */
                  }
                }
              } else if (IIretval() == 3) {
                {
/* # line 550 "sholghdr.qsc" */	/* host code */
	    /* make sure user is on scrolling area */
/* # line 551 "sholghdr.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(2,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
                      IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,field_name,19,
                      (char *)0,0);
                    } /* IIiqset */
                  }
/* # line 552 "sholghdr.qsc" */	/* host code */
	    if (STcompare(field_name, "log_xlist") != 0)  /* NOT on field */
	    {
		POP_MSG("The cursor must be on the transaction scrolling area");
/* # line 555 "sholghdr.qsc" */	/* resume */
                  {
                    IIresfld((char *)"log_xlist");
                    if (1) goto IIfdB2;
                  }
/* # line 556 "sholghdr.qsc" */	/* host code */
	    }
	    /* get number of dbs in logging system */
/* # line 559 "sholghdr.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(3,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)
"log_xlist",0);
                    } /* IIiqset */
                  }
/* # line 560 "sholghdr.qsc" */	/* host code */
	    if (list_count == 0)
	    {
		POP_MSG("There are no transactions to get information on");
/* # line 563 "sholghdr.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB2;
                  }
/* # line 564 "sholghdr.qsc" */	/* host code */
	    }
	    morelghdtx();	/* display the extra info */
                }
              } else if (IIretval() == 4) {
                {
/* # line 569 "sholghdr.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(3,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
                      IIiqfsio((short *)0,1,30,4,&list_count,32,(char *)
"log_xlist",0);
                    } /* IIiqset */
                  }
/* # line 570 "sholghdr.qsc" */	/* host code */
	    if (list_count == 0)
	    {
		POP_MSG("There is nothing to find");
/* # line 573 "sholghdr.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB2;
                  }
/* # line 574 "sholghdr.qsc" */	/* host code */
	    }
/* # line 575 "sholghdr.qsc" */	/* inquire_frs */
                  {
                    if (IIiqset(4,0,Fshowlghdr,(char *)"log_xlist",(char *)0) 
                    != 0) {
                      IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,field_name,19,
                      (char *)0,0);
                    } /* IIiqset */
                  }
/* # line 576 "sholghdr.qsc" */	/* host code */
	    tablefield_find();
/* # line 577 "sholghdr.qsc" */	/* resume */
                  {
                    IIrescol((char *)"log_xlist",field_name);
                    if (1) goto IIfdB2;
                  }
                }
              } else if (IIretval() == 5) {
                {
/* # line 581 "sholghdr.qsc" */	/* host code */
	    /* Set the refresh interval */
	    timeout = frs_timeout;		/* place into local frs var */
/* # line 583 "sholghdr.qsc" */	/* set_frs */
                  {
                    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                      IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
                    } /* IIiqset */
                  }
/* # line 584 "sholghdr.qsc" */	/* set_frs */
                  {
                    if (IIiqset(2,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
                      IIstfsio(54,(char *)"low_page",0,(short *)0,1,30,sizeof(
                      1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"high_page",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                      IIstfsio(54,(char *)"title1",0,(short *)0,1,30,sizeof(1)
                      ,(void *)IILQint(1));
                      IIstfsio(54,(char *)"title2",0,(short *)0,1,30,sizeof(1)
                      ,(void *)IILQint(1));
                      IIstfsio(54,(char *)"title3",0,(short *)0,1,30,sizeof(1)
                      ,(void *)IILQint(1));
                      IIstfsio(54,(char *)"title4",0,(short *)0,1,30,sizeof(1)
                      ,(void *)IILQint(1));
                      IIstfsio(54,(char *)"title5",0,(short *)0,1,30,sizeof(1)
                      ,(void *)IILQint(1));
                      IIstfsio(54,(char *)"title6",0,(short *)0,1,30,sizeof(1)
                      ,(void *)IILQint(1));
                      IIstfsio(15,(char *)"log_diagram",0,(short *)0,1,30,
                      sizeof(0),(void *)IILQint(0));
                      IIstfsio(54,(char *)"log_xlist",0,(short *)0,1,30,
                      sizeof(1),(void *)IILQint(1));
                    } /* IIiqset */
                  }
/* # line 591 "sholghdr.qsc" */	/* host code */
	    /* Set the current_position marker in log_diagram to what it was */
	    if ((current_position == start_position) &&
		(start_position == end_position)) /* end = beginning */
	    {
		log_diagram[current_position] = NORANGE_MARKER;
	    }
	    else if ((current_position == start_position) ||
		(current_position == end_position)) /* set to eof/bof char */
	    {
		log_diagram[current_position] = BOFEOF_MARKER;
	    }
	    else
	    {
		log_diagram[current_position] = RANGE_MARKER;
	    }
/* # line 606 "sholghdr.qsc" */	/* putform */
                  {
                    if (IIfsetio(Fshowlghdr) != 0) {
                      IIputfldio((char *)"log_diagram",(short *)0,1,32,0,
                      log_diagram);
                    } /* IIfsetio */
                  }
/* # line 607 "sholghdr.qsc" */	/* breakdisplay */
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
            goto IIfdE2;
IImuI2:
            if (IIinitmu() == 0) goto IIfdE2;
            if (IInmuact((char *)"Next",(char *)0,2,2,1) == 0) goto IIfdE2;
            if (IInmuact((char *)"Previous",(char *)0,2,2,2) == 0) goto IIfdE2;
            if (IInmuact((char *)"More_Info",(char *)0,2,2,3) == 0) goto IIfdE2;
            if (IInmuact((char *)"Find",(char *)0,2,2,4) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(7,(char *)0,2,2,4) == 0) goto IIfdE2;
            if (IInmuact((char *)"End",(char *)0,2,2,5) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE2;
            if (IIendmu() == 0) goto IIfdE2;
            goto IIfdI2;
IIfdE2:;
            IIendnest();
          }
/* # line 609 "sholghdr.qsc" */	/* host code */
	refresh_status(VISIBLE, "flogmenu");
/* # line 610 "sholghdr.qsc" */	/* redisplay */
          {
            IIredisp();
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 614 "sholghdr.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 615 "sholghdr.qsc" */	/* host code */
	my_help("ipmshlgh.hlp",  "Logging System Header");
	/* Set the refresh interval */
	timeout = frs_timeout;		/* place into local frs var */
/* # line 619 "sholghdr.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeout);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 623 "sholghdr.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 627 "sholghdr.qsc" */	/* host code */
	e_xit = TRUE;		/* user wants to leave */
/* # line 628 "sholghdr.qsc" */	/* message */
          {
            IImessage((char *)" ");
          }
/* # line 629 "sholghdr.qsc" */	/* breakdisplay */
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
    if (IInmuact((char *)"More_Info",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Examine",(char *)0,2,2,4) == 0) goto IIfdE1;
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
/* # line 632 "sholghdr.qsc" */	/* host code */
    if (e_xit) {		/* user wants to leave */
	terminate(OK);
    }
/* # line 635 "sholghdr.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 636 "sholghdr.qsc" */	/* host code */
    refresh_status(INVISIBLE, "flogmenu");
    return;
}
/*
** morelghdtx() - This routine displays the popup form which show the
**	less needed header examine transaction info.
**
** History
**	4/2/90		tomt	written
**
*/
VOID
morelghdtx()
{
/*
** Declare Variables
*/
  int write, split, force, wait, tr_first, tr_last;
  int tr_cp;
  char tr_status[81], tr_pid[HEXID_SIZE + 1];
/* # line 658 "sholghdr.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(5),(void *)IILQint(5));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fmorelghdtx,(char *)"r") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
    {
/* # line 661 "sholghdr.qsc" */	/* host code */
	/*
	**   Get the values off of the tablefield on the base form and
	**   place them on the current form
	*/
/* # line 665 "sholghdr.qsc" */	/* getrow */
      {
        if (IItbsetio(2,Fshowlghdr,(char *)"log_xlist",-2) != 0) {
          IItcogetio((short *)0,1,30,4,&write,(char *)"write");
          IItcogetio((short *)0,1,30,4,&split,(char *)"split");
          IItcogetio((short *)0,1,30,4,&force,(char *)"force");
          IItcogetio((short *)0,1,30,4,&wait,(char *)"wait");
          IItcogetio((short *)0,1,30,4,&tr_first,(char *)"tr_first");
          IItcogetio((short *)0,1,30,4,&tr_last,(char *)"tr_last");
          IItcogetio((short *)0,1,30,4,&tr_cp,(char *)"tr_cp");
          IItcogetio((short *)0,1,32,HEXID_SIZE + 1-1,tr_pid,(char *)
"tr_pid");
          IItcogetio((short *)0,1,32,80,tr_status,(char *)"tr_status");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 669 "sholghdr.qsc" */	/* putform */
      {
        if (IIfsetio(Fmorelghdtx) != 0) {
          IIputfldio((char *)"write",(short *)0,1,30,4,&write);
          IIputfldio((char *)"split",(short *)0,1,30,4,&split);
          IIputfldio((char *)"force",(short *)0,1,30,4,&force);
          IIputfldio((char *)"wait",(short *)0,1,30,4,&wait);
          IIputfldio((char *)"tr_first",(short *)0,1,30,4,&tr_first);
          IIputfldio((char *)"tr_last",(short *)0,1,30,4,&tr_last);
          IIputfldio((char *)"tr_cp",(short *)0,1,30,4,&tr_cp);
          IIputfldio((char *)"tr_pid",(short *)0,1,32,0,tr_pid);
          IIputfldio((char *)"tr_status",(short *)0,1,32,0,tr_status);
        } /* IIfsetio */
      }
/* # line 672 "sholghdr.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 673 "sholghdr.qsc" */	/* submenu */
      {
        goto IImuI4;
IIfdB4:
        IIrunmu(0);
        if (IIretval() == 1) {
          {
/* # line 676 "sholghdr.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
          }
        } else {
          if (1) goto IIfdE4;
        } /* IIretval */
        goto IIfdE4;
IImuI4:
        IImuonly();
        if (IIinitmu() == 0) goto IIfdE4;
        if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE4;
        IIFRInternal(0);
        if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE4;
        if (IIendmu() == 0) goto IIfdE4;
        goto IIfdB4;
IIfdE4:;
      }
    }
IIfdB3:
    while (IIrunform() != 0) {
    } /* IIrunform */
    if (IIFRitIsTimeout() != 0) goto IIfdE3;
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    goto IIfdE3;
IImuI3:
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 680 "sholghdr.qsc" */	/* host code */
    return;
}
/*
** printlghdr - puts the log header values on the form or to a file
**
** Inputs
**	None
**
** Outputs
**	values to form or to screen
**
** Return
**	None
**
** History
**	9/2/89		tomt	created
**	9/5/89		tomt	added bargraph for % of log file used
**	9/29/89		tomt	added blocks in use field
**	10/1/89		tomt	integrated 6.3 changes
**	3/15/90		tomt	added blocks free field and changed form
**	3/17/90		tomt	changed percent display to diagram of log file
**	12/13/92	jpk	support side effects of XA project
**      15-jan-1996 (toumi01; from 1.1 axp_osf port)
**            Added kchin's change (from 6.4) for axp_osf.  Also changed
**            blocks_reserved, blocks_reserved_mb, and reserve_bytes which
**            not part of kchin's change
**            9/10/93 kchin changed type of blocks_in_use, blocks_in_use_mb,
**                          blocks_free, blocks_free_mb, bufcnt, and
**                          lgd_status from long to i4, to make the
**                          declarations of these variables consistent,
**                          eg. tally with the decl'n in get_lklgstats().
**                          This is to fix problems when running
**                          on 64-bit platform such as Alpha OSF/1, in
**                          this case long is 64-bit, which is no longer
**                          similar in size to 32-bit int, thus mixing
**                          them will result in error, moreover, 64-bit
**                          integer is not supported by SQL/QUEL yet.
**
**	18/03/96	mckba01	70619, Divide by zero arithmetic trap.
**				Move calculation of blocks_reserved
**				to a point where we are sure the
**				logging system in online.
**
**	5-Jan-2006 (kschendel)
**	    Log reservation in bytes is now a u_i8.
*/
VOID
printlghdr()
{
  i4 cp, ncp_page;
  i4 blocks_in_use, blocks_in_use_mb;
  i4 blocks_free, blocks_free_mb;
  i4 blocks_reserved, blocks_reserved_mb;
  i8 reserved_bytes;
  double fend_position, fstart_position, freserved_position;
  int percent_full, counter;
  i4 bufcnt;
  i4 lgd_status;
  char tran_id[17], warning[12];
  char lgh_begin[25], lgh_cp[26], lgh_end[26];
  char log_status[81], jnl_window[81];
    LG_LA	fcp;
    LG_LA	lcp;
    LG_HEADER	header;
  i4 lgh_l_logfull;
  i4 lgh_size;
  i4 lgh_count, lgh_count_mb;
  i4 lgh_l_cp;
  i4 lgh_l_abort;
    /*
    **    Put information on the form.  some of it must be formatted first
    */
    get_lklgstats(GET_LOGHEADER, (PTR) &header);
    get_lklgstats(GET_LOGFJNLCP, (PTR) &fcp);
    get_lklgstats(GET_LOGLJNLCP, (PTR) &lcp);
    get_lklgstats(GET_LOGSTS, (PTR) &lgd_status);
    get_lklgstats(GET_LG_A_RES_SPACE, (PTR) &reserved_bytes);
    /* if ((lgd_status & LG_E_ONLINE) == 0) what the call is on UNIX */
    if ((lgd_status & 1) == 0)
    {
	POP_MSG("Warning: Can't get logging information, system not online");
	return;
    }
    blocks_reserved = reserved_bytes / header.lgh_size;
    get_lklgstats(GET_LOGBCNT, (PTR) &bufcnt);
    STprintf(tran_id, "%x%0x", header.lgh_tran_id.db_high_tran,
	header.lgh_tran_id.db_low_tran);
    bof = header.lgh_begin.la_block,
    STprintf(lgh_begin, "<%d:%d:%d>",  header.lgh_begin.la_sequence, 
        bof, header.lgh_begin.la_offset);
    cp = header.lgh_cp.la_block;
    STprintf(lgh_cp, "<%d:%d:%d>", header.lgh_cp.la_sequence,
	cp, header.lgh_cp.la_offset);
    ii_eof = header.lgh_end.la_block;
    STprintf(lgh_end, "<%d:%d:%d>", header.lgh_end.la_sequence,
	ii_eof, header.lgh_end.la_offset);
    STprintf(jnl_window, "<%d,%d,%d>..<%d,%d,%d>",
	fcp.la_sequence,
	fcp.la_block,
	fcp.la_offset,
	lcp.la_sequence,
	lcp.la_block,
	lcp.la_offset);
    stat_decode(log_status, Lghdrstat, (u_i4)lgd_status);
    /* Calculate how full the log file is, then build display string. */
    if (ii_eof < bof)	/* need to account for wraparound */
    {
	blocks_in_use = (header.lgh_count - bof) + ii_eof;
    }
    else
    {
	blocks_in_use = ii_eof - bof;
    }
    blocks_in_use_mb = (blocks_in_use * header.lgh_size) / 1024;
    /* properly determine next cp accounting for wraparound */
    if ((cp + header.lgh_l_cp) > header.lgh_count)	/* wraparound */
    {
	ncp_page = (cp + header.lgh_l_cp) - header.lgh_count;
    }
    else
    {
	ncp_page = header.lgh_l_cp + cp;		/* calculate next cp */
    }
    blocks_free = header.lgh_count - blocks_in_use - blocks_reserved;
    blocks_free_mb = (blocks_free * header.lgh_size) / 1024;
    percent_full = (i4)(((double) (blocks_in_use + blocks_reserved) /
	    (double) header.lgh_count) * 100.0);
    /*
    **  Determine where the logical end and beginning of file are for
    **  the diagram. Perform proper rounding and handle when end or
    **  beginning is at full scale.
    */
    pages_per_block = (double)header.lgh_count / (double)DIAGRAM_SIZE;
    fend_position = ((double)ii_eof / pages_per_block);
    fstart_position = ((double)bof / pages_per_block);
    if (bof >= blocks_reserved)
    {
	freserved_position = ((double) (bof - blocks_reserved) 
		/ pages_per_block);
    }
    else		/* it too wraps around */
    {
	freserved_position = ((double) ((header.lgh_count - blocks_reserved)
		 + bof) / pages_per_block);
    }
    start_position = (int)fstart_position;
    end_position = (int)fend_position;
    reserved_position = (int)freserved_position;
/*    if ((fstart_position - (int)fstart_position) < 0.5)
**    {
**	start_position = (int)fstart_position;
**    }
**    else
**    {
**	start_position = (int)fstart_position + 1;
**    }
**
**    if ((fend_position - (int)fend_position) < 0.5)
**    {
**	end_position = (int)fend_position;
**    }
**    else
**    {
**	end_position = (int)fend_position + 1;
**    }
*/
    if (end_position == DIAGRAM_SIZE)
    {
	end_position = DIAGRAM_SIZE - 1;  /* correct array index */
    }	
    if (start_position == DIAGRAM_SIZE)
    {
	start_position = DIAGRAM_SIZE - 1;  /* correct array index */
    }
    /* clear diagram */
    for (counter = 0; counter < DIAGRAM_SIZE; counter++)
    {
	log_diagram[counter] = ' ';  /* space fill */
    }
    log_diagram[DIAGRAM_SIZE] = EOS;  /* null terminate */
    /*
    **  Fill in the log file diagram between the beginning and end
    */
    for (counter = 0; counter < DIAGRAM_SIZE; counter++)
    {
	if (start_position == end_position)
	{
	    if (counter < start_position)
	    {
		log_diagram[counter] = ' ';
	    }
	    else if (counter == start_position)
	    {
		log_diagram[start_position] = NORANGE_MARKER;  /* start = end */
	    }
	    else
	    {
		log_diagram[counter] = '\0';	/* null terminate and break */
		break;
	    }
	}
	else if (end_position < start_position)
					/* need to fill from phys begin
					** to logical end and from logical
                                        ** begin to physical end
					*/
	{
	    if ((counter > start_position)
		|| (counter < end_position)) /* fill with '-' */
	    {
		log_diagram[counter] = RANGE_MARKER;
	    }
	    else if (counter == start_position)  /* indicate start */
	    {
		log_diagram[counter] = BOFEOF_MARKER;
	    }
	    else if ((counter > end_position) && (counter < start_position))
	    {
		log_diagram[counter] = ' ';
	    }
	    else if (counter == end_position)
	    {
		log_diagram[counter] = BOFEOF_MARKER;
	    }
	}
	else  /* BOF is less than EOF */
	{
	    if (counter < start_position) 
	    {
		log_diagram[counter] = ' ';
	    }
	    else if (counter == start_position)  /* indicate start */
	    {
		log_diagram[counter] = BOFEOF_MARKER;
	    }
	    else if ((counter < end_position)
		&& (counter > start_position)) /* fill with '-' */
	    {
		log_diagram[counter] = RANGE_MARKER;
	    }
	    else if (counter == end_position)
	    {
		log_diagram[counter] = BOFEOF_MARKER;
	    }
	    else /* past the logical end: null term and break */
	    {
#if SHOW_RESERVED
		if ((start_position >= reserved_position)
			&& (counter >= reserved_position))
		{
#endif
		    log_diagram[counter] = EOS;
		    break;
#if SHOW_RESERVED
		}
#endif
	    }
	}
#if SHOW_RESERVED
	if (start_position > reserved_position)
	{
	    if ((counter > reserved_position) && (counter < start_position))
	    {
		log_diagram[counter] = RESERVED_MARKER;
	    }
	}
	else if (start_position < reserved_position)	/* wrapped */
	{
	    if ((counter < start_position) || (counter > reserved_position))
	    {
		log_diagram[counter] = RESERVED_MARKER;
	    }
	}
	else
	{
		;	/* no need to mark anything as reserved */
	}
#endif
    } /* end of FOR loop */
    log_diagram[DIAGRAM_SIZE] = EOS;  /* null terminate */
    /* determine whether or not to warn of log file force_abort or log_full */
    if (((blocks_in_use + blocks_reserved) >= header.lgh_l_abort) &&
	((blocks_in_use  + blocks_reserved) < header.lgh_l_logfull))
    {
	STcopy("FORCE ABORT", warning);
/* # line 984 "sholghdr.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowlghdr) != 0) {
      IIputfldio((char *)"warning",(short *)0,1,32,0,warning);
    } /* IIfsetio */
  }
/* # line 985 "sholghdr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"warning",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(15,(char *)"warning",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(16,(char *)"warning",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 987 "sholghdr.qsc" */	/* host code */
    }
    else if ((blocks_in_use  + blocks_reserved) >= header.lgh_l_logfull)
    {
	STcopy("LOG FULL", warning);
/* # line 991 "sholghdr.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowlghdr) != 0) {
      IIputfldio((char *)"warning",(short *)0,1,32,0,warning);
    } /* IIfsetio */
  }
/* # line 992 "sholghdr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"warning",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(15,(char *)"warning",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(16,(char *)"warning",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 994 "sholghdr.qsc" */	/* host code */
    }
    else
    {
	STcopy("OK", warning);
/* # line 998 "sholghdr.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowlghdr) != 0) {
      IIputfldio((char *)"warning",(short *)0,1,32,0,warning);
    } /* IIfsetio */
  }
/* # line 999 "sholghdr.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowlghdr,(char *)0, (char *)0) != 0) {
      IIstfsio(15,(char *)"warning",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(16,(char *)"warning",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
      IIstfsio(54,(char *)"warning",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1001 "sholghdr.qsc" */	/* host code */
    }
    lgh_l_logfull = header.lgh_l_logfull;
    lgh_size = header.lgh_size;
    lgh_count = header.lgh_count;
    lgh_count_mb = (lgh_count * header.lgh_size) / 1024;
    lgh_l_cp = header.lgh_l_cp;
    lgh_l_abort = header.lgh_l_abort;
/* # line 1010 "sholghdr.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowlghdr) != 0) {
      IIputfldio((char *)"lgh_size",(short *)0,1,30,4,&lgh_size);
      IIputfldio((char *)"lgh_count",(short *)0,1,30,4,&lgh_count);
      IIputfldio((char *)"bufcnt",(short *)0,1,30,4,&bufcnt);
      IIputfldio((char *)"lgh_l_cp",(short *)0,1,30,4,&lgh_l_cp);
      IIputfldio((char *)"lgh_l_logfull",(short *)0,1,30,4,&lgh_l_logfull);
      IIputfldio((char *)"lgh_l_abort",(short *)0,1,30,4,&lgh_l_abort);
      IIputfldio((char *)"lgh_begin",(short *)0,1,32,0,lgh_begin);
      IIputfldio((char *)"lgh_cp",(short *)0,1,32,0,lgh_cp);
      IIputfldio((char *)"lgh_end",(short *)0,1,32,0,lgh_end);
      IIputfldio((char *)"lgh_tran_id",(short *)0,1,32,0,tran_id);
      IIputfldio((char *)"lga",(short *)0,1,32,0,jnl_window);
      IIputfldio((char *)"lgd_status",(short *)0,1,32,0,log_status);
      IIputfldio((char *)"blocks_free",(short *)0,1,30,4,&blocks_free);
      IIputfldio((char *)"log_diagram",(short *)0,1,32,0,log_diagram);
      IIputfldio((char *)"blocks_in_use",(short *)0,1,30,4,&blocks_in_use);
      IIputfldio((char *)"reserved",(short *)0,1,30,4,&blocks_reserved);
      IIputfldio((char *)"percent_used",(short *)0,1,30,4,&percent_full);
      IIputfldio((char *)"bof_page",(short *)0,1,30,4,&bof);
      IIputfldio((char *)"eof_page",(short *)0,1,30,4,&ii_eof);
      IIputfldio((char *)"lcp_page",(short *)0,1,30,4,&cp);
      IIputfldio((char *)"ncp_page",(short *)0,1,30,4,&ncp_page);
    } /* IIfsetio */
  }
/* # line 1022 "sholghdr.qsc" */	/* host code */
    return;
}
/*
** morelghdr() - This routine displays the popup form which show the
**	less needed header info.
**
** History
**	9/12/90		tomt	written
**
*/
VOID
morelghdr()
{
    /*
    ** Declare Variables
    */
  char lghbegin[25], lghcp[26], lghend[26];
  char jnl_win[81], lgh_tranid[17];
/* # line 1042 "sholghdr.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(5,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(14),(void *)IILQint(14));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(6),(void *)IILQint(6));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(Fmorelghdr,(char *)"r") == 0) goto IIfdE5;
    goto IImuI5;
IIfdI5:;
    if (IIfsetio((char *)0) == 0) goto IIfdE5;
    {
/* # line 1046 "sholghdr.qsc" */	/* host code */
	/*
	**   Get the values off of the tablefield on the base form and
	**   place them on the current form
	*/
/* # line 1050 "sholghdr.qsc" */	/* getform */
      {
        if (IIfsetio(Fshowlghdr) != 0) {
          IIgetfldio((short *)0,1,32,24,lghbegin,(char *)"lgh_begin");
          IIgetfldio((short *)0,1,32,25,lghcp,(char *)"lgh_cp");
          IIgetfldio((short *)0,1,32,25,lghend,(char *)"lgh_end");
          IIgetfldio((short *)0,1,32,16,lgh_tranid,(char *)"lgh_tran_i\
d");
          IIgetfldio((short *)0,1,32,80,jnl_win,(char *)"lga");
        } /* IIfsetio */
      }
/* # line 1052 "sholghdr.qsc" */	/* putform */
      {
        if (IIfsetio(Fmorelghdr) != 0) {
          IIputfldio((char *)"lgh_begin",(short *)0,1,32,0,lghbegin);
          IIputfldio((char *)"lgh_cp",(short *)0,1,32,0,lghcp);
          IIputfldio((char *)"lgh_end",(short *)0,1,32,0,lghend);
          IIputfldio((char *)"lgh_tran_id",(short *)0,1,32,0,lgh_tranid);
          IIputfldio((char *)"lga",(short *)0,1,32,0,jnl_win);
        } /* IIfsetio */
      }
/* # line 1055 "sholghdr.qsc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 1056 "sholghdr.qsc" */	/* submenu */
      {
        goto IImuI6;
IIfdB6:
        IIrunmu(0);
        if (IIretval() == 1) {
          {
/* # line 1059 "sholghdr.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE5;
            }
          }
        } else {
          if (1) goto IIfdE6;
        } /* IIretval */
        goto IIfdE6;
IImuI6:
        IImuonly();
        if (IIinitmu() == 0) goto IIfdE6;
        if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE6;
        IIFRInternal(0);
        if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE6;
        if (IIendmu() == 0) goto IIfdE6;
        goto IIfdB6;
IIfdE6:;
      }
    }
IIfdB5:
    while (IIrunform() != 0) {
    } /* IIrunform */
    if (IIFRitIsTimeout() != 0) goto IIfdE5;
IIfdF5:
    if (IIchkfrm() == 0) goto IIfdB5;
    goto IIfdE5;
IImuI5:
    goto IIfdI5;
IIfdE5:;
    IIendfrm();
  }
/* # line 1063 "sholghdr.qsc" */	/* host code */
    return;
}
