/*
**Copyright (c) 2004 Ingres Corporation
*/
/*
** REGISTER TABLE statements which register the current LK MO data structures
** for retrieval through the IMA MO Gateway.
**
** History:
**	6-oct-1992 (bryanp)
**	    Created for the new locking system.
**	14-Dec-1992 (daveb)
**	    Update to 12/14/92 register syntax.
**	11-Mar-1993 (daveb)
**	    Add lkb_rsb_id_id field, so you can join to the RSB.
**	26-apr-1993 (bryanp)
**	    Updates for 6.5 cluster support.
**	    Set VNODE to VNODE.
**	24-may-1993 (bryanp)
**	    More cluster-related changes: new statistics fields.
**	    Track RSB's separately from LKB's.
*/

remove lkmo_lkd;\p\g
remove lkmo_llb;\p\g
remove lkmo_rsb;\p\g
remove lkmo_lkb;\p\g

register table lkmo_lkd (

    vnode	    varchar(4) not null not default is 'VNODE',

    lkd_status			    char(100) not null not default
			    is 'exp.dmf.lk.lkd_status',
    lkd_status_num		    i4 not null not default
			    is 'exp.dmf.lk.lkd_status_num',
    lkd_csp_id			    i4 not null not default
			    is 'exp.dmf.lk.lkd_csp_id',
    lkd_rsh_size		    i4 not null not default
			    is 'exp.dmf.lk.lkd_rsh_size',
    lkd_lkh_size		    i4 not null not default
			    is 'exp.dmf.lk.lkd_lkh_size',
    lkd_max_lkb			    i4 not null not default
			    is 'exp.dmf.lk.lkd_max_lkb',
    lkd_llb_inuse		    i4 not null not default
			    is 'exp.dmf.lk.lkd_llb_inuse',
    lkd_rsb_inuse		    i4 not null not default
			    is 'exp.dmf.lk.lkd_rsb_inuse',
    lkd_lkb_inuse		    i4 not null not default
			    is 'exp.dmf.lk.lkd_lkb_inuse',
    lkd_lbk_count		    i4 not null not default
			    is 'exp.dmf.lk.lkd_lbk_count',
    lkd_lbk_size		    i4 not null not default
			    is 'exp.dmf.lk.lkd_lbk_size',
    lkd_sbk_count		    i4 not null not default
			    is 'exp.dmf.lk.lkd_sbk_count',
    lkd_sbk_size		    i4 not null not default
			    is 'exp.dmf.lk.lkd_sbk_size',
    lkd_rbk_count		    i4 not null not default
			    is 'exp.dmf.lk.lkd_rbk_count',
    lkd_rbk_size		    i4 not null not default
			    is 'exp.dmf.lk.lkd_rbk_size',
    lkd_lock_stall		    i4 not null not default
			    is 'exp.dmf.lk.lkd_lock_stall',
    lkd_node_fail		    i4 not null not default
			    is 'exp.dmf.lk.lkd_node_fail',
    lkd_csid			    i4 not null not default
			    is 'exp.dmf.lk.lkd_csid',
    lkd_stat_next_id		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.next_id',
    lkd_stat_create_list	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.create_list',
    lkd_stat_request_new	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.request_new',
    lkd_stat_request_cvt	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.request_convert',
    lkd_stat_convert		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.convert',
    lkd_stat_release		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.release',
    lkd_stat_rlse_partial	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.release_partial',
    lkd_stat_release_all	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.release_all',
    lkd_stat_wait		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.wait',
    lkd_stat_convert_wait	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.convert_wait',
    lkd_stat_cvt_search		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.convert_search',
    lkd_stat_cvt_deadlock	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.convert_deadlock',
    lkd_stat_dlock_srch		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.deadlock_search',
    lkd_stat_deadlock		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.deadlock',
    lkd_stat_cancel		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.cancel',
    lkd_stat_enq		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.enq',
    lkd_stat_deq		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.deq',
    lkd_stat_gdlck_search	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.gdlck_search',
    lkd_stat_gdeadlock		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.gdeadlock',
    lkd_stat_gldck_grant	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.gdlck_grant',
    lkd_stat_tot_gdlck_srch	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.totl_gdlck_search',
    lkd_stat_gdlck_call		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.gdlck_call',
    lkd_stat_gdlck_sent		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.gdlck_sent',
    lkd_stat_cnt_gdlck_call	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.cnt_gdlck_call',
    lkd_stat_cnt_gdlck_sent	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.cnt_gdlck_sent',
    lkd_stat_uns_gdlck_srch	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.unsent_gdlck_search',
    lkd_stat_sent_all		    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.sent_all',
    lkd_stat_synch_complete	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.synch_complete',
    lkd_stat_asynch_complete	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.asynch_complete',
    lkd_stat_csp_msgs_rcvd	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.csp_msgs_rcvd',
    lkd_stat_csp_wakeups_sent	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.csp_wakeups_sent',
    lkd_stat_allocate_cb	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.allocate_cb',
    lkd_stat_deallocate_cb	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.deallocate_cb',
    lkd_stat_sbk_highwater	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.sbk_highwater',
    lkd_stat_lbk_highwater	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.lbk_highwater',
    lkd_stat_rbk_highwater	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.rbk_highwater',
    lkd_stat_rsb_highwater	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.rsb_highwater',
    lkd_stat_lkb_highwater	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.lkb_highwater',
    lkd_stat_llb_highwater	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.llb_highwater',
    lkd_stat_max_lcl_dlk_srch	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.max_lcl_dlk_srch',
    lkd_stat_dlock_locks_examined   i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.dlock_locks_examined',
    lkd_stat_max_rsrc_chain_len	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.max_rsrc_chain_len',
    lkd_stat_max_lock_chain_len	    i4 not null not default
			    is 'exp.dmf.lk.lkd_stat.max_lock_chain_len'
)
as import from 'tables'
with dbms = IMA,
     structure = sortkeyed,
     key = (vnode);
