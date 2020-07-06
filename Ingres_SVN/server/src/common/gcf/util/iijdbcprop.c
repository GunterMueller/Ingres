/*
** Copyright (c) 2007 Ingres Corporation
*/

# include       <compat.h>
# include       <er.h>
# include       <gl.h>
# include       <si.h>
# include       <lo.h>
# include       <nm.h>
# include       <st.h>
# include       <er.h>

/*
**
**             Ingres JDBC driver property generator utility
**             ---------------------------------------------
**
**        Usage: iijdbcprop -a | -t | -c | -f <fname> | -h |
**
**        -a          Lists all supported driver properties
**                    including tracing properties, with all commented out
**                    that are not in the Ingres config files to STDOUT.
**
**        -t          Lists all trace properties (commented out)
**                    to STDOUT.
**
**        -c          Same as flag 'a' but writes to a disk file
**                    named "iijdbc.properties".
**
**        -f <fname>  Same as flag 'c' but writes to a disk file
**                    provided by the user.
**
**        -h          Help information (this display).
*/

/*
** Name: iijdbcprop.c
**
** Description:
**	Using this utility, the Ingres JDBC properties can be listed on the
**	STDOUT or stored in a disk file named "iijdbc.properties" or in a
**	file name provided by the user under the $II_SYSTEM/ingres/files
**	directory. This utility is invoked from DAS server setup utility.
**	The various flags that can be used with this utility are listed above.
**
** History:
**	23-Oct-07 (rajus01) SIR 119618 
**	    Created.
**	13-Dec-07 (rajus01) SIR 119618
**	    Made changes suggested by Bruce.
**	19-Dec-07 (rajus01) SIR 119618
**	    Ported DIR_LOC reference to VMS.
**	20-Dec-07 (drivi01)
**	    Ported DIR_LOC reference to Windows.
*/


//property prefix
#define PROP_KEY_PREFIX		"ingres.jdbc.property"

//property filename
#define PROP_FNAME		"iijdbc.properties"
#ifdef VMS
#define DIR_LOC			"II_SYSTEM:[ingres.files]"
#elif NT_GENERIC
#define DIR_LOC			"%II_SYSTEM%\\ingres\\files"
#else
#define DIR_LOC			"$II_SYSTEM/ingres/files"
#endif /* VMS */

//property IDs

#define PROP_USR_ID		0	
#define PROP_PWD_ID		1	
#define PROP_GRP_ID		2	
#define PROP_ROLE_ID   		3	
#define PROP_DBUSR_ID		4
#define PROP_DBPWD_ID  		5	
#define PROP_POOL_ID   		6	
#define PROP_XACM_ID   		7	
#define PROP_VNODE_ID		8
#define PROP_LOOP_ID   		9	
#define PROP_CRSR_ID   		10	
#define PROP_ENCODE_ID    	11	
#define PROP_TIMEZONE_ID  	12
#define PROP_DEC_CHAR_ID  	13
#define PROP_DATE_FRMT_ID 	14 
#define PROP_MNY_FRMT_ID  	15 
#define PROP_MNY_PREC_ID  	16 
#define PROP_DATE_ALIAS_ID 	17

#define PROP_MAX		18


//property names
#define	PROP_USR_NAME		"user"
#define PROP_PWD_NAME		"password"
#define PROP_GRP_NAME		"group"
#define PROP_ROLE_NAME		"role"
#define PROP_DBUSR_NAME		"dbms_user"
#define PROP_DBPWD_NAME		"dbms_password"
#define PROP_POOL_NAME		"connect_pool"
#define PROP_XACM_NAME		"autocommit_mode"
#define PROP_VNODE_NAME		"vnode_usage"
#define PROP_LOOP_NAME		"select_loop"
#define PROP_CRSR_NAME		"cursor_mode"
#define PROP_ENCODE_NAME	"char_encode"
#define PROP_TIMEZONE_NAME	"timezone"
#define PROP_DEC_CHAR_NAME	"decimal_char"
#define PROP_DATE_FRMT_NAME	"date_format"
#define PROP_MNY_FRMT_NAME	"money_format"
#define PROP_MNY_PREC_NAME	"money_precision"
#define PROP_DATE_ALIAS_NAME	"date_alias"


