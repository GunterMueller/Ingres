# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
#include    <compat.h>
#include    <dbms.h>
#include    <er.h>
#include    <duerr.h>
#include    <st.h>
#include    <nm.h>
#include    <me.h>
#include    <ut.h>
#include    "duustrings.h"
# include "dut.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/**
**
**  Name: DUTFORM.SC - form utility functions.
**
**  Description:
**	This file contain functions that deals with the 3 full screen
**	forms used in starview.
**
**	dut_ff1_1intro		- Form 1, the introductory form.
**	dut_ff1_2load_ddb	- Load DDB names to form 1.
**	dut_ff2_1ldblist	- Form 2, the LDB list form.
**	dut_ff2_2load_ldb	- Load LDB info to form 2.
**	dut_ff2_3test_node	- TestNode submenu.
**	dut_ff2_4reload_ldb	- ReLoad LDB list info to form 2.
**	dut_ff2_5test_ldb	- TestLDB submenu.
**	dut_ff3_1objlist	- Form 3, the OBJECT list form.
**	dut_ff3_2load_obj	- Load Object info to form 3.
**	dut_ff3_3reload_obj	- Reload Object info to form 3.
**	dut_ff3_4objinfo	- Object information.
**	dut_ff3_5drop_object	- ***Removed, not use anymore.***
**	dut_ff3_6remove_object	- Remove a registered table.
**	dut_ff3_7objects	- Load objects from ddb.
**	dut_ff3_8fixlist        - Fix objects list after a delete.
**
**  History:
**      02-nov-1988 (alexc)
**          Creation.
**	08-mar-1989 (alexc)
**	    Alpha test version.
**      12-jun-1989 (alexc)
**          Modify remove function to take care of all register tables, and
**		register views.
**	06-jun-1991 (fpang)
**	    Changed copyright to 1991.
**	02-jul-1991 (rudyw)
**	    Create a line of forward function references to remedy a compiler
**	    complaint of redefinition.
**	09-jul-1991 (rudyw)
**	    Back out previous change which VMS compiler didn't accept.
**	10-jul-1991 (rudyw)
**	    Redo forward reference fix so acceptable to all boxes.
**	25-jul-1991 (fpang)
**	    1. dut_ff1_1intro()
**	       - Add activations to update ddbname and ddbnode fields when
**	         user moves up and down in the DdbHelp table field.
**	       - Removed the 'Select' menuitem.
**	       - Fixed Top/Bottom/Find to keep ddbname and ddbnode fields
**	         current when scrolling DdbHelp table field.
**	    2. dut_ff1_2load_ddb()
**	       - Explicitly return E_DU_OK if successful.
**	    3. dut_ff2_3test_node() and dut_ff2_5test_ldb()
**	       - Cleanup associations after AllLdbs and AllNodes sub menu
**	         selections only. Don't do it when 'End' is selected
**	         because connections were never made.
**	       - Nicer text when displaying node/ldb accessibility message.
**	    4. dut_ff3_1objlist()
**	       - Fix B38733, indexes still displayed when registered object
**	         with indexes is removed. The reverse was also true, when
**		 a local object with indexes was registered, indexes were not
**		 displayed.
**	       - Changed Browse/Remove to reload the objlist because they may
**		 may have changed as a result of the Register/Remove.
**	         Added dut_ff3_7objects() and dut_ff3_8fixlist() to support
**		 reloading of objlist.
**	12-sep-1991 (fpang)
**          1. Changed Star*View to StarView. -B39562.
**	    2. Mapped FIND, TOP and BOTTOM to standard INGRES FRSKEYS. -B39661
**	04-sep-1992 (jpk)
**		add more forward references.
**      10-12-94 (newca01)
**             Added code in dut_ff1_1intro for B44836
**      02-15-95 (brogr02)
**             Changed copyright message
**      03-02-95 (newca01)
**             Addec code in dut_ff1_1intro for B67259
**      21-mar-1995 (sarjo01)
**          Bug 67417: add test for upper case 'IISTAR_CDBS' as well as
**          'iistar_cdbs' to allow query to work correctly in FIPS.
**	08-apr-97 (mcgem01)
**	    Changed CA-OpenIngres to OpenIngres and updated Copyright
**	    notice.
**	10-apr-98 (mcgem01)
**	    Change OpenIngres to Ingres and update copyright notice.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
DUT_STATUS dut_ff2_1ldblist(), dut_ff2_2load_ldb(), dut_ff2_3test_node(),
           dut_ff2_4reload_ldb(), dut_ff2_5test_ldb();
DUT_STATUS dut_ff3_1objlist(), dut_ff3_2load_obj(), dut_ff3_3reload_obj(),
           dut_ff3_4objinfo(), dut_ff3_6remove_object(), dut_ff3_7objects(),
	   dut_ff3_8fixlist();
/*
** Name: dut_ff1_1intro	- Load form 'dut_f1_intro' with data 
**			in a DUT_F1_INTRO pointer
**
** Description:
**	FORM 1 is the introductroy form, which maybe avoided by
**	issuing the starview command with a DDBname as the parameter.
**		options:
**			Go	- enter Star*View with a given DDB.
**			Select	- Select a DDB from list to DDB field.
**			DDBHelp	- List all DDBs in iidbdb of given node.
**			NodeHelp- List nodes.
**			Top, Bottom, Find - operations on DDBlist(table field).
**			Help	- Help for this form.
**			Quit	- Quit Star*View.
**
** Inputs:
**      dut_cb				Control block for starview.
**      dut_errcb                       Error Control block for starview.
**
** Outputs:
**	Returns:
**	    not applicable.
**	Exceptions:
**
** Side Effects:
**	    none
**
** History:
**      2-nov-1988 (alexc)
**          Creation.
**     25-jul-1991 (fpang)
**	    - Add activations to update ddbname and ddbnode fields when
**	      user moves up and down in the DdbHelp table field.
**	    - Removed the 'Select' menuitem.
**	    - Fixed Top/Bottom/Find to keep ddbname and ddbnode fields
**	      current when scrolling DdbHelp table field.
**     10-12-94 (newca01)
**          - Added "if" stmt in activate routine for before ddblist
**            for B44836
**     03-02-95 (newca01)
**          - Added second condition on "if" above for b67259
**            Added "if" around every getrow in dut_ff1_1intro().
*/
dut_ff1_1intro(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_F1_INTRO *dut_f1_1intro_p;
  char dut_buffer[DDB_2MAXNAME];
    char *dut_f1_curr_node = NULL;
    dut_f1_1intro_p = dut_cb->dut_form_intro;
    dut_cb->dut_c9_status	= DUT_F_FORM_1;
/* # line 174 "dutform.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_f1_intro",(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 177 "dutform.sc" */	/* host code */
	/* Set error flag to DUT_ERROR */
	dut_cb->dut_c8_status = DUT_ERROR;
	/* RTI banner */
/* # line 180 "dutform.sc" */	/* message */
    {
      IImessage((char *)
"Ingres StarView - Copyright (c) 2004 Ingres Corporation");
    }
/* # line 189 "dutform.sc" */	/* host code */
	STcopy(dut_cb->dut_c2_nodename, dut_cb->dut_c7_currnode);
/* # line 190 "dutform.sc" */	/* inittable */
    {
      if (IItbinit((char *)"dut_f1_intro",(char *)"dut_f1_4ddblist",(char *)"\
r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 191 "dutform.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_f1_intro") != 0) {
        IIputfldio((char *)"dut_f1_1ddbname",(short *)0,1,32,0,
        dut_cb->dut_c1_ddbname);
        IIputfldio((char *)"dut_f1_2nodename",(short *)0,1,32,0,
        dut_cb->dut_c2_nodename);
        IIputfldio((char *)"dut_f1_3nodename",(short *)0,1,32,0,
        dut_cb->dut_c2_nodename);
      } /* IIfsetio */
    }
/* # line 197 "dutform.sc" */	/* host code */
	if (dut_uu5_chkdbname(dut_cb->dut_c1_ddbname) == E_DU_OK)
	{
	    /* If connections is ok to open given DDB, go into LDB form */
	    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
	    {
		dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 
                              2, 0, dut_cb->dut_c1_ddbname);
	    }
	    else
	    {
		dut_ff2_1ldblist(dut_cb, dut_errcb);
	    }
	}
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
	 /* if stmt added 10/12/94 (newca01) for b44836  */
	 /* second condition in if added 03/02/95 (newca01) for b67259 */
	 if ( (dut_cb->dut_form_intro->dut_f1_4ddblist != NULL) 
	    && (dut_cb->dut_form_intro->dut_f1_4ddblist->dut_t1_1ddbname[0]
	       != EOS) )
	 {
/* # line 221 "dutform.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_f1_intro",(char *)"dut_f1_4ddbl\
ist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c1_ddbname,
            (char *)"dut_t1_1ddbname");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 223 "dutform.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_f1_intro") != 0) {
            IIputfldio((char *)"dut_f1_1ddbname",(short *)0,1,32,0,
            dut_cb->dut_c1_ddbname);
          } /* IIfsetio */
        }
/* # line 225 "dutform.sc" */	/* host code */
         }
      } else if (IIretval() == 2) {
/* # line 231 "dutform.sc" */	/* getform */
        {
          if (IIfsetio((char *)"dut_f1_intro") != 0) {
            IIgetfldio(&dut_cb->dut_null_1,1,32,DDB_MAXNAME-1,
            dut_cb->dut_c1_ddbname,(char *)"dut_f1_1ddbname");
            IIgetfldio(&dut_cb->dut_null_2,1,32,DDB_MAXNAME-1,
            dut_cb->dut_c2_nodename,(char *)"dut_f1_2nodename");
          } /* IIfsetio */
        }
/* # line 237 "dutform.sc" */	/* host code */
	if (dut_cb->dut_null_1 == -1)
	{
	    dut_cb->dut_c1_ddbname[0] = EOS;
	}
	if (dut_cb->dut_null_2 == -1)
	{
	    STcopy(dut_cb->dut_c13_vnodename, dut_cb->dut_c2_nodename);
	}
	if (dut_cb->dut_c1_ddbname[0] != EOS)
	{
	    if (dut_ii2_check_str(DUT_CK_DDBNAME, dut_cb->dut_c1_ddbname,
		dut_errcb)
		!= E_DU_OK)
	    {
		dut_ee1_error(dut_errcb, W_DU1803_SV_BAD_DDBNAME,
				2, 0, dut_cb->dut_c1_ddbname);
	    }
	    else 
	    {
		STprintf(dut_cb->frs_message, "%s::%s", 
			dut_cb->dut_c2_nodename,
			dut_cb->dut_c1_ddbname);
/* # line 259 "dutform.sc" */	/* message */
        {
          IImessage(dut_cb->frs_message);
        }
/* # line 260 "dutform.sc" */	/* host code */
		if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
		{
		    dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 
				  2, 0, dut_cb->dut_c1_ddbname);
		}
		else
		{
		    dut_ff2_1ldblist(dut_cb, dut_errcb);
		}
	    }
	}
	else	/* if ddbname field is NULL */
	{
	    dut_ee1_error(dut_errcb, W_DU1801_SV_NO_DDB_SELECTED, 0);
	}
      } else if (IIretval() == 3) {
/* # line 281 "dutform.sc" */	/* clear */
        {
          IIfldclear((char *)"dut_f1_4ddblist");
        }
/* # line 282 "dutform.sc" */	/* redisplay */
        {
          IIredisp();
        }
/* # line 283 "dutform.sc" */	/* getform */
        {
          if (IIfsetio((char *)"dut_f1_intro") != 0) {
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_cb->dut_form_intro->dut_f1_2nodename,(char *)
"dut_f1_2nodename");
          } /* IIfsetio */
        }
