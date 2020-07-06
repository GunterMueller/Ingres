/*
** Copyright (c) 1992, 2007 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <er.h>
#include    <ex.h>
#include    <me.h>
#include    <nm.h>
#include    <pc.h>
#include    <st.h>
#include    <tr.h>
#include    <iosbdef.h>
#include    <descrip.h>
#include    <exhdef.h>
#include    <iodef.h>
#include    <psldef.h>
#include    <ssdef.h>
#include    <lib$routines.h>
#include    <starlet.h>
#include    <vmstypes.h>

#include    <csinternal.h>

#include    <ints.h>
#include    <chfdef.h>

/*
** Name: CSCPRES.C	- Cross-process resume support
**
** Description:
**	This file contains the routines which implement cross-process resume.
**
**	Cross-process resume is used by the Logging and Locking components of
**	DMF.
**
**	    CScp_resume	    - Resume (a thread in) a process
**
**	VMS Implementation:
**
**	    Each process which calls CSinitiate() establishes a mailbox named
**	    II_CPRES_xx_pid, and queues a READ on that mailbox. It then
**	    immediately deletes the mailbox, so that it'll disappear when the
**	    process dies.
**
**	    If another process wishes to resume (a thread in) this process, that
**	    other process prepares a "wakeup message" containing the session ID
**	    of the thread to awaken, assigns a channel to this process's
**	    mailbox if it hasn't yet done so, and sends the wakeup message to
**	    this process.
**
**	    When the read completes, the completion routine (running in a user
**	    mode AST) calls CSresume on behalf of the indicated thread and
**	    queues the next READ on the mailbox.
**
**	    Internal-to-the-CL subroutines:
**		CS_cp_mbx_create	- mailbox creation & initialization
**		cpres_mbx_assign	- get channel to other process's mbox,
**					  assigning the channel if needed.
**		cpres_mbx_complete	- QIO completion handler.
**		cpres_q_read_io		- queue a read IO.
**
**	TO DO: when a sibling process dies, we must de-assign its mailbox and
**		recover the space in our remembered channels array.
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	9-oct-1992 (bryanp)
**	    Using an IOSB on the stack is not safe for an async qio call,
**	    because the function may return before the i/o completes,
**	    and later when the I/O completes it will write into that
**	    iosb. So use a global variable for the IOSB.
**	19-oct-1992 (bryanp)
**	    CSresume expects to be called at AST level. Oblige it by invoking it
**	    via sys$dclast().
**	20-oct-1992 (bryanp)
**	    Back out the DCLAST change; CSresume can now be called at normal
**	    level.
**	14-dec-1992 (bryanp)
**	    ERsend calls should be ERlog calls. If caller gone, log it but don't
**	    die; this is not a fatal error.
**      16-jul-93 (ed)
**	    added gl.h
**	29-sep-1993 (walt)
**	    In Cscp_resume get an event flag number from lib$get_ef rather than
**	    use event flag zero in the sys$qio call.  
**	18-oct-1993 (rachael)
**	    Call lib$signal(SS$_DEBUG) only when compiled with xDEBUG flag.
**	    Modified TRdisplay statement to reflect calling function.
**	23-may-1994 (bryanp) B60736
**	    Added clean_channels() routine and called it periodically to
**		ensure that a process doesn't accumulate channels to dead
**		processes indefinitely.
**	09-mar-1998 (kinte01)
**	    Cross-integrate change 433580 from VAX CL into AXP VMS CL
**	16-Nov-1998 (jenjo02)
**	    CScp_resume() prototype changed to pass CS_CPID * instead of PID, SID.
**	30-dec-1999 (kinte01)
**	   Add missing includes for ANSI C compiler
**	01-dec-2000	(kinte01)
**		Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**		from VMS CL as the use is no longer allowed
**	28-may-2003 (horda03) Bug 109272
**	    When the RCP actions a Cross-Process resume an ACCVIO can
**	    occur.
**	28-aug-2003 (abbjo03)
**	    Changes to use VMS headers provided by HP.
**    18-aug-2004 (horda03) Bug 112983/INGSRV3009
**        Create the mailbox with sufficient space to handle simultaneous resumes
**        for a minimum of 5 sessions (or 1 tenth of the number of sessions).
**        Thus preventing other processes entering RWMBX state.
**	02-may-2007 (jonj/joea)
**	    Use CS_TAS/CS_ACLR instead of CSp/v_semaphore for the
**	    cpres_channels_sem.
*/

