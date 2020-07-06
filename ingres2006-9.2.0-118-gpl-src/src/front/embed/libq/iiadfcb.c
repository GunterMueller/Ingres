/*
** Copyright (c) 2004 Ingres Corporation
*/

# include       <compat.h>
# include       <me.h>
# include	<gl.h>
# include	<st.h>
# include	<er.h>
# include	<sl.h>
# include	<iicommon.h>
# include       <generr.h>
# include	<adf.h>
# include	<iisqlca.h>
# include       <iilibq.h>
# include       <erlq.h>

/*
** Name: iiadfcb.c - Set up multiple ADF control blocks for LIBQ
**
** Description:
**	This is an LIBQ specific interface to the FEadfcb routine to allow
**	multiple ADF control blocks to be created. FEadfcb only allows
**	one ADF CB. This routine will setup an ADF control block for
**	the current database session if an ADF CB does not already exist.
**
**	Multiple ADF control blocks are required for LIBQ so that each
**	multiple session will have it's own ADF control block. When a 
**	connection is done, command flags such as '-f', '-i' are stored
**	in the FE ADF control block. Each session needs to store which
**	ADF command flags were given for a database connection;
**	otherwise, subsequent connections will lose any ADF command flags
**	specified. Also, multiple ADF control blocks are required because
**	at connection time, the protocol level is stored in the ADF CB.
**	This protocol level is used to determine if decimal is supported.
**	Multiple ADF CBs are needed for decimal interoperability to work
**	for multiple sessions.
**
** This file defines:
**
**	IILQasAdfcbSetup	- Setup an ADF control block for a session
**				  if one has not been set up yet.
**
** History:
**	05-jan-1993 	(teresal)
**		Written.
**	18-Dec-97 (gordy)
**	    Added support for multi-threaded applications.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/

static		ADF_CB	*adf_cb = NULL;
FUNC_EXTERN	ADF_CB	*FEadfcb();

/*
** Name: IILQasAdfcbSetup
**
** Description:
**	Setup an ADF Control block.
**
**	Multiple sessions are stored as a linked list of session control 
**	blocks. However, there always exists a default session block.
**	A static ADF control block is set up for the default session
**	block while ADF control blocks for each session in the link list
**	are dynamically allocated. There must always be a static ADF 
**	control block setup before a dynamic ADF CB can be allocated. The
**	static ADF CB is setup using the FEadfcb() routine which initializes 
**	all default values for the ADF CB. When allocating a dynamic ADF CB
**	for a session, the default values are copied from the static ADF CB.
**
** Inputs:
**	thr_cb		Thread-local-storage control block.
**
** Outputs:
**	None.
**
** Returns:
**	STATUS
**	 	OK 	- ADF Control Block was succesfully allocated.
**		FAIL 	- Failed to allocate an ADF Control Block.
**
** Side Effects:
**	Allocates an ADF control block for the current session (or
**	for the default saved session) if one does not exist.
**
** History:
**	05-jan-1993	(teresal)
**		Written.
*/

STATUS
IILQasAdfcbSetup( II_THR_CB *thr_cb )
{
    II_LBQ_CB	*IIlbqcb = thr_cb->ii_th_session;
    II_LBQ_CB	*def = &thr_cb->ii_th_defsess;

    /*
    ** Setup the static ADF CB.
    */
    /* ??? Semaphore protect adf_cb/FEadfcb() */
    if ( ! adf_cb  &&  ! (adf_cb = FEadfcb()) )	
    {
        IIlocerr(GE_NO_RESOURCE, E_LQ0003_ADFINIT, II_ERR, 0, (char *)0);
	return FAIL;
    }	

    /*	
    ** Default sessions use the static ADF CB.
    */
    if ( ! def->ii_lq_adf )  def->ii_lq_adf = adf_cb;

    /*
    ** Real sessions receive their own copy of the ADF CB.
    */
    if ( ! IIlbqcb->ii_lq_adf )
    {
	if ((IIlbqcb->ii_lq_adf = (ADF_CB *)MEreqmem((u_i4)0, 
		(u_i4)sizeof(ADF_CB), TRUE, (STATUS *) NULL)) == NULL)
	{
	    char ebuf[10];	

	    CVna(IIlbqcb->ii_lq_sid, ebuf);	
	    IIlocerr(GE_NO_RESOURCE, E_LQ00E8_ADFALLOC, II_ERR, 1, ebuf);
	    return FAIL;
	}
	/* Fill with the default values of the static ADF control block */

	MEcopy((PTR)def->ii_lq_adf, (u_i2)sizeof(ADF_CB), 
	       (PTR)IIlbqcb->ii_lq_adf);
    }	

    return OK;
}

