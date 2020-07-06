/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>

#ifdef unix
#include    <clconfig.h>
#include    <systypes.h>
#endif /* unix */
#include    <errno.h>

#include    <er.h>
#include    <cs.h>
#include    <me.h>
#include    <st.h>
#include    <tm.h>

#ifdef xCL_069_UNISTD_H_EXISTS
#include    <unistd.h>
#endif /* xCL_069_UNISTD_H_EXISTS */

/**
**
**  Name: TMPRFSTAT.C - return per-process performance statistics.
**
**  Description:
**
**	Returns per-process performance statistics.  Implements TMperfstat()
**	function.  Currently contains implementations for getrusage() machines
**	(BSD - mostly) and XXX machines (SVR4 - mostly).
**
**	External routines:
**
**	    TMperfstat()  - Return performance statistics available from the OS.
**
**	Internal routines:
**	
**	    default_perfstat()   - default (null) implementation of TMperfstat()
**	    getrusage_perfstat() - getrusage() implementation of TMperfstat()
**
**  History:
**      26-jul-93 (mikem)
**	    Created.
**	23-aug-93 (mikem)
**	    Fixes to the procfs implementation to work make it work on programs
**	    which alter their effective uid after process startup.
**	15-mar-1999 (popri01)
**	    Relocate time constant definitions to tmcl.h.
**/


/*
**  Definition of static variables and forward static functions.
*/
#ifdef xCL_003_GETRUSAGE_EXISTS
static STATUS getrusage_perfstat(
    TM_PERFSTAT *stat,
    CL_SYS_ERR  *sys_err);
#endif /* xCL_003_GETRUSAGE_EXISTS */

#ifdef xCL_PROCFS_EXISTS
static STATUS procfs_perfstat(
    TM_PERFSTAT *stat,
    CL_SYS_ERR  *sys_err);
#endif /* xCL_PROCFS_EXISTS */

static STATUS default_perfstat(
    TM_PERFSTAT *stat,
    CL_SYS_ERR  *sys_err);

static TM_PERFSTAT default_tmperf = {
    {-1, -1}, 	/* stat->tmperf_utime    */
    {-1, -1}, 	/* stat->tmperf_stime    */
    {-1, -1}, 	/* stat->tmperf_cpu      */
    -1, 	/* stat->tmperf_maxrss   */
    -1,		/* stat->tmperf_idrss    */
    -1,		/* stat->tmperf_minflt   */
    -1,		/* stat->tmperf_majflt	 */
    -1,		/* stat->tmperf_nswap	 */
    -1,		/* stat->tmperf_reads	 */
    -1,		/* stat->tmperf_writes	 */
    -1,		/* stat->tmperf_dio   	 */
    -1,		/* stat->tmperf_msgsnd	 */
    -1,		/* stat->tmperf_msgrcv	 */
    -1,		/* stat->tmperf_msgtotal */
    -1,		/* stat->tmperf_nsignals */
    -1,		/* stat->tmperf_nvcsw	 */
    -1		/* stat->tmperf_nivcsw	 */
};

/*
**  Defines of other constants.
*/
 

