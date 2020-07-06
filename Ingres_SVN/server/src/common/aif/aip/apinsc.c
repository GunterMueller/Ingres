/*
** Copyright (c) 2004 Ingres Corporation
*/

# include <compat.h>
# include <gl.h>
# include <st.h>

# include <iicommon.h>
# include <generr.h>

# include <iiapi.h>
# include <api.h>
# include <apins.h>
# include <apichndl.h>
# include <apithndl.h>
# include <apishndl.h>
# include <apiehndl.h>
# include <apierhnd.h>
# include <apilower.h>
# include <apigcap.h>
# include <apimisc.h>
# include <apitrace.h>
# include <apige2ss.h>

/*
** Name: apinsc.c
**
** Description:
**	Name Server Connection State Machine.
**
** History:
**	13-Feb-97 (gordy)
**	    Created.
**	25-Mar-97 (gordy)
**	    At protocol level 63 Name Server protocol fixed to
**	    handle end-of-group and end-of-data by GCA standards.
**	 3-Jul-97 (gordy)
**	    Re-worked state machine initialization.  Added
**	    IIapi_ns_init(), IIapi_ns_cinit().
**      21-Aug-98 (rajus01)
**          Added support for IIapi_abort().
**	 3-Sep-98 (gordy)
**	    Call functions to abort transaction, statement and
**	    database event handles.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	11-Dec-02 (gordy)
**	    Tracing added for local error conditions.
*/

/*
** Forward references
*/

static IIAPI_SM_OUT	*sm_evaluate( IIAPI_EVENT, IIAPI_STATE, IIAPI_HNDL *, 
				      IIAPI_HNDL *, II_PTR, IIAPI_SM_OUT * );

static II_BOOL		sm_execute( IIAPI_ACTION, IIAPI_HNDL *, 
				    IIAPI_HNDL *, II_PTR );

II_EXTERN i4	ss_encode( char * );


/*
** Information to be exported through
** the state machine interface.
*/

/*
** Name Server Connection States.
**
** The following values are used as array
** indices.  Care must be taken when adding
** new entries to ensure there are no gaps
** or duplicates and all associated arrays
** are kept in sync with these definitions.
**
** Note that the IDLE state must be 0 to
** match IIAPI_IDLE.
*/

# define NS_CS_IDLE	((IIAPI_STATE)  0)	/* Idle */
# define NS_CS_CONN	((IIAPI_STATE)  1)	/* Connected */
# define NS_CS_FAIL	((IIAPI_STATE)  2)	/* Connection failure */
# define NS_CS_REQ	((IIAPI_STATE)  3)	/* Connection requested */
# define NS_CS_RLS	((IIAPI_STATE)  4)	/* Release sent */
# define NS_CS_DISC	((IIAPI_STATE)  5)	/* Disconnect requested */

# define NS_CS_CNT	(NS_CS_DISC + 1)

static char *ns_cs_id[] =
{
    "IDLE", "CONN", "FAIL", "REQ",  "RLS",  "DISC"
};

/*
** Name Server Connection actions
**
** The following values are used as array
** indices.  Care must be taken when adding
** new entries to ensure there are no gaps
** or duplicates and all associated arrays
** are kept in sync with these definitions.
*/

# define NS_CA_REMC	((IIAPI_ACTION)  0)	/* Remember callback */
# define NS_CA_CONN	((IIAPI_ACTION)  1)	/* Connect */
# define NS_CA_DISC	((IIAPI_ACTION)  2)	/* Disconnect */
# define NS_CA_RECV	((IIAPI_ACTION)  3)	/* Receive message */
# define NS_CA_REDO	((IIAPI_ACTION)  4)	/* Resume GCA request */
# define NS_CA_SRLS	((IIAPI_ACTION)  5)	/* Send release */
# define NS_CA_RERR	((IIAPI_ACTION)  6)	/* Read error */
# define NS_CA_PEND	((IIAPI_ACTION)  7)	/* Abort pending operations */
# define NS_CA_DELH	((IIAPI_ACTION)  8)	/* Delete connection handle */
# define NS_CA_ERGC	((IIAPI_ACTION)  9)	/* GCA failure/error */
# define NS_CA_CBOK	((IIAPI_ACTION) 10)	/* Callback Success */
# define NS_CA_CBIF	((IIAPI_ACTION) 11)	/* Callback Invalid Function */
# define NS_CA_CBAB	((IIAPI_ACTION) 12)	/* Callback Abort */
# define NS_CA_CBDC	((IIAPI_ACTION) 13)	/* Callback disconnect */

