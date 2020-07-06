# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/* globals and includes */
#include <compat.h>
#include <si.h>
#include <cs.h>
#include <st.h>
#include <dbms.h>
#include <fe.h>
#include <pc.h>
#include <lg.h>
#include <lk.h>
#include <er.h>
#include        <adf.h>
#include        <ft.h>
#include        <fmt.h>
#include        <frame.h>
#include "lists.h"
#include "msgs.h" 
#include "dba.h"
/* Option codes for option_work routine */
#define		CHECK		 0	/* check user options */
#define		CHECK_AND_SAVE	 1	/* check user options then set flags */
#define		YNSIZE		1 + 1	/* str = "y" or "n" */
/*
** Forward and External References
*/
VOID put_options();
VOID set_options();
i4  get_db_id();
i4  get_nextdb();
i4  option_work();
PTR displktype();
PTR dispdb();
i4  trans_lock_type();
i4  trns_lnm_to_lktyp();
i4  getlockcat();
/*
** Declares
*/
  static char Fshowopts[]= "fshowopts";
/*
**	put_options - this routine puts the options chosen by the
**	the user on the command line.  This allows a user to specify
**	options on startup and see them by calling up the options form.
**
**		Written by:	Tom Turchioe   Relational Technology
**		Date	  :     3-aug-1988
**
**	History
**	2/6/89   tomt   Added support for popup help for list of dbnames
**	3/8/89   tomt   Added lockname popup help and moved routines for
**			displaying those popups into this file
**			dispdb().
**	9/6/89	 tomt  Added an frs timeout field which the user can
**		       set.  This is only used on certian forms.
**      3/13/90  tomt  added lock and log stats
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
VOID
put_options()
{
    /* declare local variables */
  int lock_type_id, timeout;
  char *db_name_ptr;
  char *table_ptr;
  char lock_type_nm[FE_MAXNAME + 1];
    /* copy global pointers into local pointers for INGRES stmts */
    lock_type_id = ing_locktype;
    db_name_ptr = ptr_dbname;
    table_ptr = ptr_tablename;
    if (flag_nl)  /* flag_nl set indicates printing of Granted NL locks */
    {
/* # line 96 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_null",(short *)0,1,32,0,(char *)"y");
    } /* IIfsetio */
  }
/* # line 97 "showopts.qsc" */	/* host code */
    }
    else
    {
/* # line 100 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_null",(short *)0,1,32,0,(char *)"n");
    } /* IIfsetio */
  }
/* # line 101 "showopts.qsc" */	/* host code */
    }
    if (flag_all_locks)
    {
/* # line 105 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_all_lk",(short *)0,1,32,0,(char *)"y");
      IIputfldio((char *)"flag_lktype",(short *)0,1,32,0,(char *)"n");
    } /* IIfsetio */
  }
/* # line 106 "showopts.qsc" */	/* host code */
    }
    else if (flag_locktype)
    {
	(VOID) trans_lock_type(lock_type_id, lock_type_nm);
/* # line 110 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"locktype",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 111 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"locktype",(short *)0,1,32,0,lock_type_nm);
    } /* IIfsetio */
  }
/* # line 112 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_lktype",(short *)0,1,32,0,(char *)"y");
      IIputfldio((char *)"flag_all_lk",(short *)0,1,32,0,(char *)"n");
    } /* IIfsetio */
  }
/* # line 113 "showopts.qsc" */	/* host code */
    }
    else
    {
/* # line 116 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_all_lk",(short *)0,1,32,0,(char *)"n");
      IIputfldio((char *)"flag_lktype",(short *)0,1,32,0,(char *)"n");
    } /* IIfsetio */
  }
/* # line 117 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"locktype",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 118 "showopts.qsc" */	/* host code */
    }
    timeout = frs_timeout;	/* put value into frs known local var */
/* # line 121 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"frs_timeout",(short *)0,1,30,4,&timeout);
    } /* IIfsetio */
  }
/* # line 123 "showopts.qsc" */	/* host code */
    if (flag_db)
    {
/* # line 125 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_db",(short *)0,1,32,0,(char *)"y");
      IIputfldio((char *)"dbname",(short *)0,1,32,0,db_name_ptr);
    } /* IIfsetio */
  }
