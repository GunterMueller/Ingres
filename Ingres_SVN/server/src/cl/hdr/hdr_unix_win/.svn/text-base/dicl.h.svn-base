/*
** Copyright (c) 2004 Ingres Corporation
*/

#ifndef		DI_INCLUDED
#define		DI_INCLUDED	1

# include <cs.h>

/**CL_SPEC
** Name: DI.H - This file contains the external structs for DI.
**
** Description:
**      The DI.H file is used to contain all structures and 
**      definitions required to call the compatability library
**      DI routines.  These routines perform all direct file
**      I/O for all database files, such as database tables,
**      configuration files, etc.  It is not meant to be used for
**      such operations as copying tables to user files, the 
**      stream I/O(SI) routines are used for this.  The main
**      structure used to keep track of operating system information
**      needed to perform the I/O is called DI_IO and is defined
**      below.  This structure varies significantly depending
**      on the operating system.
**
** Specification:
**  
**  Description:
**	Direct file I/O for shared files. These files have the following
**	extensions:
**
**               .cnf  - For configuration file.
**               .tab  - For all database files.
**               .d##  - For all files waiting to be deleted.
**                       If any of these are left around 
**                       while a database is not in use they
**                       can be deleted.
**               .m##  - The new  database file as a result of
**                       a modify.  If modify is successfull, then
**                       at the end it is changed to the .tab extention
**                       identifier.  If any of these are left 
**                       around while a database is not in use, they
**                       can be deleted.
**               .ali  - Detects aliasing of locations during 
**                       checkpointing.  This is a temporary table
**                       and can be deleted if left around while 
**                       a database is not in use.
**               .ovl  - The overflow pages during the modify to
**		         ISAM.  If modify is successfull, then at
**                       the end it is deleted.  If any of these are
**                       left around while a database is not in use,
**                       they can be deleted.
**               .sm1  - During a sysmod, a table is temporarily 
**                       rename to a .sm1 file name.
**               .sm2  - During a sysmod, an index table is temporarily 
**                       rename to a .sm2 file name.
**                          
** 
**  Intended Uses:
**      DI is used to create, delete,  and maintain the files used 
**      by the DBMS to store the data associated with INGRES Databases.  
**      This includes all files associated with system and user tables 
**      and the database configuration file.  All these files are 
**      shared resources amoung users and processes.  No serialization
**      of calls can be expected.  For example you maybe trying
**      to open a file for one caller and be interrupted to open
**      a file for another caller.  The DI code must be able to 
**      handle this.  The DI routines
**      provide a direct access mechanism for accessing data in blocks.
**      It is a requirement of DI files that they can be created and 
**      openeed with the same block size.  Host operating systems 
**      which can check this should.  If they don't match it should
**      return DI_BADPARAM.
**      These routines are designed to provide enough functionality 
**      to insure that the data in the files can be recovered in case 
**      of system crashes or database recovery.  
**
**      Recovery also depends on a file system
**      capable of insuring the integrity of disk space structures used
**      to implement the file system.  For example, VAX/VMS must insure
**      all directory updates are atomic. DI is intended to be used only by DMF
**      and the database support routines such as CreateDB, DestroyDB,
**      VerifyDB, RecoverDB, etc.  No other part of the DBMS 
**      should be a client of the DI module.
**      
**    Assumptions:
**      It is assumed that the concept of a random access shared file
**      is understood.  The System is assumed to have some kind of
**      file system or disk I/O mechanism that DI is built on top of.  
**
**      In addition, it is assumed that the DI routines contain enough
**      functionality to insure that the main client DMF does not have
**      to have any machine dependent code to insure the same results 
**      on all machines.  For example, with some operating systems
**      all pages are physically on disk after a write operation
**      completes.  Other operating systems may complete the request
**      without actually insuring the data is on disk.  For these systems
**      the DMF will use the DIforce call to insure all data is on disk
**      at critical points like end transaction.
**      
**    Definitions:
**
**    Concepts:
**
**      The DI routines are assumed to be built on top of System service
**      routines which allow  random access to data within a file
**      by addressing a specific byte of that data.  It also assumes that
**      data can be segmented into  blocks (usually the sector size of
**      the disk ) and that contiguous blocks can be accessed as pages.
**      The basic unit of access for the DI routines is a page.  The page
**      size is set at creation time and must be openned with same size.
**      The first addressable page in a file
**      is always page 0.  For certain types of access (for example
**      read ahead) multiple pages will be accessed with one call of
**      the appropriate DI routines.  In these cases the pages must
**      be logically contiguous.
**
**      DI routines participate in guaranteeing the consistency of the
**      data.  For example the DIflush and DIforce calls exist to
**      guarantee the file header information and the data are on
**      disk prior to transaction commit, etc.   
**
**      All DI files are sharable.  That is more than one open 
**      of the file may exist at any one time for both reading
**      and writing.
**
**      The DI routines must be reentrant, namely all information
**      that must be maintained between DI calls must be passed as
**      an argument.  To accomplish this all DI calls contain a 
**      pointer to a file context (of type DI_IO) which can be used to
**	contain any information needed for future calls.  For example the
**	System file identifier, the file allocation information such as 
**      physical end of file and logical end of file, etc.  Implementation
**      of the DI routines on different systems will have different file
**      context structures and often the file context will be ignored
**      on a specific call for a specific System.
**
**      All DI routines are responsible for providing in a structure
**      (of type CL_SYS_ERR) any operating system error that occurred
**      during execution.  It should be initialized to OK by all
**      DI routines.  This operating system error will be logged
**      by the ULF log utility routines.  The operating system errors
**      must also be converted to one of the following DI errors:
**	    
**		DI_OK           
**		DI_BADPARAM     
**		DI_BADFILE	
**		DI_ENDFILE      
**		DI_BADOPEN	
**		DI_BADCLOSE	
**		DI_BADWRITE	
**		DI_BADREAD	
**		DI_BADEXTEND	
**		DI_BADDIR	
**		DI_BADSEEK	
**		DI_BADSENSE	
**		DI_BADCREATE	
**		DI_BADDELETE    
**		DI_BADRENAME    
**              DI_NODISKSPACE
**              DI_EXCEED_LIMIT
**              DI_DIRNOTFOUND
**              DI_FILENOTFOUND
**              DI_EXISTS
**      
**	Before I/O operations can be performed on a file the file
**      must exist.  If it does not exist it must be created.  The
**      DIcreate routine only creates the file, it does not allocate
**      any physical storage or open it.  Effectively it creates an
**      entry in a directory.  After the initial creation, the file
**      can be opened by the DIopen call.  A file is created or opened
**      by name.  The name has two parts, the pathname (directory tree)
**      and the filename.  A pathname to DI is a pathname recognized by
**      the System.  A pathname can be up to DI_PATH_MAX characters.
**	Any DI call being passed a pathname will also be passed its length.
**      The filename to DI is a filename recognized by the System.  A
**      filename can be up to DI_FILE_MAX characters including any file
**	type extension.  Any DI call being passed a filename will also 
**      be passed its length.  
**
**      The DI pathname is one created by calls to LOingpath and
**      usually has at least two parts, the physical location
**      string provided when a location is defined to accessdb.
**      For VAX/VMS this is usually a disk device name such 
**      as disk$data.  However it can be a rooted directory
**      name such as disk$data:[aaa.bbb.ccc.]. This string is
**      operating system dependent.  LOingpath adds to this
**      the operating system specific ingres name to create the entire
**      path name.  The VAX/VMS ingres name is [ingres.data.dbname]
**      which would create the path disk$data:[ingres.data.dbname]
**      or disk$data:[aaa.bbb.ccc.][ingres.data.dbname].  The 
**      maximum values for the pathname and filename are defined
**      with the constants DI_PATH_MAX and DI_FILENAME_MAX.
**
**      Once a file is opened it can be extended, read, written or
**      closed.  The DI routines to perform these operations determine
**      the specific file to access through a System file identifier
**      stored in the file context.  Each of these routines relies
**      on a valid file context to function properly.  The routines
**      used to maintain the file context are: 
**
**                  DIalloc - Extend the logical & physical file.
**                  DIflush - Flush file header to disk.
**                  DIforce - Force all pages held by System to disk.
**                  DIread  - Reads pages from the file.
**                  DIwrite - Writes  pages to the file.
**                  DIsense - Determine logical end of file.
**                 
**      Since you can DIread or DIwrite more than one page
**      these routines must be able to tell you if less than 
**      the number requested was written or read.  This is
**      done by returning DI_ENDFILE for these calls instead
**      of OK when the request cannot be satisfied.
**
**      The logical end of file is maintained through the use
**      of the DIsense, DIwrite, and DIalloc calls as follows:
**         1.  DIsense is guaranteed to return correct
**             logical end-of-file.  DIsense should 
**             return the logical
**             end-of-file on disk.  For most systems
**             this will be the value written to the file
**	       header at DIflush time.  For those systems
**	       that track end-of-file by actual writes to
**             the file, then the logical end-of-file is
**             the highest numbered page ever written by 
**             DIwrite.
**	   2.  It is guaranteed that between the time 
**             DIalloc is called on a file and the time
**             DIflush is called on a file no other DIalloc
**             calls on the file will be made by either the original
**             caller or any other caller (in the current 
**             server or any other).  This is guaranteed 
**             through the use of a file extend lock.
**         3.  A file is extended by calling DIalloc to 
**             logically extend a file.  This includes 
**             a physical extention if enough space 
**             is not currently available.  For those operating
**             systems that do not have a way of extending
**	       the file without writing blocks to
**             the file, this call may do nothing but 
**             keep track of the allocation request
**             in the DI_IO control area in memory.  
**         4.  Any DIwrite call that is for a page greater than
**             the DI_IO logical end-of-file and less than or equal to
**             the DI_IO logical end-of-file + allocation request
**             can be legally written.
**             These writes can be thought of updating the physical 
**             end-of-file on disk for 
**             those file systems that do not have a file header
**             on disk.  This call 
**             would update the DI_IO logical end-of-file value.
**	   4.  Between the time DIalloc is called and the time
**             DIflush is called DIsense will return 
**             the logical end-of-file from disk.
**	   5.  The DIflush call causes the DI_IO logical 
**	       end-of-file value to be updated in the DI_IO
**	       context and flushed to disk for those systems
**	       that have file header information on disk.
**	   
**
**	Note that the DI_IO logical end-of-file may
**      not be the same for different servers, if one server has
**      had a file open and been actively extending it, then 
**      its DI_IO logical end-of-file reflects what has been
**      allocated even if the pages have not been written yet.
**      if another server open this file while the first server
**      is in the middle of an allocation request, the second 
**      server will initialize its DI_IO logical end-of-file 
**      from the disk information.  For file systems that
**      have a file header, then the two servers will have 
**      the same value for DI_IO logical end-of-file since
**      the header on disk is not updated until the DIflush
**      call.  However, for those systems that keep track of
**      the end-of-file on disk by what has been written, the
**      second server can have a DI_IO logical end-of-file 
**      that is different from the first one.  However DMF
**      guarantees in a multi-server environment that the
**      disk is always consistent, therefore the DI_IO logical
**      end-of-file in the second server will always be 
**      greater than the one in the first server 
**
**      When you are finished with a file it must be closed with a
**      call to DIclose.  If the file is no longer needed it should
**      be deleted with a call to DIdelete.  You cannot delete a 
**      file that is open.  A delete call can be made while the file 
**      is opened, but the file will not be deleted until all 
**      opened contexts on this file are closed.  It is not possible
**      to open a file that has been marked for deletion.  Any other
**      user who has the file open already may perform all other
**      operations (such as sense, alloc, read, write, close).  
**      A file that is marked for deletion cannot be renamed.
**      A delete of a file is not associated with any open or open
**      context.  The close call uses the System identifier in the
**      file context to determine which file to close, the delete
**      is done by name.  A System path and file name must be passed
**      to the delete routine.  In order to support recovery of certain
**      file operations it is sometimes necessary to delay deletion
**      of a file to after transaction commit.  These files are 
**      often renamed until they can be deleted.  Therefore there is
**      a DIrename routine.  The file must be closed to rename it.
**
**      The other DI routines are needed to create paths (directories)
**      and destroy paths (directories) for the CreateDB and DestroyDB
**      support routines.  These DI routines are:
**
**	    DIdircreate - Creates a directory.
**	    DIdirdelete - Deletes a directory.
**	    DIlistdir   - List all directories in a directory.
**          DIlistfiles - List all files in a directory.
**
**	For the DIdircreate and DIcreate routines a check is
**      made to see if the object already exists.  If it does
**      either the DI_DIRNOTFOUND or DI_FILENOTFOUND must be
**      returned as errors.  DIlistdir and DIlistfiles must
**      return DI_ENDFILE when it finds the last object in 
**      a directory.
**
**      For all DI files there is a concept of quota. Therefore
**      if you try to exceed the operating system quota for 
**      a specific user or the entire process when creating,
**      opening or writing a DI file or directory, then the
**      error DI_EXCEED_LIMIT must be returned.  In addition
**      it is understood that at a minimum either the operating
**      system or the DI filesystem must be able to handle at
**      least 250 logical file opens before it can declare
**	DI_EXCEED_LIMIT;  THis is to insure that the single maximum
**	query can  be run. (32 table join with at most 6 indexes on each
**      table = 192).
**    
**	The concept of physical file ownership for such purposes
**      as operating system accounting and quota must be handled
**      by DIcreate and DIdircreate.  If a particular operating 
**      system requires that the files be owned by the user running 
**      at the time of the create the DI routines must use
**      the appropriate calls to CS to determine who is running,
**      set appropriate user id to this user and then make the
**      operating system call so the file is associated with
**      the user instead of the process.  This insures that in 
**      a server process the ingres user does not own
**      all the tables it creates. 
**      
**      To allow for multiple locations for one table (i.e. 
**      table spread accross multiple disk drives) two new
**      constants have been added to DI.  DI_EXTENT_SIZE is
**      the number of pages to be written to each disk before
**      writing to the next disk.  For example if DI_EXTENT_SIZE 
**      is four and there are three locations, the logical pages
**      0-24 would be allocated as follows:
**               Location 0  Pages 0,1,2,3,  12,13,24,15
**               Location 1  Pages 4,5,6,7,  16,17,18,19
**               Location 2  Pages 7,8,9,10, 11,12,13,14
**      
**      The DI_EXTENT_SIZE must be a power of two to make the 
**      calculations for determining where to write a logical 
**      page simple.  There is an additional constant used
**      in these calculations which is log2 of the extent 
**      size and is used in shifting.  The constant is 
**      DI_LOG2_EXTENT_SIZE.
**
**  History:
**      06-sep-85 (jennifer)
**          Created new for 6.0.
**	15-nov-87 (rogerk)
**	    Changed DI_FILENAME_MAX from 12 to 28.  This is so DI
**	    routines can handle directory names which are can be
**	    up to 24 (supported database name length) characters.
**	    28 is 24 + 4 bytes for ".dir".
**      10-DEC-87 (JENNIFER)
**          Added multiple locations constants.
**	18-jan-89 (mikem)
**	    DI_SYNC_MASK support.
**	21-jan-89 (mikem)
**	    Some DI slave statistics gathering support.
**	    DI_SYNC_MASK support.
**	06-feb-89 (mikem)
**	    Added DI_IO_*_INVALID constants to keep track of who marks a
**	    DI_IO invalid.
**	08-aug-89 (mikem)
**	    Added DI_O_NODISKSPACE_DEBUG for debugging out of disk 
** 	    space handling - see diwrite/dialloc.
**      22-may-90 (blaise)
**          Integrated changes from 61 and ingresug:
**            Remove superfluous "typedef" before structures.
**	30-October-1992 (rmuth)
**	    - Add new function calls DIgalloc and DIguarantee_space.
**	    - Add some new DI error messages.
**	    - Remove DI_ZERO_ALLOC and DI_O_ZERO_ALLOC.
**	    - Prototype some of the functions.
**	1-dec-92 (ed)
**	    - increased DI_MAXNAME to 36, to complement DB_MAXNAME=32
**	    36-32 is the length of ".dir" required for directories.
**	1-jan-1992 (rmuth)
**	    - Prototype the remainder of DI.
**	    - Add some DIlru error messages.
**	10-mar-1993 (mikem)
**	    bug #47626
**	    Added DI_INPROGRESS and DI_BADRELEASE to do better error checking
**          in DI of asynchronous event errors.
**	26-apr-1993 (bryanp)
**	    Prototyped DIlistdir and DIlistfile.
**	    Please note that arg_list is a PTR, not a (PTR *).
**	7-jun-93 (ed)
**	    created glhdr version contain prototypes, renamed existing file
**	    to xxcl.h and removed any func_externs
**	26-jul-1993 (bryanp)
**	    Added DI_SLAVE_INIT_ERROR.
**	18-apr-1994 (jnash)
**	    fsync() project.  Added DI_USER_SYNC_MASK, DI_O_USYNC_MASK.
**	01-may-1995 (amo ICL)
**	    Added ANB's asyncio and gather write amendments
**	20-jul-1995 (canor01)
**	    add DI_LOG_FILE_MASK for use with DIopen() for those versions
**	    of DI that may need it
**	09-nov-1995 (itb ICL)
**	    Added the DI_CHECK_LISTIO flag for DIwrite_list.
**	03-jun-1996 (canor01)
**	    Added io_sem to DI_IO for operating-system thread coordination.
**	02-sep-1997 (wonst02)
**	    Added DI_ACCESS for file open access error.
**	04-dec-1997 (canor01)
**	    Add definition of DIlru_free() and DIlru_flush() for internal
**	    use by other CL facilities.
**	18-Dec-1997 (jenjo02)
**	    Added uniq_di_file to DI_UNIQUE_ID structure.
**	07-jan-1998 (canor01)
**	    Add io_next and io_prev to DI_IO structure.
**	12-feb-1998 (canor01)
**	    Add io_refcnt to DI_IO structure.
**	02-mar-1998 (canor01)
**	    Restore io_next and io_prev which got lost in integration.
**	01-oct-1998 (somsa01)
**	    Added DI_NODISKSPACE to distinguish between an out-of-disk-space
**	    condition and an out-of-quota condition.
**	04-Dec-1998 (muhpa01)
**	    Include <cs.h> for non-OS_THREADS platforms - needed for recent
**	    cross process changes which are using CS_ structures.
**	22-Dec-1998 (jenjo02)
**	  o Added DI_RAWIO_ALIGN defines, previously scattered in lgdef.h,
**	    dicl.h, now apply to raw database files as well as log files.
**	  o Added io_fd_q QUEUE for (experimental) maintaining list of 
**	    open FDs per file when DI is compiled with DI_FD_PER_THREAD.
**	  o Changed uniq_seqno to u_i4.
**	10-may-1999 (walro03)
**	    Remove obsolete version string ps2_us5, sqs_u42, sqs_us5.
**	08-dec-1999 (somsa01)
**	    On NT, redefined DI_RAWIO_ALIGN to be equal to the system
**	    memory page size.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	02-jan-2001 (mosjo01)
**	    for sqs_ptx set DI_RAWIO_ALIGN to 512 to support Direct I/O
**	    that as of Jan. 2001 seems to be exclusively a feature of 
**	    Sequent Dynix systems (sqs_ptx), specifically for the environment
**	    variable II_DIRECT_IO = 'y' that can be passed to cldio.c
**	08-feb-2001 (somsa01)
**	    Set DI_RAWIO_ALIGN for i64_win.
**	25-oct-2001 (somsa01)
**	    Changed "int_wnt" to "int_w32".
**	23-sep-2003 (somsa01)
**	    For Windows, added io_nt_gw_fh to the DI_IO struct. This file
**	    handle will strictly be used for GatherWrite.
**	14-Oct-2005 (jenjo02)
**	    Added DIopen(DI_DIRECTIO_MASK) to request directio 
**	    per file, rather than "all" files.
**      20-apr-2005 (rigka01) bug 114010, INGSRV3180
**        Sessions hang in a mutex deadlock on DI LRU SEM and
**        DI <filename> SEM.
*/

