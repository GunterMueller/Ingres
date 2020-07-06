/*
**  Copyright (c) 2007 Ingres Corporation
**
*/

/*
**  Name: GCWINSOCK2.C
**  Description:
**	The main driver for all protocols that go thru the WinSock 2.2 API.
**	The protocols accessed via the WinSock 2.2 API will all basically
**	use the same set of calls and mechanisms. There are some slight
**	differences between adresses and such which get addressed in
**	specific routines for the protocol.
** 
**  History: 
**	01-oct-2003 (somsa01)
**	    Created.
**	29-jan-2004 (somsa01)
**	    Changed SecureZeroMemory() to ZeroMemory(). SecureZeroMemory()
**	    is only available on Windows Server 2003.
**	09-feb-2004 (somsa01)
**	    Removed extra declaration of GCwinsock2_exit(). Also, in
**	    GCwinsock2_open(), when working with an instance identifier as
**	    a port address, make sure we start with only the first two
**	    characters. The subport is calculated from within the driver's
**	    addr_func() function.
**	20-feb-2004 (somsa01)
**	    Updates to account for multiple listen ports per protocol.
**	08-oct-2004 (somsa01)
**	    In GCwinsock2(), if we're going to queue a completion routine
**	    execution, make sure we return OK from this function. Also, in
**	    GCC_DISCONNECT, make sure the pcb is valid. Also, in
**	    GCwinsock2_worker_thread(), make sure lpPerHandleData is valid
**	    in OP_DISCONNECT.
**      15-Feb-2005 (lunbr01)  Bug 113903, Problem EDJDBC 97
**          Do not clear the 3rd char of the input port when passed in
**          as XX# format.  Some callers, such as the JDBC server, pass in
**          an explicit symbolic port to use.
**      23-feb-2005 (lunbr01)  bug 113686, prob INGNET 160
**          E_GC4810_TL_FSM_STATE errors occur occasionally in GCD due to 
**          SEND (or RECV) completion routine being called AFTER disconnect
**          compl rtn. Add "call completion routine" requests at END of
**          queue rather than beginning to force FIFO processing of
**          completions.  Each occurrence of E_GC4810 results in approx 
**          9K memory leak.
**      29-feb-2005 (lunbr01)  bug 113987, prob INGNET 165
**          Pending async RECEIVES occasionally do not post complete after
**          the socket is closed during disconnect, which resulted in the
**          disconnect looping forever waiting for the IO to complete.
**          Limit the number of times disconnect will wait.  Symptom
**          is that subsequent connect requests may hang.
**      14-jul-2004 (loera01) Bug 114852
**          For GCwinsock2_worker_thread() and GCwinsock2_AIO_Callback(),
**          in the OP_RECV_MSG_LEN operation, fetch the second byte of
**          the message length if only one byte was sent.
**      16-Aug-2005 (loera01) Bug 115050, Problem INGNET 179
**          In GCwinsock2_listen(), don't retrieve the hostname
**          via gethostbyaddr() if the connection is local.  Instead,
**          copy the local hostname from GChostname().
**      26-Jan-2006 (Ralph Loen) Bug 115671
**          Added GCTCPIP_log_rem_host to optionally disable invocation of 
**          gethostbyaddr() in GCwinsock2_listen() and 
**          GCwinsock2_worker_thread().
**      28-Aug-2006 (lunbr01) Sir 116548
**          Add IPv6 support.
**      22-Jan-2007 (lunbr01) Bug 117523
**          Corrupted (invalid) incoming data with large length prefix 
**          can crash caller (eg, GCC or GCD).  Add check to ensure
**          caller's buffer length is not exceeded (ie, avoid buffer overrun).
**      06-Feb-2007 (Ralph Loen) SIR 117590
**          In GCwinsock2_listen_complete(), removed getnameinfo() and set 
**          the node_id field in the GCC listen parms to NULL.
**      03-Aug-2007 (Ralph Loen) SIR 118898
**          Add GCdns_hostname().  In GCwinsock2_init(), moved WSA
**          startup logic into GCwinsock2_startup_WSA(), so that both
**          GCdns_hostname() and GCwinsock2_init() could check and startup
**          WSA. 
*/

/* FD_SETSIZE is the maximum number of sockets that can be
** passed to a select() call.  This must be defined before
** winsock2.h is included
*/
#define	FD_SETSIZE 1024
#define WINSOCK_VERSION2

#include <winsock2.h>
#include <ws2tcpip.h>  /* IPv6 extensions */
#include <wspiapi.h>   /* Dynamic link for pre-IPv6 Windows */
#include <mswsock.h>
#include <compat.h>
#include <wsipx.h>
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
#include <st.h>
#include <nm.h>
#include <gcccl.h>
#include <gc.h>
#include "gclocal.h"
#include "gcclocal.h"

typedef unsigned char *u_caddr_t;
typedef char   *caddr_t;

#define GC_STACK_SIZE   0x8000
#define MAX_DISCONNECT_ASYNC_WAITS 10

/*
**  Forward and/or External function references.
*/

STATUS		GCwinsock2_init(GCC_PCE * pptr);
STATUS          GCwinsock2_startup_WSA();
STATUS		GCwinsock2(i4 function_code, GCC_P_PLIST * parm_list);
VOID		GCwinsock2_exit(void);
DWORD WINAPI	GCwinsock2_worker_thread(LPVOID CompletionPortID);
STATUS		GCwinsock2_open(GCC_P_PLIST *parm_list);
VOID 		GCwinsock2_open_setSPXport(SOCKADDR_IPX *spx_sockaddr, char *port_name);
STATUS		GCwinsock2_listen(void *parms);
VOID 		GCwinsock2_listen_complete(GCC_P_PLIST *parm_list, char *AcceptExBuffer);
STATUS  	GCwinsock2_connect(GCC_P_PLIST *parm_list, char *lpPerIoData);
VOID 		GCwinsock2_connect_complete(GCC_P_PLIST *parm_list);
char *  	GCwinsock2_display_IPaddr(struct addrinfo *lpAddrinfo, char *IPaddr_out);
THREAD_EVENTS	*GCwinsock2_get_thread(char *);
VOID CALLBACK	GCwinsock2_AIO_Callback(DWORD, DWORD, WSAOVERLAPPED *, DWORD);
VOID 		GCwinsock2_close_listen_sockets(GCC_LSN *lsn);

/*
** Global references to externals: these handles are used by modules outside
** of this one which also need to synchronize  with the threads created here.
**
** Protocol threads are all synchronized with the main struture:
** IIGCc_proto_threads -- the main Event structure for protocol driver threads.
*/
GLOBALREF bool			WSAStartup_called;
GLOBALREF HANDLE		hAsyncEvents[];
GLOBALREF HANDLE		hMutexGccCompleteQ;
GLOBALREF THREAD_EVENT_LIST	IIGCc_proto_threads;
GLOBALREF BOOL			is_comm_svr;
GLOBALREF i4			GCWINSOCK2_trace;

/*
** The PER_HANDLE_DATA is allocated per socket, and contains useful information
** related to the socket.
*/
typedef struct _PER_HANDLE_DATA
{
    volatile LONG	AsyncIoRunning;
} PER_HANDLE_DATA;

/*
** The PER_IO_DATA is allocated per I/O request, conaining information
** pertaining to the I/O request.
*/
typedef struct _PER_IO_DATA
{
    WSAOVERLAPPED	Overlapped;
    SOCKET		listen_sd;
    SOCKET		client_sd;
    int			OperationType;
#define		OP_ACCEPT	0x010	/* AcceptEx */
#define		OP_CONNECT	0x020	/* ConnectEx */
#define		OP_RECV_MSG_LEN	0x040	/* WSARecv message length */
#define		OP_RECV		0x080	/* WSARecv */
#define		OP_SEND		0x100	/* WSASend */
#define		OP_DISCONNECT	0x200	/* Disconnect */
    WSABUF		wbuf;
    bool		block_mode;
    GCC_P_PLIST		*parm_list;
    GCC_LSN     	*lsn;              /* ptr to LSN struct for addr/port */
    struct addrinfo	*addrinfo_posted;  /* ptr to last posted lsn addr */
    int     		socket_ix_posted;  /* socket index last posted */
    char		AcceptExBuffer[(sizeof(SOCKADDR_STORAGE) + 16) * 2];
} PER_IO_DATA;

/*
**  The PCB is allocated on listen or connection request.  The format is
**  internal and specific to each protocol driver.
*/
typedef struct _PCB
{
    SOCKET		sd;
    i4			tot_snd;	    /* total to send */
    i4			tot_rcv;	    /* total to rcv */
    char		*snd_bptr;	    /* utility buffer pointer */
    char		*rcv_bptr;	    /* utility buffer pointer */
    char		recv_msg_len[2];    /* used in GCC_RECEIVE to read
					    ** in the 1st 2 bytes which is
					    ** a length of message field.
					    */
    struct addrinfo	*addrinfo_list;      /* ptr to addrinfo list for connect */
    struct addrinfo	*lpAddrinfo;        /* ptr to current addrinfo entry */
    GCC_ADDR		lcl_addr, rmt_addr;
    PER_HANDLE_DATA	*lpPerHandleData;
} PCB;

/*
**  Defines of other constants.
*/
#ifdef xDEBUG
#define GCTRACE(n)	if (GCWINSOCK2_trace >= n) (void)GCtrace
#else
#define GCTRACE(n)	if (GCWINSOCK2_trace >= n) (void)TRdisplay
#endif

/*
**  Definition of static variables and forward static functions.
*/
static bool				Winsock2_exit_set = FALSE;
static bool				is_win9x = FALSE;
static WSADATA  			startup_data;
static i4				GCWINSOCK2_timeout;
static HANDLE				GCwinsock2CompletionPort = NULL;
static LPFN_ACCEPTEX			lpfnAcceptEx = NULL;
static LPFN_CONNECTEX			lpfnConnectEx = NULL;
static LPFN_GETACCEPTEXSOCKADDRS	lpfnGetAcceptExSockaddrs = NULL;


/*
**  Name: GCwinsock2_init
**  Description:
**	WinSock 2.2 inititialization function. This routine is called from
**	GCpinit() -- the routine GCC calls to initialize protocol drivers.
**
**	This function does initialization specific to the protocol:
**	    Initialize tracing variables
**	    Creates I/O completion port for the protocol (non-Windows 9x)
**	    Create worker threads for the I/O completion port (non-Windows 9x)
**	    Load WinSock extension functions (non-Windows 9x)
**
**  History:
**	01-oct-2003 (somsa01)
**	    Created.
**      28-Aug-2006 (lunbr01) 
**          Cannot call WSAGetLastError() if WSAStartup() fails, as with
**          other Windows Sockets calls, because WS2_32.DLL will not have
**          been loaded.
**      03-Aug-2007 (Ralph Loen) SIR 118898
**          Moved WSA startup logic into GCwinsock2_startup_WSA(), so that both
**          GCdns_hostname() and GCwinsock2_init() could check and startup
**          WSA.
*/