/* # line 126 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 127 "showopts.qsc" */	/* host code */
    }
    else
    {
/* # line 130 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_db",(short *)0,1,32,0,(char *)"n");
    } /* IIfsetio */
  }
/* # line 131 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 132 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"dbname");
  }
/* # line 133 "showopts.qsc" */	/* host code */
    }
    if (flag_table)
    {
/* # line 137 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tablename",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 138 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_table",(short *)0,1,32,0,(char *)"y");
      IIputfldio((char *)"tablename",(short *)0,1,32,0,table_ptr);
    } /* IIfsetio */
  }
/* # line 139 "showopts.qsc" */	/* host code */
    }
    else
    {
/* # line 142 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tablename",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 143 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_table",(short *)0,1,32,0,(char *)"n");
    } /* IIfsetio */
  }
/* # line 144 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"tablename");
  }
/* # line 145 "showopts.qsc" */	/* host code */
    }
    if (flag_nonprot_lklists)
    {
/* # line 149 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_nonprot_lklists",(short *)0,1,32,0,(char *)"\
y");
    } /* IIfsetio */
  }
/* # line 150 "showopts.qsc" */	/* host code */
    }
    else
    {
/* # line 153 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_nonprot_lklists",(short *)0,1,32,0,(char *)"\
n");
    } /* IIfsetio */
  }
/* # line 154 "showopts.qsc" */	/* host code */
    }
    if (flag_inactive_tx)
    {
/* # line 158 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_inactive_tx",(short *)0,1,32,0,(char *)"\
y");
    } /* IIfsetio */
  }
/* # line 159 "showopts.qsc" */	/* host code */
    }
    else
    {
/* # line 162 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_inactive_tx",(short *)0,1,32,0,(char *)"\
n");
    } /* IIfsetio */
  }
/* # line 163 "showopts.qsc" */	/* host code */
    }
    if (flag_internal_sess)
    {
/* # line 167 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_internal_sess",(short *)0,1,32,0,(char *)"\
y");
    } /* IIfsetio */
  }
/* # line 168 "showopts.qsc" */	/* host code */
    }
    else
    {
/* # line 171 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_internal_sess",(short *)0,1,32,0,(char *)"\
n");
    } /* IIfsetio */
  }
