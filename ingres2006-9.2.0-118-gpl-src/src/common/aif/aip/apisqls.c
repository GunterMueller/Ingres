/*
** Copyright (c) 1993, 2007 Ingres Corporation All Rights Reserved.
*/

# include <compat.h>
# include <gl.h>
# include <me.h>
# include <st.h>

# include <iicommon.h>
# include <copy.h>

# include <iiapi.h>
# include <api.h>
# include <apisql.h>
# include <apichndl.h>
# include <apishndl.h>
# include <apiihndl.h>
# include <apierhnd.h>
# include <apilower.h>
# include <apigcap.h>
# include <apidatav.h>
# include <apidspth.h>
# include <apimisc.h>
# include <apitrace.h>

/*
** Name: apisqls.c
**
** Description:
**	SQL Statement State Machine.
**
** History:  Original history found in apisstbl.c (now defunct).
**	 2-Oct-96 (gordy)
**	    Replaced original SQL state machines which a generic
**	    interface to facilitate additional connection types.
**	31-Jan-97 (gordy)
**	    Added het-net descriptor to IIapi_sndGCA() and statement 
**	    handle.  Needed during COPY FROM processing.
**	13-Mar-97 (gordy)
**	    Action sequence 42 identical to AS 2 so no longer used.
**	 3-Jul-97 (gordy)
**	    Re-worked state machine initialization.  Added
**	    IIapi_sql_sinit().
**	21-Aug-98 (rajus01)
**	    Added environment handle to IIapi_cnvtGDVDescr().
**	 3-Sep-98 (gordy)
**	    Added abort state to sm interface.
**      14-Oct-98 (rajus01)
**          Ignore IIAPI_EV_PROMPT_RCVD message.
**	 9-Feb-99 (gordy)
**	    Handles in IDLE state should allow IIapi_close().
**	22-Mar-99 (rajus01)
**	    Fixed a type error. 
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	19-May-00 (gordy)
**	    We enter the INTR state when we are waiting for a response,
**	    send an ATTENTION and want to wait for the send to complete.
**	    If the receive completes first, we should go to the INTS
**	    state to wait for the send to complete.
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-Nov-1999 (hanch04)
**          First stage of updating ME calls to use OS memory management.
**          Make sure me.h is included.  Use SIZE_TYPE for lengths.
**	04-jan-2001 (somsa01 for gordy)
**	    Made changes to the state machine to allow IIapi_close()
**	    following an IIapi_query() call. This was especially made for
**	    the TNG team, since they had code which could not be changed
**	    at the time which performed this very sequence of OpenAPI
**	    calls.
**     29-Aug-201 (loera01) SIR 105641 
**          Added new state, SQL_SS_WPRC, so that the API will wait for
**          a GCA_RESPONSE (EOD) message after receiving GCA_RETPROC
**          (procedure completion) message.  This is necessary to support
**          dbprocs that return multiple rows.
**	11-Dec-02 (gordy)
**	    Tracing added for local error conditions.
**	20-Feb-03 (gordy)
**	    Fixed the length of data used when returning an object key.
**	 6-Jun-03 (gordy)
**	    Clear query info mask once data has been returned.
**	17-Oct-03 (gordy)
**	    Added INIT state to distinguish from IDLE (closed) state to
**	    allow CLOSE in INIT but not in IDLE.  Rename INTS to INTQ
**	    (interrupt query) and SMSG to INTS (interrupt send).  Added
**	    states WIC and WCC to handle INTR race condition (replaces
**	    change of 19-may-00).  Reworked cancel processing (yet again)
**	    to ensure thread affinity of GCA operations and API callbacks.
**	    Added CLOSE entries for all active query states and CANCEL
**	    entries for INIT, WPRC.  Removed CLOSE/CANCEL entries for
**	    states following CLOSE requests (invalid function error is
**	    sufficient).
**	 8-Apr-04 (gordy)
**	    To be consistent, clear query info flags along with mask.
**	28-Jul-06 (gordy)
**	    Check for XA errors in GCA_RESPONSE messages..
*/

/*
** Forward references
*/

static IIAPI_SM_OUT	*sm_evaluate( IIAPI_EVENT, IIAPI_STATE, IIAPI_HNDL *,
				      IIAPI_HNDL *, II_PTR, IIAPI_SM_OUT * );

static II_BOOL		sm_execute( IIAPI_ACTION, IIAPI_HNDL *,
				    IIAPI_HNDL *, II_PTR parmBlock );

static II_BOOL		EvaluateTuple( IIAPI_STMTHNDL *, IIAPI_GCA_BUFF * );

static II_VOID		GetResponseInfo( IIAPI_GCA_BUFF *, IIAPI_STMTHNDL * );

static IIAPI_STATUS	GetDescrInfo( IIAPI_STMTHNDL *, IIAPI_GCA_BUFF * );

static IIAPI_STATUS	SetDescrInfo( IIAPI_STMTHNDL *, IIAPI_SETDESCRPARM * );

static IIAPI_STATUS	GetCopyMap( IIAPI_STMTHNDL *, IIAPI_GCA_BUFF * );

static IIAPI_STATUS	GetPre50CopyMap( PTR, PTR, IIAPI_STMTHNDL * );

static IIAPI_STATUS	GetPost50CopyMap( PTR, PTR, IIAPI_STMTHNDL * );

/*
** Information to be exported through
** the state machine interface.
*/

/*
** SQL Statement States.
**
** The following values are used as array
** indices.  Care must be taken when adding
** new entries to ensure there are no gaps
** or duplicates and all associated arrays
** are kept in sync with these definitions.
**
** Note that the INIT state must be 0 to
** match IIAPI_IDLE.
**
** History:
**	17-Oct-03 (gordy)
**	    Added INIT to separate from IDLE, and CANCEL states
**	    WIC, WCC.  Renamed INTS to INTQ, SMSG to INTS.
**	15-Mar-07 (gordy)
**	    Added SCRL state for scrollable cursors.
*/

# define SQL_SS_INIT	((IIAPI_STATE)  0)	/* Initial */
# define SQL_SS_SQRY	((IIAPI_STATE)  1)	/* Query sent */
# define SQL_SS_DESC	((IIAPI_STATE)  2)	/* Describe query parameters */
# define SQL_SS_PARM	((IIAPI_STATE)  3)	/* Put parameters */
# define SQL_SS_SPRM	((IIAPI_STATE)  4)	/* Send query parameters */
# define SQL_SS_SENT	((IIAPI_STATE)  5)	/* Query/parameters sent */
# define SQL_SS_WQR	((IIAPI_STATE)  6)	/* Wait for query response */
# define SQL_SS_WQDR	((IIAPI_STATE)  7)	/* Wait query desc response */
# define SQL_SS_TUPL	((IIAPI_STATE)  8)	/* Get tuple data */
# define SQL_SS_WTR	((IIAPI_STATE)  9)	/* Wait for tuple response */
# define SQL_SS_WPRC	((IIAPI_STATE) 10)	/* Wait for procedure return */
# define SQL_SS_WCDR	((IIAPI_STATE) 11)	/* Wait cursor desc response */
# define SQL_SS_CURS	((IIAPI_STATE) 12)	/* Get cursor data */
# define SQL_SS_FTCH	((IIAPI_STATE) 13)	/* Fetch */
# define SQL_SS_SCRL	((IIAPI_STATE) 14)	/* Scroll */
# define SQL_SS_WFR	((IIAPI_STATE) 15)	/* Wait for fetch response */
# define SQL_SS_PART	((IIAPI_STATE) 16)	/* Partial fetch data */
# define SQL_SS_WCMR	((IIAPI_STATE) 17)	/* Wait for copy map response */
# define SQL_SS_ACKI	((IIAPI_STATE) 18)	/* Copy Into ACK sent */
# define SQL_SS_ACKF	((IIAPI_STATE) 19)	/* Copy From ACK sent */
# define SQL_SS_COPY	((IIAPI_STATE) 20)	/* Put copy data */
# define SQL_SS_CDAT	((IIAPI_STATE) 21)	/* Copy data sent */
# define SQL_SS_CRSP	((IIAPI_STATE) 22)	/* Copy response sent */
# define SQL_SS_IACK	((IIAPI_STATE) 23)	/* IACK sent */
# define SQL_SS_WAR	((IIAPI_STATE) 24)	/* Wait acknowledge response */
# define SQL_SS_CINT	((IIAPI_STATE) 25)	/* Copy interrupted */
# define SQL_SS_INTQ	((IIAPI_STATE) 26)	/* Interrupt a query */
# define SQL_SS_INTS	((IIAPI_STATE) 27)	/* Interrupt a send */
# define SQL_SS_INTR	((IIAPI_STATE) 28)	/* Interrupt a receive */
# define SQL_SS_WIR	((IIAPI_STATE) 29)	/* Wait interrupt response */
# define SQL_SS_WIC	((IIAPI_STATE) 30)	/* Wait interrupt/close  */
# define SQL_SS_CNCL	((IIAPI_STATE) 31)	/* Cancelled */
# define SQL_SS_DONE	((IIAPI_STATE) 32)	/* Done */
# define SQL_SS_WQRC	((IIAPI_STATE) 33)	/* Wait query response/close */
# define SQL_SS_CLOS	((IIAPI_STATE) 34)	/* Close cursor sent */
# define SQL_SS_WCR	((IIAPI_STATE) 35)	/* Wait close response */
# define SQL_SS_WCC	((IIAPI_STATE) 36)	/* Wait interrupt closed */
# define SQL_SS_IDLE	((IIAPI_STATE) 37)	/* Closed */

# define SQL_SS_CNT     (SQL_SS_IDLE + 1)

static char *sql_ss_id[] =
{
    "INIT", "SQRY", "DESC", "PARM", "SPRM", "SENT",
    "WQR",  "WQDR", "TUPL", "WTR",  "WPRC",
    "WCDR", "CURS", "FTCH", "SCRL", "WFR",  "PART",
    "WCMR", "ACKI", "ACKF", "COPY", "CDAT",
    "CRSP", "IACK", "WAR",  "CINT",
    "INTQ", "INTS", "INTR", "WIR",  "WIC",  "CNCL",  
    "DONE", "WQRC", "CLOS", "WCR",  "WCC",  "IDLE"
};

/*
** SQL Statement actions
**
** The following values are used as array
** indices.  Care must be taken when adding
** new entries to ensure there are no gaps
** or duplicates and all associated arrays
** are kept in sync with these definitions.
**
** History:
**	17-Oct-03 (gordy)
**	    Separated CBIF into ERIF, ICBF to allow other error
**	    codes with immediate callback (ICBF).
**	15-Mar-07 (gordy)
**	    Added SPOS and SSCR for scrollable cursors.
*/

# define SQL_SA_REMC	((IIAPI_ACTION)  0)	/* Remember callback */
# define SQL_SA_CNCL	((IIAPI_ACTION)  1)	/* Cancel query */
# define SQL_SA_RECV	((IIAPI_ACTION)  2)	/* Receive message */
# define SQL_SA_RCVB	((IIAPI_ACTION)  3)	/* Receive message buffered */
# define SQL_SA_REXP	((IIAPI_ACTION)  4)	/* Receive expedited message */
# define SQL_SA_SQRY	((IIAPI_ACTION)  5)	/* Send query */
# define SQL_SA_SPRM	((IIAPI_ACTION)  6)	/* Send query parameters */
# define SQL_SA_SCDT	((IIAPI_ACTION)  7)	/* Send copy data */
# define SQL_SA_SATT	((IIAPI_ACTION)  8)	/* Send attention */
# define SQL_SA_SACK	((IIAPI_ACTION)  9)	/* Send acknowledge */
# define SQL_SA_SIAK	((IIAPI_ACTION) 10)	/* Send interrupt acknowledge */
# define SQL_SA_SRSP	((IIAPI_ACTION) 11)	/* Send response */
# define SQL_SA_SFCH	((IIAPI_ACTION) 12)	/* Send fetch */
# define SQL_SA_SPOS	((IIAPI_ACTION) 13)	/* Send position fetch */
# define SQL_SA_SSCR	((IIAPI_ACTION) 14)	/* Send scroll fetch */
# define SQL_SA_SCLS	((IIAPI_ACTION) 15)	/* Send close */
# define SQL_SA_RQID	((IIAPI_ACTION) 16)	/* Read query ID */
# define SQL_SA_RCID	((IIAPI_ACTION) 17)	/* Read cursor ID */
# define SQL_SA_RDSC	((IIAPI_ACTION) 18)	/* Read descriptor */
# define SQL_SA_RCM	((IIAPI_ACTION) 19)	/* Read copy map */
# define SQL_SA_RRSP	((IIAPI_ACTION) 20)	/* Read response */
# define SQL_SA_RPRC	((IIAPI_ACTION) 21)	/* Read procedure return */
# define SQL_SA_SETD	((IIAPI_ACTION) 22)	/* Set descriptor */
# define SQL_SA_INFO	((IIAPI_ACTION) 23)	/* Return query info */
# define SQL_SA_DESC	((IIAPI_ACTION) 24)	/* Return descriptor */
# define SQL_SA_COPY	((IIAPI_ACTION) 25)	/* Return copy map */
# define SQL_SA_TUPL	((IIAPI_ACTION) 26)	/* Fake GCA_TUPLE message */
# define SQL_SA_RESP	((IIAPI_ACTION) 27)	/* Fake GCA_RESPONSE message */
# define SQL_SA_STAT	((IIAPI_ACTION) 28)	/* Return tuple status */
# define SQL_SA_BUFF	((IIAPI_ACTION) 29)	/* Handle GCA receive buffer */
# define SQL_SA_DELH	((IIAPI_ACTION) 30)	/* Delete statement handle */
# define SQL_SA_ERQA	((IIAPI_ACTION) 31)	/* Query active */
# define SQL_SA_ERQD	((IIAPI_ACTION) 32)	/* Query done */
# define SQL_SA_ERQC	((IIAPI_ACTION) 33)	/* Query cancelled */
# define SQL_SA_ERQI	((IIAPI_ACTION) 34)	/* Query interrupted */
# define SQL_SA_ERIF	((IIAPI_ACTION) 35)	/* Invalid Function Call */
# define SQL_SA_CBOK	((IIAPI_ACTION) 36)	/* Callback Success */
# define SQL_SA_CBFL	((IIAPI_ACTION) 37)	/* Callback Failure */
# define SQL_SA_CBND	((IIAPI_ACTION) 38)	/* Callback No Data */
# define SQL_SA_CBGC	((IIAPI_ACTION) 39)	/* Callback getColumns status */
# define SQL_SA_ICBF	((IIAPI_ACTION) 40)	/* Immediate Callback Failure */
# define SQL_SA_CNOK	((IIAPI_ACTION) 41)	/* Callback Cancel Success */
# define SQL_SA_CNWN	((IIAPI_ACTION) 42)	/* Callback Cancel Warning */
# define SQL_SA_CNFL	((IIAPI_ACTION) 43)	/* Callback Cancel Failure */
# define SQL_SA_RCVE	((IIAPI_ACTION) 44)	/* Receive Error */

# define SQL_SA_CNT     (SQL_SA_RCVE + 1)