/* # line 287 "dutform.sc" */	/* host code */
	if (dut_cb->dut_c0_iidbdbnode[0] == EOS)
	{
	    STcopy(dut_cb->dut_c13_vnodename, dut_cb->dut_c0_iidbdbnode);
	}
	if (dut_cb->dut_form_intro->dut_f1_2nodename[0] != EOS)
	{
	    STcopy(dut_cb->dut_form_intro->dut_f1_2nodename,
			dut_cb->dut_c0_iidbdbnode);
	}
	if (dut_ff1_2load_ddb(dut_cb, dut_errcb, dut_f1_1intro_p) == DUT_ERROR)
	{
	    dut_cb->dut_c8_status = DUT_ERROR;
	}
	else
	{
	 /* if stmt added 03/02/95 (newca01) for b67259 */
	 if ( (dut_cb->dut_form_intro->dut_f1_4ddblist != NULL) 
	    && (dut_cb->dut_form_intro->dut_f1_4ddblist->dut_t1_1ddbname[0]
	       != EOS) )
	 {
/* # line 307 "dutform.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_f1_intro") != 0) {
            IIputfldio((char *)"dut_f1_1ddbname",(short *)0,1,32,0,
            dut_f1_1intro_p->dut_f1_4ddblist->dut_t1_1ddbname);
          } /* IIfsetio */
        }
/* # line 312 "dutform.sc" */	/* resume */
        {
          IIresfld((char *)"dut_f1_4ddblist");
          if (1) goto IIfdB1;
        }
/* # line 313 "dutform.sc" */	/* host code */
	 }
	 else 
	 {
	   dut_ee1_error(dut_errcb, W_DU181F_SV_NO_DDBS_FOUND, 0);
	 }
	}
      } else if (IIretval() == 4) {
	/* following 'if' added around getrow 03-02-95 newca01 b67259 */
	if ((dut_cb->dut_form_intro->dut_f1_4ddblist != NULL)
	   && (dut_cb->dut_form_intro->dut_f1_4ddblist->dut_t1_1ddbname[0]
	       != EOS ))
	{
	dut_uu7_table_top("dut_f1_4ddblist");
/* # line 329 "dutform.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_f1_intro",(char *)"dut_f1_4ddbl\
ist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c1_ddbname,
            (char *)"dut_t1_1ddbname");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 331 "dutform.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_f1_intro") != 0) {
            IIputfldio((char *)"dut_f1_1ddbname",(short *)0,1,32,0,
            dut_cb->dut_c1_ddbname);
          } /* IIfsetio */
        }
/* # line 333 "dutform.sc" */	/* resume */
        {
          IIresfld((char *)"dut_f1_4ddblist");
          if (1) goto IIfdB1;
        }
/* # line 334 "dutform.sc" */	/* host code */
	}
      } else if (IIretval() == 5) {
     	/* following 'if' added around getrow 03-02-95 newca01 b67259 */
        if ((dut_cb->dut_form_intro->dut_f1_4ddblist != NULL)
	&& (dut_cb->dut_form_intro->dut_f1_4ddblist->dut_t1_1ddbname[0]
	   != EOS ))
	{
	dut_uu8_table_bottom("dut_f1_4ddblist");
/* # line 345 "dutform.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_f1_intro",(char *)"dut_f1_4ddbl\
ist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c1_ddbname,
            (char *)"dut_t1_1ddbname");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 347 "dutform.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_f1_intro") != 0) {
            IIputfldio((char *)"dut_f1_1ddbname",(short *)0,1,32,0,
            dut_cb->dut_c1_ddbname);
          } /* IIfsetio */
        }
/* # line 349 "dutform.sc" */	/* resume */
        {
          IIresfld((char *)"dut_f1_4ddblist");
          if (1) goto IIfdB1;
        }
/* # line 350 "dutform.sc" */	/* host code */
	}
      } else if (IIretval() == 6) {
	/* following 'if' added around getrow 03-02-95 newca01 b67259 */
	if ((dut_cb->dut_form_intro->dut_f1_4ddblist != NULL)
	&& (dut_cb->dut_form_intro->dut_f1_4ddblist->dut_t1_1ddbname[0]
	    != EOS ))
        {
	dut_uu9_table_find(dut_cb, dut_errcb, DUT_M00_DDBNAME,
			"dut_f1_4ddblist",
			NULL);
/* # line 363 "dutform.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_f1_intro",(char *)"dut_f1_4ddbl\
ist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c1_ddbname,
            (char *)"dut_t1_1ddbname");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 365 "dutform.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_f1_intro") != 0) {
            IIputfldio((char *)"dut_f1_1ddbname",(short *)0,1,32,0,
            dut_cb->dut_c1_ddbname);
          } /* IIfsetio */
        }
/* # line 367 "dutform.sc" */	/* resume */
        {
          IIresfld((char *)"dut_f1_4ddblist");
          if (1) goto IIfdB1;
        }
/* # line 368 "dutform.sc" */	/* host code */
	}
      } else if (IIretval() == 7) {
/* # line 373 "dutform.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hf1_1name);
        }
      } else if (IIretval() == 8) {
/* # line 382 "dutform.sc" */	/* host code */
	dut_cb->dut_c8_status = E_DU_OK;
	dut_cb->dut_c9_status = DUT_F_NULL;
/* # line 384 "dutform.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    goto IIfdE1;
IImuI1:
    if (IITBcaClmAct((char *)"dut_f1_4ddblist",(char *)"all",1,1) == 0) goto 
    IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact((char *)"Go",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"DDBHelp",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Top",(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"Find",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,8) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,8) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 388 "dutform.sc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 389 "dutform.sc" */	/* host code */
}
/*
** Name: dut_ff1_2load_ddb	- Load DDB information from iidbdb.
**
** Description:
**		This function loads information about DDBs from 'iistar_cdbs'
**	in iidbdb into a table field in form 'dut_f1_intro' and into an 
**	dynamically allocated array of DUT_T1_DDBLIST.
**
** Inputs:
**      dut_cb				Control block for starview.
**      dut_errcb                       Error Control block for starview.
**
** Outputs:
**	Returns:
**	    not applicable.
**	Exceptions:
**
** Side Effects:
**	    none
**
** History:
**      2-nov-1988 (alexc)
**          Creation.
**     25-jul=1991 (fpang)
**	    Explicitly return E_DU_OK if successful.
*/
dut_ff1_2load_ddb(dut_cb, dut_errcb, dut_f1_intro_p)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
  DUT_F1_INTRO *dut_f1_intro_p;
{
  DUT_T1_DDBLIST *dut_f1_4ddblist_p;
  DUT_T1_DDBLIST *dut_f1_0ddblist_curr_p;
  i4 dut_f1_0ddb_count;
  char dut_msg[DB_LONGMSG];
    i4		dut_f1_0ddb_fill;
    dut_f1_0ddb_count = 0;
    dut_f1_0ddb_fill  = 0;
    dut_f1_4ddblist_p = dut_f1_intro_p->dut_f1_4ddblist;
    STprintf(dut_msg, DUT_M01_LOADDDB,
	dut_cb->dut_c0_iidbdbnode);
/* # line 439 "dutform.sc" */	/* message */
  {
    IImessage(dut_msg);
  }
/* # line 441 "dutform.sc" */	/* host code */
    if (dut_uu2_dbop(DUT_IIDBDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU180A_SV_CANT_OPEN_IIDBDB, 0);
	return(DUT_ERROR);
    }
/* # line 447 "dutform.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_name='iistar_cdbs' or table_\
name='IISTAR_CDBS'");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_f1_0ddb_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 453 "dutform.sc" */	/* host code */
    if (dut_f1_0ddb_count == 0)
    {
	/* This means there is no table call iistar_cdbs in remote iidbdb */
	/* Remote installation does not run INGRES/STAR */
	return(DUT_ERROR);
    }
/* # line 459 "dutform.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from iistar_c\
dbs");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_f1_0ddb_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 463 "dutform.sc" */	/* host code */
    if (dut_f1_0ddb_count > dut_cb->dut_form_intro->dut_f1_0ddb_size)
    {
	MEfree(dut_f1_4ddblist_p);
	dut_f1_intro_p->dut_f1_4ddblist = NULL;
	dut_f1_4ddblist_p = NULL;
    }
    if (dut_f1_4ddblist_p == NULL)
    {
	dut_f1_4ddblist_p = (DUT_T1_DDBLIST *)MEreqmem(0,
		(dut_f1_0ddb_count + DUT_ME_EXTRA) * sizeof(DUT_T1_DDBLIST),
		TRUE, NULL);
	dut_f1_intro_p->dut_f1_4ddblist = dut_f1_4ddblist_p;
	dut_cb->dut_form_intro->dut_f1_0ddb_size
		= dut_f1_0ddb_count + DUT_ME_EXTRA;
    }
    dut_f1_0ddblist_curr_p		= dut_f1_4ddblist_p;
/* # line 482 "dutform.sc" */	/* getform */
  {
    if (IIfsetio((char *)"dut_f1_intro") != 0) {
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f1_intro_p->dut_f1_2nodename,(char *)"dut_f1_2nodename");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f1_intro_p->dut_f1_3nodename,(char *)"dut_f1_3nodename");
    } /* IIfsetio */
  }
/* # line 488 "dutform.sc" */	/* host code */
    STcopy(dut_f1_intro_p->dut_f1_2nodename, dut_cb->dut_c0_iidbdbnode);
    CVlower(dut_cb->dut_c0_iidbdbnode);
/* # line 492 "dutform.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct lowercase(trim(ddb_name)), lowercase(trim(ddb_owner))\
, lowercase(trim(cdb_name)), lowercase(trim(cdb_node))from iistar_cdbs\
 where lowercase(cdb_node)=");
    IIputdomio((short *)0,1,32,0,dut_cb->dut_c0_iidbdbnode);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f1_0ddblist_curr_p->dut_t1_1ddbname);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f1_0ddblist_curr_p->dut_t1_2ddbowner);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f1_0ddblist_curr_p->dut_3cdbname);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f1_0ddblist_curr_p->dut_4nodename);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 506 "dutform.sc" */	/* loadtable */
      {
        if (IItbact((char *)"dut_f1_intro",(char *)"dut_f1_4ddblist",1) != 0) 
        {
          IItcoputio((char *)"dut_t1_1ddbname",(short *)0,1,32,0,
          dut_f1_0ddblist_curr_p->dut_t1_1ddbname);
          IItcoputio((char *)"dut_t1_2ddbowner",(short *)0,1,32,0,
          dut_f1_0ddblist_curr_p->dut_t1_2ddbowner);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 512 "dutform.sc" */	/* host code */
	dut_f1_0ddb_fill++;
	dut_f1_0ddblist_curr_p++;
	if (sqlca.sqlcode < 0)
	{
	    dut_cb->dut_c8_status = !E_DU_OK;
/* # line 518 "dutform.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 519 "dutform.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    dut_cb->dut_form_intro->dut_f1_0ddb_num_entry = dut_f1_0ddb_fill;
    STcopy(dut_f1_intro_p->dut_f1_2nodename, dut_f1_intro_p->dut_f1_3nodename);
/* # line 526 "dutform.sc" */	/* putform */
  {
    if (IIfsetio((char *)"dut_f1_intro") != 0) {
      IIputfldio((char *)"dut_f1_3nodename",(short *)0,1,32,0,
      dut_f1_intro_p->dut_f1_3nodename);
    } /* IIfsetio */
  }
/* # line 531 "dutform.sc" */	/* host code */
    return ( E_DU_OK );
}
/*
** Name: dut_ff2_1ldblist	- Form 2, 'dut_f2_ldblist'.
**
** Description:
**      FORM 2 is the form that give LDB information for a DDB.
**
** Inputs:
**      dut_cb				Control block for starview.
**      dut_errcb                       Error Control block for starview.
**
** Outputs:
**	Returns:
**	    not applicable.
**	Exceptions:
**
** Side Effects:
**	    none
**
** History:
**      28-nov-1988 (alexc)
**          Creation.
*/
DUT_STATUS
dut_ff2_1ldblist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_F2_LDBLIST *dut_f2_2ldblist_p;
  DUT_T2_LDBLIST *dut_t2_ldblist_p;
  char dut_dbname[DDB_2MAXNAME];
    dut_f2_2ldblist_p = dut_cb->dut_form_ldblist;
    dut_t2_ldblist_p  = dut_f2_2ldblist_p->dut_f2_3ldblist_curr_p;
    dut_cb->dut_c8_status = E_DU_OK;
    dut_cb->dut_c9_status = DUT_F_FORM_3;
    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
                      dut_cb->dut_c1_ddbname);
	dut_cb->dut_c8_status = DUT_ERROR;
	return(DUT_ERROR);
    }
