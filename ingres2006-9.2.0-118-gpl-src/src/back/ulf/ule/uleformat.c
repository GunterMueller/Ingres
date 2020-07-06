/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ulf.h>
#include    <cs.h>
#include    <scf.h>
#include    <er.h>
#include    <me.h>
#include    <pc.h>
#include    <st.h>
#include    <tr.h>
#include    <lo.h>
#include    <pm.h>
#include    <uleint.h>
#include    <stdarg.h>
#include    <erclf.h>
#include    <dmf.h>

#define  NUM_ER_ARGS 12

/**
**
**  Name: ULEFORMAT.C - Format and log error messages.
**
**  Description:
**      This utility routine formats and/or logs error messages to
**	the installation wide error log file.
**
**	    ule_initiate - Initiate the ule system
**          ule_format - Format error message.
**
**
**  History:
**      31-oct-1985 (derek)    
**          Created new for 5.0.
**	03-jun-1986 (derek)
**	    Change the format of the message in ule_format().
**	02-jul-1986 (fred)
**	    Add code so that msg_buf is logged if flag == ULE_MESSAGE.
**	03-aug-1987 (fred)
**	    Added code to put [node name] <server name, server id> onto
**	    the front of any logged message
**      06-aug-1987 (fred)
**          Added ule_initiate() routine to fetch the string (above) to
**          to put onto any string.  The main purpose of separating this
**	    this routine out is to allow the shared image system to continue
**	    to be used.  Otherwise, we have ulf_shr->dmf_shr(home of LG)-
**	    ->ulf_shr  --><--
**      27-sep-88 (thurston)
**	    When ERlookup() reported it could not find a message, ule_format()
**	    assumed that the error code returned by ERlookup() was a system
**	    error, and attempted to look that up as such, usually never finding
**	    it.  Now, if an error code comes back from the initial call to
**	    ERlookup(), that error is looked at to see if it is an ER error.
**	    If so, ERlookup() is called again to look up that error as an INGRES
**	    error.
**      27-sep-88 (thurston)
**	    The lack of proper type casting in this file was *HORRENDOUS*!!!
**	    Some of this code would have only worked on VMS-like machines where
**	    byte swapping makes it so a 4 byte integer can be passed to a
**	    routine that is expecting a 2 byte integer, and pointers are
**	    pointers are pointers.  I tried to clean up as much as I could.
**	    All ME and ER calls should now have proper args.
**      23-jan-1989 (roger)
**          Revised to conform to latest ERlookup specification.
**	08-feb-1989 (greg)
**	    add num_params arg to ERlookup
**	19-may-1989 (jrb)
**	    Add support to ule_format for generic errors.  If param is NULL
**	    then no value is filled in, otherwise the generic error code is
**	    filled in (this is taken care of by ERlookup).
**	06-jun-89 (fred)
**	    Alter pattern so that ule_initiate will initiate even if ule_format
**	    was called previously.  This is necessary so that the server can
**	    print errors before callint ule_initiate.
**	31-aug-1992 (rog)
**	    Prototype ULF.  (Cannot prototype ule_format due to varargs.)
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      23-Jun-92 (radve01)
**          axp_osf: session id extended to 16 bytes (64-bit addr)
**	23-Sep-1997 (kosma01)
**	    Change the way ule_format formats sid, (SCF_SESSION) into the
**	    ule message header. Base the size of ule_session on SCF_SESSION
**	    instead of 8. Get the hexidecimal characters from an array 
**	    instead of adding a nibble to '0'.
**	24-Sep-1997 (hanch04)
**	    Fix compile errors.
**	01-oct-1997 (nanpr01)
**	    Change the hostname to be 8 bytes instead of 6.
**	29-dec-1997 (canor01)
**	    Save the error code in a local variable.
**	20-May-1998 (hanch04)
**	    Changed name of ule_format to ule_doformat.
**	    The macro ule_format calles ule_doformat.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	06-oct-2000 (somsa01)
**	    The size of ule_server has been increased to 18.
**	17-oct-2000 (somsa01)
**	    Moved the definition of ULE_MHDR to uleint.h, and moved
**	    Ule_mhdr and Ule_started to uledata.c .
**      17-Apr-2001 (hweho01)
**          Changed ule_doformat() to a variable-arg list function.
**          The mismatch of parameter numbers between the calling routines     
**          and ule_doformat() caused stack corruptions during runtime.
**          According to ANSI standard, variable argument list should 
**          be used when the numbers of parameter passed by the callers  
**          are not fixed.   
**	28-jun-2001 (devjo01)
**	    Allow for a node nickname" to prevent confusion when
**	    logging messages in a clustered installation where the
**	    cluster member nodes have names identical in the 1st
**	    eight places.
**      10-jun-2003 (stial01)
**          When getting session information, also request qbuf, qlen and 
**          error number(s) being traced. Maybe send query to errlog, trace log
**      10-jun-2003 (stial01)
**          Moved TRdisplay after validation of qlen
**      11-jun-2003 (stial01)
**          Trace errors only if error_code != 0, check for some exceptions
**      17-jun-2003 (stial01)
**          Also printing SCI_LAST_QBUF, SCI_LAST_QLEN
**      18-jun-2003 (stial01)
**          Added trace for some additional internal errors
**      09-jul-2003 (stial01)
**          Added (query) trace for E_UL0017_DIAGNOSTIC.
**          Fixed query text length sent to errlog
**      11-jul-2003 (stial01)
**          Fixed stack var initialization
**      26-nov-2003 (stial01)
**          Don't trace errors from exception handlers. (b111359)
**      21-Mar-2005 (mutma03)
**          Simplified the node translation code for clusters.
**      28-Mar-2005 (drivi01)
**          Moved definition of nickname to the top of the if block to avoid
**          compile errors on windows.	
**      21-feb-2007 (stial01)
**          Print queries longer than ER_MAX_LEN, added SCI_PSQ_QBUF, SCI_PSQ_QLEN
**      26-Apr-2007 (hanal04) SIR 118196
**          Add process pid to ULE header.
**	3-May-2007 (bonro01)
**	    Bad change 486441 - local variable definitions must come before
**	    executable statements.
**/