/*
**  Forward and/or External typedef/struct references.
*/

/*
The following structures are defined below:
*/
typedef struct _DI_IO		DI_IO;
typedef struct _DI_UNIQUE_ID	DI_UNIQUE_ID;

/* 
** Defined Constants
*/

/* Arguments to DIinfo "operation" field */

# define		DI_INFO_SLAVE		0x001
# define		DI_INFO_SUM_SLAVE	0x002
# define		DI_INFO_SERVER		0x003
# define		DI_INFO_RCP		0x004
# define		DI_INFO_ACP		0x005
# define		DI_INFO_SLAVE_NUM	0x006

/* masks for the DIopen() flags argument */

# define		DI_SYNC_MASK		0x001
# define		DI_USER_SYNC_MASK	0x002
# define		DI_LOG_FILE_MASK	0x004
# define		DI_NOT_LRU_MASK		0x008
# define		DI_DIRECTIO_MASK	0x010

/* DI return status codes. */

# define                DI_OK           	0
# define                DI_BADPARAM     	(E_CL_MASK + E_DI_MASK + 0x01)
# define		DI_BADFILE		(E_CL_MASK + E_DI_MASK + 0x02)
# define                DI_ENDFILE      	(E_CL_MASK + E_DI_MASK + 0x03)
# define		DI_BADOPEN		(E_CL_MASK + E_DI_MASK + 0x04)
# define		DI_BADCLOSE		(E_CL_MASK + E_DI_MASK + 0x05)
# define		DI_BADWRITE		(E_CL_MASK + E_DI_MASK + 0x06)
# define		DI_BADREAD		(E_CL_MASK + E_DI_MASK + 0x07)
# define		DI_BADEXTEND		(E_CL_MASK + E_DI_MASK + 0x08)
# define		DI_BADDIR		(E_CL_MASK + E_DI_MASK + 0x09)
# define		DI_BADINFO		(E_CL_MASK + E_DI_MASK + 0x0A)
# define		DI_BADSENSE		(E_CL_MASK + E_DI_MASK + 0x0B)
# define		DI_BADCREATE		(E_CL_MASK + E_DI_MASK + 0x0C)
# define		DI_BADDELETE    	(E_CL_MASK + E_DI_MASK + 0x0D)
# define                DI_BADRNAME     	(E_CL_MASK + E_DI_MASK + 0x0E)
# define                DI_EXCEED_LIMIT 	(E_CL_MASK + E_DI_MASK + 0x0F)
# define                DI_DIRNOTFOUND  	(E_CL_MASK + E_DI_MASK + 0x10)
# define                DI_FILENOTFOUND 	(E_CL_MASK + E_DI_MASK + 0x11)
# define                DI_EXISTS       	(E_CL_MASK + E_DI_MASK + 0x12)
# define                DI_BADLIST      	(E_CL_MASK + E_DI_MASK + 0x13)
# define		DI_INPROGRESS		(E_CL_MASK + E_DI_MASK + 0x14)
# define		DI_BADRELEASE		(E_CL_MASK + E_DI_MASK + 0x15)
# define		DI_GENERAL_ERR		(E_CL_MASK + E_DI_MASK + 0x16)
# define		DI_SLAVE_INIT_ERROR	(E_CL_MASK + E_DI_MASK + 0x17)
# define		DI_OPENDIR_FAILED	(E_CL_MASK + E_DI_MASK + 0x18)
# define		DI_MEREQMEM_ERR		(E_CL_MASK + E_DI_MASK + 0x19)
# define		DI_NO_AIO_RESOURCE	(E_CL_MASK + E_DI_MASK + 0x1A)
# define		DI_ACCESS		(E_CL_MASK + E_DI_MASK + 0x1B)
# define		DI_NODISKSPACE		(E_CL_MASK + E_DI_MASK + 0x1C)

