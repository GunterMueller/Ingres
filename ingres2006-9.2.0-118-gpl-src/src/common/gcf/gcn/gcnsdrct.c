/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>

#include    <gc.h>
#include    <mh.h>
#include    <me.h>
#include    <qu.h>
#include    <si.h>
#include    <st.h>
#include    <tr.h>
#include    <tm.h>

#include    <sl.h>
#include    <iicommon.h>
#include    <gca.h>
#include    <gcaint.h>
#include    <gcn.h>
#include    <gcnint.h>
#include    <gcs.h>
#include    <gcu.h>
#include    <gcaimsg.h>

/*
** Name: gcnsdrct.c
**
** Description:
**	Routines to facilitate direct client connections to remote
**	systems bypassing Ingres/Net.  The local Name Server connects
**	to the remote Name Server using Ingres/Net to determine if
**	the two installations are sufficiently compatible for direct
**	connections (homogeneous, GCA version), resolve the address
**	of the remote server, and authenticate the client on the
**	remote system.  If everything is OK, the client resolve
**	request is formatted such that the client will connect
**	directly to the remote server rather than making a remote
**	connection through a local Comm Server.
**	
**	gcn_direct		Format GCN_NS_RESOLVE message on client
**	gcn_resolved		Process GCN2_RESOLVED message on client
**	gcn_connect_info	Build GCN2_RESOLVED for GCA_REQUEST.
**
** History:
**	14-Sep-98 (gordy)
**	    Extracted from gcnsauth.c
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	15-Jul-04 (gordy)
**	    Enhanced password encryption between gcn and gcc.
**	26-Oct-05 (gordy)
**	    Ensure buffers are large enough to hold passwords,
**	    encrypted passwords and authentication certificates.
*/	


/*
** Name: gcn_direct
**
** Description:
**	Builds GCN2_RESOLVED and GCN_NS_RESOLVE messages for
**	connecting to a remote Name Server so the client can
**	make a direct server connection bypassing the comm
**	server.
**	
** Inputs:
**	grcb		Resolve control block.
**	mbout		MBUF queue for formatted message.
**	
** Outputs:
**	None.
**
** Returns:
**	STATUS		OK or error code.
**
** History:
**	12-Jan-98 (gordy)
**	    Created.
**	 2-Oct-98 (gordy)
**	    Need to restore quoting to database name when
**	    passing on to the remote system.
**	15-Jul-04 (gordy)
**	    Enhanced password encryption between gcn and gcc.
**	    Check flag to see if resolved password is encrypted.
**	26-Oct-05 (gordy)
**	    Ensure buffer is large enough for encrypted password.
*/	

STATUS
gcn_direct( GCN_RESOLVE_CB *grcb, GCN_MBUF *mbout )
{
    char	pbuff[ GC_L_PASSWORD * 2 + 1 ];
    char	*pwd = pbuff;
    char	*m, partner[ GCN_EXTNAME_MAX_LEN + 1 ];
    GCN_MBUF 	*mbuf;
    STATUS	status;
    i4		i;

    /*
    ** Produce the GCN2_RESOLVED message to make connection.
    ** Password must be encrypted for the COMSVR, but may
    ** already be encrypted as well.
    */
    if ( grcb->flags & GCN_RSLV_PWD_ENC )
	pwd = grcb->passwd_out;
    else
        gcn_login( GCN_VLP_COMSVR, 1, TRUE, 
		   grcb->user_out, grcb->passwd_out, pwd );

    status = gcn_connect_info( grcb, mbout, grcb->user_out, pwd );
    if ( status != OK )  return( status );

    /*
    ** Prepare GCN_NS_RESOLVE message.
    */
    mbuf = gcn_add_mbuf( mbout, TRUE, &status );
    if ( status != OK )  return status;

    m = mbuf->data;
    m += gcu_put_str( m, "" );
    m += gcu_put_str( m, "" );
    if ( grcb->flags & GCN_RSLV_QUOTE )  gcn_quote( &grcb->dbname );
    gcn_unparse( "", grcb->dbname, grcb->class, partner );
    if ( grcb->flags & GCN_RSLV_QUOTE )  gcn_unquote( &grcb->dbname );
    m += gcu_put_str( m, partner );
    m += gcu_put_str( m, "" );
    m += gcu_put_str( m, "" );
    mbuf->used = m - mbuf->data;
    mbuf->type = GCN_NS_RESOLVE;

    return( OK );
}


