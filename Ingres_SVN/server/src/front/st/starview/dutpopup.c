# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
#include    <compat.h>
#include    <dbms.h>
#include    <er.h>
#include    <duerr.h>
#include    <me.h>
#include    <st.h>
#include    <nm.h>
# include "dut.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
#include    "duustrings.h"
/**
**
**  Name: DUTPOPUP.SC - Popup functions for StarView.
**
**  Description:
**	dut_pp2_1criteria - Popup for Distributed object selection
**			criteria.
**	dut_pp3_1register - ** Removed **
**	dut_pp4_1dblist - ** Removed **
**	dut_pp4_2load_dblist - ** Removed **
**	dut_pp5_1tablelist - ** Removed **
**	dut_pp5_2load_tablelist - ** Removed **
**	dut_pp6_1nodelist - Popup for Node listing of the current installation
**			or node listing all LDBs registered with a given
**			DDB.
**	dut_pp6_2load_nodelist - Function to load node information from
**			iistar_cdbs from CDB(list of CDB nodes).
**	dut_pp6_3load_nodelist - Function to load node information from
**			iiddb_ldbids(iidd_ddb_ldbids) the list of nodes
**			where local objects resides.
**	dut_pp7_1dbattr - Popup for displaying database attributes.
**	dut_pp7_2get_dbname - Get dbname from screen.
**	dut_pp7_3load_dbattr - Loads attributes from a given Database.
**	dut_pp8_1tableattr - Popup for displaying table attributes.
**	dut_pp9_1viewattr - Popup for displaying view attributes.
**	dut_pp10_1regattr - ** Removed **
**	dut_pp11_1perror - Popup used for debugging.
**	dut_pp12_1ldblist - Popup for listing of local databases for
**			select into the Register syntax popup.
**	dut_pp12_2load_ldblist - Function to load Local database names into
**			an array.
**	dut_pp13_1ownlist - Popup for list of owners of lcoal database(s).
**	dut_pp13_2load_ownlist - Function to load local object owner names
**			into an array.
**	dut_pp14_1browse - Popup for browsing nodes, databases and tables.
**	dut_pp14_1browse_node - Popup for browsing nodes, databases and tables.
**	dut_pp14_2load_node - Load node name into Popup.
**	dut_pp15_1ldb	- Browse local database name.
**	dut_pp16_1browse_table - browse local database name.
**	dut_pp16_2register - Register the table object.
**	dut_pp16_3browse_load_table - load information into the browse table.
**
**
**  History:
**      02-dec-1988 (alexc)
**          Creation.
**	08-mar-1989 (alexc)
**	    Alpha test version.
**	03-may-1989 (alexc)
**	    Added Browsing functions: dut_pp14...() to dut_pp16_2...().
**	11-jul-1989 (alexc)
**	    Removed functions: dut_pp3_1register(), dut_pp4_1dblist(),
**		dut_pp4_2load_dblist(), dut_pp5_1tablelist(),
**		dut_pp5_2load_tablelist(), dut_pp10_1regattr().
**		These functions are no longer used.
**	11-Aug-1989 (alexc)
**	    Added function dut_pp16_3browse_load_table() to reload
**		table field from database after SQL or TABLES.
**	25-Jul-1991 (fpang)
**	    1. dut_pp7_1dbattr() modified to recognize forced consistent
**	       databases.
**	    2. Return E_DU_OK in dut_pp16_3browse_load_table() if 
**	       successful.
**      12-sep-1991 (fpang)
**          1. Changed Star*View to StarView. -B39562.
**          2. Mapped FIND, TOP and BOTTOM to standard INGRES FRSKEYS. -B39661
**	26-Aug-1993 (fredv)
**		Included <me.h>.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
** Name: dut_pp2_1criteria	- Distributed Object Selection Popup
**
** Description:
**	Uses FRS, and has the following options for the form DUT_P2_SELECT.
**	    'Select'	Sets the selection criteria in (PTR *)dut_f3_objlist_p.
**	    'NodeHelp'	List all nodes from iiddb_ldbids(iidd_ddb_ldbids).
**	    'LDBHelp'	List all LDBs that has objects register in current DDB.
**	    'OwnerHelp'	List all owners of DDB objects.
**	    'Help'	Help on form.
**	    'End'	Exit.
**
** Inputs:
**	dut_cb
**	dut_errcb
**
** Outputs:
**	dut_f3_objlist_p
**
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      4-jan-1989 (alexc)
**          Creation.
*/
dut_pp2_1criteria(dut_cb, dut_errcb, dut_f3_objlist_p)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
  DUT_F3_OBJLIST *dut_f3_objlist_p;
{
    /* If connected to CDB, use a different cursor */
/* # line 135 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p2_select",(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 139 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p2_select") != 0) {
        IIputfldio((char *)"dut_p2_1nodename",(short *)0,1,32,0,
        dut_f3_objlist_p->dut_f3_1nodename);
        IIputfldio((char *)"dut_p2_2dbname",(short *)0,1,32,0,
        dut_f3_objlist_p->dut_f3_2dbname);
        IIputfldio((char *)"dut_p2_3owner",(short *)0,1,32,0,
        dut_f3_objlist_p->dut_f3_3owner);
        IIputfldio((char *)"dut_p2_5show_view",(short *)0,1,32,0,
        dut_f3_objlist_p->dut_f3_5show_view);
        IIputfldio((char *)"dut_p2_6show_table",(short *)0,1,32,0,
        dut_f3_objlist_p->dut_f3_6show_table);
        IIputfldio((char *)"dut_p2_7show_system",(short *)0,1,32,0,
        dut_f3_objlist_p->dut_f3_7show_system);
      } /* IIfsetio */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 152 "dutpopup.sc" */	/* getform */
        {
          if (IIfsetio((char *)"dut_p2_select") != 0) {
            IIgetfldio(&dut_cb->dut_null_1,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_1nodename,(char *)"dut_p2_1nodena\
me");
            IIgetfldio(&dut_cb->dut_null_2,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_2dbname,(char *)"dut_p2_2dbname");
            IIgetfldio(&dut_cb->dut_null_3,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_3owner,(char *)"dut_p2_3owner");
            IIgetfldio(&dut_cb->dut_null_5,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_5show_view,(char *)"dut_p2_5show_\
view");
            IIgetfldio(&dut_cb->dut_null_6,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_6show_table,(char *)
"dut_p2_6show_table");
            IIgetfldio(&dut_cb->dut_null_7,1,32,DDB_MAXNAME-1,
            dut_f3_objlist_p->dut_f3_7show_system,(char *)
"dut_p2_7show_system");
          } /* IIfsetio */
        }
/* # line 167 "dutpopup.sc" */	/* host code */
	if (dut_cb->dut_null_1 == -1)
	{
	    dut_f3_objlist_p->dut_f3_1nodename[0] = EOS;
	}
	if (dut_cb->dut_null_2 == -1)
	{
	    dut_f3_objlist_p->dut_f3_2dbname[0] = EOS;
	}
	if (dut_cb->dut_null_3 == -1)
	{
	    dut_f3_objlist_p->dut_f3_3owner[0] = EOS;
	}
	if (dut_cb->dut_null_5 == -1)
	{
	    dut_f3_objlist_p->dut_f3_5show_view[0] = EOS;
	}
	if (dut_cb->dut_null_6 == -1)
	{
	    dut_f3_objlist_p->dut_f3_6show_table[0] = EOS;
	}
	if (dut_cb->dut_null_7 == -1)
	{
	    dut_f3_objlist_p->dut_f3_7show_system[0] = EOS;
	}
	if ((dut_f3_objlist_p->dut_f3_1nodename[0] == EOS)	||
	    (dut_f3_objlist_p->dut_f3_2dbname[0]   == EOS)	||
	    (dut_f3_objlist_p->dut_f3_3owner[0]   == EOS)	||
	    (dut_f3_objlist_p->dut_f3_5show_view[0] == EOS)	||
	    (dut_f3_objlist_p->dut_f3_6show_table[0] == EOS)	||
	    (dut_f3_objlist_p->dut_f3_7show_system[0] == EOS))
	{
	    dut_ee1_error(dut_errcb, W_DU1817_SV_ERROR_IN_CRITERIA, 0);
	}
	else
	{
	    dut_cb->dut_c8_status = DUT_S_NEW_OBJ_SELECT;
/* # line 203 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 204 "dutpopup.sc" */	/* host code */
	}
      } else if (IIretval() == 2) {
	dut_cb->dut_c9_status = DUT_P_POPUP_2;
	dut_pp6_1nodelist(dut_cb, dut_errcb);
	dut_cb->dut_c9_status = DUT_F_FORM_3;
	if (dut_cb->dut_c0_buffer[0] != EOS)
	    STcopy(dut_cb->dut_c0_buffer, dut_f3_objlist_p->dut_f3_1nodename);
/* # line 214 "dutpopup.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_p2_select") != 0) {
            IIputfldio((char *)"dut_p2_1nodename",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_1nodename);
          } /* IIfsetio */
        }
      } else if (IIretval() == 3) {
/* # line 222 "dutpopup.sc" */	/* host code */
	/* This will help user change selection criteria for Node Name
	   and Database Name.  This is the only way dut_f3_1nodename,
	   and dut_f3_2dbname can be changed. */
	dut_cb->dut_c9_status = DUT_P_POPUP_2;
	dut_pp12_1ldblist(dut_cb, dut_errcb);
	dut_cb->dut_c9_status = DUT_F_FORM_3;
	if (dut_cb->dut_c0_buffer[0] != EOS)
	    STcopy(dut_cb->dut_c0_buffer, dut_f3_objlist_p->dut_f3_2dbname);
/* # line 230 "dutpopup.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_p2_select") != 0) {
            IIputfldio((char *)"dut_p2_2dbname",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_2dbname);
          } /* IIfsetio */
        }
      } else if (IIretval() == 4) {
/* # line 238 "dutpopup.sc" */	/* host code */
	dut_cb->dut_c9_status = DUT_P_POPUP_2;
	dut_pp13_1ownlist(dut_cb, dut_errcb);
	dut_cb->dut_c9_status = DUT_F_FORM_3;
	if (dut_cb->dut_c0_buffer[0] != EOS)
	    STcopy(dut_cb->dut_c0_buffer, dut_f3_objlist_p->dut_f3_3owner);
/* # line 243 "dutpopup.sc" */	/* putform */
        {
          if (IIfsetio((char *)"dut_p2_select") != 0) {
            IIputfldio((char *)"dut_p2_3owner",(short *)0,1,32,0,
            dut_f3_objlist_p->dut_f3_3owner);
          } /* IIfsetio */
        }
      } else if (IIretval() == 5) {
/* # line 251 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp2_1name);
        }
      } else if (IIretval() == 6) {
/* # line 260 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
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
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"NodeHelp",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"LDBHelp",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"OwnerHelp",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 262 "dutpopup.sc" */	/* host code */
}
/*
** Name: dut_pp6_1nodelist
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      2-dec-1988 (alexc)
**          Creation.
*/
dut_pp6_1nodelist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  i4 n_rows;
    DUT_P6_NODELIST *dut_p6_nodelist_p;
/* # line 297 "dutpopup.sc" */	/* host code */
    dut_p6_nodelist_p = dut_cb->dut_popup_nodelist;
/* # line 299 "dutpopup.sc" */	/* inittable */
  {
    if (IItbinit((char *)"dut_p6_nodelist",(char *)"dut_p6_1nodelist",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 300 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p6_nodelist",(char *)"f") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
/* # line 304 "dutpopup.sc" */	/* host code */
	dut_cb->dut_c0_buffer[0] = (char)EOS;
	if (dut_cb->dut_c9_status == DUT_F_FORM_1)
	{
/* # line 307 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p6_nodelist") != 0) {
        IIputfldio((char *)"dut_p6_0name",(short *)0,1,32,0,(char *)
"STAR installation.");
      } /* IIfsetio */
    }
/* # line 311 "dutpopup.sc" */	/* host code */
	    dut_pp6_2load_nodelist(dut_cb, dut_errcb);
	}
	else
	{
	    dut_cb->dut_c8_status = E_DU_OK;
/* # line 316 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p6_nodelist") != 0) {
        IIputfldio((char *)"dut_p6_0name",(short *)0,1,32,0,(char *)
"Distributed Database.");
      } /* IIfsetio */
    }
/* # line 320 "dutpopup.sc" */	/* host code */
	    dut_pp6_3load_nodelist(dut_cb, dut_errcb);
	    if (dut_cb->dut_c8_status == DUT_ERROR)
	    {
/* # line 323 "dutpopup.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE2;
    }
/* # line 324 "dutpopup.sc" */	/* host code */
	    }
	}
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 330 "dutpopup.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"dut_p6_nodelist",(char *)0, (char *)0) != 0
          ) {
            IIiqfsio((short *)0,1,30,4,&n_rows,31,(char *)
"dut_p6_1nodelist",0);
          } /* IIiqset */
        }
/* # line 334 "dutpopup.sc" */	/* host code */
	if (n_rows == 0)
	{
	    dut_cb->dut_c0_buffer2[0] = EOS;
/* # line 337 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
/* # line 338 "dutpopup.sc" */	/* host code */
	}