GLOBALREF CS_SYSTEM           Cs_srv_block;

#define CS_CP_MIN_MSGS	    5
# define VMS_OK(x)   (((x) & 1) != 0)
# define VMS_FAIL(x) (((x) & 1) == 0)

typedef struct
{
    i4		wakeup_sid;

    /* horda03 - Added to track Cross Process problem */
    PID         wakeup_pid;
    i4          from_sid;
    PID         from_pid;
    i4          sender_in_ast;
} CS_CP_WAKEUP_MSG;


static	II_VMS_CHANNEL	    cpres_mbx_chan;
static	IOSB		    cpres_iosb;
static	IOSB		    cpres_write_iosb;
static	CS_CP_WAKEUP_MSG    cpres_msg_buffer;
static	CS_ASET		    cpres_channels_sem;
static	i4		    cpres_num_channels_assigned;
static	struct {PID	pid; II_VMS_CHANNEL chan;} cpres_channels[1000];

static  EX_CONTEXT cpres_context;

static	void	cpres_q_read_io(void);
static	void	cpres_mbx_complete( PTR astprm );
static	STATUS	clean_channels(void);
static	STATUS	cpres_mbx_assign(PID pid, i4 *mbox_chan);

/*
** Name: CS_cp_mbx_create	- mailbox creation and initialization
**
** Description:
**	This subroutine is called from CSinitiate().
**
**	It does the following:
**	    a) establishes a mailbox, with the name II_CPRES_xx_pid, where
**		xx is the (optional) installation code, and pid is the
**		process ID in hex.
**	    b) queues a read on the mailbox, with completion routine set to
**		CS_cp_mbx_complete
**	    c) deletes the mailbox, so it'll go away when the process dies.
**
** Inputs:
**	num_sessions    - Number of sessions for the process.
**
** Outputs:
**	sys_err		- reason for error
**
** Returns:
**	OK, !OK
**
** Side Effects:
**	Sets cpres_mbx_chan to the mailbox's channel
**	Defines the system-wide logical name II_CPRES_xx_pid
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
*/
STATUS
CS_cp_mbx_create(i4 num_sessions, CL_ERR_DESC	*sys_err)
{
    struct	dsc$descriptor_s    name_desc;
    i4	vms_status;
    char	mbx_name[100];
    char	*inst_id;
    PID		pid;
    STATUS	status;
    i4          num_messages = max(CS_CP_MIN_MSGS, num_sessions/10);

    /*
    ** Build the mailbox logical name:
    */
    PCpid(&pid);
    NMgtAt("II_INSTALLATION", &inst_id);
    if (inst_id && *inst_id)
	STprintf(mbx_name, "II_CPRES_%s_%x", inst_id, (i4)pid);
    else
	STprintf(mbx_name, "II_CPRES_%x", (i4)pid);

    name_desc.dsc$a_pointer = mbx_name;
    name_desc.dsc$w_length = STlength(mbx_name);
    name_desc.dsc$b_dtype = DSC$K_DTYPE_T;
    name_desc.dsc$b_class = DSC$K_CLASS_S;

    vms_status = sys$crembx(
		1,		    /* Mailbox is "permanent" */
		&cpres_mbx_chan,    /* where to put channel */
		(i4)sizeof(CS_CP_WAKEUP_MSG),
				    /* maximum message size */
		sizeof(CS_CP_WAKEUP_MSG) * num_messages,    /* buffer quota */
		0,		    /* prot mask = all priv */
		PSL$C_USER,	    /* acmode */
		&name_desc);	    /* logical name descriptor */

    if (VMS_FAIL(vms_status))
    {
	sys_err->error = vms_status;
	if (vms_status == SS$_NOPRIV)
	    return (E_CS00F8_CSMBXCRE_NOPRIV);
	else
	    return (E_CS00F7_CSMBXCRE_ERROR);
    }

    cpres_q_read_io();

    /* Mark for deletion, so it disappears when we exit. */

    sys$delmbx(cpres_mbx_chan);

    cpres_channels_sem = 0;

    cpres_num_channels_assigned = 0;

    return (OK);
}