# define NS_CA_CNT	(NS_CA_CBDC + 1)

static char *ns_ca_id[] =
{
    "REMC", "CONN", "DISC", "RECV", "REDO", 
    "SRLS", "RERR", "PEND", "DELH", 
    "ERGC", "CBOK", "CBIF", "CBAB", "CBDC"
};

/*
** Name: ns_conn_sm
**
** Description:
**	The Name Server Connection State Machine Interface
*/

static IIAPI_SM ns_conn_sm =
{
    "NS Conn",
    NS_CS_CNT,
    ns_cs_id,
    NS_CA_CNT,
    ns_ca_id,
    sm_evaluate,
    sm_execute,
    NS_CS_FAIL
};


/*
** Name: ns_act_seq
**
** Description:
**	Action sequences define the groups of
**	actions which occur at state transitions.
**	An action sequence may be used by several
**	state transitions, so the next state
**	value is not defined here.  The SM_OUT
**	structure is used for conveniance in
**	assigning the action sequences during
**	evaluation.
**
** History:
**	13-Feb-97 (gordy)
**	    Created.
*/

static IIAPI_SM_OUT ns_act_seq[] =
{
    { 0, 0, { 0, 0, 0 } },					/* CAS_0  */
    { 0, 1, { NS_CA_RECV, 0, 0 } },				/* CAS_1  */
    { 0, 1, { NS_CA_DISC, 0, 0 } },				/* CAS_2  */
    { 0, 2, { NS_CA_REMC, NS_CA_CONN, 0 } },			/* CAS_3  */
    { 0, 2, { NS_CA_REMC, NS_CA_SRLS, 0 } },			/* CAS_4  */
    { 0, 2, { NS_CA_REMC, NS_CA_DISC, 0 } },			/* CAS_5  */
    { 0, 3, { NS_CA_PEND, NS_CA_DELH, NS_CA_CBOK } },		/* CAS_6  */
    { 0, 3, { NS_CA_ERGC, NS_CA_PEND, NS_CA_CBAB } },		/* CAS_7  */
    { 0, 3, { NS_CA_RERR, NS_CA_PEND, NS_CA_CBAB } },		/* CAS_8  */
    { 0, 1, { NS_CA_CBOK, 0, 0 } },				/* CAS_9  */
    { 0, 2, { NS_CA_RERR, NS_CA_RECV, 0 } },			/* CAS_10 */
    { 0, 3, { NS_CA_SRLS, NS_CA_PEND, NS_CA_CBAB } },		/* CAS_11 */
    { 0, 1, { NS_CA_CBIF, 0, 0 } },				/* CAS_12 */
    { 0, 2, { NS_CA_ERGC, NS_CA_CBAB, 0 } },			/* CAS_13 */
    { 0, 3, { NS_CA_REMC, NS_CA_DELH, NS_CA_CBOK } },		/* CAS_14 */
    { 0, 1, { NS_CA_REDO, 0, 0 } },				/* CAS_15 */
    { 0, 2, { NS_CA_REMC, NS_CA_CBDC, 0 } }                     /* CAS_16 */

};

/*
** Indexes into the action sequence array.
*/

# define NS_CAS_0	0
# define NS_CAS_1	1
# define NS_CAS_2	2
# define NS_CAS_3	3
# define NS_CAS_4	4
# define NS_CAS_5	5
# define NS_CAS_6	6
# define NS_CAS_7 	7
# define NS_CAS_8	8
# define NS_CAS_9 	9
# define NS_CAS_10	10
# define NS_CAS_11	11
# define NS_CAS_12	12
# define NS_CAS_13	13
# define NS_CAS_14	14
# define NS_CAS_15	15
# define NS_CAS_16	16

/*
** Name: smt_list
**
** Description:
**	State machine transitions define the 
**	next state and action sequence for 
**	static evaluations (transitions which 
**	depend solely on the current state 
**	and input event).
**
**	These transitions entries are used 
**	to populate a sparse matrix for quick 
**	access via the current state and event.
**
** History:
**	13-Feb-97 (gordy)
**	    Created.
*/

typedef struct
{

    IIAPI_EVENT		smt_event;
    IIAPI_STATE 	smt_state;
    IIAPI_STATE		smt_next;
    IIAPI_ACTION	smt_action;

} SM_TRANSITION;