static char *sql_sa_id[] =
{
    "REMC", "CNCL", "RECV", "RCVB", "REXP",
    "SQRY", "SPRM", "SCDT", "SATT", "SACK",
    "SIAK", "SRSP", "SFCH", "SPOS", "SSCR", "SCLS", 
    "RQID", "RCID", "RDSC", "RCM",  "RRSP", "RPRC",
    "SETD", "INFO", "DESC", "COPY", "TUPL",
    "RESP", "STAT", "BUFF", "DELH", 
    "ERQA", "ERQD", "ERQC", "ERQI", "ERIF",
    "CBOK", "CBFL", "CBND", "CBGC", "ICBF", 
    "CNOK", "CNWN", "CNFL", "RCVE"
};

/*
** Name: sql_stmt_sm
**
** Description:
**	The SQL Statement State Machine Interface
*/

static IIAPI_SM sql_stmt_sm =
{
    "SQL Stmt",
    SQL_SS_CNT,
    sql_ss_id,
    SQL_SA_CNT,
    sql_sa_id,
    sm_evaluate,
    sm_execute,
    SQL_SS_DONE
};


/*
** Name: sql_act_seq
**
** Description:
**      Action sequences define the groups of
**      actions which occur at state transitions.
**      An action sequence may be used by several
**      state transitions, so the next state
**      value is not defined here.  The SM_OUT
**      structure is used for conveniance in
**      assigning the action sequences during
**      evaluation.
**
** History:
**	 2-Oct-96 (gordy)
**	    Created.
**	13-Mar-97 (gordy)
**	    SQL Action Sequence 42 identical to SAS 2 so no longer used.
**	19-May-00 (gordy)
**	    Re-use SAS 42 to read cursor ID during cancel (INTR -> INTS).
**	17-Oct-03 (gordy)
**	    Separation of CBIF into error code (ERIF) and immediate
**	    callback (ICBF) allows SAS 4 and SAS 7 to use ICBF rather
**	    than saving callback and extends the states in which they
**	    can be used.  The query cancelled callback, SAS 26, does
**	    not need CNOK due to cleanup in CANCEL processing which
**	    maintains thread and callback affinity.  When deleting the
**	    statement handle, SAS 35 doesn't need to read the RESPONSE 
**	    message since the handle is being deleted, but it does need 
**	    both CNOK and CBOK to notify the thread which completes last 
**	    (without this, new states similar to CLOS and WCR would be 
**	    needed for cancel processing).  SAS 42 actions no longer
**	    used due to changes in INTR processing, so replaced with
**	    actions for CANCEL while GCA is inactive (INTQ).  SAS 47
**	    also changed due to CANCEL processing changes as well as
**	    adding SAS 50.
**	15-Mar-07 (gordy)
**	    Added sequences for position and scroll.
*/

static IIAPI_SM_OUT sql_act_seq[] =
{
    { 0, 0, { 0, 0, 0 } },					/* SAS_0  */
    { 0, 1, { SQL_SA_RECV, 0, 0 } },				/* SAS_1  */
    { 0, 2, { SQL_SA_REMC, SQL_SA_RECV, 0 } },			/* SAS_2  */
    { 0, 3, { SQL_SA_CNCL, SQL_SA_ERQD, SQL_SA_CNFL } },	/* SAS_3  */
    { 0, 2, { SQL_SA_ERQI, SQL_SA_ICBF, 0 } },			/* SAS_4  */
    { 0, 1, { SQL_SA_CNWN, 0, 0 } },				/* SAS_5  */
    { 0, 3, { SQL_SA_REMC, SQL_SA_DELH, SQL_SA_CBOK } },	/* SAS_6  */
    { 0, 2, { SQL_SA_ERQA, SQL_SA_ICBF, 0 } },			/* SAS_7  */
    { 0, 2, { SQL_SA_REXP, SQL_SA_CBOK, 0 } },			/* SAS_8  */
    { 0, 2, { SQL_SA_REMC, SQL_SA_CBND, 0 } },			/* SAS_9  */
    { 0, 3, { SQL_SA_REMC, SQL_SA_SETD, SQL_SA_CBOK } },	/* SAS_10 */
    { 0, 1, { SQL_SA_CBOK, 0, 0 } },				/* SAS_11 */
    { 0, 3, { SQL_SA_REMC, SQL_SA_INFO, SQL_SA_CBOK } },	/* SAS_12 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_CBOK, 0 } },			/* SAS_13 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SQRY, 0 } },			/* SAS_14 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SPRM, 0 } },			/* SAS_15 */
    { 0, 1, { SQL_SA_SPRM, 0, 0 } },				/* SAS_16 */
    { 0, 2, { SQL_SA_CNCL, SQL_SA_SATT, 0 } },			/* SAS_17 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SCLS, 0 } },			/* SAS_18 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SFCH, 0 } },			/* SAS_19 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SRSP, 0 } },			/* SAS_20 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SCDT, 0 } },			/* SAS_21 */
    { 0, 1, { SQL_SA_SCDT, 0, 0 } },				/* SAS_22 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SIAK, 0 } },			/* SAS_23 */
    { 0, 2, { SQL_SA_RCM,  SQL_SA_RECV, 0 } },			/* SAS_24 */
    { 0, 3, { SQL_SA_RCM,  SQL_SA_COPY, SQL_SA_SACK } },	/* SAS_25 */
    { 0, 2, { SQL_SA_ERQC, SQL_SA_CBFL, 0 } },			/* SAS_26 */
    { 0, 2, { SQL_SA_RCID, SQL_SA_RECV, 0 } },			/* SAS_27 */
    { 0, 2, { SQL_SA_RQID, SQL_SA_RECV, 0 } },			/* SAS_28 */
    { 0, 3, { SQL_SA_RRSP, SQL_SA_ERQI, SQL_SA_CBFL } },	/* SAS_29 */
    { 0, 3, { SQL_SA_RRSP, SQL_SA_STAT, SQL_SA_CBGC } },	/* SAS_30 */
    { 0, 2, { SQL_SA_RRSP, SQL_SA_CBOK, 0 } },			/* SAS_31 */
    { 0, 3, { SQL_SA_REMC, SQL_SA_ERQC, SQL_SA_CBFL } },	/* SAS_32 */
    { 0, 2, { SQL_SA_RRSP, SQL_SA_CBND, 0 } },			/* SAS_33 */
    { 0, 3, { SQL_SA_RRSP, SQL_SA_INFO, SQL_SA_CBOK } },	/* SAS_34 */
    { 0, 3, { SQL_SA_DELH, SQL_SA_CNOK, SQL_SA_CBOK } },	/* SAS_35 */
    { 0, 2, { SQL_SA_RRSP, SQL_SA_RECV, 0 } },			/* SAS_36 */
    { 0, 3, { SQL_SA_RDSC, SQL_SA_DESC, SQL_SA_CBOK } },	/* SAS_37 */
    { 0, 3, { SQL_SA_RDSC, SQL_SA_DESC, SQL_SA_RECV } },	/* SAS_38 */
    { 0, 2, { SQL_SA_RPRC, SQL_SA_RECV, 0 } },			/* SAS_39 */
    { 0, 3, { SQL_SA_BUFF, SQL_SA_STAT, SQL_SA_CBOK } },	/* SAS_40 */
    { 0, 1, { SQL_SA_RCVB, 0, 0 } },				/* SAS_41 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SATT, 0 } },			/* SAS_42 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_TUPL, 0 } },			/* SAS_43 */
    { 0, 2, { SQL_SA_RPRC, SQL_SA_RESP, 0 } },			/* SAS_44 */
    { 0, 2, { SQL_SA_CNCL, SQL_SA_CNOK, 0 } },			/* SAS_45 */
    { 0, 1, { SQL_SA_SATT, 0, 0 } },				/* SAS_46 */
    { 0, 1, { SQL_SA_CNOK, 0, 0 } },				/* SAS_47 */
    { 0, 2, { SQL_SA_ERIF, SQL_SA_ICBF, 0 } },			/* SAS_48 */
    { 0, 1, { SQL_SA_RCVE, 0, 0 } },				/* SAS_49 */
    { 0, 1, { SQL_SA_SCLS, 0, 0 } },				/* SAS_50 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SPOS, 0 } },			/* SAS_51 */
    { 0, 2, { SQL_SA_REMC, SQL_SA_SSCR, 0 } },			/* SAS_52 */
    { 0, 2, { SQL_SA_RDSC, SQL_SA_RECV, 0 } },			/* SAS_53 */
};

/*
** Indexes into the action sequence array.
*/

# define SQL_SAS_0	0
# define SQL_SAS_1	1
# define SQL_SAS_2	2
# define SQL_SAS_3	3
# define SQL_SAS_4	4
# define SQL_SAS_5	5
# define SQL_SAS_6	6
# define SQL_SAS_7	7
# define SQL_SAS_8	8
# define SQL_SAS_9	9
# define SQL_SAS_10	10
# define SQL_SAS_11	11
# define SQL_SAS_12	12
# define SQL_SAS_13	13
# define SQL_SAS_14	14
# define SQL_SAS_15	15
# define SQL_SAS_16	16
# define SQL_SAS_17	17
# define SQL_SAS_18	18
# define SQL_SAS_19	19
# define SQL_SAS_20	20
# define SQL_SAS_21	21
# define SQL_SAS_22	22
# define SQL_SAS_23	23
# define SQL_SAS_24	24
# define SQL_SAS_25	25
# define SQL_SAS_26	26
# define SQL_SAS_27	27
# define SQL_SAS_28	28
# define SQL_SAS_29	29
# define SQL_SAS_30	30
# define SQL_SAS_31	31
# define SQL_SAS_32	32
# define SQL_SAS_33	33
# define SQL_SAS_34	34
# define SQL_SAS_35	35
# define SQL_SAS_36	36
# define SQL_SAS_37	37
# define SQL_SAS_38	38
# define SQL_SAS_39	39
# define SQL_SAS_40	40
# define SQL_SAS_41	41
# define SQL_SAS_42	42
# define SQL_SAS_43	43
# define SQL_SAS_44	44
# define SQL_SAS_45	45
# define SQL_SAS_46	46
# define SQL_SAS_47	47
# define SQL_SAS_48	48
# define SQL_SAS_49	49
# define SQL_SAS_50	50
# define SQL_SAS_51	51
# define SQL_SAS_52	52
# define SQL_SAS_53	53

/*
** Name: smt_list
**
** Description:
**	State machine transitions define the next state and action 
**	sequence for static evaluations (transitions which depend 
**	solely on the current state and input event).  These 
**	transitions entries are used to populate a sparse matrix 
**	for quick access via the current state and event.
**
**	The following rules and conventions should be followed for
**	handling IIapi_close() (CLOSE) and IIapi_cancel() (CANCEL)
**	requests in the various SQL Statement states.
**
**	In general, CLOSE should only be allowed once the processing
**	of a statement has completed and should not be permitted in
**	a state which requires CANCEL due to connection activity:
**
**	1) CLOSE is permitted when query complete: DONE, CINT, CNCL, WIC.
**	2) CLOSE is permitted when query is dormant: INIT, CURS.
**	3) CLOSE invalid during cancel (E_AP000A): INTQ, INTS, INTR, WIR.
**	4) CLOSE invalid after CLOSE (E_AP0006): WQRC, CLOS, WCR, WCC.
**	5) CLOSE invalid when query is active (E_AP0007): all other
**	   states except SENT and TUPL (see below).
**
**	CLOSE really should be invalid for SENT and TUPL states since
**	the query is active. Unfortunately, this has been allowed and
**	changing this may break applications.  Calling CLOSE in these 
**	states results in state WQRC reading query results until the 
**	query completes which may take a long time, or hang.  CANCEL 
**	cannot be called following CLOSE so there is no way to handle 
**	any problem which may arise in WQRC.  The correct thing to do
**	is call IIapi_getQueryInfo() which results in state WQR and
**	allows CANCEL to be called if a problem occurs.
**
**	CANCEL should be allowed while a query is active:
**
**	1) CANCEL invalid after CANCEL (WARN): INTQ, INTS, INTR, WIR.
**	2) CANCEL invalid when query complete (E_AP0008): DONE, CINT, 
**	   CNCL, WIC.
**	3) CANCEL invalid after CLOSE (E_AP0006): WQRC, CLOS, WCR, WCC.
**	4) CANCEL is permitted when query is dormant: INIT, CURS.
**	5) CANCEL is permitted when query is active: all other states
**
**	Four different conditions may exist when CANCEL is issued:
**	query may be dormant (INIT, CURS), no GCA request is active 
**	(DESC, PARM, SENT, TUPL, PART, COPY), a GCA_RECEIVE may be
**	active (W***), or a GCA_SEND may be active (remaining states).
**
**	o  For dormant queries, the query is effectively cancelled 
**	   immediately and enters the CNCL state.  
**	o  If no GCA request is active, query cancelation is driven by 
**	   the CANCEL request which only returns with E_AP0009 once the 
**	   query has been cancelled.  
**	o  If GCA_SEND is active, CANCEL returns SUCCESS immediatly and 
**	   the cancellation is driven by the active API operation which 
**	   should return E_AP0009 when the cancel is complete.
**	o  When GCA_RECEIVE is active, CANCEL initiates the cancellation
**	   and returns with SUCCESS once the request has been sent.  The
**	   cancelled API operation will return E_AP0009 once the cancel
**	   is complete.  Due to various interactions between GCA_RECEIVE
**	   and GCA_SEND, it is possible for either request to complete
**	   before the other.
**
**	Care must be taken when processing a CANCEL request to handle
**	the case of CANCEL being issued from a different thread than
**	that which issued the active API request: GCA requests callback
**	in the same thread context which issued the request and API
**	callbacks for the original API request and CANCEL request must
**	be made on the last cancel processing action for each thread.
**
**	o  For dormant queries and when no GCA request is active, the
**	   cancellation occurs on the thread issuing the CANCEL.  The
**	   CANCEL completes with E_AP0009.
**	o  For an active GCA_SEND, the cancellation occurs entirely in
**	   the thread which issued the active API request once the send
**	   completes (CANCEL completes immediatly with SUCCESS).
**	o  The active GCA_RECEIVE case is very complicated.  
**	     o  The thread which issued the CANCEL must send the server 
**		interrupt and wait for the send to complete.  
**	     o  The active API operation thread must read/ignore query 
**		results waiting for the IACK to be received.
**	     o  If the send completes prior to the IACK being received, 
**		CANCEL thread completes with SUCCESS and cancellation 
**		will be handled by the active API thread (state WIR).  
**		API operation completes with E_AP0009 when IACK received.
**	     o  If the IACK is received before the send completes, the 
**		API operation completes with E_AP0009 and the cancellation 
**		will be handled by the CANCEL thread (state WIC).
**	     o  When IACK is received first, the cancelled query may
**		also be closed prior to the cancellation being complete.
**		The actual close operations are delayed until the cancel
**		has completed (state WCC).
**	     o  Query may have involved a cursor which will need to be
**		closed (WCC uses existing CLOS, WCR states).
**
**	Note that the multi-threaded operational sequences are also valid
**	when sequenced in a single thread.
**
** History:
**	 2-Oct-96 (gordy)
**	    Created.
**	 9-Feb-99 (gordy)
**	    Handles in IDLE state should allow IIapi_close().
**	19-May-00 (gordy)
**	    Add INTR -> INTS transitions for response messages.
**	17-Oct-03 (gordy)
**	    The prior change for CANCEL transitions is insufficient
**	    to properly handle callbacks and multi-threading.  Added
**	    new CANCEL states WIC, WCC to handle post INTR processing.
**	    Added CLOSE entries for query active states and CANCEL
**	    entries for INIT, WPRC.  Removed CLOSE/CANCEL entries for
**	    state which follow the CLOSE request.  Reworked CANCEL
**	    entries: INTR reads responses until IACK received (WIC)
**	    or interrupt send completes (WIR).  CLOSE is now allowed
**	    after IACK received (WIC,WCC) with final state (CNCL,IDLE)
**	    determined by the order of the IACK, CLOSE, and send
**	    completion events.
**	15-Mar-07 (gordy)
**	    Added position and scroll functions.
*/