# define		DILRU_RELEASE_ERR	(E_CL_MASK + E_DI_MASK + 0x20)
# define                DILRU_CLOSE_ERR         (E_CL_MASK + E_DI_MASK + 0x21)
# define                DILRU_OPEN_ERR          (E_CL_MASK + E_DI_MASK + 0x22)
# define                DILRU_INIT_ERR          (E_CL_MASK + E_DI_MASK + 0x23)
# define                DILRU_FREE_ERR          (E_CL_MASK + E_DI_MASK + 0x24)
# define                DILRU_INPROC_ERR        (E_CL_MASK + E_DI_MASK + 0x25)
# define                DILRU_UNIQ_ERR          (E_CL_MASK + E_DI_MASK + 0x26)
# define                DILRU_FLUSH_ERR         (E_CL_MASK + E_DI_MASK + 0x27)

/*
** DI errors which can occur in both the DILRU and DI code. These error
** messages are used to set the err_code.intern field to provide more
** information on tracking down the error.
*/
# define		DI_LRU_PSEM_ERR		(E_CL_MASK + E_DI_MASK + 0x40)
# define		DI_LRU_VSEM_ERR		(E_CL_MASK + E_DI_MASK + 0x41)
# define		DI_LRU_CSRESERVE_ERR	(E_CL_MASK + E_DI_MASK + 0x42)
# define		DI_LRU_GENPSEM_ERR	(E_CL_MASK + E_DI_MASK + 0x43)
# define		DI_LRU_GENVSEM_ERR	(E_CL_MASK + E_DI_MASK + 0x44)
# define		DI_LRU_CSCAUSE_EV_ERR	(E_CL_MASK + E_DI_MASK + 0x45)
# define		DI_LRU_CSSUSPEND_ERR	(E_CL_MASK + E_DI_MASK + 0x46)
# define		DI_LRU_CSFREE_EV_ERR	(E_CL_MASK + E_DI_MASK + 0x47)