static SM_TRANSITION smt_list[] =
{
    /*
    ** API Functions which directly affect the connection state.
    */
    { IIAPI_EV_CONNECT_FUNC,	NS_CS_IDLE,	NS_CS_REQ,	NS_CAS_3  },
    { IIAPI_EV_DISCONN_FUNC,	NS_CS_IDLE,	NS_CS_IDLE,	NS_CAS_14 },
    { IIAPI_EV_DISCONN_FUNC,	NS_CS_CONN,	NS_CS_RLS,	NS_CAS_4  },
    { IIAPI_EV_DISCONN_FUNC,	NS_CS_FAIL,	NS_CS_DISC,	NS_CAS_5  },
    { IIAPI_EV_ABORT_FUNC,      NS_CS_IDLE,     NS_CS_IDLE,     NS_CAS_14 },
    { IIAPI_EV_ABORT_FUNC,      NS_CS_CONN,     NS_CS_DISC,     NS_CAS_5 },
    { IIAPI_EV_ABORT_FUNC,      NS_CS_FAIL,     NS_CS_DISC,     NS_CAS_5 },
    { IIAPI_EV_ABORT_FUNC,      NS_CS_RLS,      NS_CS_RLS,      NS_CAS_16 },
    { IIAPI_EV_ABORT_FUNC,      NS_CS_DISC,     NS_CS_DISC,     NS_CAS_16 },


    /*
    ** The following functions are permitted while connected, 
    ** but do not directly affect the connection state.
    */
    { IIAPI_EV_AUTO_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_CANCEL_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_CATCHEVENT_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_CLOSE_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_GETCOLUMN_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_GETDESCR_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_GETQINFO_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_PUTPARM_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_QUERY_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_SETDESCR_FUNC,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },

    /*
    ** The following messages are ignored in the connected 
    ** state.  They are the results of actions in other
    ** state machines.
    */
    { IIAPI_EV_ERROR_RCVD,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_GCN_RESULT_RCVD,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },

    /*
    ** GCA request completions.
    */
    { IIAPI_EV_RESUME,		NS_CS_CONN,	NS_CS_CONN,	NS_CAS_15 },
    { IIAPI_EV_RESUME,		NS_CS_FAIL,	NS_CS_FAIL,	NS_CAS_15 },
    { IIAPI_EV_RESUME,		NS_CS_REQ,	NS_CS_REQ,	NS_CAS_15 },
    { IIAPI_EV_RESUME,		NS_CS_RLS,	NS_CS_RLS,	NS_CAS_15 },
    { IIAPI_EV_RESUME,		NS_CS_DISC,	NS_CS_DISC,	NS_CAS_15 },
    { IIAPI_EV_CONNECT_CMPL,	NS_CS_REQ,	NS_CS_CONN,	NS_CAS_9  },
    { IIAPI_EV_DISCONN_CMPL,	NS_CS_DISC,	NS_CS_IDLE,	NS_CAS_6  },
    { IIAPI_EV_SEND_CMPL,	NS_CS_CONN,	NS_CS_CONN,	NS_CAS_0  },
    { IIAPI_EV_SEND_CMPL,	NS_CS_RLS,	NS_CS_DISC,	NS_CAS_2  },
    { IIAPI_EV_SEND_ERROR,	NS_CS_CONN,	NS_CS_FAIL,	NS_CAS_7  },
    { IIAPI_EV_SEND_ERROR,	NS_CS_FAIL,	NS_CS_FAIL,	NS_CAS_0  },
    { IIAPI_EV_SEND_ERROR,	NS_CS_REQ,	NS_CS_FAIL,	NS_CAS_13 },
    { IIAPI_EV_SEND_ERROR,	NS_CS_RLS,	NS_CS_DISC,	NS_CAS_2  },
    { IIAPI_EV_SEND_ERROR,	NS_CS_DISC,	NS_CS_DISC,	NS_CAS_0  },
    { IIAPI_EV_RECV_ERROR,	NS_CS_CONN,	NS_CS_FAIL,	NS_CAS_7  },
    { IIAPI_EV_RECV_ERROR,	NS_CS_FAIL,	NS_CS_FAIL,	NS_CAS_0  },
    { IIAPI_EV_RECV_ERROR,	NS_CS_RLS,	NS_CS_RLS,	NS_CAS_0  },
    { IIAPI_EV_RECV_ERROR,	NS_CS_DISC,	NS_CS_DISC,	NS_CAS_0  }
};


