/*
** Copyright (c) 2004 Ingres Corporation
*/

#include <compat.h>
#include <me.h>
#include <st.h>
#include <gl.h>
#include <iicommon.h>
#include <gca.h>
#include <gcn.h>
#include <gcu.h>
#include <qu.h>
#include <tm.h>
#include <nm.h>
#include <gcnint.h>
#include <id.h>
#include <er.h>
#include <gcnapi.h>


/*
** Name:        gcnapi.c
**
** Description:
**      Contains the II_GCNapi_ModifyNode API function for adding, deleting 
**	or editing a vnode.
**
** History:
**      05-oct-98 (mcgem01)
**          Created for the Unicenter team.
**	05-mar-99 (devjo01)
**	    Formally incorporated into Ingres.
**	26-apr-1999 (mcgem01)
**	    Updated to compile with Ingres 2.5
**	04-may-1999 (mcgem01)
**	    Add a node test function, similar to Shift-F5 in Netutil.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**  11-Jan-2005 (fanra01)
**      Sir 113724
**      Suppress default message action by supplying a callback function.
**  11-Jan-2005 (fanra01)
**      Correct minor typos.
**  12-Jan-2005 (fanra01)
**      Manual Cross integration of changes 474444 and 474447.
**  12-Jan-2005 (fanra01)
**      Bug 113735
**      Ensure that password is not part of a delete operation.
*/

static char	gUserID[GCN_UID_MAX_LEN];
static bool	bIngresWasInit = FALSE;
static bool node_found = FALSE;

static int InitGCNInterface (void);
static int gcn_api_call (int flag, PTR obj, int opcode, PTR value, PTR type);
static i4 gcn_err_func (char * msg) { return 0; }
static i4	gcn_api_response();
static i4	gcn_api_slient();

/*
** Name: gcn_api_silent - suppress default message behaviour
**
** Description:
**  A callback response function passed to gcn_api_call that performs a noop.
**
** Inputs:
**  msg_type        GCA message primitive type (ignored).
**  buf             Response message (ignored).
**  len             Length of response message (ignored).
**
** Outputs:
**  None
**
**  Returns:
**      0
**
**  History:
**      11-01-2005 (fanra01)
**          Created.
*/
static i4
gcn_api_silent(i4 msg_type, char *buf, i4 len)
{
    switch( msg_type )
    {
        case GCN_RESULT:
            break;
        default:
            break;
    }
    return(0);
}

/*
**
**	Name:	II_GCNapi_ModifyNode
**
**	Description:
**
**	This function adds and deletes virtual nodes in the 
**	IILOGIN and IINODE files via the Ingres name server interface.
**
**	opcode values are either ADD_NODE or DELETE_NODE.
**
**	The virtual node name may be concatenated with the database 
**	name (i.e., vnode::database).
**
**	This function returns a status code as:  0 = no error, 
**	0xC0021 = server unavailabe, or some other error number.
**
** History:
**      12-Jan-2005 (fanra01)
**          Ensure that the encrypted password is not appended to the value
**          for a delete operation.
*/

int 
II_GCNapi_ModifyNode( int opcode, 
	int    entryType,
	char * virtualNode, 
	char * netAddress, 
	char * protocol, 
	char * listenAddress, 
	char * username, 
	char * password)
{
	char	vnode[GCN_VAL_MAX_LEN];
	char	value[GCN_VAL_MAX_LEN];
	char	encrypted[GCN_VAL_MAX_LEN];
	int	status;
	int	len;

	status = InitGCNInterface();

	if (status != OK) 
	    goto xit;
	
 	len = strcspn(virtualNode, ":");
	strncpy(vnode, virtualNode, len);
	vnode[len] = 0;

    if ((username != NULL) && (password != NULL))
    {
        status = gcu_encode(username, password, encrypted);
    }
    else
    {
        status = FAIL;
    }
	if (status != OK) 
	    goto xit;

	STcopy(netAddress, value);
	STcat(value, ",");
	STcat(value, protocol);
	STcat(value, ",");
	STcat(value, listenAddress);

	status = gcn_api_call(entryType, vnode, opcode, value, GCN_NODE);
	if (status != OK) 
	    goto xit;

	STcopy(username, value);
	STcat(value, ",");
    if(opcode != DELETE_NODE)
    {
        STcat(value, encrypted);
    }    
    
	status = gcn_api_call(entryType, vnode, opcode, value, GCN_LOGIN);
	if (status != OK) 
	    goto xit;

	status = IIGCn_call(GCN_TERMINATE, NULL);
	return status;

xit:
	IIGCn_call(GCN_TERMINATE, NULL);
	return status;
}

/*
**
**      Name:   II_GCNapi_TestNode
**
**      Description:
**		Test the name server connection to a given vnode.
**
*/
int 
II_GCNapi_TestNode( char *vnode) 
{
	char 	target[GCN_EXTNAME_MAX_LEN + 1];
	int	status;

        status = InitGCNInterface();

        if (status != OK)
            goto xit;

	STpolycat( 2, vnode, ERx("::/IINMSVR"), target );

	status = gcn_testaddr( target, 0 , NULL );
        if (status != OK)
            goto xit;

        status = IIGCn_call(GCN_TERMINATE, NULL);
        return status;

xit:
	IIGCn_call(GCN_TERMINATE, NULL);
	return status;
}

