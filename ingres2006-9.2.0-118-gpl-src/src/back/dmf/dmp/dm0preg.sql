/*
**Copyright (c) 2004 Ingres Corporation
*/
/*
** REGISTER TABLE statements which register the current Buffer Manager
** MO data structures ** for retrieval through the IMA MO Gateway.
**
** History:
**	18-oct-1993 (rmuth)
**	    Created.
*/

remove dm0p_lbmcb_mo_stats;
\p\g

register table dm0p_lbmcb_mo_stats (
   vnode       varchar(4)  not null not default is 'VNODE',
   lbm_fix         i4 not null not default
                      is 'exp.dmf.dm0p.lbm_fix',
   lbm_hit         i4 not null not default
                      is 'exp.dmf.dm0p.lbm_hit',
   lbm_check       i4 not null not default
                      is 'exp.dmf.dm0p.lbm_check',
   lbm_refresh     i4 not null not default
                      is 'exp.dmf.dm0p.lbm_refresh',
   lbm_reads       i4 not null not default
                      is 'exp.dmf.dm0p.lbm_reads',
   lbm_unfix       i4 not null not default
                      is 'exp.dmf.dm0p.lbm_unfix',
   lbm_dirty       i4 not null not default
                      is 'exp.dmf.dm0p.lbm_dirty',
   lbm_force       i4 not null not default
                      is 'exp.dmf.dm0p.lbm_force',
   lbm_writes      i4 not null not default
                      is 'exp.dmf.dm0p.lbm_writes',
   lbm_iowait      i4 not null not default
                      is 'exp.dmf.dm0p.lbm_iowait',
   lbm_greads      i4 not null not default
                      is 'exp.dmf.dm0p.lbm_greads',
   lbm_gwrites     i4 not null not default
                      is 'exp.dmf.dm0p.lbm_gwrites',
   lbm_fwait       i4 not null not default
                      is 'exp.dmf.dm0p.lbm_fwait',
   lbm_mwait       i4 not null not default
                      is 'exp.dmf.dm0p.lbm_mwait',
   lbm_lockreclaim i4 not null not default
                      is 'exp.dmf.dm0p.lbm_lockreclaim',
   lbm_fcflush     i4 not null not default
                      is 'exp.dmf.dm0p.lbm_fcflush',
   lbm_wbflush     i4 not null not default
                      is 'exp.dmf.dm0p.lbm_wbflush',
   lbm_bmcwait     i4 not null not default
                      is 'exp.dmf.dm0p.lbm_bmcwait'
)
as import from 'tables'
with dbms = IMA,
    structure = sortkeyed,
    key = (vnode);
\p\g

remove dm0p_bmcb_mo_stats;
\p\g

register table dm0p_bmcb_mo_stats (
   vnode        varchar(4)  not null not default is 'VNODE',
   bm_hshcnt    i4 not null not default
                	is 'exp.dmf.dm0p.bm_hshcnt',
   bm_bufcnt    i4 not null not default
                	is 'exp.dmf.dm0p.bm_bufcnt',
   bm_sbufcnt   i4 not null not default
                	is 'exp.dmf.dm0p.bm_sbufcnt',
   bm_gcnt      i4 not null not default
                	is 'exp.dmf.dm0p.bm_gcnt',
   bm_gpages    i4 not null not default
                	is 'exp.dmf.dm0p.bm_gpages',
   bm_dbcsize   i4 not null not default
                	is 'exp.dmf.dm0p.bm_dbcsize',
   bm_tblcsize  i4 not null not default
                	is 'exp.dmf.dm0p.bm_tblcsize',
   bm_tblpsize  i4 not null not default
                	is 'exp.dmf.dm0p.bm_tblpsize',
   bm_tblpindex i4 not null not default
                	is 'exp.dmf.dm0p.bm_tblpindex',
   bm_flimit    i4 not null not default
                	is 'exp.dmf.dm0p.bm_flimit',
   bm_mlimit    i4 not null not default
                	is 'exp.dmf.dm0p.bm_mlimit',
   bm_wbstart   i4 not null not default
                	is 'exp.dmf.dm0p.bm_wbstart',
   bm_wbend     i4 not null not default
                	is 'exp.dmf.dm0p.bm_wbend',
   bm_lcount    i4 not null not default
                	is 'exp.dmf.dm0p.bm_lcount',
   bm_glcount   i4 not null not default
                	is 'exp.dmf.dm0p.bm_glcount',
   bm_fcount    i4 not null not default
                	is 'exp.dmf.dm0p.bm_fcount',
   bm_gfcount   i4 not null not default
                	is 'exp.dmf.dm0p.bm_gfcount',
   bm_mcount    i4 not null not default
                	is 'exp.dmf.dm0p.bm_mcount',
   bm_gmcount   i4 not null not default
                	is 'exp.dmf.dm0p.bm_gmcount',
   bm_srv_count i4 not null not default
                	is 'exp.dmf.dm0p.bm_srv_count',
   bm_cpcount   i4 not null not default
                	is 'exp.dmf.dm0p.bm_cpcount',
   bm_cpindex   i4 not null not default
                	is 'exp.dmf.dm0p.bm_cpindex',
   bm_cpcheck   i4 not null not default
                	is 'exp.dmf.dm0p.bm_cpcheck',
   bm_fix       i4 not null not default
                      	is 'exp.dmf.dm0p.bm_fix',
   bm_hit       i4 not null not default
                      	is 'exp.dmf.dm0p.bm_hit',
   bm_check     i4 not null not default
                      	is 'exp.dmf.dm0p.bm_check',
   bm_refresh   i4 not null not default
                     	is 'exp.dmf.dm0p.bm_refresh',
   bm_reads     i4 not null not default
                      	is 'exp.dmf.dm0p.bm_reads',
   bm_unfix     i4 not null not default
                      	is 'exp.dmf.dm0p.bm_unfix',
   bm_dirty     i4 not null not default
                      	is 'exp.dmf.dm0p.bm_dirty',
   bm_force     i4 not null not default
                      	is 'exp.dmf.dm0p.bm_force',
   bm_writes    i4 not null not default
                      	is 'exp.dmf.dm0p.bm_writes',
   bm_iowait    i4 not null not default
                      	is 'exp.dmf.dm0p.bm_iowait',
   bm_greads    i4 not null not default
                     	is 'exp.dmf.dm0p.bm_greads',
   bm_gwrites   i4 not null not default
                      	is 'exp.dmf.dm0p.bm_gwrites',
   bm_fwait     i4 not null not default
                      	is 'exp.dmf.dm0p.bm_fwait',
   bm_mwait     i4 not null not default
                      	is 'exp.dmf.dm0p.bm_mwait',
   bm_lockreclaim i4 not null not default
                      	is 'exp.dmf.dm0p.bm_lockreclaim',
   bm_fcflush   i4 not null not default
                      	is 'exp.dmf.dm0p.bm_fcflush',
   bm_wbflush   i4 not null not default
                      	is 'exp.dmf.dm0p.bm_wbflush',
   bm_bmcwait   i4 not null not default
                      	is 'exp.dmf.dm0p.bm_bmcwait',
   bm_status    i4 not null not default
                      	is 'exp.dmf.dm0p.bm_status',
   bm_clock     i4 not null not default
                	is 'exp.dmf.dm0p.bm_clock'
   bm_syncwr    i4 not null not default
                	is 'exp.dmf.dm0p.bm_syncwr'
   bm_gsyncwr   i4 not null not default
                	is 'exp.dmf.dm0p.bm_gsyncwr'
)
as import from 'tables'
with dbms = IMA,
    structure = sortkeyed,
    key = (vnode);
\p\g