//property values
#define PROP_USR_VAL		"User ID on the target DBMS"	
#define PROP_PWD_VAL		"User's OS password"	
#define PROP_GRP_VAL       	"User's group ID"	
#define PROP_ROLE_VAL   	"Role ID | Role ID/password"	
#define PROP_DBUSR_VAL		"User ID of DBMS session" 		
#define PROP_DBPWD_VAL  	"User's DBMS password"	
#define PROP_POOL_VAL   	"on/off"	
#define PROP_XACM_VAL   	"dbms/single/multi"	
#define PROP_VNODE_VAL		"connect/login"
#define PROP_LOOP_VAL   	"on/off"	
#define PROP_CRSR_VAL   	"dbms/update/readonly"	
#define PROP_ENCODE_VAL    	"Java Character Encoding"	
#define PROP_TIMEZONE_VAL  	"II_TIMEZONE_NAME equivalent"	
#define PROP_DEC_CHAR_VAL  	"II_DECIMAL equivalent"	
#define PROP_DATE_FRMT_VAL 	"II_DATE_FORMAT equivalent" 
#define PROP_MNY_FRMT_VAL  	"II_MONEY_FORMAT equivalent" 
#define PROP_MNY_PREC_VAL  	"II_MONEY_PREC equivalent" 
#define PROP_DATE_ALIAS_VAL	"ingresdate/ansidate"	

#define PROP_TRACE_LOG_ID	0	
#define PROP_TRACE_DRV_ID	1
#define PROP_TRACE_DS_ID	2
#define PROP_TRACE_MSG_ID	3
#define PROP_TRACE_MSG_TL_ID 	4	
#define PROP_TRACE_MSG_NL_ID    5	
#define PROP_TRACE_TS_ID	6
#define PROP_TRACE_DBMS_LOG_ID	7

#define PROP_TRACE_MAX		8

// Trace properties
#define PROP_TRACE_LOG_NAME		"ingres.jdbc.trace.log"
#define PROP_TRACE_DRV_NAME		"ingres.jdbc.trace.drv"
#define PROP_TRACE_DS_NAME		"ingres.jdbc.trace.ds"
#define PROP_TRACE_MSG_NAME		"ingres.jdbc.trace.msg"
#define PROP_TRACE_MSG_TL_NAME		"ingres.jdbc.trace.msg.tl"
#define PROP_TRACE_MSG_NL_NAME		"ingres.jdbc.trace.msg.nl"
#define PROP_TRACE_TS_NAME		"ingres.jdbc.trace.timestamp"
#define PROP_TRACE_DBMS_LOG_NAME	"ingres.jdbc.dbms.trace.log"

#define PROP_TRACE_LOG_VAL		"Path and file name for driver trace"
#define PROP_TRACE_DRV_VAL		"Driver tracing level (0-5)"
#define PROP_TRACE_DS_VAL		"Datasource tracing level (0-5)"
#define PROP_TRACE_MSG_VAL		"Messaging I/O tracing level (0-5)"
#define PROP_TRACE_MSG_TL_VAL		"Transport I/O tracing level (0-5)"
#define PROP_TRACE_MSG_NL_VAL		"Network I/O tracing level (0-5)"
#define PROP_TRACE_TS_VAL		"Include Timstamp in tracing (true)"
#define PROP_TRACE_DBMS_LOG_VAL		"Path and file name for DBMS trace"