/*
**
** Name: InitGCNInterface
**
** Description:
**	Initialize the name server client interface.
**
*/
static int 
InitGCNInterface(void)
{
    int	status;
    char	*user;

    IIGCn_call(GCN_TERMINATE, NULL);

    if (! bIngresWasInit)
    {
        status = MEadvise(ME_INGRES_ALLOC);

        if (status != OK) 
            return status;

        SIeqinit();

        status = IIGCn_call(GCN_INITIATE, NULL);

        if (status != OK) 
            return status;

        gcn_seterr_func(gcn_err_func);

        user = gUserID;
        IDname(&user);

        bIngresWasInit = TRUE;
    }
    else
    {
        status = IIGCn_call(GCN_INITIATE, NULL);
        if (status != OK) 
            return status;
    }

    return OK;
}

/*  
**  Name: gcn_api_call 
**
**  Description: generic call to the name server interface scarfed
**		from the nv_api_call function in netutil.
**
**  Inputs:
**
**    flag    -- Flag parameter for GCN_CALL_PARMS.
**                 GCN_PUB_FLAG - for global operations
**                 GCN_UID_FLAG - if impersonating another user.
**    obj     -- the thing being operated on.
**    opcode  -- GCN_ADD, GCN_DEL, etc., specifying the operation to be done.
**    value   -- the value being changed to or added.
**    type    -- GCN_NODE or GCN_LOGIN depending on what we're operating on.
**
**  History:
**      11-Jan-2005 (fanra01)
**          Add silent response function as parameter.
*/

static int 
gcn_api_call(int flag, 
	char *obj, 
	int opcode, 
	char *value, 
	char *type)
{
    GCN_CALL_PARMS	gcn_parm;
    int			status;

    if (opcode == ADD_NODE)
	opcode = GCN_ADD;
    else if (opcode == DELETE_NODE)
	opcode = GCN_DEL;

    gcn_parm.gcn_response_func = gcn_api_silent;
    gcn_parm.gcn_uid = gUserID;
    gcn_parm.gcn_flag = flag;
    gcn_parm.gcn_obj = obj;
    gcn_parm.gcn_type = type;
    gcn_parm.gcn_value = value;
    gcn_parm.gcn_host = NULL;

    NMgtAt("II_INSTALLATION", &gcn_parm.gcn_install);

    status = IIGCn_call(opcode, &gcn_parm);
    return status;
}


/*
**
**   Name:   II_GCNapi_Node_Exists
**
**   Description:
**	Test to see if a vnode Exists. This will not 
**	test to see if the vnode is valid. This will
**	only work with globally defined vnodes.
**
**   Inputs:
**
**    vnode    -- Character string of the vnode you wish
**                to  test.
**   Outputs:
**   
**    boolean  -- True if a vnode name exists.
**
**	 History:
**      22-may-2000 (rodjo04)
**          Created for the Unicenter team.
*/

bool 
II_GCNapi_Node_Exists( char *vnode) 
{
    GCN_CALL_PARMS gcn_parm;
    STATUS status;
    char    *username;
        
    char *ntype = "NODE";   /* create local vars instead of a buffer. */
    char *ltype = "LOGIN";  /* We will save calls to STcopy */
    char *nvalue = ",,";
    char *lvalue = ",";     
    int opcode = GCN_RET;  
    node_found = FALSE; /* assume not found */

    status = IIGCn_call(GCN_INITIATE, NULL);

    if (status != OK) 
        return FALSE;

    gcn_parm.gcn_response_func = gcn_api_response;  /* Handle response locally. */

    username = gUserID;
    IDname(&username);

    gcn_parm.gcn_uid = username;
    gcn_parm.gcn_flag = GCN_PUB_FLAG;  
    gcn_parm.gcn_obj = vnode;
    gcn_parm.gcn_type = ltype;
    gcn_parm.gcn_value = lvalue;

   
    gcn_parm.gcn_install = "\0";
    gcn_parm.gcn_host = "\0";

    status = IIGCn_call(opcode,&gcn_parm);  /* check for Login info */

    if (node_found)
    {
        IIGCn_call(GCN_TERMINATE, NULL);
        return (node_found);  /* exit if we find Login info */
    }
     
    gcn_parm.gcn_type = ntype;
    gcn_parm.gcn_value = nvalue;

    status = IIGCn_call(opcode,&gcn_parm);  /* check for Connection info */
   
    IIGCn_call(GCN_TERMINATE, NULL);
    return (node_found);
}
	

/*
**  gcn_api_response -- handle a response from the name server.
**
**  This function is called by the name server to handle the response to
**  function II_GCNapi_Node_Exists.  This is a modified version of gcn_responce
**  used in neutil. It will only test to see if a single row was returned. 
*/

static i4
gcn_api_response(i4 msg_type, char *buf, i4 len)
{
        i4 row_count = 0, op = 0;
        
        /* Handle the message responses from name server. */
        switch( msg_type )
        {
            case GCN_RESULT:
         /*        Pull off the results from the operation  */
                buf += gcnapi_get_int( buf, &op );
                if ( op == GCN_RET )
                {
                    buf += gcnapi_get_int( buf, &row_count );
                    if (row_count != 1)
                        node_found = FALSE;
                    else             
                        node_found = TRUE;
                }
                break;
            default:
                break;
        }
        return op;
}   
	
/*
** Name: gcnapi_get_int() - get integer from buffer.
**
** Description:
**	Gets integer i from buffer p.
**  Ripped from netutil.
**
** Returns:
**	Amount of buffer used.
*/

i4
gcnapi_get_int( char *p, i4 *i )
{
	i4	i4val;

	I4ASSIGN_MACRO( *p, i4val );

	*i = i4val;

	return sizeof( i4val );
}
	
	
	
	
	
	
	
	
	
	
	
	
	
