/*
**  Copyright (c) 1989, 2004 Ingres Corporation
*/

# include <bzarch.h>
# include <systypes.h>
# include <compat.h>
# include <gl.h>
# include <clconfig.h>
# ifdef axp_osf
# include <time.h>
# endif
# include <er.h>
# include <me.h>
# include <nm.h>
# include <cs.h>
# include <di.h>
# include <st.h>
# include <pm.h>
# include <errno.h>
# include <fcntl.h>
# include <seek.h>
# include <sys/stat.h>
# include <clnfile.h>
# include <cldio.h>
# ifdef	xCL_SUN_ASYNC_IO
#  include	<sys/asynch.h>
# endif
# ifdef LNX
/* Oracle OCFS[/2] magic number, file system type */
#  define OCFS_SUPER_MAGIC	0xa156f7eb
/* Redhat GFS magic number, file system type */
#  define GFS_SUPER_MAGIC	0x1161970
/* CFS Lustre magic number, file system type */
#  define LUSTRE_SUPER_MAGIC	0xbd00bd0
/* NFS magic number, oddly in an ifdef __KERNEL__ block */
#  define NFS_SUPER_MAGIC	0x6969
/* XFS magic number */
#  define XFS_SUPER_MAGIC	0x58465342	/* "XFSB" */
/* EXT3 magic number, file system type */
#  define EXT3_SUPER_MAGIC      0xef53
#  include <linux/fs.h>
#  if !defined(_I386_STATFS_H) && !defined(_X86_64_STATFS_H) && \
	!defined(_ASM_IA64_STATFS_H)
#   include <sys/vfs.h>
#  endif
#  include <sys/types.h>
#  include <sys/ioctl.h>
# endif

