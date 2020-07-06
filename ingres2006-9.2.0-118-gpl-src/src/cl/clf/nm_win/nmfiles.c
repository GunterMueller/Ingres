/*
** Copyright (c) 1987, 2004 Ingres Corporation
*/

# include	<compat.h>
# include	<gl.h>
# include	<clconfig.h>
# include	<er.h>
# include	<gc.h>
# include	<gcccl.h>
# include	<id.h>
# include	<lo.h>
# include	<nm.h>
# include	<pc.h>
# include	<pe.h>
# include	<si.h>
# include	<me.h>
# include	<meprivate.h>
# include	<st.h>
# include	"nmlocal.h"

/**
** Name: NMFILES.C - Functions used to implement the nm file manipulations:
**
** Description:
**      This file contains the following external routines:
**    
**      NMloc() 	       -  Return location associated with input value
**      NMf_open() 	       -  Open location associated with file in the
**				  files directory.
**      NMt_topen() 	       -  Open location associated with file in the
**				  temp location.
**	NMlogOperation()       -  Log a symbol table update action.
**
**      This file contains the following internal routines:
**
**      NMopensyms()	       -  Open table of symbols.
**      NMaddsym()	       -  add symbol.
**      NMreadsyms()	       -  read file with symbols.
**      NMgtIngAt()            -  Get an ingres symbol.
**
** History: Log:	nmfiles.c,v
 * Revision 1.3  88/12/12  17:23:21  billc
 * bugfix -- don't cache open failure except for reads.
 * 
 * Revision 1.2  88/11/30  13:41:26  billc
 * if opensym fails, don't keep trying.
 * 
 * Revision 1.1  88/08/05  13:43:12  roger
 * UNIX 6.0 Compatibility Library as of 12:23 p.m. August 5, 1988.
 * 
** Revision 1.5  88/07/01  09:39:46  mikem
** handle UTILITY case correctly on UNIX.
**      
** Revision 1.4  88/01/07  13:15:39  mikem
** use new environment variables.
**      
** Revision 1.3  87/11/13  11:33:49  mikem
** use nmlocal.h rather than "nm.h" to disambiguate the global nm.h header from
** the local nm.h header.
**      
** Revision 1.2  87/11/10  14:44:29  mikem
** Initial integration of 6.0 changes into 50.04 cl to create 
** 6.0 baseline cl
**      
**	12-Mar-89 (GordonW)
**	    added call to LOlast to update symbol table modification time and
**	    added NMflushIng() routine to flush symbol cache.
**      26-aug-89 (rexl)
**          Added calls to protect stream allocator.
**	16-feb-90 (greg)
**	    NMgtAt() is a VOID not STATUS.
**	27-Mar-90 (seiwald)
**	    Added support for II_ADMIN, the environment variable which 
**	    points to the ADMIN location, which houses (at least) the
**	    symbol table file.   If unset, II_ADMIN defaults to the
**	    directory $II_SYSTEM/ingres/admin/<hostname>, if present,
**	    and then to $II_SYSTEM/ingres/files.  Specific changes are:
**	
**	    o	Introduced NM_initsym(), to determine II_SYSTEM and II_ADMIN.
**	    o	Restructured NMopensyms() to use the ADMIN directory.
**	    o	Restructured NMloc() to understand the ADMIN location.
**	    o	Removed NMsyspath(); NM_initsym() does its job.
**	    o	Collected some statics into central structure.
**	    o	Removed SEINGRES references.
**      30-May-90 (seiwald)
**          Fixed SUBDIR case broken by above change.
**    11-aug-93 (ed)
**      add missing include
**    14-oct-94 (nanpr01)
**      Removed #include "nmerr.h". Bug # 63157.
**    19-jun-95 (emmag)
**       Desktop porting changes - directory separation, and changed call
**	 to gethostname to GetComputerName.
**    06-jun-1995 (canor01)
**      Semaphore protect memory allocation routines
**      12-jun-1995 (canor01)
**          semaphore protect NMloc()'s static buffer (MCT)
**	22-aug-95 (canor01)
**	    NMopensyms(), NMreadsyms() semaphore protected (MCT)
**     8-aug-95 (allst01)
**	Fixed the truncated symbol.tbl for su4_cmw ONLY.
**	A generic fix for both secure and normal systems
**	may be very difficult. See the later discussion.
**      12-Dec-95 (fanra01)
**          Added definitions for extracting data for DLLs on windows NT
**	27-mar-96 (thoda04)
**	    In NMloc(), pass copy of dname to LOfroms, else LOfroms() 
**	    may try to change the constant string.
**	03-jun-1996 (canor01)
**	    Clean up semaphore usage for use with operating-system threads.
**	    Use local semaphore to protect static structure.
**	    Make NMloc()'s static buffer thread-specific.
**	23-sep-1996 (canor01)
**	    Move global data definitions to nmdata.c.
**	10-oct-1996 (canor01)
**	    Allow generic system parameter overrides.
**      22-nov-1996 (canor01)
**          Changed names of CL-specific thread-local storage functions
**          from "MEtls..." to "ME_tls..." to avoid conflict with new
**          functions in GL.  Included <meprivate.h> for function prototypes.
**  13-mar-1996 (harpa06)
**      If II_SYSTEM cannot be obtained by the environment, get rid of the
**      semaphore required to read the symbol table in NM_initsym().
**      27-may-97 (mcgem01)
**          Clean up compiler warnings.
**	17-jan-1998 (canor01)
**	    Allow functions to be called from programs not linked with
**	    threading libraries.
**	09-sep-1998 (canor01)
**	    Use NMgtAt() to retrieve II_SYSTEM.  This allows the value to
**	    be retrieved from the registry if it is not in the local
**	    environment.
**      06-aug-1999 (mcgem01)
**          Replace nat and longnat with i4.
**	28-jan-2003 (somsa01)
**	    Extended symbol.tbl fix for sun_cmw to Windows as well. Also,
**	    removed code which is not used on Windows.
**	14-jan-2004 (somsa01)
**	    In NM_initsym(), added initialization of NMBakSymloc, NMLogSymloc
**	    and NMLckSymloc. In NMopensyms(), do not create the symbol.tbl on
**	    a failed read since will be created properly when we have
**	    something to add to it. In NMreadsyms(), treat an empty symbol.tbl
**	    file as a fatal error. In NMgtIngAt(), make sure symbol.tbl exists
**	    before testing for its modification. In NMflushIng(), reset
**	    ReadFailed so that a proper read can be performed again. Also
**	    added NMlogOperation(), which will log update operations on the
**	    symbol table file to symbol.log.
**	03-feb-2004 (somsa01)
**	    Backed out last changes for now.
**	06-apr-2004 (somsa01)
**	    In NM_initsym(), added initialization of NMBakSymloc and
**	    NMLogSymloc. Also, added NMlogOperation(), which will log update
**	    operations on the symbol table file to symbol.log.
**	20-Jul-2004 (lakvi01)
**		SIR #112703, cleaned-up warnings.
**      26-Jul-2004 (lakvi01)
**          Backed-out the above change to keep the open-source stable.
**          Will be revisited and submitted at a later date. 
**  16-Aug-2005 (fanra01)
**      Bug 115070
**      An exception in a process caused by a read failure of the symbol table
**      was detected with no more information.
**      Add log output of a failed SIopen from any NM function.
**	09-Aug-2005 (fanra01 & drivi01)
**		Occasionally, attempt to retrieve a symbol from a symbol table results
**		in a NULL value returned b/c of the following reasons:
**		1. We fail to open a table with Access Denied message.
**		2. We sucessfully open a table, but it's empty
**		3. We sucessfully open a table, but fail to get records
**		   b/c table gets trashed.
**      The NULL value returned only happens in the environment where
**	    symbol table is being accessed by multiple processes with 
**      concurrent read/writes.
**      These problems are being resolved in this change in the following
**      way:
**		1. In NMgtIntAt, added routines will retry openning of the table
**         for some number of times or until the access is granted.
**		2. In NMgtIngAt, added routines will test the list of symbols
**		   retrieved and if it's NULL will attempt to reopen and retrieve
**         the list of symbols for some number of times or until the list
**		   is not null.  In the same routines we'll test for trashed symbol
**	       table, and if it's detected, we'll attempt to reopen the file
**		   and reload the data.
**      To summarize, addition of these routines gives NM intelligence  
**      to handle situation where symbol.tbl is being used and might not
**      reflect accurate state of the file.
**  24-Oct-2005 (drivi01)
**	Modified routines that create symbol.tbl file if itsn't there already
**	and eliminated wait loops from previous change in case symbol.tbl file
**	isn't there to avoid slow builds.
**/

