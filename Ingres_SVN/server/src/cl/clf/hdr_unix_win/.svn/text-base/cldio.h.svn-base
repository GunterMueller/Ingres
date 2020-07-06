/*
** Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: CLDIO.H - Used for the CL dio abstraction.
**
** Description:
**	Prototypes for the CL dio abstraction.
**
**	30-November-1992 (rmuth)
**	    Created.
**	05-Nov-1997 (hanch04)
**	    Added large file system defines
**	21-jul-1997 (canor01)
**          Add DIFILE_FD macro to limit DI file descriptors to numbers
**          above those used by stream IO (parallels CLPOLL_FD macro in
**          clpoll.h c.f.).
**      21-jan-1999 (hanch04)
**          replace nat and longnat with i4
**	08-Jun-1999 (jenjo02)
**	    Added IIdio_writev() prototype.
**      28-May-2002 (hanal04) Bug 107686 INGSRV 1764
**          Moved prototype for dodirect() from cldio.c so that
**          DIalloc() can use it to determine whether we need to
**          redirect the allocation request to DIgalloc().
**/

/*
**  Forward and/or External typedef/struct references.
*/
FUNC_EXTERN	int 	IIdio_open(
				char *file,
				int mode,
				int perms,
				int pagesize,
				u_i4 *fprop,
				CL_ERR_DESC *err);

FUNC_EXTERN 	int 	IIdio_write(
				int fd,
				char *buf,
				int len,
				OFFSET_TYPE off,
				OFFSET_TYPE *loffp,
				u_i4 fprop,
				CL_ERR_DESC *err);

FUNC_EXTERN 	int 	IIdio_writev(
				int fd,
				char *iov,
				int iovcnt,
				OFFSET_TYPE off,
				OFFSET_TYPE *loffp,
				u_i4 fprop,
				CL_ERR_DESC *err);

FUNC_EXTERN 	int 	IIdio_read(
				int fd,
				char *buf,
				int len,
				OFFSET_TYPE off,
				OFFSET_TYPE *loffp,
				u_i4 fprop,
				CL_ERR_DESC *err);

FUNC_EXTERN		int IIdio_alloc_aligned(
				int fd,
				int len,
				SIZE_TYPE *apages,
				char **am,
				char *fn);

FUNC_EXTERN		int IIdio_alloc_realign(
				int fd,
				int len,
				SIZE_TYPE *apages,
				char *m,
				char **am,
				u_i4 fprop,
				char *fn);

FUNC_EXTERN 	int dodirect( VOID );

FUNC_EXTERN	OFFSET_TYPE IIdio_get_file_eof(
				i4   fd,
				u_i4 fprop);

/*
** Name: FPROP_* - macros for file properties
**
** Description:
**    A collection of macros and function definitions to enable
**    per file properties.
**
**    Properties include:
**      alignment attributes, required for O_DIRECT and RAW
**      file extension, give hints how to extend the file
**      RAW access is being done on the file
**      O_DIRECT access is being done on the file
**
** 01-apr-2004 (fanch01)
**    Created.
**	14-Oct-2005 (jenjo02)
**	    Chris's file descriptor properties now cached in io_fprop
**	    (file properties) and established on the first open, 
**	    not every open.
**	    Macro names, defines changed from "FDPR_" to "FPROP_"
**	    to reflect this distinction.
**	    DIRECTIO is now requested on individual files
**	    via DIopen(DI_DIRECTIO_MASK), rather than defaulting
**	    to "all" files.
**
*/
/* file descriptor properties associated with files */
/* properties have been checked and set; what is, is */
#define FPROP_OPEN 0x80000000
/* raw flag */
#define FPROP_RAW 0x40000000
/* DIRECT_IO is used, really */
#define FPROP_DIRECT 0x20000000
/* DIRECT_IO requested by DIopen */
#define FPROP_DIRECT_REQ 0x10000000
/* perform a fsync() after allocating pages to force metadata flush */
#define FPROP_SYNCEXTEND 0x08000000
	