\p\g

register table lkmo_llb (

    vnode	    varchar(4) not null not default is 'VNODE',

    llb_id_instance	    i4 not null not default
			    is 'exp.dmf.lk.llb_id.id_instance',
    llb_id_id		    i4 not null not default
			    is 'exp.dmf.lk.llb_id.id_id',
    llb_lkb_count	    i4 not null not default
			    is 'exp.dmf.lk.llb_lkb_count',
    llb_status		    char(100) not null not default
			    is 'exp.dmf.lk.llb_status',
    llb_status_num	    i4 not null not default
			    is 'exp.dmf.lk.llb_status_num',
    llb_llkb_count	    i4 not null not default
			    is 'exp.dmf.lk.llb_llkb_count',
    llb_max_lkb		    i4 not null not default
			    is 'exp.dmf.lk.llb_max_lkb',
    llb_related_llb	    i4 not null not default
			    is 'exp.dmf.lk.llb_related_llb',
    llb_related_llb_id_id	i4 not null not default
			    is 'exp.dmf.lk.llb_related_llb_id_id',
    llb_related_count	    i4 not null not default
			    is 'exp.dmf.lk.llb_related_count',
    llb_wait_id_id		i4 not null not default
			    is 'exp.dmf.lk.llb_wait_id_id',
    llb_name0		    i4 not null not default
			    is 'exp.dmf.lk.llb_name0',
    llb_name1		    i4 not null not default
			    is 'exp.dmf.lk.llb_name1',
    llb_search_count	    i4 not null not default
			    is 'exp.dmf.lk.llb_search_count',
    llb_pid		    i4 not null not default
			    is 'exp.dmf.lk.llb_pid',
    llb_sid		    i4 not null not default
			    is 'exp.dmf.lk.llb_sid',
    llb_connect_count	    i4 not null not default
			    is 'exp.dmf.lk.llb_connect_count',
    llb_event_type	    i4 not null not default
			    is 'exp.dmf.lk.llb_event_type',
    llb_event_value	    i4 not null not default
			    is 'exp.dmf.lk.llb_event_value',
    llb_evflags		    i4 not null not default
			    is 'exp.dmf.lk.llb_evflags',
    llb_timeout		    i4 not null not default
			    is 'exp.dmf.lk.llb_timeout',
    llb_stamp		    i4 not null not default
			    is 'exp.dmf.lk.llb_stamp',
    llb_tick		    i4 not null not default
			    is 'exp.dmf.lk.llb_tick'

)
as import from 'tables'
with dbms = IMA,
     structure = unique sortkeyed,
     key = (vnode, llb_id_id);
