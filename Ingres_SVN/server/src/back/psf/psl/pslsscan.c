/* Copyright (c) 1986, 2005 Ingres Corporation
**
**
*/

#include    <compat.h>
#ifdef i64_aix
#define MAX 403
#define MIN 405
#endif
#include    <gl.h>
#include    <cs.h>
#include    <st.h>
#include    <cm.h>
#include    <er.h>
#include    <cv.h>
#include    <me.h>
#include    <qu.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <adf.h>
#include    <adfops.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <rdf.h>
#include    <psfparse.h>
#include    <psfindep.h>
#include    <pshparse.h>
#include    "pslsgram.h"
#include    "pslscan.h"
#include    <yacc.h>
#include    <cui.h>
#include    <adudate.h>

/**
**
**  Name: PSLSSCAN.C - The scanner for the server's SQL parser.
**
**  Description:
**      This file contains the scanner (lexical analyser) for the SQL scanner
**	in the server.  It also contains all related tables.
**
**          psl_sscan - The SQL scanner.
**
**
**  History:
**	28-jan-87 (daved)
**	    Adapted from the Jupiter QUEL scanner.
**	27-apr-88 (stec)
**	    Added keywords for database procedures.
**	12-aug-88 (stec)
**	    Accept curly brackets in a DB procedure with a select stmt.
**	31-aug-88 (andre)
**	    Recognize FORREADONLY as FROM clause terminator.
**	02-sep-88 (andre)
**	    Placed j_freesz? before all j[a-z]* and lock_trace before lockmode
**	    since '_' preceeds alphabetical chars in both EBCDIC and ASCII
**	03-oct-88 (stec)
**	    Complete Kanji related changes.
**	18-oct-88 (stec)
**	    Fix recognition of a Kanji name too long bug.
**	22-nov-88 (stec)
**	    Fix set noio_trace command.
**	05-mar-89 (ralph)
**	    GRANT Enhancements, Phase 1:
**	    Added keywords ADD, APPLICATION, APPLICATION_ID
**	09-mar-89 (neil)
**	    Rules, phase 1:
**	    Added RULE, AFTER, BEFORE, REPEAT, REFERENCING,
**		  RAISE, "RAISE ERROR".
**	03-apr-89 (ralph)
**	    GRANT Enhancements, Phase 2:
**	    Added double keywords "ON APPLICATION", "ON DATABASE", and
**		"ON LOCATION".
**	    Removed reserved keyword APPLICATION.
**	05-may-89 (ralph)
**	    GRANT Enhancements, Phase 2b:
**		Removed reserved words APLID and RULE.
**		Added double reserved words: ALTGROUP, ALTROLE, ALTUSER,
**		    CRTGROUP, CRTROLE, CRTRULE, CRTUSER, DROPGROUP,
**		    DROPROLE, DROPRULE, DROPUSER, SETPRTRULES,
**		    FROMGROUP, FROMROLE, FROMUSER, TOGROUP, TOROLE, TOUSER
**	16-jun-89 (andre)
**	    get rid of code dealing with [un]stacking of the elements of the
**	    target list + define new keywords: FULL, INNER, JOIN, LEFT, NATURAL,
**	    RIGHT.
**	24-jun-89 (ralph)
**	    GRANT Enhancements, Phase 2e:
**	    Added double reserved words: SETMXIO, SETMXROW, SETMXCPU,
**	    SETMXPAGE, and SETMXCOST.
**	27-jul-89 (jrb)
**	    Changed scanning of numeric literals to return decimal for a certain
**	    class of numbers.  Also, allowed for backward compatibility with old
**	    literal semantics by checking pss_num_literals flag in session cb.
**	08-sep-89 (ralph)
**	    Fixed bug 7811 -- Fromwords had wrong size
**	08-sep-89 (ralph)
**	    Add new keywords for B1:
**		CALLPROC, WHEN.
**	    Add new double keywords for B1:
**		ALTLOC, CRTLOC, CRTSECALM, DROPLOC, DROPSECALM, 
**		ENSECAUDIT, DISECAUDIT, ALLDBS, BYUSER.
**	    Added non-keywords for double keywords:
**		DISABLE, ENABLE.
**	    Changed RAISE to be non-keyword (but RAISE ERROR is still dbl kwd).
**	    Removed `SORT' as keyword (should not have been reserved).
**	08-sep-89 (ralph)
**	    Fixed bug 7811 -- Fromwords had wrong size
**	20-sep-89 (neil)
**	    Unreserved RAISE, BEFORE & AFTER.
**	29-sep-89 (neil)
**	    Added tokens SET [NO]RULES.
**	08-oct-89 (neil)
**	    Alerters: Added new double reserved words:
**		CREATEEVENT, DROPEVENT, REGISTEREVENT,  REMOVEEVENT,
**		RAISEEVENT, ONEVENT, SETPRTEVENTS, SETLOGEVENTS, SETRULES
**	    Until Star REGISTER and REMOVE are not reserved alone.
**	04-dec-89 (andre)
**	    $dba and $ingres are now keywords.
**	20-feb-90 (andre)
**	    Recognize COMMENT ON as a double keyword; COMMENT by itself will not
**	    be treated as one.
**	1-mar-90 (andre)
**	    Tentatively added support for CREATE/DROP SYNONYM.
**	9-mar-90 (andre)
**	    treat SET [NO]OPTIMIZEONLY as a double keyword +
**	    INNER/OUTER/LEFT/RIGHT will not be tretaed as keywords when
**	    appearing by themselves.
**	09-apr-90 (sandyh)
**	    Added SET LOG_TRACE.
**	16-apr-90 (ralph)
**	    Removed ONLOCATION and ONAPPLICATION double reserved words to
**	    fix bug 21090.  These are not needed for Release 6.3.
**	18-apr-90 (linda)
**	    Defined double tokens:  REGTABLE, REGINDEX, REGUNIQUE.  These are
**	    used in REGISTER TABLE and REGISTER [UNIQUE] INDEX commands for
**	    non-SQL Gateways.
**	29-may-90 (linda)
**	    Try, try again.  Remove "register" token from Key_info array,
**	    also remove corresponding token from pslsgram.yi.  Now "register"
**	    by itself is truly not a keyword.
**	08-aug-90 (ralph)
**	    Remove `ALL DATABASES' and `ON APPLICATION' double reserved words;
**	    Add `ON INSTALLATION' double reserved word.
**      12-sep-90 (teresa)
**          moved some booleans into bitflags: pss_txtemit
**	2-nov-90 (andre)
**	    fix bug 33417.  If toksize > 0 AND the current symbol block doesn't
**	    have enough space for the piece pointer, we need to copy the data
**	    being returned into the new block.  yacc_cb->yylval contains an
**	    address of the data being returned, so we must copy toksize bytes
**	    starting with the address found in yacc_cb->yylval, and not starting
**	    with address of yacc_cb->yylval as was done until now.
**	5-nov-90 (andre)
**	    replaced pss_flag with pss_stmt_flags
**	29-apr-91 (andre)
**	    Fix bug 35924:
**	     - "ON INSTALLATION" will no longer be treated as a double keyword;
**	     - ONEVENT, ONDATABASE, and ONLOCATION will be recognized if the
**	       token following ON was EVENT, DATABASE, or LOCATION,
**	       respectively, and one of the following is true
**		- (psq_cb->psq_mode == PSQ_GRANT) &&
**		  (the token following EVENT|DATABASE|LOCATION is nether COMMA
**		   nor TO) 
**		  or
**		- (psq_cb->psq_mode == PSQ_REVOKE) &&
**		  (the token following EVENT|DATABASE|LOCATION is nether COMMA
**		   nor FROM);
**	       otherwise, ON will be returned, since then we will assume that
**	       EVENT, DATABASE, or LOCATION are table names.
**	17-may-91 (andre)
**	    Define double keywords SETROLE, SET_NOROLE, SETGROUP, SET_NOGROUP,
**	    SETUSER.
**	12-jun-91 (andre)
**	    define new keyword: CASCADE.
**	12-jun-91 (andre)
**	    REGISTER and RAISE will be treated as keywords if found as a single
**	    keyword when (sess_cb->pss_stmt_flags & PSS_PARSING_PRIVS).  This
**	    will enable us to distinguish database privileges (which are just
**	    NAMEs) from event privileges which will be returned as keywords.
**	13-jun-91 (andre)
**	    ONCURRENT will be treated as a double keyword
**	12-nov-91 (rblumer)
**	  merged from 6.4:   4-feb-91 (rogerk)
**	    Added SET [NO]LOGGING as a double keyword.
**	  merged from 6.4:  25-feb-91 (rogerk)
**	    Added SET SESSION as a double keyword.
**	  merged from 6.4:  16-jul-91 (ralph)
**	   Change EVENT to DBEVENT.  This affects the following double keywords:
**		CREATEEVENT, DROPEVENT, REGISTEREVENT, REMOVEEVENT, RAISEEVENT,
**		SETLOGEVENTS, SETPRTEVENTS, ONEVENT
**	  merged from 6.4:  22-jul-91 (rog)
**	    Specify correct number of arguments for errors 2700 and 2705. (9877)
**	14-jan-92 (barbara)
**	    Included ddb.h for Star.
**	17-jan-1992 (bryanp)
**	    Add temporary table keywords: SESSION, TEMPORARY, MODULE, GLOBAL,
**	    LOCAL, PRESERVE, and ROWS.
**	21-feb-92 (andre)
**	    define new keyword: RESTRICT
**	08-jun-92 (barbara)
**	    Sybil merge.  Merged Star comments:
**	    27-sep-88 (andre)
**		Added LINK to the list of Createwords and Dropwords.
**	    11-oct-88 (andre)
**		Put in code to allow for storing of text of constants.
**	    23-feb-89 (andre)
**		Added INDEX to the list of REGWORDS, RMVWORDS.
**	    28-feb-89 (andre)
**              Change length of Key_index to be that of the longest
**		SQL keyword + 1 (PSS_1SQL_LONGEST_KWRD + 1).  Change the
**		code that recognizes keywords accordingly.
**	    15-aug-91 (kirke)
**		Removed MEcopy() declaration, added include of me.h.
**	    End of merged comments.
**	    REGISTER and REMOVE are keywords in Star.  Removed double tokens
**	    REGTABLE, REGINDEX, REGUNIQUE.  However, left REGISTEREVENT and 
**	    REMOVEEVENT as keywords because productions for REGISTER DBEVENT
**	    and REGISTER <dbevent> AS LINK ... are easier to parse under
**	    completely separate productions.  As in the case of ONEVENT,
**	    ONDATABASE, etc. (see comment above), the scanner will look
**	    ahead after REGISTER DBEVENT to see whether to return the
**	    double keyword REGISTEREVENT or REGISTER NAME (where NAME=dbevent).
**	    Same deal with REMOVE DBEVENT.
**	19-jul-92 (andre)
**	    defined new keyword - EXCLUDING
**	03-aug-92 (barbara)
**	    Added double keyword for Star, CREATE LINK.
**	29-sep-1992 (fred)
**	    Added support for bit constants (b'<bit>{<bit>...}').
**	10-oct-92 (barbara)
**	    Added double keywords for Star, DROP LINK, SET DDLCONCURRENCY
**      8-nov-1992 (ed)
**          add elements for DB_MAXNAME to key_info structure
**	9-nov-1992 (rblumer)
**	    Added support for FIPS constraints;
**	    new keywords KEY, DEFAULT, FOREIGN, PRIMARY,
**	        CONSTRAINT, REFERENCES, CURRENT_USER
**	03-nov-92 (jrb)
**	    Added "SET WORK" as double reserved word.
**	13-oct-92 (ralph)
**	    Add support for delimited identifiers
**	01-dec-92 (andre)
**	    undefined double keywords SETROLE, SET_NOROLE, SETGROUP,
**	    SET_NOGROUP, and SETUSER.  We are shelving support for SET
**	    GROUP/ROLE and replacing SET USER with SET SESSION
**
**	    defined new keywords: SESSION_USER, SYSTEM_USER, INITIAL_USER
**	24-nov-92 (ralph)
**	    CREATE SCHEMA:
**	    Added the "SCHEMA" keyword.
**	17-dec-92 (ralph)
**	    Fixed compiler warnings introduced by delimited identifier
**	    integration
**	04-jan-93 (rblumer)
**	    Added support for TXTEMIT2 and 2nd text chain; removed unused
**	    variables and added cv.h to get rid of compiler warnings.
**	12-feb-93 (ralph)
**	    DELIM_IDENT:
**	    Translate regular identifiers according to database semantics
**	06-apr-93 (ralph)
**	    DELIM_IDENT:
**          Change reference to pss_cb->pss_dbxlate to be ptr to u_i4
**      06-may-93 (anitap)
**          Added support for "SET UPDATE_ROWCOUNT statement. New keyword
**          CHANGED.
**          Fixed compiler warnings in psl_sscan().
**	06-jul-93 (anitap)
**	    Undid the above change for "SET UPDATE_ROWCOUNT" statement.
**      14-jul-93 (ed)
**          replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>.
**	29-jul-93 (robf)
**	    Add support for ALTER SECURITY_AUDIT
**	15-sep-93 (swm)
**	    Added cs.h include above other header files which need its
**	    definition of CS_SID.
**	22-oct-93 (robf)
**          Add COPY_FROM/COPY_INTO
**	    Add SETMXIDLE, SETMXCONNECT
**	16-nov-93 (robf)
**          Add SET [NO]ROWLABEL_VISIBLE
**	24-nov-93 (robf)
**          Add BYROLE as double keyword for security alarm enhancments
**	25-nov-93 (robf)
**          Correct typo, nomaxconnect not nomaxconnext
**	22-feb-94 (robf)
**          Added SETROLE as double keyword.
**      21-mar-95 (ramra01)
**          Interpretation of II_DECIMAL during create table with decimal
**          Bug 67519.
**      04-apr-1995 (dilma04)
**          Added new SET TRANSACTION keywords: COMMITTED, ISOLATION, LEVEL,
**          READ, REPEATABLE, SERIALIZABLE, UNCOMMITTED. 
**	03-nov-95 (pchang)
**	    Change to use cui_f_idxlate() which is faster than cui_idxlate(), 
**	    when translating regular identifiers.
**      07-mar-96 (dilma04)
**          Back out keywords related to <set transaction statement> which are
**          not reserved words under OpenINGRES 1.2/00.
**	19-mar-96 (pchang)
**	    Fixed bug 70204.  Incorrect test on the next symbol location for
**	    byte alignment prevented a new symbol block to be allocated when
**	    there are exactly 2 bytes left on the current symbol table, and
**	    subsequent alignment operation on the next symbol pointer caused
**	    it to advance beyond the current symbol block and thus, corrupting
**	    adjacent object when the next symbol was assigned.  SEGVIO followed
**	    when the trashed object was later referenced.
**	26-mar-96 (abowler)
**	    Bugs 69866 & 75493: If II_DECIMAL is ',', handle case of
**	    'select col,0' by checking to see if last token was a NAME.
**	11-jun-1996 (angusm)
**		Extend fix for 67519 to include DECLARE GLOBAL TEMPORARY
**		TABLE (bug 76902)
**      22-jul-1996 (ramra01 for bryanp)
**          Add new reserved word COLUMN for ALTER TABLE support.
**      22-nov-1996 (dilma04)
**         Re-added SET TRANSACTION keywords: COMMITTED, ISOLATION, LEVEL,
**         READ, REPEATABLE, SERIALIZABLE, UNCOMMITTED which had to be 
**         backed out because of the integration mistake.  
**	27-Feb-1997 (jenjo02)
**	    Added new keywords for SET TRANSACTION | SESSION  <access mode> 
**	    (READ ONLY | READ WRITE), ONLY and WRITE.
**	12-may-1997 (hayke02)
**	    Modified fix for 67519 and 76902 to swtich off the PSS_CREATE_STMT
**	    and PSS_CREATE_DGTT flags if we encounter a SELECT token. This
**	    prevents syntax errors on 'create table ... as select ...' or
**	    'declare global ... as select ...' statements when the where
**	    clause contains decimal/float  constants and II_DECIMAL is set to
**	    ','. This change fixes bug 81706.
**	6-june-97 (inkdo01)
**	    Added OUTER as reserved word (for bug 81291).
**	21-jul-97 (inkdo01)
**	    Added flatten/noflatten as new SET words (as more friendly 
**	    alternative to set [no]trace point op132)
**	26-sept-97 (inkdo01)
**	    Added LEFT OUTER, RIGHT OUTER and FULL OUTER as alternative 
**	    outer join tokens.
**	24-Nov-1997 (shero03)
**	    Bug 87127 - decimal constant over 15 digits is truncated to 15.
**	15-oct-98 (inkdo01)
**	    Made "system_maintained" into reserved word (believe it or not) 
**	    to permit disambiguating grammar for 2.5 constraint "with" clause.
**  07-Dec-1998 (rigka01)
**      bug 94215 - long insert statement causes ULM memory corruption and
**      later  ACCVIO due to byte alignment check was passed but bytes left
**      was 0 and a new symbol block did not get allocated.
**	19-Jan-1999 (shero03)
**	    Add set random_seed
**      21-jan-1999 (hanch04)
**          replace nat and longnat with i4
**	25-feb-99 (inkdo01)
**	    Restored "row" to "row" after properly fixing "row" references in 
**	    grammar.
**	16-aug-99 (inkdo01)
**	    Add "||" as ANSI-compliant synonym for string concatenation.
**	2-mar-00 (inkdo01)
**	    Add "first" for "select first n ..." syntax.
**	21-sep-00 (inkdo01)
**	    Changed "firsk" to "first".
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      18-sep-2000 (hanch04)
**          Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**	17-nov-2000 (abbjo03) bug 102580
**	    Change scanning of numerical values so that a numeric floating
**	    point or decimal value can be entered when II_DECIMAL is a comma.
**	29-jul-2001 (toumi01)
**	    Early definition of MIN/MAX for i64_aix to avoid conflict with
**	    system header macros with the same names.
**	28-sep-2001 (toumi01)
**	    Add RAWPCT as keyword to avoid syntactic ambiguity between
**	    loc_rawpct_tok and loc_area_tok in the loc_with rule.  This
**	    fixes bug 105929.
**	6-mar-02 (inkdo01)
**	    Add bunches of stuff for Oracle/ANSI sequences.
**	06-jun-2002 (toumi01)
**	    Return FIRST as a (potential) keyword or an identifier depending
**	    upon the following token and the syntactic context. This is
**	    needed so that we can parse SELECT FIRST FROM T1 where FIRST is
**	    a column identifier.
**	21-oct-02 (inkdo01)
**	    Removed "result" as standalone reserved word and replaced with 
**	    "result row" 2-word reserved word.
**	31-oct-02 (inkdo01)
**	    Added "cross join" doubleword reserved word.
**	10-Jan-2003 (hanch04)
**	    Fix missing cross integration.
**	4-mar-03 (inkdo01)
**	    Reordered Nowords (they must be sorted, dummy!).
**      01-May-2003 (hanch04)
**          Added PSS_DBPROC_LHSVAR for bug 110041.  If II_DECIMAL=','
**          then variable assignments should be allow to have ','
**          instead of '.' in CREATE PROCEDURE statements.
**	4-june-03 (inkdo01)
**	    Added SETOJFLATTEN for "set [no]ojflatten" statement and 
**	    SETHASH for "set [no]hash".
**	2-dec-03 (inkdo01)
**	    Removed "no" as standalone resword (only used in double words).
**	17-dec-03 (inkdo01)
**	    Added cast, nullif, coalesce for ANSI functions update.
**      02-jan-2004 (stial01)
**          Changes for SET [NO]BLOBJOURNALING, SET [NO]BLOBLOGGING
**	23-Jan-2004 (schka24)
**	    Remove KEY as a keyword.  Define double keywords for PRIMARY
**	    KEY and FOREIGN KEY.  Implement alternate keyword sets for
**	    easier PARTITION= clause parsing.
**	    Detect and properly return i8 constants.
**      19-feb-2004 (gupsh01)
**          Added support for set [no]unicode_substitution.
**	23-feb-2004 (schka24)
**	    Alternate keyword list is creeping up -- add TO.
**	4-mar-04 (inkdo01)
**	    Added SETPARALLEL to enable "set [no]parallel [n]".
**	4-mar-04 (inkdo01)
**	    Enabled CAST, NULLIF and COALESCE (added earlier) for new funcs.
**      15-Apr-2004 (hanal04) Bug 112111 INGSRV2782
**          Resolve spurious FROM clause errors when undelimited table
**          names include specials characters.
**      13-may-2004 (stial01)
**          Removed support for SET [NO]BLOBJOURNALING ON table.
**      27-aug-2004 (stial01)
**          Removed SET NOBLOBLOGGING statement
**	2-dec-04 (inkdo01)
**	    Added "collate".
**	5-jan-05 (inkdo01)
**	    Added "symmetric" and "asymmetric" for BETWEEN predicates.
**	31-aug-05 (inkdo01)
**	    Added "top" as synonym for "first".
**	19-sep-05 (inkdo01)
**	    Added "anti" joins & "intersect" joins.
**	07-Nov-05 (toumi01) SIR 115505
**	    Support hex constants in format 0x696E67636F7270 as well as the
**	    traditional X'6279656279654341' to aid application portability.
**	29-nov-05 (inkdo01)
**	    Remove Fromwords, Bywords compound reserved words.
**	16-jan-06 (dougi)
**	    Added "create/drop trigger" as synonyms for "create/drop rule".
**	14-mar-06 (dougi)
**	    Added "cube", "rollup" and "grouping sets" for aggregate 
**	    enhancements.
**	24-apr-06 (dougi)
**	    Added "with time", "with local" and "without time" double words
**	    for date/time enhancements.
**	24-May-2006 (kschendel)
**	    Add double-keywording for DESCRIBE to support DESCRIBE INPUT;
**	    remove a dozen ancient and obsolete SET keywords (J_FREESZ, etc).
**	7-june-06 (dougi)
**	    Add OUT, INOUT for parameter declarations.
**	23-june-06 (dougi)
**	    Set pss_stmtno to the number of semi-colons.
**	30-aug-2006 (gupsh01)
**	    Added support for ANSI date/time literals and ANSI constant 
**	    functions.
**	12-jan-2007 (dougi)
**	    Removed interval literals to parser (too complex to handle here)
**	    and added INTERVAL as reserverd word.
**	17-jan-2007 (dougi)
**	    Added SCROLL.
**	9-may-2007 (dougi)
**	    Added FREE LOCATOR.
**	18-july-2007 (dougi)
**	    Added OFFSET & tidied date/time constants.
**	21-dec-2007 (dougi)
**	    Added cache_dynamic to set options.
**	3-march-2008 (dougi)
**	    Added REPERATED as synonym for REPEAT (as in ESQL).
**/