typedef struct
{

    IIAPI_EVENT         smt_event;
    IIAPI_STATE         smt_state;
    IIAPI_STATE         smt_next;
    IIAPI_ACTION        smt_action;

} SM_TRANSITION;

static SM_TRANSITION smt_list[] =
{
    /*
    ** API Functions which directly affect the statement state.
    */
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_INIT,	SQL_SS_CNCL,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_SQRY,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_DESC,	SQL_SS_INTQ,	SQL_SAS_42 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_PARM,	SQL_SS_INTQ,	SQL_SAS_42 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_SPRM,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_SENT,	SQL_SS_INTQ,	SQL_SAS_42 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WQR,	SQL_SS_INTR,	SQL_SAS_17 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WQDR,	SQL_SS_INTR,	SQL_SAS_17 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_TUPL,	SQL_SS_INTQ,	SQL_SAS_42 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WTR,	SQL_SS_INTR,	SQL_SAS_17 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WPRC,	SQL_SS_INTR,	SQL_SAS_17 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WCDR,	SQL_SS_INTR,	SQL_SAS_17 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_CURS,	SQL_SS_CNCL,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_FTCH,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_SCRL,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WFR,	SQL_SS_INTR,	SQL_SAS_17 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_PART,	SQL_SS_INTQ,	SQL_SAS_42 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WCMR,	SQL_SS_INTR,	SQL_SAS_17 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_ACKI,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_ACKF,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_COPY,	SQL_SS_INTQ,	SQL_SAS_42 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_CDAT,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_CRSP,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_IACK,	SQL_SS_INTS,	SQL_SAS_45 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WAR,	SQL_SS_INTR,	SQL_SAS_17 },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_CINT,	SQL_SS_CINT,	SQL_SAS_3  },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_INTQ,	SQL_SS_INTQ,	SQL_SAS_5  },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_INTS,	SQL_SS_INTS,	SQL_SAS_5  },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_INTR,	SQL_SS_INTR,	SQL_SAS_5  },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WIR,	SQL_SS_WIR,	SQL_SAS_5  },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_WIC,	SQL_SS_WIC,	SQL_SAS_3  },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_CNCL,	SQL_SS_CNCL,	SQL_SAS_3  },
    { IIAPI_EV_CANCEL_FUNC,	SQL_SS_DONE,	SQL_SS_DONE,	SQL_SAS_3  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_INIT,	SQL_SS_IDLE,	SQL_SAS_6  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_SQRY,	SQL_SS_SQRY,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_DESC,	SQL_SS_DESC,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_PARM,	SQL_SS_PARM,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_SPRM,	SQL_SS_SPRM,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_SENT,	SQL_SS_WQRC,	SQL_SAS_2  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WQR,	SQL_SS_WQR,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WQDR,	SQL_SS_WQDR,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_TUPL,	SQL_SS_WQRC,	SQL_SAS_2  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WTR,	SQL_SS_WTR,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WPRC,	SQL_SS_WPRC,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WCDR,	SQL_SS_WCDR,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_CURS,	SQL_SS_CLOS,	SQL_SAS_18 },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_FTCH,	SQL_SS_FTCH,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_SCRL,	SQL_SS_SCRL,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WFR,	SQL_SS_WFR,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_PART,	SQL_SS_PART,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WCMR,	SQL_SS_WCMR,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_ACKI,	SQL_SS_ACKI,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_ACKF,	SQL_SS_ACKF,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_COPY,	SQL_SS_COPY,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_CDAT,	SQL_SS_CDAT,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_CRSP,	SQL_SS_CRSP,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_IACK,	SQL_SS_IACK,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WAR,	SQL_SS_WAR,	SQL_SAS_7  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_CINT,	SQL_SS_IDLE,	SQL_SAS_6  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_INTQ,	SQL_SS_INTQ,	SQL_SAS_4  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_INTS,	SQL_SS_INTS,	SQL_SAS_4  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_INTR,	SQL_SS_INTR,	SQL_SAS_4  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WIR,	SQL_SS_WIR,	SQL_SAS_4  },
    { IIAPI_EV_CLOSE_FUNC,	SQL_SS_WIC,	SQL_SS_WCC,	SQL_SAS_13 },
    { IIAPI_EV_GETCOLUMN_FUNC,	SQL_SS_CURS,	SQL_SS_FTCH,	SQL_SAS_19 },
    { IIAPI_EV_GETCOLUMN_FUNC,	SQL_SS_DONE,	SQL_SS_DONE,	SQL_SAS_9  },
    { IIAPI_EV_GETCOPYMAP_FUNC,	SQL_SS_SENT,	SQL_SS_WCMR,	SQL_SAS_2  },
    { IIAPI_EV_GETDESCR_FUNC,	SQL_SS_SENT,	SQL_SS_WQDR,	SQL_SAS_2  },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_DESC,	SQL_SS_DESC,	SQL_SAS_7  },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_PARM,	SQL_SS_PARM,	SQL_SAS_7  },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_SENT,	SQL_SS_WQR,	SQL_SAS_2  },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_TUPL,	SQL_SS_WQR,	SQL_SAS_2  },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_CURS,	SQL_SS_CURS,	SQL_SAS_12 },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_PART,	SQL_SS_WQR,	SQL_SAS_2  },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_COPY,	SQL_SS_CRSP,	SQL_SAS_20 },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_CINT,	SQL_SS_DONE,	SQL_SAS_12 },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_CNCL,	SQL_SS_CNCL,	SQL_SAS_32 },
    { IIAPI_EV_GETQINFO_FUNC,	SQL_SS_DONE,	SQL_SS_DONE,	SQL_SAS_12 },
    { IIAPI_EV_POSITION_FUNC,	SQL_SS_CURS,	SQL_SS_SCRL,	SQL_SAS_51 },
    { IIAPI_EV_PUTPARM_FUNC,	SQL_SS_PARM,	SQL_SS_SPRM,	SQL_SAS_15 },
    { IIAPI_EV_SCROLL_FUNC,	SQL_SS_CURS,	SQL_SS_SCRL,	SQL_SAS_52 },
    { IIAPI_EV_SETDESCR_FUNC,	SQL_SS_DESC,	SQL_SS_PARM,	SQL_SAS_10 },

    /*
    ** The following messages are processed at the statement level.
    */
    { IIAPI_EV_CFROM_RCVD,	SQL_SS_WQR,	SQL_SS_WQR,	SQL_SAS_1  },
    { IIAPI_EV_CFROM_RCVD,	SQL_SS_INTR,	SQL_SS_INTR,	SQL_SAS_1  },
    { IIAPI_EV_CFROM_RCVD,	SQL_SS_WIR,	SQL_SS_WIR,	SQL_SAS_1  },
    { IIAPI_EV_CFROM_RCVD,	SQL_SS_WQRC,	SQL_SS_WQRC,	SQL_SAS_1  },
    { IIAPI_EV_CINTO_RCVD,	SQL_SS_WQR,	SQL_SS_WQR,	SQL_SAS_1  },
    { IIAPI_EV_CINTO_RCVD,	SQL_SS_INTR,	SQL_SS_INTR,	SQL_SAS_1  },
    { IIAPI_EV_CINTO_RCVD,	SQL_SS_WIR,	SQL_SS_WIR,	SQL_SAS_1  },
    { IIAPI_EV_CINTO_RCVD,	SQL_SS_WQRC,	SQL_SS_WQRC,	SQL_SAS_1  },
    { IIAPI_EV_IACK_RCVD,	SQL_SS_INTR,	SQL_SS_WIC,	SQL_SAS_26 },
    { IIAPI_EV_IACK_RCVD,	SQL_SS_WIR,	SQL_SS_CNCL,	SQL_SAS_26 },
    { IIAPI_EV_RESPONSE_RCVD,	SQL_SS_WQDR,	SQL_SS_DONE,	SQL_SAS_33 },
    { IIAPI_EV_RESPONSE_RCVD,	SQL_SS_WTR,	SQL_SS_DONE,	SQL_SAS_30 },
    { IIAPI_EV_RESPONSE_RCVD,	SQL_SS_WPRC,	SQL_SS_DONE,	SQL_SAS_30 },
    { IIAPI_EV_RESPONSE_RCVD,	SQL_SS_WCMR,	SQL_SS_DONE,	SQL_SAS_33 },
    { IIAPI_EV_RESPONSE_RCVD,	SQL_SS_INTR,	SQL_SS_INTR,	SQL_SAS_36 },
    { IIAPI_EV_RESPONSE_RCVD,	SQL_SS_WIR,	SQL_SS_WIR,	SQL_SAS_36 },
    { IIAPI_EV_RESPONSE_RCVD,	SQL_SS_WQRC,	SQL_SS_IDLE,	SQL_SAS_35 },
    { IIAPI_EV_RESPONSE_RCVD,	SQL_SS_WCR,	SQL_SS_IDLE,	SQL_SAS_35 },
    { IIAPI_EV_TDESCR_RCVD,	SQL_SS_WQR,	SQL_SS_WQR,	SQL_SAS_53 },
    { IIAPI_EV_TDESCR_RCVD,	SQL_SS_WCDR,	SQL_SS_WCDR,	SQL_SAS_38 },
    { IIAPI_EV_TDESCR_RCVD,	SQL_SS_INTR,	SQL_SS_INTR,	SQL_SAS_1  },
    { IIAPI_EV_TDESCR_RCVD,	SQL_SS_WIR,	SQL_SS_WIR,	SQL_SAS_1  },
    { IIAPI_EV_TDESCR_RCVD,	SQL_SS_WQRC,	SQL_SS_WQRC,	SQL_SAS_1  },
    { IIAPI_EV_TUPLE_RCVD,	SQL_SS_WQR,	SQL_SS_WQR,	SQL_SAS_1  },
    { IIAPI_EV_TUPLE_RCVD,	SQL_SS_INTR,	SQL_SS_INTR,	SQL_SAS_1  },
    { IIAPI_EV_TUPLE_RCVD,	SQL_SS_WIR,	SQL_SS_WIR,	SQL_SAS_1  },
    { IIAPI_EV_TUPLE_RCVD,	SQL_SS_WQRC,	SQL_SS_WQRC,	SQL_SAS_1  },
    { IIAPI_EV_RETPROC_RCVD,	SQL_SS_WTR,	SQL_SS_WPRC,	SQL_SAS_39 },
    
    /*
    ** GCA request completions.
    */
    { IIAPI_EV_SEND_CMPL,	SQL_SS_SQRY,	SQL_SS_SENT,	SQL_SAS_11 },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_FTCH,	SQL_SS_WFR,	SQL_SAS_1  },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_SCRL,	SQL_SS_PART,	SQL_SAS_11 },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_ACKI,	SQL_SS_TUPL,	SQL_SAS_11 },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_ACKF,	SQL_SS_COPY,	SQL_SAS_8  },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_CRSP,	SQL_SS_WQR,	SQL_SAS_1  },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_IACK,	SQL_SS_WAR,	SQL_SAS_1  },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_CLOS,	SQL_SS_WCR,	SQL_SAS_1  },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_INTS,	SQL_SS_INTQ,	SQL_SAS_46 },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_INTQ,	SQL_SS_WIR,	SQL_SAS_1  },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_INTR,	SQL_SS_WIR,	SQL_SAS_47 },
    { IIAPI_EV_SEND_CMPL,	SQL_SS_WIC,	SQL_SS_CNCL,	SQL_SAS_47 }
};


/*
** Name: smt_array
**
** Description:
**      Two dimensional array used for static state
**      transition evaluations.  The array provides
**      quick access based on the current state and
**      event.  The separation of the array and the
**      state transition entry list is primarily for
**      ease of maintenance.  Initialization occurs
**      at the first evaluation.
**
**      The array is sparsly populated; missing
**      entries are either dynamically evaluated
**      transitions or invalid events.
*/

static SM_TRANSITION	*smt_array[ IIAPI_EVENT_CNT ][ SQL_SS_CNT ] ZERO_FILL;
static II_BOOL		initialized = FALSE;


/*
** Name: IIapi_sql_sinit
**
** Description:
**	Initialize the SQL Statement state machine.
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
IIapi_sql_sinit( VOID )
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

    IIapi_sm[ IIAPI_SMT_SQL ][ IIAPI_SMH_STMT ] = &sql_stmt_sm;

    return( IIAPI_ST_SUCCESS );
}


/*
** Name: sm_evaluate
**
** Description:
**	Evaluation function for the Statement State Machine
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
**	 2-Oct-96 (gordy)
**	    Created.
**	19-May-00 (gordy)
**	    Add INTR -> INTS transitions for response messages.
**	17-Oct-03 (gordy)
**	    INTR needs to read QCID message same as other cases so 
**	    that cursor can be handled after cancel (similar addition 
**	    to RETPROC).  Combined resulting QCID common cases.  Add 
**	    WCC send completion case to check/close cursor.
**	25-Oct-06 (gordy)
**	    Query flags saved as statement flags.
*/

