/*
** Copyright (c) 2004 Ingres Corporation
**
*/

#include   <compat.h>
#include   <gl.h>
#include   <clconfig.h>
#include   <systypes.h>
#include   <fdset.h>
#include   <er.h>
#include   <cs.h>
#include   <csev.h>
#include   <di.h>
#include   <dislave.h>
#include   <cldio.h>
#include   "dilocal.h"
#include   "dilru.h"

/* unix includes */

#ifdef	    xCL_006_FCNTL_H_EXISTS
#include    <fcntl.h>
#endif      /* xCL_006_FCNTL_H_EXISTS */

#ifdef      xCL_007_FILE_H_EXISTS
#include    <sys/file.h>
#endif	    /* xCL_007_FILE_H_EXISTS */

#include    <errno.h>

/**
**
**  Name: DIALLOC.C - This file contains the UNIX DIallocate() routine.
**
**  Description:
**      The DIALLOC.C file contains the DIallocate() routine.
**
**        DIalloc - Allocates pages to a file.
**
** History:
**      26-mar-87 (mmm)    
**          Created new for 5.0.
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.      
**      Revision 1.3  87/11/05  09:57:19  mikem
**      fixed alloc vs. real eof problems.
**      30-mar-88 (anton)
**          Di in slave
**	17-oct-88 (jeff)
** 	    new DI lru calls - pinning of open files in a multi-thread 
**	    environment.
**	24-jan-89 (roger)
** 	    Make use of new CL error descriptor.
**	06-feb-89 (mikem)
**	    better support for DI CL_ERR_DESC, including initializing to zero
**	    and errors passed back from DIlru_open().
**	27-Feb-1989 (fredv)
**	    Replaced xDI_xxx by corresponding xCL_xxx defs in clconfig.h.
**	    Include <systypes.h>.
**	23-mar-89 (mikem)
**	    update io_system_eof when necessary (bug 4854).
**	03-aug-89 (mikem)
**	    Added debugging to stress the system's handling of out of disk
**	    space.  Define xOUT_OF_DISK_SPACE_TEST to get this code compiled in.
**	01-Oct-90 (anton)
**	    Changes for reentrant CL
**	15-nov-1991 (bryanp)
**	    Moved "dislave.h" to <dislave.h> to share it with LG.
**	30-October-1992 (rmuth)
**	    - Remove all references to file type DI_ZERO_ALLOC, this flag
**	      would cause us to physically allocate space on disc. The
**	      client is now required to call DIgalloc for this behaviour.
**	    - Prototype code.
**	    - Add xDEV_TST message to non-slave part of code.
**	30-nov-1992 (rmuth)
**	    Various
**	     - include <cldio.h>
**	     - Call IIdio_get_file_eof instead of lseek as this can
**	       deal with both raw and ordinary files. Change some
**	       types accordingly.
**	     - DIlru error checking changes.
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	17-aug-1993 (tomm)
**		Must include fdset.h since protypes being included reference
**		fdset.
**	19-apr-95 (canor01)
**	    added <errno.h>
**	23-jul-1996 (canor01)
**	    Semaphore protect the lseek/write combination when used with
**	    operating-system threads.
**	18-Dec-1997 (jenjo02)
**	    DI cleanup. Removed GLOBALREFs, changed Di_alloc_count to static.
**	11-Nov-1999 (jenjo02)
**	    Use CL_CLEAR_ERR instead of SETCLERR to clear CL_ERR_DESC.
**	    The latter causes a wasted ___errno function call.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    28-May-2002 (hanal04) Bug 107686 INGSRV 1764
**          Resolve Sequent DIRECT I/O problems by redirecting DIalloc()
**          calls to use DIgalloc() instead.
**/


/*
**  Forward and/or External function references.
*/


/*
** Definition of all global variables owned by this file.
*/


/* 
** count of DIalloc() for debugging 
*/
static i4             Di_alloc_count ZERO_FILL; 