/* # defines */
# ifdef NT_GENERIC
# define NM_LOG_ERROR( msg, count, value, mode, err ) \
{ \
    int     error_no = err; \
    time_t  ltime; \
    char    p[MAX_LOC +1]; \
    char*   s = NULL; \
    FILE*   f; \
    if (NMpathIng( &s ) == OK) \
    { \
        sprintf( p, "%s\\files\\nm.log", s ); \
        if ((f = fopen( p, "a+" )) != NULL) \
        { \
            time(&ltime); \
            fprintf( f, "%s%s\n%06d-%06d %s %d: errno=%d(%s) %s %s\n", ctime( &ltime ), \
                GetCommandLine(), GetCurrentProcessId(), GetCurrentThreadId(), \
                msg, count, error_no, errstr[error_no], \
                (value && *value) ? value : "Unknown", mode ); \
            fclose( f ); \
        } \
    } \
}
# else  /* NT_GENERIC */
# define NM_LOG_ERROR( msg, count, value, mode, err )
# endif /* NT_GENERIC */

# define NM_OPEN_RETRY  50
# define NM_OPEN_WAIT   10
# define NM_OPEN_WAIT_MAX	30

/* typedefs */
/* forward references */
/* externs */
 
/* Globals */
GLOBALREF SYM		*s_list;		/* list of symbols */
GLOBALREF SYSTIME	NMtime;			/* modification time */
GLOBALREF LOCATION	NMSymloc;		/* location of symbol.tbl */
GLOBALREF II_NM_STATIC  NM_static;
GLOBALREF LOCATION	NMBakSymloc;		/* location of symbol.bak */
GLOBALREF LOCATION	NMLogSymloc;		/* location of symbol.log */