/*
**  Defines of other constants.
*/

/*
**      KEY_INFO_SIZE is the number of entries in the Key_info table.
*/
#define                 KEY_INFO_SZ (sizeof(Key_info)/sizeof(KEYINFO)

/* Return values from multi-word-keyword */
#define MULTI_RETTOK	1		/* Just fall through */
#define MULTI_RETNAME	2		/* Goto retname (not a keyword) */
#define MULTI_TOKRETURN	3		/* Dbl keyword, goto tokreturn */


/*}
** Name: SECONDARY - Table of second words for a keyword token.
**
** Description:
**      This structure is used to contain the set of second words for a
**      two-word keyword.  It is assumed that each such table is in
**	alphabetical order, to make searching faster.  The structure contains
**	a pointer to a second word, the token to return for that word, and
**	the value for the token.
**
** History:
**     28-jan-86 (jeff)
**          written
*/
typedef struct _SECONDARY
{
    char            *word2;		/* A second word */
    i4              token2;		/* The corresponding token */
    i4              val2;           /* The corresponding value */
}   SECONDARY;

/*}
** Name: KEYINFO - Information about keywords.
**
** Description:
**      This structure is used to define keywords in the QUEL grammar.  For
**      each keyword, it gives the token to return, the value associated with
**	the token (if any), and a pointer to another keyword table for those
**	cases of compund keywords (keywords composed of more than one word).
**
** History:
**     28-jan-86 (jeff)
**          Modified from old Key_info struct.
*/
typedef struct _KEYINFO
{
    i4              token;              /* The token to be returned */
    i4		    tokval;             /* Value of the token */
    i4              num2ary;            /* Number of possible 2nd words */
    SECONDARY       *secondary;         /* Table of second words */
}   KEYINFO;

/*
** Definition of all global variables owned by this file.
*/

/*
** This structure contains all of the keywords.  It doesn't contain pointers
** to the keyword strings; rather, the strings are embedded directly in this
** structure.  The keywords are arranged by length order: all of the two-letter
** keywords are together, as are the three-letter keywords, etc.  Within each
** group they are in alphabetical order.  Each keyword is preceded by an index
** into the Key_info[] array, and is followed by a zero.  Each set of
** keywords is followed by a zero.
**
** When searching for a keyword, first find the length n of the word, and get
** the n'th entry in Key_index[].  This will be a pointer to the set of
** keywords of length n in Key_string[].  Then you can look for the word,
** searching until you find word or the end of the set of words.  If you find
** the word, use the number immediately preceding it as an index into
** Key_info[].  The entry there will give the token and its corresponding
** value.
**
** Some tokens are composed of two words.  In cases like this, Key_info[]
** will contain a pointer to a list of possible second words, along with the
** number of possible second words.  The list of second words will be in
** alphabetical order.  You can do a binary search to look up the second word.
** If you find it, the list of second words will contain the token and its
** corrseponding value.  If you don't find the second word, then the token and
** its value will be in Key_info[], which you have already looked up.  If
** the token there is zero, this means that the first word should be treated
** as a name if the second word is not found.
*/
 
static const u_char     Key_string[] = {
                  /* Keywords of length 0, and other non-existent lengths */
/* 0 */           0,
                  /* Keywords of length 2 */
/* 1 */           1, 'a', 's', 0,
/* 5 */           2, 'a', 't', 0,
                  3, 'b', 'y', 0,
                 26, 'd', 'o', 0,
                 27, 'i', 'f', 0,
                  4, 'i', 'n', 0,
                  5, 'i', 's', 0,
		167, 'n', 'o', 0,
                  6, 'o', 'f', 0,
                  7, 'o', 'n', 0,
                  8, 'o', 'r', 0,
                  9, 't', 'o', 0,
/* 49 */          0,
                  /* Keywords of length 3 */
/* 50 */        102, 'a', 'd', 'd', 0,
                 10, 'a', 'l', 'l', 0,
                 11, 'a', 'n', 'd', 0,
                 12, 'a', 'n', 'y', 0,
                 63, 'a', 's', 'c', 0,
                 13, 'a', 'v', 'g', 0,
                 57, 'e', 'n', 'd', 0,
                 61, 'f', 'o', 'r', 0,
                 14, 'm', 'a', 'x', 0,
                 15, 'm', 'i', 'n', 0,
                 16, 'n', 'o', 't', 0,
		193, 'o', 'u', 't', 0,
		157, 'r', 'o', 'w', 0,
                 17, 's', 'e', 't', 0,
                 64, 's', 'q', 'l', 0,
                 18, 's', 'u', 'm', 0,
		162, 't', 'o', 'p', 0,
/* 135 */         0,
                 /* Keywords of length 4 */
/* 136 */        51, 'b', 'a', 's', 'e', 0,
		155, 'c', 'a', 's', 'e', 0,
		183, 'c', 'a', 's', 't', 0,
                 20, 'c', 'o', 'p', 'y', 0,
		189, 'c', 'u', 'b', 'e', 0,
                 66, 'd', 'r', 'o', 'p', 0,
                 32, 'e', 'l', 's', 'e', 0,
		202, 'f', 'r', 'e', 'e', 0,
                 21, 'f', 'r', 'o', 'm', 0,
                108, 'f', 'u', 'l', 'l', 0,
                 23, 'i', 'n', 't', 'o', 0,
                109, 'j', 'o', 'i', 'n', 0,
                110, 'l', 'e', 'f', 't', 0,
                 67, 'l', 'i', 'k', 'e', 0,
		168, 'n', 'e', 'x', 't', 0,
                 68, 'n', 'u', 'l', 'l', 0,
                151, 'o', 'n', 'l', 'y', 0,
                 59, 'o', 'p', 'e', 'n', 0,
                143, 'r', 'e', 'a', 'd', 0,
                124, 'r', 'o', 'w', 's', 0,
                 25, 's', 'a', 'v', 'e', 0,
                 69, 's', 'o', 'm', 'e', 0,
                 33, 't', 'h', 'e', 'n', 0,
                 24, 'u', 's', 'e', 'r', 0,
                 97, 'v', 'i', 'e', 'w', 0,
                 114,'w', 'h', 'e', 'n', 0,
                 28, 'w', 'i', 't', 'h', 0,
                 70, 'w', 'o', 'r', 'k', 0,
/* 304 */         0,
                 /* Keywords of length 5 */
/* 305 */        29, 'a', 'b', 'o', 'r', 't', 0,
                 100,'a', 'l', 't', 'e', 'r', 0,
                 30, 'b', 'e', 'g', 'i', 'n', 0,
                136, 'b', 'y', 'r', 'e', 'f', 0,
		169, 'c', 'a', 'c', 'h', 'e', 0,
                 71, 'c', 'h', 'e', 'c', 'k', 0,
                 60, 'c', 'l', 'o', 's', 'e', 0,
                 31, 'c', 'o', 'u', 'n', 't', 0,
		166, 'c', 'r', 'o', 's', 's', 0,
		170, 'c', 'y', 'c', 'l', 'e', 0,
                 36, 'e', 'n', 'd', 'i', 'f', 0,
                 99, 'f', 'e', 't', 'c', 'h', 0,
		162, 'f', 'i', 'r', 's', 't', 0,
                 72, 'g', 'r', 'a', 'n', 't', 0,
                 73, 'g', 'r', 'o', 'u', 'p', 0,
                 58, 'i', 'n', 'd', 'e', 'x', 0,
                111, 'i', 'n', 'n', 'e', 'r', 0,
                194, 'i', 'n', 'o', 'u', 't', 0,
		159, 'l', 'e', 'a', 'v', 'e', 0,
                144, 'l', 'e', 'v', 'e', 'l', 0,
                122, 'l', 'o', 'c', 'a', 'l', 0,
                 74, 'o', 'r', 'd', 'e', 'r', 0,
		153, 'o', 'u', 't', 'e', 'r', 0,
                 105,'r', 'a', 'i', 's', 'e', 0,
                112, 'r', 'i', 'g', 'h', 't', 0,
		171, 's', 't', 'a', 'r', 't', 0,
                 75, 't', 'a', 'b', 'l', 'e', 0,
                 76, 'u', 'n', 'i', 'o', 'n', 0,
                 34, 'u', 'n', 't', 'i', 'l', 0,
                 87, 'u', 's', 'i', 'n', 'g', 0,
                 35, 'w', 'h', 'e', 'r', 'e', 0,
                 37, 'w', 'h', 'i', 'l', 'e', 0,
                152, 'w', 'r', 'i', 't', 'e', 0,
/* 536 */         0,
                 /* Keywords of length 6 */
/* 537 */       150, 'c', 'o', 'l', 'u', 'm', 'n', 0,
                 77, 'c', 'o', 'm', 'm', 'i', 't', 0,
                 38, 'c', 'r', 'e', 'a', 't', 'e', 0,
                 78, 'c', 'u', 'r', 's', 'o', 'r', 0,
                 39, 'd', 'e', 'f', 'i', 'n', 'e', 0,
                 40, 'd', 'e', 'l', 'e', 't', 'e', 0,
                 39, 'e', 'l', 's', 'e', 'i', 'f', 0,
                 116,'e', 'n', 'a', 'b', 'l', 'e', 0,
		160, 'e', 'n', 'd', 'f', 'o', 'r', 0,
                 96, 'e', 's', 'c', 'a', 'p', 'e', 0,
		163, 'e', 'x', 'c', 'e', 'p', 't', 0,
                 79, 'e', 'x', 'i', 's', 't', 's', 0,
                121, 'g', 'l', 'o', 'b', 'a', 'l', 0,
                 80, 'h', 'a', 'v', 'i', 'n', 'g', 0,
                 65, 'i', 'm', 'p', 'o', 'r', 't', 0,
                 81, 'i', 'n', 's', 'e', 'r', 't', 0,
                 41, 'm', 'o', 'd', 'i', 'f', 'y', 0,
                120, 'm', 'o', 'd', 'u', 'l', 'e', 0,
		184, 'n', 'u', 'l', 'l', 'i', 'f', 0,
		203, 'o', 'f', 'f', 's', 'e', 't', 0,
                 82, 'o', 'p', 't', 'i', 'o', 'n', 0,
                 19, 'p', 'e', 'r', 'm', 'i', 't', 0,
                 83, 'p', 'u', 'b', 'l', 'i', 'c', 0,
		165, 'r', 'a', 'w', 'p', 'c', 't', 0,
                 103,'r', 'e', 'm', 'o', 'v', 'e', 0,
                 106,'r', 'e', 'p', 'e', 'a', 't', 0,
		156, 'r', 'e', 's', 'u', 'l', 't', 0,
                 42, 'r', 'e', 't', 'u', 'r', 'n', 0,
                 101,'r', 'e', 'v', 'o', 'k', 'e', 0,
		190, 'r', 'o', 'l', 'l', 'u', 'p', 0,
                140, 's', 'c', 'h', 'e', 'm', 'a', 0,
		201, 's', 'c', 'r', 'o', 'l', 'l', 0,
                 84, 's', 'e', 'l', 'e', 'c', 't', 0,
                 43, 'u', 'n', 'i', 'q', 'u', 'e', 0,
                 62, 'u', 'p', 'd', 'a', 't', 'e', 0,
                 85, 'v', 'a', 'l', 'u', 'e', 's', 0,
/* 825 */         0,
                 /* Keywords of length 7 */
/* 826 */        86, 'b', 'e', 't', 'w', 'e', 'e', 'n', 0,
                126, 'c', 'a', 's', 'c', 'a', 'd', 'e', 0,
                118, 'c', 'o', 'm', 'm', 'e', 'n', 't', 0,
		186, 'c', 'o', 'l', 'l', 'a', 't', 'e', 0,
                 88, 'c', 'u', 'r', 'r', 'e', 'n', 't', 0,
		172, 'c', 'u', 'r', 'r', 'v', 'a', 'l', 0,
                 44, 'd', 'e', 'c', 'l', 'a', 'r', 'e', 0,
                129, 'd', 'e', 'f', 'a', 'u', 'l', 't', 0,
                117, 'd', 'i', 's', 'a', 'b', 'l', 'e', 0,
                 45, 'e', 'n', 'd', 'l', 'o', 'o', 'p', 0,
                 46, 'e', 'x', 'e', 'c', 'u', 't', 'e', 0,
                130, 'f', 'o', 'r', 'e', 'i', 'g', 'n', 0,
                 47, 'm', 'e', 's', 's', 'a', 'g', 'e', 0,
                113, 'n', 'a', 't', 'u', 'r', 'a', 'l', 0,
		173, 'n', 'e', 'x', 't', 'v', 'a', 'l', 0,
		174, 'n', 'o', 'c', 'a', 'c', 'h', 'e', 0,
		175, 'n', 'o', 'c', 'y', 'c', 'l', 'e', 0,
		176, 'n', 'o', 'o', 'r', 'd', 'e', 'r', 0,
                 89, 'p', 'r', 'e', 'p', 'a', 'r', 'e', 0,
                131, 'p', 'r', 'i', 'm', 'a', 'r', 'y', 0,
		177, 'r', 'e', 's', 't', 'a', 'r', 't', 0,
                125, 's', 'e', 's', 's', 'i', 'o', 'n', 0,
		192, 'w', 'i', 't', 'h', 'o', 'u', 't', 0,
/* 1033 */         0,
                 /* Keywords of length 8 */
/* 1034 */       115, 'c', 'a', 'l', 'l', 'p', 'r', 'o', 'c', 0,
		185, 'c', 'o', 'a', 'l', 'e', 's', 'c', 'e', 0,
                 90, 'c', 'o', 'n', 't', 'i', 'n', 'u', 'e', 0,
                 91, 'd', 'e', 's', 'c', 'r', 'i', 'b', 'e', 0,
                 92, 'd', 'i', 's', 't', 'i', 'n', 'c', 't', 0,
                 48, 'e', 'n', 'd', 'w', 'h', 'i', 'l', 'e', 0,
		191, 'g', 'r', 'o', 'u', 'p', 'i', 'n', 'g', 0,
		200, 'i', 'n', 't', 'e', 'r', 'v', 'a', 'l', 0,
		178, 'm', 'a', 'x', 'v', 'a', 'l', 'u', 'e', 0,
		179, 'm', 'i', 'n', 'v', 'a', 'l', 'u', 'e', 0,
                123, 'p', 'r', 'e', 's', 'e', 'r', 'v', 'e', 0,
                 107,'r', 'e', 'g', 'i', 's', 't', 'e', 'r', 0,
                 50, 'r', 'e', 'l', 'o', 'c', 'a', 't', 'e', 0,
		106, 'r', 'e', 'p', 'e', 'a', 't', 'e', 'd', 0,
                 127,'r', 'e', 's', 't', 'r', 'i', 'c', 't', 0,
                 93, 'r', 'o', 'l', 'l', 'b', 'a', 'c', 'k', 0,
/* 1194 */         0,
                 /* Keywords of length 9 */
/* 1195 */      145, 'c', 'o', 'm', 'm', 'i', 't', 't', 'e', 'd', 0,
                141, 'c', 'o', 'p', 'y', '_', 'f', 'r', 'o', 'm', 0,
                142, 'c', 'o', 'p', 'y', '_', 'i', 'n', 't', 'o', 0,
		158, 'e', 'n', 'd', 'r', 'e', 'p', 'e', 'a', 't', 0,
                128, 'e', 'x', 'c', 'l', 'u', 'd', 'i', 'n', 'g', 0,
                 94, 'i', 'm', 'm', 'e', 'd', 'i', 'a', 't', 'e', 0,
		180, 'i', 'n', 'c', 'r', 'e', 'm', 'e', 'n', 't', 0,
                 22, 'i', 'n', 't', 'e', 'g', 'r', 'i', 't', 'y', 0,
		164, 'i', 'n', 't', 'e', 'r', 's', 'e', 'c', 't', 0,
                146, 'i', 's', 'o', 'l', 'a', 't', 'i', 'o', 'n', 0,
                 49, 'p', 'r', 'o', 'c', 'e', 'd', 'u', 'r', 'e', 0,
                 53, 's', 'a', 'v', 'e', 'p', 'o', 'i', 'n', 't', 0,
		161, 's', 'u', 'b', 's', 't', 'r', 'i', 'n', 'g', 0,
		187, 's', 'y', 'm', 'm', 'e', 't', 'r', 'i', 'c', 0,
                119, 't', 'e', 'm', 'p', 'o', 'r', 'a', 'r', 'y', 0,
/* 1360 */         0,
                 /* Keywords of length 10 */
/* 1361 */	188, 'a', 's', 'y', 'm', 'm', 'e', 't', 'r', 'i', 'c', 0,
		134, 'c', 'o', 'n', 's', 't', 'r', 'a', 'i', 'n', 't', 0,
	        198, 'l', 'o', 'c', 'a', 'l', '_', 't', 'i', 'm', 'e', 0,
		181, 'n', 'o', 'm', 'a', 'x', 'v', 'a', 'l', 'u', 'e', 0,
		182, 'n', 'o', 'm', 'i', 'n', 'v', 'a', 'l', 'u', 'e', 0,
                 95, 'p', 'r', 'i', 'v', 'i', 'l', 'e', 'g', 'e', 's', 0,
                133, 'r', 'e', 'f', 'e', 'r', 'e', 'n', 'c', 'e', 's', 0,
                147, 'r', 'e', 'p', 'e', 'a', 't', 'a', 'b', 'l', 'e', 0,
/* 1457 */       0,
                 /* Keywords of length 11 */
/* 1458 */      104,'r','e','f','e','r','e','n','c','i','n','g',0,
                137,'s','y','s','t','e','m','_','u','s','e','r',0,
                148,'u','n','c','o','m','m','i','t','t','e','d',0,
/* 1497 */       0,
                 /* Keywords of length 12 */
/* 1498 */	195,'c','u','r','r','e','n','t','_','d','a','t','e',0,
	        196,'c','u','r','r','e','n','t','_','t','i','m','e',0,
		135,'c','u','r','r','e','n','t','_','u','s','e','r',0,
                138,'i','n','i','t','i','a','l','_','u','s','e','r',0,
                149,'s','e','r','i','a','l','i','z','a','b','l','e',0,
                139,'s','e','s','s','i','o','n','_','u','s','e','r',0,
/* 1582 */       0,
                 /* Keywords of length 13 */
/* 1583 */       98, 'a','u','t','h','o','r','i','z','a','t','i','o','n',0,
/* 1598 */       0,
		 /* Keywords of length 15 */
/* 1599 */      199,'l','o','c','a','l','_','t','i','m','e','s','t','a','m',
		    'p',0,
/* 1616 */	 0,
		 /* Keywords of length 17 */
/* 1617 */      197,'c','u','r','r','e','n','t','_','t','i','m','e','s','t','a',
		    'm','p',0,
		154,'s','y','s','t','e','m','_','m','a','i','n','t','a','i','n',
		    'e','d',0,
/* 1655 */	 0
                 };

/*
** This table contains pointers into the above Psl_keystr[] array.  The
** n'th pointer points to the keywords of length n in Psl_keystr (where n
** begins at zero).
**
** NOTE: WHENEVER YOU ADD A KEYWORD TO PSL_KEYSTR, YOU MUST ADJUST THE
**       POINTERS IN THIS STRUCTURE.
*/
 