/*
** Name: smt_array
**
** Description:
**	Two dimensional array used for static state 
**	transition evaluations.  The array provides
**	quick access based on the current state and
**	event.  The separation of the array and the
**	state transition entry list is primarily for
**	ease of maintenance.  Initialization occurs
**	at the first evaluation.
**
**	The array is sparsly populated; missing
**	entries are either dynamically evaluated
**	transitions or invalid events.
*/

static SM_TRANSITION	*smt_array[ IIAPI_EVENT_CNT ][ NS_CS_CNT ] ZERO_FILL;
static II_BOOL		initialized = FALSE;



/*
** Name: IIapi_ns_init
**
** Description:
**	Initialize the Name Server state machines.  Adds
**	entries for the Name Server state machines in the
**	global dispatch table IIapi_sm.
**
**	The SQL Database Event state machine is used even 
**	though the Name Server does not support Database 
**	Events.  This will permit the application to create, 
**	cancel, and close Database Event handles, but no 
**	other activity will occur.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	IIAPI_STATUS		API status.
**
** History:
**	 3-Jul-97 (gordy)
**	    Created.
*/

II_EXTERN IIAPI_STATUS
IIapi_ns_init( VOID )
{
    IIAPI_STATUS status;

    for(;;)
    {
	/*
	** Initialize Name Server state machines.
	*/
	if ( (status = IIapi_ns_cinit()) != IIAPI_ST_SUCCESS )  break;
	if ( (status = IIapi_ns_tinit()) != IIAPI_ST_SUCCESS )  break;
	if ( (status = IIapi_ns_sinit()) != IIAPI_ST_SUCCESS )  break;

	/*
	** Use SQL Database Event state machine.
	** Make sure it is initialized and copy
	** the state machine dispatch entry.
	*/
	status = IIapi_sql_dinit();
	IIapi_sm[ IIAPI_SMT_NS ][ IIAPI_SMH_DBEV ] = 
			IIapi_sm[ IIAPI_SMT_SQL ][ IIAPI_SMH_DBEV ];

	break;
    }

    return( status );
}


/*
** Name: IIapi_ns_cinit
**
** Description:
**	Initialize the Name Server Connection state machine.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	IIAPI_STATUS		API status.
**
** History:
**	 3-Jul-97 (gordy)
**	    Created.
*/

II_EXTERN IIAPI_STATUS
IIapi_ns_cinit( VOID )
{
    if ( ! initialized )
    {
	i4 i;

	/*
	** Fill in the transition array from the transition list.
	*/
	for( i = 0; i < (sizeof( smt_list ) / sizeof( smt_list[0] )); i++ )
	    smt_array[ smt_list[i].smt_event ]
		     [ smt_list[i].smt_state ] = &smt_list[i];

	initialized = TRUE;
    }

    IIapi_sm[ IIAPI_SMT_NS ][ IIAPI_SMH_CONN ] = &ns_conn_sm;

    return( IIAPI_ST_SUCCESS );
}


/*
** Name: sm_evaluate
**
** Description:
**	Evaluation function for the Connection State Machine
**
**	Evaluates input (state, event) and returns output (next
**	state, actions).  A NULL return value indicates the event
**	is to be ignored.
**
**	An output buffer is provided by the caller so the output
**	can be built dynamically (return value identical to the
**	parameter).  The output buffer may be ignored and a static
**	buffer returned instead.
**	
**	Evaluations which are solely dependent on the input are
**	done statically through a sparse two dimensional array.
**	If no entry is found in the array, dynamic evaluations
**	(dependent on addition information in event handles or 
**	parameter block) are performed.  If neither evaluation
**	produces output, default error handling evaluations
**	are done.
**
** Input:
**	state		Current state.
**	event		API event.
**	ev_hndl		Handle associated with event.
**	sm_hndl		State machine instance handle.
**	parmBlock	Parameter block associated with event.
**
** Output:
**	smo		Buffer for state machine output, may be ignored.
**
** Returns:
**	IIAPI_SM_OUT *	State machine output, may be NULL.
**
** History:
**	13-Feb-97 (gordy)
**	    Created.
**	25-Mar-97 (gordy)
**	    At protocol level 63 Name Server protocol fixed to
**	    handle end-of-group and end-of-data by GCA standards.
*/

