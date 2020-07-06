/*
**Copyright (c) 2004 Ingres Corporation
*/
/*
** REGISTER TABLE statements which register the current DI slave MO data 
** structures for retrieval through the IMA MO Gateway.
**
** History:
**	26-jul-1993 (mikem)
**	    Created (modeled after the lg ima interface).
*/

remove dimo_perf;\p\g

register table dimo_perf (

    server		varchar(32) not null not default is 'SERVER',

    dimo_slaveno  i4 not null not default 
			is 'exp.clf.di.di_slaveno',
    dimo_utime_secs  i4 not null not default 
			is 'exp.clf.di.dimo_utime.tm_secs',
    dimo_utime_msecs i4 not null not default 
			is 'exp.clf.di.dimo_utime.tm_msecs',
    dimo_stime_secs  i4 not null not default 
			is 'exp.clf.di.dimo_stime.tm_secs',
    dimo_stime_msecs i4 not null not default 
			is 'exp.clf.di.dimo_stime.tm_msecs',
    dimo_cpu_secs  i4 not null not default 
			is 'exp.clf.di.dimo_cpu.tm_secs',
    dimo_cpu_msecs i4 not null not default 
			is 'exp.clf.di.dimo_cpu.tm_msecs',
    dimo_maxrss      i4 not null not default 
			is 'exp.clf.di.dimo_maxrss',
    dimo_idrss      i4 not null not default 
			is 'exp.clf.di.dimo_idrss',
    dimo_minflt      i4 not null not default 
			is 'exp.clf.di.dimo_minflt',
    dimo_majflt      i4 not null not default 
			is 'exp.clf.di.dimo_majflt',
    dimo_nswap      i4 not null not default 
			is 'exp.clf.di.dimo_nswap',
    dimo_reads      i4 not null not default 
			is 'exp.clf.di.dimo_reads',
    dimo_writes      i4 not null not default
			is 'exp.clf.di.dimo_writes',
    dimo_dio      i4 not null not default
			is 'exp.clf.di.dimo_dio',
    dimo_msgsnd      i4 not null not default 
			is 'exp.clf.di.dimo_msgsnd',
    dimo_msgrcv      i4 not null not default 
			is 'exp.clf.di.dimo_msgrcv',
    dimo_msgtotal      i4 not null not default 
			is 'exp.clf.di.dimo_msgtotal',
    dimo_nsignals      i4 not null not default 
			is 'exp.clf.di.dimo_nsignals',
    dimo_nvcsw      i4 not null not default 
			is 'exp.clf.di.dimo_nvcsw',
    dimo_nivcsw      i4 not null not default 
			is 'exp.clf.di.dimo_nivcsw'
)
as import from 'tables'
with dbms = IMA,
structure = unique sortkeyed,
key = ( server, dimo_slaveno )
\p\g
grant all on dimo_perf to public;
\p\g
commit;
\p\g

remove dimo_perf_control;\p\g

register table dimo_perf_control (
    server	  varchar(32) not null not default is 'SERVER',
    dimo_collect  i4 not null not default is 'exp.clf.di.dimo_collect')
as import from 'tables'
with dbms = IMA,
structure = sortkeyed,
key = ( server);

/* Allow anyone to query the table */
grant all on dimo_perf_control to public;
\p\g

select * from (dimo_perf_control);

/* qry to make server update it's static buffer which contains the perf stats */
update yymib_objects set value = dbmsinfo('ima_vnode')
        where classid = 'exp.clf.di.dimo_collect'
        and instance = '0'
        and server = dbmsinfo('ima_server');
\p\g
commit;
\p\g