/*
** Name: CScp_resume	- Resume (a thread in) a process
**
** Description:
**	This routine resumes the indicated thread in the indicated process.
**
**	If the indicated process is this process, then this is a simple
**	CSresume operation. If the indicated process is another process, then
**	that process must be notified that it should CSresume the indicated
**	thread.
**
** Inputs:
**	cpid		pointer to CS_CPID with
**	   .pid		- the indicated process
**	   .sid		- the indicated session
**
** Outputs:
**	None
**
** Returns:
**	void
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	9-oct-1992 (bryanp)
**	    Use global IOSB, not stack IOSB, so that when QIO completes
**	    some time from now it will not overwrite arbitrary stack stuff.
**	19-oct-1992 (bryanp)
**	    CSresume expects to be called at AST level. Oblige it by invoking it
**	    via sys$dclast().
**	20-oct-1992 (bryanp)
**	    Back out the DCLAST change; CSresume can now be called at normal
**	    level.
**	14-dec-1992 (bryanp)
**	    ERsend() calls should be ERlog() calls.
**	29-sep-1993 (walt)
**	    Get an event flag number from lib$get_ef rather than use event flag
**	    zero in the sys$qio call.  
**	18-oct-1993 (rachael)
**	    Call lib$signal(SS$_DEBUG) only when compiled with xDEBUG flag.
**	16-Nov-1998 (jenjo02)
**	    Prototype changed to pass CS_CPID * instead of PID, SID.
*/
void
CScp_resume( CS_CPID *cpid )
{
    i4		    vms_status;
    CS_CP_WAKEUP_MSG	    wakeup_msg;
    i4		    mbox_chan;
    char		    msg_buf[100];
    CL_ERR_DESC		    local_sys_err;
    II_VMS_EF_NUMBER	    efn;
    i4		    efn_status;

    if (cpid->pid == Cs_srv_block.cs_pid)
    {
	CSresume(cpid->sid);
    }
    else
    {
	vms_status = 0;	    /* initialize to failure */

	if (cpres_mbx_assign(cpid->pid, &mbox_chan) == OK)
	{
	    wakeup_msg.wakeup_sid = cpid->sid;

            /* horda03 - Fill in details to help track Cross-Process
            **           ACCVIO problem.
            */
            wakeup_msg.wakeup_pid    = cpid->pid;
            wakeup_msg.from_sid      = Cs_srv_block.cs_current;
            wakeup_msg.from_pid      = Cs_srv_block.cs_pid;
            wakeup_msg.sender_in_ast = lib$ast_in_prog();
	    
	    if ((efn_status = lib$get_ef(&efn)) != SS$_NORMAL)
		efn = 0;    /* For now, fall back to efn zero if there's no others. */
	    vms_status = sys$qio(efn, mbox_chan, IO$_WRITEVBLK,
				&cpres_write_iosb,
				NULL, NULL, &wakeup_msg, sizeof(wakeup_msg),
				0, 0, 0, 0);
	    lib$free_ef(&efn);
	    if (VMS_FAIL(vms_status))
	    {
		STprintf(msg_buf, "Error (%x) queueing write to %x",
			vms_status, cpid->pid);
		ERlog(msg_buf, STlength(msg_buf), &local_sys_err);
	    }
	}
	else
	{
	    STprintf(msg_buf, "Unable to assign channel to %x", cpid->pid);
	    ERlog(msg_buf, STlength(msg_buf), &local_sys_err);
	}

	if (vms_status == 0)
	{
	    STcopy("Ignoring error in assigning mailbox", msg_buf);
	    ERlog(msg_buf, STlength(msg_buf), &local_sys_err);
	    /*
	    ** The process we were going to send this message to will probably
	    ** "hang", which at least allows some sort of diagnosis. Killing
	    ** ourselves at this point is less useful, since it tends to crash
	    ** the entire installation.
	    */
	}
	else if (VMS_FAIL(vms_status))
	{
	    TRdisplay("CScp_resume QIO to %x failed with status %x\n",
		    cpid->pid, vms_status);
#ifdef xDEBUG
	    lib$signal(SS$_DEBUG);
#endif
	    PCexit(FAIL);
	}
    }
    return;
}

