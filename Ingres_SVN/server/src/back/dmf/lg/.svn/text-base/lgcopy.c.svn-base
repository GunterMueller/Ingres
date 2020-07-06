/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <di.h>
#include    <er.h>
#include    <me.h>
#include    <pc.h>
#include    <cx.h>
#include    <sl.h>
#include    <st.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <tr.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <lg.h>
#include    <lgdef.h>
#include    <lgdstat.h>

/**
**
**  Name: LGCOPY.C - Implements the LGcopy function of the logging system
**
**  Description:
**	This module contains the code which implements LGcopy.
**	
**	    LGcopy -- Copy one logfile to another (for dual logging usage)
**
**  History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	17-mar-1993 (rogerk)
**	    Moved lgd status values to lgdstat.h so that they are visible
**	    by callers of LGshow(LG_S_LGSTS).
**	21-jun-1993 (andys)
**	    Add nodename parameter so that rcpconfig on cluster can copy
**	    logfiles on other nodes. 
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (bryanp)
**	    Include <tr.h>
**	12-jun-1995 (canor01)
**	    semaphore protect memory allocation (MCT)
**      03-jun-1996 (canor01)
**          New ME for operating-system threads does not need external
**          semaphores.
**	10-oct-96 (mcgem01)
**	    E_CL0F31_LG_CANT_OPEN modified to include product name.
**	29-oct-1996 (canor01)
**	    Include <st.h> for definition of STlength().
**	26-Jan-1998 (jenjo02)
**	    Partitioned Log File Project:
**	    Replaced DI_IO references with LG_IO structures.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	30-may-2001 (devjo01)
**	    Add cx.h for CXcluster_enabled(). s103715.
**      7-oct-2004 (thaju02)
**          Use SIZE_TYPE to allow memory pools > 2Gig.
*/


