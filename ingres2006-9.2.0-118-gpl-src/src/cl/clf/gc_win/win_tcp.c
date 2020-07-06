/*
** Copyright (c) 1987, 2007 Ingres Corporation
*/
/*
**  Name: GCWINTCP.C
**  Description:
**      TCP/IP specific functions for the winsock driver.
**	Contains:
**	    GCwintcp_init()
**	    GCwintcp_addr()
**	    GCwintcp_port()
**  History: 
**	03-dec-93 (edg)
**	   Original.  pulled pieces out of former gcwintcp.c module since
**	   gcwinsck.c now handles most pof the real work.
**      12-Sep-95 (fanra01)
**          Extracted data for DLLs on Windows NT.
**	16-feb-98 (mcgem01)
**	    Read the port identifier from config.dat.  Use II_INSTALLATION
**	    as a last resort, rather than as the default.
**	23-Feb-1998 (thaal01)
**	    Allow space ( 8 bytes ) for string port_id.
**	27-feb-98 (mcgem01)
**	    Replace lines dropped by propagate script.
**	07-Jul-1998 (macbr01)
**	    Bug 91972 - jasgcc not receiving incoming communications. This is 
**	    due to incorrect usage of PMget() in function GCwintcp_init(). 
**	    Changed to test return code of PMget() instead of testing passed 
**	    in parameter for not equal to NULL.
**	15-jul-1998 (canor01)
**	    Move assignment of port_id to port_id_buf to prevent possible
**	    access violation. Clean up comments.
**	15-jul-1998 (canor01)
**	    macbr01's change uses "install" buffer instead of "port_id_buf",
**	    so remove port_id_buf.
**      06-aug-1999 (mcgem01)
**          Replace nat and longnat with i4.
**	02-may-2000 (somsa01)
**	    Changed subport such that it is now passed in.
**	30-jun-2000 (somsa01)
**	    subport is back here again where it belongs.
**	09-feb-2004 (somsa01)
**	    In GCwintcp_init() and GCwintcp_addr(), if the port ID that is
**	    passed in seems to be a valid port number, use it as is. Also,
**	    renamed global subport variable so as to not clash with the one
**	    used in GCwintcp_port().
**	20-feb-2004 (somsa01)
**	    Updated to account for multiple listen ports per protocol.
**	13-may-2004 (somsa01)
**	    In GCwintcp_init(), updated config.dat string used to retrieve port
**	    information such that we do not rely specifically on the GCC port.
**	    Also, corrected function used to convert subport into a number.
**      26-Jan-06 (loera01) Bug 115671
**          Added GCWINTCP_log_rem_host to allow invocation of gethostbyaddr() 
**          to be disabled in GCwintcp_init().
**      06-Feb-2007 (Ralph Loen) SIR 117590
**          Removed II_TCPIP_LOG_REM_HOST, since gethostbyaddr() is no
**          longer invoked for network listens.
*/

# include	<compat.h>
# include 	<winsock.h>
# include	<er.h>
# include	<cm.h>
# include	<cs.h>
# include	<cv.h>
# include	<me.h>
# include	<qu.h>
# include	<pc.h>
# include	<lo.h>
# include	<pm.h>
# include	<tr.h>
# include	<nm.h>
# include	<st.h>
# include	<gcccl.h>
# include	<gc.h>
# include	"gclocal.h"
# include	"gcclocal.h"

#define GC_STACK_SIZE   0x8000

/*
* *  Forward and/or External function references.
*/

STATUS 		GCwintcp_init(GCC_PCE * , GCC_WINSOCK_DRIVER *);
STATUS		GCwintcp_addr( char *, char *, struct sockaddr_in *, char * );
STATUS		GCwintcp_port( char *, i4 , char * );

GLOBALREF	WS_DRIVER WS_wintcp;

/*
** Statics
*/
static i4 sbprt = 0;

GLOBALREF	THREAD_EVENT_LIST IIGCc_proto_threads;

/*
**  Defines of other constants.
*/
GLOBALREF i4 GCWINTCP_trace;

# define GCTRACE(n) if ( GCWINTCP_trace >= n ) (void)TRdisplay