static const u_char     *Key_index[] = {
                                  &Key_string[0],   /* Keywords of length 0 */
                                  &Key_string[0],   /* Keywords of length 1 */
                                  &Key_string[1],   /* Keywords of length 2 */
                                  &Key_string[50],  /* Keywords of length 3 */
                                  &Key_string[136], /* Keywords of length 4 */
                                  &Key_string[305], /* Keywords of length 5 */
                                  &Key_string[537], /* Keywords of length 6 */
                                  &Key_string[826], /* Keywords of length 7 */
                                  &Key_string[1034],/* Keywords of length 8 */
                                  &Key_string[1195],/* Keywords of length 9 */
                                  &Key_string[1361],/* Keywords of length 10 */
                                  &Key_string[1458],/* Keywords of length 11 */
                                  &Key_string[1498],/* Keywords of length 12 */
                                  &Key_string[1583],/* Keywords of length 13 */
                                  &Key_string[0],   /* Keywords of length 14 */
                                  &Key_string[1599],/* Keywords of length 15 */
                                  &Key_string[0],   /* Keywords of length 16 */
                                  &Key_string[1617],/* Keywords of length 17 */
                                  &Key_string[0],   /* Keywords of length 18 */
                                  &Key_string[0],   /* Keywords of length 19 */
                                  &Key_string[0],   /* Keywords of length 20 */
                                  &Key_string[0],   /* Keywords of length 21 */
                                  &Key_string[0],   /* Keywords of length 22 */
                                  &Key_string[0],   /* Keywords of length 23 */
                                  &Key_string[0],   /* Keywords of length 24 */
                                  &Key_string[0],   /* Keywords of length 25 */
                                  &Key_string[0],   /* Keywords of length 26 */
                                  &Key_string[0],   /* Keywords of length 27 */
                                  &Key_string[0],   /* Keywords of length 28 */
                                  &Key_string[0],   /* Keywords of length 29 */
                                  &Key_string[0],   /* Keywords of length 30 */
                                  &Key_string[0],   /* Keywords of length 31 */
                                  &Key_string[0]    /* Keywords of length 32 */
                                              };

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "Alter" to form a keyword.
*/
static const SECONDARY      Alterwords[] = {
		       { "group",	ALTGROUP,   PSL_GOVAL },
		       { "location",	ALTLOC,	    PSL_GOVAL },
		       { "profile",	ALTPROFILE, PSL_GOVAL },
		       { "role",	ALTROLE,    PSL_GOVAL },
		       { "security_audit", ALTSECAUDIT, PSL_GOVAL },
		       { "sequence",	ALTSEQ,	    PSL_GOVAL },
		       { "user",	ALTUSER,    PSL_GOVAL }
					     };

#define             ALTERSIZE		(sizeof(Alterwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "base" to form a keyword.
** Actually, not even BASE TABLE is a keyword;  the actual keyword is
** the triple BASE TABLE STRUCTURE.  At present, this is the only triple
** keyword, so it's special-cased in the multi keyword handler.
*/
static const SECONDARY      Basewords[] = {
		       { "table",	BASE_TABLE_STRUCTURE,	0 }
					     };

#define             BASESIZE		(sizeof(Basewords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "no" to form a keyword.
*/
static const SECONDARY      Nowords[] = {
		       { "cache",	NOCACHE, 0},
		       { "cycle",	NOCYCLE, 0},
		       { "maxvalue",	NOMAX,	0},
		       { "minvalue",	NOMIN,	0},
		       { "order",	NOORDER, 0}
					     };

#define             NOSIZE		(sizeof(Nowords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "begin" to form a keyword.
*/
static const SECONDARY      Beginwords[] = {
		       { "transaction", BGNXACT, PSL_GOVAL }
					     };

#define             BEGINSIZE           (sizeof(Beginwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "comment" to form a keyword.
*/
static const SECONDARY      Commentwords[] = {
		       { "on", COMMENT_ON, PSL_GOVAL }
					     };

#define             COMMENTSIZE	    (sizeof(Commentwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "create" to form a keyword.
*/
static const SECONDARY      Createwords[] = {
		       { "dbevent",	    CREATEEVENT,PSL_GOVAL },
		       { "group",	    CRTGROUP,	PSL_GOVAL },
		       { "integrity",	    CRTINTEG,	PSL_GOVAL },
		       { "link",	    CRTLINK,	PSL_GOVAL },
		       { "location",	    CRTLOC,	PSL_GOVAL },
		       { "permit",	    CRTPERM,	PSL_GOVAL },
		       { "profile",	    CRTPROFILE, PSL_GOVAL },
		       { "role",	    CRTROLE,	PSL_GOVAL },
		       { "rule",	    CRTRULE,	PSL_GOVAL },
		       { "security_alarm",  CRTSECALM,	PSL_GOVAL },
		       { "sequence",	    CRTSEQ,	PSL_GOVAL },
		       { "synonym",	    CRTSYNONYM, PSL_GOVAL }, 
		       { "trigger",	    CRTRULE,	PSL_GOVAL }, 
		       { "user",	    CRTUSER,	PSL_GOVAL },
		       { "view",	    CRTVIEW,	PSL_GOVAL }
					     };

#define             CREATESIZE	    (sizeof(Createwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "define" to form a keyword.
*/
static const SECONDARY      Definewords[] = {
			/* 6.0 repeat query */
			{ "qry",     DEFQRY,	 PSL_GOVAL },
			/* 5.0 repeat query */
			{ "query",     DEFQRY,	 PSL_GOVAL },
					      };

#define             DEFSIZE             (sizeof(Definewords) / sizeof(SECONDARY))


/*
** This table contains the words and corresponding tokens and values that
** can follow the introductory word "describe" to form a keyword.
*/
static const SECONDARY Describewords[] = {
		{"input",	DESCINPUT,	PSL_GOVAL},
		{"output",	DESCRIBE,	PSL_GOVAL}
};
#define	DESCRSIZE	(sizeof(Describewords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "drop" to form a keyword.
*/
static const SECONDARY      Dropwords[] = {
			{ "dbevent",	    DROPEVENT,  PSL_GOVAL },
			{ "group",	    DROPGROUP,	PSL_GOVAL },
			{ "integrity",	    DROPINTEG,	PSL_GOVAL },
			{ "link",	    DROPLINK,	PSL_GOVAL },
			{ "location",	    DROPLOC,	PSL_GOVAL },
			{ "permit",	    DROPPERM,	PSL_GOVAL },
			{ "profile",	    DROPPROFILE,PSL_GOVAL },
			{ "role",	    DROPROLE,	PSL_GOVAL },
			{ "rule",	    DROPRULE,	PSL_GOVAL },
			{ "security_alarm", DROPSECALM,	PSL_GOVAL },
		        { "sequence",	    DROPSEQ,	PSL_GOVAL },
			{ "synonym",	    DROPSYNONYM, PSL_GOVAL },
			{ "trigger",	    DROPRULE,	PSL_GOVAL },
			{ "user",	    DROPUSER,	PSL_GOVAL },
			{ "view",	    DROPVIEW,	PSL_GOVAL }
					       };

#define             DROPSIZE            (sizeof(Dropwords)/sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "disable" to form a keyword.
*/
static const SECONDARY      Disablewords[] = {
		       { "security_audit",  DISECAUDIT,	PSL_GOVAL }
					     };

#define             DISABLESIZE           (sizeof(Disablewords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "for" to form a keyword.
*/
static const SECONDARY      Forwords[] = {
		       { "deferred",	DEFERUPD,   0},
		       { "direct",	DIRECTUPD,  0},
		       { "readonly",	FORREADONLY,0},
					     };

#define             FORSIZE           (sizeof(Forwords) / sizeof(SECONDARY))


/* This table contains the words and the corresponding tokens and values that
** can follow the introductory word "foreign" to form a keyword.
*/
static const SECONDARY	Foreignwords[] = {
			{ "key",	FOREIGNKEY, 0}
				};

#define		FOREIGNSIZE	(sizeof(Foreignwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "enable" to form a keyword.
*/
static const SECONDARY      Enablewords[] = {
		       { "security_audit",  ENSECAUDIT,	PSL_GOVAL }
					     };

#define             ENABLESIZE           (sizeof(Enablewords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "not" to form a keyword.
*/
static const SECONDARY      Notwords[] = {
		       { "like",	NOTLIKE,   0},
					     };

#define             NOTSIZE           (sizeof(Notwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "on" to form a keyword.
*/
static const SECONDARY      Onwords[] = {
		       { "commit",	ONCOMMIT,     0},
		       { "current",     ONCURRENT,    0},
		       { "database",	ONDATABASE,   0},
		       { "dbevent",	ONEVENT,      0},
		       { "location",	ONLOCATION,   0},
		       { "procedure",	ONPROCEDURE, 0},
		       { "sequence",	ONSEQUENCE,   0}
					     };

#define             ONSIZE            (sizeof(Onwords) / sizeof(SECONDARY))


/* This table contains the words and the corresponding tokens and values that
** can follow the introductory word "primary" to form a keyword.
*/
static const SECONDARY	Primarywords[] = {
			{ "key",	PRIMARYKEY, 0}
				};

#define		PRIMARYSIZE	(sizeof(Primarywords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "set" to form a keyword.
**
** NOTE:  '_' < '[a-z]' in both EBCDIC and ASCII
*/
static const SECONDARY      Setwords[] = {
			{ "aggregate",    SETAGGR,     PSL_GOVAL  },
			{ "autocommit",	  SETAUTOCOMMIT, PSL_GOVAL},
			{ "cache_dynamic",SETCACHEDYN, PSL_ONSET  },
		        { "cpufactor",    SETCPUFACT,  PSL_GOVAL  },
			{ "date_format",  SETDATEFMT,  PSL_GOVAL  },
			{ "ddl_concurrency",SETDDLCONCUR,PSL_GOVAL},
			{ "decimal",      SETDECIMAL,  PSL_GOVAL  },
			{ "flatten",      SETFLATTEN,  PSL_ONSET  },
			{ "hash",         SETHASH,     PSL_ONSET  },
			{ "io_trace",     SETIOTRACE,  PSL_ONSET  },
			{ "joinop",       SETJOINOP,   PSL_ONSET  },
			{ "journaling",   SETJOURNAL,  PSL_ONSET  },
			{ "lock_trace",   SETLOCKTRACE,PSL_ONSET  },
			{ "lockmode",     SETLOCKMODE, PSL_GOVAL  },
			{ "log_trace",    SETLOGTRACE, PSL_ONSET  },
			{ "logdbevents",  SETLOGEVENTS,PSL_ONSET  },
			{ "logging",	  SETLOGGING,  PSL_ONSET  },
			{ "maxconnect",   SETMXCONNECT,PSL_ONSET  },
			{ "maxcost",      SETMXCOST,   PSL_ONSET  },
			{ "maxcpu",       SETMXCPU,    PSL_ONSET  },
			{ "maxidle",      SETMXIDLE,   PSL_ONSET  },
			{ "maxio",	  SETMXIO,     PSL_ONSET  },
			{ "maxpage",      SETMXPAGE,   PSL_ONSET  },
			{ "maxquery",     SETMXIO,     PSL_ONSET  },
			{ "maxrow",	  SETMXROW,    PSL_ONSET  },
			{ "money_format", SETMNYFMT,   PSL_GOVAL  },
			{ "money_prec",   SETMNYPREC,  PSL_GOVAL  },
			{ "nocache_dynamic",SETCACHEDYN, PSL_OFFSET},
			{ "noflatten",    SETFLATTEN,  PSL_OFFSET },
			{ "nohash",       SETHASH,     PSL_OFFSET },
			{ "noio_trace",   SETIOTRACE,  PSL_OFFSET },
			{ "nojoinop",     SETJOINOP,   PSL_OFFSET },
			{ "nojournaling", SETJOURNAL,  PSL_OFFSET },
			{ "nolock_trace", SETLOCKTRACE,PSL_OFFSET },
			{ "nolog_trace",  SETLOGTRACE, PSL_OFFSET },
			{ "nologdbevents",SETLOGEVENTS,PSL_OFFSET },
			{ "nologging",	  SETLOGGING,  PSL_OFFSET },
			{ "nomaxconnect", SETMXCONNECT,PSL_OFFSET },
			{ "nomaxcost",    SETMXCOST,   PSL_OFFSET },
			{ "nomaxcpu",     SETMXCPU,    PSL_OFFSET },
			{ "nomaxidle",	  SETMXIDLE,   PSL_OFFSET },
			{ "nomaxio",	  SETMXIO,     PSL_OFFSET },
			{ "nomaxpage",    SETMXPAGE,   PSL_OFFSET },
			{ "nomaxquery",   SETMXIO,     PSL_OFFSET },
			{ "nomaxrow",	  SETMXROW,    PSL_OFFSET },
			{ "noojflatten",  SETOJFLATTEN, PSL_OFFSET },
			{ "nooptimizeonly", SETOPTIMIZEONLY, PSL_OFFSET },
			{ "noparallel",	  SETPARALLEL,	PSL_OFFSET },
			{ "noprintdbevents",SETPRTEVENTS,PSL_OFFSET },
			{ "noprintqry",	  SETPRINTQRY, PSL_OFFSET },
			{ "noprintrules", SETPRTRULES, PSL_OFFSET  },
			{ "noqep",	  SETQEP,      PSL_OFFSET },
			{ "norules",	  SETRULES,    PSL_OFFSET },
			{ "nostatistics", SETSTATS,    PSL_OFFSET },
			{ "notrace",      SETTRACE,    PSL_OFFSET },
			{ "nounicode_substitution",	  SETUNICODESUB, PSL_OFFSET },
			{ "ojflatten",    SETOJFLATTEN, PSL_ONSET },
			{ "optimizeonly", SETOPTIMIZEONLY, PSL_ONSET },
			{ "parallel",	  SETPARALLEL,	PSL_ONSET },
			{ "printdbevents",SETPRTEVENTS,	PSL_ONSET },
			{ "printqry",	  SETPRINTQRY, PSL_ONSET  },
			{ "printrules",	  SETPRTRULES, PSL_ONSET  },
			{ "qep",	  SETQEP,      PSL_ONSET  },
			{ "random_seed",  SETRANDOMSEED,PSL_OFFSET },
			{ "result_structure",SETRESSTRUCT,PSL_GOVAL},
			{ "ret_into",     SETRETINTO,  PSL_GOVAL  },
			{ "role",	  SETROLE,     PSL_ONSET},
			{ "rules",	  SETRULES,    PSL_ONSET  },
			{ "session",	  SETSESSION,  PSL_GOVAL  },
			{ "statistics",   SETSTATS,    PSL_ONSET  },
			{ "trace",        SETTRACE,    PSL_ONSET  },
                        { "transaction",  SETTRANSACTION,    PSL_GOVAL },
			{ "unicode_substitution",	  SETUNICODESUB, PSL_GOVAL},
			{ "update_rowcount", SETUPDROWCNT, PSL_GOVAL },
			{ "work",	  SETWORK,     PSL_GOVAL  },
					   };

#define             SETSIZE             (sizeof(Setwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "end" to form a keyword.
*/
static const SECONDARY      Endwords[] = {
			{ "transaction",    ENDXACT,     PSL_GOVAL  }
					    };
#define             ENDSIZE             (sizeof(Endwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "current" to form a keyword.
*/
static const SECONDARY      Currwords[] = {
			{ "value",	    CURRVAL,     PSL_GOVAL  }
					    };
#define             CURRSIZE             (sizeof(Currwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "next" to form a keyword.
*/
static const SECONDARY      Nextwords[] = {
			{ "value",	    NEXTVAL,     PSL_GOVAL  }
					    };
#define             NEXTSIZE             (sizeof(Nextwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "raise" to form a keyword.
*/
static const SECONDARY      Raisewords[] = {
		       { "dbevent", 	RAISEEVENT,   PSL_GOVAL},
		       { "error",	RAISEERROR,   PSL_GOVAL}
					     };

#define             RAISESIZE           (sizeof(Raisewords) / sizeof(SECONDARY))


/* This table contains the words that can follow "register" to form a keyword */
static const SECONDARY 	Registerwords[] = {
			{ "dbevent", REGISTEREVENT, PSL_GOVAL }
					    };

#define             REGSIZE            (sizeof(Registerwords)/sizeof(SECONDARY))

/* This table contains the words that can follow "remove" to form a keyword */
static const SECONDARY Removewords[] = {
			{ "dbevent", 	REMOVEEVENT, 0},
					    };

#define             REMOVESIZE          (sizeof(Removewords)/sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "TO" to form a keyword.
*/
static const SECONDARY      Towords[] = {
		       { "group",	TOGROUP,    0},
		       { "role",	TOROLE,	    0},
		       { "user",	TOUSER,	    0}
					     };

#define             TOSIZE		(sizeof(Towords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "CROSS" to form a keyword.
*/
static const SECONDARY      Crosswords[] = {
		       { "join",	CROSSJOIN,   0}
					     };

#define             CROSSSIZE		(sizeof(Crosswords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "FULL" to form a keyword.
*/
static const SECONDARY      Fullwords[] = {
		       { "anti",	FULLANTI,   0},
		       { "join",	FULLJOIN,   0},
		       { "outer",	FULLOUTER,  0}
					     };

#define             FULLSIZE		(sizeof(Fullwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "FREE" to form a keyword.
*/
static const SECONDARY      Freewords[] = {
		       { "locator",	FREELOCATOR,0}
					     };

#define             FREESIZE		(sizeof(Freewords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "LEFT" to form a keyword.
*/
static const SECONDARY      Leftwords[] = {
		       { "anti",	LEFTANTI,   0},
		       { "join",	LEFTJOIN,   0},
		       { "outer",	LEFTOUTER,  0}
					     };

#define             LEFTSIZE		(sizeof(Leftwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "INNER" to form a keyword.
*/
static const SECONDARY      Innerwords[] = {
		       { "join",	INNERJOIN,  0}
					     };

#define             INNERSIZE		(sizeof(Innerwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "INTERSECT" to form a keyword.
*/
static const SECONDARY      Intersectwords[] = {
		       { "join",	INTERSECTJOIN,  0}
					     };

#define             INTERSECTSIZE	(sizeof(Intersectwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "RIGHT" to form a keyword.
*/
static const SECONDARY      Rightwords[] = {
		       { "anti",	RIGHTANTI,   0},
		       { "join",	RIGHTJOIN,  0},
		       { "outer",	RIGHTOUTER, 0}
					     };

#define             RIGHTSIZE		(sizeof(Rightwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "GROUPING" to form a keyword.
*/
static const SECONDARY      Groupingwords[] = {
		       { "sets",	GROUPINGSETS, 0}
					     };

#define             GROUPINGSIZE	(sizeof(Groupingwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "RESULT" to form a keyword.
*/
static const SECONDARY      Resultwords[] = {
		       { "row",		RESULTROW,  0}
					     };

#define             RESULTSIZE		(sizeof(Resultwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "WITH" to form a keyword.
*/
static const SECONDARY      Withwords[] = {
			{ "local",	WLOCAL,	0},
			{ "short_remark",	SHORT_REMARK,  0},
			{ "time",	WTIME, 0}
					     };

#define             WITHSIZE		(sizeof(Withwords) / sizeof(SECONDARY))

/*
** This table contains the words and the corresponding tokens and values that
** can follow the introductory word "WITHOUT" to form a keyword.
*/
static const SECONDARY      Withoutwords[] = {
			{ "time",	WOTIME, 0}
					     };

#define             WITHOUTSIZE		(sizeof(Withoutwords) / sizeof(SECONDARY))

/*
** This table contains the tokens and corresponding values for each keyword.
** For those keywords that are made up of two words, there will be a pointer
** to a secondary word table, and a count telling the length of the secondary
** table.  NOTE THAT THE ORDER OF THE ROWS IN THIS TABLE CORRESPONDS TO THE
** NUMBERS IN Key_string[].  DO NOT CHANGE THINGS AROUND IN HERE WITHOUT
** BRINGING Key_string[] UP TO DATE.
*/