static IIAPI_SM_OUT *
sm_evaluate
(
    IIAPI_EVENT		event,
    IIAPI_STATE		state,
    IIAPI_HNDL		*ev_hndl,
    IIAPI_HNDL		*sm_hndl,
    II_PTR		parmBlock,
    IIAPI_SM_OUT	*smo_buff
)
{
    IIAPI_STMTHNDL	*stmtHndl = (IIAPI_STMTHNDL *)sm_hndl;
    IIAPI_CONNHNDL	*connHndl;
    IIAPI_PARMNMEM	*parmNmem;
    IIAPI_SM_OUT	*smo = NULL;
    SM_TRANSITION	*smt;
    IIAPI_GCA_BUFF	*gcaBuff;
    GCA_RV_PARMS	*rcvParm;
    GCD_RE_DATA		*reData;

    IIAPI_TRACE( IIAPI_TR_VERBOSE )
	( "%s evaluate: evaluating event %s in state %s\n",
	  sql_stmt_sm.sm_id, IIAPI_PRINTEVENT( event ),
	  IIAPI_PRINT_ID( state, SQL_SS_CNT, sql_ss_id ) );

    /*
    ** Static evaluations depend solely on the current state and event.
    */
    if ( (smt = smt_array[ event ][ state ]) )
    {
	IIAPI_TRACE( IIAPI_TR_DETAIL )
	    ( "%s evaluate: static evaluation\n", sql_stmt_sm.sm_id );

	smo = smo_buff;
	STRUCT_ASSIGN_MACRO( sql_act_seq[ smt->smt_action ], *smo );
	smo->smo_next_state = smt->smt_next;
    }
    else		/* Dynamic evaluations require additional predicates */
    switch( event )	/* to determine the correct state transition.        */
    {
	/*
	** Cursors must be closed before statement 
	** handles can be deleted.
	*/
	case IIAPI_EV_CLOSE_FUNC :
	    switch( state )
	    {
		case SQL_SS_CNCL :
		case SQL_SS_DONE :
		    smo = smo_buff;

		    if ( stmtHndl->sh_flags & IIAPI_SH_CURSOR )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_18 ], *smo );
			smo->smo_next_state = SQL_SS_CLOS;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_6 ], *smo );
			smo->smo_next_state = SQL_SS_IDLE;
		    }
		    break;
	    }
	    break;
    
	/*
	** Check to see if we have buffered data
	** which may satisfy the request.
	*/
	case IIAPI_EV_GETCOLUMN_FUNC :
	    switch( state )
	    {
		case SQL_SS_TUPL :
		    smo = smo_buff;

		    if ( stmtHndl->sh_gcaBuff )
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_43 ], *smo );
		    else
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_2 ], *smo );

		    smo->smo_next_state = SQL_SS_WTR;
		    break;

		case SQL_SS_PART :
		    smo = smo_buff;

		    if ( stmtHndl->sh_gcaBuff )
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_43 ], *smo );
		    else
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_2 ], *smo );

		    smo->smo_next_state = SQL_SS_WFR;
		    break;
	    }
	    break;
    
	/*
	** Check for interrupts during copy processing.
	*/
	case IIAPI_EV_PUTCOLUMN_FUNC :
	    switch( state )
	    {
		case SQL_SS_COPY :
		    connHndl = IIapi_getConnHndl( sm_hndl );
		    smo = smo_buff;

		    if ( connHndl->ch_flags & IIAPI_CH_INTERRUPT )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_23 ], *smo );
			smo->smo_next_state = SQL_SS_IACK;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_21 ], *smo );
			smo->smo_next_state = SQL_SS_CDAT;
		    }
		    break;
	    }
	    break;

	/*
	** If a query has parameters, we wait to send
	** the query with the parameters.  Otherwise,
	** we send the query now.
	*/
	case IIAPI_EV_QUERY_FUNC :
	    switch( state )
	    {
		case SQL_SS_INIT :
		    smo = smo_buff;

		    if ( stmtHndl->sh_flags & IIAPI_SH_PARAMETERS )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_13 ], *smo );
			smo->smo_next_state = SQL_SS_DESC;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_14 ], *smo );
			smo->smo_next_state = SQL_SS_SQRY;
		    }
		    break;
	    }
	    break;

	/*
	** Copy maps and tuple descriptors may be very large.  
	** Process split messages before preceeding to next 
	** state.
	*/
	case IIAPI_EV_CFROM_RCVD :
	    switch( state )
	    {
		case SQL_SS_WCMR :
		    gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;
		    rcvParm = (GCA_RV_PARMS *)gcaBuff->gca_parm;
		    smo = smo_buff;

		    if ( rcvParm->gca_end_of_data )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_25 ], *smo );
			smo->smo_next_state = SQL_SS_ACKF;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_24 ], *smo );
			smo->smo_next_state = SQL_SS_WCMR;
		    }
		    break;
	    }
	    break;

	case IIAPI_EV_CINTO_RCVD :
	    switch( state )
	    {
		case SQL_SS_WCMR :
		    gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;
		    rcvParm = (GCA_RV_PARMS *)gcaBuff->gca_parm;
		    smo = smo_buff;

		    if ( rcvParm->gca_end_of_data )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_25 ], *smo );
			smo->smo_next_state = SQL_SS_ACKI;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_24 ], *smo );
			smo->smo_next_state = SQL_SS_WCMR;
		    }
		    break;
	    }
	    break;

	case IIAPI_EV_TDESCR_RCVD :
	    switch( state )
	    {
		case SQL_SS_WQDR :
		    gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;
		    rcvParm = (GCA_RV_PARMS *)gcaBuff->gca_parm;
		    smo = smo_buff;

		    if ( rcvParm->gca_end_of_data )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_37 ], *smo );
			smo->smo_next_state = SQL_SS_TUPL;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_38 ], *smo );
			smo->smo_next_state = SQL_SS_WQDR;
		    }
		    break;
	    }
	    break;

	/*
	** Query IDs are expected for repeat queries and cursors.
	*/
	case IIAPI_EV_QCID_RCVD :
	    switch( state )
	    {
		case SQL_SS_WQDR :
		    smo = smo_buff;

		    if ( stmtHndl->sh_queryType == IIAPI_QT_DEF_REPEAT_QUERY )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_28 ], *smo );
			smo->smo_next_state = SQL_SS_WQDR;
		    }
		    else  if ( stmtHndl->sh_queryType == IIAPI_QT_OPEN )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_27 ], *smo );
			smo->smo_next_state = SQL_SS_WCDR;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_1 ], *smo );
			smo->smo_next_state = SQL_SS_WQDR;
		    }
		    break;

		case SQL_SS_WQR :
		case SQL_SS_INTR :
		case SQL_SS_WIR :
		case SQL_SS_WQRC :
		    smo = smo_buff;

		    if ( stmtHndl->sh_queryType == IIAPI_QT_DEF_REPEAT_QUERY )
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_28 ], *smo );
		    else  if ( stmtHndl->sh_queryType == IIAPI_QT_OPEN )
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_27 ], *smo );
		    else
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_1 ], *smo );

		    smo->smo_next_state = state;
		    break;
	    }
	    break;
    
	/*
	** Response messages contain flags which control
	** the processing of copy requests and cursors.
	*/
	case IIAPI_EV_RESPONSE_RCVD :
	    gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;
	    reData = gcaBuff ? (GCD_RE_DATA *)gcaBuff->data : NULL;

	    switch( state )
	    {
		case SQL_SS_WAR :
		    smo = smo_buff;

		    if ( ! reData || reData->gca_rqstatus & GCA_CONTINUE_MASK )
		    {
			/*
			** Continue copy processing.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_22 ], *smo );
			smo->smo_next_state = SQL_SS_CDAT;
		    }
		    else
		    {
			/*
			** Copy processing interrupted.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_29 ], *smo );
			smo->smo_next_state = SQL_SS_CINT;
		    }
		    break;

		case SQL_SS_WCDR :
		    smo = smo_buff;

		    if ( reData  &&  reData->gca_rqstatus & GCA_FAIL_MASK )
		    {
			/*
			** Error opening cursor.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_33 ], *smo );
			smo->smo_next_state = SQL_SS_DONE;
		    }
		    else
		    {
			/*
			** Cursor open.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_31 ], *smo );
			smo->smo_next_state = SQL_SS_CURS;
		    }
		    break;

		case SQL_SS_WQR :
		    smo = smo_buff;
		    STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_34 ], *smo );

		    if ( 
		         ! (stmtHndl->sh_flags & IIAPI_SH_CURSOR)  ||
		         (reData && reData->gca_rqstatus & GCA_END_QUERY_MASK) 
		       )
			smo->smo_next_state = SQL_SS_DONE;    /* end-of-data */
		    else
			smo->smo_next_state = SQL_SS_CURS;    /* more data */
		    break;

		case SQL_SS_WFR :
		    smo = smo_buff;
		    STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_30 ], *smo );

		    if ( reData && reData->gca_rqstatus & GCA_END_QUERY_MASK )
			smo->smo_next_state = SQL_SS_DONE;    /* end-of-data */
		    else
			smo->smo_next_state = SQL_SS_CURS;    /* more data */
		    break;
	    }
	    break;

	/*
	** The original GCA_RETPROC protocol ended with 
	** the GCA_RETPROC message while the preferred 
	** protocol has a GCA_RESPONSE message following 
	** the GCA_RETPROC.  If this is the original 
	** protocol, we fake a GCA_RESPONSE message to 
	** simplify subsequent processing.  Otherwise, 
	** we need to wait for the actual GCA_RESPONSE 
	** message.
	*/
	case IIAPI_EV_RETPROC_RCVD :
	    switch( state )
	    {
		case SQL_SS_WQR :
		case SQL_SS_WQDR :
		case SQL_SS_INTR :
		case SQL_SS_WIR :
		case SQL_SS_WQRC :
		    connHndl = IIapi_getConnHndl( sm_hndl );
		    smo = smo_buff;

		    if ( connHndl->ch_partnerProtocol < GCA_PROTOCOL_LEVEL_60 )
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_44 ], *smo );
		    else
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_39 ], *smo );

		    smo->smo_next_state = state;
		    break;
	    }
	    break;


	/*
	** Check to see if current request is complete.
	*/
	case IIAPI_EV_TUPLE_RCVD :
	    switch( state )
	    {
		case SQL_SS_WTR :
		    smo = smo_buff;

		    if ( EvaluateTuple(stmtHndl, (IIAPI_GCA_BUFF *)parmBlock) )
		    {
			/*
			** Request completed.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_40 ], *smo );
			smo->smo_next_state = SQL_SS_TUPL;
		    }
		    else
		    {
			/*
			** Get more tuples.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_41 ], *smo );
			smo->smo_next_state = SQL_SS_WTR;
		    }
		    break;
		
		case SQL_SS_WFR :
		    smo = smo_buff;

		    if ( EvaluateTuple(stmtHndl, (IIAPI_GCA_BUFF *)parmBlock) )
		    {
			/*
			** Request completed.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_40 ], *smo );
			smo->smo_next_state = SQL_SS_PART;
		    }
		    else
		    {
			/*
			** Get more tuples.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_41 ], *smo );
			smo->smo_next_state = SQL_SS_WFR;
		    }
		    break;
	    }
	    break;

	/*
	** Sending of query parameters and copy tuples
	** may require multiple GCA operations to complete.
	*/
	case IIAPI_EV_SEND_CMPL :
	    switch( state )
	    {
		case SQL_SS_SPRM :
		    parmNmem = (IIAPI_PARMNMEM *)parmBlock;
		    smo = smo_buff;

		    if ( parmNmem->pm_endFlag )
		    {
			/*
			** Query is complete.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_11 ], *smo );
			smo->smo_next_state = SQL_SS_SENT;
		    }
		    else  if ( stmtHndl->sh_parmSend )
		    {
			/*
			** More parameters to send.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_16 ], *smo );
			smo->smo_next_state = SQL_SS_SPRM;
		    }
		    else
		    {
			/*
			** Need more parameters.
			*/
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_11 ], *smo );
			smo->smo_next_state = SQL_SS_PARM;
		    }
		    break;

		case SQL_SS_CDAT :
		    smo = smo_buff;

		    /*
		    ** State only changes when all columns have
		    ** been sent (column counter reaches 0).
		    */
		    if ( stmtHndl->sh_colFetch )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_22 ], *smo );
			smo->smo_next_state = SQL_SS_CDAT;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_11 ], *smo );
			smo->smo_next_state = SQL_SS_COPY;
		    }
		    break;

		/*
		** May need to close cursor.
		*/
		case SQL_SS_WCC :
		    smo = smo_buff;

		    if ( stmtHndl->sh_flags & IIAPI_SH_CURSOR )
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_50 ], *smo );
			smo->smo_next_state = SQL_SS_CLOS;
		    }
		    else
		    {
			STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_35 ], *smo );
			smo->smo_next_state = SQL_SS_IDLE;
		    }
		    break;
	    }
	    break;

	/*
	** The following events are ignored at the
	** statement level.  They will be processed
	** by the connection state machine.  Since
	** they are unconditionally ignored, it is
	** easier to handle them here than maintaining
	** static transitions for every state.
	*/
	case IIAPI_EV_ERROR_RCVD :
	case IIAPI_EV_EVENT_RCVD :
	case IIAPI_EV_NPINTERUPT_RCVD :
	case IIAPI_EV_RELEASE_RCVD :
	case IIAPI_EV_TRACE_RCVD :
	case IIAPI_EV_PROMPT_RCVD :
	case IIAPI_EV_UNEXPECTED_RCVD :
	case IIAPI_EV_RESUME :
	case IIAPI_EV_RECV_ERROR :
	case IIAPI_EV_SEND_ERROR :
	    smo = smo_buff;
	    smo->smo_next_state = state;
	    smo->smo_action_cnt = 0;
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
		  sql_stmt_sm.sm_id );

	    STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_48 ], *smo );
	    smo->smo_next_state = state;
	}
	else  if ( event <= IIAPI_EVENT_MSG_MAX )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s Evaluate: invalid message received\n",
		  sql_stmt_sm.sm_id );

	    STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_49 ], *smo );
	    smo->smo_next_state = state;
	}
	else
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s Evaluate: unexpected I/O completion\n",
		  sql_stmt_sm.sm_id );

	    STRUCT_ASSIGN_MACRO( sql_act_seq[ SQL_SAS_0 ], *smo );
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
	      sql_stmt_sm.sm_id );
	smo = NULL;
    }

    return( smo );
}