/*
** DI LRU specific errors
*/
# define		DILRU_CSCND_WT_ERR	(E_CL_MASK + E_DI_MASK + 0x50)
# define		DILRU_MEREQMEM_ERR	(E_CL_MASK + E_DI_MASK + 0x51)
# define		DILRU_SLAVE_OP_ERR	(E_CL_MASK + E_DI_MASK + 0x52)
# define		DILRU_CSCND_BRD_ERR	(E_CL_MASK + E_DI_MASK + 0x53)
# define		DILRU_ALL_PINNED_ERR	(E_CL_MASK + E_DI_MASK + 0x54)
# define		DILRU_VIC_NOT_OP	(E_CL_MASK + E_DI_MASK + 0x55)
# define		DILRU_CSCND_INIT_ERR	(E_CL_MASK + E_DI_MASK + 0x56)
# define		DILRU_CACHE_LIMIT	(E_CL_MASK + E_DI_MASK + 0X57)



/* 
** Related symbolic constants used for multiple locations in DIroutines.
**      To allow for multiple locations for one table (i.e. 
**      table spread accross multiple disk drives) two new
**      constants have been added to DI.  DI_EXTENT_SIZE is
**      the number of pages to be written to each disk before
**      writing to the next disk.  For example if DI_EXTENT_SIZE 
**      is four and there are three locations, the logical pages
**      0-24 would be allocated as follows:
**               Location 0  Pages 0,1,2,3,  12,13,24,15
**               Location 1  Pages 4,5,6,7,  16,17,18,19
**               Location 2  Pages 7,8,9,10, 11,12,13,14
**      
**      The DI_EXTENT_SIZE must be a power of two to make the 
**      calculations for determining where to write a logical 
**      page simple.  There is an additional constant used
**      in these calculations which is log2 of the extent 
**      size and is used in shifting.  The constant is 
**      DI_LOG2_EXTENT_SIZE.
*/
# define                DI_EXTENT_SIZE     16
# define                DI_LOG2_EXTENT_SIZE 4