/* # line 339 "dutpopup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_p6_nodelist",(char *)
"dut_p6_1nodelist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer,
            (char *)"dut_t6_1nodename");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 343 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      } else if (IIretval() == 2) {
/* # line 348 "dutpopup.sc" */	/* host code */
	dut_uu7_table_top(NULL);
      } else if (IIretval() == 3) {
	dut_uu8_table_bottom(NULL);
      } else if (IIretval() == 4) {
	dut_uu9_table_find(dut_cb, dut_errcb, DUT_M08_NODENAME, NULL, NULL);
      } else if (IIretval() == 5) {
/* # line 363 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp6_1name);
        }
      } else if (IIretval() == 6) {
/* # line 372 "dutpopup.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact((char *)"Top",(char *)0,2,2,2) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact((char *)"Find",(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE2;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 374 "dutpopup.sc" */	/* host code */
}
/*
** Name: dut_pp6_2load_nodelist
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      2-dec-1988 (alexc)
**          Creation.
*/
dut_pp6_2load_nodelist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P6_NODELIST *dut_p6_nodelist_p;
  DUT_T6_NODELIST *dut_t6_nodelist_p;
  DUT_T6_NODELIST *dut_t6_nodelist_curr_p;
  i4 dut_p6_0count;
    i4			dut_p6_0fill;
/* # line 412 "dutpopup.sc" */	/* host code */
    dut_p6_0count	= 0;
    dut_p6_0fill	= 0;
    dut_p6_nodelist_p = (DUT_P6_NODELIST *)dut_cb->dut_popup_nodelist;
    dut_t6_nodelist_p = 
	    (DUT_T6_NODELIST *)dut_cb->dut_popup_nodelist->dut_p6_1nodelist;
    dut_cb->dut_on_error_exit = FALSE;
    dut_cb->dut_on_error_noop = FALSE;
    STcopy(dut_cb->dut_c2_nodename, dut_cb->dut_c0_iidbdbnode);
    if (dut_uu2_dbop(DUT_IIDBDBOPEN, dut_cb, dut_errcb) 
	!= E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU180A_SV_CANT_OPEN_IIDBDB, 0);
	return(DUT_ERROR);
    }
/* # line 429 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct count(*)from iistar_cdbs");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_p6_0count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 433 "dutpopup.sc" */	/* host code */
    if (dut_p6_0count > dut_p6_nodelist_p->dut_p6_0size)
    {
	MEfree(dut_p6_nodelist_p->dut_p6_1nodelist);
	dut_p6_nodelist_p->dut_p6_1nodelist = NULL;
	dut_t6_nodelist_p = NULL;
    }
    dut_cb->dut_on_error_exit = TRUE;
    if (dut_t6_nodelist_p == NULL)
    {
	dut_t6_nodelist_p = (DUT_T6_NODELIST *)MEreqmem(0,
		(dut_p6_0count + DUT_ME_EXTRA) * sizeof(DUT_T6_NODELIST),
		TRUE, NULL);
	dut_p6_nodelist_p->dut_p6_1nodelist
		= (DUT_T6_NODELIST *)dut_t6_nodelist_p;
	dut_p6_nodelist_p->dut_p6_0size = (dut_p6_0count + DUT_ME_EXTRA);
    }
    dut_t6_nodelist_curr_p = (DUT_T6_NODELIST *)dut_t6_nodelist_p;
    dut_cb->dut_on_error_exit = TRUE;
/* # line 456 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct trim(cdb_node)from iistar_cdbs order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_t6_nodelist_p->dut_t6_1nodename);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 463 "dutpopup.sc" */	/* loadtable */
      {
        if (IItbact((char *)"dut_p6_nodelist",(char *)"dut_p6_1nodelis\
t",1) != 0) {
          IItcoputio((char *)"dut_t6_1nodename",(short *)0,1,32,0,
          dut_t6_nodelist_p->dut_t6_1nodename);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 468 "dutpopup.sc" */	/* host code */
	dut_p6_0fill++;
	dut_t6_nodelist_p++;
	if (sqlca.sqlcode < 0)
	{
	    dut_cb->dut_c8_status = DUT_ERROR;
/* # line 473 "dutpopup.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
        if (sqlca.sqlcode < 0) 
          dut_ee5_none_starview();
      }
/* # line 474 "dutpopup.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
    dut_cb->dut_on_error_exit = FALSE;
    dut_p6_nodelist_p->dut_p6_0num_entry = dut_p6_0fill;
}
/*
** Name: dut_pp6_3load_nodelist
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      2-dec-1988 (alexc)
**          Creation.
*/
dut_pp6_3load_nodelist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P6_NODELIST *dut_p6_nodelist_p;
  DUT_T6_NODELIST *dut_t6_nodelist_p;
  DUT_T6_NODELIST *dut_t6_nodelist_curr_p;
  i4 dut_p6_0count;
    i4			dut_p6_0fill;
/* # line 517 "dutpopup.sc" */	/* host code */
    dut_p6_0count	= 0;
    dut_p6_0fill	= 0;
    dut_p6_nodelist_p = (DUT_P6_NODELIST *)dut_cb->dut_popup_nodelist;
    dut_t6_nodelist_p =
	    (DUT_T6_NODELIST *)dut_cb->dut_popup_nodelist->dut_p6_1nodelist;
    if (dut_uu2_dbop(DUT_DIRECT_CONNECT_CDB, dut_cb, dut_errcb) 
	!= E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT, 2,
		0, DUT_1CDB_LONG);
	return(DUT_ERROR);
    }
/* # line 531 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct count(*)from iidd_ddb_ldbids");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_p6_0count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 535 "dutpopup.sc" */	/* host code */
    if (dut_p6_0count > dut_p6_nodelist_p->dut_p6_0size)
    {
	MEfree(dut_p6_nodelist_p->dut_p6_1nodelist);
	dut_p6_nodelist_p->dut_p6_1nodelist = NULL;
	dut_t6_nodelist_p = NULL;
    }
    if (dut_t6_nodelist_p == NULL)
    {
	dut_t6_nodelist_p = (DUT_T6_NODELIST *)MEreqmem(0,
		(dut_p6_0count + DUT_ME_EXTRA) * sizeof(DUT_T6_NODELIST),
		TRUE, NULL);
	dut_p6_nodelist_p->dut_p6_1nodelist =
		(DUT_T6_NODELIST *)dut_t6_nodelist_p;
	dut_p6_nodelist_p->dut_p6_0size = (dut_p6_0count + DUT_ME_EXTRA);
    }
    dut_t6_nodelist_curr_p = (DUT_T6_NODELIST *)dut_t6_nodelist_p;
    if (dut_cb->dut_c9_status == DUT_P_POPUP_2)
    {
/* # line 556 "dutpopup.sc" */	/* loadtable */
  {
    if (IItbact((char *)"dut_p6_nodelist",(char *)"dut_p6_1nodelist",1) != 0) 
    {
      IItcoputio((char *)"dut_t6_1nodename",(short *)0,1,32,0,(char *)"\
*");
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 560 "dutpopup.sc" */	/* host code */
    }
    dut_cb->dut_on_error_exit = TRUE;
/* # line 565 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct trim(ldb_node)from iidd_ddb_ldbids order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_t6_nodelist_p->dut_t6_1nodename);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 572 "dutpopup.sc" */	/* loadtable */
      {
        if (IItbact((char *)"dut_p6_nodelist",(char *)"dut_p6_1nodelis\
t",1) != 0) {
          IItcoputio((char *)"dut_t6_1nodename",(short *)0,1,32,0,
          dut_t6_nodelist_p->dut_t6_1nodename);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 577 "dutpopup.sc" */	/* host code */
	dut_p6_0fill++;
	dut_t6_nodelist_p++;
	if (sqlca.sqlcode < 0)
	{
	    dut_cb->dut_c8_status = DUT_ERROR;
/* # line 582 "dutpopup.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
        if (sqlca.sqlcode < 0) 
          dut_ee5_none_starview();
      }
/* # line 583 "dutpopup.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
    dut_cb->dut_on_error_exit = FALSE;
    dut_p6_nodelist_p->dut_p6_0num_entry = dut_p6_0fill;
}
/*
** Name: dut_pp7_1dbattr
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      11-jan-1989 (alexc)
**          Creation.
**	25-jul-1991 (fpang)
**	    Recognize forced consistent databases.
**      14-Feb-2008 (horda03) Bug 119914
**          Disconnect the IIDBDB connection once details
**          displayed. Leaving the connection results in ISQL
**          connecting to teh IIDBDB and not the Star DB.
*/
dut_pp7_1dbattr(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P7_DBATTR *dut_p7_dbattr_p;
  i4 dut_count;
  i4 open_iidbdb = FALSE;
    dut_p7_dbattr_p = dut_cb->dut_popup_dbattr;
    if (dut_p7_dbattr_p->dut_p7_0nodename[0] == EOS)
    {
	STcopy(dut_cb->dut_c0_iidbdbnode, 
    		dut_p7_dbattr_p->dut_p7_0nodename);
    }
    if (dut_p7_dbattr_p->dut_p7_1dbname[0] == EOS)
    {
	dut_pp7_2get_dbname(dut_cb, dut_errcb);
    }
    if (!STcompare(dut_p7_dbattr_p->dut_p7_0nodename, 
		    dut_cb->dut_c0_iidbdbnode))
    {
	if (dut_uu2_dbop(DUT_IIDBDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
	{
	    dut_ee1_error(dut_errcb, E_DU5111_SV_CANT_OPEN_IIDBDB, 0);
	    return(DUT_ERROR);
	}
        open_iidbdb = TRUE;
    }
    else
    {
	/* Direct connect to remote node for DBattr */
	STcopy(dut_p7_dbattr_p->dut_p7_0nodename, dut_cb->dut_c4_ldbnode);
	STcopy("iidbdb", dut_cb->dut_c3_ldbname);
	if (dut_uu2_dbop(DUT_DIRECT_CONNECT, dut_cb, dut_errcb) != E_DU_OK)
	{
	    dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT, 2, 0, 
			  dut_cb->dut_c3_ldbname);
	    return(DUT_ERROR);
	}
    }
/* # line 668 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p7_dbattr",(char *)"f") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
/* # line 673 "dutpopup.sc" */	/* select */
    {
      IIsqInit(&sqlca);
      IIsqMods(1);
      IIwritio(0,(short *)0,1,32,0,(char *)
"select count(name)from iidatabase where iidatabase.name=");
      IIputdomio((short *)0,1,32,0,dut_p7_dbattr_p->dut_p7_1dbname);
      IIretinit((char *)0,0);
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&dut_count);
      } /* IInextget */
      IIsqFlush((char *)0,0);
      if (sqlca.sqlcode < 0) 
        dut_ee5_none_starview();
    }
/* # line 682 "dutpopup.sc" */	/* host code */
	if (dut_count == 0)
	{
	    /* Local database does not exist */
	    dut_ee1_error(dut_errcb, W_DU1818_SV_DB_DOES_NOT_EXIST, 0);
/* # line 686 "dutpopup.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE3;
    }
/* # line 687 "dutpopup.sc" */	/* host code */
	}
/* # line 689 "dutpopup.sc" */	/* select */
    {
      IIsqInit(&sqlca);
      IIsqMods(1);
      IIwritio(0,(short *)0,1,32,0,(char *)
"select own, dbdev, ckpdev, jnldev, sortdev, access, dbservice from ii\
database where iidatabase.name=");
      IIputdomio((short *)0,1,32,0,dut_p7_dbattr_p->dut_p7_1dbname);
      IIretinit((char *)0,0);
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_p7_dbattr_p->dut_p7_2own);
        IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
        dut_p7_dbattr_p->dut_p7_3dbdev);
        IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
        dut_p7_dbattr_p->dut_p7_4ckpdev);
        IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
        dut_p7_dbattr_p->dut_p7_5jnldev);
        IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
        dut_p7_dbattr_p->dut_p7_6sortdev);
        IIgetdomio((short *)0,1,30,4,&dut_p7_dbattr_p->dut_p7_0access);
        IIgetdomio((short *)0,1,30,4,&dut_p7_dbattr_p->dut_p7_0dbservice);
      } /* IInextget */
      IIsqFlush((char *)0,0);
      if (sqlca.sqlcode < 0) 
        dut_ee5_none_starview();
    }
/* # line 710 "dutpopup.sc" */	/* host code */
	if (sqlca.sqlcode < 0)
	{
	    /* Database does not exist */
	    dut_ee1_error(dut_errcb, W_DU1818_SV_DB_DOES_NOT_EXIST, 0);
	    return(DUT_ERROR);
	}
	STprintf(dut_p7_dbattr_p->dut_p7_7access,
		DUT_M09_UNKNOWN_ACCESS,
		dut_p7_dbattr_p->dut_p7_0access);
	if (dut_p7_dbattr_p->dut_p7_0access & DUT_GLOBAL)
	{
	}
	if (dut_p7_dbattr_p->dut_p7_0access & DUT_RES_DDB)
	{
	    /* Never being use */
	    STcopy(DUT_M10_DB_IS_DDB,
			dut_p7_dbattr_p->dut_p7_7access);
	}
	if (dut_p7_dbattr_p->dut_p7_0access & DUT_DESTROYDB)
	{
	    STcopy(DUT_M11_DB_IS_DESTROY,
			dut_p7_dbattr_p->dut_p7_7access);
	}
	if (dut_p7_dbattr_p->dut_p7_0access & DUT_CREATEDB)
	{
	    STcopy(DUT_M12_DB_IS_CREATE,
			dut_p7_dbattr_p->dut_p7_7access);
	}
	if (dut_p7_dbattr_p->dut_p7_0access & DUT_OPERATIVE)
	{
	    STcopy(DUT_M13_DB_IS_OPERATE,
			dut_p7_dbattr_p->dut_p7_7access);
	}
	if (dut_p7_dbattr_p->dut_p7_0access & DUT_CONVERTING)
	{
	    STcopy(DUT_M14_DB_IS_CONVERTING,
			dut_p7_dbattr_p->dut_p7_7access);
	}
	switch (dut_p7_dbattr_p->dut_p7_0dbservice)
	{
	case DUT_LDB_TYPE:
	    /* Local database */
	    STcopy(DUT_M15_LDB,
			dut_p7_dbattr_p->dut_p7_8dbservice);
	    break;
	case DUT_DDB_TYPE:
	    /* Distributed database */
	    STcopy(DUT_M16_DDB,
			dut_p7_dbattr_p->dut_p7_8dbservice);
	    break;
	case DUT_CDB_TYPE:
	    /* Coordinator database of a Distributed database */
	    STcopy(DUT_M17_CDB,
			dut_p7_dbattr_p->dut_p7_8dbservice);
	    break;
	case DUT_RMS_TYPE:
	    /* RMS database */
	    STcopy(DUT_M54_RMS,
			dut_p7_dbattr_p->dut_p7_8dbservice);
	    break;
	case DUT_FRCD_CNST_TYPE:
	    /* Forced consistent database */
	    STcopy(DUT_M55_FRCD_CNST,
			dut_p7_dbattr_p->dut_p7_8dbservice);
	    break;
	default:
	    STprintf(dut_p7_dbattr_p->dut_p7_8dbservice,
			DUT_M18_UNKNOWN_DBSERV,
			dut_p7_dbattr_p->dut_p7_0dbservice);
	    break;
	}
/* # line 783 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p7_dbattr") != 0) {
        IIputfldio((char *)"dut_p7_0nodename",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_0nodename);
        IIputfldio((char *)"dut_p7_1dbname",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_1dbname);
        IIputfldio((char *)"dut_p7_2own",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_2own);
        IIputfldio((char *)"dut_p7_3dbdev",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_3dbdev);
        IIputfldio((char *)"dut_p7_4ckpdev",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_4ckpdev);
        IIputfldio((char *)"dut_p7_5jnldev",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_5jnldev);
        IIputfldio((char *)"dut_p7_6sortdev",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_6sortdev);
        IIputfldio((char *)"dut_p7_7access",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_7access);
        IIputfldio((char *)"dut_p7_8dbservice",(short *)0,1,32,0,
        dut_p7_dbattr_p->dut_p7_8dbservice);
      } /* IIfsetio */
    }
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 799 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
      } else {
        if (1) goto IIfdE3;
      } /* IIretval */
    } /* IIrunform */
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    goto IIfdE3;
IImuI3:
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 802 "dutpopup.sc" */	/* host code */
    if (open_iidbdb)
    {
        if (dut_uu2_dbop(DUT_IIDBDBCLOSE, dut_cb, dut_errcb) != E_DU_OK)
        {
            return(DUT_ERROR);
        }
    }
    else if (dut_uu2_dbop(DUT_DIRECT_DISCONNECT, dut_cb, dut_errcb) != E_DU_OK)
    {
         return(DUT_ERROR);
    }
}
/*
** Name: dut_pp7_2get_dbname
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      11-jan-1989 (alexc)
**          Creation.
*/
dut_pp7_2get_dbname(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P7_DBATTR *dut_p7_dbattr_p;
    dut_p7_dbattr_p = dut_cb->dut_popup_dbattr;
/* # line 849 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p7_1dbname",(char *)"f") == 0) goto IIfdE4;
    goto IImuI4;
IIfdI4:;
    if (IIfsetio((char *)0) == 0) goto IIfdE4;
/* # line 853 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p7_1dbname") != 0) {
        IIputfldio((char *)"dut_p7_0nodename",(short *)0,1,32,0,
        dut_cb->dut_c6_cdbnode);
        IIputfldio((char *)"dut_p7_1dbname",(short *)0,1,32,0,
        dut_cb->dut_c5_cdbname);
      } /* IIfsetio */
    }