static struct
{
     i2	  prop_id;
     char *prop_name;
     char *prop_val;

}jdbc_prop_info[] =
{
    { PROP_USR_ID,		PROP_USR_NAME,		PROP_USR_VAL },
    { PROP_PWD_ID,		PROP_PWD_NAME,		PROP_PWD_VAL },	
    { PROP_GRP_ID,		PROP_GRP_NAME,		PROP_GRP_VAL },
    { PROP_ROLE_ID,		PROP_ROLE_NAME,		PROP_ROLE_VAL }, 
    { PROP_DBUSR_ID,		PROP_DBUSR_NAME,	PROP_DBUSR_VAL },
    { PROP_DBPWD_ID,		PROP_DBPWD_NAME,	PROP_DBPWD_VAL },	
    { PROP_POOL_ID,		PROP_POOL_NAME,		PROP_POOL_VAL },	
    { PROP_XACM_ID,		PROP_XACM_NAME,		PROP_XACM_VAL },
    { PROP_VNODE_ID,		PROP_VNODE_NAME,	PROP_VNODE_VAL },
    { PROP_LOOP_ID,		PROP_LOOP_NAME,		PROP_LOOP_VAL },
    { PROP_CRSR_ID,		PROP_CRSR_NAME,		PROP_CRSR_VAL },
    { PROP_ENCODE_ID,		PROP_ENCODE_NAME,	PROP_ENCODE_VAL },	
    { PROP_TIMEZONE_ID,		PROP_TIMEZONE_NAME,	PROP_TIMEZONE_VAL}, 
    { PROP_DEC_CHAR_ID,		PROP_DEC_CHAR_NAME,	PROP_DEC_CHAR_VAL}, 
    { PROP_DATE_FRMT_ID,	PROP_DATE_FRMT_NAME,	PROP_DATE_FRMT_VAL }, 
    { PROP_MNY_FRMT_ID,		PROP_MNY_FRMT_NAME,	PROP_MNY_FRMT_VAL }, 
    { PROP_MNY_PREC_ID,		PROP_MNY_PREC_NAME, 	PROP_MNY_PREC_VAL }, 
    { PROP_DATE_ALIAS_ID,	PROP_DATE_ALIAS_NAME,	PROP_DATE_ALIAS_VAL },
};

static struct
{
     i2	  trace_id;
     char *trace_name;
     char *trace_val;

}trace_prop_info[] =
{
    { PROP_TRACE_LOG_ID,	PROP_TRACE_LOG_NAME,	PROP_TRACE_LOG_VAL },
    { PROP_TRACE_DRV_ID,	PROP_TRACE_DRV_NAME,	PROP_TRACE_DRV_VAL },
    { PROP_TRACE_DS_ID,		PROP_TRACE_DS_NAME,	PROP_TRACE_DS_VAL },
    { PROP_TRACE_MSG_ID,	PROP_TRACE_MSG_NAME,	PROP_TRACE_MSG_VAL },
    { PROP_TRACE_MSG_TL_ID,	PROP_TRACE_MSG_TL_NAME,	PROP_TRACE_MSG_TL_VAL },
    { PROP_TRACE_MSG_NL_ID,	PROP_TRACE_MSG_NL_NAME,	PROP_TRACE_MSG_NL_VAL },
    { PROP_TRACE_TS_ID,		PROP_TRACE_TS_NAME,	PROP_TRACE_TS_VAL },
    { PROP_TRACE_DBMS_LOG_ID,	PROP_TRACE_DBMS_LOG_NAME,PROP_TRACE_DBMS_LOG_VAL },
};

void print_all_prop_file( char *fname );
void print_all_prop( FILE ** );
void print_all_trace_prop( FILE ** );
void print_usage();

char filename[LO_NM_LEN];

int
main( int argc, char **argv )
{
    bool write_to_file = FALSE;
    int i;

    if( argv[1] && STlength(argv[1]) > 2 )
    {
	print_usage();
	return(OK);
    }

    for( i = 1; i < argc; i++ )
	if( argv[i][0] == '-' )
        {
            switch( argv[i][1] )
            {
		case 'a':
		    print_all_prop(NULL);
		    print_all_trace_prop(NULL);
		    return (OK);
		case 'c':
		    print_all_prop_file(PROP_FNAME);
		    return (OK);
		case 'f':
		    write_to_file = TRUE;
		    continue;
		case 't':
		    print_all_trace_prop(NULL);
		    return (OK);
		case 'h':
		    print_usage();
		    return (OK);
		default:
		    SIfprintf( stderr, "Invalid flag: %s\n", argv[i]);
		    print_usage();
		    return( FAIL );
	    }
	}

	/* Read the file name */
	if( write_to_file && argv[i] != NULL )
	{
	    STlcopy(argv[i], filename, LO_NM_LEN);
	    print_all_prop_file(filename);
	    return( OK );
	}  

    print_usage();
    return (OK);
}

void print_all_prop_file( char *fname )
{
    LOCATION loc;
    FILE  *file;
    NMloc( FILES, FILENAME, fname, &loc );
    SIopen( &loc, "w", &file );
    print_all_prop(&file);
    print_all_trace_prop(&file);
    SIclose( file);     
    SIfprintf( stdout, 
	"The JDBC driver properties file '%s' was created in %s directory\n", fname, DIR_LOC );
    return;
}