\p\g

register table lkmo_lkb (

    vnode	    varchar(4) not null not default is 'VNODE',

    lkb_id_instance	    i4 not null not default
			    is 'exp.dmf.lk.lkb_id.id_instance',
    lkb_id_id		    i4 not null not default
			    is 'exp.dmf.lk.lkb_id.id_id',
    lkb_count		    i4 not null not default
			    is 'exp.dmf.lk.lkb_count',
    lkb_grant_mode		char(10) not null not default
			    is 'exp.dmf.lk.lkb_grant_mode',
    lkb_grant_mode_num		i4 not null not default
			    is 'exp.dmf.lk.lkb_grant_mode_num',
    lkb_request_mode		char(10) not null not default
			    is 'exp.dmf.lk.lkb_request_mode',
    lkb_request_mode_num	i4 not null not default
			    is 'exp.dmf.lk.lkb_request_mode_num',
    lkb_state			char(100) not null not default
			    is 'exp.dmf.lk.lkb_state',
    lkb_state_num		i4 not null not default
			    is 'exp.dmf.lk.lkb_state_num',
    lkb_attribute		char(100) not null not default
			    is 'exp.dmf.lk.lkb_attribute',
    lkb_attribute_num		i4 not null not default
			    is 'exp.dmf.lk.lkb_attribute_num',
    lkb_rsb_id_id	    i4 not null not default
			    is 'exp.dmf.lk.lkb_rsb_id_id',
    lkb_llb_id_id	    i4 not null not default
			    is 'exp.dmf.lk.lkb_llb_id_id',
    lkb_lksb_lock_id	    i4 not null not default
			    is 'exp.dmf.lk.lkb_lksb.lock_id'
)
as import from 'tables'
with dbms = IMA,
     structure = unique sortkeyed,
     key = (vnode, lkb_id_id);
\p\g

register table lkmo_rsb (

    vnode	    varchar(4) not null not default is 'VNODE',

    rsb_id_instance	    i4 not null not default
			    is 'exp.dmf.lk.rsb_id.id_instance',
    rsb_id_id		    i4 not null not default
			    is 'exp.dmf.lk.rsb_id.id_id',
    rsb_grant_mode		char(10) not null not default
			    is 'exp.dmf.lk.rsb_grant_mode',
    rsb_grant_mode_num		i4 not null not default
			    is 'exp.dmf.lk.rsb_grant_mode_num',
    rsb_convert_mode		char(10) not null not default
			    is 'exp.dmf.lk.rsb_convert_mode',
    rsb_convert_mode_num	i4 not null not default
			    is 'exp.dmf.lk.rsb_convert_mode_num',
    rsb_name			char(100) not null not default
			    is 'exp.dmf.lk.rsb_name',
    rsb_name0		    i4 not null not default
			    is 'exp.dmf.lk.rsb_name0',
    rsb_name1		    i4 not null not default
			    is 'exp.dmf.lk.rsb_name1',
    rsb_name2		    i4 not null not default
			    is 'exp.dmf.lk.rsb_name2',
    rsb_name3		    i4 not null not default
			    is 'exp.dmf.lk.rsb_name3',
    rsb_name4		    i4 not null not default
			    is 'exp.dmf.lk.rsb_name4',
    rsb_name5		    i4 not null not default
			    is 'exp.dmf.lk.rsb_name5',
    rsb_name6		    i4 not null not default
			    is 'exp.dmf.lk.rsb_name6',
    rsb_value0		    i4 not null not default
			    is 'exp.dmf.lk.rsb_value0',
    rsb_value1		    i4 not null not default
			    is 'exp.dmf.lk.rsb_value1',
    rsb_invalid		    i4 not null not default
			    is 'exp.dmf.lk.rsb_invalid'

)
as import from 'tables'
with dbms = IMA,
     structure = unique sortkeyed,
     key = (vnode, rsb_id_id);
\p\g