/*{
** Name: TMperfstat()   - Return performance statistics available from the OS.
**
** Description:
**      Returns available performance statistics available from the OS for
**      the current process.  As best as is possible, per platform, TMperfstat()
**      provides a snapshot of all performance statistics available and
**      returns the statistics by filling in the TM_PERFSTAT structure passed
**      in by the caller.  Implimentations of this function will attempt to
**      collect the data required with the minimum possible affect on the
**      state being measured (ie. will try to make a single system call vs.
**      multiple system calls where possible).
**
**      The members of the TM_PERFSTAT structure are public and are defined in
**      <tm.h>;  see <tm.h> for description of current members of TM_PERFSTAT
**      structure.
**
**      Some statistics may not be available on some operating systems, if
**      a statistic is not available then TMperfstat() will initialize the
**      field to the value "-1".  In the case of SYSTIME structure members
**      one must test both members of the structure for -1.
**
**      It is expected that over time new CL proposals will add members to
**      the TM_PERFSTAT structure as new OS performance statistics become
**      available.   Clients which access the structure members by name,
**      and do not make structure order assumptions will be upward compatible
**      with future structure definition changes.
**
** Inputs:
**      stat                            pointer to a TM_PERFSTAT structure.
**
** Outputs:
**      stat                            TM_PERFSTAT structure is filled in by
**                                      this routine with available OS
**                                      statistics.  Unavailable fields will be
**                                      set to -1.
**      sys_err
**
**      Returns:
**          OK - success.
**          TM_NOSUPPORT - no support for gathering system statistics.
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      26-jul-93 (mikem)
**         Created.
*/
STATUS 
TMperfstat(
    TM_PERFSTAT *stat,
    CL_SYS_ERR  *sys_err)
{
    STATUS	status;

    SETCLERR(sys_err, 0, 0);

#if vms
    if ((status = vms_perfstat(stat, sys_err)) != OK)
	(void) default_perfstat(stat, sys_err);
#elif defined(xCL_PROCFS_EXISTS)
    if ((status = procfs_perfstat(stat, sys_err)) != OK)
	(void) default_perfstat(stat, sys_err);
#elif defined(xCL_003_GETRUSAGE_EXISTS)
    if ((status = getrusage_perfstat(stat, sys_err)) != OK)
	(void) default_perfstat(stat, sys_err);
#else
    status = default_perfstat(stat, sys_err);
#endif

    return(status);
}

#ifdef xCL_003_GETRUSAGE_EXISTS
#include    <rusage.h>

/*{
** Name: getrusage_perfstat()	- getrusage() implementation of TMperfstat()
**
** Description:
**	Implementation of TMperfstat() on machines with getrusage() support.
**	
**	See TMperfstat() for function details.
**
** Inputs:
**      stat                            pointer to a TM_PERFSTAT structure.
**
** Outputs:
**      stat                            TM_PERFSTAT structure is filled in by
**                                      this routine with available OS
**                                      statistics.  Unavailable fields will be
**                                      set to -1.
**      sys_err
**
**      Returns:
**          OK - success.
**          TM_NOSUPPORT - no support for gathering system statistics.
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      26-jul-93 (mikem)
**         Created.
*/
static STATUS
getrusage_perfstat(
    TM_PERFSTAT *stat,
    CL_SYS_ERR  *sys_err)
{
    STATUS		status = OK;
    struct rusage	pstat;

    if (getrusage(RUSAGE_SELF, &pstat) != -1)
    {
        stat->tmperf_utime.TM_secs   = pstat.ru_utime.tv_sec;
        stat->tmperf_utime.TM_msecs  = pstat.ru_utime.tv_usec / MICRO_PER_MILLI;
        stat->tmperf_stime.TM_secs   = pstat.ru_stime.tv_sec;
        stat->tmperf_stime.TM_msecs  = pstat.ru_stime.tv_usec / MICRO_PER_MILLI;
        stat->tmperf_maxrss          = pstat.ru_maxrss;
        stat->tmperf_idrss           = pstat.ru_idrss;
        stat->tmperf_minflt          = pstat.ru_minflt;
        stat->tmperf_majflt          = pstat.ru_majflt;
        stat->tmperf_nswap           = pstat.ru_nswap;
        stat->tmperf_reads           = pstat.ru_inblock;
        stat->tmperf_writes          = pstat.ru_oublock;
        stat->tmperf_msgsnd          = pstat.ru_msgsnd;
        stat->tmperf_msgrcv          = pstat.ru_msgrcv;
        stat->tmperf_nsignals        = pstat.ru_nsignals;
        stat->tmperf_nvcsw           = pstat.ru_nvcsw;
        stat->tmperf_nivcsw          = pstat.ru_nivcsw;

	/* derived fields */

	stat->tmperf_cpu.TM_secs = 
		stat->tmperf_utime.TM_secs + stat->tmperf_stime.TM_secs;
	stat->tmperf_cpu.TM_msecs = 
		stat->tmperf_utime.TM_msecs + stat->tmperf_stime.TM_msecs;
        stat->tmperf_dio = stat->tmperf_reads + stat->tmperf_writes;
        stat->tmperf_msgtotal = stat->tmperf_msgsnd + stat->tmperf_msgrcv;
    }
    else
    {
	status = default_perfstat(stat, sys_err);
    }

    return(status);
}