/*
** Name: sm_execute
**
** Description:
**	Action execution function for the Statement State Machine
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
**	 2-Oct-96 (gordy)
**	    Created.
**	31-Jan-97 (gordy)
**	    Added het-net descriptor to IIapi_sndGCA() and statement 
**	    handle.  Needed during COPY FROM processing.
**       9-Aug-99 (rajus01)
**          Asynchronous behaviour of API requires the removal of 
**          handles marked for deletion from the queue. ( Bug #98303 )
**	11-Dec-02 (gordy)
**	    Tracing added for local error conditions.
**	20-Feb-03 (gordy)
**	    Fixed the length of data used when returning an object key.
**	 6-Jun-03 (gordy)
**	    Clear query info mask once data has been returned.
**	17-Oct-03 (gordy)
**	    Separated CBIF into ERIF and ICBF.
**	 8-Apr-04 (gordy)
**	    To be consistent, clear query info flags along with mask.
**	15-Mar-07 (gordy)
**	    Support for scrollable cursors.
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
    IIAPI_STMTHNDL	*stmtHndl = (IIAPI_STMTHNDL *)sm_hndl;
    IIAPI_PARMNMEM	*parmNmem;
    IIAPI_STATUS	status;
    II_BOOL		success = TRUE;

    switch( action )
    {
	case SQL_SA_REMC :
	    /*
	    ** Remember callback.
	    */
	    stmtHndl->sh_callback = TRUE;
	    stmtHndl->sh_parm = (IIAPI_GENPARM *)parmBlock;
	    break;

	case SQL_SA_CNCL :
	    /*
	    ** Remember cancel callback.
	    */
	    stmtHndl->sh_cancelled = TRUE;
	    stmtHndl->sh_cancelParm = (IIAPI_GENPARM *)parmBlock;
	    break;
    
	case SQL_SA_RECV :
	    /*
	    ** Issue receive message request.
	    */
	    status = IIapi_rcvNormalGCA( sm_hndl, NULL, (II_LONG)(-1) );
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    break;

	case SQL_SA_RCVB :
	{
	    /*
	    ** Issue receive message request.
	    */
	    IIAPI_GCA_BUFF *gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;
	    status = IIapi_rcvNormalGCA( sm_hndl, &gcaBuff, (II_LONG)(-1) );
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    break;
	}
	case SQL_SA_REXP :
	{
	    /*
	    ** Issue receive request on expedited channel.
	    **
	    ** COPY processing is interrupted by async messages on
	    ** the expedited channel.  We don't begin receiving
	    ** expedited messages until the first COPY FROM request
	    ** is made.  Once started, expedited channel processing
	    ** occurs completly independent of other API functions,
	    ** and does not stop until GCA is shutdown.
	    */
	    IIAPI_CONNHNDL *connHndl = IIapi_getConnHndl( sm_hndl );

	    if ( ! (connHndl->ch_flags & IIAPI_CH_GCA_EXP_ACTIVE) )
	    {
		IIapi_rcvExpediteGCA( (IIAPI_HNDL *)connHndl );
		connHndl->ch_flags |= IIAPI_CH_GCA_EXP_ACTIVE;
	    }
	    break;
	}
	case SQL_SA_SQRY :
	    /*
	    ** Format and send GCA_QUERY (no parameters) message.
	    */
	    if ( ! (parmNmem = IIapi_createGCAParm( stmtHndl, NULL )) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }

	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case SQL_SA_SPRM :
	    /*
	    ** Format and send GCA_QUERY (with parameters) message.
	    */
	    if ( ! (parmNmem = IIapi_createGCAParm( stmtHndl, 
				    (IIAPI_PUTPARMPARM *)stmtHndl->sh_parm )) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case SQL_SA_SCDT :
	    /*
	    ** Format and send GCA_CDATA message.
	    */
	    if ( ! (parmNmem = IIapi_createGCATuple( stmtHndl, 
				    (IIAPI_PUTCOLPARM *)stmtHndl->sh_parm )) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    parmNmem->pm_msgType = GCA_CDATA;
	    status = IIapi_sndGCA( sm_hndl, 
				   parmNmem, stmtHndl->sh_cpyDescriptor );
    
	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case SQL_SA_SATT :
	    /*
	    ** Format and send GCA_ATTENTION message.
	    */
	    if ( ! (parmNmem = IIapi_createGCAAttention( sm_hndl )) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case SQL_SA_SACK :
	    /*
	    ** Format and send GCA_ACK message.
	    */
	    if ( ! ( parmNmem = IIapi_createGCAAck( stmtHndl ) ) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case SQL_SA_SIAK :
	    /*
	    ** Format and send GCA_IACK message.
	    */
	    if ( ! ( parmNmem = IIapi_createGCAAck( stmtHndl ) ) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    parmNmem->pm_msgType = GCA_IACK;
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case SQL_SA_SRSP :
	    /*
	    ** Format and send GCA_RESPONSE message.
	    */
	    if ( ! ( parmNmem = IIapi_createGCAResponse( stmtHndl ) ) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case SQL_SA_SFCH :
	{
	    /*
	    ** Format and send GCA_FETCH message.
	    */
	    IIAPI_GETCOLPARM	*getColParm = (IIAPI_GETCOLPARM *)parmBlock;
       
	    if ( ! (parmNmem = IIapi_createGCAFetch( getColParm->gc_rowCount, 
	    					     stmtHndl )) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status == IIAPI_ST_SUCCESS )
		stmtHndl->sh_rowCount = getColParm->gc_rowCount;
	    else
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;
	}
	case SQL_SA_SPOS :
	{
	    /*
	    ** Format and send positioned GCA_FETCH message.
	    **
	    ** Note: if protocol does not permit positioned fetchs,
	    ** request is reduced to NEXT.  Caller must ensure actual 
	    ** request meets the following criteria in this case:
	    **     po_reference == IIAPI_POS_CURRENT
	    **     po_offset == 1
	    **     po_rowCount >= 1
	    */
	    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl( sm_hndl );
	    IIAPI_POSPARM	*posParm = (IIAPI_POSPARM *)parmBlock;
	    II_INT2		prefetch = posParm->po_rowCount;
       
	    if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_67 )
	        parmNmem = IIapi_createGCAScroll( stmtHndl,
						  posParm->po_reference, 
						  posParm->po_offset, 
						  posParm->po_rowCount );
	    else
		parmNmem = IIapi_createGCAFetch( posParm->po_rowCount, 
						 stmtHndl );

	    if ( ! parmNmem )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status == IIAPI_ST_SUCCESS )
		stmtHndl->sh_rowCount = posParm->po_rowCount;
	    else
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;
	}
	case SQL_SA_SSCR :
	{
	    /*
	    ** Format and send positioned GCA_FETCH message.
	    **
	    ** Note: if protocol does not permit positioned fetchs,
	    ** request is reduced to NEXT.  Caller must ensure actual 
	    ** request meets the following criteria in this case:
	    **     sl_orientation == IIAPI_SCROLL_NEXT
	    */
	    IIAPI_SCROLLPARM	*scrollParm = (IIAPI_SCROLLPARM *)parmBlock;
	    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl( sm_hndl );
	    II_UINT2		reference;
	    II_INT4		offset;
	    II_INT2		prefetch = 1;

	    /*
	    ** Convert scroll request to positioned.
	    */
	    switch( scrollParm->sl_orientation )
	    {
	    case IIAPI_SCROLL_BEFORE :
		reference = IIAPI_POS_BEGIN;
		offset = 0;
		prefetch = 0;
		break;

	    case IIAPI_SCROLL_FIRST :
		reference = IIAPI_POS_BEGIN;
		offset = 1;
		break;

	    case IIAPI_SCROLL_PRIOR :
		reference = IIAPI_POS_CURRENT;
		offset = -1;
		break;

	    case IIAPI_SCROLL_CURRENT :
		reference = IIAPI_POS_CURRENT;
		offset = 0;
		break;

	    case IIAPI_SCROLL_NEXT :
		reference = IIAPI_POS_CURRENT;
		offset = 1;
		break;

	    case IIAPI_SCROLL_LAST :
		reference = IIAPI_POS_END;
		offset = -1;
		break;

	    case IIAPI_SCROLL_AFTER :
		reference = IIAPI_POS_END;
		offset = 0;
		prefetch = 0;
		break;

	    case IIAPI_SCROLL_ABSOLUTE :
		reference = (scrollParm->sl_offset >= 0)
			    ? IIAPI_POS_BEGIN : IIAPI_POS_END;
		offset = scrollParm->sl_offset;
		break;

	    case IIAPI_SCROLL_RELATIVE :
		reference = IIAPI_POS_CURRENT;
		offset = scrollParm->sl_offset;
		break;

	    default :
		/*
		** Should not happen, default to simple NEXT.
		*/
		reference = IIAPI_POS_CURRENT;
		offset = 1;
		break;
	    }

	    if ( connHndl->ch_partnerProtocol >= GCA_PROTOCOL_LEVEL_67 )
	        parmNmem = IIapi_createGCAScroll( stmtHndl, reference, 
						  offset, prefetch );
	    else
		parmNmem = IIapi_createGCAFetch( prefetch, stmtHndl );

	    if ( ! parmNmem )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status == IIAPI_ST_SUCCESS )
		stmtHndl->sh_rowCount = prefetch;
	    else
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;
	}
	case SQL_SA_SCLS :
	    /*
	    ** Format and send GCA_CLOSE message.
	    */
	    if ( ! ( parmNmem = IIapi_createGCAClose( stmtHndl ) ) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    status = IIapi_sndGCA( sm_hndl, parmNmem, NULL );
    
	    if ( status != IIAPI_ST_SUCCESS )
	    {
		IIapi_freeMemTag( parmNmem->pm_memTag );
		success = FALSE;
	    }
	    break;

	case SQL_SA_RQID :
	{
	    /*
	    ** Read query GCA_QC_ID.
	    */
	    IIAPI_CONNHNDL *connHndl = IIapi_getConnHndl( sm_hndl );
	    IIAPI_GCA_BUFF *gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;

	    if ( ! (stmtHndl->sh_responseData.gq_repeatQueryHandle = 
		    IIapi_getConnIdHndl( connHndl, IIAPI_ID_REPEAT_QUERY, 
					 (GCD_ID *)gcaBuff->data )) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    stmtHndl->sh_responseData.gq_mask |= IIAPI_GQ_REPEAT_QUERY_ID;
	    break;
	}
	case SQL_SA_RCID :
	{
	    /*
	    ** Read cursor GCA_QC_ID.
	    */
	    IIAPI_GCA_BUFF *gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;

	    stmtHndl->sh_flags |= IIAPI_SH_CURSOR;
	    stmtHndl->sh_responseData.gq_mask |= IIAPI_GQ_CURSOR;
	    stmtHndl->sh_responseData.gq_cursorType = 0;
    
	    if ( ! (stmtHndl->sh_cursorHndl = 
		    IIapi_createIdHndl( IIAPI_ID_CURSOR, 
					(GCD_ID *)gcaBuff->data )) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
	    }
	    break;
	}
	case SQL_SA_RDSC :
	    /*
	    ** Read GCA_TDESCR message.
	    */
	    status = GetDescrInfo( stmtHndl, (IIAPI_GCA_BUFF *)parmBlock );
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    break;
    
	case SQL_SA_RCM :
	    /*
	    ** Read GCA copy map.
	    */
	    status = GetCopyMap( stmtHndl, (IIAPI_GCA_BUFF *)parmBlock );
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    break;

	case SQL_SA_RRSP :
	    /*
	    ** Read GCA_RESPONSE message.
	    */
	    GetResponseInfo( (IIAPI_GCA_BUFF *)parmBlock, stmtHndl );

	    /*
	    ** If we saved a GCA receive buffer during
	    ** processing we can now release it since
	    ** the query has completed.
	    */
	    if ( stmtHndl->sh_gcaBuff )
	    {
		IIapi_releaseGCABuffer( stmtHndl->sh_gcaBuff );
		stmtHndl->sh_gcaBuff = NULL;
	    }
	    break;

	case SQL_SA_RPRC :
	{
	    /*
	    ** Read GCA_RETPROC message.
	    */
	    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl( sm_hndl );
	    IIAPI_GCA_BUFF	*gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;
	    GCD_RP_DATA		*retProcData = (GCD_RP_DATA *)gcaBuff->data;
	    IIAPI_IDHNDL	*idHndl;

	    if ( ! (idHndl = IIapi_getConnIdHndl(connHndl, IIAPI_ID_PROCEDURE, 
						 &retProcData->gca_id_proc)) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
		break;
	    }
    
	    stmtHndl->sh_responseData.gq_mask |= IIAPI_GQ_PROCEDURE_ID;
	    stmtHndl->sh_responseData.gq_procedureHandle = idHndl;
	    stmtHndl->sh_responseData.gq_mask |= IIAPI_GQ_PROCEDURE_RET;
	    stmtHndl->sh_responseData.gq_procedureReturn = 
						 retProcData->gca_retstat;
	    break;
	}
	case SQL_SA_SETD :
	    /*
	    ** Set descriptor.
	    */
	    status = SetDescrInfo( stmtHndl, (IIAPI_SETDESCRPARM *)parmBlock );
	    if ( status != IIAPI_ST_SUCCESS )  success = FALSE;
	    break;

	case SQL_SA_INFO :
	{
	    /*
	    ** Return query info.
	    */
	    IIAPI_GETQINFOPARM	*gqiParm, *qinfo;
	    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl( sm_hndl );
	    IIAPI_ENVHNDL	*envHndl = connHndl ? connHndl->ch_envHndl 
	    					    : NULL;

	    gqiParm = (IIAPI_GETQINFOPARM *)stmtHndl->sh_parm;
	    qinfo = &stmtHndl->sh_responseData;

	    gqiParm->gq_flags = qinfo->gq_flags;
	    gqiParm->gq_mask = qinfo->gq_mask;
	    gqiParm->gq_rowCount = qinfo->gq_rowCount;
	    gqiParm->gq_readonly = qinfo->gq_readonly;
	    gqiParm->gq_procedureReturn = qinfo->gq_procedureReturn;
	    gqiParm->gq_procedureHandle = qinfo->gq_procedureHandle;
	    gqiParm->gq_repeatQueryHandle = qinfo->gq_repeatQueryHandle;
	    MEcopy( qinfo->gq_tableKey, IIAPI_TBLKEYSZ, gqiParm->gq_tableKey );
	    MEcopy( qinfo->gq_objectKey,IIAPI_OBJKEYSZ,gqiParm->gq_objectKey );

	    if ( ! envHndl  ||  envHndl->en_version < IIAPI_VERSION_6 )
	    	gqiParm->gq_mask &= ~IIAPI_GQ_ROW_STATUS;
	    else
	    {
		gqiParm->gq_cursorType = qinfo->gq_cursorType;
	    	gqiParm->gq_rowStatus = qinfo->gq_rowStatus;
		gqiParm->gq_rowPosition = qinfo->gq_rowPosition;
	    }

	    qinfo->gq_flags = 0;
	    qinfo->gq_mask = 0;
	    break;
	}
	case SQL_SA_DESC :
	{
	    /*
	    ** Return descriptor.
	    */
	    IIAPI_GETDESCRPARM *getDescrParm = 
	    		       (IIAPI_GETDESCRPARM *)stmtHndl->sh_parm;

	    getDescrParm->gd_descriptorCount = stmtHndl->sh_colCount;
	    getDescrParm->gd_descriptor = stmtHndl->sh_colDescriptor;
	    break;
	}
	case SQL_SA_COPY :
	    /*
	    ** Return copy map.
	    */
	    STRUCT_ASSIGN_MACRO( stmtHndl->sh_copyMap, 
		    ((IIAPI_GETCOPYMAPPARM *)stmtHndl->sh_parm)->gm_copyMap );
	    break;

	case SQL_SA_TUPL :
	    /*
	    ** Fake a GCA_TUPLE message using buffered data.
	    */
	    IIapi_liDispatch( IIAPI_EV_TUPLE_RCVD, sm_hndl, 
			      (II_PTR)stmtHndl->sh_gcaBuff, NULL );
	    break;

	case SQL_SA_RESP :
	    /*
	    ** Fake a GCA_RESPONSE message.  
	    **
	    ** The NULL parm block signals this condition 
	    ** to the rest of the code, which will treat 
	    ** this as a successful response with no 
	    ** additional data.
	    */
	    IIapi_liDispatch( IIAPI_EV_RESPONSE_RCVD, sm_hndl, NULL, NULL );
	    break;

	case SQL_SA_STAT :
	{
	    /*
	    ** Check data status.
	    */
	    IIAPI_GETCOLPARM *getColParm = 
	    		     (IIAPI_GETCOLPARM *)stmtHndl->sh_parm;

	    getColParm->gc_moreSegments = 
		(stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS) ? TRUE : FALSE;

	    if ( stmtHndl->sh_flags & IIAPI_SH_LOST_SEGMENTS )
	    {
		/*
		** Segments are lost if the application does
		** not follow the guidelines for reading BLOBs.
		** We allow this as type of operation to help
		** applications which may not really care about
		** BLOBs.  Return a warning message to let the
		** application know what has happened.
		*/
		stmtHndl->sh_flags &= ~IIAPI_SH_LOST_SEGMENTS;

		IIAPI_TRACE( IIAPI_TR_WARNING )
		    ( "%s: BLOB data discarded\n", sql_stmt_sm.sm_id );

		if ( ! IIapi_localError( sm_hndl, E_AP0017_SEGMENT_DISCARDED, 
					 II_SS5000R_RUN_TIME_LOGICAL_ERROR,
					 IIAPI_ST_WARNING ) )
		{
		    status = IIAPI_ST_OUT_OF_MEMORY;
		    success = FALSE;
		}
	    }

	    if ( stmtHndl->sh_flags & IIAPI_SH_LOST_TUPLES )
	    {
		/*
		** Tuples should only be lost due to internal
		** API errors or the server returning more
		** rows than requested for a cursor fetch.
		*/
		stmtHndl->sh_flags &= ~IIAPI_SH_LOST_TUPLES;

		if ( ! IIapi_localError( sm_hndl, E_AP0016_ROW_DISCARDED, 
					 II_SS5000R_RUN_TIME_LOGICAL_ERROR,
					 IIAPI_ST_FAILURE ) )
		{
		    status = IIAPI_ST_OUT_OF_MEMORY;
		    success = FALSE;
		}
	    }
	    break;
	}
	case SQL_SA_BUFF :
	{
	    /*
	    ** Handle GCA receive buffer.
	    */
	    IIAPI_GCA_BUFF *gcaBuff = (IIAPI_GCA_BUFF *)parmBlock;

	    /*
	    ** Save the GCA receive buffer if more data
	    ** available and buffer is not already saved.
	    */
	    if ( gcaBuff->data_len  &&  gcaBuff != stmtHndl->sh_gcaBuff )
	    {
		if ( stmtHndl->sh_gcaBuff )
		    IIapi_releaseGCABuffer( stmtHndl->sh_gcaBuff );

		IIapi_reserveGCABuffer( gcaBuff );
		stmtHndl->sh_gcaBuff = gcaBuff;
	    }

	    /*
	    ** Release saved GCA buffer if no more data available.
	    */
	    if ( ! gcaBuff->data_len  &&  stmtHndl->sh_gcaBuff )
	    {
		IIapi_releaseGCABuffer( stmtHndl->sh_gcaBuff );
		stmtHndl->sh_gcaBuff = NULL;
	    }
	    break;
	}
	case SQL_SA_DELH :
	    /*
	    ** Mark handle for deletion.
	    */
	    sm_hndl->hd_delete = TRUE;
	    QUremove( (QUEUE *)stmtHndl );
	    break;

	case SQL_SA_ERQA :
	    /*
	    ** Query active.
	    */
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s: Query active - must be cancelled\n", sql_stmt_sm.sm_id );

	    if ( ! IIapi_localError( sm_hndl, E_AP0007_INCOMPLETE_QUERY, 
				     II_SS5000R_RUN_TIME_LOGICAL_ERROR,
				     IIAPI_ST_FAILURE ) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
	    }
	    break;

	case SQL_SA_ERQD :
	    /*
	    ** Query completed.
	    */
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s: Query done - cannot be cancelled\n", sql_stmt_sm.sm_id );

	    if ( ! IIapi_localError( sm_hndl, E_AP0008_QUERY_DONE, 
				     II_SS5000R_RUN_TIME_LOGICAL_ERROR,
				     IIAPI_ST_FAILURE ) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
	    }
	    break;

	case SQL_SA_ERQC :
	    /*
	    ** Query cancelled .
	    */
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s: Query has been cancelled\n", sql_stmt_sm.sm_id );

	    if ( ! IIapi_localError( sm_hndl, E_AP0009_QUERY_CANCELLED, 
				     II_SS5000R_RUN_TIME_LOGICAL_ERROR,
				     IIAPI_ST_FAILURE ) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
	    }
	    break;

	case SQL_SA_ERQI :
	    /*
	    ** Query interrupted.
	    */
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s: Query interrupted\n", sql_stmt_sm.sm_id );

	    if ( ! IIapi_localError( sm_hndl, E_AP000A_QUERY_INTERRUPTED, 
				     II_SS5000R_RUN_TIME_LOGICAL_ERROR,
				     IIAPI_ST_FAILURE ) )
	    {
		status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
	    }
	    break;

	case SQL_SA_ERIF :
            /*
            ** API function called in wrong state.
            */
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "%s: invalid API function call\n", sql_stmt_sm.sm_id );

            if ( ! IIapi_localError( sm_hndl, E_AP0006_INVALID_SEQUENCE,
                                     II_SS5000R_RUN_TIME_LOGICAL_ERROR,
                                     IIAPI_ST_FAILURE ) )
	    {
                status = IIAPI_ST_OUT_OF_MEMORY;
		success = FALSE;
	    }
	    break;

	case SQL_SA_CBOK :
	    /*
	    ** Callback with success.
	    */
	    if ( stmtHndl->sh_callback )
	    {
		IIapi_appCallback(stmtHndl->sh_parm, sm_hndl, IIAPI_ST_SUCCESS);
		stmtHndl->sh_callback = FALSE;
	    }
	    break;

	case SQL_SA_CBFL :
	    /*
	    ** Callback with failure.
	    */
	    if ( stmtHndl->sh_callback )
	    {
		IIapi_appCallback(stmtHndl->sh_parm, sm_hndl, IIAPI_ST_FAILURE);
		stmtHndl->sh_callback = FALSE;
	    }
	    break;

	case SQL_SA_CBND :
	    /*
	    ** Callback with no data.
	    */
	    if ( stmtHndl->sh_callback )
	    {
		IIapi_appCallback(stmtHndl->sh_parm, sm_hndl, IIAPI_ST_NO_DATA);
		stmtHndl->sh_callback = FALSE;
	    }
	    break;

	case SQL_SA_CBGC :
	    /*
	    ** Callback IIapi_getColumns() with status.
	    */
	    if ( stmtHndl->sh_callback )
	    {
		IIAPI_GETCOLPARM *getColParm = 
				 (IIAPI_GETCOLPARM *)stmtHndl->sh_parm;

		IIapi_appCallback( stmtHndl->sh_parm, sm_hndl, 
				   (getColParm->gc_rowsReturned < 1) 
				       ? IIAPI_ST_NO_DATA : IIAPI_ST_SUCCESS );
		stmtHndl->sh_callback = FALSE;
	    }
	    break;

        case SQL_SA_ICBF :
            /*
            ** This may not have been a statement related function, and 
	    ** we may have a callback saved on the statement handle, so 
	    ** we carefully make the callback to the caller making sure 
	    ** not to disturb the statement handle.
            */
            IIapi_appCallback( (IIAPI_GENPARM *)parmBlock, sm_hndl, 
				IIAPI_ST_FAILURE);

            /*
            ** We must return the failure here rather than following the 
	    ** normal exit route to ensure that the statement handle
            ** callback does not get made.
            */
            return( FALSE );

	case SQL_SA_CNOK :
	    /*
	    ** Callback with success: query cancelled.
	    */
	    if ( stmtHndl->sh_cancelled )
	    {
		IIapi_appCallback( stmtHndl->sh_cancelParm, 
				   NULL, IIAPI_ST_SUCCESS );
		stmtHndl->sh_cancelled = FALSE;
	    }
	    break;

	case SQL_SA_CNWN :
	    /*
	    ** Callback with warning: duplicate cancel.
	    **
	    ** We don't want to disturb the existing
	    ** function or cancel callback info on the
	    ** statement handle, nor do we want to
	    ** return any other status other than the
	    ** warning (this will all be done when
	    ** the original cancel completes).
	    */
	    IIapi_appCallback( (IIAPI_GENPARM *)parmBlock, 
			       NULL, IIAPI_ST_WARNING );
	    break;

	case SQL_SA_CNFL :
	    /*
	    ** Callback with failure: cancel failed
	    */
	    if ( stmtHndl->sh_cancelled )
	    {
		IIapi_appCallback( stmtHndl->sh_cancelParm, 
				   sm_hndl, IIAPI_ST_FAILURE );
		stmtHndl->sh_cancelled = FALSE;
	    }
	    break;

        case SQL_SA_RCVE :
            /*
            ** Receive error.
            **
            ** We have received an invalid message.
            ** Since the connection state machine
            ** may ignore this particular message
            ** type, convert to a type which will
            ** ensure the proper handling in all
            ** state machines.
            */
            IIapi_liDispatch( IIAPI_EV_UNEXPECTED_RCVD, sm_hndl, NULL, NULL );
            return( FALSE );
    }

    /*
    ** If we couldn't complete the action, callback with failure.
    */
    if ( ! success  &&  stmtHndl->sh_callback )
    {
	IIapi_appCallback( stmtHndl->sh_parm, sm_hndl, status );
	stmtHndl->sh_callback = FALSE;
    }

    return( success );
}




/*
** Name EvaluateTuple: 
**
** Description:
**	This function evaluate the current predicates after a 
**	GCA_TUPLE is received and returns TRUE if the current 
**	request has been satisfied.
**
**	Note that there is a special case for cursor fetches:  
**	if this is the end of the last tuple we will continue 
**	and read the GCA_RESPONSE message which follows.  So, 
**	while the request is satisfied, we still return FALSE.
**	
**	This function does not test for overrun on the number 
**	of rows returned.  This can happen in two cases: an 
**	internal API error where a row spanning request (other 
**	than multi-row request) is allowed (gc_colFetch not 0 
**	at the end of a tuple), and a multi-row cursor fetch 
**	where the number of rows returned by the server is 
**	greater than the number requested.  IIapi_validGColCount() 
**	and IIapi_loadColumns() should assure that the former 
**	does not occur, while the cursor processing logic must 
**	handle the latter.
**
** History:
**	10-Nov-94 (gordy)
**	    Created.
**	13-Jun-95 (gordy)
**	    Updated to handle partial row fetches and long data types.
**	20-Jun-95 (gordy)
**	    Test flag rather than cursor handle.
**	22-Mar-99 (rajus01)
**	    Fixed a type error.
*/

static II_BOOL
EvaluateTuple( IIAPI_STMTHNDL *stmtHndl, IIAPI_GCA_BUFF *gcaBuff )
{
    IIAPI_GETCOLPARM	*getColParm = (IIAPI_GETCOLPARM *)stmtHndl->sh_parm;

    /*
    ** If we receive a tuple but the output buffers are
    ** filled, we are in big doo-doo.  This should only
    ** happen for an internal API error (heaven forbid!)
    ** or a cursor for which the server has returned more
    ** rows then requested.  
    **
    ** NOTE: when processing BLOBs we intentionally read
    ** ahead to check for end-of-segments.  If not end,
    ** the next segment is buffered so exclude this case.
    */
    if ( gcaBuff->data_len  &&
	 getColParm->gc_rowsReturned >= getColParm->gc_rowCount  &&
	 ! ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS ) )
    {
	/*
	** There is no clean way to recover from this: 
	** dump the tuple, flag the loss, and consider 
	** the fetch completed.
	*/
	gcaBuff->data_len = 0;
	stmtHndl->sh_flags |= IIAPI_SH_LOST_TUPLES;
	return( TRUE );
    }

    /*
    ** We need to know if the current info will satisfy the request.  
    ** The easiest way to do this is to go ahead and load the columns 
    ** into the output buffers.  This means any subsequent attempts to
    ** process the info will turn out to be no-ops, but this shouldn't 
    ** hurt anything.
    */
    IIapi_loadColumns( stmtHndl, getColParm, gcaBuff );

    /*
    ** If haven't yet satisfied the request, need more data.
    ** Note that this handles multi-row requests since
    ** IIapi_loadColumns() resets sh_colFetch at the end
    ** of a tuple when there are more rows to fetch.
    */
    if ( stmtHndl->sh_colFetch )  return( FALSE );

    /*
    ** So we have satisfied the request.  Normally we would be done,
    ** but there is a special case for the end of the last tuple of
    ** a cursor based fetch.  We first handle the cases where we have
    ** not yet reached the end of the tuple.  First, if we are reading
    ** a long data value and there are more segments to read, then we
    ** are done with the request but not done with the column nor the
    ** tuple.
    */
    if ( stmtHndl->sh_flags & IIAPI_SH_MORE_SEGMENTS )  return( TRUE );
    
    /*
    ** Next, while we have finished the last column requested, we still
    ** may not have finished the last column in the tuple.  When the
    ** last column is processed, sh_colIndex is reset to 0.  There is
    ** only one other way that sh_colIndex could be 0 and sh_colFetch
    ** is also 0 (which it is at this point as tested above): the first
    ** column is a long data type which is still being read.  Since we
    ** tested for this case above, sh_colIndex will only be 0 here if
    ** we have read the last column.
    */
    if ( stmtHndl->sh_colIndex )  return( TRUE );
    
    /*
    ** We have reached the end of a tuple and the request has been
    ** satisfied.  If this is a cursor fetch AND we have received
    ** all the tuples requested, we want to continue so that the
    ** RESPONSE message which should follow can be processed.  The
    ** fetch request will be completed at that point.  Otherwise,
    ** the request has been completed.
    **
    ** Note: during cursor processing, the RESPONSE message should
    ** be processed only be processed on the last requested tuple
    ** or when IIapi_getColumns() returns NO DATA or fewer than
    ** requested rows.  Restricting this test to the last requested
    ** row assures that the NO DATA or fewer rows conditions occurs
    ** when necessary.
    */
    if ( stmtHndl->sh_flags & IIAPI_SH_CURSOR  &&  stmtHndl->sh_rowCount <= 0 )
	return( FALSE );

    return( TRUE );
}



/*
** Name: GetResponseInfo
**
** Description:
**	Reads a GCA_RE_DATA object and updates the response
**	info structure in a statement handle.
**
** Input:
**	gcaBuff		GCA receive buffer.
**
** Output:
**	stmtHndl	Info placed in sh_responseData.
**
** Returns:
**	II_VOID.
**
** History:
**	 2-Oct-96 (gordy)
**	    Replaced original SQL state machines which a generic
**	    interface to facilitate additional connection types.
**	28-Jul-06 (gordy)
**	    Check for XA errors.
**	15-Mar-07 (gordy)
**	    Check for cursor row position and status info.
*/

static II_VOID
GetResponseInfo( IIAPI_GCA_BUFF *gcaBuff, IIAPI_STMTHNDL *stmtHndl )
{
    GCD_RE_DATA		*rspData;
    IIAPI_GETQINFOPARM	*getQInfoParm = &stmtHndl->sh_responseData;

    if ( ! gcaBuff )
    {
	/*
	** A fake GCA_RESPONSE message is generated for GCA_RETPROC
	** messages under certain GCA protocol levels (so as to simulate
	** the preferred protocol).  We treat this as a success with
	** no additional data.
	*/
    }
    else
    {
	rspData = (GCD_RE_DATA *)gcaBuff->data;
    
	getQInfoParm->gq_flags = rspData->gca_rqstatus;

	if ( rspData->gca_rqstatus & GCA_XA_ERROR_MASK  &&  rspData->gca_errd5 )
	    IIapi_xaError( (IIAPI_HNDL *)stmtHndl, rspData->gca_errd5 );

	if ( rspData->gca_rqstatus & GCA_RPQ_UNK_MASK )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "GetResponseInfo: unkown repeat query ID\n" );

	    /*
	    ** Not much we can do if this fails.
	    */
	    IIapi_localError( (IIAPI_HNDL *)stmtHndl,
			      E_AP0014_INVALID_REPEAT_ID,
			      II_SS50007_INVALID_SQL_STMT_ID,
			      IIAPI_ST_FAILURE );
	}

	if ( rspData->gca_rqstatus & GCA_FLUSH_QRY_IDS_MASK )
	{
	    /* !!!!! FIX-ME: flush cursor/repeat/procedure IDs */
	    IIAPI_TRACE( IIAPI_TR_TRACE )
		( "GetResponseInfo: need to flush IDs\n" );
	}

	if ( rspData->gca_rqstatus & GCA_ILLEGAL_XACT_STMT  &&
	     ! ( rspData->gca_rqstatus & GCA_FAIL_MASK ) )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		("GetResponseInfo: illegal transaction statement attempted\n");

	    /*
	    ** Not much we can do if this fails.
	    */
	    IIapi_localError( (IIAPI_HNDL *)stmtHndl,
			      E_AP0015_INVALID_TRANS_STMT,
			      II_SS5000R_RUN_TIME_LOGICAL_ERROR,
			      IIAPI_ST_FAILURE );
	}

	if ( rspData->gca_rqstatus & GCA_OBJKEY_MASK )
	{
	    getQInfoParm->gq_mask |= IIAPI_GQ_OBJECT_KEY;
	    MEcopy( (II_PTR)rspData->gca_logkey, IIAPI_OBJKEYSZ,
		    getQInfoParm->gq_objectKey );
	}
    
	if ( rspData->gca_rqstatus & GCA_TABKEY_MASK )
	{
	    getQInfoParm->gq_mask |= IIAPI_GQ_TABLE_KEY;
	    MEcopy( (II_PTR) rspData->gca_logkey, IIAPI_TBLKEYSZ,
		    (II_PTR) getQInfoParm->gq_tableKey );
	}
    
	if ( rspData->gca_rowcount >= 0 )
	{
	    getQInfoParm->gq_mask |= IIAPI_GQ_ROW_COUNT;
	    getQInfoParm->gq_rowCount = rspData->gca_rowcount;
	}

	/*
	** A row position of 0 with no row status flag is an
	** invalid condition and is used to indicate that no
	** row status information is available.
	*/
    	if ( rspData->gca_errd0 == 0  &&  rspData->gca_errd1 == 0 )
	{
	    getQInfoParm->gq_mask &= ~IIAPI_GQ_ROW_STATUS;
	    getQInfoParm->gq_rowStatus = 0;
	    getQInfoParm->gq_rowPosition = 0;
	}
	else
	{
	    getQInfoParm->gq_mask |= IIAPI_GQ_ROW_STATUS;
	    getQInfoParm->gq_rowStatus = rspData->gca_errd0;
	    getQInfoParm->gq_rowPosition = rspData->gca_errd1;
	}
    }
    
    return;
}