/**
**
**  Name: cldio.c -- possibly direct disk i/o for use within the CL.
**
**  Description:
**
**	This module provides a combined seek/read and seek/write
**	interface for disk i/o, mapping nearly directly to the raw UNIX
**	system calls.
**
**  Intended Uses:
**
**	This should be used by DI and LG, so that system specific i/o
**	enhancements such as synchronous direct i/o can be neatly
**	encapsulated with one set of ifdefs rather than spread all over.
**
**  Interface:
**
**    The file defines the following visible functions.
**
**	IIdio_open( file, mode, perms, err )
**
**	    Wrapper for UNIX open(2) system call, may do something system
**	    specific.
**
**	IIdio_read( fd, buf, len, off, loffp, err )
**
**	    This does a combined seek and read, possibly optimizing the
**	    seek if the current offset, stored at the location pointed to
**	    by loffp, is the same as the desired location.  The saved offset
**	    will be updated if needed.
**
**	IIdio_write( fd, buf, len, off, loffp, err )
**
**	    This does a combined seek and write, possibly optimizing the
**	    seek if the current offset, stored at the location pointed to
**	    by loffp, is the same as the desired location.  The saved offset
**	    will be updated if needed.
**
**	IIdio_writev( fd, buf, len, off, loffp, err )
**
**	    This does a combined seek and writev, possibly optimizing the
**	    seek if the current offset, stored at the location pointed to
**	    by loffp, is the same as the desired location.  The saved offset
**	    will be updated if needed.
**
**	IIdio_get_file_eof( fd )
**	    
**	    This finds a file eof, including raw files.  For most
**	    files on most systems this is a simple lseek, but raw files
**	    on some systems require a binary search.
**
**      And the private
** 
**	IIdio_compute_file_size( file_desc )
**
**          This does a binary search of the specified file to get its size.
**	    Used only by LG to sense end of raw log files.
**
**	IIdio_rw( op, fd, buf, len, off, loffp, err )
**
**	dolog()
**
**  History:
**	27-jun-89 (daveb)
**		Created.
**	11-july-89 (daveb)
**		Handle unaligned buffers for raw i/o.
**		Add IIdio_open(), changed names to IIdio_read and IIdio_write,
**		add CL_ERR_DESCs to the arguments.
**	22-jul-89 (daveb)
**		Sanity build revealed that a compiler that really wants
**		static char funct() forward ref to match the function
**		definition.  Add the static to getalbuf().
**	24-jul-89 (daveb)
**		don't try to do the Bmap Cache stuff if the symbol 
**		II_BMAP_CACHE isn't set to 'y'.  INGRES
**		would fall over on kernels that didn't support the system
**		calls; I had assumed that it would be a no-op to call them,
**		not fatal. oops.  
**
**		Change all symbols to look for 'y' as the "on" position.
**	2-aug-89 (kimman)
**		moved systypes.h top so it DECstation doesn't choke during
**		compiling 
**	22-jan-90 (mikem)
**		Copied over from NSE line of code into "termcl".
**	21-feb-90 (fredp)
**		Added xCL_081_SUN_FAST_OSYNC_EXISTS code.
**	16-Mar-90 (anton)
**		Changed DIO traceing to have several levels.
**	4-sep-90 (jkb)
**		define SQT_DIRECT_IO and IO_ALIGN for sqs_ptx
**	22-apr-91 (szeng)
**		Silencing warning about L_SET, L_INCR and L_XTND
**		redefined.
**	25-sep-1991 (mikem) integrated following change: 19-aug-91 (rogerk)
**		Don't align buffers before reading/writing unless acting on
**		a raw device.  Other read/write requests do not require
**		aligned buffers and copying them is pretty expensive.
**	10-dec-91 (jnash)
**		Include new IIdio_get_file_eof fn call to get end of
**		raw log files.  Used by LG, this routine is necessary 
**		cuz one can't always lseek to a raw file's eof.  Also added
**		IIdio_compute_file_size, called by former for raw logs.
**	05-Nov-1992 (mikem)
**	    su4_us5 port.  Changed direct calls to CL_NFILE to calls to new
**	    function iiCL_get_fd_table_size().
**	30-nov-1992 (rmuth)
**		Various changes.
**		 - Prototype code and add cldio.h
**		 - Change IIdio_get_file_eof 'fd' parameter to type int.
**      10-feb-1992 (smc)
**              Added time.h to define tm for axp_osf.
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	26-jul-1993 (mikem)
**	    Closed comment in D_MIN_DISK_BLK_SIZE define, produced warning on
**	    su4_us5.
**      01-sep-93 (smc)
**          Commented lint pointer truncation warning.
**	14-sep-1993 (pearl)
**	    In IIdio_rw(), if environmental variable II_DIO_LSEEK_CHK is set, 
**	    check saved offset value by calling lseek().  Log if the offset 
**	    is incorrect.  This can be used to debug E_DM934D_DM0P_BAD_PAGE 
**	    errors.
**	07-oct-1993 (tad)
**	    Bug #56449
**	    Changed %x to %p for pointer values in IIdio_write(), 
**	    IIdio_read() and IIdio_rw().
**	14-oct-1993 (tad/swm)
**	    Bug #58877
**	    If Posix asynchronous disk IO is supported (for regular files),
**	    then xCL_ASYNC_IO can be defined. 
**	    Added include file aio.h, and added aio calls into 
**	    IIdio_write() and IIdio_read() if xCL_ASYNC_IO is enabled.
**	    Caveats:
**	    Before enabling xCL_ASYNC_IO the following should be checked
**	    carefully:
**	    o Is it supported for regular files as well as raw devices?
**	    o Would other file operations - open(), close(), rename(),
**	      mknod(), creat() ...  block? (It is ok for these to block
**	      in a slave process not in a DBMS server).
**	    o Are there sufficent file descriptors available in a DBMS to
**	      support the required number of GCA connections and async.
**	      disk i/o?
**	    o Does it actually perform better?
**	06-jun-1995 (canor01)
**	    semaphore protect memory allocation routines and statics
**	    for MCT server
**	03-jun-1996 (canor01)
**	    clean up semaphore protection for operating system threads
**	    Replace Posix async I/O with Solaris async I/O.
**      29-aug-1997 (musro02)
**          In dodirect, allow II_DIRECT_IO to be y or Y.
**      23-sep-1997/01-Apr-1997 (bonro01/mosjo01)
**          System includes before <compat.h> and
**          <bzarch.h> caused problems because defines for
**          POSIX thread support did not get defined before
**          system header files were included.
**      01-Apr-1997 (bonro01)
**                   System includes before <compat.h> and
**                  <bzarch.h> caused problems because defines for
**                  POSIX thread support did not get defined before
**                  system header files were included.
**      24-sep-1997 (hweho01)
**          Remove the preprocessor directive stmts for systypes.h file.
**      05-Nov-1997 (hanch04)
**          Remove declare of lseek, done in seek.h
**          Added LARGEFILE64 for files > 2gig
**          Change i4 to OFFSET_TYPE
**	23-feb-1998 (canor01)
**	    Restore the DIFILE_FD macro, which was lost in last massive
**	    integration.
**	22-Dec-1998 (jenjo02)
**	    Removed use of static buffer (getalbuf()) for aligning raw data. 
**	    It wasn't thread safe (duh). 
**	    The onus is now on the raw data reader/writer
**	    to ensure that the data being written is properly aligned
**	    Defines of raw IO_ALIGN moved to dicl.h and renamed to
**	    DI_RAWIO_ALIGN so that everyone can know proper behavour.
**	10-may-1999 (walro03)
**	    Remove obsolete version string sqs_us5.
**	08-Jun-1999 (jenjo02)
**	    Added IIdio_writev() for GatherWrite.
**	    ifdef'd out check for aligned raw buffers. On Solaris, it
**	    doesn't seem to matter and I'm unaware of any platform
**	    on which it does...
**	11-Nov-1999 (jenjo02)
**	    Use CL_CLEAR_ERR instead of SETCLERR to clear CL_ERR_DESC.
**	    The latter causes a wasted ___errno function call.
**	25-feb-2000 (somsa01)
**	    Caught a couple of places where we should be typecasting the
**	    argiment to lseek64(). Also, use stat64() when LARGEFILE64
**	    is defined.
**	11-jul-2000 (somsa01 for jenjo01)
**	    ifdef'd out check for aligned raw buffers. On Solaris, it
**	    doesn't seem to matter and I'm unaware of any platform
**	    on which it does...
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	02-jan-2001 (mosjo01)
**	    by popular demand.. II_DIRECT_IO support for sqs_ptx has been 
**	    reinserted/reinstated (from Ingres 2.0). Since sqs_ptx currently
**	    DOES NOT use THREADS it is safe to use getalbuf(). It has been 
**	    demonstrated that sqs_ptx requires 512 for DI_RAWIO_ALIGN and in 
**	    most instances the higher level routines (ie, DI_SL_WRITE) will 
**	    pass an aligned buffer. Also it is necessary for sqs_ptx to rely
**	    on dodirect() inlieu of fisraw() to distingush when to use 
**	    Direct I/O system functions (ie, DirectRead and DirectWrite). 
**	    Considerable testing was performed that demonstrated that the code
**	    in cldio.c works with one minor exception and that is <temporary 
**	    tables> processed by gfault_page(). A change was made in dm0p.c
**	    to exit that function (defaulting back to single fault_page()) for
**	    <temporary tables> for xCL_064_SQT_DIRECT_IO only. 
**      28-feb-2001 (mosjo01)
**	    Emergency fix to my cross-integration from ingres25. I failed to 
**	    note changes to IIdio_rw now using D_READ, D_WRITE and D_WRITEV.
**	14-may-2002 (devjo01)
**	    Formalize support for Tru64's O_DIRECTIO flag.
**      28-May-2002 (hanal04) Bug 107686 INGSRV 1764
**          Made dodirect() available to Dialloc() by making it non-static
**          and adding the prototype to cldio.h
**	32-dec-2002 (hanch04)
**	    Fix cross integration, replace nat with i4
**	11-Mar-2005 (schka24)
**	    Add XFS to list of recognized Linux directio filesystems.
**  01-Apr-2004 (fanch01)
**      Add O_DIRECT support on Linux depending on the filesystem
**      properties, pagesize.  Fixups for misaligned buffers on read()
**      and write() operations.  Add generic support for more intelligently
**      performing some file system operations.
**  24-Jun-2005 (fanch01)
**      Remove warning if O_DIRECT mode fails.  Polyserve's PSFS masquerades
**      as OCFS and does not support O_DIRECT on fcntl(.. F_SETFL ..).
**      Hopefully a temporary workaround while Polyserve comes up with a
**      change of behavior on their end or a fast reliable method of detecting
**      the difference between the two.
**  07-Sep-2005 (fanch01)
**      Change sys/vfs.h include conditional to be more portable.
**      RedHat ES 3.0 unfortunately defined it's own struct statfs when
**      linux/fs.h was included.
**  29-Sep-2005 (hanje04)
**	Make similar sys/vfs.h change for AMD64 Linux. May need one for IA64
**	too, was hard to tell without building.
**  06-Oct-2005 (hanje04)
**	Yup, we need to conditionally include sys/vfs.h on IA64 Linux too.
**	14-Oct-2005 (jenjo02)
**	    Removed all Sequent-specific code, deprecated platform.
**	    Chris's file descriptor properties now cached in io_fprop
**	    (file properties) and established on the first open, 
**	    not every open.
**	    DIRECTIO is now requested on a per-file basis via
**	    DIopen(DI_DIRECTIO_MASK) rather than brute-force
**	    "all" files.
**	    Whether the server is configured for direct_io or not
**	    is determined by the config parameter
**	    "ii.$.$.$.direct_io: ON|OFF" and resolved by
**	    DIlru_init(), deprecating the dodirect() function.
**	    The slaves are told to use direct_io via the
**	    file's properties sent from the server
**	    (FPROP_DIRECT_REQ).
**   9-Nov-2006 (hanal04) SIR 117044
**      Include asm/ioctl.h for int_rpl build.
**   9-Nov-2006 (hanal04) SIR 117044
**      Remove asm/ioctl.h for int_rpl build. Latest Rpath Linux version
**      no longer requires it.
**/

/* defines */