IIfdB4:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 862 "dutpopup.sc" */	/* getform */
        {
          if (IIfsetio((char *)"dut_p7_1dbname") != 0) {
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_p7_dbattr_p->dut_p7_0nodename,(char *)"dut_p7_0nodenam\
e");
            IIgetfldio((short *)0,1,32,DDB_MAXNAME-1,
            dut_p7_dbattr_p->dut_p7_1dbname,(char *)"dut_p7_1dbname");
          } /* IIfsetio */
        }
/* # line 867 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE4;
        }
      } else if (IIretval() == 2) {
/* # line 872 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp7_2name);
        }
      } else if (IIretval() == 3) {
/* # line 881 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE4;
        }
      } else {
        if (1) goto IIfdE4;
      } /* IIretval */
    } /* IIrunform */
IIfdF4:
    if (IIchkfrm() == 0) goto IIfdB4;
    goto IIfdE4;
IImuI4:
    if (IIinitmu() == 0) goto IIfdE4;
    if (IInmuact((char *)"Go",(char *)0,2,2,1) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE4;
    if (IInmuact((char *)"Help",(char *)0,2,2,2) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,2) == 0) goto IIfdE4;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE4;
    if (IIendmu() == 0) goto IIfdE4;
    goto IIfdI4;
IIfdE4:;
    IIendfrm();
  }
/* # line 883 "dutpopup.sc" */	/* host code */
}
/*
** Name: dut_pp7_3load_dbattr
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      11-jan-1989 (alexc)
**          Creation.
*/
dut_pp7_3load_dbattr(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P7_DBATTR *dut_p7_dbattr_p;
    dut_p7_dbattr_p = dut_cb->dut_popup_dbattr;
    if (dut_p7_dbattr_p->dut_p7_1dbname[0] == EOS)
    {
	return(DUT_ERROR);
    }
    else
    {
	STcopy(dut_cb->dut_c2_nodename, dut_cb->dut_c0_iidbdbnode);
	if (dut_uu2_dbop(DUT_IIDBDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
        {
	    dut_ee1_error(dut_errcb, E_DU5111_SV_CANT_OPEN_IIDBDB, 0);
	    return(DUT_ERROR);
	}
/* # line 930 "dutpopup.sc" */	/* repeat select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"dutpopup1",7461,12539);
      IIputdomio((short *)0,1,32,0,dut_p7_dbattr_p->dut_p7_1dbname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"dutpopup1",7461,12539);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select own, dbdev, ckpdev, jnldev, sortdev, access, dbservice from ii\
dbdb where iidatabase.name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,dut_p7_dbattr_p->dut_p7_1dbname);
        IIexDefine(0,(char *)"dutpopup1",7461,12539);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_p7_dbattr_p->dut_p7_2own);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_p7_dbattr_p->dut_p7_3dbdev);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_p7_dbattr_p->dut_p7_4ckpdev);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_p7_dbattr_p->dut_p7_5jnldev);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p7_dbattr_p->dut_p7_6sortdev);
      IIgetdomio((short *)0,1,30,4,&dut_p7_dbattr_p->dut_p7_0access);
      IIgetdomio((short *)0,1,30,4,&dut_p7_dbattr_p->dut_p7_0dbservice);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 951 "dutpopup.sc" */	/* host code */
	switch (dut_p7_dbattr_p->dut_p7_0access)
	{
	}
	switch (dut_p7_dbattr_p->dut_p7_0dbservice)
	{
	case 0:
	    STcopy(DUT_M19_LDB_OR_CDB,
		dut_p7_dbattr_p->dut_p7_8dbservice);
	    break;
	case 1:
	    STcopy(DUT_M16_DDB,
		dut_p7_dbattr_p->dut_p7_8dbservice);
	    break;
	default:
	    STcopy(DUT_M20_UNKNOWN,
		dut_p7_dbattr_p->dut_p7_8dbservice);
	    break;
	}
/* # line 971 "dutpopup.sc" */	/* putform */
  {
    if (IIfsetio((char *)"dut_p7_dbattr") != 0) {
      IIputfldio((char *)"dut_p7_0nodename",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_0nodename);
      IIputfldio((char *)"dut_p7_1dbname",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_1dbname);
      IIputfldio((char *)"dut_p7_2own",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_2own);
      IIputfldio((char *)"dut_p7_3dbdev",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_3dbdev);
      IIputfldio((char *)"dut_p7_4ckpdev",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_4ckpdev);
      IIputfldio((char *)"dut_p7_5jnldev",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_5jnldev);
      IIputfldio((char *)"dut_p7_6sortdev",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_6sortdev);
      IIputfldio((char *)"dut_p7_7access",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_7access);
      IIputfldio((char *)"dut_p7_8dbservice",(short *)0,1,32,0,
      dut_p7_dbattr_p->dut_p7_8dbservice);
    } /* IIfsetio */
  }
/* # line 983 "dutpopup.sc" */	/* host code */
    }
}
/*
** Name: dut_pp8_1objattr
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      11-jan-1989 (alexc)
**          Creation.
*/
dut_pp8_1objattr(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_T3_OBJLIST *dut_t3_objlist_p;
  DUT_P8_OBJATTR *dut_p8_objattr_p;
    dut_t3_objlist_p	= dut_cb->dut_form_objlist->dut_f3_9objlist_curr_p;
    dut_p8_objattr_p	= dut_cb->dut_popup_objattr;
    /* Get Table information */
    if (dut_uu2_dbop(DUT_DIRECT_CONNECT_CDB, dut_cb, dut_errcb) 
	!= E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT, 2,
		0, DUT_1CDB_LONG);
	return(DUT_ERROR);
    }
/* # line 1031 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidd_tables.create_date, iidd_tables.alter_date, iidd_tables.t\
able_type, iidd_tables.table_subtype, iidd_tables.table_version, iidd_\
tables.system_use from iidd_tables where iidd_tables.table_name=");
    IIputdomio((short *)0,1,32,0,dut_p8_objattr_p->dut_p8_2objname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and iidd_tables.table_owner=");
    IIputdomio((short *)0,1,32,0,dut_p8_objattr_p->dut_p8_3objowner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_4create_date);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_5alter_date);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_1objtype);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_6objsubtype);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_7objversion);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_8system_use);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 1054 "dutpopup.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	/* Error, object does not exist */
	dut_cb->dut_c8_status = DUT_ERROR;
	return(DUT_ERROR);
    }
/* # line 1061 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select iidd_ddb_objects.object_name, iidd_ddb_tableinfo.table_name, i\
idd_ddb_ldbids.ldb_dbms, iidd_ddb_ldbids.ldb_node, iidd_ddb_ldbids.ldb\
_database from iidd_ddb_objects, iidd_ddb_ldbids, iidd_ddb_tableinfo w\
here iidd_ddb_objects.object_name=");
    IIputdomio((short *)0,1,32,0,dut_p8_objattr_p->dut_p8_2objname);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and iidd_ddb_objects.object_owner=");
    IIputdomio((short *)0,1,32,0,dut_p8_objattr_p->dut_p8_3objowner);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and iidd_ddb_objects.object_base=iidd_ddb_tableinfo.object_base and i\
idd_ddb_objects.object_index=iidd_ddb_tableinfo.object_index and iidd_\
ddb_tableinfo.ldb_id=iidd_ddb_ldbids.ldb_id");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_2objname);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_9local_name);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_10dbmstype);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_11nodename);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p8_objattr_p->dut_p8_12ldbname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 1090 "dutpopup.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	/* Error, object does not exist */
	dut_cb->dut_c8_status = DUT_ERROR;
	return(DUT_ERROR);
    }
    STtrmwhite(dut_p8_objattr_p->dut_p8_1objtype);
    STtrmwhite(dut_p8_objattr_p->dut_p8_8system_use);
    STtrmwhite(dut_p8_objattr_p->dut_p8_6objsubtype);
    switch(dut_p8_objattr_p->dut_p8_6objsubtype[0])
    {
	case 'L':
		switch(dut_p8_objattr_p->dut_p8_1objtype[0])
		{
			case 'T':
				STcopy(DUT_M21_REGTABLE,
					dut_p8_objattr_p->dut_p8_1objtype);
				STcopy(DUT_M22_TABLE,
					dut_p8_objattr_p->dut_p8_1objtype);
				break;
			case 'V':
				STcopy(DUT_M23_REGVIEW,
					dut_p8_objattr_p->dut_p8_1objtype);
				STcopy(DUT_M24_VIEW,
					dut_p8_objattr_p->dut_p8_1objtype);
				break;
			case 'I':
				STcopy(DUT_M25_REGINDEX,
					dut_p8_objattr_p->dut_p8_1objtype);
				STcopy(DUT_M26_INDEX,
					dut_p8_objattr_p->dut_p8_1objtype);
				break;
		}
		break;
	case 'N':
		switch(dut_p8_objattr_p->dut_p8_1objtype[0])
		{
			case 'T':
				STcopy(DUT_M27_STAR_TABLE,
					dut_p8_objattr_p->dut_p8_1objtype);
				STcopy(DUT_M22_TABLE,
					dut_p8_objattr_p->dut_p8_1objtype);
				break;
			case 'V':
				STcopy(DUT_M28_DIST_VIEW,
					dut_p8_objattr_p->dut_p8_1objtype);
				STcopy(DUT_M24_VIEW,
					dut_p8_objattr_p->dut_p8_1objtype);
				break;
			case 'P':
				STcopy(DUT_M29_PART_TABLE,
					dut_p8_objattr_p->dut_p8_1objtype);
				break;
			case 'R':
				STcopy(DUT_M30_REPLICATE_TABLE,
					dut_p8_objattr_p->dut_p8_1objtype);
				break;
			case 'I':
				STcopy(DUT_M31_NATIVE_INDEX,
					dut_p8_objattr_p->dut_p8_1objtype);
				STcopy(DUT_M26_INDEX,
					dut_p8_objattr_p->dut_p8_1objtype);
				break;
		}
		break;
    }
    switch (dut_p8_objattr_p->dut_p8_8system_use[0])
    {
	case 'S':
	    STcopy(DUT_M32_SYSTEM_OBJ,
			dut_p8_objattr_p->dut_p8_8system_use);
	    break;
	case 'U':
	    STcopy(DUT_M33_USER_OBJ,
			dut_p8_objattr_p->dut_p8_8system_use);
	    break;
    }
    if (!STcompare(dut_p8_objattr_p->dut_p8_6objsubtype, "N"))
    {
	STcopy(DUT_M34_NATIVE_OBJ,
		dut_p8_objattr_p->dut_p8_6objsubtype);
    }
    else if (!STcompare(dut_p8_objattr_p->dut_p8_6objsubtype, "L"))
    {
	STcopy(DUT_M35_REG_OBJ,
		dut_p8_objattr_p->dut_p8_6objsubtype);
    }