/*
** Name: IILQasAdfcbChange
**
** Description:
**	Change the date_format on the fly.
**
**	Multiple sessions are stored as a linked list of session control 
**	blocks. However, there always exists a static default session block.
**	A static ADF control block is set up for the static default session
**	block while ADF control blocks for each session in the link list
**	are dynamically allocated. 
**	This routine only changes the current session date format and
**	the default session date format. 
** Inputs:
**	Date_format string.
**
** Outputs:
**	None.
**
** Returns:
**	STATUS
**	 	OK 	- ADF Control Block was succesfully allocated.
**		FAIL 	- Failed to allocate an ADF Control Block.
**
** Side Effects:
**	Changes the ADF control block date format for the current session (and
**	for the default saved session).  You must issue a set date_format
**	statement to change the back-end date format. Otherwise all insert,
**	update date will be messed up.
**
** History:
**	05-jun-1997	(nanpr01)
**	    Written on request for karl schendel.
**	16-jun-1997 (nanpr01)
**	    Take out the dmy, mdy and ymd support since they are not supported
**	    in set date_format command.
**	24-Jun-1999 (shero03)
**	    Support ISO4 date.
*/
STATUS
IILQasAdfcbChange(char *date_format)
{
    II_THR_CB	*thr_cb = IILQthThread();
    II_LBQ_CB	*IIlbqcb = thr_cb->ii_th_session;
    II_LBQ_CB	*def = &thr_cb->ii_th_defsess;

    if (date_format != NULL)
    {
	CVlower(date_format);
	if ( STequal(date_format, ERx("multinational")) )
	{
	    IIlbqcb->ii_lq_adf->adf_dfmt = DB_MLTI_DFMT;
	    def->ii_lq_adf->adf_dfmt = DB_MLTI_DFMT;
	}
	else if ( STequal(date_format, ERx("multinational4")) )
	{
	    IIlbqcb->ii_lq_adf->adf_dfmt = DB_MLT4_DFMT;
	    def->ii_lq_adf->adf_dfmt = DB_MLT4_DFMT;
	}
	else if (STequal(date_format, ERx("finland")) || 
		 STequal(date_format, ERx("sweden")))
	{
	    IIlbqcb->ii_lq_adf->adf_dfmt = DB_FIN_DFMT;
	    def->ii_lq_adf->adf_dfmt = DB_FIN_DFMT;
	}
	else if ( STequal(date_format, ERx("iso4")) )
	{
	    IIlbqcb->ii_lq_adf->adf_dfmt = DB_ISO4_DFMT;
	    def->ii_lq_adf->adf_dfmt = DB_ISO4_DFMT;
	}
	else if ( STequal(date_format, ERx("iso")) )
	{
	    IIlbqcb->ii_lq_adf->adf_dfmt = DB_ISO_DFMT;
	    def->ii_lq_adf->adf_dfmt = DB_ISO_DFMT;
	}
	else if ( STequal(date_format, ERx("us")) )
	{
	    IIlbqcb->ii_lq_adf->adf_dfmt = DB_US_DFMT;
	    def->ii_lq_adf->adf_dfmt = DB_US_DFMT;
	}
	else  if ( STequal(date_format, ERx("german")) )
	{
	    IIlbqcb->ii_lq_adf->adf_dfmt = DB_GERM_DFMT;
	    def->ii_lq_adf->adf_dfmt = DB_GERM_DFMT;
	}
	else
	{
	    /* donot do anything */
	    return(E_DB_ERROR);
	}
    }
    return(E_DB_OK);
}
