/*
**Copyright (c) 2004 Ingres Corporation
*/
/*
** REGISTER TABLE statements which register the current LG MO data structures
** for retrieval through the IMA MO Gateway.
**
** History:
**	6-oct-1992 (bryanp)
**	    Created for the new logging system.
**	10-November-1992 (rmuth)
**	    Remove lgd_n_servers
**	13-nov-1992 (bryanp)
**	    Add new formatted log address fields.
**	14-Dec-1992 (daveb)
**	    Revamped for 12/14 register syntax.
**	18-jan-1993 (bryanp)
**	    Add more MO objects for XA_RECOVER support.
**	26-apr-1993 (bryanp)
**	    More work following 6.5 cluster support.
**	20-sep-1993 (mikem)
**	    Updated register statement to reflect new lfb_archive_window_*
**	    fields.
**	31-jan-1994 (mikem)
**	    bug #57045
**	    Remove registering for the following fields: lgd_stall_limit, 
**	    lgd_dmu_cnt, lxb_dmu_cnt.  These fields were removed from the code
**	    in the 18-oct-93 integration.  Also fixed up some other fields
**	    that had become out of date with lgmo.c (because of naming changes).
**      13-Apr-1994 (daveb) 62240
**          Add % logful object.
*/

remove lgmo_lgd;\p\g
remove lgmo_lfb;\p\g
remove lgmo_lxb;\p\g
remove lgmo_lpb;\p\g
remove lgmo_ldb;\p\g

register table lgmo_lgd (

    vnode	varchar(4)  not null not default is 'VNODE',

    lgd_status	    char(100) not null not default is 'exp.dmf.lg.lgd_status',
    lgd_status_num  i4 not null not default is 'exp.dmf.lg.lgd_status_num',
    lgd_lpb_inuse   i4 not null not default is 'exp.dmf.lg.lgd_lpb_inuse',
    lgd_lxb_inuse   i4 not null not default is 'exp.dmf.lg.lgd_lxb_inuse',
    lgd_ldb_inuse   i4 not null not default is 'exp.dmf.lg.lgd_ldb_inuse',
    lgd_lpd_inuse   i4 not null not default is 'exp.dmf.lg.lgd_lpd_inuse',
    lgd_protect_count	i4 not null not default
					is 'exp.dmf.lg.lgd_protect_count',
    lgd_n_logwriters	i4 not null not default
					is 'exp.dmf.lg.lgd_n_logwriters',
    lgd_no_bcp	    i4 not null not default is 'exp.dmf.lg.lgd_no_bcp',
    lgd_sbk_count   i4 not null not default is 'exp.dmf.lg.lgd_sbk_count',
    lgd_sbk_size    i4 not null not default is 'exp.dmf.lg.lgd_sbk_size',
    lgd_lbk_count   i4 not null not default is 'exp.dmf.lg.lgd_lbk_count',
    lgd_lbk_size    i4 not null not default is 'exp.dmf.lg.lgd_lbk_size',

    lgd_cnodeid		    i4 not null not default
				is 'exp.dmf.lg.lgd_cnodeid',
    lgd_csp_pid		    i4 not null not default
				is 'exp.dmf.lg.lgd_csp_pid',
    lgd_stat_add	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.add',
    lgd_stat_remove	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.remove',
    lgd_stat_begin	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.begin',
    lgd_stat_end	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.end',
    lgd_stat_write	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.write',
    lgd_stat_split	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.split',
    lgd_stat_force	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.force',
    lgd_stat_readio	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.readio',
    lgd_stat_writeio	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.writeio',
    lgd_stat_wait	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.wait',
    lgd_stat_group_force    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.group_force',
    lgd_stat_group_count    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.group_count',
    lgd_stat_inconsist_db   i4 not null not default
				is 'exp.dmf.lg.lgd_stat.inconsist_db',
    lgd_stat_pgyback_check  i4 not null not default
				is 'exp.dmf.lg.lgd_stat.pgyback_check',
    lgd_stat_pgyback_write  i4 not null not default
				is 'exp.dmf.lg.lgd_stat.pgyback_write',
    lgd_stat_kbytes	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.kbytes',
    lgd_stat_free_wait	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.free_wait',
    lgd_stat_stall_wait	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.stall_wait',
    lgd_stat_bcp_stall_wait i4 not null not default
				is 'exp.dmf.lg.lgd_stat.bcp_stall_wait',
    lgd_stat_log_readio	    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.log_readio',
    lgd_stat_dual_readio    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.dual_readio',
    lgd_stat_log_writeio    i4 not null not default
				is 'exp.dmf.lg.lgd_stat.log_writeio',
    lgd_stat_dual_writeio   i4 not null not default
				is 'exp.dmf.lg.lgd_stat.dual_writeio',

    lgd_cpstall		    i4 not null not default
				is 'exp.dmf.lg.lgd_cpstall',
    lgd_check_stall	    i4 not null not default
				is 'exp.dmf.lg.lgd_check_stall',
    lgd_gcmt_threshold	    i4 not null not default
				is 'exp.dmf.lg.lgd_gcmt_threshold',
    lgd_gcmt_numticks	    i4 not null not default
				is 'exp.dmf.lg.lgd_gcmt_numticks'

)
as import from 'tables'
with dbms = IMA,
     structure = sortkeyed,
     key = (vnode);