/* # line 1181 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p8_objattr",(char *)"f") == 0) goto IIfdE5;
    goto IImuI5;
IIfdI5:;
    if (IIfsetio((char *)0) == 0) goto IIfdE5;
/* # line 1185 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p8_objattr") != 0) {
        IIputfldio((char *)"dut_p8_1objtype",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_1objtype);
        IIputfldio((char *)"dut_p8_2objname",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_2objname);
        IIputfldio((char *)"dut_p8_3objowner",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_3objowner);
        IIputfldio((char *)"dut_p8_4create_date",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_4create_date);
        IIputfldio((char *)"dut_p8_5alter_date",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_5alter_date);
        IIputfldio((char *)"dut_p8_6objsubtype",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_6objsubtype);
        IIputfldio((char *)"dut_p8_7objversion",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_7objversion);
        IIputfldio((char *)"dut_p8_8system_use",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_8system_use);
        IIputfldio((char *)"dut_p8_9local_name",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_9local_name);
        IIputfldio((char *)"dut_p8_10dbmstype",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_10dbmstype);
        IIputfldio((char *)"dut_p8_11nodename",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_11nodename);
        IIputfldio((char *)"dut_p8_12ldbname",(short *)0,1,32,0,
        dut_p8_objattr_p->dut_p8_12ldbname);
      } /* IIfsetio */
    }
IIfdB5:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1204 "dutpopup.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE5;
    if (IIendmu() == 0) goto IIfdE5;
    goto IIfdI5;
IIfdE5:;
    IIendfrm();
  }
/* # line 1206 "dutpopup.sc" */	/* host code */
}
/*
** Name: dut_pp9_1viewattr
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      11-jan-1989 (alexc)
**          Creation.
*/
dut_pp9_1viewattr(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_T3_OBJLIST *dut_t3_objlist_p;
  DUT_P9_VIEWATTR *dut_p9_viewattr_p;
  DUT_P8_OBJATTR *dut_p8_objattr_p;
    dut_t3_objlist_p	= dut_cb->dut_form_objlist->dut_f3_9objlist_curr_p;
    dut_p9_viewattr_p	= dut_cb->dut_popup_viewattr;
    dut_p8_objattr_p	= dut_cb->dut_popup_objattr;
    /* Get view information */
    if (dut_uu2_dbop(DUT_DIRECT_CONNECT_CDB, dut_cb, dut_errcb) 
	!= E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT, 2,
		0, DUT_1CDB_LONG);
	return(DUT_ERROR);
    }
/* # line 1255 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_type, table_subtype from iidd_tables where iidd_tables.t\
able_name=");
    IIputdomio((short *)0,1,32,0,dut_p8_objattr_p->dut_p8_2objname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and iidd_tables.table_owner=");
    IIputdomio((short *)0,1,32,0,dut_p8_objattr_p->dut_p8_3objowner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 1268 "dutpopup.sc" */	/* host code */
    STtrmwhite(dut_cb->dut_c0_buffer);
    STtrmwhite(dut_cb->dut_c0_buffer2);
    if (dut_cb->dut_c0_buffer2[0] == 'L')
    {
	/* "L" means Registered View, 
	** and "N" means native distributed View.
	*/
	dut_pp8_1objattr(dut_cb, dut_errcb);
	return(E_DU_OK);
    }
/* # line 1279 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, view_dml, check_option, text_sequence\
, text_segment from iidd_views where(iidd_views.table_name=");
    IIputdomio((short *)0,1,32,0,dut_p9_viewattr_p->dut_p9_3view_name);
    IIwritio(0,(short *)0,1,32,0,(char *)")and(iidd_views.table_owner=");
    IIputdomio((short *)0,1,32,0,dut_p9_viewattr_p->dut_p9_4view_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)")and(iidd_views.text_sequenc\
e=1)");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p9_viewattr_p->dut_p9_3view_name);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p9_viewattr_p->dut_p9_4view_owner);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p9_viewattr_p->dut_p9_5view_dml);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_p9_viewattr_p->dut_p9_6check_option);
      IIgetdomio((short *)0,1,30,4,&dut_p9_viewattr_p->dut_p9_7text_sequence);
      IIgetdomio((short *)0,1,32,(DB_MAXNAME << 2)-1,
      dut_p9_viewattr_p->dut_p9_8text_segment);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 1302 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p9_viewattr",(char *)"f") == 0) goto IIfdE6;
    goto IImuI6;
IIfdI6:;
    if (IIfsetio((char *)0) == 0) goto IIfdE6;
/* # line 1306 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p9_viewattr") != 0) {
        IIputfldio((char *)"dut_p9_3view_name",(short *)0,1,32,0,
        dut_p9_viewattr_p->dut_p9_3view_name);
        IIputfldio((char *)"dut_p9_4view_owner",(short *)0,1,32,0,
        dut_p9_viewattr_p->dut_p9_4view_owner);
        IIputfldio((char *)"dut_p9_5view_dml",(short *)0,1,32,0,
        dut_p9_viewattr_p->dut_p9_5view_dml);
        IIputfldio((char *)"dut_p9_6check_option",(short *)0,1,32,0,
        dut_p9_viewattr_p->dut_p9_6check_option);
        IIputfldio((char *)"dut_p9_7text_sequence",(short *)0,1,30,4,&
        dut_p9_viewattr_p->dut_p9_7text_sequence);
        IIputfldio((char *)"dut_p9_8text_segment",(short *)0,1,32,0,
        dut_p9_viewattr_p->dut_p9_8text_segment);
      } /* IIfsetio */
    }
IIfdB6:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1319 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE6;
        }
      } else {
        if (1) goto IIfdE6;
      } /* IIretval */
    } /* IIrunform */
IIfdF6:
    if (IIchkfrm() == 0) goto IIfdB6;
    goto IIfdE6;
IImuI6:
    if (IIinitmu() == 0) goto IIfdE6;
    if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE6;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE6;
    if (IIendmu() == 0) goto IIfdE6;
    goto IIfdI6;
IIfdE6:;
    IIendfrm();
  }
/* # line 1321 "dutpopup.sc" */	/* host code */
}
/*
** Name: dut_pp11_1perror	- print error
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      2-dec-1988 (alexc)
**          Creation.
*/
dut_pp11_1perror(dut_msgstr)
  char dut_msgstr[];
{
/* # line 1350 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p11_perror",(char *)"f") == 0) goto IIfdE7;
    goto IImuI7;
IIfdI7:;
    if (IIfsetio((char *)0) == 0) goto IIfdE7;
/* # line 1354 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p11_perror") != 0) {
        IIputfldio((char *)"dut_p11_1msgstr",(short *)0,1,32,0,dut_msgstr);
      } /* IIfsetio */
    }
IIfdB7:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1362 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE7;
        }
      } else {
        if (1) goto IIfdE7;
      } /* IIretval */
    } /* IIrunform */
IIfdF7:
    if (IIchkfrm() == 0) goto IIfdB7;
    goto IIfdE7;
IImuI7:
    if (IIinitmu() == 0) goto IIfdE7;
    if (IInmuact((char *)"End",(char *)0,2,2,1) == 0) goto IIfdE7;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE7;
    if (IIendmu() == 0) goto IIfdE7;
    goto IIfdI7;
IIfdE7:;
    IIendfrm();
  }
/* # line 1364 "dutpopup.sc" */	/* host code */
    return(DUT_ERROR);
}
/*
** Name: dut_pp12_1ldblist
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      2-dec-1988 (alexc)
**          Creation.
*/
dut_pp12_1ldblist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  i4 n_rows;
    DUT_P12_LDBLIST *dut_p12_ldblist_p;
/* # line 1400 "dutpopup.sc" */	/* host code */
    dut_p12_ldblist_p = dut_cb->dut_popup_ldblist;
/* # line 1402 "dutpopup.sc" */	/* inittable */
  {
    if (IItbinit((char *)"dut_p12_ldblist",(char *)"dut_p12_1ldblist",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1403 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p12_ldblist",(char *)"f") == 0) goto IIfdE8;
    goto IImuI8;
IIfdI8:;
    if (IIfsetio((char *)0) == 0) goto IIfdE8;
/* # line 1407 "dutpopup.sc" */	/* host code */
	dut_cb->dut_c0_buffer[0] = (char)EOS;
	dut_cb->dut_c8_status = E_DU_OK;
	dut_pp12_2load_ldblist(dut_cb, dut_errcb);
	if (dut_cb->dut_c8_status == DUT_ERROR)
	{
/* # line 1412 "dutpopup.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE8;
    }
/* # line 1413 "dutpopup.sc" */	/* host code */
	}
IIfdB8:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1418 "dutpopup.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"dut_p12_ldblist",(char *)0, (char *)0) != 0
          ) {
            IIiqfsio((short *)0,1,30,4,&n_rows,31,(char *)
"dut_p12_1ldblist",0);
          } /* IIiqset */
        }
/* # line 1422 "dutpopup.sc" */	/* host code */
	if (n_rows == 0)
	{
	    dut_cb->dut_c0_buffer2[0] = EOS;
/* # line 1425 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE8;
        }
/* # line 1426 "dutpopup.sc" */	/* host code */
	}
/* # line 1427 "dutpopup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_p12_ldblist",(char *)
"dut_p12_1ldblist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer,
            (char *)"dut_t12_1ldbname");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1431 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE8;
        }
      } else if (IIretval() == 2) {
/* # line 1436 "dutpopup.sc" */	/* host code */
	dut_uu7_table_top(NULL);
      } else if (IIretval() == 3) {
	dut_uu8_table_bottom(NULL);
      } else if (IIretval() == 4) {
	dut_uu9_table_find(dut_cb, dut_errcb, DUT_M36_LDB_NAME, NULL, NULL);
      } else if (IIretval() == 5) {
/* # line 1451 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp12_1name);
        }
      } else if (IIretval() == 6) {
/* # line 1460 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE8;
        }
      } else {
        if (1) goto IIfdE8;
      } /* IIretval */
    } /* IIrunform */
IIfdF8:
    if (IIchkfrm() == 0) goto IIfdB8;
    goto IIfdE8;
IImuI8:
    if (IIinitmu() == 0) goto IIfdE8;
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE8;
    if (IInmuact((char *)"Top",(char *)0,2,2,2) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,2) == 0) goto IIfdE8;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,3) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,3) == 0) goto IIfdE8;
    if (IInmuact((char *)"Find",(char *)0,2,2,4) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,4) == 0) goto IIfdE8;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE8;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE8;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE8;
    if (IIendmu() == 0) goto IIfdE8;
    goto IIfdI8;
IIfdE8:;
    IIendfrm();
  }
/* # line 1462 "dutpopup.sc" */	/* host code */
}
/*
** Name: dut_pp12_2load_ldblist
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      2-dec-1988 (alexc)
**          Creation.
*/
dut_pp12_2load_ldblist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P12_LDBLIST *dut_p12_ldblist_p;
  DUT_T12_LDBLIST *dut_t12_ldblist_p;
  DUT_T12_LDBLIST *dut_t12_ldblist_curr_p;
  i4 dut_p12_0count;
    i4			dut_p12_0fill;
/* # line 1500 "dutpopup.sc" */	/* host code */
    dut_p12_0count	= 0;
    dut_p12_0fill	= 0;
    dut_p12_ldblist_p = (DUT_P12_LDBLIST *)dut_cb->dut_popup_ldblist;
    dut_t12_ldblist_p =
	(DUT_T12_LDBLIST *)dut_cb->dut_popup_ldblist->dut_p12_1ldblist;
    if (dut_uu2_dbop(DUT_DIRECT_CONNECT_CDB, dut_cb, dut_errcb) 
	!= E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT, 2,
		0, DUT_1CDB_LONG);
	return(DUT_ERROR);
    }