/*
** Name: cpres_q_read_io	- queue a read I/O on our CP mailbox
**
** Description:
**	This subroutine queues an I/O on this process's CPRES mailbox.
**
** Inputs:
**	None
**
** Outputs:
**	None
**
** Returns:
**	void
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	18-oct-1993 (rachael)
**	    Call lib$signal(SS$_DEBUG) only when compiled with xDEBUG flag.
**	    Modified TRdisplay statement to reflect calling function.
*/
static void
cpres_q_read_io(void)
{
    i4	vms_status;

    vms_status = sys$qio((i4)0, cpres_mbx_chan, IO$_READVBLK, &cpres_iosb,
	    cpres_mbx_complete, (__int64)&cpres_iosb, &cpres_msg_buffer,
	    sizeof(CS_CP_WAKEUP_MSG), 0, 0, 0, 0);

    if (VMS_FAIL(vms_status))
    {
	TRdisplay("cpres_q_read_io QIO failed, status %x\n", vms_status);
#ifdef xDEBUG
	lib$signal(SS$_DEBUG);
#endif
	PCexit(FAIL);
    }
    return;
}

/*
** Name: cpres_mbx_assign	- get channel to target process, assign if need
**
** Description:
**	This subroutine looks up the channel to the target process. If we have
**	not yet assigned a channel to the target process, then we assign one
**	and remember it. The resulting channel is returned.
**
** Inputs:
**	pid			- the target process's pid
**
** Outputs:
**	mbox_chan		- channel to the target process
**
** Returns:
**	OK, !OK
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**	14-dec-1992 (bryanp)
**	    ERsend() calls should be ERlog() calls.
**	23-may-1994 (bryanp) B60736
**	    Added clean_channels() routine and called it periodically to
**		ensure that a process doesn't accumulate channels to dead
**		processes indefinitely.
**	02-May-2007 (jonj)
**	    Use of CSp/v_semaphore is prohibited from within an AST as
**	    it corrupts cs_inkernel and the ready queues. Use CS_ASET/ACLR
**	    instead.
*/
static STATUS
cpres_mbx_assign(PID pid, i4 *mbox_chan)
{
    i4	    i;
    struct	dsc$descriptor_s    name_desc;
    i4	vms_status;
    char	mbx_name[100];
    char	*inst_id;
    char	msg_buf[250];
    STATUS	status;
    STATUS	cl_status;
    PID		my_pid;
    CL_ERR_DESC	local_sys_err;
    i4		ReenableASTs;

    /* Disable AST delivery while we hold the sem */
    ReenableASTs = (sys$setast(0) == SS$_WASSET);

    /* Loop until we get the sem */
    while ( !CS_TAS(&cpres_channels_sem) );

    for (i = 0; i < cpres_num_channels_assigned; i++)
    {
	if (cpres_channels[i].pid == pid)
	{
	    *mbox_chan = cpres_channels[i].chan;
	    CS_ACLR(&cpres_channels_sem);
	    if ( ReenableASTs )
	        sys$setast(1);
	    return (OK);
	}
    }

    status = clean_channels();
    if (status)
    {
	CS_ACLR(&cpres_channels_sem);
	if ( ReenableASTs )
	    sys$setast(1);
	return (status);
    }

    if (cpres_num_channels_assigned <
			    (sizeof(cpres_channels)/sizeof(cpres_channels[0])))
    {
	/*
	** New process, and room remains in the channel array, so assign a
	** channel.
	*/
	NMgtAt("II_INSTALLATION", &inst_id);
	if (inst_id && *inst_id)
	    STprintf(mbx_name, "II_CPRES_%s_%x", inst_id, (i4)pid);
	else
	    STprintf(mbx_name, "II_CPRES_%x", (i4)pid);

	name_desc.dsc$a_pointer = mbx_name;
	name_desc.dsc$w_length = STlength(mbx_name);
	name_desc.dsc$b_dtype = DSC$K_DTYPE_T;
	name_desc.dsc$b_class = DSC$K_CLASS_S;

	cpres_channels[cpres_num_channels_assigned].pid = pid;

	vms_status = sys$assign(&name_desc,
			    &cpres_channels[cpres_num_channels_assigned].chan,
			    0, 0);
	if (VMS_OK(vms_status))
	{
	    *mbox_chan = cpres_channels[cpres_num_channels_assigned].chan;
	    cpres_num_channels_assigned++;
	    CS_ACLR(&cpres_channels_sem);
	    if ( ReenableASTs )
		sys$setast(1);
	    return (OK);
	}
	else
	{
	    STprintf(msg_buf,
		    "cpres_mbx_assign: Error (%x) assigning channel to %s",
		    vms_status, mbx_name);
	    ERlog(msg_buf, STlength(msg_buf), &local_sys_err);
	}
    }
    else
    {
	/*
	** No room left in channels array. One possibility is to go through
	** the array looking for assigned channels to mailboxes of dead
	** processes and clean those up.
	*/
	STcopy("PANIC! No room left in channels array!", msg_buf);
	ERlog(msg_buf, STlength(msg_buf), &local_sys_err);
    }

    CS_ACLR(&cpres_channels_sem);
    if ( ReenableASTs )
	sys$setast(1);
    return (FAIL);
}