\p\g
    
register table lgmo_lfb (

    vnode	varchar(4)  not null not default is 'VNODE',
    lfb_status		    char(100) not null not default
				is 'exp.dmf.lg.lfb_status',
    lfb_status_num	    i4 not null not default
				is 'exp.dmf.lg.lfb_status_num',
    lfb_hdr_lgh_version	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_version',
    lfb_hdr_lgh_checksum    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_checksum',
    lfb_hdr_lgh_size	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_size',
    lfb_hdr_lgh_count	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_count',
    lfb_hdr_lgh_status	    char(100) not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_status',
    lfb_hdr_lgh_status_num  i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_status_num',
    lfb_hdr_lgh_l_logfull   i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_l_logfull',
    lfb_hdr_lgh_l_abort	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_l_abort',
    lfb_hdr_lgh_l_cp	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_l_cp',
    lfb_hdr_lgh_cpcnt	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_cpcnt',
    lfb_hdr_lgh_tran_id	    char (20) not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_tran_id',
    lfb_hdr_lgh_tran_high   i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_tran_high',
    lfb_hdr_lgh_tran_low    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_tran_low',
    lfb_hdr_lgh_begin	    char (30) not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_begin',
    lfb_hdr_lgh_begin_seq   i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_begin_seq',
    lfb_hdr_lgh_begin_off   i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_begin_off',
    lfb_hdr_lgh_end	    char (30) not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_end',
    lfb_hdr_lgh_end_seq	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_end_seq',
    lfb_hdr_lgh_end_off	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_end_off',
    lfb_hdr_lgh_cp	    char (30) not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_cp',
    lfb_hdr_lgh_cp_seq	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_cp_seq',
    lfb_hdr_lgh_cp_off	    i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_cp_off',
    lfb_hdr_lgh_active_logs i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_active_logs',
    lfb_hdr_lgh_percentage_logfull i4 not null not default
				is 'exp.dmf.lg.lfb_hdr_lgh_percentage_logfull,
    lfb_forced_lga	    char(30) not null not default
				is 'exp.dmf.lg.lfb_forced_lga',
    lfb_forced_high	    i4 not null not default
				is 'exp.dmf.lg.lfb_forced_lga.la_sequence',
    lfb_forced_low	    i4 not null not default
				is 'exp.dmf.lg.lfb_forced_lga.la_offset',
    lfb_forced_lsn	    char (20) not null not default
				is 'exp.dmf.lg.lfb_forced_lsn',
    lfb_forced_lsn_high	    i4 not null not default
				is 'exp.dmf.lg.lfb_forced_lsn_high',
    lfb_forced_lsn_low	    i4 not null not default
				is 'exp.dmf.lg.lfb_forced_lsn_low',
    lfb_last_lsn	    char (20) not null not default
				is 'exp.dmf.lg.lfb_last_lsn',
    lfb_last_lsn_high	    i4 not null not default
				is 'exp.dmf.lg.lfb_last_lsn_high',
    lfb_last_lsn_low	    i4 not null not default
				is 'exp.dmf.lg.lfb_last_lsn_low',
    lfb_archive_start_lga   char(30) not null not default
				is 'exp.dmf.lg.lfb_archive_start',
    lfb_archive_start_high  i4 not null not default
				is 'exp.dmf.lg.lfb_archive_start.la_sequence',
    lfb_archive_start_low   i4 not null not default
				is 'exp.dmf.lg.lfb_archive_start.la_offset',
    lfb_archive_end_lga	    char(30) not null not default
				is 'exp.dmf.lg.lfb_archive_end',
    lfb_archive_end_high    i4 not null not default
				is 'exp.dmf.lg.lfb_archive_end.la_sequence',
    lfb_archive_end_low	i4 not null not default
				is 'exp.dmf.lg.lfb_archive_end.la_offset',
    lfb_archive_prevcp_lga  char(30) not null not default
				is 'exp.dmf.lg.lfb_archive_prevcp',
    lfb_archive_prevcp_high i4 not null not default
				is 'exp.dmf.lg.lfb_archive_prevcp.la_sequence',
    lfb_archive_prevcp_low  i4 not null not default
				is 'exp.dmf.lg.lfb_archive_prevcp.la_offset',
    lfb_active_log	    char (40) not null not default
				is 'exp.dmf.lg.lfb_active_log',
    lfb_active_log_num	    i4 not null not default
				is 'exp.dmf.lg.lfb_active_log_num',
    lfb_buf_cnt		    i4 not null not default
				is 'exp.dmf.lg.lfb_buf_cnt',
    lfb_reserved_space	    i4 not null not default
				is 'exp.dmf.lg.lfb_reserved_space',
    lfb_channel_blk	    i4 not null not default
				is 'exp.dmf.lg.lfb_channel_blk',
    lfb_dual_channel_blk    i4 not null not default
				is 'exp.dmf.lg.lfb_dual_channel_blk',
    lfb_stat_split	    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_split',
    lfb_stat_write	    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_write',
    lfb_stat_force	    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_force',
    lfb_stat_wait	    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_wait',
    lfb_stat_end	    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_end',
    lfb_stat_writeio	    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_writeio',
    lfb_stat_kbytes	    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_kbytes',
    lfb_stat_log_readio	    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_log_readio',
    lfb_stat_dual_readio    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_dual_readio',
    lfb_stat_log_writeio    i4 not null not default
				is 'exp.dmf.lg.lfb_stat_log_writeio',
    lfb_stat_dual_writeio   i4 not null not default
				is 'exp.dmf.lg.lfb_stat_dual_writeio'

)
as import from 'tables'
with dbms = IMA,
     structure = sortkeyed,
     key = (vnode);