/* statics */

static char	NMSymbuf[ MAX_LOC + 1 ];	/* buffer for Symloc */
static char	NMBakSymbuf[MAX_LOC + 1];	/* buffer for BakSymloc */
static char	NMLogSymbuf[MAX_LOC + 1];	/* buffer for LogSymloc */

static char* errstr[] = {
    "\0",
    "EPERM",        
    "ENOENT",       
    "ESRCH",        
    "EINTR",        
    "EIO",          
    "ENXIO",        
    "E2BIG",        
    "ENOEXEC",      
    "EBADF",        
    "ECHILD",       
    "EAGAIN",       
    "ENOMEM",       
    "EACCES",       
    "EFAULT",       
    "\0",
    "EBUSY",        
    "EEXIST",       
    "EXDEV",        
    "ENODEV",       
    "ENOTDIR",      
    "EISDIR",       
    "EINVAL",       
    "ENFILE",       
    "EMFILE",       
    "ENOTTY",       
    "\0",
    "EFBIG",        
    "ENOSPC",       
    "ESPIPE",       
    "EROFS",        
    "EMLINK",       
    "EPIPE",        
    "EDOM",         
    "ERANGE",       
    "\0",
    "EDEADLK",      
    "\0",
    "ENAMETOOLONG", 
    "ENOLCK",       
    "ENOSYS",       
    "ENOTEMPTY",    
    "EILSEQ" };     

# ifdef OS_THREADS_USED
static ME_TLS_KEY	NMlockey = 0;
# endif


/*{
** Name: NM_initsym - initialize known, frequently requested locations
**       (The function call must be semaphore protected for MCT).
**
** Inputs:
**	None
**
** Outputs:
**	None
**
** Returns:
**	OK
**	status
**
** History:
**	28-Mar-90 (seiwald)
**	    Written.
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**  13-mar-1996 (harpa06)
**      If II_SYSTEM cannot be obtained by the environment, get rid of the
**      semaphore required to read the symbol table.
**	29-sep-2000 (devjo01)
**	    Make sure user does not pass a value of II_ADMIN that will
**	    overflow 'Admbuf' (b102777).
**	02-oct-2000 (devjo01)
**	    Put in similar check for II_SYSTEM (b102788).
**	06-apr-2004 (somsa01)
**	    Added initialization of NMBakSymloc and NMLogSymloc.
*/

STATUS
NM_initsym()
{
	char	*sym;
	bool	found;

	MUw_semaphore( &NM_static.Sem, "NM local sem" );
	MUp_semaphore( &NM_static.Sem );

	NM_static.Init = TRUE;

	/*
	** Get II_SYSTEM from the environment.
	*/

    NMgtAt( SystemLocationVariable, &sym );
    if ( !sym || !*sym )
    {
        MUv_semaphore( &NM_static.Sem );            
	    return NM_INGUSR;
    }
    if ( STlength(sym) + 1 + STlength(SystemLocationSubdirectory) > MAX_LOC )
    {
	MUv_semaphore( &NM_static.Sem );
	return NM_TOOLONG;
    }

	STcopy( sym, NM_static.Sysbuf );
	LOfroms( PATH, NM_static.Sysbuf, &NM_static.Sysloc );
	LOfaddpath( &NM_static.Sysloc, 
		    SystemLocationSubdirectory,
		    &NM_static.Sysloc );

	/*
	** Get II_ADMIN, from one of:
	**	the environment 			(if set)
	**	II_SYSTEM/ingres/admin/<hostname> 	(if present)
	**	II_SYSTEM/ingres/files 			(otherwise)
	*/

	found = FALSE;

	/* Try II_ADMIN */

	if( ( sym = getenv( "II_ADMIN" ) ) && *sym )
	{
	    if ( STlength(sym) > MAX_LOC )
	    {
		MUv_semaphore( &NM_static.Sem );
		return NM_TOOLONG;
	    }
	    STcopy( sym, NM_static.Admbuf );
	    LOfroms( PATH, NM_static.Admbuf, &NM_static.Admloc );
	    found = TRUE;
	} 

	/* Try $II_SYSTEM/ingres/admin/<hostname> */

	if( !found )
	{
	    /* IDhost() anyone? */
	    char buf[ 32 ];
	    i2 lojunk;

# ifdef NT_GENERIC
            DWORD dwMaxNameSize = MAX_COMPUTERNAME_LENGTH + 1;
            GetComputerName (buf, &dwMaxNameSize);
            STpolycat( 3, NM_static.Sysbuf, "\\admin\\",buf,NM_static.Admbuf );
# else
	    gethostname( buf, sizeof( buf ) );
	    STpolycat( 3, NM_static.Sysbuf, "/admin/", buf, NM_static.Admbuf );
# endif
	    LOfroms( PATH, NM_static.Admbuf, &NM_static.Admloc );
	    found = LOisdir( &NM_static.Admloc, &lojunk ) == OK;
	}

	/* Use $II_SYSTEM/ingres/files */

	if( !found )
	{
# ifdef NT_GENERIC
	    STpolycat( 2, NM_static.Sysbuf, "\\files", NM_static.Admbuf );
# else
	    STpolycat( 2, NM_static.Sysbuf, "/files", NM_static.Admbuf );
# endif
	    LOfroms( PATH, NM_static.Admbuf, &NM_static.Admloc );
	}

	/*
	** Build symbol.tbl path.
	*/

	LOcopy( &NM_static.Admloc, NMSymbuf, &NMSymloc );
	(void)LOfstfile( "symbol.tbl", &NMSymloc );

	/*
	** Build symbol.bak path. This file will be stored in the same
	** location as the symbol.tbl file.
	*/

	STcopy(NM_static.Admbuf, NM_static.Bakbuf);
	LOfroms(PATH, NM_static.Bakbuf, &NM_static.Bakloc);
	LOcopy(&NM_static.Bakloc, NMBakSymbuf, &NMBakSymloc);
	LOfstfile("symbol.bak", &NMBakSymloc);

	/*
	** Build symbol.log path. This file will be stored in the same
	** location as the symbol.tbl file.
	*/

	STcopy(NM_static.Admbuf, NM_static.Logbuf);
	LOfroms(PATH, NM_static.Logbuf, &NM_static.Logloc);
	LOcopy(&NM_static.Logloc, NMLogSymbuf, &NMLogSymloc);
	LOfstfile("symbol.log", &NMLogSymloc);

	/*
	** Done.
	*/

	MUv_semaphore( &NM_static.Sem );
	return OK;
}