#endif /* xCL_003_GETRUSAGE_EXISTS */

#ifdef xCL_PROCFS_EXISTS

#include <sys/signal.h>
#include <sys/fault.h>
#include <sys/syscall.h>
#include <sys/procfs.h>
#include <sys/stat.h>
#include <fcntl.h>

/*{
** Name: procfs_perfstat()   - "/proc" implementation of TMperfstat()
**
** Description:
**      Implementation of TMperfstat() on machines with "/proc" filesystem
**	support.  On these machines one "open()'s" the file "/proc/NNNN",
**	where "NNNN" is the pid of the process of interest, and then performs
**	ioctl()'s on the file descriptor in order to get information about the
**	process - see "man proc" for more information.
**
**      See TMperfstat() for function details.
**
** Inputs:
**      stat                            pointer to a TM_PERFSTAT structure.
**
** Outputs:
**      stat                            TM_PERFSTAT structure is filled in by
**                                      this routine with available OS
**                                      statistics.  Unavailable fields will be
**                                      set to -1.
**      sys_err
**
**      Returns:
**          OK - success.
**          TM_NOSUPPORT - no support for gathering system statistics.
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      26-jul-93 (mikem)
**         Created.
**	23-aug-93 (mikem)
**	   Changes to make TMperfstat() work when called from a server started
**	   by root.  In order to open the /proc/pid file the process's effective
**	   id must have read permission for both the /proc/pid file and the 
**	   actual executable file.  In the case of the server started by root
**	   change the effective id of the server to "root", open the file, and
**	   then reestablish the original effective uid.
**	06-jun-1995 (canor01)
**	   protect static fd with semaphore
*/
static STATUS
procfs_perfstat(
    TM_PERFSTAT *stat,
    CL_SYS_ERR  *sys_err)
{
    static int		fd = -1;
    pid_t		pid;
    prpsinfo_t		prpsinfo;
    prusage_t		prusage;
    STATUS		status = OK;

    if (fd == -1)
    {
	char	pid_fname[sizeof("/proc/") + sizeof("NNNNNN")];

	/* open current process "file" - /tmp/pid */
	pid = getpid();

	STprintf(pid_fname, "%s%d", "/proc/", pid);

	if ((fd = open(pid_fname, O_RDONLY)) < 0)
	{
	    SETCLERR(sys_err, 0, ER_open);

#if defined(xCL_SETEUID_EXISTS)

	    if (errno == EACCES)
	    {
		/* one valid way to get into this situation is if a process's
		** real uid is different than it's effective id.  The /proc/xxx
		** file is owned by the real uid of the process and is mode
		** 600, so only processes whose effective id is the same as
		** their real uid can open this file.  Try switching the
		** effective to the real and doing the open() again, making
		** sure to restore the uid on the way out.  This situation
		** happens when dbms servers are started by "root".
		*/
		uid_t	real_uid;
		uid_t   effective_uid;

		real_uid = getuid();
		effective_uid = geteuid();

		if (real_uid != effective_uid)
		{
		    /* try setting effective uid to real and do the open */
		    seteuid(real_uid);

		    if ((fd = open(pid_fname, O_RDONLY)) < 0)
		    {
			SETCLERR(sys_err, 0, ER_open);
		    }

		    /* set the uid back */
		    seteuid(effective_uid);
		}
	    }
#endif

	    if (fd < 0)
	    {
		return(FAIL);
	    }
	}
    }

    /* process usage information */

    MEfill(sizeof(prusage), 0, (PTR) &prusage);

    if (ioctl(fd, PIOCUSAGE, (void *) &prusage) == 0)
    {
	stat->tmperf_utime.TM_secs = prusage.pr_utime.tv_sec;
	stat->tmperf_utime.TM_msecs = 
			    (prusage.pr_utime.tv_nsec / NANO_PER_MILLI);
	stat->tmperf_stime.TM_secs = prusage.pr_stime.tv_sec;
	stat->tmperf_stime.TM_msecs = 
			    (prusage.pr_stime.tv_nsec / NANO_PER_MILLI);
	stat->tmperf_minflt = prusage.pr_minf;
	stat->tmperf_majflt = prusage.pr_majf;
	stat->tmperf_nswap = prusage.pr_nswap;
	stat->tmperf_reads = prusage.pr_inblk;
	stat->tmperf_writes = prusage.pr_oublk;
	stat->tmperf_msgsnd = prusage.pr_msnd;
	stat->tmperf_msgrcv = prusage.pr_mrcv;
	stat->tmperf_nsignals = prusage.pr_sigs;
	stat->tmperf_nvcsw = prusage.pr_vctx;
	stat->tmperf_nivcsw = prusage.pr_ictx;

        /* derived fields */
 
        stat->tmperf_cpu.TM_secs = 
                stat->tmperf_utime.TM_secs + stat->tmperf_stime.TM_secs;
        stat->tmperf_cpu.TM_msecs =
                stat->tmperf_utime.TM_msecs + stat->tmperf_stime.TM_msecs;
        stat->tmperf_dio = stat->tmperf_reads + stat->tmperf_writes;
        stat->tmperf_msgtotal = stat->tmperf_msgsnd + stat->tmperf_msgrcv;
    }
    else
    {
	SETCLERR(sys_err, 0, ER_ioctl);
	status = FAIL;
    }

    /* process status information */

    MEfill(sizeof(prpsinfo), 0, (PTR) &prpsinfo);

    if (ioctl(fd, PIOCPSINFO, (void *) &prpsinfo) == 0)
    {
	/* pr_size is actually the process image size */
	stat->tmperf_maxrss = prpsinfo.pr_bysize;
	stat->tmperf_idrss = prpsinfo.pr_byrssize;
    }
    else
    {
	SETCLERR(sys_err, 0, ER_ioctl);
	status = FAIL;
    }

    return(status);
}