static const KEYINFO                Key_info[] = {
		       /* Zero is not a legal index, this is a placeholder */
/* 0 */		       {    0,	    0,		0,	(SECONDARY *) NULL   },
/* 1 */		       {    AS,	    0,		0,	(SECONDARY *) NULL   },
/* 2 */		       {    AT,	    0,		0,	(SECONDARY *) NULL   },
/* 3 */		       {    BY,	    0,		0,	(SECONDARY *) NULL   },
/* 4 */		       {    IN,	    0,		0,	(SECONDARY *) NULL   },
/* 5 */		       {    IS,	    0,		0,	(SECONDARY *) NULL   },
/* 6 */		       {    OF,	    0,		0,	(SECONDARY *) NULL   },
/* 7 */		       {    ON,	    0,		ONSIZE,	(SECONDARY *) Onwords},
/* 8 */		       {    OR,     0,		0,	(SECONDARY *) NULL   },
/* 9 */		       {    TO,	    0,	        TOSIZE,	(SECONDARY *) Towords},
/* 10 */	       {    ALL,    0,		0,	(SECONDARY *) NULL   },
/* 11 */	       {    AND,    0,		0,	(SECONDARY *) NULL   },
/* 12 */	       {    ANY,    0,		0,	(SECONDARY *) NULL   },
/* 13 */	       {    AVG,    0,		0,	(SECONDARY *) NULL   },
/* 14 */	       {    MAX,    0,		0,	(SECONDARY *) NULL   },
/* 15 */	       {    MIN,    0,		0,	(SECONDARY *) NULL   },
/* 16 */	       {    NOT,    0,		NOTSIZE,(SECONDARY *) Notwords},
/* 17 */	       {    SET,    PSL_GOVAL,	SETSIZE,(SECONDARY *) Setwords},
/* 18 */	       {    SUM,    0,		0,	(SECONDARY *) NULL   },
/* 19 */	       {    PERMIT, 0,		0,	(SECONDARY *) NULL   },
/* 20 */	       {    COPY,   PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 21 */	       {    FROM,   0,		0,	(SECONDARY *) NULL   },
/* 22 */	       {    INTEGRITY, 0,	0,	(SECONDARY *) NULL   },
/* 23 */	       {    INTO,   0,		0,	(SECONDARY *) NULL   },
/* 24 */	       {    USER,   0,		0,	(SECONDARY *) NULL   },
/* 25 */	       {    SAVE,   PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 26 */	       {    DO,	    0,		0,	(SECONDARY *) NULL   },
/* 27 */	       {    IF,	    0,		0,	(SECONDARY *) NULL   },
/* 28 */	       {    WITH,   0,		WITHSIZE,(SECONDARY *) Withwords},
/* 29 */	       {    ABORT,  PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 30 */	       {    BEGIN,  0,		BEGINSIZE,(SECONDARY *) Beginwords	     },
/* 31 */	       {    COUNT,  0,		0,	(SECONDARY *) NULL   },
/* 32 */	       {    ELSE,   0,		0,	(SECONDARY *) NULL   },
/* 33 */	       {    THEN,   0,		0,	(SECONDARY *) NULL   },
/* 34 */	       {    UNTIL,  0,		0,	(SECONDARY *) NULL   },
/* 35 */	       {    WHERE,  0,		0,	(SECONDARY *) NULL   },
/* 36 */	       {    ENDIF,  0,		0,	(SECONDARY *) NULL   },
/* 37 */	       {    WHILE,  0,		0,	(SECONDARY *) NULL   },
/* 38 */	       {    CREATE, PSL_GOVAL,	CREATESIZE,(SECONDARY *)  Createwords      },
/* 39 */	       {    ELSEIF, 0,		DEFSIZE, (SECONDARY *) Definewords	     },
/* 40 */	       {    DELETE, PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 41 */	       {    MODIFY, PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 42 */	       {    RETURN, 0,		0,	(SECONDARY *) NULL   },
/* 43 */	       {    UNIQUE, 0,		0,	(SECONDARY *) NULL   },
/* 44 */	       {    DECLARE,0,		0,	(SECONDARY *) NULL   },
/* 45 */	       {    ENDLOOP,0,		0,	(SECONDARY *) NULL   },
/* 46 */	       {    EXECUTE,PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 47 */	       {    MESSAGE,0,		0,	(SECONDARY *) NULL   },
/* 48 */	       {   ENDWHILE,0,		0,	(SECONDARY *) NULL   },
/* 49 */	       {  PROCEDURE,0,		0,	(SECONDARY *) NULL   },
/* 50 */	       {  RELOCATE, PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 51 */	       {/* BASE */ 0,  0,	BASESIZE,(SECONDARY *) Basewords },
/* 52 */	       {    0,	    0,		0,	(SECONDARY *) NULL   },
/* 53 */	       { SAVEPOINT, PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 54 */	       {    0,	    0,		0,	(SECONDARY *) NULL   },
/* 55 */	       {    0,	    0,		0,	(SECONDARY *) NULL   },
/* 56 */	       {    0,      0,          0,	(SECONDARY *) NULL   },
/* 57 */	       {    END,    0,		ENDSIZE,  (SECONDARY *) Endwords           },
/* 58 */	       {    INDEX,  PSL_GOVAL,  0,	(SECONDARY *) NULL   },
/* 59 */	       {    OPEN,   PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 60 */	       {    CLOSE,  PSL_GOVAL,  0,	(SECONDARY *) NULL   },
/* 61 */	       {    FOR,    0,		FORSIZE,  (SECONDARY *) Forwords	     },
/* 62 */	       {    UPDATE, 0,		0,	(SECONDARY *) NULL   },
/* 63 */	       {    ASC,    0,		0,	(SECONDARY *) NULL   },
/* 64 */	       {    SQL,    0,		0,	(SECONDARY *) NULL   },
/* 65 */	       {    IMPORT, 0,		0,	(SECONDARY *) NULL   },
/* 66 */	       {    DROP,   PSL_GOVAL,	DROPSIZE, (SECONDARY *) Dropwords	     },
/* 67 */	       {    LIKE,   0,		0,	(SECONDARY *) NULL   },
/* 68 */	       {    NULLWORD,0,		0,	(SECONDARY *) NULL   },
/* 69 */	       {    SOME,   0,		0,	(SECONDARY *) NULL   },
/* 70 */	       {    WORK,   0,		0,	(SECONDARY *) NULL   },
/* 71 */	       {    CHECK,  0,		0,	(SECONDARY *) NULL   },
/* 72 */	       {    GRANT,  PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 73 */	       {    GROUP,  0,		0,	(SECONDARY *) NULL   },
/* 74 */	       {    ORDER,  0,		0,	(SECONDARY *) NULL   },
/* 75 */	       {    TABLE,  0,		0,	(SECONDARY *) NULL   },
/* 76 */	       {    UNION,  0,		0,	(SECONDARY *) NULL   },
/* 77 */	       {    COMMIT, PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 78 */	       {    CURSOR, 0,		0,	(SECONDARY *) NULL   },
/* 79 */	       {    EXISTS, 0,		0,	(SECONDARY *) NULL   },
/* 80 */	       {    HAVING, 0,		0,	(SECONDARY *) NULL   },
/* 81 */	       {    INSERT, PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 82 */	       {    OPTION, 0,		0,	(SECONDARY *) NULL   },
/* 83 */	       {    PUBLIC, 0,		0,	(SECONDARY *) NULL   },
/* 84 */	       {    SELECT, PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 85 */	       {    VALUES, 0,		0,	(SECONDARY *) NULL   },
/* 86 */	       {    BETWEEN, 0,		0,	(SECONDARY *) NULL   },
/* 87 */	       {    USING,  0,		0, 	(SECONDARY *) NULL   },
/* 88 */	       {    CURRENT,	    0,CURRSIZE,	(SECONDARY *)Currwords},
/* 89 */	       {    PREPARE,  PSL_GOVAL,0,	(SECONDARY *) NULL   },
/* 90 */	       {    CONTINUE,	    0,	0,	(SECONDARY *) NULL   },
/* 91 */	       {    DESCRIBE, PSL_GOVAL,DESCRSIZE, (SECONDARY *) Describewords   },
/* 92 */	       {    DISTINCT, 0,	0,	(SECONDARY *) NULL   },
/* 93 */	       {    ROLLBACK,PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 94 */	       {    IMMEDIATE,	    0,  0,	(SECONDARY *) NULL   },
/* 95 */	       {    PRIVILEGES,	    0,	0,	(SECONDARY *) NULL   },
/* 96 */	       {    ESCAPE,	    0,	0,	(SECONDARY *) NULL   },
/* 97 */	       {    VIEW,	    0,	0,	(SECONDARY *) NULL   },
/* 98 */	       {    AUTHORIZATION,  0,	0,	(SECONDARY *) NULL   },
/* 99 */	       {    FETCH,  PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 100 */	       {    ALTER,  PSL_GOVAL,	ALTERSIZE,(SECONDARY *) Alterwords},
/* 101 */	       {    REVOKE, PSL_GOVAL,	0,	(SECONDARY *) NULL   },
/* 102 */	       {    ADD,	    0,	0,	(SECONDARY *) NULL   },
/* 103 */	       {    REMOVE, PSL_GOVAL,  REMOVESIZE, (SECONDARY *) Removewords	     },
/* 104 */	       {    REFERENCING,    0,	0,	(SECONDARY *) NULL   },
/* 105 */	       {    RAISE,  0,		RAISESIZE,(SECONDARY *)Raisewords},
/* 106 */	       {    REPEAT, 	    0,	0,	(SECONDARY *) NULL   },
/* 107 */	       {    REGISTER, PSL_GOVAL, REGSIZE, (SECONDARY *) Registerwords},
/* 108 */	       {/* FULL */  0,	    0,FULLSIZE,(SECONDARY *)Fullwords},
/* 109 */	       {    JOIN,	    0,	0,	(SECONDARY *) NULL   },
/* 110 */	       {/* LEFT */  0,	    0,LEFTSIZE,(SECONDARY *)Leftwords},
/* 111 */	       {/* INNER */ 0,	    0,INNERSIZE,(SECONDARY*)Innerwords},
/* 112 */	       {/* RIGHT */ 0,	    0,RIGHTSIZE,(SECONDARY*)Rightwords},
/* 113 */	       {    NATURAL,	    0,	0,	(SECONDARY *) NULL   },
/* 114 */	       {    WHEN,	    0,	0,	(SECONDARY *) NULL   },
/* 115 */	       {    CALLPROC,	    PSL_GOVAL,0, (SECONDARY *) NULL  },
/* 116 */	       {/* ENABLE*/ 0,PSL_GOVAL, ENABLESIZE,
							(SECONDARY *)Enablewords},
/* 117 */	       {/*DISABLE*/ 0,PSL_GOVAL,DISABLESIZE,
							(SECONDARY *)Disablewords},
/* 118 */	       {/*COMMENT*/0, PSL_GOVAL, COMMENTSIZE,
							(SECONDARY *) Commentwords},
/* 119 */	       {TEMPORARY,	    0,	0,	(SECONDARY *) NULL   },
/* 120 */	       {   MODULE,	    0,	0,	(SECONDARY *) NULL   },
/* 121 */	       {   GLOBAL,	    0,	0,	(SECONDARY *) NULL   },
/* 122 */	       {    LOCAL,	    0,	0,	(SECONDARY *) NULL   },
/* 123 */	       { PRESERVE,	    0,	0,	(SECONDARY *) NULL   },
/* 124 */	       {     ROWS,	    0,	0,	(SECONDARY *) NULL   },
/* 125 */	       {  SESSION,	    0,	0,	(SECONDARY *) NULL   },
/* 126 */	       { CASCADE,	    0,	0,	(SECONDARY *) NULL   },
/* 127 */	       { RESTRICT,	    0,  0,      (SECONDARY *) NULL   },
/* 128 */	       { EXCLUDING,	    0,  0,      (SECONDARY *) NULL   },
   /* for constraints */
/* 129 */	       {  DEFAULT,	    0,	0,	(SECONDARY *) NULL   },
/* 130 */	       {/*FOREIGN*/ 0,	    0,	FOREIGNSIZE, (SECONDARY *)Foreignwords},
/* 131 */	       {/*PRIMARY*/ 0,	    0,	PRIMARYSIZE, (SECONDARY *) Primarywords},
/* 132 */	       {  /* notused */ 0,  0,	0,	(SECONDARY *) NULL   },
/* 133 */	       {  REFERENCES,	    0,	0,	(SECONDARY *) NULL   },
/* 134 */	       {  CONSTRAINT,	    0,	0,	(SECONDARY *) NULL   },
/* 135 */	       {  CURRENT_USER,	    0,	0,	(SECONDARY *) NULL   },
/* 136 */	       { BYREF,		    0,  0,      (SECONDARY *) NULL   },
/* 137 */              {  SYSTEM_USER,      0,  0,      (SECONDARY *) NULL   },
/* 138 */              {  INITIAL_USER,     0,  0,      (SECONDARY *) NULL   },
/* 139 */              {  SESSION_USER,     0,  0,      (SECONDARY *) NULL   },
/* 140 */	       { SCHEMA,	    0,  0,      (SECONDARY *) NULL },
/* 141 */	       { COPY_FROM,	    0,  0,      (SECONDARY *) NULL },
/* 142 */	       { COPY_INTO,	    0,  0,      (SECONDARY *) NULL },
/* 143 */              { READ,              0,  0,      (SECONDARY *) NULL   },
/* 144 */              { LEVEL,             0,  0,      (SECONDARY *) NULL   },
/* 145 */              { COMMITTED,         0,  0,      (SECONDARY *) NULL   },
/* 146 */              { ISOLATION,         0,  0,      (SECONDARY *) NULL   },
/* 147 */              { REPEATABLE,        0,  0,      (SECONDARY *) NULL   },
/* 148 */              { UNCOMMITTED,       0,  0,      (SECONDARY *) NULL   },
/* 149 */              { SERIALIZABLE,      0,  0,      (SECONDARY *) NULL   },
/* 150 */	       { COLUMN,            0,  0,      (SECONDARY *) NULL   },
/* 151 */	       { ONLY,              0,  0,      (SECONDARY *) NULL   },
/* 152 */	       { WRITE,             0,  0,      (SECONDARY *) NULL   },
/* 153 */	       { OUTER,             0,  0,      (SECONDARY *) NULL   },
/* 154 */	       { SYSTEM_MAINTAINED, 0,  0,      (SECONDARY *) NULL   },
/* 155 */	       { CASE,              0,  0,      (SECONDARY *) NULL   },
/* 156 */	       {/* RESULT */ 0,	    0,RESULTSIZE,(SECONDARY*)Resultwords},
/* 157 */	       { ROW,               0,  0,      (SECONDARY *) NULL   },
/* 158 */	       { ENDREPEAT,         0,  0,      (SECONDARY *) NULL   },
/* 159 */	       { LEAVE,             0,  0,      (SECONDARY *) NULL   },
/* 160 */	       { ENDFOR,            0,  0,      (SECONDARY *) NULL   },
/* 161 */	       { SUBSTRING,         0,  0,      (SECONDARY *) NULL   },
/* 162 */	       { FIRST,             0,  0,      (SECONDARY *) NULL   },
/* 163 */	       { EXCEPT,            0,  0,      (SECONDARY *) NULL   },
/* 164 */	       { INTERSECT,         0,  0,      (SECONDARY *) NULL   },
/* 165 */	       { RAWPCT,            0,  0,      (SECONDARY *) NULL   },
/* 166 */	       {/* CROSS */ 0,	    0,CROSSSIZE,(SECONDARY*)Crosswords},
/* 167 */	       {/* NO */ 0,    	    0,	NOSIZE,	(SECONDARY *) Nowords},
/* 168 */	       { NEXT,	    	    0,	NEXTSIZE,(SECONDARY *)Nextwords},
/* 169 */	       { CACHE,             0,  0,      (SECONDARY *) NULL   },
/* 170 */	       { CYCLE,             0,  0,      (SECONDARY *) NULL   },
/* 171 */	       { START,             0,  0,      (SECONDARY *) NULL   },
/* 172 */	       { CURRVAL,           0,  0,      (SECONDARY *) NULL   },
/* 173 */	       { NEXTVAL,           0,  0,      (SECONDARY *) NULL   },
/* 174 */	       { NOCACHE,           0,  0,      (SECONDARY *) NULL   },
/* 175 */	       { NOCYCLE,           0,  0,      (SECONDARY *) NULL   },
/* 176 */	       { NOORDER,           0,  0,      (SECONDARY *) NULL   },
/* 177 */	       { RESTART,           0,  0,      (SECONDARY *) NULL   },
/* 178 */	       { MAXVALUE,          0,  0,      (SECONDARY *) NULL   },
/* 179 */	       { MINVALUE,          0,  0,      (SECONDARY *) NULL   },
/* 180 */	       { INCREMENT,         0,  0,      (SECONDARY *) NULL   },
/* 181 */	       { NOMAX,             0,  0,      (SECONDARY *) NULL   },
/* 182 */	       { NOMIN,             0,  0,      (SECONDARY *) NULL   },
/* 183 */	       { CAST,		    0,	0,	(SECONDARY *) NULL   },
/* 184 */	       { NULLIF,	    0,	0,	(SECONDARY *) NULL   },
/* 185 */	       { COALESCE,	    0,	0,	(SECONDARY *) NULL   },
/* 186 */	       { COLLATE,	    0,	0,	(SECONDARY *) NULL   },
/* 187 */	       { SYMMETRIC,	    0,	0,	(SECONDARY *) NULL   },
/* 188 */	       { ASYMMETRIC,	    0,	0,	(SECONDARY *) NULL   },
/* 189 */	       { CUBE,		    0,  0,	(SECONDARY *) NULL   },
/* 190 */	       { ROLLUP,	    0,  0,	(SECONDARY *) NULL   },
/* 191 */	       { GROUPING, PSL_GOVAL, GROUPINGSIZE,(SECONDARY *) Groupingwords },
/* 192 */	       { WITHOUT, PSL_GOVAL, WITHOUTSIZE, (SECONDARY *) Withoutwords },
/* 193 */	       { OUT,	    	    0,  0,	(SECONDARY *) NULL   },
/* 194 */	       { INOUT,	    	    0,  0,	(SECONDARY *) NULL   },
/* 195 */	       { CURDATE,	    0,	0,	(SECONDARY *) NULL   },
/* 196 */	       { CURTIME,	    0,	0,	(SECONDARY *) NULL   },
/* 197 */	       { CURTIMESTAMP, 0,	0,	(SECONDARY *) NULL   },
/* 198 */     	       { LOCTIME,	    0,	0,	(SECONDARY *) NULL   },
/* 199 */      	       { LOCTIMESTAMP,   0,	0,	(SECONDARY *) NULL   },
/* 200 */      	       { INTERVAL,	    0,	0,	(SECONDARY *) NULL   },
/* 201 */	       { SCROLL,	    0,	0,	(SECONDARY *) NULL   },
/* 202 */	       {/* FREE */ 	    0,  0,FREESIZE,(SECONDARY*)Freewords},
/* 203 */	       { OFFSET,	    0,	0,	(SECONDARY *) NULL   }
};

/* Alternate keyword lists for inside WITH parsing (specifically, when
** the WITH PARTITION= production has been seen).  Partition clause
** parsing is just a bit too hard for YACC-based parsers, unless suitable
** keywords exist.  We don't want AUTOMATIC, HASH, etc to be keywords
** in general.  Therefore they are only turned on when the grammar
** says to, by setting the yy_partalt_kwds flag in yyvars.
** Note that these keywords are used IN PLACE OF the usual list, not
** in addition to them.
**
** See the main keyword lists for details on how these things work.
** Don't mess with the lists unless you understand them!
*/

static const u_char alt_part_strings[] =
{
	/* PARTITION= keywords of length 0, or unused lengths */
	/* 0 */		0,

	/* PARTITION= keywords of length 2 */
	/* 1 */		1, 'o', 'n', 0,
			2, 't', 'o', 0,
	/* 9 */		0,
	/* PARTITION= keywords of length 4 */
	/* 10 */	3, 'h', 'a', 's', 'h', 0,
			4, 'l', 'i', 's', 't', 0,
			5, 'n', 'u', 'l', 'l', 0,
			6, 'w', 'i', 't', 'h', 0,
	/* 34 */	0,
	/* PARTITION= keywords of length 5 */
	/* 35 */	7, 'r', 'a', 'n', 'g', 'e', 0,
	/* 42 */	0,
	/* PARTITION= keywords of length 6 */
	/* 43 */	8, 'v', 'a', 'l', 'u', 'e', 's', 0,
	/* 51 */	0,
	/* PARTITION= keywords of length 9 */
	/* 52 */	 9, 'a', 'u', 't', 'o', 'm', 'a', 't', 'i', 'c', 0,
			10, 'p', 'a', 'r', 't', 'i', 't', 'i', 'o', 'n', 0,
	/* 74 */	0
};

/* Index into alt_part_kwds for all possible keyword lengths */
static const u_char *alt_part_index[] =
{
	&alt_part_strings[0],	/* Length 0 */
	&alt_part_strings[0],	/* Length 1 */
	&alt_part_strings[1],	/* Length 2 */
	&alt_part_strings[0],	/* Length 3 */
	&alt_part_strings[10],	/* Length 4 */
	&alt_part_strings[35],	/* Length 5 */
	&alt_part_strings[43],	/* Length 6 */
	&alt_part_strings[0],	/* Length 7 */
	&alt_part_strings[0],	/* Length 8 */
	&alt_part_strings[52],	/* Length 9 */
	&alt_part_strings[0],	/* Length 10 */
	&alt_part_strings[0],	/* Length 11 */
	&alt_part_strings[0],	/* Length 12 */
	&alt_part_strings[0],	/* Length 13 */
	&alt_part_strings[0],	/* Length 14 */
	&alt_part_strings[0],	/* Length 15 */
	&alt_part_strings[0],	/* Length 16 */
	&alt_part_strings[0],	/* Length 17 */
	&alt_part_strings[0],	/* Length 18 */
	&alt_part_strings[0],	/* Length 19 */
	&alt_part_strings[0],	/* Length 20 */
	&alt_part_strings[0],	/* Length 21 */
	&alt_part_strings[0],	/* Length 22 */
	&alt_part_strings[0],	/* Length 23 */
	&alt_part_strings[0],	/* Length 24 */
	&alt_part_strings[0],	/* Length 25 */
	&alt_part_strings[0],	/* Length 26 */
	&alt_part_strings[0],	/* Length 27 */
	&alt_part_strings[0],	/* Length 28 */
	&alt_part_strings[0],	/* Length 29 */
	&alt_part_strings[0],	/* Length 30 */
	&alt_part_strings[0],	/* Length 31 */
	&alt_part_strings[0]	/* Length 32 */
};

/* Token and secondary keyword table pointers for each of the PARTITION=
** keywords.  Index by the magic number in front of each keyword in the
** keyword list.
** No double-keywords for this set.
*/
static const KEYINFO alt_part_info[] =
{
	/* The 0'th entry is a placeholder */
	{ 0,		0,	0,	NULL},
	{ ON,		0,	0,	NULL},
	{ TO,		0,	0,	NULL},
	{ HASH,		0,	0,	NULL},
	{ LIST,		0,	0,	NULL},
	{ NULLWORD,	0,	0,	NULL},
	{ WITH,		0,	0,	NULL},
	{ RANGE,	0,	0,	NULL},
	{ VALUES,	0,	0,	NULL},
	{ AUTOMATIC,	0,	0,	NULL},
	{ PARTITION,	0,	0,	NULL},
};

/* Define summary structures that point to the normal or the alternate
** keyword tables.
*/
typedef struct _KEYTABLE
{
	const u_char	**indexTable;	/* Pointer into stringTable by length */
	const KEYINFO	*infoTable;	/* Tokens and secondaries */
} KEYTABLE;

static const KEYTABLE normal_kwds =
{
	&Key_index[0],
	&Key_info[0]
};

static const KEYTABLE alt_part_kwds =
{
	&alt_part_index[0],
	&alt_part_info[0]
};
	

/* Local procedure declarations */

static i4 multi_word_keyword(PSS_SESBLK *pss_cb,
	PSQ_CB *psq_cb,
	const struct _KEYINFO *tret,
	u_char *this_char,
	u_char *qry_end,
	i4 *ret_valp);


/*{
** Name: psl_sscan	- The scanner for the server's SQL parser.
**
** Description:
**      This function is the scanner for the server's SQL parser.  It takes 
**      a pointer to a parser session control block, and looks at the data in 
**      there to get the next token from the query buffer.  Each token can be 
**      returned with a corresponding value.  The tokens are defined by the
**	grammar. 
**
**  Notes:
**      The idea is to make this routine fast but small, to this end a couple
**      tricks have been used.  The next character pointer is load from the
**	control block at the start of the function, it then up to the code to
**	update the control block before exiting.  The name routine assumes that
**	the entry will go in the symbol_table, so it copies and lowercases names
**	as it goes.  The number routines assume that the extra ' ' is at the
**	end of the buffer, so that we can handle weird termination conditions
**	easily.
**
**	The single quote is the quote character for SQL. To escape it, double
**	it.
**	The ansi not operator is represented as '^' on ascii machines.
**
** Inputs:
**      pss_cb                          Pointer to a parser session cb
**        .pss_nxtchar                   Place where next token begins
**	  .pss_endbuf			  End of query buffer
**	  .pss_prvtok			  Place where last token began
**	  .pss_lineno			  Current line number in query buffer
**	  .pss_symnext			  Next available space in symbol table
**	  .pss_symblk			  Current symbol table block
**	    .pss_symdata		    Beginning of data area
**	    .pss_sbnext			    Pointer to next symbol table block
**	  .pss_symstr			  Memory stream for symbol table blocks
**	  .pss_yacc			  Yacc control block for this thread
**	    .pss_yylval			    Place to put value of next token
**	  .pss_decimal			  The decimal marker character to use
**	  .pss_qualdepth		  Current depth of where clauses (so
**					  we know whether to convert pattern-
**					  matching characters)
**	  .pss_qrydata			  data values for query text
**	  .pss_dval			  current data value number
**	psq_cb				Pointer to control block used to call
**					the parser
**	  .psq_error			  For holding error information
**
** Outputs:
**	pss_cb
**        .pss_nxtchar                   Set to spot after current token
**	  .pss_prvtok			  Set to spot before current token
**	  .pss_lineno			  Incremented for each newline found
**	  .pss_symnext			  Moved to just beyond space used by
**					  current token.
**	  .pss_symblk			  Set to point to new symbol table block
**					  if one is allocated
**	    .pss_sbnext			    Set to NULL if new block allocated
**	  .pss_yacc			  Yacc control block for this thread
**	    .pss_yylval			    Value associated with token put here
**	  .pss_dval			  current data value number
**	Returns:
**	    The token, or 0 if no more tokens, or -1 if error.
**	Exceptions:
**	    none
**
** Side Effects:
**	    Can allocate more symbol table space.
**	    Can send errors to user or log them.
**
** History:
**	27-jan-87 (daved)
**          written
**	14-may-87 (stec)
**	    Allow @,#,$ in names.
**	    Hex constants delimited by ', not ".
**	09-sep-87 (stec)
**	    Put in a fix for SET secondary words, so that they do
**	    not screw up the UPDATE command.
**	    Also added !<, ^<, !>, ^> operators (few days ago).
**	21-jan-88 (stec)
**	    Fix the fix for SET secondary words, so that
**	    EXECUTE IMMEDIATE SET ... statements parse too.
**	31-mar-88 (stec)
**	    Cleanup of 2703 error code handling.
**	12-aug-88 (stec)
**	    Accept curly brackets in a DB procedure with a select stmt.
**	31-aug-88 (andre)
**	    Recognize FORREADONLY as FROM clause terminator.
**	03-oct-88 (stec)
**	    Complete Kanji related changes.
**	18-oct-88 (stec)
**	    Fix recognition of a Kanji name too long bug.
**	27-jul-89 (jrb)
**	    Changed scanning of numeric literals to return decimal for a certain
**	    class of numbers.  Also, allowed for backward compatibility with old
**	    literal semantics by checking pss_num_literals flag in session cb.
**	27-oct-89 (andre)
**	    A very belated comment: since SQl grammar no longer plays funny
**	    games with tarhet lists in SELECT, we need not bother with
**	    [un]stacking of tokens.
**	12-nov-91 (rblumer)
**	  merged from 6.4:  03-jul-91 (rog)
**	    Error 2701 expects an argument, but we weren't passing one. (37928)
**	  merged from 6.4:  25-jul-91 (andre)
**	    NLs inside string constants will be stripped only if the session is
**	    with a pre-64 FE (pss_cb->pss_ses_flag & PSS_STRIP_NL_IN_STRCONST).
**	    This will fix bug 38098
**	  merged from 6.4:  22-jul-91 (rog)
**	    Specify correct number of arguments for errors 2700 and 2705. (9877)
**	17-sep-92 (andre)
**	    REGISTER is now a keyword in its own right, so it will be returned
**	    as such regardless of whether we are parsing privileges.  RAISE will
**	    continue to be returned as a keyword ONLY WHEN parsing privileges.
**	13-oct-92 (ralph)
**	    Add support for delimited identifiers
**	12-feb-93 (ralph)
**	    Translate regular identifiers according to database semantics
**	05-mar-1993 (rog)
**	    When storing numbers, especially floats, if we call psf_symall()
**	    to increase the symbol table space, we need to align pss_symnext
**	    on BYTE_ALIGN machines.  (bug 49529)
**	12-may-93 (anitap)
**	    Fixed compiler warnings.
** 	10-aug-93 (andre)
**	    fixed causes of compiler warnings
**	17-aug-93 (andre)
**	    uppercase regular identifiers whenever database semantics specify 
**	    that reg ids must be uppercased instead whenever pss_dbxlate
**	    does not represent INGRES-like semantics
**      21-mar-95 (ramra01)
**          If the II_DECIMAL is set to ',' the query would fail with a
**          CREATE table with DECIMAL(4,2). The fact that the stmt is a
**          CREATE is now maintained in the pss_stmt_flags. Any bearing 
**          on the decimal will examine this flag for interpreting a
**          comma or decimal point. (B67519)
**	03-nov-95 (pchang)
**	    Change to use cui_f_idxlate() which is faster than cui_idxlate(), 
**	    when translating regular identifiers.
**	19-mar-96 (pchang)
**	    Fixed bug 70204.  Incorrect test on the next symbol location for
**	    byte alignment prevented a new symbol block to be allocated when
**	    there are exactly 2 bytes left on the current symbol table, and
**	    subsequent alignment operation on the next symbol pointer caused
**	    it to advance beyond the current symbol block and thus, corrupting
**	    adjacent object when the next symbol was assigned.  SEGVIO followed
**	    when the trashed object was later referenced.
**  11-jun-1996 (angusm)
**      DECLARE GLOBAL TEMPORARY TABLE is now flagged by parser
**		in pss_cb->pss_statement_flags. Extend fix
**		for bug 67519 to DGTT with II_DECIMAL (bug 76902)
**  08-May-1997 (merja01)
**      Changes try to ii_try to prevent conflict with Digital Unix 4.0
**      definition of try in c_excpt.h.
**      07-Dec-1998 (rigka01)
**          bug 94215 - long insert statement causes ULM memory corruption and
**          later  ACCVIO due to byte alignment check was passed but bytes left
**          was 0 and a new symbol block did not get allocated.
**  15-Jan-1999 (hweho01)
**      Moved the declarations of register integer variable to the
**      outside of the for() loop in the hex constant processing. They
**      can't be initialized correctly during the runtime if they are
**      declared inside of the loop on NCR platform (nc4_us5).
**	 7-jul-1999 (hayke02)
**	    Modified the call to psf_error() for errorno 2716 (E_US0A9C)
**	    to include the query line number (pss_cb->pss_lineno). The number
**	    of parameters (num_parms) has been upped from 0 to 1. This change
**	    fixes bug 94238.
**	17-nov-2000 (abbjo03) bug 102580
**	    Change scanning of numerical values so that a numeric floating
**	    point or decimal value can be entered when II_DECIMAL is a comma.
**	11-sep-01 (inkdo01)
**	    Attempts to make reserved words context sensitive - also change 
**	    PSS_SYMBLKs to be 256 bytes (rest is wasted).
**	26-Jan-2004 (schka24)
**	    Implement alternate keywords.
**	12-may-04 (inkdo01)
**	    Remove Karl's bypass of I8CONST support.
**	14-May-2004 (schka24)
**	    Fix spurious overflow indication caused by c typing trap.
**	3-Feb-2005 (schka24)
**	    Renamed num_literals to parser_compat, fix here.
**	7-dec-05 (inkdo01)
**	    Add support for Unicode literals.
**	10-aug-2006 (gupsh01)
**	    Added support for ANSI date/time literals.
**	06-sep-2006 (gupsh01)
**	    Fixed returning of error message 2730.
**	13-oct-2006 (gupsh01)
**	    Fixed uninitialized variable buffer.
**	06-Nov-2006 (kiria01) b117042
**	    Conform CMspace macro calls to relaxed bool form
**	14-Dec-2006 (gupsh01)
**	    Allow ANSI date/time literals to have space between 
**	    keyword and the value.
**	20-Dec-2006 (gupsh01)
**	    Removed unused constant openquote.
**	21-dec-2006 (dougi)
**	    Minor changes to remove warning messages.
**	23-feb-2007 (gupsh01)
**	    Initialize the utoken and uchunk count for Unicode
**	    literals. This retains the previously parsed values
**	    for the unicode literals.
**	07-Mar-2007 (kiria01) 
**	    Make || map to concat() and not '+' operator.
**	04-Jan-2008 (kiria01) b119694
**	    Correct handling of MAXI8 and MINI8 in CVal8.
**	    Overflow and underflow are now reported so MINI8
**	    and MAXI8 may now be used as I8 values.
**      27-Feb-2008 (kiria01) b120010
**	    Removed the undocumented, non-functioning b'xxxx'
**	    syntax.
*/
i4
psl_sscan(
	register PSS_SESBLK *pss_cb,
	register PSQ_CB	    *psq_cb)
{
    register u_char  *next_char;
    register u_char  *qry_end;
    i4	    err_code;
    i4		    ret_val = 0;
    u_char	    *firstchar;
    PTR		    piece;
    i4		    toksize;
    i4		    dval_num;
    char	    *number;
    char	    ch;
    YACC_CB	    *yacc_cb;
    char	    marker;
    DB_DATA_VALUE   *db_val;
    DB_CURSOR_ID    *db_cursid;
    i4		    lineno = 0;
    i4		    nonwhite = 0;
    DB_STATUS	    status;
    i4		    left;
    bool	    leave_loop = TRUE;
    const KEYTABLE  *kwd_table;
    bool	    hexconst_0xNN = FALSE;
    bool	    check_datetime_type = TRUE;

    next_char	= pss_cb->pss_nxtchar;
    qry_end	= pss_cb->pss_endbuf;
    firstchar	= next_char;
    toksize	= 0;
    dval_num	= pss_cb->pss_dval;
    yacc_cb	= pss_cb->pss_yacc;
# ifdef BYTE_ALIGN
    left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE] - pss_cb->pss_symnext;
    /* 
    ** If not aligned and it is not possible to align or
    ** it doesn't make sense to align, allocate symbol memory block
    */
    if (((((PSS_SBSIZE - DB_CNTSIZE - left) % DB_ALIGN_SZ) != 0) &&
	(left <= DB_ALIGN_SZ) 
       ) || (left == 0))
    {
	status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
	if (DB_FAILURE_MACRO(status))
	{
	    return(-1);	/* error */
	}
    }
    /* always start with aligned values */
    pss_cb->pss_symnext =
	(u_char *) ME_ALIGN_MACRO(pss_cb->pss_symnext, DB_ALIGN_SZ);
# endif /* BYTE_ALIGN */

    /* Init reserved word control fields. */
    yacc_cb->yyrestok = 0;
    yacc_cb->yy_rwdstr = NULL;
    yacc_cb->yyreswd = FALSE;

    /* Decide which reserved words we're using today */
    kwd_table = &normal_kwds;
    if (yacc_cb->yy_partalt_kwds)
	kwd_table = &alt_part_kwds;

    /*
    ** Remember starting point for previous token, for error reporting and so
    ** we can compensate for the parser's look-ahead when parsing two statements
    ** in succession.
    */
    pss_cb->pss_prvtok = next_char;

    while (next_char <= qry_end)
    {
        switch (*next_char)
        {
        case '\t': case '\r':
	    CMnext(next_char);
            continue;

        case '\n':
	    lineno++;
	    CMnext(next_char);
            continue;

	/* check for a ANSI date/time constants 
	** The declared types for ANSI date/time types are
	** Datetime constants of the form
	** DATE'xxxx...',
	** TIME'xxxx...'
	** TIMESTAMP'xxxx...'
	** 
	** INTERVAL literals come in enough variations that they
	** are handled in the parser.
	*/
        case 'd': 
        case 'D': 
	  nonwhite++;
	  if ((*(next_char + 1) != 'a') && 
	        (*(next_char + 1) != 'A'))
		break;
	  check_datetime_type = FALSE;
        case 't': 
 	case 'T':
	   if (check_datetime_type) 
	   {
	      nonwhite++;
	      if ((*(next_char + 1) != 'i') && 
	          (*(next_char + 1) != 'I'))
	         break;
	      check_datetime_type = FALSE;
	   }
	   /* Could be datetime type validate further */
	   {
		register u_char *string;
		register i4     length;
		register i4     left;
		u_char		*start;
		i4		open_quote = 0;
		i4		close_quote = 0;

		i4		precision = 0;
		i4		yearprec = 0;
		i4		dayprec = 0;
		i4		secprec = 6;
		i4		secpreccal = 0;
		i4	  	secprec_begin = 0;
		i4	  	secprec_end = 0;
		i4	  	minuscount = 0;

		bool		nomatch = FALSE;
		i4 	        int_postchar_count = 0;
		i4		retvalue;

		i4		*prec = &precision;
		i4		*precloc;
		i4		composite;
	        u_char		*ch1;
		i4		ch1_cnt;

		/* check which keyword is given */
		start = string =
		 (u_char *) ((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text;
		length =  DB_MAXSTRING;
		left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE] - start;

		for (;;)
		{
		    if (next_char > qry_end)
		    {
			_VOID_ psf_error(2700L, 0L, PSF_USERERR,
			    &err_code, &psq_cb->psq_error, 1,
			    (i4) sizeof(pss_cb->pss_lineno),
			    &pss_cb->pss_lineno);
			return (-1);
		    }

		    ch = *next_char;

		    switch (ch)
		    {
			case 'd':
			case 'D':
	    		  if ( STncasecmp("date", (PTR)next_char, 4) == 0)
			  {
			      ch1 = next_char + 4;
			      ch1_cnt = 0;
			      while ((ch1 <= qry_end) && CMwhite(ch1))
			      {
				CMnext(ch1);
				ch1_cnt++;
			      }
			      if (*ch1 != '\'')
				nomatch = TRUE;	
			      else
			      {	
		      	        next_char += 4 + ch1_cnt + 1;
			        open_quote = 1;
		    	        retvalue = DCONST;
			        precision = 0;
			      }
			  }
			  else
			      nomatch = TRUE;
			break;

			case 't':
			case 'T':
			{
	    	  	   if ( STncasecmp("timestamp", (PTR)next_char, 9) == 0)
		  	   {
			      ch1 = next_char + 9; 
			      ch1_cnt = 0;
			      while ((ch1 <= qry_end) && CMwhite(ch1))
			      {
				CMnext(ch1);
				ch1_cnt++;
			      }
			      if (*ch1 != '\'')
				nomatch = TRUE;	
			      else
			      {	
		      	        next_char += 9 + ch1_cnt + 1;
			        open_quote = 1;
		    	        retvalue = TSWOCONST;
			        precision = 6;
			      }	
		  	   }
			   else if ( STncasecmp("time", (PTR)next_char, 4) == 0)
			   {	
			      ch1 = next_char + 4; 
			      ch1_cnt = 0;
			      while ((ch1 <= qry_end) && CMwhite(ch1))
			      {
				CMnext(ch1);
				ch1_cnt++;
			      }
			      if (*ch1 != '\'')
				nomatch = TRUE;	
			      else
			      {	
		      	        next_char += 4 + ch1_cnt + 1;
			        open_quote = 1;
		    	        retvalue = TMWOCONST;
			        precision = 6;
			      }
			   }
			   else
			       nomatch = TRUE;
			}
			break;

			case '\'':
			{
			   if (open_quote)
			    close_quote = 1;
			  else
			    open_quote = 1;

			    CMnext(next_char);
			}
			break;

			case ',':
			case ')':
			    nomatch = TRUE;
			    break;

			default:
			    if (open_quote)
				break; /* continue */
			    else
			        nomatch = TRUE;
			    pss_cb->pss_lineno += lineno;
			    psf_error(2730L, 0L, PSF_USERERR,
				    &err_code, &psq_cb->psq_error, 2,
				    (i4) sizeof(pss_cb->pss_lineno),
				    &(pss_cb->pss_lineno), (i4) 1, &ch);
			    return (-1);
			    break;
		    }

		    if (nomatch)
			break;

		    if (open_quote && !close_quote)
		    {
			/* Now we validate the recognize the 
			** input date/time string.
			** Only certain characters are valid in
			** The data string 
			*/
			switch (*next_char)
			{
			    case ':':
				break;

			    case '-':
			    case '+':
			        if (retvalue == TMWOCONST)
				{
		  		  retvalue = TMWCONST;
				  secprec_end = 1;
				}
				else if (retvalue == TSWOCONST)
				{
				  if ((*next_char == '+') || (minuscount == 2))
				  { 
		  		    retvalue = TSWCONST;
				    secprec_end = 1;
				  } 
				  else
				    minuscount++;
				}
				break;

			    case '.':
				secprec_begin = 1;
				break;
			
			    default:
			    {
			    	if  ((CMdigit(next_char) == FALSE) && 
				      (CMwhite(next_char) == FALSE))
				{
			    	    pss_cb->pss_lineno += lineno;
			    	    psf_error(2730L, 0L, PSF_USERERR,
				      &err_code, &psq_cb->psq_error, 2,
				      (i4) sizeof(pss_cb->pss_lineno),
				      &(pss_cb->pss_lineno), (i4) 1, next_char);
				    return (-1); 
				}
				if ((secprec_begin) && !(secprec_end) )
				   secpreccal++;

		   		if (secpreccal > 9)
				   secpreccal = 9;
				break;
			    }
		        }
			/* Ignore all digits greater than 9 */
			if (secpreccal < 10)
  		          *string++ = *next_char;

			CMnext(next_char);
		    }
		    else 
		    {
		      /* skip blanks */
		      while (CMwhite(next_char)) 
		      {
		         CMnext(next_char);
		      }
		    }

  		    if (--length < 0)
  		    {
  		  	pss_cb->pss_lineno += lineno;
  			_VOID_ psf_error(2701L, 0L, PSF_USERERR,
  			    &err_code, &psq_cb->psq_error, 1,
  			    (i4) sizeof(pss_cb->pss_lineno),
  			    &pss_cb->pss_lineno);
  			return (-1);
  		    }
  
  		    if (--left < 0)
  		    {
  			status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
  			if (DB_FAILURE_MACRO(status))
  			{
  			    return(-1);	/* error */
  			}
  
  			/*
  			** We need to check whether there is enough memory
  			** in this new block to avoid memory corruption.
  			*/
  			if ((PSS_SBSIZE - DB_CNTSIZE) < (DB_MAXSTRING - length))
  			{
  			    psf_error(E_PS0376_INSUF_MEM, 0L, PSF_INTERR,
  				&err_code, &psq_cb->psq_error, 0);
  			    return (-1);
  			}
  
  			/* Copy the string (read so far) into the new block */
  			MEcopy((char *) start, DB_MAXSTRING - length,
  		  	  (PTR) (((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text));
  			start =
  			  ((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text;
  			string = start + (DB_MAXSTRING - length - 1);
  			left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE] - string;
  		    }

		    if (close_quote)
  			break;
		}

		if (nomatch)
		    break;

  		*string++ = EOS;

		if ((retvalue == TMWOCONST) || (retvalue == TSWOCONST) ||
		    (retvalue == TMWCONST) || (retvalue == TSWCONST))
		    precision = secpreccal;

		/* For date types fudge the count so that 
		** It contains the precision part
		** Need to check if this string is too
		** Long for a date type
		*/
		composite = DB_PS_ENCODE_MACRO(precision, 
					(DB_MAXSTRING - length - 1));
		((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_count =
		    			composite;
		pss_cb->pss_nxtchar = next_char;
		yacc_cb->yylval.psl_textype =
		    (DB_TEXT_STRING *) pss_cb->pss_symnext;
		pss_cb->pss_symnext = string;
		ret_val = retvalue;
		toksize = (DB_MAXSTRING - length - 1) + DB_CNTSIZE;
		goto tokreturn;
	   }

	/* check for hex constant */
	case '0':
	    if ( next_char + 1 > qry_end ||
		( *(next_char + 1) != 'X' &&
		  *(next_char + 1) != 'x' ) ) 
	    {
		goto digits;
	    }
	    /*
	    ** flag this hex constant as format 0xNN rather than
	    ** x'nn' and fall into hex constant processing
	    */
	    hexconst_0xNN = TRUE;
	case 'X': case 'x':
	    nonwhite++;
	    if (*(next_char + 1) != '\'' && !hexconst_0xNN)
	    {
		/* Not a hex constant, drop through
		** for name/symbol validation */
		break;
	    }

	    /* Hex constant case */
	    {
		register u_char *string;
		register i4     length;
		register i4     left;
		register i4     c;
		register i4     hex_num;
		u_char		*start;
		register i4     hex_size = 0;
		bool		hexconst_end = FALSE;

		CMnext(next_char); /* skip X or x or 0 */
		CMnext(next_char); /* skip X or x or ' */
		start = string =
		 (u_char *) ((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text;
		length =  DB_MAXSTRING;
		left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE] - start;

		for (;;)
		{
		    if (next_char > qry_end)
		    {
			_VOID_ psf_error(2700L, 0L, PSF_USERERR,
			    &err_code, &psq_cb->psq_error, 1,
			    (i4) sizeof(pss_cb->pss_lineno),
			    &pss_cb->pss_lineno);
			return (-1);
		    }
		    
		    ch = *next_char;
		    CMbyteinc(hex_size, next_char);
		    switch (ch)
		    {
			case '0':
			    hex_num = 0;    break;
			case '1':
			    hex_num = 1;    break;
			case '2':
			    hex_num = 2;    break;
			case '3':
			    hex_num = 3;    break;
			case '4':
			    hex_num = 4;    break;
			case '5':
			    hex_num = 5;    break;
			case '6':
			    hex_num = 6;    break;
			case '7':
			    hex_num = 7;    break;
			case '8':
			    hex_num = 8;    break;
			case '9':
			    hex_num = 9;    break;
			case 'a': case 'A':
			    hex_num = 10;   break;
			case 'b': case 'B':
			    hex_num = 11;   break;
			case 'c': case 'C':
			    hex_num = 12;   break;
			case 'd': case 'D':
			    hex_num = 13;   break;
			case 'e': case 'E':
			    hex_num = 14;   break;
			case 'f': case 'F':
			    hex_num = 15;   break;
			case '\'':
			    if (!hexconst_0xNN)
			    {
				hexconst_end = TRUE;
				CMnext(next_char);
				/* if not a null string and even number of bytes */
				if (hex_size > 1 && hex_size % 2)
				{
				    break;
				}
			    }
			    hexconst_0xNN = FALSE;
			    /* else bad hex constant */
			default:
			    if (hexconst_0xNN)
			    {
				hexconst_end = TRUE;
				/* if not a null string and even number of bytes */
				if (hex_size > 1 && hex_size % 2)
				{
				    break;
				}
			    }
			    pss_cb->pss_lineno += lineno;
			    psf_error(2709L, 0L, PSF_USERERR,
				&err_code, &psq_cb->psq_error, 2,
				(i4) sizeof(pss_cb->pss_lineno),
				&(pss_cb->pss_lineno), (i4) 1, &ch);
			    return (-1);
		    }
		    
		    if (hexconst_end)
			break;
			
		    CMnext(next_char);
		    if (hex_size % 2)
		    {
			/* load high order nibble of character */
			c = hex_num * 16;
			/* get next character */
			continue;
		    }
		    else
			c += hex_num;

		    if (--length < 0)
		    {
			pss_cb->pss_lineno += lineno;
			_VOID_ psf_error(2701L, 0L, PSF_USERERR,
			    &err_code, &psq_cb->psq_error, 1,
			    (i4) sizeof(pss_cb->pss_lineno),
			    &pss_cb->pss_lineno);
			return (-1);
		    }

		    if (--left < 0)
		    {
			status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
			if (DB_FAILURE_MACRO(status))
			{
			    return(-1);	/* error */
			}

			/*
			** We need to check whether there is enough memory
			** in this new block to avoid memory corruption.
			*/
			if ((PSS_SBSIZE - DB_CNTSIZE) < (DB_MAXSTRING - length))
			{
			    psf_error(E_PS0376_INSUF_MEM, 0L, PSF_INTERR,
				&err_code, &psq_cb->psq_error, 0);
			    return (-1);
			}

			/* Copy the string (read so far) into the new block */
			MEcopy((char *) start, DB_MAXSTRING - length,
		  (PTR) (((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text));
			start =
			  ((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text;
			string = start + (DB_MAXSTRING - length - 1);
			left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE]
			    - string;
		    }

		    *string++ = c;
		}


		((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_count =
		    DB_MAXSTRING - length;
		pss_cb->pss_nxtchar = next_char;
		yacc_cb->yylval.psl_textype =
		    (DB_TEXT_STRING *) pss_cb->pss_symnext;
		pss_cb->pss_symnext = string;
		ret_val = HEXCONST;
		toksize = DB_MAXSTRING - length + DB_CNTSIZE;
		goto tokreturn;
	    }
        case '?':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
	    yacc_cb->yylval.psl_tytype = 0;
	    ret_val = QUESTIONMARK;
	    goto tokreturn;
	case ',':
	    /*
	    ** Due to internationalization, we must determine whether either
	    ** of the characters is the decimal point.
	    */

	    nonwhite++;

	    if (pss_cb->pss_decimal != ',' || next_char >= qry_end ||
		!CMdigit(next_char + 1) || ret_val == NAME ||
               ( pss_cb->pss_decimal == ',' && 
		 ((pss_cb->pss_stmt_flags & PSS_CREATE_STMT) ||
		 ( pss_cb->pss_stmt_flags & PSS_CREATE_DGTT)) ))
	    {
		CMnext(next_char);
		pss_cb->pss_nxtchar = next_char;
		yacc_cb->yylval.psl_tytype = 0;
		ret_val = COMMA;
		goto tokreturn;
	    }
	    /* This comma is part of a real number, fall through to that case */

        case '.':
	    nonwhite++;
            if (next_char > qry_end || !CMdigit(next_char + 1))
            {
                /*  not a number of the form .ddd so must be a '.'  */

		CMnext(next_char);
                pss_cb->pss_nxtchar = next_char;
		yacc_cb->yylval.psl_tytype = 0;
		ret_val = PERIOD;
		goto tokreturn;
            }

digits:
	case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	{
	    register u_char	    *number = next_char;
	    register i4	    type;
            i4			    c = *number;
	    register i4	    decimal = pss_cb->pss_decimal;
	    i8			    int_number;
	    f8			    double_number;
	    i4			    tot_digs = 0;
	    i4			    frac_digs = 0;
	    bool		dot_or_comma;

	    nonwhite++;
	    CMnext(next_char);

	    /*
	    ** We're going to scan a numeric literal.  If the user wants
	    ** the old numeric literal semantics, we only return integer
	    ** and floating types.  If the user wants the new (INGRES 8.0)
	    ** numeric literal semantics, we return integer, packed decimal,
	    ** and floating types.  `type' keeps track of the result type
	    ** we expect to return (although we never return the type, just
	    ** the token).
	    */

	    type = DB_INT_TYPE;

	    /*  pick off the leading digits */
	    if (c != decimal)
	    {
		tot_digs++;
		
		while (next_char <= qry_end)
		{
		    c = *next_char;
		    CMnext(next_char);
		    if (!CMdigit(&c))
			break;
		    tot_digs++;
		}
	    }

	    /*
	    ** See if we need to scan fractional part of floating point
	    ** number.  According to the user's manual, a floating point
	    ** number must have a digit after the decimal marker.  However,
	    ** there have been a couple of versions of INGRES in which this
	    ** was not enforced.  This causes problems when the decimal
	    ** marker is a comma: how should we interpret (x=10,y=20)?
	    ** However, it would probably cause problems for the users to
	    ** start enforcing this now.  The decision was to require a
	    ** digit after the decimal only if the marker is a comma.
	    ** This little section of code will leave next_char pointing
	    ** to the place after the fractional part of the number, if
	    ** any.  If there isn't any, it will leave next_char pointing
	    ** to the place after the integer part of the number.
	    */
	    dot_or_comma = !( ((pss_cb->pss_stmt_flags & PSS_CREATE_STMT) || 
		(pss_cb->pss_stmt_flags & PSS_CREATE_DGTT)) && decimal == ',' )
		|| (pss_cb->pss_stmt_flags & PSS_DEFAULT_VALUE)
		|| (pss_cb->pss_stmt_flags & PSS_DBPROC_LHSVAR);
	    if (dot_or_comma &&  c == decimal
		&&  next_char <= qry_end
		&&  (   (!CMdigit(next_char)  &&  decimal != ',')
		     || CMdigit(next_char)
		    )
	       )
	    {
		type = (pss_cb->pss_parser_compat & PSQ_FLOAT_LITS)  ?
			DB_FLT_TYPE  :  DB_DEC_TYPE;
		    
		while (next_char <= qry_end)
		{
		    c = *next_char;
		    CMnext(next_char);
		    if (!CMdigit(&c))
			break;
		    frac_digs++;
		}
		tot_digs += frac_digs;
	    }

	    /*  if there is an exponent, eat it up  */

	    if ((c == 'e' || c == 'E') && next_char <= qry_end)
	    {
		type = DB_FLT_TYPE;
		c = *next_char;
		CMnext(next_char);
		if (CMdigit(&c) || c == '-' || c == '+')
		{
		    while (next_char <= qry_end)
		    {
			c = *next_char;
			CMnext(next_char);
			if (!CMdigit(&c))
			    break;
		    }
		}
	    }

	    /*  zero terminate the number for conversion    */

	    if (next_char <= qry_end + 1)
	    {
		CMprev(next_char, number);
		*next_char = EOS;
	    }

	    pss_cb->pss_nxtchar = next_char;

	    /*  make sure there is room in the symbol table;  actually
	    **  max (sizeof(i2), sizeof(i4), sizeof(f8), DB_MAX_DECLEN)
	    **  would be sufficient, but let's be on the safe side.
	    */

	    if (pss_cb->pss_symnext + DB_MAXNAME + 1 >=
		&pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE - 1])
	    {
		status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
		if (DB_FAILURE_MACRO(status))
		{
		    return(-1);	/* error */
		}
	    }

# ifdef BYTE_ALIGN
		/* always start with aligned values */
		pss_cb->pss_symnext =
		    (u_char *) ME_ALIGN_MACRO(pss_cb->pss_symnext, DB_ALIGN_SZ);
# endif /* BYTE_ALIGN */

	    yacc_cb->yylval.psl_strtype = (char *) pss_cb->pss_symnext;

	    if (	type == DB_INT_TYPE
		    &&	CVal8((char *) number, &int_number) == 0
	       )
	    {
		/*  syntax says integer and there was no overflow   */

		if (int_number >= MINI2 && int_number <= MAXI2)
		{
		    *next_char = c;
		    *(i2 *)pss_cb->pss_symnext = (i2)int_number;
		    pss_cb->pss_symnext += sizeof(i2);
		    ret_val = I2CONST;
		    toksize = sizeof(i2);
		    goto tokreturn;
		}
		else if (int_number >= MINI4LL && int_number <= MAXI4)
		{
		    *next_char = c;
		    *(i4 *)pss_cb->pss_symnext = (i4)int_number;
		    pss_cb->pss_symnext += sizeof(i4);
		    ret_val = I4CONST;
		    toksize = sizeof(i4);
		    goto tokreturn;
		}
		else /*if (int_number >= MINI8 && int_number <= MAXI8)*/
		{
		    *next_char = c;
		    *(i8 *) pss_cb->pss_symnext = int_number;
		    pss_cb->pss_symnext += sizeof(i8);
		    ret_val = I8CONST;
		    toksize = sizeof(i8);
		    goto tokreturn;
		}
	    }

	    /* If decimal and not too many digits... */
	    /* B87127 allow integer to become decimal, unless
	    ** II_NUMERIC_LITERAL FLOAT, or old non-decimal client.
	    */
	    if ((((type == DB_INT_TYPE) && 
	   	 (pss_cb->pss_parser_compat & PSQ_FLOAT_LITS)==0) ||
	       (type == DB_DEC_TYPE))  &&  tot_digs <= DB_MAX_DECPREC)
	    {
	        i2		    ps;

		/* put precision/scale in first */
		ps = DB_PS_ENCODE_MACRO(tot_digs, frac_digs);
		I2ASSIGN_MACRO(ps, *pss_cb->pss_symnext);

		/* followed by the number itself */
		_VOID_ CVapk((char *)number, (char)decimal,
			tot_digs, frac_digs,
			(PTR)((u_char *)pss_cb->pss_symnext + sizeof(i2)));
		
		*next_char = c;
		ret_val = DECCONST;
		toksize = DB_PREC_TO_LEN_MACRO(tot_digs) + sizeof(i2);
		pss_cb->pss_symnext += toksize;
		goto tokreturn;
	    }

	    /*
	    ** Either a floating point number, or a decimal number that was
	    ** too large, or an integer that was too large
	    */

	    if (CVaf((char *) number, decimal, &double_number) != OK)
	    {
		pss_cb->pss_lineno += lineno;
		psf_error(2707L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 2,
		    (i4) sizeof(pss_cb->pss_lineno),
		    &(pss_cb->pss_lineno),
		    (i4) STtrmwhite((char *) number), number);
		return (-1);
	    }
	    *next_char = c;
	    *(f8 *) pss_cb->pss_symnext = (f8)double_number;
	    pss_cb->pss_symnext += sizeof(f8);
	    ret_val = F8CONST;
	    toksize = sizeof(f8);
	    goto tokreturn;
	}

        case 'U': case'u':
	{
	    i4	i, j, hcount;

	    nonwhite++;
	    if (*(next_char + 1) != '&')
		break;
	    if (*(next_char + 2) != '\'')
		break;

            {
		/* Unicode literal - U&'...'. */

                DB_NVCHR_STRING	*utoken, *uchunk;
		u_char	*curristr;	/* start of current input segment */
		i4	ulen;		/* length (so far) of token */
		i4	currilen;	/* length of current input seg */
		i4	left;		/* remaining space in token area */
		DB_DATA_VALUE	indv, outdv; /* for coercing char to Uni */

		bool	endoflit, escape;

		CMnext(next_char);		/* skip delimiters */
		CMnext(next_char);
		nonwhite++;
                utoken = uchunk = (DB_NVCHR_STRING *) pss_cb->pss_symnext;
                ulen = 0;
		left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE] - 
				((u_char *)utoken);
		endoflit = FALSE;
		indv.db_datatype = DB_CHA_TYPE;
		outdv.db_datatype = DB_NVCHR_TYPE;
                utoken->count = uchunk->count = 0;

                for (;;)
                {
		    /* Outer loop - processes whole U&' ... ' string. */
                    register i4      c;

		    escape = FALSE;
		    curristr = &next_char[1];
		    currilen = 0;
		    hcount = 0;

		    for ( ; ; )
		    {
			/* Inner loop - processes segments of input
			** string terminated by final quote, an escape
			** sequence, a pair of quotes or a pair of 
			** escape characters. */

			CMnext(next_char);	/* next char to process */

                	if (next_char > qry_end)
			{
			    _VOID_ psf_error(2700L, 0L, PSF_USERERR,
				&err_code, &psq_cb->psq_error, 1,
				(i4) sizeof(pss_cb->pss_lineno),
				&pss_cb->pss_lineno);
			    return (-1);
			}
                    
                	c = *next_char;
			currilen++;

			/* Check for escape sequence. */
			if (c == '\\')
			{
			    if (*(next_char + 1) == '\\')
			    {
				/* Double \ - advance and process segment. */
				CMnext(next_char);
				break;
			    }

			    if (*(next_char + 1) == '+')
			    {
				/* \+hhhhhh - means 6 hexits. */
				CMnext(next_char);
				hcount = 4;	/* size of resulting cp */
			    }
			    else hcount = 2;

			    /* Escape sequence - process segment and 
			    ** then, the escape sequence. */
			    escape = TRUE;
			    currilen--;
			    break;
			}

			/* Check for quote(s). */
			if (c == '\'')
			{
			    CMnext(next_char);	/* advance in either case */
			    if (*(next_char) != '\'')
			    {
				/* Single quote - end of literal. */
				currilen--;
				endoflit = TRUE;
				break;
			    }

			    /* Double quote - break and process segment. */
			    break;
			}

		    }	/* end of inner for */

		    /* We get here when we reach end of string or a '', \\
		    ** or escape sequence. First check for need to allocate
		    ** new token block and copy token so far into it. */
		    if (currilen * 3 * sizeof(UCS2) + hcount > left)
		    {
			/* Allocate new token block and copy 
			** string so far. */
			status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
			if (DB_FAILURE_MACRO(status))
			{
			    return(-1);	/* error */
			}

			/*
			** We need to check whether there is enough memory
			** in this new block to avoid memory corruption.
			*/
			if ((PSS_SBSIZE - DB_CNTSIZE) < 
				(ulen + hcount + currilen * 3 * sizeof(UCS2)))
			{
			    psf_error(E_PS0376_INSUF_MEM, 0L, PSF_INTERR,
				    &err_code, &psq_cb->psq_error, 0);
			    return (-1);
    			}

			/* Copy the string (processed so far) into the 
			** new block */
			utoken = (DB_NVCHR_STRING *) pss_cb->pss_symnext;
			if (ulen)
			{
			    MEcopy((char *) utoken, ulen, 
						(PTR)pss_cb->pss_symnext);
			    uchunk = (DB_NVCHR_STRING *) &(utoken->
				element_array[utoken->count]);
						/* next available piece */
			}
			else uchunk = utoken;
			left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE]
				- ((u_char *)uchunk);
		    }

		    /* If there is a chunk of ASCII characters to be 
		    ** processed, we convert it into the equivalent
		    ** Unicode code points and tack it on the end of 
		    ** the token. */
		    if (currilen > 0)
		    {
			/* We have a segment of ASCII chars - coerce and 
			** append to token. */
			indv.db_data = (char *)curristr;
			indv.db_length = currilen;
			outdv.db_length = 3 * currilen * sizeof(UCS2);
			outdv.db_data = (char *)uchunk;
			status = adu_nvchr_coerce(pss_cb->pss_adfcb,
					&indv, &outdv);
			if (DB_FAILURE_MACRO(status))
			{
			    /* Error. */
			    return(-1);
			}

			if (utoken != uchunk)
			{
			    /* New piece of existing string - roll its
			    ** length into the original and suck new code
			    ** points over top of chunk length. */
			    for (i = 0, j = uchunk->count; i < j; i++)
				uchunk->element_array[i-1] =
					uchunk->element_array[i];
			    utoken->count += j;		/* update count */
			}
			uchunk = (DB_NVCHR_STRING *) &(utoken->
			    element_array[utoken->count]);
						/* next available piece */
			ulen = (char *)uchunk - (char *)utoken;
						/* cumulative token length */
		    }

		    if (endoflit)
			break;		/* break from outer loop */

		    if (escape)
		    {
			i4	uval;
			/* We're positioned on U& - "parse" next 4 bytes
			** (must be hexits) and chuck into next code 
			** point of token. */
			uval = 0;		/* init code point */
			if (hcount == 4)	/* set no. of hexits */
			    hcount = 6;
			else hcount = 4;

			for (i = 0; i < hcount; i++)
			{
			    CMnext(next_char);
			    c = *next_char;
			    uval <<= 4;	/* push it over 1 nibble */

			    switch (c) {
			      case '0':
				c = 0;
				break;
			      case '1':
				c = 1;
				break;
			      case '2':
				c = 2;
				break;
			      case '3':
				c = 3;
				break;
			      case '4':
				c = 4;
				break;
			      case '5':
				c = 5;
				break;
			      case '6':
				c = 6;
				break;
			      case '7':
				c = 7;
				break;
			      case '8':
				c = 8;
				break;
			      case '9':
				c = 9;
				break;
			      case 'A': case 'a':
				c = 10;
				break;
			      case 'B': case 'b':
				c = 11;
				break;
			      case 'C': case 'c':
				c = 12;
				break;
			      case 'D': case 'd':
				c = 13;
				break;
			      case 'E': case 'e':
				c = 14;
				break;
			      case 'F': case 'f':
				c = 15;
				break;
			      default:
				/* Syntax error. */
				_VOID_ psf_error(2729L, 0L, PSF_USERERR,
				    &err_code, &psq_cb->psq_error, 1,
				    (i4) sizeof(pss_cb->pss_lineno),
				    &pss_cb->pss_lineno);
				return(-1);
			    }
			    /* Roll hexit into the new code point. */
			    uval += c;
			}   /* end of escape value loop */

			/* Place escaped values into Unicode string. */
			if (hcount == 4)
			    utoken->element_array[utoken->count] = uval;
			else
			{
			    /* Must copy the 4 byte value. */
			    MEcopy((char *)&uval, sizeof(uval), (char *)uchunk);
			    utoken->count++;	/* incr once here 
						** & again below */
			    left -= sizeof(UCS2); /* reduce left once here 
						** & again below */
			}
			utoken->count++;
			left -= sizeof(UCS2);
			uchunk = (DB_NVCHR_STRING *) &(utoken->
			    element_array[utoken->count]);
						/* next available piece */
			ulen = (char *)uchunk - (char *)utoken;
						/* cumulative token length */
		    }	/* end of escape value */

		}	/* end of outer for */

		/* Finished processing Unicode literal - tidy up
		** and exit. */
		pss_cb->pss_nxtchar = next_char;
                yacc_cb->yylval.psl_utextype = utoken;
                pss_cb->pss_symnext = (u_char *) &(utoken->element_array
			[utoken->count]);
		ret_val = UCONST;
		toksize = ulen;
		goto tokreturn;
            }
	}

        case '\'':
            {

		/*@FIX_ME@ Call cui_idxlate() to translate string constants */

                register u_char *string;
                register i4     length;
                register i4     esc; 
                register i4     left;
		u_char		*start;
		i4		lgth;

		CMnext(next_char);
		nonwhite++;
                esc = 0;
                start = string =
		 (u_char *) ((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text;
                length =  DB_MAXSTRING;
		left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE] - start;

                for (;;)
                {
                    register i4      c;

                    if (next_char > qry_end)
		    {
			_VOID_ psf_error(2700L, 0L, PSF_USERERR,
			    &err_code, &psq_cb->psq_error, 1,
			    (i4) sizeof(pss_cb->pss_lineno),
			    &pss_cb->pss_lineno);
			return (-1);
		    }
                    
                    c = *next_char;
		    lgth = CMbytecnt(next_char);

                    if (c == '\'')
                    {
			if (*(next_char + 1) == '\'')
			{
			    CMnext(next_char);
			}
			else
			{
			    CMnext(next_char);
			    break;
			}

                    }
                    else if (c == '\n' && 
                             pss_cb->pss_ses_flag & PSS_STRIP_NL_IN_STRCONST)
                    {
			/*
			** only strip the NL if connected to an older FE;
			** otherwise it is considered a part of the string
			*/
			lineno++;
			CMnext(next_char);
                        continue;
                    }
                    
		    length -= lgth;
                    if (length < 0)
		    {
			pss_cb->pss_lineno += lineno;
			_VOID_ psf_error(2701L, 0L, PSF_USERERR,
			    &err_code, &psq_cb->psq_error, 1,
			    (i4) sizeof(pss_cb->pss_lineno),
			    &pss_cb->pss_lineno);
			return (-1);
		    }

		    left -= lgth;
                    if (left < 0)
                    {
			status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
			if (DB_FAILURE_MACRO(status))
			{
			    return(-1);	/* error */
			}

			/*
			** We need to check whether there is enough memory
			** in this new block to avoid memory corruption.
			*/
			if ((PSS_SBSIZE - DB_CNTSIZE) < (DB_MAXSTRING - length - 1))
			{
			    psf_error(E_PS0376_INSUF_MEM, 0L, PSF_INTERR,
				&err_code, &psq_cb->psq_error, 0);
			    return (-1);
    			}

			/* Copy the string (read so far) into the new block */
			MEcopy((char *) start, DB_MAXSTRING - length,
		   (PTR) (((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text));
			start =
			   ((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_text;
			string = start + (DB_MAXSTRING - length - 1);
			left = &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE]
			    - string;
		    }

		    CMcpyinc(next_char, string);
                }

                ((DB_TEXT_STRING *) pss_cb->pss_symnext)->db_t_count =
		    DB_MAXSTRING - length;
		pss_cb->pss_nxtchar = next_char;
                yacc_cb->yylval.psl_textype =
		    (DB_TEXT_STRING *) pss_cb->pss_symnext;
                pss_cb->pss_symnext = string;
		ret_val = SCONST;
		toksize = DB_MAXSTRING - length + DB_CNTSIZE;
		goto tokreturn;
            }

        case '"':
            {
		register u_char *word = pss_cb->pss_symnext;
		i4	 	src_len = qry_end - next_char;
		i4     	dst_len = DB_MAXNAME;
		u_char		*lim;

		nonwhite++;

		/*
		** make sure there is room in the symbol table.
		** in case of delimited identifier body being too long we need 3
		** extra chars: 2 for Kanji char and 1 for terminating null.
		*/

		lim = word + DB_MAXNAME + 3 ;

		/* If text is emitted we may have to store piece ptr 
		** as well. This is needed because DB procedures
		** require owner names to be stored for unqualified
		** table/view/index names. Piece ptr is ptr to emitted txt. The
		** layout of these in the "symbol memory" is:
		** DELIM_ID......|\0|piece ptr|
		*/
	        if (pss_cb->pss_stmt_flags & PSS_TXTEMIT)
		    lim += sizeof(piece);

		if (lim >= &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE - 1])
		{
		    status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
		    if (DB_FAILURE_MACRO(status))
		    {
			return(-1);	/* error */
		    }

		    word = pss_cb->pss_symnext;
		}

		/*
		** Invoke cui_idxlate() to translate the identifier
		*/

		status = cui_idxlate(next_char, (u_i4 *) &src_len,
			      word, (u_i4 *) &dst_len, *pss_cb->pss_dbxlate,
				      (u_i4 *)NULL, &psq_cb->psq_error);
		if (DB_FAILURE_MACRO(status))
		{
		    psf_error(psq_cb->psq_error.err_code, 0L, PSF_USERERR,
				&err_code, &psq_cb->psq_error, 2,
				sizeof(ERx("Delimited identifier"))-1,
				ERx("Delimited identifier"),
				src_len, next_char);
		    return(-1);
		}
		
		next_char += src_len;
		word += dst_len;

		/* update next character pointer */
		pss_cb->pss_nxtchar = next_char;
		*(pss_cb->pss_symnext + dst_len) = EOS;
		yacc_cb->yylval.psl_strtype = (char *) pss_cb->pss_symnext;
		pss_cb->pss_symnext += dst_len + 1;
		ret_val = DELIM_IDENT;
		toksize = dst_len + 1;
		goto tokreturn;
            }

        case '(':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = 0;
	    ret_val = LPAREN;
	    goto tokreturn;

        case ')':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = 0;
	    ret_val = RPAREN;
	    goto tokreturn;

        case '{':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = 0;
	    ret_val = LCURLY;
	    goto tokreturn;

        case '}':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = 0;
	    ret_val = RCURLY;
	    goto tokreturn;

        case ':':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = 0;
	    ret_val = COLON;
	    goto tokreturn;

        case ';':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = 0;
	    ret_val = SEMICOLON;
	    pss_cb->pss_stmtno++;
	    /* For the time being, this is probably also useful as
	    ** it uses pss_lineno in syntax error messages. */
	    pss_cb->pss_lineno++;
	    goto tokreturn;

        case '$':
	{
	    char	*c1, *s;

	    CMnext(next_char);
	    c1 = (char *) next_char;
	    nonwhite++;
	    do	    /* something to break out of */
	    {
		if (!CMcmpnocase(c1, "d"))	    /* maybe $dba */
		{
		    ret_val = DBA;
		    s = "ba";
		}
		else if (!CMcmpnocase(c1, "i"))	    /* maybe $ingres */
		{
		    ret_val = INGRES;
		    s = "ngres";
		}
		else
		{
		    break;
		}
		
		for (CMnext(next_char);		/* first char is 'i' or 'd' */
		     *s != EOS &&
		     next_char <= qry_end &&
		     !CMcmpnocase(next_char, s);
		     CMnext(next_char), CMnext(s))
		;

		/*
		** if all the chars matched and either there are no more chars
		** or the next char is not a valid name char, we just saw
		** $DBA or $INGRES.  ret_val contains correct value
		*/
		if (*s == EOS &&
		    (next_char == qry_end || !CMnmchar(next_char)))
		{
		    
		    pss_cb->pss_nxtchar = next_char;
		    yacc_cb->yylval.psl_tytype = 0;
		    goto tokreturn;
		}

		/* 
		** we get here only if we fail to match $dba/$ingres. 
		** leave_loop has already been set to TRUE;
		*/
	    } while (!leave_loop);

	    /* Reset pss_nxtchar to right after $ and return DOLLAR token */
            next_char = pss_cb->pss_nxtchar = (u_char *) c1;
            yacc_cb->yylval.psl_tytype = 0;
	    ret_val = DOLLAR;
	    goto tokreturn;
	}
        case '+':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = ADI_ADD_OP;
	    ret_val = UAOP;
	    goto tokreturn;

        case '-':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = ADI_SUB_OP;
	    ret_val = UAOP;
	    goto tokreturn;
        
        case '=':
	    CMnext(next_char);
	    nonwhite++;
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = ADI_EQ_OP;
	    ret_val = EQUAL;
	    goto tokreturn;

        case '!': case '^':
	    {
		char	buf[3];

		nonwhite++;
		buf[0] = *(next_char);
		buf[1] = ' ';
		buf[2] = '\0';

		CMnext(next_char);

		if (next_char > qry_end)
		{
		    pss_cb->pss_lineno += lineno;
		    psf_error(2702L, 0L, PSF_USERERR, &err_code,
			&psq_cb->psq_error, 2, sizeof (pss_cb->pss_lineno),
			&pss_cb->pss_lineno, STtrmwhite(buf), buf);
		    return (-1);
		}
		else if (*next_char == '=')
		{
		    yacc_cb->yylval.psl_tytype = ADI_NE_OP;
		    ret_val = EOP;
		}
		else if (*next_char == '<')
		{
		    yacc_cb->yylval.psl_tytype = ADI_GE_OP;
		    ret_val = BDOP;
		}
		else if (*next_char == '>')
		{
		    yacc_cb->yylval.psl_tytype = ADI_LE_OP;
		    ret_val = BDOP;
		}
		else
		{
		    buf[1] = *next_char;
		    pss_cb->pss_lineno += lineno;
		    psf_error(2702L, 0L, PSF_USERERR, &err_code,
			&psq_cb->psq_error, 2, sizeof (pss_cb->pss_lineno),
			&pss_cb->pss_lineno, STtrmwhite(buf), buf);
		    return (-1);
		}

		next_char++;
		pss_cb->pss_nxtchar = next_char;
		goto tokreturn;
	    }        

        case '*':
	    CMnext(next_char);
	    nonwhite++;
            if (next_char <= qry_end && *next_char == '*')
            {
                pss_cb->pss_nxtchar = next_char + 1;
                yacc_cb->yylval.psl_tytype =
		    ADI_POW_OP;
		ret_val = BAOPH;
		goto tokreturn;
            }
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = ADI_MUL_OP;
	    ret_val = BAOP;
	    goto tokreturn;

	case '|':
	    {
		char	buf[3];

		nonwhite++;
		buf[0] = *(next_char);
		buf[1] = ' ';
		buf[2] = '\0';

		CMnext(next_char);
		nonwhite++;
		if (next_char <= qry_end && *next_char == '|')
		{
		    next_char++;
		    yacc_cb->yylval.psl_tytype = ADI_CNCAT_OP; /* ADI_ADD_OP */
		}
		else
		{
		    buf[1] = *next_char;
		    pss_cb->pss_lineno += lineno;
		    psf_error(2702L, 0L, PSF_USERERR, &err_code,
			&psq_cb->psq_error, 2, sizeof (pss_cb->pss_lineno),
			&pss_cb->pss_lineno, STtrmwhite(buf), buf);
		    return (-1);
		}
		pss_cb->pss_nxtchar = next_char;
		ret_val = UAOP;
		goto tokreturn;
	    }

        case '<':
	    CMnext(next_char);
	    nonwhite++;
            if (next_char <= qry_end && *next_char == '>')
	    {
		next_char++;
		yacc_cb->yylval.psl_tytype = ADI_NE_OP;
	    }
	    else if (next_char <= qry_end && *next_char == '=')
            {
                next_char++;
                yacc_cb->yylval.psl_tytype = ADI_LE_OP;
            }
            else
	    {
                yacc_cb->yylval.psl_tytype = ADI_LT_OP;
	    }
	    pss_cb->pss_nxtchar = next_char;
	    ret_val = BDOP;
	    goto tokreturn;

        
        case '>':
	    CMnext(next_char);
	    nonwhite++;
            if (next_char <= qry_end && *next_char == '=')
            {
                next_char++;
                yacc_cb->yylval.psl_tytype = ADI_GE_OP;
            }
            else
	    {
                yacc_cb->yylval.psl_tytype = ADI_GT_OP;
	    }
            pss_cb->pss_nxtchar = next_char;
	    ret_val = BDOP;
	    goto tokreturn;

        case '/':
	    CMnext(next_char);
	    nonwhite++;
            if (next_char <= qry_end && *next_char == '*')
            {
                register i4 endcomment = 0;

	        CMnext(next_char);
                while (next_char <= qry_end)
                {
                    if (*next_char == '*' &&
			(next_char + 1) <= qry_end && *(next_char + 1) == '/')
                    {
                        CMnext(next_char);
                        CMnext(next_char);
                        endcomment++;
                        break;
                    }
                    else if (*next_char == '\n')
                    {
                        lineno++;
                    }
		    CMnext(next_char);
                }
                if (endcomment == 0)
		{
		    _VOID_ psf_error(2705L, 0L, PSF_USERERR,
			&err_code, &psq_cb->psq_error, 1,
			(i4) sizeof(pss_cb->pss_lineno),
			&pss_cb->pss_lineno);
		    return (-1);
		}
                continue;
            }
            pss_cb->pss_nxtchar = next_char;
            yacc_cb->yylval.psl_tytype = ADI_DIV_OP;
	    ret_val = BDIVP;
	    goto tokreturn;

	case PSQ_HV:
    	    {
		/* this character must be preceded by a space and followed by
		** an optional number and then a space.
		*/
		char	buf[3];

		nonwhite++;
		err_code = 0;
		buf[0] = *next_char;
		CMnext(next_char);
		buf[1] = *next_char;
		buf[2] = '\0';

		do
		{
		    /* check previous character */
		    if (*(next_char - 2) != ' ')
		    {
			err_code = 1;
			break;
		    }
		    /* get the special marker */
		    if (*next_char != PSQ_HVD && *next_char != PSQ_HVQ)
		    {
			err_code = 1;
			break;
		    }
		    marker	= *next_char;
		    CMnext(next_char);

		    /* check for number */
		    if (CMdigit(next_char))
		    {
			number  = (char*) next_char;
			ch	    = *next_char;

			CMnext(next_char);
			while(next_char <= qry_end && CMdigit(next_char))
			{
			    CMnext(next_char);
			}

			/* Determine the number */
			ch = *next_char;
			*next_char ='\0';
			CVal((char*) number, &dval_num);
			*next_char	    = ch; 
		    }
		    if (next_char > qry_end)
		    {
			err_code = 1;
			break;
		    }
		    /* check for trailing blank */
		    if (!CMspace(next_char))
		    {
			err_code = 1;
			break;
		    }
		    CMnext(next_char);
		    pss_cb->pss_nxtchar	= next_char;
		    
		    /* leave_loop has already been set to TRUE */
		} while (!leave_loop);

		if (err_code)
		{
		    pss_cb->pss_lineno += lineno;
		    psf_error(2702L, 0L, PSF_USERERR, &err_code,
			&psq_cb->psq_error, 2, sizeof (pss_cb->pss_lineno),
			&pss_cb->pss_lineno, STtrmwhite(buf), buf);
		    return (-1);
		}
		else
		{
		    if 
		    (
			dval_num >= pss_cb->pss_dmax
			||
			(
			    marker == PSQ_HVQ
			    &&
			    dval_num + 2 >= pss_cb->pss_dmax
			)
		    )
		    {
			psf_error(2716L, 0L, PSF_USERERR, &err_code,
			    &psq_cb->psq_error, 1,
			    (i4) sizeof(pss_cb->pss_lineno),
			    &(pss_cb->pss_lineno));
			return (-1);
		    }

		    /* current data value */
		    db_val = pss_cb->pss_qrydata[dval_num];

		    toksize = (marker == PSQ_HVQ) ? sizeof (DB_CURSOR_ID)
						  : sizeof (DB_DATA_VALUE);

		    /*  make sure there is room in the symbol table */

		    if (pss_cb->pss_symnext + toksize >=
			&pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE - 1])
		    {
			status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
			if (DB_FAILURE_MACRO(status))
			{
			    return(-1);	/* error */
			}
		    }


		    /* copy the value into the symbol table. when we
		    ** fix text emission, we might not need to do this and
		    ** could just pass a pointer to the value.
		    */
		    yacc_cb->yylval.psl_dbval = (DB_DATA_VALUE*)pss_cb->pss_symnext;
		    if (marker == PSQ_HVD)
		    {
			STRUCT_ASSIGN_MACRO(*db_val,
			    *(DB_DATA_VALUE*) pss_cb->pss_symnext);
			dval_num++;
			ret_val = QDATA;
		    }
		    else
		    {
			db_cursid = (DB_CURSOR_ID*) pss_cb->pss_symnext;
			STRUCT_ASSIGN_MACRO(*(i4*)db_val->db_data, 
			    db_cursid->db_cursor_id[0]);
			db_val = pss_cb->pss_qrydata[++dval_num];
			STRUCT_ASSIGN_MACRO(*(i4*)db_val->db_data, 
			    db_cursid->db_cursor_id[1]);
			db_val = pss_cb->pss_qrydata[++dval_num];
			MEcopy((PTR) db_val->db_data, DB_MAXNAME,
			    (PTR) db_cursid->db_cur_name);
			dval_num++;
			ret_val = QUERYID;
		    }
		    pss_cb->pss_dval    = dval_num;
		    pss_cb->pss_symnext += toksize;
		    /* put value into symbol table and return token */
		    goto tokreturn;
		}
	    }		    
        }   /* switch */

	if (CMspace(next_char))
	{
	    CMnext(next_char);
	    continue;
	}

	if (CMnmstart(next_char) || *next_char == '_')
	{
	    register u_char  *word = pss_cb->pss_symnext;
	    register i4      length;
	    register const u_char  *key;
	    u_char	     *mark;
	    register i4      keylen;
	    u_char	     *lim;
	    i4		     lgth;

	    nonwhite++;

	    /*  make sure there is room in the symbol table.
	    **  in case of name too long we need 3 extra chars
	    **  2 for Kanji char and 1 for terminating null.
	    */

	    lim = word + DB_MAXNAME + 3 ;

	    /* If text is emitted we may have to store piece ptr 
	    ** as well. This is needed because DB procedures
	    ** require owner names to be stored for unqualified
	    ** objects. Piece ptr is ptr to emitted txt. The
	    ** layout of these in the "symbol memory" is:
	    ** NAME......|\0|piece ptr|
	    */
	    /* FIXME
	    ** The ptr should actually be aligned, else
	    ** all systems using BYTE_ALIGN may get screwed up.
	    ** Waiting for resolution from CL (turns out there is no
	    ** prescribed way for aligning memory in a preallocated
	    ** buffer)
	    ** Fix other places in this code where piece ptr is stored
	    ** in buffer !
	    */
	    if (pss_cb->pss_stmt_flags & PSS_TXTEMIT)
		lim += sizeof(piece);

	    if (lim >=
		&pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE - 1])
	    {
		status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
		if (DB_FAILURE_MACRO(status))
		{
		    return(-1);	/* error */
		}

		word = pss_cb->pss_symnext;
	    }

	    /*  start entering the name into the symbol table   */

	    (VOID) CMtolower(next_char, word);
	    length = CMbytecnt(next_char);
	    CMnext(next_char);
	    CMnext(word);
	    mark = word;
	    while (next_char <= qry_end)
	    {
		/* if a good character and not a '$' in a 5.0 repeat
		** query.
		*/
		if (CMnmchar(next_char) &&
		    !(pss_cb->pss_defqry == PSQ_50DEFQRY &&
		    CMcmpcase(next_char, "$") == 0 )
		)
		{
		    lgth = CMbytecnt(next_char);
		    CMtolower(next_char, word);
		    CMnext(next_char);
		    CMnext(word);
		}
		else
		    break;

		length = length + lgth;
		if (length > DB_MAXNAME)
		{
		    /* Name too long */
		    *word = '\0';
		    psf_error(2703L, 0L, PSF_USERERR,
			&err_code, &psq_cb->psq_error, 2,
			(i4) sizeof(pss_cb->pss_lineno),
			&(pss_cb->pss_lineno),
			(i4) STtrmwhite((char *) pss_cb->pss_symnext),
			pss_cb->pss_symnext);
		    return (-1);
		}
		mark = word;
	    }
	    /* update next character pointer */
	    pss_cb->pss_nxtchar = next_char;

	    /*
	    **  special anchor value for search, at
	    **  last legal char in word
	    */
	    *mark = -1;

	    /* return to the beginning of the word */
	    word = pss_cb->pss_symnext;

	    /*  Check to see if this is a keyword   */

	    key = kwd_table->indexTable[length]; /* find starting position by length */

	    /*  While there are more keywords of the same length    */

	    while (*key++)
	    {
		for (keylen = 0; ; keylen++)
		    if (key[keylen] != word[keylen])
			if (keylen == length)
			{
			    register const KEYINFO *tret =
					&kwd_table->infoTable[*--key];
			    i4 multi;

			    /* For the SET keyword don't look up secondary
			    ** word if we are processing an UPDATE statement,
			    ** otherwise statements like `update . set qep = '
			    ** will not parse.
			    **
			    ** For the ON keyword, don't look up secondary word
			    ** word unless the query being parsed is GRANT or
			    ** REVOKE, or CREATE/DROP SECURITY_ALARM
			    ** otherwise we get erroneous behaviour trying to
			    ** process statements where object name may
			    ** follow the keyword ON.
			    ** look up secondary keyword if processing 
			    ** DECLARE GLOBAL TEMPORARY TABLE - ONCOMMIT had
			    ** to be made a double keyword because otherwise
			    ** we get a shift/reduce conflict trying to figure
			    ** out whether ON belongs to ON clause of an outer
			    ** join or ON COMMIT PRESERVE ROWS clause
			    */
			    if ((tret->token != SET ||
				  psq_cb->psq_mode != PSQ_REPLACE)
				   &&
				(tret->token != ON		 ||
				 psq_cb->psq_mode == PSQ_GRANT   ||
				 psq_cb->psq_mode == PSQ_REVOKE  ||
				 psq_cb->psq_mode == PSQ_CALARM  ||
				 psq_cb->psq_mode == PSQ_KALARM  ||
				 psq_cb->psq_mode == PSQ_DGTT    ||
				 psq_cb->psq_mode == PSQ_DGTT_AS_SELECT)
				)
			    {

				/* If there could be a 2nd word, look for it;
				** also continue to scan input if we need
				** lookahead for determining FIRST keyword
				** context.
				*/
				if ( tret->secondary != (SECONDARY *) NULL ||
				   tret->token == FIRST )
				{
				    multi = multi_word_keyword(pss_cb, psq_cb,
						tret, next_char, qry_end,
						&ret_val);
				    if (multi == MULTI_TOKRETURN)
					/* Multi-word kwd all set up */
					goto tokreturn;
				    else if (multi == MULTI_RETNAME)
					goto retname;
				    /* else falls through to rettok */
				}
				else if (tret->token == 0)
				{
				    goto retname;
				}
			    }

			    /* rettok: */
			    /*
			    ** RAISE will be treated as a keyword only if we
			    ** are in the process of parsing privilege spec
			    ** of GRANT statement; otherwise it will be treated
			    ** as an identifier.
			    */
			    if (   tret->token == RAISE
				&& ~pss_cb->pss_stmt_flags & PSS_PARSING_PRIVS
			       )
			    {
				goto retname;
			    }

			    /* Keyword token, set up the keyword info */

			    yacc_cb->yylval.psl_tytype = tret->tokval;
			    if (yacc_cb->yylval.psl_tytype == PSL_GOVAL)
				pss_cb->pss_prvgoval = pss_cb->pss_prvtok;
			    ret_val = tret->token;
			    yacc_cb->yyreswd = TRUE;
			    yacc_cb->yy_rwdstr = (char *)word;

			    /* Drop out of all keyword loops */
			    goto retname;
			}
			else
			    break;	/* to try next keyword */
		key += length; key++;	/* try next keyword */
	    }

	/* update symbol_table pointer and terminate the word.
	** It's a keyword if yyreswd is set, else it's a name.
	*/
	retname:
	    word[length] = EOS;

	    /*
	    ** If regular identifiers must be uppercased, do so now
	    */
	    if (*pss_cb->pss_dbxlate & CUI_ID_REG_U)
	    {
		if ( cui_f_idxlate(
			(char *)word, length, *pss_cb->pss_dbxlate) == FALSE )
		{
		    status = cui_idxlate(word, (u_i4 *)NULL, (u_char *)NULL,
					(u_i4 *)NULL, *pss_cb->pss_dbxlate,
					(u_i4 *)NULL, &psq_cb->psq_error);

		    if (DB_FAILURE_MACRO(status))
		    {
			if (yacc_cb->yyreswd)
			{
			    /* Turn it back into simple keyword token. */
			    yacc_cb->yyreswd = FALSE;
			    goto tokreturn;
			}
			psf_error(psq_cb->psq_error.err_code, 0L, PSF_USERERR,
					&err_code, &psq_cb->psq_error, 2,
					sizeof(ERx("Identifier"))-1,
					ERx("Identifier"),
					length, word);
		    	return(-1);
		    }
		}
	    }

	    pss_cb->pss_symnext += length + 1;
	    yacc_cb->yylval.psl_strtype = (char *)word;
	    if (yacc_cb->yyreswd == FALSE)
		ret_val = NAME;
	    toksize = length + 1;
	    goto tokreturn;
	}

	nonwhite++;
	pss_cb->pss_lineno += lineno;
	psf_error(2708L, 0L, PSF_USERERR, &err_code,
	    &psq_cb->psq_error, 2, sizeof (pss_cb->pss_lineno), 
	    &pss_cb->pss_lineno, sizeof(char), next_char);
	return (-1);

    }	/* scan while */

    /* we are done */
    yacc_cb->yylval.psl_tytype = PSL_GOVAL;
    pss_cb->pss_prvgoval = pss_cb->pss_prvtok;
    ret_val = 0;

tokreturn:
    /*
    ** If we are emitting text, and the we are storing the token value in
    ** the symbol table, also put a pointer to the piece of text directly
    ** after the symbol value in the symbol table.  This will enable us to
    ** substitute for these pieces of text while in the parser.
    */
    if ( (pss_cb->pss_stmt_flags & PSS_TXTEMIT) && toksize != 0)
    {
	/* Make sure there's room for piece pointer */
	if (pss_cb->pss_symnext + sizeof(piece) >=
	    &pss_cb->pss_symblk->pss_symdata[PSS_SBSIZE - 1])
	{
	    status = psf_symall(pss_cb, psq_cb, PSS_SBSIZE);
	    if (DB_FAILURE_MACRO(status))
	    {
		return(-1);	/* error */
	    }

	    pss_cb->pss_symnext += toksize;

	    /*
	    ** Note that when toksize is != 0, yacc_cb->yylval contains a
	    ** address of the value of the current token.  Therefore, in order
	    ** to copy value of a token from the old symbol block to the new one
	    ** we need to copy toksize bytes starting with the address found in
	    ** yacc_cb->yylval and not starting with address of yacc_cb->yylval.
	    **
	    ** This fixes bug 33417 and maybe some others.  (2-nov-90 (andre))
	    */

	    /* Copy token value to new block */
	    MEcopy((PTR) yacc_cb->yylval.psl_strtype, toksize,
	           (PTR) pss_cb->pss_symblk->pss_symdata);
	    yacc_cb->yylval.psl_strtype = 
		(char *) pss_cb->pss_symblk->pss_symdata;
	}
    }

    if (pss_cb->pss_stmt_flags & PSS_TXTEMIT)
    {
	/* Add the current token text to the chain */
	if (ret_val != QDATA)
	{
	    if (psq_tadd(pss_cb->pss_tchain, firstchar,
		pss_cb->pss_nxtchar - firstchar, &piece, &psq_cb->psq_error)
		    != E_DB_OK)
	    {
		return (-1);
	    }
	}
	else
	{
	    if (psq_tcnvt(pss_cb, pss_cb->pss_tchain, 
		(DB_DATA_VALUE*) yacc_cb->yylval.psl_dbval,
		&piece, &psq_cb->psq_error)
		    != E_DB_OK)
	    {
		return (-1);
	    }
	}

	/*
	** If the current token has a value in the symbol table, store the
	** piece pointer there, too.
	**
	** For DB procedures store the piece ptr at the end
	** of the symbol memory allocated for the symbol (after
	** NULL). We need to be able to get back to the piece
	** in order to manipulate text chain for insertion
	** of object owner name for DB procedures.
	*/
	if (toksize != 0)
	{
#ifdef    BYTE_ALIGN
	    MEcopy((char *) &piece, sizeof(piece),
		(char *) pss_cb->pss_symnext);
#else
	    *((PTR *) pss_cb->pss_symnext) = piece;
#endif
	    pss_cb->pss_symnext += sizeof(piece);
	}
    } /* end if TXTEMIT */

    /* do same for 2nd text chain 
     */
    if (pss_cb->pss_stmt_flags & PSS_TXTEMIT2)
    {
	/* Add the current token text to the chain */
	if (ret_val != QDATA)
	{
	    if (psq_tadd(pss_cb->pss_tchain2, firstchar,
		pss_cb->pss_nxtchar - firstchar, &piece, &psq_cb->psq_error)
		    != E_DB_OK)
	    {
		return (-1);
	    }
	}
	else
	{
	    if (psq_tcnvt(pss_cb, pss_cb->pss_tchain2, 
		(DB_DATA_VALUE*) yacc_cb->yylval.psl_dbval,
		&piece, &psq_cb->psq_error)
		    != E_DB_OK)
	    {
		return (-1);
	    }
	}
    } /* end if TXTEMIT2 */

    if ((ret_val > 0) && (nonwhite))
    {
	pss_cb->pss_lineno += lineno;
    }
    if (ret_val == CREATE)
	pss_cb->pss_stmt_flags |= PSS_CREATE_STMT;

    if (ret_val == SELECT)
    {
	pss_cb->pss_stmt_flags &= ~PSS_CREATE_STMT;
	pss_cb->pss_stmt_flags &= ~PSS_CREATE_DGTT;
    }

    return (ret_val);
}

/*
** Name: multi_word_keyword
**
** Description:
**	When the scanner was originally written, the authors tried to
**	fit everything into the giant switch statement.  The code for
**	the multi-word keyword lookup got squished up against the
**	right-hand edge, and apparently they were afraid of falling off;
**	because all the indentation stopped...
**
**	This routine is just a hatchet job, for some semblance of
**	readability.  It's called when a word has been scanned, and
**	there's the possibility of a double- or triple-word keyword
**	token.  We look ahead for whatever follows next, and if it looks
**	like a word, check it to see if it might be a double (or triple!)
**	keyword.
**
**	We also come here for the FIRST keyword, just to look ahead
**	to decide whether we should treat it as a name or a keyword.
**	If it's FIRST <digit>, or in the ORDER BY, first is a keyword,
**	else it's just an identifier.
**
** Inputs:
**	pss_cb		Parser session state control block
**	psq_cb		Query parsing state control block
**	tret		Pointer to keyword info for original word
**	this_char	Where we are in the input buffer
**	qry_end		Where the input buffer ends
**	ret_valp	an output
**
** Outputs:
**	Returns one of:
**	    MULTI_RETTOK - word is a keyword but not a double keyword
**	    MULTI_RETNAME - word is a name, not a keyword
**	    MULTI_TOKRETURN - double-keyword setup, goto tokreturn
**	ret_valp	Set to scanner return value if MULTI_TOKRETURN
**
** History:
**	24-Jan-2004 (schka24)
**	    hatchet job so that I can read this thing, so that I can add
**	    a triple keyword, so that I can consolidate some WITH-clause
**	    productions, so that I can add parsing for PARTITION=.
**      15-Apr-2004 (hanal04) Bug 112111 INGSRV2782
**          '#', '@' and '$' are special characters that do not require
**          delimiters. We were not checking for these characters and
**          incorrectly setting the end of word pointer.
**	6-jul-04 (toumi01)
**	    fix multi word logic modification for partitioning re checking
**	    whether the next token contains a valid identifier name to
**	    allow for quoted identifiers
**	9-Sep-04 (gorvi01)
**	    Modified multi word keyword logic to understand single quoted
**	    identifiers used in drop location query syntax. 
**	10-Dec-04 (gorvi01)
**	    Modified multi word keyword logic to understand equals operator
**	    used in drop location query syntax. 
**	27-Jan-05 (toumi01)
**	    Back out above modified multi word logic for equals operator,
**	    which according to its IP fixes test c2_aud18.sep, but which
**	    breaks BASE_TABLE_STRUCTURE parse and thus breaks mdb load.
**	04-May-05 (gorvi01)
**		Resolved the multi keyword logic problem stated above by 
**		parsing only for equals sign instead of generic operator.
**		The change now does not break mdb load and sep is clean.
**
*/

static i4
multi_word_keyword(PSS_SESBLK *pss_cb, PSQ_CB *psq_cb,
	const KEYINFO *tret, u_char *this_char, u_char *qry_end,
	i4 *ret_valp)

{
    i4 compval;
    i4 lines = 0;
    register i4 ii_try;
    register i4	low;
    register i4	high;
    SECONDARY *second;
    u_char savechar;
    register u_char *word2;
    YACC_CB *yacc_cb;

    second = tret->secondary;
    while ( (*this_char == '\t' || *this_char == '\r' ||
	     CMspace(this_char) || *this_char == '\n')
	    && this_char <= qry_end)
    {
	if (*this_char == '\n')
	    lines++;
	 CMnext(this_char);
    }

    /*
    ** if FIRST token
    **	if next token is numeric or parsing ORDER BY
    **	   FIRST is keyword
    **	else
    **	   FIRST is identifier
    */
    if (tret->token == FIRST)
    {
	if (CMdigit(this_char) ||
	  (psq_cb->psq_ret_flag & PSQ_PARSING_ORDER_BY))
	    return (MULTI_RETTOK);
	return (MULTI_RETNAME);
    }

    /*
    ** If there is no second word at all, it could
    ** be a keyword or a name.  This depends on
    ** whether the Key_info[] structure has a token
    ** entry for this word.  If not, it can't be a
    ** keyword all by itself; it depends on the
    ** following word to make it part of a two-
    ** word keyword.
    */
    if (this_char > qry_end || !CMalpha(this_char))
    {
	if (tret->token)
	    return (MULTI_RETTOK);
	else
	    return (MULTI_RETNAME);
    }
    word2 = this_char;
    CMnext(this_char);

    /*
    ** Now find the end of the word.  This part
    ** depends on there being a blank at the end
    ** of the query buffer.  If there isn't, it
    ** could run off the end.  This is an optimization
    ** that should be documented externally.
    **
    ** Bug 112111: #,$ and @ are not the end of the word.
    */
    for ( ; CMalpha(this_char) || *this_char == '_'
		|| CMdigit(this_char) || *this_char == '#'
		|| *this_char == '@' || *this_char == '$';
	 this_char = CMnext(this_char))
	 ;  /* NULL BODY */

    /* Null-terminate the second word. */
    if ((savechar = *this_char) == '\n')
	lines++;
    *this_char = '\0';

    /*
    ** Binary search for the second word among the
    ** set of possible second words.
    */
    low = 0;
    high = tret->num2ary - 1;
    while (high >= low)
    {
	ii_try = (high + low) / 2;
	/*
	** Normally, we try not to use function calls
	** to do comparisons within the scanner.  This
	** particular comparison isn't critical, however,
	** so we'll take the easy way out.
	*/
	if (!(compval = STcasecmp((char *) word2, second[ii_try].word2)))
	{
	    break;
	}
	else if (compval > 0)
	{
	    low = ii_try + 1;
	}
	else
	{
	    high = ii_try - 1;
	}
    }

    if (high >= low)	/* TRUE means found 2nd word */
    {
	/*
	** replace NULL with saved char; we want to do it
	** here in case we determine that the double keyword
	** ONEVENT|ONDATABASE|ONLOCATION is really ON
	** followed by a table name; similarly we must
	** check if REGISTEREVENT and REMOVEEVENT are
	** really Star statements to register/remove a
	** table named "dbevent".
	*/
	*this_char = savechar;

	/* Simplify code from second[ii_try] to second-> ... */
	second = &second[ii_try];

	/*
	** if the double keyword is ONEVENT, ONDATABASE,
	** ONLOCATION, REGISTEREVENT, REMOVEEVENT, DROPEVENT,
	** DROPLOC, or BASE_TABLE_STRUCTURE we need to perform one
	** more check: 
	** If it's BASE_TABLE_STRUCTURE see if the word structure
	** follows.  Otherwise:
	** (Note ONDATABASE is always a reserved double word
	** for CREATE/DROP SECURITY_ALARM)
	** if (
	**     qry_mode == PSQ_GRANT && next token is TO or
	**					COMMA
	**	  ||
	**	  qry_mode = PSQ_REVOKE && next token is FROM
	**					or COMMA
	**	  ||
	**	  token2 == REGISTEREVENT && next token is
	**					LPAREN or AS
	**	  ||
	**	     (   token2 == REMOVEEVENT
	**	      || token2 == DROPEVENT
	**	      || token2 == DROPLOC
	**	     )
	**	     &&
	**	     (   next token is COMMA
	**	      || end of buffer has been reached)
	**	 )
	** then
	**	   DATABASE/LOCATION/DBEVENT is, in
	**	   fact, a table name, so we return
	**	   ON/REGISTER/REMOVE/DROP, as appropriate
	** else
	**	   return ONEVENT/ONDATABASE/ONLOCATION
	**	   /REGISTEREVENT/REMOVEEVENT/DROPEVENT/DROPLOC,
	**	   as appropriate 
	** endif
	*/
	if (   second->token2 == ONEVENT
	   || (second->token2 == ONDATABASE
	       && psq_cb->psq_mode!=PSQ_CALARM 
	       && psq_cb->psq_mode!=PSQ_KALARM)
	   || second->token2 == ONLOCATION
	   || second->token2 == REGISTEREVENT
	   || second->token2 == REMOVEEVENT
	   || second->token2 == DROPEVENT
	   || second->token2 == DROPLOC
	   || second->token2 == BASE_TABLE_STRUCTURE
	     )
	{
	    register u_char	*char_p;
	    register u_char	*word3;

	    if (second->token2 == REGISTEREVENT)
		word3 = (u_char *) "AS";
	    else if (   second->token2 == REMOVEEVENT
		     || second->token2 == DROPEVENT
		     || second->token2 == DROPLOC
		 )
		word3 = (u_char *) "";
	    else if (psq_cb->psq_mode == PSQ_GRANT)
		word3 = (u_char *) "TO";
	    else if (second->token2 == BASE_TABLE_STRUCTURE)
		word3 = (u_char *) "STRUCTURE";
	    else
		word3 = (u_char *) "FROM";

	    /* skip over "white" characters */
	    for (char_p = this_char;
		 (char_p <= qry_end && CMwhite(char_p));
		 CMnext(char_p) )
		;

	    if (char_p <= qry_end)
	    {
		/*
		** if (the double keyword was REGISTEREVENT)
		** {
		**   if (the next token is LPAREN)
		**   {
		**     return REGISTER
		**   }
		** }
		** else if (the next token is COMMA)
		** {
		**   return ON/REMOVE/DROP as appropriate
		** }
		** else
		** {
		**   compare the next token (if any) with what we
		**   expected to find
		** }
		*/
		if (second->token2 == REGISTEREVENT)
		{
		    if (!CMcmpcase(char_p, "("))
			return (MULTI_RETTOK);
		}
		else if (!CMcmpcase(char_p, ","))
		{
		    return (MULTI_RETTOK);
		}
		/*
		** check if the next token matches the 3-rd
		** word
		*/
		for (;
		     (char_p <= qry_end && *word3 != EOS &&
			    !CMcmpnocase(char_p, word3));
		     CMnext(char_p), CMnext(word3)
		    )
		    ;

		/* did the token match the 3rd word? */
		if (*word3 == EOS && (char_p > qry_end ||
			(!CMnmchar(char_p) && *char_p != '\"' && *char_p != '\'' && *char_p != '=')))
		{
		    if (second->token2 != BASE_TABLE_STRUCTURE)
			/* return ON */
			return (MULTI_RETTOK);
		    else
			/* Found BASE TABLE STRUCTURE, update scan pointer
			** to include all three words
			*/
			this_char = char_p;
			/* Fall thru out of the if's */
		} else if (second->token2 == BASE_TABLE_STRUCTURE)
		    /* Looking for STRUCTURE, didn't find it, BASE is a name,
		    ** not a keyword.
		    */
		    return (MULTI_RETNAME);
		/* else go with the double-keyword we found */
	    }	  /* if (char_p <= qry_end) */
	}	/* if ONEVENT or ONDATABASE or ONLOCATION*/

	pss_cb->pss_nxtchar = this_char;
	pss_cb->pss_lineno += lines;
	yacc_cb = pss_cb->pss_yacc;
	yacc_cb->yylval.psl_tytype = second->val2;
	if (yacc_cb->yylval.psl_tytype == PSL_GOVAL)
	    pss_cb->pss_prvgoval = pss_cb->pss_prvtok;
	*ret_valp = second->token2;
	return (MULTI_TOKRETURN);
    }

    /*
    ** 2nd word not found, so the first word is
    ** either a token or a name.  Which it is depends
    ** on whether there is a token value in the
    ** Key_info entry.  If so, it is a token.  If
    ** not, it's a name.
    */

    *this_char = savechar;
    if (tret->token == 0)
	return (MULTI_RETNAME);
    return (MULTI_RETTOK);
} /* multi_word_keyword */