/*
** Name: GCwintcp_init
** Description:
**	WINTCP inititialization function.  This routine is called from
**	GCwinsock_init() -- the routine GCC calls to initialize protocol 
**	drivers.
**
**	This function does initialization specific to the protocol:
**	   Reads any protocol-specific env vars.
**	   Sets up the winsock protocol-specific control info.
** History:
**	05-Nov-93 (edg)
**	    created.
**	23-Feb-1998 (thaal01)
**	    Allow space for port_id, stops GCC crashing on startup sometimes.
**	07-Jul-1998 (macbr01)
**	    Bug 91972 - jasgcc not receiving incoming communications. This is 
**	    due to incorrect usage of PMget() in function GCwintcp_init(). 
**	    Changed to test return code of PMget() instead of testing passed 
**	    in parameter for not equal to NULL.
**	15-jul-1998 (canor01)
**	    Move assignment of port_id to port_id_buf to prevent possible
**	    access violation. Clean up comments.
**	09-feb-2004 (somsa01)
**	    When working with instance identifiers as port IDs, make sure
**	    we initialize sbprt with the trailing number, if set.
**	13-may-2004 (somsa01)
**	    Updated config.dat string used to retrieve port information such
**	    that we do not rely specifically on the GCC port. Also, corrected
**	    function used to convert subport into a number.
**      26-Jan-2006 (loera01) Bug 115671
**          Added GCWINTCP_log_rem_host to allow of gethostbyaddr() to be
**          disabled.
**      06-Feb-2007 (Ralph Loen) SIR 117590
**          Removed GCWINTCP_log_rem_host, since gethostbyaddr() is no
**          longer invoked for network listens.
*/
STATUS
GCwintcp_init(GCC_PCE * pptr, GCC_WINSOCK_DRIVER *wsd)
{

	char           *ptr = NULL;
	char            real_name_size[] = "100";
	char           *host, *server_id, *port_id;
	char            config_string[256];
	char            install[32]; //II_INSTALLATION code

        /*
        ** Get set up for the PMget call.
        */
        PMinit();
        if( PMload( NULL, (PM_ERR_FUNC *)NULL ) != OK )
                PCexit( FAIL );

        /*
        ** Construct the network port identifier.
        */

        host = PMhost();
        server_id = PMgetDefault(3);
        if (!server_id)
                server_id = "*" ;
        STprintf( config_string, ERx("!.wintcp.port"),
                  SystemCfgPrefix, host, server_id);

        /*
        ** Search config.dat for a match on the string we just built.
        ** If we don't find it, then use the value for II_INSTALLATION
        ** failing that, default to II.
        */
        if ((PMget( config_string, &port_id ) != OK) ||
            (port_id == NULL ))
        {
                NMgtAt("II_INSTALLATION", &ptr);
		port_id = install; 
                if (ptr != NULL && *ptr != '\0')
                {
                        STcopy(ptr, port_id);
                }
                else
                {
                        STcopy(SystemVarPrefix, port_id);
                }
        }

	if (CMalpha(port_id) && port_id[2])
	{
	    CVal(&port_id[2], &sbprt);
	    port_id[2] = '\0';
	}

	STcopy(port_id, pptr->pce_port);
	GCTRACE(1)("GCwintcp_init: port = %s\n", pptr->pce_port );


	/*
	** Fill in protocol-specific info
	*/
	wsd->addr_len = sizeof( struct sockaddr_in );
	wsd->sock_fam = AF_INET;
	wsd->sock_type = SOCK_STREAM;
	wsd->sock_proto = 0;
	wsd->block_mode = FALSE;
	wsd->pce_driver = (PTR)&WS_wintcp;

	/*
	** Get trace variable
	*/
        ptr = NULL;
	NMgtAt( "II_WINTCP_TRACE", &ptr );
	if ( !(ptr && *ptr) && PMget("!.wintcp_trace_level", &ptr) != OK )
	{
	    GCWINTCP_trace = 0;
	}
	else
	{
	    GCWINTCP_trace = atoi( ptr );
	}

	return OK;
}