/* # line 1514 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct count(*)from iidd_ddb_ldbids");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_p12_0count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 1518 "dutpopup.sc" */	/* host code */
    if (dut_p12_0count > dut_p12_ldblist_p->dut_p12_0size)
    {
	MEfree(dut_p12_ldblist_p->dut_p12_1ldblist);
	dut_p12_ldblist_p->dut_p12_1ldblist = NULL;
	dut_t12_ldblist_p = NULL;
    }
    if (dut_t12_ldblist_p == NULL)
    {
	dut_t12_ldblist_p = (DUT_T12_LDBLIST *)MEreqmem(0,
		(dut_p12_0count + DUT_ME_EXTRA) * sizeof(DUT_T12_LDBLIST),
		TRUE, NULL);
	dut_p12_ldblist_p->dut_p12_1ldblist
		= (DUT_T12_LDBLIST *)dut_t12_ldblist_p;
	dut_p12_ldblist_p->dut_p12_0size = (dut_p12_0count + DUT_ME_EXTRA);
    }
    dut_t12_ldblist_curr_p = (DUT_T12_LDBLIST *)dut_t12_ldblist_p;
    if (dut_cb->dut_c9_status == DUT_P_POPUP_2)
    {
/* # line 1539 "dutpopup.sc" */	/* loadtable */
  {
    if (IItbact((char *)"dut_p12_ldblist",(char *)"dut_p12_1ldblist",1) != 0) 
    {
      IItcoputio((char *)"dut_t12_1ldbname",(short *)0,1,32,0,(char *)"\
*");
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1543 "dutpopup.sc" */	/* host code */
    }
    dut_cb->dut_on_error_exit = TRUE;
/* # line 1548 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct trim(ldb_database)from iidd_ddb_ldbids order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_t12_ldblist_p->dut_t12_1ldbname);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 1555 "dutpopup.sc" */	/* loadtable */
      {
        if (IItbact((char *)"dut_p12_ldblist",(char *)"dut_p12_1ldblis\
t",1) != 0) {
          IItcoputio((char *)"dut_t12_1ldbname",(short *)0,1,32,0,
          dut_t12_ldblist_p->dut_t12_1ldbname);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1560 "dutpopup.sc" */	/* host code */
	dut_p12_0fill++;
	dut_t12_ldblist_p++;
	if (sqlca.sqlcode < 0)
	{
	    dut_cb->dut_c8_status = DUT_ERROR;
/* # line 1565 "dutpopup.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
        if (sqlca.sqlcode < 0) 
          dut_ee5_none_starview();
      }
/* # line 1566 "dutpopup.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
    dut_cb->dut_on_error_exit = FALSE;
    dut_p12_ldblist_p->dut_p12_0num_entry = dut_p12_0fill;
}
/*
** Name: dut_pp13_1ownlist
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      7-apr-1989 (alexc)
**          Creation.
*/
dut_pp13_1ownlist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  i4 n_rows;
    DUT_P13_OWNLIST *dut_p13_ownlist_p;
/* # line 1608 "dutpopup.sc" */	/* host code */
    dut_p13_ownlist_p = dut_cb->dut_popup_ownlist;
/* # line 1610 "dutpopup.sc" */	/* inittable */
  {
    if (IItbinit((char *)"dut_p13_ownlist",(char *)"dut_p13_1ownlist",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1611 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p13_ownlist",(char *)"f") == 0) goto IIfdE9;
    goto IImuI9;
IIfdI9:;
    if (IIfsetio((char *)0) == 0) goto IIfdE9;
/* # line 1615 "dutpopup.sc" */	/* host code */
	dut_cb->dut_c0_buffer[0] = (char)EOS;
	dut_cb->dut_c8_status = E_DU_OK;
	dut_pp13_2load_ownlist(dut_cb, dut_errcb);
	if (dut_cb->dut_c8_status == DUT_ERROR)
	{
/* # line 1620 "dutpopup.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE9;
    }
/* # line 1621 "dutpopup.sc" */	/* host code */
	}
IIfdB9:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1626 "dutpopup.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"dut_p13_ownlist",(char *)0, (char *)0) != 0
          ) {
            IIiqfsio((short *)0,1,30,4,&n_rows,31,(char *)
"dut_p13_1ownlist",0);
          } /* IIiqset */
        }
/* # line 1630 "dutpopup.sc" */	/* host code */
	if (n_rows == 0)
	{
	    dut_cb->dut_c0_buffer2[0] = EOS;
/* # line 1633 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE9;
        }
/* # line 1634 "dutpopup.sc" */	/* host code */
	}
/* # line 1635 "dutpopup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_p13_ownlist",(char *)
"dut_p13_1ownlist",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer,
            (char *)"dut_t13_1owner");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1639 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE9;
        }
      } else if (IIretval() == 2) {
/* # line 1644 "dutpopup.sc" */	/* host code */
	dut_uu7_table_top(NULL);
      } else if (IIretval() == 3) {
	dut_uu8_table_bottom(NULL);
      } else if (IIretval() == 4) {
	dut_uu9_table_find(dut_cb, dut_errcb, DUT_M37_OWNER_NAME, NULL, NULL);
      } else if (IIretval() == 5) {
/* # line 1659 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp13_1name);
        }
      } else if (IIretval() == 6) {
/* # line 1668 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE9;
        }
      } else {
        if (1) goto IIfdE9;
      } /* IIretval */
    } /* IIrunform */
IIfdF9:
    if (IIchkfrm() == 0) goto IIfdB9;
    goto IIfdE9;
IImuI9:
    if (IIinitmu() == 0) goto IIfdE9;
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE9;
    if (IInmuact((char *)"Top",(char *)0,2,2,2) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,2) == 0) goto IIfdE9;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,3) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,3) == 0) goto IIfdE9;
    if (IInmuact((char *)"Find",(char *)0,2,2,4) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,4) == 0) goto IIfdE9;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE9;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE9;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE9;
    if (IIendmu() == 0) goto IIfdE9;
    goto IIfdI9;
IIfdE9:;
    IIendfrm();
  }
/* # line 1670 "dutpopup.sc" */	/* host code */
}
/*
** Name: dut_pp13_2load_ownlist
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      7-apr-1989 (alexc)
**          Creation.
*/
dut_pp13_2load_ownlist(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P13_OWNLIST *dut_p13_ownlist_p;
  DUT_T13_OWNLIST *dut_t13_ownlist_p;
  DUT_T13_OWNLIST *dut_t13_ownlist_curr_p;
  i4 dut_p13_0count;
    i4			dut_p13_0fill;
/* # line 1708 "dutpopup.sc" */	/* host code */
    dut_p13_0count	= 0;
    dut_p13_0fill	= 0;
    dut_p13_ownlist_p = (DUT_P13_OWNLIST *)dut_cb->dut_popup_ownlist;
    dut_t13_ownlist_p =
	(DUT_T13_OWNLIST *)dut_cb->dut_popup_ownlist->dut_p13_1ownlist;
    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) 
	!= E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
			dut_cb->dut_c1_ddbname);
	return(DUT_ERROR);
    }
/* # line 1722 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct count(object_owner)from iiddb_objects");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_p13_0count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 1726 "dutpopup.sc" */	/* host code */
    if (dut_p13_0count > dut_p13_ownlist_p->dut_p13_0size)
    {
	MEfree(dut_p13_ownlist_p->dut_p13_1ownlist);
	dut_p13_ownlist_p->dut_p13_1ownlist = NULL;
	dut_t13_ownlist_p = NULL;
    }
    if (dut_t13_ownlist_p == NULL)
    {
	dut_t13_ownlist_p = (DUT_T13_OWNLIST *)MEreqmem(0,
		(dut_p13_0count + DUT_ME_EXTRA) * sizeof(DUT_T13_OWNLIST),
		TRUE, NULL);
	dut_p13_ownlist_p->dut_p13_1ownlist
		= (DUT_T13_OWNLIST *)dut_t13_ownlist_p;
	dut_p13_ownlist_p->dut_p13_0size = (dut_p13_0count + DUT_ME_EXTRA);
    }
    dut_t13_ownlist_curr_p = (DUT_T13_OWNLIST *)dut_t13_ownlist_p;
    if (dut_cb->dut_c9_status == DUT_P_POPUP_2)
    {
/* # line 1747 "dutpopup.sc" */	/* loadtable */
  {
    if (IItbact((char *)"dut_p13_ownlist",(char *)"dut_p13_1ownlist",1) != 0) 
    {
      IItcoputio((char *)"dut_t13_1owner",(short *)0,1,32,0,(char *)"*");
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1751 "dutpopup.sc" */	/* host code */
    }
    dut_cb->dut_on_error_exit = TRUE;
/* # line 1756 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct trim(object_owner)from iiddb_objects order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_t13_ownlist_p->dut_t13_1owner);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 1763 "dutpopup.sc" */	/* loadtable */
      {
        if (IItbact((char *)"dut_p13_ownlist",(char *)"dut_p13_1ownlis\
t",1) != 0) {
          IItcoputio((char *)"dut_t13_1owner",(short *)0,1,32,0,
          dut_t13_ownlist_p->dut_t13_1owner);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 1768 "dutpopup.sc" */	/* host code */
	dut_p13_0fill++;
	dut_t13_ownlist_p++;
	if (sqlca.sqlcode < 0)
	{
	    dut_cb->dut_c8_status = DUT_ERROR;
/* # line 1773 "dutpopup.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE4;
        if (sqlca.sqlcode < 0) 
          dut_ee5_none_starview();
      }
/* # line 1774 "dutpopup.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
    dut_cb->dut_on_error_exit = FALSE;
    dut_p13_ownlist_p->dut_p13_0num_entry = dut_p13_0fill;
}
/*
** Name: dut_pp14_1browse	- Browsing Nodes, databases, and tables.
**
** Description:
**      Browse the node name.
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      3-may-1989 (alexc)
**          Creation.
*/
dut_pp14_1browse(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
    /* Load GCA file information into array */
    /* Allow selection of Node name, or user entered node name */
    /* Call browse_ldb */
    if (dut_pp14_1browse_node(dut_cb, dut_errcb) == E_DU_OK)
    {
    	return(E_DU_OK);
    }
}
/*
** Name: dut_pp14_1browse_node	- Browsing Nodes, databases, and tables.
**
** Description:
**      Browse the node name.
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      3-may-1989 (alexc)
**          Creation.
*/
dut_pp14_1browse_node(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P14_NODE *dut_p14_node_p;
  DUT_T14_NODE *dut_t14_node_p;
    i4			dut_error_status;
    dut_p14_node_p = dut_cb->dut_popup_b_node;
    dut_t14_node_p = dut_p14_node_p->dut_p14_1node;
/* # line 1854 "dutpopup.sc" */	/* inittable */
  {
    if (IItbinit((char *)"dut_p14_node",(char *)"dut_p14_1node",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 1855 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p14_node",(char *)"f") == 0) goto IIfdE10;
    goto IImuI10;
IIfdI10:;
    if (IIfsetio((char *)0) == 0) goto IIfdE10;
/* # line 1859 "dutpopup.sc" */	/* host code */
	dut_cb->dut_c8_status = E_DU_OK;
	dut_error_status = dut_uu24_gcf_node(dut_cb, dut_errcb);
	if (dut_error_status != DUT_ERROR &&
	    dut_cb->dut_c8_status != DUT_ERROR)
	{
	    dut_pp14_2load_node(dut_cb, dut_errcb);
	}
IIfdB10:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 1870 "dutpopup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_p14_node",(char *)"dut_p14_1nod\
e",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer,
            (char *)"dut_t14_1node");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1874 "dutpopup.sc" */	/* host code */
	STcopy(dut_cb->dut_c0_buffer, dut_cb->dut_c16_browse_node);
	if  (dut_cb->dut_c0_buffer[0] != EOS)
	{
	    /* check for connectability first */
	    dut_pp15_1ldb(dut_cb, dut_errcb);
	}
      } else if (IIretval() == 2) {
	dut_uu7_table_top("dut_p14_1node");
      } else if (IIretval() == 3) {
	dut_uu8_table_bottom("dut_p14_1node");
      } else if (IIretval() == 4) {
	dut_uu9_table_find(dut_cb, dut_errcb,
		DUT_M08_NODENAME, "dut_p14_1node", NULL);
      } else if (IIretval() == 5) {
/* # line 1900 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp14_1name);
        }
      } else if (IIretval() == 6) {
/* # line 1909 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE10;
        }
      } else {
        if (1) goto IIfdE10;
      } /* IIretval */
    } /* IIrunform */
IIfdF10:
    if (IIchkfrm() == 0) goto IIfdB10;
    goto IIfdE10;
IImuI10:
    if (IIinitmu() == 0) goto IIfdE10;
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE10;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE10;
    if (IInmuact((char *)"Top",(char *)0,2,2,2) == 0) goto IIfdE10;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,2) == 0) goto IIfdE10;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,3) == 0) goto IIfdE10;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,3) == 0) goto IIfdE10;
    if (IInmuact((char *)"Find",(char *)0,2,2,4) == 0) goto IIfdE10;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,4) == 0) goto IIfdE10;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE10;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE10;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE10;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE10;
    if (IIendmu() == 0) goto IIfdE10;
    goto IIfdI10;
IIfdE10:;
    IIendfrm();
  }
