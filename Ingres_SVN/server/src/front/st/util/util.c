/*
**  Copyright (c) 2004 Ingres Corporation
**
**  Name: util.c -- contains useful functions for testing types and
**	values generated by the configuration rules system.
**
**  Description:
**	The functions in this file can be used to test whether values
**	are integers or booleans.
**
**  History:
**	14-dec-92 (tyler)
**		Created.
**	16-jul-93 (tyler)
**		Added PMerror() for reporting PMload() errors.
**	23-jul-93 (tyler)
**		Added write_transaction_log().
**	26-jul-93 (tyler)
**		Added lock_config_data() and unlock_config_data(). 
**      30-aug-93 (huffman)
**              Add <me.h>.
**	19-oct-93 (tyler)
**		Call PMhost() instead of GChostname().
**	28-oct-93 (fredv)
**		In write_transaction_log(), locbuf was declared as
**		char *locbuf[257]. It should be char locbuf[257].
**	22-nov-93 (tyler)
**		Switched (back) to using GChostname() for host name
**		written to config.lck
**	05-jan-94 (tyler)
**		Optimized write_transaction_log() to perform better 
**		on large files (thanks to rog).  Make sure file size
**		can be determined before the transaction log is opened.
**		Fixed BUG which caused "ii.*.rcp.file.size not found."
**		to be displayed when trying to create/erase logs under
**		some circumstances.
**	12-jan-94 (tyler)	
**		Set LG_PAGE_INCREMENT to 32L since that translates 
**		to the maximum number of bytes allowed by most Unix
**		write() implementations. (32 * 2K pages = 64K)
**	30-jun-95 (whitfield)
**		Initialize scb.  Even though the scb doesn't seem to be used,
**              it is required for the following reason:  DIcreate and
**              DIopen on UNIX eventually call DIlru_init which eventually
**              calls IICSp_semaphore.  At this point, CS_srv_block.cs_current
**              must be non-NULL or IICSp_semaphore thinks the current thread
**              already hols semaphore and generates I_CS0017_SMPR_DEADLOCK.
**              On VMS, CS_srv_block.cs_current, if non-NULL, is checked for
**              a UIC.  That UIC is ignored if 0.
**	13-oct-95 (nick)
**	    Alter some error message text in write_transaction_log() to ease
**	    diagnosis of point of failure.
**	12-mar-96 (hanch04)
**	    Solaris 2.5 returns 0x7FFFFFFF for size in the stat call in LOinfo
**	    for a raw partition.  2.4 and earlier returned 0.  We must
**	    check for both.
**	22-mar-96 (nick)
**	    Removed above change by hanch04 ; fixed this in LO instead.
**	02-mar-96 (morayf)
**		The message, init_graph, update_graph function pointer
**		declarations were not strictly declared properly.
**		I added the brackets and preceding '*' character.	
**	03-nov-1997 (canor01)
**	    Use kbytes instead of bytes for file sizes so we can support
**	    files larger than 2BG.
**	06-nov-1997 (canor01)
**	    Use ii.*.rcp.file.kbytes.
**	27-Jan-1998 (jenjo02)
**	    Partitioned log file project:
**	    Integrated balst01's changes for logfile allocation.
**      12-Aug-98 (fanra01)
**          Modified recomposition of path and filename to use LO.
**	24-Aug-1998 (hanch04)
**	    Use log locations and filename for partitioned logs.
**	07-mar-2000 (somsa01)
**	    Added putting the PID inside the config.lck . Also, if the
**	    config.lck file exists and the PID does not exist anymore,
**	    then successfully lock the file.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	23-jul-2001 (devjo01)
**	    DO NOT "decorate" of log names used when clustering is
**	    enabled.  File names passed here have already been
**	    decorated.
**	05-Oct-2002 (devjo01)
**	    Simplified 'write_transaction_log', by requiring that a
**	    PM_CONTEXT be passed in, and that defaults for system,
**	    and host be set.  Do not reset defaults.  Much cleaner,
**	    and helped with NUMA project.
**	15-March-2004 (gorvi01) SRS section 3.1.30
**	    Added ValueIsFloat(). The function checks and passes TRUE
**	    if the string passed is float and returns FAIL otherwise.
**	15-March-2004 (gorvi01) SRS section 3.1.30
**	    Added ValueIsSignedInt(). The function checks and passes TRUE
**	    if the string passed is signed integer and returns FAIL otherwise.
**	9-Aug-2004 (bonro01)
**	    Split message output into two calls because TRdisplay() splits
**	    messages over 131 characters.
**	2-Sep-2004 (schka24)
**	    Tx log (total) size can exceed an i4, use i8.
**	    Correct "not found" messages to say kbytes, not size, since
**	    file.kbytes is what we really want.  (size is the old resource.)
**      15-Apr-2005 (hweho01)
**          Fixed the error in ValueIsSignedInt().
**          1) Removed the test of uninitialized file pointer.
**          2) Changed boolean return code from FAIL to FALSE in the case of
**             error, since FAIL is defined as 1 which is the same as TRUE. 
*/