/*{
** Name: NMloc - get a location to a given area
**
** Description:
**    set 'ploc1' to point to LOCATION whose buffer contains the
**		path to 'fname' (which depending on the value of
**		'what' is some combination of FILENAME, PATH, DEV)
**		and which is located in a sub-directory of the home
**		directory of the INGRES superuser (ING_HOME). 'which'
**		is one of the following:
**			BIN	 for executables
**			FILES	 for error files
**			LIB	 for INGRES libraries
**			TEMP	 for temporary files
**			DBTMPLT	 database template directory
**			DBDBTMPLT dbdb database template directory
**			UTILITY	 for the users file, *.bin
**			RECOVER  for qbfrecover files
**
**	NOTES:
**		UTILITY handles files that ought really be written
**			by the backend, but can't be for various
**			reasons.  They are currently "users," 
**			"acc.bin", "ext.bin" and "loc.bin", used
**			by accessdb and by finddbs.
**			If ING_UTILITY is not set, we use ING_FILES.
**			
**		TEMP is a special case in that if ING_TEMP is not
**			defined the path chosen is the directory
**			the process is running in.
**
**		RECOVER is a special case like TEMP that uses
**			the current directory if ING_RECOVER is
**			not set.  RECOVER is used only for ingres/CS.
**
**		As a static buffer is associated with 'ploc1' it may
**			be advisable to do an LOcopy() upon return.
**			The interface should (obviously) be changed
**			to require the user to pass in the required
**			buffer.
**
**	returns OK on success.
**
** Inputs:
**	which				Constant indicating what directory
**	what				type of location.
**	fname				file in the specified location
**
** Output:
**	ploc1				Resulting location.
**
**      Returns:
**          OK                         	    Function completed normally. 
**          FAIL                            Function completed abnormally.
**
** History:
**	09/85	- (rogerk) added RECOVER as a valid directory.
**      20-jul-87 (mmm)
**          Updated to meet jupiter standards.
**	9-dec-1996 (donc)
**	    Integrate OpenROAD change (chan 14-oct-1994) that adds support
**	    for an II_LOG location.
*/
STATUS
NMloc(which, what, fname, ploc1)
char		which;
LOCTYPE		what;
char		*fname;
LOCATION	*ploc1;
{
	STATUS		CLerror = OK;
	char		*symbol = NULL;
	char		*dname = NULL;
	bool		found	= FALSE;
	LOCATION	tmploc;
	char		dnamebuf[MAX_LOC];
	char		*buffer;
	STATUS		local_status;
 
	if ( !NM_static.Init )
	{
	    if( ( CLerror = NM_initsym() ) != OK )
	    {
		return CLerror;
	    }
	}

# ifdef OS_THREADS_USED
        if ( NMlockey == 0 )
        {
            ME_tls_createkey( &NMlockey, &local_status );
            ME_tls_set( NMlockey, NULL, &local_status );
        }
 	if ( NMlockey == 0 )
 	{
 	    /* not linked with threading library */
 	    NMlockey = -1;
 	}
 	if ( NMlockey == -1 )
 	{
 	    buffer = NM_static.Locbuf;
 	}
 	else
 	{
            ME_tls_get( NMlockey, (PTR *)&buffer, &local_status );
            if ( buffer == NULL )
            {
                buffer = MEreqmem(0, MAX_LOC+1, TRUE, NULL );
                ME_tls_set( NMlockey, (PTR)buffer, &local_status );
            }
	}
# else /* OS_THREADS_USED */
        buffer = NM_static.Locbuf;
# endif /* OS_THREADS_USED */

	switch (which)
	{
	case BIN:
		symbol = "II_BINARY";
		dname = "bin";
		break;

	case DBTMPLT:
		symbol = "II_TEMPLATE";
		dname = "dbtmplt";
		break;

	case DBDBTMPLT:
		symbol = "ING_DBDBTMP";
		dname = "dbdbtmplt";
		break;

	case UTILITY:
	case FILES:
		symbol = "II_CONFIG";
		dname = "files";
		break;

	case LIB:
		symbol = "II_LIBRARY";
		dname = "lib";
		break;

	case LOG:
		symbol = "II_LOG";
		dname = "files";
		break;

	case SUBDIR:
		if ( !(what & PATH) )
		    return NM_LOC;
		dname = fname;
		fname = NULL;
		break;

	case TEMP:
		symbol = "II_TEMPORARY";
		break;

	case ADMIN:
		break;

	default:
		return NM_LOC;
		break;
	}
 
	/*
	** Look at the user's local environment first.
	*/

	if( symbol )
	{
		char	*ptr;
 
		NMgtAt( symbol, &ptr );

		if( ptr && *ptr )
		{
			STcopy( ptr, buffer );
			LOfroms( PATH, buffer, ploc1 );
			found = TRUE;
		}
	}
		
	/*
	** If symbol not found in environment, use a default.
	*/

	if( !found )
	    switch( which )
	{
	case TEMP:
		/* Use current directory */
		if( ( CLerror = LOgt( buffer, ploc1 ) ) != OK )
		{
			return CLerror;
		}
		break;

	case ADMIN:
		/* Use ADMIN location found by NM_initsym() */
		STcopy( NM_static.Admbuf, buffer );
		LOfroms( PATH, buffer, ploc1 );
		break;

	default:
		/* Use directory $II_SYSTEM/ingres/dname */
		STcopy( NM_static.Sysbuf, buffer );
		LOfroms( PATH, buffer, ploc1 );
		STcopy( dname, dnamebuf );
		LOfroms( PATH, dnamebuf, &tmploc );
		LOaddpath( ploc1, &tmploc, ploc1 );
		break;
	}
		
	if( fname != NULL )
	{
		if( what == FILENAME )
		{
			LOfstfile( fname, ploc1 );
		}
		else if( what & PATH )
		{
			LOfroms( what, fname, &tmploc );
			LOaddpath( ploc1, &tmploc, ploc1 );
		}
	}
 
	return OK;
}


