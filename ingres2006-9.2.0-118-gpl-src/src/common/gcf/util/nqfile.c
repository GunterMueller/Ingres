/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>

#include    <gc.h>
#include    <lo.h>
#include    <me.h>
#include    <nm.h>
#include    <pc.h>
#include    <qu.h>
#include    <si.h>
#include    <st.h>

#include    <iicommon.h>
#include    <gca.h>
#include    <gcn.h>
#include    <gcnint.h>

/*
** Name: nqfile.c
**
** Description:
**	Display information on Name Server Name Queue disk file.
**
**	usage: nqfile [-v] <queue_name>
**
**	    -v		Verbose mode (includes individual record info).
**	    <queue>	Name of Name Server Queue (INGRES, NODE, etc).
**
**
** History:
**	16-Jun-98 (gordy)
**	    Created.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	29-Sep-2004 (drivi01)
**	    Removed MALLOCLIB from NEEDLIBS
*/

/*
** MING Hints:
**
NEEDLIBS =	GCFLIB COMPATLIB 
OWNER =		INGUSER
PROGRAM =	nqfile
*/

/*
** Forward and/or External function references.
*/

static	VOID	usage( char *name );
static	VOID	analyze( char *name );
static	STATUS	gcn_read_rec( FILE *file, GCN_DB_RECORD *record );
static	VOID	gcn_nq_filename( char *type, char *host, char *filename );
static	STATUS	gcn_nq_fopen( char *name, char *mode, FILE **file, i4 reclen );

static	bool	verbose = FALSE;


/*
** Name: main
**
** Description:
**	Initialize and process command line.
**
** History:
**	16-Jun-98 (gordy)
**	    Created.
*/

i4
main( i4  argc, char **argv )
{
    i4  arg = 1;

    MEadvise(ME_INGRES_ALLOC);
    SIeqinit();

    if ( argc > 1  &&  argv[1][0] == '-' )
    {
	arg++;

	switch( argv[1][1] )
	{
	    case 'v' : 
		verbose = TRUE;
		break;

	    default  :
		usage( argc >= 1 ? argv[0] : NULL );
		PCexit(FAIL);
	}
    }

    if ( arg >= argc )
    {
	usage( argc >= 1 ? argv[0] : NULL );
	PCexit(FAIL);
    }
    else
    {
	analyze( argv[ arg ] );
	PCexit(OK);
    }

    return(OK);
}


/*
** Name: usage
**
** Description:
**	Display command line usage info.
**
** Input:
**	name		Name used to execute this utility.
**
** Output:
**	None.
**
** Returns:
**	VOID
**
** History:
**	16-Jun-98 (gordy)
**	    Created.
*/

static VOID
usage( char *name )
{
    SIprintf( "usage: %s [-v] <name>\n", (name && *name) ? name : "nqfile" );
    return;
}


/*
** Name: analyze
**
** Description:
**	Opens disk file associated with a Name Queue.  Reads records
**	in disk file, compiles and displays statistics.
**
** Input:
**	name		Name of Queue.
**
** Output:
**	None.
**
** Returns:
**	VOID
**
** History:
**	16-Jun-98 (gordy)
**	    Created.
*/

static VOID 
analyze( char *name )
{
    GCN_DB_RECORD	record;
    STATUS		status;
    FILE		*file;
    char		host[50];
    char		filename[MAX_LOC];
    i4			records = 0;
    i4			deleted = 0;

    GChostname( host, sizeof( host ) );
    gcn_nq_filename( name, host, filename );
    status = gcn_nq_fopen( filename, "r", &file, sizeof( record ) );

    if ( status != OK )
    {
        SIprintf( "could not open %s: 0x%x\n", filename, status );
	return;
    }

    while( gcn_read_rec( file, &record ) == OK  &&  record.gcn_tup_id != -1 )
    {
	records++;

	if ( record.gcn_invalid )  
	{
	    deleted++;
	    if ( verbose )  SIprintf( "record %d: deleted\n", records - 1 );
	}
	else  if ( verbose )
	{
	    if ( record.gcn_l_uid < GCN_UID_MAX_LEN )
		record.gcn_uid[ record.gcn_l_uid ] = '\0';
	    if ( record.gcn_l_obj < GCN_OBJ_MAX_LEN )
		record.gcn_obj[ record.gcn_l_obj ] = '\0';
	    if ( record.gcn_l_val < GCN_VAL_MAX_LEN )
		record.gcn_val[ record.gcn_l_val ] = '\0';

	    SIprintf( "record %d: '%s', '%s', '%s'\n", records - 1, 
		      record.gcn_uid, record.gcn_obj, record.gcn_val );
	}
    }

    SIclose( file );

    SIprintf( "\nSummary of file %s:\n", filename );
    SIprintf( "Records: %d\n", records );
    SIprintf( "Active : %d\n", records - deleted );
    SIprintf( "Deleted: %d\n", deleted );
    if ( record.gcn_tup_id == -1 )  SIprintf( "LEOF at: %d\n", records );
    SIprintf( "\n" );

    return;
}

