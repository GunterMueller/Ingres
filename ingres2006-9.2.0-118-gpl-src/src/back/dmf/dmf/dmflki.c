/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cm.h>
#include    <er.h>
#include    <tm.h>
#include    <cs.h>
#include    <tr.h>
#include    <me.h>
#include    <pc.h>
#include    <si.h>
#include    <lo.h>
#include    <nm.h>
#ifdef	VMS
#include    <descrip.h>
#include    <ssdef.h>
#include    <lkidef.h>
#include    <starlet.h>
#include    <vmstypes.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lk.h>
#include    <lg.h>
#include    <adf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <scf.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmpp.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0llctx.h>
#include    <dmve.h>
#include    <dmfcsp.h>
#include    <dmfrcp.h>
#include    <dmfacp.h>
/*
**
**  Name: DMFLKI.C - wrapper for sys$getlki
**
**  Description:
**	Print out information about VMS locks gathered from a getlki call.
**	It is helpful to look at the VMS system services reference manual
**	entry fro sys$getlki. 
**
**  History:    
**      26-apr-1993 (andys)
**          Created. 
**	    Sources included dmfcsp.c, lockstat.c and BryanP's dlmbug.c.
**	24-may-1993 (bryanp)
**	    This file is built only on VMS, not on any other machines. We
**	    don't want Unix machines even to attempt to build this file. But
**	    the "mkming" utility used by our build scripts sees the "main" at
**	    the start of the line and tries to build the file. To get around
**	    this, I've inserted whitespace in front of the "main"; since
**	    mkming searches for "^main" as its pattern, the whitespace makes
**	    it think that this is NOT a main file, so it doesn't try to build
**	    an executable program out of it (it just compiles an empty source
**	    file, which is acceptable).
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	18-june-1993 (shailaja)
**	    Fixed compiler warning on empty translation unit.
**	31-aug-1995 (dougb)
**	    Separate retrieval of lock value -- may not be authorized.  Print
**	    list of all locks on this resource.
**      22-nov-96 (dilma04)
**          Row Locking Project:
**	    Add LK_PH_PAGE for row level locking project.
**	16-feb-1999 (nanpr01)
**	    Support for update mode lock.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	17-sep-2003 (abbjo03)
**	    Changes to use VMS headers provided by HP.
*/

typedef struct _LKI_SIZE
{
    u_i4	length:16;
    u_i4	width:15;
    u_i4	overflow:1;
}   LKI_SIZE;	    


/*
** Name: main		    - main routine for dmflki
**
** History:
**	24-may-1993 (bryanp)
**	    Insert whitespace in front of "main" to fool mkming.
*/
int

/* */    main(

int	    argc, 
char	    *argv[])

{
    STATUS              status = FAIL;
    CL_ERR_DESC          sys_err;
    u_i4		lockid;

    MEadvise(ME_INGRES_ALLOC);

    if (status = TRset_file(TR_T_OPEN, TR_OUTPUT, TR_L_OUTPUT, &sys_err))
    {
	TRdisplay("bad TRset_file, status 0x%x\n", status);
	PCexit(1);
    }
    if (argc != 2)
    {
	TRdisplay("Usage: dmflki lockid\n");
	PCexit(1);
    }
    if (status = CVahxl(argv[1], &lockid))
    {
	TRdisplay("bad lockid %s, CVahxl status 0x%x\n", argv[1], status);
	PCexit(1);
    }
    get_dlm(lockid);
    return OK;
}

