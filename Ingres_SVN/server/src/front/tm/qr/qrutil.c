# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>		 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<si.h>
# include	<me.h>
# include	<cm.h>
# include	<qr.h>
# include	<ui.h>
# include	<er.h>
# include <uigdata.h>
# include	"erqr.h"
# include	"qrhelp.h"
/**
** Name:	qrutil.qsc - QR utility routines
**
** Description:
**
**	Contains utility routines for QR module, primarily
**	routines for reading query input and sending statements
**	to the BE.
**
**	Public (extern):
**		qrgettok(qrb, put)
**		qrputtok(qrb)
**		qrprintf(qrb, s, a1, ...)
**		qrputc(qrb, c)
**		qrscanstmt(s)
**		qrtblowner(qrb, atable, the_owner)
**
**	Private (static):
**		qrgetchar(qrb)
**		qrungetc(qrb, c)
**
** History:
**	19-feb-1988	Added qrscript for BUG 1547 (neil)
**	19-may-88 (bruceb)
**		Changed ME[c]alloc calls to MEreqmem calls.
**	08-jun-88 (bruceb)
**		Changed code to use SQL.
**	23-jun-88 (bruceb)
**		Changed code to use a i4  as return type from
**		a getchar routine, not a char.
**	11-aug-88 (bruceb)
**		Added ricks' kanji changes.
**	01-sep-88 (bruceb)	Fix for bugs 2580 and 3273
**		No longer generate new lines between SQL/QUEL statements.
**		These blank lines confuse the dbms into incorrectly assigning
**		the query language to any following 'generic' queries (such as
**		'help') in the go-block.  Generate a single space instead.
**	29-sep-88 (bruceb)
**		Add new IIQR routines as functional equivalents to
**		qrtblowner for specific object types.
**	11-oct-88 (bruceb)
**		When getting a token inside a help command, treat the '*'
**		character as potentially part of an id, due to pattern matching
**		in the help command.
**	27-oct-88 (bruceb)
**		New routine for use in help link (register).
**	19-may-89 (teresal)
**		Added new routine for help rule.
**	18-sep-89 (teresal - Ken L.'s fix)
**		Part of the fix for bug 6640. Eliminated sometimes stripping
**		of excess blanks from string literals.  Also, fixed problem 
**		where a long string literal would be truncated when sent to
**		the backend.
**	31-oct-89 (teresal)
**		Fix for bug 3895.  Modified qrgettok so embedded quotes (\")
**		in QUEL string literals no longer terminate the string literal.
**	02/10/90 (dkh) - Added routine qradd() to speed up TM.  It replaces
**			 qrprintf() is some circumstances.
**	22-mar-91 (kathryn)
**		Added function: IIQRevo_EventOwner.
**	01-aug-91 (kathryn)
**		Add handling for boolean qrb->striptok. Set TRUE in qrgettok
**		when a newline (<cr>) is encountered within a literal string 
**		token, set FALSE in qrputok when nl is stripped before sending
**		to DBMS.
**	17-mar-92 (seg)
**		Can't do arithmetic or dereference PTRs.
**	19-jun-92 (rogerl)
**		qrscanstmt() now knows about ','s within delimited stings
**		moved IIQRrel_exists() from qrmlthlp.qsc, added qrrelx()
**	19-oct-92 (rogerl)
**		New parameter for 6.5 FErelexists()
**	19-oct-92 (rogerl)
**		Rework qroutalloc() to dynamically allocate buffers.
**	9-dec-1992 (rogerl)
**		Change qrprintf's involving object/owner names to qrdiprintf()
**		Which will ensure display of a delimited ident is quoted OK.
**	07-feb-93 (rogerl)
**		Add qrcpbuf() for use with largish buffers.  Overcome u_i2
**		copy limit.  Change MEcopy() of potentially large buffers to
**		qrcpbuf(); change MEreqmem() to MEreqlng() for similar reason.
**	11-feb-93 (rogerl) 
**		Fix up an integration omission (qroutalloc().offset)
**	10-aug-93 (dkhor)
**		Add checks for DOUBLEBYTE stuff in qrgettok().  
**              Without this fix, DBL chars whose 2nd byte is in the range of 
**		0x21 - 0x7e will be mistaken for one of the SQL special 
**		chars/tokens.
**	20-aug-93 (dkhor)
**		Overzealous parsing for DBL chars 'eats' up tokens before they
**		could be properly analyzed.  Remove this 'extra' parsing for
**		DBL chars.
**	15-Nov-1993 (tad)
**		Bug #56449
**		Replace %x with %p for pointer values where necessary.
**	31-aug-1995 (emmag)
**		Remove IIbreak in IIQR_relexists which is causing a 
**		help to generate an Interrupt Query which is treated
**		as an error condition in the case where on_error is
**		set to rollback transaction for a session.  
**	28-sep-1995 (canor01)
**		Make IIbreak in IIQR_relexists dependent on whether or not 
**		there is an open transaction.  The help will not generate
**		an error condition inside a transaction.  A help as the
**		first command in a net connection can receive junk without
**		the IIbreak to clear the buffers.
**	23-sep-1996 (sarjo01)
**		Bug 77944: give qroutalloc() a 2nd arg, newlen. Check that
**		the greater of newlen and 2 * current size is used.
**	28-oct-96 (nick)
**	    Reinstate IIbreak() call for all platforms if not in MST.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-Nov-1999 (hanch04)
**          First stage of updating ME calls to use OS memory management.
**          Make sure me.h is included.  Use SIZE_TYPE for lengths.
**	04-apr-2002 (somsa01)
**	    Added LEVEL1_OPTIM for i64_win to prevent the Terminal Monitor
**	    from SEGV'ing when two successive "help permit" statements are
**	    issued via SEP.
**	05-apr-2002 (somsa01)
**	    Removed LEVEL1_OPTIM restriction from i64_win.
**	31-Jul-2002 (fanra01)
**	    Need to cast char to u_char before converting to an int.
**	    FF is actually 255 but was being cast to -1 in the convert.
*/
# define	QRSTMTSIZ	500
STATUS		qrputtok();
static VOID	qrungetc();
static i4	qrgetchar();
STATUS		qrtblowner();
VOID		qrcpbuf( char * olde, char * newb, i4  size );
VOID		qrtrimwhite( char *str );
VOID		qrrelok( QRB *qrb, char *relation, ER_MSGID errid, bool *chkd,
		    bool *truth );
