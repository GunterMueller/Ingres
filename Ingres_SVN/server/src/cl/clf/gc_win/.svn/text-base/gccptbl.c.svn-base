#include	<compat.h>
#include	<winsock.h>
#include	<clconfig.h>
#include	<gcccl.h>
#include	<qu.h>
#include	"gcclocal.h"

/*} 
** Protocol setup table
**
**	char		pce_pid[GCC_L_PROTOCOL];    Protocol ID
**	char		pce_port[GCC_L_PORT];	    Port ID
**	STATUS		(*pce_protocol_rtne)();	    Protocol handler
**	longnat		pce_pkt_sz;		    Packet size
**	longnat		pce_exp_pkt_sz;		    Exp. Packet size
**	u_nat		pce_options;		    Protocol options
**	u_nat		pce_flags;		    flags
**	PTR             pce_private;                handed to prot driver
**
LIBRARY = IMPCOMPATLIBDATA
**
** History:
**	03-Nov-93 (edg)
**	    Taken from UNIX version for use with NT.
**	12-Nov-93 (edg)
**	    Added lan manager netbios to table.
**	01-Dec-93 (edg)
**	    Added Novel SPX.
**	10-jan-94 (edg)
**	    Added include <qu.h>
**      12-Dec-95 (fanra01)
**          Extracted data for DLLs on windows NT.
**	24-apr-96 (tutto01)
**	    Added support for DECnet.
**	09-may-1996 (canor01)
**	    Define IIGCc_prot_exit array of protocol exit functions.
**	01-jul-97 (somsa01)
**	    Removed DEcnet stuff. We do not need this anymore, since DECnet is
**	    contained in its own DLL, and it is loaded in at ingstart time.
**	29-sep-2003 (somsa01)
**	    Added entries for tcp_ip protocol driver.
**	20-Jul-2004 (lakvi01)
**		SIR #112703, cleaned-up warnings.
**      26-Jul-2004 (lakvi01)
**          Backed-out the above change to keep the open-source stable.
**          Will be revisited and submitted at a later date. 
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to have this file as part of IMPCOMPATLIBDATA
*/

GLOBALREF	GCC_PCT		IIGCc_gcc_pct;

GLOBALDEF	STATUS		(*IIGCc_prot_init[8])() =
{
	GCwinsock_init,
	GCwinsock2_init,
	GClanman_init,
	GCwinsock_init,
	NULL,
	NULL,
	NULL,
	NULL
};

GLOBALDEF	STATUS		(*IIGCc_prot_exit[8])() =
{
	GCwinsock_exit,
	GCwinsock2_exit,
	GClanman_exit,
	GCwinsock_exit,
	NULL,
	NULL,
	NULL,
	NULL
};