void print_all_prop( FILE **file )
{

    int i;
    char prop_key[128];
    char *env = 0;
    char prop_val[128];
    bool symbol_found = FALSE;
    for( i = 0; i < PROP_MAX; i++ )
    {
        switch( jdbc_prop_info[i].prop_id )
        {
        case PROP_TIMEZONE_ID:
        NMgtAt( ERx("II_TIMEZONE_NAME"), &env );
        if ( env && *env )
        {
            STcopy( env, prop_val ); 
	    symbol_found = TRUE;
        }
        break;
        case PROP_DEC_CHAR_ID:
        NMgtAt( ERx("II_DECIMAL"), &env );
        if ( env && *env )
        {
            STcopy( env, prop_val ); 
	    symbol_found = TRUE;
        }
        break;
        case PROP_DATE_FRMT_ID:
        NMgtAt( ERx("II_DATE_FORMAT"), &env );
        if ( env && *env )
	{
            STcopy( env, prop_val ); 
	    symbol_found = TRUE;
        }
        break;
        case PROP_MNY_FRMT_ID:
        NMgtAt( ERx("II_MONEY_FORMAT"), &env );
        if ( env && *env )
	{
            STcopy( env, prop_val ); 
	    symbol_found = TRUE;
	}
        break;
        case PROP_MNY_PREC_ID:
        NMgtAt( ERx("II_MONEY_PREC"), &env );
        if ( env && *env )
	{
            STcopy( env, prop_val ); 
	    symbol_found = TRUE;
	}
        break;
        default:
	symbol_found = FALSE;
        break;
        }
	STprintf(prop_key, "%s.%s", PROP_KEY_PREFIX,
				   jdbc_prop_info[i].prop_name );  
	if( !file )
	    SIfprintf( stdout, symbol_found?"%s=%s\n":"#%s=<%s>\n", prop_key, 
				symbol_found?prop_val:jdbc_prop_info[i].prop_val );
	else
	    SIfprintf( *file, symbol_found?"%s=%s\n":"#%s=<%s>\n", prop_key, 
				symbol_found?prop_val:jdbc_prop_info[i].prop_val );
	symbol_found = FALSE;
        prop_val[0]='\0';
    }

    return;
}

void print_all_trace_prop( FILE **file)
{
    int i;
    for( i = 0; i < PROP_TRACE_MAX; i++ )
    if( !file )
	SIfprintf( stdout, "#%s=<%s>\n", trace_prop_info[i].trace_name, 
					trace_prop_info[i].trace_val );
    else
	SIfprintf( *file, "#%s=<%s>\n", trace_prop_info[i].trace_name, 
					trace_prop_info[i].trace_val );
    return;
}

void print_usage()
{
    SIfprintf(stdout, "\n\t\t%s\n", 
		"Ingres JDBC driver property generator utility" );
    SIfprintf(stdout, "\t\t%s\n\n", 
		"---------------------------------------------" );
    SIfprintf( stdout, "%\t%s\n",
		 "Usage: iijdbcprop -a | -t | -c | -f <fname> | -h |\n");
    SIfprintf( stdout, "%\t%s\n",
		 "-a           Lists all supported driver properties");
    SIfprintf( stdout, "%\t%s\n",
		 "             including tracing properties, with all commented out");
    SIfprintf( stdout, "%\t%s\n",
		 "             that are not in the Ingres config files to STDOUT.\n");
    SIfprintf( stdout, "%\t%s\n",
		 "-t           Lists all trace properties (commented out)");
    SIfprintf( stdout, "%\t%s\n",
		 "             to STDOUT.\n");
    SIfprintf( stdout, "%\t%s\n",
		 "-c           Same as flag 'a' but writes to a disk file");
    SIfprintf( stdout, "%\t%s\n",
		 "             named \"iijdbc.properties\".\n"); 
    SIfprintf( stdout, "%\t%s\n",
		 "-f <fname>   Same as flag 'c' but writes to a disk file");
    SIfprintf( stdout, "%\t%s\n",
		 "             provided by the user.\n"); 
    SIfprintf( stdout, "%\t%s\n",
		 "-h           Help information (this display).\n"); 
    SIfprintf( stdout, "\t%-65s\n",
		"Please refer to Ingres Connectivity guide to learn more about");
    SIfprintf( stdout, "\t%-55s\n",
		"all of the supported Ingres JDBC driver properties.\n" );  

    return;
}