/*
** Name: gcn_resolved
**
** Description:
**	Process a GCN2_RESOLVED message response for a direct
**	connection request.  Replaces the local/remote connection
**	info for the current RESOLVE request with the values
**	from the processed message.  Also checks to see if the
**	remote Name Server returned an error instead.
**
** Input:
**	grcb		Resolve control block.
**	mbuf		Message buffer.
**
** Output:
**	None.
**
** Returns:
**	STATUS		OK or error code.
**
** History:
**	12-Jan-98 (gordy)
**	    Created.
**	31-Mar-98 (gordy)
**	    Made GCN2_D_RESOLVED extensible with variable array of values.
**	15-Jul-04 (gordy)
**	    Flag password as encrypted.
*/

STATUS
gcn_resolved( GCN_RESOLVE_CB *grcb, GCN_MBUF *mbin )
{
    GCN_RESOLVE_CB	save_cb;
    char		*ptr, *m = mbin->data;
    i4			i, count, type, length;
    STATUS		status;

    /*
    ** Check for error.
    */
    if ( mbin->type == GCA_ERROR )
    {
	m += gcu_get_int( m, &i );	/* toss gca_l_e_element */
	m += gcu_get_int( m, &status );
	return( status );
    }

    if ( mbin->type != GCN2_RESOLVED )  return( E_GC0008_INV_MSG_TYPE );

    /*
    ** Save the current resolve control block info.
    ** If an error occurs such that a direct connect
    ** will not be possible, the original resolve
    ** info will need to be restored so we can do
    ** a normal remote connection.
    */
    STRUCT_ASSIGN_MACRO( *grcb, save_cb );

    m += gcn_copy_str( m, grcb->user );		/* User ID for connection */
    m += gcu_get_int( m, &grcb->catl.tupc );	/* local server count */

    for( i = 0; i < grcb->catl.tupc; i++ )
    {
	/*
	** If the remote Name Server does not return
	** a host name, use the node name which was
	** used for the remote resolve request.
	*/
	ptr = grcb->catl.tupv[i];		/* Server host */
	m += gcn_copy_str( m, ptr );

	if ( ! (length = STlength( ptr )) )
	    grcb->catl.host[i] = grcb->catr.node[0];
	else
	{
	    grcb->catl.host[i] = ptr;
	    ptr += length + 1;
	}

	m += gcn_copy_str( m, ptr );		/* Server address */
	grcb->catl.addr[i] = ptr;

	m += gcu_get_int( m, &grcb->catl.auth_len[i] );	/* Server auth */

	if ( grcb->catl.auth_len[i] )
	{
	    grcb->catl.auth[i] = MEreqmem( 0, grcb->catl.auth_len[i], 
					   FALSE, &status );
	    if ( ! grcb->catl.auth[i] )  
	    {
		grcb->catl.auth_len[i] = 0;
		grcb->catl.tupc = i;
		gcn_rslv_cleanup( grcb );
		STRUCT_ASSIGN_MACRO( save_cb, *grcb );
		return( status );
	    }

	    MEcopy( m, grcb->catl.auth_len[i], grcb->catl.auth[i] );
	    m += grcb->catl.auth_len[i];
	}
	else
	{
	    /*
	    ** If the remote Name Server could not generate
	    ** a server authentication, we can't permit a
	    ** direct connection, so toss this entry.
	    */
	    grcb->catl.tupc--;
	    i--;	/* adjust for loop increment */
	}
    }

    /*
    ** If no suitable server was found for a direct
    ** connection, attempt a normal remote connection.
    */
    if ( ! grcb->catl.tupc )
    {
	gcn_rslv_cleanup( grcb );
	STRUCT_ASSIGN_MACRO( save_cb, *grcb );
	return( E_GC0163_GCN_DC_NO_SERVER );
    }

    /*
    ** No vnode was used when making the RESOLVE request,
    ** so you wouldn't expect remote connection info to
    ** be returned.  Interestingly enough, the remote
    ** installation could have a remote_vnode set in the
    ** config file which would force requests local to
    ** the remote machine to go remote as well.
    */
    m += gcu_get_int( m, &grcb->catr.tupc );	/* remote address count */

    for( i = 0; i < grcb->catr.tupc; i++ )
    {
	ptr = grcb->catr.tupv[ i ];		/* remote node */
	m += gcn_copy_str( m, ptr );
	grcb->catr.node[ i ] = ptr;
	ptr += STlength( ptr ) + 1;

	m += gcn_copy_str( m, ptr );		/* remote protocol */
	grcb->catr.protocol[ i ] = ptr;
	ptr += STlength( ptr ) + 1;

	m += gcn_copy_str( m, ptr );		/* remote port */
	grcb->catr.port[ i ] = ptr;	
    }

    ptr = grcb->usrpwd;
    m += gcu_get_int( m, &count );	/* remote data count */

    for( i = 0; i < count; i++ )
    {
	m += gcu_get_int( m, &type );	/* remote data type */

	switch( type )
	{
	    case GCN_RMT_USR :
		m += gcn_copy_str( m, ptr );		/* remote user ID */
		grcb->user_out = ptr;
		ptr += STlength( ptr ) + 1;
		break;

	    case GCN_RMT_PWD :
		/*
		** Password from remote GCN is encrypted for
		** remote GCC.  We can't decrypt the password
		** nor encrypt a password for the remote GCC,
		** so flag that it has already been encrypted.
		*/
		m += gcn_copy_str( m, ptr );		/* remote password */
		grcb->passwd_out = ptr;
		grcb->flags |= GCN_RSLV_PWD_ENC;
		ptr += STlength( ptr ) + 1;
		break;

	    case GCN_RMT_AUTH :
		m += gcu_get_int( m, &grcb->auth_len );	/* remote auth */

		if ( grcb->auth_len )
		{
		    if ( ! (grcb->auth = MEreqmem( 0, grcb->auth_len, 
						   TRUE, &status )) )
		    {
			grcb->auth_len = 0;
			gcn_rslv_cleanup( grcb );
			STRUCT_ASSIGN_MACRO( save_cb, *grcb );
			return( status );
		    }

		    MEcopy( m, grcb->auth_len, grcb->auth );
		    m += grcb->auth_len;
		}
		break;


	    case GCN_RMT_EMECH :
		m += gcn_copy_str( m, grcb->emech );	/* encryption mech */
		break;

	    case GCN_RMT_EMODE :
		m += gcn_copy_str( m, grcb->emode );	/* encryption mode */
		break;

	    case GCN_RMT_MECH :
		m += gcn_copy_str( m, grcb->rmech );	/* auth mech */
		break;

	    default :
		m += gcu_get_int( m, &length );		/* skip */
		m += length;
		break;
	}
    }

    /*
    ** We read the user ID to be used for the local connection
    ** into grcb->user and grcb->user_out points at the user
    ** to be used for the remote connection.  Direct connects
    ** do not generally make a remote connection (though there
    ** is an obscure case in which it is possible), in which
    ** case grcb->user_out must point to the user ID to be
    ** used when connecting to the server.
    */
    if ( ! grcb->catr.tupc )  grcb->user_out = grcb->user;

    return( OK );
}