/*{
** Name: NMf_open - open a file in the files directory
**
** Description:
**    open 'fname' which is in the directory ~ingres/files with
**    'mode' of "r", "w" or "rw".  Set 'fptr' to the file
**    pointer of the opened file.
**
**    return OK on success.
**
** Inputs:
**	mode				    mode to open the file in.
**	fname				    name of file to open
**
** Output:
**	fptr				    On success FILE ptr to open file.
**                                          E_DM0000_OK                
**
**      Returns:
**          OK                         	    Function completed normally. 
**          FAIL                            Function completed abnormally.
**
** History:
**      20-jul-87 (mmm)
**          Updated to meet jupiter standards.
*/
STATUS
NMf_open(mode, fname, fptr)
char	*mode;
char	*fname;
FILE	**fptr;
{
	STATUS		rval;
	LOCATION	loc;
 
	STATUS		NMloc(),
			SIopen();
 
	rval = NMloc(FILES, FILENAME, fname, &loc);
	if ( rval == OK )
    {
	    rval = SIopen(&loc, mode, fptr);
        if (rval != OK)
        {
            NM_LOG_ERROR( "NMf_open", 0, fname, mode, errno );
        }
    }
	return rval;
}
 

/*{
** Name: NMt_open - Create and open temporary file.
**
** Description:
**    create and open with 'mode', a uniquly named file (prefix = 'fname'
**    and suffix = 'suffix') in the ingres temp directory.
**    set 'fptr' to be FILE * associated with 'fname' and fill in 't_loc'
**    so it may be used in subsequent call to LOdelete().
**    
**    As a static buffer is associated with 't_loc' it may
**    be advisable to do an LOcopy() upon return.
**
** Inputs:
**	mode				    mode to open the file in.
**	fname				    prefix of filename
**	suffix				    suffix of filename
**
** Output:
**	t_loc				    resulting location.
**	fptr				    resultion file pointer to open file.
**                                          E_DM0000_OK                
**
**      Returns:
**          OK                         	    Function completed normally. 
**          FAIL                            Function completed abnormally.
**
** History:
**      20-jul-87 (mmm)
**          Updated to meet jupiter standards.
*/
STATUS
NMt_open(mode, fname, suffix, t_loc, fptr)
char			*mode;
char			*fname;
char			*suffix;
LOCATION		*t_loc;
FILE			**fptr;
{
	LOCATION	tmploc;
	char		tmpbuf[ MAX_LOC + 1 ];
	STATUS		rval;
 
	/*
	This is using a side effect of LOuniq() to fill in tmpbuf.
	*/
 
	tmpbuf[ 0 ] = '\0';
	LOfroms(PATH & FILENAME, tmpbuf, &tmploc);
	LOuniq(fname, suffix, &tmploc);
 
	rval = NMloc(TEMP, FILENAME, tmpbuf, t_loc);
	if ( rval == OK )
		rval = LOcreate(t_loc);
	if ( rval == OK )
    {
		rval = SIopen(t_loc, mode, fptr);
        if (rval != OK)
        {
            char* filename = NULL;
            LOtos( &t_loc, &filename );
            if (filename != NULL)
                NM_LOG_ERROR( "NMt_open", 0, filename, mode, errno );
        }
    }
	return( rval );
}

