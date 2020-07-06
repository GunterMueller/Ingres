/*
** Copyright 2006 Ingres Corporation. All rights reserved.
*/

/*
** Name: giprpm.h
**
** Description:
**
**      Header file for Linux installer module used for querying RPM package
**	and database information.
**
** History:
**
**    09-Oct-2006 (hanje04)
**	SIR 116877
**      Created.
**	24-Apr-2007 (hanje04)
**	    BUG 118090
**	    SD 116383
**	    Add wrappers for setting an unsetting II_CONFIG to allow
**	    probing of config.dat and symbol.tbl from multiple instances,
**	    using PM and NM.
**	04-Jan-2008 (hanje04)
**	    BUG 119700
**	    Add wrappers for setting and unsetting II_SYSTEM too.
**	    
*/

/* wrapper for setting II_SYSTEM in program environment */
# define gip_setIIConfig( a )	setenv( "II_CONFIG", a, 1 )
# define gip_unSetIIConfig( )	unsetenv( "II_CONFIG" )
# define gip_setIISystem( a )	setenv( "II_SYSTEM", a, 1 )
# define gip_unSetIISystem( )	unsetenv( "II_SYSTEM" )

/* return values for rpmVersionCompare */
# define INST_RPM_NEWER -1
# define INST_RPM_SAME 0
# define INST_RPM_OLDER 1

STATUS
query_rpm_package_info			( int arg,
					char *argv[],
					UGMODE *state,
					i4 *count );
rpmRC
find_and_compare_existing_instance	( Header file_hdr,
				 	UGMODE *state,
					i4 *count,
					char *instance_loc );
rpmRC
get_new_pkg_hdr				( Header *file_hdr );

rpmRC
load_saveset_from_rpm_hdr 		( Header rpmheader, 
					saveset *target_info );

int
get_pkg_arch_info			( Header header,
					char **arch );
rpmRC
find_files_to_install			( void );

rpmRC
add_pkg_to_pkglst			( char *filename,
					PKGLST *pkglst );

rpmRC
init_saveset_info			( Header *core_fhdr, 
					const char *pkgformat );