/*
** Name: cpres_catch      - Catch any exceptions.
**
** Description:
**       This routine mirrors the action of EXcatch(). Because
**       cpres_mbx_complete() functions in an AST it is not
**       possible to use the normal EX routines to trap exceptions.
**       In an AST, there may not be a "cs_current" session, and
**       the so the EXdeclare() call will ACCVIO, causing the
**       server to enter an infinite loop.
**
**       This routine is envoked when an ACCIO occurs trying to resume
**       a session, it will wind back the thread to the point where
**       the exception handler was declared.
**
**       Because the Cross Process resume is done via an AST, and is
**       thus synchronous, the AST has exclusive access to the static
**       variable cpres_context.
**
**       For more information on the steps being performed to return
**       to the CSCPdeclare() call, see EXcatch().
**
** Inputs:
**       chf$signal_array
**       chf$mech_array
**
** Outputs:
**       EXDECLARE
**
** Side effects:                          
**       Control returned to point where exception occured.
**
** History:
**       20-aug-2003 (horda03)
**            Created.
*/
static int
cpres_catch( struct chf$signal_array *sigs, struct chf$mech_array *mechs)
{
   int64  establisher_fp      = mechs->chf$q_mch_frame & 0xffffffff;
   int64  *jmpbuf             = cpres_context.iijmpbuf;
   int    establishers_handle = lib$get_invo_handle( jmpbuf );
   uint64 invo_value	      = EX_DECLARE;
   uint64 invo_mask           = 0x0FFFCul | ((uint64) 0X03FCul << (BITS_IN(uint64) / 2));
   
   lib$put_invo_registers( establishers_handle, jmpbuf, &invo_mask );

   /* Return to CSCPdeclare() call, but EX_DECLARE returned */

   sys$goto_unwind(&establishers_handle, jmpbuf+2, &invo_value, 0);

   /* Should never reach here */

   TRdisplay( "%@ cscpres_catch:: SYS$GOTO_UNWIND failed\n\n");

   PCexit( FAIL );

   return SS$_RESIGNAL;
}

