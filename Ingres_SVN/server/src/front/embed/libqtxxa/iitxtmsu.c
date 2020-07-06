# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include       <compat.h>
# include       <gl.h>
# include       <sl.h>
# include       <er.h>
# include       <iicommon.h>
# include       <me.h>
# include       <st.h>
# include       <si.h>
# include	<xa.h>
# include       <iicx.h>
# include       <iicxfe.h>
# include       <iicxxa.h>
# include       <iicxfe.h>
# include	<iixagbl.h>
# include	<iixamain.h>
# include       <generr.h>
# include       <erlq.h>
# include	<eqrun.h>		/* for IIsqconSysGen */
/* TUXEDO specific headers */
# include       <iitxgbl.h>
# include       <iitxtms.h>
# include	<iitxtmsu.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
static VOID IItux_sqlerror();
static bool table_found = FALSE;
/*
**  Name:  iitxtmsu.c - TUXEDO TMS utility functions.
**
**  Description:
**	This file contains utility routines necessary to support TUXEDO TMS
**	functionality.  The routines in this file are called from
**	from iitxtms.c (front!embed!libqtxxa).
**
**  Defines:
**	IItux_build_rec_action_table	- build a list of willing-commit
**					  transactions that need to be
**					  committed or aborted to get the
**					  database into a state consistent
**					  with TUXEDO's understanding of it. 
**	IItux_tms_recover_xid		- recovers one XID by trying to 
**					  reconnect with DB and committing/
**					  rolling back.
**	IItux_shm_locktbl		- check existence of 'tuxedo' table,
**					  create if copy owned by this user
**					  is not present.
**  History:
**	08-nov-1993 (larrym)
**	    written.
**	16-nov-1993 (larrym)
**	    moved IItux_build_as_service_name to iitxutil.c
**	01-dec-1993 (larrym)
**	    changed interface.  iitux_xid_seqnum_view is now a (char **)
**	    Tuxedo can change the address of the buffer whenever it wants
**	    so we have to pass that (possibly) new address back to the
**	    caller
**	22-dec-1993 (larrym)
**	    added IItux_build_rec_action_list to support recovery.
**	07-Jan-1994 (mhughes) 
**	    Added debug mode. Stash tpcall flags in tuxedo global cb.
**	12-Jan-1994 (mhughes)
**	    Modified tpcall to send tpcall & tprply flags.
**	31-Jan-1994 (larrym)
**	    Added IItux_tms_recover_xid;
**      01-Jan-1995 (stial01)
**          Deleted functions not needed for new INGRES-TUXEDO 
**          architecture.
**	25-march-1997(angusm)
**	    as part of changes w.r. bug 79829, add IItux_shm_locktbl().
**	    This should offer help for bug 78444 as well.
**      11-nov-98 (stial01)
**          Minor changes to IItux_shm_lock queries.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
**  Name:	IItux_build_rec_action_table
**
**  Description:
**	build a list of willing-commit transactions that need to be committed 
**	or aborted to get the database into a state consistent with TUXEDO's 
**	understanding of it. 
**
**  Inputs:
**	rmi			- the RMI to recover 
**
**  Outputs:
**	iitux_tms_xid_action    - an array of structures containing a table
**				  of INGRES transactions that need to be
**				  committed or aborted.
**	rec_count		- The number of elements in iitux_tms_xid_action
**
**      Returns:
**          XA_OK          	- normal execution.
**          XAER_RMERR     	- input args invalid
**
**  Side Effects:
**      None.
**
**  Notes:
**	Refer to the TUXEDO Bridge Arch and Design spec for the algorithm
**	description and rationale.
**
**  History:
**	06-Jan-1994 (larrym)
**	    written.
**	31-jan-1994 (larrym)
**	    added IITUX_TMS_2P_ACTION_LIST to follow FE model of memory 
**	    allocation.
**	09-Mar-1994 (larrym)
**	    Modified to call IIxa_connect_to_iidbdb (and disconnect)
**	11-mar-1994 (larrym)
**	    fixed bug 60547; added null indicators to select statements.
*/
int
IItux_build_rec_action_table(
                       	IICX_XA_RMI_CB		  *xa_rmi_cb_p,
 			IITUX_TMS_2P_ACTION_LIST  *iitux_tms_2P_action_list,
                       	i4			  *rec_count)
{
  char *dbname_p;
  char xa_dis_tran_id[IIXA_XID_STRING_LENGTH];
  i4 sum_flags;
  i4 min_seqnum;
  i4 count_seqnum;
  i4 max_seqnum;
  i4 errorno;
  i2 dummy_null_ind;
i4			old_ingres_state;
IICX_CB         	*fe_thread_cx_cb_p;
IICX_FE_THREAD_CB  	*fe_thread_cb_p;
DB_STATUS		db_status;
int			xa_ret_value;
i4			i;
XID			cur_xid;
IITUX_TMS_2P_ACTION	*cur_action_p;
    *rec_count = 0;
    db_status = IICXget_lock_fe_thread_cb( NULL, &fe_thread_cx_cb_p );
    if (DB_FAILURE_MACRO(db_status))
    {
	return( XAER_RMERR );
    }
    fe_thread_cb_p =
	     fe_thread_cx_cb_p->cx_sub_cb.fe_thread_cb_p;
    /* connect to the iidbdb */
    xa_ret_value = IIxa_connect_to_iidbdb(
	                fe_thread_cb_p,
			xa_rmi_cb_p,
			&dbname_p);
    if (xa_ret_value != XA_OK)
    {
        return (xa_ret_value);
    }
    old_ingres_state = fe_thread_cb_p->ingres_state;
    fe_thread_cb_p->ingres_state = IICX_XA_T_ING_INTERNAL;
	/* start select loop based on dbname, ordered by XID,seqnum */
/* # line 176 "iitxtmsu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(xa_dis_tran_id), sum(xa_flags), min(xa_seqnum), count(xa_\
seqnum), max(xa_seqnum)from lgmo_xa_dis_tran_ids where xa_database_nam\
e=");
    IIputdomio((short *)0,1,32,0,dbname_p);
    IIwritio(0,(short *)0,1,32,0,(char *)"group by xa_dis_tran_id");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio(&dummy_null_ind,1,32,IIXA_XID_STRING_LENGTH-1,xa_dis_tran_id);
      IIgetdomio(&dummy_null_ind,1,30,4,&sum_flags);
      IIgetdomio(&dummy_null_ind,1,30,4,&min_seqnum);
      IIgetdomio((short *)0,1,30,4,&count_seqnum);
      IIgetdomio(&dummy_null_ind,1,30,4,&max_seqnum);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 190 "iitxtmsu.sc" */	/* host code */
	    /* do some validation */
	    if ( ((min_seqnum + count_seqnum) == (max_seqnum + 1))
	       &&(sum_flags != DB_XA_EXTD_BRANCH_FLAG_NOOP))
	    {
		/* everything is cool with this XID */
		/* check to see if the pool is empty */
		if (iitux_tms_2P_action_list->action_pool == NULL)
		{
		    /* fill up the pool */
		    if ((cur_action_p = (IITUX_TMS_2P_ACTION *) 
			   FEreqmem(iitux_tms_2P_action_list->action_tag,
			   (u_i4)(IITUX_ACTION_ALLOC * sizeof (*cur_action_p)),
			   TRUE, (STATUS *)NULL)) == NULL)
		    {
			/* CHECK error */
			return (XAER_RMERR);
		    }
		    iitux_tms_2P_action_list->action_pool = cur_action_p;
		    /* format the pool into a linked list */
		    for( i = 1; i < IITUX_ACTION_ALLOC; i++, cur_action_p++)
		    {
			cur_action_p->action_next = cur_action_p + 1;
		    }
		    cur_action_p->action_next = NULL;	/* end of list */
		}
		/* snag one out of the pool */
		cur_action_p = iitux_tms_2P_action_list->action_pool;
		iitux_tms_2P_action_list->action_pool = 
				    cur_action_p->action_next;
		/* and put it on the list */
		cur_action_p->action_next = 
				    iitux_tms_2P_action_list->action_list;
		iitux_tms_2P_action_list->action_list = cur_action_p;
		/* now fill in the goodies */
		/* copy the XID */
                db_status = 
		    IICXconv_to_struct_xa_xid( xa_dis_tran_id,
                    (i4) STlength( xa_dis_tran_id ),
                    (DB_XA_DIS_TRAN_ID *)&(cur_action_p->xid),
                    (i4 *)0,
                    (i4 *)0);
		if (db_status != E_DB_OK)
		{
		    /* CHECK error */
		    /* caller will free the list */
		    return(XAER_RMERR);
		}
		cur_action_p->first_seq = min_seqnum;
		cur_action_p->last_seq = max_seqnum;
		cur_action_p->tp_flags = sum_flags;
		(*rec_count)++;
	        if( IIcx_fe_thread_main_cb->fe_main_flags & 
                    IICX_XA_MAIN_TRACE_XA)
		{
		    char	outbuf[256];
		    STprintf(outbuf, 
			ERx("\tConsistency verified for %s\n"),
			xa_dis_tran_id); 
		    IIxa_fe_qry_trace_handler( (PTR) 0, outbuf, TRUE);
		    STprintf(outbuf, 
			ERx("\tFirst = %d, Last = %d, tp_flags = %2x\n"),
			min_seqnum, max_seqnum, sum_flags); 
		    IIxa_fe_qry_trace_handler( (PTR) 0, outbuf, TRUE);
		}
	    }
	    else
	    {
		/* Could be an Encina/CICS xid, or ERROR CHECK */
	        if( IIcx_fe_thread_main_cb->fe_main_flags & 
                    IICX_XA_MAIN_TRACE_XA)
		{
		    char	outbuf[256];
		    STprintf(outbuf, 
			ERx("\tINCONSISTENT XID %s\n"), xa_dis_tran_id); 
		    IIxa_fe_qry_trace_handler( (PTR) 0, outbuf, TRUE);
		}
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 278 "iitxtmsu.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errorno,2);
  }
/* # line 279 "iitxtmsu.sc" */	/* host code */
	if (errorno == 0)
	{
	    xa_ret_value = XA_OK;
	}
	else
	{
	    /* CHECK */
	    xa_ret_value = XAER_RMERR;
	}
	/* close up shop */
/* # line 290 "iitxtmsu.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 292 "iitxtmsu.sc" */	/* host code */
        fe_thread_cb_p->ingres_state = old_ingres_state;
        IICXunlock_cb( fe_thread_cx_cb_p );
	xa_rmi_cb_p->rec_sid = 0;
        IICXunlock_cb( rmi_cx_cb_p );
	return (xa_ret_value);
}
/*
**  Name:	IItux_tms_recover_xid
**
**  Description:
**	based on the given XID,RMID, find all outstanding willing-commit
**	transactions that need to be committed or aborted to get the 
**	database into a state consistent with TUXEDO's understanding of it. 
**	This module is called when TUXEDO wants us to recover a specific
**	XID.
**
**  Inputs:
**	xa_call			- should be COMMIT or ROLLBACK
**	xa_xid			- the XID to recover
**	rmi			- the RMI to recover 
**
**  Outputs:
**
**      Returns:
**          XA_OK          	- normal execution.
**	    XAER_NOTA		- nothing to recover
**          XAER_RMERR     	- input args invalid
**
**  Side Effects:
**      None.
**
**  History:
**	31-Jan-1994 (larrym)
**	    written.
**	09-Mar-1994 (larrym)
**	    Modified to call IIxa_connect_to_iidbdb (and disconnect)
**	11-mar-1994 (larrym)
**	    fixed bug 60547; added null indicators to select statements.
**	10-May-1994 (mhughes)
**	    We were setting tp_flags wrong for reattach to thread.
*/
int
IItux_tms_recover_xid( i4  xa_call, XID *xa_xid, int rmid)
{
  char *dbname_p;
  char xid_str[IIXA_XID_STRING_LENGTH];
  i4 sum_flags;
  i4 min_seqnum;
  i4 count_seqnum;
  i4 current_rec_session;
  i4 save_rec_session;
  i4 rec_session;
  char iidbdbname[64];
  i4 max_seqnum;
  i4 errorno;
  i2 dummy_null_ind;
i4			seqnum;
i4			tp_flags;
IICX_ID			rmi_cx_id;
IICX_CB			*rmi_cx_cb_p;
IICX_CB         	*fe_thread_cx_cb_p;
IICX_FE_THREAD_CB  	*fe_thread_cb_p;
IICX_XA_RMI_CB    	*xa_rmi_cb_p;
DB_STATUS		db_status;
int			xa_ret_value = XA_OK;
i4			old_ingres_state;
char    		*nodename_p, *iidbdbname_p;
    /* convert XID to xid_str */
    IICXformat_xa_xid( (DB_XA_DIS_TRAN_ID *)xa_xid, (char *)xid_str );
    /* get dbname from rmi, based on rmid passed in */
    IICXsetup_xa_rmi_id_MACRO( rmi_cx_id, rmid );
    db_status = IICXfind_lock_cb( &rmi_cx_id, &rmi_cx_cb_p );
    if (DB_FAILURE_MACRO(db_status))
	return( XAER_RMERR );
    if (rmi_cx_cb_p == NULL)
    {
	/* error CHECK */
	return (XAER_RMERR);
    }
    xa_rmi_cb_p = rmi_cx_cb_p->cx_sub_cb.xa_rmi_cb_p;
    db_status = IICXget_lock_fe_thread_cb( NULL, &fe_thread_cx_cb_p );
    if (DB_FAILURE_MACRO(db_status))
    {
	return( XAER_RMERR );
        IICXunlock_cb( rmi_cx_cb_p );
    }
    fe_thread_cb_p =
	     fe_thread_cx_cb_p->cx_sub_cb.fe_thread_cb_p;
    old_ingres_state = fe_thread_cb_p->ingres_state;
    fe_thread_cb_p->ingres_state = IICX_XA_T_ING_INTERNAL;
    /* connect to the iidbdb */
    xa_ret_value = IIxa_connect_to_iidbdb(
	                fe_thread_cx_cb_p->cx_sub_cb.fe_thread_cb_p,
			xa_rmi_cb_p,
			&dbname_p);
    if (xa_ret_value != XA_OK)
    {
	return (xa_ret_value);
    }
    /* build action table entry based on SELECT */
/* # line 405 "iitxtmsu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select sum(xa_flags), min(xa_seqnum), count(xa_seqnum), max(xa_seqnum\
)from lgmo_xa_dis_tran_ids where xa_database_name=");
    IIputdomio((short *)0,1,32,0,dbname_p);
    IIwritio(0,(short *)0,1,32,0,(char *)"and xa_dis_tran_id=");
    IIputdomio((short *)0,1,32,0,xid_str);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio(&dummy_null_ind,1,30,4,&sum_flags);
      IIgetdomio(&dummy_null_ind,1,30,4,&min_seqnum);
      IIgetdomio((short *)0,1,30,4,&count_seqnum);
      IIgetdomio(&dummy_null_ind,1,30,4,&max_seqnum);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 416 "iitxtmsu.sc" */	/* host code */
            /* GROUP BY    xa_dis_tran_id;	to make sure we get one row */
    /* now loose the connection */
/* # line 419 "iitxtmsu.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 420 "iitxtmsu.sc" */	/* host code */
    xa_rmi_cb_p->rec_sid = 0;
    fe_thread_cb_p->ingres_state = old_ingres_state;
    IICXunlock_cb( rmi_cx_cb_p );
    IICXunlock_cb( fe_thread_cx_cb_p );
    /* if nothing to recover, return XAER_NOTA */
    if( count_seqnum == 0 )
    {
	return (XAER_NOTA);
    }
    /* validate ? */
    /* switch xa_call */
    if (xa_call == IIXA_COMMIT_CALL)
    {
	for (seqnum = max_seqnum; seqnum >= min_seqnum && 
				  xa_ret_value == XA_OK; seqnum-- )
	{
	    /* for now, have to use the same tp_flag */
	    tp_flags = DB_XA_EXTD_BRANCH_FLAG_NOOP;
	    if (seqnum == max_seqnum)
	    {
		if (sum_flags & DB_XA_EXTD_BRANCH_FLAG_LAST)
		{
		    tp_flags |= DB_XA_EXTD_BRANCH_FLAG_LAST;
		}
		if ( seqnum == 0 )
		{
		    if ( sum_flags & DB_XA_EXTD_BRANCH_FLAG_FIRST )
			tp_flags |= DB_XA_EXTD_BRANCH_FLAG_FIRST;
		    if ( sum_flags & DB_XA_EXTD_BRANCH_FLAG_2PC )
			tp_flags |= DB_XA_EXTD_BRANCH_FLAG_2PC;
		}
	    }
	    else if (seqnum == 0)
	    {
		if ( max_seqnum > 0 )
		    tp_flags = (sum_flags & ~DB_XA_EXTD_BRANCH_FLAG_LAST);
		else
		    tp_flags = sum_flags;
	    }
	    xa_ret_value = IIxa_do_2phase_operation(
			       IIXA_COMMIT_CALL,
			       xa_xid,
			       rmid,
			       TMNOFLAGS,
			       seqnum,
			       tp_flags);
	}
    }
    else if (xa_call == IIXA_ROLLBACK_CALL)
    {
	/* CHECK, this should be optimized */
	for (seqnum = min_seqnum; seqnum <= max_seqnum && 
				  xa_ret_value == XA_OK; seqnum++ )
	{
	    /* for now, have to use the same tp_flag */
	    tp_flags = DB_XA_EXTD_BRANCH_FLAG_NOOP;
	    if (seqnum == max_seqnum)
	    {
		if (sum_flags & DB_XA_EXTD_BRANCH_FLAG_LAST)
		{
		    tp_flags |= DB_XA_EXTD_BRANCH_FLAG_LAST;
		}
		if ( seqnum == 0 )
		{
		    if ( sum_flags & DB_XA_EXTD_BRANCH_FLAG_FIRST )
			tp_flags |= DB_XA_EXTD_BRANCH_FLAG_FIRST;
		    if ( sum_flags & DB_XA_EXTD_BRANCH_FLAG_2PC )
			tp_flags |= DB_XA_EXTD_BRANCH_FLAG_2PC;
		}
	    }
	    else if (seqnum == 0)
	    {
		if ( max_seqnum > 0 )
		    tp_flags = (sum_flags & ~DB_XA_EXTD_BRANCH_FLAG_LAST);
		else
		    tp_flags = sum_flags;
	    }
	    xa_ret_value = IIxa_do_2phase_operation(
			       IIXA_ROLLBACK_CALL,
			       xa_xid,
			       rmid,
			       TMNOFLAGS,
			       seqnum,
			       tp_flags);
	}
    }
    return (xa_ret_value);
}
/*{
**  Name: IItux_shm_lock  - Obtain EXCL lock on table 'tuxedo' in iidbdb.
**
**  Description: 
**      Obtain an EXCL lock on table 'tuxedo' in the iidbdb database.
**      We do this by setting lockmode = table and declaring an update cursor. 
**      This is how we control access to the shared memory when an AS or TMS
**      server creates/attaches, detaches/destroys the shared memory segment
**      created to coordinate transaction management.
**
**      NOTE: We are intentionally connecting to the iidbdb of the
**            server in this group. All servers in the installation 
**            will attach to the same shared memory segment.
**
**  Inputs:
**      None.
**
**  Outputs:
**	Returns:
**          OK             - lock obtained.
**          FAIL           - lock not obtained.
**
**	Errors:
**	    None.
**
**  Side Effects:
**	    None.
**	
**  History:
**      01-jan-1996  -  Written (stial01,thaju02)
**	25-march-1997(angusm)
**	Modify so that if we are creating per-user shared memory
**	segments, we create (where neccessary) versions of 'tuxedo'
**	table owned by different users. (bug 79829).
**      11-nov-98 (stial01)
**          Minor changes to IItux_shm_lock queries.
*/
STATUS
IItux_shm_lock()
{
  i4 cnt;
  char username[DB_MAXNAME +1];
  char buf[100];
        STATUS  s;
	char    *pusername=username;
/* # line 571 "iitxtmsu.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,(char *)"iidbdb",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 573 "iitxtmsu.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	IItux_sqlerror();
	return(FAIL);
    }
    if (sqlca.sqlcode < 0)
    {
	i4      sqlcode;
	char    *errm;
	i2      errl;
        char    ebuf[500];
	sqlcode = sqlca.sqlcode;
	errm = sqlca.sqlerrm.sqlerrmc;
	errl = sqlca.sqlerrm.sqlerrml;
	STprintf(ebuf, 
	    ERx(" %x %s"), sqlcode, errm);
        IIxa_error( GE_LOGICAL_ERROR, E_LQ00D0_XA_INTERNAL, 1, ebuf);
	return(FAIL);
    }
    IDname(&pusername);
    if (table_found == FALSE)
    {
	s = IItux_shm_locktbl(username);
    	if (s != OK)
      	    return(FAIL);
	table_found = TRUE;
    }
/* # line 604 "iitxtmsu.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set lockmode session where readlock=exclusive, level=table");
    IIsyncup((char *)0,0);
  }
/* # line 606 "iitxtmsu.sc" */	/* host code */
    (VOID)STprintf(buf, "select count(*) from %s.tuxedo", username);
/* # line 608 "iitxtmsu.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	IItux_sqlerror();
	return(FAIL);
    }
/* # line 614 "iitxtmsu.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"c1",7074,15603);
    IIwritio(1,(short *)0,1,32,0,buf);
    IIcsQuery((char *)"c1",7074,15603);
  }
/* # line 615 "iitxtmsu.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	IItux_sqlerror();
	return(FAIL);
    }
    return(OK);
}   /* IItux_shm_lock */
/*{
**  Name: IItux_shm_unlock  - Release EXCL lock on table 'tuxedo' in iidbdb.
**
**  Description: 
**      Release EXCL lock on table 'tuxedo' in iidbdb. We do this by 
**      disconnecting from the iidbdb database.
**
**  Inputs:
**      None.
**
**  Outputs:
**	Returns:
**          OK             - lock released.
**          FAIL           - lock not released.
**
**	Errors:
**	    None.
**
**  Side Effects:
**	    None.
**	
**  History:
**      01-jan-1996  -  Written (stial01,thaju02)
*/
STATUS
IItux_shm_unlock()
{
/* # line 651 "iitxtmsu.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 652 "iitxtmsu.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	IItux_sqlerror();
	return(FAIL);
    }
    return(OK);
}  /* IItux_shm_unlock */
/*{
**  Name: IItux_shm_locktbl  - check existence of table 'tuxedo' in iidbdb.
**
**  Description: 
**      Check if table 'tuxedo' exists in iidbdb owned by this users.
**      Create it if not.
**
**  Inputs:
**      None.
**
**  Outputs:
**	Returns:
**          OK             - table present or created successfully.
**          FAIL           - unable to create table.
**
**  Side Effects:
**      None.
**
**  History:
**	25-march-1997 (angusm)
**	Created
*/
STATUS 
IItux_shm_locktbl(char *username)
{
  int tcount;
  char *uname;
    uname = username;
    tcount = 0;
/* # line 693 "iitxtmsu.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iirelation where relid='tuxedo' and relowner=");
    IIputdomio((short *)0,1,32,0,uname);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tcount);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 698 "iitxtmsu.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
/* # line 700 "iitxtmsu.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 701 "iitxtmsu.sc" */	/* host code */
        if (sqlca.sqlcode < 0)
	{
	    IItux_sqlerror();
	    return(FAIL);
        }
    }
    if (tcount == 1)
    {
/* # line 709 "iitxtmsu.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 710 "iitxtmsu.sc" */	/* host code */
        if (sqlca.sqlcode < 0)
	{
	    IItux_sqlerror();
	    return(FAIL);
        }
	return(OK);
    }
    else
    {
/* # line 719 "iitxtmsu.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table tuxedo(a integer not null)");
    IIsyncup((char *)0,0);
  }
/* # line 720 "iitxtmsu.sc" */	/* host code */
        if (sqlca.sqlcode < 0)
	{
	    IItux_sqlerror();
	    return(FAIL);
        }
/* # line 726 "iitxtmsu.sc" */	/* grant */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"grant all on tuxedo to publi\
c");
    IIsyncup((char *)0,0);
  }
/* # line 727 "iitxtmsu.sc" */	/* host code */
        if (sqlca.sqlcode < 0)
	{
	    IItux_sqlerror();
	    return(FAIL);
        }
/* # line 733 "iitxtmsu.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 734 "iitxtmsu.sc" */	/* host code */
        if (sqlca.sqlcode < 0)
	{
	    IItux_sqlerror();
	    return(FAIL);
        }
    }
    return(OK);
}/* IItux_shm_locktbl */
static VOID
IItux_sqlerror()
{
	i4      sqlcode;
	char    *errm;
	i2      errl;
        char    ebuf[500];
	sqlcode = sqlca.sqlcode;
	errm = sqlca.sqlerrm.sqlerrmc;
	errl = sqlca.sqlerrm.sqlerrml;
	STprintf(ebuf, 
	    ERx(" %x %s"), sqlcode, errm);
        IIxa_error( GE_LOGICAL_ERROR, E_LQ00D0_XA_INTERNAL, 1, ebuf);
}