/* # line 1911 "dutpopup.sc" */	/* host code */
    return(E_DU_OK);
}
/*
** Name: dut_pp14_2load_node	- Load node into popup.
**
** Description:
**      Load nodes form dut_cb->dut_p14_node into the popup.
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      8-may-1989 (alexc)
**          Creation.
*/
dut_pp14_2load_node(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P14_NODE *dut_p14_node_p;
  DUT_T14_NODE *dut_t14_node_p;
  DUT_T14_NODE *dut_t14_node_curr_p;
    i4			dut_p14_0fill;
    dut_p14_node_p = dut_cb->dut_popup_b_node;
    dut_t14_node_p = dut_p14_node_p->dut_p14_1node;
    dut_t14_node_curr_p = dut_t14_node_p;
/* # line 1951 "dutpopup.sc" */	/* clear */
  {
    IIfldclear((char *)"dut_p14_1node");
  }
/* # line 1952 "dutpopup.sc" */	/* host code */
    for (dut_p14_0fill = 0;
	dut_p14_0fill < dut_p14_node_p->dut_p14_0num_entry;
	dut_p14_0fill++)
    {
/* # line 1956 "dutpopup.sc" */	/* loadtable */
  {
    if (IItbact((char *)"dut_p14_node",(char *)"dut_p14_1node",1) != 0) {
      IItcoputio((char *)"dut_t14_1node",(short *)0,1,32,0,
      dut_t14_node_curr_p->dut_t14_1node);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 1960 "dutpopup.sc" */	/* host code */
	dut_t14_node_curr_p++;
    }
/* # line 1962 "dutpopup.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1963 "dutpopup.sc" */	/* host code */
    return(E_DU_OK);
}
/*
** Name: dut_pp15_1ldb	- Browse Local Database Names.
**
** Description:
**
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      8-may-1989 (alexc)
**          Creation.
*/
dut_pp15_1ldb(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P15_LDB *dut_p15_ldb_p;
  DUT_T15_LDB *dut_t15_ldb_p;
  DUT_T15_LDB *dut_t15_ldb_curr_p;
  i4 dut_p15_0count;
    i4		dut_p15_0fill;
    dut_p15_ldb_p = dut_cb->dut_popup_b_ldb;
    dut_t15_ldb_p = dut_p15_ldb_p->dut_p15_1ldb;
    dut_t15_ldb_curr_p = dut_t15_ldb_p;
    STcopy("iidbdb", dut_cb->dut_c3_ldbname);
    STcopy(dut_cb->dut_c16_browse_node, dut_cb->dut_c4_ldbnode);
    STprintf(dut_cb->dut_c0_buffer, "%s::%s",
			dut_cb->dut_c4_ldbnode,
			"iidbdb");
    if (dut_uu2_dbop(DUT_DIRECT_CONNECT, dut_cb, dut_errcb) != E_DU_OK)
    {
        dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT,
			2, 0, dut_cb->dut_c0_buffer);
	return(DUT_ERROR);
    }
/* # line 2018 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from iidataba\
se");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_p15_0count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 2022 "dutpopup.sc" */	/* host code */
    dut_cb->dut_on_error_exit = FALSE;
    if (dut_p15_0count > dut_p15_ldb_p->dut_p15_0size)
    {
	MEfree(dut_t15_ldb_p);
	dut_p15_ldb_p->dut_p15_1ldb = NULL;
	dut_t15_ldb_p = NULL;
    }
    if (dut_t15_ldb_p == NULL)
    {
	dut_t15_ldb_p = (DUT_T15_LDB *)MEreqmem(0,
		(dut_p15_0count + DUT_ME_EXTRA) * sizeof(DUT_T15_LDB),
		TRUE, NULL);
	dut_p15_ldb_p->dut_p15_1ldb = dut_t15_ldb_p;
	dut_cb->dut_popup_b_ldb->dut_p15_0size
		= dut_p15_0count + DUT_ME_EXTRA;
    }
    dut_t15_ldb_curr_p = dut_t15_ldb_p;
/* # line 2043 "dutpopup.sc" */	/* inittable */
  {
    if (IItbinit((char *)"dut_p15_ldb",(char *)"dut_p15_1ldb",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 2044 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p15_ldb",(char *)"f") == 0) goto IIfdE11;
    goto IImuI11;
IIfdI11:;
    if (IIfsetio((char *)0) == 0) goto IIfdE11;
/* # line 2048 "dutpopup.sc" */	/* host code */
	dut_cb->dut_on_error_exit = TRUE;
/* # line 2050 "dutpopup.sc" */	/* select */
    {
      IIsqInit(&sqlca);
      IIwritio(0,(short *)0,1,32,0,(char *)
"select trim(name)from iidatabase where not dbservice=1 order by 1");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
        dut_t15_ldb_curr_p->dut_t15_1ldb);
        if (IIerrtest() != 0) goto IIrtB5;
/* # line 2060 "dutpopup.sc" */	/* host code */
	    STcopy("ingres", dut_t15_ldb_curr_p->dut_t15_2dbms_type);
/* # line 2061 "dutpopup.sc" */	/* loadtable */
        {
          if (IItbact((char *)"dut_p15_ldb",(char *)"dut_p15_1ldb",1) != 0) {
            IItcoputio((char *)"dut_t15_1ldb",(short *)0,1,32,0,
            dut_t15_ldb_curr_p->dut_t15_1ldb);
            IItcoputio((char *)"dut_t15_2dbms_type",(short *)0,1,32,0,
            dut_t15_ldb_curr_p->dut_t15_2dbms_type);
            IITBceColEnd();
          } /* IItbact */
        }
/* # line 2066 "dutpopup.sc" */	/* host code */
	    dut_p15_0fill++;
	    dut_t15_ldb_curr_p++;
	    if (sqlca.sqlcode < 0)
	    {
	    	dut_cb->dut_c8_status = DUT_ERROR;
/* # line 2071 "dutpopup.sc" */	/* endselect */
        {
          IIbreak();
          if (1) goto IIrtE5;
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2072 "dutpopup.sc" */	/* host code */
	    }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE5:;
      if (sqlca.sqlcode < 0) 
        dut_ee5_none_starview();
    }
	dut_cb->dut_on_error_exit = FALSE;
	dut_p15_ldb_p->dut_p15_0num_entry = dut_p15_0fill;
IIfdB11:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 2082 "dutpopup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_p15_ldb",(char *)"dut_p15_1ldb",-2) != 
          0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer,
            (char *)"dut_t15_1ldb");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer3,
            (char *)"dut_t15_2dbms_type");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 2087 "dutpopup.sc" */	/* host code */
	STcopy(dut_cb->dut_c0_buffer, dut_cb->dut_c17_browse_database);
	STcopy(dut_cb->dut_c0_buffer3, dut_cb->dut_c18_browse_dbms);
	dut_cb->dut_on_error_exit = FALSE;
	dut_pp16_1browse_table(dut_cb, dut_errcb);
      } else if (IIretval() == 2) {
	dut_uu7_table_top(NULL);
      } else if (IIretval() == 3) {
	dut_uu8_table_bottom(NULL);
      } else if (IIretval() == 4) {
	dut_uu9_table_find(dut_cb, dut_errcb,
		DUT_M36_LDB_NAME, "dut_p15_1ldb", NULL);
      } else if (IIretval() == 5) {
/* # line 2111 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp15_1name);
        }
      } else if (IIretval() == 6) {
/* # line 2120 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE11;
        }
      } else {
        if (1) goto IIfdE11;
      } /* IIretval */
    } /* IIrunform */
IIfdF11:
    if (IIchkfrm() == 0) goto IIfdB11;
    goto IIfdE11;
IImuI11:
    if (IIinitmu() == 0) goto IIfdE11;
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE11;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE11;
    if (IInmuact((char *)"Top",(char *)0,2,2,2) == 0) goto IIfdE11;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,2) == 0) goto IIfdE11;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,3) == 0) goto IIfdE11;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,3) == 0) goto IIfdE11;
    if (IInmuact((char *)"Find",(char *)0,2,2,4) == 0) goto IIfdE11;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,4) == 0) goto IIfdE11;
    if (IInmuact((char *)"Help",(char *)0,2,2,5) == 0) goto IIfdE11;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE11;
    if (IInmuact((char *)"End",(char *)0,2,2,6) == 0) goto IIfdE11;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE11;
    if (IIendmu() == 0) goto IIfdE11;
    goto IIfdI11;
IIfdE11:;
    IIendfrm();
  }
/* # line 2123 "dutpopup.sc" */	/* host code */
    return(E_DU_OK);
}
/*
** Name: dut_pp16_1browse_table	- Browse Local Database Names.
**
** Description:
**
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      10-may-1989 (alexc)
**          Creation.
**	06-jun-1991 (fpang)
**	    If dut_pp16_3browse_load_table() fails, ldb cannot be reached,
**	    so go back to previous form.
*/
dut_pp16_1browse_table(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P16_TABLE *dut_p16_table_p;
  DUT_T16_TABLE *dut_t16_table_p;
  DUT_T16_TABLE *dut_t16_table_curr_p;
  i4 dut_p16_0count;
  char dut_dbname[DDB_2MAXNAME];
    i4		dut_p16_0fill;
    dut_p16_table_p = dut_cb->dut_popup_b_table;
    dut_t16_table_p = dut_p16_table_p->dut_p16_1table;
    dut_t16_table_curr_p = dut_t16_table_p;
    STcopy(dut_cb->dut_c16_browse_node, dut_cb->dut_c4_ldbnode);
    STcopy(dut_cb->dut_c17_browse_database, dut_cb->dut_c3_ldbname);
    STprintf(dut_cb->dut_c0_buffer, "%s::%s",
		dut_cb->dut_c4_ldbnode,
		dut_cb->dut_c3_ldbname);
/* # line 2176 "dutpopup.sc" */	/* inittable */
  {
    if (IItbinit((char *)"dut_p16_table",(char *)"dut_p16_1table",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 2177 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p16_table",(char *)"f") == 0) goto IIfdE12;
    goto IImuI12;
IIfdI12:;
    if (IIfsetio((char *)0) == 0) goto IIfdE12;
/* # line 2181 "dutpopup.sc" */	/* host code */
	if (dut_pp16_3browse_load_table(dut_cb, dut_errcb) != E_DU_OK)
/* # line 2182 "dutpopup.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE12;
    }
IIfdB12:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 2187 "dutpopup.sc" */	/* host code */
        if (dut_p16_table_p->dut_p16_0num_entry == 0)
        {
		dut_ee1_error(dut_errcb, W_DU181D_SV_EMPTY_LDB, 0);
/* # line 2190 "dutpopup.sc" */	/* redisplay */
        {
          IIredisp();
        }
/* # line 2191 "dutpopup.sc" */	/* host code */
	} 
	else
	{
/* # line 2194 "dutpopup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"dut_p16_table",(char *)"dut_p16_1ta\
ble",-2) != 0) {
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer,
            (char *)"dut_t16_1table");
            IItcogetio((short *)0,1,32,DDB_MAXNAME-1,dut_cb->dut_c0_buffer2,
            (char *)"dut_t16_2own");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 2199 "dutpopup.sc" */	/* host code */
	    STcopy(dut_cb->dut_c0_buffer, dut_cb->dut_c19_browse_table);
	    STcopy(dut_cb->dut_c0_buffer2, dut_cb->dut_c20_browse_own);
	    dut_pp16_2register(dut_cb, dut_errcb);
	    /* Restore connection to browse ldb */
    	    STcopy(dut_cb->dut_c16_browse_node, dut_cb->dut_c4_ldbnode);
    	    STcopy(dut_cb->dut_c17_browse_database, dut_cb->dut_c3_ldbname);
    	    STprintf(dut_cb->dut_c0_buffer, "%s::%s",
	   	     dut_cb->dut_c4_ldbnode,
		     dut_cb->dut_c3_ldbname);
    	    if (dut_uu2_dbop(DUT_DIRECT_CONNECT, dut_cb, dut_errcb) != E_DU_OK)
    	    {
                dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT,
			2, 0, dut_cb->dut_c0_buffer);
    	    }
	}
      } else if (IIretval() == 2) {
	STcopy(dut_cb->dut_c16_browse_node, dut_cb->dut_c4_ldbnode);
	STcopy(dut_cb->dut_c17_browse_database, dut_cb->dut_c3_ldbname);
	STprintf(dut_cb->dut_c0_buffer, "%s::%s",
		dut_cb->dut_c4_ldbnode,
		dut_cb->dut_c3_ldbname);
	if (dut_uu2_dbop(DUT_DIRECT_CONNECT, dut_cb, dut_errcb) != E_DU_OK)
	{
            dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT,
			2, 0, dut_cb->dut_c0_buffer);
/* # line 2232 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE12;
        }
/* # line 2233 "dutpopup.sc" */	/* host code */
	}
	if ((dut_cb->dut_c16_browse_node[0] == EOS) ||
	    !STcompare(dut_cb->dut_c1_ddbname, dut_cb->dut_c13_vnodename))
	{
	    STprintf(dut_dbname, "%s",
			dut_cb->dut_c17_browse_database);
	}
	else
	{
	    STprintf(dut_dbname, "%s::%s",
			dut_cb->dut_c2_nodename,
			dut_cb->dut_c17_browse_database);
	}
	STprintf(dut_cb->dut_c0_buffer, "ISQL %s", dut_dbname);
/* # line 2248 "dutpopup.sc" */	/* message */
        {
          IImessage(dut_cb->dut_c0_buffer);
        }
/* # line 2249 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2250 "dutpopup.sc" */	/* call */
        {
          IIsqInit(&sqlca);
          IIutsys(0,(char *)"isql",(char *)0);
          IIutsys(1,(char *)"database",dut_dbname);
          IIutsys(2,(char *)0,(char *)0);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2251 "dutpopup.sc" */	/* host code */
	dut_cb->dut_on_error_noop = TRUE;
/* # line 2252 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2253 "dutpopup.sc" */	/* select */
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
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2254 "dutpopup.sc" */	/* host code */
	/* Want to set it to direct disconnect mode */
	STtrmwhite(dut_cb->dut_c0_buffer);
	if (!STcompare(dut_cb->dut_c0_buffer, dut_cb->dut_c1_ddbname))
	{
	    /* Already direct disconnected in the sub-call */
	}
	else
	{
/* # line 2262 "dutpopup.sc" */	/* direct disconnect */
        {
          IIsqInit(&sqlca);
          IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
          IIsyncup((char *)0,0);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2263 "dutpopup.sc" */	/* host code */
	}
	dut_cb->dut_c7_status = DUT_S_CONNECT_DDB;
/* # line 2265 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2266 "dutpopup.sc" */	/* host code */
	dut_cb->dut_on_error_noop = FALSE;
	dut_pp16_3browse_load_table(dut_cb, dut_errcb);
/* # line 2268 "dutpopup.sc" */	/* redisplay */
        {
          IIredisp();
        }
      } else if (IIretval() == 3) {
/* # line 2273 "dutpopup.sc" */	/* host code */
	STcopy(dut_cb->dut_c16_browse_node, dut_cb->dut_c4_ldbnode);
	STcopy(dut_cb->dut_c17_browse_database, dut_cb->dut_c3_ldbname);
	STprintf(dut_cb->dut_c0_buffer, "%s::%s",
		dut_cb->dut_c4_ldbnode,
		dut_cb->dut_c3_ldbname);
	if (dut_uu2_dbop(DUT_DIRECT_CONNECT, dut_cb, dut_errcb) != E_DU_OK)
	{
            dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT,
			2, 0, dut_cb->dut_c0_buffer);
/* # line 2284 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE12;
        }
/* # line 2285 "dutpopup.sc" */	/* host code */
	}
	if ((dut_cb->dut_c2_nodename[0] == EOS) ||
	    !STcompare(dut_cb->dut_c2_nodename, dut_cb->dut_c13_vnodename))
	{
	    STprintf(dut_dbname, "%s",
			dut_cb->dut_c17_browse_database);
	}
	else
	{
	    STprintf(dut_dbname, "%s::%s",
			dut_cb->dut_c2_nodename,
			dut_cb->dut_c17_browse_database);
	}
	STprintf(dut_cb->frs_message, "TABLES %s", dut_dbname);
/* # line 2300 "dutpopup.sc" */	/* message */
        {
          IImessage(dut_cb->frs_message);
        }
/* # line 2301 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2302 "dutpopup.sc" */	/* call */
        {
          IIsqInit(&sqlca);
          IIutsys(0,(char *)"tables",(char *)0);
          IIutsys(1,(char *)"database",dut_dbname);
          IIutsys(2,(char *)0,(char *)0);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2303 "dutpopup.sc" */	/* host code */
	dut_cb->dut_on_error_noop = TRUE;
/* # line 2304 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2305 "dutpopup.sc" */	/* select */
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
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2306 "dutpopup.sc" */	/* host code */
	/* Want to set it to direct disconnect mode */
	STtrmwhite(dut_cb->dut_c0_buffer);
	if (!STcompare(dut_cb->dut_c0_buffer, dut_cb->dut_c1_ddbname))
	{
	    /* Already direct disconnected in the sub-call */
	}
	else
	{
/* # line 2314 "dutpopup.sc" */	/* direct disconnect */
        {
          IIsqInit(&sqlca);
          IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
          IIsyncup((char *)0,0);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2315 "dutpopup.sc" */	/* host code */
	}
	dut_cb->dut_c7_status = DUT_S_CONNECT_DDB;
/* # line 2317 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2318 "dutpopup.sc" */	/* host code */
	dut_cb->dut_on_error_noop = FALSE;
	dut_pp16_3browse_load_table(dut_cb, dut_errcb);
/* # line 2320 "dutpopup.sc" */	/* redisplay */
        {
          IIredisp();
        }
      } else if (IIretval() == 4) {
/* # line 2325 "dutpopup.sc" */	/* host code */
	dut_uu7_table_top(NULL);
      } else if (IIretval() == 5) {
	dut_uu8_table_bottom(NULL);
      } else if (IIretval() == 6) {
	dut_uu9_table_find(dut_cb, dut_errcb, 
		DUT_M04_OBJNAME, "dut_p16_1table", NULL);
      } else if (IIretval() == 7) {
/* # line 2341 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp16_1name);
        }
      } else if (IIretval() == 8) {
/* # line 2350 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE12;
        }
      } else {
        if (1) goto IIfdE12;
      } /* IIretval */
    } /* IIrunform */
IIfdF12:
    if (IIchkfrm() == 0) goto IIfdB12;
    goto IIfdE12;
IImuI12:
    if (IIinitmu() == 0) goto IIfdE12;
    if (IInmuact((char *)"Register",(char *)0,2,2,1) == 0) goto IIfdE12;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE12;
    if (IInmuact((char *)"SQL",(char *)0,2,2,2) == 0) goto IIfdE12;
    if (IInmuact((char *)"Tables",(char *)0,2,2,3) == 0) goto IIfdE12;
    if (IInmuact((char *)"Top",(char *)0,2,2,4) == 0) goto IIfdE12;
    IIFRInternal(0);
    if (IInfrskact(5,(char *)0,2,2,4) == 0) goto IIfdE12;
    if (IInmuact((char *)"Bottom",(char *)0,2,2,5) == 0) goto IIfdE12;
    IIFRInternal(0);
    if (IInfrskact(6,(char *)0,2,2,5) == 0) goto IIfdE12;
    if (IInmuact((char *)"Find",(char *)0,2,2,6) == 0) goto IIfdE12;
    IIFRInternal(0);
    if (IInfrskact(7,(char *)0,2,2,6) == 0) goto IIfdE12;
    if (IInmuact((char *)"Help",(char *)0,2,2,7) == 0) goto IIfdE12;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,7) == 0) goto IIfdE12;
    if (IInmuact((char *)"End",(char *)0,2,2,8) == 0) goto IIfdE12;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,8) == 0) goto IIfdE12;
    if (IIendmu() == 0) goto IIfdE12;
    goto IIfdI12;