STATUS
get_dlm(u_i4 lockid)
{
    STATUS	    status;
    i4	    i;
    i4	    out_count;
    LKIDEF	    lki_blocked[64];
    LKIDEF	    lki_blocking[64];
    LKIDEF	    lki_fullist [128];
    LK_LOCK_KEY	    lk_lock;
    LK_LOCK_KEY *   lkp = &lk_lock;
    LKI_SIZE	    lki_size;
    LKI_SIZE	    lki_bed_size;
    LKI_SIZE	    lki_bng_size;
    LKI_SIZE	    lki_full_size;
    CL_ERR_DESC	    sys_err;
    i4	    lki_csid;
    LKI_SIZE	    lki_csid_len;
    i4	    lki_mstcsid;
    LKI_SIZE	    lki_mstcsid_len;
    i4	    lki_mstlkid;
    LKI_SIZE	    lki_mstlkid_len;
    i4	    lki_cvtcount;
    LKI_SIZE	    lki_cvtcount_len;
    i4	    lki_grantcount;
    LKI_SIZE	    lki_grantcount_len;
    i4	    lki_lckrefcnt;
    LKI_SIZE	    lki_lckrefcnt_len;
    i4	    lki_lkid;
    LKI_SIZE	    lki_lkid_len;
    i4	    lki_lockid;
    LKI_SIZE	    lki_lockid_len;
    NAMSPACE	    lki_namspace;
    LKI_SIZE	    lki_namspace_len;
    i4	    lki_parent;
    LKI_SIZE	    lki_parent_len;
    i4	    lki_pid;
    LKI_SIZE	    lki_pid_len;
    i4	    lki_rsbrefcnt;
    LKI_SIZE	    lki_rsbrefcnt_len;
    i4	    lki_waitcount;
    LKI_SIZE	    lki_waitcount_len;
    LKI_SIZE	    lki_state_len;
    STATEF	    lki_state;
    i4	    lki_vms_valblk[4];
    LKI_SIZE	    lki_vms_valblk_len;
    II_VMS_ITEM_LIST_3	lki_item_list[] =
    {
	{ sizeof(lki_blocked), LKI$_BLOCKEDBY, &lki_blocked, &lki_bed_size }, 
	{ sizeof(lki_blocking), LKI$_BLOCKING, &lki_blocking, &lki_bng_size },
	{ sizeof lki_fullist, LKI$_LOCKS, &lki_fullist, &lki_full_size },
	{ sizeof(lki_csid), LKI$_CSID, &lki_csid, &lki_csid_len },
	{ sizeof(lki_cvtcount), LKI$_CVTCOUNT, &lki_cvtcount, &lki_cvtcount_len },
	{ sizeof(lki_grantcount), LKI$_GRANTCOUNT, &lki_grantcount, &lki_grantcount_len },
	{ sizeof(lki_lckrefcnt), LKI$_LCKREFCNT, &lki_lckrefcnt, &lki_lckrefcnt_len },
	{ sizeof(lki_lkid), LKI$_LKID, &lki_lkid, &lki_lkid_len },
	{ sizeof(lki_lockid), LKI$_LOCKID, &lki_lockid, &lki_lockid_len },
	{ sizeof(lki_mstcsid), LKI$_MSTCSID, &lki_mstcsid, &lki_mstcsid_len },
	{ sizeof(lki_mstlkid), LKI$_MSTLKID, &lki_mstlkid, &lki_mstlkid_len },
	{ sizeof(lki_namspace), LKI$_NAMSPACE, &lki_namspace, &lki_namspace_len },
	{ sizeof(lki_parent), LKI$_PARENT, &lki_parent, &lki_parent_len },
	{ sizeof(lki_pid), LKI$_PID, &lki_pid, &lki_pid_len },
	{ sizeof(lk_lock), LKI$_RESNAM, lkp, &lki_size }, 
	{ sizeof(lki_rsbrefcnt), LKI$_RSBREFCNT, &lki_rsbrefcnt, &lki_rsbrefcnt_len },
	{ sizeof(lki_state), LKI$_STATE, &lki_state, &lki_state_len },
	{ sizeof(lki_waitcount), LKI$_WAITCOUNT, &lki_waitcount, &lki_waitcount_len },
	{ 0, 0, 0, 0}
    };
    II_VMS_ITEM_LIST_3	lki_item_list2[] =
    {
	{ sizeof(lki_vms_valblk), LKI$_VALBLK, lki_vms_valblk, &lki_vms_valblk_len },
	{ 0, 0, 0, 0}
    };


    status = sys$getlkiw(0, &lockid, lki_item_list, 0, 0, 0, 0);
    if ((status & 1) == 0)
    {
	fail_msg(status);
	return FAIL ;	    
    }
    status = sys$getlkiw( 0, &lockid, lki_item_list2, 0, 0, 0, 0 );
    if ( 0 == ( 1 & status ))
    {
	if ( SS$_ILLRSDM == status )
	{
	    MEfill( sizeof lki_vms_valblk, 0, (PTR)&lki_vms_valblk );
	}
	else
	{
	    fail_msg(status);
	    return FAIL ;	    
	}
    }

    TRdisplay("VMSLock Info: VMS LKID:%x VMS Value:%d.%d.%d.%d\n",
	lockid,
	lki_vms_valblk[0], lki_vms_valblk[1],
	lki_vms_valblk[2], lki_vms_valblk[3]);
    TRdisplay("    CSID:%x MCSID:%x MLKID:%x cCOUNT:%d gCOUNT:%d wCOUNT:%d\n",
	lki_csid, lki_mstcsid, lki_mstlkid, lki_cvtcount, lki_grantcount,
	lki_waitcount);
    TRdisplay("    lrCOUNT:%d LKID:%x LOCKID:%x\n",
	lki_lckrefcnt, lki_lkid, lki_lockid); 
    TRdisplay("    NAMSP: UICGRP %x ACCM %x SYS=%d PARENT:%x PID:%x\n",
	lki_namspace.lki$w_group, 
	lki_namspace.lki$b_rmod, 
	lki_namspace.lki$v_sysnam, 
	lki_parent, lki_pid);
    TRdisplay("    rCOUNT:%d STATE: req %.1w grn %.1w QUE %d\n",
	lki_rsbrefcnt, 
	"N,IS,IX,S,SIX,U,X", 
	lki_state.lki$b_state_rqmode, 
	"N,IS,IX,S,SIX,U,X", 
	lki_state.lki$b_state_grmode, 
	lki_state.lki$b_state_queue);

    TRdisplay("    RESNAME= KEY(%w",
	",DATABASE,TABLE,PAGE,EXTEND,SV_PAGE,CREATE,NAME,CONFIG,\
DB_TBL_ID,SV_DATABASE,SV_TABLE,EVENT,CONTROL,JOURNAL,OPEN_DB,CKP_DB,\
CKP_CLUSTER,BM_LOCK,BM_DATABASE,BM_TABLE,SYS_CONTROL,EV_CONNECT,AUDIT,ROW",
	lkp->lk_type);

    switch(lkp->lk_type)
    {
    case LK_DATABASE:
    case LK_SV_DATABASE:
    case LK_OWNER_ID:
    case LK_JOURNAL:
    case LK_DB_TEMP_ID:
    case LK_CONFIG:
    case LK_OPEN_DB:
    case LK_CONTROL:
    case LK_CKP_CLUSTER:
	TRdisplay(",%~t)\n", 24, &lkp->lk_key1);
	break;

    case LK_TABLE:
    case LK_SV_TABLE:
    case LK_TBL_CONTROL:
	TRdisplay(",DB=%x,TABLE=[%d,%d])\n", 
	lkp->lk_key1, lkp->lk_key2, lkp->lk_key3);
	break;

    case LK_PAGE:
    case LK_BM_PAGE:
    case LK_PH_PAGE:
	TRdisplay(",DB=%x,TABLE=[%d,%d],PAGE=%d)\n",
	lkp->lk_key1, lkp->lk_key2, 
	lkp->lk_key3, lkp->lk_key4);
	break;

    case LK_EXTEND_FILE:
	TRdisplay(",DB=%x,TABLE=[%d,%d])\n",
	    lkp->lk_key1, lkp->lk_key2, lkp->lk_key3);
	break;

    case LK_SS_EVENT:
	TRdisplay(",SERVER=%x,[%x,%x])\n", lkp->lk_key1,
	    lkp->lk_key2, lkp->lk_key3);
	break;

    case LK_CREATE_TABLE:
    case LK_CKP_DB:
	TRdisplay(",DB=%x,NAME='%t-%t')\n", 
	    lkp->lk_key1, 
	    12,&lkp->lk_key4,
	    8,&lkp->lk_key2);
	break;

    case LK_BM_LOCK:
	TRdisplay(", BUFMGR='%t')\n", 8, &lkp->lk_key1);
	break;

    case LK_BM_DATABASE:
	TRdisplay(",DB=%x)\n", lkp->lk_key1);
	break;

    case LK_BM_TABLE:
	TRdisplay(",DB=%x,TABLE=[%d,%d])\n",
	    lkp->lk_key1, lkp->lk_key2, lkp->lk_key3);
	break;

    case LK_EVCONNECT:
	TRdisplay(",SERVER=%x)\n", lkp->lk_key1);
	break;

    case LK_AUDIT:
	TRdisplay(",TYPE=%d,OPER=%d)\n", 
	   lkp->lk_key1, lkp->lk_key2);
	break;

    case LK_ROW:
	TRdisplay(",DB=%x,TABLE=[%d,%d],PAGE=%d,LINE=%d)\n", 
	    lkp->lk_key1, lkp->lk_key2, lkp->lk_key3,
	    lkp->lk_key4, lkp->lk_key5);
	break;

    default:
	TRdisplay("%d,(%x,%x,%x,%x,%x,%x))\n",
	    lkp->lk_type, lkp->lk_key1,
	    lkp->lk_key2, lkp->lk_key3,
	    lkp->lk_key4, lkp->lk_key5, lkp->lk_key6); 
    }

    TRdisplay("    BLOCKEDBY:\n%#.#{     VMS_ID=%x PID=%x CSID=%x RQM=%.1w\n\
                   GRM=%.1w QUEUE=%.1d REMLKID=%x REMCSID=%x\n%}",
	lki_bed_size.length / lki_bed_size.width, 
	lki_bed_size.width, 
	lki_blocked,
	CL_OFFSETOF(LKIDEF, lki$l_lockid), 
	CL_OFFSETOF(LKIDEF, lki$l_pid), 
	CL_OFFSETOF(LKIDEF, lki$l_sysid),
	"N,IS,IX,S,SIX,U,X", 
	CL_OFFSETOF(LKIDEF, lki$b_rqmode),
	"N,IS,IX,S,SIX,U,X", 
	CL_OFFSETOF(LKIDEF, lki$b_grmode),
	CL_OFFSETOF(LKIDEF, lki$b_queue), 
	CL_OFFSETOF(LKIDEF, lki$l_remlkid), 
	CL_OFFSETOF(LKIDEF, lki$l_remsysid));

    TRdisplay("    BLOCKING: \n%#.#{     VMS_ID=%x PID=%x CSID=%x RQM=%.1w\n\
                   GRM=%.1w QUEUE=%.1d REMLKID=%x REMCSID=%x\n%}",
	lki_bng_size.length / lki_bng_size.width, 
	lki_bng_size.width, 
	lki_blocking,
	CL_OFFSETOF(LKIDEF, lki$l_lockid), 
	CL_OFFSETOF(LKIDEF, lki$l_pid), 
	CL_OFFSETOF(LKIDEF, lki$l_sysid),
	"N,IS,IX,S,SIX,U,X", 
	CL_OFFSETOF(LKIDEF, lki$b_rqmode),
	"N,IS,IX,S,SIX,U,X", 
	CL_OFFSETOF(LKIDEF, lki$b_grmode),
	CL_OFFSETOF(LKIDEF, lki$b_queue), 
	CL_OFFSETOF(LKIDEF, lki$l_remlkid), 
	CL_OFFSETOF(LKIDEF, lki$l_remsysid));

    TRdisplay("    ALL LOCKS:\n%#.#{     VMS_ID=%x PID=%x CSID=%x RQM=%.1w\n\
                   GRM=%.1w QUEUE=%.1d REMLKID=%x REMCSID=%x\n%}",
	lki_full_size.length / lki_full_size.width,
	lki_full_size.width,
	lki_fullist,
	CL_OFFSETOF(LKIDEF, lki$l_lockid),
	CL_OFFSETOF(LKIDEF, lki$l_pid),
	CL_OFFSETOF(LKIDEF, lki$l_sysid),
	"N,IS,IX,S,SIX,U,X",
	CL_OFFSETOF(LKIDEF, lki$b_rqmode),
	"N,IS,IX,S,SIX,U,X",
	CL_OFFSETOF(LKIDEF, lki$b_grmode),
	CL_OFFSETOF(LKIDEF, lki$b_queue),
	CL_OFFSETOF(LKIDEF, lki$l_remlkid),
	CL_OFFSETOF(LKIDEF, lki$l_remsysid));

}

fail_msg(STATUS status)
{
	switch (status)
	{
	    case SS$_NOPRIV:
		TRdisplay("Not enough priv to run getlkiw.\n");
		break;
	    case SS$_IVLOCKID:
		TRdisplay("Bad lockid\n");
		break;
	    default:
		TRdisplay("Unexpected status from sys$getlkiw is %d\n", status);
		break;
	}
}
#else

static int x;

#endif