/*{
** Name: LGcopy	- Copy from one log file to another.
**
** Description:
**      This routine copies the contents of one log file to another.
**
**	If the copy succeeds, we upgrade the headers of both logs to indicate
**	that both logs are active. Otherwise, we leave the header unchanged.
**
**	Currently, this operation can only be performed when the logging system
**	is OFFLINE. This is because we need to ensure that the logfile contents
**	do not change while we are processing the logfile. There are a variety
**	of possible algorithms which could allow us to re-activate dual logging
**	while the logging system is still online; someday we hope to implement
**	such a feature.
**
**	Only one process at a time should call this routine; this is currently
**	ensured by rcpconfig, which only calls this routine after successfullly
**	opening the logfile as the LG_MASTER.
**
** Inputs:
**	from_log			The log file to copy from.
**	    LG_PRIMARY_LOG		Copy from PRIMARY to DUAL
**	    LG_DUAL_LOG			Copy from DUAL to PRIMARY
**	nodename			Which logfile to use (cluster).
**					May be NULL for default node.
**
** Outputs:
**      sys_err                         Reason for error return status.
**	Returns:
**	    OK
**	    LG_CANTOPEN			Error opening log file.
**	    LG_READERROR		Error reading log file.
**	    LG_WRITEERROR               Error writing log.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	21-jun-1993 (andys)
**	    Add nodename parameter so that rcpconfig on cluster can copy
**	    logfiles on other nodes. 
*/
STATUS
LGcopy(
i4		    from_log,
char		    *nodename,
CL_ERR_DESC	    *sys_err)
{
    i4	    status;
    char	    file_name[LG_MAX_FILE][128];
    i4		    l_file_name[LG_MAX_FILE];
    char	    path[LG_MAX_FILE][128];
    i4		    l_path[LG_MAX_FILE];
    i4	    i;
    LG_IO	    from_logfile_lg_io;
    LG_IO	    to_logfile_lg_io;
    LG_IO	    *lg_read_io = &from_logfile_lg_io;
    LG_IO	    *lg_write_io = &to_logfile_lg_io;
    char	    *buffer;
    PTR		    mem;
    LG_HEADER	    header;
    i4	    last_to_page;
    i4	    last_from_page;
    i4		    num_pages;
    SIZE_TYPE	    alloc_pages;
    STATUS	    me_status;
    CL_ERR_DESC	    me_sys_err;
    i4	    err_code;
    i4	    blocks_partition;

    /* allocate the buffer */
    status = MEget_pages(ME_MZERO_MASK, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
			     (char *) NULL, &mem, &alloc_pages, sys_err);
    if (status)
    {
	return(LG_CANTINIT);
    }
    else
    {
	buffer = (char *) mem;
    }

    /* Open the "to" log file. */

    if (from_log == LG_PRIMARY_LOG)
	status = LG_build_file_name( LG_DUAL_LOG, nodename,
					file_name, l_file_name,
					path, l_path,
					&to_logfile_lg_io.di_io_count );
    else
	status = LG_build_file_name( LG_PRIMARY_LOG, nodename,
					file_name, l_file_name,
					path, l_path,
					&to_logfile_lg_io.di_io_count );

    for (i = 0, last_to_page = 0; 
	 i < to_logfile_lg_io.di_io_count && status == OK; 
	 i++)
    {
	status = DIopen(&to_logfile_lg_io.di_io[i], path[i], l_path[i], 
			file_name[i], l_file_name[i],
			LGD_MIN_BLK_SIZE, DI_IO_WRITE, 
			DI_SYNC_MASK|DI_LOG_FILE_MASK, sys_err);
	if (status)
	{
	    (VOID) ule_format(E_CL0F31_LG_CANT_OPEN, sys_err,
				ULE_LOG, NULL, 0, 0, 0, &err_code,
				3,
				0, file_name,
				0, 1,
				STlength(SystemProductName), SystemProductName);
	}
	else
	{
	    status = DIsense(&to_logfile_lg_io.di_io[i],
				    &blocks_partition, sys_err);
	    if (status == OK)
	    {
		/* All partitions must be the same size. */
		if (last_to_page == 0)
		    last_to_page = blocks_partition;
		else if (blocks_partition != last_to_page)
		    status = E_DMA810_LOG_PARTITION_MISMATCH;
	    }
	}
    }

    if (status)
    {
	(VOID) ule_format(status, sys_err, ULE_LOG, NULL, 0, 0, 0,
			    &err_code, 0);
	_VOID_ MEfree_pages(mem, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
			    &me_sys_err);
	return (status);
    }

    /* Open the active log if not yet opened. */

    status = LG_build_file_name( from_log, nodename,
					file_name, l_file_name,
					path, l_path,
					&from_logfile_lg_io.di_io_count);

    /* Both files must have the same number of partitions */
    if (status == OK &&
        to_logfile_lg_io.di_io_count != from_logfile_lg_io.di_io_count)
	status = E_DMA810_LOG_PARTITION_MISMATCH;

    for (i = 0, last_from_page = 0; 
	 i < from_logfile_lg_io.di_io_count && status == OK;
	 i++)
    {
	status = DIopen(&from_logfile_lg_io.di_io[i], path[i], l_path[i],
			file_name[i], l_file_name[i],
			LGD_MIN_BLK_SIZE, DI_IO_WRITE, 
			DI_SYNC_MASK|DI_LOG_FILE_MASK, sys_err);
	if (status)
	{
	    (VOID) ule_format(E_CL0F31_LG_CANT_OPEN, sys_err,
				ULE_LOG, NULL, 0, 0, 0, &err_code,
				2,
				0, file_name,
				0, -1);
	}
	else
	{
	    status = DIsense(&from_logfile_lg_io.di_io[i], 
				&blocks_partition, sys_err);
	    if (status == OK)
	    {
		if (last_from_page == 0)
		    last_from_page = blocks_partition;
		if (blocks_partition != last_from_page)
		    status = E_DMA810_LOG_PARTITION_MISMATCH;
	    }
	}
    }

    if (status)
    {
	(VOID) ule_format(status, sys_err, ULE_LOG, NULL, 0, 0, 0,
			    &err_code, 0);
	_VOID_ MEfree_pages(mem, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
			    &me_sys_err);
	return (status);
    }

    if (last_to_page != last_from_page)
    {
	/*
	** Log file sizes do not match. Something is rotten here.
	*/
	_VOID_ MEfree_pages(mem, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
			    &me_sys_err);
	return (LG_CANTOPEN);
    }

    /*
    ** Begin to copy from from_log to the inactive log. Loop through the
    ** log file reading and writing pages until we have copied the
    ** entire file. The total number of pages to copy is the number of
    ** pages per partition multiplied by the number of partitions.
    */

    for (i = 0; i < (last_to_page * to_logfile_lg_io.di_io_count); i ++)
    {
	num_pages = 1;
	status = LG_READ(lg_read_io, &num_pages, (i4)i,
			buffer, sys_err);
	if (status != OK || num_pages != 1)
	{
	    (VOID) ule_format(status, sys_err, ULE_LOG, NULL, 0, 0, 0,
				&err_code, 0);
	    _VOID_ MEfree_pages(mem, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
				&me_sys_err);
	    return (LG_READERROR);
	}

	status = LG_WRITE(lg_write_io, &num_pages, (i4)i,
			buffer, sys_err);
	if (status != OK || num_pages != 1)
	{
	    (VOID) ule_format(status, sys_err, ULE_LOG, NULL, 0, 0, 0,
				&err_code, 0);
	    _VOID_ MEfree_pages(mem, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
				&me_sys_err);
	    return (LG_WRITEERROR);
	}
    }

    /*
    ** Copy has succeeded. Upgrade the header of both logs to indicate that
    ** both logs are now active. (The header page is always the first page
    ** of the first partition).
    */
    num_pages = 1;
    status = DIread(&from_logfile_lg_io.di_io[0], &num_pages, (i4)0,
		    buffer, sys_err);
    if (status != OK || num_pages != 1)
    {
	(VOID) ule_format(status, sys_err, ULE_LOG, NULL, 0, 0, 0,
			    &err_code, 0);
	_VOID_ MEfree_pages(mem, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
			    &me_sys_err);
	return (LG_READERROR);
    }

    if (LGchk_sum(buffer, sizeof(LG_HEADER)) ==
				((LG_HEADER *)buffer)->lgh_checksum)
    {
	header = *(LG_HEADER *)buffer;
    }
    else
    {
	TRdisplay("LGcopy: Header of FROM file does not checksum.\n");
	_VOID_ MEfree_pages(mem, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
			    &me_sys_err);
	return (LG_READERROR);
    }

    header.lgh_active_logs = LGH_BOTH_LOGS_ACTIVE;

    status = LG_write_log_headers( &header, &from_logfile_lg_io,
				&to_logfile_lg_io, sys_err );

    /* free up the memory */

    me_status = MEfree_pages(mem, (LGD_MAX_BLK_SIZE / ME_MPAGESIZE),
			     &me_sys_err);
    if (me_status)
    {
	(VOID) ule_format(me_status, &me_sys_err, ULE_LOG, NULL, 0, 0, 0,
			    &err_code, 0);
	if (status == OK)
	{
	    status = LG_WRITEERROR;
	    STRUCT_ASSIGN_MACRO(me_sys_err, *sys_err);
	}
    }
    return (status);
}