\p\g
    
register table lgmo_lpb (

    vnode	varchar(4)  not null not default is 'VNODE',

    lpb_id_instance	    i4 not null not default
				is 'exp.dmf.lg.lpb_id.id_instance',
    lpb_id_id		    i4 not null not default
				is 'exp.dmf.lg.lpb_id.id_id',
    lpb_status		    char(100) not null not default
				is 'exp.dmf.lg.lpb_status',
    lpb_status_num	    i4 not null not default
				is 'exp.dmf.lg.lpb_status_num',
    lpb_lpd_count	    i4 not null not default
				is 'exp.dmf.lg.lpb_lpd_count',
    lpb_pid		    i4 not null not default
				is 'exp.dmf.lg.lpb_pid',
    lpb_cond		    i4 not null not default
				is 'exp.dmf.lg.lpb_cond',
    lpb_bufmgr_id	    i4 not null not default
				is 'exp.dmf.lg.lpb_bufmgr_id',
    lpb_force_abort_sid	    i4 not null not default
				is 'exp.dmf.lg.lpb_force_abort_sid',
    lpb_gcmt_sid	    i4 not null not default
				is 'exp.dmf.lg.lpb_gcmt_sid',
    lpb_gcmt_asleep	    i4 not null not default
				is 'exp.dmf.lg.lpb_gcmt_asleep',
    lpb_stat_readio	    i4 not null not default
				is 'exp.dmf.lg.lpb_stat.readio',
    lpb_stat_write	    i4 not null not default
				is 'exp.dmf.lg.lpb_stat.write',
    lpb_stat_force	    i4 not null not default
				is 'exp.dmf.lg.lpb_stat.force',
    lpb_stat_wait	    i4 not null not default
				is 'exp.dmf.lg.lpb_stat.wait',
    lpb_stat_begin	    i4 not null not default
				is 'exp.dmf.lg.lpb_stat.begin',
    lpb_stat_end	    i4 not null not default
				is 'exp.dmf.lg.lpb_stat.end'

)
as import from 'tables'
with dbms = IMA,
     structure = unique sortkeyed,
     key = (vnode, lpb_id_id);
\p\g