/* # line 583 "dutform.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_f2_ldblist",(char *)"f") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
/* # line 587 "dutform.sc" */	/* host code */
	dut_cb->dut_c8_status = !E_DU_OK;
/* # line 589 "dutform.sc" */	/* inittable */
    {
      if (IItbinit((char *)"dut_f2_ldblist",(char *)"dut_f2_3ldblist",(char *)"\
r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 590 "dutform.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_f2_ldblist") != 0) {
        IIputfldio((char *)"dut_f2_1ddbname",(short *)0,1,32,0,
        dut_cb->dut_c1_ddbname);
        IIputfldio((char *)"dut_f2_2nodename",(short *)0,1,32,0,
        dut_cb->dut_c2_nodename);
      } /* IIfsetio */
    }
/* # line 595 "dutform.sc" */	/* host code */
	dut_cb->dut_c8_status = E_DU_OK;
	dut_ff2_2load_ldb(dut_cb, dut_errcb, dut_f2_2ldblist_p);
	if (dut_cb->dut_c8_status == DUT_ERROR)
	{
/* # line 599 "dutform.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE2;
    }
/* # line 600 "dutform.sc" */	/* host code */
	    dut_cb->dut_c9_status = DUT_F_FORM_1;
	    dut_cb->dut_c8_status = DUT_ERROR;
	    return(DUT_ERROR);
	}
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
	dut_cb->dut_form_ldblist->dut_f2_0modify = FALSE;
/* # line 609 "dutform.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_f2_ldblist",(char *)
"dut_f2_3ldblist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t2_ldblist_p->dut_t2_1status,(char *)"dut_t2_1status");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t2_ldblist_p->dut_t2_2nodename,(char *)"dut_t2_2nodena\
me");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t2_ldblist_p->dut_t2_3ldbname,(char *)"dut_t2_3ldbname");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t2_ldblist_p->dut_t2_4ldbtype,(char *)"dut_t2_4ldbtype");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 616 "dutform.sc" */	/* host code */
	dut_ff3_1objlist(dut_cb, dut_errcb, dut_f2_2ldblist_p);
	if (dut_cb->dut_c8_status != E_DU_OK)
	{
	    dut_ee1_error(dut_errcb, W_DU1808_SV_CANT_GET_LDB_INFO, 2,
		0, dut_cb->dut_c3_ldbname);
	}
	if (dut_cb->dut_form_ldblist->dut_f2_0modify == TRUE)
	{
	    dut_cb->dut_form_ldblist->dut_f2_0modify = FALSE;
	    dut_ff2_4reload_ldb(dut_cb, dut_errcb);
	}
	dut_uu23_reset_ldblist(dut_cb, dut_errcb);
	dut_ff2_4reload_ldb(dut_cb, dut_errcb);
      } else if (IIretval() == 2) {
/* # line 633 "dutform.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_f2_ldblist",(char *)
"dut_f2_3ldblist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t2_ldblist_p->dut_t2_1status,(char *)"dut_t2_1status");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t2_ldblist_p->dut_t2_2nodename,(char *)"dut_t2_2nodena\
me");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t2_ldblist_p->dut_t2_3ldbname,(char *)"dut_t2_3ldbname");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t2_ldblist_p->dut_t2_4ldbtype,(char *)"dut_t2_4ldbtype");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 640 "dutform.sc" */	/* host code */
	STcopy(dut_t2_ldblist_p->dut_t2_2nodename, 
		dut_cb->dut_popup_dbattr->dut_p7_0nodename);
	STcopy(dut_t2_ldblist_p->dut_t2_3ldbname, 
		dut_cb->dut_popup_dbattr->dut_p7_1dbname);
	dut_pp7_1dbattr(dut_cb, dut_errcb);
      } else if (IIretval() == 3) {
	dut_ff2_3test_node(dut_cb, dut_errcb);
/* # line 650 "dutform.sc" */	/* resume */
        {
          IIresmu();
          if (1) goto IIfdB2;
        }
      } else if (IIretval() == 4) {
/* # line 655 "dutform.sc" */	/* host code */
	dut_ff2_5test_ldb(dut_cb, dut_errcb);
/* # line 656 "dutform.sc" */	/* resume */
        {
          IIresmu();
          if (1) goto IIfdB2;
        }
      } else if (IIretval() == 5) {
/* # line 661 "dutform.sc" */	/* host code */
	if ((dut_cb->dut_c2_nodename[0] == EOS) ||
	    !STcompare(dut_cb->dut_c2_nodename, dut_cb->dut_c13_vnodename))
	{
	    STprintf(dut_dbname, "%s/star", dut_cb->dut_c1_ddbname);
	}
	else
	{
	    STprintf(dut_dbname, "%s::%s/star",
			dut_cb->dut_c2_nodename,
			dut_cb->dut_c1_ddbname);
	}
	STprintf(dut_cb->dut_c0_buffer, "ISQL %s", dut_dbname);
/* # line 673 "dutform.sc" */	/* message */
        {
          IImessage(dut_cb->dut_c0_buffer);
        }
/* # line 674 "dutform.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
        }
/* # line 675 "dutform.sc" */	/* call */
        {
          IIsqInit(&sqlca);
          IIutsys(0,(char *)"isql",(char *)0);
          IIutsys(1,(char *)"database",dut_dbname);
          IIutsys(2,(char *)0,(char *)0);
        }
/* # line 676 "dutform.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
        }
/* # line 677 "dutform.sc" */	/* select */
        {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('database')");
          IIretinit((char *)0,0);
          if (IInextget() != 0) {
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer);
          } /* IInextget */
          IIsqFlush((char *)0,0);
        }
/* # line 678 "dutform.sc" */	/* host code */
	/* Want to set it to direct disconnect mode */
	STtrmwhite(dut_cb->dut_c0_buffer);
	if (!STcompare(dut_cb->dut_c0_buffer, dut_cb->dut_c1_ddbname))
	{
	    /* Already direct disconnected in the sub-call */
	}
	else
	{
/* # line 686 "dutform.sc" */	/* direct disconnect */
        {
          IIsqInit(&sqlca);
          IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
          IIsyncup((char *)0,0);
        }
/* # line 687 "dutform.sc" */	/* select */
        {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('database')");
          IIretinit((char *)0,0);
          if (IInextget() != 0) {
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer);
          } /* IInextget */
          IIsqFlush((char *)0,0);
        }
/* # line 688 "dutform.sc" */	/* host code */
	    STtrmwhite(dut_cb->dut_c0_buffer);
	    if (STcompare(dut_cb->dut_c0_buffer, dut_cb->dut_c1_ddbname))
	    {
		/* After direct disconnect, the dbname is not ddb */
		PCexit(0);
	    }
	}
	dut_cb->dut_c7_status = DUT_S_CONNECT_DDB;
/* # line 696 "dutform.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
        }
/* # line 697 "dutform.sc" */	/* redisplay */
        {
          IIredisp();
        }
      } else if (IIretval() == 6) {
/* # line 702 "dutform.sc" */	/* host code */
	if ((dut_cb->dut_c2_nodename[0] == EOS) ||
	    !STcompare(dut_cb->dut_c2_nodename, dut_cb->dut_c13_vnodename))
	{
	    STprintf(dut_dbname, "%s/star", dut_cb->dut_c1_ddbname);
	}
	else
	{
	    STprintf(dut_dbname, "%s::%s/star",
			dut_cb->dut_c2_nodename,
			dut_cb->dut_c1_ddbname);
	}
	STprintf(dut_cb->frs_message, "TABLES %s", dut_dbname);
/* # line 714 "dutform.sc" */	/* message */
        {
          IImessage(dut_cb->frs_message);
        }
/* # line 715 "dutform.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
        }
/* # line 716 "dutform.sc" */	/* call */
        {
          IIsqInit(&sqlca);
          IIutsys(0,(char *)"tables",(char *)0);
          IIutsys(1,(char *)"database",dut_dbname);
          IIutsys(2,(char *)0,(char *)0);
        }
/* # line 717 "dutform.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
        }
/* # line 718 "dutform.sc" */	/* select */
        {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('database')");
          IIretinit((char *)0,0);
          if (IInextget() != 0) {
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer);
          } /* IInextget */
          IIsqFlush((char *)0,0);
        }
/* # line 719 "dutform.sc" */	/* host code */
	STtrmwhite(dut_cb->dut_c0_buffer);
	if (STcompare(dut_cb->dut_c0_buffer, dut_cb->dut_c1_ddbname))
	{
	    /* After direct disconnect, the dbname is not ddb */
	    PCexit(0);
	}