# include <compat.h>
# include <gl.h>
# include <iicommon.h>
# include <me.h>
# include <st.h>
# include <cm.h>
# include <di.h>
# include <er.h>
# include <gcccl.h>
# include <lo.h>
# include <te.h>
# include <tm.h>
# include <cs.h>
# include <cv.h>
# include <nm.h>
# include <pc.h>
# include <si.h>
# include <lg.h>
# include <simacro.h>
# include <pm.h>
# include <util.h>
# include <cr.h>

static LOCATION	mutex_loc;
static char	mutex_locbuf[ MAX_LOC+ 1 ];
static char	*mutex_file;


/*
** Tries to lock config.dat by creating config.lck.
*/
STATUS
lock_config_data( char *application )
{
	PM_CONTEXT	*context, *context1;
	char		*username;
	char		buf[ 100 ]; 
	char		host[ GCC_L_NODE + 1 ];
	SYSTIME		timestamp;
	PID		pid;

	/* insert lock data into PM context */

	(void) PMmInit( &context );

	PMmLowerOn( context );

	if( application != NULL )
		PMmInsert( context, ERx( "application" ), application );

	/* get user name */
	username = (char *) MEreqmem( 0, 100, FALSE, NULL );
	IDname( &username );
	PMmInsert( context, ERx( "user" ), username );
	MEfree( username );

	/* get terminal id */
	if( TEname( buf ) == OK )
		PMmInsert( context, ERx( "terminal" ), buf );

	/* get host name */
	GChostname( host, sizeof( host ) );
	PMmInsert( context, ERx( "host" ), host );

	/* get timestamp */
	TMnow( &timestamp );
	TMstr( &timestamp, buf );
	PMmInsert( context, ERx( "timestamp" ), buf );

	/* get process id */
	PCpid(&pid);
	CVla(pid, buf);
	PMmInsert( context, ERx( "pid" ), buf );

# ifdef VMS
# define MUTEX_FILE	ERx( "config.lck;1" )
# else /* VMS */
# define MUTEX_FILE	ERx( "config.lck" )
# endif /* VMS */

	/* prepare mutex file LOCATION */
	NMloc( FILES, FILENAME, MUTEX_FILE, &mutex_loc );
	LOcopy( &mutex_loc, mutex_locbuf, &mutex_loc );
	LOtos( &mutex_loc, &mutex_file );

	/*
        ** if config.lck can be opened for reading, assume that another
	** invocation of cbf has config.dat "locked".
	*/
	PMmInit( &context1 );
	PMmLowerOn( context1 );
	if( PMmLoad( context1, &mutex_loc, NULL ) == OK )
	{
		char *value;

		/* See if the PID that had this file locked still exists */
		if( PMmGet( context1, ERx( "pid" ), &value ) == OK )
		{
		    CVal(value, &pid);
		    if (!PCis_alive(pid))
			goto procdead;
		}

		PRINT( "\nThe configuration file is locked by another application (config.lck exists).\n\n" );
		if( PMmGet( context1, ERx( "application" ), &value ) == OK )
		{
			SIprintf( "\t%-15s%s\n", ERx( "application:" ),
				value );
		}
		if( PMmGet( context1, ERx( "host" ), &value ) == OK )
			SIprintf( ERx( "\t%-15s%s\n" ), "host:", value );
		if( PMmGet( context1, ERx( "user" ), &value ) == OK )
			SIprintf( ERx( "\t%-15s%s\n" ), "user:", value );
		if( PMmGet( context1, ERx( "terminal" ), &value ) == OK )
			SIprintf( ERx( "\t%-15s%s\n" ), "terminal:", value );
		if( PMmGet( context1, ERx( "timestamp" ), &value ) == OK )
			SIprintf( ERx( "\t%-15s%s\n" ), "time:", value );
		if( PMmGet( context1, ERx( "pid" ), &value ) == OK )
			SIprintf( ERx( "\t%-15s%s\n" ), "pid:", value );
		PRINT( ERx( "\n" ) );
		PMmFree( context );
		PMmFree( context1 );
		return( FAIL );
	}

procdead:
	/*
	** This locking scheme has a small chance of failing right here 
	** if a second caller tries to open config.lck for reading at
	** this instant. 
	*/

	/* lock config.dat by writing config.lck */
	if( PMmWrite( context, &mutex_loc ) != OK )
		F_ERROR( "Unable to write %s", mutex_file );

	PMmFree( context );
	PMmFree( context1 );
	
	return( OK );
}