/*
** Name: GetDescrInfo
**
** Description:
**	Builds the column descriptor in a statement handle from
**	a GCA_TDESCR message.
**
** Input:
**	stmtHndl	Current statement handle.
**	gcaBuff		GCA buffer containing GCA_TDESCR message.
**
** Output:
**	None.
**
** Returns:
**	IIAPI_STATUS	API status.
**
** History:
**	 8-Mar-96 (gordy)(
**	    Added events to handle processing of large message which may
**	    be split across buffers.  Updated actions which read these
**	    messages.
**	15-Mar-96 (gordy)
**	    Added support for compressed varchars.
**	15-Mar-07 (gordy)
**	    Check for additional cursor attributes.
*/

static IIAPI_STATUS
GetDescrInfo( IIAPI_STMTHNDL *stmtHndl, IIAPI_GCA_BUFF *gcaBuff )
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    IIAPI_ENVHNDL	*envHndl = connHndl->ch_envHndl;
    GCA_RV_PARMS	*rcvParm = (GCA_RV_PARMS *)gcaBuff->gca_parm;
    GCD_TD_DATA		*tdData;
    GCD_COL_ATT		*colAtt, *colEnd;
    STATUS		status;

    colEnd = (GCD_COL_ATT *)(gcaBuff->data + gcaBuff->data_len);

    if ( connHndl->ch_flags & IIAPI_CH_MSG_CONTINUED )
	colAtt = (GCD_COL_ATT *)gcaBuff->data;
    else
    {
	tdData = (GCD_TD_DATA *)gcaBuff->data;

	/*
	** Process cursor attributes in descriptor.
	*/
	if ( stmtHndl->sh_flags & IIAPI_SH_CURSOR )
	{
	    stmtHndl->sh_responseData.gq_mask |= IIAPI_GQ_CURSOR;
	    stmtHndl->sh_responseData.gq_cursorType = 0;

	    if ( tdData->gca_result_modifier & GCA_READONLY_MASK )
		stmtHndl->sh_responseData.gq_readonly = TRUE;
	    else
	    {
		stmtHndl->sh_responseData.gq_readonly = FALSE;
	    	stmtHndl->sh_responseData.gq_cursorType |= IIAPI_CURSOR_UPDATE;
	    }

	    if ( tdData->gca_result_modifier & GCA_SCROLL_MASK )
		stmtHndl->sh_responseData.gq_cursorType |= IIAPI_CURSOR_SCROLL;
	}

	if ( tdData->gca_result_modifier & GCA_COMP_MASK )
	    stmtHndl->sh_flags |= IIAPI_SH_COMP_VARCHAR;

	if ( tdData->gca_l_col_desc > 0 )
	{
	    stmtHndl->sh_colDescriptor = (IIAPI_DESCRIPTOR *)
		MEreqmem( 0, sizeof(IIAPI_DESCRIPTOR) * tdData->gca_l_col_desc, 
			  TRUE, &status );

	    if ( ! stmtHndl->sh_colDescriptor )
	    {
		IIAPI_TRACE( IIAPI_TR_FATAL ) 
		    ( "GetDescrInfo: can't allocate descriptor\n" );
		return( IIAPI_ST_OUT_OF_MEMORY );
	    }
	}

	/*
	** We start sh_colCount at 0 and increment as
	** we read in column descriptors.  It should
	** increase to gca_l_col_desc, but will always
	** correctly reflect the number of descriptors
	** currently active in sh_colDescriptor.
	*/
	stmtHndl->sh_colCount = 0;
	colAtt = &tdData->gca_col_desc[ 0 ];
    }

    for( ; colAtt < colEnd; colAtt++ )
	if ( 
	     ! IIapi_cnvtGDV2Descr( 
		    envHndl,
		    colAtt->gca_attdbv.db_datatype, 
		    colAtt->gca_attdbv.db_length, 
		    colAtt->gca_attdbv.db_precision,
		    colAtt->gca_l_attname, colAtt->gca_attname, 
		    &stmtHndl->sh_colDescriptor[ stmtHndl->sh_colCount++ ] ) 
	   )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL ) 
		( "GetDescrInfo: can't allocate column name\n" );
	    return( IIAPI_ST_OUT_OF_MEMORY );
	}

    /*
    ** Set/reset the continuation flag if the message is continued/done
    ** so that we will process the correct portion of the message next 
    ** time this routine is called.
    */
    if ( rcvParm->gca_end_of_data )
	connHndl->ch_flags &= ~IIAPI_CH_MSG_CONTINUED;
    else
	connHndl->ch_flags |= IIAPI_CH_MSG_CONTINUED;

    return( IIAPI_ST_SUCCESS );
}