/*{
** Name: NMopensyms - Open table of symbols.
**
** Description:
**    Open the symbol table file in the specified mode, either "r" or "w".
**
**    Returns either a file descriptor or NULL.
**
** Inputs:
**	mode				    mode to open the table in.
**
** Output:
**	none
**
**      Returns:
**	    File pointer to opened file on success.
**	    NULL on failure.
**
** History:
**      20-jul-87 (mmm)
**          Updated to meet jupiter standards.
**	14-Mar-90 (seiwald)
**	    Added new symbol II_ADMIN to point to directory of symbol
**	    table.  Default remains $II_SYSTEM/ingres/files.
**	    This is for (future) client installation support.
**	25-mar-90 (greg)
**	    PEsave/PEreset are both VOIDs don't check return status
**      29-sep-94 (cwaldman) Cross Integ 64 to 11 (ramra01) 7-Dec-94
**          Changed to return (FILE *)NULL if symbol.tbl can't be
**          opened (instead of opening a new file). This is part of
**          the fix for bug 44445 (symbol.tbl disappears).
**      27-feb-95 (cwaldman)
**          The above change caused another bug (#67049). Undid change.
**	 8-aug-95 (allst01)
**	    Fixed the symbol.tbl bug for su4_cmw
**	28-jan-2003 (somsa01)
**	    Extended above fix to Windows as well. Also, removed code not
**	    used on Windows.
*/

FILE *
NMopensyms( mode )
char *mode;
{
	FILE	*fp = NULL;
    
	/* Can't use NMloc() or could get into recursive infinite loop. */
 
	if ( !NM_static.Init )
	{
	    if( NM_initsym() != OK )
	    {
		return (FILE *)NULL;
	    }
	}

	/* Try to read once.  Don't keep trying if we failed already. */

	if( NM_static.ReadFailed && *mode == 'r' )
	{
		return (FILE *)NULL;
	}

	/*
	** Set fp to NULL if can't open existing file and
	**	  can't create new file and open it
	**
	** BUT only for write case ! For the read
	** case more checking must be done !
	**
	** If there is any error from the first SIopen
	** for the read case then the code zonks the
	** file, even if it is there. The errno must
	** be carefully checked. For CMW the caller
	** may be bounced if he is at a different
	** processs label, and in this case the code
	** deletes the existing file, since it can
	** write to the directory ! This leaves
	** a truncated "symbol.tbl".
	** Unfortunately there isn't a simple fix
	** since secure systems may return different
	** error values. A true B1 system may deny
	** the existance of the file even if it does
	** in fact exist. The only true way to be
	** certain is to look in the directory to see
	** if an entry exists. LOinfo and LOexists
	** cannot be used because they use stat() and
	** don't check in the directory. A file may
	** exist and yet be inaccessible on a secure
	** system. Using stat() will give an error
	** the same as open() in this case.
	**
	** I have applied a fix for CMW.
	** This is still broken generically because any error
	** from SIopen which isn't a "file does not exist"
	** error will cause the file to be zapped.
	** The fix may be generic for secure systems, but
	** this remains to be seen. It really needs to
	** be tighter to be airtight.
	**
	** The real generic fix must involve checking the
	** directory to see if it thinks that the file
	** exists. If it does then it may be assumed
	** that access to the file is blocked by
	** security. Unfortunately the CL does not
	** support this operation. Perhaps LOexists()
	** can be resurected and rewritten to look in
	** the directory so a generic fix for all systems
	** can be written ?
	*/

	if ( SIopen( &NMSymloc, mode, &fp ) != OK )
	{
        char* fname = NULL;
        LOtos( &NMSymloc, &fname );
        if (fname != NULL)
            NM_LOG_ERROR( "NMopensyms 1", 0, fname, mode, errno );
		if (*mode!='r' && (errno!=EACCES && errno!=EPERM))
		{
			PEsave();
			fp = NULL;

			if ( PEumask("rw-r--") == OK &&
			     LOcreate(&NMSymloc) == OK)
			{
			     PEreset();
			     if ( SIopen( &NMSymloc, mode, &fp ) != OK )
                 {
                    LOtos( &NMSymloc, &fname );
                    if (fname != NULL)
                        NM_LOG_ERROR( "NMopensyms 2", 0, fname, mode, errno );
				fp = NULL;
                 }
			}
	        }		 
		else
		{
			/* force a read error */
			fp=NULL;
            if (fname != NULL)
                NM_LOG_ERROR( "NMopensyms 3", 0, fname, mode, errno );
		}
	}

	NM_static.ReadFailed = (*mode == 'r' && fp == NULL) ? TRUE : FALSE;
	return (fp);
}