/* # line 725 "dutform.sc" */	/* redisplay */
        {
          IIredisp();
        }
      } else if (IIretval() == 7) {
/* # line 730 "dutform.sc" */	/* host code */
	dut_uu7_table_top(NULL);
      } else if (IIretval() == 8) {
	dut_uu8_table_bottom(NULL);
      } else if (IIretval() == 9) {
/* # line 740 "dutform.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hf2_1name);
        }
      } else if (IIretval() == 10) {
/* # line 749 "dutform.sc" */	/* host code */
	dut_cb->dut_c8_status = E_DU_OK;
	/* Need to clean all LDB, and CDB data in 'dut_cb' */
	dut_cb->dut_c1_ddbname[0] = EOS;
	dut_cb->dut_c2_nodename[0] = EOS;
	dut_cb->dut_c3_ldbname[0] = EOS;
	dut_cb->dut_c4_ldbnode[0] = EOS;
	dut_cb->dut_c5_cdbname[0] = EOS;
	dut_cb->dut_c6_cdbnode[0] = EOS;
	dut_cb->dut_c8_status   = E_DU_OK;
	dut_cb->dut_c9_status   = DUT_F_FORM_1;
/* # line 759 "dutform.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"ListObj",(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact((char *)"LDBAttr",(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact((char *)"TestNode",(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact((char *)"TestLDB",(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IInmuact((char *)"SQL",(char *)0,2,2,5) == 0) goto IIfdE2;
    if (IInmuact((char *)"Tables",(char *)0,2,2,6) == 0) goto IIfdE2;
    if (IInmuact((char *)"Top",(char *)0,2,2,7) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,7) == 0) goto IIfdE2;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,8) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,8) == 0) goto IIfdE2;
    if (IInmuact((char *)"Help",(char *)0,2,2,9) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,9) == 0) goto IIfdE2;
    if (IInmuact((char *)"End",(char *)0,2,2,10) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,10) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 762 "dutform.sc" */	/* host code */
}
/*
** Name: dur_ff2_2load_ldb - Loads Local databases from iidd_ddb_ldbids.
**
** Description:
**      Loads local database information from iidd_ddb_ldbids.  This means 
**      all local databases that have objects registered in this Distributed
**      database.
**
** Inputs:
**      dut_cb                          StarView main control block.
**      dut_errcb                       StarView error control block.
**      dut_f2_ldblist_p                pointer to array of LDBs.
**
** Outputs:
**      None.
**	Returns:
**	    None
**	Exceptions:
**	    None
**
** Side Effects:
**	    None
**
** History:
**      5-dec-1988 (alexc)
**          Creation.
*/
DUT_STATUS
dut_ff2_2load_ldb(dut_cb, dut_errcb, dut_f2_ldblist_p)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
  DUT_F2_LDBLIST *dut_f2_ldblist_p;
{
  DUT_T2_LDBLIST *dut_f2_3ldblist_p;
  DUT_T2_LDBLIST *dut_f2_0ldblist_curr_p;
  i4 dut_f2_0ldb_count;
  char dut_temp_ddbname[DDB_MAXNAME + 4];
  char dut_temp_ddbnode[DDB_MAXNAME + 4];
    i4			dut_f2_0ldb_fill;
    dut_f2_ldblist_p->dut_f2_0modify = FALSE;
    dut_f2_0ldb_count = 0;
    dut_f2_0ldb_fill  = 0;
    dut_f2_3ldblist_p = (DUT_T2_LDBLIST *)dut_f2_ldblist_p->dut_f2_3ldblist;
/* # line 814 "dutform.sc" */	/* getform */
  {
    if (IIfsetio((char *)"dut_f2_ldblist") != 0) {
      IIgetfldio((short *)0,1,32,DDB_MAXNAME + 4-1,dut_temp_ddbname,(char *)"\
dut_f2_1ddbname");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME + 4-1,dut_temp_ddbnode,(char *)"\
dut_f2_2nodename");
    } /* IIfsetio */
  }
/* # line 820 "dutform.sc" */	/* host code */
    if (!STcompare(dut_cb->dut_c1_ddbname, dut_temp_ddbname) &&
	!STcompare(dut_cb->dut_c2_nodename, dut_temp_ddbnode))
    {
/* # line 823 "dutform.sc" */	/* clear */
  {
    IIclrflds();
  }
/* # line 824 "dutform.sc" */	/* host code */
    }
    else
    {
	return(E_DU_OK);
    }
/* # line 830 "dutform.sc" */	/* putform */
  {
    if (IIfsetio((char *)"dut_f2_ldblist") != 0) {
      IIputfldio((char *)"dut_f2_1ddbname",(short *)0,1,32,0,
      dut_cb->dut_c1_ddbname);
      IIputfldio((char *)"dut_f2_2nodename",(short *)0,1,32,0,
      dut_cb->dut_c2_nodename);
    } /* IIfsetio */
  }
/* # line 836 "dutform.sc" */	/* host code */
    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
			dut_cb->dut_c1_ddbname);
	dut_cb->dut_c8_status = DUT_ERROR;
	return(DUT_ERROR);
    }
/* # line 843 "dutform.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from iiddb_ld\
bids");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_f2_0ldb_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 847 "dutform.sc" */	/* host code */
    if (dut_f2_0ldb_count > dut_cb->dut_form_ldblist->dut_f2_0ldb_size)
    {
	MEfree(dut_f2_3ldblist_p);
	dut_f2_ldblist_p->dut_f2_3ldblist = NULL;
	dut_f2_3ldblist_p = NULL;
    }
    if (dut_f2_3ldblist_p == NULL)
    {
	dut_f2_3ldblist_p = (DUT_T2_LDBLIST *)MEreqmem(0,
		(dut_f2_0ldb_count + DUT_ME_EXTRA) * sizeof(DUT_T2_LDBLIST),
		TRUE, NULL);
	dut_f2_ldblist_p->dut_f2_3ldblist = dut_f2_3ldblist_p;
	dut_cb->dut_form_ldblist->dut_f2_0ldb_size 
		= dut_f2_0ldb_count + DUT_ME_EXTRA;
    }
    dut_f2_0ldblist_curr_p	= dut_f2_3ldblist_p;