/* 
** Related symbolic constants used to for DIroutines.
*/

# define                DI_PATH_MAX        128
# define                DI_FILENAME_MAX    36

/*
** Alignment required on this machine for buffers involved in i/o
** to raw disk partitions.  For /dev/rxy0b, consult "man xy" for details
** of your hardware.  512 should always be sufficient, but you might not
** want to "waste" that much space.  I don't think it really matters. (daveb)
*/

# if defined(sqs_ptx)
# define DI_RAWIO_ALIGN	512
# else
# if defined(int_w32)
# define DI_RAWIO_ALIGN 4096
# else
# if defined(axp_wnt) || defined(NT_IA64)
# define DI_RAWIO_ALIGN 8192
# endif
# endif
# endif

# ifndef DI_RAWIO_ALIGN
# define DI_RAWIO_ALIGN	512
# endif

/*}
** Name: DI_UNIQUE_ID - unique per-server identifier for DI file
**
** Description:
**	The DI_UNIQUE_ID is generated by the DIopen call and is
**	stored internally in the DI_IO struct.  It is used to optimize
**	the open file descriptor caching done by DI on UNIX systems.
**	The DI_UNIQUE_ID is simply a set of numbers assigned to a
**	path and filename at open-file time.  It is guaranteed that
**	no other file open'd in a server will have the same DI_UNIQUE_ID.
**	DIlru_uniq() an internal DI routine assigns these unique id's.
**
** History:
**     09-mar-87 (mmm)
**          Created new for jupiter.
**	18-Dec-1997 (jenjo02)
**	    Added uniq_di_file to DI_UNIQUE_ID structure.
**	    Removed uniq_pid, uniq_time.
**	22-Dec-1998 (jenjo02)
**	    Changed uniq_seqno to u_i4.
*/
struct _DI_UNIQUE_ID
{
    PTR	   	    uniq_di_file;	/* Pointer to file descriptor 
					** cache entry for quick, non-hash
					** lookup.
					*/
    u_i4	    uniq_seqno;		/* sequence number */
};

