/*
** Copyright (c) 2004 Ingres Corporation
*/

#include <compat.h>
#include <gl.h>
#include <cs.h>
#include <pc.h>
#include <er.h>
#include <si.h>
#include <st.h>
#include <cv.h>
#include <id.h>
#include <pm.h>

/*
** eroptlog.c - echo startup options into the errlog.log
**
** History:
**	1-Sep-93 (seiwald)
**	    Written for CS option revamp.
**      1-Mar-95 (liibi01)
**          Change the format for node name from %-6.6s to %-8.8s.
**	01-oct-1997 (nanpr01)
**	    Correct Bin Li's fix to change the host name to 8 chars.
**      21-apr-1999 (hanch04)
**        Replace STrindex with STrchr
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	15-feb-2001 (somsa01)
**	    Adjusted print header for LP64 machines.
**	29-jun-2001 (devjo01)
**	    Add support for node nicknames.
**	20-Jul-2004 (lakvi01)
**		SIR #112703, cleaned-up warnings.
**      26-Jul-2004 (lakvi01)
**          Backed-out the above change to keep the open-source stable.
**          Will be revisited and submitted at a later date. 
**	21-Mar-2005 (mutma03)
**	    Cleaned up "node name" to nickname translation code.
*/

/*
** Name: ERoptlog() - echo an option into the errlog.log
**
** Description:
**	ERoptlog() echoes a startup option and value into the errlog.log, trying
**	to mimic the format produced by ule_format() (and others).  ERoptlog()
**	does minimal error checking: if ERsend() fails, the messages are simply
**	not logged.
**
**	The option name may be in the format expected by PMget(), with !'s and
**	.'s.  ERoptlog() echoes only the component after the last '.'.
**
** Inputs:
**	char *option	The name of the option; it will be truncated in the log
**			if longer than 32 characters.
**	char *value	The value of the option; it will be truncated in the log
**			if longer than 64 characters.
**
** Outputs:
**	None
**
** Returns:
**	VOID
**
** Side Effects:
**	Calls ERsend() to write to the errlog.log file.
**
** History:
**	1-Sep-93 (seiwald)
**	    Written.
**	21-Mar-05 (mutma03)
**	    cleanup code for translating node name to nickname for clusters.
*/

VOID
ERoptlog( char *option, char *value )
{
#ifdef LP64
	static char msg_header[ 115 ];	/* size: see below */
#else
	static char msg_header[ 107 ];	/* size: see below */
#endif

	char buf[ ER_MAX_LEN ];
	CL_ERR_DESC cl_err_code;
	char *opttail;

	/* Strip any leading !.$. crud */

	if( ( opttail = STrchr( option, '.' ) ) )
	    option = opttail + 1;

        /* Format the message header */

	if( !msg_header[0] )
	{
	    CS_SID	user_pid;
	    char	user_name[33];
	    char	*user_name_ptr = user_name;
	    char	*nick_name;
	    char	host_name[128];/*qwikfix: must be CX_MAX_NODE_NAME_LEN*/
	    i4      	msg_language = 0;
	    char	msg_text[64];
	    i4		msg_length;
	    STATUS	status;

	    PCpid( &user_pid );
	    IDname( &user_name_ptr );
	    ERlangcode( (char *)NULL, &msg_language );
	    CXnode_name(host_name);
	    CXnickname_from_node_name(host_name,host_name);
	    nick_name = host_name;

	    if( (status = ERslookup( 
		    E_CS0030_CS_PARAM,
		    (CL_ERR_DESC *)0,
		    ER_TIMESTAMP, 
		    (char *)NULL,
		    msg_text, 
		    sizeof( msg_text ),
		    msg_language,
		    &msg_length, 
		    &cl_err_code, 
		    0, 
		    (ER_ARGUMENT *)0 ) ) != OK )
	    {
		*msg_text = '\0';
	    }

#ifdef LP64
	    /* Sizeof msg_header = 8 + 3 + 18 + 2 + 16 + 3 + 64 + 1 = 115 */
	    STprintf( msg_header, "%-8.8s::[%-18.18s, %016.16x]: %-.64s", 
		    nick_name, user_name, user_pid, msg_text );
#else
	    /* Sizeof msg_header = 8 + 3 + 18 + 2 + 8 + 3 + 64 + 1 = 107 */
	    STprintf( msg_header, "%-8.8s::[%-18.18s, %08.8x]: %-.64s", 
		    nick_name, user_name, user_pid, msg_text );
#endif
	}

	/* Assert: sizeof( msg_header ) + 64 + 32 + 3 + 1 <= ER_MAX_LEN */

	STprintf( buf, "%s%-.32s = %-.64s", msg_header, option, value );

	ERsend( ER_ERROR_MSG, buf, (i4)STlength( buf ), &cl_err_code);
}