/*
** Name: cpres_mbx_complete	- QIO completion handler.
**
** Description:
**	This subroutine is called as a user-mode AST routine when the read I/O
**	on the cross-process resume mailbox completes.
**
**	It calls CSresume on behalf of the indicated session ID and schedules
**	the next read I/O on the mailbox.
**
** Inputs:
**	astprm			- ast parameter
**
** Outputs:
**	None
**
** Returns:
**	void
**
** Side Effects:
**	A session is resumed
**	Another I/O is queued.
**
** History:
**	Summer, 1992 (bryanp)
**	    Working on the new portable logging and locking system.
**      28-may-2003 (horda03) Bug 109272
**          Set up an exception handler, to catch any ACCVIO's caused
**          by invalid SIDs.
*/
static void
cpres_mbx_complete( PTR astprm )
{
    CS_CPID	cpid;

    if (cpres_msg_buffer.wakeup_pid != Cs_srv_block.cs_pid)
    {
       TRdisplay( "%@ cpres::Wakeup received for wrong process. W_PID = %08x W_SID = %08x, S_PID = %08x, S_SID = %08x, S_AST = %d\n",
                  cpres_msg_buffer.wakeup_pid, cpres_msg_buffer.wakeup_sid,
                  cpres_msg_buffer.from_pid, cpres_msg_buffer.from_sid,
                  cpres_msg_buffer.sender_in_ast );

       TRdisplay( "cpres::Attempting to resume correct session.\n");

	cpid.pid = cpres_msg_buffer.wakeup_pid;
	cpid.sid = cpres_msg_buffer.wakeup_sid;
	CScp_resume(&cpid);
    }
    else
    {
       /* Bug 109272; catch any exceptions.
       **   CSCPdeclare() is similar to EXdeclare(), but as we are in an AST
       **   there may not be a current session (Cs_srv_block.cs_current == 0)
       **   which would lead to an infinite ACCVIO loop. So instead use a
       **   static EX_CONTEXT block (cpres_context) to store the return address
       **   and let the EXCEPTION call the local function cpres_catch, which
       **   will return to the "if" statement below with EX_DECLARE if an
       **   exception occurs.
       */
#define CSCPdeclare( y ) ( VAXC$ESTABLISH( cpres_catch ), \
                           EXgetctx( (y)->iijmpbuf))

       if (CSCPdeclare( &cpres_context ) )
       {
          TRdisplay( "%@ cpres::Exception occured for W_PID = %08x W_SID = %08x, S_PID = %08x, S_SID = %08x, S_AST = %d\n",
                     cpres_msg_buffer.wakeup_pid, cpres_msg_buffer.wakeup_sid,
                     cpres_msg_buffer.from_pid, cpres_msg_buffer.from_sid,
                     cpres_msg_buffer.sender_in_ast );
       }
       else
       {

          CSresume(cpres_msg_buffer.wakeup_sid);
       }

#define CSCPdelete() (VAXC$ESTABLISH( NULL ))

       CSCPdelete();
    }
    cpres_q_read_io();
}

/*
** Name: clean_channels		- garbage collect channels to dead processes
**
** Description:
**	Called whenever we're about to assign a channel to a new process.
**	Before assigning the new channel, we compact the existing channels
**	array by reclaiming any channels to old, dead processes.
**
** Inputs:
**	None
**
** Outputs:
**	None
**
** Returns:
**	STATUS
**
** History:
**	23-may-1994 (bryanp)
**	    Created as part of resolving B60736.
*/
static STATUS
clean_channels(void)
{
    i4	i;
    i4	j;
    i4	s;
    char	msg_buf[250];
    CL_ERR_DESC	local_sys_err;

    for (i = 0; i < cpres_num_channels_assigned; i++)
    {
	if (PCis_alive(cpres_channels[i].pid) == FALSE)
	{
	    TRdisplay("%@ cscpres: garbage collect channel to %x\n",
			cpres_channels[i].pid);
	    /*
	    ** What cleanup is required:
	    **	    1) cancel any outstanding I/O on this channel
	    **	    2) deassign the channel
	    **	    3) shift all the other channels down one in the array
	    **		(ugh, but we don't do this very often)
	    **	    4) back 'i' off by one so that we don't skip any
	    **		channels in the loop
	    **	    5) back cpres_num_channels_assigned off by one to
	    **		reflect the cleaned-up channel.
	    */
	    s = sys$cancel(cpres_channels[i].chan);
	    if ((s & 1) == 0)
	    {
		STprintf(msg_buf, "Error %x cancelling channel %x",
			    s, cpres_channels[i].chan);
		ERlog(msg_buf, STlength(msg_buf), &local_sys_err);
		TRdisplay("%s\n", msg_buf);
		return (FAIL);
	    }
	    s = sys$dassgn(cpres_channels[i].chan);
	    if ((s & 1) == 0)
	    {
		STprintf(msg_buf, "Error %x releasinging channel %x",
			    s, cpres_channels[i].chan);
		ERlog(msg_buf, STlength(msg_buf), &local_sys_err);
		TRdisplay("%s\n", msg_buf);
		return (FAIL);
	    }

	    for (j = i + 1; j < cpres_num_channels_assigned; j++)
	    {
		cpres_channels[j - 1].pid  = cpres_channels[j].pid;
		cpres_channels[j - 1].chan = cpres_channels[j].chan;
	    }

	    i--;
	    cpres_num_channels_assigned--;
	}
    }
    return (OK);
}