/*}
** Name: DI_IO - This is the I/O structure needed to do direct I/O.
**
** Description:
**      The DI_IO structure is used to contain all the operating
**      system dependent I/O information required for direct I/O.
**      It is very operating system specific.  The memory for this
**      structure is allocated by the caller of the DI routines.
**	This is the structured used on VAX/VMS.
**
** History:
**     06-sep-85 (jennifer)
**          Created new for 5.0.
**	18-jan-89 (mikem)
**	    Added DI_O_FSYNC_MASK, DI_O_OSYNC_MASK
**	06-feb-89 (mikem)
**	    Added DI_IO_*_INVALID constants to keep track of who marks a
**	    DI_IO invalid.
**	08-aug-89 (mikem)
**	    Added DI_O_NODISKSPACE_DEBUG for debugging out of disk 
** 	    space handling - see diwrite/dialloc.
**	13-Nov-90 (anton)
**	    Added DI_O_OPEN_IN_PROGRESS and DI_O_I_WANT to
**	    solve reopen race condition using CSconditions.
**	30-September-1992 (rmuth)
**	    Remove DI_O_ZERO_ALLOC.
**	18-apr-1994 (jnash)
**	    Add DI_O_USYNC_MASK.
**	12-may-1994 (rog)
**	    Added DI_O_STICKY_UNSET to flag when we have turned off the 
**	    sticky bit when were not using O_SYNC (SunOS only).
**	03-jun-1996 (canor01)
**	    Added io_sem for operating-system thread coordination.
**	22-Dec-1998 (jenjo02)
**	    Added io_fd_q QUEUE for (experimental) maintaining list of 
**	    open FDs per file when DI is compiled with DI_FD_PER_THREAD.
**	23-sep-2003 (somsa01)
**	    For Windows, added io_nt_gw_fh to the DI_IO struct. This file
**	    handle will strictly be used for GatherWrite.
**	25-Jul-2005 (schka24)
**	    Need a mutex to protect the io_fd_q.
**	30-Sep-2005 (jenjo02)
**	    Changed Karl's mutex to a CS_SYNCH object.
**	14-Oct-2005 (jenjo02)
**	    Chris's file descriptor properties now cached in io_fprop
**	    (file properties) and established on the first open, 
**	    not every open. Stole "io_reserved1" for this purpose.
**	26-apr-2005 (rigka01)
**	    Added flag indicating whether or not file is on LRU queue
*/
struct _DI_IO
{
# ifdef NT_GENERIC
    DI_IO	    *io_next;
    DI_IO	    *io_prev;
# else
    QUEUE	    io_fd_q;		/* Queue of fds open on this file */
# endif /* NT_GENERIC */
    i4	    io_system_eof;	/* End of file information.  */
    i4	    io_alloc_eof;	/* Last allocated disk block.*/
    i4	    io_type;            /* Control block identifier. */
# if defined(NT_GENERIC)
    HANDLE  io_nt_fh;		/* NT file handle            */
    HANDLE  io_nt_gw_fh;	/* NT file handle for GatherWrite */
# endif /* NT_GENERIC */
# ifdef OS_THREADS_USED
    CS_SEMAPHORE    io_sem;	/* Held during IO if multiple OS calls needed */
    CS_SYNCH        io_fd_q_sem; /* Protects io_fd_q */
# endif /* OS_THREADS_USED */

#define                 DI_IO_ASCII_ID     	0x44494342	/* 'DICB' */
#define			DI_IO_LFILE_INVALID	-1
#define			DI_IO_LDIR_INVALID	-2
#define			DI_IO_LRUCLOSE_INVALID	-3
#define			DI_IO_OPEN_INVALID	-4
#define			DI_IO_CLOSE_INVALID	-5
#define			DI_IO_DCREATE_INVALID	-6
#define			DI_IO_DDELETE_INVALID	-7