/*
** Name: SetDescrInfo
**
** Description:
**	Saves the applications descriptor for IIapi_setDescriptor().
**
** Input:
**	stmtHndl	Statement handle to receive descriptor.
**	setDescrParm	Parameter block with descriptor to be saved.
**
** Output:
**	None.
**
** Returns:
**	IIAPI_STATUS	IIAPI_ST_SUCCESS, IIAPI_ST_OUT_OF_MEMORY.
**
** History:
**	 2-Oct-96 (gordy)
**	    Replaced original SQL state machines which a generic
**	    interface to facilitate additional connection types.
*/

static IIAPI_STATUS
SetDescrInfo( IIAPI_STMTHNDL *stmtHndl, IIAPI_SETDESCRPARM *setDescrParm )
{
    STATUS	status;
    i4		i;

    if ( (stmtHndl->sh_parmCount = setDescrParm->sd_descriptorCount) )
    {
	if ( ! (stmtHndl->sh_parmDescriptor = (IIAPI_DESCRIPTOR *)
		    MEreqmem(0, sizeof( IIAPI_DESCRIPTOR ) * 
				stmtHndl->sh_parmCount, TRUE, &status)) )
	{
	    IIAPI_TRACE( IIAPI_TR_FATAL )
		( "SetDescrInfo: can't allocate descriptor\n" );
	    return( IIAPI_ST_OUT_OF_MEMORY );
	}
	
	for( i = 0; i < stmtHndl->sh_parmCount; i++ )
	{
	    STRUCT_ASSIGN_MACRO( setDescrParm->sd_descriptor[i],
				 stmtHndl->sh_parmDescriptor[i] );

	    if ( 
		 setDescrParm->sd_descriptor[i].ds_columnName   &&
		 ! (stmtHndl->sh_parmDescriptor[i].ds_columnName =
			STalloc(setDescrParm->sd_descriptor[i].ds_columnName)) 
	       )
	    {
		IIAPI_TRACE( IIAPI_TR_FATAL )
		    ( "SetDescrInfo: can't allocate column name\n" );
		return( IIAPI_ST_OUT_OF_MEMORY );
	    }
	}
    }

    return( IIAPI_ST_SUCCESS );
}


/*
** Name: GetCopyMap
**
** Description:
**	Builds the copy map in a statement handle from a GCA_C_FROM/INTO 
**	message.
**
** Input:
**	stmtHndl	Current statement handle.
**	gcaBuff		GCA buffer containing GCA_TDESCR message.
**
** Output:
**	None.
**
** Returns:
**	BOOL		TRUE if successful, FALSE if memory allocation failed.
**
** History:
**	 8-Mar-96 (gordy)
**	    Created.
**	15-Mar-96 (gordy)
**	    Added support for compressed varchars.
**      23-May-96 (lawst01)
**          Changed function type to STATUS to match prototype.
**	31-Jan-97 (gordy)
**	    Added het-net descriptor to IIapi_sndGCA() and statement 
**	    handle.  Needed during COPY FROM processing.
*/