STATUS
GCwinsock2_init(GCC_PCE * pptr)
{
    char		*ptr;
    GCC_WINSOCK_DRIVER	*wsd;
    THREAD_EVENTS	*Tptr;
    WS_DRIVER		*driver = (WS_DRIVER *)pptr->pce_driver;
    SECURITY_ATTRIBUTES	sa;
    OSVERSIONINFOEX	osver;
    u_i4		NumThreads;

    if (!WSAStartup_called)  
        if (GCwinsock2_startup_WSA() == FAIL)
            return FAIL;

    iimksec(&sa);

    /*
    ** Should use GVverifyOSversion here but it doesn't have the options 
    ** required.
    ** Could use GVosvers but would require explicit string comparisons for 95
    ** and 98.
    */
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if (GetVersionEx((LPOSVERSIONINFO)&osver))
    {
        /*
        ** Platform Ids are
        ** VER_PLATFORM_WIN32s
        ** VER_PLATFORM_WIN32_WINDOWS
        **      4.0     = Win95
        **      4.x     = Win98 & WinMe
        ** VER_PLATFORM_WIN32_NT
        ** VER_PLATFORM_WIN32_CE
        */
        is_win9x = ((osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
            ((osver.dwMajorVersion == 4) && (osver.dwMinorVersion >= 0)));
    }
    else
    {
        osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if (GetVersionEx((LPOSVERSIONINFO)&osver))
        {
            is_win9x = ((osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
                ((osver.dwMajorVersion == 4) && (osver.dwMinorVersion >= 0)));
        }
        else
            is_win9x = TRUE;
    }
    
    /*
    ** Get trace variable
    */
    NMgtAt("II_WINSOCK2_TRACE", &ptr);
    if (!(ptr && *ptr) && PMget("!.winsock2_trace_level", &ptr) != OK)
	GCWINSOCK2_trace = 0;
    else
	GCWINSOCK2_trace = atoi(ptr);

    /*
    ** Get timeout variable
    */
    NMgtAt("II_WINSOCK2_TIMEOUT", &ptr);
    if (!(ptr && *ptr) && PMget("!.winsock2_timeout", &ptr) != OK)
	GCWINSOCK2_timeout = 10;
    else
	GCWINSOCK2_timeout = atoi(ptr);

    /*
    ** Get pointer to WinSock 2.2 protocol's control entry in table.
    */
    Tptr = (THREAD_EVENTS *)pptr->pce_dcb;
    wsd = &Tptr->wsd;

    /*
    ** Now call the protocol-specific init routine.
    */
    if ((*driver->init_func)(pptr, wsd) != OK)
	return(FAIL);

    /*
    ** If this is not Windows 9x, create the I/O completion port to handle
    ** completion of asynchronous events, along with its worker threads.
    ** Also, load the WinSock extension functions.
    */
    if (!is_win9x)
    {
	SYSTEM_INFO	SystemInfo;
	u_i4		i;
	SOCKET		sd;
	GUID		GuidAcceptEx = WSAID_ACCEPTEX;
	GUID		GuidConnectEx = WSAID_CONNECTEX;
	GUID		GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	DWORD		dwBytes;

	/*
	** Create a dummy socket.
	*/
	if ((sd = WSASocket(wsd->sock_fam, wsd->sock_type, wsd->sock_proto,
			    NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
	    GCTRACE(1)("GCwinsock2_init: Couldn't create socket, status = %d\n",
			WSAGetLastError());
	    return(FAIL);
	}

	WSAIoctl(sd, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx,
		 sizeof(GuidAcceptEx), &lpfnAcceptEx, sizeof(lpfnAcceptEx),
		 &dwBytes, NULL, NULL);

	WSAIoctl(sd, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx,
		 sizeof(GuidConnectEx), &lpfnConnectEx, sizeof(lpfnConnectEx),
		 &dwBytes, NULL, NULL);

	WSAIoctl(sd, SIO_GET_EXTENSION_FUNCTION_POINTER,
		 &GuidGetAcceptExSockaddrs, sizeof(GuidGetAcceptExSockaddrs),
		 &lpfnGetAcceptExSockaddrs, sizeof(lpfnGetAcceptExSockaddrs),
		 &dwBytes, NULL, NULL);

	closesocket(sd);

	GCwinsock2CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
							  NULL, 0, 0);
	if (!GCwinsock2CompletionPort)
	{
	    GCTRACE(1)("GCwinsock2_init: Couldn't create I/O completion port, status = %d\n",
			GetLastError());
	    return(FAIL);
	}

	/*
	** Set up default # of worker threads for IO Completion Port.
	** More may be added later in GCC_OPEN based on number of addresses
	** being listened on to avoid unlikely, but possible, tie-up of
	** all worker threads in pending listen completion status (OP_ACCEPT).
	*/
	GetSystemInfo(&SystemInfo);
	NumThreads =
		SystemInfo.dwNumberOfProcessors + 2 > MAX_WORKER_THREADS ?
		    MAX_WORKER_THREADS : SystemInfo.dwNumberOfProcessors + 2;
	for (i = 0; i < NumThreads; i++)
	{
	    HANDLE	ThreadHandle;
	    int		tid;

	    ThreadHandle = CreateThread(
			    &sa, GC_STACK_SIZE,
			    (LPTHREAD_START_ROUTINE)GCwinsock2_worker_thread,
			    GCwinsock2CompletionPort, 0, &tid);
	    if (ThreadHandle)
	    {
		GCTRACE(3)("GCwinsock2_init: Worker Thread [%d] tid = %d, Handle = %d\n",
			   i, tid, ThreadHandle);
		Tptr->hWorkerThreads[i] = ThreadHandle;
	    }
	    else
	    {
		GCTRACE(1)("GCwinsock2_init: Couldn't create worker thread, status = %d\n",
			   GetLastError());
		CloseHandle(GCwinsock2CompletionPort);
		return(FAIL);
	    }
	}

	Tptr->NumWorkerThreads = i;
    }

    return(OK);
}


/*
**  Name: GCwinsock2
**  Description:
**	Main entry point for the WinSock 2.2 protocol driver. This
** 	driver is essentially just a dispatcher -- it runs in the primary
**	GCC thread and mostly just allows the I/O completion port to handle
**	aynchronous operations (Windows NT platforms) or completion routines
**	to handle asynchronous operations (Windows 9x). It may also start a
**	listen thread if it is a LISTEN request.
**
**	The following functions are handled:
**	    GCC_OPEN	- call GCwinsock2_open
**	    GCC_LISTEN  - post a listen using GCwinsock2_listen
**	    GCC_CONNECT - satisfy a synch / asynch connect request
**	    GCC_SEND    - satisfy an asynch send request
**	    GCC_RECEIVE - satisfy an asynch recieve request
**	    GCC_DISCONN - satisfy an asynch disconnect request
**
**  History:
**	16-oct-2003 (somsa01)
**	    Created.
**	20-feb-2004 (somsa01)
**	    Updated to account for multiple listen ports per protocol.
**	08-oct-2004 (somsa01)
**	    If we're going to queue a completion routine execution, make
**	    sure we return OK from this function. Also, in GCC_DISCONNECT,
**	    make sure the pcb is valid.
**      27-feb-2005 (lunbr01)
**          Fix trace stmts following WSASend and WSARecv by initializing
**          dwBytes to 0 and correcting descriptions on WSASend trace.
*/

STATUS
GCwinsock2(i4 function_code, GCC_P_PLIST *parm_list)
{
    STATUS		status = OK;
    THREAD_EVENTS	*Tptr = NULL;
    GCC_LSN		*lsn = NULL;
    GCC_WINSOCK_DRIVER	*wsd;
    PCB			*pcb = (PCB *)parm_list->pcb;
    char		*port = NULL;
    char		*node = NULL;
    int			i, rval;
    DWORD		dwBytes=0;
    char		*proto = parm_list->pce->pce_pid;
    WS_DRIVER		*driver;
    char		port_name[36];
    PER_HANDLE_DATA	*lpPerHandleData = NULL;
    PER_IO_DATA  	*lpPerIoData = NULL;
    REQUEST_Q		*rq;
    struct addrinfo	hints, *lpAddrinfo;
    int  		tid;
    SECURITY_ATTRIBUTES sa;

    parm_list->generic_status = OK;
    CLEAR_ERR(&parm_list->system_status);

    /*
    ** Get entry for driver in Winsock protocol driver table 
    ** Tptr=THREAD_EVENTS=dcb (driver ctl block) and embedded struct wsd.
    */
    Tptr = (THREAD_EVENTS *)parm_list->pce->pce_dcb;
    wsd = &Tptr->wsd;

    /*
    ** Set error based on function code and determine whether we got a
    ** valid function.
    */
    switch (function_code)
    {
	case GCC_OPEN:
	    is_comm_svr = TRUE;
	    GCTRACE(2)("GCwinsock2 %s: GCC_OPEN\n", proto);
	    return GCwinsock2_open(parm_list);

	case GCC_LISTEN:
	    iimksec(&sa);

	    GCTRACE(2)("GCwinsock2 %s: GCC_LISTEN\n", proto);

	    /*
	    ** Use the proper listen port.
	    */
	    lsn = wsd->lsn;
	    while ( lsn &&
		STcompare(parm_list->pce->pce_port, lsn->pce_port) != 0)
	    {
	        lsn = lsn->lsn_next;
	    }

	    if (!lsn)
	    {
		SETWIN32ERR(&parm_list->system_status, status, ER_listen);
	        parm_list->generic_status = GC_LISTEN_FAIL;
	        status = FAIL;
	        goto complete;
	    }

	    if (lpfnAcceptEx)
		return GCwinsock2_listen(parm_list);
	    else
	    {
		if (lsn->hListenThread =
			CreateThread(&sa, GC_STACK_SIZE,
				     (LPTHREAD_START_ROUTINE)GCwinsock2_listen,
				     parm_list, NULL, &tid))
		{
		    return(OK);
		}

		status = GetLastError();
		SETWIN32ERR(&parm_list->system_status, status, ER_threadcreate);
		break;
	    }

	case GCC_CONNECT:

	    GCTRACE(2)("GCwinsock2 %s: GCC_CONNECT\n", proto);
	    port = parm_list->function_parms.connect.port_id;
	    node = parm_list->function_parms.connect.node_id;
            GCTRACE(2)("GCwinsock2 %s: GCC_CONNECT node=%s port=%s\n",
                       proto, node, port);

	    driver = (WS_DRIVER *)wsd->pce_driver;

	    /*
	    ** Allocate the protocol control block.
	    */
	    pcb = (PCB *) MEreqmem(0, sizeof(PCB), TRUE, &status);
	    parm_list->pcb = (char *)pcb;
	    if (pcb == NULL)
	    {
		SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
		parm_list->generic_status = GC_CONNECT_FAIL;
		break;
	    }

	    /*
	    ** Call the protocol-specific addressing function.
	    ** If an Ingres symbolic port was input, this routine
	    ** will convert it to a numeric port that can be used
	    ** in the network function calls.
	    */ 
	    if ((*driver->addr_func)(node, port, port_name) == FAIL)
	    {
		status = WSAGetLastError();
		SETWIN32ERR(&parm_list->system_status, status, ER_socket);
		parm_list->generic_status = GC_CONNECT_FAIL;
		break;
	    }

	    GCTRACE(2)( "GCwinsock2 %s: GCC_CONNECT %p connecting to %s port %s(%s)\n", 
		proto, parm_list, node, port, port_name );

	    /*
	    ** Set up criteria for getting list of IPv* addrs to connect to.
	    ** This will be used as the main input parm to getaddrinfo().
	    */
	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family   = wsd->sock_fam;
	    hints.ai_socktype = wsd->sock_type;
	    hints.ai_protocol = wsd->sock_proto;

	    /*
	    ** Get a list of qualifying addresses for the target hostname,
	    ** which may be a mix of IPv6 & IPv4, and may be more than 1
	    ** of each, particularly for IPv6.
	    */
	    status = getaddrinfo(node, port_name, &hints, &pcb->addrinfo_list);
	    if (status != 0)
	    {
	        status = WSAGetLastError();
	        GCTRACE(1)( "GCwinsock2 %s: GCC_CONNECT %p getaddrinfo() failed for node=%s port=%s(%s), status=%d\n",
			    proto, parm_list, node, port, port_name, status);
	        SETWIN32ERR(&parm_list->system_status, status, ER_getaddrinfo);
	        parm_list->generic_status = GC_CONNECT_FAIL;
	        break;
	    }

	    /*
	    ** Ensure at least one address returned.
	    */
	    if (pcb->addrinfo_list == NULL)
	    {
	        GCTRACE(1)( "GCwinsock2 %s: GCC_CONNECT %p getaddrinfo() returned no addresses for node=%s port=%s(%s)\n",
			    proto, parm_list, node, port, port_name);
	        SETWIN32ERR(&parm_list->system_status, status, ER_getaddrinfo);
	        parm_list->generic_status = GC_CONNECT_FAIL;
	        status = FAIL;
	        break;
	    }

	    if ((lpfnConnectEx) && (!is_win9x))  /* If using Compl Port IO */
	    {

		/*
		** Allocate the PER_HANDLE_DATA structure and initialize.
		*/
		lpPerHandleData = (PER_HANDLE_DATA *)MEreqmem(0, 
			sizeof(PER_HANDLE_DATA), TRUE, &status);
		if (lpPerHandleData == NULL)
		{
		    SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
		    parm_list->generic_status = GC_CONNECT_FAIL;
		    break;
		}
		pcb->lpPerHandleData = lpPerHandleData;

		/*
		** Allocate the PER_IO_DATA structure and initialize.
		*/
		lpPerIoData = (PER_IO_DATA *)MEreqmem(0, sizeof(PER_IO_DATA),
		  				      TRUE, &status);
		if (lpPerIoData == NULL)
		{
		    SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
		    parm_list->generic_status = GC_CONNECT_FAIL;
		    break;
		}

		lpPerIoData->OperationType = OP_CONNECT;
		lpPerIoData->parm_list = parm_list;
		lpPerIoData->block_mode = wsd->block_mode;

	    }  /* Endif ConnectEx() exists. */

	    /*
	    ** Walk through the list of addresses returned and
	    ** connect to each until one is successful.
	    ** NOTE that for certain types of errors, we bail
	    ** out of the loop immediately and return with an error.
	    */
	    pcb->sd = INVALID_SOCKET;
	    for (lpAddrinfo = pcb->addrinfo_list;
	         lpAddrinfo;
	         lpAddrinfo = lpAddrinfo->ai_next)
	    {
		pcb->lpAddrinfo = lpAddrinfo;

		if (GCwinsock2_connect (parm_list, (char *)lpPerIoData) == OK)
		    if (lpPerHandleData)  /* Async connect ... in progress */
		        return(OK);
		    else                  /* Sync connect ... connected */
		        goto complete;
                if (parm_list->generic_status == GC_CONNECT_FAIL)  /* fatal error */
                    goto complete;

	    }  /* end of loop through addresses */

	    if (!lpAddrinfo)  /* If exited because reached the end of addrs */
	    {
		SETWIN32ERR(&parm_list->system_status, status, ER_connect);
		parm_list->generic_status = GC_CONNECT_FAIL;
		GCTRACE(1)("GCwinsock2 %s: GCC_CONNECT, All IP addrs failed\n",
			    proto);
		goto complete;
	    }

	case GCC_SEND:
	    GCTRACE(2) ("GCwinsock2 %s: GCC_SEND\n", proto);

	    if (!pcb) goto complete;

	    if (wsd->block_mode)
	    {
		pcb->snd_bptr = parm_list->buffer_ptr;
		pcb->tot_snd = parm_list->buffer_lng;
	    }
	    else
	    {
		u_i2	tmp_i2;
		u_char	*cptr;

		/*
		** Handle 1st 2 bytes which are a msg length field.
		*/
		pcb->snd_bptr = parm_list->buffer_ptr - 2;
		tmp_i2 = (u_i2)(pcb->tot_snd = (i4)(parm_list->buffer_lng + 2));
		cptr = (u_char *)&tmp_i2;
		pcb->snd_bptr[1] = cptr[1];
		pcb->snd_bptr[0] = cptr[0];
	    }

	    /*
	    ** Allocate the PER_IO_DATA structure and initialize.
	    */
	    lpPerIoData = (PER_IO_DATA *)MEreqmem(0, sizeof(PER_IO_DATA),
						  TRUE, &status);
	    if (lpPerIoData == NULL)
	    {
		SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
		parm_list->generic_status = GC_SEND_FAIL;
		break;
	    }

	    lpPerIoData->OperationType = OP_SEND;
	    lpPerIoData->parm_list = parm_list;
	    lpPerIoData->block_mode = wsd->block_mode;
	    lpPerIoData->wbuf.buf = pcb->snd_bptr;
	    lpPerIoData->wbuf.len = pcb->tot_snd;

	    if (!is_win9x)
	    {
		InterlockedIncrement(&pcb->lpPerHandleData->AsyncIoRunning);
		rval = WSASend( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes, 0,
				&lpPerIoData->Overlapped, NULL );
	    }
	    else
	    {
		rval = WSASend( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes, 0,
				&lpPerIoData->Overlapped,
				GCwinsock2_AIO_Callback );
	    }

	    GCTRACE(2)( "GCwinsock2 %s: GCC_SEND, try send %d bytes, act send %d bytes, status=%d\n",
			proto, pcb->tot_snd, dwBytes, rval);
	    if (rval == SOCKET_ERROR)
	    {
		status = WSAGetLastError();
		if (status != WSA_IO_PENDING && status != OK)
		{
		    if (pcb->lpPerHandleData)
		    {
			InterlockedDecrement(
					&pcb->lpPerHandleData->AsyncIoRunning);
		    }

		    parm_list->generic_status = GC_SEND_FAIL;
		    SETWIN32ERR(&parm_list->system_status, status, ER_socket);
		    goto complete;
		}
	    }

	    return(OK);

	case GCC_RECEIVE:
	    GCTRACE(2) ("GCwinsock2 %s: GCC_RECEIVE\n", proto);

	    if (!pcb) goto complete;

	    if (wsd->block_mode)
	    {
		pcb->rcv_bptr = parm_list->buffer_ptr;
		pcb->tot_rcv = parm_list->buffer_lng;
	    }
	    else
	    {
		/*
		** We first want to get the 2 bytes that
		** tell us what the real message length is.
		*/
		pcb->rcv_bptr = pcb->recv_msg_len;
		pcb->tot_rcv = 2;
	    }

	    /*
	    ** Allocate the PER_IO_DATA structure and initialize.
	    */
	    lpPerIoData = (PER_IO_DATA *)MEreqmem(0, sizeof(PER_IO_DATA),
						  TRUE, &status);
	    if (lpPerIoData == NULL)
	    {
		SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
		parm_list->generic_status = GC_RECEIVE_FAIL;
		break;
	    }

	    lpPerIoData->OperationType = OP_RECV_MSG_LEN;
	    lpPerIoData->parm_list = parm_list;
	    lpPerIoData->block_mode = wsd->block_mode;
	    lpPerIoData->wbuf.buf = pcb->rcv_bptr;
	    lpPerIoData->wbuf.len = pcb->tot_rcv;

	    i = 0;
	    if (!is_win9x)
	    {
		InterlockedIncrement(&pcb->lpPerHandleData->AsyncIoRunning);
		rval = WSARecv( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes, &i,
				&lpPerIoData->Overlapped, NULL );
	    }
	    else
	    {
		rval = WSARecv( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes, &i,
				&lpPerIoData->Overlapped,
				GCwinsock2_AIO_Callback );
	    }

	    GCTRACE(4)( "GCwinsock2 %s: GCC_RECEIVE, want %d bytes got %d bytes\n",
			proto, pcb->tot_rcv, dwBytes );
	    if (rval == SOCKET_ERROR)
	    {
		status = WSAGetLastError();
		if (status != WSA_IO_PENDING && status != OK)
		{
		    if (pcb->lpPerHandleData)
		    {
			InterlockedDecrement(
					&pcb->lpPerHandleData->AsyncIoRunning);
		    }

		    parm_list->generic_status = GC_RECEIVE_FAIL;
		    SETWIN32ERR(&parm_list->system_status, status, ER_socket);
		    goto complete;
		}
	    }

	    return(OK);

	case GCC_DISCONNECT:
	    GCTRACE(2)("GCwinsock2 %s: GCC_DISCONNECT\n", proto);

	    if (!is_win9x && pcb)
	    {
		/*
		** Allocate the PER_IO_DATA structure and initialize.
		*/
		lpPerIoData = (PER_IO_DATA *)MEreqmem(0, sizeof(PER_IO_DATA),
						      TRUE, &status);
		if (lpPerIoData == NULL)
		{
		    SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
		    parm_list->generic_status = GC_DISCONNECT_FAIL;
		    break;
		}

		lpPerIoData->OperationType = OP_DISCONNECT;
		lpPerIoData->parm_list = parm_list;

		if (!PostQueuedCompletionStatus(GCwinsock2CompletionPort,
						0,
						(ULONG_PTR)pcb->lpPerHandleData,
						&lpPerIoData->Overlapped))
		{
		    status = GetLastError();
		    SETWIN32ERR(&parm_list->system_status, status, ER_close);
		    parm_list->generic_status = GC_DISCONNECT_FAIL;
		    break;
		}

		return(OK);
	    }
	    else
	    {
		if (pcb)
		{
		    closesocket(pcb->sd);

		    if (pcb->addrinfo_list)
			freeaddrinfo(pcb->addrinfo_list);

		    MEfree((PTR)pcb);
		}

		parm_list->pcb = NULL;
		break;
	    }

	default:
	    return(FAIL);
    }

complete:

    /*
    ** Drive the completion routine.
    */
    if ((rq = (REQUEST_Q *)MEreqmem(0, sizeof(*rq), TRUE, NULL)) != NULL)
    {
	status = OK;
	rq->plist = parm_list;

	/*
	** Get mutex for completion Q.
	*/
	WaitForSingleObject(hMutexGccCompleteQ, INFINITE);

	/*
	** Now insert the completed request into the completed Q.
	*/
	QUinsert(&rq->req_q, &IIGCc_proto_threads.completed_head);

	/*
	** Release mutex for completion Q.
	*/
	ReleaseMutex(hMutexGccCompleteQ);

	/*
	** Raise the completion event to wake up GCexec.
	*/
	if (!SetEvent(hAsyncEvents[GCC_COMPLETE]))
	{
	    status = GetLastError();
	    GCTRACE(1)("GCwinsock2, SetEvent error = %d\n", status);
	}
    }

    return(status);
}


/*
**  Name: GCwinsock2_open
**  Description:
**	Open the listen socket for WinSock 2.2. Called from GCwinsock2(). This
**	routine should only be called once at server startup.
**
**  History:
**	16-oct-2003 (somsa01)
**	    Created.
**	09-feb-2004 (somsa01)
**	    When working with an instance identifier as a port address, make
**	    sure we start with only the first two characters. The subport is
**	    calculated from within the driver's addr_func() function.
**	20-feb-2004 (somsa01)
**	    Updated to account for multiple listen ports per protocol.
**      15-Feb-2005 (lunbr01)  Bug 113903, Problem EDJDBC 97
**          Do not clear the 3rd char of the input port when passed in
**          as XX# format.  Some callers, such as the JDBC server, pass in
**          an explicit symbolic port to use.
**      28-Aug-2006 (lunbr01) Sir 116548
**          Add IPv6 support.  Add call to getaddrinfo() to get list of
**          IP addresses that we should listen on--may be an IPv4 &
**          0-n IPv6 addresses.
*/

STATUS
GCwinsock2_open(GCC_P_PLIST *parm_list)
{
    int			addr_reuse_on = 1;
    STATUS		status = OK;
    GCC_LSN		*lsn, *lsn_ptr;
    GCC_WINSOCK_DRIVER	*wsd;
    WS_DRIVER		*driver = (WS_DRIVER *)parm_list->pce->pce_driver;
    THREAD_EVENTS	*Tptr;
    char		*port  = parm_list->function_parms.open.port_id;
    char		port_id[GCC_L_PORT];
    int			i, num_port_tries;
#define			MAX_PORT_TRIES 100  /* if reach this, we're looping! */
    struct addrinfo	hints, *lpAddrinfo;
    struct sockaddr_storage ws_addr;
    int			ws_addr_len = sizeof(ws_addr);
    char        	hoststr[NI_MAXHOST], servstr[NI_MAXSERV];
    SECURITY_ATTRIBUTES	sa;

    parm_list->function_parms.open.lsn_port = NULL;

    /*
    ** See whether this is a "no listen" protocol.
    */
    if(parm_list->pce->pce_flags & PCT_NO_PORT)
	goto complete_open;

    Tptr = (THREAD_EVENTS *)parm_list->pce->pce_dcb;
    wsd = &Tptr->wsd;

    /*
    ** Create the GCC_LSN structure for this listen port.
    */
    lsn = (GCC_LSN *)MEreqmem(0, sizeof(GCC_LSN), TRUE, NULL);
    if (!lsn)
    {
	parm_list->generic_status = GC_OPEN_FAIL;
	SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
	status = FAIL;
	goto complete_open;
    }

    if ( (lsn->hListenEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
    {
	status = WSAGetLastError();
	parm_list->generic_status = GC_OPEN_FAIL;
	SETWIN32ERR(&parm_list->system_status, status, ER_create);
	goto complete_open;
    }

    lsn_ptr = wsd->lsn;
    if (!lsn_ptr)
	wsd->lsn = lsn;
    else
    {
	while (lsn_ptr->lsn_next)
	    lsn_ptr = lsn_ptr->lsn_next;
	lsn_ptr->lsn_next = lsn;
    }

    /*
    ** Set up criteria for getting list of IPv* addrs to listen on.
    ** This will be used as the main input parm to getaddrinfo().
    */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = wsd->sock_fam;
    hints.ai_socktype = wsd->sock_type;
    hints.ai_protocol = wsd->sock_proto;
    hints.ai_flags    = AI_PASSIVE;

    /*
    ** Determine port to listen on.
    */
    if (STcompare(port, "II"))
    {
	STcopy(port, port_id);
    }
    else
    {
	char	*value;

	NMgtAt("II_INSTALLATION", &value);

	if (value && *value)
	    STcopy(value, port_id);
	else if (*parm_list->pce->pce_port != EOS)
	    STcopy(parm_list->pce->pce_port, port_id);
	else
	    STcopy(port, port_id);
    }

    /*
    ** Continue attempting to bind to a unique port id until we are
    ** successfull, or until we have retried an unreasonable number 
    ** of times.  If the latter occurs, it is likely because a specific
    ** numeric port was requested, in which case it won't be incremented,
    ** and someone else is listening on it.
    */
    for (num_port_tries = 0;;num_port_tries++)
    {
	if (num_port_tries > MAX_PORT_TRIES)
	{
	    GCTRACE(1)( "GCwinsock2_open: %p error - MAX_PORT_TRIES exceeded (=%d)\n", 
			parm_list, num_port_tries);
	    parm_list->generic_status = GC_OPEN_FAIL;
	    SETWIN32ERR(&parm_list->system_status, status, ER_bind);
	    goto complete_open;
	}

	/*
	** Call the protocol-specific adressing function.
	** Pass NULL in for node which will be taken to mean the address is
	** local host.  On 2nd and subsequent calls, this routine will 
	** increment the port number by 1 if a symbolic port was requested.
	*/
	status = (*driver->addr_func)(NULL, port_id, lsn->port_name);
	if (status == FAIL)
	{
	    GCTRACE(1)( "GCwinsock2_open: %p addr_func() failed on port_id=%s, status=%d\n",
			parm_list, port_id, status);
	    parm_list->generic_status = GC_OPEN_FAIL;
	    SETWIN32ERR(&parm_list->system_status, WSAEADDRINUSE, ER_badadar);
	    goto complete_open;
	}
	GCTRACE(1)( "GCwinsock2_open: %p addr_func() - port=%s(%s)\n",
			parm_list, port_id, lsn->port_name);

	/*
	** If this is a "retry" attempt, there will already be an
	** addrinfo_list and associated structures from prior attempt; if so,
	** free up the memory allocated to them because we want to start 
	** with fresh, clean structures.  We could probably use the 
	** existing ones and just scan and replace the port field 
	** wherever needed, but why take a chance messing with these 
	** system structures, particularly when supporting different 
	** protocols (IPv4 and IPv6) and hence different addr structs.
	*/
	GCwinsock2_close_listen_sockets(lsn);

	/*
	** Get a list of qualifying addresses...may be a mix of IPv6 & IPv4.
	*/
	status = getaddrinfo(NULL, lsn->port_name, &hints, &lsn->addrinfo_list);
	if (status != 0)
	{
	    status = WSAGetLastError();
	    GCTRACE(1)( "GCwinsock2_open: %p getaddrinfo() failed on port=%s, status=%d\n",
			parm_list, lsn->port_name, status);
	    parm_list->generic_status = GC_OPEN_FAIL;
	    SETWIN32ERR(&parm_list->system_status, status, ER_getaddrinfo);
	    goto complete_open;
	}

	/*
	** Ensure at least one address returned.
	*/
	if (lsn->addrinfo_list == NULL)
	{
	    GCTRACE(1)( "GCwinsock2_open: %p getaddrinfo() returned no addresses for port=%s\n",
			parm_list, lsn->port_name);
	    parm_list->generic_status = GC_OPEN_FAIL;
	    SETWIN32ERR(&parm_list->system_status, status, ER_getaddrinfo);
	    status = FAIL;
	    goto complete_open;
	}

	/*
	** Count number of addresses returned.
	*/
	lsn->num_sockets = 0;
	for (lpAddrinfo = lsn->addrinfo_list; 
	     lpAddrinfo;
             lpAddrinfo = lpAddrinfo->ai_next)
	{
            lsn->num_sockets++;
	}
	GCTRACE(1)( "GCwinsock2_open: %p getaddrinfo() succeeded on port=%s, num addresses=%d\n",
			parm_list, lsn->port_name, lsn->num_sockets);

	/*
	** Allocate space for the listen sockets
	*/
	lsn->listen_sd = (SOCKET *)MEreqmem( 0, sizeof(SOCKET) * lsn->num_sockets,
						TRUE, NULL );
	if (lsn->listen_sd == NULL)
	{
	    parm_list->generic_status = GC_OPEN_FAIL;
	    SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
	    goto complete_open;
	}

	/*
	** Create, bind and listen on sockets for each address in the array.
	** If at least one is successful, then consider successful and 
	** start up.  NOTE that we may have to go through the loop 
	** several times until a unique port id is found; this is primarily
	** to handle multiple comm server instances(processes). 
	*/
	lsn->num_good_sockets = 0;
	for (i = 0, lpAddrinfo = lsn->addrinfo_list; 
	     i < lsn->num_sockets; 
	     i++, lpAddrinfo = lpAddrinfo->ai_next)
	{
	    /*
	    ** Create the socket.
	    */
	    lsn->listen_sd[i] = INVALID_SOCKET;
	    if ((lsn->listen_sd[i] = WSASocket(lpAddrinfo->ai_family, 
                                               lpAddrinfo->ai_socktype, 
                                               lpAddrinfo->ai_protocol,
					       NULL, 0,
					       WSA_FLAG_OVERLAPPED)
		) == INVALID_SOCKET)
	    {
		status = WSAGetLastError();
		parm_list->generic_status = GC_OPEN_FAIL;
		SETWIN32ERR(&parm_list->system_status, status, ER_socket);
		goto complete_open;
	    }

	    /*   Non-IPv* special check.
	    ** Not sure if following still needed (still support IPX?)
	    ** but leaving in "just in case".
	    */
	    if ((lpAddrinfo->ai_family != AF_INET) &&
	        (lpAddrinfo->ai_family != AF_INET6))
	    {
	        /* Set the socket options to allow address reuse. */
	        if ((status = setsockopt(lsn->listen_sd[i], SOL_SOCKET,
			         SO_REUSEADDR, (u_caddr_t)&addr_reuse_on,
			         sizeof(addr_reuse_on))) != 0)
	        {
		    status = WSAGetLastError();
		    parm_list->generic_status = GC_OPEN_FAIL;
		    SETWIN32ERR(&parm_list->system_status, status, ER_socket);
		    goto complete_open;
	        }
	    }  /*  End of Non-Internet Protocol special check  */

	    /*
	    ** Bind the socket to the address/port.
	    */
	    if (bind(lsn->listen_sd[i], 
		     lpAddrinfo->ai_addr, 
		     (int)lpAddrinfo->ai_addrlen) != 0) 
	    {
	        status = WSAGetLastError();
	        if (status == WSAEADDRINUSE)
	        {
		    GCTRACE(1)( "GCwinsock2_open: %p bind() port=%s already used...try next port\n", 
			parm_list, lsn->port_name);
		    break;
	        }
		GCTRACE(1)( "GCwinsock2_open: %p bind() port=%s failed, status=%d\n", 
		    parm_list, lsn->port_name, status);
	        parm_list->generic_status = GC_OPEN_FAIL;
	        SETWIN32ERR(&parm_list->system_status, status, ER_bind);
	        goto complete_open;
	    }

	    if (!is_win9x)
	    {
		/*
		** Associate listening socket to completion port.
		*/
		if (!CreateIoCompletionPort((HANDLE)lsn->listen_sd[i],
					    GCwinsock2CompletionPort,
					    (ULONG_PTR)0, 0))
		{
		    status = WSAGetLastError();
		    parm_list->generic_status = GC_OPEN_FAIL;
		    SETWIN32ERR(&parm_list->system_status, status, ER_createiocompport);
		    goto complete_open;
		}
    	    }

	    /*
 	    ** Grab the name, in case we used a port number of 0 and got a
 	    ** random address...
 	    */
	    if (getsockname(lsn->listen_sd[i],
	    		    (struct sockaddr *)&ws_addr,
	    		    &ws_addr_len) != 0) 
	    {
		status = WSAGetLastError();
		parm_list->generic_status = GC_OPEN_FAIL;
		SETWIN32ERR(&parm_list->system_status, status, ER_getsockname);
		goto complete_open;
	    }

	    /*
	    ** SPX specific setup 
	    ** This assumes not mixing other protocols in addrinfo list.
	    */
	    if (lpAddrinfo->ai_family == AF_IPX)
		GCwinsock2_open_setSPXport((SOCKADDR_IPX *)lpAddrinfo->ai_addr, lsn->port_name);

	    /*
	    ** OK, now listen...
	    */
	    if (listen(lsn->listen_sd[i], SOMAXCONN) != 0) 
	    {
		status = WSAGetLastError();
		GCTRACE(1)( "GCwinsock2_open: %p listen failed for port %s, status=%d\n", 
			parm_list, lsn->port_name, status );
		parm_list->generic_status = GC_OPEN_FAIL;
		SETWIN32ERR(&parm_list->system_status, status, ER_listen);
		goto complete_open;
	    } 

	    GCTRACE(1)( "GCwinsock2_open: %p listening at %s\n", parm_list,
		    lsn->port_name );
	    lsn->num_good_sockets++;

	    /*
	    ** Trace the address this socket is bound to.
	    ** ...but don't bother if proper tracing level not on.
	    */
	    if ((GCWINSOCK2_trace >= 1) &&
	        (getnameinfo(lpAddrinfo->ai_addr,
                             (socklen_t)lpAddrinfo->ai_addrlen,
                             hoststr,
                             NI_MAXHOST,
                             servstr,
                             NI_MAXSERV,
                             NI_NUMERICHOST | NI_NUMERICSERV
                            ) == 0))
	    {
	        GCTRACE(1)( "GCwinsock2_open: %p socket 0x%x bound to address %s and port %s\n",
            		parm_list, lsn->listen_sd[i], hoststr, servstr);
	    }

	}  /*** End of loop through the addresses ***/

	/*
	** If someone (another GCC process for instance) is already 
	** listening on our port, try again->port will be incremented by 1
	** by moving port to port_id and calling GCtcpip_addr() at loop start.
	*/
	if (status == WSAEADDRINUSE)
	{
	    status = OK;
	    STcopy(port, port_id);
	    continue;
	}

	/*
	** Did we get at least one good socket?  If not, error!
	*/
	if (lsn->num_good_sockets == 0)
	{
	    parm_list->generic_status = GC_OPEN_FAIL;
	    SETWIN32ERR(&parm_list->system_status, status, ER_socket);
	    goto complete_open;
	}

	/*
	** !!SUCCESS!!  We have a good listen on at least one of the sockets
	** in the address list...probably all of them.
	*/

	/*
	** Add more worker threads...
	** to avoid unlikely, but possible, tie-up of IO Completion Port
	** worker threads in a pending listen complete status (OP_ACCEPT
	** entered and waiting in WFSO on next Listen request).
	** Generally, most of the threads will be idle, and we only
	** create these threads once during initialization, so the
	** overhead is not significant.
	*/
	iimksec(&sa);
	for (i = 0; i < lsn->num_good_sockets; i++)
	{
	    HANDLE	ThreadHandle;
	    int		tid;

	    if (Tptr->NumWorkerThreads > MAX_WORKER_THREADS)
		break;

	    ThreadHandle = CreateThread(
			    &sa, GC_STACK_SIZE,
			    (LPTHREAD_START_ROUTINE)GCwinsock2_worker_thread,
			    GCwinsock2CompletionPort, 0, &tid);
	    if (ThreadHandle)
	    {
		GCTRACE(3)("GCwinsock2_open: Worker Thread [%d] tid = %d, Handle = %d\n",
			   Tptr->NumWorkerThreads, tid, ThreadHandle);
		Tptr->hWorkerThreads[Tptr->NumWorkerThreads] = ThreadHandle;
		Tptr->NumWorkerThreads++;
	    }
	    else
	    {
		GCTRACE(1)("GCwinsock2_open: Couldn't create worker thread, status = %d\n",
			   GetLastError());
		CloseHandle(GCwinsock2CompletionPort);
		return(FAIL);
	    }
	} /* End foreach good listen, add worker thread */

	break;

    }  /*** End of loop until unique port found ***/

    parm_list->generic_status = OK;
    STcopy(port_id, port);
    parm_list->function_parms.open.lsn_port = lsn->port_name;
    STcopy(port_id, parm_list->pce->pce_port);
    STcopy(port_id, lsn->pce_port);

complete_open:
    /*
    ** Drive the completion routine.
    */
    (*parm_list->compl_exit)(parm_list->compl_id);

    return(status);
}


/*
**  Name: GCwinsock2_open_setSPXport
**  Description:
**	This routine adjusts the SPX port.  The code was split out from
**	GCwinsock2_open for clarity and separation from other protocols.
**
**  History:
**	28-aug-2006 (lunbr01)
**	    Created.
*/

VOID
GCwinsock2_open_setSPXport(SOCKADDR_IPX *spx_sockaddr, char *port_name)
{

    i4	i, j;

    for (i = 0, j = 0; i < 4; i++)
    {
	i4  byt;

	byt = spx_sockaddr->sa_netnum[i] & 0xFF;
	if ((byt & 0x0F) <= 9)
	    port_name[j+1] = (char)((byt & 0x0F) + '0');
	else
	    port_name[j+1] = (char)((byt & 0x0F) - 10 + 'A');
	byt >>= 4;
	if (byt <= 9)
	    port_name[j] = (char)(byt + '0');
	else
	    port_name[j] = (char)(byt - 10 + 'A');
	j += 2;
    }

    port_name[j++] = '.';
    for (i = 0; i < 6; i++)
    {
	i4	byt;

	byt = spx_sockaddr->sa_nodenum[i] & 0xFF;
	if ((byt & 0x0F) <= 9)
	    port_name[j+1] = (char)((byt & 0x0F) + '0');
	else
	    port_name[j+1] = (char)((byt & 0x0F) - 10 + 'A');
	byt >>= 4;
	if (byt <= 9)
	    port_name[j] = (char)(byt + '0');
	else
	    port_name[j] = (char)(byt - 10 + 'A');
	j += 2;
    }

    port_name[j] = EOS;
}


/*
**  Name: GCwinsock2_listen
**  Description:
**	This is the listen routine for Winsock 2.2. It runs an
**	asyncronous / synchronous accept() on the listen socket. This
**	routine is invoked each time GCwinsock() gets a request to repost
**	the listen.
**
**  History:
**	28-oct-2003 (somsa01)
**	    Created.
**	20-feb-2004 (somsa01)
**	    Updated to account for multiple listen ports per protocol.
**      16-Aug-2005 (loera01) Bug 115050, Problem INGNET 179
**          Don't retrieve the hostname via gethostbyaddr() if the 
**          connection is local.  Instead, copy the local hostname 
**          from GChostname().
**      26-Jan-2006 (Ralph Loen) Bug 115671
**          Added GCTCPIP_log_rem_host to optionally disable invocation of
**          gethostbyaddr().
**      28-Aug-2006 (lunbr01) Sir 116548
**          Add IPv6 support.
*/

STATUS
GCwinsock2_listen(VOID *parms)
{
    GCC_P_PLIST		*parm_list = (GCC_P_PLIST *)parms;
    char		*proto = parm_list->pce->pce_pid;
    int			i, i_stop;
    SOCKET		sd;
    PCB			*pcb;
    int			status = OK;
    GCC_LSN		*lsn;
    GCC_WINSOCK_DRIVER	*wsd;
    THREAD_EVENTS	*Tptr;
    struct addrinfo	*lpAddrinfo;
    struct sockaddr_storage peer;
    int			peer_len = sizeof(peer);
    DWORD		dwBytes;
    PER_IO_DATA		*lpPerIoData;
    REQUEST_Q		*rq;

    GCTRACE(2)("GCwinsock2_listen %s: %p \n", proto, parm_list);

    Tptr = (THREAD_EVENTS *)parm_list->pce->pce_dcb;
    wsd = &Tptr->wsd;

    /*
    ** Use the proper listen port.
    */
    lsn = wsd->lsn;
    while (lsn && STcompare(parm_list->pce->pce_port, lsn->pce_port) != 0)
	lsn = lsn->lsn_next;

    if (!lsn)
    {
	SETWIN32ERR(&parm_list->system_status, status, ER_listen);
	parm_list->generic_status = GC_LISTEN_FAIL;
	goto complete_listen;
    }

    lsn->parm_list = parm_list; /* Save for use by IO worker thread(s) */

    /*
    ** Initialize some listen parameters to NULL.
    */
    parm_list->function_parms.listen.node_id = NULL;
    parm_list->function_parms.listen.lcl_addr = NULL;
    parm_list->function_parms.listen.rmt_addr = NULL;

    /* 
    ** Initialize peer connection status to zero (remote).
    */
    parm_list->options = 0;

    /*
    ** Allocate Protcol Control Block specific to this driver/port
    */
    if ((pcb = (PCB *)MEreqmem(0, sizeof(PCB), TRUE, &status)) == NULL)
    {
	SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
	parm_list->generic_status = GC_LISTEN_FAIL;
	goto complete_listen;
    }

    parm_list->pcb = (char *)pcb;  

    if (!lpfnAcceptEx) goto normal_accept;

  /* 
  ** Set up a listen for each addr in addrinfo on the first time in.
  ** Thereafter, only one address will need to have the network listen
  ** reinvoked each time -- the one that satisfied and posted complete
  ** the last GCC_LISTEN request for this port.
  ** The tricky part about Listen processing is that more than one 
  ** incoming network connect request could have occurred (max of one 
  ** per address), but only one of them can service (post) each
  ** input GCC_LISTEN request.
  */
  if (lsn->addrinfo_posted)  /* if not first time in */
  {     /* Process only the address last completed/posted */
    lpAddrinfo=lsn->addrinfo_posted;
    i = lsn->socket_ix_posted;
    i_stop = i+1;
  }
  else  /* Process all the addresses ... first time in */
  {
    lpAddrinfo=lsn->addrinfo_list;
    i = 0;
    i_stop = lsn->num_sockets;
  }
  for (;
       i < i_stop;
       i++, lpAddrinfo = lpAddrinfo->ai_next)
  {
    /*
    ** Allocate the PER_IO_DATA structure and initialize.
    */
    lpPerIoData = (PER_IO_DATA *)MEreqmem(0, sizeof(PER_IO_DATA), TRUE,
					  &status);
    if (lpPerIoData == NULL)
    {
	SETWIN32ERR(&parm_list->system_status, status, ER_alloc);
	parm_list->generic_status = GC_LISTEN_FAIL;
	goto complete_listen;
    }

    lpPerIoData->OperationType = OP_ACCEPT;
    lpPerIoData->parm_list = NULL;  /* parmlist may be gone when AcceptEx completes */
    lpPerIoData->block_mode = wsd->block_mode;
    lpPerIoData->listen_sd = lsn->listen_sd[i];
    lpPerIoData->lsn       = lsn;
    lpPerIoData->addrinfo_posted = lpAddrinfo;
    lpPerIoData->socket_ix_posted = i;

    /*
    ** Create the client socket for the accepted connection.
    */
    if ((lpPerIoData->client_sd = WSASocket(lpAddrinfo->ai_family, 
					    lpAddrinfo->ai_socktype,
					    lpAddrinfo->ai_protocol, 
					    NULL, 0, WSA_FLAG_OVERLAPPED)
	) == INVALID_SOCKET)
    {
	status = WSAGetLastError();
	parm_list->generic_status = GC_LISTEN_FAIL;
	SETWIN32ERR(&parm_list->system_status, status, ER_socket);
	goto complete_listen;
    }

    if (!lpfnAcceptEx(lpPerIoData->listen_sd, lpPerIoData->client_sd, 
		      lpPerIoData->AcceptExBuffer, 0,
		      sizeof(SOCKADDR_STORAGE) + 16,
		      sizeof(SOCKADDR_STORAGE) + 16, &dwBytes,
		      &lpPerIoData->Overlapped))
    {
	status = WSAGetLastError();
	if (status != WSA_IO_PENDING && status != OK)
	{
	    parm_list->generic_status = GC_LISTEN_FAIL;
	    SETWIN32ERR(&parm_list->system_status, status, ER_accept);
	    goto complete_listen;
	}
    }

    /*
    ** Do not run the completion routine, as the completion routine
    ** for the AcceptEx() call will do that for us when it is time.
    */

  } /* End of loop through listen addresses */

    /*
    ** The AcceptEx() on each Listen address has been issued.  These IOs
    ** will pend in the OS.  When a remote user connects, the IO will 
    ** complete and be given to an IO Completion Port worker thread, which 
    ** will process the incoming connection as much as possible, but then
    ** will wait on the Listen event which is set here.  NOTE that only one
    ** worker thread will be awakened by the event; he will then pick up
    ** the GCC_LISTEN parm_list saved in the lsn block and post the
    ** completion routine for GCC_LISTEN.  The reason we can afford
    ** for the worker thread to wait on this event is because the wait
    ** time should be minimal as LISTENs are immediately redispatched
    ** by the caller (GCC for instance).  If this was not true, it would
    ** tie up worker threads, which need to be kept free to process
    ** completed network IO.
    */
    if (!SetEvent(lsn->hListenEvent))
    {
	status = GetLastError();
	GCTRACE(1)("GCwinsock2_listen, SetEvent error = %d\n", status);
	parm_list->generic_status = GC_LISTEN_FAIL;
	SETWIN32ERR(&parm_list->system_status, status, ER_sevent);
	goto complete_listen;
    }

    return(OK);

normal_accept:
    /*
    ** If we are here, then we are in a spawned thread that handles
    ** the listen for this port; this code is only used if IO Completion
    ** ports and the AcceptEx function are not supported, which is the
    ** case on Win95/98 and some network service providers (SPX perhaps?).
    ** Using a normal accept() will block, so if we have multiple addresses
    ** for this port, then we need to spawn a subthread for each address,
    ** or don't allow more than one address.  For now, limit to one address
    ** since there is no apparent need for multiple addresses per port
    ** capabilities when normal accepts must be used.
    */

    /*
    ** Use only the first if more than one address being listened on.  
    ** If determined to be a requirement later, can enhance at that point.
    /*
    if (lsn->num_sockets > 1)
    {
	GCTRACE(1)( "GCwinsock2_listen %s: %p WARNING--Multiple listen addresses returned, only listening on first\n since Winsock AcceptEx() function unavailable.  port=%s, #addrs=%d\n",
		   proto, parm_list, port, lsn->num_sockets);
    }

    /*
    ** Accept connections on the listen socket.
    */
    sd = accept(lsn->listen_sd[0], (struct sockaddr *)&peer, &peer_len);
    if (sd == INVALID_SOCKET) 
    {
	status = WSAGetLastError();
	SETWIN32ERR(&parm_list->system_status, status, ER_accept);
	parm_list->generic_status = GC_LISTEN_FAIL;
	goto complete_listen;
    }

    pcb->sd = sd;

    GCwinsock2_listen_complete(parm_list, NULL);

complete_listen:
    /*
    ** Drive the completion routine.
    */
    if ((rq = (REQUEST_Q *)MEreqmem(0, sizeof(*rq), TRUE, NULL)) != NULL)
    {
	rq->plist = parm_list;

	/*
	** Get mutex for completion Q.
	*/
	WaitForSingleObject(hMutexGccCompleteQ, INFINITE);

	/*
	** Now insert the completed request into the completed Q.
	*/
	QUinsert(&rq->req_q, &IIGCc_proto_threads.completed_head);

	/*
	** Release mutex for completion Q.
	*/
	ReleaseMutex(hMutexGccCompleteQ);

	/*
	** Raise the completion event to wake up GCexec.
	*/
	if (!SetEvent(hAsyncEvents[GCC_COMPLETE]))
	{
	    status = GetLastError();
	    GCTRACE(1)("GCwinsock2_listen, SetEvent error = %d\n", status);
	}
    }

    return(status);
}


/*
**  Name: GCwinsock2_listen_complete
**  Description:
**	This is the listen completion routine for Winsock 2.2.
**	It is called when a socket has been accepted successfully
**	by ANY means: blocking accept or asyncronous (IO Completion Port).
**	This contains all the common code needed to finish setting up
**	the information related to an incoming connection.  Previously,
**	the same or similar code was in several places.
**
**  History:
**	19-sep-2006 (lunbr01)
**	    Created as part of IPv6 support to consolidate redundant code.
**      06-Feb-2007 (Ralph Loen) SIR 117590
**          Removed getnameinfo() and set the node_id field in the GCC
**          listen parms to NULL.
*/

VOID
GCwinsock2_listen_complete(GCC_P_PLIST *parm_list, char *AcceptExBuffer)
{
    PCB			*pcb = (PCB *)parm_list->pcb;
    STATUS		retval;
    char		hostname[NI_MAXHOST]={0};
    SOCKADDR		*lpLocalSockaddr=NULL, *lpRemoteSockaddr = NULL;
    int			LocalSockaddrLen  = sizeof(SOCKADDR_STORAGE);
    int			RemoteSockaddrLen = sizeof(SOCKADDR_STORAGE);
    int			i;

    if (AcceptExBuffer && lpfnGetAcceptExSockaddrs)
    {
	lpfnGetAcceptExSockaddrs(AcceptExBuffer, 0,
			         sizeof(SOCKADDR_STORAGE) + 16,
			         sizeof(SOCKADDR_STORAGE) + 16,
			         (LPSOCKADDR *)&lpLocalSockaddr,
			         &LocalSockaddrLen,
			         (LPSOCKADDR *)&lpRemoteSockaddr,
			         &RemoteSockaddrLen);
    }
    else
    {
	getsockname(pcb->sd, lpLocalSockaddr,  &LocalSockaddrLen);
	getpeername(pcb->sd, lpRemoteSockaddr, &RemoteSockaddrLen);
    }


    if (((struct sockaddr_in6 *)lpLocalSockaddr)->sin6_family == AF_INET6)
    {
        if( MEcmp( &((struct sockaddr_in6 *)(lpLocalSockaddr))->sin6_addr,
            &((struct sockaddr_in6 *)(lpRemoteSockaddr))->sin6_addr,
                sizeof(((struct sockaddr_in6 *)(lpLocalSockaddr))->sin6_addr)) 
                    == 0 )
            parm_list->options |= GCC_LOCAL_CONNECT;
    }
    else
    {
        if( MEcmp( &((struct sockaddr_in *)(lpLocalSockaddr))->sin_addr,
            &((struct sockaddr_in *)(lpRemoteSockaddr))->sin_addr,
                sizeof(((struct sockaddr_in *)(lpLocalSockaddr))->sin_addr)) 
                    == 0 )
            parm_list->options |= GCC_LOCAL_CONNECT;
    }

    GCTRACE(1)( "GCwinsock2_listen_complete, Connection is %s\n",
                parm_list->options & GCC_LOCAL_CONNECT ? "local" : "remote" );

    parm_list->function_parms.listen.node_id = NULL;

    i = 1;
    setsockopt(pcb->sd, SOL_SOCKET, SO_KEEPALIVE, (caddr_t)&i,
	       sizeof(i));

    /*
    ** Set socket blocking status: 0=blocking, 1=non blocking.
    */
    i = 1;
    if (ioctlsocket(pcb->sd, FIONBIO, &i) == SOCKET_ERROR)
    {
	retval = WSAGetLastError();
	SETWIN32ERR(&parm_list->system_status, retval, ER_ioctl);
	parm_list->generic_status = GC_LISTEN_FAIL;
	return;
    }
}


/*
**  Name: GCwinsock2_connect
**  Description:
**	This routine will set up and issue a connect request for
**	the requested target IP address (in addrinfo).
** 
**  History:
**	19-sep-2006 (lunbr01)
**	    Created as part of IPv6 support.
*/
STATUS
GCwinsock2_connect(GCC_P_PLIST *parm_list, char *inPerIoData)
{
    STATUS		status = OK;
    int			i, rval;
    char		*proto = parm_list->pce->pce_pid;
    char		*port = parm_list->function_parms.connect.port_id;
    char		*node = parm_list->function_parms.connect.node_id;
    PCB			*pcb = (PCB *)parm_list->pcb;
    SOCKET		sd;
    struct addrinfo	*lpAddrinfo = pcb->lpAddrinfo;
    PER_HANDLE_DATA 	*lpPerHandleData = pcb->lpPerHandleData;
    PER_IO_DATA		*lpPerIoData = (PER_IO_DATA *)inPerIoData;
    u_i2		port_num = ntohs( ((struct sockaddr_in *)(lpAddrinfo->ai_addr))->sin_port );
    struct sockaddr_storage ws_addr;
    int			ws_addr_len = sizeof(ws_addr);
    char                hoststr[NI_MAXHOST] = {0};

    /*
    ** If not first try, clean up from last iteration.
    ** The final iteration will get cleaned up in GCC_DISCONNECT.
    */
    if (pcb->sd != INVALID_SOCKET)
	closesocket(pcb->sd);

    /*
    ** Create the socket
    */
    if ((pcb->sd = WSASocket(lpAddrinfo->ai_family, 
                        lpAddrinfo->ai_socktype, 
                        lpAddrinfo->ai_protocol,
		        NULL, 0,
		        WSA_FLAG_OVERLAPPED)
	) == INVALID_SOCKET)
    {
	status = WSAGetLastError();
	SETWIN32ERR(&parm_list->system_status, status, ER_socket);
	parm_list->generic_status = GC_CONNECT_FAIL;
	return(FAIL);
    }

    sd = pcb->sd;

    /*
    ** Set socket blocking status.  0=blocking, 1=non blocking.
    */
    i = 1;
    if (ioctlsocket(sd, FIONBIO, &i) == SOCKET_ERROR)
    {
	status = WSAGetLastError();
	SETWIN32ERR(&parm_list->system_status, status, ER_ioctl);
	parm_list->generic_status = GC_CONNECT_FAIL;
	return(FAIL);
    }

    GCTRACE(2)( "GCwinsock2_connect %s: %p connecting to %s(%s) port %s(%d)\n", 
	proto, parm_list, node, GCwinsock2_display_IPaddr(lpAddrinfo,hoststr), port, port_num );
/* FIX: Add GCwinsock2_display_IPaddr() to other GCTRACE calls below if it works */

    if (!lpPerHandleData) goto normal_connect;

    /*
    ** Associate client socket to completion port.
    */
    if (!CreateIoCompletionPort((HANDLE)sd,
			        GCwinsock2CompletionPort,
			        (ULONG_PTR)lpPerHandleData, 0))
    {
	status = WSAGetLastError();
	SETWIN32ERR(&parm_list->system_status, status,
		    ER_createiocompport);
	parm_list->generic_status = GC_CONNECT_FAIL;
	return(FAIL);
    }

    /*
    ** Bind the socket.  This is required by ConnectEx(),
    ** but not normal connect().  
    ** Bind to port=0, addr=ADDR_ANY(=0x00000000); zeroing the
    ** contents of the sockaddr struct (regardless of protocol)
    ** accomplishes this without having to deal with the diff formats.
    */
    ZeroMemory(&ws_addr, sizeof(struct sockaddr_storage));
    ws_addr.ss_family = lpAddrinfo->ai_family;
    if (bind(sd, (struct sockaddr *)&ws_addr, ws_addr_len) != 0)
    {
	status = WSAGetLastError();
	SETWIN32ERR(&parm_list->system_status, status, ER_bind);
	parm_list->generic_status = GC_CONNECT_FAIL;
	GCTRACE(1)("GCwinsock2_connect %s: bind() error = %d\n",
		    proto, status);
	return(FAIL);
    }

    /*
    ** Connect to the remote target asynchronously.
    */
    if (!lpfnConnectEx(sd, lpAddrinfo->ai_addr, lpAddrinfo->ai_addrlen, NULL, 0,
			NULL, &lpPerIoData->Overlapped))
    {
	status = WSAGetLastError();
	if (status == WSA_IO_PENDING || status == OK)
	    return(OK);
	if ((status == WSAECONNREFUSED) ||
	    (status == WSAENETUNREACH)  ||
	    (status == WSAETIMEDOUT))
	    return(FAIL);
	SETWIN32ERR(&parm_list->system_status, status, ER_connect);
	parm_list->generic_status = GC_CONNECT_FAIL;
	GCTRACE(1)("GCwinsock2_connect %s: ConnectEx() error = %d\n",
		    proto, status);
	return(FAIL);
    }
    return(OK);

normal_connect:

    rval = connect(sd, lpAddrinfo->ai_addr, lpAddrinfo->ai_addrlen);
    GCTRACE(2)("GCwinsock2_connect %s: connect() rval = %d\n", proto, rval);

    if (rval == SOCKET_ERROR)
    {
	status = WSAGetLastError();
	if (status == WSAEWOULDBLOCK)
	{
	    fd_set		check_fds;
	    struct timeval	tm;

	    /*
	    ** State is completing. Use select() -- when socket
	    ** is writeable the connect has completed.
	    */
	    status = OK;
	    tm.tv_sec = GCWINSOCK2_timeout;
	    tm.tv_usec = 0;
	    FD_ZERO(&check_fds);
	    FD_SET(sd, &check_fds);

	    rval = select((int)1, NULL, &check_fds, NULL, &tm);
	    GCTRACE(4)("GCwinsock2_connect %s: select() rval = %d\n",
			proto, rval);
	    if (rval == SOCKET_ERROR)
	    {
		status = WSAGetLastError();
		if (status != WSAEINPROGRESS)
		{
	            GCTRACE(1)("GCwinsock2_connect %s: select() error = %d\n",
			        proto, status);
		    return(FAIL);
		}

		status = OK;
	    }
	    else if (rval == 0)
	    {
		/* Connection timed out */
	        GCTRACE(1)("GCwinsock2_connect %s: timeout after %d secs\n",
			    proto, tm.tv_sec);
		return(FAIL);
	    }
	}
	else  /* Not WSAEWOULDBLOCK status, real failure to connect */
	{
	    GCTRACE(1)("GCwinsock2_connect %s: connect() error = %d\n",
		    proto, status);
	    return(FAIL);
	}
    }  /* end if connect returned a SOCKET_ERROR */

    GCwinsock2_connect_complete(parm_list);

    return(OK);
}


/*
**  Name: GCwinsock2_connect_complete
**  Description:
**	This is the connect completion routine for Winsock 2.2.  It is
**	called when a socket has successfully connected to a remote node 
**	by ANY means: blocking connect or asyncronous (IO Completion Port).
**	This contains all the common code needed to finish setting up
**	the information related to an outgoing connection.
**
**  History:
**	19-sep-2006 (lunbr01)
**	    Created as part of IPv6 support to consolidate redundant code.
*/

VOID
GCwinsock2_connect_complete(GCC_P_PLIST *parm_list)
{
    PCB			*pcb = (PCB *)parm_list->pcb;
    struct addrinfo	*lpAddrinfo = pcb->lpAddrinfo;
    STATUS		retval;
    struct sockaddr_storage  ws_addr;
    int			ws_addr_len = sizeof(ws_addr);
    int			i;

    /*
    ** Set up the local and remote address information.
    */
    getsockname(pcb->sd, (struct sockaddr *)&ws_addr, &ws_addr_len);
    if (getnameinfo((struct sockaddr *)&ws_addr, ws_addr_len,
                    pcb->lcl_addr.node_id,
                    sizeof(pcb->lcl_addr.node_id),
                    pcb->lcl_addr.port_id,
                    sizeof(pcb->lcl_addr.port_id),
                    NI_NUMERICHOST | NI_NUMERICSERV) != 0)
    {
	retval = WSAGetLastError();
	GCTRACE(1)("GCwinsock2_connect_complete: %p getnameinfo(local) error = %d\n",
		   parm_list, retval);
	parm_list->generic_status = GC_CONNECT_FAIL;
	SETWIN32ERR(&parm_list->system_status, retval, ER_getnameinfo);
	return;
    }

    if (getnameinfo(lpAddrinfo->ai_addr, lpAddrinfo->ai_addrlen,
                    pcb->rmt_addr.node_id,
                    sizeof(pcb->rmt_addr.node_id),
                    pcb->rmt_addr.port_id,
                    sizeof(pcb->rmt_addr.port_id),
                    NI_NUMERICHOST | NI_NUMERICSERV) != 0)
    {
	retval = WSAGetLastError();
	GCTRACE(1)("GCwinsock2_connect_complete: %p getnameinfo(remote) error = %d\n",
		   parm_list, retval);
	parm_list->generic_status = GC_CONNECT_FAIL;
	SETWIN32ERR(&parm_list->system_status, retval, ER_getnameinfo);
	return;
    }

    i = 1;
    setsockopt(pcb->sd, SOL_SOCKET, SO_KEEPALIVE, (u_caddr_t)&i,
	       sizeof(i));

    return;

}


/*
**  Name: GCwinsock2_display_IPaddr
**  Description:
**	This routine will convert the IP address in the sockaddr
**	structure to a display format and return it as a string.
**	To avoid leaking memory, the 2nd input parm must specify
**	an area to place the display form of the IP address, which
**	is also then used as the return value of the function.
**	The output area must be large enough to fit the largest
**	value including string termination character (that is, 
**	length should be NI_MAXHOST or longer).
** 
**  History:
**	19-sep-2006 (lunbr01)
**	    Created as part of IPv6 support.
*/
char *
GCwinsock2_display_IPaddr(struct addrinfo *lpAddrinfo, char *IPaddr_out)
{
    if (getnameinfo(lpAddrinfo->ai_addr,
                    (socklen_t)lpAddrinfo->ai_addrlen,
                    IPaddr_out,
                    NI_MAXHOST,
                    NULL,
                    0,
                    NI_NUMERICHOST) != 0)
	STcopy("Bad IP Address", IPaddr_out);
    return(IPaddr_out);
}


/*
**  Name: GCwinsock2_exit
**  Description:
**	This function exists from the Winsock 2.2 protocol, cleaning up.
**
**  History:
**	30-oct-2003 (somsa01)
**	    Created.
**	20-feb-2004 (somsa01)
**	    Updated to account for multiple listen ports per protocol.
*/
VOID
GCwinsock2_exit()
{
    int			i = 1, j;
    GCC_WINSOCK_DRIVER	*wsd;
    THREAD_EVENTS	*Tptr;

    /*
    ** Have we already been called once?
    */
    if (Winsock2_exit_set)
	return;
    Winsock2_exit_set = TRUE;

    /*
    ** Close listen sockets.
    */
    if ((Tptr = GCwinsock2_get_thread(TCPIP_ID)) != NULL)
    {
	GCC_LSN	*lsn;

	wsd = &Tptr->wsd;
	lsn = wsd->lsn;
	while (lsn)
	{
	    GCwinsock2_close_listen_sockets(lsn);
	    if (lsn->hListenThread)
	    {
		TerminateThread(lsn->hListenThread, 0);
		CloseHandle(lsn->hListenThread);
	    }

	    lsn = lsn->lsn_next;
	}

	for (j = 0; j < Tptr->NumWorkerThreads; j++)
	{
	    TerminateThread(Tptr->hWorkerThreads[j], 0);
	    CloseHandle(Tptr->hWorkerThreads[j]);
	}

	if (GCwinsock2CompletionPort) CloseHandle(GCwinsock2CompletionPort);
    }

    WSACleanup();
}

/*
**  Name: GCwinsock_get_thread
**  Description:
**	Return pointer to appropriate entry in IIGCc_wsproto_tab.
**	This table is indexed the same as IIGCc_proto_threads.
**
**  History:
**	16-oct-2003 (somsa01)
**	    Created.
*/

THREAD_EVENTS *
GCwinsock2_get_thread(char *proto_name)
{
    int i;

    /*
    ** Go thru the the protocol threads event list and find the index
    ** of the winsock thread.  Set the Global Tptr for easy reference
    ** to the event q's for this protocols thread.
    */
    for (i = 0; i < IIGCc_proto_threads.no_threads; i++)
    {
	THREAD_EVENTS *p = &IIGCc_proto_threads.thread[i];

	if (!STcompare(proto_name, p->thread_name))
	    return p;
    }

    /*
    ** Failed
    */
    return NULL;
}


/*
**  Name: GCwinsock2_worker_thread
**  Description:
**	This routine services the completion port.
**
**  History:
**	29-oct-2003 (somsa01)
**	    Created.
**	08-oct-2004 (somsa01)
**	    Make sure lpPerHandleData is valid in OP_DISCONNECT.
**      23-feb-2005 (lunbr01)  bug 113686, prob INGNET 160
**          E_GC4810_TL_FSM_STATE errors occur occasionally in GCD due to 
**          SEND (or RECV) completion routine being called AFTER disconnect
**          compl rtn. Add "call completion routine" requests at END of
**          queue rather than beginning to force FIFO processing of
**          completions.
**      29-feb-2005 (lunbr01)  bug 113987, prob INGNET 165
**          Pending async RECEIVES occasionally do not post complete after
**          the socket is closed during disconnect, which resulted in the
**          disconnect looping forever waiting for the IO to complete.
**          Limit the number of times disconnect will wait.  Symptom
**          is that subsequent connect requests may hang.
**      14-jul-2004 (loera01) Bug 114852
**          In the OP_RECV_MSG_LEN operation, fetch the second byte of
**          the message length if only one byte was sent.
**      26-Jan-2006 (Ralph Loen) Bug 115671
**          Added GCTCPIP_log_rem_host to optionally disable invocation of
**          gethostbyaddr().
**      22-Jan-2007 (lunbr01) Bug 117523
**          Corrupted (invalid) incoming data with large length prefix 
**          can crash caller (eg, GCC or GCD).  Add check to ensure
**          caller's buffer length is not exceeded (ie, avoid buffer overrun).
*/

DWORD WINAPI
GCwinsock2_worker_thread(LPVOID CompletionPortID)
{
    HANDLE		CompletionPort;
    DWORD		bytes_transferred, i;
    LPOVERLAPPED	lpOverlapped;
    GCC_P_PLIST		*parm_list;
    GCC_LSN    		*lsn;
    STATUS		retval;
    DWORD		dwBytes;
    PCB			*pcb = NULL;
    PER_HANDLE_DATA	*lpPerHandleData;
    PER_IO_DATA		*lpPerIoData;
    REQUEST_Q		*rq;
    bool		DecrementIoCount;
    int                 numAsyncIoWaits;

    CompletionPort = (HANDLE)CompletionPortID;

    while(TRUE)
    {
	DecrementIoCount = TRUE;
	retval = GetQueuedCompletionStatus(CompletionPort, &bytes_transferred,
					   (PULONG_PTR)&lpPerHandleData,
					   &lpOverlapped, INFINITE);

	lpPerIoData = CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, Overlapped);
	parm_list = lpPerIoData->parm_list;
        if (parm_list)  /* No parm list OK for LISTEN -> OP_ACCEPT */
	{
	    pcb = (PCB *)parm_list->pcb;
	    if (!pcb) goto worker_thread_err_exit;
	}

	switch(lpPerIoData->OperationType)
	{
	    case OP_ACCEPT:
		DecrementIoCount = FALSE;
		lsn = lpPerIoData->lsn;

		/*
		** Don't start processing the incoming connection request
		** until we have exclusive access to a Listen request.
		** This shouldn't take long as caller will reissue listens
		** as fast as he can.
		** Could optimize this slightly more by moving WFSO further
		** down in processing, but then need to be sure not to 
		** use PCB before WFSO; this approach required fewer changes
		** and is more straightforward.
		** One problem with this approach is that, though unlikely,
		** it is possible to tie up all of the worker threads here.
		** One escape mechanism that could be, but wasn't, taken
		** for that possibility would be to set the timeout 
		** value to a reasonable value so we can bail out
		** of this pending listen completion and free up the 
		** worker thread for other activities (send, receive, etc). 
		** An alternative approach was taken, which was to start 
		** up enough extra worker threads to handle a listen pending 
		** completion on every address for every port being listened 
		** on, which may be overkill and wastes threads.  The only
		** performance impact is the extra virtual memory required
		** for each thread (default 1 MB stack heap).  The advantage
		** of the extra thread approach is that we would never
		** fail an incoming connection for a timeout, whose value
		** would be arbitrary.
		*/
		WaitForSingleObject(lsn->hListenEvent, INFINITE);
		parm_list = lsn->parm_list;
		pcb = (PCB *)parm_list->pcb;
		pcb->sd = lpPerIoData->client_sd;
		lsn->addrinfo_posted  = lpPerIoData->addrinfo_posted;
		lsn->socket_ix_posted = lpPerIoData->socket_ix_posted;

		if (!retval)
		{
		    retval = GetLastError();
		    if (retval == ERROR_OPERATION_ABORTED)
			goto worker_thread_err_exit;
		    SETWIN32ERR(&parm_list->system_status, retval, ER_accept);
		    parm_list->generic_status = GC_LISTEN_FAIL;
		    break;
		}

		setsockopt(pcb->sd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
			   (char *)&lpPerIoData->listen_sd,
			   sizeof(lpPerIoData->listen_sd));

		GCwinsock2_listen_complete(parm_list, lpPerIoData->AcceptExBuffer);
		if (parm_list->generic_status == GC_LISTEN_FAIL)
		    break;

		/*
		** Allocate the PER_HANDLE_DATA structure and initialize.
		*/
		lpPerHandleData = (PER_HANDLE_DATA *)MEreqmem(
							0,
							sizeof(PER_HANDLE_DATA),
							TRUE, &retval);
		if (lpPerHandleData == NULL)
		{
		    SETWIN32ERR(&parm_list->system_status, retval, ER_alloc);
		    parm_list->generic_status = GC_LISTEN_FAIL;
		    break;
		}

		pcb->lpPerHandleData = lpPerHandleData;

		/*
		** Associate accepted client socket to completion port.
		*/
		if (!CreateIoCompletionPort((HANDLE)pcb->sd,
					    GCwinsock2CompletionPort,
					    (ULONG_PTR)lpPerHandleData, 0))
		{
		    retval = WSAGetLastError();
		    parm_list->generic_status = GC_LISTEN_FAIL;
		    SETWIN32ERR(&parm_list->system_status, retval,
				ER_createiocompport);
		    break;
		}

		break;

	    case OP_CONNECT:
		DecrementIoCount = FALSE;
		if (!retval)
		{
		    retval = GetLastError();
		    if (retval == ERROR_OPERATION_ABORTED)
			goto worker_thread_err_exit;

op_connect_next_addr:
	            pcb->lpAddrinfo = pcb->lpAddrinfo->ai_next;
	            if (pcb->lpAddrinfo)   /* If more addresses, try next one */
		    {
		        if (GCwinsock2_connect (parm_list, (char *)lpPerIoData) == OK)
		            continue;  /* Async connect issued */
		        else           /* Async connect failed */
                            if (parm_list->generic_status == GC_CONNECT_FAIL)
		                break; /* fatal error */
		            else
		                goto op_connect_next_addr;
		    }
		    else  /* No more target IP addresses...connect failed */
		    {
		        SETWIN32ERR(&parm_list->system_status, retval, ER_connect);
		        parm_list->generic_status = GC_CONNECT_FAIL;
		        GCTRACE(1)("GCwinsock2_worker_thread %s: OP_CONNECT, All IP addrs failed\n",
			    parm_list->pce->pce_pid);
		        break;
		    }
		}

		setsockopt(pcb->sd, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT,
			   NULL, 0);

		GCwinsock2_connect_complete(parm_list);

		break;

	    case OP_SEND:
		if (!retval)
		{
		    retval = GetLastError();
		    if (retval == ERROR_OPERATION_ABORTED)
			goto worker_thread_err_exit;
		    parm_list->generic_status = GC_SEND_FAIL;
		    SETWIN32ERR(&parm_list->system_status, retval, ER_socket);
		    break;
		}

		if (!bytes_transferred)
		{
		    /* The session has been disconnected */
		    pcb->tot_snd = 0;
		    parm_list->generic_status = GC_SEND_FAIL;
		    break;
		}

		break;

	    case OP_RECV_MSG_LEN:
		GCTRACE(2)( "GCwinsock2_worker_thread %s: RECV'D, want %d bytes got %d bytes\n",
			    parm_list->pce->pce_pid, pcb->tot_rcv,
			    bytes_transferred );
		if (!retval)
		{
		    retval = GetLastError();
		    if (retval == ERROR_OPERATION_ABORTED)
			goto worker_thread_err_exit;
		    parm_list->generic_status = GC_RECEIVE_FAIL;
		    SETWIN32ERR(&parm_list->system_status, retval, ER_socket);
		    break;
		}

		if (!bytes_transferred)
		{
		    /* The session has been disconnected */
		    pcb->tot_rcv = 0;
		    parm_list->generic_status = GC_RECEIVE_FAIL;
		    break;
		}

                if (bytes_transferred != pcb->tot_rcv)
                {
                    /*
                    ** We need to issue another recv to get the rest of the
                    ** message.
                    */
                    pcb->tot_rcv -= bytes_transferred;
                    pcb->rcv_bptr += bytes_transferred;

                    ZeroMemory(&lpPerIoData->Overlapped, sizeof(OVERLAPPED));
                    lpPerIoData->OperationType = OP_RECV_MSG_LEN;
                    lpPerIoData->wbuf.buf = pcb->rcv_bptr;
                    lpPerIoData->wbuf.len = pcb->tot_rcv;
 
                    i = 0;
                    retval = WSARecv( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes,
                                      &i, &lpPerIoData->Overlapped, NULL );
                    GCTRACE(4)( "GCwinsock2_worker_thread %s: GCC_RECEIVE, want %d bytes got %d bytes\n",
                                parm_list->pce->pce_pid, pcb->tot_rcv,
                                dwBytes );
                    if (retval == SOCKET_ERROR)
                    {
                        retval = WSAGetLastError();
                        if (retval != WSA_IO_PENDING && retval != OK)
                        {
                            parm_list->generic_status = GC_RECEIVE_FAIL;
                            SETWIN32ERR(&parm_list->system_status, retval,
                                        ER_socket);
                            break;
                        }
                    }
                    continue;
                }

		/*
		** If this is a block mode protocol, once the
		** recv completes successfully, we should have
		** recv'd all the bytes we're going to so we're
		** done.
		*/
		if (lpPerIoData->block_mode)
		{
		    parm_list->buffer_lng = bytes_transferred;
		    break;
		}

		/*
		** This part only runs for non-block mode ....
		** We've gotten the complete message length,
		** now fill in some variables and get the message...
		** after ensuring incoming msg will fit in buffer.
		*/
		pcb->tot_rcv = ((u_char)pcb->recv_msg_len[1] << 8) +
		                (u_char)pcb->recv_msg_len[0] - 2;
		if (pcb->tot_rcv < 0 || pcb->tot_rcv > parm_list->buffer_lng)
		{
                    parm_list->generic_status = GC_RECEIVE_FAIL;
                    GCTRACE(1)( "GCwinsock2_worker_thread %s: GCC_RECEIVE failure - Invalid incoming message len = %d bytes, buffer = %d bytes\n",
		                parm_list->pce->pce_pid, 
		                pcb->tot_rcv, 
		                parm_list->buffer_lng);
                    break;
		}
		parm_list->buffer_lng  = pcb->tot_rcv;
		pcb->rcv_bptr = parm_list->buffer_ptr;

		ZeroMemory(&lpPerIoData->Overlapped, sizeof(OVERLAPPED));
		lpPerIoData->OperationType = OP_RECV;
		lpPerIoData->wbuf.buf = pcb->rcv_bptr;
		lpPerIoData->wbuf.len = pcb->tot_rcv;

		i = 0;
		retval = WSARecv( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes, &i,
				  &lpPerIoData->Overlapped, NULL );
		GCTRACE(4)( "GCwinsock2_worker_thread %s: GCC_RECEIVE, want %d bytes got %d bytes\n",
			    parm_list->pce->pce_pid, pcb->tot_rcv, dwBytes );
		if (retval == SOCKET_ERROR)
		{
		    retval = WSAGetLastError();
		    if (retval != WSA_IO_PENDING && retval != OK)
		    {
			parm_list->generic_status = GC_RECEIVE_FAIL;
			SETWIN32ERR(&parm_list->system_status, retval,
				    ER_socket);
			break;
		    }
		}

		continue;

	    case OP_RECV:
		GCTRACE(2)( "GCwinsock2_worker_thread %s: RECV'D, want %d bytes got %d bytes\n",
			    parm_list->pce->pce_pid, pcb->tot_rcv,
			    bytes_transferred );
		if (!retval)
		{
		    retval = GetLastError();
		    if (retval == ERROR_OPERATION_ABORTED)
			goto worker_thread_err_exit;
		    parm_list->generic_status = GC_RECEIVE_FAIL;
		    SETWIN32ERR(&parm_list->system_status, retval, ER_socket);
		    break;
		}

		if (!bytes_transferred)
		{
		    /* The session has been disconnected */
		    pcb->tot_rcv = 0;
		    parm_list->generic_status = GC_RECEIVE_FAIL;
		    break;
		}

		if (lpPerIoData->block_mode)
		{
		    parm_list->buffer_lng = bytes_transferred;
		    break;
		}

		if (bytes_transferred != pcb->tot_rcv)
		{
		    /*
		    ** We need to issue another recv to get the rest of the
		    ** message.
		    */
		    pcb->tot_rcv -= bytes_transferred;
		    pcb->rcv_bptr += bytes_transferred;

		    ZeroMemory(&lpPerIoData->Overlapped, sizeof(OVERLAPPED));
		    lpPerIoData->OperationType = OP_RECV;
		    lpPerIoData->wbuf.buf = pcb->rcv_bptr;
		    lpPerIoData->wbuf.len = pcb->tot_rcv;

		    i = 0;
		    retval = WSARecv( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes,
				      &i, &lpPerIoData->Overlapped, NULL );
		    GCTRACE(4)( "GCwinsock2_worker_thread %s: GCC_RECEIVE, want %d bytes got %d bytes\n",
				parm_list->pce->pce_pid, pcb->tot_rcv,
				dwBytes );
		    if (retval == SOCKET_ERROR)
		    {
			retval = WSAGetLastError();
			if (retval != WSA_IO_PENDING && retval != OK)
			{
			    parm_list->generic_status = GC_RECEIVE_FAIL;
			    SETWIN32ERR(&parm_list->system_status, retval,
					ER_socket);
			    break;
			}
		    }

		    continue;
		}

		break;

	    case OP_DISCONNECT:
		DecrementIoCount = FALSE;
		if (pcb)
		{
		    closesocket(pcb->sd);

		    /*
		    ** Wait for all outstanding I/O on this socket to finish.
		    */
		    if (pcb->lpPerHandleData)
		    {
			numAsyncIoWaits = 0;
			while ((pcb->lpPerHandleData->AsyncIoRunning > 0) &&
                               ( numAsyncIoWaits < MAX_DISCONNECT_ASYNC_WAITS))
			{
			    Sleep(100);
                            numAsyncIoWaits++;
			}

			MEfree((PTR)pcb->lpPerHandleData);
		    }

		    if (pcb->addrinfo_list)
			freeaddrinfo(pcb->addrinfo_list);

		    MEfree((PTR)pcb);
		}

		parm_list->pcb = NULL;
		break;

	    default:
		continue;
	}

	/*
	** Drive the completion routine.
	*/
	if ((rq = (REQUEST_Q *)MEreqmem(0, sizeof(*rq), TRUE, NULL)) != NULL)
	{
	    rq->plist = parm_list;

	    /*
	    ** Get mutex for completion Q.
	    */
	    WaitForSingleObject(hMutexGccCompleteQ, INFINITE);

	    /*
	    ** Now insert the completed request into the completed Q.
            ** Place at end to cause FIFO processing.
	    */
	    QUinsert(&rq->req_q, IIGCc_proto_threads.completed_head.q_prev);

	    /*
	    ** Release mutex for completion Q.
	    */
	    ReleaseMutex(hMutexGccCompleteQ);

	    /*
	    ** Raise the completion event to wake up GCexec.
	    */
	    if (!SetEvent(hAsyncEvents[GCC_COMPLETE]))
	    {
		retval = GetLastError();
		GCTRACE(1)("GCwinsock2_worker_thread, SetEvent error = %d\n",
			   retval);
	    }
	}

worker_thread_err_exit:
	if (DecrementIoCount)
	    InterlockedDecrement(&pcb->lpPerHandleData->AsyncIoRunning);
	MEfree((PTR)lpPerIoData);
    }
}


/*
**  Name: GCwinsock2_AIO_Callback
**  Description:
**	This callback function is executed upon Asynchronous I/O completion.
**	It is used only on Windows 9x machines where completion ports cannot
**	be used.
**
**  History:
**	29-oct-2003 (somsa01)
**	    Created.
**      14-jul-2004 (loera01) Bug 114852
**          In the OP_RECV_MSG_LEN operation, fetch the second byte of
**          the message length if only one byte was sent.
*/

VOID CALLBACK
GCwinsock2_AIO_Callback(DWORD dwError, DWORD BytesTransferred, WSAOVERLAPPED *lpOverlapped, DWORD dwFlags)
{
    PER_IO_DATA		*lpPerIoData;
    GCC_P_PLIST		*parm_list;
    THREAD_EVENTS	*Tptr;
    GCC_WINSOCK_DRIVER	*wsd;
    PCB			*pcb;
    STATUS		status;
    int			i;
    DWORD		dwBytes;
    REQUEST_Q		*rq;

    lpPerIoData = CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, Overlapped);
    parm_list = lpPerIoData->parm_list;
    pcb = (PCB *)parm_list->pcb;

    switch(lpPerIoData->OperationType)
    {
	case OP_SEND:
	    if (dwError != OK)
	    {
		status = WSAGetLastError();
		if (status == ERROR_OPERATION_ABORTED)
		    return;
		parm_list->generic_status = GC_SEND_FAIL;
		SETWIN32ERR(&parm_list->system_status, status, ER_socket);
	    }

	    if (!BytesTransferred)
	    {
		/* The session has been disconnected */
		pcb->tot_snd = 0;
		parm_list->generic_status = GC_SEND_FAIL;
	    }

	    break;

	case OP_RECV_MSG_LEN:
	    GCTRACE(2)( "GCwinsock2_AIO_Callback %s: RECV'D, want %d bytes got %d bytes\n",
			parm_list->pce->pce_pid, pcb->tot_rcv,
			BytesTransferred );
	    if (dwError != OK)
	    {
		status = WSAGetLastError();
		if (status == ERROR_OPERATION_ABORTED)
		    return;
		parm_list->generic_status = GC_RECEIVE_FAIL;
		SETWIN32ERR(&parm_list->system_status, status, ER_socket);
		break;
	    }

	    if (BytesTransferred != pcb->tot_rcv)
	    {
		/*
		** We need to issue another recv to get the rest of the
		** length.
		*/
		pcb->tot_rcv -= BytesTransferred;
		pcb->rcv_bptr += BytesTransferred;

		ZeroMemory(&lpPerIoData->Overlapped, sizeof(OVERLAPPED));
		lpPerIoData->OperationType = OP_RECV_MSG_LEN;
		lpPerIoData->wbuf.buf = pcb->rcv_bptr;
		lpPerIoData->wbuf.len = pcb->tot_rcv;

		i = 0;
		status = WSARecv( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes, &i,
				  &lpPerIoData->Overlapped,
				  GCwinsock2_AIO_Callback );
		GCTRACE(4)( "GCwinsock2_AIO_Callback %s: GCC_RECEIVE, want %d bytes got %d bytes\n",
			    parm_list->pce->pce_pid, pcb->tot_rcv, dwBytes );
		if (status == SOCKET_ERROR)
		{
		    status = WSAGetLastError();
		    if (status != WSA_IO_PENDING && status != OK)
		    {
			parm_list->generic_status = GC_RECEIVE_FAIL;
			SETWIN32ERR(&parm_list->system_status, status,
				    ER_socket);
			break;
		    }
		}

		return;
	    }

	    if (!BytesTransferred)
	    {
		/* The session has been disconnected */
		pcb->tot_rcv = 0;
		parm_list->generic_status = GC_RECEIVE_FAIL;
		break;
	    }

	    Tptr = (THREAD_EVENTS *)parm_list->pce->pce_dcb;
	    wsd = &Tptr->wsd;

	    /*
	    ** If this is a block mode protocol, once the
	    ** recv completes successfully, we should have
	    ** recv'd all the bytes we're going to so we're
	    ** done.
	    */
	    if (wsd->block_mode)
	    {
		parm_list->buffer_lng = BytesTransferred;
		break;
	    }

	    /*
	    ** This part only runs for non-block mode ....
	    ** We've gotten the complete message length,
	    ** now fill in some variables and get the message.
	    */
	    pcb->tot_rcv = parm_list->buffer_lng =
					((u_char)pcb->recv_msg_len[1] << 8) +
					(u_char)pcb->recv_msg_len[0] - 2;
	    pcb->rcv_bptr = parm_list->buffer_ptr;

	    ZeroMemory(&lpPerIoData->Overlapped, sizeof(OVERLAPPED));
	    lpPerIoData->OperationType = OP_RECV;
	    lpPerIoData->wbuf.buf = pcb->rcv_bptr;
	    lpPerIoData->wbuf.len = pcb->tot_rcv;

	    i = 0;
	    status = WSARecv( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes, &i,
			      &lpPerIoData->Overlapped,
			      GCwinsock2_AIO_Callback );
	    GCTRACE(4)( "GCwinsock2_AIO_Callback %s: GCC_RECEIVE, want %d bytes got %d bytes\n",
			parm_list->pce->pce_pid, pcb->tot_rcv, dwBytes );
	    if (status == SOCKET_ERROR)
	    {
		status = WSAGetLastError();
		if (status != WSA_IO_PENDING && status != OK)
		{
		    parm_list->generic_status = GC_RECEIVE_FAIL;
		    SETWIN32ERR(&parm_list->system_status, status, ER_socket);
		    break;
		}
	    }

	    return;

	case OP_RECV:
	    GCTRACE(2)( "GCwinsock2_AIO_Callback %s: RECV'D, want %d bytes got %d bytes\n",
			parm_list->pce->pce_pid, pcb->tot_rcv,
			BytesTransferred );
	    if (dwError != OK)
	    {
		status = WSAGetLastError();
		if (status == ERROR_OPERATION_ABORTED)
		    return;
		parm_list->generic_status = GC_RECEIVE_FAIL;
		SETWIN32ERR(&parm_list->system_status, status, ER_socket);
		break;
	    }

	    if (!BytesTransferred)
	    {
		/* The session has been disconnected */
		pcb->tot_rcv = 0;
		parm_list->generic_status = GC_RECEIVE_FAIL;
		break;
	    }

	    Tptr = (THREAD_EVENTS *)parm_list->pce->pce_dcb;
	    wsd = &Tptr->wsd;

	    if (wsd->block_mode)
	    {
		parm_list->buffer_lng = BytesTransferred;
		break;
	    }

	    if (BytesTransferred != pcb->tot_rcv)
	    {
		/*
		** We need to issue another recv to get the rest of the
		** message.
		*/
		pcb->tot_rcv -= BytesTransferred;
		pcb->rcv_bptr += BytesTransferred;

		ZeroMemory(&lpPerIoData->Overlapped, sizeof(OVERLAPPED));
		lpPerIoData->OperationType = OP_RECV;
		lpPerIoData->wbuf.buf = pcb->rcv_bptr;
		lpPerIoData->wbuf.len = pcb->tot_rcv;

		i = 0;
		status = WSARecv( pcb->sd, &lpPerIoData->wbuf, 1, &dwBytes, &i,
				  &lpPerIoData->Overlapped,
				  GCwinsock2_AIO_Callback );
		GCTRACE(4)( "GCwinsock2_AIO_Callback %s: GCC_RECEIVE, want %d bytes got %d bytes\n",
			    parm_list->pce->pce_pid, pcb->tot_rcv, dwBytes );
		if (status == SOCKET_ERROR)
		{
		    status = WSAGetLastError();
		    if (status != WSA_IO_PENDING && status != OK)
		    {
			parm_list->generic_status = GC_RECEIVE_FAIL;
			SETWIN32ERR(&parm_list->system_status, status,
				    ER_socket);
			break;
		    }
		}

		return;
	    }

	    break;

	default:
	    return;
    }

    /*
    ** Drive the completion routine.
    */
    if ((rq = (REQUEST_Q *)MEreqmem(0, sizeof(*rq), TRUE, NULL)) != NULL)
    {
	rq->plist = parm_list;

	/*
	** Get mutex for completion Q.
	*/
	WaitForSingleObject(hMutexGccCompleteQ, INFINITE);

	/*
	** Now insert the completed request into the completed Q.
	*/
	QUinsert(&rq->req_q, &IIGCc_proto_threads.completed_head);

	/*
	** Release mutex for completion Q.
	*/
	ReleaseMutex(hMutexGccCompleteQ);

	/*
	** Raise the completion event to wake up GCexec.
	*/
	if (!SetEvent(hAsyncEvents[GCC_COMPLETE]))
	{
	    status = GetLastError();
	    GCTRACE(1)( "GCwinsock2_AIO_Callback, SetEvent error = %d\n",
			status );
	}
    }

    MEfree((PTR)lpPerIoData);
}


/*
**  Name: GCwinsock2_close_listen_sockets
**  Description:
**	This function closes all of the listen sockets and frees 
**	associated memory.
**
**  History:
**      28-Aug-2006 (lunbr01) Sir 116548
**          Add IPv6 support.
*/
VOID
GCwinsock2_close_listen_sockets(GCC_LSN *lsn)
{
    int 	i;
    if (lsn->addrinfo_list)
    {
	freeaddrinfo(lsn->addrinfo_list);
	lsn->addrinfo_list = NULL;
	if (lsn->listen_sd)
	{
	    for (i=0; i < lsn->num_sockets; i++)  /* close open sockets */
	    {
	        if (lsn->listen_sd[i] != INVALID_SOCKET)
	        {
		    setsockopt(lsn->listen_sd[i], SOL_SOCKET, 
			       SO_DONTLINGER,
			       (u_caddr_t)&i, sizeof(i));
	            closesocket(lsn->listen_sd[i]);
		    GCTRACE(1)( "GCwinsock2_close_listen_sockets: socket 0x%x closed\n",
            		lsn->listen_sd[i]);
	        }
	    }
	    lsn->num_sockets = 0;
	    MEfree((PTR)lsn->listen_sd);
	    lsn->listen_sd = NULL;
	}
    }
}


/*
**  Name: GCdns_hostname()
**
**  Description:
**
**      GCdns_hostname() -- get fully-qualified host name
**
**      This function differs from GChostname() and PMhost() in that it
**      fetches the true, fully qualified network name of the local host--
**      provided that such a name exists.  If a fully qualified network
**      name cannot be found, an empty string is returned.  If a fully 
**      qualified network name is found, but the output buffer is too 
**      small, the host name is truncated and an error is returned.
**
** Inputs:
**      hostlen         Byte length of host name buffer.
**      ourhost         Allocated buffer at least "buflen" in size.
**
** Outputs:
**      ourhost         Fully qualified host name - null terminated string.
**
** Returns:
**                      OK
**                      GC_NO_HOST
**                      GC_HOST_TRUNCATED
**                      GC_INVALID_ARGS
** Exceptions:
**                      N/A
**
**  History:
**    03-Aug-2007 (Ralph Loen) SIR 118898
**      Created.
*/

STATUS GCdns_hostname( char *ourhost, i4 hostlen )
{
    struct hostent *hp, *hp1;
    struct addrinfo *result=NULL;
    int err;
    size_t size;
    struct sockaddr *s;
    char hostname[GCC_L_NODE+1]="\0";
    u_i4 i = 0;
    char *ipaddr;
    int addr;
    STATUS status = OK;
    i2 len;

    /*
    ** Evaluate input arguments.
    */
    
    if ((ourhost == NULL) || (hostlen <= 0) )
    {
        status = GC_INVALID_ARGS;
        goto exit_routine; 
    }

    ourhost[0] = '\0';

    if (!WSAStartup_called)
        status = GCwinsock2_startup_WSA();

    /*
    ** gethostname() is the most straightforward means of getting
    ** the host name.
    */
    
    hostname[0] = '\0';
    if (gethostname(hostname, GCC_L_NODE))
        goto deprecated_funcs; 

    /*
    ** Sometimes gethostname() returns the fully qualified host name, so
    ** there is nothing more required.
    */
    if (STindex(hostname, ".", 0))
    {
        len = STlength(hostname);
        if ((len > hostlen-1) || (len > GCC_L_NODE))
        {
            STlcopy(hostname, ourhost, hostlen-1);
            status = GC_HOST_TRUNCATED;
        }
        else
            STcopy(hostname, ourhost);
        goto exit_routine;
    }

    /*
    ** Try getaddrinfo() and getnameinfo() first.
    */
    if (err = getaddrinfo(hostname, NULL, NULL, &result))
        goto deprecated_funcs;

    for( i=0; result; result = result->ai_next, i++ )
    {
        if (result->ai_canonname)
        {
            if (STindex(result->ai_canonname,".",0))
            {
                len = STlength(result->ai_canonname);
                if ((len > hostlen-1) || (len > GCC_L_NODE))
                {
                    STlcopy(result->ai_canonname, ourhost, hostlen-1);
                    status = GC_HOST_TRUNCATED;
                }
                else
                    STcopy (result->ai_canonname, ourhost);
                goto exit_routine;
            }
        }
        s = result->ai_addr;
        size = result->ai_addrlen;
        err = getnameinfo(s, 
            size,
            hostname, sizeof(hostname),         
            NULL, 
            0, 
            0);

        if (!err)
        {
            if (STindex(hostname, ".", 0))
            {
                len = STlength(hostname);
                if ((len > hostlen-1) || (len > GCC_L_NODE))
                {
                    STlcopy(hostname, ourhost, hostlen-1);
                    status = GC_HOST_TRUNCATED;
                }
                else
                    STcopy(hostname, ourhost);
                goto exit_routine;
            }
        }
    } /* for( i=0; result; result = result->ai_next, i++ ) */

deprecated_funcs:
    /*
    ** If the preferred approach, using getnameinfo() and getaddrinfo(),
    ** doesn't produce a fully qualified host name, fall back on
    ** the deprecated gethostname() and gethostbyname() functions.
    */        
    hp  = gethostbyname(hostname);
    if (hp != NULL) 
    {
        if (STindex(hp->h_name, ".", 0))
        {
            len = STlength(hp->h_name);
            if ((len > hostlen-1) || (len > GCC_L_NODE))
            {
                STlcopy(hp->h_name, ourhost, hostlen-1);
                status = GC_HOST_TRUNCATED;
            }
            else
                STcopy(hp->h_name, ourhost);
            goto exit_routine;
        }
        /*
        ** If the result of gethostbyname() didn't return anything useful,
        ** search through the address list and see if something turns up
        ** from gethostbyaddr().
        */
        i = 0;
        while ( hp->h_addr_list[i] != NULL)
        {
            ipaddr = inet_ntoa( *( struct in_addr*)( hp-> h_addr_list[i])); 
            if (ipaddr)
            {
                addr = inet_addr(ipaddr);
                hp1 = gethostbyaddr((const char *)&addr, sizeof(struct in_addr),
                    AF_INET);
                if (hp1)
                {
                    if (STindex(hp1->h_name, ".", 0))
                    {
                        len = STlength(hp1->h_name);
                        if ((len > hostlen-1) || (len > GCC_L_NODE) )
                        {
                            STlcopy(hp1->h_name, ourhost, hostlen-1);
                            status = GC_HOST_TRUNCATED;
                        }
                        else
                            STcopy(hp1->h_name, ourhost);
                        goto exit_routine;
                    } 
                }
            } /* if (ipaddr) */              
           i++;
        } /* while ( hp->h_addr_list[i] != NULL) */
    } /* if (hp != NULL) */

exit_routine:
    if (result)
        freeaddrinfo(result);
    if (!STlength(ourhost))
        status = GC_NO_HOST;
    else
        CVlower(ourhost);
    return status;
}
STATUS GCwinsock2_startup_WSA()
{
    WORD	version;
    int	err;

    version = MAKEWORD(2,2);
    err = WSAStartup(version, &startup_data);
    if (err != 0)
    {
        GCTRACE(1)("GCwinsock2_startup_WSA: Couldn't find usable WinSock DLL, status = %d\n", err);
        return(FAIL);
    }
    
    /*
    ** Confirm that the WinSock DLL supports 2.2.
    ** Note that if the DLL supports versions greater
    ** than 2.2 in addition to 2.2, it will still return
    ** 2.2 in wVersion since that is the version we
    ** requested.
    */
    if (LOBYTE(startup_data.wVersion) != 2 ||
        HIBYTE(startup_data.wVersion) != 2)
    {
        GCTRACE(1)("GCwinsock2_starup_WSA: Couldn't find usable WinSock DLL\n");
        WSACleanup();
        return(FAIL);
    }
    
    WSAStartup_called = TRUE;
    return (OK);
}