/*
** Use slave processes only if direct i/o (or async i/o) is not used
**
** [ Nb.
**    Before enabling xCL_ASYNC_IO the following should be checked
**    carefully:
**    o Is it supported for regular files as well as raw devices?
**    o Would other file operations - open(), close(), rename(),
**      mknod(), creat() ...  block? (It is ok for these to block
**      in a slave process not in a DBMS server).
**    o Are there sufficent file descriptors available in a DBMS to
**      support the required number of GCA connections and async.
**      disk i/o?
**    o Does it actually perform better?
** ]
*/

# ifdef xCL_SUN_ASYNC_IO
CS_SEMAPHORE DI_async_sem;
# endif /* xCL_SUN_ASYNC_IO */

/*
** These definitions are used by IIdio_compute_file_size when computing log
** file sizes.
*/
#define			D_MIN_BLK_SIZE	    4096    /* min log file blk size */
#define			D_MAX_BLK_SIZE	    32768   /* max log file blk size */
#define			D_MIN_DISK_BLK_SIZE 2048    /* min unix raw disk blk */

/* Op code defines for IIdio_rw: */
#define			D_READ		1
#define			D_WRITE		2
#define			D_WRITEV	3

/* Variables */

/* Externals */

/* Forward References */

static int IIdio_rw( 
		int op,
		int fd,
		char *buf,
		int len,
		OFFSET_TYPE off,
		OFFSET_TYPE *loffp,
		u_i4 fprop,
		CL_ERR_DESC *err);

static int dolog( VOID );

static i4 IIdio_compute_file_size( 
		i4 file_desc,
		u_i4 fprop);

static int IIdio_file_fd( int fd );

/*{
** Name: IIdio_open() - open a file for disk block oriented i/o
**
** Description:
**	Same as regular UNIX open(2), only may do some other system
**	specific stuff.
**
** Inputs:
**	file		- name of the file
**	mode		- open mode, same as UNIX open(2).
**	perm		- permissions, same as UNIX open(2).
**	err		- pointer to a CL_ERR_DESC to be filled in, or NULL.
**
** Outputs:
**	returns newly opened fd or -1.  If error return, sets the supplied
**	CL_ERR_DESC.  errno should not be presumed to be valid.
**
**
** Side effects:
**
**	Sets the rawfd[] entry for the newly opened file, for use by
**	subsequent IIdio calls.
**
** History:
**	12-jul-89 (daveb)
**		Created to make handling machines with direct i/o easier.
**	05-Nov-1992 (mikem)
**	    su4_us5 port.  Changed direct calls to CL_NFILE to calls to new
**	    function iiCL_get_fd_table_size().
**	30-nov-1992 (rmuth)
**	    Prototype.
**      10-feb-1993 (smc)
**          Defined a local throw away variable of type STATUS, the address 
**          of which can be passed to MEreqmem and so adhere to the
**          prototype of that function.
**      12-may-1994 (rog)
**          Don't set the sticky bit on files that are not O_SYNCed.
**	28-feb-2000 (somsa01)
**	    Use stat64() when LARGEFILE64 is defined.
**	18-May-2000 (bonro01)
**	    Allocate FD table with an additional number of entries
**	    equal to STREAM_RESVFD when xCL_RESERVE_STREAM_FDS is used.
**	    This caused a problem because fcntl() will return a duplicate
**	    file descriptor over the MAX FD limit.
**	14-may-2002 (devjo01)
**	    Formalize support for Tru64's O_DIRECTIO flag.
**	27-Mar-2003 (jenjo02)
**	    Added directio() to bypass system caching when "possible"
**	    (see man page for directio). xCL_DIRECTIO_EXISTS is set
**	    by mksecret if the directio() function exists.
**	    The user enables it with the environment variable
**	    II_DIRECT_IO Y|y. This interpretation is disabled for 
**	    sqs_ptx (xCL_064_SQT_DIRECT_IO), for which II_DIRECT_IO
**	    has a distinctly different meaning. SIR 109925.
**	01-Apr-2004 (fanch01)
**	    Add O_DIRECT support on Linux depending on the filesystem
**          properties, pagesize.  Fixups for misaligned buffers on read()
**          and write() operations.  Add generic support for more intelligently
**          performing some file system operations.
**	15-Apr-2004 (fanch01)
**          Enable O_DIRECT for ext3 page sizes smaller than 4k if the
**          block size supports it.
**	19-aug-04 (toumi01)
**	    Ugly hack to include the right Linux headers for O_DIRECT for the
**	    system library level. Can't test e.g. __GLIBC_MINOR__ because this
**	    seems to change from 2.3.2 to 2.3.3 and there's no easy wasy to get
**	    that third version digit without executing /lib/libc.so.6. So by
**	    examination of fs.h find that MAY_APPEND is the first distinguishing
**	    definition in the header.
**	30-Mar-2005 (hanje04)
**	    BUG 115917
**	    Include sys/ioctl.h on Linux as we call ioctl() in IIdio_open().
**	    Failing to do so causes undefined references to '_IO' when
**	    building on RHEL 4.
**	14-Oct-2005 (jenjo02)
**	    Whether DIRECTIO should be used or not now depends on
**	    what's in the file properties (fprop). 
**	    If FPROP_DIRECT_REQ_CHECK(fprop) is true, it's because
**	    the server is configured for direct_io and the file
**	    opener wants it.
*/