static IIAPI_SM_OUT *
sm_evaluate
(
    IIAPI_EVENT		event,
    IIAPI_STATE 	state,
    IIAPI_HNDL		*ev_hndl,
    IIAPI_HNDL		*sm_hndl,
    II_PTR		parmBlock,
    IIAPI_SM_OUT	*smo_buff
)
{
    IIAPI_CONNHNDL	*connHndl = (IIAPI_CONNHNDL *)sm_hndl;
    IIAPI_SM_OUT	*smo = NULL;
    SM_TRANSITION	*smt;
    IIAPI_GCA_BUFF      *gcaBuff;
    STATUS		gcaError;

    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "%s evaluate: evaluating event %s in state %s\n",
	  ns_conn_sm.sm_id, IIAPI_PRINTEVENT( event ),
	  IIAPI_PRINT_ID( state, NS_CS_CNT, ns_cs_id ) );

    /*
    ** Static evaluations depend solely on the current state and event.
    */
    if ( (smt = smt_array[ event ][ state ]) )
    {
	IIAPI_TRACE( IIAPI_TR_DETAIL )
	    ( "%s evaluate: static evaluation\n", ns_conn_sm.sm_id );

	smo = smo_buff;
	STRUCT_ASSIGN_MACRO( ns_act_seq[ smt->smt_action ], *smo );
	smo->smo_next_state = smt->smt_next;
    }
    else		/* Dynamic evaluations require additional predicates */
    switch( event )	/* to determine the correct state transition.	     */
    {
	/*
	** Check for end-of-data.
	*/
	case IIAPI_EV_RELEASE_RCVD :
	    switch( state )
	    {
		case NS_CS_CONN :
		    gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;
		    smo = smo_buff;

		    if ( NS_EOG(connHndl, (GCA_RV_PARMS *)gcaBuff->gca_parm) )
		    {
			STRUCT_ASSIGN_MACRO( ns_act_seq[ NS_CAS_8  ], *smo );
			smo->smo_next_state = NS_CS_FAIL;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( ns_act_seq[ NS_CAS_10 ], *smo );
			smo->smo_next_state = state;
		    }
		    break;
	    }
	    break;
    }

    /*
    ** If a specific response was not generated above,
    ** produce a generalized error response.
    */ 
    if ( ! smo )
    {
	smo = smo_buff;

	if ( event <= IIAPI_EVENT_FUNC_MAX )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s Evaluate: API function called in wrong state\n",
		  ns_conn_sm.sm_id );

	    STRUCT_ASSIGN_MACRO( ns_act_seq[ NS_CAS_12 ], *smo );
	    smo->smo_next_state = state;
	}
	else  if ( event <= IIAPI_EVENT_MSG_MAX )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s Evaluate: invalid message received\n",
		  ns_conn_sm.sm_id );

	    STRUCT_ASSIGN_MACRO( ns_act_seq[ NS_CAS_11 ], *smo );
	    smo->smo_next_state = NS_CS_RLS;
	}
	else
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s Evaluate: unexpected I/O completion\n",
		  ns_conn_sm.sm_id );

	    STRUCT_ASSIGN_MACRO( ns_act_seq[ NS_CAS_0 ], *smo );
	    smo->smo_next_state = state;
	}
    }

    /*
    ** If the input resulted in no state change 
    ** and no actions to be executed, we can 
    ** just ignore the event.
    */
    if ( smo->smo_next_state == state  &&  ! smo->smo_action_cnt )  
    {
	IIAPI_TRACE( IIAPI_TR_DETAIL )
	    ( "%s evaluate: nothing to do, transition ignored\n", 
	      ns_conn_sm.sm_id );
	smo = NULL;
    }

    return( smo );
}			    




/*
** Name: sm_execute
**
** Description:
**	Action execution function for the Connection State Machine
**
**	Executes and output action as defined by the sm_evaluate().
**	A FALSE return value indicates that any remaining output
**	actions should be skipped and the event dispatching should
**	be aborted.
**
** Input:
**	action		Action ID.
**	ev_hndl		Handle associated with event.
**	sm_hndl		State machine instance handle.
**	parmBlock	Parameter block associated with event.
**
** Output:
**	None.
**
** Returns:
**	II_BOOL		TRUE if successful, FALSE if event should be cancelled.
**
** History:
**	13-Feb-97 (gordy)
**	    Created.
**	25-Mar-97 (gordy)
**	    At protocol level 63 Name Server protocol fixed to
**	    handle end-of-group and end-of-data by GCA standards.
**	 3-Sep-98 (gordy)
**	    Call functions to abort transaction, statement and
**	    database event handles.
**       9-Aug-99 (rajus01)
**          Asynchronous behaviour of API requires the removal of 
**	    handles from the queue. ( Bug #98303 )
**	11-Dec-02 (gordy)
**	    Tracing added for local error conditions.
*/