    i4	    io_mode;            /* mode the file was opened with */
    i4 	    io_bytes_per_page;	/* File block size. */
    i4	    io_refcnt;	        /* I/O in progress count */
    u_i4    io_fprop;       	/* File properties, from open,
				** see cldio.h for content.
				*/
#define			DI_IO_BLK_SIZE		512
    i4	    io_queue;		/* LRU queue or some other or none */
#define			DI_IO_NO_QUEUE		0
#define			DI_IO_LRU_QUEUE		1	
    i4	    io_logical_eof;	/* logical end of file */
    DI_UNIQUE_ID    io_uniq;		/* uniq identifier for this file */
    u_i4	    io_open_flags;	/* flags passed in on open of file */
#define			DI_O_FSYNC_MASK		0x0001
#define			DI_O_OSYNC_MASK		0x0002
					/* totally for debugging out of disk 
					** space handling - see diwrite/dialloc
					*/
#define			DI_O_NODISKSPACE_DEBUG  0x0004	
#define			DI_O_OPEN_IN_PROGRESS	0x0008
#define			DI_O_I_WANT		0x0010
#define			DI_O_USYNC_MASK		0x0020
					/*
					** We have turned off the sticky bit
					** because we're not using O_SYNC.
					** (SunOS-ism)
					*/
#define			DI_O_STICKY_UNSET	0x0040
#define			DI_O_LOG_FILE_MASK	0x0080
					/* This file is not LRU eligible */
#define			DI_O_NOT_LRU_MASK	0x0100
    u_i4	    io_l_pathname;	/* length of pathname */
    u_i4	    io_l_filename;	/* length of filename */