IIfdE12:;
    IIendfrm();
  }
/* # line 2353 "dutpopup.sc" */	/* host code */
}
/*
** Name: dut_pp16_2register	- Register the table object.
**
** Description:
**
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      10-may-1989 (alexc)
**          Creation.
*/
dut_pp16_2register(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  char *register_call;
  char msg_buffer[DB_SHORTMSG];
  char dut_p16_reg_buffer[DB_LONGMSG];
  DUT_P3_REGISTER *dut_p16_register_p;
  DUT_F3_OBJLIST *dut_f16_objlist_p;
    i4			bad_param;
    dut_cb->dut_on_error_exit = FALSE;
    dut_f16_objlist_p = dut_cb->dut_form_objlist;
    dut_p16_register_p = dut_cb->dut_popup_register;
    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) 
	!= E_DU_OK)
    {
	dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
			dut_cb->dut_c1_ddbname);
	return(DUT_ERROR);
    }
/* # line 2405 "dutpopup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dut_p3_register",(char *)"f") == 0) goto IIfdE13;
    goto IImuI13;
IIfdI13:;
    if (IIfsetio((char *)0) == 0) goto IIfdE13;
/* # line 2409 "dutpopup.sc" */	/* host code */
	STcopy(dut_cb->dut_c19_browse_table,
			dut_p16_register_p->dut_p3_1table);
	STcopy(dut_cb->dut_c19_browse_table,
			dut_p16_register_p->dut_p3_2source);
	STcopy(dut_cb->dut_c18_browse_dbms, 
			dut_p16_register_p->dut_p3_3dbms);
	STcopy(dut_cb->dut_c16_browse_node,
			dut_p16_register_p->dut_p3_4node);
	STcopy(dut_cb->dut_c17_browse_database,
			dut_p16_register_p->dut_p3_5database);
/* # line 2419 "dutpopup.sc" */	/* putform */
    {
      if (IIfsetio((char *)"dut_p3_register") != 0) {
        IIputfldio((char *)"dut_p3_1table",(short *)0,1,32,0,
        dut_p16_register_p->dut_p3_1table);
        IIputfldio((char *)"dut_p3_2source",(short *)0,1,32,0,
        dut_p16_register_p->dut_p3_2source);
        IIputfldio((char *)"dut_p3_3dbms",(short *)0,1,32,0,
        dut_p16_register_p->dut_p3_3dbms);
        IIputfldio((char *)"dut_p3_4node",(short *)0,1,32,0,
        dut_p16_register_p->dut_p3_4node);
        IIputfldio((char *)"dut_p3_5database",(short *)0,1,32,0,
        dut_p16_register_p->dut_p3_5database);
      } /* IIfsetio */
    }
IIfdB13:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 2431 "dutpopup.sc" */	/* host code */
	bad_param = TRUE;
	while (bad_param)
	{
	    bad_param = FALSE;
/* # line 2436 "dutpopup.sc" */	/* getform */
        {
          if (IIfsetio((char *)"dut_p3_register") != 0) {
            IIgetfldio(&dut_cb->dut_null_1,1,32,DDB_MAXNAME-1,
            dut_p16_register_p->dut_p3_1table,(char *)"dut_p3_1table");
            IIgetfldio(&dut_cb->dut_null_2,1,32,DDB_MAXNAME-1,
            dut_p16_register_p->dut_p3_2source,(char *)"dut_p3_2source");
            IIgetfldio(&dut_cb->dut_null_3,1,32,DDB_MAXNAME-1,
            dut_p16_register_p->dut_p3_3dbms,(char *)"dut_p3_3dbms");
            IIgetfldio(&dut_cb->dut_null_4,1,32,DDB_MAXNAME-1,
            dut_p16_register_p->dut_p3_4node,(char *)"dut_p3_4node");
            IIgetfldio(&dut_cb->dut_null_5,1,32,DDB_MAXNAME-1,
            dut_p16_register_p->dut_p3_5database,(char *)"dut_p3_5data\
base");
          } /* IIfsetio */
        }
/* # line 2449 "dutpopup.sc" */	/* host code */
	    if (dut_cb->dut_null_1 == -1)
	    {
	    	dut_p16_register_p->dut_p3_1table[0] = EOS;
		bad_param = TRUE;
	    }
	    if (dut_cb->dut_null_2 == -1)
	    {
	    	dut_p16_register_p->dut_p3_2source[0] = EOS;
		bad_param = TRUE;
	    }
	    if (dut_cb->dut_null_3 == -1)
	    {
	    	dut_p16_register_p->dut_p3_3dbms[0] = EOS;
		bad_param = TRUE;
	    }
	    if (dut_cb->dut_null_4 == -1)
	    {
	    	dut_p16_register_p->dut_p3_4node[0] = EOS;
		bad_param = TRUE;
	    }
	    if (dut_cb->dut_null_5 == -1)
	    {
	    	dut_p16_register_p->dut_p3_5database[0] = EOS;
		bad_param = TRUE;
	    }
	    if (dut_p16_register_p->dut_p3_1table[0] == 'i' 
		&& dut_p16_register_p->dut_p3_1table[1] == 'i')
	    {
		bad_param = TRUE;
	    }
	    if (bad_param == TRUE)
	    {
		dut_ee1_error(dut_errcb, W_DU181A_SV_CANT_REG_FROM_CRIT, 0);
	    }
	}
	CVlower(dut_p16_register_p->dut_p3_1table);
	STtrmwhite(dut_p16_register_p->dut_p3_1table);
	CVlower(dut_p16_register_p->dut_p3_2source);
	STtrmwhite(dut_p16_register_p->dut_p3_2source);
	STtrmwhite(dut_p16_register_p->dut_p3_3dbms);
	CVlower(dut_p16_register_p->dut_p3_4node);
	STtrmwhite(dut_p16_register_p->dut_p3_4node);
	CVlower(dut_p16_register_p->dut_p3_5database);
	STtrmwhite(dut_p16_register_p->dut_p3_5database);