static II_BOOL
sm_execute
(
    IIAPI_ACTION	action,
    IIAPI_HNDL		*ev_hndl,
    IIAPI_HNDL		*sm_hndl,
    II_PTR		parmBlock
)
{
    IIAPI_CONNHNDL	*connHndl = (IIAPI_CONNHNDL *)sm_hndl;
    IIAPI_CONNPARM	*connParm;
    IIAPI_PARMNMEM	*parmNmem;
    IIAPI_GCA_BUFF      *gcaBuff;
    char		*username;
    IIAPI_STATUS	status;
    II_BOOL		success = TRUE;

    switch( action )
    {
	case NS_CA_REMC :
	    /*
	    ** Remember callback
	    */
	    connHndl->ch_callback = TRUE;
	    connHndl->ch_parm = (IIAPI_GENPARM *)parmBlock;
	    break;

	case NS_CA_CONN :
	    /* 
	    ** Issue connection request.  
	    **
	    ** Connection target (if provided) is assumed to
	    ** be a vnode.  Note: enough space was allocated
	    ** in IIapi_connect() to permit the concatenations
	    ** done below.
	    */
	    if ( *connHndl->ch_target )  STcat( connHndl->ch_target, "::" );
	    STcat( connHndl->ch_target, "/iinmsvr" );

	    /*
	    ** We allow privileged users to access the 
	    ** Name Server using a different username 
	    ** without providing a password.  This is 
	    ** done using GCN_UID_FLAG during requests.  
	    ** If a password is provided, we can just 
	    ** connect and allow normal validation to 
	    ** handle the change of ID.  If no password
	    ** is provided, we need to hide the username 
	    ** during the initial connect or else it will 
	    ** fail.
	    */
	    username = connHndl->ch_username;
	    if ( ! connHndl->ch_password )  connHndl->ch_username = NULL;

	    connParm = (IIAPI_CONNPARM *)parmBlock;
	    status = IIapi_connGCA( ev_hndl, connParm->co_timeout );
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    connHndl->ch_username = username;
	    break;

	case NS_CA_DISC :
	    /*
	    ** Issue disconnect request
	    */
	    status = IIapi_disconnGCA( connHndl );
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    break;

	case NS_CA_RECV :
	    /*
	    ** Issue receive message request.  This is
	    ** the result of default message processing
	    ** during a receive initiated by some other
	    ** entity.  We want to redo the receive with
	    ** the same formatting characteristics, so
	    ** re-use the GCA buffer from the original
	    ** receive request.
	    */
	    gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;
	    gcaBuff->data_len = 0;
	    status = IIapi_rcvNormalGCA(ev_hndl, &gcaBuff, (II_LONG)(-1));
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    break;

	case NS_CA_REDO :
	    /*
	    ** Resume the GCA request.
	    */
	    status = IIapi_resumeGCA( ev_hndl, parmBlock );
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    break;

	case NS_CA_SRLS :
	    /*
	    ** Format and send GCA_RELEASE message.
	    */
	    if ( ! (parmNmem = IIapi_createGCAError( sm_hndl )) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    /*
	    ** Override default message type.
	    */
	    parmNmem->pm_msgType = GCA_RELEASE;
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );

	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case NS_CA_RERR :
	    /*
	    ** Read a GCA_ER_DATA object (GCA_RELEASE)
	    */
	    gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;

	    if ( gcaBuff->formatted )
	    {
		GCD_E_ELEMENT	*elem;
		i4		count;

		if ( connHndl->ch_flags & IIAPI_CH_MSG_CONTINUED )
		    elem = (GCD_E_ELEMENT *)gcaBuff->data;
		else
		    elem = &((GCD_ER_DATA *)gcaBuff->data)->gca_e_element[0];

		count = ((gcaBuff->data + gcaBuff->data_len) - (char *)elem) /
			sizeof(GCD_E_ELEMENT);

		/*
		** Set/reset the continuation flag if the message is
		** continued/done so that we will process the correct
		** portion of the message next time this routine is
		** called.
		*/
		if ( NS_EOD( connHndl, (GCA_RV_PARMS *)gcaBuff->gca_parm ) )
		    connHndl->ch_flags &= ~IIAPI_CH_MSG_CONTINUED;
		else
		    connHndl->ch_flags |= IIAPI_CH_MSG_CONTINUED;

		if ( count  &&  ! IIapi_serverError( ev_hndl, count, elem ) )
		{
		    status = IIAPI_ST_OUT_OF_MEMORY;
		    success = FALSE;
		}
	    }
	    else
	    {
		GCD_E_ELEMENT	elem;
		GCD_DATA_VALUE	data;
		char		text[ 1024 ];
		i4		count, len;

		gcaBuff->data += gcu_get_int( gcaBuff->data, &count );

		/*
		** Handle each error element separately.
		*/
		while( count-- )
		{
		    /*
		    ** We must convert unformatted buffer to
		    ** formatted structure to call the server
		    ** error processing function.
		    */
		    gcaBuff->data += gcu_get_int( gcaBuff->data, 
						  &elem.gca_id_error );
		    gcaBuff->data += gcu_get_int( gcaBuff->data, 
						  &elem.gca_id_server );
		    gcaBuff->data += gcu_get_int( gcaBuff->data, 
						  &elem.gca_server_type );
		    gcaBuff->data += gcu_get_int( gcaBuff->data, 
						  &elem.gca_severity );
		    gcaBuff->data += gcu_get_int( gcaBuff->data, 
						  &elem.gca_local_error );
		    gcaBuff->data += gcu_get_int( gcaBuff->data, 
						  &elem.gca_l_error_parm );

		    /*
		    ** Name Server sends one error parameter,
		    ** but we must scan through the buffer for
		    ** all possible cases.  We only process
		    ** the last paramenter (if any).
		    */
		    if ( elem.gca_l_error_parm )
		    {
			while( elem.gca_l_error_parm-- )
			{
			    gcaBuff->data += gcu_get_int( gcaBuff->data, 
							  &data.gca_type );
			    gcaBuff->data += gcu_get_int( gcaBuff->data, 
							  &data.gca_precision );
			    gcaBuff->data += gcu_get_int( gcaBuff->data, 
							  &data.gca_l_value );

			    len = min( data.gca_l_value, sizeof( text ) );
			    MEcopy( (PTR)gcaBuff->data, len, (PTR)text );
			    gcaBuff->data += data.gca_l_value;
			    data.gca_l_value = len;
			}

			elem.gca_l_error_parm = 1;
			elem.gca_error_parm = &data;
			data.gca_value = text;
		    }

		    /*
		    ** Name Server replaces generic error code
		    ** with the local error code and does not
		    ** support SQLSTATE.  Clean this mess up.
		    */
		    elem.gca_local_error = elem.gca_id_error;

		    if ( connHndl->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_60 )
			elem.gca_id_error = E_GE80E8_LOGICAL_ERROR;
		    else
			elem.gca_id_error = ss_encode( 
			    IIapi_gen2IntSQLState( E_GE80E8_LOGICAL_ERROR, 
						   elem.gca_local_error ) );

		    if ( ! IIapi_serverError( ev_hndl, 1, &elem ) )
		    {
			status = IIAPI_ST_OUT_OF_MEMORY;
			success = FALSE;
			break;
		    }
		}
	    }
	    break;

	case NS_CA_PEND :
	    /*
	    ** Return connection aborted to all pending operations
	    */
	    {
		IIAPI_TRANHNDL	*tranHndl;
		IIAPI_DBEVHNDL	*dbevHndl;
    
		for( 
		     tranHndl = (IIAPI_TRANHNDL *)connHndl->
						ch_tranHndlList.q_next;
		     tranHndl != (IIAPI_TRANHNDL *)&connHndl->ch_tranHndlList;
		     tranHndl = (IIAPI_TRANHNDL *)tranHndl->
						th_header.hd_id.hi_queue.q_next
		   )
		    IIapi_abortTranHndl( tranHndl, 
					 E_AP0001_CONNECTION_ABORTED, 
					 II_SS08006_CONNECTION_FAILURE, 
					 IIAPI_ST_FAILURE );
    
		for( 
		     dbevHndl = (IIAPI_DBEVHNDL *)connHndl->
						ch_dbevHndlList.q_next;
		     dbevHndl != (IIAPI_DBEVHNDL *)&connHndl->ch_dbevHndlList;
		     dbevHndl = (IIAPI_DBEVHNDL *)dbevHndl->
						eh_header.hd_id.hi_queue.q_next
		   )
		    IIapi_abortDbevHndl( dbevHndl, 
					 E_AP0001_CONNECTION_ABORTED, 
					 II_SS08006_CONNECTION_FAILURE, 
					 IIAPI_ST_FAILURE );
	    }
	    break;

	case NS_CA_DELH :
	    /*
	    ** Mark handle for deletion.
	    */
	    sm_hndl->hd_delete = TRUE;
	    QUremove( (QUEUE *)connHndl );
	    break;

	case NS_CA_ERGC :
	    /*
	    ** GCA error.
	    */
	    if ( parmBlock )
	    {
		STATUS gcaError = *((STATUS *)parmBlock);

		IIAPI_TRACE( IIAPI_TR_ERROR )
		    ( "%s: GCA request failed 0x%x\n", 
		      ns_conn_sm.sm_id, gcaError );

		if ( ! IIapi_localError( ev_hndl, gcaError, 
					 II_SS08004_CONNECTION_REJECTED,
					 IIAPI_ST_WARNING ) )
		{
		    status = IIAPI_ST_OUT_OF_MEMORY;
		    success = FALSE;
		}
	    }
	    break;

	case NS_CA_CBOK :
	    /*
	    ** Callback with success.  Note that
	    ** this will pick up the most severe
	    ** status from the errors associated
	    ** with the event handle.
	    */
	    if ( connHndl->ch_callback )
	    {
		IIapi_appCallback(connHndl->ch_parm, ev_hndl, IIAPI_ST_SUCCESS);
		connHndl->ch_callback = FALSE;
	    }
   	    break;

	case NS_CA_CBIF :
	    /*
	    ** API function called in wrong state.
	    */
	    if ( ! IIapi_localError( sm_hndl, E_AP0006_INVALID_SEQUENCE, 
				     II_SS5000R_RUN_TIME_LOGICAL_ERROR,
				     IIAPI_ST_FAILURE ) )
		status = IIAPI_ST_OUT_OF_MEMORY;
	    else
		status = IIAPI_ST_FAILURE;

	    /*
	    ** This may not have been a connection
	    ** related function, and we may have a
	    ** callback saved on the connection
	    ** handle, so we carefully make the
	    ** callback to the caller making sure
	    ** not to disturb the connection handle.
	    */
	    IIapi_appCallback( (IIAPI_GENPARM *)parmBlock, sm_hndl, status );

	    /*
	    ** We must return the failure here rather
	    ** than following the normal exit route
	    ** to ensure that the connection handle
	    ** callback does not get made.
	    */
	    return( FALSE );

	case NS_CA_CBAB :
	    /*
	    ** Callback with connection failure.
	    */
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s: Connection aborted\n", ns_conn_sm.sm_id );

	    if ( connHndl->ch_callback )
	    {
		if ( ! IIapi_localError( sm_hndl, E_AP0001_CONNECTION_ABORTED, 
					 II_SS08006_CONNECTION_FAILURE,
					 IIAPI_ST_FAILURE ) )
		    status = IIAPI_ST_OUT_OF_MEMORY;
		else
		    status = IIAPI_ST_FAILURE;

		IIapi_appCallback( connHndl->ch_parm, sm_hndl, status );
		connHndl->ch_callback = FALSE;
	    }
	    break;

	case NS_CA_CBDC:
	    /*
	    ** API abort function is called while disconnect is in
	    ** progress. Callback with connection failure.
	    */
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s: Disconnec in progress\n", ns_conn_sm.sm_id );

	    if ( connHndl->ch_callback )
	    {
	    	if ( ! IIapi_localError( sm_hndl, E_AP0018_INVALID_DISCONNECT, 
				     II_SS5000R_RUN_TIME_LOGICAL_ERROR,
				     IIAPI_ST_FAILURE ) )
		    status = IIAPI_ST_OUT_OF_MEMORY;
	    	else
		    status = IIAPI_ST_FAILURE;

		IIapi_appCallback( connHndl->ch_parm, sm_hndl, status );
		connHndl->ch_callback = FALSE;
	    }
	    break;
    }

    /*
    ** If we couldn't complete the action, callback with failure.
    */
    if ( ! success  &&  connHndl->ch_callback )
    {
	IIapi_appCallback( connHndl->ch_parm, ev_hndl, status );
	connHndl->ch_callback = FALSE;
    }

    return( success );
}