/* # line 172 "showopts.qsc" */	/* host code */
    }
}
/*
**	set_options - this routine displays the options form and allows
**	the user to change them.  This allows a user to specify
**	options without having to leave the lockinfo program.
**
**		Written by:	Tom Turchioe   Relational Technology
**		Date	  :     3-aug-1988
**
**	History
**		1/26/89		tomt	adapted for use in Lockinfo R6
**		3/08/89		tomt	more work for R6 version (lock names)
**		21 Sept 1990	jpk	modify menu items "Save" to "OK",
**					"Forget" to "Cancel",
**					"Restore" to "Defaults"
**
*/
VOID
set_options()
{
    bool	e_xit;
  char field_name[FE_MAXNAME + 1];
    static 	forms_added = FALSE;
  GLOBALREF int *fdispdb;
  GLOBALREF int *fdisplktype;
  GLOBALREF int *fshowopts;
    /*
    **  Determine if forms are added, if not addform them.
    */
    if (!forms_added)
    {
/* # line 207 "showopts.qsc" */	/* addform */
  {
    IIaddform(fdispdb);
  }
/* # line 208 "showopts.qsc" */	/* addform */
  {
    IIaddform(fdisplktype);
  }
/* # line 209 "showopts.qsc" */	/* addform */
  {
    IIaddform(fshowopts);
  }
/* # line 210 "showopts.qsc" */	/* host code */
	forms_added = TRUE;
    }
/* # line 213 "showopts.qsc" */	/* display */
  {
    if (IIdispfrm(Fshowopts,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 216 "showopts.qsc" */	/* host code */
	/* display current options on form */
	put_options();
/* # line 218 "showopts.qsc" */	/* set_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIstfsio(37,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        } /* IIiqset */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 222 "showopts.qsc" */	/* host code */
	(VOID) option_work(CHECK, field_name);
/* # line 223 "showopts.qsc" */	/* resume */
          {
            IIresfld(field_name);
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 227 "showopts.qsc" */	/* host code */
	if (option_work(CHECK_AND_SAVE, field_name) == FAIL)
	{
/* # line 229 "showopts.qsc" */	/* resume */
          {
            IIresfld(field_name);
            if (1) goto IIfdB1;
          }
/* # line 230 "showopts.qsc" */	/* host code */
	}
	e_xit = FALSE;
/* # line 232 "showopts.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 236 "showopts.qsc" */	/* host code */
	e_xit = FALSE;
/* # line 237 "showopts.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 241 "showopts.qsc" */	/* message */
          {
            IImessage((char *)"Restoring original options...");
          }
/* # line 242 "showopts.qsc" */	/* sleep */
          {
            IIsleep(2);
          }
/* # line 243 "showopts.qsc" */	/* clear */
          {
            IIclrflds();
          }
/* # line 244 "showopts.qsc" */	/* host code */
	put_options();	/* display original settings */
/* # line 245 "showopts.qsc" */	/* resume */
          {
            IIresfld((char *)"flag_null");
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 249 "showopts.qsc" */	/* host code */
	my_help("ipmshwop.hlp", "Option Selection");
        }
      } else if (IIretval() == 6) {
        {
/* # line 253 "showopts.qsc" */	/* host code */
	e_xit = TRUE;
/* # line 254 "showopts.qsc" */	/* breakdisplay */
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
    if (IIFRafActFld((char *)"all",0,1) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact((char *)"OK",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(8,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Cancel",(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Defaults",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 257 "showopts.qsc" */	/* host code */
    if (e_xit)
    {
	terminate(OK);
    }
    return;
}
/*
**  option_work(option) - 
**	This routine gets the option settings from the fshowopts form;
**	if option is CHECK then
**		check if the user selected conflicting options
**	else if option is CHECK_AND_SAVE then
**		check if the user selected conflicting options
**		if there are no errors then
**			set appropriate internal flags
**		endif
**	endif
**
**	This routine copies the name of the field to resume on into the
**	option character variable.  The caller issues the RESUME statement.
**
**	Checking the options involves ensuring that they are not set in
**	conflicting ways.
**
**	History
**		1/26/89		tomt	adapted for use in Lockinfo R6
**		3/09/89		tomt	lock name work
**              3/12/90         tomt    added lock and tx filters
**		2/28/93		jpk	add internal sess flag
*/
i4
option_work(option, field_name)
i4	option;
char 	*field_name;
{
  char c_locktype[20], msg[81];
  char c_flag_null[YNSIZE];
  char c_flag_all_lk[YNSIZE];
  char c_flag_locktype[YNSIZE];
  char c_flag_table[YNSIZE];
  char c_flag_db[YNSIZE];
  char c_flag_nonprot_lklists[YNSIZE];
  char c_flag_inactive_tx[YNSIZE];
  char c_flag_internal_sess[YNSIZE];
  char c_dbname[FE_MAXNAME + 1];
  char c_tablename[FE_MAXNAME + 1], *db_name, *locktype_ptr;
  int lfrs_timeout;
    i4		db_id, lock_type_id;
    i4		lock_cat;
    /* what field am I on */
/* # line 309 "showopts.qsc" */	/* inquire_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,field_name,19,(char *)0,0);
    } /* IIiqset */
  }
/* # line 311 "showopts.qsc" */	/* host code */
    /* get all values off the form */
/* # line 312 "showopts.qsc" */	/* getform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,c_tablename,(char *)
"tablename");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_table,(char *)"flag_t\
able");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_null,(char *)"flag_nu\
ll");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_all_lk,(char *)
"flag_all_lk");
      IIgetfldio((short *)0,1,32,19,c_locktype,(char *)"locktype");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_locktype,(char *)
"flag_lktype");
      IIgetfldio((short *)0,1,30,4,&lfrs_timeout,(char *)"frs_timeout");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_inactive_tx,(char *)
"flag_inactive_tx");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_db,(char *)"flag_db");
      IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,c_dbname,(char *)"db\
name");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_nonprot_lklists,(char *)
"flag_nonprot_lklists");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_inactive_tx,(char *)
"flag_inactive_tx");
      IIgetfldio((short *)0,1,32,YNSIZE-1,c_flag_internal_sess,(char *)
"flag_internal_sess");
    } /* IIfsetio */
  }
/* # line 320 "showopts.qsc" */	/* host code */
    /*
    ** check the values.  This routine is called after every
    ** field activate as well as when the SAVE operation is picked.
    ** If the option flag is set to CHECK, then this routine is being
    ** called from the ACTIVATE FIELD.  In this mode, we want to set
    ** other fields based on the current field the user is on.
    ** The error checking below effectively collects the error checking
    ** that would be performed in various ACTIVATE FIELD blocks and puts
    ** them in one place.
    **
    ** Determine the lock catagory (database, table, page, other).  This
    ** simplifies the code.
    */
    if (c_flag_locktype[0] == 'y')
    {
	lock_type_id = trns_lnm_to_lktyp(c_locktype); /* locktype */
	lock_cat = getlockcat(lock_type_id); /* lock catagory */
    }
    if (option == CHECK)
    {
	if (STcompare(field_name, "flag_null") == 0)
	{
	    STcopy("flag_all_lk", field_name);
	}
	else if (STcompare(field_name, "flag_all_lk") == 0)
	{
	    if (c_flag_all_lk[0] == 'y')
	    {
/* # line 349 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_lktype",(short *)0,1,32,0,(char *)"n");
      IIputfldio((char *)"flag_table",(short *)0,1,32,0,(char *)"n");
      IIputfldio((char *)"flag_null",(short *)0,1,32,0,(char *)"y");
    } /* IIfsetio */
  }
/* # line 351 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tablename",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"locktype",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 353 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"tablename");
  }
/* # line 354 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"locktype");
  }
/* # line 355 "showopts.qsc" */	/* host code */
		STcopy("flag_db", field_name);
	    }
	    else
	    {
		STcopy("flag_lktype", field_name);
	    }
	}
	else if (STcompare(field_name, "flag_lktype") == 0)
	{
	    if (c_flag_locktype[0] == 'y')
	    {
/* # line 366 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_all_lk",(short *)0,1,32,0,(char *)"n");
      IIputfldio((char *)"flag_table",(short *)0,1,32,0,(char *)"n");
      IIputfldio((char *)"flag_null",(short *)0,1,32,0,(char *)"y");
    } /* IIfsetio */
  }
/* # line 368 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tablename",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"locktype",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 370 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"tablename");
  }
/* # line 371 "showopts.qsc" */	/* host code */
		STcopy("locktype", field_name);
	    }
	    else
	    {
		STcopy("flag_db", field_name);
/* # line 376 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"locktype",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 377 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"locktype");
  }
/* # line 378 "showopts.qsc" */	/* host code */
	    }
	}
	else if (STcompare(field_name, "locktype") == 0)
	{
	    if (!lock_type_id)
	    {
		if (c_locktype[0] != '?') 	/* bad lktype not HELP */
		{
	    	    STprintf(msg, "Locktype %s does not exist.  Getting available locktypes...", c_locktype);
/* # line 387 "showopts.qsc" */	/* message */
  {
    IImessage(msg);
  }
/* # line 388 "showopts.qsc" */	/* sleep */
  {
    IIsleep(2);
  }
/* # line 389 "showopts.qsc" */	/* host code */
		}
		/* display help form */
		if ((locktype_ptr = displktype()) != NULL)  /* place value on form if usr selected one*/
		{
/* # line 393 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"locktype",(short *)0,1,32,0,locktype_ptr);
    } /* IIfsetio */
  }
/* # line 394 "showopts.qsc" */	/* host code */
		}
		STcopy("locktype", field_name);
	    }
	    else
	    {
		if ((c_flag_db[0] == 'n') && ((lock_cat != DATABASE_LOCK) &&
		    (lock_cat != OTHER_LOCK)))
		{ /* not a db or other type lock */
/* # line 402 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_db",(short *)0,1,32,0,(char *)"y");
    } /* IIfsetio */
  }
/* # line 403 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 404 "showopts.qsc" */	/* host code */
	    	    STcopy("dbname", field_name);
		}
		else
		{
	    	    STcopy("flag_db", field_name);
		}
	    }
	}
	else if (STcompare(field_name, "flag_db") == 0)
	{
	    if (c_flag_db[0] == 'y')
	    {
	        STcopy("dbname", field_name);
/* # line 417 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 418 "showopts.qsc" */	/* host code */
	    }
	    else
	    {
/* # line 421 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_table",(short *)0,1,32,0,(char *)"n");
    } /* IIfsetio */
  }
/* # line 422 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"tablename");
    IIfldclear((char *)"dbname");
  }
/* # line 423 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tablename",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 425 "showopts.qsc" */	/* host code */
		if ((c_flag_locktype[0] == 'y') && (lock_cat != DATABASE_LOCK))
		{
/* # line 427 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_lktype",(short *)0,1,32,0,(char *)"n");
    } /* IIfsetio */
  }
/* # line 428 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"locktype",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 429 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"locktype");
  }
/* # line 430 "showopts.qsc" */	/* host code */
		}
	        STcopy("flag_table", field_name);
	    }
	}
	else if (STcompare(field_name, "dbname") == 0)
	{
	    if ((db_id = get_db_id(c_dbname)) == -1)
	    {
		if (c_dbname[0] != '?') 	/* wrong dbname not HELP */
		{
	    	    STprintf(msg, "Database %s does not exist. Getting available databases... ", c_dbname);
/* # line 441 "showopts.qsc" */	/* message */
  {
    IImessage(msg);
  }
/* # line 442 "showopts.qsc" */	/* sleep */
  {
    IIsleep(2);
  }
/* # line 443 "showopts.qsc" */	/* host code */
		}
		/* display help form */
		if ((db_name = dispdb()) != NULL)  /* place value on form if usr selected one*/
		{
/* # line 447 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"dbname",(short *)0,1,32,0,db_name);
    } /* IIfsetio */
  }
/* # line 448 "showopts.qsc" */	/* host code */
		}
		STcopy("dbname", field_name);
	    }
	    else
	    {
		STcopy("flag_table", field_name);
	    }
	}
	else if (STcompare(field_name, "flag_table") == 0)
	{
	    if (c_flag_table[0] == 'y')
	    {
/* # line 460 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_all_lk",(short *)0,1,32,0,(char *)"n");
      IIputfldio((char *)"flag_lktype",(short *)0,1,32,0,(char *)"n");
      IIputfldio((char *)"flag_null",(short *)0,1,32,0,(char *)"y");
    } /* IIfsetio */
  }
/* # line 462 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"locktype");
  }
/* # line 463 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"locktype",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 464 "showopts.qsc" */	/* host code */
		if (c_flag_db[0] == 'n')
		{
/* # line 466 "showopts.qsc" */	/* putform */
  {
    if (IIfsetio(Fshowopts) != 0) {
      IIputfldio((char *)"flag_db",(short *)0,1,32,0,(char *)"y");
    } /* IIfsetio */
  }
/* # line 467 "showopts.qsc" */	/* host code */
		    POP_MSG("Please specify a database name");
/* # line 468 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"dbname",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 469 "showopts.qsc" */	/* host code */
		    STcopy("dbname", field_name);
		}
		else
		{
		    STcopy("tablename", field_name);
/* # line 474 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tablename",0,(short *)0,1,30,sizeof(0),
      (void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 475 "showopts.qsc" */	/* host code */
		}
	    }
	    else
	    {
/* # line 479 "showopts.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,Fshowopts,(char *)0, (char *)0) != 0) {
      IIstfsio(54,(char *)"tablename",0,(short *)0,1,30,sizeof(1),
      (void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 480 "showopts.qsc" */	/* clear */
  {
    IIfldclear((char *)"tablename");
  }
/* # line 481 "showopts.qsc" */	/* host code */
		STcopy("frs_timeout", field_name);
	    }
	}
	else if (STcompare(field_name, "tablename") == 0)
	{
	    if (STlength(c_tablename) == 0)
	    {
		POP_MSG("Please enter a tablename or select Defaults or Cancel");
		STcopy("tablename", field_name);
	    }
	    STcopy("frs_timeout", field_name);
	}
	else if (STcompare(field_name, "frs_timeout") == 0)
	{
	    STcopy("flag_inactive_tx", field_name);
	}
        else if (STcompare(field_name, "flag_inactive_tx") == 0)
        {
            STcopy("flag_nonprot_lklists", field_name);
        }
        else if (STcompare(field_name, "flag_nonprot_lklists") == 0)
        {
            STcopy("flag_internal_sess", field_name);
        }
	else if (STcompare(field_name, "flag_internal_sess") == 0)
	{
	    STcopy("flag_null", field_name);
	} 
	return(OK);
    }
    else if (option == CHECK_AND_SAVE)
    {
	/*
	**    Perform error checking and if OK, set flags accordingly.
	**    The error checking above is different from what needs to be
	**    done before saving.  The error checking below needs to detect
	**    inconsistent selection of options and report the error.  Only
	**    if no errors are found, will the internal flags be set.
	*/
	/*
	**    Make sure user enters values for locktype and or tablename
	**    if they indicated that they wanted to.
	*/
	if ((c_flag_locktype[0] == 'y') && (STlength(c_locktype) == 0))
	{
	    POP_MSG("Please enter a locktype or select Defaults or Cancel");
	    STcopy("locktype", field_name);
	    return(FAIL);
	}
	if ((c_flag_table[0] == 'y') && (STlength(c_tablename) == 0))
	{
	    POP_MSG("Please enter a tablename or select Defaults or Cancel");
	    STcopy("tablename", field_name);
	    return(FAIL);
	}
	/*
	**    validate option selections.  Instead of toggling
	**    other fields, report error
	*/
	if (c_flag_all_lk[0] == 'y') 		/* check flag_all_locks field */
	{
	    if (c_flag_null[0] == 'n') 	/* don't suppress null locks */
	    {
		POP_MSG("Suppressing display of null locks is incompatible with displaying all locks");
		STcopy("flag_null", field_name);
		return(FAIL);
	    }
	    else if (c_flag_locktype[0] == 'y') 	/* can't both be "y" */
	    {
		if (c_flag_table[0] == 'y')   /* cant view all lks & a tbl */
		{
		    POP_MSG("Checking all locks is incompatible with show locks for a particular table");
		    STcopy("flag_table", field_name);
		}
		else
		{
		    POP_MSG("Checking all locks is incompatible with checking for a particular locktype");
		    STcopy("flag_lktype", field_name);
		}
		return(FAIL);
	    }
	    else if (c_flag_table[0] == 'y') /* cant view all lks & a tbl */
	    {
		POP_MSG("Checking all locks is incompatible with show locks for a particular table");
		STcopy("flag_table", field_name);
		return(FAIL);
	    }
	}
	if (c_flag_db[0] == 'n')
	{
	    if ((c_flag_locktype[0] == 'y') && (lock_cat != DATABASE_LOCK) &&
		(lock_cat != OTHER_LOCK))
	    {
		POP_MSG("Showing locks on all databases is incompatible with showing page or table locktypes");
		STcopy("flag_lktype", field_name);
		return(FAIL);
	    }
	    else if (c_flag_table[0] == 'y')
	    {
		POP_MSG("You specified a table, must specify which database");
		STcopy("flag_db", field_name);
	        return(FAIL);
	    }
	}
	else if (c_flag_db[0] == 'y')
	{
	    if ((db_id = get_db_id(c_dbname)) == -1)
	    {
		STprintf(msg, "Database %s does not exist.  Enter a valid dbname", c_dbname);
		POP_MSG(msg);
		STcopy("dbname", field_name);
		return(FAIL);
	    }
	}
	if (c_flag_locktype[0] == 'y')
	{
	    if (! (lock_type_id = trns_lnm_to_lktyp(c_locktype)))
	    {
	    	STprintf(msg, "Locktype %s does not exist.  Type '?' for a list", c_locktype);
		POP_MSG(msg);
		STcopy("locktype", field_name);
		return(FAIL);
	    }
	}
	/*
	**    Now, set the flags
	*/
	if (c_flag_null[0] == 'y')
	{
	    flag_nl++;		/* set this flag to show ALL locks */
	}
	else
	{
	    flag_nl = 0;	/* clear to SUPPRESS Granted NL locks */
	}
	if (c_flag_all_lk[0] == 'y')
	{
	    flag_all_locks++;
	    flag_locktype = 0;
	    ing_locktype = 0;	/* clear locktype */
	}
	else if (c_flag_locktype[0] == 'y')
	{
	    flag_all_locks = 0;
	    flag_locktype++;		/* Suppress all_locks flag and set
					** specific lock flag
					*/
	    ing_locktype = lock_type_id;	/* put into global */
	}
	else
	{
	    flag_all_locks = 0;
	    flag_locktype = 0;
	    ing_locktype = 0;	/* clear locktype */
	}
	frs_timeout = lfrs_timeout;	/* place in global */
	if (c_flag_db[0] == 'y')
	{
	    flag_db = TRUE;
	    STcopy(c_dbname, ptr_dbname);	/* copy to gbl area */
	    dbname_id = db_id; /* change global tid val so we select desired db */
	}
	else
	{
	    flag_db = FALSE;
	    ptr_dbname[0] = EOS;	/* erase gbl db value */
	}
	if (c_flag_table[0] == 'y')
	{
	    flag_table++;
	    STcopy(c_tablename, ptr_tablename);	/* copy to gbl area */
	}
	else
	{
	    flag_table = 0;
	    ptr_tablename[0] = EOS;	/* erase gbl tbl value */
	}
	if (c_flag_nonprot_lklists[0] == 'y')
	{
	    flag_nonprot_lklists++;
	}
	else	
	{
	    flag_nonprot_lklists = 0;
	}
        if (c_flag_inactive_tx[0] == 'y')
        {
            flag_inactive_tx++;
        }
        else
        {
            flag_inactive_tx = 0;
        }
        if (c_flag_internal_sess[0] == 'y')
        {
            flag_internal_sess = TRUE;
        }
        else
        {
            flag_internal_sess = FALSE;
        }
    }
    return(OK);
}
/*
**  dispdb() - this routine displays
**	a form which contains a list of database names.
**
**  History
**	2/6/89		tomt	Created
**	3/9/89		tomt	moved inside of showopts.qc file
**	21 Sept 1990	jpk	change "Forget" to "Cancel"
**
*/
PTR dispdb()
{
  static char dbname[FE_MAXNAME + 1];
/* # line 713 "showopts.qsc" */	/* display */
  {
    if (IIdispfrm((char *)"Fdispdb",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 716 "showopts.qsc" */	/* inittable */
      {
        if (IItbinit((char *)"Fdispdb",(char *)"dblist",(char *)"r") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 718 "showopts.qsc" */	/* host code */
	while (get_nextdb(dbname))
	{
/* # line 720 "showopts.qsc" */	/* loadtable */
      {
        if (IItbact((char *)"Fdispdb",(char *)"dblist",1) != 0) {
          IItcoputio((char *)"database",(short *)0,1,32,0,dbname);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 721 "showopts.qsc" */	/* host code */
	}
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 725 "showopts.qsc" */	/* host code */
	my_help("ipmdspdb.hlp", "Database List");
        }
      } else if (IIretval() == 2) {
        {
/* # line 729 "showopts.qsc" */	/* getrow */
          {
            if (IItbsetio(2,(char *)"Fdispdb",(char *)"dblist",-2) != 0) {
              IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,dbname,(char *)
"database");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 730 "showopts.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 734 "showopts.qsc" */	/* host code */
	dbname[0] = EOS;	/* return null */	
/* # line 735 "showopts.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
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
    if (IInmuact((char *)"Help",(char *)0,2,2,1) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact((char *)"Select",(char *)0,2,2,2) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact((char *)"Cancel",(char *)0,2,2,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 738 "showopts.qsc" */	/* host code */
    return(&dbname[0]);
}