/* # line 867 "dutform.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct ldb_id, trim(ldb_node), trim(ldb_database), trim(ldb_\
dbms)from iiddb_ldbids order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_f2_0ldblist_curr_p->dut_t2_0ldbid);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f2_0ldblist_curr_p->dut_t2_2nodename);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f2_0ldblist_curr_p->dut_t2_3ldbname);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f2_0ldblist_curr_p->dut_t2_4ldbtype);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 880 "dutform.sc" */	/* host code */
	STcopy("?", dut_f2_0ldblist_curr_p->dut_t2_1status);
	dut_f2_0ldb_fill++;
	dut_f2_0ldblist_curr_p++;
	if (sqlca.sqlcode < 0)
	{
	    dut_cb->dut_c8_status = !E_DU_OK;
/* # line 888 "dutform.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 889 "dutform.sc" */	/* host code */
	    dut_ee1_error(dut_errcb, W_DU1807_SV_CANT_GET_DDB_INFO, 2,
		0, dut_cb->dut_c1_ddbname);
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
    dut_cb->dut_form_ldblist->dut_f2_0ldb_num_entry = dut_f2_0ldb_fill;
    if (dut_f2_0ldblist_curr_p->dut_t2_0ldbid == 1)
    {
	STcopy(dut_f2_0ldblist_curr_p->dut_t2_3ldbname,
		dut_cb->dut_c5_cdbname);
	STcopy(dut_f2_0ldblist_curr_p->dut_t2_2nodename,
		dut_cb->dut_c6_cdbnode);
    }
    dut_uu13_buble_sort(dut_cb->dut_form_ldblist->dut_f2_3ldblist,
		sizeof(DUT_T2_LDBLIST), DDB_MAXNAME, 
		dut_cb->dut_form_ldblist->dut_f2_0ldb_num_entry);
    dut_ff2_4reload_ldb(dut_cb, dut_errcb);
    if (sqlca.sqlcode < 0)
    {
	int sqlcode = sqlca.sqlcode;
	dut_ee1_error(dut_errcb, W_DU1808_SV_CANT_GET_LDB_INFO, 2,
		0, dut_cb->dut_c3_ldbname);
	dut_cb->dut_c8_status = DUT_ERROR;
	return(sqlcode);
    }
    dut_cb->dut_c8_status = E_DU_OK;
    return(E_DU_OK);
}
/*
** Name: dut_ff2_3test_node - Test connection to node by connecting to iidbdb
**				of the nodes.
**
** Description:
**      Test node options: SingleNode, AllNodes, Newnode. 
**
** Inputs:
**      dut_cb->dut_form_ldblist    - pointer to list of local database, and
**				    node.
**
** Outputs:
**	dut_cb->dut_f2_3ldblist->dut_t2_1status	- '?' -> ['up'|'down'].
**
**	Returns:
**	    E_DU_OK
**	Exceptions:
**
** Side Effects:
**
** History:
**      5-dec-1988 (alexc)
**          Creation.
**     25-jul-1991 (fpang)
**	    - Cleanup associations after AllLdbs and AllNodes sub menu
**	      selections only. Don't do it when 'End' is selected
**	      because connections were never made.
**	    - Nicer text when displaying node/ldb accessibility message.
*/
DUT_STATUS
dut_ff2_3test_node(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  char new_nodename[DDB_MAXNAME];
  i4 dut_f2_curr_row;
  DUT_T2_LDBLIST *dut_t2_curr_p;
    char dut_buffer[DB_2MAXNAME];
    dut_cb->dut_on_error_exit = FALSE;
    dut_cb->dut_on_error_noop = TRUE;
    dut_t2_curr_p = dut_cb->dut_form_ldblist->dut_f2_3ldblist_curr_p;
/* # line 971 "dutform.sc" */	/* submenu */
  {
    goto IImuI3;
IIfdB3:
    IIrunmu(0);
    if (IIretval() == 1) {
/* # line 975 "dutform.sc" */	/* host code */
	if (!dut_uu12_empty_table(dut_cb, dut_errcb, "dut_f2_3ldblist"))
	{
	    dut_ee1_error(dut_errcb, W_DU1812_SV_EMPTY_TABLE, 0);
	}
        else
	{
/* # line 981 "dutform.sc" */	/* putrow */
      {
        if (IItbsetio(3,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldbl\
ist",-2) != 0) {
          IItcoputio((char *)"dut_t2_1status",(short *)0,1,32,0,(char *)"\
?");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 985 "dutform.sc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 986 "dutform.sc" */	/* getrow */
      {
        if (IItbsetio(2,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldbl\
ist",-2) != 0) {
          IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
          dut_t2_curr_p->dut_t2_1status,(char *)"dut_t2_1status");
          IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
          dut_t2_curr_p->dut_t2_2nodename,(char *)"dut_t2_2nodename");
          IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
          dut_t2_curr_p->dut_t2_3ldbname,(char *)"dut_t2_3ldbname");
          IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
          dut_t2_curr_p->dut_t2_4ldbtype,(char *)"dut_t2_4ldbtype");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 993 "dutform.sc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,(char *)"dut_f2_ldblist",(char *)0, (char *)0) != 0) 
        {
          IIiqfsio((short *)0,1,30,4,&dut_f2_curr_row,29,(char *)
"dut_f2_3ldblist",0);
        } /* IIiqset */
      }
/* # line 997 "dutform.sc" */	/* host code */
	    dut_uu21_test_node(dut_cb, dut_errcb,
				dut_t2_curr_p->dut_t2_2nodename);
	    dut_ff2_4reload_ldb(dut_cb, dut_errcb);
/* # line 1000 "dutform.sc" */	/* scroll */
      {
        if (IItbsetio(1,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldbl\
ist",-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,dut_f2_curr_row) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
/* # line 1002 "dutform.sc" */	/* host code */
	}
    } else if (IIretval() == 2) {
	dut_uu20_test_nodelist(dut_cb, dut_errcb);
/* # line 1008 "dutform.sc" */	/* message */
      {
        IImessage((char *)"Cleaning up associations. . .");
      }
/* # line 1009 "dutform.sc" */	/* host code */
	dut_uu2_dbop(DUT_DDBCLOSE, dut_cb, dut_errcb);
	if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
	{
	    dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0,
			  dut_cb->dut_c1_ddbname);
	}
	dut_ff2_4reload_ldb(dut_cb, dut_errcb);
    } else if (IIretval() == 3) {
/* # line 1020 "dutform.sc" */	/* help_frs */
      {
        IIfrshelp(0,(char *)"Distributed Database Help",
        dut_cb->dut_helpcb->dut_hf2_2name);
      }
    } else if (IIretval() == 4) {
    } else {
      if (1) goto IIfdE3;
    } /* IIretval */
    goto IIfdE3;
IImuI3:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact((char *)"SingleNode",(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IInmuact((char *)"AllNodes",(char *)0,2,2,2) == 0) goto IIfdE3;
    if (IInmuact((char *)"Help",(char *)0,2,2,3) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE3;
    if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdB3;
IIfdE3:;
  }
/* # line 1030 "dutform.sc" */	/* host code */
}
/*
** Name: dur_ff2_4reload_ldb - Reload ldb list from array to screen again
**			    without reloading the array from database.
**
** Description:
**      Reload LDB from dut_cb->dut_form_ldblist->dut_f2_3ldblist(array)
**	    to table field in dut_f2_ldblist. 
**
** Inputs:
**      dut_cb	                         .dut_form_ldblist
**					    pointer to form structure.
**
** Outputs:
**      none. #41$
**	Returns:
**	    E_DU_OK
**	    DUT_ERROR
**	Exceptions:
**
** Side Effects:
**
** History:
**      5-dec-1988 (alexc)
**          Creation.
*/
DUT_STATUS
dut_ff2_4reload_ldb(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
    /* Debugging routine */
  DUT_T2_LDBLIST *t2_p;
  DUT_T2_LDBLIST t2_0buffer;
  DUT_T2_LDBLIST t2_1buffer;
  i4 t2_row;
    i4			t2_cnt;
    char		buffer[100];
    t2_p	= dut_cb->dut_form_ldblist->dut_f2_3ldblist;
    t2_cnt	= dut_cb->dut_form_ldblist->dut_f2_0ldb_num_entry;
    t2_row	= 0;
    if (dut_uu12_empty_table(dut_cb, dut_errcb, "dut_f2_3ldblist"))
    {
/* # line 1081 "dutform.sc" */	/* getrow */
  {
    if (IItbsetio(2,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldblist",-2) != 
    0) {
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,t2_0buffer.dut_t2_1status,
      (char *)"dut_t2_1status");
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,t2_0buffer.dut_t2_2nodename,
      (char *)"dut_t2_2nodename");
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,t2_0buffer.dut_t2_3ldbname,
      (char *)"dut_t2_3ldbname");
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,t2_0buffer.dut_t2_4ldbtype,
      (char *)"dut_t2_4ldbtype");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 1088 "dutform.sc" */	/* host code */
	t2_row = 1;
    }
/* # line 1090 "dutform.sc" */	/* clear */
  {
    IIfldclear((char *)"dut_f2_3ldblist");
  }
/* # line 1092 "dutform.sc" */	/* host code */
    while (t2_cnt--)
    {
/* # line 1094 "dutform.sc" */	/* loadtable */
  {
    if (IItbact((char *)"dut_f2_ldblist",(char *)"dut_f2_3ldblist",1) != 0) {
      IItcoputio((char *)"dut_t2_1status",(short *)0,1,32,0,
      t2_p->dut_t2_1status);
      IItcoputio((char *)"dut_t2_2nodename",(short *)0,1,32,0,
      t2_p->dut_t2_2nodename);
      IItcoputio((char *)"dut_t2_3ldbname",(short *)0,1,32,0,
      t2_p->dut_t2_3ldbname);
      IItcoputio((char *)"dut_t2_4ldbtype",(short *)0,1,32,0,
      t2_p->dut_t2_4ldbtype);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1101 "dutform.sc" */	/* host code */
	t2_p++;
    }
    /* Move cursor back to where it was */
    if (t2_row == 0)
    {
/* # line 1107 "dutform.sc" */	/* unloadtable */
  {
    if (IItbact((char *)"dut_f2_ldblist",(char *)"dut_f2_3ldblist",0) == 0) 
    goto IItbE1;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,t2_1buffer.dut_t2_1status,
      (char *)"dut_t2_1status");
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,t2_1buffer.dut_t2_2nodename,
      (char *)"dut_t2_2nodename");
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,t2_1buffer.dut_t2_3ldbname,
      (char *)"dut_t2_3ldbname");
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,t2_1buffer.dut_t2_4ldbtype,
      (char *)"dut_t2_4ldbtype");
      IItcogetio((short *)0,1,30,4,&t2_row,(char *)"_RECORD");
      IITBceColEnd();
/* # line 1116 "dutform.sc" */	/* host code */
	    if (!STcompare(t2_0buffer.dut_t2_2nodename,
			t2_1buffer.dut_t2_2nodename)
			&&
		!STcompare(t2_0buffer.dut_t2_3ldbname,
			t2_1buffer.dut_t2_3ldbname))
	    {
/* # line 1122 "dutform.sc" */	/* scroll */
      {
        if (IItbsetio(1,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldbl\
ist",-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,t2_row) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
/* # line 1124 "dutform.sc" */	/* host code */
	    }
    } /* IItunload */
IItbE1:
    IItunend();
  }
    }
    else
    {
/* # line 1129 "dutform.sc" */	/* scroll */
  {
    if (IItbsetio(1,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldblist",-3) != 
    0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,t2_row) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 1131 "dutform.sc" */	/* host code */
    }
    return(E_DU_OK);
}
/*
** Name: dur_ff2_5test_ldb - Test connection to local database.
**
** Description:
**      Driver routine to test connection to local database by doing
**	    direct connects to the local database.  User can choose the
**	    following options in testing LDB connections: 
**		SingleLDB   - test single local database. 
**		AllLDBs	    - test all local databases. 
**		NewLDB	    - Allow user to enter a database name. 
**
** Inputs:
**      none #41$
**
** Outputs:
**      none. #41$
**	Returns:
**	    E_DU_OK
**	    DUT_ERROR
**	Exceptions:
**
** Side Effects:
**
** History:
**      23-mar-1988 (alexc)
**          Creation.
**     25-jul-1991 (fpang)
**	    - Cleanup associations after AllLdbs and AllNodes sub menu
**	      selections only. Don't do it when 'End' is selected
**	      because connections were never made.
**	    - Nicer text when displaying node/ldb accessibility message.
*/
DUT_STATUS
dut_ff2_5test_ldb(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  char new_nodename[DDB_MAXNAME];
  char new_ldbname[DDB_MAXNAME];
  i4 dut_f2_curr_row;
  DUT_T2_LDBLIST *dut_t2_curr_p;
    char dut_buffer[DB_2MAXNAME];
    dut_t2_curr_p = dut_cb->dut_form_ldblist->dut_f2_3ldblist_curr_p;
/* # line 1185 "dutform.sc" */	/* submenu */
  {
    goto IImuI4;
IIfdB4:
    IIrunmu(0);
    if (IIretval() == 1) {
/* # line 1189 "dutform.sc" */	/* host code */
	if (!dut_uu12_empty_table(dut_cb, dut_errcb, "dut_f2_3ldblist"))
	{
	    dut_ee1_error(dut_errcb, W_DU1812_SV_EMPTY_TABLE, 0);
	}
        else
        {
/* # line 1195 "dutform.sc" */	/* putrow */
      {
        if (IItbsetio(3,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldbl\
ist",-2) != 0) {
          IItcoputio((char *)"dut_t2_1status",(short *)0,1,32,0,(char *)"\
?");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 1199 "dutform.sc" */	/* redisplay */
      {
        IIredisp();
      }
/* # line 1200 "dutform.sc" */	/* getrow */
      {
        if (IItbsetio(2,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldbl\
ist",-2) != 0) {
          IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
          dut_t2_curr_p->dut_t2_1status,(char *)"dut_t2_1status");
          IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
          dut_t2_curr_p->dut_t2_2nodename,(char *)"dut_t2_2nodename");
          IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
          dut_t2_curr_p->dut_t2_3ldbname,(char *)"dut_t2_3ldbname");
          IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
          dut_t2_curr_p->dut_t2_4ldbtype,(char *)"dut_t2_4ldbtype");
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 1207 "dutform.sc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,(char *)"dut_f2_ldblist",(char *)0, (char *)0) != 0) 
        {
          IIiqfsio((short *)0,1,30,4,&dut_f2_curr_row,29,(char *)
"dut_f2_3ldblist",0);
        } /* IIiqset */
      }
/* # line 1211 "dutform.sc" */	/* host code */
	    dut_uu19_test_ldb(dut_cb, dut_errcb, dut_t2_curr_p);
	    dut_ff2_4reload_ldb(dut_cb, dut_errcb);
/* # line 1213 "dutform.sc" */	/* scroll */
      {
        if (IItbsetio(1,(char *)"dut_f2_ldblist",(char *)"dut_f2_3ldbl\
ist",-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,dut_f2_curr_row) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
/* # line 1215 "dutform.sc" */	/* host code */
	}
    } else if (IIretval() == 2) {
	dut_uu22_test_ldblist(dut_cb, dut_errcb);
	/* Disconnect from DDB and reconnect back to DDB
	** to clean up associations.
	*/
/* # line 1224 "dutform.sc" */	/* message */
      {
        IImessage((char *)"Cleaning up associations. . .");
      }
/* # line 1225 "dutform.sc" */	/* host code */
	dut_uu2_dbop(DUT_DDBCLOSE, dut_cb, dut_errcb);
	if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
	{
	    dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
			dut_cb->dut_c1_ddbname);
	}
	dut_ff2_4reload_ldb(dut_cb, dut_errcb);
    } else if (IIretval() == 3) {
	dut_cb->dut_on_error_exit = FALSE;
	dut_cb->dut_on_error_noop = TRUE;
/* # line 1238 "dutform.sc" */	/* prompt */
      {
        IIprmptio(0,(char *)"Enter New Node Name:",(short *)0,1,32,DDB_MAXNAME
        -1,new_nodename);
      }
/* # line 1243 "dutform.sc" */	/* host code */
	if (new_nodename[0] == EOS)
	    return(DUT_ERROR);
/* # line 1245 "dutform.sc" */	/* prompt */
      {
        IIprmptio(0,(char *)"Enter New LDB Name:",(short *)0,1,32,DDB_MAXNAME
        -1,new_ldbname);
      }
/* # line 1250 "dutform.sc" */	/* host code */
	if (new_ldbname[0] == EOS)
	    return(DUT_ERROR);
	STcopy(new_nodename, dut_cb->dut_c4_ldbnode);
	STcopy(new_ldbname, dut_cb->dut_c3_ldbname);
	if (dut_uu2_dbop(DUT_DIRECT_CONNECT, dut_cb, dut_errcb) != E_DU_OK)
	{
	    /* Not accessible */
	    STprintf(dut_buffer,DUT_M02_NOTACCESS,new_nodename,new_ldbname);
	}
	else
	{
	    /* Accessible */
	    STprintf(dut_buffer,DUT_M03_ACCESS,new_nodename,new_ldbname);
	}
	dut_pp11_1perror(dut_buffer);
    } else if (IIretval() == 4) {
/* # line 1270 "dutform.sc" */	/* help_frs */
      {
        IIfrshelp(0,(char *)"Distributed Database Help",
        dut_cb->dut_helpcb->dut_hf2_5name);
      }
    } else if (IIretval() == 5) {
    } else {
      if (1) goto IIfdE4;
    } /* IIretval */
    goto IIfdE4;
IImuI4:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE4;
    if (IInmuact((char *)"SingleLDB",(char *)0,2,2,1) == 0) goto IIfdE4;
    if (IInmuact((char *)"AllLDBs",(char *)0,2,2,2) == 0) goto IIfdE4;
    if (IInmuact((char *)"NewLDB",(char *)0,2,2,3) == 0) goto IIfdE4;
    if (IInmuact((char *)"Help",(char *)0,2,2,4) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE4;
    if (IInmuact((char *)"End",(char *)0,2,2,5) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE4;
    if (IIendmu() == 0) goto IIfdE4;
    goto IIfdB4;
IIfdE4:;
  }
/* # line 1280 "dutform.sc" */	/* host code */
}
/*
** Name: dur_ff3_1objlist - List distributed objects in distributed database.
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      5-dec-1988 (alexc)
**          Creation.
**     25-jul-1991 (fpang)
**	    - Fix B38733, indexes still displayed when registered object
**	      with indexes is removed. The reverse was also true, when
**	      local object with indexes was registered, indexes were not
**	      displayed.
**	    - Changed Browse/Remove to reload the objlist because they may
**	      may have changed as a result of the Register/Remove.
**	      Added dut_ff3_7objects() and dut_ff3_8fixlist() to support
**	      reloading of objlist.
*/
DUT_STATUS
dut_ff3_1objlist(dut_cb, dut_errcb, dut_f2_ldblist_p)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
  DUT_F2_LDBLIST *dut_f2_ldblist_p;
{
  DUT_F3_OBJLIST *dut_f3_objlist_p;
  DUT_T3_OBJLIST *dut_t3_objlist_p;
  DUT_P3_REGISTER *dut_p3_register_p;
    dut_cb->dut_c9_status = DUT_F_FORM_3;
    dut_f3_objlist_p	= dut_cb->dut_form_objlist;
    dut_p3_register_p	= dut_cb->dut_popup_register;
    dut_t3_objlist_p	= dut_cb->dut_form_objlist->dut_f3_9objlist_curr_p;
    STcopy(dut_f2_ldblist_p->dut_f2_3ldblist_curr_p->dut_t2_2nodename, 
		dut_cb->dut_c4_ldbnode);
    STcopy(dut_f2_ldblist_p->dut_f2_3ldblist_curr_p->dut_t2_3ldbname, 
		dut_cb->dut_c3_ldbname);
/* # line 1332 "dutform.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_f3_objlist",(char *)"f") == 0) goto IIfdE5;
    goto IImuI5;
IIfdI5:;
    if (IIfsetio((char *)0) == 0) goto IIfdE5;
/* # line 1336 "dutform.sc" */	/* inittable */
    {
      if (IItbinit((char *)"dut_f3_objlist",(char *)"dut_f3_9objlist",(char *)"\
r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 1337 "dutform.sc" */	/* host code */
	dut_cb->dut_c8_status = !E_DU_OK;
/* # line 1338 "dutform.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_f3_objlist") != 0) {
        IIputfldio((char *)"dut_f3_8ddbname",(short *)0,1,32,0,
        dut_cb->dut_c1_ddbname);
        IIputfldio((char *)"dut_f3_1nodename",(short *)0,1,32,0,(char *)"\
*");
        IIputfldio((char *)"dut_f3_2dbname",(short *)0,1,32,0,(char *)"\
*");
        IIputfldio((char *)"dut_f3_3owner",(short *)0,1,32,0,(char *)"\
*");
        IIputfldio((char *)"dut_f3_5show_view",(short *)0,1,32,0,(char *)
"y");
        IIputfldio((char *)"dut_f3_6show_table",(short *)0,1,32,0,(char *)"\
y");
        IIputfldio((char *)"dut_f3_7show_system",(short *)0,1,32,0,(char *)"\
n");
      } /* IIfsetio */
    }
/* # line 1348 "dutform.sc" */	/* host code */
	dut_ff3_2load_obj(dut_cb, dut_errcb);
IIfdB5:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
	if (!dut_uu12_empty_table(dut_cb, dut_errcb, "dut_f3_9objlist"))
	{
	    dut_ee1_error(dut_errcb, W_DU1812_SV_EMPTY_TABLE, 0);
	}
	else
	{
	    dut_ff3_4objinfo(dut_cb, dut_errcb);
	}
      } else if (IIretval() == 2) {
	dut_cb->dut_c8_status = E_DU_OK;
	dut_pp14_1browse(dut_cb, dut_errcb);
	if (dut_cb->dut_c8_status == DUT_OBJLIST_CHANGED)
	{
	    dut_ff3_7objects(dut_cb, dut_errcb);
            dut_ff3_3reload_obj(dut_cb, dut_errcb);
	    dut_cb->dut_c8_status = E_DU_OK;
	}
	dut_uu17_newldb(dut_cb, dut_errcb);
/* # line 1376 "dutform.sc" */	/* redisplay */
        {
          IIredisp();
        }
      } else if (IIretval() == 3) {
/* # line 1382 "dutform.sc" */	/* host code */
	if (!dut_uu12_empty_table(dut_cb, dut_errcb, "dut_f3_9objlist"))
	{
	    dut_ee1_error(dut_errcb, W_DU1812_SV_EMPTY_TABLE, 0);
	}
	else
	{
/* # line 1388 "dutform.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_f3_objlist",(char *)
"dut_f3_9objlist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t3_objlist_p->dut_t3_0objowner,(char *)"dut_t3_0objown\
er");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t3_objlist_p->dut_t3_1objname,(char *)"dut_t3_1objname");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t3_objlist_p->dut_t3_2objtype,(char *)"dut_t3_2objtype");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1394 "dutform.sc" */	/* host code */
	    dut_ff3_6remove_object(dut_cb, dut_errcb);
	}
      } else if (IIretval() == 4) {
/* # line 1401 "dutform.sc" */	/* getform */
        {
          if (IIfsetio((char *)"dut_f3_objlist") != 0) {
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_1nodename,(char *)"dut_f3_1nodena\
me");
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_2dbname,(char *)"dut_f3_2dbname");
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_3owner,(char *)"dut_f3_3owner");
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_5show_view,(char *)"dut_f3_5show_\
view");
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_6show_table,(char *)
"dut_f3_6show_table");
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_7show_system,(char *)
"dut_f3_7show_system");
          } /* IIfsetio */
        }
/* # line 1412 "dutform.sc" */	/* host code */
	dut_pp2_1criteria(dut_cb, dut_errcb, dut_f3_objlist_p);
/* # line 1414 "dutform.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_f3_objlist") != 0) {
            IIputfldio((char *)"dut_f3_1nodename",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_1nodename);
            IIputfldio((char *)"dut_f3_2dbname",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_2dbname);
            IIputfldio((char *)"dut_f3_3owner",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_3owner);
            IIputfldio((char *)"dut_f3_5show_view",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_5show_view);
            IIputfldio((char *)"dut_f3_6show_table",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_6show_table);
            IIputfldio((char *)"dut_f3_7show_system",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_7show_system);
          } /* IIfsetio */
        }
/* # line 1423 "dutform.sc" */	/* host code */
	if (dut_cb->dut_c8_status == DUT_S_NEW_OBJ_SELECT)
	{
	    dut_cb->dut_c8_status = E_DU_OK;
	    dut_ff3_3reload_obj(dut_cb, dut_errcb);
	}
/* # line 1428 "dutform.sc" */	/* redisplay */
        {
          IIredisp();
        }
      } else if (IIretval() == 5) {
/* # line 1433 "dutform.sc" */	/* host code */
	dut_uu7_table_top(NULL);
      } else if (IIretval() == 6) {
	dut_uu8_table_bottom(NULL);
      } else if (IIretval() == 7) {
	dut_uu9_table_find(dut_cb, dut_errcb, DUT_M04_OBJNAME, NULL, NULL);
      } else if (IIretval() == 8) {
/* # line 1448 "dutform.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hf3_1name);
        }
      } else if (IIretval() == 9) {
/* # line 1457 "dutform.sc" */	/* host code */
	dut_cb->dut_c8_status = E_DU_OK;
	dut_cb->dut_c9_status = DUT_F_FORM_2;
/* # line 1459 "dutform.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"ObjAttr",(char *)0,2,2,1) == 0) goto IIfdE5;
    if (IInmuact((char *)"Browse",(char *)0,2,2,2) == 0) goto IIfdE5;
    if (IInmuact((char *)"Remove",(char *)0,2,2,3) == 0) goto IIfdE5;
    if (IInmuact((char *)"Criteria",(char *)0,2,2,4) == 0) goto IIfdE5;
    if (IInmuact((char *)"Top",(char *)0,2,2,5) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,5) == 0) goto IIfdE5;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,6) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,6) == 0) goto IIfdE5;
    if (IInmuact((char *)"Find",(char *)0,2,2,7) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,7) == 0) goto IIfdE5;
    if (IInmuact((char *)"Help",(char *)0,2,2,8) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,8) == 0) goto IIfdE5;
    if (IInmuact((char *)"End",(char *)0,2,2,9) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,9) == 0) goto IIfdE5;
    if (IIendmu() == 0) goto IIfdE5;
    goto IIfdI5;
IIfdE5:;
    IIendfrm();
  }
/* # line 1461 "dutform.sc" */	/* host code */
}
/*
** Name: dut_ff3_2load_obj - 
**
** Description:
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      5-dec-1988 (alexc)
**          Creation.
**     24-jul-1991 (fpang)
**	    Moved majority of code into dut_ff3_7objects().
*/
DUT_STATUS
dut_ff3_2load_obj(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_F3_OBJLIST *dut_f3_objlist_p;
  DUT_T3_OBJLIST *dut_f3_9objlist_p;
  DUT_T3_OBJLIST *dut_f3_0objlist_curr_p;
  i4 dut_f3_0obj_count;
  char dut_tmp_1ddbname[DDB_MAXNAME + 4];
  char dut_tmp_2ddbnode[DDB_MAXNAME + 4];
  char dut_tmp_3dbname[DDB_MAXNAME + 4];
  char dut_tmp_4owner[DDB_MAXNAME + 4];
  char dut_tmp_buffer[DB_LONGMSG];
    dut_f3_objlist_p = (DUT_F3_OBJLIST *)dut_cb->dut_form_objlist;
/* # line 1503 "dutform.sc" */	/* getform */
  {
    if (IIfsetio((char *)"dut_f3_objlist") != 0) {
      IIgetfldio((short *)0,1,32,DDB_MAXNAME + 4-1,dut_tmp_1ddbname,(char *)"\
dut_f3_8ddbname");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME + 4-1,dut_tmp_2ddbnode,(char *)"\
dut_f3_1nodename");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME + 4-1,dut_tmp_3dbname,(char *)"\
dut_f3_2dbname");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME + 4-1,dut_tmp_4owner,(char *)"\
dut_f3_3owner");
    } /* IIfsetio */
  }
/* # line 1511 "dutform.sc" */	/* getform */
  {
    if (IIfsetio((char *)"dut_f3_objlist") != 0) {
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_1nodename,(char *)"dut_f3_1nodename");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_2dbname,(char *)"dut_f3_2dbname");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,dut_f3_objlist_p->dut_f3_3owner
      ,(char *)"dut_f3_3owner");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_5show_view,(char *)"dut_f3_5show_view");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_6show_table,(char *)"dut_f3_6show_table");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_7show_system,(char *)"dut_f3_7show_syst\
em");
    } /* IIfsetio */
  }