/*{
** Name: NMaddsym - Append a symbol to a chain.
**
** Description:
**    Append a symbol to a chain.
**
**    Returns status, failure meaning we couldn't allocate memory.
**
** Inputs:
**	name				    name of symbol.
**	value				    value of symbol.
**	endsp				    end of chain.
**
** Output:
**	none.
**
**      Returns:
**          OK                         	    Function completed normally. 
**          FAIL                            Function completed abnormally.
**
** History:
**      20-jul-87 (mmm)
**          Updated to meet jupiter standards.
*/
STATUS
NMaddsym( name, value, endsp )
register char 	*name;
register char	*value;
register SYM	*endsp;
{
	auto SYM	*sp;
	register i4	status = FAIL;
 
	/* allocate and save the new symbol, then link into chain */
 
        sp = (SYM *) MEreqmem(0, sizeof(*sp), FALSE, (STATUS *) NULL);
	if ((NULL != sp) &&
	     NULL != (sp->s_sym = STalloc(name) ) &&
	     NULL != (sp->s_val = STalloc(value) ) ) 
	{
		if( endsp != NULL )
			endsp->s_next = sp;
		else
		{
			s_list = sp;
		}

		sp->s_last = endsp;
		sp->s_next = NULL;
		status = OK;
	}
 
	return ( status );
}

/*{
** Name: NMreadsyms - read the symbol table.
**
** Description:
**    Read the symbol table into a linked list of SYMs.
**
**    This should only be called once.
**
** Inputs:
**	none.
**
** Output:
**	none.
**
**      Returns:
**          OK                         	    Function completed normally. 
**          FAIL                            Function completed abnormally.
**
** History:
**      20-jul-87 (mmm)
**          Updated to meet jupiter standards.
**      29-sep-94 (cwaldman) Cross Integ from 6.4 (ramra01) 7-Dec-94
**          Changed check of NMopensyms return value to compare against
**          (FILE *)NULL (was NULL).
**	06-apr-2004 (somsa01)
**	    If symbol.tbl is trashed, return NM_STCORRUPT.
*/
STATUS
NMreadsyms()
{
	register SYM	*sp = NULL;
	register FILE	*fp;
	register char	*bufr;
	register i4	status = OK;
 
	char	buf[ MAXLINE + NULL_NL_NULL ];
 
        if ((FILE *)NULL == (fp = NMopensyms( "r" )))
		return (NM_STOPN);
 
	/* Read each line in the file and add to the list of symbols. */
 
	while ( !status && !SIgetrec(buf, MAXLINE + NULL_NL, fp) )
	{
		if ( STlength(buf) != MAXLINE )
		{
			SIfprintf(stderr, "Trashed symbol table %s\n",
					NMSymloc.path);
			status = NM_STCORRUPT;
            NMflushIng();
            break;
		}
 
		/*
		** Tab separates symbol from value.
		**
		** Terminate the symbol name string, and
		** leave bufr at the first char of the value.
		*/
 
		for ( bufr = buf ; bufr != '\0'; bufr++)
		{
			if (*bufr == '\t')
			{
				*bufr++ = '\0';
				break;
			}
		}
 
		STtrmwhite( bufr );
		status = NMaddsym( buf, bufr, sp );
		if( sp != NULL )
			sp = sp->s_next;
		else
		{
			sp = s_list;
		}
	}
 
	/* Uninterested in errors closing file opened only for reading */
 
	(VOID) SIclose( fp );
 
	return (status);
}
 
 

 

/*{
** Name: NMgtIngAt - get a symbol from the symbol.tbl
**
** Description:
**    get a symbol from the symbol.tbl
**
** Inputs:
**	name				    name of symbol
**
** Output:
**	value				    returned value of symbol.
**
**      Returns:
**          OK                         	    Function completed normally. 
**          FAIL                            Function completed abnormally.
**
** History:
**      20-jul-87 (mmm)
**          Updated to meet jupiter standards.
**	15-jan-1996 (reijo01)
**	    Allow generic system parameter overrides on symbol table access.
*/
NMgtIngAt(name, value)
register char	*name;
register char	**value;
{
	register SYM	*sp;
	register i4	rval = OK;
	SYSTIME		thistime;
	char 		newname[MAXLINE];
	STATUS		CLerror = OK;
    register    i4  retry = NM_OPEN_RETRY;
	int			retrycount = 0;
    
	if ( MEcmp( name, "II", 2 ) == OK )
	    STpolycat( 2, SystemVarPrefix, name+2, newname );
	else
	    STcopy( name, newname );

	/* prevent user from referencing a NULL pointer */
 
	*value = NULL_STRING;
 
	/* make sure all globals have been initialized */
	if ( !NM_static.Init )
	{
	    if( ( CLerror = NM_initsym() ) != OK )
	    {
		return CLerror;
	    }
	}
	MUp_semaphore( &NM_static.Sem );

	/* Read the symbol table file once.  Error if can't read symbols. */
 
	if ( s_list == NULL )
	{
        do
        {
		    rval = NMreadsyms();
		    if(rval == OK && s_list != NULL)
            {
			    LOlast(&NMSymloc, &NMtime);
                break;
            }
            else
            {
                retry -= 1;
				PCsleep(min(NM_OPEN_WAIT_MAX, (NM_OPEN_WAIT + retrycount)));
				NM_static.ReadFailed = 0;
				retrycount++;
            }
        }
        while((((retry > 0) && (rval == NM_STCORRUPT)) ||
        	((retry > 0) && (s_list == NULL))) && (rval != NM_STOPN));
	}
	else
	{
		/* test if symbol tbale file was modified ? */

		LOlast(&NMSymloc, &thistime);
		if(MEcmp((PTR)&thistime, (PTR)&NMtime, sizeof(NMtime)) != 0)
		{
            NMflushIng();    
            do
            {
                rval = NMreadsyms();
                if(rval == OK && s_list != NULL)
                {
                    LOlast(&NMSymloc, &NMtime);
                    break;
                }
                else
                {
                    retry -= 1;
					PCsleep(min(NM_OPEN_WAIT_MAX, (NM_OPEN_WAIT + retrycount)));
					NM_static.ReadFailed = 0;
					retrycount++;
                }
            }
            while((((retry > 0) && (rval == NM_STCORRUPT)) ||
        	((retry > 0) && (s_list == NULL))) && (rval != NM_STOPN));
		}
	}
 
	if( OK == rval )
	{
		/*
		**  Search symbol table for the desired symbol.  If found,
		**    set value to that record and return.   If not found,
		**    value is still set to NULL_STRING.
		**
		**  Tedious linear search, but the list isn't very big.
		*/
	
		for ( sp = s_list; sp != NULL; sp = sp->s_next )
		{
			/* Check one char, then call slow function */
	
			if (*sp->s_sym == *newname && !STcompare(sp->s_sym,newname))
				break;
		}
	
		if ( sp != NULL )
			*value = sp->s_val;
	}
	MUv_semaphore( &NM_static.Sem );

	return( rval );
}