/*
** Name: gcn_connect_info
**
** Description:
**	Format a GCN2_RESOLVED message to be used as resolved
**	connection info for GCA_REQUEST.
**
** Input:
**	grcb		Resolve Info.
**	mbout		MBUF queue for formatted message.
**	user		User ID for connection.
**	pwd		Password, encrypted for COMSVR.
**
** Output:
**	None.
**
** Returns:
**	STATUS		OK or error code.
**
** History:
**	12-Jan-98 (gordy)
**	    Extracted from gcn_get_auth().
**	17-Mar-98 (gordy)
**	    User GCusername() for effective user ID.
**	31-Mar-98 (gordy)
**	    Made GCN2_D_RESOLVED extensible with variable array of values.
**	 5-May-98 (gordy)
**	    Client user ID added to server auth.
**	17-Sep-98 (gordy)
**	    Generate user auth if can't generate server auth.
*/

STATUS
gcn_connect_info( GCN_RESOLVE_CB *grcb, GCN_MBUF *mbout, char *user, char *pwd )
{
    char	*m, *cnt_ptr;
    GCN_MBUF 	*mbuf;
    STATUS	status;
    i4		i, count;

    mbuf = gcn_add_mbuf( mbout, TRUE, &status );
    if ( status != OK )  return( status );

    m = mbuf->data;
    m += gcu_put_str( m, IIGCn_static.username );	/* Local userID */

    /* 
    ** Put the local address catalogs into the buffer 
    */
    m += gcu_put_int( m, grcb->catl.tupc );

    for( i = 0; i < grcb->catl.tupc; i++ )
    {
	char	*ptr;
	i4	len;

	m += gcu_put_str( m, grcb->catl.host[ i ] );
	m += gcu_put_str( m, grcb->catl.addr[ i ] );

	/*
	** Create a server auth for Name Server to
	** local server connection.  If server auth
	** fails (we may not have the server key),
	** generate a user auth instead.
	*/
	m += gcu_put_int( ptr = m, 0 );		/* Place holder */
	len = gcn_server_auth( grcb->catl.addr[ i ], 
			       IIGCn_static.username, IIGCn_static.username,
			       mbuf->size - (m - mbuf->data), (PTR)m );
	if ( ! len )  
	    len = gcn_user_auth( mbuf->size - (m - mbuf->data), (PTR)m );

	gcu_put_int( ptr, len );
	m += len;
    }

    /* 
    ** Put the remote address catalogs into the buffer 
    */
    m += gcu_put_int( m, grcb->catr.tupc );
		
    for( i = 0; i < grcb->catr.tupc; i++ )
    {
	m += gcu_put_str( m, grcb->catr.node[ i ] );
	m += gcu_put_str( m, grcb->catr.protocol[ i ] );
	m += gcu_put_str( m, grcb->catr.port[ i ] );
    }

    /* 
    ** Fill in the user id to data message 
    */
    cnt_ptr = m;
    count = 0;
    m += gcu_put_int( m, count );		/* place holder */

    if ( grcb->catr.tupc > 0 )
    {
	m += gcu_put_int( m, GCN_RMT_DB );
	m += gcu_put_str( m, "/IINMSVR" );
	count++;

	/*
	** Provide authentication info with the following precedence:
	**
	** 1. Existing remote authentication (installation password).
	** 2. Generate remote auth (delegated auth) for client.
	** 3. Password.
	*/
	if ( grcb->auth_len > 0 )
	{
	    m += gcu_put_int( m, GCN_RMT_AUTH );
	    m += gcu_put_int( m, grcb->auth_len );
	    MEcopy( grcb->auth, grcb->auth_len, m );
	    m += grcb->auth_len;
	    MEfree( grcb->auth );
	    grcb->auth = NULL;
	    grcb->auth_len = 0;
	    count++;
	}
	else  if ( grcb->rmech[0]  &&  grcb->deleg_len  &&
		   STcompare( user, GCN_NOUSER ) )
	{
	    char	*ptr = m;
	    char	*len_ptr;
	    i4		len;

	    /* Build header (temporary until remote auth succeeds) */
	    ptr += gcu_put_int( ptr, GCN_RMT_AUTH );
	    len_ptr = ptr;
	    ptr += gcu_put_int( ptr, 0 );
	    len = mbuf->size - (ptr - mbuf->data);

	    if ( gcn_rem_auth(grcb->rmech, grcb->catr.node[0], 
			      grcb->deleg_len, grcb->deleg, &len, ptr) == OK )
	    {
		gcu_put_int( len_ptr, len );
		m = ptr + len;
		user = grcb->user;
		count++;
	    }
	    else  if ( *pwd )
	    {
		m += gcu_put_int( m, GCN_RMT_PWD );
		m += gcu_put_str( m, pwd );
		count++;
	    }
	}
	else  if ( *pwd )
	{
	    m += gcu_put_int( m, GCN_RMT_PWD );
	    m += gcu_put_str( m, pwd );
	    count++;
	}

	if ( *user )
	{
	    m += gcu_put_int( m, GCN_RMT_USR );
	    m += gcu_put_str( m, user );
	    count++;
	}

	if ( *grcb->emech )
	{
	    m += gcu_put_int( m, GCN_RMT_EMECH );
	    m += gcu_put_str( m, grcb->emech );
	    count++;
	}

	if ( *grcb->emode )
	{
	    m += gcu_put_int( m, GCN_RMT_EMODE );
	    m += gcu_put_str( m, grcb->emode );
	    count++;
	}
    }

    if ( count )  gcu_put_int( cnt_ptr, count );

    mbuf->used = m - mbuf->data;
    mbuf->type = GCN2_RESOLVED;

    return( OK );
}