/*
** Tries to lock config.dat by creating config.lck.
*/
void
unlock_config_data( void )
{
	LOdelete( &mutex_loc );
}


/*
** Writes or erases a transaction log and displays a completion thermometer.
**
** 'context' must not be NULL, and defaults for element 0 & 1 of context
** must have been set.
*/
i4
write_transaction_log( bool create, PM_CONTEXT *context,
	char *log_dir[], char *log_file[],
	void (*message)( char * ), void (*init_graph)( bool, i4 ), 
	i4 graph_size, void (*update_graph)() )
{

# define LG_PAGE_SIZE		2048L
# define LG_NUMBER_OF_PAGES	32L	/* Number of pages to write at a time */

    DI_IO 		dio[LG_MAX_FILE];
    char 		buf[LG_PAGE_SIZE * LG_NUMBER_OF_PAGES];
    char		nodename[GL_MAXNAME];
    i4  		i, j, marker, count, num_logs;
    i4 		num_pages, page, part_size, remainder, loop;
    CL_ERR_DESC 	err;
    char		*string;
    LOCATION 		loc[LG_MAX_FILE];
    char 		locbuf[MAX_LOC + 1];
    char 		*path[LG_MAX_FILE];
    i4  		LOinfo_flag;
    LOINFORMATION 	loc_info;
    i8 		size;
    CS_SCB 		scb;
    bool		size_in_kbytes = FALSE;

    MEfill( sizeof( scb ), 0, ( PTR ) &scb ); 
    
    ++graph_size;

    /* Prepare transaction for log LOCATION(s) */
    for (num_logs = 0; num_logs < LG_MAX_FILE; num_logs++)
    {
	if (log_dir[num_logs] == 0)
	    break;
	STcopy( log_dir[num_logs], locbuf );
	LOfroms( PATH, locbuf, &loc[num_logs] );
	LOfstfile( log_file[num_logs], &loc[num_logs] );
	LOtos( &loc[num_logs], &path[num_logs] );
    }

    /* get size (in bytes) of log file to be created or erased */
    if( create )
    {
	STATUS status;
	char *value;
	bool havevalue = FALSE;

	status = PMmGet( context, ERx( "$.$.rcp.file.kbytes" ), &value );
	if ( status == OK )
	{
	    havevalue = TRUE;
	    size_in_kbytes = TRUE;
	}

	if ( havevalue == FALSE )
	{
	    status = PMmGet( context, ERx( "$.$.rcp.file.size" ), &value );
	    if ( status == OK )
		havevalue = TRUE;
	}

	if ( havevalue == FALSE )
	{
	    if ( message != NULL )
	    {
		char msg[ BIG_ENOUGH ];
		STprintf( msg, ERx( "%s %s" ),
			PMmExpandRequest( context,
			ERx( "$.$.rcp.file.kbytes" ) ),
			"not found." );
		message( msg );
	    }
	    return( 0 );	
	}
	CVal8( value, &size );
    }
    else
    {
	LOinfo_flag = LO_I_SIZE;
	size = 0;

	for (i = 0; i < num_logs; i++)
	{
	    if ( LOinfo( &loc[i], &LOinfo_flag, &loc_info ) != OK )
	    {
		if ( message != NULL )
		{
		    char msg[ BIG_ENOUGH ];

		    STprintf( msg,
			    "Unable to get size of transaction log:\n\n\t%s",
			    path[i] );	
		    (*message)( msg );
		}
		return( 0 );	
	    }
	    else
		size += loc_info.li_size;
	    if ((LOinfo_flag & LO_I_SIZE) == 0)
		break;
	}

	if ( (LOinfo_flag & LO_I_SIZE) == 0 || size == 0L)
	{
	    STATUS status;
	    char *value;
	    bool havevalue = FALSE;

	    status = PMmGet( context, ERx( "$.$.rcp.file.kbytes" ), &value );
	    if ( status == OK )
	    {
		havevalue = TRUE;
		size_in_kbytes = TRUE;
	    }

	    if ( havevalue == FALSE )
	    {
		status = PMmGet( context, ERx( "$.$.rcp.file.size" ), &value );
		if ( status == OK )
		    havevalue = TRUE;
	    }

	    if ( havevalue == FALSE  )
	    {
		if ( message != NULL )
		{
		    char msg[ BIG_ENOUGH ];

		    STprintf( msg, ERx( "%s %s." ),
			    PMmExpandRequest( context,
			    ERx( "$.$.rcp.file.kbytes" ) ),
			    "not found." );
		    message( msg );
		}
		return( 0 );	
	    }
	    CVal8( value, &size );
	}
    }

    for (i = 0; i < num_logs; i++)
    {
	if ( create && LOexist( &loc[i] ) == OK )
	{
		char msg[ BIG_ENOUGH ];

		if ( message != NULL )
		{
			STprintf( msg, "%s already exists.", path[i] ); 
			(*message)( msg ); 
			STprintf( msg, "To create a new transaction log, you must first delete all partitions of the old one." ); 
			(*message)( msg ); 
		}
		return( 0 );	
	}
    }

    if ( CSinitiate( (i4 *) NULL, (char ***) NULL, (CS_CB *) NULL )
	    != OK )
    {
	    if ( message != NULL )
		    message( "Unable to connect to shared memory" );
	    return( 0 );	
    }

    CSset_sid( &scb );

    /* create DI file */
    for (i = 0; i < num_logs; i++)
    {
	if ( create && DIcreate( &dio[i], 
		log_dir[i],  (u_i4)STlength(log_dir[i]), 
		log_file[i], (u_i4)STlength(log_file[i]),
		(i4)LG_PAGE_SIZE, &err) != OK )
	{
	    char msg[ BIG_ENOUGH ];

	    if ( message != NULL )
	    {
		STprintf( msg,
			"Unable to create transaction log:\n\n\t%s",
			path[i] );	
		(*message)( msg );
	    }
	    return( 0 );
	}

	/* open DI file */
	if ( DIopen( &dio[i],
		log_dir[i],  (u_i4)STlength(log_dir[i]), 
		log_file[i], (u_i4)STlength(log_file[i]),
		(i4)LG_PAGE_SIZE, DI_IO_WRITE, 0, &err) != OK )
	{
	    if ( message != NULL )
	    {
		char msg[ BIG_ENOUGH ];

		STprintf( msg,
			"Unable to open transaction log:\n\n\t%s",
			path[i] );	
		(*message)( msg );
	    }
	    return( 0 );
	}
    }

    if ( size_in_kbytes == FALSE )
	size = (size + 1023) / 1024;

    (*init_graph)( create, (i4) size );

    num_pages = size / (LG_PAGE_SIZE/1024);

    part_size = num_pages / num_logs;

    /* Readjust num_pages to be a multiple of num_logs */
    num_pages = part_size * num_logs;

    loop = part_size / LG_NUMBER_OF_PAGES;
    remainder = part_size % LG_NUMBER_OF_PAGES;

    marker = loop / graph_size;

    /* Fill buffer with zeroes */
    MEfill( sizeof( buf ), 0 , buf);

    for (i = 0; i < num_logs; i++)
    {
	if ( create &&
		DIalloc( &dio[i], part_size, &page, &err ) != OK )
	{
	    DIdelete( &dio[i], log_dir[i], (u_i4)STlength( log_dir[i] ),
		    log_file[i], (u_i4)STlength( log_file[i] ), &err );
	    if ( message != NULL )
		    (*message)( "Unable to allocate space in transaction log file." );
	    return( 0 );
	}
    }

    count = 0;
    for( j = 0; j < loop; j++ )
    {
	i4	n = LG_NUMBER_OF_PAGES;
	i4	page_no = j * n;

	for (i = 0; i < num_logs; i++)
	{
	    if ( DIwrite( &dio[i], &n, page_no, 
			buf, &err ) != OK )
	    {
		DIdelete( &dio[i], 
		    log_dir[i], (u_i4)STlength(log_dir[i]),
		    log_file[i], (u_i4)STlength(log_file[i]),
		    &err );
		if ( message != NULL )
			(*message)( "Unable to continue writing transaction log." );
		return( 0 );
	    }
	}

	if ( j >= marker && j % marker == 0 &&
		(f4) ((f4) j / (f4) loop) >= ((f4) count + 1) /
		graph_size )
	{
	    ++count;
	    (*update_graph)();
	    SIflush( stdout );
	}
    }

    if (remainder)
    {
	i4	page_no = loop * LG_NUMBER_OF_PAGES;

	for (i = 0; i < num_logs; i++)
	{
	    if ( DIwrite( &dio[i], &remainder, page_no,
			buf, &err ) != OK )
	    {
		DIdelete( &dio[i], 
		    log_dir[i], (u_i4)STlength(log_dir[i]),
		    log_file[i], (u_i4)STlength(log_file[i]),
		    &err );
		if ( message != NULL )
			(*message)( "Unable to continue writing transaction log." );
		return( 0 );
	    }
	}
    }

    for (i = 0; i < num_logs; i++)
    {
	if ( DIforce( &dio[i], &err ) != OK )
	{
	    DIdelete( &dio[i], log_dir[i], (u_i4)STlength(log_dir[i]),
		    log_file[i], (u_i4)STlength(log_file[i]), &err );
	    if ( message != NULL )
		(*message)( "Unable to force changes to transaction log." );
	    return( 0 );
	}

	if ( create && DIflush( &dio[i], &err ) != OK )
	{
	    DIdelete( &dio[i], log_dir[i], (u_i4)STlength(log_dir[i]),
		    log_file[i], (u_i4)STlength(log_file[i]), &err );
	    if ( message != NULL )
		(*message)( "Unable to flush transaction log to disk." );
	    return( 0 );
	}

	if( DIclose( &dio[i], &err ) != OK)
	{
	    DIdelete( &dio[i], log_dir[i], (u_i4)STlength(log_dir[i]),
		    log_file[i], (u_i4)STlength(log_file[i]), &err );
	    if ( message != NULL )
		(*message)( "Unable to finish writing transaction log." );
	    return( 0 );
	}
    }

    return( (i4) size );
}