/*
** Name: gcn_read_rec
**
** Description:
**	Read a record from a Name Queue disk file.
**
** Input:
**	file		Open file handle.
**
** Output:
**	record		Contents of next record in file.
**
** Returns:
**	STATUS		OK or error code.
**
** History:
**	16-Jun-98 (gordy)
**	    Created.
*/

static STATUS
gcn_read_rec( FILE *file, GCN_DB_RECORD *record )
{
    STATUS      status = OK;
    char        *rec_ptr = (char *)record;
    i4     bytes_read = 0;
    i4		rec_len = sizeof( *record );

    while( rec_len )
    {
	status = SIread( file, rec_len, &bytes_read, rec_ptr );
	if ( status != OK )  break;

	rec_ptr += bytes_read;
	rec_len -= bytes_read;
    }

    return( status );
}


/*
** Name: gcn_nq_filename
**
** Description:
**	Form a disk filename for a name queue.  The host name should
**	be provided for non-global queues.
**
** Input:
**	type		Name of queue.
**	host		Name of host (NULL if global queue).
**
** Output:
**	filename	Disk filename.
**
** Returns:
**	VOID
**
** History:
**	26-Feb-96 (gordy)
**	    Created.
*/

static VOID
gcn_nq_filename( char *type, char *host, char *filename )
{
    i4		len, plen, slen;

    /*
    ** Filename template: II<type>[_<host>]
    */
    plen = 2;
    len = STlength( type );
    slen = (host && *host) ? STlength( host ) + 1 : 0;

    /*
    ** Adjust filename for platform limitations:
    **   drop prefix if type and suffix OK; 
    **   drop suffix if prefix and type OK;
    **   otherwise, drop both prefix and suffix.
    */
    if ( plen + len + slen > LO_NM_LEN )
	if ( len + slen <= LO_NM_LEN )
	    plen = 0;
	else  if ( plen + len <= LO_NM_LEN )
	    slen = 0;
	else
	    plen = slen = 0;

    STprintf( filename, "%s%s%s%s", 
	      plen ? "II" : "", type, slen ? "_" : "", slen ? host : "" );

    /*
    ** Finally, truncate the filename if it is too long
    ** (hopefully this will never happen).
    */
    if ( STlength( filename ) > LO_NM_LEN )
	filename[ LO_NM_LEN ] = '\0';
    
    return;
}


/*
** Name: gcn_nq_fopen 	- Open a named file.
**
** Description:
**	This routine opens a named file which contains the type of database 
**	identified by its name. The file descriptor is returned.
**
** Inputs:
**	name	- The file name.
**	mode  - read/write mode.
**	reclen - record length for those with fixed record lengths
**
** Outputs:
**	file
**
** Returns:
**	OK  - succeed	
**	status - Error code
**
** Exceptions:
**	    none
**
** Side Effects:
**	    The file is leaved opened until gcn_closefile is called.
**
** History:
**
**      23-Mar-88 (Lin)
**          Initial function creation.
**	01-Mar-89 (seiwald)
**	    Revamped.  Added reclen.
**	29-Jun-92 (fredb)
**	    Integrate bryanp's 1989 changes for MPE (hp9_mpe):
**      	17-Sep-89 (bryanp)
**          		Fixed a bug in the hp9_mpe filename generation code 
**			introduced during 61b3ug integration.
**      	18-Sep-89 (bryanp)
**          		Fixed another bug in the hp9_mpe filename generation 
**			code which was introduced during 61b3ug integration.
**
*/

static STATUS 
gcn_nq_fopen( char *name, char *mode, FILE **file, i4  reclen )
{
    LOCATION	loc;
    STATUS		status;

#ifndef hp9_mpe
    if ( ( status = NMloc( FILES, PATH, (char *)NULL, &loc ) ) != OK )
	goto error;

    LOfaddpath( &loc, "name", &loc );
    LOfstfile( name, &loc );
#else
    if ( (status = NMloc( FILES, FILENAME, name, &loc )) != OK )
	goto error;
#endif

    if ( ! STcompare( mode, "r" )  &&  (status = LOexist( &loc )) != OK )
	goto error;
		
    if ( reclen )
	status = SIfopen( &loc, mode, SI_RACC, reclen, file );
    else
#ifndef hp9_mpe
	status = SIopen( &loc, mode, file );
#else
	status = SIfopen( &loc, mode, SI_TXT, 256, file );
#endif

    if ( status == OK )  return( OK );

error:

    *file = NULL;

    return( status );
}