/* # line 1521 "dutform.sc" */	/* host code */
    dut_cb->dut_form_objlist->dut_f3_0obj_num_entry = 0;
/* # line 1523 "dutform.sc" */	/* message */
  {
    IImessage((char *)"Loading Distributed Objects List");
  }
/* # line 1525 "dutform.sc" */	/* host code */
    dut_ff3_7objects(dut_cb, dut_errcb);
    dut_ff3_3reload_obj(dut_cb, dut_errcb);
}
/*
** Name: dut_ff3_3reload_obj
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      5-dec-1988 (alexc)
**          Creation.
*/
DUT_STATUS
dut_ff3_3reload_obj(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_T3_OBJLIST *dut_f3_0objlist_curr_p;
  DUT_F3_OBJLIST *dut_f3_objlist_p;
  i4 dut_f3_0obj_count;
  i4 dut_f3_scroll;
  i4 dut_f3_current;
    i4			dut_f3_0obj_fill;
    dut_f3_current	    = 0;
    dut_f3_scroll	    = 0;
    dut_f3_0obj_count	    = dut_cb->dut_form_objlist->dut_f3_0obj_num_entry;
    dut_f3_objlist_p	    = dut_cb->dut_form_objlist;
    dut_f3_0objlist_curr_p  = dut_f3_objlist_p->dut_f3_9objlist;
/* # line 1568 "dutform.sc" */	/* clear */
  {
    IIfldclear((char *)"dut_f3_9objlist");
  }
/* # line 1570 "dutform.sc" */	/* getform */
  {
    if (IIfsetio((char *)"dut_f3_objlist") != 0) {
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_1nodename,(char *)"dut_f3_1nodename");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_2dbname,(char *)"dut_f3_2dbname");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,dut_f3_objlist_p->dut_f3_3owner
      ,(char *)"dut_f3_3owner");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_5show_view,(char *)"dut_f3_5show_view");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_6show_table,(char *)"dut_f3_6show_table");
      IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_objlist_p->dut_f3_7show_system,(char *)"dut_f3_7show_syst\
em");
    } /* IIfsetio */
  }