/*
** Returns TRUE if a string is a valid integer.
*/
bool
ValueIsInteger( s )
char *s;
{
	i4 len;
	char *c;

	if( s == NULL )
		return( FALSE );
	len = STlength( s );
	if( CMcmpcase( s, ERx( "-" ) ) == 0 )
		s = CMnext( s ); 
	for( c = s; c < s + len; c = CMnext( c ) ) 
	{
		if( ! CMdigit( c ) )
			return( FALSE );
	}
	return( TRUE );
}

/*
** Returns TRUE if a string is a signed integer
*/
bool
ValueIsSignedInt ( char *s )
{
	i4 number;
	
	if (s == NULL || *s == EOS )
		return( FALSE );
        if ( CVal (s, &number ) != OK)
	{
		return( FALSE );
        }
	if (number < 0)
	{
		return( TRUE );
	}
	else
	{
		return( FALSE );
	}
}
	
/*
** Returns TRUE if a string is a float value
*/
bool
ValueIsFloat (char *s)
{

	i4 len;
	char *ctemp;
	f8 *ftemp;

	if (s == NULL )
		return( FALSE );
	len = STlength( s );
	for (ctemp=s; ctemp < s + len; ctemp = CMnext( ctemp ) )
	{
		if (! CVaf( ctemp, '.',ftemp) )
			return( FALSE );
	}
	return( TRUE );
}
		