/*
**  Global Definitions of variables.
*/

GLOBALREF  ULE_MHDR  Ule_mhdr;	/* `dm_cname_max::[server_name, session_id]:' */
GLOBALREF  i4        Ule_started;        /* did we fill in the above */

/*{
** Name: ule_initiate	- Provide server constant info for the server header
**
** Description:
**      This routine takes in the server constant parameters SERVER NAME and
**      NODE NAME, placing them in an area which is easily found to place on 
**      all error messages.
**
** Inputs:
**      node_name                       Pointer to node name, which is
**      l_node_name                     characters long
**      server_name                     Same for server name
**      l_server_name
**
** Outputs:
**      None.
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      06-Aug-1987 (fred)
**          Created.
**      21-Mar-2005 (mutma03)
**          cleanup of node translation code for clusters.
[@history_template@]...
*/
VOID
ule_initiate( char *node_name, i4  l_node_name, char *server_name,
	      i4  l_server_name )
{
    PID proc_id;
    char fmt[20];

    if (Ule_started <= 0)
    {
	char nickname[128]; /* qwikfix: must be CX_MAX_NODE_NAME_LEN */

        PCpid(&proc_id);
	
	/*
	** if a node name is provided, check for a nickname.
	** This allows users to provide customized names for
	** the error logs, which may be useful if site has
	** multiple machines which are identical or similar
	** in their 1st 8 characters.   The need for this
	** was made clear when developing Ingres clusters on
	** UNIX on machines usilatu01 & usilatu02!
	*/

	MEfill(sizeof(ULE_MHDR), (u_char)' ', (PTR)&Ule_mhdr);	
	CXnode_name(nickname);
	CXnickname_from_node_name(nickname,nickname);
	node_name = nickname;
	l_node_name = STlength(nickname);

	MEmove( l_node_name,
		(PTR)  node_name,
		(char) ' ',
		sizeof(Ule_mhdr.ule_node),
		(PTR)  &Ule_mhdr.ule_node[0]);
	MEmove(	l_server_name,
		(PTR)  server_name,
		(char) ' ',
		sizeof(Ule_mhdr.ule_server),
		(PTR)  &Ule_mhdr.ule_server[0]);
	MEfill(	sizeof(Ule_mhdr.ule_session),
		(u_char) '*',
		(PTR)  &Ule_mhdr.ule_session[0]);
	Ule_mhdr.ule_pad1[0] = ':',
	    Ule_mhdr.ule_pad1[1] = ':',
	    Ule_mhdr.ule_pad1[2] = '['; 	
	Ule_mhdr.ule_pad2[0] = ',',
	    Ule_mhdr.ule_pad2[1] = ' '; 	
        STprintf( fmt, "%s", PIDFMT );
        STprintf(Ule_mhdr.ule_pid, fmt, proc_id);
        Ule_mhdr.ule_pad3[0] = ',',
            Ule_mhdr.ule_pad3[1] = ' ';
	Ule_mhdr.ule_pad4[0] = ']',
	    Ule_mhdr.ule_pad4[1] = ':',
	    Ule_mhdr.ule_pad4[2] = ' '; 	
	Ule_started = 1;
    }
}