#endif /* xCL_PROCFS_EXISTS */

#ifdef vms
#include	<jpidef.h>
#include	<ssdef.h>
#include	<syidef.h>


/*{
** Name: vms_perfstat()   - VMS implementation of TMperfstat()
**
** Description:
**      Implementation of TMperfstat() on VMS.
**
**	The current implementation supports the following structure members:
**		tmperf_cpu
**		tmperf_maxrss
**		tmperf_idrss
**		tmperf_majflt
**		tmperf_dio
**		tmperf_msgtotal
**	all other members are set to -1.
**
**	It is believed that this implementation should run on both vms vax 
**	and vms alpha.
**
**      See TMperfstat() for function details.
**
** Inputs:
**      stat                            pointer to a TM_PERFSTAT structure.
**
** Outputs:
**      stat                            TM_PERFSTAT structure is filled in by
**                                      this routine with available OS
**                                      statistics.  Unavailable fields will be
**                                      set to -1.
**      sys_err
**
**      Returns:
**          OK - success.
**          TM_NOSUPPORT - no support for gathering system statistics.
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      26-jul-93 (mikem)
**         Created.
*/
static STATUS
vms_perfstat(
    TM_PERFSTAT *stat,
    CL_SYS_ERR  *sys_err)
{
    $SYIDEF	syi_itmlst[2];
    int		system_status;
    STATUS	status = OK;
    static int	bytes_per_page = -1;
    $JPIDEF     jpiget[] = { 
        /* JPI$_CPUTIM - proc's accumulated CPU time in 10-millisecond ticks */
        {4, JPI$_CPUTIM, &stat->tmperf_cpu.TM_msecs, 0},

	/* JPI$_VIRTPEAK - the peak virtual address size of the process */
        {4, JPI$_VIRTPEAK, &stat->tmperf_maxrss, 0},

	/* JPI$_PPGCNT - number of pages the process has in the working set */
        {4, JPI$_PPGCNT, &stat->tmperf_idrss, 0},

	/* JPI$_PAGEFLTS - the number of page faults incurred by the process */
        {4, JPI$_PAGEFLTS, &stat->tmperf_majflt, 0},

	/* JPI$_DIRIO - the number of direct I/O operations of the process */
        {4, JPI$_DIRIO, &stat->tmperf_dio, 0},

	/* JPI$_BUFIO - the number of buffered I/O operations of the process */
        {4, JPI$_BUFIO, &stat->tmperf_msgtotal, 0},

	/* end of list */
	{0, 0, 0, 0} };

    /* set everything we won't be using to -1 */
    default_perfstat(stat, sys_err);

    if (bytes_per_page == -1)
    {
#ifdef  alpha
	/* determine number of bytes per page - only need to do once, use
	** system call so that on it works on both vax vms and alpha vms
	*/

	syi_itmlst[0].syi$w_buflen = 4;
	syi_itmlst[0].syi$w_itemcode = SYI$_PAGE_SIZE;
	syi_itmlst[0].syi$a_bufadr = &bytes_per_page;
	syi_itmlst[0].syi$a_reslen = 0;

	syi_itmlst[1].syi$w_buflen = 0;
	syi_itmlst[1].syi$w_itmcod = 0;
	syi_itmlst[1].syi$gt_bufadr = 0;
	syi_itmlst[1].syi$gw_reslen = 0;

	system_status = sys$getsyiw(0, 0, 0, syi_itmlst, 0, 0, 0);

	if ((system_status & 1) == 0)
	{
	    return(FAIL);
	}
#else
	bytes_per_page = 512;
#endif  /* alpha */
    }

    /* get the system statistics */

    system_status = sys$getjpi(0, 0, 0, &jpiget, 0, 0, 0);
    sys$waitfr(0);
    if (system_status != SS$_NORMAL)
    {
	status = FAIL;
    }
    else
    {
	/* adjust units of system stats */

	/* vms gives 10's of ms. we want just ms. */
	stat->tmperf_cpu.TM_msecs *= 10;

	/* vms give pages we want bytes */
	stat->tmperf_maxrss *= bytes_per_page;
	stat->tmperf_idrss *= bytes_per_page;
    }

    return(status);
}

#endif /* vms */

/*{
** Name: default_perfstat()   - default (null) implementation of TMperfstat()
**
** Description:
**	If no system support exists to gather performance statistics on the
**	machine then the following implementation is used.  This implementation
**	simply initializes all fields of the structure to -1.
**
** Inputs:
**      stat                            pointer to a TM_PERFSTAT structure.
**
** Outputs:
**      stat                            TM_PERFSTAT structure is filled in by
**                                      this routine with available OS
**                                      statistics.  Unavailable fields will be
**                                      set to -1.
**
**      sys_err				system specific error.
**
**      Returns:
**          OK - success.
**          TM_NOSUPPORT - no support for gathering system statistics.
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      26-jul-93 (mikem)
**         Created.
*/
/* ARGSUSED */
static STATUS
default_perfstat(
    TM_PERFSTAT *stat,
    CL_SYS_ERR  *sys_err)
{
    /* assign -1 to all fields */

    *stat = default_tmperf;

    return(TM_NOSUPPORT);
}