/*{
** Name: NMflushIng - flush the internal cache
**
** Description:
**
** Inputs:
**
** Output:
**
**      Returns:
**
** History:
**      10-Mar-89 (GordonW)
*/
STATUS
NMflushIng()
{
	register SYM	*sp, *lsp;

	/* first determine if any cache */
	sp = s_list;
	if(sp == NULL)
		return(OK);

	/* find end of chain  */
	while (sp->s_next != NULL)
		sp = sp->s_next;

	/* now free space */
	while(sp != NULL)
	{
		lsp = sp->s_last;
        if (sp->s_sym)
        {
            MEfree( sp->s_sym );
        }
        if (sp->s_val)
        {
            MEfree( sp->s_val );
        }
		MEfree(sp);
		sp = lsp;
	}
	s_list = NULL;

	return(OK);
}

/*{
**  Name: NMlogOperation - log ingsetenv / ingunset operations.
**
**  Description:
**	This function logs update operations on the symbol.tbl file to
**	a symbol.log file.
**
**  Inputs:
**	oper			name of operation to log
**	name			name of symbol being added/updated/deleted
**	value			value of symbol, if added/updated
**	new_value		new value of symbol, if updated
**
**  Output:
**	none
**
**      Returns:
**	    none
**
**  History:
**	07-jan-2004 (somsa01)
**	    Created.
*/
VOID
NMlogOperation(char *oper, char *name, char *value, char *new_value, STATUS status)
{
    FILE	*fptr;
    char	host[GCC_L_NODE + 1];
    char	*username;
    SYSTIME	timestamp;
    char	buf[100], errbuf[ER_MAX_LEN];

    if ((status == OK || status == NM_STCORRUPT) &&
	SIopen(&NMLogSymloc, "a", &fptr) == OK)
    {
	SIfprintf(fptr, "*********************\n");
	SIfprintf(fptr, "*** BEGIN SESSION ***\n");
	SIfprintf(fptr, "*********************\n\n");

	/* Get host name */
	GChostname(host, sizeof(host));
	SIfprintf(fptr, "%-15s%s\n", "host:", host);

	/* Get user name */
	username = (char *)MEreqmem(0, 100, FALSE, NULL);
	IDname(&username);
	SIfprintf(fptr, "%-15s%s\n", "user:", username);
	MEfree(username);

	/* Get timestamp */
	TMnow(&timestamp);
	TMstr(&timestamp, buf);
	SIfprintf(fptr, "%-15s%s\n", "time:", buf);

	if (status == NM_STCORRUPT)
	{
	    ERreport(status, errbuf);
	    SIfprintf(fptr, "\n%s\n\n", errbuf);
	}
	else
	{
	    if (!STbcompare(oper, 0, "DELETE", 0, TRUE))
		SIfprintf(fptr, "\nDELETE %s\n\n", name);
	    else if (!STbcompare(oper, 0, "ADD", 0, TRUE))
	    {
		if (STstrindex(name, "II_GCN", 6, TRUE) &&
		    STstrindex(name, "_PORT", 0, TRUE))
		{
		    /* As a special case, do not log II_GCNxx_PORT value */
		    SIfprintf(fptr, "\nADD %s\n\n", name);
		}
		else
		    SIfprintf(fptr, "\nADD %s ... (%s)\n\n", name, value);
	    }
	    else if (!STbcompare(oper, 0, "CHANGE", 0, TRUE))
	    {
		if (STstrindex(name, "II_GCN", 6, TRUE) &&
		    STstrindex(name, "_PORT", 0, TRUE))
		{
		    /* As a special case, do not log II_GCNxx_PORT value */
		    SIfprintf(fptr, "\nCHANGE %s\n\n", name);
		}
		else
		{
		    SIfprintf(fptr, "\nCHANGE %s: (%s)...(%s)\n\n", name,
			      value, new_value);
		}
	    }
	}

	SIclose(fptr);
    }
}