/*
** Returns TRUE if the string is a boolean value.
*/
bool
ValueIsBool( s )
char *s;
{
	if( s == NULL )
		return( FALSE );
	if( STbcompare( s, 0, ERx( "on" ), 0, TRUE ) == 0 ||
		STbcompare( s, 0, ERx( "off" ), 0, TRUE ) == 0
	)
		return( TRUE );
	return( FALSE );
}


/*
** Returns TRUE if the string is boolean and TRUE. 
*/
bool
ValueIsBoolTrue( s )
char *s;
{
	if( s == NULL )
		return( FALSE );
	if( STbcompare( s, 0, ERx( "on" ), 0, TRUE ) == 0 )
		return( TRUE );
	return( FALSE );
}


/*
** Function passed to PMload() and PMmLoad() to display error messages.
*/
void
PMerror( STATUS status, i4  nargs, ER_ARGUMENT *args )
{
	i4 language;
	char buf[ BIG_ENOUGH ];
	i4 len;
	CL_ERR_DESC cl_err;

	if( ERlangcode( ERx( "english" ), &language ) != OK )
		return;
	if( ERlookup( status, NULL, ER_TEXTONLY, NULL, buf, sizeof( buf ),
		language, &len, &cl_err, nargs, args ) == OK
	)
		F_FPRINT( stderr, "\n%s\n\n", buf );
}