/*
**	may need this condition later.
**	    dut_p16_register_p->dut_p3_3dbms[0]		!= EOS &&
*/
	if (bad_param == FALSE)
	{
	    /* If all data setup is okay, then go ahead and create
	    ** registration.
	    */
	    dut_cb->dut_on_error_exit = FALSE;
	    dut_cb->dut_on_error_noop = FALSE;
	    if (dut_uu2_dbop(DUT_DDBOPEN, dut_cb, dut_errcb) != E_DU_OK)
	    {
		dut_ee1_error(dut_errcb, W_DU1805_SV_CANT_OPEN_DDB, 2, 0, 
				dut_cb->dut_c1_ddbname);
/* # line 2509 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE13;
        }
/* # line 2510 "dutpopup.sc" */	/* host code */
	    }
	    /* Set up registration string */
	    STprintf(dut_p16_reg_buffer,
		"register %s as link from %s \
		with dbms = %s, node = %s, database = %s",
		dut_p16_register_p->dut_p3_1table,
		dut_p16_register_p->dut_p3_2source,
		dut_p16_register_p->dut_p3_3dbms,
		dut_p16_register_p->dut_p3_4node,
		dut_p16_register_p->dut_p3_5database);
	    STprintf(msg_buffer,
			DUT_M40_CREATE_REGIST,
			dut_p16_register_p->dut_p3_1table);   
	    /* Go ahead and execute the registration string */
/* # line 2524 "dutpopup.sc" */	/* message */
        {
          IImessage(msg_buffer);
        }
/* # line 2525 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2526 "dutpopup.sc" */	/* execute */
        {
          IIsqInit(&sqlca);
          IIsqExImmed(dut_p16_reg_buffer);
          IIsyncup((char *)0,0);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2528 "dutpopup.sc" */	/* host code */
	    STcopy(dut_p16_register_p->dut_p3_1table,
			dut_cb->dut_c0_buffer);
	    STcopy(dut_p16_register_p->dut_p3_0objowner,
			dut_cb->dut_c0_buffer2);
 	    if (sqlca.sqlcode < 0)
	    {
		dut_p16_register_p->dut_p3_4node[0] = EOS;
		dut_p16_register_p->dut_p3_5database[0] = EOS;
		dut_cb->dut_c8_status = DUT_NO_OBJLIST_CHANGE;
		dut_ee1_error(dut_errcb, W_DU1816_SV_CANT_REGISTER_TABLE,
			2, 0, dut_p16_register_p->dut_p3_1table);
	    }
	    else
	    {
        DUT_T3_OBJLIST *dut_t16_append_p;
		dut_cb->dut_c8_status = DUT_OBJLIST_CHANGED;
		if ((dut_cb->dut_form_objlist->dut_f3_0obj_num_entry+1) >
			dut_cb->dut_form_objlist->dut_f3_0obj_size)
		{
		    DUT_T3_OBJLIST	*tmp_t16_p;
		    DUT_T3_OBJLIST	*tmp_t16_1p;
		    DUT_T3_OBJLIST	*tmp_t16_2p;
		    i4			tmp_index;
		    /* Alloc new block */
		    dut_cb->dut_form_objlist->dut_f3_0obj_size += DUT_ME_EXTRA;
		    tmp_t16_p = (DUT_T3_OBJLIST *)MEreqmem(0,
				(dut_f16_objlist_p->dut_f3_0obj_size *
					sizeof(DUT_T3_OBJLIST)),
				TRUE, NULL);
		    /* Copy data form old to new */
		    for (tmp_index = 0, 
			tmp_t16_1p = dut_f16_objlist_p->dut_f3_9objlist,
			tmp_t16_2p = tmp_t16_p;
			tmp_index < dut_f16_objlist_p->dut_f3_0obj_num_entry;
			tmp_index++, tmp_t16_1p++, tmp_t16_2p++)
		    {
		    	MEcopy(tmp_t16_1p,
				sizeof(DUT_T3_OBJLIST),
				tmp_t16_2p);
		    }
		    /* Destroy old block */
		    MEfree(dut_cb->dut_form_objlist->dut_f3_9objlist);
		    dut_cb->dut_form_objlist->dut_f3_9objlist = tmp_t16_p;
		}
		dut_t16_append_p = (DUT_T3_OBJLIST *)
			(dut_cb->dut_form_objlist->dut_f3_9objlist +
			dut_cb->dut_form_objlist->dut_f3_0obj_num_entry);
/* # line 2580 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2582 "dutpopup.sc" */	/* host code */
		if (dut_uu2_dbop(DUT_DIRECT_CONNECT_CDB, dut_cb, dut_errcb) 
		    != E_DU_OK)
		{
		    dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT,
			2, 0, DUT_1CDB_LONG);
/* # line 2587 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE13;
        }
/* # line 2588 "dutpopup.sc" */	/* host code */
		    return(DUT_ERROR);
		}
		/* Confirm registration */
		dut_cb->dut_on_error_exit = FALSE;
/* # line 2594 "dutpopup.sc" */	/* commit */
        {
          IIsqInit(&sqlca);
          IIxact(3);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2596 "dutpopup.sc" */	/* select */
        {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct iidd_ddb_objects.object_base, lowercase(trim(iidd_ddb\
_objects.object_name)), lowercase(trim(iidd_ddb_objects.object_owner))\
, lowercase(trim(iidd_tables.table_type)), lowercase(trim(iidd_tables.\
table_subtype)), lowercase(trim(");
          IIwritio(0,(short *)0,1,32,0,(char *)
"iidd_ddb_objects.system_object)), lowercase(trim(iidd_ddb_ldbids.ldb_\
database)), lowercase(trim(iidd_ddb_ldbids.ldb_node))from iidd_ddb_obj\
ects, iidd_ddb_tableinfo, iidd_ddb_ldbids, iidd_tables where(iidd_ddb_\
objects.object_name=");
          IIputdomio((short *)0,1,32,0,dut_cb->dut_c0_buffer);
          IIwritio(0,(short *)0,1,32,0,(char *)
")and(iidd_tables.table_name=");
          IIputdomio((short *)0,1,32,0,dut_cb->dut_c0_buffer);
          IIwritio(0,(short *)0,1,32,0,(char *)
")and(iidd_ddb_objects.object_owner=");
          IIputdomio((short *)0,1,32,0,dut_cb->dut_c15_username);
          IIwritio(0,(short *)0,1,32,0,(char *)
")and(iidd_tables.table_owner=");
          IIputdomio((short *)0,1,32,0,dut_cb->dut_c15_username);
          IIwritio(0,(short *)0,1,32,0,(char *)
")and(iidd_ddb_objects.object_base=iidd_ddb_tableinfo.object_base)and(\
iidd_ddb_objects.object_index=iidd_ddb_tableinfo.object_index)and(iidd\
_ddb_ldbids.ldb_id=iidd_ddb_tableinfo.ldb_id)");
          IIretinit((char *)0,0);
          if (IInextget() != 0) {
            IIgetdomio((short *)0,1,30,4,&dut_t16_append_p->dut_t3_0objbase);
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t16_append_p->dut_t3_1objname);
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t16_append_p->dut_t3_0objowner);
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t16_append_p->dut_t3_0objtype);
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t16_append_p->dut_t3_0subtype);
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t16_append_p->dut_t3_0system);
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t16_append_p->dut_t3_0ldbname);
            IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
            dut_t16_append_p->dut_t3_0ldbnode);
          } /* IInextget */
          IIsqFlush((char *)0,0);
          if (sqlca.sqlcode < 0) 
            dut_ee5_none_starview();
        }
/* # line 2639 "dutpopup.sc" */	/* host code */
		dut_cb->dut_on_error_exit = FALSE;
		CVupper(dut_t16_append_p->dut_t3_0system);
		CVupper(dut_t16_append_p->dut_t3_0objtype);
		dut_t16_append_p->dut_t3_0delete = FALSE;
		dut_cb->dut_form_objlist->dut_f3_0obj_num_entry++;
		MEcopy(dut_t16_append_p,
			sizeof(DUT_T3_OBJLIST),
			dut_cb->dut_form_objlist->dut_f3_9objlist_curr_p);
		dut_uu11_chk_criteria(dut_cb, dut_errcb, dut_t16_append_p);
		dut_uu13_buble_sort(dut_cb->dut_form_objlist->dut_f3_9objlist,
			sizeof(DUT_T3_OBJLIST), DDB_MAXNAME, 
			dut_cb->dut_form_objlist->dut_f3_0obj_num_entry);
	    }
/* # line 2655 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE13;
        }
/* # line 2656 "dutpopup.sc" */	/* host code */
	}
	else
	{
	    /* Incomplete information for creating a Register */
	    dut_p16_register_p->dut_p3_1table[0] = EOS;
	    dut_ee1_error(dut_errcb, W_DU181A_SV_CANT_REG_FROM_CRIT, 0);
	}
      } else if (IIretval() == 2) {
/* # line 2667 "dutpopup.sc" */	/* help_frs */
        {
          IIfrshelp(0,(char *)"Distributed Database Help",
          dut_cb->dut_helpcb->dut_hp16_2name);
        }
      } else if (IIretval() == 3) {
/* # line 2676 "dutpopup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE13;
        }
      } else {
        if (1) goto IIfdE13;
      } /* IIretval */
    } /* IIrunform */
IIfdF13:
    if (IIchkfrm() == 0) goto IIfdB13;
    goto IIfdE13;
IImuI13:
    if (IIinitmu() == 0) goto IIfdE13;
    if (IInmuact((char *)"Create",(char *)0,2,2,1) == 0) goto IIfdE13;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE13;
    if (IInmuact((char *)"Help",(char *)0,2,2,2) == 0) goto IIfdE13;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,2) == 0) goto IIfdE13;
    if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE13;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE13;
    if (IIendmu() == 0) goto IIfdE13;
    goto IIfdI13;
IIfdE13:;
    IIendfrm();
  }
/* # line 2679 "dutpopup.sc" */	/* host code */
    return(E_DU_OK);
}
/*
** Name: dut_pp16_3browse_load_table	- load table information with data
**
** Description:
**	clear table field and load it with local table names.
**
** Inputs:
**
** Outputs:
**	Returns:
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      11-Aug-1989 (alexc)
**          Creation.
**	25-jul-1991 (fpang)
**	    Return E_DU_OK successful.
**
*/
dut_pp16_3browse_load_table(dut_cb, dut_errcb)
  DUT_CB *dut_cb;
  DUT_ERRCB *dut_errcb;
{
  DUT_P16_TABLE *dut_p16_table_p;
  DUT_T16_TABLE *dut_t16_table_p;
  DUT_T16_TABLE *dut_t16_table_curr_p;
  i4 dut_p16_0count;
  char dut_dbname[DDB_2MAXNAME];
    i4		dut_p16_0fill = 0;
    dut_cb->dut_on_error_noop = FALSE;
    dut_cb->dut_on_error_exit = FALSE; /* This could be a STAR or LOCAL
					* failure, that's why no exit.
					*/
    dut_p16_table_p = dut_cb->dut_popup_b_table;
    dut_t16_table_p = dut_p16_table_p->dut_p16_1table;
    dut_t16_table_curr_p = dut_t16_table_p;
    if (dut_uu2_dbop(DUT_DIRECT_CONNECT, dut_cb, dut_errcb) != E_DU_OK)
    {
        dut_ee1_error(dut_errcb, W_DU1806_SV_CANT_DIRECT_CONNECT,
			2, 0, dut_cb->dut_c0_buffer);
	return(DUT_ERROR);
    }
    dut_cb->dut_on_error_exit = TRUE;
/* # line 2738 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from iitables");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dut_p16_0count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
/* # line 2744 "dutpopup.sc" */	/* host code */
    if (dut_p16_0count > dut_p16_table_p->dut_p16_0size)
    {
	MEfree(dut_t16_table_p);
	dut_p16_table_p->dut_p16_1table = NULL;
	dut_t16_table_p = NULL;
    }
    if (dut_t16_table_p == NULL)
    {
	dut_t16_table_p = (DUT_T16_TABLE *)MEreqmem(0,
		(dut_p16_0count + DUT_ME_EXTRA) * sizeof(DUT_T16_TABLE),
		TRUE, NULL);
	dut_p16_table_p->dut_p16_1table = dut_t16_table_p;
	dut_cb->dut_popup_b_table->dut_p16_0size
		= dut_p16_0count + DUT_ME_EXTRA;
    }
/* # line 2761 "dutpopup.sc" */	/* clear */
  {
    IIfldclear((char *)"dut_p16_1table");
  }
/* # line 2763 "dutpopup.sc" */	/* host code */
    dut_t16_table_curr_p = dut_t16_table_p;
/* # line 2765 "dutpopup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select trim(table_name), trim(table_owner), trim(table_type), trim(sy\
stem_use)from iitables order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_t16_table_curr_p->dut_t16_1table);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_t16_table_curr_p->dut_t16_2own);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_t16_table_curr_p->dut_t16_3type);
      IIgetdomio((short *)0,1,32,DDB_MAXNAME-1,
      dut_t16_table_curr_p->dut_t16_4system);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 2779 "dutpopup.sc" */	/* host code */
	switch (dut_t16_table_curr_p->dut_t16_3type[0])
	{
	    case 'I':
		STcopy(DUT_M26_INDEX, dut_t16_table_curr_p->dut_t16_3type);
		break;
	    case 'V':
		STcopy(DUT_M24_VIEW, dut_t16_table_curr_p->dut_t16_3type);
		break;
	    case 'T':
		STcopy(DUT_M22_TABLE, dut_t16_table_curr_p->dut_t16_3type);
		break;
	}
	switch (dut_t16_table_curr_p->dut_t16_4system[0])
	{
	    case 'S':
		STcopy(DUT_M38_SYSTEM,
			dut_t16_table_curr_p->dut_t16_4system);
		break;
	    case 'U':
		STcopy(DUT_M39_USER,
			dut_t16_table_curr_p->dut_t16_4system);
		break;
	}
	/* Do not list objects that are system or Index */
	if (STcompare(dut_t16_table_curr_p->dut_t16_4system,
			DUT_M38_SYSTEM) &&
	    STcompare(dut_t16_table_curr_p->dut_t16_3type,
			DUT_M26_INDEX))
	{
/* # line 2809 "dutpopup.sc" */	/* loadtable */
      {
        if (IItbact((char *)"dut_p16_table",(char *)"dut_p16_1table",1) != 0) 
        {
          IItcoputio((char *)"dut_t16_1table",(short *)0,1,32,0,
          dut_t16_table_curr_p->dut_t16_1table);
          IItcoputio((char *)"dut_t16_2own",(short *)0,1,32,0,
          dut_t16_table_curr_p->dut_t16_2own);
          IItcoputio((char *)"dut_t16_3type",(short *)0,1,32,0,
          dut_t16_table_curr_p->dut_t16_3type);
          IItcoputio((char *)"dut_t16_4system",(short *)0,1,32,0,
          dut_t16_table_curr_p->dut_t16_4system);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 2820 "dutpopup.sc" */	/* host code */
	    dut_p16_0fill++;
	    dut_t16_table_curr_p++;
	}
	if (sqlca.sqlcode < 0)
	{
	    dut_cb->dut_c8_status = DUT_ERROR;
/* # line 2826 "dutpopup.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE6;
        if (sqlca.sqlcode < 0) 
          dut_ee5_none_starview();
      }
/* # line 2827 "dutpopup.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
    if (sqlca.sqlcode < 0) 
      dut_ee5_none_starview();
  }
    dut_cb->dut_on_error_exit = FALSE;
    dut_p16_table_p->dut_p16_0num_entry = dut_p16_0fill;
    return (E_DU_OK);
}