/*{
** Name: ule_format	- Format and write error message.
**
** Description:
**      This routine manages formatting and logging error messages.
**	The message described by the error_code and/or the
**	clerror code are looked up in the error file and formatted with the
**	arguments into a message.  The message is optionally sent to error
**	logger and/or optionally returned to the caller.  The caller can
**	pass an already formatted message that will be entered in the
**	error log as a comment.  The SQLSTATE status code is not logged but
**	is returned to the caller if the pointer passed into this routine
**	to hold the SQLSTATE status code is not NULL.
**        Note, this routine SHOULD NOT be used by front end programs
**      since SCF is called to get the language.
**	  Note, this routine allows parameters to be formatted into the
**	error text.  This can be done by placing formatting specifications
**	into the error text in the error file.  The formatting specifications
**	in the error file start with the '%' character.  This is followed by
**	a number which specifies which parameter pair is to be used in
**	filling in this portion of the text.  This number is followed by
**	a letter which specifies how the parameter is to be interpreted
**	with regard to datatype.  The parameters passed to this routine
**	should occur in pairs, where the first number designates how many
**	bytes are to be used in interpreting this value and the second
**	parameter in the pair is a pointer to some data which is to be
**	interpreted according to the character designator found in the
**	error text.  Valid format designators include c, a character string,
**	d, an integer, f, a float, and x, a hexadecimal number.  If the
**	length designator associated with a character string is 0, the
**	character string will be assumed to be null-terminated; else, it
**	will be interpreted to be a character string of that length.
**
** Inputs:
**      error_code                      Error_code.
**	clerror				CL error descriptor.
**	flag				Either ULE_LOG, ULE_MESSAGE, or
**					ULE_LOOKUP.  Both ULE_LOG and
**					ULE_LOOKUP will result in the error_code
**					being looked up and formatted for the
**					caller.  ULE_LOG and ULE_MESSAGE will
**					also result in writing the formatted
**					message to the log file.
**	sqlstate 			Pointer to place to put SQLSTATE status
**					code .  If this pointer is NULL the
**					SQLSTATE status code will not be
**					returned.
**	msg_buffer			If flag == ULE_MESSAGE this
**					is address of buffer containing
**					the message.
**	msg_buf_length	                If flag == ULE_MESSAGE this is
**					the length of the message.
**					Otherwise this is the length
**					of the message buffer to be returned.
**	err_code			location of error status address
**	num_parms			Number of parameter pairs passed.
**	param_1...param_n		This is optional formatting arguments.
**					The number and type of parameters must
**					coincide with the error text, which
**					contains formatting specifications,
**					that is associated with the error code
**					that was passed in.
**					
**
** Outputs:
**	sqlstate			The SQLSTATE status code for this error.
**      msg_buffer                      If not zero, the formatted message is
**					returned here.
**	msg_length			Address to return number of characters
**					placed in msg_buffer.
**	err_code			Reason for error status.
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	05-nov-1985 (derek)
**          Created new for 5.0.
**	03-jun-1986 (derek)
**	    Change the message format so that the timestamp is generated
**	    if the message will be logged, and that non-existent system
**	    error or normal system errors are not reported.
**      13-mar-87 (jennifer)
**          Changed call to ERlookup to include language parameter.
**          Noted in comments that ule_format should not be called
**          front-end programs since SCF needs to be called to 
**          get language.
**      17-apr-87 (stec)
**          Commented out line of code that stored '-' at the end of err. msg.
**	18-nov-87 (mmm)
**	    Changed to handle variable number of arguments in a more portable
**	    fashion.  Now declare maximum number of arguments and refer to 
**	    them by name rather than walking the stack.  Also package the 
**	    arguments into an array for ERlookup.
**	19-sep-88 (rogerk)
**	    Took xDEBUG flags from around the code to send error messages to
**	    the II_DBMS_LOG file.
**      27-sep-88 (thurston)
**	    When ERlookup() reported it could not find a message, ule_format()
**	    assumed that the error code returned by ERlookup() was a system
**	    error, and attempted to look that up as such, usually never finding
**	    it.  Now, if an error code comes back from the initial call to
**	    ERlookup(), that error is looked at to see if it is an ER error.
**	    If so, ERlookup() is called again to look up that error as an INGRES
**	    error.
**      23-jan-1989 (roger)
**          Revised to conform to latest ERlookup specification: changed
**	    CL_SYS_ERR to CL_ERR_DESC; pass this to ERlookup along with
**	    `error_code' if the latter is a CL return status.  Also got
**	    rid of Code From Hell that assumed it could look inside the
**	    the CL_ERR_DESC (at `error', which no longer exists in the
**	    UNIX implementation).  Cleaned up output: clarified messages,
**	    simplified formats; don't look up clerror after ERlookup
**	    fails on error_code- it's confusing and of little help.  On
**	    the other hand, DO look up ER error if clerror lookup fails.
**	    Undid `check ERlookup return to see if it's an ER error' hack
**	    described above; ERlookup always returns either an ER error
**	    or OK.  The new logic prevents the problem described.
**	19-may-1989 (jrb)
**	    Add support to ule_format for generic errors.  If param is NULL
**	    then no value is filled in, otherwise the generic error code is
**	    filled in.
**	24-oct-92 (andre)
**	    replaced calls to ERlookup() with calls to ERslookup()
**
**	    have ule_format() return SQLSTATE status code instead of generic
**	    error message
**	23-Jun-92 (radve01)
**          axp_osf: session id extended to 16 bytes (64-bit addr)
**	01-oct-1997 (nanpr01)
**	    bug 86077 : Junk char is getting printed.
**	29-dec-1997 (canor01)
**	    Save the error code in a local variable.
**	20-May-1998 (hanch04)
**	    Changed name of ule_format to ule_doformat.
**	    The macro ule_format calles ule_doformat.
**	    
*/
/* VARARGS31 */
DB_STATUS
ule_doformat(int  error_code,   CL_ERR_DESC *clerror,
             int  flag,         DB_SQLSTATE *sqlstate,
             char *msg_buffer,  int msg_buf_length,
             int  *msg_length,  int *err_code,
             int   num_parms,
             ... )
{
    struct  {
		ULE_MHDR	hdr;
/*  FIX ME should message size be ER_MAX_LEN - sizeof(ULE_MHDR) */
		char		message[ER_MAX_LEN];
	    }	    buffer;
    i4		    i;
    i4	    length = 0;
    i4		    text_length;
    i4	    status;
    CL_ERR_DESC	    sys_err;
    i4              language;
    SCF_SESSION	    sid;
    SCF_SCI	    info[9];
    SCF_CB	    scf_cb;
    ER_ARGUMENT     er_args[NUM_ER_ARGS];
    char	    hex_chars[16] = {'0','1','2','3','4','5','6','7',
                                     '8','9','a','b','c','d','e','f'};
    i4	    local_error_code = error_code;
    char	    *qbuf = NULL;
    char	    *prev_qbuf = NULL;
    char	    *psqbuf = NULL;
    i4		    qlen = 0;
    i4		    prev_qlen = 0;
    i4		    psqlen = 0;
    i4		    trace_errno = 0;
    i4		    prlen;
    char	    *prbuf;
    i2		    hdr_size;

    va_list ap;

    if (Ule_started == 0)
    {
	MEfill(sizeof(ULE_MHDR), (u_char)' ', (PTR)&Ule_mhdr);
	Ule_started = -1;
    }

    info[0].sci_code = SCI_SID;
    info[0].sci_length = sizeof(sid);
    info[0].sci_aresult = (char *) &sid;
    info[0].sci_rlength = 0;
    info[1].sci_code = SCI_LANGUAGE;
    info[1].sci_length = sizeof(language);
    info[1].sci_aresult = (char *) &language;
    info[1].sci_rlength = 0;
    scf_cb.scf_length = sizeof(SCF_CB);
    scf_cb.scf_type = SCF_CB_TYPE;
    scf_cb.scf_ascii_id = SCF_ASCII_ID;
    scf_cb.scf_facility = DB_ULF_ID;
    scf_cb.scf_session = DB_NOSESSION;
    scf_cb.scf_len_union.scf_ilength = 2;
    scf_cb.scf_ptr_union.scf_sci = (SCI_LIST *) &info[0];
    /* scf_error is not usually an input parameter */
    if (flag == ULE_LOG || flag == ULE_MESSAGE)
    {
	info[2].sci_code = SCI_QBUF;
	info[2].sci_length = sizeof(qbuf);
	info[2].sci_aresult = (char *) &qbuf;
	info[2].sci_rlength = 0;
	info[3].sci_code = SCI_QLEN;
	info[3].sci_length = sizeof(qlen);
	info[3].sci_aresult = (char *) &qlen;
	info[3].sci_rlength = 0;
	info[4].sci_code = SCI_TRACE_ERRNO;
	info[4].sci_length = sizeof(trace_errno);
	info[4].sci_aresult = (char *) &trace_errno;
	info[4].sci_rlength = 0;
	info[5].sci_code = SCI_PREV_QBUF;
	info[5].sci_length = sizeof(prev_qbuf);
	info[5].sci_aresult = (char *) &prev_qbuf;
	info[5].sci_rlength = 0;
	info[6].sci_code = SCI_PREV_QLEN;
	info[6].sci_length = sizeof(prev_qlen);
	info[6].sci_aresult = (char *) &prev_qlen;
	info[6].sci_rlength = 0;
	info[7].sci_code = SCI_PSQ_QBUF;
	info[7].sci_length = sizeof(psqbuf);
	info[7].sci_aresult = (char *) &psqbuf;
	info[7].sci_rlength = 0;
	info[8].sci_code = SCI_PSQ_QLEN;
	info[8].sci_length = sizeof(psqlen);
	info[8].sci_aresult = (char *) &psqlen;
	info[8].sci_rlength = 0;
	scf_cb.scf_len_union.scf_ilength = 9;
    }
    status = scf_call(SCU_INFORMATION, &scf_cb);
    if (status)
    {
	language = 1;
	sid = 0;
    }
    if (!language)
	language = 1;

    /* package up the stack parameters into an ER_ARGUMENT array */

    va_start( ap, num_parms );
    for( i = 0; ( i < num_parms )  && ( i < NUM_ER_ARGS ) ; i++ )
     {
       er_args[i].er_size = (int) va_arg( ap, int );
       er_args[i].er_value = (PTR) va_arg( ap, PTR );
     }
     va_end( ap );

    *err_code = E_DB_OK;

    if (flag == 0 || flag == ULE_LOG || flag == ULE_LOOKUP)
    {
	/* Get INGRES message text. */

	status = ERslookup( local_error_code,
			    CLERROR(local_error_code)? clerror : (CL_ERR_DESC*) NULL,
			    ER_TIMESTAMP,
			    (sqlstate) ? sqlstate->db_sqlstate : (char *) NULL,
			    &buffer.message[length],
			    (i4) (sizeof(buffer.message) - length),
			    (i4) language,
			    &text_length,
			    &sys_err,
			    num_parms,
			    er_args
			  );

	if (status != OK)
	{
	    CL_ERR_DESC    junk;

	    STprintf(&buffer.message[length],
			"ULE_FORMAT: Couldn't look up message %x ",
			local_error_code);
	    length = STlength(buffer.message);

	    STprintf(&buffer.message[length], "(reason: ER error %x)\n",status);
	    length = STlength(buffer.message);
	    status = ERslookup( (i4) status,
				&sys_err,
				(i4) 0,
				(sqlstate) ? sqlstate->db_sqlstate
					   : (char *) NULL,
				&buffer.message[length],
				(i4) (sizeof(buffer.message) - length),
				(i4) language,
				&text_length,
				&junk,
				0,
				(ER_ARGUMENT *) NULL
			     );
	    if (status != OK)
	    {
		STprintf(&buffer.message[length],
		    "... ERslookup failed twice:  status = %x", status);
		length = STlength(buffer.message);
	    }
	    else
	    {
		length += text_length;
	    }

	    *err_code = E_UL0002_BAD_ERROR_LOOKUP;
	}
	else
	{
	    length = text_length;
	}

	/* Get system message text. */

	if (clerror)
	{
	    buffer.message[length++] = '\n';

	    status = ERslookup(	(i4) 0,
				clerror,
				(i4) 0,
				(sqlstate) ? sqlstate->db_sqlstate
					   : (char *) NULL,
				&buffer.message[length],
				(i4) (sizeof(buffer.message) - length),
				(i4) language,
				&text_length,
				&sys_err,
				0,
				(ER_ARGUMENT *) NULL
			      );
	    if (status != OK)
	    {
	        CL_ERR_DESC    junk;

		STprintf(&buffer.message[length],
			"ULE_FORMAT: Couldn't look up system error ");
		length = STlength(buffer.message);

	        STprintf(&buffer.message[length],
		    "(reason: ER error %x)\n", status);
	        length = STlength(buffer.message);
	        status = ERslookup( (i4) status,
				    &sys_err,
				    (i4) 0,
				    (sqlstate) ? sqlstate->db_sqlstate
					       : (char *) NULL,
				    &buffer.message[length],
				    (i4) (sizeof(buffer.message) - length),
				    (i4) language,
				    &text_length,
				    &junk,
				    0,
				    (ER_ARGUMENT *) NULL
			         );
	        if (status != OK)
	        {
		    STprintf(&buffer.message[length],
		        "... ERslookup failed twice:  status = %x", status);
		    length = STlength(buffer.message);
	        }
		else
		{
		    length += text_length;
		}
		*err_code = E_UL0001_BAD_SYSTEM_LOOKUP;
	    }
	    else
	    {
		length += text_length;
	    }
	}

	/* Copy into callers buffer if requested. */

	if (msg_buffer && msg_buf_length)
	{
	    if (msg_buf_length < length)
		length = msg_buf_length;
	    MEcopy((PTR)buffer.message, length, (PTR)msg_buffer);
	    *msg_length = length;
	}
    }
    else if (flag == ULE_MESSAGE)
    {
	if (!msg_buffer || !msg_buf_length)
	{
	    *err_code = E_UL0003_BADPARM;
	    return (E_DB_ERROR);
	}
	MEcopy((PTR)msg_buffer, msg_buf_length, (PTR)buffer.message);
	length = msg_buf_length;
    }

    if (flag == ULE_LOG || flag == ULE_MESSAGE)
    {
	SCF_SESSION tmp_sid = sid;

	MEcopy((PTR)&Ule_mhdr, sizeof(ULE_MHDR), (PTR)&buffer.hdr);

        for (i = (sizeof(Ule_mhdr.ule_session)) ; --i >= 0; )
	{
            buffer.hdr.ule_session[i] = hex_chars[(tmp_sid & 0xf)];
            tmp_sid >>= 4;
	}
	TRdisplay("ERsend--->'%t'\n", length + sizeof(ULE_MHDR), &buffer);
	status = ERsend(ER_ERROR_MSG, (PTR)&buffer, 
	    length + sizeof(ULE_MHDR), &sys_err);
    }

    /*
    ** Maybe print current and last query to the errlog.log and II_DBMS_LOG
    ** Can trigger this using "set trace point sc924",
    ** or "set trace point sc924 decimal-error-number"
    ** 
    */
    if (sid && error_code != 0 &&
	(trace_errno == -1 || 
	trace_errno == error_code ||
	error_code == E_DM_MASK + 0x002A || /* E_DM002A_BAD_PARAMETER */
	error_code == E_SC0206_CANNOT_PROCESS ||
	error_code == E_SC0220_SESSION_ERROR_MAX ||
	error_code == E_UL0017_DIAGNOSTIC ||
	error_code == E_SC0221_SERVER_ERROR_MAX))
    {
	if (psqlen && psqbuf)
	{
	    STprintf(buffer.message, ERx("PQuery: "));
	    hdr_size = STlength(buffer.message);
	    for (prbuf = psqbuf, prlen = psqlen; prlen > 0; ) 
	    {
		if (prlen > 16384)
		    prlen = 16384;
		if (prlen > ER_MAX_LEN - hdr_size)
		    i = ER_MAX_LEN - hdr_size;
		else
		    i = prlen;
		MEcopy((PTR)prbuf, i, &buffer.message[hdr_size]);
		TRdisplay("%t\n", i + sizeof(ULE_MHDR) + hdr_size, &buffer);
		status = ERsend(ER_ERROR_MSG, (PTR)&buffer, 
		    i + sizeof(ULE_MHDR) + hdr_size, &sys_err);
		hdr_size = 0;
		prlen -= i;
		prbuf += i;
	    }
	}
	if (qlen && qbuf)
	{
	    STprintf(buffer.message, ERx("Query:  "));
	    hdr_size = STlength(buffer.message);
	    for (prbuf = qbuf, prlen = qlen; prlen > 0; ) 
	    {
		if (prlen > 16384)
		    prlen = 16384;
		if (prlen > ER_MAX_LEN - hdr_size)
		    i = ER_MAX_LEN - hdr_size;
		else
		    i = prlen;
		MEcopy((PTR)prbuf, i, &buffer.message[hdr_size]);
		TRdisplay("%t\n", i + sizeof(ULE_MHDR) + hdr_size, &buffer);
		status = ERsend(ER_ERROR_MSG, (PTR)&buffer, 
		    i + sizeof(ULE_MHDR) + hdr_size, &sys_err);
		hdr_size = 0;
		prlen -= i;
		prbuf += i;
	    }
	}
	if (prev_qlen && prev_qbuf)
	{
	    STprintf(buffer.message, ERx("LQuery: "));
	    hdr_size = STlength(buffer.message);
	    for (prbuf = prev_qbuf, prlen = prev_qlen; prlen > 0; ) 
	    {
		if (prlen > 16384)
		    prlen = 16384;
		if (prlen > ER_MAX_LEN - hdr_size)
		    i = ER_MAX_LEN - hdr_size;
		else
		    i = prlen;
		MEcopy((PTR)prbuf, i, &buffer.message[hdr_size]);
		TRdisplay("%t\n", i + sizeof(ULE_MHDR) + hdr_size, &buffer);
		status = ERsend(ER_ERROR_MSG, (PTR)&buffer, 
		    i + sizeof(ULE_MHDR) + hdr_size, &sys_err);
		hdr_size = 0;
		prlen -= i;
		prbuf += i;
	    }
	}
    }
    
    if (*err_code == E_DB_OK)
	return (E_DB_OK);
    else
	return (E_DB_WARN);
}