register table lgmo_ldb (

    vnode	varchar(4)  not null not default is 'VNODE',

    ldb_id_instance	    i4 not null not default
				is 'exp.dmf.lg.ldb_id.id_instance',
    ldb_id_id		    i4 not null not default
				is 'exp.dmf.lg.ldb_id.id_id',
    ldb_status		    char(100) not null not default
				is 'exp.dmf.lg.ldb_status',
    ldb_status_num	    i4 not null not default
				is 'exp.dmf.lg.ldb_status_num',
    ldb_lpd_count	    i4 not null not default
				is 'exp.dmf.lg.ldb_lpd_count',
    ldb_lxb_count	    i4 not null not default
				is 'exp.dmf.lg.ldb_lxb_count',
    ldb_lxbo_count	    i4 not null not default
				is 'exp.dmf.lg.ldb_lxbo_count',
    ldb_database_id	    i4 not null not default
				is 'exp.dmf.lg.ldb_database_id',
    ldb_l_buffer	    i4 not null not default
				is 'exp.dmf.lg.ldb_l_buffer',
    ldb_buffer		    char(200) not null not default
				is 'exp.dmf.lg.ldb_buffer',
    ldb_db_name		    char (32) not null not default
				is 'exp.dmf.lg.ldb_db_name',
    ldb_db_owner	    char (32) not null not default
				is 'exp.dmf.lg.ldb_db_owner',
    ldb_j_first_la	    char (30) not null not default
				is 'exp.dmf.lg.ldb_j_first_la',
    ldb_j_first_high	    i4 not null not default
				is 'exp.dmf.lg.ldb_j_first_la.la_sequence',
    ldb_j_first_low	    i4 not null not default
				is 'exp.dmf.lg.ldb_j_first_la.la_offset',
    ldb_j_last_la	    char (30) not null not default
				is 'exp.dmf.lg.ldb_j_last_la',
    ldb_j_last_la_high	    i4 not null not default
				is 'exp.dmf.lg.ldb_j_last_la.la_sequence',
    ldb_j_last_la_low	    i4 not null not default
				is 'exp.dmf.lg.ldb_j_last_la.la_offset',
    ldb_d_first_la	    char (30) not null not default
				is 'exp.dmf.lg.ldb_d_first_la',
    ldb_d_first_high	    i4 not null not default
				is 'exp.dmf.lg.ldb_d_first_la.la_sequence',
    ldb_d_first_low	    i4 not null not default
				is 'exp.dmf.lg.ldb_d_first_la.la_offset',
    ldb_d_last_la	    char (30) not null not default
				is 'exp.dmf.lg.ldb_d_last_la',
    ldb_d_last_high	    i4 not null not default
				is 'exp.dmf.lg.ldb_d_last_la.la_sequence',
    ldb_d_last_low	    i4 not null not default
				is 'exp.dmf.lg.ldb_d_last_la.la_offset',
    ldb_sbackup_lga	    char (30) not null not default
				is 'exp.dmf.lg.ldb_sbackup',
    ldb_sbackup_high	    i4 not null not default
				is 'exp.dmf.lg.ldb_sbackup.la_sequence',
    ldb_sbackup_low	    i4 not null not default
				is 'exp.dmf.lg.ldb_sbackup.la_offset',
    ldb_sback_lsn	    char (20) not null not default
				is 'exp.dmf.lg.ldb_sback_lsn',
    ldb_sback_lsn_high	    i4 not null not default
				is 'exp.dmf.lg.ldb_sback_lsn_high',
    ldb_sback_lsn_low	    i4 not null not default
				is 'exp.dmf.lg.ldb_sback_lsn_low',
    ldb_stat_read	    i4 not null not default
				is 'exp.dmf.lg.ldb_stat.read',
    ldb_stat_write	    i4 not null not default
				is 'exp.dmf.lg.ldb_stat.write',
    ldb_stat_begin	    i4 not null not default
				is 'exp.dmf.lg.ldb_stat.begin',
    ldb_stat_end	    i4 not null not default
				is 'exp.dmf.lg.ldb_stat.end',
    ldb_stat_force	    i4 not null not default
				is 'exp.dmf.lg.ldb_stat.force',
    ldb_stat_wait	    i4 not null not default
				is 'exp.dmf.lg.ldb_stat.wait'

)
as import from 'tables'
with dbms = IMA,
     structure = unique sortkeyed,
     key = (vnode, ldb_id_id);
\p\g