/*
** Name: GCwintcp_addr
** Description:
**	Takes a node and port name and translates them this into
**	a sockaddr structure suitable for appropriate winsock call.
**	returns OK or FAIL.  puts a character string representation
**	into port_name.
** History:
**	03-dec-93 (edg)
**	    Original.
**	30-jun-2000 (somsa01)
**	    Update subport number after success.
**	30-aug-2001 (rodjo04) Bug 105650
**	    The machine name may have leading digits. We must see if this is
**	    a valid name before we can assume that this is an IP address. 
**	09-feb-2004 (somsa01)
**	    If the port ID that is passed in seems to be a valid port
**	    number, use it as is.
*/
STATUS
GCwintcp_addr( char *node, char *port, struct sockaddr_in *ws_addr, char *port_name )
{
    struct hostent *host_ent;
	
   
	if ( node == NULL )
    {
        /*
	** local listen address, use INADDR_ANY and subport if applicable.
	*/
	if (CMdigit(port))
	    STcopy(port, port_name);
	else
	{
	    if ( GCwintcp_port(port, sbprt, port_name) != OK )
		return FAIL;

	    if (sbprt)
		STprintf(port, "%s%d", port, sbprt);
	    sbprt++;
	}

	ws_addr->sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
       
	/* check for host id, e.g. 128.0.2.1 */
	if (CMdigit(node)) 
	{
		/*  Host name might have leading digits or could be the
		**  IP address. Call gethostbyname() anyway. If it is a name,
		**  we will get a pointer to the structure. If its an IP (in 
		**  IP format) then a structure will be returned even if the 
		**  IP is not 'alive'. If the IP is not alive, it will fail 
		**  later. 
		*/

		if (host_ent = gethostbyname(node))
			ws_addr->sin_addr.s_addr = *(u_long *) host_ent->h_addr;
		else
		{
			if (!(ws_addr->sin_addr.s_addr = inet_addr(node))) 
			 {
			    return FAIL;
			 }
		}
	} 
	else 
	{
	    if (!(host_ent = gethostbyname(node))) 
	    {
	        return FAIL;
	    }
	    ws_addr->sin_addr.s_addr = *(u_long *) host_ent->h_addr;
	}

        if ( GCwintcp_port(port, 0, port_name) != OK )
	    return FAIL;
    }

    ws_addr->sin_port = (u_short)htons( (u_short)atoi(port_name));
    ws_addr->sin_family = AF_INET;

    return OK;
}

/*
** Name: GCwintcp_port - turn a tcp port name into a tcp port number
**
** Description:
**	This routines provides mapping from II_INSTALLATION into a
**	unique tcp port number for the installation.  This scheme
**	was developed by the NCC (an RTI committee).
**
**	If pin is of the form:
**		XY
**	or	XY#
**	where
**		X is [a-zA-Z]
**		Y is [a-zA-Z0-9]
**	and	# is [0-7]
**
**	then pout is the string representation of the decimal number:
**
**        15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00
**       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
**       ! 0 ! 1 !  low 5 bits of X  !   low 6 bits of Y     !     #     !
**       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
**
**	If # is not provided, then the subport parameter is used.  This
**	allows programmatic permutations of the XY form of the port name.
**	If subport > 0 and port is not the XY form, fail.
**
**	If pin is not of the form XY or XY#; copy pin to pout.
**
** Inputs:
**	portin - source port name
**	subport - added as offset to port number
**	portout - resulting port name 
**
** History
**	23-jun-89 (seiwald)
**	    Written.
**	28-jun-89 (seiwald)
**	    Use wild CM macros instead of the sane but defunct CH ones.
**	2-feb-90 (seiwald)
**	    New subport parameter, so we can programmatically generate
**	    successive listen ports to use on listen failure.
**	24-May-90 (seiwald)
**	    Renamed from GC_tcp_port.
**	03-dec-93 (edg)
**	    This is pretty much a straight copy of BS_tcp_port.
*/

STATUS
GCwintcp_port( pin, subport, pout )
char	*pin;
i4	subport;
char	*pout;
{
	if( CMalpha( &pin[0] ) && ( CMalpha( &pin[1] ) | CMdigit( &pin[1] ) ) )
	{
		long	portid;
		char	p[ 3 ];

		if( subport > 7 || subport && pin[2] )
			return FAIL;

		CMtoupper( &pin[0], &p[0] );
		CMtoupper( &pin[1], &p[1] );
		p[2] = (char) (CMdigit( &pin[2] ) ? pin[2] : subport);

		portid = 1 << 14 
			| ( p[0] & 0x1f ) << 9
			| ( p[1] & 0x3f ) << 3
			| ( p[2] & 0x07 );

		CVla( portid, pout );

		return OK;
	} else {
		if( subport )
			return FAIL;

		STcopy( pin, pout );

		return OK;
	}
}