/* # line 1580 "dutform.sc" */	/* host code */
    while (dut_f3_0obj_count--)
    {
	CVupper(dut_f3_0objlist_curr_p->dut_t3_0system);
	CVupper(dut_f3_0objlist_curr_p->dut_t3_0objtype);
	dut_uu11_chk_criteria(dut_cb, dut_errcb, dut_f3_0objlist_curr_p);
	if (dut_f3_0objlist_curr_p->dut_t3_0select &&
		(dut_f3_0objlist_curr_p->dut_t3_0delete == FALSE))
	{
	    dut_f3_current++;
	    if (!STcompare(dut_f3_0objlist_curr_p->dut_t3_1objname,
		dut_f3_objlist_p->dut_f3_9objlist_curr_p->dut_t3_1objname))
	    {
		dut_f3_scroll = dut_f3_current;
	    }
/* # line 1596 "dutform.sc" */	/* loadtable */
  {
    if (IItbact((char *)"dut_f3_objlist",(char *)"dut_f3_9objlist",1) != 0) {
      IItcoputio((char *)"dut_t3_0objowner",(short *)0,1,32,0,
      dut_f3_0objlist_curr_p->dut_t3_0objowner);
      IItcoputio((char *)"dut_t3_1objname",(short *)0,1,32,0,
      dut_f3_0objlist_curr_p->dut_t3_1objname);
      IItcoputio((char *)"dut_t3_2objtype",(short *)0,1,32,0,
      dut_f3_0objlist_curr_p->dut_t3_2objtype);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1602 "dutform.sc" */	/* host code */
	}
	dut_f3_0objlist_curr_p++;
    }
    if (dut_f3_scroll != 0)
    {
/* # line 1607 "dutform.sc" */	/* scroll */
  {
    if (IItbsetio(1,(char *)"dut_f3_objlist",(char *)"dut_f3_9objlist",-3) != 
    0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,dut_f3_scroll) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 1609 "dutform.sc" */	/* host code */
    }
    return(E_DU_OK);
}
/*
** Name: dut_ff3_4objinfo
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      5-dec-1988 (alexc)
**          Creation.
*/
DUT_STATUS
dut_ff3_4objinfo(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_F3_OBJLIST *dut_f3_objlist_p;
  DUT_T3_OBJLIST *dut_t3_objlist_p;
  DUT_T3_OBJLIST *dut_t3_objlist_curr_p;
  DUT_T3_OBJLIST dut_t3_buffer;
  DUT_T3_OBJLIST *dut_t3_p;
    dut_f3_objlist_p	= (DUT_F3_OBJLIST *)dut_cb->dut_form_objlist;
    dut_t3_objlist_p	= (DUT_T3_OBJLIST *)dut_f3_objlist_p->dut_f3_9objlist;
/* # line 1650 "dutform.sc" */	/* getrow */
  {
    if (IItbsetio(2,(char *)"dut_f3_objlist",(char *)"dut_f3_9objlist",-2) != 
    0) {
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_t3_buffer.dut_t3_0objowner,
      (char *)"dut_t3_0objowner");
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_t3_buffer.dut_t3_1objname,
      (char *)"dut_t3_1objname");
      IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_t3_buffer.dut_t3_2objtype,
      (char *)"dut_t3_2objtype");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 1657 "dutform.sc" */	/* host code */
    dut_uu14_find_object(dut_cb,
		dut_errcb,
		&dut_t3_p,
		dut_t3_buffer.dut_t3_0objowner,
		dut_t3_buffer.dut_t3_1objname);
    if (dut_t3_p == NULL)
    {
	return(DUT_ERROR);
    }
    switch(dut_t3_p->dut_t3_0system[0])
    {
    case 'Y':
    case 'N':
	switch(dut_t3_p->dut_t3_0objtype[0])
	{
	case 'T':	/* Table		*/
	    STcopy(dut_t3_p->dut_t3_0objowner,
		dut_cb->dut_popup_objattr->dut_p8_3objowner);
	    STcopy(dut_t3_p->dut_t3_1objname,
		dut_cb->dut_popup_objattr->dut_p8_2objname);
	    dut_pp8_1objattr(dut_cb, dut_errcb);
	    break;
	case 'V':	/* View			*/
	/* this may be a distributed view or a local view that has being
	** registered.
	*/
	    STcopy(dut_t3_p->dut_t3_0objowner,
		dut_cb->dut_popup_objattr->dut_p8_3objowner);
	    STcopy(dut_t3_p->dut_t3_1objname,
		dut_cb->dut_popup_objattr->dut_p8_2objname);
	    STcopy(dut_t3_p->dut_t3_0objowner,
		dut_cb->dut_popup_viewattr->dut_p9_4view_owner);
	    STcopy(dut_t3_p->dut_t3_1objname,
		dut_cb->dut_popup_viewattr->dut_p9_3view_name);
	    dut_pp9_1viewattr(dut_cb, dut_errcb);
	    break;
	case 'I':	/* Index		*/
	    STcopy(dut_t3_p->dut_t3_0objowner,
		dut_cb->dut_popup_objattr->dut_p8_3objowner);
	    STcopy(dut_t3_p->dut_t3_1objname,
		dut_cb->dut_popup_objattr->dut_p8_2objname);
	    dut_pp8_1objattr(dut_cb, dut_errcb);
/*	    dut_ppx_indexattr(dut_cb, dut_errcb);*/
	    break;
	case 'P':	/* Partitioned		*/
/*	    dut_xxx_partattr(dut_cb, dut_errcb);*/
	    break;
	case 'R':	/* Replicate		*/
/*	    dut_xxx_replicateattr(dut_cb, dut_errcb);*/
	    break;
	default:
	    /* error occur */
	    break;
	}
    default:
	/* error occur */
	break;
    }
    return(E_DU_OK);
}
/*
** Name: dut_ff3_6remove_object
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**      5-dec-1988 (alexc)
**          Creation.
*/
DUT_STATUS
dut_ff3_6remove_object(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_F3_OBJLIST *dut_f3_objlist_p;
  DUT_T3_OBJLIST *dut_t3_objlist_p;
  DUT_T3_OBJLIST *dut_t3_objlist_curr_p;
  char dut_stmt_buffer[DB_LONGMSG];
  char dut_confirm[DDB_MAXNAME];
    i4			dut_t3_found;
    i4			dut_t3_count;
    dut_t3_found	= FALSE;
    dut_f3_objlist_p	= 
	(DUT_F3_OBJLIST *)dut_cb->dut_form_objlist;
    dut_t3_objlist_p	= 
	(DUT_T3_OBJLIST *)dut_cb->dut_form_objlist->dut_f3_9objlist;
    dut_t3_objlist_curr_p = 
	(DUT_T3_OBJLIST *)dut_cb->dut_form_objlist->dut_f3_9objlist_curr_p;
    dut_t3_count	= dut_f3_objlist_p->dut_f3_0obj_num_entry;
    while (!dut_t3_found && dut_t3_count--)
    {
	if (!STcompare(dut_t3_objlist_p->dut_t3_1objname,
			dut_t3_objlist_curr_p->dut_t3_1objname)
			&&
	    !STcompare(dut_t3_objlist_p->dut_t3_2objtype,
			dut_t3_objlist_curr_p->dut_t3_2objtype)
			&&
	    !STcompare(dut_t3_objlist_p->dut_t3_0objowner,
			dut_t3_objlist_curr_p->dut_t3_0objowner)
			&&
	    (dut_t3_objlist_p->dut_t3_0delete == FALSE)
	    )
	{
	    dut_t3_found = TRUE;
	    break;
	}
	else
	{
	    dut_t3_objlist_p++;
	}
    }
    if (dut_t3_found == TRUE)
    {
	dut_t3_objlist_curr_p->dut_t3_0select 
		= dut_t3_objlist_p->dut_t3_0select;
	dut_t3_objlist_curr_p->dut_t3_0objbase
		= dut_t3_objlist_p->dut_t3_0objbase;
	STcopy(dut_t3_objlist_p->dut_t3_0objtype,
		dut_t3_objlist_curr_p->dut_t3_0objtype);
	STcopy(dut_t3_objlist_p->dut_t3_0subtype,
		dut_t3_objlist_curr_p->dut_t3_0subtype);
	STcopy(dut_t3_objlist_p->dut_t3_0system, 
		dut_t3_objlist_curr_p->dut_t3_0system);
	STcopy(dut_t3_objlist_p->dut_t3_1objname, 
		dut_t3_objlist_curr_p->dut_t3_1objname);
	STcopy(dut_t3_objlist_p->dut_t3_2objtype, 
		dut_t3_objlist_curr_p->dut_t3_2objtype);
	STcopy(dut_t3_objlist_p->dut_t3_0objowner,
		dut_t3_objlist_curr_p->dut_t3_0objowner);
    }
    else
    {
	/* Added error message here */
	return(DUT_ERROR);
    }
    switch(dut_t3_objlist_curr_p->dut_t3_0system[0])
    {
    case 'Y':
	/* Does not allow removal of System object */
	break;
    case 'N':
	switch(dut_t3_objlist_curr_p->dut_t3_0objtype[0])
	{
	case 'V':	/* Registered View	*/
	case 'T':	/* Table		*/
	    if (dut_t3_objlist_curr_p->dut_t3_0objtype[0] == 'V' && 
		dut_t3_objlist_curr_p->dut_t3_0subtype[0] == 'N')
	    {
		/* Do not want to remove if it is a Star-level View */
	        dut_ee1_error(dut_errcb, W_DU1819_SV_RM_NONE_REG_OBJ, 0);
		break;
	    }
	    STcopy("U", dut_confirm);
	    while (STcompare(dut_confirm, "y") && STcompare(dut_confirm, "n"))
	    {
		STprintf(dut_cb->frs_message,
			DUT_M06_CONFIRM_REMOVE,
			dut_t3_objlist_curr_p->dut_t3_1objname);
/* # line 1834 "dutform.sc" */	/* prompt */
  {
    IIprmptio(0,dut_cb->frs_message,(short *)0,1,32,DDB_MAXNAME-1,dut_confirm);
  }
/* # line 1839 "dutform.sc" */	/* host code */
		CVlower(dut_confirm);
	    }
	    if (!STcompare(dut_confirm, "n") || !STcompare(dut_confirm, ""))
	    {
		return(E_DU_OK);
	    }
	    STprintf(dut_stmt_buffer,
			DUT_M07_REMOVING_DOBJ,
			dut_t3_objlist_curr_p->dut_t3_1objname);
/* # line 1849 "dutform.sc" */	/* message */
  {
    IImessage(dut_stmt_buffer);
  }
/* # line 1851 "dutform.sc" */	/* host code */
	    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
	    {
		dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
				dut_cb->dut_c1_ddbname);
	    }
	    STprintf(dut_stmt_buffer,
		"remove %s %s",
		( dut_t3_objlist_curr_p->dut_t3_0objtype[0] == 'V' ?
		"view" : (dut_t3_objlist_curr_p->dut_t3_0objtype[0] == 'T' ?
			"table" : ""
			)
		),
		dut_t3_objlist_curr_p->dut_t3_1objname);
/* # line 1866 "dutform.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1867 "dutform.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(dut_stmt_buffer);
    IIsyncup((char *)0,0);
  }
/* # line 1868 "dutform.sc" */	/* host code */
	    if (sqlca.sqlcode < 0)
	    {
		DUT_STATUS msg_no;
		switch(dut_t3_objlist_curr_p->dut_t3_0objtype[0])
		{
		case 'V':	/* Link or Register	*/
			msg_no = W_DU1814_SV_CANT_DROP_TABLE;
			break;
		case 'T':	/* Table		*/
			msg_no = W_DU1815_SV_CANT_DROP_VIEW;
			break;
		default:
			msg_no = W_DU1819_SV_RM_NONE_REG_OBJ;
			break;
		}
	        dut_ee1_error(dut_errcb, msg_no,
			2, 0, dut_t3_objlist_curr_p->dut_t3_1objname);
	    }
	    else
	    {
		dut_t3_objlist_curr_p->dut_t3_0delete = TRUE;
		dut_t3_objlist_p->dut_t3_0delete = TRUE;
	    }
/* # line 1892 "dutform.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1893 "dutform.sc" */	/* host code */
	    dut_uu18_chk_ldblist(dut_cb, dut_errcb);
	    dut_ff3_8fixlist(dut_cb, dut_errcb);
	    dut_ff3_3reload_obj(dut_cb, dut_errcb);
	    break;
	case 'I':	/* Index		*/
	    dut_ee1_error(dut_errcb, W_DU1819_SV_RM_NONE_REG_OBJ, 0);
	    break;
	case 'P':	/* Partitioned		*/
	case 'R':	/* Replicate		*/
	default:
	    /* Call error for object is not a register object */
	    /* Do not drop anything but registered tables */
	    dut_ee1_error(dut_errcb, W_DU1819_SV_RM_NONE_REG_OBJ, 0);
	    break;
	}
	break;
    default:
	/* error occur */
	dut_ee1_error(dut_errcb, W_DU1819_SV_RM_NONE_REG_OBJ, 0);
	break;
    }
    return(E_DU_OK);
}
/*
** Name: dut_ff3_7objects
**
** Description:
**  Load objects list into objects list array.
**
** History:
**      24-jul-1991 (fpang)
**          Created from dut_ff3_2load_obj().
**      10-12-94 (newca01) 
**          added new union to select to get star views for B48950
*/
DUT_STATUS
dut_ff3_7objects(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_F3_OBJLIST *dut_f3_objlist_p;
  DUT_T3_OBJLIST *dut_f3_9objlist_p;
  DUT_T3_OBJLIST *dut_f3_0objlist_curr_p;
  i4 dut_f3_0obj_count;
    i4		dut_f3_0obj_fill;
    dut_f3_0obj_count = 0;
    dut_f3_0obj_fill  = 0;
    dut_f3_objlist_p = (DUT_F3_OBJLIST *)dut_cb->dut_form_objlist;
    dut_f3_9objlist_p = (DUT_T3_OBJLIST *)dut_f3_objlist_p->dut_f3_9objlist;
    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
                      dut_cb->dut_c1_ddbname);
    }