/** allocation strategies:
 * VIRT - virtual, let later writes extend the file
 * FWWR - write forward (until file is extended to size)
 * EXTD - extend (only write new last page)
 * EXWR - extend (write last page) and fill with zeros
 */
#define FPROP_ALLOCSTRATEGY_VIRT 0
#define FPROP_ALLOCSTRATEGY_TRNC 1
#define FPROP_ALLOCSTRATEGY_EXTD 2
#define FPROP_ALLOCSTRATEGY_RESERVED 3
	
/* mask for 2^pwr alignment size */
#define FPROP_ALIGN_POWERMASK 0x01f
	
/* flag and value manipulation */
#define FPROP_CLEAR(fprop) (fprop = 0)
#define FPROP_COPY(s_fprop,t_fprop) (t_fprop = s_fprop)
#define FPROP_OPEN_SET(fprop) (fprop |= FPROP_OPEN)
#define FPROP_OPEN_CHECK(fprop) (fprop & FPROP_OPEN)
#define FPROP_RAW_SET(fprop) (fprop |= FPROP_RAW)
#define FPROP_RAW_CHECK(fprop) (fprop & FPROP_RAW)
#define FPROP_DIRECT_SET(fprop) (fprop |= FPROP_DIRECT)
#define FPROP_DIRECT_CHECK(fprop) (fprop & FPROP_DIRECT)
#define FPROP_DIRECT_REQ_SET(fprop) (fprop |= FPROP_DIRECT_REQ)
#define FPROP_DIRECT_REQ_CHECK(fprop) (fprop & FPROP_DIRECT_REQ)
#define FPROP_DIRECT_REQ_RESET(fprop) (fprop &= ~FPROP_DIRECT_REQ)
#define FPROP_ALIGN_MAXALIGN (1 << FPROP_ALIGN_POWERMASK)
#define FPROP_ALIGN_SETPOWER(fprop, b) (fprop |= ((b) & FPROP_ALIGN_POWERMASK))
#define FPROP_ALIGN_GETPOWER(fprop) (fprop & FPROP_ALIGN_POWERMASK)
#define FPROP_ALIGN_GETBYTES(fprop) (1 << FPROP_ALIGN_GETPOWER(fprop))
#define FPROP_ALIGN_GETMASK(fprop) (FPROP_ALIGN_GETBYTES(fprop) - 1)
#define FPROP_ALIGN_CHECK(fprop, m) (!(FPROP_ALIGN_GETMASK(fprop) & (int) m))
#define FPROP_ALLOCSTRATEGY_SET(fprop, s) (fprop |= ((s & 0x3) << 8))
#define FPROP_ALLOCSTRATEGY_GET(fprop) ((fprop >> 8) & 0x3)

/*
** Name: DIFILE_FD() - macros for renumbering fds for use with DI.
**
** Description:
**	Certain implementations of the CL allow both SI and DI 
**	functions to be mixed within the same server process.  SI
**	is built on stream i/o functions that are limited to use
**	of file descriptors between 0 and 255 inclusive.  To 
**	reserve as many of these file descriptors for use by SI,
**	DI file descriptors should be allocated >255.
**
**      To ensure low numbered fds are available we follow all DI calls
**      which allocate a fd with a call to the DIFILE_FD() macro.
**
**      A sample use is:
**              fd = open(...);
**              DIFILE_FD( fd );
**
**      This macro dups the fd to one above STREAM_RESVFD if the fd is
**      below STREAM_RESVFD, closing the old fd and setting fd to the
**      new one.
**
** History:
**	21-jul-1997 (canor01)
**	    Created.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# if defined(xCL_RESERVE_STREAM_FDS)

static int IIdio_file_fd( int fd );

# define STREAM_RESVFD  255

# define        DIFILE_FD( fd ) ( fd = IIdio_file_fd( fd ) )

# else /* xCL_RESERVE_STREAM_FDS */

# define        DIFILE_FD( fd )

# endif /* xCL_RESERVE_STREAM_FDS */