/*{
** Name: DIalloc - Allocates a page to a direct access file.
**
** Description:
**      The DIalloc routine is used to add pages to a direct
**      access file.  This routine can add more than one page
**      at a time by accepting a count of the number of pages to add.
**   
**      The end of file and allocated are not updated on disk until a DIflush
**      call is issued.  This insures that pages are not considered valid 
**      until after they are formatted.  The allocation can be ignored if
**      the file is closed or the system crashes before the DIflush.
**
** Inputs:
**      f                Pointer to the DI file
**                       context needed to do I/O.
**      n                The number of pages to allocate.
**
** Outputs:
**      page             Pointer to variable used to 
**                       return the page number of the
**                       first page allocated.
**      err_code         Pointer to a variable used
**                       to return operating system 
**                       errors.
**    Returns:
**        OK
**        DI_BADINFO	    	Could not find current end-of-file.
**        DI_BADFILE        	Bad file context.
**        DI_EXCEED_LIMIT   	Too many open files.
**
**	  A range of errors from unexpected errors in DIlru code.
**
**        If running with slaves  :
**        DI_BADCAUSE_EVENT     Error sending event to slaves.
**        DI_BAD_SUSPEND        Error suspending thread.
**	  DI_LRU_CSCAUSE_EV_ERR Error reserving an event.
**
**
**    Exceptions:
**        none
**
** Side Effects:
**        none
**
** History:
**    11-sep-85 (jennifer)
**          Created new for 5.0.
**    20-mar-86 (jennifer)
**          Changed i4 to i4  and err_code to CL_ERR_DESC to meet 
**          CL coding standard.
**    05-mar-87 (mmm)
**	    ported to UNIX.
**    06-feb-89 (mikem)
**	    better support for DI CL_ERR_DESC, including initializing to zero
**	    and errors passed back from DIlru_open().
**    23-mar-89 (mikem)
**	    update io_system_eof when necessary (bug 4854).
**    03-aug-89 (mikem)
**	    Added debugging to stress the system's handling of out of disk
**	    space.  We will force DIwrite() to return "DI_EXCEEED_LIMIT" once
**	    every 10 disk space allocations.
**    30-October-1992 (rmuth)
**	    - Remove all references to file type DI_ZERO_ALLOC, this flag
**	      would cause us to physically allocate space on disc. The
**	      client is now required to call DIgalloc for this behaviour.
**	    - Prototype code.
**	    - Add xDEV_TST message to non-slave part of code.
**    30-nov-1992
**	    - Call IIdio_get_file_eof instead of lseek as this can
**	      deal with both raw and ordinary files. Change some
**	      types accordingly.
**	    - This routine can now return more than just DI_xxx error
**	      messages but these messages are only when a very 
**	      unexpected error has occured.
**    23-jul-1996 (canor01)
**	    Semaphore protect the lseek/write combination when used with
**	    operating-system threads.
**    28-May-2002 (hanal04) Bug 107686 INGSRV 1764
**          We must use DIgalloc() if we are using Direct I/O for
**          Sequent. Failure to do so will generate sparse files
**          which will cause DirectRead() to return EINVAL when
**          it tries to read a hole.
**    02-Apr-2004 (fanch01)
**      Extend the file with the strategy handy recommends.  Useful
**      on Linux which supports different filesystems with different
**      extend performance characteristics.
**    13-apr-04 (toumi01)
**	Move stack variable declarations to support "standard" C compilers.
**	14-Oct-2005 (jenjo02)
**	    Chris's file descriptor properties now cached in io_fprop
**	    (file properties) and established on the first open, 
**	    not every open.
**	    Removed Sequent-specific code, deprecated platform.
**
** Design Details:
**
**	UNIX DESIGN:
**
**	This routine allows the user to extend the logical end-of-file
**	by recording the new file size in the DI_IO structure.
**	
**	This routine performs only a logical extension of the file, by
**	recording the current allocation size in the DI_IO structure.
**
**	If the system is running with slave process use the CS event system 
**	to drive a slave process to do actual file I/O, otherwise do inline.
**
**	For more detailed information on DI see di.h
*/
STATUS
DIalloc(
    DI_IO         *f,
    i4	   n,
    i4	  *page,
    CL_ERR_DESC    *err_code )
{
    i4	end_of_file;
    DI_OP	diop;
    register 	DI_SLAVE_CB *disl;
    STATUS	status = OK;
    SIZE_TYPE	apages;
    i4		bytes_per_page, cpages;
    CL_ERR_DESC	err;
    char	*abuf;

    CL_CLEAR_ERR( err_code );

    /* Check file control block pointer, return if bad. */

    if (f->io_type != DI_IO_ASCII_ID)
	return(DI_BADFILE);

    /* Count another allocate */
    f->io_stat.alloc++;

    /* get file descriptor for this file */
	status = DIlru_open( f, FALSE, &diop, err_code);
	if ( status != OK )
		return(status);
	/*
	** DI_sense updates f->io_alloc_eof with the protection
	** of io_sem (OS_THREADS), so there's no need to
	** duplicate that update here.
	*/
	status = DI_sense( f, &diop, &end_of_file, err_code );

	/*
	** If we did not find out the information then do not
	** update the relevent fields.
	*/
	if (status != OK )
	{
		CL_ERR_DESC lerr_code;
		(VOID) DIlru_release(&diop, &lerr_code);
		return(status);
	}

# ifdef OS_THREADS_USED
	CSp_semaphore( TRUE, &f->io_sem );
# endif /* OS_THREADS_USED */

# ifdef LNX
	/* f->io_alloc_eof will be -1 for zero length files... */
	apages, bytes_per_page = f->io_bytes_per_page, cpages = f->io_alloc_eof;
	/* perform the extend based on fd's strategy */
	switch (FPROP_ALLOCSTRATEGY_GET(f->io_fprop))
	{
		case FPROP_ALLOCSTRATEGY_VIRT:
			/* original behavior, "virtually" update our view of EOF */
			break;
		case FPROP_ALLOCSTRATEGY_TRNC:
			/* use ftruncate() to extend the file */
			/* don't bother if only extending one page, the write will take care of that */
			if (n > 1)
			{
				/* the one fixup is because cpages is the "last page", -1 if none */
				OFFSET_TYPE nsize = (cpages + n + 1) * (OFFSET_TYPE) bytes_per_page;
# ifdef LARGEFILE64
				int err = ftruncate64 (diop.di_fd, nsize);
# else /*  LARGEFILE64 */
				int err = ftruncate (diop.di_fd, nsize);
# endif /* LARGEFILE64 */
				if ( err )
				{
					SETCLERR(err_code, 0, ER_write);
					switch( err_code->errnum )
					{
						case EFBIG:
							status = DI_BADEXTEND;
							break;
						case ENOSPC:
							status = DI_EXCEED_LIMIT;
							break;
						default:
							status = DI_BADWRITE;
							break;
					}
					TRdisplay("DIalloc: failed to truncate extend file: %t/%t, alloc_eof: %d, first: %d, count: %d\n",
							  f->io_l_pathname, f->io_pathname,
							  f->io_l_filename, f->io_filename,
							  f->io_alloc_eof, *page, n);
				}
			}
			break;
		case FPROP_ALLOCSTRATEGY_EXTD:
			/* seek and write only to last newly allocated page */
			status = IIdio_alloc_aligned(diop.di_fd, bytes_per_page, &apages, &abuf, __FUNCTION__);
			if (OK == status)
			{
				/* don't bother if only extending one page, the write will take care of that */
				if (n > 1)
				{
					OFFSET_TYPE lseek_offset = (cpages + n) * (OFFSET_TYPE) bytes_per_page;
# ifdef LARGEFILE64
					ssize_t bytes_written = pwrite64( (int)diop.di_fd, abuf, bytes_per_page, lseek_offset );
# else /*  LARGEFILE64 */
					ssize_t bytes_written = pwrite( (int)diop.di_fd, abuf, bytes_per_page, lseek_offset );
# endif /* LARGEFILE64 */
					if ( bytes_written != bytes_per_page )
					{
						SETCLERR(err_code, 0, ER_write);
						switch( err_code->errnum )
						{
							case EFBIG:
								status = DI_BADEXTEND;
								break;
							case ENOSPC:
								status = DI_EXCEED_LIMIT;
								break;
							default:
								status = DI_BADWRITE;
								break;
						}
						TRdisplay("DIalloc: failed to write extend file: %t/%t, alloc_eof: %d, first: %d, count: %d\n",
								  f->io_l_pathname, f->io_pathname,
								  f->io_l_filename, f->io_filename,
                                  f->io_alloc_eof, *page, n);
					}
				}
				MEfree_pages(abuf, apages, &err);
			}
			break;
		case FPROP_ALLOCSTRATEGY_RESERVED:
			/* reserved, here only to provide a fullcase select */
			/* reasonable use is for replacing dodirect(), DIgalloc() */
			break;
	}
# endif

	if (status == OK)
	{
		/*
		** Return page number of first page allocated
		*/
		*page = (i4) (f->io_alloc_eof + 1);

		/*
		** Update the current allocated end-of-file
		*/
		f->io_alloc_eof = f->io_alloc_eof + n;

		/* this can fail and still indicate an error to the caller */
		status = DIlru_release(&diop, err_code);
	}
	else
	{
		/* the extend failed, propagate error back to caller */
		(VOID) DIlru_release(&diop, err_code);
	}

# ifdef OS_THREADS_USED
	CSv_semaphore( &f->io_sem );
# endif /* OS_THREADS_USED */

#ifdef xDEV_TST
	TRdisplay("DIalloc: file: %t/%t, alloc_eof: %d, first: %d, count: %d\n",
    		  f->io_l_pathname, f->io_pathname,
	      	  f->io_l_filename, f->io_filename,
		  f->io_alloc_eof, *page, n);
#endif
   
# ifdef xOUT_OF_DISK_SPACE_TEST
    	if ((++Di_alloc_count % 10) == 0)
        {
	    f->io_open_flags |= DI_O_NODISKSPACE_DEBUG;
	    TRdisplay("DIalloc() setting bit so DIwrite call will return \n");
	    TRdisplay("false DI_EXCEED_LIMIT. Adding page %d\n", *page);
    	}
# endif /* xOUT_OF_DISK_SPACE_TEST */

    return( status );
}