register table lgmo_lxb (

    vnode	varchar(4)  not null not default is 'VNODE',

    lxb_id_instance	    i4 not null not default
				is 'exp.dmf.lg.lxb_id.id_instance',
    lxb_id_id		    i4 not null not default
				is 'exp.dmf.lg.lxb_id.id_id',
    lxb_status		    char(100) not null not default
				is 'exp.dmf.lg.lxb_status',
    lxb_status_num	    i4 not null not default
				is 'exp.dmf.lg.lxb_status_num',
    lxb_db_name		    char (32) not null not default
				is 'exp.dmf.lg.lxb_db_name',
    lxb_db_owner	    char (32) not null not default
				is 'exp.dmf.lg.lxb_db_owner',
    lxb_db_id_id	    i4 not null not default
				is 'exp.dmf.lg.lxb_db_id_id',
    lxb_pr_id_id	    i4 not null not default
				is 'exp.dmf.lg.lxb_pr_id_id',
    lxb_wait_reason	    char(20) not null not default
				is 'exp.dmf.lg.lxb_wait_reason',
    lxb_wait_reason_num	    i4 not null not default
				is 'exp.dmf.lg.lxb_wait_reason_num',
    lxb_sequence	    i4 not null not default
				is 'exp.dmf.lg.lxb_sequence',
    lxb_first_lga	    char (30) not null not default
				is 'exp.dmf.lg.lxb_first_lga',
    lxb_first_high	    i4 not null not default
				is 'exp.dmf.lg.lxb_first_lga.la_sequence',
    lxb_first_low	    i4 not null not default
				is 'exp.dmf.lg.lxb_first_lga.la_offset',
    lxb_last_lga	    char (30) not null not default
				is 'exp.dmf.lg.lxb_last_lga',
    lxb_last_high	    i4 not null not default
				is 'exp.dmf.lg.lxb_last_lga.la_sequence',
    lxb_last_low	    i4 not null not default
				is 'exp.dmf.lg.lxb_last_lga.la_offset',
    lxb_last_lsn	    char (30) not null not default
				is 'exp.dmf.lg.lxb_last_lsn',
    lxb_last_lsn_high	    i4 not null not default
				is 'exp.dmf.lg.lxb_last_lsn_high',
    lxb_last_lsn_low	    i4 not null not default
				is 'exp.dmf.lg.lxb_last_lsn_low',
    lxb_cp_lga		    char (30) not null not default
				is 'exp.dmf.lg.lxb_cp_lga',
    lxb_cp_high		    i4 not null not default
				is 'exp.dmf.lg.lxb_cp_lga.la_sequence',
    lxb_cp_low		    i4 not null not default
				is 'exp.dmf.lg.lxb_cp_lga.la_offset',
    lxb_tran_id		    char(16) not null not default
				is 'exp.dmf.lg.lxb_tran_id',
    lxb_tran_high	    i4 not null not default
				is 'exp.dmf.lg.lxb_tran_id.db_high_tran',
    lxb_tran_low	    i4 not null not default
				is 'exp.dmf.lg.lxb_tran_id.db_low_tran',
    lxb_dis_tran_id_hexdump char(350) not null not default
				is 'exp.dmf.lg.lxb_dis_tran_id_hexdump',
    lxb_pid		    i4 not null not default
				is 'exp.dmf.lg.lxb_pid',
    lxb_sid		    i4 not null not default
				is 'exp.dmf.lg.lxb_sid',
    lxb_reserved_space	    i4 not null not default
				is 'exp.dmf.lg.lxb_reserved_space',
    lxb_user_name	    char(100) not null not default
				is 'exp.dmf.lg.lxb_user_name',
    lxb_is_prepared	    char (1) not null not default
				is 'exp.dmf.lg.lxb_is_prepared',
    lxb_is_xa_dis_tran_id   char (1) not null not default
				is 'exp.dmf.lg.lxb_is_xa_dis_tran_id',
    lxb_stat_split	    i4 not null not default
				is 'exp.dmf.lg.lxb_stat.split',
    lxb_stat_write	    i4 not null not default
				is 'exp.dmf.lg.lxb_stat.write',
    lxb_stat_force	    i4 not null not default
				is 'exp.dmf.lg.lxb_stat.force',
    lxb_stat_wait	    i4 not null not default
				is 'exp.dmf.lg.lxb_stat.wait'

)
as import from 'tables'
with dbms = IMA,
     structure = unique sortkeyed,
     key = (vnode, lxb_id_id);
\p\g

drop view lgmo_xa_dis_tran_ids;
\p\g
create view lgmo_xa_dis_tran_ids (xa_dis_tran_id, xa_database_name) as
	select lxb_dis_tran_id_hexdump, lxb_db_name
	    from lgmo_lxb
	    where lxb_is_prepared = 'Y' and lxb_is_xa_dis_tran_id = 'Y';
\p\g
