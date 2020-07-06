#include	<compat.h>
#include	<gl.h>
#include    	<ssdef.h>
#include	<starlet.h>


/*{
** Name: GCEXEC.C - drive GCA events
**
** Description:
**	Contains:
**	    GCexec - drive I/O completions
**	    GCshut - stop GCexec
** History:
**	27-Nov-89 (seiwald)
**	    Written.
**	11-Dec-89 (seiwald)
**	    Turned back into a CL level loop after realizing that VMS
**	    doesn't return between each I/O completion.
**	21-Sep-90 (jorge)
**	    Added 6.3/6.4 CL Spec comments.
**	    GCexec() now uses sys$setast rather than EXinterrupt(ON) and
**	    then resets AST environment to initial state.
**      16-jul-93 (ed)
**	    added gl.h
**	19-jul-2000 (kinte01)
**	   Correct prototype definitions by adding missing includes
*/	


/*{
** Name: GCexec - Drive Asynchronous Events
** 
** Description:
** 
** GCexec drives the event handling mechanism in several GCA
** applications.  The application typically posts a few initial
** asynchronous GCA CL requests (and in the case of the Comm Server, a few
** initial GCC CL requests), and then passes control to GCexec.  GCexec
** then allows asynchronous completion of events to occur, calling the
** callback routines listed for the asynchronous requests; these callback
** routines typically post more asynchronous requests.  This continues
** until one of the callback routines calls GCshut.
** 
** If the OS provides the event handling mechanism, GCexec presumably
** needs only to enable asynchronous callbacks and then block until
** GCshut is called.  If the CL must implement the event handling
** mechanism, then GCexec is the entry point to its main loop.
** 
** The event mechanism under GCexec must not interrupt one callback
** routine to run another: once a callback routine is called, the event
** mechanism must wait until the call returns before driving another
** callback.  That is, callback routines run atomically.
** 
** Inputs:
**     	none
** 
** Outputs:
** 	none
** 
** Returns:
** 	none
** 
** Exceptions:
** 	none
** 
** Side Effects:
** 	All events driven.
**
** History:
**	27-Nov-89 (seiwald)
**	    Written.
*/	

GCexec()
{
long 	old_ast;

	old_ast = sys$setast((char)TRUE);

	sys$hiber();

	if(old_ast == SS$_WASCLR)
		sys$setast((char)FALSE);	/* Disable ASTs again */
}


/*{
** Name: GCshut - Stop GCexec
** 
** Description:
** 
** GCshut causes GCexec to return.  When an application uses GCexec to drive
** callbacks of asynchronous GCA CL and GCC CL requests, it terminates
** asynchronous activity by calling GCshut.  When control returns to GCexec,
** it itself returns.  The application then presumably exits.
** 
** If the application calls GCshut before passing control to GCexec, GCexec
** should return immediately.  This can occur if the completion of asynchronous
** requests occurs before the application calls GCexec.
** 
** Inputs:
**     	none
** 
** Outputs:
** 	none
** 
** Returns:
** 	none
** 
** Exceptions:
** 	none
** 
** Side Effects:
** 	GCexec returns.
**
** History:
**	11-Dec-89 (seiwald)
**	    Written.
*/	

GCshut()
{
	sys$wake(0, 0);
}