int
IIdio_open( 
    register char *file,
    register int mode,
    register int perms,
    register int pagesize,
    u_i4 *fprop,
    register CL_ERR_DESC *err )
{
    int fd;
    STATUS memstatus;                 /* dummy parameter for MEreqmem */
#ifdef LARGEFILE64
    struct stat64 sbuf;
#else /* LARGEFILE64 */
    struct stat sbuf;
#endif /*  LARGEFILE64 */
    i4 tablesize;

    if( err )
	CL_CLEAR_ERR( err );

#if defined(xCL_081_SUN_FAST_OSYNC_EXISTS)
    {
	static bool	checked = 0;
	static bool	stickycheck = 0;
	char		*cp;	    

	/* Don't mess with unsetting the sticky bit if this is set. */
	if( !checked )
	{
	    NMgtAt("II_DIO_STICKY_SET", &cp);
	    if( stickycheck = (cp && *cp != EOS) )
	    {
		TRdisplay("IIdio: Sticky bit not being checked.\n" );
	    }

	    checked = TRUE;
	}

	if (((mode & O_SYNC) == 0) && ((mode & O_CREAT) == 0) && !stickycheck)
	{
	    /*
	    ** Turn off the sticky bit if we're not using O_SYNC.
	    ** The sticky bit feature was only designed to work with 
	    ** files that were opened with O_SYNC.
	    **
	    ** Note that we're ignoring the return status because all of the
	    ** serious file problems will be caught be the open() call.
	    */

#ifdef LARGEFILE64
	    if (!stat64(file, &sbuf))
#else /* LARGEFILE64 */
	    if (!stat(file, &sbuf))
#endif /* LARGEFILE64 */
	    {
		int	new_perms = (unsigned)sbuf.st_mode & ~S_IFMT;

		new_perms &= ~S_ISVTX;
		(VOID) chmod(file, new_perms);
	    }
	}
    }
# endif

    /* Resolve DIRECTIO on open(), if possible */
    
#if defined(axp_osf) && defined(O_DIRECTIO)
    /* If DIRECTIO already on for file, or wanted and configured... */
    if ( FPROP_DIRECT_CHECK(*fprop) || FPROP_DIRECT_REQ_CHECK(*fprop) )
    {
	mode |= O_DIRECTIO;
	FPROP_DIRECT_SET(*fprop);
    }
#endif

#ifdef LNX
    /* DIRECTIO already enabled on file? If so, open with it */
    if ( FPROP_DIRECT_CHECK(*fprop) )
	mode |= O_DIRECT;
#endif /* LNX */

#ifdef LARGEFILE64
    if( (fd = open64( file, mode, perms )) == -1 )
#else /* LARGEFILE64 */
    if( (fd = open( file, mode, perms )) == -1 )
#endif /*  LARGEFILE64 */
    {
	if( err )
	    SETCLERR(err, 0, ER_open);
    }
    else
    {
	DIFILE_FD( fd );

	/* if first open of file... */
	if ( !FPROP_OPEN_CHECK(*fprop) )
	{
	    /* Note that this has been done, for subsequent opens */
	    FPROP_OPEN_SET(*fprop);

/* NB: this will be false for LNX and axp_osf */
#if defined(xCL_DIRECTIO_EXISTS)
	    /* DIRECTIO wanted and configured? */
	    if ( FPROP_DIRECT_REQ_CHECK(*fprop) )
	    {
		if ( (directio(fd, DIRECTIO_ON)) == -1 )
		    TRdisplay("%@ IIdio_open: directio returned %d\n", errno);
		else
		    FPROP_DIRECT_SET(*fprop);
	    }
#endif /* xCL_DIRECTIO_EXISTS */

	    /* "can't fail" */
#ifdef LARGEFILE64
	    (void) fstat64( fd, &sbuf );
#else /* LARGEFILE64 */
	    (void) fstat( fd, &sbuf );
#endif /*  LARGEFILE64 */

	    if ( sbuf.st_mode & S_IFCHR )
		FPROP_RAW_SET(*fprop);
#ifdef LNX
	    else
	    {
		/* magic number, file system type */
		unsigned blk_size, pwr;

		/* is filesystem known to support O_DIRECT? */
#ifdef LARGEFILE64
		struct statfs64 fs;
		if (fstatfs64 (fd, &fs) != -1
#else /* LARGEFILE64 */
		struct statfs fs;
		if (fstatfs (fd, &fs) != -1
#endif /*  LARGEFILE64 */
			&& ioctl (fd, FIGETBSZ, &blk_size) != -1)
		{
		    /* determine power of two for supported block sizes, will use this for alignment */
		    switch (blk_size)
		    {
			case 0x00200:	/* 512b */
				pwr = 9;
				break;
			case 0x00400:	/* 1k */
				pwr = 10;
				break;
			case 0x00800:	/* 2k */
				pwr = 11;
				break;
			case 0x01000:	/* 4k */
				pwr = 12;
				break;
			case 0x02000:	/* 8k */
				pwr = 13;
				break;
			case 0x04000:	/* 16k */
				pwr = 14;
				break;
			case 0x08000:	/* 32k */
				pwr = 15;
				break;
			case 0x10000:	/* 64k */
				pwr = 16;
				break;
			case 0x20000:	/* 128k */
				pwr = 17;
				break;
			default:		/* unsupported */
				pwr = 0;
				break;
		    }
		    /* check pagesize, blk_size, alignment and filesystem type */
		    if (pwr != 0 && blk_size != 0 && pagesize % blk_size == 0 && pagesize / blk_size > 0)
		    {
			/* check filesystem type and known requirements for support */
			i4 enable_odirect = FALSE;
			i4 odirect_mandatory = FALSE;

			switch (fs.f_type)
			{
			    case GFS_SUPER_MAGIC:
				odirect_mandatory = TRUE;
				FPROP_ALLOCSTRATEGY_SET(*fprop, FPROP_ALLOCSTRATEGY_TRNC);
				break;
			    case OCFS_SUPER_MAGIC:
				odirect_mandatory = TRUE;
				FPROP_ALLOCSTRATEGY_SET(*fprop, FPROP_ALLOCSTRATEGY_TRNC);
				break;
			    case LUSTRE_SUPER_MAGIC:
				odirect_mandatory = TRUE;
				FPROP_ALLOCSTRATEGY_SET(*fprop, FPROP_ALLOCSTRATEGY_VIRT);
				break;
			    case NFS_SUPER_MAGIC:
			    case EXT3_SUPER_MAGIC:
				/* Enable only if wanted and configured */
				if ( FPROP_DIRECT_REQ_CHECK(*fprop) )
				    enable_odirect = TRUE;
				FPROP_ALLOCSTRATEGY_SET(*fprop, enable_odirect ?
						       FPROP_ALLOCSTRATEGY_TRNC :
						       FPROP_ALLOCSTRATEGY_VIRT);
				break;
			    case XFS_SUPER_MAGIC:
				/* Enable only if wanted and configured */
				if ( FPROP_DIRECT_REQ_CHECK(*fprop) )
				    enable_odirect = TRUE;
				/* Need to see which is best,
				** stick to virt for now
				*/
				FPROP_ALLOCSTRATEGY_SET(*fprop,FPROP_ALLOCSTRATEGY_VIRT);
				break;
			    default:
				enable_odirect = FALSE;
				break;
			}
			if ( enable_odirect || odirect_mandatory )
			{
			    if (fcntl(fd, F_SETFL, (mode & ~(O_SYNC | O_DSYNC)) | O_DIRECT) == -1)
			    {
				/* was O_DIRECT mandatory? */
				if ( odirect_mandatory )
				{
				    SETCLERR(err, 0, ER_open);
				    close(fd);
				    TRdisplay("%@ IIdio_open: failed to enable mandatory O_DIRECT for filesystem, f_type %x, fd %d, mode %x\n",
						      (i4) fs.f_type, fd, mode);
				    return(-1);
				}
			    }
			    else
			    {
				/* determine memory buffer alignment */
				if (dolog() > 1)
				    TRdisplay("%@ IIdio_open: O_DIRECT enabled for supported filesystem, f_type %x, fd %d, mode %x\n",
							  (i4) fs.f_type, fd, mode);
				FPROP_DIRECT_SET(*fprop);
				FPROP_ALIGN_SETPOWER(*fprop, pwr);
			    }
			}
		    }
		}
		else
		    TRdisplay("%@ IIdio_open: can not determine file and filesystem properties, fd %d, mode %x\n", fd, mode);
	    }
#endif /* LNX */

#if defined(xCL_081_SUN_FAST_OSYNC_EXISTS)
	    {
		/*
		** On SunOS systems, if we set the sticky bit and clear the 
		** execute bits, non-critical inode updates will be deferred
		** when the file is open for O_SYNC.  The feature may not be
		** enabled on all systems; so, ignore any errors. 
		**
		** Try to do this just at file creation time--assume that
		** if file is empty and O_CREAT is set, then this is file
		** creation time.
		**
		** If we are opening a file with O_SYNC, and the sticky
		** bit got turned off for some reason, let's turn it back on.
		**
		** For Solaris, we use O_DSYNC and avoid this whole mess.
		**
		*/
		if ( ((((mode & O_CREAT) == O_CREAT) && sbuf.st_size == 0)
		      || (((mode & O_SYNC) == O_SYNC)
			 && ((sbuf.st_mode & S_ISVTX) == 0)))
		     && (!FPROP_RAW_CHECK(*fprop))
		   )
		{
		    int	new_perms = (unsigned)sbuf.st_mode & ~S_IFMT;

		    new_perms |= S_ISVTX;
		    new_perms &= ~(S_IEXEC | S_IXGRP | S_IXOTH);
		    (VOID) fchmod(fd, new_perms);
		}
	    }
#endif /* xCL_081_SUN_FAST_OSYNC_EXISTS */
        } /* !FPROP_OPEN_CHECK */

    }
/* displaying fd# and path/filename on same line for better analysis */
    if( dolog() > 0 )
	TRdisplay("IIdio_open:  fd %d [%s] mode 0x%x\nfd %d=%s\n",
	     fd, ( FPROP_RAW_CHECK(*fprop) ? "raw disk" : "regular file"), mode, fd, file );

    return( fd );
}



/*{
** Name: IIdio_write() - do a seek and write operation
**
** Description:
**	Does an lseek followed by a write, skipping the lseek if
**	if the file is known to be in the right position.  May do system
**	specific things, like take advantage of direct i/o system calls.
**
** Inputs:
**	fd		- file descriptor
**	buf		- buffer to write
**	len		- length to write
**	off		- seek offset
**	loffp		- ptr to last known offset, or NULL
**	err		- CL_ERR_DESC to fill on error, or NULL.
**
** Outputs:
**	returns "len" on successful operation, something else on
**	failure, with UNIX errno set.
**
**	*loffp		 - if non-null,  updated if changed.
**
** History:
**	27-jun-89 (daveb)
**		Created to make handling machines with direct i/o easier
**	12-jul-89 (daveb)
**		Renamed from IIwrite_seek to IIdio_write, with err arg added.
**	25-sep-1991 (mikem) integrated following change: 19-aug-91 (rogerk)
**		Don't align buffers before reading/writing unless acting on
**		a raw device.  Other read/write requests do not require
**		aligned buffers and copying them is pretty expensive.
**	30-November-1992 (rmuth)
**		Prototype.
**	07-oct-1993 (tad)
**		Bug #56449
**		Changed %x to %p for pointer values.
**	22-Dec-1998 (jenjo02)
**	    Return an error if reading/writing an unaligned RAW buffer
**	    instead of copying to an (unthread-safe) aligned buffer.
**	11-jul-2000 (somsa01 for jenjo01)
**	    ifdef'd out check for aligned raw buffers. On Solaris, it
**	    doesn't seem to matter and I'm unaware of any platform
**	    on which it does...
**	02-jan-2001 (mosjo01)
**	    The first operation upon entering this function is setting 
**	    alignedbuf = iobuf. I substituted iobuf with alignedbuf in the 
**	    various write operations. Only in the case of xCL_064_SQT_DIRECT_IO
**	    and with II_DIRECT_IO = 'y' env variable "might" alignedbuf be
**	    adjusted to a new address by getalbuf() and consequently require
**	    using MEcopy to move the data from iobuf to alignedbuf before 
**	    performing the DirectWrite(). 
*/

int
IIdio_write(
    register int fd,
    register char *iobuf,
    register int len,
    register OFFSET_TYPE off,
    register OFFSET_TYPE *loffp,
    u_i4	fprop,
    register CL_ERR_DESC *err)
{
    char *alignedbuf = iobuf;
    int actual = -1;
    char *cp;

    if( err )
	CL_CLEAR_ERR( err );

# if defined(xCL_SUN_ASYNC_IO)
    /*
    ** The following code is an update of the original async i/o
    ** code, but updated for the Solaris Async i/o.  The fatal
    ** flaw with the code as written is the fact that aiowait()
    ** will return results from the first async i/o to complete,
    ** whether in this thread or not.  The semaphore can be used
    ** to single-thread it, but this will have problems in libingres.a
    ** which has HANDY, but not DI or CS.
    ** Sun says they will support Posix async i/o soon.
    */
    {
	aio_result_t	result, *resultp;

	CSp_semaphore(TRUE, &DI_async_sem);
	/* post the asynchronous write */
#ifdef LARGEFILE64
	if ( aiowrite64( fd, iobuf, len, off, SEEK_SET, &result ) )
#else /* LARGEFILE64 */
	if ( aiowrite( fd, iobuf, len, off, SEEK_SET, &result ) )
#endif /*  LARGEFILE64 */
	{
	    SETCLERR(err, result.aio_errno, ER_write);
	    actual = result.aio_return;
	}
	else
	{
	    if ( result.aio_errno )
		CS_breakpoint();
	    /* sleep on i/o completion */
	    resultp = aiowait( NULL );

	    if ( resultp->aio_return != len )
		CS_breakpoint();
	    actual = resultp->aio_return;
	}
	CSv_semaphore(&DI_async_sem);
    }
# else /* xCL_SUN_ASYNC_IO */

    actual = IIdio_rw( D_WRITE, fd, alignedbuf, len, off, loffp, 
			fprop, err );
# endif /* xCL_SUN_ASYNC_IO */

    if( dolog() > 0 )
	TRdisplay("IIdio_write: fd %d iobuf %p alignedbuf %p len %d\n\t\
	off %d loffp %d errno %d actual %d\n", fd, iobuf, alignedbuf, len,
	(i4)off, loffp ? (i4)*loffp : -1, errno, actual);

    return( actual );
}

/*{
** Name: IIdio_writev() - do a seek and writev operation
**
** Description:
**	Does an lseek followed by a writev, skipping the lseek if
**	if the file is known to be in the right position.  May do system
**	specific things, like take advantage of direct i/o system calls.
**
** Inputs:
**	fd		- file descriptor
**	iov		- array of iovec structures
**	iovcnt		- number of elements in the array
**	off		- seek offset
**	loffp		- ptr to last known offset, or NULL
**	err		- CL_ERR_DESC to fill on error, or NULL.
**
** Outputs:
**	returns "len" on successful operation, something else on
**	failure, with UNIX errno set.
**
**	*loffp		 - if non-null,  updated if changed.
**
** History:
**	06-Jun-1999 (jenjo02)
**	    Created for GatherWrite.
*/
int
IIdio_writev(
    register int fd,
    register char *iov,
    register int iovcnt,
    register OFFSET_TYPE off,
    register OFFSET_TYPE *loffp,
    u_i4 fprop,
    register CL_ERR_DESC *err)
{
    int actual = -1;
    char *cp;

    if( err )
	CL_CLEAR_ERR( err );

    actual = IIdio_rw( D_WRITEV, fd, iov, iovcnt, off, 
			loffp, fprop, err );

    if( dolog() > 2 )
	TRdisplay(
"IIdio_writev: fd %d iov %p iovcnt %d off %d loffp %p err %x\n\tactual %d\n",
	    fd, iov, iovcnt, off, loffp, err, actual );

    return( actual );
}



/*{
** Name: IIdio_read() - do a seek and read operation
**
** Description:
**	Does an lseek followed by a read, skipping the lseek if
**	if the file is known to be in the right position.  May do system
**	specific things, like take advantage of direct i/o system calls.
**
** Inputs:
**	fd		- file descriptor
**	buf		- buffer to read
**	len		- length to read
**	off		- seek offset
**	loff		- ptr to known current offset or NULL.
**	err		- CL_ERR_DESC to fill on error.
**
** Outputs:
**	returns "len" on successful operation, something else on
**	failure, with UNIX errno set.
**
**	*loffp		 - if non-null,  updated if changed.
**
** History:
**	27-jun-89 (daveb)
**		Created to make handling machines with direct i/o easier
**	12-jul-89 (daveb)
**		Renamed from IIread_seek to IIdio_read, with err arg added.
**	19-aug-91 (rogerk)
**		Don't align buffers before reading/writing unless acting on
**		a raw device.  Other read/write requests do not require
**		aligned buffers and copying them is pretty expensive.
**	30-November-1992 (rmuth)
**		Prototype.
**	07-oct-1993 (tad)
**		Bug #56449
**		Changed %x to %p for pointer values.
**	22-Dec-1998 (jenjo02)
**	    Return an error if reading/writing an unaligned RAW buffer
**	    instead of copying to an (unthread-safe) aligned buffer.
**	11-jul-2000 (somsa01 for jenjo01)
**	    ifdef'd out check for aligned raw buffers. On Solaris, it
**	    doesn't seem to matter and I'm unaware of any platform
**	    on which it does...
**	02-jan-2001 (mosjo01)
**	    Aligned buffers does matter to sqs_ptx with II_DIRECT_IO = 'y'.
**	    Through the use of xCL_064_SQT_DIRECT_IO the code for testing
**	    for aligned buffer, getting an aligned buffer, using DirectRead,
**	    and MEcopy data from aligned buffer to passed-in buffer, is only
**	    generated for sqs_ptx.
**	    NOTE: the argument 'iobuf' is the address of the buffer passed to
**	    IIdio_read. The first operation upon entering this function is to
**	    set 'alignedbuf' to this same address regardless of platform. 
*/

int
IIdio_read( 
    register int fd,
    register char *iobuf,
    register int len,
    register OFFSET_TYPE off,
    register OFFSET_TYPE *loffp,
    u_i4 fprop,
    register CL_ERR_DESC *err)
{
    char *alignedbuf = iobuf;
    int actual = -1;

    if( err )
	CL_CLEAR_ERR( err );

# ifdef xCL_SUN_ASYNC_IO 
    {
	aio_result_t	result, *resultp;

	/* post the asynchronous read */
#ifdef LARGEFILE64
	if ( aioread64( fd, iobuf, len, off, SEEK_SET, &result ) )
#else /* LARGEFILE64 */
	if ( aioread( fd, iobuf, len, off, SEEK_SET, &result ) )
#endif /*  LARGEFILE64 */
	{
	    SETCLERR(err, result.aio_errno, ER_read);
	    actual = result.aio_return;
	}
	else
	{
	    /* sleep on i/o completion */
	    resultp = aiowait( NULL );

	    actual = resultp->aio_return;
	}
    }
# else /* xCL_SUN_ASYNC_IO */

    actual = IIdio_rw( D_READ, fd, iobuf, len, off, 
			loffp, fprop, err );
# endif /* xCL_SUN_ASYNC_IO */

    if( dolog() > 0 )
	TRdisplay("IIdio_read:  fd %d iobuf %p alignedbuf %p len %d\n\t\
	off %d loffp %d errno %d actual %d\n", fd, iobuf, alignedbuf, len,
	(i4)off, loffp ? (i4)*loffp : -1, errno, actual );

    return( actual );
}

/*{
** Name: II_dio_do_lseek() -- Should we check lseek values?
**
** Description:
**      Tells whether environmental variable II_DIO_LSEEK_CHK is set.
**
** Inputs:
**      none.
**
** Outputs:
**      returns 0 or 99.
**
*
** History:
**      17-aug-92 (pearl)
**              Created.
*/
int
II_dio_do_lseek()
{
    static int checked = 0;
    static int dotrace = 0;

    char *cp;
    if( !checked )
    {
        checked = 1;
        NMgtAt( "II_DIO_LSEEK_CHK", &cp );
        if (cp)
            if (*cp == 'y')
                dotrace = 99;
            else
                dotrace = atoi(cp);
        else
            dotrace = 0;
    }
    return( dotrace );
}

/*{
** Name: IIdio_rw() - do a seek and i/o on a file.
**
** Description:
**	Does an lseek followed by a i/o operation,
**	skipping the seek if the file is known to be in the right position.
**
** Inputs:
**	op		- D_READ = read, D_WRITE = write, D_WRITEV = writev
**	fd		- file descriptor
**	buf		- buffer to write
**	len		- length to write
**	off		- seek offset
**	loffp		- ptr to known current offset, or NULL
**	err		- CL_ERR_DESC to fill on error.
**
** Outputs:
**	returns as does read and write, actual length transferred or -1,
**	with UNIX errno set on.  If the requested "op" is bad,
**
**	*loffp		 - if non-null,  updated if changed.
**
** History:
**	27-jun-89 (daveb)
**		Created to make handling machines with direct i/o easier.
**	14-sep-1993 (pearl)
**	    In IIdio_rw(), if environmental variable II_DIO_LSEEK_CHK is set, 
**	    check saved offset value by calling lseek().  Log if the offset 
**	    is incorrect.  This can be used to debug E_DM934D_DM0P_BAD_PAGE 
**	    errors.
**	07-oct-1993 (tad)
**		Bug #56449
**		Changed %x to %p for pointer values.
**  15-Apr-2004 (fanch01)
**      Move apages declaration to top of function for older compilers.
*/

static int
IIdio_rw( 
    register int op,
    register int fd,
    register char *buf,
    register int len,
    register OFFSET_TYPE off,
    register OFFSET_TYPE *loffp,
    u_i4 fprop,
    register CL_ERR_DESC *err)
{
    register int actual = -1;
    int errcode;
    char *opmsg, *abuf;
    static int check_offset = 0;
    static int do_lseek_checked = 0;
#ifdef LNX
	i4 apages;
#endif

    if( op == D_READ || op == D_WRITE || op == D_WRITEV )
    {
        if (!do_lseek_checked)
        {
            ++do_lseek_checked;
            check_offset = II_dio_do_lseek();
        }
        if (check_offset)
        {
            if (loffp)
            {
                OFFSET_TYPE file_position;

                /* check saved offset value; if it is wrong, log it, and
                ** correct it.
                */
#ifdef LARGEFILE64
                if ((file_position = lseek64(fd, (OFFSET_TYPE) 0L, L_INCR)) !=
		    *loffp)
#else /* LARGEFILE64 */
                if ((file_position = lseek(fd, 0L, L_INCR)) != *loffp)
#endif /*  LARGEFILE64 */
                {
                    TRdisplay (
"%@ IIdio_rw: bad saved offset; fd %d, off %d, loffp %d, actual offset %d\n",
                                fd, (i4)off, (i4)*loffp, (i4)file_position);
                    *loffp = file_position;
 		}
 	    }
	}

	/* seek if we are not known to be at the right place */
	if ( !loffp || (loffp && (off != *loffp)) )
	{
#ifdef LARGEFILE64
	    if( lseek64(fd, (OFFSET_TYPE) off, L_SET ) != off )
#else /* LARGEFILE64 */
	    if( lseek(fd, off, L_SET ) != off )
#endif /*  LARGEFILE64 */
	    {
		if( err )
		    SETCLERR( err, 0, ER_lseek );

		if( dolog() )
		    TRdisplay( "IIdio_rw:  bad seek, fd %d off %d errno %d\n",
				fd, (i4)off, errno );
		goto done;
	    }
	}

#ifdef LNX
	if (op == D_READ || op == D_WRITE)
	{
	    if (IIdio_alloc_realign(fd, len, &apages, buf, &abuf, 
			fprop, __FUNCTION__) && op == D_WRITE)
		MEcopy(buf, len, abuf);
	}
	else
		apages = 0;
#else
	abuf = buf;
#endif

	if( op == D_READ )
	{
	    errcode = ER_read;
	    opmsg = "read";
	    actual = read( fd, abuf, len );
	}
	else if ( op == D_WRITE )
	{
	    errcode = ER_write;
	    opmsg = "write";
	    actual = write( fd, abuf, len );
	}
	else
	{
	    /*
	    ** For writev, "buf" is a pointer to an array of
	    ** iovecs, and "len" is the number of elements in
	    ** the array.
	    */
	    errcode = ER_writev;
	    opmsg = "writev";
	    actual = writev( fd, (struct iovec *)buf, len );
	}

	if( actual == -1 )
	{
	    if( err )
		SETCLERR( err, 0, errcode );

	    if( dolog() )
		TRdisplay("IIdio_rw: bad %s, fd %d errno %d\n",
				opmsg, fd, errno );
	}
	else if( loffp != NULL )
	{
	    *loffp = off + actual;
	}
    }

#ifdef LNX
	/* free temporary aligned buffer we allocated */
	if (apages)
	{
		CL_ERR_DESC err;
		if (op == D_READ)
			MEcopy (abuf, len, buf);
		MEfree_pages(abuf, apages, &err);
	}
#endif

done:

    if( dolog() > 0 )
	TRdisplay(
"IIdio_rw: op %d fd %d buf %p len %d off %d loffp %d err %x\n\tactual %d\n",
		op, fd, buf, len, (i4)off, loffp ? (i4)*loffp : -1, err, actual );

    return( actual );
}

/*{
** Name: dolog() -- Should we log dio events?
**
** Description:
**	Tells whether II_DIO_TRACE has turned on logging.
**
** Inputs:
**	none.
**
** Outputs:
**	returns trace level
**
** Side Effects:
**	Examines the symbol "II_DIO_TRACE", and will return the value set
**	or zero if unset.  A value of 'y' is equivalent to '1'.
**
** History:
**	12-jul-89 (daveb)
**		Created.
**	30-November-1992 (rmuth)
**		Prototype.
**  05-Apr-2004 (fanch01)
**		Correct comments.
*/
static int dolog( VOID )
{
    static int checked = 0;
    static int dotrace;

    char *cp;
    if( !checked )
    {
	checked = 1;
	NMgtAt( "II_DIO_TRACE", &cp );
	if (cp)
	    if (*cp == 'y')
		dotrace = 1;
	    else
		dotrace = atoi(cp);
	else
	    dotrace = 0;
    }
    return( dotrace );
}

/*{
** Name: IIdio_get_file_eof        - determine the size of any file
**
** Description:
**	This routine finds the end of file of any file, including raw 
**	files.  It was created from bits and pieces of LG and DI
**	to satisfy the needs of LG slaves.   In most cases, an lseek
**	is all that is required, but on some systems lseek of raw files
**      doesn't work, so a binary search of the file is performed.
**
** Inputs:
**      file_desc       file descriptor of the file
**
** Outputs:
**      None
**
** Returns:
**      file_size       file size in bytes
**
** History:
**      12-oct-1990 (jnash)
**	    Created from LG and DI bits and pieces.
**	30-November-1992 (rmuth)
**	    Change the return and input type to be of type i4.
**	    Declare a local variable of type 'int' to make the lseek call.
*/
OFFSET_TYPE
IIdio_get_file_eof( i4 fd,
    u_i4 fprop )
{
    register long     blocks;
    OFFSET_TYPE       offset = (OFFSET_TYPE) -1L;

    /* unix variables */
    int		l_fd;

    if ( !FPROP_RAW_CHECK(fprop) )
    {
	l_fd = fd;
#ifdef LARGEFILE64
        offset = lseek64(l_fd, (OFFSET_TYPE) 0L, L_XTND);
#else /* LARGEFILE64 */
        offset = lseek(l_fd, (OFFSET_TYPE) 0L, L_XTND);
#endif /*  LARGEFILE64 */
        if( dolog() > 0 )
	TRdisplay("IIdio_get_file_eof: fd %d offset %d\n",
	        fd, (i4)offset );

    }
    else
    {
	blocks = IIdio_compute_file_size( fd, fprop );
        offset = (OFFSET_TYPE)blocks * (OFFSET_TYPE)D_MIN_BLK_SIZE;
        if( dolog() > 0 )
	TRdisplay("IIdio_compute_eof: fd %d offset %d blocks %d\n",
                fd, (i4)offset, blocks );
    }

    return ( offset );
}

/*{
** Name: IIdio_compute_file_size    - determine the size of a raw file.
**
** Description:
**	Unfortunately, "lseek-to-the-end" doesn't work on a raw file.
**	Therefore, we have come up with a binary search algorithm which
**	determines the file size by figuring out, using a binary search, the
**	highest offset which we can successfully read from.
**
**	the following method is used to determine the length of the file
**	rather than a simple seek() , so that we can use the same code for
**	both normal and for "raw" log files.  Even in the worse case we
**	only use approximately 20 reads, and this is only called when the
**	rcp is starting up or while running rcpconfig.
**
**	Most device drivers will onlys support raw reads on block boundaries
**	so we will limit the search to D_MIN_DISK_BLK_SIZE ("2K" currently)
**	blocks.
**
**	This function takes a file descriptor and returns the file size in
**	blocks, where the block size is D_MIN_BLK_SIZE.
**
**	Note that for raw database (not log) files, dm2f should prevent this
**	function from ever being called!
**
** Inputs:
**	file_desc	file descriptor of the file
**
** Outputs:
**	None
**
** Returns:
**	file_size	file's size in D_MIN_BLK_SIZE blocks.
**
** History:
**	12-oct-1990 (bryanp)
**	    Abstracted out from LGopen to share with LGcopy.
**	10-dec-91 (jnash)
**	    Moved into handy from lgextern.c because DIsense needs to
**	    call it for raw log files.
**	30-November-1992 (rmuth)
**	    Prototype.
*/
static	i4
IIdio_compute_file_size( 
    i4 file_desc,
    u_i4 fprop )
{
    OFFSET_TYPE   max_eof, min_eof, curr_eof;
    char	    raw_buf[D_MIN_DISK_BLK_SIZE * 2];
    char	    *char_buf = ME_ALIGN_MACRO(raw_buf, D_MIN_DISK_BLK_SIZE);
    int		    read_ret;
    i4	    blocks;
    CL_ERR_DESC	    sys_err;

    {
#ifdef hp3_us5
        max_eof = curr_eof = (lseek(file_desc, 0l, SEEK_END) /
                                D_MIN_DISK_BLK_SIZE);
        if (max_eof <= 0l)
                max_eof = curr_eof = (MAXFS / D_MIN_DISK_BLK_SIZE);
#else
	max_eof = curr_eof = (MAXFS / D_MIN_DISK_BLK_SIZE);
#endif
	min_eof = 0;

	/* Assertion: min_eof is always guaranteed to be <= than the number of
	** blocks in the file and max_eof is always guaranteed to be greater 
	** than the number of blocks in the file.
	*/

	for (;;)
	{
	    if( (read_ret = IIdio_read( file_desc, char_buf,
		    D_MIN_DISK_BLK_SIZE, 
		    ((OFFSET_TYPE)curr_eof * (OFFSET_TYPE)D_MIN_DISK_BLK_SIZE),
		    (OFFSET_TYPE *)0, 
		    fprop,
		    &sys_err )) != D_MIN_DISK_BLK_SIZE )
	    {

		/* curr_eof is too big */

		if (max_eof > curr_eof)
		    max_eof = curr_eof;
		else if (max_eof == 0)
		    break;

		curr_eof = min_eof + ((max_eof - min_eof) / 2);
	    }
	    else
	    {
		/* curr_eof is too small or perfect */
		if (min_eof <  curr_eof)
		{
		    min_eof = curr_eof;
		    curr_eof = min_eof + ((max_eof - min_eof) / 2);
		}
		else if (min_eof == curr_eof)
		{
		    if (max_eof == curr_eof + 1)
		    {
			/* success */
			break;
		    }
		    else
		    {
			curr_eof = min_eof + 1;
		    }
		}
	    }
	}

	/* only allow 32k multiple size log file */

	blocks = (max_eof * D_MIN_DISK_BLK_SIZE) / D_MAX_BLK_SIZE;

	/* Now set blocks to the number of min. sized blocks from the log
	** file which is now a multiple of D_MAX_BLK_SIZE.
	*/

	blocks = blocks * (D_MAX_BLK_SIZE / D_MIN_BLK_SIZE);
    }
    return (blocks);
}

/*
** Name: IIdio_alloc_realign() - allocate aligned memory if necessary
**
** Description:
**      Allocates aligned memory for a file descriptor if the given
**      buffer is not aligned.  Guarantees the buffer returned can
**      be used for DIRECT_IO, RAW or any I/O method that requires
**      aligned memory.
**
**      Memory allocated with this function must be freed with
**      MEfree_pages().
**
**      This should be abstracted somewhat with a allocation structure
**      and corresponding IIdio_align_free().
**
** Inputs:
**      fd      file descriptor
**      len     length in bytes to allocate
**      am      pointer to transfer buffer to check for alignment
**      fn      the name of the calling function (to help trace unaligned
**              access)
**
** Outputs:
**      apages  the number of pages to free with MEfree_pages()
**      am      pointer to an aligned buffer
**
** Returns:
**      0		no new allocation made (write will catch failure)
**      1		new aligned allocated buffer
**
** History:
**	01-Apr-2004 (fanch01)
**	    Created.
*/
int IIdio_alloc_realign(
    int fd, 
    int len, 
    SIZE_TYPE *apages, 
    char *m, 
    char **am, 
    u_i4 fprop,
    char *fn)
{
    /* if alignment is required and the buffer is not aligned */
    if ( !FPROP_ALIGN_CHECK(fprop, m) )
    {
	    CL_ERR_DESC err;
	    SIZE_TYPE pages = len / ME_MPAGESIZE + (len % ME_MPAGESIZE ? 1 : 0);
	    if (OK == MEget_pages(ME_IO_MASK, pages, NULL, am, apages, &err))
	    {
		    if (dolog() > 1)
			    TRdisplay("%@ %s: fixup for bad alignment fd %d, len %d, b %p, nb %p\n", fn, fd, pages, m, *am);
		    return(1);
	    }
	    TRdisplay("%@ %s: could not allocate aligned buffer pg %d, fd %d\n", fn, pages, fd);
    }
    *apages = 0;
    *am = m;
    return(0);
}

/*
** Name: IIdio_alloc_aligned() - allocate aligned memory
**
** Description:
**      Allocates aligned memory for a file descriptor.  Guarantees the
**      buffer returned can be used for DIRECT_IO, RAW or any I/O
**      method that required aligned memory.
**
**      Memory allocated with this function must be freed with
**      MEfree_pages().
**
**      This should be abstracted somewhat with a allocation structure
**      and corresponding IIdio_align_free().
**
** Inputs:
**      fd      file descriptor
**      len     length in bytes to allocate
**      fn      the name of the calling function (to help trace unaligned
**              access)
**
** Outputs:
**      apages  the number of pages to free with MEfree_pages()
**      am      a pointer to an aligned buffer
**
** Returns:
**      OK      on success
**      other	error code from MEget_pages
**
** History:
**	01-Apr-2004 (fanch01)
**	    Created.
*/
int IIdio_alloc_aligned(int fd, i4 len, SIZE_TYPE *apages, char **am, char *fn)
{
    CL_ERR_DESC err;
    i4 status;
    SIZE_TYPE pages = len / ME_MPAGESIZE + (len % ME_MPAGESIZE ? 1 : 0);
    
    status = MEget_pages(ME_IO_MASK, pages, NULL, am, apages, &err);

    if (OK != status)
    {
	    TRdisplay("%@ %s: could not allocate (%d) pages for aligned buffer\n", fn, *apages);
	    *apages = 0;
	    *am = NULL;
    }
    return(status);
}

# ifdef xCL_RESERVE_STREAM_FDS
/*
** Name: IIdio_file_fd() - reallocate a file descriptor higher
**
** Description:
**      Reallocates a file descriptor to one above the STREAM_RESVFD
**      threshhold on systems where low-numbered file descriptors must
**      be reserved for stdio.
**
**      Should only be accessed through the DIFILE_FD() macro (c.f.).
**
** History:
**	21-jul-1997 (canor01)
**	    Created.
*/

static int
IIdio_file_fd( int fd )
{
        int     nfd;

        if ( fd > STREAM_RESVFD )
        {
              return fd ;
        }

        nfd = fcntl( fd, F_DUPFD, STREAM_RESVFD );

        if ( nfd < 0 )
        {
            /* No high ones left. */
            return fd;
        }
        else
        {
            /* Close old & return new. */
            close( fd );
            return nfd;
        }
}
# endif /* xCL_RESERVE_STREAM_FDS */
