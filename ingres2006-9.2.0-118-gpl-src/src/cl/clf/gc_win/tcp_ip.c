/*
** Copyright (c) 2003, 2007 Ingres Corporation
*/
/*
**  Name: TCP_IP.C
**  Description:
**      TCP/IP specific functions for the Winsock 2.2 driver.
**	Contains:
**	    GCtcpip_init()
**	    GCtcpip_addr()
**	    GCtcpip_port()
**  History: 
**	29-sep-2003 (somsa01)
**	    Created based upon win_tcp.c.
**	09-feb-2004 (somsa01)
**	    In GCtcpip_init() and GCtcpip_addr(), if the port ID that is
**	    passed in seems to be a valid port number, use it as is. Also,
**	    renamed global subport variable so as to not clash with the one
**	    used in GCtcpip_port().
**	20-feb-2004 (somsa01)
**	    Updated to account for multiple listen ports per protocol.
**	13-may-2004 (somsa01)
**	    In GCtcpip_init(), updated config.dat string used to retrieve port
**	    information such that we do not rely specifically on the GCC port.
**	    Also, corrected function used to convert subport into a number.
**      26-Jan-2006 (Ralph Loen) Bug 115671
**          Added GCTCPIP_log_rem_host to optionally disable invocation of 
**          gethostbyaddr() in GCtcpip_init().
**      28-Aug-2006 (lunbr01) Sir 116548
**          Add IPv6 support.
**      06-Feb-2007 (Ralph Loen) SIR 117590
**          Removed II_TCPIP_log_rem_host, since getnameinfo() is no longer 
**          invoked for network listens.
*/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <compat.h>
#include <er.h>
#include <cm.h>
#include <cs.h>
#include <cv.h>
#include <me.h>
#include <qu.h>
#include <pc.h>
#include <lo.h>
#include <pm.h>
#include <tr.h>
#include <nm.h>
#include <st.h>
#include <gcccl.h>
#include <gc.h>
#include "gclocal.h"
#include "gcclocal.h"

/*
**  Forward and/or External function references.
*/
STATUS 		GCtcpip_init(GCC_PCE * , GCC_WINSOCK_DRIVER *);
STATUS		GCtcpip_addr( char *, char *, char * );
STATUS		GCtcpip_port( char *, i4 , char * );

GLOBALREF	WS_DRIVER WS_tcpip;

/*
**  Statics
*/
static i4 sbprt = 0;

/*
**  Defines of other constants.
*/
GLOBALREF i4 GCTCPIP_trace;


#define GCTRACE(n) if (GCTCPIP_trace >= n) (void)TRdisplay


/*
**  Name: GCtcpip_init
**  Description:
**	TCP_IP inititialization function.  This routine is called from
**	GCwinsock2_init() -- the routine GCC calls to initialize protocol 
**	drivers.
**
**	This function does initialization specific to the protocol:
**	Reads any protocol-specific env vars.
**	Sets up the Winsock 2.2 protocol-specific control info.
**
**  History:
**	29-sep-2003 (somsa01)
**	    Created.
**	09-feb-2004 (somsa01)
**	    If the port ID that is passed in seems to be a valid port
**	    number, use it as is.
**	13-may-2004 (somsa01)
**	    Updated config.dat string used to retrieve port information such
**	    that we do not rely specifically on the GCC port. Also, corrected
**	    function used to convert subport into a number.
**      26-Jan-2006 (Ralph Loen) Bug 115671
**          Added GCTCPIP_log_rem_host to disable invocation of gethostbyaddr().
**      28-Aug-2006 (lunbr01) Sir 116548
**	    Add IPv6 support. Check II_TCPIP_VERSION and tcp_ip.version to
**	    see if new protocol restricted to IPv4 or IPv6 only.
*/