/* # line 1956 "dutform.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiddb_objects, iitables where iiddb_objects.objec\
t_name=iitables.table_name and iiddb_objects.object_owner=iitables.tab\
le_owner");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_f3_0obj_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1963 "dutform.sc" */	/* host code */
    if (dut_f3_0obj_count > dut_cb->dut_form_objlist->dut_f3_0obj_size)
    {
	MEfree(dut_f3_9objlist_p);
	dut_f3_objlist_p->dut_f3_9objlist = NULL;
	dut_f3_9objlist_p = NULL;
    }
    if (dut_f3_9objlist_p == NULL)
    {
	dut_f3_9objlist_p = (DUT_T3_OBJLIST *)MEreqmem(0,
		(dut_f3_0obj_count + DUT_ME_EXTRA) * sizeof(DUT_T3_OBJLIST),
		TRUE, NULL);
	dut_f3_objlist_p->dut_f3_9objlist = dut_f3_9objlist_p;
	dut_cb->dut_form_objlist->dut_f3_0obj_size 
		= dut_f3_0obj_count + DUT_ME_EXTRA;
    }
    dut_f3_0objlist_curr_p = dut_f3_9objlist_p;
/* # line 1983 "dutform.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iiddb_objects.object_base, lowercase(trim(iiddb_objects.object\
_name)), lowercase(trim(iiddb_objects.object_owner)), lowercase(trim(i\
itables.table_type)), lowercase(trim(iitables.table_subtype)), lowerca\
se(trim(iiddb_objects.system_object)), ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"lowercase(trim(iiddb_ldbids.ldb_database)), lowercase(trim(iiddb_ldbi\
ds.ldb_node))from iiddb_objects, iiddb_tableinfo, iiddb_ldbids, iitabl\
es where(iiddb_objects.object_base=iiddb_tableinfo.object_base)and(iid\
db_objects.object_index=iiddb_tableinfo.");
    IIwritio(0,(short *)0,1,32,0,(char *)
"object_index)and(iitables.table_name=iiddb_objects.object_name)and(ii\
tables.table_owner=iiddb_objects.object_owner)and(iiddb_ldbids.ldb_id=\
iiddb_tableinfo.ldb_id)union select iiddb_objects.object_base, lowerca\
se(trim(iiddb_objects.object_name)), lowercase");
    IIwritio(0,(short *)0,1,32,0,(char *)
"(trim(iiddb_objects.object_owner)), lowercase(trim(iitables.table_typ\
e)), lowercase(trim(iitables.table_subtype)), lowercase(trim(iiddb_obj\
ects.system_object)), ' ', ' ' from iiddb_objects, iitables where(iita\
bles.table_name=iiddb_objects.object_name)and(");
    IIwritio(0,(short *)0,1,32,0,(char *)
"iitables.table_owner=iiddb_objects.object_owner)and(iiddb_objects.obj\
ect_type='V')order by 2, 3");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_f3_0objlist_curr_p->dut_t3_0objbase);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_0objlist_curr_p->dut_t3_1objname);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_0objlist_curr_p->dut_t3_0objowner);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_0objlist_curr_p->dut_t3_0objtype);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_0objlist_curr_p->dut_t3_0subtype);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_0objlist_curr_p->dut_t3_0system);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_0objlist_curr_p->dut_t3_0ldbname);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_f3_0objlist_curr_p->dut_t3_0ldbnode);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 2034 "dutform.sc" */	/* host code */
	CVupper(dut_f3_0objlist_curr_p->dut_t3_0system);
	CVupper(dut_f3_0objlist_curr_p->dut_t3_0objtype);
	CVupper(dut_f3_0objlist_curr_p->dut_t3_0subtype);
	if (dut_cb->dut_form_objlist->dut_f3_0obj_num_entry == 0)
	{
	    /* 0 indicates user requested info ( select ListObj )
	    ** as opposed to re-fetch of info because we detected
	    ** that new objects exist or cease to exist as a result
	    ** of a 'Remove' or 'Register'.
	    ** Give informational message if the former.
	    */
	    STprintf(dut_cb->frs_message,
		    DUT_M05_LOAD_DOBJ,
		    dut_f3_0objlist_curr_p->dut_t3_1objname);
/* # line 2051 "dutform.sc" */	/* message */
      {
        IImessage(dut_cb->frs_message);
      }
/* # line 2052 "dutform.sc" */	/* host code */
	}
	dut_f3_0objlist_curr_p->dut_t3_0delete = FALSE;
	dut_uu11_chk_criteria(dut_cb, dut_errcb, dut_f3_0objlist_curr_p);
	dut_f3_0obj_fill++;
	if (dut_f3_0obj_fill > dut_cb->dut_form_objlist->dut_f3_0obj_size)
	{
/* # line 2062 "dutform.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 2063 "dutform.sc" */	/* host code */
	}
	dut_f3_0objlist_curr_p++;
	if (sqlca.sqlcode < 0)
	{
	    dut_cb->dut_c8_status = !E_DU_OK;
	    dut_ee1_error(dut_errcb, W_DU1807_SV_CANT_GET_DDB_INFO, 2,
			0, dut_cb->dut_c1_ddbname);
/* # line 2071 "dutform.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 2072 "dutform.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
    dut_uu13_buble_sort(dut_cb->dut_form_objlist->dut_f3_9objlist,
			sizeof(DUT_T3_OBJLIST), DDB_MAXNAME,
			dut_f3_0obj_fill);
    if (dut_cb->dut_form_objlist->dut_f3_0obj_num_entry == 0)
    {
	/* See comment above regarding informational message.
	** This code here to place the cursor on the first entry
	** if we get here via a 'ListObj', and current otherwise.
	*/
        dut_f3_0objlist_curr_p = dut_f3_objlist_p->dut_f3_9objlist_curr_p;
        MEcopy(dut_f3_objlist_p->dut_f3_9objlist, sizeof(DUT_T3_OBJLIST),
	       dut_f3_0objlist_curr_p);
    }
    dut_cb->dut_form_objlist->dut_f3_0obj_num_entry = dut_f3_0obj_fill;
    return(E_DU_OK);
}
/*
** Name: dut_ff3_8fixlist
**
** Description:
**	Set current to point to next object after the one just deleted.
**
** History:
**      24-jul-1991 (fpang)
**          Created.
*/
DUT_STATUS
dut_ff3_8fixlist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
    DUT_T3_OBJLIST	*pp;
    DUT_F3_OBJLIST	*dut_f3_objlist_p;
    DUT_T3_OBJLIST	*curr_p;
    DUT_T3_OBJLIST	curr;
    i4		dut_f3_0obj_count;
    /* Reload objects */
    dut_ff3_7objects(dut_cb, dut_errcb);
    dut_f3_0obj_count = dut_cb->dut_form_objlist->dut_f3_0obj_num_entry;
    dut_f3_objlist_p = (DUT_F3_OBJLIST *)dut_cb->dut_form_objlist;
    pp = (DUT_T3_OBJLIST *)dut_f3_objlist_p->dut_f3_9objlist;
    curr_p = dut_cb->dut_form_objlist->dut_f3_9objlist_curr_p;
    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
                      dut_cb->dut_c1_ddbname);
    }
    /* Find new current */
    while (dut_f3_0obj_count--)
    {
	if (STcompare(curr_p->dut_t3_1objname, pp->dut_t3_1objname) <= 0)
	{
	    MEcopy(pp, sizeof(DUT_T3_OBJLIST), curr_p);
	    break;
	}
	pp++;
    }
    if (dut_f3_0obj_count == -1)
    {
	pp--;
        MEcopy(pp, sizeof(DUT_T3_OBJLIST), curr_p);
    }
    return(E_DU_OK);
}