    char	    io_pathname[DI_PATH_MAX];
					/* pathname copied from DIopen() */
    char	    io_filename[DI_FILENAME_MAX];
					/* filename copied from DIopen() */
    struct				/* File statistics */
    {
      u_i4	    alloc;
      u_i4	    close;
      u_i4	    create;
      u_i4	    delete;
      u_i4	    flush;
      u_i4	    force;
      u_i4	    galloc;
      u_i4	    guara;
      u_i4	    open;
      u_i4	    read;
      u_i4	    sense;
      u_i4	    write;
      u_i4	    lru_open;
      u_i4	    lru_close;
      u_i4	    reserved[10];
    }		io_stat;
};
typedef struct  _DI_RESOURCE_INFO	DI_RESOURCE_INFO;

/*}
** Name: DI_RESOURCE_INFO - resource gathering info about the DI slaves
**
** Description:
**	structure to communicate di slave resource info between server
**	and slave.  
**
** History:
**	21-jan-89 (mikem)
**	    created.
**      01-jan-88 (login)
**          comment about changes
*/
struct _DI_RESOURCE_INFO
{
    i4	    dires_u_sec;           /* comment about member of struct */
    i4	    dires_u_usec;           /* comment about member of struct */
    i4	    dires_s_sec;           /* comment about member of struct */
    i4	    dires_s_usec;           /* comment about member of struct */
    i4	    dires_maxrss;
    i4	    dires_ixrss;      /* integral shared text memory size */
    i4	    dires_idrss;      /* integral unshared data size */
    i4	    dires_isrss;           /* integral unshared stack size */
    i4	    dires_minflt;          /* page reclaims */
    i4	    dires_majflt;          /* page faults */
    i4	    dires_nswap;      /* swaps */
    i4	    dires_inblock;         /* block input operations */
    i4	    dires_oublock;         /* block output operations */
    i4	    dires_msgsnd;          /* messages sent */
    i4	    dires_msgrcv;          /* messages received */
    i4	    dires_nsignals;        /* signals received */
    i4	    dires_nvcsw;      /* voluntary context switches */
    i4	    dires_nivcsw;          /* involuntary context switches */
};

/* 
** Related symbolic constants used to initialize DI_IO 
** information.
*/

# define                DI_IO_READ         0L
# define                DI_IO_WRITE        1L

/*
** Operation flags for DIwrite_list()
*/
#define DI_QUEUE_LISTIO 0x01
#define DI_CHECK_LISTIO 0x02
#define DI_FORCE_LISTIO 0x04

FUNC_EXTERN STATUS DImo_init( VOID );

FUNC_EXTERN STATUS DIlru_free( CL_ERR_DESC  *err_code );
FUNC_EXTERN STATUS DIlru_flush( CL_ERR_DESC *err_code );

#endif		/* DI_INCLUDED */