static IIAPI_STATUS
GetCopyMap( IIAPI_STMTHNDL *stmtHndl, IIAPI_GCA_BUFF *gcaBuff )
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    IIAPI_ENVHNDL	*envHndl = connHndl->ch_envHndl;
    GCA_RV_PARMS	*rcvParm = (GCA_RV_PARMS *)gcaBuff->gca_parm;
    GCD_CP_MAP		*copyMap;
    GCD_TD_DATA		*gca_tdescr = NULL;
    PTR			ptr;
    STATUS		status;
    IIAPI_STATUS	api_status;
    i4			i;

    if ( connHndl->ch_flags & IIAPI_CH_MSG_CONTINUED )
	ptr = (PTR)gcaBuff->data;
    else
    {
	/*
	** Note: the only difference between GCD_CP_MAP and
	** GCD1_CP_MAP is the type of the last member.  Since
	** we don't access the last member here, we kludge
	** around and access both types via GCD_CP_MAP.
	*/
	copyMap = (GCD_CP_MAP *)gcaBuff->data;
    
	stmtHndl->sh_copyMap.cp_copyFrom = copyMap->gca_direction_cp ? TRUE 
								     : FALSE;
	stmtHndl->sh_copyMap.cp_flags = copyMap->gca_status_cp;
	stmtHndl->sh_copyMap.cp_errorCount = copyMap->gca_maxerrs_cp;

	stmtHndl->sh_copyMap.cp_fileName = 
	    (char *)MEreqmem( 0, copyMap->gca_l_fname_cp + 1, TRUE, &status );

	if ( ! stmtHndl->sh_copyMap.cp_fileName )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "GetCopyMap: couldn't allocate filename\n");
	    api_status = IIAPI_ST_OUT_OF_MEMORY;
	    goto fileAllocFail;
	}

	MEcopy( (PTR)copyMap->gca_fname_cp, copyMap->gca_l_fname_cp, 
		(PTR)stmtHndl->sh_copyMap.cp_fileName );

	stmtHndl->sh_copyMap.cp_logName = 
	    (char *)MEreqmem( 0, copyMap->gca_l_logname_cp + 1, TRUE, &status );

	if ( ! stmtHndl->sh_copyMap.cp_logName )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "GetCopyMap: couldn't allocate logname\n");
	    api_status = IIAPI_ST_OUT_OF_MEMORY;
	    goto logAllocFail;
	}
    
	MEcopy( (PTR)copyMap->gca_logname_cp, copyMap->gca_l_logname_cp, 
		(PTR)stmtHndl->sh_copyMap.cp_logName );

	if ( copyMap->gca_tup_desc_cp.gca_result_modifier & GCA_COMP_MASK )
	    stmtHndl->sh_flags |= IIAPI_SH_COMP_VARCHAR;

	stmtHndl->sh_copyMap.cp_dbmsDescr = (IIAPI_DESCRIPTOR *)
			MEreqmem( 0, sizeof(IIAPI_DESCRIPTOR) * 
				     copyMap->gca_tup_desc_cp.gca_l_col_desc, 
				  TRUE, &status );

	if ( ! stmtHndl->sh_copyMap.cp_dbmsDescr )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "GetCopyMap: couldn't allocate dbms descriptor\n");
	    api_status = IIAPI_ST_OUT_OF_MEMORY;
	    goto descrAllocFail;
	}
    
	if ( connHndl->ch_flags & IIAPI_CH_DESCR_REQ )
	{
	    stmtHndl->sh_cpyDescriptor =
		(PTR)MEreqmem( 0, sizeof(GCD_TD_DATA) + (sizeof(GCD_COL_ATT) * 
			       (copyMap->gca_tup_desc_cp.gca_l_col_desc - 1)), 
			       TRUE, &status );

	    if ( ! stmtHndl->sh_cpyDescriptor )
	    {
		IIAPI_TRACE( IIAPI_TR_ERROR )
		    ( "GetCopyMap: couldn't allocate GCA descriptor\n");
		api_status = IIAPI_ST_OUT_OF_MEMORY;
		goto gcaAllocFail;
	    }
    
	    gca_tdescr = (GCD_TD_DATA *)stmtHndl->sh_cpyDescriptor;
	    gca_tdescr->gca_tsize = copyMap->gca_tup_desc_cp.gca_tsize;
	    gca_tdescr->gca_id_tdscr = copyMap->gca_tup_desc_cp.gca_id_tdscr;
	    gca_tdescr->gca_l_col_desc = 
				copyMap->gca_tup_desc_cp.gca_l_col_desc;
	    gca_tdescr->gca_result_modifier = 
				copyMap->gca_tup_desc_cp.gca_result_modifier;
	}

	stmtHndl->sh_copyMap.cp_fileDescr = 
		(IIAPI_FDATADESCR *)MEreqmem( 0, sizeof(IIAPI_FDATADESCR) * 
						 copyMap->gca_l_row_desc_cp,
					      TRUE, &status );

	if ( ! stmtHndl->sh_copyMap.cp_fileDescr )
	{
	    IIAPI_TRACE( IIAPI_TR_ERROR )
		( "GetCopyMap: couldn't allocate file descriptor\n" );
	    api_status = IIAPI_ST_OUT_OF_MEMORY;
	    goto copyAllocFail;
	}
    
	/*
	** We start cp_fileCount at 0 and increment as
	** we read in file descriptors.  It should
	** increase to gca_l_row_desc_cp, but will always
	** correctly reflect the number of descriptors
	** currently active in cp_fileDescr.
	*/
	stmtHndl->sh_copyMap.cp_fileCount = 0;
	stmtHndl->sh_copyMap.cp_dbmsCount = 
				copyMap->gca_tup_desc_cp.gca_l_col_desc;
	stmtHndl->sh_colCount = copyMap->gca_tup_desc_cp.gca_l_col_desc;
	stmtHndl->sh_colDescriptor = stmtHndl->sh_copyMap.cp_dbmsDescr;

	for( i = 0; i < stmtHndl->sh_colCount; i++ )
	{
	    GCD_COL_ATT *gcaDescr;

	    gcaDescr  = &copyMap->gca_tup_desc_cp.gca_col_desc[i];
	    if ( gca_tdescr )  MEcopy( (PTR)gcaDescr, sizeof(*gcaDescr), 
					&gca_tdescr->gca_col_desc[i] );

	    if ( ! IIapi_cnvtGDV2Descr( envHndl,
					gcaDescr->gca_attdbv.db_datatype,
					gcaDescr->gca_attdbv.db_length,
					gcaDescr->gca_attdbv.db_precision,
					gcaDescr->gca_l_attname, 
					gcaDescr->gca_attname, 
					&stmtHndl->sh_colDescriptor[i] ) )
	    {
		IIAPI_TRACE( IIAPI_TR_ERROR )
		    ( "GetCopyMap: error allocating descriptor name\n");
		api_status = IIAPI_ST_OUT_OF_MEMORY;
		goto parmAllocFail;
	    }
	}

	ptr = (PTR)&copyMap->gca_row_desc_cp[0];
    }

    if ( 
	 rcvParm->gca_message_type == GCA_C_FROM  || 
	 rcvParm->gca_message_type == GCA_C_INTO 
       )
	api_status = GetPre50CopyMap(ptr, (PTR)(gcaBuff->data + 
						gcaBuff->data_len), stmtHndl);
    else
	api_status = GetPost50CopyMap(ptr, (PTR)(gcaBuff->data + 
						 gcaBuff->data_len), stmtHndl);

    if ( api_status != IIAPI_ST_SUCCESS )  goto parmAllocFail;
    
    /*
    ** Set/reset the continuation flag if the message is continued/done
    ** so that we will process the correct portion of the message next 
    ** time this routine is called.
    */
    if ( ! rcvParm->gca_end_of_data )
	connHndl->ch_flags |= IIAPI_CH_MSG_CONTINUED;
    else
    {
	connHndl->ch_flags &= ~IIAPI_CH_MSG_CONTINUED;

	/*
	** For some unknown reason, the column descriptor never
	** contains the column names.  Column names are available
	** in the file descriptors, but only for the subset of 
	** columns being directed to the file while the column 
	** descriptor contains all columns in the table.  Still, 
	** this is better than nothing.  Attempt to coordinate 
	** the file and column descriptors and copy the column 
	** names where possible.
	*/
	for( i = 0; i < stmtHndl->sh_copyMap.cp_fileCount; i++ )
	{
	    IIAPI_FDATADESCR *fd = &stmtHndl->sh_copyMap.cp_fileDescr[ i ];

	    /*
	    ** Delimiter entries have a column index of 0 even though
	    ** they are not related to column 0, so we need to watch
	    ** out for the special copy dummy datatypes.
	    */
	    if ( 
		 fd->fd_type != CPY_DUMMY_TYPE  && 
		 fd->fd_type != CPY_TEXT_TYPE  &&
		 fd->fd_column >= 0  && 
		 fd->fd_column < stmtHndl->sh_colCount  &&
		 ! stmtHndl->sh_colDescriptor[fd->fd_column].ds_columnName  &&
		 ! (stmtHndl->sh_colDescriptor[fd->fd_column].ds_columnName = 
							STalloc( fd->fd_name ))
	       )
	    {
		IIAPI_TRACE( IIAPI_TR_ERROR )
		    ( "GetCopyMap: error allocating column name\n");
		api_status = IIAPI_ST_OUT_OF_MEMORY;
		goto parmAllocFail;
	    }
	}
    }

    return( IIAPI_ST_SUCCESS );

  /*
  ** Clean-up after error occurs.
  */

  parmAllocFail:

    for( i = 0; i < stmtHndl->sh_copyMap.cp_fileCount; i++ )
    {
	if ( stmtHndl->sh_copyMap.cp_fileDescr[i].fd_name )
	    MEfree( (PTR)stmtHndl->sh_copyMap.cp_fileDescr[i].fd_name );
	
	if ( stmtHndl->sh_copyMap.cp_fileDescr[i].fd_delimValue )
	    MEfree( stmtHndl->sh_copyMap.cp_fileDescr[i].fd_delimValue );
	
	if ( stmtHndl->sh_copyMap.cp_fileDescr[i].fd_nullValue.dv_value )
	    MEfree(stmtHndl->sh_copyMap.cp_fileDescr[i].fd_nullValue.dv_value);
    }
    
    MEfree( (II_PTR)stmtHndl->sh_copyMap.cp_fileDescr );
    stmtHndl->sh_copyMap.cp_fileDescr = NULL;
    stmtHndl->sh_copyMap.cp_fileCount = 0;

    for( i = 0; i < stmtHndl->sh_colCount; i++ )
	if ( stmtHndl->sh_colDescriptor[i].ds_columnName )
	    MEfree( (PTR)stmtHndl->sh_colDescriptor[i].ds_columnName );

    stmtHndl->sh_colDescriptor = NULL;
    stmtHndl->sh_colCount = 0;

  copyAllocFail:

    if ( stmtHndl->sh_cpyDescriptor )
    {
	MEfree( stmtHndl->sh_cpyDescriptor );
	stmtHndl->sh_cpyDescriptor = NULL;
    }

  gcaAllocFail:

    MEfree( (PTR)stmtHndl->sh_copyMap.cp_dbmsDescr );
    stmtHndl->sh_copyMap.cp_dbmsDescr = NULL;
    stmtHndl->sh_copyMap.cp_dbmsCount = 0;

  descrAllocFail:

    MEfree( (II_PTR)stmtHndl->sh_copyMap.cp_logName );
    stmtHndl->sh_copyMap.cp_logName = NULL;

  logAllocFail:

    MEfree( (II_PTR)stmtHndl->sh_copyMap.cp_fileName );
    stmtHndl->sh_copyMap.cp_fileName = NULL;

  fileAllocFail:

    if ( connHndl )  connHndl->ch_flags &= ~IIAPI_CH_MSG_CONTINUED;

    return( api_status );
}



/*
** Name: GetPre50CopyMap
**
** Description:
**	Copy file descriptors from a pre GCA_PROTOCOL_LEVEL_50 copy map. 
**
** Input:
**	data		Start of file descriptors.
**	end		End of file descriptors.
**	stmtHndl	Statement handle to receive file descriptors.
**
** Output:
**	None.
**
** Returns:
**	IIAPI_STATUS	API status.
**
** History:
**      02-may-94 (ctham)
**          creation
**	23-Feb-96 (gordy)
**	    Fixed COPY INTO processing.  Converted copy tuple processing to use
**	    regular tuple processing, removing IIAPI_EV_CDATA_RCVD, IIAPI_SWCD
**	    and IIAPI_SRCD.  Revamped the COPYMAP data structures to properly
**	    represent copy information.  Actions 22, 31 now unused.
**	 8-Mar-96 (gordy)(
**	    Added events to handle processing of large message which may
**	    be split across buffers.  Updated actions which read these
**	    messages.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.
*/

static IIAPI_STATUS
GetPre50CopyMap( PTR data, PTR end, IIAPI_STMTHNDL *stmtHndl )
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    IIAPI_ENVHNDL	*envHndl = connHndl->ch_envHndl;
    GCD_CPRDD		*gcaDesc = (GCD_CPRDD *)data;
    IIAPI_FDATADESCR	*fileDesc;
    char		name[ GCA_MAXNAME + 1 ];
    STATUS		status;
    i4			len;
    
    for( ; gcaDesc < (GCD_CPRDD *)end; gcaDesc++ )
    {
	fileDesc = &stmtHndl->sh_copyMap.cp_fileDescr[
					 stmtHndl->sh_copyMap.cp_fileCount++ ];

	MEcopy( (PTR)gcaDesc->gca_domname_cp, GCA_MAXNAME, (PTR)name );
	name[ GCA_MAXNAME ] = '\0';
	len = STtrmwhite( name );
        
	if ( ! (fileDesc->fd_name = STalloc( name )) )
	    return( IIAPI_ST_OUT_OF_MEMORY );
	
	fileDesc->fd_type = gcaDesc->gca_type_cp;
	fileDesc->fd_length = (II_INT2)gcaDesc->gca_length_cp;
	fileDesc->fd_prec = 0;
	fileDesc->fd_column = gcaDesc->gca_tupmap_cp;
	fileDesc->fd_funcID = gcaDesc->gca_cvid_cp;
	fileDesc->fd_cvLen = gcaDesc->gca_cvlen_cp;
	fileDesc->fd_cvPrec = 0;

	fileDesc->fd_delimiter = gcaDesc->gca_user_delim_cp ? TRUE : FALSE;
        fileDesc->fd_delimLength = gcaDesc->gca_l_delim_cp;

	fileDesc->fd_delimValue = 
		(char *)MEreqmem( 0, gcaDesc->gca_l_delim_cp, TRUE, &status );

	if ( ! fileDesc->fd_delimValue )  return( IIAPI_ST_OUT_OF_MEMORY );

	MEcopy( (PTR)gcaDesc->gca_delim_cp,
		gcaDesc->gca_l_delim_cp, (PTR)fileDesc->fd_delimValue );
	
	fileDesc->fd_nullable = gcaDesc->gca_withnull_cp ? TRUE : FALSE;

	if ( fileDesc->fd_nullable )
	{
	    fileDesc->fd_nullInfo = gcaDesc->gca_nullen_cp ? TRUE : FALSE;

	    if ( fileDesc->fd_nullInfo )
	    {
		if ( ! IIapi_cnvtGDV2Descr( envHndl, IIAPI_CHR_TYPE,
					    gcaDesc->gca_nullen_cp,
					    0, 0, NULL, 
					    &fileDesc->fd_nullDescr ) )
		{
		    return( IIAPI_ST_OUT_OF_MEMORY );
		}

		fileDesc->fd_nullValue.dv_value = 
		    (char *)MEreqmem( 0, fileDesc->fd_nullDescr.ds_length,
				      FALSE, &status );

		if ( ! fileDesc->fd_nullValue.dv_value )
		    return( IIAPI_ST_OUT_OF_MEMORY );

		IIapi_cnvtGDV2DataValue( &fileDesc->fd_nullDescr,
					 gcaDesc->gca_nuldata_cp,
					 &fileDesc->fd_nullValue );
	    }
	}
    }

    return( IIAPI_ST_SUCCESS );
}




/*
** Name: GetPost50CopyMap
**
** Description:
**	Copy file descriptors from a post GCA_PROTOCOL_LEVEL_50 copy map. 
**
** Input:
**	data		Start of file descriptors.
**	end		End of file descriptors.
**	stmtHndl	Statement handle to receive file descriptors.
**
** Output:
**	None.
**
** Returns:
**	IIAPI_STATUS	API status.
**
** History:
**      02-may-94 (ctham)
**          creation
**	23-Feb-96 (gordy)
**	    Fixed COPY INTO processing.  Converted copy tuple processing to use
**	    regular tuple processing, removing IIAPI_EV_CDATA_RCVD, IIAPI_SWCD
**	    and IIAPI_SRCD.  Revamped the COPYMAP data structures to properly
**	    represent copy information.  Actions 22, 31 now unused.
**	 8-Mar-96 (gordy)(
**	    Added events to handle processing of large message which may
**	    be split across buffers.  Updated actions which read these
**	    messages.
**	15-Mar-96 (gordy)
**	    Two byte data lengths should be unsigned.
*/

static IIAPI_STATUS
GetPost50CopyMap( PTR data, PTR end, IIAPI_STMTHNDL *stmtHndl )
{
    IIAPI_CONNHNDL	*connHndl = IIapi_getConnHndl((IIAPI_HNDL *)stmtHndl);
    IIAPI_ENVHNDL	*envHndl = connHndl->ch_envHndl;
    GCD1_CPRDD		*gcaDesc = (GCD1_CPRDD *)data;
    IIAPI_FDATADESCR	*fileDesc;
    STATUS		status;
    char		name[ GCA_MAXNAME + 1 ];
    i4			len;
    
    for( ; gcaDesc < (GCD1_CPRDD *)end; gcaDesc++ )
    {
	fileDesc = &stmtHndl->sh_copyMap.cp_fileDescr[
					 stmtHndl->sh_copyMap.cp_fileCount++ ];

	MEcopy( (PTR)gcaDesc->gca_domname_cp, GCA_MAXNAME, (PTR)name );
	name[ GCA_MAXNAME ] = '\0';
	len = STtrmwhite( name );
        
	if ( ! (fileDesc->fd_name = STalloc( name )) )
	    return( IIAPI_ST_OUT_OF_MEMORY );
	
	fileDesc->fd_type = gcaDesc->gca_type_cp;
	fileDesc->fd_length = (II_INT2)gcaDesc->gca_length_cp;
	fileDesc->fd_prec = gcaDesc->gca_precision_cp;
	fileDesc->fd_column = gcaDesc->gca_tupmap_cp;
	fileDesc->fd_funcID = gcaDesc->gca_cvid_cp;
	fileDesc->fd_cvLen = gcaDesc->gca_cvlen_cp;
	fileDesc->fd_cvPrec = gcaDesc->gca_cvprec_cp;

	fileDesc->fd_delimiter = gcaDesc->gca_user_delim_cp ? TRUE : FALSE;
        fileDesc->fd_delimLength = gcaDesc->gca_l_delim_cp;

	fileDesc->fd_delimValue = 
		(char *)MEreqmem( 0, gcaDesc->gca_l_delim_cp, TRUE, &status );

	if ( ! fileDesc->fd_delimValue )  return( IIAPI_ST_OUT_OF_MEMORY );

	MEcopy( (PTR)gcaDesc->gca_delim_cp,
		gcaDesc->gca_l_delim_cp, (PTR)fileDesc->fd_delimValue );
	
	fileDesc->fd_nullable = gcaDesc->gca_withnull_cp ? TRUE : FALSE;

	if ( fileDesc->fd_nullable )
	{
	    fileDesc->fd_nullInfo = gcaDesc->gca_l_nullvalue_cp ? TRUE 
								    : FALSE;

	    if ( fileDesc->fd_nullInfo )
	    {
		if ( ! IIapi_cnvtGDV2Descr(
				envHndl,
				gcaDesc->gca_nullvalue_cp->gca_type,
				gcaDesc->gca_nullvalue_cp->gca_l_value,
				gcaDesc->gca_nullvalue_cp->gca_precision,
				0, NULL, &fileDesc->fd_nullDescr ) )
		{
		    return( IIAPI_ST_OUT_OF_MEMORY );
		}

		fileDesc->fd_nullValue.dv_value = 
		    (char *)MEreqmem( 0, fileDesc->fd_nullDescr.ds_length,
				      FALSE, &status );

		if ( ! fileDesc->fd_nullValue.dv_value )
		    return( IIAPI_ST_OUT_OF_MEMORY );

		IIapi_cnvtGDV2DataValue( 
				&fileDesc->fd_nullDescr,
				gcaDesc->gca_nullvalue_cp->gca_value,
				&fileDesc->fd_nullValue );
	    }
	}
    }

    return( IIAPI_ST_SUCCESS );
}