STATUS
GCtcpip_init(GCC_PCE * pptr, GCC_WINSOCK_DRIVER *wsd)
{
    char	*ptr;
    char	real_name_size[] = "100";
    char	*host, *server_id, *port_id;
    char	config_string[256];
    char	install_id[3];
    int		sock_fam, tcpip_version;

    /*
    **  Get trace variable
    */
    NMgtAt("II_TCP_TRACE", &ptr);
    if (!(ptr && *ptr) && PMget("!.tcp_trace_level", &ptr) != OK)
	GCTCPIP_trace = 0;
    else
	GCTCPIP_trace = atoi(ptr);

    /*
    **  Get set up for the PMget call.
    */
    PMinit();
    if (PMload(NULL, (PM_ERR_FUNC *)NULL) != OK)
	PCexit(FAIL);

    /*
    **  Construct the network port identifier.
    */
    host = PMhost();
    server_id = PMgetDefault(3);
    if (!server_id)
	server_id = "*" ;
    STprintf(config_string, ERx("!.tcp_ip.port"), SystemCfgPrefix,
	     host, server_id);

    /*
    **  Search config.dat for a match on the string we just built.
    **  If we don't find it, then use the value for II_INSTALLATION
    **  failing that, default to II.
    */
    if ((PMget(config_string, &port_id) != OK) || (port_id == NULL))
    {
	NMgtAt("II_INSTALLATION", &ptr);
	port_id = install_id; 
	if (ptr != NULL && *ptr != '\0')
	    STcopy(ptr, port_id);
	else
	    STcopy(SystemVarPrefix, port_id);
    }

    if (CMalpha(port_id) && port_id[2])
    {
	CVal(&port_id[2], &sbprt);
	port_id[2] = '\0';
    }

    STcopy(port_id, pptr->pce_port);
    GCTRACE(1)("GCtcpip_init: port = %s\n", pptr->pce_port);

    /*
    **  Check to see if driver being restricted to IPv4 or IPv6 only.
    */
    NMgtAt( "II_TCPIP_VERSION", &ptr );
    if ( !(ptr && *ptr) && PMget("!.tcp_ip.version", &ptr) != OK )
	sock_fam = AF_UNSPEC;
    else if (STbcompare(ptr, 0, "ALL", 0, TRUE) == 0)
	sock_fam = AF_UNSPEC;
    else
    {
	tcpip_version = atoi(ptr);     /* 4=IPv4 or 6=IPv6 */
	if (tcpip_version == 4)
	    sock_fam = AF_INET;
	else if (tcpip_version == 6)
	    sock_fam = AF_INET6;
	else
	{
	    GCTRACE(1)("GCtcpip_init: Invalid tcp_ip.version = %s\n", ptr);
	    return(FAIL);
	}
    }

    /*
    **  Fill in protocol-specific info
    */
    wsd->addr_len = sizeof(struct sockaddr_in);
    wsd->sock_fam = sock_fam;     /* IPv4 or IPv6 or both*/
    wsd->sock_type = SOCK_STREAM;
    wsd->sock_proto = IPPROTO_TCP;
    wsd->block_mode = FALSE;
    wsd->pce_driver = (PTR)&WS_tcpip;

    return(OK);
}

/*
**  Name: GCtcpip_addr
**  Description:
**	Takes a node and port name and translates the port into
**	a value suitable for a getaddrinfo() call.
**	Returns OK or FAIL.  Puts a character string representation
**	of the numeric port into port_name.
**
**  History:
**	29-sep-2003 (somsa01)
**	    Created.
**	09-feb-2004 (somsa01)
**	    If the port ID that is passed in seems to be a valid port
**	    number, use it as is.
**	28-Aug-2006 (lunbr01) Sir 116548
**	    For IPv6 support, use addrinfo input parm instead of 
**	    sockaddr_in.  Remove unneeded code now done by MS
**	    Winsock2 routine getaddrinfo().
*/

STATUS
GCtcpip_addr(char *node, char *port, char *port_name)
{
    if (node == NULL)
    {
	/*
	** Local listen address, use subport if applicable.
	*/
	if (CMdigit(port))
	    STcopy(port, port_name);
	else
	{
	    if (GCtcpip_port(port, sbprt, port_name) != OK)
		return(FAIL);

	    if (sbprt)
		STprintf(port, "%s%d", port, sbprt);
	    sbprt++;
	}
    }
    else
    {
        if (GCtcpip_port(port, 0, port_name) != OK)
	    return FAIL;
    }

    return(OK);
}

/*
**  Name: GCtcpip_port - turn a tcp port name into a tcp port number
**
**  Description:
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
**  Inputs:
**	portin - source port name
**	subport - added as offset to port number
**	portout - resulting port name 
**
**  History
**	29-sep-2003 (somsa01)
**	    Created.
*/

STATUS
GCtcpip_port(char *pin, i4 subport, char *pout)
{
    if (CMalpha(&pin[0]) && (CMalpha(&pin[1]) | CMdigit(&pin[1])))
    {
	long	portid;
	char	p[3];

	if (subport > 7 || subport && pin[2])
	    return(FAIL);

	CMtoupper(&pin[0], &p[0]);
	CMtoupper(&pin[1], &p[1]);
	p[2] = (char)(CMdigit(&pin[2]) ? pin[2] : subport);

	portid = 1 << 14
			| (p[0] & 0x1f) << 9
			| (p[1] & 0x3f) << 3
			| (p[2] & 0x07);

	CVla(portid, pout);

	return(OK);
    }
    else
    {
	if (subport)
	    return FAIL;

	STcopy(pin, pout);

	return(OK);
    }
}