static	bool	qrrelx( QRB *qrb, char *relation, ER_MSGID id );
static	bool	qrdorequote( char *id, char *buf );
static	bool	in_quotes = FALSE;
static	bool	end_quotes = FALSE;
FUNC_EXTERN	STATUS	FEchkname();
/*{
** Name:	qrgettok() - read an input token from query go-block
**
** Description:
**	The next single token is scanned from the query go-block of
**	the active QRB.	 The token is placed in the token buffer of
**	QRB.  A token is a single identifier, number or punctuation
**	character, except that quoted strings are considered single
**	tokens.	 White space and comments are disposed of via
**	qrputtok() (q.v.)
**
** Inputs:
**	QRB	*qrb;	// the active QR control block.
**	bool	put;	// put flag: if TRUE do a qrputtok() before
**			   returning to send token just read to BE
**			   or copy to stmt echo buffer of active QRB.
**
** Outputs:
**	QRB	*qrb;	// the active QR control block (updated).
**
** Returns:
**	bool
**
** History:
**	7/86 (peterk) - created
**	27-jul-87 (daver)	Checked the qrb's query language to set
**				the quoting char to ' or "
**	11-feb-88 (neil)	Checked for SQL -- comments.
**	31-oct-89 (teresal)	Fix for bug 3895.
**      01-aug-91 (kathryn)
**              Set qrb->striptok when a newline (<cr>) is encountered within
**              a literal string token.
**      1-aug-92 (rogerl)
**		ERx quotechars; delimited ids : treat " as token delimiter for
**		sql as well.  \ is quotechar in this case.
*/
bool
qrgettok(qrb, put)
register QRB	*qrb;
i4		put;
{
	char	c;
	i4	c_nat;
	bool	id = FALSE;
	bool	isdbl;
	char	quotechar;
#ifdef DEBUG
	SIfprintf(stderr, ERx("<qrgettok(x%p, %d)>"), qrb, put);
	qrbdump(qrb);
#endif
	if (qrb->lang == DB_QUEL)
		quotechar = ERx( '"' );
	else
		quotechar = ERx( '\'' );
	for (;;)
	{
	    /* Skip white space */
	    while ((c_nat = qrgetchar(qrb)) != EOF)
	    {
		c = (char)c_nat;
		if (!CMwhite(&c))
		    break;
		/* skip second byte of kanji space */
		if (CMbytecnt(&c) == 2)
		    c_nat = qrgetchar(qrb);
	    }
	    /* un-get non-space char just read to remove from token */
	    qrungetc(qrb, c_nat);
	    if (CMwhite(qrb->token))
		qrputtok(qrb);
	    /* check for EOF (end of query buffer input ) */
	    if (c_nat == EOF)
	    {
#ifdef DEBUG
		SIfprintf(stderr, ERx("<qrgettok:0>"));
		qrbdump(qrb);
#endif
		qrb->eoi = 1;
		return FALSE;
	    } /* end if (c_nat == EOF) */
	    /* now re-read last char again (to get it into token) */
	    qrgetchar(qrb);
	    /* scan for comment */
	    if (c_nat == '/')
	    {
		/* could be a comment */
		if ((c_nat = qrgetchar(qrb)) == '*')
		{
		    /* yes this is really a comment */
		    register bool	endstar = FALSE;
		    qrb->comment = TRUE;
		    for (;;)
		    {
			if ((c_nat = qrgetchar(qrb)) != EOF)
			{
			    if (c_nat == '*')
			    {
				endstar = TRUE;
			    }
			    else if (c_nat == '/' && endstar)
			    {
				break;
			    }
			    else
			    {
				endstar = FALSE;
			    }
			}
			else
			{
			    break;
			}
		    } /* end for (;;) */
		    qrputtok(qrb);
		    qrb->comment = FALSE;
		    /* Continue buffering */
		} /* end of real comment */
		else if (c_nat != EOF)
		{
		    /* not really a comment */
		    qrungetc(qrb, c_nat);
		    goto terminate;
		}
	    } /* end if (c_nat == '/') */
	    /*
	    ** BUG 981 - Allow '--' as SQL comment to end of line. (ncg)
	    ** 		 SQL need not bother with script buffers.
	    */
	    else if (qrb->lang == DB_SQL && c_nat == '-') /* First hyphen */
	    {
		if ((c_nat = qrgetchar(qrb)) == '-')	/* Second hyphen */
		{
		    qrb->comment = TRUE;
		    for (;;)
		    {
			c_nat = qrgetchar(qrb);
			if (c_nat == '\n' || c_nat == EOF)
			    break;
		    }
		    qrputtok(qrb);
		    qrb->comment = FALSE;
		}
		else if (c_nat != EOF) /* Backup the char if !hyphen and !EOF */
		{
		    /* not really a comment */
		    qrungetc(qrb, c_nat);
		    goto terminate;
		}
	    } 					/* End if checking for -- */
	    else
	    {
		break;
	    }
	}
	if (c_nat == (i4)quotechar)
	{
	    /* Don't bother buffering strings - flush them */
	    in_quotes = TRUE;
	    while ((c_nat = qrgetchar(qrb)) != EOF)
	    {
		c = (char)c_nat;
		/*
		** kludge to get around 'statement not reached' compiler
		** warnings in while loop on non-double byte machines where
		** CMdbl1st() is defined to FALSE. Must use isdbl instead
		** of just using the CMdbl1st() each time.
		*/
		isdbl = CMdbl1st((char *)&c);
		while (isdbl)
		{
			c_nat = qrgetchar(qrb);	/* 2nd byte */
			c_nat = qrgetchar(qrb);	/* next char */
			c = (char)c_nat;
			isdbl = CMdbl1st((char *)&c);
		}
		if (c_nat == (i4)quotechar)
			break;
		if (c_nat == '\n')
			qrb->striptok = TRUE;
		/* Process any embedded quotes (\") in a QUEL string literal
		** by reading the backslashed quote and then continue
		** to look for the end quote (Bug 3895)
		*/
		if (qrb->lang == DB_QUEL && c_nat == ERx( '\\') )
		    if (c_nat = qrgetchar(qrb) == EOF)
			break;
	    } /* End of while */
	    /* Continue buffering */
	    end_quotes = TRUE;
	}
			/* 
			** if a delimited id, token can contain any chars
			*/
	else if ( ( c_nat == (i4) ERx( '"' ) ) &&  ( qrb->lang == DB_SQL ) )
	{
	    in_quotes = TRUE;
	    while ( ( ( c_nat = qrgetchar( qrb )) != EOF)
		&& ( c_nat != (i4) ERx( '"' ) ) )
	    {
			/* quoting of " is like ' in SQL, double it to embed
			*/
			/* should the quoter be stripped out of the string? */
		if ( c_nat == ERx( '"') )
		    if (c_nat = qrgetchar(qrb) == EOF)
			break;
	    }
	    end_quotes = TRUE;
	}
	else
	{
	    for (;;)
	    {
		c = (char)c_nat;
		while (!STindex(ERx(" \t\n\f,.;:()-+=[]'\"/\\?^*|&"), &c, 0))
		{
		    id = TRUE;
                    if (CMdbl1st((char *)&c))
                    {
                        do
                        {
                            c_nat = qrgetchar(qrb);     /* 2nd byte */
                            c_nat = qrgetchar(qrb);     /* next char */
                            c = (char)c_nat;
                        } while (CMdbl1st((char *)&c));
                        if (c_nat == EOF)
                            break;
                    }
                    else if ((c_nat = qrgetchar(qrb)) == EOF)
                        break;
		    c = (char)c_nat;
		}
		if ((c_nat == '*') && (qrb->nextdesc->qrsd_type == HELPstmt))
		{
		    if ((c_nat = qrgetchar(qrb)) != EOF)
			continue;
		}
		if (id && (c_nat != EOF))
		{
		    qrungetc(qrb, c_nat);
		}
		break;
	    }
	}
	/* I guess some where in here is where we have to check for
	** double keywords, e.g. BEGIN/END TRANSACTION, DEFINE PERMIT, etc.
	*/
terminate:
	*qrb->t = EOS;
	if (put)
	    qrputtok(qrb);
#ifdef DEBUG
	SIfprintf(stderr, ERx("<qrgettok:1>"));
	qrbdump(qrb);
#endif
	return TRUE;
}
/*{
** Name:	qrputtok() - send a token to BE and into stmt buffer
**
** Description:
**	A token is moved from the token buffer of the active
**	QRB.  It is normally sent directly to the BE (via IIwritedb)
**	and may also be copied into the stmt echo buffer if the echo
**	option is set or if this is a special statement (such as
**	PRINT) which requires special interpretation.
**
**	Allocation of stmt echo buffer is done here and overflow is
**	handled by re-allocating a larger buffer.
**
**	Comments are not sent to the BE, as a performance gain,
**	however newlines within comments are sent to the BE as
**	harmless white space to ensure that line numbers in the BE's
**	query block match those of the input query.
**
** Inputs:
**	QRB	*qrb;	// the active QR control block.
**
** Outputs:
**	QRB	*qrb;	// the active QR control block (updated).
**
** Returns:
**	bool
**
** Side effects:
**	The .nonnull flag of the active QRB may be set to TRUE.
**	Query tokens and white space may be sent to the BE.
**	The stmt echo buffer of the QRB may be allocated,
**	re-allocated or updated.
**	The QRB token buffer and token buffer ptr are re-initialized.
**
** History:
**	7/86 (peterk) - created
**	29-sep-89 (teresal) 
**		Modified so Runtime INGRES statements are not sent to the 
**		backend.
**	21-mar-90 (teresal)
**		When allocating more memory for statement buffer, put in fix
**		to only copy what has been put into the old buffer - qrputtok
**		use to copy stray characters from the end of buffer. Fix for
**		bug 8310.
**	01-aug-91 (kathryn)
**		Strip nl from string literal before sending to DBMS.
*/
STATUS
qrputtok(qrb)
register QRB	*qrb;
{
	register char	*tp = qrb->token;
	register bool	special;
	STATUS		stat;
	/*
	** In quel, the we know the first token of the next statement
	** already, so we can just check and see if it is print or help.
	** In SQL, we need a flag in the qrb to tell us not to send the
	** token to the backend.
	*/
	if (qrb->lang == DB_QUEL)
	{
		special = ( (qrb->nextdesc->qrsd_type == PRINTstmt) ||
			  (qrb->nextdesc->qrsd_type == HELPstmt) ||
			  (qrb->nextdesc->qrsd_type == RUNTIMEstmt) );
	}
	else
	{
		special = qrb->nobesend;
	}
	if (!*tp)
	    return OK;
	*qrb->t = EOS;
	if (qrb->echo || special || !qrb->nosemi || qrb->script)
	{
	    if (qrb->s == NULL)
	    {
		/* allocate a stmt echo buffer */
		qrb->stmtbufsiz = QRSTMTSIZ;
		if ((qrb->stmt = (char *)MEreqmem((u_i4)0, (u_i4)QRSTMTSIZ,
		    (bool)FALSE, &stat)) == NULL)
		{
		    qrb->s = NULL;
		    return stat;
		}
		qrb->s = qrb->stmt;
	    }
	    /* Set the sfirst flag regardless of language */
	    if (qrb->sfirst<0 && !qrb->comment && !CMwhite(tp))
		qrb->sfirst = qrb->s - qrb->stmt;
	    while (*tp)
	    {
		/*
		** check stmt echo buffer o'flow
		**	Make sure we allocate more if we're within
		**	3 bytes of the boundry. This is to facilitate
		**	adding a \n character at the end of SQL statements
		**	in the statement buffer (and leaves room for second
		**	byte of a kanji character).  The \n character is
		**	checked for in the FSTM and when echoing the
		**	statement to the screen.
		*/
		if (qrb->s - qrb->stmt >= qrb->stmtbufsiz - 3)
		{
		    char	*oldstmt = qrb->stmt;
		    i4		n, oldlen;
		    /*
		    ** Calculate the length of what has been put into the
		    ** old buffer so far. 
		    */
		    oldlen = qrb->s - qrb->stmt;
		    qrb->stmtbufsiz *= 2;
		    if ((qrb->s = (char *)MEreqmem((u_i4)0,
			    (SIZE_TYPE)qrb->stmtbufsiz,
			    (bool)FALSE, &stat))
			== NULL)
		    {
			qrb->s = NULL;
			return stat;
		    }
		    /* Only copy for the length of the old buffer - Bug 8310 */
		    n = STlcopy(qrb->stmt, qrb->s, oldlen);
		    qrb->stmt = qrb->s;
		    qrb->s = qrb->stmt + n;
		    /* free old stmt buffer */
		    MEfree((PTR)oldstmt);
		}
		CMcpyinc(tp, qrb->s);
	    }
	    *qrb->s = EOS;
	}
	/*
	** No longer attempting to keep the line number in the query
	** in sync with the server (this is impossible anyway, since
	** parts of a TM go-block become separate go-blocks in the dbms).
	** Thus, if in a comment, do nothing with the tokens.  Also, if not
	** in a comment, between query statements, and the current token is
	** 'blank', don't pass it down to the dbms.  (For bug 2580)
	*/
	if (!special && !qrb->comment)
	{
	    if ((qrb->nextdesc->qrsd_type != NOstmt)
		|| (!CMwhite(qrb->token)) || in_quotes)
	    {
		if (end_quotes)
		{
			end_quotes = FALSE;
			in_quotes = FALSE;
		}
		/* strip newlines from token before sending */
		if (qrb->striptok)   
		{
			char	*p1, *p2;
			p1 = p2 = qrb->token;
			while (*p1)
			{
			    if (*p1 == '\n')
				p1++;
			    else
				CMcpyinc(p1,p2);
			}
			*p2 = EOS;
			qrb->striptok = FALSE;
		}
		IIwritio(FALSE, (i2 *)0, TRUE, DB_CHR_TYPE, 0, qrb->token);
	    }
	    else
	    {
		IIwritedb(ERx(" "));
	    }
	}
	if (!qrb->comment && (!CMwhite(qrb->token) || in_quotes))
	    qrb->nonnull = TRUE;
	qrb->t = qrb->token;
	return OK;
}
/*{
** Name:	qrgetchar() - get a character from the query go-block
**
** Description:
**	Read a single character from the query go-block, according
**	to the setting in the active QRB.  If .inbuf is non-NULL
**	simply pick up the char pointed to and advance .inbuf to
**	the next char.	Otherwise, .infunc must be non-NULL so call
**	the function to obtain a single character.
**	The chaaracter read is added to the QRB token buffer.
**
**	The QRB maintains a single character "peek" buffer used by
**	qrgetchar().
**
**	If the token buffer overflows, call qrputtok() to dispose of
**	the current contents (this can only happen with a comment or
**	long quoted string constant.)
**
**	If script mode is on and qr_script is set to save script tokens
**	then rather than print the query text to a script file, we 
**	add it the script buffer, which should be flushed at some point.
** Inputs:
**	QRB	*qrb;	// active QR control block.
**
** Outputs:
**	QRB	*qrb;	// active QR control block (updated).
**
** Returns:
**	i4	// either a char value or EOF
**
** Side Effects:
**	The QRB token buffer and token buffer ptr are updated.
**	Tokens may be written to the BE or moved into the stmt
**	echo buffer on token buffer o'flow.
**	The QRB's line count is updated if the character read is a
**	newline.
**
** History:
**	8/86 (peterk) - created
*/
static i4
qrgetchar(qrb)
register QRB	*qrb;
{
	register char	q;
#ifdef DEBUG
	register i4	from = 0;
#endif
	if (qrb->inbuf)
	{
	    q = *qrb->inbuf++;
	}
	else if (qrb->peek)
	{
	    q = qrb->peek;
	    qrb->peek = '\0';
#ifdef DEBUG
	    from = 1;
#endif
	}
	else
	{
	    q = (*qrb->infunc)();
#ifdef DEBUG
	    from = 2;
#endif
	}
	if (q == '\n')
	    qrb->lines++;
#ifdef DEBUG
	if (q)
	    SIfprintf(stderr, ERx("<%d: %d %c >"), from, q,q);
	else
	    SIfprintf(stderr, ERx("<%d: EOF >"), from);
	SIflush(stderr);
#endif
	if (q)
	{
	    if (qrb->t - qrb->token > QRTOKENSIZ - 2)
	    {
		qrputtok(qrb);
	    }
	    return((i4)(u_char)(*qrb->t++ = q));
	}
	return(EOF);
}
/*{
** Name:	qrunget() - "put back" a char to be re-read later
**
** Description:
**	"Put back" (logically) a single character into the go-block
**	and remove it from the token buffer.
**	It will be the next char read on the next qrgetchar().
**
** Inputs:
**	QRB	*qrb;	// active QR control block.
**	i4	c_nat;	// character to put back.
**
** Outputs:
**	QRB	*qrb;	// active QR control block (updated).
**
** Returns:
**	VOID
**
** Side Effects:
**	The QRB's input buffer, peek buffer, token buffer and
**	line count may be updated;
**
** History:
**	8/86 (peterk) - created
*/
static VOID
qrungetc(qrb, c_nat)
register QRB	*qrb;
i4		c_nat;
{
	char	c;
	/* nothing to un-get if an EOF */
	if (c_nat == EOF)
	    return;
	c = (char)c_nat;
	if (qrb->inbuf)
	    (*--qrb->inbuf) = c;
	else
	    qrb->peek = c;
	if (c == '\n')
	    qrb->lines--;
	if (qrb->t > qrb->token)
	    qrb->t--;
}
 /*{
** Name:	qrprintf - append output to output buffer
**
** Description:
**	Appends formatted output to QRB output buffer, by calling
**	STprintf.
**
** Inputs:
**	QRB	*qrb	- QueryRunner control block.
**	char	*fmt	- format string.
**	i4	a1, ... - arguments.
**
** Outputs:
**	QRB *qrb	- updated.
**
** Returns:
**	STATUS:
**	    OK
**	    FAIL	(output buffer overflow)
**
** Exceptions:
**	nothing
**
** History:
**	8-21-86 (peterk) - created.
**	22-feb-1988 (neil) - BUG 1637 - Extended size of buffer to take
**			     control char padding into account.	 Should be
**			     4*DB_GW4_MAXSTRING to allow for \123 over all data.
**      30-sep-1989 (teresal) - PC porting change: declared arguments a1, a2,
**                           etc. as PTR.
**	29-oct-92 (leighb) DeskTop Porting Change:
**		Alloc and Free buf, instead of putting so much on stack.
*/
VOID
qrprintf(qrb, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
register QRB	*qrb;
char		*fmt;
PTR             a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
{
	char *	buf;							 
	i4	len;
	buf = MEreqmem(0, 4*DB_GW4_MAXSTRING, FALSE, NULL);		 
	STprintf(buf, fmt, a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
	len = STlength(buf);
	if ( qrb->outp - qrb->outbuf >= qrb->outlen - len )
	{
		/*
		** if this comes back bad, then forget the whole thing.
		** the qrb->outbuf already has in it the message that
		** no more memory could be allocated.
		*/
		if (qroutalloc(qrb, len) != OK)
			return;
	}
	/*
	** This now is guarenteed to work, becuase qroutalloc
	** increases the buffer by a factor of two, much bigger
	** than the size of the buf.
	*/
	STcopy(buf,qrb->outp);
	qrb->outp += len;
	MEfree(buf);							 
}
/*{
** Name:	qradd - Add a string to the output
**
** Description:
**	This routine simply adds the passed in string to the qr
**	output buffers.
**
** Inputs:
**	qrb	Pointer to a QRB struct.
**	buf	String to place in qr output buffers
**	len	Length of output string.
**
** Outputs:
**	None.
**	Returns:
**		None.
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	02/10/90 (dkh) - Initial version.
*/
VOID
qradd(qrb, buf, len)
register QRB	*qrb;
char		*buf;
i4		len;
{
	i4	i;
	char	*cp;
	if ( qrb->outp - qrb->outbuf >= qrb->outlen - len )
	{
		/*
		** if this comes back bad, then forget the whole thing.
		** the qrb->outbuf already has in it the message that
		** no more memory could be allocated.
		*/
		if (qroutalloc(qrb,len) != OK)
			return;
	}
	/*
	** This now is guarenteed to work, becuase qroutalloc
	** increases the buffer by a factor of two, much bigger
	** than the size of the buf.
	*/
	if (len > 30)
	{
		qrcpbuf( buf, qrb->outp, len );
	}
	else
	{
		cp = qrb->outp;
		for (i = 0; i < len; i++)
		{
			*cp++ = *buf++;
		}
	}
	qrb->outp += len;
	*(qrb->outp) = EOS;
}
/*
** Name: qraddstr - put string in QRB output buffer
**
** Description:
**	This is simply a wrapper for qradd that assumes a null
**	terminated input string.
**
** Inputs:
**	qrb		Our usual qr control block
**			Defined as void * so that outsiders can call us
**	str		Null terminated string
**
** Outputs:
**	None
**
** History:
**	25-Jun-2004 (schka24)
**	    Written.
*/
void
qraddstr(void *qrb, char *str)
{
    qradd((QRB *) qrb, str, STlength(str));
}
/*{
** Name:	qrputc - put a character in a QRB output buffer
**
** Description:
**	A single character is moved into the output buffer of
**	the active QRB.
**
** Inputs:
**	QRB	*qrb;	// active QR control block.
**	char	c;	// the character to output.
**
** Outputs:
**	QRB	*qrb;	// active QR control block (updated).
**
** Returns:
**	STATUS:
**		OK	- output added successfully
**		FAIL	- error adding output.
**
** Side effects:
**	Data added to output buffer of the QRB.
**
** History:
**	8-21-86 (peterk) - created.
**      15-01-99 (islsa01) - Fix for the Bug #b91411.
**                           Make sure we allocate more if the boundary
**                           exceeds over 2 bytes. Extened the size of buffer
**                           to facilitate adding a byte varying data types.
**      4-29-02 (wanfr01) -   Bug 106667, INGCBT 419
**                            Improve performance by calculating length by
**                            subtracting existing start/end pointers versus
**                            STlength.
**
*/
VOID
qrputc(qrb, c)
QRB	*qrb;
char	c;
{
        i4   temp_len;
#ifdef DOUBLEBYTE 
        temp_len = STlength(qrb->outbuf);
#else
	temp_len = qrb->outp - qrb->outbuf;
#endif
	if (qrb->outp - qrb->outbuf >= qrb->outlen - temp_len)
	{
		/*
		** if this comes back bad, then forget the whole thing.
		** the qrb->outbuf already has in it the message that
		** no more memory could be allocated.
		*/
		if (qroutalloc(qrb, qrb->outlen + 8) != OK)
			return;
	}
	*qrb->outp++ = c;
	*qrb->outp = EOS;
	return;
}
/*{
** Name:	qrscanstmt	-	scan the stmt buffer
**
** Description:
**	This routine scans the stmt buffer, and will return every other
**	"token" read. The first token is the 'help' or 'print' token;
**	If help <keyword> is used, those programs will want to skip over
**	the keyword to the tablename. If the syntax is help tbl, tbl, tbl,
**	then we'll want to skip over the "," token.
**
**	The flag "comma_seperated" is TRUE if we're actually looking
**	at the comma seperated list of tables, FALSE if we're trying
**	to parse things like "help view viewname".
**
** Inputs:
**	s		pointer into somewhere within the qrb->stmt buffer
**
**	token		pointer to an i4
**
**	comma_separated TRUE if looking at a list of tables, FALSE otherwise.
**
** Outputs:
**	token		+n how many tokens were looked at
**			-1 if we're expecting to be in a comma separated
**			   list of table names, and don't hit a comma first
**			-2 if we found a non-terminated delimited identifer
**
**	Returns:
**		pointer to the next string
**
** History:
**	21-aug-87 (daver)	documented.
**	01-sep-87 (daver)	added comma_separated parameter.
**      11-jun-92 (rogerl)      if this is a comma separated list of objects
**        recognize quoting - since a quoted string might contain anything (,)
*/
char *
qrscanstmt(s, token, comma_separated)
char	*s;
i4	*token;
bool	comma_separated;
{
    bool	incomment = FALSE;
    /*
    ** first token will be "help" or "print",
    ** ",", or a help "<keyword>" like "view" or "all". We want to
    ** skip over this initial token and get to the next token, which will
    ** be our table name. We'll move the char *s over the string until
    ** we're at the beginning of the next token or until we hit EOS.
    ** if the first char is a '"', then assume it's a delimted id,
    ** in which case, we need to scan until the closing, non quoted
    ** '"', regardless of intermediates (unless we hit EOS first)
    */
    /*
    ** We want to skip to the 2nd token,
    ** eliding any white space or comments
    */
    for (*token = 0; *s;)
    {
	/* elide comment */
	if (incomment)
	{
	    if (*s++ == '*' && *s == '/')
	    {
		s++;
		incomment = FALSE;
	    }
	}
	/*
	** elide white space -- an if will do fine (rather
	** than a while) because we're in the middle of a
	** for loop
	*/
	else if (CMwhite(s))
	{
		CMnext(s);
	}
	/* detect start of comment */
	else if (*s == '/' && s[1] == '*')
	{
	    s += 2;
	    incomment = TRUE;
	}
	/* this is a token */
	else
	{
	    /* if it's the first, skip it
	    */
	    if ((*token)++ == 0)
	    {
		/*
		** note that it can be a delimited id, since we're lookin at
		** ',' separated list of objects at this point
		*/
		if ( ( comma_separated ) && ( *s == ',' ) )
		{
		    CMnext( s );   /* ditch ',',quot delim must be 1st */
		}
		else if ( comma_separated )
		{
		    /*
		    ** we're expecting the first token to be a comma.
		    ** its not; we've got a syntax error. set token
		    ** to -1, and return a pointer to what we hit
		    ** instead. the callers (either print or help)
		    ** will deal with the syntax error.
		    */
			*token = -1;
			return s;
		}
		else 
		{
		    if ( *s == '"' )
		    {
		    char *prevc = s;
			    CMnext( s );
			    while(1)
			    {
				while ( ( *s != '"') && ( *s != EOS ) )
				{
				    prevc = s;
				    CMnext( s );
				}
				if ( *s == '"' )
				{
				    prevc = s;
				    CMnext( s );
				    if ( ( *s == '"' ) && ( *prevc == '"' ) )
				    {
					CMnext( s );
					continue;
				    }
				}
				if ( ( *s == EOS ) && ( *prevc != '"' ) )
				{
				    /* need to pass a real error back ... */
				    /* this is a non-term del id */
				    /* @@fixme@@ */
				    *token = -1;
				}
				break;
			    }
		    }
		    else
		    {
			while ( !CMwhite(s) && (*s != EOS) )
			    CMnext( s );
		    }
		}
	    }
	    else	/* this should be the table name */
	    {
		break;
	    }		/* end else if token == 0*/
	}		/* end else big if stmt */
    }			/* end for loop */
    return s;
}
/*{
** Name:	qrtblowner	- find the owner of a table
**
** Description:
**	For help, we need to find out if the owner of a given table
**	is the dba or the user, or return an error if it's someone else.
**
** Inputs:
**	atable		table name in question
**
** Outputs:
**	the_owner	filled in with the owner's name
**
**	Returns:
**		OK	owner is dba or user
**		FAIL	table doesn't exist or is not owned by user
**
** History:
**	29-sep-88 (bruceb)
**		Changed into call on IIQRtot_TblOwnerType.
*/
STATUS
qrtblowner(qrb, atable, the_owner)
QRB	*qrb;
char	*atable;
char	*the_owner;
{
    char	type[9];
    char	subtype[9];
    return(IIQRtot_TblOwnerType(qrb, atable, the_owner, type, subtype));
}
/*{
** Name:	IIQRtot_TblOwnerType	- find the owner of a table
**
** Description:
**	For help, we need to find out if the owner of a given table
**	is the dba or the user, or return an error if it's someone else.
**	Also return the object type for further processing above.
**
** Inputs:
**	atable		table name in question
**
** Outputs:
**	the_owner	filled in with the owner's name
**	the_type	filled in with the object's name
**	the_stype	filled in with the object's subtype
**
**	Returns:
**		OK	owner is dba or user
**		FAIL	table doesn't exist or is not owned by user
**
** History:
**	16-jul-87 (daver) First written.
**	11-feb-88 (neil)  Checked that table name is legal.  If it has
**			  a pattern matching char then this is an 
**			  error (BUG 1962).
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
STATUS
IIQRtot_TblOwnerType(qrb, atable, the_owner, the_type, the_stype)
QRB	*qrb;
  char *atable;
char	*the_owner;
char	*the_type;
char	*the_stype;
{
  char save_owner[FE_MAXNAME + 1];
  char save_type[9];
  char save_stype[9];
  UIDBDATA *uidbdata;
    bool	exists = FALSE;
    *the_owner = EOS;
    /*
    ** BUG 1962 - Make sure table name is legal.  Because we run
    ** 	      a query, names like "emp*" may pass through.
    ** By checking the name we can restrict those (ncg)
    */
    if (FEchkname(atable) != OK)
	return FAIL;
    uidbdata = IIUIdbdata();
/* # line 1178 "qrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrutil1",7483,1240);
      IIputdomio((short *)0,1,32,0,atable);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrutil1",7483,1240);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_owner, table_type, table_subtype from iitables where tab\
le_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,atable);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"qrutil1",7483,1240);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,save_owner);
      IIgetdomio((short *)0,1,32,8,save_type);
      IIgetdomio((short *)0,1,32,8,save_stype);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 1186 "qrutil.sc" */	/* host code */
	exists = TRUE;
	qrtrimwhite( save_owner );
	STcopy(save_owner, the_owner);
	STcopy(save_type, the_type);
	STcopy(save_stype, the_stype);
	/*
	** If the owner is the DBA, stick around to see if the user
	** has a copy of the table. Otherwise, it's owned by the user,
	** or by $ingres (uidbdata->catowner).
	*/
	if (STcompare(save_owner, uidbdata->sdba) != 0)
	{
/* # line 1198 "qrutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 1199 "qrutil.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR0044_ret_err_iitables));
	return FAIL;
    }
    if (exists)
	return OK;
    else
	return FAIL;
}
/*{
** Name:	IIQRpro_ProcedureOwner	- find the owner of a procedure
**
** Description:
**	For help, we need to find out if the owner of a given procedure
**	is the dba or the user, or return an error if it's someone else.
**
** Inputs:
**	aproc		procedure name in question
**	the_owner	buffer to hold an owner's name
**
** Outputs:
**	the_owner	filled in with the owner's name
**
**	Returns:
**		OK	owner is dba or user
**		FAIL	procedure doesn't exist or is not owned by user
**
** History:
**	29-sep-88 (bruceb)
**		Functional modification of qrtblowner().
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
STATUS
IIQRpro_ProcedureOwner(qrb, aproc, the_owner)
QRB	*qrb;
  char *aproc;
char	*the_owner;
{
  char sv_owner[FE_MAXNAME + 1];
  UIDBDATA *uidbdata;
    bool	exists = FALSE;
    *the_owner = EOS;
    /*
    ** BUG 1962 - Make sure procedure name is legal.  Because we run
    ** 	      a query, names like "emp*" may pass through.
    ** By checking the name we can restrict those (ncg)
    */
    if (FEchkname(aproc) != OK)
	return FAIL;
    uidbdata = IIUIdbdata();
/* # line 1258 "qrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrutil2",7484,8125);
      IIputdomio((short *)0,1,32,0,aproc);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrutil2",7484,8125);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select procedure_owner from iiprocedures where procedure_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,aproc);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(procedure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or procedure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or procedure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"qrutil2",7484,8125);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,sv_owner);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 1266 "qrutil.sc" */	/* host code */
	exists = TRUE;
	qrtrimwhite( sv_owner );
	STcopy(sv_owner, the_owner);
	/*
	** If the owner is the DBA, stick around to see if the user
	** has a copy of the procedure. Otherwise, it's owned by the
	** user, or by $ingres (uidbdata->catowner).
	*/
	if (STcompare(sv_owner, uidbdata->sdba) != 0)
	{
/* # line 1276 "qrutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 1277 "qrutil.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR001C_err_ret_iiproc));
	return FAIL;
    }
    if (exists)
	return OK;
    else
	return FAIL;
}
/*{
** Name:	IIQRixo_IndexOwner	- find the owner of an index
**
** Description:
**	For help, we need to find out if the owner of a given index
**	is the dba or the user, or return an error if it's someone else.
**	This is separate from qrtblowner since indexes may not be
**	tables on non-INGRES dbms'.
**
** Inputs:
**	anindex		index name in question
**
** Outputs:
**	the_owner	filled in with the owner's name
**	the_type	if object found, but not an index, return name of type
**
**	Returns:
**		OK	owner is dba or user
**		FAIL	index doesn't exist or is not owned by user
**
** History:
**	29-sep-88 (bruceb)
**		Functional modification of qrtblowner().
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
STATUS
IIQRixo_IndexOwner(qrb, anindex, the_owner, the_type)
QRB	*qrb;
  char *anindex;
char	*the_owner;
char	*the_type;
{
  char svi_owner[FE_MAXNAME + 1];
  UIDBDATA *uidbdata;
    bool	exists = FALSE;
    *the_owner = EOS;
    *the_type = EOS;
    uidbdata = IIUIdbdata();
    if (IIQRisl_IndexStorageLoc() == USE_IIINDEXES)
    {
	/*
	** BUG 1962 - Make sure index name is legal.  Because we run
	** 	      a query, names like "emp*" may pass through.
	** By checking the name we can restrict those (ncg)
	*/
	if (FEchkname(anindex) != OK)
	    return FAIL;
/* # line 1342 "qrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrutil3",7485,3735);
      IIputdomio((short *)0,1,32,0,anindex);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrutil3",7485,3735);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select index_owner from iiindexes where index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,anindex);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"qrutil3",7485,3735);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,svi_owner);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 1350 "qrutil.sc" */	/* host code */
	    exists = TRUE;
	    qrtrimwhite( svi_owner );
	    STcopy(svi_owner, the_owner);
	    /*
	    ** If the owner is the DBA, stick around to see if the user
	    ** has a copy of the index. Otherwise, it's owned by the
	    ** user, or by $ingres (uidbdata->catowner).
	    */
	    if (STcompare(svi_owner, uidbdata->sdba) != 0)
	    {
/* # line 1360 "qrutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 1361 "qrutil.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	if (FEinqerr())
	{
	    qrprintf(qrb, ERget(F_QR0011_ret_err_iiindexes));
	    return FAIL;
	}
	if (exists)
	    return OK;
	else
	    return FAIL;
    }
    else	/* using IITABLES */
    {
	return (IIQRtbo_TableOwner(qrb, anindex, 'I', the_owner, the_type));
    }
}
/*{
** Name:	IIQRtbo_TableOwner	- find the owner of a specified object
**
** Description:
**	For help, we need to find out if the owner of a given object (type
**	specified) is the dba or the user, or return an error if it's someone
**	else.
**
** Inputs:
**	object		object name in question
**	otype		object type to look for
**
** Outputs:
**	the_owner	filled in with the owner's name
**	the_type	if object found, but of wrong type, return name of type
**
**	Returns:
**		OK	owner is dba or user
**		FAIL	object of correct type doesn't exist or is not owned
**			by user
**
** History:
**	29-sep-88 (bruceb)
**		Functional modification of qrtblowner().
*/
STATUS
IIQRtbo_TableOwner(qrb, object, otype, the_owner, the_type)
QRB	*qrb;
char	*object;
char	otype;
char	*the_owner;
char	*the_type;
{
    STATUS	stat;
    char	subtype[9];
    stat = IIQRtot_TblOwnerType(qrb, object, the_owner, the_type, subtype);
    if (stat != FAIL)
    {
	/*
	** Something with the specified name exists--is it the requested type?
	*/
	if (the_type[0] != otype)
	{
	    stat = FAIL;
	    if (the_type[0] == 'T')
		STcopy(ERget(F_QR0002_table), the_type);
	    else if (the_type[0] == 'V')
		STcopy(ERget(F_QR0003_view), the_type);
	    else
		STcopy(ERget(F_QR0004_index), the_type);
	}
    }
    else
    {
	the_type[0] = EOS;
    }
    return (stat);
}
/*{
** Name:	IIQRlko_LinkOwner	- find the owner of a specified
**					  registered object
**
** Description:
**	For help, we need to find out if the owner of a given registered
**	object is the dba or the user, or return an error if it's someone else.
**
** Inputs:
**	object		object name in question
**
** Outputs:
**	the_owner	filled in with the owner's name
**	the_stype	return subtype of this object--indicates if registered
**
**	Returns:
**		OK	owner is dba or user
**		FAIL	registered object doesn't exist or is not owned by user
**
** History:
**	27-oct-88 (bruceb)
**		Functional modification of qrtblowner().
*/
STATUS
IIQRlko_LinkOwner(qrb, object, the_owner, the_stype)
QRB	*qrb;
char	*object;
char	*the_owner;
char	*the_stype;
{
    STATUS	stat;
    char	type[9];
    stat = IIQRtot_TblOwnerType(qrb, object, the_owner, type, the_stype);
    if (stat != FAIL)
    {
	/*
	** Something with the specified name exists--is it registered?
	*/
	if ((the_stype[0] != 'L') && (the_stype[0] != 'I'))
	{
	    stat = FAIL;
	}
    }
    else
    {
	the_stype[0] = EOS;
    }
    return(stat);
}
/*{
** Name:	IIQRrlo_RuleOwner	- find the owner of a rule
**
** Description:
**	For help, we need to find out if the owner of a given rule
**	is the dba or the user, or return an error if it's someone else.
**
** Inputs:
**	aproc		rule name in question
**	the_owner	buffer to hold an owner's name
**
** Outputs:
**	the_owner	filled in with the owner's name
**
**	Returns:
**		OK	owner is dba or user
**		FAIL	rule doesn't exist or is not owned by user
**
** History:
**	19-may-89 (teresal)
**		Functional modification of qrtblowner().
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
STATUS
IIQRrlo_RuleOwner(qrb, arule, the_owner)
QRB	*qrb;
  char *arule;
char	*the_owner;
{
  char svr_owner[FE_MAXNAME + 1];
  UIDBDATA *uidbdata;
    bool	exists = FALSE;
    *the_owner = EOS;
    /*
    ** BUG 1962 - Make sure rule name is legal.  Because we run
    ** 	      a query, names like "emp*" may pass through.
    ** By checking the name we can restrict those (ncg)
    */
    if (FEchkname(arule) != OK)
	return FAIL;
    uidbdata = IIUIdbdata();
/* # line 1532 "qrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrutil4",7486,16117);
      IIputdomio((short *)0,1,32,0,arule);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrutil4",7486,16117);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_owner from iirules where rule_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,arule);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"qrutil4",7486,16117);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,svr_owner);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 1540 "qrutil.sc" */	/* host code */
	exists = TRUE;
	qrtrimwhite( svr_owner );
	STcopy(svr_owner, the_owner);
	/*
	** If the owner is the DBA, stick around to see if the user
	** has a copy of the rule. Otherwise, it's owned by the
	** user, or by $ingres (uidbdata->catowner).
	*/
	if (STcompare(svr_owner, uidbdata->sdba) != 0)
	{
/* # line 1550 "qrutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE4;
      }
/* # line 1551 "qrutil.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR002A_err_ret_iirul));
	return FAIL;
    }
    if (exists)
	return OK;
    else
	return FAIL;
}
/*{
** Name:	IIQRcle_ColumnExists	- find out if a given column exists.
**
** Description:
**	For help, we need to find out if the column name is valid.
**
** Inputs:
**	table		tablename
**	towner		tableowner
**	column		column name
**
** Outputs:
**	None
**
**	Returns:
**		OK	valid column
**		FAIL	column doesn't exist
**
** History:
**	05-jan-90 (teresal)
*/
STATUS
IIQRcle_ColumnExists(qrb, tname, towner, colname)
QRB	*qrb;
  char *tname;
  char *towner;
  char *colname;
{
  char rcolname[FE_MAXNAME + 1];
    bool	exists = FALSE;
    /*
    ** - Make sure column name is legal.
    */
    if (FEchkname(colname) != OK)
	return FAIL;
/* # line 1602 "qrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrutil5",7487,10388);
      IIputdomio((short *)0,1,32,0,colname);
      IIputdomio((short *)0,1,32,0,tname);
      IIputdomio((short *)0,1,32,0,towner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrutil5",7487,10388);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name from iicolumns where column_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,colname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tname);
        IIwritio(0,(short *)0,1,32,0,(char *)")and(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,towner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"qrutil5",7487,10388);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,rcolname);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 1609 "qrutil.sc" */	/* host code */
	exists = TRUE;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0035_err_ret), ERx("iicolumns"));
	return FAIL;
    }
    if (exists)
	return OK;
    else
	return FAIL;
}
/*{
** Name:	IIQRevo_EventOwner- find the owner of a event
**
** Description:
**	For help, we need to find out if the owner of a given event
**	is the dba or the user, or return an error if it's someone else.
**
** Inputs:
**	anevt		event name in question
**	the_owner	buffer to hold an owner's name
**
** Outputs:
**	the_owner	filled in with the owner's name
**
**	Returns:
**		OK	owner is dba or user
**		FAIL	event doesn't exist or is not owned by user
**
** History:
**	22-mar-1991 (kathryn) Written.
*/
STATUS
IIQRevo_EventOwner(qrb, anevt, the_owner)
QRB	*qrb;
  char *anevt;
char	*the_owner;
{
  char sv_owner[FE_MAXNAME + 1];
  UIDBDATA *uidbdata;
    bool	exists = FALSE;
    *the_owner = EOS;
    if (FEchkname(anevt) != OK)
	return FAIL;
    uidbdata = IIUIdbdata();
/* # line 1659 "qrutil.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrutil6",7488,10390);
      IIputdomio((short *)0,1,32,0,anevt);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrutil6",7488,10390);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select event_owner from iievents where event_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,anevt);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(event_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or event_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or event_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"qrutil6",7488,10390);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,sv_owner);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 1667 "qrutil.sc" */	/* host code */
	exists = TRUE;
	qrtrimwhite( sv_owner );
	STcopy(sv_owner, the_owner);
	/*
	** If the owner is the DBA, stick around to see if the user
	** has a copy of the event. Otherwise, it's owned by the
	** user, or by $ingres (uidbdata->catowner).
	*/
	if (STcompare(sv_owner, uidbdata->sdba) != 0)
	{
/* # line 1677 "qrutil.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE6;
      }
/* # line 1678 "qrutil.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0042_err_ret_iievt));
	return FAIL;
    }
    if (exists)
	return OK;
    else
	return FAIL;
}
/*{
** Name:	qroutalloc	- allocate more memory for qrb output buffer
**
** Description:
**	This routine is called when either qrprintf or qrputc runs out
**	of memory in the qrb->outbuf buffer. We allocate a new buffer
**	twice the size of the old one, copy the old buffer into the first
**	half of the new buffer and release the old buffer.
**
** Inputs:
**	qrb		The qrb.
**
** Outputs:
**	qrb		the qrb, with more memory in the outbuf.
**
**	Returns:
**		OK if all went well
**		FAIL	if it didn't
**
** History:
**	26-aug-1987 (daver)	First Written.
**	17-mar-92 (seg)
**		Can't do arithmetic or dereference PTRs.
**	8-oct-92 (rogerl)
**		outbuf is always dynamic alloc
**	21-oct-1993 (mgw) Bug #54515
**		Allocate required size + 1 to accommodate trailing EOS.
**      23-sep-1996 (sarjo01)
**              Bug 77944: give qroutalloc() a 2nd arg, newlen. Check that
**              the greater of newlen and 2 * current size is used.
*/
STATUS
qroutalloc(qrb, newlen)
QRB     *qrb;
i4      newlen;
{
	char		*space;
	register	i4	size;
	register	i4	off1;
	register	i4	off2;
	register	i4	offset;
	/*
	** allocate a new block of memory, twice as big as the
	** current block (buffers < 32k)
	*/
        size = qrb->outlen * 2;
        if (size < newlen)
            size = newlen;
	off1 = (i4) (qrb->outp - qrb->outbuf);
	off2 = (i4) (qrb->outlin - qrb->outbuf);
	/*
	** if this comes back bad, then forget the whole thing.
	** clear the qrb->outbuf, and put in the message that
	** no more memory could be allocated.
	*/
	if ( ( space = (char *)MEreqmem((u_i4)0, (SIZE_TYPE)size + 1, FALSE,
					(STATUS *)0 ) ) == NULL )
	{
	    qrb->outlin = qrb->outp = qrb->outbuf;
	    *qrb->outp = '\0';
	    STcopy(ERget(E_QR000F_Error_allocing_mem),
			qrb->outbuf);
	    return( FAIL );
	}
	/*
	** copy the data in the old buffer into the first half of
	** the new buffer, save offsets, init qrb
	*/
	offset = qrb->outp - qrb->outbuf;
	qrcpbuf( qrb->outbuf, space, offset );
	qrb->outp = space + offset;
	qrb->outlen = size;
	offset = qrb->outlin - qrb->outbuf;
	qrb->outlin = space + offset;
	MEfree( (PTR)qrb->outbuf );
	qrb->outbuf	= space;
	qrb->outlen	= size;
	qrb->outp	= space + off1;
	qrb->outlin	= space + off2;
	return( OK );
}
/*{
**  Name: IIQRscript - Write user input to the script file.
**
**  Description:
**	When in script mode echo the user input to the script file.
**	This function is called at the end of each statement, just
**	before sending the query to the DBMS.  
**	
**	This function is called whenever end-of-input "\g" is entered 
**	OR whenever the end of the current statement is detected.
**
**	We need to ensure that a new-line is printed at the end of each
**	statement so that the output (error messages/row count) will not
**	be written on the same line as the echoed input.
**
**	In SQL:  The end of statement is detected by a statement delimiter ";".
**	In QUEL: We only find out about the end of the current statement with
**		 the beginning of the next statement.
**
**	In SQL: The statement delimiter ";" is not copied to the statement
**		buffer so IF we are not at end-of-input ("\g") then the user
**		has issued a semicolon and we need to add that and a new-line
**		to the statement before printing it.  In this case, if a 
**		new-line followed the semicolon in the input, then it will be
**		printed as the first character of the next statement. 
**		- If we are at end-of-input ("\g") then the new-line character
**		following the "\g" is already in the stmt buffer and we can 
**		just print the statement as is.
**
**	In QUEL: We can only detect the end of the current statement when we
**		 are at the beginning of the next statement. If a <cr> was
**		 not entered after the stmt (multiple stmts on one line) then
**		 we must add one.  Otherwise, it is already in the stmt buffer 
**		 so we can print the statement as is, but for compatibility 
**		 with SQL output, we will generate a new-line at the beginning 
**		 of the next statement.
**
**  Inputs:
**	qrb
**
**  Ouputs:
**	Returns:
**          Void
**	Errors:
**	    none
**
**  Side Effects:
**	    none
**
**  History:
**	10-jan-1991 (kathryn)	- Written. 
**		In response to Bugs 20623 21260 35098 - Clean up script files.
**	15-may-1991 (kathryn) 
**		Set sptr to NULL rather than qrb->stmt when exiting.
**		If next qry is large - larger query buffer may be allocated
**		and query buffer pointed to by qrb->stmt may be freed.
*/
VOID
IIQRscript(qrb)
register QRB	*qrb;
{
	static char	*sptr = NULL;
	static bool	neednl = FALSE;
	if (!sptr && qrb->s)
	    sptr = qrb->stmt;
	if (qrb->lang == DB_QUEL)
	{
		if (neednl) /* user input new-line on last stmt */
		{
			SIputc(ERx('\n'),qrb->script);
			neednl = FALSE;
		}
		if (!qrb->s) 		/* input was "\g \n \g" */	
			SIputc(ERx('\n'),qrb->script);
		else if (qrb->s != sptr)
		{
		    if (!qrb->eoi && *(qrb->s -1) != '\n')
			SIfprintf(qrb->script, ERx("%s\n"),sptr);
		    else
		    {
			if (!qrb->eoi)
				neednl = TRUE;
			SIfprintf(qrb->script, ERx("%s"),sptr);
		    }
		}
	}
	else if(qrb->lang == DB_SQL)
	{
		    /* Just a semicolon was found - pass it through */
    		 if ((!qrb->s || qrb->s == sptr) && !qrb->eoi)  
    		 {
		        if (qrb->token != NULL && *qrb->token != '\0')
		    	    SIputc(*qrb->token,qrb->script);
			return;
		 }
    		 else if (qrb->s)
		 {
		     if (!qrb->eoi)
		        SIfprintf(qrb->script, ERx("%s;\n"),sptr);
		     else
		    	SIfprintf(qrb->script, ERx("%s"),sptr);
		 }
	}
	/* Just comments were issued  "\g comments \g" */
	if (qrb->s && !qrb->eoi && !qrb->nonnull)
		sptr = qrb->s;
	else
		sptr = NULL;
}
/*{
** Name: q_cpbuf - copy 'size' bytes of buffer1 into buffer2, not restricted
**		 by size
**
** Description:
**	Cover for MEcopy overcoming the u_i2 max-copy-size limit
**	
** Inputs/Outputs: 
**	char	*olde	- old buffer
**	char	*new	- new buffer
**	i4	size	- number of bytes to copy
**
** History:
**	22-dec-92 (rogerl) written
*/
VOID
qrcpbuf( olde, newb, size )
char	* olde;
char	* newb;
i4		size;
{
u_i2 maxui2 = (u_i2) -1;
	/* yields max signed i4
	** REPLACE WITH 'MAXUI2' DEFINE when available
	*/
	while( ( size / (i4)maxui2 ) != 0 )
	{
		MEcopy( (PTR)olde, maxui2, (PTR)newb );
		olde += maxui2;
		newb += maxui2;
		size -= maxui2;
	}
	if (size > 0)
	    MEcopy( (PTR)olde, (u_i2)size, (PTR)newb );
	return;
}
/*{
** Name: IIQR_relexists	- Terminal Monitor test for existence of a 
**				  relation.
**
** Description:
**	If query language is SQL and there is any capablility for quel 
**	(quelLevel), then change language to Quel before invoking FErelexists.
**	Ferelexists uses QUEL unless there is NO capability for it. 
**	
** Inputs:
**	lang	 	- DB_QUEL or DB_SQL  - Users query language.
**	rel_name  	- Name of relation.
**
** Outputs:
**	none.
**
** Side Effects:
**	none.
**
** History:
**	03/22/89 (kathryn)	written.
**	10/19/92 (rogerl)	new parm for FErelexists()
**	10/20/92 (rogerl)	IIbreak() always - while running quel monitor,
**				problems doing SQL help queries without it
**	15/10/93 (rogerl)	Case the hardwired rel_name; FIPS databases
**				will require upper case catalog names in help
**				queries.(55825)
**	28/10/93 (rogerl)	Hardwired rel_name is RO - Make a writable
**				copy, then case.
**      01/11/93 (huffman)      submitted for RogerL.
**	10/28/96 (nick)		do IIbreak() if not in MST.
*/
STATUS
IIQR_relexists(lang,rel_name)
i4	lang;		/* QUEL/SQL indicator */
char	*rel_name;	/* relation checking for */
{
	STATUS	status;
	char	cased_rel_name[ FE_MAXNAME ];
	if (!FEinMST())
	    IIbreak();
	STcopy( rel_name, cased_rel_name );
	IIUGdbo_dlmBEobject( cased_rel_name,
			FALSE /* not delimited - INGRES cat */ );
	if ((lang == DB_SQL) && (IIUIdcq_quelLevel() != UI_DML_NOLEVEL))
	{
		IItm_dml(DB_QUEL);
		status = FErelexists( cased_rel_name, NULL );
		IItm_dml(DB_SQL);
	}
	else
	{
		status = FErelexists( cased_rel_name, NULL );
	}
	return (status);
}
/*{
** Name: qrrelx
**
** Description:
**	Check if relation exists, print message and end trans if necessary
**	
** Inputs: 
**	QRB	* qrb
**	char 	* relation	- relation name
**	ER_MSGID	id	- id of err message to print, or 0
**
** Outputs:
**	TRUE	- relation exists
**	FALSE	- relation doesn't exist
**
** Side Effects:
**	FALSE return - print err message defined by 'id'; commit trans and
**			init qrb->endtrans to FALSE if in MST
**
** History:
**	1-jul-92 (rogerl) written
**	15-jan-92 (rogerl) spurious commits occurred; need to check
**		qrb->endtrans instead of FEinMST() for commit or not
*/
static bool
qrrelx( QRB *qrb, char *relation, ER_MSGID id )
{
    if ( IIQR_relexists( qrb->lang, relation ) != OK )
    {
		/* 0 messid means don't print anything */
	if ( id > 0 )
	    qrprintf( qrb, ERget( id ) );
	if ( qrb->endtrans )
	{
/* # line 2006 "qrutil.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 2007 "qrutil.sc" */	/* host code */
	    qrb->endtrans = FALSE;
	}
	return( FALSE );
    }
    else
    {
	return( TRUE );
    }
}
/*{
** Name: qrrelok
**
** Description:
**	Check if relation exists, print message and end trans if necessary
**	
** Inputs: 
**	QRB	* qrb
**	char 	* relation	- relation name
**	ER_MSGID	id	- id of err message to print, or 0
**	bool	*chkd		- already check or not (false - not checked)
**	bool	*truth		- return value of existance (true - exists)
**
** Side Effects:
**	initialize passed in chkd and truth
**
** History:
**	1-jul-92 (rogerl) written
**	21-oct-92 (rogerl) always print error message whether the relation
**			   exists or not
*/
VOID
qrrelok( QRB *qrb, char *relation, ER_MSGID errid, bool *chkd, bool *truth )
{
    if ( ! *chkd )
    {
	*truth = qrrelx( qrb, relation, errid );
	*chkd = TRUE;
    }
    else if ( ( errid > 0 ) && ( ! *truth ) )
    {
	qrprintf( qrb, ERget( errid ) );
    }
    return;
}
/*{
** Name: qrtrimwhite - STtrimwhite, preserve single space if all spaces
**
** Description:
**	Cover for STtrimwhite, except where the object being trimmed is
**	all white space.  In this case, a single space is preserved.
**	
** Inputs: 
**	char 	*str -	string to trim
**
** History:
**	1-jul-92 (rogerl) written
*/
VOID
qrtrimwhite( char *str )
{
    if ( str != NULL )
    {
	STtrmwhite( str );
	if ( *str == EOS )
	{	
	    CMcpychar( ERx(" "), str );
	    *CMnext( str ) = EOS;
	}
    }
    return;
}
/*{
** Name: qrdiprintf - delimited id qrprintf
**
** Description:
**	Check the arguments indicated by the flag word, and requote them
**	if necessary.  Arg positions are specified starting at 'arg 1'
**	which is just after the format string.  The (possibly requoted)
**	strings are then formatted into the QRB.  This now only is useful
**	for the first four arguments, but is easily expandable to more if
**	necessary.
**	
** Inputs: 
**	u_i4	nth		- flag word sp3
**	QRB	*qrb		- 
**	c	*fmt		- 'qrprintf' style format string
**	c	*id1-id10	- args corresponding to format string
**
** Side Effect:
**	Puts (possibly requoted) formatted string into QRB output buffer.
**
** History:
**	10-dec-92 (rogerl) written
*/
VOID
qrdiprintf(
    u_i4	nth, QRB	*qrb,
    char	*fmt, char	*id1, char	*id2,
    char	*id3, char	*id4, char	*id5,
    char	*id6, char	*id7, char	*id8,
    char	*id9, char	*id10
) {
    char	buf1[ FE_UNRML_MAXNAME + 1 ];
    char	buf2[ FE_UNRML_MAXNAME + 1 ];
    char	buf3[ FE_UNRML_MAXNAME + 1 ];
    char	buf4[ FE_UNRML_MAXNAME + 1 ];
    if ( nth & QRARG1 )
	if ( qrdorequote( id1, buf1 ) )
	    id1 = buf1;
    if ( nth & QRARG2 )
	if ( qrdorequote( id2, buf2 ) )
	    id2 = buf2;
    if ( nth & QRARG3 )
	if ( qrdorequote( id3, buf3 ) )
	    id3 = buf3;
    if ( nth & QRARG4 )
	if ( qrdorequote( id4, buf4 ) )
	    id4 = buf4;
		/* print to qrb as per usual
		*/
    qrprintf( qrb, fmt, id1, id2, id3, id4, id5, id6, id7, id8, id9, id10 );
    return;
}
/*{
** Name: qrdorequote - requote 'id' into 'buf' unless it's  FE_CAT_ID ($ingres)
**
** Description:
**	Assumes the id's to be checked were taken from the standard catalogs,
**	so doesn't check for empty strings and the like; will requote 
**	'illegal' identifiers
**	
** Inputs/Outputs: 
**	char	*id	- id to requote
**	char	*buf	- buffer for requoted id
**
** History:
**	1-jul-92 (rogerl) written
*/
bool
qrdorequote(
    char *id,
    char *buf
) {
    if ( IIUGxri_id ( id, buf ) == FALSE )
	if ( STbcompare( UI_FE_CAT_ID_65, 0, id, 0, 1) == 0 )
	    return( FALSE );
				/* it's system cat owner ('$ingres'?), don't
				** quote it; ret FALSE so caller uses the
				** original version
				*/
	else
	    IIUGrqd_Requote_dlm( id, buf );
				/* it's an illegal identifier, but we want
				** to quote it, and return TRUE so it's used
				*/
    return( TRUE );
		/* use requoted string now in output buffer */
}
